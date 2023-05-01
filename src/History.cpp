#include "History.hpp"

const int History::MAX_HISTORY_COUNT = 100;

History::History(ofParameterGroup& original)
: mOriginator(original)
, mBackCount(0)
{
    mSnapshots.reserve(MAX_HISTORY_COUNT);
}

void History::pushSnapshot()
{
    if (mSnapshots.size() == MAX_HISTORY_COUNT)
    {
        mSnapshots.erase(mSnapshots.begin());
    }
    
    mXml.clear();
    mOriginator >> mXml;
    string s = mXml.toString();
    mSnapshots.emplace_back(s);
}

void History::popSnapshot()
{
    if (mSnapshots.empty()) return;
    if (mBackCount >= mSnapshots.size()) return;
    
    mBackCount++;
    string s = *(mSnapshots.end() - mBackCount);
    mXml.clear();
    mXml.parse(s);
    mXml >> mOriginator;
}

void History::clearSnapshots()
{
    mSnapshots.clear();
    mBackCount = 0;
}
