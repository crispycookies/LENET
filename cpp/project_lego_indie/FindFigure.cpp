/**
 * @file FindFigure.cpp
 * @brief Class which finds a lego figure on the given picture, cuts it out and rotates it horizantally.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "FindFigure.h"
#include <iostream>
#include <cmath>

#include "ImgShow.h"

cv::Mat FindFigure::drawLineP(const std::vector<cv::Vec4i>& lines, const cv::Mat& pic){
    cv::Mat cpy;
    pic.copyTo(cpy);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        cv::Point pt1, pt2;
        pt1.x = lines[i][0];
        pt1.y = lines[i][1];
        pt2.x = lines[i][2];
        pt2.y = lines[i][3];

        cv::line(cpy, pt1, pt2, cv::Scalar(0,0,255), 3, cv::LINE_AA);
    }
    return cpy;
}

std::vector<cv::Vec4i> FindFigure::analyzeLines(const cv::Mat & pic){
    // find line in feet or body
    cv::Mat edges, binEdges, threshEdges;
    cv::cvtColor(pic, binEdges, cv::COLOR_BGR2GRAY);
    std::vector<cv::Vec4i> lines; 
    cv::Sobel(binEdges, edges, CV_8U, 0, 1, 3, 1.0, 1);
    cv::threshold(edges, threshEdges, 130, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::HoughLinesP(threshEdges, lines, 1, CV_PI/180, 10, 10, 20);

    std::vector<cv::Vec4i> ret;
    for(const auto& line : lines) {
        cv::Point pt1, pt2;
        pt1.x = line[0]; pt1.y = line[1];
        pt2.x = line[2]; pt2.y = line[3];

        size_t len = cv::norm(pt1 - pt2);
        double angle = std::atan2(pt1.y - pt2.y, pt1.x - pt2.x);
        if(angle < 0) angle += 2 * CV_PI;
        if(len > 25 && len < 65){
            if(angle > CV_PI - 0.6 && angle < CV_PI + 0.6)
                ret.push_back(line);
        }

    }
    std::sort(ret.begin(), ret.end(), [](const cv::Vec4i& l, const cv::Vec4i& r) -> bool {
            return (l[1] < r[1]); 
    });
    return ret;
}


cv::Mat FindFigure::correct_brightness(const cv::Mat& pic){
    // divide original image with bg for brightness correction
    cv::Mat tmp1, tmp2;
    pic.convertTo(tmp1, CV_32FC3); 
    m_Background.convertTo(tmp2, CV_32FC3); 
    cv::Mat brightness_corrected = (tmp1) / (tmp2);
    brightness_corrected.convertTo(brightness_corrected, CV_8UC3, 255);
    return brightness_corrected;
}

cv::Mat FindFigure::crop(const cv::Mat & brightness_corrected){
    cv::Mat roi = brightness_corrected(cv::Rect(m_crop_x, m_crop_y, brightness_corrected.cols - 2 * m_crop_x, brightness_corrected.rows - 2 * m_crop_y));
    roi = roi.clone(); // really get rid of of parts outside roi
    return roi;
}

cv::Mat FindFigure::shift(const cv::Mat & roi){
    cv::Mat shifted;
    cv::pyrMeanShiftFiltering(roi, shifted, 25, 45);
    return shifted;
}

cv::Mat FindFigure::make_grey(const cv::Mat & shifted){
    cv::Mat grey;
    cv::cvtColor(shifted, grey, cv::COLOR_BGR2GRAY);
    return grey;
}

cv::Mat FindFigure::make_erode(const cv::Mat & grey){
    cv::Mat erode, erode_mask;
    cv::erode(grey, erode, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
    cv::threshold(erode, erode_mask, 180, 255, cv::THRESH_BINARY_INV);
    return erode_mask;
}

cv::Mat FindFigure::make_thresh(const cv::Mat & grey, const cv::Mat & erode_mask){
    cv::Mat thresh;
    cv::threshold(grey, thresh, 230, 255, cv::THRESH_BINARY_INV);

    // combine eroding mask + threshhold
    cv::bitwise_and(thresh, erode_mask, thresh);
    return thresh;
}

std::tuple<std::vector<std::vector<cv::Point>>, std::vector<cv::Vec4i>, std::vector<cv::RotatedRect>> FindFigure::find_contours_ff(const cv::Mat & thresh){
    std::vector<std::vector<cv::Point>> cnt;
    std::vector<cv::Vec4i> hier;
    cv::findContours(thresh, cnt, hier, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE); // find contours

    std::vector<cv::RotatedRect> rot_rcts; // find min area rect
    for( size_t i = 0; i < cnt.size(); i++ )
    {
        // skip if cnt is no parent (->3), 
        // only use uppermost hierachy, skips smaller areas which occur within a bigger area, we are not interested in these
        if(hier[i][3] != -1) continue;
        
        cv::Rect rct = cv::boundingRect(cnt[i]);

        // only use rectangles with at least 17000 pixels and also filter way too big ones (caused by shadows etc.)
        if(rct.area() > 17000 && rct.width < thresh.cols * 0.90 && rct.height < thresh.rows * 0.90)
            rot_rcts.push_back(cv::minAreaRect(cnt[i]));
    }
    return std::make_tuple(cnt,hier,rot_rcts);
}

std::pair<cv::Point, cv::Point> FindFigure::check_uppermost(cv::Mat& pic, const std::vector<cv::Vec4i> & lines){
    cv::Point pt1, pt2;
    pt1.x = lines[0][0]; pt1.y = lines[0][1];
    pt2.x = lines[0][2]; pt2.y = lines[0][3];
    size_t len = cv::norm(pt1 - pt2);
    if(len < 40 &&
       pt1.y < pic.rows - pic.rows * 0.85 &&
       pt2.y < pic.rows - pic.rows * 0.85) cv::flip(pic, pic, 0);

    return std::make_pair(pt1,pt2);
}

std::tuple<cv::Point2f, cv::Mat, cv::Mat, cv::Mat> FindFigure::get_rotation_matrix(const cv::RotatedRect & rot_rect, cv::Mat & roi){
    // rotate found rectangle
    cv::Mat M, roiPadded, rotated;

    // get the rotation matrix
    auto center = rot_rect.center;
    center.x += roi.cols;
    center.y += roi.rows;
    M = getRotationMatrix2D(center, rot_rect.angle, 1.0);
    cv::copyMakeBorder(roi, roiPadded, roi.rows, roi.rows, roi.cols, roi.cols, cv::BORDER_CONSTANT, cv::Scalar(255,255,255));

    return std::make_tuple(center, roiPadded, M, rotated);
}


bool FindFigure::DoWork(cv::Mat& pic){

    // divide original image with bg for brightness correction
    auto brightness_corrected = correct_brightness(pic);

    // crop image / create roi (center of image)
    auto roi = crop(brightness_corrected);

    // load the image and perform pyramid mean shift filtering
    // to aid the thresholding step
    auto shifted = shift(roi);

    // convert to greyscale
    auto grey = make_grey(shifted);

    // Erode to get sure BG (create mask to get rid of local shadows)
    auto erode_mask = make_erode(grey);
    
    // then apply thresholding to unsharp masked image
    auto thresh = make_thresh(grey, erode_mask);

    // ----- find contours -----
    auto [cnt, hier, rot_rcts] = find_contours_ff(thresh);

    // invalid findings (yeah this part could be done more extensively)
    if(rot_rcts.size() > 1 || rot_rcts.size() < 1)
        return false;

    cv::RotatedRect rot_rect = rot_rcts[0];

    // rotate found rectangle
    // get the rotation matrix

    auto [center, roiPadded, M, rotated] = get_rotation_matrix(rot_rect, roi);
    
    // perform the affine transformation
    cv::warpAffine(roiPadded, rotated, M, roiPadded.size(), cv::INTER_CUBIC);

    // crop the resulting image
    cv::getRectSubPix(rotated, rot_rect.size, center, pic);

    // if the picture is now aligned horizontally rotate it by 90 degrees
    if(pic.cols > pic.rows)
        cv::rotate(pic, pic, cv::ROTATE_90_CLOCKWISE);

    // now check center of mass, if the figure head points to bottom flip picture 
    cv::Mat binCutPic, binCutPicGaussFlt, binCutPicMask;
    cv::cvtColor(pic, binCutPic, cv::COLOR_BGR2GRAY);
    // apply unsharp masking to reduce local shadows
    cv::GaussianBlur(binCutPic, binCutPicGaussFlt, cv::Size(5, 5), 1);
    cv::subtract(binCutPic, binCutPicGaussFlt, binCutPicMask);
    cv::addWeighted(binCutPic, 1, binCutPicMask, 2, 0, binCutPic);
    cv::bitwise_not(binCutPic, binCutPic, binCutPic == 0);
    cv::threshold(binCutPic, binCutPic, 200, 255, cv::THRESH_BINARY_INV);
    cv::Moments mu = cv::moments(binCutPic, true);
    if((mu.m01 / mu.m00) < pic.rows / 2)
        cv::flip(pic, pic, 0);
    
    std::vector<cv::Vec4i> lines = analyzeLines(pic);

    // check if uppermost line is within a certain threshhold to the image border and smaller than 30px
    // if yes the figure misses one foot and is upsidedown, so flip
    
    auto [pt1, pt2] = check_uppermost(pic, lines);
    
    // adjust figure angle
    lines = analyzeLines(pic);
    pt1.x = lines[lines.size()-1][0]; pt1.y = lines[lines.size()-1][1];
    pt2.x = lines[lines.size()-1][2]; pt2.y = lines[lines.size()-1][3];
    double angle = CV_PI - std::atan2(pt1.y - pt2.y, pt1.x - pt2.x);
    angle = angle * -180 / CV_PI;
    
    // get the rotation matrix
    cv::Mat picPadded, picRot;
    picRot = getRotationMatrix2D(cv::Point2f(pic.cols/2, pic.rows/2), angle, 1.0);
    cv::warpAffine(pic, pic, picRot, pic.size(), 1, cv::BORDER_CONSTANT, cv::Scalar(255,255,255));
    cv::resize(pic, pic, cv::Size(m_scale_x, m_scale_y));

    return true;
}