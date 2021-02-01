/**
 * @file FindHat.h
 * @brief Class which tries to find a hat feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINDHAT_H
#define FINDHAT_H

#include <opencv2/core.hpp>
#include <Object.h>

#include "IPicWorker.h"

/**
 * @brief Hat feature finder
 * Class which tries to find a hat feature in a given picture.
 */
class FindHat: public IPicWorker {
    public:
        /**
         * Tries to find the hat in the given picture.
         * @param pic [in] Picture to analyze.
         * @return true if feature was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return "Hat";
        };

    using SPtr = std::shared_ptr<FindHat>;
    using UPtr = std::unique_ptr<FindHat>;
    using WPtr = std::weak_ptr<FindHat>;

    private:
        const cv::Scalar m_LowerColorBound = cv::Scalar(6, 80, 63);
        const cv::Scalar m_UpperColorBound = cv::Scalar(19, 255, 153);
};

#endif // FINDHAT_H