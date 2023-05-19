#include "Warper.hpp"
#include "opencv2/calib3d.hpp"
#include "Common.hpp"

//----------------------------------------------------- Constructor

Warper::Warper()
: mAnchorSize(20)
{
    mDstPoints[0].xSyncFrags = { true, false, false, true };
    mDstPoints[0].ySyncFrags = { true, true, false, false };
    
    mDstPoints[1].xSyncFrags = { false, true, true, false };
    mDstPoints[1].ySyncFrags = { true, true, false, false };
    
    mDstPoints[2].xSyncFrags = { false, true, true, false };
    mDstPoints[2].ySyncFrags = { false, false, true, true };
    
    mDstPoints[3].xSyncFrags = { true, false, false, true };
    mDstPoints[3].ySyncFrags = { false, false, true, true };
}

//----------------------------------------------------- Local Functions

void Warper::drawQuadOutline()
{
    for(int i=0; i<4; i++)
    {
        int j = (i+1) % 4;
        ofDrawLine(mDstPoints[i], mDstPoints[j]);
    }
}

void Warper::drawCorners()
{
    for(int i=0; i<4; i++)
    {
        auto& point = mDstPoints[i];
        drawCornerAt(point);
    }
}

void Warper::drawHighlightedCorner()
{
    for (const auto& p : mDstPoints)
    {
        if (p.highlighted)
        {
            drawCornerAt(p);
        }
    }
}

void Warper::drawSelectedCorner()
{
    for (const auto& p : mDstPoints)
    {
        if (p.selected)
        {
            drawCornerAt(p);
        }
    }
}

void Warper::drawGrabbedCorner()
{
    for (const auto& p : mDstPoints)
    {
        if (p.grabbed)
        {
            drawCornerAt(p, 1.8);
        }
    }
}

void Warper::drawCornerAt(const glm::vec2 & point, float scale)
{
    ofDrawCircle(point.x, point.y, mAnchorSize * 0.5 * scale);
}


void Warper::selectPoint(WarperPoint &point, bool onlyThis, bool toggle)
{
    if (onlyThis)
    {
        for (auto& p : mDstPoints) p.selected = false;
    }
    if (toggle)
    {
        point.selected = !point.selected;
    }
    else
    {
        point.selected = true;
    }
    point.anchor = point;
}

void Warper::grabPoint(WarperPoint &point, const glm::vec2& controlPoint)
{
    for (auto& p : mDstPoints)
    {
        p.anchor = p;
        p.offset = p - controlPoint;
        p.grabbed = false;
    }
    point.grabbed = true;
}

void Warper::switchSelect(bool prev)
{
    auto it = find_if(mDstPoints.begin (), mDstPoints.end(), [](WarperPoint& p) { return p.selected; });
    if (it == mDstPoints.end())
    {
        mDstPoints[0].selected = true;
        return;
    }
    
    for (auto& p : mDstPoints) p.selected = false;
    if (prev)
    {
        if (it == mDstPoints.begin())
            it = mDstPoints.end() - 1;
        else
            it--;
        it->selected = true;
    }
    else
    {
        it++;
        if (it == mDstPoints.end()) it = mDstPoints.begin();
        it->selected = true;
    }
}

void Warper::movePoint(const glm::vec2& target, bool invertMode)
{
    if (invertMode)
    {
        auto grabbed = find_if(mDstPoints.begin(), mDstPoints.end(), [](WarperPoint& p) { return p.grabbed; });
        if (grabbed == mDstPoints.end()) return;
        
        size_t grabbedIndex = std::distance(mDstPoints.begin(), grabbed);
        for (int i = 0; i < 4; ++i)
        {
            auto& p = mDstPoints[i];
            if (!p.selected) continue;
            
            auto mv = target + p.offset - p.anchor;
            
            float xFrag = p.xSyncFrags[grabbedIndex] ? 1 : -1;
            float yFrag = p.ySyncFrags[grabbedIndex] ? 1 : -1;
            float x = p.anchor.x + (mv.x * xFrag);
            float y = p.anchor.y + (mv.y * yFrag);
            p.set(glm::vec2(x, y));
        }
    }
    else
    {
        for (auto& p : mDstPoints)
        {
            if (p.selected)
            {
                p.set(target + p.offset);
            }
        }
    }
    //ofNotifyEvent(updatedE, *this);
}

