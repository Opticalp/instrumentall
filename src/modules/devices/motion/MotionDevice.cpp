/**
 * @file	src/modules/devices/motion/MotionDevice.cpp
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

#include "MotionDevice.h"

MotionDevice::MotionDevice(ModuleFactory* parent, std::string customName, int axes):
    	    Module(parent, customName),
			axisIndexCnt(0),
			useExtendedParams(false)
{
	if (axes & xAxis)
	{
		xIndex = axisIndexCnt++;
		axisNames.push_back("xAxis");
		axisMasks.push_back(xAxis);
	}
	else
		xIndex = -1;

	if (axes & yAxis)
	{
		yIndex = axisIndexCnt++;
		axisNames.push_back("yAxis");
		axisMasks.push_back(yAxis);
	}
	else
		yIndex = -1;

	if (axes & zAxis)
	{
		zIndex = axisIndexCnt++;
		axisNames.push_back("zAxis");
		axisMasks.push_back(zAxis);
	}
	else
		zIndex = -1;

	if (axes & aAxis)
	{
		aIndex = axisIndexCnt++;
		axisNames.push_back("aAxis");
		axisMasks.push_back(aAxis);
	}
	else
		aIndex = -1;

	if (axes & bAxis)
	{
		bIndex = axisIndexCnt++;
		axisNames.push_back("bAxis");
		axisMasks.push_back(bAxis);
	}
	else
		bIndex = -1;

	if (axes & cAxis)
	{
		cIndex = axisIndexCnt++;
		axisNames.push_back("cAxis");
		axisMasks.push_back(cAxis);
	}
	else
		cIndex = -1;
}

void MotionDevice::construct(std::string internalName, std::string customName)
{
	setInternalName(internalName);
    setCustomName(customName);
    setLogger("module." + name());

    setInPortCount(axisIndexCnt);
    setParameterCount(axisIndexCnt);
	setOutPortCount(axisIndexCnt);

	for (int ind = 0; ind < axisIndexCnt; ind++)
	{
		addInPort(axisName(ind), axisName(ind) + " position", DataItem::typeFloat, ind);
		addOutPort(axisName(ind), axisName(ind) + " position after movement", DataItem::typeFloat, ind);
		addParameter(ind, axisName(ind) + "Pos", "Get or set " + axisName(ind) + " position", ParamItem::typeFloat);
	}

    defineParameters();

    setParametersDefaultValue();

    notifyCreation();
}

void MotionDevice::setParametersDefaultValue()
{
	ParameterSet pSet;

	getParameterSet(&pSet);

	for (int index=0; index < pSet.size(); index++)
		try
		{
			switch (pSet.at(index).datatype)
			{
			case ParamItem::typeInteger:
				setParameterValue<Poco::Int64>(index, getIntParameterDefaultValue(index), false);
				break;
			case ParamItem::typeFloat:
				setParameterValue<double>(index, getFloatParameterDefaultValue(index), false);
				break;
			case ParamItem::typeString:
				setParameterValue<std::string>(index, getStrParameterDefaultValue(index), false);
				break;
			default:
				poco_bugcheck_msg("unknown parameter type");
			}

			if (index >= axisIndexCnt)
				useExtendedParams = true;
		}
		catch (Poco::NotFoundException&)
		{
			// TODO: change e description in ParameterizedEntity::getParameterDefaultValue
			poco_notice(logger(), "No default value for the parameter: " +
					name() + "." + pSet[index].name);
		}

	applyParameters();
}

void MotionDevice::allMotionSync(std::vector<double> positions)
{
	if (positions.size() != axisIndexCnt)
		poco_bugcheck_msg("incorrect number of axis in position command");

	for (int ind = 0; ind < axisIndexCnt; ind++)
		singleMotion(axisMasks[ind], positions[ind]);
}

void MotionDevice::allMotionSeq(std::vector< std::vector<double> > positionsSeq)
{
	for (int time = 0; time < positionsSeq.size(); time++)
		allMotionSync(positionsSeq[time]);
}

double MotionDevice::getFloatParameterValue(size_t paramIndex)
{
	try
	{
		switch (axisMasks.at(paramIndex))
		{
		case xAxis:
			return 0; //TODO: 
		case yAxis:
			return 0; //TODO: 
		case zAxis:
			return 0; //TODO: 
		case aAxis:
			return 0; //TODO: 
		case bAxis:
			return 0; //TODO: 
		case cAxis:
			return 0; //TODO: 
		default:
			poco_bugcheck_msg("impossible index. axis unkown");
			throw Poco::BugcheckException();
		}
	}
	catch (std::out_of_range&)
	{
		poco_bugcheck_msg("impossible index. axis unkown");
		throw Poco::BugcheckException();
	}
}

void MotionDevice::setFloatParameterValue(size_t paramIndex, double value)
{
	try
	{
		singleMotion(axisMasks.at(paramIndex), value);
	}
	catch (std::out_of_range&)
	{
		poco_bugcheck_msg("impossible index. axis unkown");
	}
}