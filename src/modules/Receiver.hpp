#pragma once

#include "ofMain.h"

#include "ofxNDIReceiver.h"
#include "ofxNDIRecvStream.h"
#include "ofxNDIFinder.h"

#if defined(TARGET_WIN32)
#include "ofxSpout.h"
#elif defined(OF_TARGET_OSX)
//TODO: support ofxSyphon
#endif

class Receiver
{
    ofTexture mTex;
    ofPixels mPixels;
    bool mUsingNdi;
    double mReconnectRemainTime;
    int mSenderId;
    
#if defined(TARGET_WIN32)
    ofxSpout::Receiver mSpReceiver;
#elif defined(OF_TARGET_OSX)
    //TODO: support ofxSyphon
#endif
    
    ofxNDIFinder mNdiFinder;
    ofxNDIReceiver mNdiReceiver;
    ofxNDIRecvVideoFrameSync mNdiVideo;
    
public:
    inline bool isUsingNdi() { return mUsingNdi; }
    
    inline int getWidth() { return mTex.getWidth(); }
    inline int getHeight() { return mTex.getHeight(); }
    
    inline bool isReady()
    {
        return mTex.isAllocated()
            && mTex.getWidth() > 0
            && mTex.getHeight() > 0;
    }
    
public:
    Receiver();
    
    void initialize(bool usingNdi);
    void finalize();
    void update();
    void draw(const ofRectangle& rect, bool flipH = false, bool flipV = false);
    void setSenderId(int senderId);
    string getReceiverInfo();
};
