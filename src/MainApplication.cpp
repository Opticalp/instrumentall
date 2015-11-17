/**
 * @file	src/MainApplication.cpp
 * @date	nov. 2015
 * @author	PhRG - opticalp.fr
 *
 */

/*
 Copyright (c) 2015 Ph. Renaud-Goud / Opticalp

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

#include <iostream>

#include "Poco/Util/Option.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/NumberFormatter.h"

#ifdef POCO_VERSION_H
#include "Poco/Version.h"
#endif

#include "MainApplication.h"

using Poco::Util::Application;
using Poco::Util::Option;

MainApplication::MainApplication(): _helpRequested(false)
{
    // Application::instance().addSubsystem(new MySubsystem); //template

    // deps.append(new MyDependency); //template

}

void MainApplication::initialize(Application& self)
{
    // hack:
    // I tried to overload run() to prevent the initialization
    // in case of help requested, but it was not sufficient.
    // something else initializes the subsystems before run()
    // see also: main()
    if (_helpRequested)
        return;

    // load std config file
    if (0==loadConfiguration())
    {
        poco_notice(logger(), "No standard configuration file found");
    }

    Application::initialize(self);
}

void MainApplication::uninitialize()
{
    // uninitialization code comes here
    Application::uninitialize();
}

using Poco::Util::OptionCallback;

void MainApplication::defineOptions(Poco::Util::OptionSet& options)
{
    Application::defineOptions(options); // subsystems defineOptions( )

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<MainApplication>(this, &MainApplication::handleHelp)));

    options.addOption(
        Option("config-file", "f", "load configuration data from FILE")
            .required(false)
            .repeatable(true)
            .argument("FILE")
            .callback(OptionCallback<MainApplication>(this, &MainApplication::handleConfig)));
}

void MainApplication::handleConfig(const std::string& name,
        const std::string& value)
{
    loadConfiguration(value);
}

void MainApplication::handleHelp(const std::string& name,
        const std::string& value)
{
    _helpRequested = true;
    displayHelp();
    stopOptionsProcessing(); // if other callbacks were already launched, it is too late...
}

void MainApplication::displayHelp()
{
    Poco::Util::HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("[options]");
    helpFormatter.setHeader(
        "Hello version " + version() + " - test application");
    helpFormatter.format(std::cout);
}

std::string MainApplication::about()
{
    return std::string("Instrumentall version ") + version() + " - instrumentation application\n"
        "Copyright (c) 2013-2015, Opticalp.fr and contributors\n"
        "MIT License, see http://www.opensource.org/licenses/MIT\n\n"
        "Proudly using: \n"
        " - "
        "Poco libraries ("
        + pocoVersion()
        + ") http://pocoproject.org, SPDX-License-Identifier: BSL-1.0 license" ;
}

std::string MainApplication::version()
{
    return "1.0.0-dev";
}

std::string MainApplication::pocoVersion()
{
    std::string pocoV;
    int patch = (POCO_VERSION >> 8) & 0xFF;
    int minor = (POCO_VERSION >> 16) & 0xFF;
    int major = (POCO_VERSION >> 24) & 0xFF;

    pocoV = Poco::NumberFormatter::format(major) + "."
          + Poco::NumberFormatter::format(minor) + "."
          + Poco::NumberFormatter::format(patch) ;

    switch (POCO_VERSION & 0xF0)
    {
    case 0xA0:
        pocoV = pocoV + "(alpha" + Poco::NumberFormatter::format(POCO_VERSION & 0x0F) + ")";
        break;
    case 0xB0:
        pocoV = pocoV + "(beta" + Poco::NumberFormatter::format(POCO_VERSION & 0x0F) + ")";
        break;
    case 0xD0:
        pocoV = pocoV + "(dev" + Poco::NumberFormatter::format(POCO_VERSION & 0x0F) + ")";
        break;
    case 0x00:
        if ((POCO_VERSION & 0x0F) == 0 )
            pocoV = pocoV + "(stable)";
        // else: unknown
        break;
    default:
        // unknown
        break;
    }

    return pocoV;
}

int MainApplication::main(const std::vector<std::string>& args)
{
    if (_helpRequested)
        return Application::EXIT_OK;

    logger().information("Arguments to main():");
    for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        logger().information(*it);
    }
    logger().information("Application properties:");
    printProperties("");

    logger().information(about());

    return Application::EXIT_OK;
}

using Poco::Util::AbstractConfiguration;

void MainApplication::printProperties(const std::string& base)
{
    AbstractConfiguration::Keys keys;
    config().keys(base, keys);
    if (keys.empty()) // key has no child
    {
        if (config().hasProperty(base))
        {
            std::string msg;
            msg.append(base);
            msg.append(" = ");
            msg.append(config().getString(base));
            poco_information(logger(),msg);
        }
    }
    else // recursion
    {
        for (AbstractConfiguration::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it)
        {
            std::string fullKey = base;
            if (!fullKey.empty()) fullKey += '.';
            fullKey.append(*it);
            printProperties(fullKey);
        }
    }
}
