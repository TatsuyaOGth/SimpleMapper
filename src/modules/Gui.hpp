#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class Gui
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
    
public:
    void setup();
    void draw();
    void load();
    void save();
    
    void onApplyButtonPressed();
    void onLoadButtonPressed();
    void onSaveButtonPressed();
    
public:
    ofEvent<void> loadedEvent;
    ofEvent<void> savedEvent;
    ofEvent<void> applyEvent;
    
public:
    inline ofParameterGroup& getTexParams() { return mTexParams; }
    inline bool isUseNdi() { return mUseNdi; }
    inline int getTexWidth() { return mTexWidth; }
    inline int getTexHeight() { return mTexHeight; }
    inline bool isFlipH() { return mFlipH; }
    inline bool isFlipV() { return mFlipV; }
    inline int getSenderId() { return mSenderId; }
    
    inline glm::vec2 getPosition() { return mGui.getPosition(); }
    inline int getWidth() { return mGui.getWidth(); }
    inline int getHeight() { return mGui.getHeight(); }
};
