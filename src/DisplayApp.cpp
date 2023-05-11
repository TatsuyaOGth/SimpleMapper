#include "DisplayApp.hpp"

void DisplayApp::setup()
{
    ofSetWindowTitle("Simple Mapper | Display");
    ofSetFrameRate(0);
    ofSetVerticalSync(true);
    ofBackground(0);
}

void DisplayApp::update()
{
}

void DisplayApp::draw()
{
    if (mFbo.isAllocated())
    {
        mFbo.draw(0, 0, ofGetWidth(), ofGetHeight());
    }
}

void DisplayApp::keyPressed(int key)
{
    if (key == 'f')
    {
        ofToggleFullscreen();
    }
}

void DisplayApp::render(const ofTexture & tex)
{
    if (!mFbo.isAllocated() || (mFbo.getWidth() != tex.getWidth() || mFbo.getHeight() != tex.getHeight()))
    {
        mFbo.clear();
        mFbo.allocate(tex.getWidth(), tex.getHeight(), GL_RGBA, 0);
    }
    
    mFbo.begin();
    tex.draw(0, 0);
    mFbo.end();
}

void DisplayApp::setWindow(ofRectangle& rect)
{
    ofSetWindowPosition(rect.x, rect.y);
    ofSetWindowShape(rect.width, rect.height);
}
