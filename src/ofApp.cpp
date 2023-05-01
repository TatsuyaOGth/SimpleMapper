#include "ofApp.h"

static const string smSetupFileName = "settings.xml";

//--------------------------------------------------------------
// ofApp Callbacks
//--------------------------------------------------------------

void ofApp::setup()
{
    ofBackground(0);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofLogLevel(OF_LOG_VERBOSE);

    mEditMode = false;
    mTestPatternMode = 0;
    
    setupGui();

    // Setup warper module
    mWarper = make_shared<Warper>();
    setupWarper();
    
    // Setup receiver module
    mReceiver = make_shared<Receiver>();
    
    // Preload and apply settings
    load();
    setupWarperSource();
    mReceiver->initialize(mTexWidth, mTexHeight, mUseNdi);
}

void ofApp::update()
{
    mReceiver->update();
}

void ofApp::draw()
{
    drawWarper();

    if (mEditMode)
    {
        if (mTestPatternMode == 1)
        {
            drawTestPattern(ofGetWidth(), ofGetHeight());
        }
        mWarper->drawGui();
        mGui.draw();
        auto guiPos = mGui.getPosition();
        int x = guiPos.x;
        int y = guiPos.y + mGui.getHeight();
        int space = 20;
        ofDrawBitmapString(mReceiver->getReceiverInfo(), x, y += space);
    }
}

void ofApp::exit()
{
    save();
    mReceiver->finalize();
}

void ofApp::keyPressed(int key)
{
    mWarper->keyPressed(key);
    
    if (key == 'd')
    {
        mEditMode = !mEditMode;
    }

    if (mEditMode)
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
    mWarper->keyReleased(key);
}

void ofApp::mouseMoved(int x, int y)
{
    mWarper->mouseMoved(x, y);
}

void ofApp::mouseDragged(int x, int y, int button)
{
    mWarper->mouseDragged(x, y, button);
}

void ofApp::mousePressed(int x, int y, int button)
{
    mWarper->mousePressed(x, y, button);
}

void ofApp::mouseReleased(int x, int y, int button)
{
    mWarper->mouseReleased(x, y, button);
}

void ofApp::load()
{
    mGui.loadFromFile(smSetupFileName);
}

void ofApp::save()
{
    mGui.saveToFile(smSetupFileName);
}

void ofApp::applySettings()
{
    mReceiver->finalize();
    setupWarperSource();
    mReceiver->initialize(mTexWidth, mTexHeight, mUseNdi);
}

//--------------------------------------------------------------
// Gui
//--------------------------------------------------------------

void ofApp::setupGui()
{
    mApplyButton.addListener(this, &ofApp::onApplySettings);
    mLoadButton.addListener(this, &ofApp::onLoaded);
    mSaveButton.addListener(this, &ofApp::onSaved);

    mTexParams.setName("Input Texture");
    mTexParams.add(mUseNdi.set("Use NDI", false));
    mTexParams.add(mTexWidth.set("Width", 640, 10, 4096));
    mTexParams.add(mTexHeight.set("Height", 480, 10, 4096));
    mTexParams.add(mFlipH.set("Flip H", false));
    mTexParams.add(mFlipV.set("Flip V", false));
    mTexParams.add(mSenderId.set("Sender ID", 0, 0, 8));

    mGui.setup("Settings");
    mGui.add(mTexParams);
    mGui.add(mApplyButton.setup("Apply"));
    mGui.add(mSaveButton.setup("Save"));
}

void ofApp::onLoaded()
{
    mWarper->load();
}

void ofApp::onSaved()
{
    mWarper->save();
}

void ofApp::onApplySettings()
{
    applySettings();
}


//--------------------------------------------------------------
// Warper
//--------------------------------------------------------------

void ofApp::setupWarperSource()
{
    int x = 0;
    int y = 0;
    int w = mTexWidth;
    int h = mTexHeight;
    mWarper->setSourceRect(ofRectangle(x, y, w, h));
}

void ofApp::setupWarper()
{
    int x = 0;
    int y = 0;
    int w = mTexWidth;
    int h = mTexHeight;
    mWarper->setTargetRect(ofRectangle(x, y, w, h));
}

void ofApp::drawWarper()
{
    ofPushMatrix();
    ofMultMatrix(mWarper->getMatrix());
    mReceiver->draw(mFlipH, mFlipV);
    if (mTestPatternMode == 2)
    {
        int w = mTexWidth;
        int h = mTexHeight;
        drawTestPattern(w, h);
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
// Test Pattern
//--------------------------------------------------------------

void ofApp::drawTestPattern(int w, int h)
{
    ofPushStyle();
    ofNoFill();
    ofSetCircleResolution(60);
    
    const int hw = w / 2;
    const int hh = h / 2;
    const int gridSize = 100;
    
    // Grid lines
    ofSetColor(ofColor::cyan);
    ofSetLineWidth(1);
    ofPushMatrix();
    ofTranslate(hw, hh);
    int i = gridSize;
    while (i < hw)
    {
        ofDrawLine(i, -hh, i, hh);
        ofDrawLine(-i, -hh, -i, hh);
        i += gridSize;
    }
    i = gridSize;
    while (i < hh)
    {
        ofDrawLine(-hw, i, hw, i);
        ofDrawLine(-hw, -i, hw, -i);
        i += gridSize;
    }
    ofPopMatrix();
    
    // Circles
    ofSetLineWidth(1);
    ofSetRectMode(OF_RECTMODE_CENTER);
    int shortSide = min(w, h) / 2;
    int longSide = max(w, h) / 2;
    int step = shortSide / 2;
    i = step;
    while (i < longSide)
    {
        ofSetColor(ofColor::green);
        ofDrawCircle(hw, hh, i);
        ofSetColor(ofColor::yellow);
        ofDrawRectangle(hw, hh, i * 2, i * 2);
        i += step;
    }
    
    // Cross lines
    ofSetColor(255);
    ofSetLineWidth(2);
    ofDrawLine(0, hh, w, hh);
    ofDrawLine(hw, 0, hw, h);
    ofDrawLine(0, 0, w, h);
    ofDrawLine(w, 0, 0, h);
    
    ofPopStyle();
}
