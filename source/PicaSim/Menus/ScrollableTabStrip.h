#ifndef SCROLLABLETABSTRIP_H
#define SCROLLABLETABSTRIP_H

#include "ScrollHelper.h"

class ScrollableTabStrip
{
public:
    // Renders the tab strip.
    // labels: array of tab label strings
    // count: number of tabs
    // selectedTab: current selection (in/out)
    // height: button height in pixels
    // id: unique ImGui ID string
    void Render(const char* id, const char** labels, int count, int& selectedTab, float height);

private:
    ScrollHelper mScrollHelper;
    float mScrollOffset = 0.0f;
    float mTotalTabsWidth = 0.0f;
    int   mLastSelectedTab = -1;
    bool  mWasDragging = false;
};

#endif // SCROLLABLETABSTRIP_H
