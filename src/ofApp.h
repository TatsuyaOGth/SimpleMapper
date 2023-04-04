#pragma once

#include "ofMain.h"
#include "ofxQuadWarp.h"
#include "ofxGui.h"

#if defined(OF_TARGET_WIN)
#include "ofxSpout.h"
#elif defined(OF_TARGET_OSX)
//TODO: support ofxSyphon
#endif

#include "ofxNDIReceiver.h"
#include "ofxNDIRecvStream.h"
#include "ofxNDIFinder.h"

class ofApp : public ofBaseApp
{
	ofxQuadWarp mWarper;
	ofFbo mFbo;
    ofTexture mTex;
    ofPixels mPixels;
    bool mUsingNdi;
    
#if defined(OF_TARGET_WIN)
	ofxSpout::Receiver mSpReceiver;
#elif defined(OF_TARGET_OSX)
    //TODO: support ofxSyphon
#endif
    
    ofxNDIFinder mNdiFinder;
    ofxNDIReceiver mNdiReceiver;
    ofxNDIRecvVideoFrameSync mNdiVideo;
    

	ofParameterGroup mTexParams;
    ofParameter<bool> mUseNdi;
	ofParameter<int> mTexWidth;
	ofParameter<int> mTexHeight;
	ofParameter<bool> mKeepAlpha;

	ofxPanel mGui;
	ofxButton mApplyButton;
	ofxButton mLoadButton;
	ofxButton mSaveButton;
	
	bool mEditMode;

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);

private:	
	void setupGui();
	void drawGui();
	void loadSettings();
	void saveSettings();
	void onApplyButtonPressed();
	void onLoadButtonPressed();
	void onSaveButtonPressed();

	void setupWarper();
	void drawWarper();
	void drawWarperGui();

	void setupFbo();
	void updateFbo();

	void initializeReceiver();
    void finalizeReceiver();
	void updateReceiver();
    void drawReceiver();
    string getReceiverInfo();
};
