#include "ofMain.h"
#include "ofApp.h"
#include "DisplayApp.hpp"
#include "ofAppGLFWWindow.h"

int main()
{
    ofGLFWWindowSettings s;
    
    // Set default values
    s.setSize(1920, 1080);
    s.resizable = true;
    
    shared_ptr<ofAppBaseWindow> consoleWindow = ofCreateWindow(s);
    
    s.shareContextWith = consoleWindow;
    shared_ptr<ofAppBaseWindow> displayWindow = ofCreateWindow(s);
    
    
    shared_ptr<ofApp> mainApp(new ofApp);
    shared_ptr<DisplayApp> displayApp(new DisplayApp);
    mainApp->displayApp = displayApp;
    
    ofRunApp(consoleWindow, mainApp);
    ofRunApp(displayWindow, displayApp);
    return ofRunMainLoop();
}
