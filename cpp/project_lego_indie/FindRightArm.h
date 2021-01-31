/**
 * @file FindRightArm.h
 * @brief Class which tries to find a right arm feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINDRIGHTARM_H
#define FINDRIGHTARM_H

#include <opencv2/core.hpp>
#include <Object.h>

#include "IPicWorker.h"

/**
 * @brief Right arm feature finder
 * Class which tries to find a right arm feature in a given picture.
 */
class FindRightArm: public IPicWorker {
    public:

        /**
         * CTor
         * @param tmpl Example template used to match the arm.
         */
        FindRightArm(const cv::Mat & tmpl) : m_Template(tmpl) {};

        /**
         * Tries to find the right arm in the given picture.
         * @param pic [in] Picture to analyze.
         * @return true if feature was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return "Right arm";
        };

    using SPtr = std::shared_ptr<FindRightArm>;
    using UPtr = std::unique_ptr<FindRightArm>;
    using WPtr = std::weak_ptr<FindRightArm>;

    private:
        cv::Mat m_Template;
};

#endif // FINDRIGHTARM_H