#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class Warper
{
    ofPoint srcPoints[4];
    ofPoint dstPoints[4];
    
    ofPoint position;

    float anchorSize;
    float anchorSizeHalf;
    int selectedCornerIndex;
    int highlightCornerIndex;
    
    bool mShiftKeyPressed;
    bool mAltKeyPressed; // or Opt key on macOS
    bool mCtrlKeyPressed; // or Cmd key on macOS
    
    ofPoint mSubPointOffset;
    ofPoint mSelectedPos;
    ofPoint mDstPointOffsets[4];
    
private:
    ofPoint& getNextDstPoint(int selectedIndex);
    ofPoint& getDiagonalDstPoint(int selectedIndex);
    
    void drawQuadOutline();
    void drawCorners();
    void drawHighlightedCorner();
    void drawSelectedCorner();
    void drawCornerAt(const ofPoint& point);
    
public:
    Warper();
    virtual ~Warper() = default;
    
    void drawGui();
    
    void setSourceRect(const ofRectangle& rect);
    void setTargetRect(const ofRectangle& rect);
    void reset();
    
    ofMatrix4x4 getMatrix() const;
    ofMatrix4x4 getMatrix(const ofPoint* srcPoints, const ofPoint* dstPoints) const;
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    void save(const string& path = "quadwarp.xml");
    void load(const string& path = "quadwarp.xml");
};
