#include "Warper.hpp"
#include "opencv2/calib3d.hpp"
#include "Common.hpp"

//----------------------------------------------------- Constructor

Warper::Warper()
: anchorSize(20)
, anchorSizeHalf(anchorSize * 0.5)
{
    dstPoints[0].xSyncFrags = { true, false, false, true };
    dstPoints[0].ySyncFrags = { true, true, false, false };
    
    dstPoints[1].xSyncFrags = { false, true, true, false };
    dstPoints[1].ySyncFrags = { true, true, false, false };
    
    dstPoints[2].xSyncFrags = { false, true, true, false };
    dstPoints[2].ySyncFrags = { false, false, true, true };
    
    dstPoints[3].xSyncFrags = { true, false, false, true };
    dstPoints[3].ySyncFrags = { false, false, true, true };
}

//----------------------------------------------------- Local Functions

void Warper::drawQuadOutline()
{
    for(int i=0; i<4; i++)
    {
        int j = (i+1) % 4;
        ofDrawLine(dstPoints[i].x + position.x,
                   dstPoints[i].y + position.y,
                   dstPoints[j].x + position.x,
                   dstPoints[j].y + position.y);
    }
}

void Warper::drawCorners()
{
    for(int i=0; i<4; i++)
    {
        auto& point = dstPoints[i];
        drawCornerAt(point);
    }
}

void Warper::drawHighlightedCorner()
{
    for (const auto& p : dstPoints)
    {
        if (p.highlighted)
        {
            drawCornerAt(p);
        }
    }
}

void Warper::drawSelectedCorner()
{
    for (const auto& p : dstPoints)
    {
        if (p.selected)
        {
            drawCornerAt(p);
        }
    }
}

void Warper::drawGrabbedCorner()
{
    for (const auto& p : dstPoints)
    {
        if (p.grabbed)
        {
            drawCornerAt(p, 1.8);
        }
    }
}

void Warper::drawCornerAt(const glm::vec2 & point, float scale)
{
    ofDrawCircle(point.x + position.x, point.y + position.y, anchorSizeHalf * scale);
}


