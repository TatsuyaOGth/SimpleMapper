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
    ofEnableSmoothing();

    mEditMode = false;

    setupGui();
    setupWarper();
    loadSettings();
    onApplyButtonPressed();
}

void ofApp::update()
{
    updateReceiver();
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
    finalizeReceiver();
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
            loadSettings();
            onApplyButtonPressed();
        }

        if (key == 's')
        {
            saveSettings();
        }

        if (key == 'r')
        {
            mWarper.reset();
        }

        if (key == 'p')
        {
#if defined(OF_TARGET_WIN)
            mSpReceiver.selectSenderPanel();
#endif
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
    mTexParams.add(mUseNdi.set("Use NDI", false));
    mTexParams.add(mTexWidth.set("Width", 640, 10, 4096));
    mTexParams.add(mTexHeight.set("Height", 480, 10, 4096));
    mTexParams.add(mKeepAlpha.set("Keep Alpha", false));

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
    ofDrawBitmapString(getReceiverInfo(), x, y += space);
    ofPopStyle();
}

void ofApp::loadSettings()
{
    mGui.loadFromFile(smSetupFileName);
    mWarper.load();
}

void ofApp::saveSettings()
{
    mGui.saveToFile(smSetupFileName);
    mWarper.save();
}

void ofApp::onApplyButtonPressed()
{
    mUsingNdi = mUseNdi;
    finalizeReceiver();
    initializeReceiver();
    setupFbo();
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
    drawReceiver();
    mFbo.end();
}



//--------------------------------------------------------------
// Spout
//--------------------------------------------------------------

void ofApp::initializeReceiver()
{
    if (mUsingNdi)
    {
        NDIlib_initialize();
        mNdiFinder.watchSources();
    }
    else
    {
#if defined(OF_TARGET_WIN)
        mSpReceiver.init();
        mTex.allocate(mTexWidth, mTexHeight, mKeepAlpha ? GL_RGBA : GL_RGB);
#elif defined(OF_TARGET_OSX)
        //TODO: support ofxSyphon
#endif
    }
}

void ofApp::finalizeReceiver()
{
    if (mUsingNdi)
    {
        mNdiReceiver.disconnect();
        NDIlib_destroy();
        mPixels.clear();
    }
    else
    {
#if defined(OF_TARGET_WIN)
        if (!mSpReceiver.isInitialized() || mTex.isAllocated())
        {
            mSpReceiver.release();
            mTex.clear();
        }
#elif defined(OF_TARGET_OSX)
        //TODO: support ofxSyphon
#endif
    }
}

void ofApp::updateReceiver()
{
    if (mUsingNdi)
    {
        if (mNdiReceiver.isConnected())
        {
            mNdiVideo.update();
            if (mNdiVideo.isFrameNew())
            {
                mNdiVideo.decodeTo(mPixels);
            }
        }
        else
        {
            auto sources = mNdiFinder.getSources();
            if(sources.size() > 0)
            {
                if(mNdiReceiver.isSetup()
                   ? (mNdiReceiver.changeConnection(sources[0]), true)
                   : mNdiReceiver.setup(sources[0]))
                {
                    mNdiVideo.setup(mNdiReceiver);
                }
            }
        }
    }
    else
    {
#if defined(OF_TARGET_WIN)
        if (!mSpReceiver.isInitialized()) return;
        mSpReceiver.receive(mTex);
#elif defined(OF_TARGET_OSX)
        //TODO: support ofxSyphon
#endif
    }
}

void ofApp::drawReceiver()
{
    if (mUsingNdi)
    {
        if (mPixels.isAllocated())
        {
            ofImage(mPixels).draw(0, 0);
        }
    }
    else
    {
#if defined(OF_TARGET_WIN)
        mTex.draw(0, 0);
#elif defined(OF_TARGET_OSX)
        //TODO: support ofxSyphon
#endif
    }
}

string ofApp::getReceiverInfo()
{
    if (mUsingNdi)
    {
        auto sources = mNdiFinder.getSources();
        auto names = accumulate(begin(sources), end(sources), vector<string>(), [](vector<string> result, const ofxNDI::Source &src) {
            result.push_back(ofToString(result.size()+1, 2, '0')+". "+src.p_ndi_name+"("+src.p_url_address+")");
            return result;
        });
        return ofJoinString(names, "\n");
    }
    else
    {
#if defined(OF_TARGET_WIN)
        return "Texture Name: " + mSpReceiver.getChannelName()
#elif defined(OF_TARGET_OSX)
        //TODO: support ofxSyphon
#endif
    }
    return "";
}
