/**
 * @file FindRightFoot.h
 * @brief Class which tries to find a right foot feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINDRIGHTFOOT_H
#define FINDRIGHTFOOT_H

#include <opencv2/core.hpp>
#include <Object.h>

#include "IPicWorker.h"

/**
 * @brief Right foot feature finder
 * Class which tries to find a right foot feature in a given picture.
 */
class FindRightFoot: public IPicWorker {
    public:
        /**
         * Tries to find the right foot in the given picture.
         * @param pic [in] Picture to analyze.
         * @return true if feature was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return "Right foot";
        };

    using SPtr = std::shared_ptr<FindRightFoot>;
    using UPtr = std::unique_ptr<FindRightFoot>;
    using WPtr = std::weak_ptr<FindRightFoot>;

    private:
        const cv::Scalar m_LowerColorBound = cv::Scalar(12, 107, 178);
        const cv::Scalar m_UpperColorBound = cv::Scalar(20, 178, 229);
};

#endif // FINDRIGHTFOOT_H