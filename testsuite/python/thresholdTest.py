# -*- coding: utf-8 -*-

## @file   testsuite/python/thresholdTest.py
## @date   jan. 2017
## @author PhRG - opticalp.fr
##
## Test the threshold img proc modules

#
# Copyright (c) 2017 Ph. Renaud-Goud / Opticalp
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

def myMain(baseDir, testThres):
    """Main function. Run the tests. """

    from os.path import join

    print("Test the features of the thresholding modules. ")

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

    imgDir = join(baseDir,"resources")
    print("Set image file directory to " + imgDir)
    cam.setParameterValue("directory", imgDir)
    print("Set file to be read: thresholdMe.png")
    cam.setParameterValue("files", "thresholdMe.png")

    print("Force grayscale images. ")
    cam.setParameterValue("forceGrayscale", "ON")

    print("create a new fake cam module")
    camMask = fac.select("camera").select("fromFiles").create("camMask")

    print("Set image file directory to " + imgDir)
    camMask.setParameterValue("directory", imgDir)
    print("Set file to be read: rectangles.png")
    camMask.setParameterValue("files", "rectangles.png")

    print("Force grayscale images. ")
    cam.setParameterValue("forceGrayscale", "ON")

    testThres("absolute", 128)
    testThres("population", 0.5)
    testThres("mean", 40)

    print("End of script thresholdTest.py")
  
def testThres(thresholdName, thresValue):
    """build workflow and run the tests for a given threshold"""

    import time
    
    print("run tests for threshold method: " + thresholdName)

    from instru import *
        
    cam = Module("fakeCam") 
        
    print("module " + cam.name + " retrieved (" + cam.internalName + ") ")

    print("Create a threshold ("+ thresholdName +", no mask) module")
    thres = Factory("ImageProcFactory").select("maskGen").select("threshold").select(thresholdName).create()

    print("Bind the image of the pseudo-camera to the input of threshold")
    bind(cam.outPort("image"), thres.inPort("image"))
    
    print("Attaching a data logger to show the image...")
    loggerClasses = dataLoggerClasses() # DataManager::dataLoggerClasses()
    print("Available data logger classes: ")
    for loggerClass in loggerClasses:
        print(" - " + loggerClass + ": " + loggerClasses[loggerClass])
    
    print('Logger creation using the constructor: DataLogger("ShowImageLogger")')
    logger = DataLogger("ShowImageLogger") 
    print("Logger description: " + logger.description)
#    logger.setName("imgShow")

    #cam.outPort("image").register(logger)
    thres.outPort("binImage").register(logger)

    print("Set threshold parameters")
    thres.setParameterValue("thresholdValue", thresValue)
    thres.setParameterValue("onValue",128)
    thres.setParameterValue("lowHigh","high")

    print("Run...")
    runModule(cam)
    waitAll()
    print(str(thres.outPort("count").getDataValue()*100/thres.outPort("totalCount").getDataValue()) +
          "% pixels where thresholded")
          
    time.sleep(1) # wait 1s in order to show the image

    print("Change thresholding to low")
    thres.setParameterValue("lowHigh","low")

    print("Re-run...")
    runModule(cam)
    waitAll()
    print(str(thres.outPort("count").getDataValue()*100/thres.outPort("totalCount").getDataValue()) +
          "% pixels where thresholded")
    time.sleep(1) # wait 1s in order to show the image

    print("Retrieve the mask fake cam module")
    camMask = Module("camMask")

    print("Bind the image of the second pseudo-camera to the mask input of thres")
    bind(camMask.outPort("image"), thres.inPort("mask"))

    print("Reset threshold parameters")
    thres.setParameterValue("thresholdValue", thresValue)
    thres.setParameterValue("onValue",255)
    thres.setParameterValue("lowHigh","high")

    print("Run...")
    runModule(cam)
    runModule(camMask)
    waitAll()
    print(str(thres.outPort("count").getDataValue()*100/thres.outPort("totalCount").getDataValue()) +
          "% pixels where thresholded")
    time.sleep(1) # wait 1s in order to show the image

    print("change threshold parameters")
    thres.setParameterValue("onValue",128)

    print("Re-run...")
    runModule(cam)
    runModule(camMask)
    waitAll()
    print(str(thres.outPort("count").getDataValue()*100/thres.outPort("totalCount").getDataValue()) +
          "% pixels where thresholded")
    time.sleep(1) # wait 1s in order to show the image

    print("unbind threshold input")
    unbind(thres.inPort("image"))
    unbind(thres.inPort("mask"))
    
# main body    
import sys
import os
from os.path import dirname
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print("current script: " + os.path.realpath(__file__))
        
        baseDir = dirname(dirname(__file__))
        
        print globals()
        print locals()
        myMain(baseDir, testThres)
        exit(0)

print("Presumably not called from InstrumentAll >> Exiting...")

exit("This script has to be launched from inside InstrumentAll")
