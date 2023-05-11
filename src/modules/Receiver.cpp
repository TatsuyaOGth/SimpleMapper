#include "Receiver.hpp"

Receiver::Receiver()
: mUsingNdi(false)
, mSenderId(0)
, mReconnectRemainTime(1)
{}

void Receiver::initialize(bool usingNdi)
{
    mUsingNdi = usingNdi;
    
    if (mUsingNdi)
    {
        bool ini = NDIlib_initialize();
        mNdiFinder.watchSources();
    }
    else
    {
#if defined(TARGET_WIN32)
        mSpReceiver.init();
#elif defined(OF_TARGET_OSX)
        //TODO: support ofxSyphon
#endif
    }
}

void Receiver::finalize()
{
    if (mUsingNdi)
    {
        if (mNdiReceiver.isSetup())
        {
            mNdiReceiver.disconnect();
            mNdiReceiver.clearConnectionMetadata();
            mNdiFinder.terminate(true);
            NDIlib_destroy();
            mTex.clear();
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

void Receiver::update()
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
            if (mReconnectRemainTime < 0)
            {
                setSenderId(mSenderId);
                mReconnectRemainTime = 1;
            }
            else
            {
                mReconnectRemainTime -= ofGetLastFrameTime();
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

void Receiver::draw(const ofRectangle& rect, bool flipH, bool flipV)
{
    if (!mTex.isAllocated()) return;
    
    ofPushStyle();
    ofPushMatrix();
    ofSetColor(255);
    
    float tx = flipH ? rect.width : 0;
    float ty = flipV ? rect.height : 0;
    float sx = flipH ? -1.0f : 1.0f;
    float sy = flipV ? -1.0f : 1.0f;
    ofTranslate(tx, ty);
    ofScale(sx, sy);
    
    mTex.drawSubsection(ofRectangle(0, 0, rect.width, rect.height), rect);
    
    ofPopMatrix();
    ofPopStyle();
}

void Receiver::setSenderId(int senderId)
{
    mSenderId = senderId;
    
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

string Receiver::getReceiverInfo()
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
