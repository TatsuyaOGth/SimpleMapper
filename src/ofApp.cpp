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
//    ofEnableSmoothing();
    ofLogLevel(OF_LOG_VERBOSE);

    mEditMode = false;
    mTestPatternMode = 0;

    setupGui();
    setupWarper();
    loadSettings();
    mUsingNdi = mUseNdi;
    setupWarperSource();
    
    if (mUsingNdi)
    {
        // NDI Initialization is must be called in the setup method because it to unstable unless.
        // I don't know that reason. why?
        mNdiFinder.watchSources();
    }
    else
    {
        initializeReceiver();
    }
}

void ofApp::update()
{
    updateReceiver();
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
        drawWarperGui();
        drawGui();
    }
}

void ofApp::exit()
{
    finalizeReceiver();
    saveSettings();
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
            setSenderId();
        }
        
        if (key == 't')
        {
            mTestPatternMode = (mTestPatternMode + 1) % 3;
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
    mTexParams.add(mSenderId.set("Sender ID", 0, 0, 8));

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
    finalizeReceiver();

    mUsingNdi = mUseNdi;

    setupWarperSource();
    setupWarper();
    initializeReceiver();
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

void ofApp::setupWarperSource()
{
    int x = 0;
    int y = 0;
    int w = mTexWidth;
    int h = mTexHeight;
    mWarper.setSourceRect(ofRectangle(x, y, w, h));
}

void ofApp::setupWarper()
{
    int x = 0;
    int y = 0;
    int w = mTexWidth;
    int h = mTexHeight;
    mWarper.setTargetRect(ofRectangle(x, y, w, h));
    mWarper.setup();
}

void ofApp::drawWarper()
{
    ofPushMatrix();
    ofMultMatrix(mWarper.getMatrix());
    drawReceiver();
    if (mTestPatternMode == 2)
    {
        drawTestPattern(mTexWidth, mTexHeight);
    }
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
// Receiver
//--------------------------------------------------------------

void ofApp::initializeReceiver()
{
    if (mUsingNdi)
    {
        mNdiFinder.watchSources();
    }
    else
    {
#if defined(TARGET_WIN32)
        mSpReceiver.init();
        mTex.allocate(mTexWidth, mTexHeight, GL_RGB);
#elif defined(OF_TARGET_OSX)
        //TODO: support ofxSyphon
#endif
    }
}

void ofApp::finalizeReceiver()
{
    if (mUsingNdi)
    {
        if (mNdiReceiver.isSetup())
        {
            mNdiReceiver.disconnect();
            mNdiReceiver.clearConnectionMetadata();
            mNdiFinder.terminate(true);
            NDIlib_destroy();
            mPixels.clear();
        }
    }
    else
    {
#if defined(TARGET_WIN32)
        if (mSpReceiver.isInitialized() || mTex.isAllocated())
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
                if (mPixels.isAllocated())
                {
                    mTex.loadData(mPixels);
                }
            }
        }
        else
        {
            int index = mSenderId;
            auto sources = mNdiFinder.getSources();
            if(0 < sources.size() && index < sources.size())
            {
                if(mNdiReceiver.isSetup()
                   ? (mNdiReceiver.changeConnection(sources[index]), true)
                   : mNdiReceiver.setup(sources[index]))
                {
                    mNdiVideo.setup(mNdiReceiver);
                }
            }
        }
    }
    else
    {
#if defined(TARGET_WIN32)
        if (mSpReceiver.isInitialized() && mSpReceiver.getAvailableSenders().size() > 0)
        {
            mSpReceiver.receive(mTex);
        }
#elif defined(OF_TARGET_OSX)
        //TODO: support ofxSyphon
#endif
    }
}

void ofApp::drawReceiver()
{
    ofPushStyle();
    ofSetColor(255);
        
    if (mTex.isAllocated())
    {
        mTex.draw(0, 0);
    }

    ofPopStyle();
}

void ofApp::setSenderId()
{
    if (mUsingNdi)
    {
        int index = mSenderId;
        auto sources = mNdiFinder.getSources();
        if (0 < sources.size() && index < sources.size())
        {
            if (mNdiReceiver.isSetup())
            {
                mNdiReceiver.changeConnection(sources[index]);
                mTex.clear();
                mPixels.clear();
            }
            else
            {
                if (mNdiReceiver.setup(sources[index]))
                {
                    mNdiVideo.setup(mNdiReceiver);
                }
            }
        }
    }
    else
    {
#if defined(TARGET_WIN32)
        mSpReceiver.selectSenderPanel();
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
            result.push_back(ofToString(result.size(), 2, '0') + ". " + src.p_ndi_name + "(" + src.p_url_address + ")");
            return result;
        });
        return ofJoinString(names, "\n");
    }
    else
    {
#if defined(TARGET_WIN32)
        string texName = mSpReceiver.getChannelName();
        if (texName == "") return "(No Texture)";
        stringstream ss;
        ss << "Texture Name: ";
        ss << mSpReceiver.getChannelName();
        ss << " (";
        ss << mSpReceiver.getWidth();
        ss << ", ";
        ss << mSpReceiver.getHeight();
        ss << ")";
        return ss.str();
#elif defined(OF_TARGET_OSX)
        //TODO: support ofxSyphon
#endif
    }
    return "";
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
