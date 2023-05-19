#pragma once

#include "ofMain.h"

class History
{
public:
    static const int MAX_HISTORY_COUNT;
    
protected:
    ofParameterGroup& mOriginator;
    vector<shared_ptr<ofJson> > mSnapshots;
    int mBackCount;
    
public:
    History(ofParameterGroup& original);
    virtual ~History() = default;
    
    void saveSnapshot();
    void clearSnapshots();

    void undo();
    void redo();
};
