#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class WarperPoint : public glm::vec2
{
public:
    bool selected;
    bool highlighted;
    bool grabbed;
    glm::vec2 anchor;
    glm::vec2 offset;
    
    array<bool, 4> xSyncFrags;
    array<bool, 4> ySyncFrags;
    
    WarperPoint()
        : selected(false)
        , highlighted(false)
        , grabbed(false)
    {}
    
    inline void set(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
    
    inline void set(const glm::vec2& other)
    {
        set(other.x, other.y);
    }
};

class Warper
{
    array<WarperPoint, 4> srcPoints;
    array<WarperPoint, 4> dstPoints;
    
    glm::vec2 position;

    float anchorSize;
    float anchorSizeHalf;

public:
    ofEvent<Warper> updatedE;
    
private:
    void drawQuadOutline();
    void drawCorners();
    void drawHighlightedCorner();
    void drawSelectedCorner();
    void drawGrabbedCorner();
    void drawCornerAt(const glm::vec2& point, float scale = 1.0);
    
    void selectPoint(WarperPoint& point, bool onlyThis, bool toggle = false);
    void grabPoint(WarperPoint& point, const glm::vec2& controlPoint);
    void switchSelect(bool prev);
    
    void movePoint(const glm::vec2& target, bool invertMode);
    
public:
    Warper();
    virtual ~Warper() = default;
    
    void drawGui();
    
    void setSourceRect(ofRectangle& rect);
    void setTargetRect(ofRectangle& rect);

    void reset();
    
    ofMatrix4x4 getMatrix() const;
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    void save(const string& path = "quadwarp.xml");
    void load(const string& path = "quadwarp.xml");
};
