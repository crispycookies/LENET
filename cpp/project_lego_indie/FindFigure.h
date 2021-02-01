/**
 * @file FindFigure.h
 * @brief Class which finds a lego figure on the given picture, cuts it out and rotates it horizantally.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINDFIGURE_H
#define FINDFIGURE_H

#include <opencv2/core.hpp>
#include <Object.h>

#include <tuple>

#include "IPicWorker.h"

/**
 * @brief Lego figure finder.
 * Class which finds a lego figure on the given picture, cuts it and rotates it horizantally.
 */
class FindFigure : public IPicWorker {
protected:
    virtual cv::Mat correct_brightness(const cv::Mat& pic);
    virtual cv::Mat crop(const cv::Mat & brightness_corrected);
    virtual cv::Mat shift(const cv::Mat & roi);
    virtual cv::Mat make_grey(const cv::Mat & shifted);
    virtual cv::Mat make_erode(const cv::Mat & grey);
    virtual cv::Mat make_thresh(const cv::Mat & grey, const cv::Mat & erode_mask);
    virtual std::tuple<std::vector<std::vector<cv::Point>>, std::vector<cv::Vec4i>, std::vector<cv::RotatedRect>> find_contours_ff(const cv::Mat & thresh);
    virtual std::pair<cv::Point, cv::Point> check_uppermost(cv::Mat& pic, const std::vector<cv::Vec4i> & lines);
    virtual std::tuple<cv::Point2f, cv::Mat, cv::Mat, cv::Mat> get_rotation_matrix(const cv::RotatedRect & rot_rect, cv::Mat & roi);
    virtual std::vector<cv::Vec4i> analyzeLines(const cv::Mat & pic);
public:
    FindFigure(const cv::Mat& bg) : m_Background(bg){};

    /**
     * Tries to find a lego figure on the picture.
     * @param pic [in/out] Tries to find any lego figure. Outputs cut out and horizantally rotated figure.
     * @return true if any figure was found, false otherwise.
     */
    virtual bool DoWork(cv::Mat& pic) override;


    virtual std::string GetName() override{
         return "Lego figure";
    };

    using SPtr = std::shared_ptr<FindFigure>;
    using UPtr = std::unique_ptr<FindFigure>;
    using WPtr = std::weak_ptr<FindFigure>;

private:
    cv::Mat m_Background;
    const size_t m_crop_x = 35;
    const size_t m_crop_y = 27;
    const size_t m_scale_x = 124;
    const size_t m_scale_y = 200;

    cv::Mat drawLineP(const std::vector<cv::Vec4i>& lines, const cv::Mat& pic);
};

#endif // FINDFIGURE_H