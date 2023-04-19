#include "Warper.hpp"

ofPoint& Warper::getNextDstPoint(int selectedIndex)
{
    int i = (selectedIndex + 1) % 4;
    return dstPoints[i];
}

ofPoint& Warper::getDiagonalDstPoint(int selectedIndex)
{
    int i = (selectedIndex + 2) % 4;
    return dstPoints[i];
}

void Warper::drawGui()
{
    if (!isShowing()) show();

    ofPushStyle();

    ofSetColor(ofColor::magenta);
    drawQuadOutline();

    ofSetColor(ofColor::yellow);
    drawCorners();

    ofSetColor(ofColor::magenta);
    drawHighlightedCorner();

    ofSetColor(ofColor::red);
    drawSelectedCorner();

    ofPopStyle();
}

void Warper::mouseMoved(int x, int y)
{
    if (!bShow) return;
    
    ofPoint mousePoint(x, y);
    mousePoint -= position;
    for(int i=0; i<4; i++) {
        ofPoint & dstPoint = dstPoints[i];
        if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
            highlightCornerIndex = i;
            return;
        }
    }
    highlightCornerIndex = -1;
}

void Warper::mousePressed(int x, int y, int button)
{
    if (!bShow) return;
    
    ofPoint mousePoint(x, y);
    mousePoint -= position;
    
    if (button == OF_MOUSE_BUTTON_LEFT)
    {
        for(int i=0; i<4; i++) {
            ofPoint & dstPoint = dstPoints[i];
            if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
                dstPoint.set(mousePoint);
                selectedCornerIndex = i;
                mSubPointOffset = getNextDstPoint(selectedCornerIndex) - mousePoint;
                mSelectedPos = mousePoint;
                return;
            }
        }
        selectedCornerIndex = -1;
    }
    else if (button == OF_MOUSE_BUTTON_MIDDLE)
    {
        for (int i = 0; i < 4; ++i)
        {
            mDstPointOffsets[i] = dstPoints[i] - mousePoint;
        }
    }
}

void Warper::mouseDragged(int x, int y, int button)
{
    if (!bShow) return;
    

    
    ofPoint mousePoint(x, y);
    mousePoint -= position;
    
    if (button == OF_MOUSE_BUTTON_LEFT)
    {
        if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
            return;
        }
        
        if (mShiftKeyPressed)
        {
            ofPoint & p1 = dstPoints[selectedCornerIndex];
            ofPoint & p2 = getNextDstPoint(selectedCornerIndex);
            p1.set(mousePoint);
            p2.set(mousePoint + mSubPointOffset);
        }
        else if (mAltKeyPressed)
        {
            ofPoint & p1 = dstPoints[selectedCornerIndex];
            ofPoint & p2 = getNextDstPoint(selectedCornerIndex);
            p1.set(mousePoint);
            auto p2p = mSelectedPos + (mSelectedPos - mousePoint) + mSubPointOffset;
            p2.set(p2p);
        }
        else
        {
            dstPoints[selectedCornerIndex].set(mousePoint);
        }
    }
    else if (button == OF_MOUSE_BUTTON_MIDDLE)
    {
        for (int i = 0; i < 4; ++i)
        {
            dstPoints[i] = mousePoint + mDstPointOffsets[i];
        }
    }
}

void Warper::mouseReleased(int x, int y, int button)
{
//    if (!bShow) return;
//
//    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
//        return;
//    }
//
//    if (button == OF_MOUSE_BUTTON_LEFT)
//    {
//        ofPoint mousePoint(x, y);
//        mousePoint -= position;
//        dstPoints[selectedCornerIndex].set(mousePoint);
//    }
}


void Warper::keyPressed(int key)
{
    if (!bShow) return;
    
    if (key == OF_KEY_SHIFT) mShiftKeyPressed = true;
    if (key == OF_KEY_ALT) mAltKeyPressed = true;
    if (key == OF_KEY_CONTROL || key == OF_KEY_COMMAND) mCtrlKeyPressed = true;
    
    switch (key)
    {
        case '1':
            selectedCornerIndex = 0;
            mSelectedPos = dstPoints[selectedCornerIndex];
            mSubPointOffset = getNextDstPoint(selectedCornerIndex) - mSelectedPos;
            break;
        case '2':
            selectedCornerIndex = 1;
            mSelectedPos = dstPoints[selectedCornerIndex];
            mSubPointOffset = getNextDstPoint(selectedCornerIndex) - mSelectedPos;
            break;
        case '3':
            selectedCornerIndex = 2;
            mSelectedPos = dstPoints[selectedCornerIndex];
            mSubPointOffset = getNextDstPoint(selectedCornerIndex) - mSelectedPos;
            break;
        case '4':
            selectedCornerIndex = 3;
            mSelectedPos = dstPoints[selectedCornerIndex];
            mSubPointOffset = getNextDstPoint(selectedCornerIndex) - mSelectedPos;
            break;
        default:
            break;
    }
    
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3)
    {
        return;
    }
    
    float nudgeAmount = mShiftKeyPressed ? 10 : 0.3;
    ofPoint moveAmount(0, 0);
    switch (key)
    {
        case OF_KEY_LEFT:
            moveAmount.x -= nudgeAmount;
            break;
        case OF_KEY_RIGHT:
            moveAmount.x += nudgeAmount;
            break;
        case OF_KEY_UP:
            moveAmount.y -= nudgeAmount;
            break;
        case OF_KEY_DOWN:
            moveAmount.y += nudgeAmount;
            break;
        default:
            break;
    }
    
    if (mAltKeyPressed)
    {
        ofPoint & p1 = dstPoints[selectedCornerIndex];
        ofPoint & p2 = getNextDstPoint(selectedCornerIndex);
        p1 += moveAmount;
        p2 += moveAmount;
    }
    else if (mCtrlKeyPressed)
    {
        ofPoint & p1 = dstPoints[selectedCornerIndex];
        ofPoint & p2 = getDiagonalDstPoint(selectedCornerIndex);
        p1 += moveAmount;
        p2 -= moveAmount;
    }
    else
    {
        ofPoint & selectedPoint = dstPoints[selectedCornerIndex];
        selectedPoint += moveAmount;
    }
}

void Warper::keyReleased(int key)
{
    if (key == OF_KEY_SHIFT) mShiftKeyPressed = false;
    if (key == OF_KEY_ALT) mAltKeyPressed = false;
    if (key == OF_KEY_CONTROL || key == OF_KEY_COMMAND) mCtrlKeyPressed = false;
}
