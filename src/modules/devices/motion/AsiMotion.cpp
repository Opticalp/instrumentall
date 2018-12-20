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

size_t AsiMotion::refCount = 0;

AsiMotion::AsiMotion(ModuleFactory* parent, std::string customName, SerialCom& commObj):
    Module(parent, customName),
    serial(commObj)
{
    setInternalName("AsiMotion" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // ports
    setInPortCount(inPortCnt);

    addInPort("X", "x position",
            DataItem::typeDblFloat, inPortX);
    addInPort("Y", "y position",
            DataItem::typeDblFloat, inPortY);
    addInPort("Z", "z position",
            DataItem::typeDblFloat, inPortZ);

    // the output ports are set with the current position when
    // a motion has finished.
    setOutPortCount(outPortCnt);

    addOutPort("X", "x position after movement",
            DataItem::typeDblFloat, outPortX);
    addOutPort("Y", "y position after movement",
            DataItem::typeDblFloat, outPortY);
    addOutPort("Z", "z position after movement",
            DataItem::typeDblFloat, outPortZ);

//    // parameters
//    setParameterCount(paramCnt);

    notifyCreation();

    // if nothing failed
    refCount++;
}

AsiMotion::~AsiMotion()
{
    // the serial port closing is directly done by the SerialCom object destruction
}

void AsiMotion::process(int startCond)
{
	poco_information(logger(), "serial port opened;");

    serial.checkOpen();

    // prevent a target module from using expired position data
    reserveAllOutPorts();

    DataAttributeIn attrs[inPortCnt];
	DataAttributeOut outAttrs[inPortCnt];

    switch (startCond)
    {
    case allDataStartState:
		readLockInPort(inPortX);
    	readInPortDataAttribute(inPortX, attrs + inPortX);
		readLockInPort(inPortY);
    	readInPortDataAttribute(inPortY, attrs + inPortY);
		readLockInPort(inPortZ);
    	readInPortDataAttribute(inPortZ, attrs + inPortZ);
		outAttrs[inPortX] = attrs[inPortX];
		outAttrs[inPortY] = attrs[inPortY];
		outAttrs[inPortZ] = attrs[inPortZ];
    	move();
    	releaseAllInPorts(); // release when the movement has stopped
    	break;
    case noDataStartState:
    	outAttrs[inPortY] = outAttrs[inPortX];
    	outAttrs[inPortZ] = outAttrs[inPortX];
    	break;
    default:
    	poco_bugcheck_msg("impossible start condition");
    	throw Poco::BugcheckException();
    }

    double* outData[outPortCnt];
    getDataToWrite<double>(outPortX, outData[outPortX]);
    getDataToWrite<double>(outPortY, outData[outPortY]);
    getDataToWrite<double>(outPortZ, outData[outPortZ]);

    whereXYZ(*outData[outPortX], *outData[outPortY], *outData[outPortZ]);

    notifyOutPortReady(outPortX, outAttrs[inPortX]);
    notifyOutPortReady(outPortY, outAttrs[inPortY]);
    notifyOutPortReady(outPortZ, outAttrs[inPortZ]);
}

void AsiMotion::move()
{
    double* pData[inPortCnt];

    readInPortData(inPortX, *(pData + inPortX));
    readInPortData(inPortY, *(pData + inPortY));
    readInPortData(inPortZ, *(pData + inPortZ));

    waitMoveStop();

	goXYZ(*pData[inPortX], *pData[inPortY], *pData[inPortZ]);

	releaseAllInPorts();
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
	Poco::FastMutex::ScopedLock lock(mutex);

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

void AsiMotion::goXYZ(double posX, double posY, double posZ)
{
	Poco::FastMutex::ScopedLock lock(mutex);

	poco_information(logger(), "Go to ( "
            + Poco::NumberFormatter::format(posX) + " ; "
            + Poco::NumberFormatter::format(posY) + " ; "
            + Poco::NumberFormatter::format(posZ) + " )");

    serial.checkOpen();

    std::string query("MOVE X=");
    query += Poco::NumberFormatter::format(posX*10);
    query += " Y=";
    query += Poco::NumberFormatter::format(posY*10);
    query += " Z=";
    query += Poco::NumberFormatter::format(posZ*10);

    serial.write(query);

    std::string answer(readUntilCRLF());

    int err = parseErrorAnswer(answer);
    switch (err)
    {
    case 0: // no error
        return;
    case -4:
        throw Poco::RangeException("goXYZ", "Out of range position parameter");
    default:
        throw Poco::RuntimeException("goXYZ",
                Poco::NumberFormatter::format(err)
                + ": unknown error");
    }
}

void AsiMotion::whereXYZ(double& posX, double& posY, double& posZ)
{
    poco_information(logger(), "Retrieve XYZ position");

    serial.checkOpen();

    waitMoveStop();

	Poco::FastMutex::ScopedLock lock(mutex);

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

void AsiMotion::sendXYZ()
{
    double* outData[outPortCnt];
    DataAttributeOut attr;

    reserveAllOutPorts();

    getDataToWrite<double>(outPortX, outData[outPortX]);
    getDataToWrite<double>(outPortY, outData[outPortY]);
    getDataToWrite<double>(outPortZ, outData[outPortZ]);

    whereXYZ(*outData[outPortX], *outData[outPortY], *outData[outPortZ]);

    notifyAllOutPortReady(attr);
}