void Warper::selectPoint(WarperPoint &point, bool onlyThis, bool toggle)
{
    if (onlyThis)
    {
        for (auto& p : dstPoints) p.selected = false;
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
    for (auto& p : dstPoints)
    {
        p.anchor = p;
        p.offset = p - controlPoint;
        p.grabbed = false;
    }
    point.grabbed = true;
}

void Warper::switchSelect(bool prev)
{
    auto it = find_if(dstPoints.begin (), dstPoints.end(), [](WarperPoint& p) { return p.selected; });
    if (it == dstPoints.end())
    {
        dstPoints[0].selected = true;
        return;
    }
    
    for (auto& p : dstPoints) p.selected = false;
    if (prev)
    {
        if (it == dstPoints.begin())
            it = dstPoints.end() - 1;
        else
            it--;
        it->selected = true;
    }
    else
    {
        it++;
        if (it == dstPoints.end()) it = dstPoints.begin();
        it->selected = true;
    }
}

void Warper::movePoint(const glm::vec2& target, bool invertMode)
{
    if (invertMode)
    {
        auto grabbed = find_if(dstPoints.begin(), dstPoints.end(), [](WarperPoint& p) { return p.grabbed; });
        if (grabbed == dstPoints.end()) return;
        
        size_t grabbedIndex = std::distance(dstPoints.begin(), grabbed);
        for (int i = 0; i < 4; ++i)
        {
            auto& p = dstPoints[i];
            if (!p.selected) continue;
            
            auto mv = target + p.offset - p.anchor;
            
            float xFrag = p.xSyncFrags[grabbedIndex] ? 1 : -1;
            float yFrag = p.ySyncFrags[grabbedIndex] ? 1 : -1;
            float x = p.anchor.x + (mv.x * xFrag);
            float y = p.anchor.y + (mv.y * yFrag);
            p.set(x, y);
        }
    }
    else
    {
        for (auto& p : dstPoints)
        {
            if (p.selected)
            {
                p.set(target + p.offset);
            }
        }
    }
    ofNotifyEvent(updatedE, *this);
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
    
    auto highlighted = find_if(dstPoints.begin(), dstPoints.end(), [](WarperPoint& p) { return p.highlighted; });
    if (highlighted != dstPoints.end())
    {
        long index = std::distance(dstPoints.begin(), highlighted);
        ofSetColor(ofColor::white);
        stringstream ss;
        ss << index << endl;
        ss << "(" << highlighted->x << ", " << highlighted->y << ")";
        ofDrawBitmapString(ss.str(), ofGetMouseX() + 20, ofGetMouseY() - 20);
    }
    
    ofPopStyle();
}

//----------------------------------------------------- Corners

void Warper::setSourceRect(ofRectangle& r)
{
    srcPoints[0].set(r.x, r.y);
    srcPoints[1].set(r.x + r.width, r.y);
    srcPoints[2].set(r.x + r.width, r.y + r.height);
    srcPoints[3].set(r.x, r.y + r.height);
    ofNotifyEvent(updatedE, *this);
}

void Warper::setTargetRect(ofRectangle& r)
{
    dstPoints[0].set(r.x, r.y);
    dstPoints[1].set(r.x + r.width, r.y);
    dstPoints[2].set(r.x + r.width, r.y + r.height);
    dstPoints[3].set(r.x, r.y + r.height);
    ofNotifyEvent(updatedE, *this);
}

void Warper::reset()
{
    dstPoints[0].set(srcPoints[0]);
    dstPoints[1].set(srcPoints[1]);
    dstPoints[2].set(srcPoints[2]);
    dstPoints[3].set(srcPoints[3]);
    ofNotifyEvent(updatedE, *this);
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
        src_mat.at<float>(i, 0) = srcPoints[i].x;
        src_mat.at<float>(i, 1) = srcPoints[i].y;
        dst_mat.at<float>(i, 0) = dstPoints[i].x;
        dst_mat.at<float>(i, 1) = dstPoints[i].y;
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
    mousePoint -= position;
    
    for (auto& p : dstPoints)
    {
        if(glm::distance(mousePoint, p) <= anchorSizeHalf)
        {
            p.highlighted = true;
            return;
        }
    }
    
    for (auto& p : dstPoints) p.highlighted = false;
}

void Warper::mousePressed(int x, int y, int button)
{
    glm::vec2 mousePoint(x, y);
    mousePoint -= position;
    
    if (button == OF_MOUSE_BUTTON_LEFT)
    {
        if (!Common::isShiftKeyPressed())
        {
            if (count_if(dstPoints.begin(), dstPoints.end(), [](WarperPoint& p) { return p.selected; }) == 1)
            {
                for (auto& p : dstPoints) p.selected = false;
            }
        }
        
        for (auto& p : dstPoints)
        {
            if(glm::distance(mousePoint, p) <= anchorSizeHalf)
            {
                selectPoint(p, false);
                grabPoint(p, mousePoint);
                return;
            }
        }
        
        for (auto& p : dstPoints)
        {
            p.selected = false;
            p.grabbed = false;
        }
    }
    else if (button == OF_MOUSE_BUTTON_MIDDLE)
    {
        for (auto& p : dstPoints)
        {
            grabPoint(p, mousePoint);
            p.grabbed = false;
        }
    }
}

void Warper::mouseDragged(int x, int y, int button)
{
    glm::vec2 mousePoint(x, y);
    mousePoint -= position;
    
    if (button == OF_MOUSE_BUTTON_LEFT)
    {
        if (find_if(dstPoints.begin(), dstPoints.end(), [](WarperPoint& p) { return p.selected; }) == dstPoints.end()) return;
        movePoint(mousePoint, Common::isCtrlKeyPressed());
    }
    else if (button == OF_MOUSE_BUTTON_MIDDLE)
    {
        for (auto& p : dstPoints)
        {
            auto moveAmount = mousePoint - p.anchor;
            p.set(moveAmount + p.anchor + p.offset);
        }
        ofNotifyEvent(updatedE, *this);
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
            selectPoint(dstPoints[0], false);
            selectPoint(dstPoints[1], false);
            selectPoint(dstPoints[2], false);
            selectPoint(dstPoints[3], false);
            return;
    }
    
    if (find_if(dstPoints.begin(), dstPoints.end(), [](WarperPoint& p) { return p.selected; }) == dstPoints.end()) return;
    
    
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
        const auto it = find_if(dstPoints.begin(), dstPoints.end(), [](WarperPoint& p) { return p.selected; });
        if (it != dstPoints.end())
        {
            grabPoint(*it, *it);
            auto target = (*it) + moveAmount;
            movePoint(target, true);
        }
    }
    else
    {
        for (auto& p : dstPoints)
        {
            if (p.selected)
            {
                p += moveAmount;
            }
        }
        ofNotifyEvent(updatedE, *this);
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
