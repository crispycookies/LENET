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
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
bool FindFeature::DoWork(cv::Mat& pic) {
    auto detector = cv::SIFT::create(2000);
    

    std::vector<cv::KeyPoint> keypoints_templ, keypoints_pic;
    cv::Mat found_templ, found_pic;
    detector->detectAndCompute(m_Feature, cv::noArray(), keypoints_templ, found_templ);
    detector->detectAndCompute(pic, cv::noArray(), keypoints_pic, found_pic);

    // match features
    cv::Ptr<cv::BFMatcher> matcher = cv::BFMatcher::create(cv::NORM_L2SQR, true);

    std::vector<cv::DMatch> matches;
    matcher->match(found_templ, found_pic, matches);
    
   /* std::vector<cv::DMatch> good_matches;
    const float ratio_thresh = 0.75f;
    for (size_t i = 0; i < matches.size(); i++) {
        if (matches[i].distance < ratio_thresh) {
            good_matches.push_back(matches[i]);
        }
    }*/

    std::vector<cv::Point2f> pts_pic;
    std::vector<cv::Point2f> pts_tmpl;  
    for( size_t i = 0; i < matches.size(); i++ ){
        //-- Get the keypoints from the good matches
        pts_pic.push_back( keypoints_pic[ matches[i].trainIdx ].pt );
        pts_tmpl.push_back( keypoints_templ[ matches[i].queryIdx ].pt );
    }
    
    cv::Mat M = cv::findHomography(pts_pic, pts_tmpl, cv::RANSAC, 1.0);
    cv::Mat warped_perspective;

    // XOpenIM() failed
    // terminate called after throwing an instance of 'cv::Exception'
    // what():  OpenCV(4.5.1) /home/giri/workspace/repoSystem/repos/packages/005_opencv/opencv-4.5.1/modules/calib3d/src/fundam.cpp:378: error: (-5:Bad argument) The input arrays should be 2D or 3D point sets in function 'findHomography'



    //res = cv2.warpPerspective(homography,M,(img.shape[0], img.shape[1]),0)

    cv::Mat result;
    cv::warpPerspective(pic, result, M, pic.size());
    
    pic = result;
    
   // cv::Mat fin;
   // drawMatches(m_Feature, keypoints_templ, pic, keypoints_pic, matches, fin);
   // pic = fin;

    if(matches.size())
        return true;

    return false;
}