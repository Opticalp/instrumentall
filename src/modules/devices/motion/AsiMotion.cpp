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

AsiMotion::AsiMotion(ModuleFactory* parent, std::string customName, bool tiger):
    Module(parent, customName),
    tigerType(tiger)
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

    // the output ports are set with the current position when
    // a motion has finished.
    setOutPortCount(outPortCnt);

    addOutPort("X", "x position after movement",
            DataItem::typeDblFloat, outPortX);
    addOutPort("Y", "y position after movement",
            DataItem::typeDblFloat, outPortY);

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramSerialPortIndex,
                    "serialPort",
                    "serial port name, e.g. COM3 for windows or /dev/ttys1 for unix",
                    ParamItem::typeString, "");

    std::string portName = getStrParameterDefaultValue(paramSerialPortIndex);

    if (portName.size())
        setStrParameterValue(paramSerialPortIndex, portName);

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
		outAttrs[inPortX] = attrs[inPortX];
		outAttrs[inPortY] = attrs[inPortY];
    	move();
    	releaseAllInPorts(); // release when the movement has stopped
    	break;
    case noDataStartState:
    	outAttrs[inPortY] = outAttrs[inPortX];
    	break;
    default:
    	poco_bugcheck_msg("impossible start condition");
    	throw Poco::BugcheckException();
    }

    double* outData[outPortCnt];
    getDataToWrite<double>(outPortX, outData[outPortX]);
    getDataToWrite<double>(outPortY, outData[outPortY]);

    whereXY(*outData[outPortX], *outData[outPortY]);

    notifyOutPortReady(outPortX, outAttrs[inPortX]);
    notifyOutPortReady(outPortY, outAttrs[inPortY]);
}

void AsiMotion::move()
{
    double* pData[inPortCnt];

    readInPortData(inPortX, *(pData + inPortX));
    readInPortData(inPortY, *(pData + inPortY));

    waitMoveStop();

	goXY(*pData[inPortX], *pData[inPortY]);

	releaseAllInPorts();
}

std::string AsiMotion::getStrParameterValue(size_t paramIndex)
{
    // get the serial port name from the serial port object
    return serial.deviceName();
}

void AsiMotion::setStrParameterValue(size_t paramIndex, std::string value)
{
    serial.close();
    serial.open(value);

    // ASI Motion default settings
    serial.setPortSettings(
            (tigerType?115200:9600), // baudrate
            'n',  // parity
            8,    // word size
            1,    // stop bits
            2048 ); // buffer size
    serial.setDelimiter('\r'); // carriage return

    poco_information(logger(),"port "+ serial.deviceName() + " configured");

    info();

	setRunningTask(NULL);
	sendXY();
}

void AsiMotion::info()
{
    std::string query;
    std::vector<std::string> answer;

    poco_information(logger(), "Querying ASI motion build info (BU X)...");
    query = "BU X";
    serial.write(query);
    readUntilCRLF();

    poco_information(logger(), "Querying ASI motion X axis info (INFO X)...");
    query = "INFO X";
    serial.write(query);
    readUntilCRLF();

    poco_information(logger(), "Querying ASI motion Y axis info (INFO Y)...");
    query = "INFO Y";
    serial.write(query);
    readUntilCRLF();

    poco_information(logger(),"done.");
}

#define COUNT_LIMIT 300

std::string AsiMotion::readUntilCRLF()
{
    std::string response;
    bool end = false;

    poco_information(logger(), ">>>>>");

    for (size_t count = 0; count < COUNT_LIMIT; count++)
    {
        std::string resp;
        resp = serial.read(32);

        if (resp.size() == 0)
            continue;

        //if (count)
        //    poco_information(logger(), "--------");

        //poco_information(logger(), serial.niceString(resp));

        response += resp;

        if (*(response.rbegin()) == '\n')
        {
            if (*(response.rbegin()+1) == '\r')
            {
				poco_information(logger(), serial.niceString(response));
                poco_information(logger(), "<<<<<");
                return response;
            }
            else
            {
                poco_information(logger(), "\\n alone?   <=======");
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

void AsiMotion::goXY(double posX, double posY)
{
	Poco::FastMutex::ScopedLock lock(mutex);

	poco_information(logger(), "Go to ( "
            + Poco::NumberFormatter::format(posX) + " ; "
            + Poco::NumberFormatter::format(posY) + " )");

    serial.checkOpen();

    std::string query("MOVE X=");
    query += Poco::NumberFormatter::format(posX*10);
    query += " Y=";
    query += Poco::NumberFormatter::format(posY*10);

    serial.write(query);

    std::string answer(readUntilCRLF());

    int err = parseErrorAnswer(answer);
    switch (err)
    {
    case 0: // no error
        return;
    case -4:
        throw Poco::RangeException("goXY", "Out of range position parameter");
    default:
        throw Poco::RuntimeException("goXY",
                Poco::NumberFormatter::format(err)
                + ": unknown error");
    }
}

void AsiMotion::whereXY(double& posX, double& posY)
{
    poco_information(logger(), "Retrieve XY position");

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
}

void AsiMotion::sendXY()
{
    double* outData[outPortCnt];
    DataAttributeOut attr;

    reserveAllOutPorts();

    getDataToWrite<double>(outPortX, outData[outPortX]);
    getDataToWrite<double>(outPortY, outData[outPortY]);

    whereXY(*outData[outPortX], *outData[outPortY]);

    notifyAllOutPortReady(attr);
}
