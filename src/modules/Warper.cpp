#include "Warper.hpp"
#include "opencv2/calib3d.hpp"

//----------------------------------------------------- Constructor

Warper::Warper()
: anchorSize(10)
, anchorSizeHalf(anchorSize * 0.5)
, selectedCornerIndex(-1)
, highlightCornerIndex(-1)
{
}

//----------------------------------------------------- Local Functions

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

void Warper::drawQuadOutline()
{
    for(int i=0; i<4; i++) {
        int j = (i+1) % 4;
        ofDrawLine(dstPoints[i].x + position.x,
                   dstPoints[i].y + position.y,
                   dstPoints[j].x + position.x,
                   dstPoints[j].y + position.y);
    }
}

void Warper::drawCorners()
{
    for(int i=0; i<4; i++) {
        ofPoint & point = dstPoints[i];
        drawCornerAt(point);
    }
}

void Warper::drawHighlightedCorner()
{
    if(highlightCornerIndex < 0 || highlightCornerIndex > 3) {
        return;
    }
    
    ofPoint & point = dstPoints[highlightCornerIndex];
    drawCornerAt(point);
}

void Warper::drawSelectedCorner()
{
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }
    
    ofPoint & point = dstPoints[selectedCornerIndex];
    drawCornerAt(point);
}

void Warper::drawCornerAt(const ofPoint & point)
{
    ofDrawRectangle(point.x + position.x - anchorSizeHalf,
                    point.y + position.y - anchorSizeHalf,
                    anchorSize, anchorSize);
}

//----------------------------------------------------- GUI

void Warper::drawGui()
{
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

//----------------------------------------------------- Corners

void Warper::setSourceRect(const ofRectangle& r)
{
    srcPoints[0].set(r.x, r.y);
    srcPoints[1].set(r.x + r.width, r.y);
    srcPoints[2].set(r.x + r.width, r.y + r.height);
    srcPoints[3].set(r.x, r.y + r.height);
}

void Warper::setTargetRect(const ofRectangle& r)
{
    dstPoints[0].set(r.x, r.y);
    dstPoints[1].set(r.x + r.width, r.y);
    dstPoints[2].set(r.x + r.width, r.y + r.height);
    dstPoints[3].set(r.x, r.y + r.height);
}

void Warper::reset()
{
    dstPoints[0].set(srcPoints[0]);
    dstPoints[1].set(srcPoints[1]);
    dstPoints[2].set(srcPoints[2]);
    dstPoints[3].set(srcPoints[3]);
}

//----------------------------------------------------- Matrix

ofMatrix4x4 Warper::getMatrix() const
{
    return getMatrix(&srcPoints[0], &dstPoints[0]);
}

ofMatrix4x4 Warper::getMatrix(const ofPoint* srcPoints, const ofPoint* dstPoints) const
{
    // ***** Original code is ofxQuadWarp *****
    
    //Store the source and destination points into cv::Mat matrices
    cv::Mat src_mat = cv::Mat(4, 2, CV_32FC1);
    cv::Mat dst_mat = cv::Mat(4, 2, CV_32FC1);
    for (int i = 0; i < 4; i++) {
        src_mat.at<float>(i, 0) = srcPoints[i].x;
        src_mat.at<float>(i, 1) = srcPoints[i].y;
        dst_mat.at<float>(i, 0) = dstPoints[i].x;
        dst_mat.at<float>(i, 1) = dstPoints[i].y;
    }
    
    //figure out the warping!
    //warning - older versions of openCV had a bug
    //in this function.
    cv::Mat translate = cv::findHomography(src_mat, dst_mat);
    
    //we need to copy these values
    //from the 3x3 2D openCV matrix which is row ordered
    //
    // ie:   [0][1][2] x
    //       [3][4][5] y
    //       [6][7][8] w
    
    //to openGL's 4x4 3D column ordered matrix
    //        x  y  z  w
    // ie:   [0][3][ ][6]
    //       [1][4][ ][7]
    //         [ ][ ][ ][ ]
    //       [2][5][ ][9]
    //
    
    ofMatrix4x4 matrixTemp;
    matrixTemp.getPtr()[0]  = translate.at<double>(0);
    matrixTemp.getPtr()[4]  = translate.at<double>(1);
    matrixTemp.getPtr()[12] = translate.at<double>(2);
    
    matrixTemp.getPtr()[1]  = translate.at<double>(3);
    matrixTemp.getPtr()[5]  = translate.at<double>(4);
    matrixTemp.getPtr()[13] = translate.at<double>(5);
    
    matrixTemp.getPtr()[3]  = translate.at<double>(6);
    matrixTemp.getPtr()[7]  = translate.at<double>(7);
    matrixTemp.getPtr()[15] = translate.at<double>(8);
    
    translate.release();
    src_mat.release();
    dst_mat.release();
    
    return matrixTemp;
}

//----------------------------------------------------- User Inputs

void Warper::mouseMoved(int x, int y)
{
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
}


void Warper::keyPressed(int key)
{
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

//----------------------------------------------------- save / load

void Warper::save(const string& path)
{
    ofXml xml;
    xml.appendChild("quadwarp");
    
    ofXml src = xml.getChild("quadwarp").appendChild("src");
    for (int i = 0; i < 4; i++) {
        auto t = src.appendChild("point");
        t.setAttribute("x", ofToString(srcPoints[i].x));
        t.setAttribute("y", ofToString(srcPoints[i].y));
    }
    
    ofXml dst = xml.getChild("quadwarp").appendChild("dst");
    for (int i = 0; i < 4; i++) {
        auto t = dst.appendChild("point");
        t.setAttribute("x", ofToString(dstPoints[i].x));
        t.setAttribute("y", ofToString(dstPoints[i].y));
    }
    
    xml.save(path);
}

void Warper::load(const string& path)
{
    ofXml xml;
    if (!xml.load(path)) return;
    
    auto src = xml.getChild("quadwarp").getChild("src");
    int i = 0;
    for (auto it = src.getChildren().begin(); it != src.getChildren().end(); it++) {
        srcPoints[i].x = it->getAttribute("x").getFloatValue();
        srcPoints[i].y = it->getAttribute("y").getFloatValue();
        i++;
    }
    
    auto dst = xml.getChild("quadwarp").getChild("dst");
    i = 0;
    for (auto it = dst.getChildren().begin(); it != dst.getChildren().end(); it++) {
        dstPoints[i].x = it->getAttribute("x").getFloatValue();
        dstPoints[i].y = it->getAttribute("y").getFloatValue();
        i++;
    }
}
