/**
 * @file FindRightArm.cpp
 * @brief Class which tries to find a right arm feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindRightArm.h"
#include "ImgShow.h"
#include "Icon.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

// FLTK MathGL plotting widget
#include <mgl2/fltk.h>

bool FindRightArm::DoWork(cv::Mat& pic) {
    cv::Mat found;
    cv::Mat roi = pic(cv::Rect(pic.cols * 0.5, pic.rows * 0.2, pic.cols * 0.5, pic.rows * 0.8));
    cv::matchTemplate(roi, m_Template, found, cv::TM_SQDIFF_NORMED);

    double min, max;
    cv::minMaxLoc(found, &min, &max);

    if(m_ShowInfo){
        // 3D Plots
        mglFLTK gr = mglFLTK(
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
            ,"Tamplate Matching right arm"
            
            // data passed to plotting callback
            ,&found
        );

        // add window icon
        std::shared_ptr<Fl_RGB_Image> icon = std::make_shared<Fl_RGB_Image>(icon_data, 128, 128, ImgShow::fl_imgtype::rgba, 0);
        Fl_Widget* widget = (Fl_Widget*)mgl_fltk_widget(gr.Self()); // get the underlaying fltk widget
        Fl_Double_Window* window = ((Fl_Double_Window*)widget->parent()); // get the underlaying fltk window
        window->icon(icon.get()); // now we can do all the wonderful FLTK stuff on the window

        ImgShow(roi, "Region right arm", ImgShow::rgb, false, true);
    }

    if(min < 0.103)
        return true;
    return false;
}