//----------------------------------------------------- GUI

void Warper::drawGui()
{
    ofPushStyle();
    ofNoFill();
    
    ofSetColor(ofColor::magenta);
    drawQuadOutline();
    
    ofSetColor(ofColor::yellow);
    drawCorners();
    
    ofSetColor(ofColor::magenta);
    drawHighlightedCorner();
    
    ofSetColor(ofColor::red);
    drawSelectedCorner();
    
    ofSetColor(ofColor::yellow);
    drawGrabbedCorner();
    
    ofPopStyle();
}

//----------------------------------------------------- Corners

void Warper::setSourceRect(const ofRectangle& r)
{
    mSrcPoints[0].set(glm::vec2(r.x, r.y));
    mSrcPoints[1].set(glm::vec2(r.x + r.width, r.y));
    mSrcPoints[2].set(glm::vec2(r.x + r.width, r.y + r.height));
    mSrcPoints[3].set(glm::vec2(r.x, r.y + r.height));
    //ofNotifyEvent(updatedE, *this);
}

void Warper::setTargetRect(const ofRectangle& r)
{
    mDstPoints[0].set(glm::vec2(r.x, r.y));
    mDstPoints[1].set(glm::vec2(r.x + r.width, r.y));
    mDstPoints[2].set(glm::vec2(r.x + r.width, r.y + r.height));
    mDstPoints[3].set(glm::vec2(r.x, r.y + r.height));
    //ofNotifyEvent(updatedE, *this);
}

void Warper::reset()
{
    mDstPoints[0].set(mSrcPoints[0]);
    mDstPoints[1].set(mSrcPoints[1]);
    mDstPoints[2].set(mSrcPoints[2]);
    mDstPoints[3].set(mSrcPoints[3]);
    //ofNotifyEvent(updatedE, *this);
}

void Warper::reset(glm::vec2 center)
{
    mDstPoints[0].set(mSrcPoints[0]);
    mDstPoints[1].set(mSrcPoints[1]);
    mDstPoints[2].set(mSrcPoints[2]);
    mDstPoints[3].set(mSrcPoints[3]);
    float w = mSrcPoints[1].get().x;
    float h = mSrcPoints[2].get().y;
    for (auto& p : mDstPoints) p += center;
    for (auto& p : mDstPoints) p -= glm::vec2(w * 0.5, h * 0.5);
    //ofNotifyEvent(updatedE, *this);
}

//----------------------------------------------------- Matrix

ofMatrix4x4 Warper::getMatrix() const
{
    // ***** Original code is ofxQuadWarp *****
    
    //Store the source and destination points into cv::Mat matrices
    ofMatrix4x4 matrixTemp;
    cv::Mat src_mat = cv::Mat(4, 2, CV_32FC1);
    cv::Mat dst_mat = cv::Mat(4, 2, CV_32FC1);
    for (int i = 0; i < 4; i++) {
        src_mat.at<float>(i, 0) = mSrcPoints[i].get().x;
        src_mat.at<float>(i, 1) = mSrcPoints[i].get().y;
        dst_mat.at<float>(i, 0) = mDstPoints[i].get().x;
        dst_mat.at<float>(i, 1) = mDstPoints[i].get().y;
    }
    
    //figure out the warping!
    //warning - older versions of openCV had a bug
    //in this function.
    cv::Mat translate = cv::findHomography(src_mat, dst_mat);
    
    if (translate.empty()) return matrixTemp;
    
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
    //       [ ][ ][ ][ ]
    //       [2][5][ ][9]
    //
    
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
    glm::vec2 mousePoint(x, y);
    
    for (auto& p : mDstPoints)
    {
        if(glm::distance(mousePoint, p.get()) <= mAnchorSize * 0.5)
        {
            p.highlighted = true;
            return;
        }
    }
    
    for (auto& p : mDstPoints) p.highlighted = false;
}

