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
            ("use_console", po::value<bool>(), "Print the result to console rather than using a GUI. (if not set or invalid a gui prompt will force you to select one)")
            ("show_steps", po::value<bool>(), "Visualize every working step. (if not set or invalid a gui prompt will force you to select one)")
            ("images", po::value<std::string>(), "Image folder to be used. (if not set or invalid a gui prompt will force you to select one)");

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
    bool show_steps;
    bool use_console;
};

r_val getFromCmdLine(po::variables_map vm){
    std::filesystem::path bg_img_path = "./pic/Other/image_100.jpg";
    std::filesystem::path path = "";
    std::filesystem::path templDir = "./pic/templates";
    bool show_steps = false;
    bool use_console = true;

    if(vm.count("background")){
        bg_img_path = vm["background"].as<std::string>();
    }
    if(vm.count("images")){
        path = vm["images"].as<std::string>();
    }
    if(!std::filesystem::exists(path)){
        path = fl_dir_chooser("Choose image folder...", "./pic/", 1);
    }
    if(vm.count("templdir")){
        templDir = vm["templdir"].as<std::string>();
    }
    if(vm.count("show_steps")){
        show_steps =  vm["show_steps"].as<bool>();
    }
    else{
        fl_message_title("Visualize?");
        show_steps = fl_choice("Do you want to visualize all processing steps?", "No", "Yes", 0);
    }
    if(vm.count("use_console")){
        use_console =  vm["use_console"].as<bool>();
    }
    else{
        fl_message_title("Use console?");
        use_console = fl_choice("Do you want to print the result to console rather than using a GUI?", "No", "Yes", 0);
    }

    return {bg_img_path, path, templDir, show_steps, use_console};
}

int main(int argc, char** argv)
{
    Fl::scheme("gleam");

    // wm icon
    auto icon = std::make_shared<Fl_RGB_Image>(icon_data, 128, 128, ImgShow::fl_imgtype::rgba, 0);
    ((Fl_Double_Window*)fl_message_icon()->parent())->icon(icon.get());
    auto vm_b = parseCmdLine(argc, argv);
    if(vm_b == std::nullopt){
        return EXIT_SUCCESS;
    }
    auto vm = vm_b.value();
    auto config = getFromCmdLine(vm);

    // read images
    auto bg_img = imreadChecked(config.bg_img_path, cv::IMREAD_COLOR);
    auto templFace = imreadChecked(config.templDir.append("template_face.png"), cv::IMREAD_COLOR);
    auto templLarm = imreadChecked(config.templDir.remove_filename().append("template_left_arm.png"), cv::IMREAD_COLOR);
    auto templRarm = imreadChecked(config.templDir.remove_filename().append("template_right_arm.png"), cv::IMREAD_COLOR);

    {
        config.templDir.remove_filename();
    }


#ifdef _WIN32
    if(!config.use_console){
        FreeConsole();
    }
#endif

    IPicWorker::SPtr cutter = std::make_shared<FindFigure>(bg_img, config.show_steps);
    IPicWorker::SPtr headFinder = std::make_shared<FindHead>(config.show_steps);
    IPicWorker::SPtr hatFinder = std::make_shared<FindHat>(config.show_steps);
    IPicWorker::SPtr leftHandFinder = std::make_shared<FindLeftHand>(config.show_steps);
    IPicWorker::SPtr rightHandFinder = std::make_shared<FindRightHand>(config.show_steps);
    IPicWorker::SPtr rightFootFinder = std::make_shared<FindRightFoot>(config.show_steps);
    IPicWorker::SPtr leftFootFinder = std::make_shared<FindLeftFoot>(config.show_steps);
    IPicWorker::SPtr bodyPrintFinder = std::make_shared<FindBodyPrint>(config.show_steps);
    IPicWorker::SPtr facePrintFinder = std::make_shared<FindFacePrint>(templFace, config.show_steps);
    IPicWorker::SPtr leftArmFinder = std::make_shared<FindLeftArm>(templLarm, config.show_steps);
    IPicWorker::SPtr rightArmFinder = std::make_shared<FindRightArm>(templRarm, config.show_steps);


    for (const auto & entry : std::filesystem::directory_iterator(config.path)) {

        auto head = false, hat = false, leftHand = false, rightHand = false, leftArm = false, rightArm = false, leftFoot = false, rightFoot = false, facePrint = false, bodyPrint = false;
        auto tmp = imreadChecked(entry, cv::IMREAD_COLOR);
        if(!cutter->DoWork(tmp)){
            std::stringstream strstr;
            strstr << entry.path().string() << ": No indie detected!" << std::endl;
            if(config.use_console){
                std::cout << strstr.str();
            }
            else {
                ImgShow a(tmp, "Cut Picture", ImgShow::rgb, false);
                fl_message_title("Result");
                fl_message(strstr.str().c_str());
            }
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

        std::stringstream strstr;
        strstr << "#############################################" << std::endl;
        strstr << "File #" << entry << std::endl;
        strstr << "---------------------------------------------" << std::endl;
        strstr << std::boolalpha;
        strstr << "Hat       -> " << hat << std::endl;
        strstr << "Head      -> " << head << std::endl;
        strstr << "Left Hand -> " << leftHand << std::endl;
        strstr << "Right Hand-> " << rightHand << std::endl; 
        strstr << "Left Arm  -> " << leftArm << std::endl; 
        strstr << "Right Arm -> " << rightArm << std::endl; 
        strstr << "Left Foot -> " << leftFoot << std::endl; 
        strstr << "Right Foot-> " << rightFoot << std::endl; 
        strstr << "Face      -> " << facePrint << std::endl; 
        strstr << "Body Print-> " << bodyPrint << std::endl; 
        strstr << "#############################################" << std::endl;

        if(config.use_console){
            std::cout << strstr.str();
        }
        else{
            ImgShow a(tmp, "Cut Picture", ImgShow::rgb, false);
            fl_message_title("Result");
            fl_message(strstr.str().c_str());
        }
    }



    return(Fl::run());
}