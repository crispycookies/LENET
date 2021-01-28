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

#include "IPicWorker.h"

/**
 * @brief Lego figure finder.
 * Class which finds a lego figure on the given picture, cuts it and rotates it horizantally.
 */
class FindFigure : public IPicWorker {
    public:
        FindFigure(const cv::Mat& bg) : m_Background(bg){};

        /**
         * Tries to find a lego figure on the picture.
         * @param pic [in/out] Tries to find any lego figure. Outputs cut out and horizantally rotated figure.
         * @return true if any figure was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return "Find Lego figure";
        };

    using SPtr = std::shared_ptr<FindFigure>;
    using UPtr = std::unique_ptr<FindFigure>;
    using WPtr = std::weak_ptr<FindFigure>;

    private:
        cv::Mat m_Background;
        const size_t m_crop_x = 30;
        const size_t m_crop_y = 23;
        const size_t m_scale_x = 124;
        const size_t m_scale_y = 200;
};

#endif // FINDFIGURE_H