/**
 * @file FindBodyPrint.h
 * @brief Class which tries to find a body print feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINBODYPRINT_H
#define FINBODYPRINT_H

#include <opencv2/core.hpp>
#include <Object.h>

#include "IPicWorker.h"

/**
 * @brief Body print feature finder
 * Class which tries to find a body print feature in a given picture.
 */
class FindBodyPrint: public IPicWorker {
    public:
        /**
         * Tries to find the body print in the given picture.
         * @param pic [in] Picture to analyze.
         * @return true if feature was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return "Body print";
        };

    using SPtr = std::shared_ptr<FindBodyPrint>;
    using UPtr = std::unique_ptr<FindBodyPrint>;
    using WPtr = std::weak_ptr<FindBodyPrint>;

    private:
        const cv::Scalar m_LowerColorBound = cv::Scalar(25, 48, 155);
        const cv::Scalar m_UpperColorBound = cv::Scalar(33, 104, 214);
};

#endif // FINBODYPRINT_H