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
    
public:
    Receiver();
    
    void initialize(int width, int height, bool usingNdi);
    void finalize();
    void update();
    void draw(bool flipH = false, bool flipV = false);
    void setSenderId(int senderId);
    string getReceiverInfo();
};
