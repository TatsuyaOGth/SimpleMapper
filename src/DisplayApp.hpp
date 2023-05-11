#pragma once

#include "ofMain.h"

class DisplayApp : public ofBaseApp
{
    ofFbo mFbo;
    
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    void render(const ofTexture& tex);
    void setWindow(ofRectangle& rect);
};
