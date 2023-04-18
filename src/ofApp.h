#pragma once

#include "ofMain.h"
#include "ofxQuadWarp.h"
#include "ofxGui.h"
#include "ofxNDIReceiver.h"
#include "ofxNDIRecvStream.h"
#include "ofxNDIFinder.h"

#if defined(TARGET_WIN32)
#include "ofxSpout.h"
#elif defined(OF_TARGET_OSX)
//TODO: support ofxSyphon
#endif

#include "Gui.hpp"

class ofApp : public ofBaseApp
{
	ofxQuadWarp mWarper;
    ofTexture mTex;
    ofPixels mPixels;
    bool mUsingNdi;
    int mTexOffset;
    
#if defined(TARGET_WIN32)
	ofxSpout::Receiver mSpReceiver;
#elif defined(OF_TARGET_OSX)
    //TODO: support ofxSyphon
#endif
    
    ofxNDIFinder mNdiFinder;
    ofxNDIReceiver mNdiReceiver;
    ofxNDIRecvVideoFrameSync mNdiVideo;
    
    shared_ptr<Gui> mGui;
	
	bool mEditMode;
    int mTestPatternMode;

public:
	void setup();
	void update();
	void draw();
	void exit();
    
	void keyPressed(int key);
    
    void applySettings();

	void onLoaded();
	void onSaved();
    void onApplySettings();

	void setupWarperSource();
	void setupWarper();
	void drawWarper();
	void drawWarperGui();

	void initializeReceiver();
    void finalizeReceiver();
	void updateReceiver();
    void drawReceiver();
	void setSenderId();
    string getReceiverInfo();
    
    void drawTestPattern(int w, int h);
};
