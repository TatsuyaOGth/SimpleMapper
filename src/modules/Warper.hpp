#pragma once

#include "ofMain.h"
#include "ofxQuadWarp.h"

class Warper : public ofxQuadWarp
{
    bool mShiftKeyPressed;
    bool mAltKeyPressed; // or Opt key on macOS
    bool mCtrlKeyPressed; // or Cmd key on macOS
    
    ofPoint mSubPointOffset;
    ofPoint mSelectedPos;
    ofPoint mDstPointOffsets[4];
    
private:
    ofPoint& getNextDstPoint(int selectedIndex);
    ofPoint& getDiagonalDstPoint(int selectedIndex);
    
public:
    void drawGui();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
};
