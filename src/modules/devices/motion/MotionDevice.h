/**
 * @file	src/modules/devices/motion/MotionDevice.h
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

#ifndef SRC_MODULES_DEVICES_MOTION_MOTIONDEVICE_H_
#define SRC_MODULES_DEVICES_MOTION_MOTIONDEVICE_H_

#include "core/Module.h"

/**
 * MotionDevice
 *
 * Common ancestor to motion devices
 *
 * Build input ports, output ports, and compatibility parameters
 */
class MotionDevice: public Module
{
public:
	/**
	 * Constructor
	 *
	 * The derived class constructor shall call construct()
	 */
	MotionDevice(ModuleFactory* parent, std::string customName,
			int axes);

protected:
	/**
	 * Achieve the construction steps.
	 *
	 * To be called from the derived class constructor.
     *
	 *  - define the input and output ports
     *  - define the compatibility parameters: pos for each axis
	 */
	void construct(std::string internalName, std::string customName);

    void process();

    enum axisMask ///< axes to be used as masks
	{
		xAxis = 001,
		yAxis = 002,
		zAxis = 004,
		aAxis = 010,
		bAxis = 020,
		cAxis = 040,
	};

    /// Indexes of the axis ports and parameters (-1 if not used)
	int xIndex, yIndex, zIndex, aIndex, bIndex, cIndex, axisIndexCnt;

    /**
     * Define the additional parameters (e.g. channel specific parameters)
     *
     * Resize the parameter set from paramCompatCnt to new size
     */
    virtual void defineParameters() { }

    bool useExtendedParams; ///< flag used to know if the extended parameters are used... or just the compat param

	std::string axisName(int index) { return axisNames[index]; }

	/// The derivated class should call this function at the end of this overloaded method
	virtual double getFloatParameterValue(size_t paramIndex);

	/// The derivated class should call this function at the end of this overloaded method
	virtual void setFloatParameterValue(size_t paramIndex, double value);

	virtual void singleMotion(int axis, double position) = 0;
	virtual void allMotionSync(std::vector<double> positions);
	virtual void allMotionSeq(std::vector< std::vector<double> > positionsSeq);

	virtual double getPosition(int axis) = 0;

private:
    /**
     * Set the default values of the parameters (if they have a default value)
     *
     * And then apply.
     */
    void setParametersDefaultValue();

	std::vector< std::string > axisNames; ///< index to axis correspondance
	std::vector<int> axisMasks; ///< retrieve mask from index
};

#endif /* SRC_MODULES_DEVICES_MOTION_MOTIONDEVICE_H_ */
