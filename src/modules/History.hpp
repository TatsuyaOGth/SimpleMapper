#pragma once

#include "ofMain.h"

class History
{
public:
    static const int MAX_HISTORY_COUNT;
    
protected:
    ofParameterGroup& mOriginator;
    ofXml mXml;
    vector<string> mSnapshots;
    int mBackCount;
    
public:
    History(ofParameterGroup& original);
    virtual ~History() = default;
    
    void pushSnapshot();
    void popSnapshot();
    void clearSnapshots();
};
