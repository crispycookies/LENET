/**
 * @file FindHead.h
 * @brief Class which tries to find a head feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINDHEAD_H
#define FINDHEAD_H

#include <opencv2/core.hpp>
#include <Object.h>

#include "IPicWorker.h"

/**
 * @brief Head feature finder
 * Class which tries to find a head feature in a given picture.
 */
class FindHead: public IPicWorker {
    public:
        /**
         * Tries to find the head in the given picture.
         * @param pic [in] Picture to analyze.
         * @return true if feature was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return "Head";
        };

    using SPtr = std::shared_ptr<FindHead>;
    using UPtr = std::unique_ptr<FindHead>;
    using WPtr = std::weak_ptr<FindHead>;

    private:
        const cv::Scalar m_LowerColorBound = cv::Scalar(11, 85, 240);
        const cv::Scalar m_UpperColorBound = cv::Scalar(29, 107, 255);
};

#endif // FINHEAD_H