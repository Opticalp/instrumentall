/**
 * @file	src/modules/devices/motion/Meca500Factory.h
 * @date	Jan. 2019
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2019 Ph. Renaud-Goud

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

#ifndef SRC_MODULES_DEVICES_MOTION_MECA500FACTORY_H_
#define SRC_MODULES_DEVICES_MOTION_MECA500FACTORY_H_

#include "core/ModuleFactoryBranch.h"

#include "tools/comm/net/TcpComm.h"

/**
 * Mecademic Meca500 module factory
 * 
 * Create the control TCP socket
 * Handle the comm
 */
class Meca500Factory: public ModuleFactoryBranch
{
public:
	/**
	 * Constructor
	 *
	 * @throw Poco::IOException if the communication can not be established
	 */
	Meca500Factory(ModuleFactory* parent, std::string selector);

	~Meca500Factory() 
	{
		comm.close();
	}

    std::string name() { return "Meca500Factory"; }

    std::string description() 
	{ 
		return "Factory that builds a Meca500 module \n"
			"for the Mecademic meca500 robot arm \n"
			"on comm: " + comm.deviceName();
	}

    size_t countRemain();

private:
	/**
	 * Retrieve the first response after connection
	 * 
	 * and check that it fits with 
	 *     
	 *     "[3000][Connected to Meca500 3_7.0.6]\0"
	 *
	 * @throw Poco::IOException if not OK
	 */
	void checkFirstResponse();

	/**
	 * Check if the robot is activated and homed
	 */
	bool isAvailable();

	Module* newChildModule(std::string customName);

	TcpComm comm;
};

#endif /* SRC_MODULES_DEVICES_MOTION_MECA500FACTORY_H_ */
