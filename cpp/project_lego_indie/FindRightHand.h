/**
 * @file FindRightHand.h
 * @brief Class which tries to find a right hand feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINDRIGHTHAND_H
#define FINDRIGHTHAND_H

#include <opencv2/core.hpp>
#include <Object.h>

#include "IPicWorker.h"

/**
 * @brief Right hand feature finder
 * Class which tries to find a right hand feature in a given picture.
 */
class FindRightHand: public IPicWorker {
    public:

        /**
         * CTor
         * @param inf if true blocking window showing a graphical result of this worker will be displayed.
         */
        FindRightHand(bool inf = false) : m_ShowInfo(inf) {};

        /**
         * Tries to find the right hand in the given picture.
         * @param pic [in] Picture to analyze.
         * @return true if feature was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return "Right hand";
        };

    using SPtr = std::shared_ptr<FindRightHand>;
    using UPtr = std::unique_ptr<FindRightHand>;
    using WPtr = std::weak_ptr<FindRightHand>;

    private:
        const cv::Scalar m_LowerColorBound = cv::Scalar(11, 85, 240);
        const cv::Scalar m_UpperColorBound = cv::Scalar(29, 107, 255);
        bool m_ShowInfo;
};

#endif // FINDRIGHTHAND_H