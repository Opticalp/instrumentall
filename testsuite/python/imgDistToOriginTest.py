# -*- coding: utf-8 -*-

## @file   testsuite/python/imgDistToOriginTest.py
## @date   aug 2018
## @author PhRG - opticalp.fr
##
## Test the display of decentering informations on an image

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

    print("Test the basic features of the imgPrintInfo module. ")
    
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

    print("Create the sources for the vector to be displayed")
    print("Create 2 modules from the floatDataGen factory")
    deltaX = Factory("DataGenFactory").select("float").create("deltaX")
    deltaY = Factory("DataGenFactory").select("float").create("deltaY")

    print("Create distToOrigin module")
    dist = Factory("ImageProcFactory").select("modify").select("distToOrigin").create("dist")

    print("set color to black (0)")
    dist.setParameterValue("level", 0)

    print("Bind the image source to distToOrigin")
    bind(cam.outPort("image"), dist.inPort("image"))
    print("Bind the output image to the image shower")
    bind(dist.outPort("image"), DataTarget(logger))

    print("Bind the decenter sources to distToOrigin")
    bind(deltaX.outPort("data"), dist.inPort("deltaX"))
    bind(deltaY.outPort("data"), dist.inPort("deltaY"))

    print("set deltaX value to 50")
    deltaX.setParameterValue("value", 50)
    print("set deltaY value to -25")
    deltaY.setParameterValue("value", -25)
    print("set color to gray (50%)")
    dist.setParameterValue("level", 128)

    print("run all source modules: img and deltas")
    runModule(cam)
    runModule(deltaX)
    runModule(deltaY)

    time.sleep(1)

    print("change color mode to blue")
    dist.setParameterValue("colorMode","blue")
    dist.setParameterValue("level", 255)

    print("change amplitude ratio to -1")
    dist.setParameterValue("ratio", -1)

    print("run all source modules (2): img and deltas")
    runModule(cam)
    runModule(deltaX)
    runModule(deltaY)

    time.sleep(1)

    print("change amplitude ratio to 2")
    dist.setParameterValue("ratio", 2)

    print("run all source modules (2): img and deltas")
    runModule(cam)
    runModule(deltaX)
    runModule(deltaY)

    time.sleep(1)

    print("End of script imgDistToOriginTest.py")
    
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
