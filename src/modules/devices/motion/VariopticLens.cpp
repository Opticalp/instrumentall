/**
 * @file	src/modules/devices/VariopticLens.cpp
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

#include "VariopticLens.h"

#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/String.h" // trim in place

#define REG_FOCUS_LSB  0x00
#define REG_FOCUS_MSB  0x01
#define REG_CONTROL    0x02
#define     CTRL_EEPROM     0x01
#define REG_MODE       0x03
#define     MODE_STAND_BY   0x01
#define     MODE_ANALOG     0x02
#define REG_SW_VERSION 0x05
#define REG_FAULT      0x0A
#define     FAULT_D_OVERL   0x01
#define     FAULT_D_NORES   0x02
#define     FAULT_D_OVERT   0x04

VariopticLens::VariopticLens(ModuleFactory* parent, std::string customName, SerialCom& commObj):
    serial(commObj),
   	MotionDevice(parent, customName,
			zAxis)
{
    setLogger("module.VariopticLens.startup");

    // TODO: purge input

	construct("VariopticLens_" + serial.deviceName(), customName);
}

VariopticLens::~VariopticLens()
{
    // the serial port closing is directly done by the SerialCom object destruction
}

void VariopticLens::info(SerialCom &commObj, Poco::Logger& tmpLog)
{
	commObj.write(readRegisterCommand(REG_MODE));
	int ret = parseRead(readCommBuffer(commObj, tmpLog));

	if (ret & MODE_STAND_BY)
		poco_information(tmpLog, "Stand-by mode");
	
	if (ret & MODE_ANALOG)
		poco_information(tmpLog, "Analog mode");
	else
		poco_information(tmpLog, "Register control mode");

	commObj.write(readRegisterCommand(REG_FAULT));
	ret = parseRead(readCommBuffer(commObj, tmpLog));
	
	if (ret)
	{
		if (ret & FAULT_D_OVERL)
			poco_information(tmpLog, "Driver is overloaded");
		if (ret & FAULT_D_NORES)
			poco_information(tmpLog, "Driver is not responding to I2C req");
		if (ret & FAULT_D_OVERT)
			poco_information(tmpLog, "Driver is in thermal shutdown");
	}
	else
		poco_information(tmpLog, "Driver fault register not raised");

	commObj.write(readRegisterCommand(REG_SW_VERSION));
	ret = parseRead(readCommBuffer(commObj, tmpLog));

	poco_information(tmpLog, "Driver software version: " + 
		Poco::NumberFormatter::format(ret));

	poco_information(tmpLog,"done.");
}

std::string VariopticLens::readCommBuffer(SerialCom &commObj, Poco::Logger& tmpLog)
{
    std::string buffer = commObj.read(32);
	poco_information(tmpLog, "[IN]:" + commObj.renderHex(buffer));
	return buffer;
}

void VariopticLens::addMoreParameters()
{
	poco_information(logger(), "define device-specific parameters");

	setParameterCount(totalParamCnt);

	addParameter(paramQuery, "query", 
		"direct query to the lens controler. \n"
		"Convert readable hex string to hex vector", 
		ParamItem::typeString);
}

std::string VariopticLens::getStrParameterValue(size_t paramIndex)
{
	switch (paramIndex)
	{
	case paramQuery:
        try
        {
            return readCommBuffer();
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

void VariopticLens::setStrParameterValue(size_t paramIndex, std::string value)
{
	if (paramIndex != paramQuery)
		poco_bugcheck_msg("invalid parameter index");

	std::string bytes(serial.byteVect(value));
	poco_information(logger(), "[OU]:" + serial.renderHex(bytes));
	serial.write(bytes);
}

void VariopticLens::singleMotion(int axis, std::vector<double> positions)
{
	if (axis != zAxis)
		poco_bugcheck_msg("impossible axis");

	int value = static_cast<int>((positions.at(0) - 24) * 1000);

	if (value < 0)
		throw Poco::RangeException("zAxis", "Value can not be less than 24.0(V)");

	if (value > 0xB3B0)
		throw Poco::RangeException("zAxis", "Value can not exceed 70.0(V)");

	serial.write(writeRegisterCommand(REG_FOCUS_LSB, value & 0xFF));
	checkWriteAck(readCommBuffer(serial, logger()));

	serial.write(writeRegisterCommand(REG_FOCUS_MSB, (value >> 8) & 0xFF));
	checkWriteAck(readCommBuffer(serial, logger()));
}

double VariopticLens::getPosition(int axis)
{
	serial.write(readRegisterCommand(REG_FOCUS_MSB));
	int ret = parseRead(readCommBuffer(serial, logger()));
	ret = ret << 8;
	
	serial.write(readRegisterCommand(REG_FOCUS_LSB));
	ret += parseRead(readCommBuffer(serial, logger()));
	   
	// conversion: see USB-M drivboard user guide
	return 0.001 * ret + 24.0;
}

#define STX  '\x02'
#define WRI  '\x37'
#define READ '\x38'
#define ACK  '\x06'
#define NACK '\x15'

std::string VariopticLens::readRegisterCommand(int reg)
{
	std::string cmd;
	cmd += STX;
	cmd += READ;
	cmd += static_cast<char>(reg); 
	cmd += '\x01'; // number of registers to be read
	cmd += stringSum(cmd);   // CRC
	
	return cmd;
}

std::string VariopticLens::writeRegisterCommand(int reg, int value)
{
	std::string cmd;
	cmd += STX;
	cmd += WRI;
	cmd += static_cast<char>(reg);
	cmd += '\x01'; // number of registers to be read
	cmd += static_cast<char>(value);
	cmd += stringSum(cmd);   // CRC

	return cmd;
}

std::string VariopticLens::writeRegisterCommand(int regStart, std::vector<int> values)
{
	std::string cmd;
	cmd += STX;
	cmd += WRI;
	cmd += static_cast<char>(regStart);
	cmd += static_cast<char>(values.size());

	for (std::vector<int>::iterator it = values.begin(), ite = values.end(); 
		it != ite; it++)
		cmd += static_cast<char>(*it);
	
	cmd += stringSum(cmd);   // CRC

	return cmd;
}

void VariopticLens::checkWriteAck(std::string response)
{
	std::string ackStr;
	ackStr += STX;
	ackStr += WRI;
	ackStr += ACK;
	ackStr += stringSum(ackStr);

	if (response == ackStr)
		return;
	
	ackStr[2] = NACK;
	ackStr.resize(3);
	ackStr += stringSum(ackStr);

	if (response == ackStr)
		throw Poco::IOException("VariopticLens::checkWriteAck", "Write not acknowledged");

	throw Poco::IOException("VariopticLens::checkWriteAck", "unreckognized response");
}

int VariopticLens::parseRead(std::string response)
{
	if (response.size() != 4)
		throw Poco::NotImplementedException("VariopticLens::parseRead", 
			"Not able to read more than 1 reg at a time");

	if (*(response.end()-1) != stringSum(response.substr(0, response.size()-1)))
		throw Poco::IOException("VariopticLens::parseRead",
		"CRC check failed");

	std::string comp;
	comp += STX;
	comp += READ;
	if (comp.compare(response.substr(0,2)))
		throw Poco::IOException("VariopticLens::parseRead",
			"response is not a READ command response");
	
	return response[3];
}

char VariopticLens::stringSum(std::string cmd)
{
	char accu = '\0';
	for (std::string::iterator it = cmd.begin(), ite = cmd.end();
		it != ite; it++)
		accu += *it;

	return accu;
}

