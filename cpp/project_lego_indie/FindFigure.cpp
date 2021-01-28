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
#include <iostream>

#include "ImgShow.h"

cv::Mat removeShadows(const cv::Mat& pic){
    std::vector<cv::Mat> channels;
    cv::split(pic, channels);

    cv::Mat res_norm;
    std::vector<cv::Mat> result_norm_planes;

    for(auto &  plane : channels){
    cv::Mat dilated_img, bg_img, norm_img, diff_img;
        cv::dilate(plane, dilated_img, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7)));
        cv::medianBlur(dilated_img, bg_img, 21);
        cv::absdiff(plane, bg_img, diff_img);
        diff_img = 255 - diff_img;
        cv::normalize(diff_img, norm_img, 130, 255, cv::NORM_MINMAX, CV_8UC1);
        //result_planes.push_back(diff_img)
        result_norm_planes.emplace_back(norm_img);
    }

    cv::merge(result_norm_planes, res_norm);


    return res_norm;
}

bool FindFigure::DoWork(cv::Mat& pic){
    
    // divide original image with bg for brightness correction
    cv::Mat tmp1, tmp2;
    pic.convertTo(tmp1, CV_32FC3); 
    m_Background.convertTo(tmp2, CV_32FC3); 
    cv::Mat brightness_corrected = (tmp1) / (tmp2);
    brightness_corrected.convertTo(brightness_corrected, CV_8UC3, 255);

    cv::Mat without_shadows = removeShadows(brightness_corrected);
    

    // crop image / create roi (center of image)
    cv::Mat roi_without_shadows = without_shadows(cv::Rect(m_crop_x, m_crop_y, brightness_corrected.cols - 2 * m_crop_x, brightness_corrected.rows - 2 * m_crop_y));
    cv::Mat roi_brightness_corrected = brightness_corrected(cv::Rect(m_crop_x, m_crop_y, brightness_corrected.cols - 2 * m_crop_x, brightness_corrected.rows - 2 * m_crop_y));
    ImgShow a(roi_without_shadows, "brightness_corrected", ImgShow::rgb, false);
    // load the image and perform pyramid mean shift filtering
    // to aid the thresholding step
    cv::Mat shifted;
    cv::pyrMeanShiftFiltering(roi_without_shadows, shifted, 5, 15);
    ImgShow b(shifted, "shifted", ImgShow::rgb, false);   
    // convert to greyscale
    cv::Mat grey;
    cv::cvtColor(shifted, grey, cv::COLOR_BGR2GRAY);

    // Erode to get sure BG (create mask to get rid of local shadows)
    //cv::Mat erode, erode_mask;
    //cv::erode(grey, erode, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 20)));
    //cv::threshold(erode, erode_mask, 180, 255, cv::THRESH_BINARY_INV);

   // ImgShow c(erode_mask, "erode_mask", ImgShow::grey, false);

    // apply unsharp masking to reduce local shadows even more
    cv::Mat gaussfltr, mask, unsharp_masked;
    //cv::GaussianBlur(grey, gaussfltr, cv::Size(5, 5), 1);
    //cv::subtract(grey, gaussfltr, mask);
    //cv::addWeighted(grey, 1, mask, 2, 0, unsharp_masked);

    //ImgShow(unsharp_masked, "unsharp_masked", ImgShow::grey, false);

    // then apply thresholding to unsharp masked image
    cv::Mat thresh;
    cv::threshold(grey, thresh, 230, 255, cv::THRESH_BINARY_INV);

    // combine eroding mask + threshhold
    //cv::bitwise_and(thresh, erode_mask, thresh);
    ImgShow d(thresh, "thresh", ImgShow::grey, false, true);


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
    cv::warpAffine(roi_brightness_corrected, rotated, M, roi_brightness_corrected.size(), cv::INTER_CUBIC);

    // crop the resulting image
    cv::getRectSubPix(rotated, rot_rect.size, rot_rect.center, pic);

    // if the picture is now aligned horizontally rotate it by 90 degrees
    if(pic.cols > pic.rows)
        cv::rotate(pic, pic, cv::ROTATE_90_CLOCKWISE);

    // now check center of mass, if the figure head points to bottom flip picture 
    cv::Mat binCutPic;
    cv::cvtColor(pic, binCutPic, cv::COLOR_BGR2GRAY);
    // apply unsharp masking to reduce local shadows
    cv::GaussianBlur(binCutPic, gaussfltr, cv::Size(5, 5), 1);
    cv::subtract(binCutPic, gaussfltr, mask);
    cv::addWeighted(binCutPic, 1, mask, 2, 0, binCutPic);
    cv::bitwise_not(binCutPic, binCutPic, binCutPic == 0);
    cv::threshold(binCutPic, binCutPic, 200, 255, cv::THRESH_BINARY_INV);
    cv::Moments mu = cv::moments(binCutPic, true);
    if((mu.m01 / mu.m00) < pic.rows / 2)
        cv::flip(pic, pic, 0);
    
    cv::resize(pic, pic, cv::Size(m_scale_x, m_scale_y));
    ImgShow z(pic, "pic", ImgShow::rgb, false, true);
    return true;
}