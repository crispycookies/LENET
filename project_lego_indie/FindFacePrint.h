/**
 * @file FindFacePrint.h
 * @brief Class which tries to find a face print featur in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef FINDFACEPRINT_H
#define FINDFACEPRINT_H

#include <opencv2/core.hpp>
#include <Object.h>

#include "IPicWorker.h"

/**
 * @brief Face print feature finder
 * Class which tries to find a face print feature in a given picture.
 */
class FindFacePrint: public IPicWorker {
    public:

        /**
         * CTor
         * @param tmpl Example template used to match the face.
         * @param inf if true blocking window showing a graphical result of this worker will be displayed.
         */
        FindFacePrint(const cv::Mat & tmpl, bool inf = false) : m_Template(tmpl), m_ShowInfo(inf) {};

        /**
         * Tries to find the face print in the given picture.
         * @param pic [in] Picture to analyze.
         * @return true if feature was found, false otherwise.
         */
        virtual bool DoWork(cv::Mat& pic) override;


        virtual std::string GetName() override{
            return "Face print";
        };

    using SPtr = std::shared_ptr<FindFacePrint>;
    using UPtr = std::unique_ptr<FindFacePrint>;
    using WPtr = std::weak_ptr<FindFacePrint>;

    private:
        cv::Mat m_Template;
        bool m_ShowInfo;
};

#endif // FINDFACEPRINT_H