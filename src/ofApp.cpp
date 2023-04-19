#include "ofApp.h"

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
    
    mGui = make_shared<Gui>();
    ofAddListener(mGui->loadedEvent, this, &ofApp::onLoaded);
    ofAddListener(mGui->savedEvent, this, &ofApp::onSaved);
    ofAddListener(mGui->applyEvent, this, &ofApp::onApplySettings);
    
    mGui->setup();

    mWarper = make_shared<Warper>();
    
    setupWarper();
    
    mGui->load();
    mUsingNdi = mGui->isUseNdi();
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
        mWarper->drawGui();
        mGui->draw();
        auto guiPos = mGui->getPosition();
        int x = guiPos.x;
        int y = guiPos.y + mGui->getHeight();
        int space = 20;
        ofDrawBitmapString(getReceiverInfo(), x, y += space);
    }
}

void ofApp::exit()
{
    finalizeReceiver();
    mGui->save();
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
            mGui->load();
            applySettings();
        }

        if (key == 's')
        {
            mGui->save();
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

void ofApp::applySettings()
{
    finalizeReceiver();
    
    mUsingNdi = mGui->isUseNdi();
    
    setupWarperSource();
    initializeReceiver();
}

//--------------------------------------------------------------
// Gui Callbacks
//--------------------------------------------------------------

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
    int w = mGui->getTexWidth();
    int h = mGui->getTexHeight();
    mWarper->setSourceRect(ofRectangle(x, y, w, h));
}

void ofApp::setupWarper()
{
    int x = 0;
    int y = 0;
    int w = mGui->getTexWidth();
    int h = mGui->getTexHeight();
    mWarper->setTargetRect(ofRectangle(x, y, w, h));
}

void ofApp::drawWarper()
{
    ofPushMatrix();
    ofMultMatrix(mWarper->getMatrix());
    drawReceiver();
    if (mTestPatternMode == 2)
    {
        int w = mGui->getTexWidth();
        int h = mGui->getTexHeight();
        drawTestPattern(w, h);
    }
    ofPopMatrix();
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
            int index = mGui->getSenderId();
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
    if (!mTex.isAllocated()) return;
    
    ofPushStyle();
    ofPushMatrix();
    ofSetColor(255);
    
    bool flipH = mGui->isFlipH();
    bool flipV = mGui->isFlipV();
    float tx = flipH ? mTex.getWidth() : 0;
    float ty = flipV ? mTex.getHeight() : 0;
    float sx = flipH ? -1.0f : 1.0f;
    float sy = flipV ? -1.0f : 1.0f;
    ofTranslate(tx, ty);
    ofScale(sx, sy);
    
    mTex.draw(0, 0);
    
    ofPopMatrix();
    ofPopStyle();
}

void ofApp::setSenderId()
{
    if (mUsingNdi)
    {
        int index = mGui->getSenderId();
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
