#pragma once

#include "ofMain.h"
#include "ofxQuadWarp.h"
#include "ofxSpout.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
	ofxQuadWarp mWarper;
	ofFbo mFbo;
	ofxSpout::Receiver mSpReceiver;
	ofTexture mSpTex;

	ofParameterGroup mTexParams;
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

	void setupSpout();
	void updateSpout();
	

};
