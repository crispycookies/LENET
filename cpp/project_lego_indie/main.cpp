/**
 * @file main.cpp
 * @brief Program entry point.
 * @author Daniel Giritzer
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

// FLTK
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>

// std library
#include <memory>
#include <iostream>
#include <filesystem>

// opencv
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// boost
#include <boost/program_options.hpp>

#include "Icon.h" // icon for window manager (embedded into executable for maximum portability)

/**
 * Helper class to display opencv matrix containing a image data.
 */
class ImgShow {
public:

    enum fl_imgtype{
        grey =  1,
        greya = 2,
        rgb =   3,
        rgba =  4
    };

    /**
     * @param img OpenCV matrix containing image data
     * @param title Title to be shown in the window header
     * @param type Image type (use enum from this class namespace)
     * @param rescale if true min and max values of a given grayscale picture are rescaled between 0-255, defaults to false
     */
    ImgShow(const cv::Mat& img, const std::string& title, const ImgShow::fl_imgtype& type, bool rescale = false){

        // Convert image data from BRG (opencv default) to RGB
        if(type == fl_imgtype::rgb || type == fl_imgtype::rgba){
            cv::cvtColor(img, m_ImgRGB, cv::COLOR_BGR2RGB);
        }
        else {
            if(rescale){
                m_ImgRGB = img;
                cv::normalize(m_ImgRGB, m_ImgRGB, 0, 255, cv::NORM_MINMAX, CV_8U);
            }
            else
            {
                if(img.type() != CV_8U)
                    img.convertTo(m_ImgRGB, CV_8U);
                else
                    m_ImgRGB = img;
            }
        }

        m_Win = std::make_shared<Fl_Window>(m_ImgRGB.cols,m_ImgRGB.rows, title.c_str());
        m_Scr = std::make_shared<Fl_Scroll>(0,0,m_Win->w(), m_Win->h());
        m_Pic = std::make_shared<Fl_RGB_Image>(m_ImgRGB.data, m_ImgRGB.cols, m_ImgRGB.rows, type, 0);
        m_Box = std::make_shared<Fl_Box>(0,0, m_Pic->w(), m_Pic->h());
        m_Box->image(m_Pic.get());
        m_Icon = std::make_shared<Fl_RGB_Image>(icon_data, 128, 128, ImgShow::fl_imgtype::rgba, 0);
        m_Win->icon(m_Icon.get());
        m_Win->resizable(m_Box.get()); // some window managers may truncate big windows according their needs, so allow resizing here.
        m_Win->show();
    }
    virtual ~ImgShow() = default;

private:
    std::shared_ptr<Fl_Window> m_Win;
    std::shared_ptr<Fl_Scroll> m_Scr;
    std::shared_ptr<Fl_RGB_Image> m_Pic;
    std::shared_ptr<Fl_RGB_Image> m_Icon;
    std::shared_ptr<Fl_Box> m_Box;
    cv::Mat m_ImgRGB;
};

namespace po = boost::program_options;

