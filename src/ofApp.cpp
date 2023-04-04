#include "ofApp.h"

static const string smSetupFileName = "setup.xml";

//--------------------------------------------------------------
// ofApp Callbacks
//--------------------------------------------------------------

void ofApp::setup()
{
    ofBackground(0);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableSmoothing();

    mEditMode = false;

    setupGui();
    setupWarper();
    setupFbo();
    setupSpout();

    loadSettings();
}

void ofApp::update()
{
    updateSpout();
    updateFbo();
}

void ofApp::draw()
{
    drawWarper();

    if (mEditMode)
    {
        drawWarperGui();
        drawGui();
    }
}

void ofApp::exit()
{
}

void ofApp::keyPressed(int key)
{
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
            mWarper.load();
        }

        if (key == 's')
        {
            mWarper.save();
        }

        if (key == 'r')
        {
            mWarper.reset();
        }

        if (key == 'p')
        {
            mSpReceiver.selectSenderPanel();
        }
    }
}

//--------------------------------------------------------------
// GUI
//--------------------------------------------------------------

void ofApp::setupGui()
{
    mApplyButton.addListener(this, &ofApp::onApplyButtonPressed);
    mLoadButton.addListener(this, &ofApp::onLoadButtonPressed);
    mSaveButton.addListener(this, &ofApp::onSaveButtonPressed);

    mTexParams.setName("Input Texture");
    mTexParams.add(mTexWidth.set("Width", 640, 10, 4096));
    mTexParams.add(mTexHeight.set("Height", 480, 10, 4096));

    mGui.setup("Settings");
    mGui.add(mTexParams);
    mGui.add(mApplyButton.setup("Apply"));
    mGui.add(mSaveButton.setup("Save"));
}

void ofApp::drawGui()
{
    ofPushStyle();

    ofSetColor(ofColor::white);
    mGui.draw();

    int x = mGui.getPosition().x;
    int y = mGui.getPosition().y + mGui.getHeight();
    int space = 20;
    ofDrawBitmapString("Texture Name: " + mSpReceiver.getChannelName(), x, y += space);
    ofPopStyle();
}

void ofApp::loadSettings()
{
    mGui.loadFromFile(smSetupFileName);
    mWarper.load();
    setupFbo();
    setupSpout();
}

void ofApp::saveSettings()
{
    mGui.saveToFile(smSetupFileName);
    mWarper.save();
}

void ofApp::onApplyButtonPressed()
{
    setupWarper();
    setupFbo();
    setupSpout();
}

void ofApp::onLoadButtonPressed()
{
    loadSettings();
}

void ofApp::onSaveButtonPressed()
{
    saveSettings();
}

//--------------------------------------------------------------
// Warper
//--------------------------------------------------------------

void ofApp::setupWarper()
{
    int x = 0;
    int y = 0;
    int w = mTexWidth;
    int h = mTexHeight;
    mWarper.setSourceRect(ofRectangle(0, 0, w, h));              
    mWarper.setTopLeftCornerPosition(ofPoint(x, y));             
    mWarper.setTopRightCornerPosition(ofPoint(x + w, y));        
    mWarper.setBottomLeftCornerPosition(ofPoint(x, y + h));      
    mWarper.setBottomRightCornerPosition(ofPoint(x + w, y + h)); 
    mWarper.setup();
}

void ofApp::drawWarper()
{
    ofPushMatrix();
    ofMultMatrix(mWarper.getMatrix());
    ofSetColor(255);
    mFbo.draw(0, 0);
    ofPopMatrix();
}

void ofApp::drawWarperGui()
{
    if (!mWarper.isShowing()) mWarper.show();

    ofPushStyle();

    ofSetColor(ofColor::magenta);
    mWarper.drawQuadOutline();

    ofSetColor(ofColor::yellow);
    mWarper.drawCorners();

    ofSetColor(ofColor::magenta);
    mWarper.drawHighlightedCorner();

    ofSetColor(ofColor::red);
    mWarper.drawSelectedCorner();

    ofPopStyle();
}


//--------------------------------------------------------------
// Frame Bufffer Object
//--------------------------------------------------------------

void ofApp::setupFbo()
{
    mFbo.clear();
    mFbo.allocate(mTexWidth, mTexHeight);
}

void ofApp::updateFbo()
{
    mFbo.begin();
    ofSetColor(255);
    mSpTex.draw(0, 0);
    mFbo.end();
}



//--------------------------------------------------------------
// Spout
//--------------------------------------------------------------

void ofApp::setupSpout()
{
    if (!mSpReceiver.isInitialized() || mSpTex.isAllocated())
    {
        mSpReceiver.release();
        mSpTex.clear();
    }

    mSpReceiver.init();
    mSpTex.allocate(mTexWidth, mTexHeight, mKeepAlpha ? GL_RGBA : GL_RGB);
}

void ofApp::updateSpout()
{
    if (!mSpReceiver.isInitialized()) return;
    mSpReceiver.receive(mSpTex);
}
