/**
 * @file IPicWorker.h
 * @brief Interface for classes, which will perform manipulations, or analysis on the picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#ifndef I_PICWORKER_H
#define I_PICWORKER_H

#include <string>
#include <opencv2/core.hpp>
#include <Object.h>

/**
 * @brief Interface which describes objects which will perform actions on a given picture.
 */
class IPicWorker : public giri::Object<IPicWorker> {
    public:

    /**
     * This is the work function a concrete class should implement.
     * @param pic Picture to be processed. (may be in/out)
     * @return true if feature was detected, false otherwise.
     */
    virtual bool DoWork(cv::Mat& pic) = 0;

    /**
     * @return Name of this feature.
     */
    virtual std::string GetName() = 0;

    protected:
        IPicWorker() = default; // CTor locking, this is an interface only
};

#endif // I_PICWORKER_H