int main(int argc, char** argv)
{

    // commandline options
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "Print help.")
    ("background", po::value<std::string>(), "Background image.");
    ("image", po::value<std::string>(), "Image to be used. (if not set or invalid a gui prompt will force you to select one)");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // print help message
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    std::filesystem::path bg_img_path = "./pic/Other/image_100.jpg";
    if(vm.count("background")){
        bg_img_path = vm["background"].as<std::string>();
    }

    if(!std::filesystem::exists(bg_img_path)){
        std::cerr << "Image does not exist: " << bg_img_path.string() << std::endl;
        return EXIT_FAILURE;
    }

    std::filesystem::path path = "";
    if(vm.count("image")){
        path = vm["image"].as<std::string>();
    }

    if(!std::filesystem::exists(path))
        path = fl_file_chooser("Choose image file...", "*.jpg", "./pic/All/image_0.jpg", 1);

    // read image
    cv::Mat img = cv::imread(path.string(), cv::IMREAD_COLOR);
    if(img.empty()){
        std::cerr << "Could not read the image: " << path.string() << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat bg_img = cv::imread(bg_img_path.string(), cv::IMREAD_COLOR);
    if(bg_img.empty()){
        std::cerr << "Could not read the image: " << bg_img_path.string() << std::endl;
        return EXIT_FAILURE;
    }

#ifdef _WIN32
	FreeConsole();
#endif

    std::vector<ImgShow> windows;

    // draw original image
    windows.emplace_back(img, "Original Image", ImgShow::fl_imgtype::rgb);

    // divide original image with bg
    cv::Mat tmp1, tmp2;
    img.convertTo(tmp1, CV_32FC3); 
    bg_img.convertTo(tmp2, CV_32FC3); 
    cv::Mat brightness_corrected = (tmp1 / (tmp2+1));
    brightness_corrected.convertTo(brightness_corrected, CV_8UC3, 255);
    windows.emplace_back(brightness_corrected, "Brightness adjusted", ImgShow::fl_imgtype::rgb);

    // find figure within roi (center of image)
    cv::Mat roi = brightness_corrected(cv::Rect(30, 23, brightness_corrected.cols - 30 - 30, brightness_corrected.rows - 23 - 23));
    windows.emplace_back(roi, "Brightness adjusted", ImgShow::fl_imgtype::rgb);

    // load the image and perform pyramid mean shift filtering
    // to aid the thresholding step
    cv::Mat shifted;
    cv::pyrMeanShiftFiltering(roi, shifted, 20, 40);
    windows.emplace_back(shifted, "Filtered Image", ImgShow::fl_imgtype::rgb);


    // convert to greyscale
    cv::Mat grey;
    cv::cvtColor(shifted, grey, cv::COLOR_BGR2GRAY);

    // Erode mask to get rid of local shadows
    cv::Mat erode, erode_mask;
    cv::erode(grey, erode, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
    cv::threshold(erode, erode_mask, 185, 255, cv::THRESH_BINARY_INV);

    // apply unsharp masking to reduce local shadows even more
    cv::Mat gaussfltr, mask, unsharp_masked;
    cv::GaussianBlur(grey, gaussfltr, cv::Size(5, 5), 1);
    cv::subtract(grey, gaussfltr, mask);
    cv::addWeighted(grey, 1, mask, 2, 0, unsharp_masked);

    // convert the mean shift image to grayscale, then apply Otsu's thresholding
    cv::Mat thresh;
    cv::threshold(unsharp_masked, thresh, 240, 255, cv::THRESH_BINARY_INV);
    cv::bitwise_and(thresh, erode_mask, thresh);
    windows.emplace_back(thresh, "Thresh", ImgShow::fl_imgtype::grey);

    // perform segmentation here
    // ---

    std::vector<std::vector<cv::Point>> cnt;
    std::vector<cv::Vec4i> hier;
    cv::findContours(thresh, cnt, hier, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE); // find contours
    // create bounding rectangles around contours
    std::vector<cv::Rect> rcts;
    std::vector<cv::RotatedRect> rot_rcts;
    for( size_t i = 0; i < cnt.size(); i++ )
    {
        // skip if cnt is no parent (->3), 
        // only use uppermost hierachy, skips smaller areas which occur within a bigger area
        if(hier[i][3] != -1) continue;
        cv::Rect rct = cv::boundingRect(cnt[i]);
        // only use rectangles with at least 20000 pixels and also filter way too big ones (caused by shadows etc.)
        if(rct.area() > 17000 && rct.width < thresh.cols * 0.90 && rct.height < thresh.rows * 0.90)
        {
            rcts.push_back(rct);
            rot_rcts.push_back(cv::minAreaRect(cnt[i]));
        }
    }

    std::cout << rcts.size() << std::endl << rot_rcts.size() << std::endl;
    for(auto const& curRct : rcts)
        cv::rectangle(roi, curRct, cv::Scalar(0, 255, 0), 2);

    for(auto const& curRct : rot_rcts) {
        cv::Point2f edgesOfRotRect[4];
        curRct.points(edgesOfRotRect);
        std::vector<cv::Point> vec(std::begin(edgesOfRotRect), std::end(edgesOfRotRect)); // convert to point
        cv::drawContours(roi, std::vector<std::vector<cv::Point>>{ vec }, -1, cv::Scalar(0, 0, 255), 2);
    }
    windows.emplace_back(roi, "Found", ImgShow::fl_imgtype::rgb);
    
    if(rcts.size()){
        // rotate found rectangle
        cv::Mat M, rotated, cropped;
        // get angle and size from the bounding box
        float angle = rot_rcts[0].angle;
        cv::Size rect_size = rot_rcts[0].size;
        if (rot_rcts[0].angle < -45.) {
            angle += 90.0;
            cv::swap(rect_size.width, rect_size.height);
        }
        // get the rotation matrix
        M = getRotationMatrix2D(rot_rcts[0].center, angle, 1.0);
        // perform the affine transformation
        cv::warpAffine(roi, rotated, M, roi.size(), cv::INTER_CUBIC);
        // crop the resulting image
        cv::getRectSubPix(rotated, rect_size, rot_rcts[0].center, cropped);

        windows.emplace_back(cropped, "Cropped", ImgShow::fl_imgtype::rgb);
    }


    // check leg

    // Result msgbox
/* std::shared_ptr<Fl_RGB_Image> icon = std::make_shared<Fl_RGB_Image>(icon_data, 128, 128, ImgShow::fl_imgtype::rgba, 0);
    fl_message_hotspot(true); // popup msg box near mousepointer
    ((Fl_Double_Window*)fl_message_icon()->parent())->icon(icon.get());
    fl_message_title("Result");
    fl_message("");*/

    return(Fl::run());
}