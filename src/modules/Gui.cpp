#include "Gui.hpp"

static const string smSetupFileName = "settings.xml";

void Gui::setup()
{
    mApplyButton.addListener(this, &Gui::onApplyButtonPressed);
    mLoadButton.addListener(this, &Gui::onLoadButtonPressed);
    mSaveButton.addListener(this, &Gui::onSaveButtonPressed);

    mTexParams.setName("Input Texture");
    mTexParams.add(mUseNdi.set("Use NDI", false));
    mTexParams.add(mTexWidth.set("Width", 640, 10, 4096));
    mTexParams.add(mTexHeight.set("Height", 480, 10, 4096));
    mTexParams.add(mFlipH.set("Flip H", false));
    mTexParams.add(mFlipV.set("Flip V", false));
    mTexParams.add(mSenderId.set("Sender ID", 0, 0, 8));

    mGui.setup("Settings");
    mGui.add(mTexParams);
    mGui.add(mApplyButton.setup("Apply"));
    mGui.add(mSaveButton.setup("Save"));
}

void Gui::draw()
{
    ofPushStyle();

    ofSetColor(ofColor::white);
    mGui.draw();

    ofPopStyle();
}

void Gui::load()
{
    mGui.loadFromFile(smSetupFileName);
    ofNotifyEvent(loadedEvent, this);
}

void Gui::save()
{
    mGui.saveToFile(smSetupFileName);
    ofNotifyEvent(savedEvent, this);
}

void Gui::onApplyButtonPressed()
{
    ofNotifyEvent(applyEvent, this);
}

void Gui::onLoadButtonPressed()
{
    load();
}

void Gui::onSaveButtonPressed()
{
    save();
}
