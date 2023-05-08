#pragma once

#include "ofMain.h"

class DisplayApp : public ofBaseApp
{
    ofFbo mFbo;
    
public:
    void setup();
    void update();
    void draw();
    
    void render(const ofTexture & tex);
    
    void keyPressed(int key);
};
