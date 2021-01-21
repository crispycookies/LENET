/**
 * @file FindFigure.cpp
 * @brief Class which finds a lego figure on the given picture, cuts it out and rotates it horizantally.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include <opencv2/imgproc.hpp>

#include "FindFigure.h"

#include "ImgShow.h"

bool FindFigure::DoWork(cv::Mat& pic){

    
    // divide original image with bg for brightness correction
    cv::Mat tmp1, tmp2;
    pic.convertTo(tmp1, CV_32FC3); 
    m_Background.convertTo(tmp2, CV_32FC3); 
    cv::Mat brightness_corrected = (tmp1 / (tmp2+1));
    brightness_corrected.convertTo(brightness_corrected, CV_8UC3, 255);

    // crop image / create roi (center of image)
    cv::Mat roi = brightness_corrected(cv::Rect(30, 23, brightness_corrected.cols - 30 - 30, brightness_corrected.rows - 23 - 23));

    // load the image and perform pyramid mean shift filtering
    // to aid the thresholding step
    cv::Mat shifted;
    cv::pyrMeanShiftFiltering(roi, shifted, 20, 40);

    // convert to greyscale
    cv::Mat grey;
    cv::cvtColor(shifted, grey, cv::COLOR_BGR2GRAY);

    // Erode (create mask to get rid of local shadows)
    cv::Mat erode, erode_mask;
    cv::erode(grey, erode, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
    cv::threshold(erode, erode_mask, 185, 255, cv::THRESH_BINARY_INV);

    // apply unsharp masking to reduce local shadows even more
    cv::Mat gaussfltr, mask, unsharp_masked;
    cv::GaussianBlur(grey, gaussfltr, cv::Size(5, 5), 1);
    cv::subtract(grey, gaussfltr, mask);
    cv::addWeighted(grey, 1, mask, 2, 0, unsharp_masked);

    // then apply thresholding to unsharp masked image
    cv::Mat thresh;
    cv::threshold(unsharp_masked, thresh, 240, 255, cv::THRESH_BINARY_INV);
    
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
    cv::Mat M, rotated;

    // get the rotation matrix
    M = getRotationMatrix2D(rot_rect.center, rot_rect.angle, 1.0);

    // perform the affine transformation
    cv::warpAffine(roi, rotated, M, roi.size(), cv::INTER_CUBIC);

    // crop the resulting image
    cv::getRectSubPix(rotated, rot_rect.size, rot_rect.center, pic);

    return true;
}