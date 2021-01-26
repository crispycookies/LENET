/**
 * @file main.cpp
 * @brief Program entry point.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */


// std library
#include <memory>
#include <iostream>
#include <filesystem>
#include <optional>

// opencv
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// boost
#include <boost/program_options.hpp>

#include "IPicWorker.h"
#include "FindFigure.h"
#include "FindFeature.h"
#include "ImgShow.h"
#include "Icon.h" // icon for window manager (embedded into executable for maximum portability)

namespace po = boost::program_options;

/**
 * Reads picture from file. exits program on error
 * @param f File to be read.
 * @param m Pirctre read mode.
 */
cv::Mat imreadChecked(const std::filesystem::path& f, cv::ImreadModes m){
    if(!std::filesystem::exists(f)){
        std::cerr << "Image does not exist: " << f.string() << std::endl;
        exit(EXIT_FAILURE);
    }
    cv::Mat img = cv::imread(f.string(), m);
    if(img.empty()){
        std::cerr << "Could not read the image: " << f.string() << std::endl;
        exit(EXIT_FAILURE);
    }
    return img;
}

std::optional<po::variables_map> parseCmdLine(int argc, char** argv){
    // commandline options
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "Print help.")
            ("background", po::value<std::string>(), "Background image. (defaults to ./pic/Other/image_100.jpg)")
            ("templdir", po::value<std::string>(), "Folder containing template files. (defaults to ./pic/templates)")
            ("image", po::value<std::string>(), "Image to be used. (if not set or invalid a gui prompt will force you to select one)");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return {};
    }
    return vm;
}

struct r_val {
    std::filesystem::path bg_img_path;
    std::filesystem::path path;
    std::filesystem::path templDir;
};

r_val getFromCmdLine(po::variables_map vm){
    std::filesystem::path bg_img_path = "./pic/Other/image_100.jpg";
    std::filesystem::path path = "";
    std::filesystem::path templDir = "./pic/templates";

    if(vm.count("background")){
        bg_img_path = vm["background"].as<std::string>();
    }
    if(vm.count("image")){
        path = vm["image"].as<std::string>();
    }
    if(!std::filesystem::exists(path)){
        path = fl_file_chooser("Choose image file...", "*.jpg", "./pic/All/image_0.jpg", 1);
    }
    if(vm.count("templdir")){
        templDir = vm["templdir"].as<std::string>();
    }

    return {bg_img_path, path, templDir};
}

int main(int argc, char** argv)
{

    // wm icon
    auto icon = std::make_shared<Fl_RGB_Image>(icon_data, 128, 128, ImgShow::fl_imgtype::rgba, 0);
    auto vm_b = parseCmdLine(argc, argv);
    if(vm_b == std::nullopt){
        return EXIT_SUCCESS;
    }
    auto vm = vm_b.value();
    auto paths = getFromCmdLine(vm);

    // read images
    auto img = imreadChecked(paths.path, cv::IMREAD_COLOR);
    auto bg_img = imreadChecked(paths.bg_img_path, cv::IMREAD_COLOR);
    auto templIndie = imreadChecked(paths.templDir.append("template.png"), cv::IMREAD_COLOR);
    auto templBody = imreadChecked(paths.templDir.remove_filename().append("mask_body.png"), cv::IMREAD_COLOR) & templIndie;
    auto templFace = imreadChecked(paths.templDir.remove_filename().append("mask_face.png"), cv::IMREAD_COLOR) & templIndie;
    auto templHat = imreadChecked(paths.templDir.remove_filename().append("mask_hat.png"), cv::IMREAD_COLOR) & templIndie;
    auto templHead = imreadChecked(paths.templDir.remove_filename().append("mask_head.png"), cv::IMREAD_COLOR) & templIndie;
    auto templLarm = imreadChecked(paths.templDir.remove_filename().append("mask_larm.png"), cv::IMREAD_COLOR) & templIndie;
    auto templRarm = imreadChecked(paths.templDir.remove_filename().append("mask_rarm.png"), cv::IMREAD_COLOR) & templIndie;
    auto templLhand = imreadChecked(paths.templDir.remove_filename().append("mask_lhand.png"), cv::IMREAD_COLOR) & templIndie;
    auto templRhand = imreadChecked(paths.templDir.remove_filename().append("mask_rhand.png"), cv::IMREAD_COLOR) & templIndie;
    auto templLleg = imreadChecked(paths.templDir.remove_filename().append("mask_lleg.png"), cv::IMREAD_COLOR) & templIndie;
    auto templRleg = imreadChecked(paths.templDir.remove_filename().append("mask_rleg.png"), cv::IMREAD_COLOR) & templIndie;

    {
        paths.templDir.remove_filename();
    }

#ifdef _WIN32
	FreeConsole();
#endif

    IPicWorker::SPtr ff = std::make_shared<FindFigure>(bg_img);
    IPicWorker::SPtr fLhand = std::make_shared<FindFeature>(templLleg, "Body");


    ff->DoWork(img);
    std::cout << fLhand->DoWork(img) << std::endl;

    ImgShow I(img, "Original Image", ImgShow::fl_imgtype::rgb);
    ImgShow B(templBody, "Original Image", ImgShow::fl_imgtype::rgb);



    // check leg

    // Result msgbox
/*
    fl_message_hotspot(true); // popup msg box near mousepointer
    ((Fl_Double_Window*)fl_message_icon()->parent())->icon(icon.get());
    fl_message_title("Result");
    fl_message("");*/

    return(Fl::run());
}