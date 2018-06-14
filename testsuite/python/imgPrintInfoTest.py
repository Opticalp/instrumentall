# -*- coding: utf-8 -*-

## @file   testsuite/python/imgPrintInfoTest.py
## @date   jun 2018
## @author PhRG - opticalp.fr
##
## Test the display of informations on an image

#
# Copyright (c) 2018 Ph. Renaud-Goud / Opticalp
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

def myMain(baseDir):
    """Main function. Run the tests. """

    from os.path import join
    import time

    print("Test the basic features of the cameraFromFiles module. ")
    
    from instru import *
    import instruTools
    
    fac = Factory("DeviceFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from CameraFromFilesFactory")
    try:
        cam = fac.select("camera").select("fromFiles").create("fakeCam")
    except RuntimeError as e:
        print("Runtime error: {0}".format(e.message))
        print("OpenCV is probably not present. Exiting. ")
        exit(0)
        
    print("module " + cam.name + " created (" + cam.internalName + ") ")

    imgDir = join(baseDir,"resources")
    print("Set image file directory to " + imgDir)
    cam.setParameterValue("directory", imgDir)
    print("Set file to be read: 001.png")
    cam.setParameterValue("files", "001.png")

    print('Logger creation using the constructor: DataLogger("ShowImageLogger")')
    logger = DataLogger("ShowImageLogger") 
    print("Logger description: " + logger.description)
    logger.setName("imgShow")

    print("Create the source for the value to be displayed")
    print("Create a module from the floatDataGen factory")
    floatGen = Factory("DataGenFactory").select("float").create("floatGen")
    print("Create a module from the intDataGen factory")
    intGen = Factory("DataGenFactory").select("int64").create("intGen")

    print("Create imgPrintInfo module")
    printInfo = Factory("ImageProcFactory").select("modify").select("printInfo").create("printInfo")

    print("set title: test")
    printInfo.setParameterValue("title", "test")
    instruTools.printModuleDetails(printInfo)

    print("set color to black (0)")
    printInfo.setParameterValue("level", 0)

    print("Bind the image source to imgPrintInfo")
    bind(cam.outPort("image"), printInfo.inPort("image"))
    print("Bind the output image to the image shower")
    bind(printInfo.outPort("image"), DataTarget(logger))

    print("run cam")
    runModule(cam)
    time.sleep(1)

    print("Firstly bind the int source to printInfo")
    bind(intGen.outPort("data"), printInfo.inPort("integer"))

    print("set intGen value to 121")
    intGen.setParameterValue("value", 121)
    print("set color to gray (50%)")
    printInfo.setParameterValue("level", 128)

    print("run both source modules: img and int")
    runModule(cam)
    runModule(intGen)

    time.sleep(1)

    print("change color mode to blue")
    printInfo.setParameterValue("colorMode","blue")
    printInfo.setParameterValue("level", 255)

    print("unbind intGen, bind floatGen")
    unbind(printInfo.inPort("integer"))

    bind(floatGen.outPort("data"), printInfo.inPort("float"))
    floatGen.setParameterValue("value", 3.14)
    
    print("run both source modules: img and float")
    runModule(cam)
    runModule(floatGen)

    time.sleep(4)

    print("End of script imgPrintInfoTest.py")
    
# main body    
import sys
import os
from os.path import dirname
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print("current script: ",os.path.realpath(__file__))
        
        baseDir = dirname(dirname(__file__))
        
        myMain(baseDir)
        exit(0)

print("Presumably not called from InstrumentAll >> Exiting...")

exit("This script has to be launched from inside InstrumentAll")
