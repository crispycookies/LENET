/**
 * @file FindFeature.cpp
 * @brief Class which finds a feature on the given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindFeature.h"

#include <opencv2/features2d.hpp>
#include <iostream>
bool FindFeature::DoWork(cv::Mat& pic) {
    
    // analyze pics using ORB feature detector
    cv::Ptr<cv::ORB> orb = cv::ORB::create(100, 1.5, 10, 5);
    std::vector<cv::KeyPoint> keypoints_templ, keypoints_pic;
    cv::Mat found_templ, found_pic;
    orb->detectAndCompute(m_Feature, cv::noArray(), keypoints_templ, found_templ);
    orb->detectAndCompute(pic, cv::noArray(), keypoints_pic, found_pic);

    // match features
    cv::Ptr<cv::BFMatcher> matcher = cv::BFMatcher::create(cv::NORM_L2SQR, true);

    std::vector<cv::DMatch> matches;
    matcher->match(found_templ, found_pic, matches);

    cv::Mat fin;
    drawMatches(m_Feature, keypoints_templ, pic, keypoints_pic, matches, fin);
    pic = fin;

    if(matches.size())
        return true;

    return false;
}