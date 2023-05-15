#pragma once

#include "ofMain.h"

namespace Common
{

    inline bool isShiftKeyPressed()
    {
        return ofGetKeyPressed(OF_KEY_SHIFT) || ofGetKeyPressed(OF_KEY_LEFT_SHIFT) || ofGetKeyPressed(OF_KEY_RIGHT_SHIFT);
    }

    inline bool isCtrlKeyPressed()
    {
#if defined(TARGET_WIN32)
        return ofGetKeyPressed(OF_KEY_CONTROL);
#elif defined(TARGET_OSX)
        return ofGetKeyPressed(OF_KEY_COMMAND);
#endif
    }

    inline bool isAltKeyPressed()
    {
        return ofGetKeyPressed(OF_KEY_ALT);
    }


    inline void drawTestPattern(int w, int h)
    {
        ofPushStyle();
        ofNoFill();
        ofSetCircleResolution(60);

        const int hw = w / 2;
        const int hh = h / 2;
        const int gridSize = 100;

        // Grid lines
        ofSetColor(ofColor::white);
        ofSetLineWidth(1);
        ofPushMatrix();
        ofTranslate(hw, hh);
        int i = gridSize;
        while (i < hw)
        {
            ofDrawLine(i, -hh, i, hh);
            ofDrawLine(-i, -hh, -i, hh);
            i += gridSize;
        }
        i = gridSize;
        while (i < hh)
        {
            ofDrawLine(-hw, i, hw, i);
            ofDrawLine(-hw, -i, hw, -i);
            i += gridSize;
        }
        ofPopMatrix();

        // Circles
        ofSetLineWidth(1);
        ofSetRectMode(OF_RECTMODE_CENTER);
        int shortSide = min(w, h) / 2;
        int longSide = max(w, h) / 2;
        int step = shortSide / 2;
        i = step;
        while (i <= longSide)
        {
            ofSetColor(ofColor::green);
            ofDrawCircle(hw, hh, i);
            ofSetColor(ofColor::yellow);
            ofDrawRectangle(hw, hh, i * 2, i * 2);
            i += step;
        }

        // Cross lines
        ofSetColor(255);
        ofSetLineWidth(2);
        ofDrawLine(0, hh, w, hh);
        ofDrawLine(hw, 0, hw, h);
        ofDrawLine(0, 0, w, h);
        ofDrawLine(w, 0, 0, h);
        
        // Outline Rect
        ofSetColor(ofColor::magenta);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofDrawRectangle(0, 0, w, h);

        ofPopStyle();
    }
}
