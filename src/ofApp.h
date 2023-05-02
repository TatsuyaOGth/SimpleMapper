#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Warper.hpp"
#include "Receiver.hpp"
#include "History.hpp"

class ofApp : public ofBaseApp
{
    ofParameterGroup mTexParams;
    ofParameter<bool> mUseNdi;
    ofParameter<int> mTexWidth;
    ofParameter<int> mTexHeight;
    ofParameter<bool> mFlipH;
    ofParameter<bool> mFlipV;
    ofParameter<int> mSenderId;
    
    ofxPanel mGui;
    ofxButton mApplyButton;
    ofxButton mLoadButton;
    ofxButton mSaveButton;
    
    shared_ptr<Warper> mWarper;
    shared_ptr<Receiver> mReceiver;
    
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
    
    void load();
    void save();
    void applySettings();

    void setupGui();

	void setupWarperSource();
	void setupWarper();
	void drawWarper();

    void drawTestPattern(int w, int h);
};
