/**
 * @file	src/modules/devices/motion/Meca500.h
 * @date	May 2018
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2018 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULES_DEVICES_MOTION_MECA500_H_
#define SRC_MODULES_DEVICES_MOTION_MECA500_H_

#include "core/Module.h"
#include "MotionDevice.h"

#include "Poco/Net/IPAddress.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/WebSocket.h"

#include "Poco/Mutex.h"

/**
 * Module interfacing a Meca500 Mecademic robot arm
 */
class Meca500: public MotionDevice
{
public:
    Meca500(ModuleFactory* parent, std::string customName);
	~Meca500();

    std::string description();

private:
	Poco::Net::IPAddress ipAddressFromFactoryTree();
	Poco::Net::IPAddress ipAddress;
	Poco::Net::SocketAddress sa; // control port
	Poco::Net::HTTPClientSession httpSession;
	Poco::Net::HTTPRequest simpleReq;
	Poco::Net::HTTPResponse resp;
	Poco::Net::WebSocket tcpSocket;

	void initComm();
	void closeComm();

    int getStatus();
    void fixStatus(int status);

    enum statusRobot
    {
        activated = 0x01,
        homed = 0x02,
        simu = 0x04,
        errored = 0x08,
        paused = 0x10,
        eobEnabled = 0x20,
        eomEnabled = 0x40,
    };

    /**
     * replace newline or semicolon by \0 in query
     */
    std::string parseMultipleQueries(std::string queries);

	/**
	 * Send the given query
	 *
	 * @return response
	 */
	std::string sendQuery(std::string query);

    /**
     * Send command
     *
     * do not wait for an answer
     */
    void sendCommand(std::string command);

	/**
	 * Send the given query and wait that the given substring is present in the response
	 *
	 * @return response
	 */
	std::string sendQueryCheckResp(std::string query, std::string respSubStr);

	/**
	 * Wait a response
	 *
	 * @return the response
	 * @throw Poco::TimeoutException
	 */
	std::string waitResp();

	enum supplParameters
	{
		paramQuery=6, // direct query
		paramSimuMode, // change to simu mode (0, 1)
		totalParamCnt
	};

	/**
	 * Define additional parameters
	 * 
	 * - simu mode
	 * - send command
	 */
	void defineParameters();

    Poco::Int64 getIntParameterValue(size_t paramIndex);
	void setIntParameterValue(size_t paramIndex, Poco::Int64 value);

	double getFloatParameterValue(size_t paramIndex);
	void setFloatParameterValue(size_t paramIndex, double value);

	std::string getStrParameterValue(size_t paramIndex);
	void setStrParameterValue(size_t paramIndex, std::string value);

	///**
    // * Apply simultanously all the parameters.
    // *
    // * use TcpIpWithWatchDog variable comm for the communication
    // */
    //void applyParameters();

    //void refreshParameterInternalValues();

    ///**
    // * Check if the given response string contains an error code
    // *
    // * @return 0 if no error. error code if any.
    // *
    // * error codes:
    // *  * Err 1: A parameter value is invalid
    // *  * Err 2: Command not recognised
    // *  * Err 3: Numeric value is wrong format
    // *  * Err 4: Wrong number of parameters
    // *  * Err 5: (Warning only) A timing parameter was out of range and has been adjusted
    // */
    //int checkResponseError(std::string response);

	 void singleMotion(int axis, std::vector<double> positions);
	 void allMotionSync(std::vector<double> positions);

	 double getPosition(int axis);

	 void getPosition(double& x, double& y, double& z, double& a, double& b, double& c);
	 void setPosition(double x, double y, double z, double a, double b, double c);

     bool isErrored(std::string resp);

	 Poco::FastMutex usingSocket;
};

#endif /* SRC_MODULES_DEVICES_MOTION_MECA500_H_ */
