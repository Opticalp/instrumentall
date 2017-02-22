/**
 * @file    src/UI/GuiManager.h
 * @date    Apr. 2014
 * @author  PhRG - opticalp.fr
 */

/*
Copyright (c) 2014 Ph. Renaud-Goud / Opticalp

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

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#ifdef HAVE_WXWIDGETS

#include "core/VerboseEntity.h"

#include "Poco/Util/Subsystem.h"
#include "Poco/Util/Application.h"
#include "Poco/Logger.h"
#include "Poco/Path.h"
#include "Poco/File.h"


class GuiProcessingUnit;

/**
 * GuiManager
 *
 * GuiManager is the graphical interface manager.
 * It derives from Poco::Util::Subsystem to possibly
 * handle its own command line options.
 *
 * The GUI is designed with wxWidgets
 */
class GuiManager : public Poco::Util::Subsystem, VerboseEntity
{
public:
    /// Default constructor
    GuiManager();
    /// Default destructor
    ~GuiManager();

    /// subsystem name
    const char * name() const { return "GuiManager"; }

    /// @name un/re/initialization methods
    ///@{
    void initialize(Poco::Util::Application& app);
    // void reinitialize(Application & app); // not needed? by default: uninit, then init.
    void uninitialize();
    ///@}

    /// subsystem main logic.
    /// @return exit code from Poco::Util::Application::ExitCode enum
    int main(Poco::Util::Application& app);

    /// Called before the Application's command line processing begins.
    /// enable to support command line arguments
    void defineOptions(Poco::Util::OptionSet & options);

    /// check if the GUI is loaded
    bool isGuiRunning() { return guiRun; }

    /// get the script. get an empty path if no script was given by the user.
    Poco::Path getScript() { return scriptPath; }

    /**
     * @return GuiProcessingUnit if the GUI was initialized, else: NULL
     */
    GuiProcessingUnit* getGuiProcUnit();

    bool autostart() { return autostartFlag; }

private:
    /// indicator to tell if the GUI is launched or not
    bool guiRun;

    bool autostartFlag;

    /**
     * Callback function to handle the use of 'a' or 'autostart' command line option
     */
    void handleAutostart(const std::string& name, const std::string& value);

    Poco::Path scriptPath;
};

#endif /* HAVE_WXWIDGETS */
#endif /* GUIMANAGER_H */
