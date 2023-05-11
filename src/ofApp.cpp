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
    ofSetVerticalSync(true);

    mVisibledSettings = true;
    mIsDirty = false;
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
    switchMap(0);
    
    // Setup receiver module
    mReceiver = make_shared<Receiver>();
    
    // Preload and apply settings
    load();
    applySettings();
}

void ofApp::update()
{
    mReceiver->update();
}

void ofApp::draw()
{
    drawWarper();

    mMaps[mMapId]->drawGui();

    // Draw texture area
    ofPushStyle();
    ofNoFill();
    ofSetColor(0, 0, 255);
    ofDrawRectangle(0, 0, mDstSize.get().x, mDstSize.get().y);
    ofPopStyle();

    if (mVisibledSettings)
    {
        mGui.draw();
        auto guiPos = mGui.getPosition();
        int x = guiPos.x;
        int y = guiPos.y + mGui.getHeight();
        int space = 20;
        ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), x, y += space);
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
    mMaps[mMapId]->keyPressed(key);
    
    if (key == 'd')
    {
        mVisibledSettings = !mVisibledSettings;
    }

    if (mVisibledSettings)
    {
        if (key == 'f')
        {
            ofToggleFullscreen();
        }

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
        
        if (key == 't')
        {
            mTestPatternMode = (mTestPatternMode + 1) % 3;
        }
    }
}

void ofApp::keyReleased(int key)
{
    mMaps[mMapId]->keyReleased(key);
}

void ofApp::mouseMoved(int x, int y)
{
    mMaps[mMapId]->mouseMoved(x, y);
}

void ofApp::mouseDragged(int x, int y, int button)
{
    mMaps[mMapId]->mouseDragged(x, y, button);
}

void ofApp::mousePressed(int x, int y, int button)
{
    mMaps[mMapId]->mousePressed(x, y, button);
}

void ofApp::mouseReleased(int x, int y, int button)
{
    mMaps[mMapId]->mouseReleased(x, y, button);
}

void ofApp::load()
{
    for (auto& map : mMaps) map->load();

    ofJson json = ofLoadJson(SETTINGS_FILENAME);
    if (json.empty()) return;

    ofRemoveListener(mSettings.parameterChangedE(), this, &ofApp::onParameterChanged);
    ofDeserialize(json, mSettings);
    switchMap(mMapId);
    ofAddListener(mSettings.parameterChangedE(), this, &ofApp::onParameterChanged);
    mIsDirty = false;
}

void ofApp::save()
{
    for (auto& map : mMaps) map->save();

    ofJson json;
    ofSerialize(json, mSettings);
    mIsDirty = !ofSavePrettyJson(SETTINGS_FILENAME, json);
}

void ofApp::applySettings()
{
    mReceiver->finalize();
    mReceiver->initialize(mUseNdi);

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
    //mGlobalSettings.add(mDispWindowRect.set("Display Window Rect",
    //    ofRectangle(200, 200, 960, 540),
    //    ofRectangle(-8192, -8192, 8, 8),
    //    ofRectangle(8192, 8192, 4092, 4092)));

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
}

void ofApp::onIsDirtyChanged(bool& v)
{
    if (v)
        ofSetWindowTitle(TITLE_NAME + "*");
    else
        ofSetWindowTitle(TITLE_NAME);
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

void ofApp::drawWarper()
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
