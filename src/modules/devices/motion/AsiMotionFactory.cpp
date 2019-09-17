/**
 * @file	src/modules/devices/motion/AsiMotionFactory.cpp
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

#include "AsiMotionFactory.h"

#include "AsiMotion.h"

AsiMotionFactory::AsiMotionFactory(ModuleFactory* parent, std::string selector):
    ModuleFactoryBranch(parent, selector),
    tiger(false)
{
    setLogger(name());

    if (selector.compare("AsiTiger") == 0)
        tiger = true;
}

size_t AsiMotionFactory::countRemain()
{
	if (getChildModules().size())
		return 0;

	std::string port = reinterpret_cast<ModuleFactoryBranch*>(parent())->getSelector();

	if (!serial.isOpen())
	{
		try
		{
			poco_information(logger(), "Opening port " + port + "... ");
			serial.open(port);
			poco_information(logger(), "Port " + port + " opened. ");

			// ASI Motion default settings
			serial.setPortSettings(
				(tiger ? 115200 : 9600), // baudrate
				'n',  // parity
				8,    // word size
				1,    // stop bits
				2048); // buffer size
			serial.setDelimiter('\r'); // carriage return
			poco_information(logger(), "Port settings donne. ");

			poco_information(logger(), "port " + serial.deviceName() + " configured");
		}
		catch (Poco::Exception& e)
		{
			poco_error(logger(), "Port " + port + " can not be opened: " +
				e.displayText());
			return 0;
		}
		catch (...)
		{
			poco_error(logger(), "Port " + port + " opening failed on unknown error. ");
			return 0;
		}
	}

    try
    {
        AsiMotion::info(serial, logger());
    }
    catch (Poco::Exception& e)
    {
        poco_error(logger(), "No " + getSelector() + " stage found on port "
						+ port +
                        + " with standard baudrate configuration. \n"
						+ e.displayText());
        serial.close();
        return 0;
    }
	catch (...)
	{
		poco_error(logger(), "No " + getSelector() + " stage found on port "
			+ port +
			+ " with standard baudrate configuration. \n"
			"Unknown error. ");
		serial.close();
		return 0;
	}

    return 1;
}

Module* AsiMotionFactory::newChildModule(std::string customName)
{
    return new AsiMotion(this, customName, serial);
}