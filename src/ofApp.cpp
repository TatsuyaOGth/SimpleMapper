#include "ofApp.h"
#include "Common.hpp"

static const string TITLE_NAME = "Simple Mapper";
static const string SETTINGS_FILENAME = "settings.json";
static const int MAP_COUNT = 8;


//--------------------------------------------------------------
// ofApp Callbacks
//--------------------------------------------------------------

void ofApp::setup()
{
    ofSetWindowTitle(TITLE_NAME);
    ofLogToConsole();
    ofBackground(0);
    ofSetFrameRate(0);
    ofSetVerticalSync(false);

    mVisibledSettings = true;
    mIsDirty = false;
    mDirtyWhileMousePressed = false;
    mTestPatternMode = 0;

    for (int i = 0; i < MAP_COUNT; ++i)
    {
        auto o = make_shared<Map>(*this);
        stringstream ss;
        ss << "Map " << (i + 1);
        o->setup(ss.str());
        mMaps.emplace_back(o);
    }
    
    setupGui();
    setupCamera();
    switchMap(0);
    
    // Setup receiver module
    mReceiver = make_shared<Receiver>();

    // Setup edit history
    mHistory = make_shared<History>(mSettings);
    
    // Preload and apply settings
    if (!load())
    {
        mHistory->saveSnapshot();
    }
    applySettings();
}

void ofApp::update()
{
    mReceiver->update();
}

void ofApp::draw()
{
    mCam.begin();

    ofPushMatrix();

    // Translate to center of camera, and flip vertical because axis changing.
    ofScale(1, -1, 1);
    ofTranslate(-(ofGetWidth() * 0.5), -(ofGetHeight() * 0.5));

    // Draw display area
    ofPushStyle();
    ofNoFill();
    ofSetColor(0, 0, 255);
    ofDrawRectangle(0, 0, mDstSize.get().x, mDstSize.get().y);
    ofPopStyle();

    // Draw mapping stuf
    drawMaps();
    ofPushStyle();
    if (mGui.isMinimized())
    {
        ofSetColor(ofColor::white, 50);
    }
    else
    {
        ofSetColor(ofColor::white, 255);
    }
    mMaps[mMapId]->drawGui();
    ofPopStyle();

    ofPopMatrix();
    mCam.end();

    if (mVisibledSettings)
    {
        mGui.draw();

        // Draw infomation text
        auto guiPos = mGui.getPosition();
        int x = guiPos.x;
        int y = guiPos.y + mGui.getHeight();
        int space = 20;

        stringstream ss;

        ss << "selected: ";
        const auto& pts = mMaps[mMapId]->getWarper()->getDstPoints();
        for (int i = 0; i < pts.size(); ++i)
        {
            if (pts[i].selected) ss << i << " ";
        }

        ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), x, y += space);
        ofDrawBitmapString("cam: " + ofToString(mCam.getPosition()), x, y += space);
        ofDrawBitmapString(ss.str(), x, y += space);
        ofDrawBitmapString(mReceiver->getReceiverInfo(), x, y += space);
    }
}

void ofApp::exit()
{
    //save();
    mReceiver->finalize();
}

void ofApp::keyPressed(int key)
{
    if (key == 'd')
    {
        mVisibledSettings = !mVisibledSettings;
    }
    
    if (key == 'f')
    {
        ofToggleFullscreen();
    }

    if (key == 't')
    {
        mTestPatternMode = (mTestPatternMode + 1) % 3;
    }

    if (mGui.isMinimized()) return;

    mMaps[mMapId]->keyPressed(key);
    
    if (key == 'l')
    {
        load();
        applySettings();
    }
    
    if (key == 's')
    {
        save();
    }
    
    if (key == 'p')
    {
        mReceiver->setSenderId(mSenderId);
    }

    if (key == 'r')
    {
        mMaps[mMapId]->reset(mDstSize * 0.5);
    }

    if (key == 'z')
    {
        ofRemoveListener(mSettings.parameterChangedE(), this, &ofApp::onParameterChanged);
        mHistory->undo();
        ofAddListener(mSettings.parameterChangedE(), this, &ofApp::onParameterChanged);
    }

    if (key == 'y')
    {
        ofRemoveListener(mSettings.parameterChangedE(), this, &ofApp::onParameterChanged);
        mHistory->redo();
        ofAddListener(mSettings.parameterChangedE(), this, &ofApp::onParameterChanged);
    }
}

void ofApp::keyReleased(int key)
{
    if (mGui.isMinimized()) return;
    mMaps[mMapId]->keyReleased(key);
}

void ofApp::mouseMoved(int x, int y)
{
    if (mGui.isMinimized()) return;
    auto p = screenToWorld(x, y);
    mMaps[mMapId]->mouseMoved(p.x, p.y);
}

void ofApp::mouseDragged(int x, int y, int button)
{
    if (mGui.isMinimized()) return;
    auto p = screenToWorld(x, y);
    mMaps[mMapId]->mouseDragged(p.x, p.y, button);
}

void ofApp::mousePressed(int x, int y, int button)
{
    if (mGui.isMinimized()) return;
    auto p = screenToWorld(x, y);
    mMaps[mMapId]->mousePressed(p.x, p.y, button);
}

void ofApp::mouseReleased(int x, int y, int button)
{
    if (mGui.isMinimized()) return;

    auto p = screenToWorld(x, y);
    mMaps[mMapId]->mouseReleased(p.x, p.y, button);

    if (mDirtyWhileMousePressed)
    {
        mHistory->saveSnapshot();
        mDirtyWhileMousePressed = false;
    }
}

