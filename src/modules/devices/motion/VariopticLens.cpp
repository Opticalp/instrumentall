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

VariopticLens::VariopticLens(ModuleFactory* parent, std::string customName, SerialCom& commObj):
    serial(commObj),
   	MotionDevice(parent, customName,
			zAxis)
{
    setLogger("module.VariopticLens.startup");

    // TODO: purge input

	construct("VariopticLens_" + serial.deviceName(), customName);

	poco_information(logger(), "construction done. ");
}

VariopticLens::~VariopticLens()
{
    // the serial port closing is directly done by the SerialCom object destruction
}

void VariopticLens::info(SerialCom &commObj, Poco::Logger& tmpLog)
{
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

	// TODO: evtl add LUT file? 
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

	serial.write(serial.byteVect(value));
}

void VariopticLens::singleMotion(int axis, std::vector<double> positions)
{
    
}

double VariopticLens::getPosition(int axis)
{
	return 1.0;
}
