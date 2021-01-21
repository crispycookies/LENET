/**
 * @file FindFeature.h
 * @brief Class which finds a feature on the given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINDFEATURE_H
#define FINDFEATURE_H

#include <opencv2/core.hpp>
#include <Object.h>

#include "IPicWorker.h"

/**
 * @brief Feature finder
 * Class which tries to find a certain feature on a given picture.
 */
class FindFeature: public IPicWorker {
    public:

        /**
         * @param feat feature to work with
         * @param name Name describing this feature
         */
        FindFeature(const cv::Mat& feat, const std::string& name) : m_Feature(feat), m_Name(name) {};

        /**
         * Tries to find the configured feature on a given picture.
         * @param pic [in] Picture to analyze.
         * @return true if feature was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return m_Name;
        };

    using SPtr = std::shared_ptr<FindFeature>;
    using UPtr = std::unique_ptr<FindFeature>;
    using WPtr = std::weak_ptr<FindFeature>;

    private:
        cv::Mat m_Feature;
        std::string m_Name;
};

#endif // FINDFEATURE_H