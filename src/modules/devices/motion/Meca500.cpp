/**
 * @file	src/modules/devices/motion/Meca500.cpp
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

#include "Meca500.h"

#include "core/ModuleFactoryBranch.h"

#include "IpDeviceFactory.h"
#include "tools/comm/Decorated.h"

#include "Poco/String.h"
#include "Poco/NumberFormatter.h"

#include "Poco/Net/NetException.h"

using namespace Poco::Net;

/// socket receive timeout (ms)
#define TIMEOUT 500

// how many times timeout before aborting response awaiting
#define TIMEOUT_COUNTER_LIMIT 20

Meca500::Meca500(ModuleFactory* parent, std::string customName):
		MotionDevice(parent, customName, 
			xAxis | yAxis | zAxis | aAxis | bAxis | cAxis), 
		ipAddress(ipAddressFromFactoryTree()), 
		sa(ipAddress, 10000), // control port
		httpSession(sa),
		simpleReq(HTTPRequest::HTTP_GET, "/", HTTPMessage::HTTP_1_1),
		tcpSocket(httpSession, simpleReq, resp)
{
	// create the control socket
	initComm();

	construct("meca500_ip" + ipAddress.toString(), customName);
}

Meca500::~Meca500()
{
	closeComm();
}

std::string Meca500::description()
{
	return "Meca500 robot device, ";
	// TODO: add ID, version, etc
}

Poco::Net::IPAddress Meca500::ipAddressFromFactoryTree()
{
	ModuleFactoryBranch* dad =
		reinterpret_cast<ModuleFactoryBranch*>(parent());

	IpDeviceFactory* grandPa =
		reinterpret_cast<IpDeviceFactory*>(dad->parent());

	return grandPa->parseSelector(dad->getSelector());
}

void Meca500::initComm()
{
	setLogger("module.Meca500.startup");

	httpSession.setTimeout(TIMEOUT * 1000, TIMEOUT * 1000, TIMEOUT * 1000);

	try
	{
		tcpSocket.setSendTimeout(TIMEOUT * 1000); // to check if the connections is still active
		tcpSocket.setReceiveTimeout(TIMEOUT * 1000);

		try
		{
			waitResp(); // purge input
		}
		catch (Poco::TimeoutException&)
		{
			poco_warning(logger(), "No msg received at socket connexion");
		}

		fixStatus(getStatus());

        if (isErrored(sendQuery("GetPose"))
            || isErrored(sendQuery("SetEOB(0)"))
            || isErrored(sendQuery("SetEOM(1)")))
	            throw Poco::RuntimeException("Robot init error");
	}
	catch (Poco::Exception& e)
	{
		closeComm();
		poco_warning(logger(), "Not able to open the control port: " + e.displayText());
	}
}

void Meca500::closeComm()
{
	// close comm
    try 
    {
        waitResp(); // purge input
    }
    catch (Poco::TimeoutException&)
    {
        // nothing to do
    }

	tcpSocket.shutdown();
	tcpSocket.close();
}

std::string Meca500::sendQuery(std::string query)
{
	Poco::FastMutex::ScopedLock lock(usingSocket);

	int length;

	length = query.size();
	try
	{
		// we send length + 1 chars to include the \0 char
		tcpSocket.sendFrame(query.c_str(), length + 1); 

		poco_information(logger(),
			Poco::NumberFormatter::format(length) + " bytes sent: \n" +
			decoratedCommandKeep(query, length + 1));
	}
	catch (Poco::Exception& e)
	{
		poco_error(logger(), query + " [## SEND ERROR ##]: " + e.displayText());
	}

	return waitResp();
}

void Meca500::sendCommand(std::string command)
{
    Poco::FastMutex::ScopedLock lock(usingSocket);

    int length = command.size();
    try
    {
        // we send length + 1 chars to include the \0 char
        tcpSocket.sendBytes(command.c_str(), length + 1);

        poco_information(logger(),
            Poco::NumberFormatter::format(length) + " bytes sent: \n" +
            decoratedCommandKeep(command, length + 1));
    }
    catch (Poco::Exception& e)
    {
        poco_error(logger(), command + " [## SEND ERROR ##]: " + e.displayText());
    }
}

std::string Meca500::sendQueryCheckResp(std::string query,
	std::string respSubStr)
{
    std::string resp;

    try
    {
        resp = sendQuery(query);
    }
    catch (Poco::TimeoutException&)
    {
        poco_warning(logger(), "response awaiting timed out once. ");
    }

    int timeoutCnter = 0;
    while (resp.find(respSubStr) == std::string::npos)
    {
        try
        {
            resp += waitResp();
        }
        catch (Poco::TimeoutException&)
        {
            poco_warning(logger(), "response awaiting timed out (#"
                + Poco::NumberFormatter::format(timeoutCnter)
                + ")");
            timeoutCnter++;
            if (timeoutCnter > TIMEOUT_COUNTER_LIMIT)
                throw;
        }

        if (isErrored(resp))
            throw Poco::RuntimeException("Meca500 error");
    }

	return resp;
}

std::string Meca500::waitResp()
{
	std::string response;

	char buffer[4096];
	int length = 4096;
	int flags;
	int len = tcpSocket.receiveFrame(buffer, length, flags);

	if (!(flags & WebSocket::FRAME_FLAG_FIN))
		poco_warning(logger(), "webSocket frame is not complete");

	if (!(flags & WebSocket::FRAME_OP_TEXT))
		poco_warning(logger(), "receiving: not a text frame");

	if (len)
	{
		response.assign(buffer, len);

		poco_information(logger(),
			Poco::NumberFormatter::format(len) + " bytes received: \n" +
			decoratedCommandKeep(response));
	}
	else
	{
		throw Poco::Net::ConnectionResetException("Connection reset by peer");
	}

    for (std::string::iterator it = response.begin(),
        ite = response.end(); it != ite; it++)
    {
        if (*it == '\0')
            *it = '\n';
    }

	return response;
}

void Meca500::defineParameters()
{
	poco_information(logger(), "define device-specific parameters");

	setParameterCount(totalParamCnt);

	addParameter(paramQuery, "query", "direct query to the robot", ParamItem::typeString);
	addParameter(paramSimuMode, "simuMode", "go to simulation mode (0=simu, 1=real motion)", ParamItem::typeInteger);
}

Poco::Int64 Meca500::getIntParameterValue(size_t paramIndex)
{
	switch (paramIndex)
	{
	case paramSimuMode:
        if (getStatus() & simu)
            return 1;
        else
            return 0;
	default:
		poco_bugcheck_msg("invalid parameter index");
		throw Poco::BugcheckException();
	}
}

void Meca500::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
	if (paramIndex != paramSimuMode)
		poco_bugcheck_msg("invalid parameter index");

    if (value)
    {
        if (getIntParameterValue(paramIndex) == 0)
        {
            sendQueryCheckResp("DeactivateRobot", "[2004][Motors deactivated.]");
            sendQueryCheckResp("ActivateSim", "[2045][The simulation mode is enabled.]");
            sendQueryCheckResp("ActivateRobot", "[2000][Motors activated.]");
            sendQueryCheckResp("Home", "[2002][Homing done.]");
            sendQuery("SetEOB(0)");
            sendQuery("SetEOM(1)");
        }
    }
    else
    {
        if (getIntParameterValue(paramIndex) == 1)
        {
            sendQueryCheckResp("DeactivateRobot", "[2004][Motors deactivated.]");
            sendQueryCheckResp("DeactivateSim", "[2046][The simulation mode is disabled.]");
            sendQueryCheckResp("ActivateRobot", "[2000][Motors activated.]");
            sendQuery("SetEOB(0)");
            sendQuery("SetEOM(1)");
            poco_warning(logger(), "Please reset manually the homing, TRF, WRF, ... ");
        }
    }
}

double Meca500::getFloatParameterValue(size_t paramIndex)
{
	if (paramIndex < axisIndexCnt)
		return MotionDevice::getFloatParameterValue(paramIndex);
	else
	{
		// no supplementary parameter implemented here
		poco_bugcheck_msg("invalid parameter index");
		throw Poco::BugcheckException();
	}
}

void Meca500::setFloatParameterValue(size_t paramIndex, double value)
{
	if (paramIndex < axisIndexCnt)
		return MotionDevice::setFloatParameterValue(paramIndex, value);
	else
	{
		// no supplementary parameter implemented here
		poco_bugcheck_msg("invalid parameter index");
		throw Poco::BugcheckException();
	}
}

std::string Meca500::getStrParameterValue(size_t paramIndex)
{
	switch (paramIndex)
	{
	case paramQuery:
        try
        {
            return waitResp();
        }
        catch (Poco::TimeoutException&)
        {
            return sendQuery("GetStatusRobot");
        }
	default:
		poco_bugcheck_msg("invalid parameter index");
		throw Poco::BugcheckException();
	}
}

void Meca500::setStrParameterValue(size_t paramIndex, std::string value)
{
	if (paramIndex != paramQuery)
		poco_bugcheck_msg("invalid parameter index");

	sendCommand(parseMultipleQueries(value));
}

/// minimal movement: 0.5 um, 0.0005 deg (arbitrary)
#define EPS_MV 0.0005

void Meca500::singleMotion(int axis, std::vector<double> positions)
{
	double x, y, z, a, b, c;
	getPosition(x, y, z, a, b, c);

	bool move = false; 

	if (axis & xAxis)
	{
		move = (abs(positions.at(xIndex) - x) > EPS_MV);
		x = positions.at(xIndex);
	}
	if (axis & yAxis)
	{
		move = move || (abs(positions.at(yIndex) - y) > EPS_MV);
		y = positions.at(yIndex);
	}
	if (axis & zAxis)
	{
		move = move || (abs(positions.at(zIndex) - z) > EPS_MV);
		z = positions.at(zIndex);
	}
	if (axis & aAxis)
	{
		move = move || (abs(positions.at(aIndex) - a) > EPS_MV);
		a = positions.at(aIndex);
	}
	if (axis & bAxis)
	{
		move = move || (abs(positions.at(bIndex) - b) > EPS_MV);
		b = positions.at(xIndex);
	}
	if (axis & cAxis)
	{
		move = move || (abs(positions.at(cIndex) - c) > EPS_MV);
		c = positions.at(cIndex);
	}

	// set new position
	if (move)
		setPosition(x, y, z, a, b, c);
	else
		poco_information(logger(), "new position too close to move");
}

void Meca500::allMotionSync(std::vector<double> positions)
{
	setPosition(positions[xIndex], positions[yIndex], positions[zIndex], 
		positions[aIndex], positions[bIndex], positions[cIndex]);
}

double Meca500::getPosition(int axis)
{
	double x, y, z, a, b, c;
	getPosition(x, y, z, a, b, c);
	switch (axis)
	{
	case xAxis:
		return x;
	case yAxis: 
		return y;
	case zAxis:
		return z;
	case aAxis:
		return a;
	case bAxis:
		return b;
	case cAxis:
		return c;
	default:
		poco_bugcheck_msg("impossible axis index");
		throw Poco::BugcheckException();
	}
}

#include "Poco/RegularExpression.h"
#include "Poco/NumberParser.h"

using Poco::RegularExpression;
using Poco::NumberParser;

int Meca500::getStatus()
{
    std::string resp = sendQuery("GetStatusRobot");

    RegularExpression regexp("\\[2007\\]\\[([01]),([01]),([01]),([01]),([01]),([01]),([01])\\]");
    std::vector<std::string> strVec;
    regexp.split(resp, 0, strVec);

    poco_information(logger(), "status parsing: got "
        + Poco::NumberFormatter::format(strVec.size() - 1) + " bits");

    if (strVec.size() != 8)
        throw Poco::DataFormatException("not able to parse the Meca500 status");

    int status = 0;

    if (strVec[1] == "1")
        status |= activated;
    if (strVec[2] == "1")
        status |= homed;
    if (strVec[3] == "1")
        status |= simu;
    if (strVec[4] == "1")
        status |= errored;
    if (strVec[5] == "1")
        status |= paused;
    if (strVec[6] == "1")
        status |= eobEnabled;
    if (strVec[7] == "1")
        status |= eomEnabled;

    return status;
}


void Meca500::fixStatus(int status)
{
    if (status & errored)
    {
        if (sendQuery("ResetError").find("[2005][The error was reset.]") != std::string::npos)
            poco_information(logger(), "The error was reset");
        else
            throw Poco::RuntimeException("unable to reset robot error");
    }

    if (status & paused)
        sendQueryCheckResp("ResumeMotion","[2043][Motion resumed.]");
}


void Meca500::getPosition(double& x, double& y, double& z, double& a, double& b, double& c)
{
	std::string response = sendQuery("GetPose");

	// parse answer, e.g.
	// [2027][-182.255,0.000,-124.203,0.000,90.000,0.000]
	RegularExpression regexp("\\[2027\\]\\[(-?[0-9]+\\.[0-9]+),(-?[0-9]+\\.[0-9]+)"
		",(-?[0-9]+\\.[0-9]+),(-?[0-9]+\\.[0-9]+)"
		",(-?[0-9]+\\.[0-9]+),(-?[0-9]+\\.[0-9]+)\\]");

    std::vector<std::string> strVec;
    regexp.split(response, 0, strVec);
    poco_information(logger(), "position parsing: got " 
        + Poco::NumberFormatter::format(strVec.size()-1) + " coords");

    if (strVec.size() != 7)
        throw Poco::DataFormatException("not able to parse the Meca500 position");

    x = NumberParser::parseFloat(strVec[1]);
    y = NumberParser::parseFloat(strVec[2]);
    z = NumberParser::parseFloat(strVec[3]);
    a = NumberParser::parseFloat(strVec[4]);
    b = NumberParser::parseFloat(strVec[5]);
    c = NumberParser::parseFloat(strVec[6]);
}


#include "Poco/Format.h"

void Meca500::setPosition(double x, double y, double z, double a, double b, double c)
{
    double xo, yo, zo, ao, bo, co;
    getPosition(xo, yo, zo, ao, bo, co);
    if ((x == xo) && (y == yo) && (z == zo) && (a == ao) && (b == bo) && (c == co))
        return;

    // move and wait end of move
    sendQueryCheckResp(
        Poco::format("MoveLin(%f,%f,%f,%f,%f,%f)",x,y,z,a,b,c),
        "[3004][End of movement.]");
}

bool Meca500::isErrored(std::string resp)
{
    RegularExpression regexp("\\[(10[0-9][0-9])\\]\\[(.*)\\]");

    std::vector<std::string> vec;
    if (regexp.split(resp, vec))
    {
        poco_error(logger(), name() + " error #" + vec[0]
            + ": " + vec[1]);
        return true;
    }
    else
        return false;
}

std::string Meca500::parseMultipleQueries(std::string queries)
{
    RegularExpression regexp("[\\n\\r\\t\\;]+");
    regexp.subst(queries, std::string("\0",1), RegularExpression::RE_GLOBAL);

    return queries;
}

