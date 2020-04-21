/**
 * @file	src/modules/devices/motion/VariopticLensFactory.cpp
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

#include "VariopticLensFactory.h"

#include "VariopticLens.h"

VariopticLensFactory::VariopticLensFactory(ModuleFactory* parent, std::string selector):
    ModuleFactoryBranch(parent, selector)
{
    setLogger(name());
}

size_t VariopticLensFactory::countRemain()
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
				(57600), // baudrate
				'n',  // parity
				8,    // word size
				1,    // stop bits
				2048); // buffer size
			serial.noDelimiter(); // do not use a delimiter
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
        VariopticLens::info(serial, logger());
    }
    catch (Poco::Exception& e)
    {
        poco_error(logger(), "No " + getSelector() + " Varioptic lens found on port "
						+ port +
                        + " with standard baudrate configuration. \n"
						+ e.displayText());
        serial.close();
        return 0;
    }
	catch (...)
	{
		poco_error(logger(), "No " + getSelector() + " Varioptic lens found on port "
			+ port +
			+ " with standard baudrate configuration. \n"
			"Unknown error. ");
		serial.close();
		return 0;
	}

    return 1;
}

Module* VariopticLensFactory::newChildModule(std::string customName)
{
    return new VariopticLens(this, customName, serial);
}
