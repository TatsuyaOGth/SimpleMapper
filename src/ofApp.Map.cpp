#include "ofApp.h"
#include "Common.hpp"
#include <regex>

ofApp::Map::Map(ofApp& master)
    : mom(master)
{
}

void ofApp::Map::setup(const string& name)
{
    mWarper = make_shared<Warper>();

    // Setup parameters

    mSettings.setName(name);
    mSettings.add(mEnabled.set("Enabled", false));

    mSrcParams.setName("Source");
    mSrcParams.add(mSrcRect.set("Crop",
        ofRectangle(0, 0, 1920, 1080),
        ofRectangle(0, 0, 8, 8),
        ofRectangle(4096, 4096, 4096, 4096)));

    mDstParams.setName("Destination");
    auto& pts = mWarper->getDstPoints();
    for (int i = 0; i < pts.size(); ++i)
    {
        pts[i].setName("Point" + ofToString(i));
        pts[i].setMin(glm::vec2(0, 0));
        pts[i].setMax(glm::vec2(4096, 4096));
        mDstParams.add(pts[i]);
    }
    mDstParams.add(mFlipH.set("Flip H", false));
    mDstParams.add(mFlipV.set("Flip V", false));

    mSettings.add(mSrcParams);
    mSettings.add(mDstParams);

    mSrcRect.addListener(this, &ofApp::Map::onSrcRectValueChanged);

    
    mWarper->setTargetRect(mSrcRect);
    
    //ofAddListener(mWarper->updatedE, this, &ofApp::Map::onWarperUpdated);
}

void ofApp::Map::onSrcRectValueChanged(ofRectangle& v)
{
    auto rect = ofRectangle(0, 0, v.width, v.height);
    mWarper->setSourceRect(rect);
}

void ofApp::Map::onWarperUpdated(Warper& warper)
{
    mom.mIsDirty = true;
}

void ofApp::Map::draw(Receiver& receiver, bool drawTestPattern)
{
    ofPushMatrix();
    ofMultMatrix(mWarper->getMatrix());

    if (receiver.isReady())
    {
        receiver.draw(mSrcRect, mFlipH, mFlipV);
    }

    if (drawTestPattern)
    {
        Common::drawTestPattern(mSrcRect.get().width, mSrcRect.get().height);
    }

    ofPopMatrix();
}

void ofApp::Map::drawGui()
{
    mWarper->drawGui();
}

void ofApp::Map::reset()
{
    mWarper->reset();
}

void ofApp::Map::reset(glm::vec2 center)
{
    mWarper->reset(center);
}

void ofApp::Map::load()
{
    string fname = regex_replace(mSettings.getName(), regex(" "), "_") + "_warp.xml";
    mWarper->load(fname);
}

void ofApp::Map::save()
{
    string fname = regex_replace(mSettings.getName(), regex(" "), "_") + "_warp.xml";
    mWarper->save(fname);
}

void ofApp::Map::keyPressed(int key)
{
    mWarper->keyPressed(key);
}

void ofApp::Map::keyReleased(int key)
{
    mWarper->keyReleased(key);
}

void ofApp::Map::mouseMoved(int x, int y)
{
    mWarper->mouseMoved(x, y);
}

void ofApp::Map::mouseDragged(int x, int y, int button)
{
    mWarper->mouseDragged(x, y, button);
}

void ofApp::Map::mousePressed(int x, int y, int button)
{
    mWarper->mousePressed(x, y, button);
}

void ofApp::Map::mouseReleased(int x, int y, int button)
{
    mWarper->mouseReleased(x, y, button);
}