void Warper::mousePressed(int x, int y, int button)
{
    glm::vec2 mousePoint(x, y);
    
    if (button == OF_MOUSE_BUTTON_LEFT)
    {
        if (!Common::isShiftKeyPressed())
        {
            if (count_if(mDstPoints.begin(), mDstPoints.end(), [](WarperPoint& p) { return p.selected; }) == 1)
            {
                for (auto& p : mDstPoints) p.selected = false;
            }
        }
        
        for (auto& p : mDstPoints)
        {
            if(glm::distance(mousePoint, p.get()) <= mAnchorSize * 0.5)
            {
                selectPoint(p, false);
                grabPoint(p, mousePoint);
                return;
            }
        }
        
        for (auto& p : mDstPoints)
        {
            p.selected = false;
            p.grabbed = false;
        }
    }
}

void Warper::mouseDragged(int x, int y, int button)
{
    glm::vec2 mousePoint(x, y);

    if (button == OF_MOUSE_BUTTON_LEFT)
    {
        if (find_if(mDstPoints.begin(), mDstPoints.end(), [](WarperPoint& p) { return p.selected; }) == mDstPoints.end()) return;
        movePoint(mousePoint, Common::isCtrlKeyPressed());
    }
}

void Warper::mouseReleased(int x, int y, int button)
{
}


void Warper::keyPressed(int key)
{
    switch (key)
    {
        case OF_KEY_TAB:
            switchSelect(Common::isShiftKeyPressed());
            return;

        case 'a':
            selectPoint(mDstPoints[0], false);
            selectPoint(mDstPoints[1], false);
            selectPoint(mDstPoints[2], false);
            selectPoint(mDstPoints[3], false);
            return;
    }
    
    if (find_if(mDstPoints.begin(), mDstPoints.end(), [](WarperPoint& p) { return p.selected; }) == mDstPoints.end()) return;
    
    
    float nudgeAmount = Common::isShiftKeyPressed() ? 10 : 0.3;
    glm::vec2 moveAmount(0, 0);
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
            return;
    }

    if (Common::isCtrlKeyPressed())
    {
        const auto it = find_if(mDstPoints.begin(), mDstPoints.end(), [](WarperPoint& p) { return p.selected; });
        if (it != mDstPoints.end())
        {
            grabPoint(*it, *it);
            auto target = (*it) + moveAmount;
            movePoint(target, true);
        }
    }
    else
    {
        for (auto& p : mDstPoints)
        {
            if (p.selected)
            {
                p += moveAmount;
            }
        }
        //ofNotifyEvent(updatedE, *this);
    }
}

void Warper::keyReleased(int key)
{
}

//----------------------------------------------------- save / load

void Warper::save(const string& path)
{
    ofXml xml;
    xml.appendChild("quadwarp");
    
    ofXml src = xml.getChild("quadwarp").appendChild("src");
    for (int i = 0; i < 4; i++) {
        auto t = src.appendChild("point");
        t.setAttribute("x", ofToString(mSrcPoints[i].get().x));
        t.setAttribute("y", ofToString(mSrcPoints[i].get().y));
    }
    
    ofXml dst = xml.getChild("quadwarp").appendChild("dst");
    for (int i = 0; i < 4; i++) {
        auto t = dst.appendChild("point");
        t.setAttribute("x", ofToString(mDstPoints[i].get().x));
        t.setAttribute("y", ofToString(mDstPoints[i].get().y));
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
        float x = it->getAttribute("x").getFloatValue();
        float y = it->getAttribute("y").getFloatValue();
        mSrcPoints[i].set(glm::vec2(x, y));
        i++;
    }
    
    auto dst = xml.getChild("quadwarp").getChild("dst");
    i = 0;
    for (auto it = dst.getChildren().begin(); it != dst.getChildren().end(); it++) {
        float x = it->getAttribute("x").getFloatValue();
        float y = it->getAttribute("y").getFloatValue();
        mDstPoints[i].set(glm::vec2(x, y));
        i++;
    }
}
