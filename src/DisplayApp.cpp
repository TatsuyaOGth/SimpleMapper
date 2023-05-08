#include "DisplayApp.hpp"

void DisplayApp::setup()
{
    ofSetWindowTitle("Simple Mapper | Display");
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

void DisplayApp::render(const ofTexture & tex)
{
    if (!mFbo.isAllocated() || (mFbo.getWidth() != tex.getWidth() || mFbo.getHeight() != tex.getHeight()))
    {
        mFbo.clear();
        mFbo.allocate(tex.getWidth(), tex.getHeight(), GL_RGB, 0);
    }
    
    mFbo.begin();
    tex.draw(0, 0);
    mFbo.end();
}


void DisplayApp::keyPressed(int key)
{
    if (key == 'f')
    {
        ofToggleFullscreen();
    }
}
