/**
 * @file	src/modules/devices/AsiMotion.h
 * @date	Mar 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef SRC_ASIMOTION_H_
#define SRC_ASIMOTION_H_

#include "core/Module.h"
#include "MotionDevice.h"

#include "tools/comm/serial/SerialCom.h"

/**
 * AsiMotion
 *
 * ASI Motion stage device controller module
 */
class AsiMotion: public MotionDevice
{
public:
    AsiMotion(ModuleFactory* parent, std::string customName, SerialCom& commObj);

    /**
     * Destructor
     *
     * Close the serial port communication
     */
    virtual ~AsiMotion();

    std::string description()
    {
        return "Control a XY ASI Motion positioning stage (MS 2000)";
    }

    static void info(SerialCom &commObj, Poco::Logger& tmpLog);
    static std::string readUntilCRLF(SerialCom &commObj, Poco::Logger& tmpLog);

private:
    static size_t refCount; ///< reference counter to generate a unique internal name

    void info() { info(serial, logger()); }

    enum supplParameters
    {
        paramQuery=3, // direct query
        totalParamCnt
    };

    /**
     * Define additional parameters
     * 
     * - send command
     */
    void defineParameters();

    double getFloatParameterValue(size_t paramIndex);
    void setFloatParameterValue(size_t paramIndex, double value);

    std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);

    void singleMotion(int axis, std::vector<double> positions);
    void allMotionSync(std::vector<double> positions);

    double getPosition(int axis);

    /**
     * replace newline or semicolon by delimiter ('\r') in query
     */
    std::string parseMultipleQueries(std::string queries);

    /// Indexes of the input ports
    enum inPorts
    {
        inPortX,
        inPortY,
        inPortZ,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        outPortX,
        outPortY,
        outPortZ,
        outPortCnt
    };

    SerialCom &serial; ///< serial communication object

    /**
     * Called by process() if input data is present
     */
    void move();

    /**
     * Read data until "\r\n" is reached
     */
    std::string readUntilCRLF() { return readUntilCRLF(serial, logger()); }

    double parseSimpleAnswer(const std::string& answer);
    int parseErrorAnswer(const std::string& answer);

    void waitMoveStop();

    void goXYZ(double posX, double posY, double posZ);
    void whereXYZ(double& posX, double& posY, double& posZ);

    /**
     * Update the output ports with the current position
     *
     * To be used when the settings just changed.
     */
    void sendXYZ();

    Poco::FastMutex mutex;
};

#endif /* SRC_ASIMOTION_H_ */
