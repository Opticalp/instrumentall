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

#include "Poco/Net/SocketAddress.h"

#include "Poco/Net/NetException.h"

using namespace Poco::Net;

/// socket receive timeout (ms)
#define TIMEOUT 500

// how many times timeout before aborting response awaiting
#define TIMEOUT_COUNTER_LIMIT 100

Meca500::Meca500(ModuleFactory* parent, std::string customName):
		MotionDevice(parent, customName, 
			xAxis | yAxis | zAxis | aAxis | bAxis | cAxis)
{
	setIpAddressFromFactoryTree();

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

void Meca500::setIpAddressFromFactoryTree()
{
	ModuleFactoryBranch* dad =
		reinterpret_cast<ModuleFactoryBranch*>(parent());

	IpDeviceFactory* grandPa =
		reinterpret_cast<IpDeviceFactory*>(dad->parent());

	ipAddress = grandPa->parseSelector(dad->getSelector());
}

void Meca500::initComm()
{
	setLogger("module.Meca500.startup");

	// open communication with the device
	SocketAddress sa(ipAddress, 10000); // control port

	try
	{
		tcpSocket.connect(sa);
		tcpSocket.setReuseAddress(true);
		tcpSocket.setReusePort(true);
		tcpSocket.setKeepAlive(true);
		// tcpSocket.setNoDelay(true);
		tcpSocket.setReceiveTimeout(TIMEOUT * 1000);
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
        {
            closeComm();
            throw Poco::RuntimeException("Robot init error");
        }
	}
	catch (Poco::Exception& e)
	{
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
		tcpSocket.sendBytes(query.c_str(), length + 1); 

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
	std::string resp = sendQuery(query);

    while (resp.find(respSubStr) == std::string::npos)
    {
        int timeoutCnter = 0;
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
	int len = tcpSocket.receiveBytes(buffer, length);

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

	//TODO
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
		return sendQuery("GetStatusRobot");
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

//void Meca500::applyParameters()
//{
//	poco_information(logger(), "apply " + name() + " parameters");
//
//	// set pulse mode for every channel
//	for (int ind = 1; ind <= chanCnt; ind++)
//	{
//		std::string command, resp;
//
//		double delay, duration;
//		Poco::Int64 brightness;
//
//		getInternalFloatParameterValue(paramDelay, delay);
//		getInternalFloatParameterValue(paramDuration, duration);
//		getInternalIntParameterValue(paramBrightness, brightness);
//
//		command = Poco::format("RT%d,%.3f,%.3f,%?d\r",
//				ind, duration, delay, brightness);
//
//		resp = comm.sendQuery(command);
//
//		if (*(resp.end()-1) != '>')
//			poco_warning(logger(), "Command acknowledgment failed for command: \n" + command);
//
//		int err = checkResponseError(resp);
//		if (err)
//			poco_warning(logger(), Poco::format("Error #%d happened. ", err));
//	}
//
//	// set output current rating
//	for (int ind = 1; ind <= chanCnt; ind++)
//	{
//		std::string command, resp;
//
//		double nominalCur;
//
//		getInternalFloatParameterValue(paramNominalCurrent, nominalCur);
//
//		command = Poco::format("RR%d,%.3f\r",
//				ind, nominalCur);
//
//		resp = comm.sendQuery(command);
//
//		if (*(resp.end()-1) != '>')
//			poco_warning(logger(), "Command acknowledgment failed for command: \n" + command);
//
//		int err = checkResponseError(resp);
//		if (err)
//			poco_warning(logger(), Poco::format("Error #%d happened. ", err));
//	}
//
//	refreshParameterInternalValues();
//}
//
//#include "Poco/StringTokenizer.h"
//#include "Poco/NumberParser.h"
//
//void Meca500::refreshParameterInternalValues()
//{
//	poco_information(logger(), "refreshing parameters value");
//
//	std::string resp = comm.sendQuery("ST\r");
//
//	std::vector<double> delay(chanCnt), duration(chanCnt), nominalCurr(chanCnt);
//	std::vector<Poco::Int64> brightness(chanCnt);
//
//	for (int ind=1; ind <= chanCnt; ind++)
//	{
//		size_t startPos = resp.find(Poco::format("CH %d", ind));
//		if (startPos == std::string::npos)
//		{
//			poco_error(logger(), "Not able to find CH "
//					+ Poco::NumberFormatter::format(ind)
//					+ " status response");
//			return;
//		}
//
//		size_t endPos = resp.find('\n', startPos); // termination is "\n\r"
//
//		std::string chanSt(resp, startPos, (endPos == std::string::npos) ? endPos : endPos - startPos);
//
//		poco_information(logger(), "Channel #" + Poco::NumberFormatter::format(ind)
//			+ " status: ");
//
//		startPos = chanSt.find("MD");
//		endPos = chanSt.find(",", startPos);
//		switch (Poco::NumberParser::parse(chanSt.substr(startPos+3, endPos-startPos-3)))
//		{
//		case 0:
//			poco_information(logger(), " - Continuous mode");
//			break;
//		case 1:
//			poco_information(logger(), " - Pulsed mode");
//			break;
//		case 2:
//			poco_information(logger(), " - Switched mode");
//			break;
//		default:
//			poco_bugcheck_msg("unrecognized mode");
//		}
//
//		startPos = chanSt.find("CS");
//		endPos = chanSt.find(",", startPos);
//		poco_assert(chanSt.at(endPos-1) == 'A'); // assuming ampers
//		nominalCurr[ind-1] = Poco::NumberParser::parseFloat(chanSt.substr(startPos+3, endPos-startPos-3-1));
//
//		poco_information(logger(), " - nominal current: "
//				+ Poco::NumberFormatter::format(nominalCurr[ind-1]) + " A");
//
//		startPos = chanSt.find("SE");
//		endPos = chanSt.find(",", startPos);
//		brightness[ind-1] = static_cast<Poco::Int64>(Poco::NumberParser::parseFloat(chanSt.substr(startPos+3, endPos-startPos-3)));
//
//		poco_information(logger(), " - brightness: " +
//				Poco::NumberFormatter::format(brightness[ind-1]) + "%");
//
//		startPos = chanSt.find("DL");
//		endPos = chanSt.find(",", startPos);
//		poco_assert(chanSt.at(endPos-1) == 's');
//		double multiplier = 1;
//		if (chanSt.at(endPos-2) == 'u')
//			multiplier = 0.001;
//		else
//			poco_assert(chanSt.at(endPos-2) == 'm');
//		delay[ind-1] = multiplier * Poco::NumberParser::parseFloat(chanSt.substr(startPos+3, endPos-startPos-3-2));
//
//		poco_information(logger(), " - delay: "
//				+ Poco::NumberFormatter::format(delay[ind-1]) + " ms");
//
//		startPos = chanSt.find("PU");
//		endPos = chanSt.find(",", startPos);
//		poco_assert(chanSt.at(endPos-1) == 's');
//		multiplier = 1;
//		if (chanSt.at(endPos-2) == 'u')
//			multiplier = 0.001;
//		else
//			poco_assert(chanSt.at(endPos-2) == 'm');
//		duration[ind-1] = multiplier * Poco::NumberParser::parseFloat(chanSt.substr(startPos+3, endPos-startPos-3-2));
//
//		poco_information(logger(), " - duration: "
//				+ Poco::NumberFormatter::format(duration[ind-1]) + " ms");
//	}
//
//	setParameterValue<double>(paramDelay, delay[0], false);
//	setParameterValue<double>(paramDuration, duration[0], false);
//	setParameterValue<double>(paramNominalCurrent, nominalCurr[0], false);
//	setParameterValue<Poco::Int64>(paramBrightness, brightness[0], false);
//
//	// clear needApplication flag
//	double purger;
//	Poco::Int64 purger64;
//	getInternalFloatParameterValue(paramDelay, purger);
//	getInternalFloatParameterValue(paramDuration, purger);
//	getInternalFloatParameterValue(paramNominalCurrent, purger);
//	getInternalIntParameterValue(paramBrightness, purger64);
//}
//
//Poco::Int64 Meca500::getIntParameterValue(size_t paramIndex)
//{
//	Poco::Int64 value;
//	bool needApplication = getInternalIntParameterValue(paramIndex, value);
//
//	if (needApplication)
//	{
//		setParameterValue<Poco::Int64>(paramIndex, value, false);
//		applyParameters();
//		return getIntParameterValue(paramIndex);
//	}
//	else
//		return value;
//}
//
//double Meca500::getFloatParameterValue(size_t paramIndex)
//{
//	double value;
//	bool needApplication = getInternalFloatParameterValue(paramIndex, value);
//
//	if (needApplication)
//	{
//		setParameterValue<double>(paramIndex, value, false);
//		applyParameters();
//		return getFloatParameterValue(paramIndex);
//	}
//	else
//		return value;
//}
//
//int Meca500::checkResponseError(std::string response)
//{
//	size_t pos = response.find("Err");
//
//	if (pos == std::string::npos)
//		return 0;
//	else
//		return response.at(pos+4)-'0';
//}

void Meca500::singleMotion(int axis, double position)
{
	poco_information(logger(), "single move axis: " + axisName(axis));

	double x, y, z, a, b, c;
	getPosition(x, y, z, a, b, c);

	// change position
	switch (axis)
	{
	case xAxis:
		setPosition(x, y, z, a, b, c);
		break;
	case yAxis:
		setPosition(x, y, z, a, b, c);
		break;
	case zAxis:
		setPosition(x, y, z, a, b, c);
		break;
	case aAxis:
		setPosition(x, y, z, a, b, c);
		break;
	case bAxis:
		setPosition(x, y, z, a, b, c);
		break;
	case cAxis:
		setPosition(x, y, z, a, b, c);
		break;
	default:
		poco_bugcheck_msg("impossible axis index");
		throw Poco::BugcheckException();
	}
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

