#include "ofMain.h"
#include "ofApp.h"
#include "DisplayApp.hpp"
#include "ofAppGLFWWindow.h"
#include "libs/cmdline.h"

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char* argv[])
{
    cmdline::parser ps;
    ps.add<int>("posx", 'x', "Display window position X", false, 200);
    ps.add<int>("posy", 'y', "Display window position Y", false, 200);
    ps.add<int>("width", 'w', "Display window width", false, 1920/2);
    ps.add<int>("height", 'h', "Display window height", false, 1080/2);
    ps.add("nodecorate", 'd', "Display window disable decoration");
    ps.add("multi", 'm', "Display window enable multi monitor fullscreen");
    ps.parse_check(argc, argv);

    ofGLFWWindowSettings s;
    
    // Set default values
    s.setSize(1920/2, 1080/2);
    s.resizable = true;
    
    shared_ptr<ofAppBaseWindow> consoleWindow = ofCreateWindow(s);
    
    int x = ps.get<int>("posx");
    int y = ps.get<int>("posy");
    int w = ps.get<int>("width");
    int h = ps.get<int>("height");
    s.setPosition(glm::vec2(x, y));
    s.setSize(w, h);

    s.decorated = !ps.exist("nodecorate");
    s.multiMonitorFullScreen = ps.exist("multi");

    s.shareContextWith = consoleWindow;
    shared_ptr<ofAppBaseWindow> displayWindow = ofCreateWindow(s);
    
    shared_ptr<ofApp> mainApp(new ofApp);
    shared_ptr<DisplayApp> displayApp(new DisplayApp);
    mainApp->displayApp = displayApp;
    
    ofRunApp(consoleWindow, mainApp);
    ofRunApp(displayWindow, displayApp);
    return ofRunMainLoop();
}