bool ofApp::load()
{
    //for (auto& map : mMaps) map->load();

    ofJson json = ofLoadJson(SETTINGS_FILENAME);
    if (json.empty()) return false;

    ofRemoveListener(mSettings.parameterChangedE(), this, &ofApp::onParameterChanged);
    ofDeserialize(json, mSettings);
    switchMap(mMapId);
    ofAddListener(mSettings.parameterChangedE(), this, &ofApp::onParameterChanged);
    mIsDirty = false;
    mHistory->clearSnapshots();
    mHistory->saveSnapshot();
    return true;
}

void ofApp::save()
{
    //for (auto& map : mMaps) map->save();

    ofJson json;
    ofSerialize(json, mSettings);
    mIsDirty = !ofSavePrettyJson(SETTINGS_FILENAME, json);
}

void ofApp::applySettings()
{
    mReceiver->finalize();
    mReceiver->initialize(mUseNdi);
    mReceiver->setSenderId(mSenderId);

    mFbo.clear();
    auto s = ofFboSettings();
    s.width = mDstSize.get().x;
    s.height = mDstSize.get().y;
    s.internalformat = mUseAlpha ? GL_RGBA : GL_RGB;
    s.numSamples = 0;
    mFbo.allocate(s);
}

//--------------------------------------------------------------
// Gui
//--------------------------------------------------------------

void ofApp::setupGui()
{
    mMapId
        .setup("Map ID", 0, 0, 8)
        ->addListener(this, &ofApp::onMapIdChanged);

    mApplyButton
        .setup("Apply")
        ->addListener(this, &ofApp::applySettings);

    //mDispWindowRect.addListener(displayApp.get(), &DisplayApp::setWindow);

    mIsDirty.addListener(this, &ofApp::onIsDirtyChanged);

    mGlobalSettings.setName("Global");
    mGlobalSettings.add(mSenderId.set("Sender ID", 0, 0, 8));
    mGlobalSettings.add(mUseAlpha.set("Use Alpha", false));
    mGlobalSettings.add(mUseNdi.set("Use NDI", false));
    mGlobalSettings.add(mDstSize.set("Destination Size",
        glm::vec2(1920, 1080),
        glm::vec2(8, 8),
        glm::vec2(4096, 4096)));

    mGui.setup("Settings");
    mGui.setHeaderBackgroundColor(ofColor::darkCyan);

    ofAddListener(mGui.savePressedE, this, &ofApp::onSaveIconPressed);
    ofAddListener(mGui.loadPressedE, this, &ofApp::onLoadIconPressed);

    mSettings.setName("Settings");
    mSettings.add(mGlobalSettings);
    for (const auto& map : mMaps)
    {
        mSettings.add(map->getSettings());
    }
    ofAddListener(mSettings.parameterChangedE(), this, &ofApp::onParameterChanged);

    mGui.minimize();
}

void ofApp::onIsDirtyChanged(bool& v)
{
    if (v)
    {
        ofSetWindowTitle(TITLE_NAME + "*");

        if (ofGetMousePressed)
        {
            mDirtyWhileMousePressed = true;
        }
        else
        {
            mHistory->saveSnapshot();
        }
    }
    else
    {
        ofSetWindowTitle(TITLE_NAME);
    }
}

bool ofApp::onLoadIconPressed()
{
    load();
    return true; // disable ofxPanel's default load function
}

bool ofApp::onSaveIconPressed()
{
    save();
    return true; // disable ofxPanel's default save function
}

void ofApp::onMapIdChanged(int& id)
{
    switchMap(id);
}

void ofApp::onDispWindowRectChanged(ofRectangle& v)
{
    displayApp->setWindow(v);
}

void ofApp::onParameterChanged(ofAbstractParameter& p)
{
    mIsDirty = true;
}


//--------------------------------------------------------------
// Camera
//--------------------------------------------------------------

void ofApp::setupCamera()
{
    mCam.enableOrtho();
    mCam.removeInteraction(ofEasyCam::TRANSFORM_ROTATE, OF_MOUSE_BUTTON_LEFT, -1);
}

glm::vec2 ofApp::screenToWorld(int x, int y)
{
    auto p = mCam.screenToWorld(glm::vec3(x, y, 0), ofGetCurrentViewport());
    p.x += ofGetWidth() * 0.5;
    p.y += ofGetHeight() * 0.5;
    p.y = ofGetHeight() - p.y;
    return p;
}

//--------------------------------------------------------------
// Map
//--------------------------------------------------------------

void ofApp::switchMap(int mapId)
{
    if (mapId < 0 || mapId >= mMaps.size()) throw;

    mGui.clear();
    mGui.add(mGlobalSettings);
    mGui.add(&mMapId);
    mGui.add(mMaps[mapId]->getSettings());
    mGui.add(&mApplyButton);
}

//--------------------------------------------------------------
// Warper
//--------------------------------------------------------------

void ofApp::drawMaps()
{
    if (!mFbo.isAllocated()) return;

    mFbo.begin();
    ofBackground(0);
    for (auto& map : mMaps)
    {
        if (map->isEnabled())
        {
            bool testpattern = (mTestPatternMode == 2 && map == mMaps[mMapId]);
            map->draw(*mReceiver, testpattern);
        }
    }

    if (mTestPatternMode == 1)
    {
        Common::drawTestPattern(mDstSize.get().x, mDstSize.get().y);
    }

    mFbo.end();
    
    // draw to console window
    mFbo.draw(0, 0);
    
    // Buffered texture to display window
    displayApp->render(mFbo.getTexture());
}
