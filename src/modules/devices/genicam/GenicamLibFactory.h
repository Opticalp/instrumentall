/**
 * @file	src/modules/devices/genicam/GenicamLibFactory.h
 * @date	Jan. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULES_DEVICES_GENICAMLIBFACTORY_H_
#define SRC_MODULES_DEVICES_GENICAMLIBFACTORY_H_

#include "core/ModuleFactoryBranch.h"

#include "GenTLLib.h"


/**
 * GenicamLibFactory
 *
 * Select chain:
 *  - genTL.cti selection in GenicamRootFactory -- DONE --
 *  - interface selection in GenicamLibFactory
 *  - device selection in GenicamIfaceFactory
 *  - conf file selection in GenicamDeviceFactory
 *  - GenicamConfDeviceFactory creates the device
 */
class GenicamLibFactory: public ModuleFactoryBranch
{
public:
    /**
     * Constructor
     *
     * Load lib functions, load genTL system.
     * Launch interfaces discovery using deviceDiscover()
     */
    GenicamLibFactory(ModuleFactory* parent, std::string selector);

    std::string name() { return "GenicamLibFactory"; }
    std::string description()
        { return "Factory to create GenICam interface factory, given a .cti lib. "; }

    std::string selectDescription()
        { return "Select the genTL interface "
                "to be used to look for devices"; }

    /**
     * Find the available genicam interfaces and expose it
     */
    std::vector<std::string> selectValueList();

private:
    GenicamLibFactory();
	~GenicamLibFactory();

    ModuleFactoryBranch* newChildFactory(std::string selector);

    /**
     * Discover the interfaces
     */
    void discover();

    /**
     * Retrieve extended info about the current GenTL
     *
     * No other initialization than TLOpen is required.
     */
    void genTLExInfo();

    /**
     * Elementary info for genTLExInfo
     *
     * Should not throw exception: treated internally and
     * displayed using poco_warning
     */
    void genTLExInfoElem( std::string title,
        GenTL::TL_INFO_CMD infoCmd,
        char* pBuffer, size_t bufferSize);

    /**
     * Retrieve extended info about a GenTL interface
     */
    void genTLIfaceExInfo(char* pIfaceID);

    /**
     * Elementary info for genTLIfaceExInfo
     */
    void genTLIfaceExInfoElem( std::string title, char* pIfaceID,
        GenTL::INTERFACE_INFO_CMD infoCmd,
        char* pBuffer, size_t bufferSize);

    GenTLLib* mGenTL; ///< Interface to the shared lib
    std::vector<std::string> interfaces; ///< available interfaces
};

#endif /* SRC_MODULES_DEVICES_GENICAMLIBFACTORY_H_ */
