#pragma once

#include "ofMain.h"
#include "DisplayApp.hpp"
#include "ofxGui.h"
#include "Warper.hpp"
#include "Receiver.hpp"
#include "History.hpp"

class ofApp : public ofBaseApp
{
    class Map
    {
        ofApp& mom;

        ofParameterGroup mSettings;
        ofParameter<bool> mEnabled;

        ofParameterGroup mSrcParams;
        ofParameter<ofRectangle> mSrcRect;

        ofParameterGroup mDstParams;
        ofParameter<bool> mFlipH;
        ofParameter<bool> mFlipV;

        shared_ptr<Warper> mWarper;

    public:
        inline ofParameterGroup& getSettings() { return mSettings; }
        inline bool isEnabled() { return mEnabled; }

        inline const shared_ptr<Warper>& getWarper() { return mWarper; }

    public:
        Map(ofApp& master);

        void setup(const string& name);

        void onSrcRectValueChanged(ofRectangle& v);
        void onWarperUpdated(Warper& warper);

        void draw(Receiver& receiver, bool drawTestPattern);
        void drawGui();

        void reset();
        void reset(glm::vec2 center);

        void load();
        void save();

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y);
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
    };

    vector<shared_ptr<Map> > mMaps;

    ofParameterGroup mSettings;

    ofParameterGroup mGlobalSettings;
    ofParameter<int> mSenderId;
    ofParameter<bool> mUseAlpha;
    ofParameter<bool> mUseNdi;
    ofParameter<glm::vec2> mDstSize;
    ofParameter<ofRectangle> mDispWindowRect;
    ofParameter<int> mTargetFrameRate;
    ofParameter<bool> mVerticalSync;

    ofxPanel mGui;
    ofxIntSlider mMapId;
    ofxButton mApplyButton;
    
    shared_ptr<Receiver> mReceiver;
    shared_ptr<History> mHistory;
    
    ofFbo mFbo;
	bool mVisibledSettings;
    ofParameter<bool> mIsDirty;
    bool mDirtyWhileMousePressed;
    int mTestPatternMode;

    ofEasyCam mCam;
    
public:
    // display app instance
    shared_ptr<DisplayApp> displayApp;

public:
	void setup();
	void update();
	void draw();
	void exit();
    
	void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    bool load();
    void save();
    void applySettings();

    void setupGui();
    bool onLoadIconPressed();
    bool onSaveIconPressed();
    void onMapIdChanged(int& id);
    void onDispWindowRectChanged(ofRectangle& v);
    void onParameterChanged(ofAbstractParameter& p);
    void onIsDirtyChanged(bool& v);

    void setupCamera();
    glm::vec2 screenToWorld(int x, int y);

    void switchMap(int mapId);
	void drawMaps();
};
