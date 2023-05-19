#include "History.hpp"

const int History::MAX_HISTORY_COUNT = 100;

History::History(ofParameterGroup& original)
: mOriginator(original)
, mBackCount(0)
{
    mSnapshots.reserve(MAX_HISTORY_COUNT);
}

void History::saveSnapshot()
{
    if (mSnapshots.size() == MAX_HISTORY_COUNT)
    {
        mSnapshots.erase(mSnapshots.begin());
    }

    if (mBackCount > 1)
    {
        mSnapshots.erase(mSnapshots.end() - (mBackCount - 1), mSnapshots.end());
    }
    
    auto s = make_shared<ofJson>();
    ofSerialize(*s, mOriginator);
    mSnapshots.emplace_back(s);
    mBackCount = 1;
    ofLogVerbose("History") << "save " << mSnapshots.size();
}

void History::clearSnapshots()
{
    mSnapshots.clear();
    mBackCount = 1;
    ofLogVerbose("History") << "clear " << mSnapshots.size();
}

void History::undo()
{
    if (mSnapshots.empty()) return;
    if (mBackCount >= mSnapshots.size()) return;
    
    mBackCount++;
    auto& s = *(mSnapshots.end() - mBackCount);
    ofDeserialize(*s, mOriginator);
    ofLogVerbose("History") << "undo " << mBackCount << "/" << mSnapshots.size();
}

void History::redo()
{
    if (mBackCount <= 1) return;

    mBackCount--;
    auto& s = *(mSnapshots.end() - mBackCount);
    ofDeserialize(*s, mOriginator);
    ofLogVerbose("History") << "redo " << mBackCount << "/" << mSnapshots.size();
}
