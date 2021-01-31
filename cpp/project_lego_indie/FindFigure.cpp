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

cv::Mat drawLineP(const std::vector<cv::Vec4i>& lines, const cv::Mat& pic){
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

std::vector<cv::Vec4i> analyzeLines(const cv::Mat & pic){
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

bool FindFigure::DoWork(cv::Mat& pic){

    // divide original image with bg for brightness correction
    cv::Mat tmp1, tmp2;
    pic.convertTo(tmp1, CV_32FC3); 
    m_Background.convertTo(tmp2, CV_32FC3); 
    cv::Mat brightness_corrected = (tmp1) / (tmp2);
    brightness_corrected.convertTo(brightness_corrected, CV_8UC3, 255);

    // crop image / create roi (center of image)
    cv::Mat roi = brightness_corrected(cv::Rect(m_crop_x, m_crop_y, brightness_corrected.cols - 2 * m_crop_x, brightness_corrected.rows - 2 * m_crop_y));
    roi = roi.clone(); // really get rid of of parts outside roi

    // load the image and perform pyramid mean shift filtering
    // to aid the thresholding step
    cv::Mat shifted;
    cv::pyrMeanShiftFiltering(roi, shifted, 25, 45);

    // convert to greyscale
    cv::Mat grey;
    cv::cvtColor(shifted, grey, cv::COLOR_BGR2GRAY);

    // Erode to get sure BG (create mask to get rid of local shadows)
    cv::Mat erode, erode_mask;
    cv::erode(grey, erode, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
    cv::threshold(erode, erode_mask, 180, 255, cv::THRESH_BINARY_INV);

    // then apply thresholding to unsharp masked image
    cv::Mat thresh;
    cv::threshold(grey, thresh, 230, 255, cv::THRESH_BINARY_INV);

    // combine eroding mask + threshhold
    cv::bitwise_and(thresh, erode_mask, thresh);

    // ----- find contours -----
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

    // invalid findings (yeah this part could be done more )
    if(rot_rcts.size() > 1 || rot_rcts.size() < 1)
        return false;

    cv::RotatedRect rot_rect = rot_rcts[0];

    // rotate found rectangle
    cv::Mat M, roiPadded, rotated;

    // get the rotation matrix
    auto center = rot_rect.center;
    center.x += roi.cols;
    center.y += roi.rows;
    M = getRotationMatrix2D(center, rot_rect.angle, 1.0);
    cv::copyMakeBorder(roi, roiPadded, roi.rows, roi.rows, roi.cols, roi.cols, cv::BORDER_CONSTANT, cv::Scalar(255,255,255));

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
    cv::Point pt1, pt2;
    pt1.x = lines[0][0]; pt1.y = lines[0][1];
    pt2.x = lines[0][2]; pt2.y = lines[0][3];
    size_t len = cv::norm(pt1 - pt2);
    if(len < 40 &&
       pt1.y < pic.rows - pic.rows * 0.85 &&
       pt2.y < pic.rows - pic.rows * 0.85) cv::flip(pic, pic, 0);

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


    cv::Mat pic_HSV, thresh_hands_head, thresh_body_hat_arms, thresh_feet, thresh_print_body;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::inRange(pic_HSV, cv::Scalar(2, 50, 235), cv::Scalar(30, 107, 255), thresh_hands_head);
    cv::inRange(pic_HSV, cv::Scalar(12, 107, 178), cv::Scalar(20, 178, 229), thresh_feet);
    cv::inRange(pic_HSV, cv::Scalar(25, 48, 155), cv::Scalar(33, 104, 214), thresh_print_body);
    cv::inRange(pic_HSV, cv::Scalar(6, 80, 63), cv::Scalar(19, 255, 153), thresh_body_hat_arms);


    // --- detect features
    cv::Mat roiLeftHand = pic_HSV(cv::Rect(0, pic.rows/ 2, pic.cols * 0.3, pic.rows/2));
    cv::Mat hasLHand;
    cv::inRange(roiLeftHand, cv::Scalar(11, 85, 240), cv::Scalar(29, 107, 255), hasLHand);
    if(cv::countNonZero(hasLHand))
        std::cout << "Has LHand" << std::endl;


    cv::Mat roiRightHand = pic_HSV(cv::Rect(pic.cols - pic.cols * 0.3, pic.rows/ 2, pic.cols * 0.3, pic.rows/2));
    cv::Mat hasRHand;
    cv::inRange(roiRightHand, cv::Scalar(11, 85, 240), cv::Scalar(29, 107, 255), hasRHand);
    if(cv::countNonZero(hasRHand))
        std::cout << "Has RHand" << std::endl;

    cv::Mat roiHead = pic_HSV(cv::Rect(0, 0, pic.cols, pic.rows * 0.3));
    cv::Mat hasHead;
    cv::inRange(roiHead, cv::Scalar(11, 85, 240), cv::Scalar(29, 107, 255), hasHead);
    if(cv::countNonZero(hasHead))
        std::cout << "Has Head" << std::endl;


    //cv::Mat hasFace;
    //cv::inRange(roiFace, cv::Scalar(9, 155, 109), cv::Scalar(15, 214, 130), hasFace);
    //if(cv::countNonZero(hasFace))
    //    std::cout << "Has Face" << std::endl;

    cv::Mat roiHat = pic_HSV(cv::Rect(0, 0, pic.cols, pic.rows * 0.2));
    cv::Mat hasHat;
    cv::inRange(roiHat, cv::Scalar(6, 80, 63), cv::Scalar(19, 255, 153), hasHat);
    if(cv::countNonZero(hasHat) > 500)
        std::cout << "Has Hat" << std::endl;

    cv::Mat roiLeftFoot = pic_HSV(cv::Rect(0, pic.rows * 0.8, pic.cols * 0.4, pic.rows-pic.rows * 0.8));
    cv::Mat hasLFoot;
    cv::inRange(roiLeftFoot, cv::Scalar(12, 107, 178), cv::Scalar(20, 178, 229), hasLFoot);
    if(cv::countNonZero(hasLFoot))
        std::cout << "Has LFoot" << std::endl;

    cv::Mat roiRightFoot = pic_HSV(cv::Rect(pic.cols * 0.6, pic.rows * 0.8, pic.cols - pic.cols * 0.6, pic.rows-pic.rows * 0.8));
    cv::Mat hasRFoot;
    cv::inRange(roiRightFoot, cv::Scalar(12, 107, 178), cv::Scalar(20, 178, 229), hasRFoot);
    if(cv::countNonZero(hasRFoot))
        std::cout << "Has RFoot" << std::endl;

    cv::Mat roiCenter = pic_HSV(cv::Rect(pic.cols * 0.38, pic.rows * 0.42, (pic.cols - pic.cols * 0.38) - (pic.cols - pic.cols * 0.62), (pic.rows - pic.rows * 0.42) - (pic.rows - pic.rows * 0.58)));
    cv::Mat hasBodyPrint;
    cv::inRange(roiCenter, cv::Scalar(25, 48, 155), cv::Scalar(33, 104, 214), hasBodyPrint);
    if(cv::countNonZero(hasBodyPrint))
        std::cout << "Has BodyPrint" << std::endl;

    /*cv::Mat roiLeftArm = pic(cv::Rect(0, pic.rows * 0.3, pic.cols * 0.15, (pic.rows - pic.rows * 0.3) - (pic.rows - pic.rows * 0.4)));
    cv::Mat hasLArm;
    cv::inRange(roiLeftArm, cv::Scalar(6, 80, 63), cv::Scalar(19, 255, 153), hasLArm);
    if(cv::countNonZero(hasLArm))
        std::cout << "Has LArm" << std::endl;

    cv::Mat roiRightArm = pic(cv::Rect(pic.cols * 0.85, pic.rows * 0.3, pic.cols - pic.cols * 0.85, (pic.rows - pic.rows * 0.3) - (pic.rows - pic.rows * 0.4)));
    cv::Mat hasRArm;
    cv::inRange(roiRightArm, cv::Scalar(6, 80, 63), cv::Scalar(19, 255, 153), hasRArm);
    if(cv::countNonZero(hasRArm))
        std::cout << "Has RArm" << std::endl;*/



    cv::imwrite("test.png", pic);
    ImgShow(pic, "result", ImgShow::rgb, false, true);

    return true;
}