# -*- coding: utf-8 -*-

## @file   testsuite/python/imgStatsTest.py
## @date   jan. 2019
## @author PhRG - opticalp.fr
##
## Test the img stats module

#
# Copyright (c) 2019 Ph. Renaud-Goud / Opticalp
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

    print("Test image stats module. ")
    
    from instru import *
    
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
    print("Set file to be read: ramp.png")
    cam.setParameterValue("files", "ramp.png")
    
    print('Logger creation using the constructor: DataLogger("ShowImageLogger")')
    logger = DataLogger("ShowImageLogger") 
    print("Logger description: " + logger.description)
    logger.setName("imgShow")
    
    cam.outPort("image").register(logger)

    print("Create the img stats module")
    stats = Factory("ImageProcFactory").select("analyze").select("simpleStats").create("stats")
    print("module " + stats.name + " created (" + stats.internalName + ") ")

    print("bind fake cam output to stats input")
    bind(cam.outPort("image"), stats.inPort("image"))

    print("Analyze")
    runModule(cam)
    time.sleep(1) # wait 1s in order to show the image

    result = stats.outPort("max").getDataValue()
    print("check the result: " + str(result))
    if result<>255:
        raise RuntimeError("Wrong max value, should be 255, is " + str(result))

    print("Add a mask")
    maskFac = Factory("ImageProcFactory").select("maskGen").select("boxMask")
    mask = maskFac.create("mask")

    print("Bind the image of the pseudo-camera to mask and mask to img stats")
    bind(cam.outPort("image"), mask.inPort("image"))
    bind(mask.outPort("mask"), stats.inPort("mask"))

    print("set mask parameters")
    print('mask.setParameterValue("imgInType","ref"): ' +
          'only the size of the input image is considered')
    params = dict()
    params["imgInType"] = "ref"
    params["boxType"] =  "rect"
    params["inValue"] = 255
    params["outValue"] = 0
    params["boxWidth"] = 200
    params["boxXcenter"] = 127
    mask.setParameterValues(params)

    print("Analyze (with mask)")
    runModule(cam)
    waitAll()
    time.sleep(1) # wait 1s in order to show the image

    result = stats.outPort("max").getDataValue()
    print("check the result: " + str(result))
    if result>=230:
        raise RuntimeError("Wrong max value")

    print("End of script imgStatsTest.py")
    
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
