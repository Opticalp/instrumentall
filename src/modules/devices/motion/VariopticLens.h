/**
 * @file	src/modules/devices/VariopticLens.h
 * @date	Apr 2020
 * @author	PhRG
 */

/*
 Copyright (c) 2020 Ph. Renaud-Goud

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

#ifndef SRC_VARIOPTICLENS_H_
#define SRC_VARIOPTICLENS_H_

#include "core/Module.h"
#include "MotionDevice.h"

#include "tools/comm/serial/SerialCom.h"

/**
 * VariopticLens
 *
 * Varioptic/Corning liquid lens device controller module
 */
class VariopticLens: public MotionDevice
{
public:
    VariopticLens(ModuleFactory* parent, std::string customName, SerialCom& commObj);

    /**
     * Destructor
     *
     * Close the serial port communication
     */
    virtual ~VariopticLens();

    std::string description()
    {
        return "Control a varioptic liquid lens, like a z positioner";
    }

    static void info(SerialCom &commObj, Poco::Logger& tmpLog);
    static std::string readCommBuffer(SerialCom &commObj, Poco::Logger& tmpLog);

private:
    void info() { info(serial, logger()); }

    enum supplParameters
    {
        paramQuery=1, // direct query
					  // TODO: evtl add LUT file? 
        totalParamCnt
    };

    /**
     * Define additional parameters
     * 
     * - send command
     */
    void addMoreParameters();

    std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);

    void singleMotion(int axis, std::vector<double> positions);

    double getPosition(int axis);

    SerialCom &serial; ///< serial communication object

    /**
     * Read data in the comm queue
     */
    std::string readCommBuffer() { return readCommBuffer(serial, logger()); }

	/**
	 * Generate the byte vector to read the given register address
	 */
	static std::string readRegisterCommand(int reg);

	/**
	 * Generate the byte vector to write the given value at the given register address
	 */
	static std::string writeRegisterCommand(int reg, int value);

	/**
	 * Generate the byte vector to write the given values starting at the given register address
	 */
	static std::string writeRegisterCommand(int regStart, std::vector<int> values);

	/**
	 * Check acknoledgement if the writing operation was successful
	 *
	 * @param[in] response response string to analyze
	 * @throw IOexception
	 */
	static void checkWriteAck(std::string response);

	/**
	 * Parse the response to get the register value
	 *
	 * @param[in] response response string to analyze
	 * @throw IOexception
	 */
	static int parseRead(std::string response);

	/**
	 * Calculate CRC
	 *
	 * sum of all characters
	 */
	static char stringSum(std::string cmd);
};

#endif /* SRC_VARIOPTICLENS_H_ */
