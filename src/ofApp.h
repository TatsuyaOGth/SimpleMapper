#pragma once

#include "ofMain.h"
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
#include "Warper.hpp"


class ofApp : public ofBaseApp
{
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
    shared_ptr<Warper> mWarper;
	
	bool mEditMode;
    int mTestPatternMode;

public:
	void setup();
	void update();
	void draw();
	void exit();
    
	void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    void applySettings();

	void onLoaded();
	void onSaved();
    void onApplySettings();

	void setupWarperSource();
	void setupWarper();
	void drawWarper();

	void initializeReceiver();
    void finalizeReceiver();
	void updateReceiver();
    void drawReceiver();
	void setSenderId();
    string getReceiverInfo();
    
    void drawTestPattern(int w, int h);
};
