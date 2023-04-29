#pragma once

#include "ofMain.h"
#include "Gui.hpp"
#include "Warper.hpp"
#include "Receiver.hpp"


class ofApp : public ofBaseApp
{
    shared_ptr<Gui> mGui;
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
    
    void applySettings();

	void onLoaded();
	void onSaved();
    void onApplySettings();

	void setupWarperSource();
	void setupWarper();
	void drawWarper();

//	void initializeReceiver();
//    void finalizeReceiver();
//	void updateReceiver();
//    void drawReceiver();
//	void setSenderId();
//    string getReceiverInfo();
    
    void drawTestPattern(int w, int h);
};
