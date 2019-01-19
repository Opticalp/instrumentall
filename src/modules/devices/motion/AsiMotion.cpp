/**
 * @file	src/modules/devices/AsiMotion.cpp
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

#include "AsiMotion.h"

#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/String.h" // trim in place

AsiMotion::AsiMotion(ModuleFactory* parent, std::string customName, SerialCom& commObj):
    serial(commObj),
   	MotionDevice(parent, customName,
			xAxis | yAxis | zAxis)
{
    setLogger("module.AsiMotion.startup");

    waitMoveStop(); // purge input

    construct("AsiMotion_" + serial.deviceName(), customName);
}

AsiMotion::~AsiMotion()
{
    // the serial port closing is directly done by the SerialCom object destruction
}

void AsiMotion::info(SerialCom &commObj, Poco::Logger& tmpLog)
{
    std::string query;
    std::vector<std::string> answer;

    poco_information(tmpLog, "Querying ASI motion build info (BU X)...");
    query = "BU X";
    commObj.write(query);
    readUntilCRLF(commObj, tmpLog);

    poco_information(tmpLog, "Querying ASI motion X axis info (INFO X)...");
    query = "INFO X";
    commObj.write(query);
    readUntilCRLF(commObj, tmpLog);

    poco_information(tmpLog, "Querying ASI motion Y axis info (INFO Y)...");
    query = "INFO Y";
    commObj.write(query);
    readUntilCRLF(commObj, tmpLog);

    poco_information(tmpLog, "Querying ASI motion Z axis info (INFO Z)...");
    query = "INFO Z";
    commObj.write(query);
    readUntilCRLF(commObj, tmpLog);

    poco_information(tmpLog,"done.");
}

#define COUNT_LIMIT 300

std::string AsiMotion::readUntilCRLF(SerialCom &commObj, Poco::Logger& tmpLog)
{
    std::string response;
    bool end = false;

    poco_information(tmpLog, ">>>>>");

    for (size_t count = 0; count < COUNT_LIMIT; count++)
    {
        std::string resp;
        resp = commObj.read(32);

        if (resp.size() == 0)
            continue;

        //if (count)
        //    poco_information(tmpLog, "--------");

        //poco_information(tmpLog, commObj.niceString(resp));

        response += resp;

        if (*(response.rbegin()) == '\n')
        {
            if (*(response.rbegin()+1) == '\r')
            {
				poco_information(tmpLog, commObj.niceString(response));
                poco_information(tmpLog, "<<<<<");
                return response;
            }
            else
            {
                poco_information(tmpLog, "\\n alone?   <=======");
            }
        }
    }

    throw Poco::TimeoutException("readUntilCRLF");
}

double AsiMotion::parseSimpleAnswer(const std::string& answer)
{
    std::string pattern(":A ");

    size_t pos = answer.find(pattern);

    if (pos == std::string::npos)
        throw Poco::IOException("parseSimpleAnswer",
                "no matching answer found");

    std::string resultStr = answer.substr(pos + pattern.size());

    // remove trailing spaces
    Poco::trimRightInPlace(resultStr);

    return Poco::NumberParser::parseFloat(resultStr);
}

int AsiMotion::parseErrorAnswer(const std::string& answer)
{
    std::string pattern(":N");

    size_t pos = answer.find(pattern);

    if (pos == std::string::npos)
        return 0; // no error

    std::string resultStr = answer.substr(pos + pattern.size());

    // remove trailing spaces
    Poco::trimRightInPlace(resultStr);

    return Poco::NumberParser::parse(resultStr);
}

void AsiMotion::waitMoveStop()
{
	std::string query("STATUS");
//    char answer[3];
//    size_t ansSize;

    poco_information(logger(), "Wait until motion stops");

    for (size_t count = 0; count < COUNT_LIMIT; count++)
    {
        poco_information(logger(),"Query state (STATUS)");
        serial.write(query);

        std::string resp;

        // read, until getting a response
        for (size_t count = 0; count < COUNT_LIMIT; count++)
        {
            resp = (serial.read(3));
            if (resp.size())
            {
            	poco_information(logger(), serial.niceString(resp));

            	if (resp[0] == 'N')
            		return;

            	break;
            }
        }

        Poco::Thread::sleep(100);
    }

    throw Poco::TimeoutException("waitMoveStop");
}

void AsiMotion::whereXYZ(double& posX, double& posY, double& posZ)
{
    poco_information(logger(), "Retrieve XYZ position");

    serial.checkOpen();

    waitMoveStop();

	std::string query, answer;

    poco_information(logger(), "Query X position (WHERE X)");
    query = "WHERE X";
    serial.write(query);

    answer = readUntilCRLF();
    poco_information(logger(), serial.niceString(answer));
    posX = parseSimpleAnswer(answer)/10;

    poco_information(logger(), "Query Y position (WHERE Y)");
    query = "WHERE Y";
    serial.write(query);

    answer = readUntilCRLF();
    poco_information(logger(), serial.niceString(answer));
    posY = parseSimpleAnswer(answer)/10;

    poco_information(logger(), "Query Z position (WHERE Z)");
    query = "WHERE Z";
    serial.write(query);

    answer = readUntilCRLF();
    poco_information(logger(), serial.niceString(answer));
    posZ = parseSimpleAnswer(answer)/10;
}

void AsiMotion::defineParameters()
{
	poco_information(logger(), "define device-specific parameters");

	setParameterCount(totalParamCnt);

	addParameter(paramQuery, "query", "direct query to the robot", ParamItem::typeString);
}

double AsiMotion::getFloatParameterValue(size_t paramIndex)
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

void AsiMotion::setFloatParameterValue(size_t paramIndex, double value)
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

std::string AsiMotion::getStrParameterValue(size_t paramIndex)
{
	switch (paramIndex)
	{
	case paramQuery:
        try
        {
            return readUntilCRLF();
        }
        catch (Poco::TimeoutException&)
        {
            return "";
        }
	default:
		poco_bugcheck_msg("invalid parameter index");
		throw Poco::BugcheckException();
	}
}

void AsiMotion::setStrParameterValue(size_t paramIndex, std::string value)
{
	if (paramIndex != paramQuery)
		poco_bugcheck_msg("invalid parameter index");

	serial.write(parseMultipleQueries(value));
}

void AsiMotion::singleMotion(int axis, std::vector<double> positions)
{
    std::string query("MOVE");
    
    if (axis & xAxis)
        query += " X=" + Poco::NumberFormatter::format(positions.at(xIndex)*10000);
    if (axis & yAxis)
        query += " Y=" + Poco::NumberFormatter::format(positions.at(yIndex)*10000);
    if (axis & zAxis)
        query += " Z=" + Poco::NumberFormatter::format(positions.at(zIndex)*10000);

    serial.write(query);

    std::string answer(readUntilCRLF());

    int err = parseErrorAnswer(answer);
    switch (err)
    {
    case 0: // no error
        return;
    case -4:
        throw Poco::RangeException("singeMotion", "Out of range position parameter");
    default:
        throw Poco::RuntimeException("singleMotion",
                Poco::NumberFormatter::format(err)
                + ": unknown error");
    }
    
    waitMoveStop();
}

double AsiMotion::getPosition(int axis)
{
    waitMoveStop();

	std::string query, answer;
    
    switch (axis)
    {
    case xAxis:
        poco_information(logger(), "Query X position (WHERE X)");
        query = "WHERE X";
        serial.write(query);
        break;
    case yAxis:
        poco_information(logger(), "Query Y position (WHERE Y)");
        query = "WHERE Y";
        serial.write(query);
        break;
    case zAxis:
        poco_information(logger(), "Query Z position (WHERE Z)");
        query = "WHERE Z";
        serial.write(query);
        break;
    default:
        poco_bugcheck_msg("getPosition: impossible axis mask");
    }

    answer = readUntilCRLF();
    poco_information(logger(), serial.niceString(answer));
    return parseSimpleAnswer(answer)/10000;
}

#include "Poco/RegularExpression.h"
using Poco::RegularExpression;

std::string AsiMotion::parseMultipleQueries(std::string queries)
{
    RegularExpression regexp("[\\n\\r\\t\\;]+");
    regexp.subst(queries, std::string("\0",1), RegularExpression::RE_GLOBAL);

    return queries;
}
