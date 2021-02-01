/**
 * @file FindFacePrint.cpp
 * @brief Class which tries to find a face print featur in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindFacePrint.h"
#include "ImgShow.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

// FLTK MathGL plotting widget
#include <mgl2/fltk.h>

bool FindFacePrint::DoWork(cv::Mat& pic) {
    cv::Mat found;
    cv::matchTemplate(pic, m_Template, found, cv::TM_SQDIFF_NORMED);

    double min, max;
    cv::minMaxLoc(found, &min, &max);

    // 3D Plots
    std::shared_ptr<mglFLTK> gr = std::make_shared<mglFLTK>(
        // plotting callback (needed for interactive plots)
        [](mglBase *p1, void *p2) -> int{
            mglGraph gr(p1);
            cv::Mat d = *(static_cast<cv::Mat*>(p2));
        
            double min, max;
            cv::minMaxLoc(d, &min, &max);

            mglData d1{d.rows, d.cols, (float*)d.data};
            gr.SubPlot(1,1,0);
            gr.Title("");
            gr.Box();
            gr.SetRanges(0, d1.nx, 0, d1.ny, min, max);
            gr.Axis("z");
            gr.Surf(d1, "BbcyrR"); // BbcyrR == jet color map
            //gr.Dens(cur.dat, "BbcyrR"); // for 2D only
            gr.Colorbar();

            return 0;
        }
        // window title
        ,"Interactive Plots"
        
        // data passed to plotting callback
        ,&found
        );
    ImgShow(found, "", ImgShow::rgb, false, false);

    if(min < 0.05)
        return true;
    return false;
}

    //cv::Mat hasFace;
    //cv::inRange(roiFace, cv::Scalar(9, 155, 109), cv::Scalar(15, 214, 130), hasFace);
    //if(cv::countNonZero(hasFace))
    //    std::cout << "Has Face" << std::endl;
