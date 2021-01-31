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
#include "FindRightHand.h"
#include "FindLeftHand.h"
#include "FindRightFoot.h"
#include "FindLeftFoot.h"
#include "FindHead.h"
#include "FindHat.h"
#include "FindBodyPrint.h"
#include "FindFacePrint.h"
#include "FindLeftArm.h"
#include "FindRightArm.h"

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
    Fl::scheme("gleam");

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

    auto templFace = imreadChecked(paths.templDir.append("template_face.png"), cv::IMREAD_COLOR);
    auto templLarm = imreadChecked(paths.templDir.remove_filename().append("template_left_arm.png"), cv::IMREAD_COLOR);
    auto templRarm = imreadChecked(paths.templDir.remove_filename().append("template_right_arm.png"), cv::IMREAD_COLOR);

    {
        paths.templDir.remove_filename();
    }

#ifdef _WIN32
	FreeConsole();
#endif

    IPicWorker::SPtr cutter = std::make_shared<FindFigure>(bg_img);
    IPicWorker::SPtr headFinder = std::make_shared<FindHead>();
    IPicWorker::SPtr hatFinder = std::make_shared<FindHat>();
    IPicWorker::SPtr leftHandFinder = std::make_shared<FindLeftHand>();
    IPicWorker::SPtr rightHandFinder = std::make_shared<FindRightHand>();
    IPicWorker::SPtr rightFootFinder = std::make_shared<FindRightFoot>();
    IPicWorker::SPtr leftFootFinder = std::make_shared<FindLeftFoot>();
    IPicWorker::SPtr bodyPrintFinder = std::make_shared<FindBodyPrint>();
    IPicWorker::SPtr facePrintFinder = std::make_shared<FindFacePrint>(templFace);
    IPicWorker::SPtr leftArmFinder = std::make_shared<FindLeftArm>(templLarm);
    IPicWorker::SPtr rightArmFinder = std::make_shared<FindRightArm>(templRarm);


    for (const auto & entry : std::filesystem::directory_iterator("pic/All")) {
        std::cout << entry << std::endl;
        
        auto head = false, hat = false, leftHand = false, rightHand = false, leftArm = false, rightArm = false, leftFoot = false, rightFoot = false, facePrint = false, bodyPrint = false;
        auto tmp = imreadChecked(entry, cv::IMREAD_COLOR);
        if(!cutter->DoWork(tmp)){
            std::cout << "No indie detected" << std::endl;
            continue;
        }


        if(headFinder->DoWork(tmp)){
            head = true;
            hat = hatFinder->DoWork(tmp);
            facePrint = facePrintFinder->DoWork(tmp);
        }

        if(leftHandFinder->DoWork(tmp)){
            leftHand = true;
            leftArm = true;
        }
        else{
            leftArm = leftArmFinder->DoWork(tmp);
        }

        if(rightHandFinder->DoWork(tmp)){
            rightHand = true;
            rightArm = true;
        }
        else{
            rightArm = rightArmFinder->DoWork(tmp);
        }

        leftFoot = leftFootFinder->DoWork(tmp);
        rightFoot = rightFootFinder->DoWork(tmp);
        bodyPrint = bodyPrintFinder->DoWork(tmp);

        std::cout << "#############################################" << std::endl;
        std::cout << "File #" << entry << std::endl;
        std::cout << "---------------------------------------------" << std::endl;
        std::cout << std::boolalpha;
        std::cout << "Hat       -> " << hat << std::endl;
        std::cout << "Head      -> " << head << std::endl;
        std::cout << "Left Hand -> " << leftHand << std::endl;
        std::cout << "Right Hand-> " << rightHand << std::endl; 
        std::cout << "Left Arm  -> " << leftArm << std::endl; 
        std::cout << "Right Arm -> " << rightArm << std::endl; 
        std::cout << "Left Foot -> " << leftFoot << std::endl; 
        std::cout << "Right Foot-> " << rightFoot << std::endl; 
        std::cout << "Face      -> " << facePrint << std::endl; 
        std::cout << "Body Print-> " << bodyPrint << std::endl; 
        std::cout << "#############################################" << std::endl;
    }



    return(Fl::run());
}