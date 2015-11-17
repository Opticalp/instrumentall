/**
 * @file	src/MainApplication.h
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

#ifndef SRC_MAINAPPLICATION_H_
#define SRC_MAINAPPLICATION_H_

#include "Poco/Util/Application.h"
#include "Poco/Util/OptionSet.h"

#include "Dependency.h"

/**
 * MainApplication
 *
 * This class is the nutshell of all of the instrumentall application.
 * It uses some of the features of the Util::Application class,
 * such as command line arguments processing, logging, etc.
 *
 * Try instrumentall --help (on Unix platforms)
 * or instrumentall /help (on windows platforms)
 * for more information.
 */
class MainApplication: public Poco::Util::Application
{
public:
    /**
     * Default constructor
     *
     *  - Add new subsystems
     *  - TODO: list dependencies
     */
    MainApplication();

protected:
    /**
     * Initializes the application and all registered subsystems.
     *
     * Subsystems are always initialized in the exact same order
     * in which they have been registered.
     *
     * @note Must call the base class implementation.
     */
    void initialize(Application& self);

    /**
     * Uninitializes the application and all registered subsystems.
     *
     * Subsystems are always uninitialized in reverse order in which
     * they have been initialized.
     *
     * @note Must call the base class implementation.
     */
    void uninitialize();

    /**
     * Used to support command line arguments.
     *
     * Called before command line processing begins.
     * Call defineOptions() on all registered subsystems.
     *
     * @note Should call the base class implementation.
     */
    void defineOptions(Poco::Util::OptionSet& options);

    /**
     * Handle custom config file at a non-standard path
     */
    void handleConfig(const std::string& name, const std::string& value);

    /**
     * Callback function to handle the use of 'h' or 'help' command line option
     */
    void handleHelp(const std::string& name, const std::string& value);

    /**
     * Display the help message
     *
     * Command name, options, usage...
     */
    void displayHelp();

    // void displayAbout();

    /**
     * Get general information about instrumentall
     */
    std::string about();

    /**
     * Get current instrumentall version
     *
     * TODO: dynamic semantic version using cmake, git and CI
     */
    std::string version();

    /**
     * Get POCO (external dependency) version
     *
     * TODO: get it out of this class
     */
    std::string pocoVersion();

    /**
     * The application's main logic.
     *
     * Unprocessed command line arguments are passed in args.
     */
    int main(const std::vector<std::string>& args);

    /**
     * Recursive display of all the application properties
     */
    void printProperties(const std::string& base);

private:
    bool _helpRequested; ///< flag to stop processing if help is requested
    std::vector<Dependency*> deps; ///< dependencies descriptors
};

#endif /* SRC_MAINAPPLICATION_H_ */
