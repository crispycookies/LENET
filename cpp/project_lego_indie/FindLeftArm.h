/**
 * @file FindLeftArm.h
 * @brief Class which tries to find a left arm feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINDLEFTARM_H
#define FINDLEFTARM_H

#include <opencv2/core.hpp>
#include <Object.h>

#include "IPicWorker.h"

/**
 * @brief Left arm feature finder
 * Class which tries to find a left arm feature in a given picture.
 */
class FindLeftArm: public IPicWorker {
    public:
        /**
         * CTor
         * @param tmpl Example template used to match the arm.
         */
        FindLeftArm(const cv::Mat & tmpl) : m_Template(tmpl) {};

        /**
         * Tries to find the left arm in the given picture.
         * @param pic [in] Picture to analyze.
         * @return true if feature was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return "Left arm";
        };

    using SPtr = std::shared_ptr<FindLeftArm>;
    using UPtr = std::unique_ptr<FindLeftArm>;
    using WPtr = std::weak_ptr<FindLeftArm>;

    private:
        cv::Mat m_Template;
};

#endif // FINDLEFTARM_H