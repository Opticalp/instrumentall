/**
 * @file	src/modules/UI/GuiProperties.h
 * @date	Feb. 2017
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

#ifndef SRC_MODULES_UI_GUIPROPERTIES_H_
#define SRC_MODULES_UI_GUIPROPERTIES_H_

#ifdef HAVE_WXWIDGETS

#include "core/Module.h"

class GuiProcessingUnit;

/**
 * GuiProperties
 * 
 * Module to change data of the GUI via the workflow.
 *
 * @note Possible improvement: implement applyParameters to update
 * the GUI only once, even when multiple values changed.
 */
class GuiProperties: public Module
{
public:
    GuiProperties(ModuleFactory* parent, std::string customName);

    std::string description()
    {
        return "Interfaces GUI data fields. ";
    }

private:
    enum params
    {
        paramStatusBar0,
//        paramStatusBar1,
        paramTextCtrl,
        paramCnt
    };

//    Poco::Int64 getIntParameterValue(size_t paramIndex);
//    double getFloatParameterValue(size_t paramIndex);

    std::string getStrParameterValue(size_t paramIndex);

//    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);
//    void setFloatParameterValue(size_t paramIndex, double value);

    void setStrParameterValue(size_t paramIndex, std::string value);

    GuiProcessingUnit* guiProc;
};

#endif /* HAVE_WXWIDGETS */
#endif /* SRC_MODULES_UI_GUIPROPERTIES_H_ */
