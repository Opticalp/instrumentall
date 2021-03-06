# -*- coding: utf-8 -*-

## @file   testsuite/python/rotCropTest.py
## @date   jan. 2017
## @author PhRG - opticalp.fr
##
## Test the rotation/crop img proc module

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

def myMain(baseDir):
    """Main function. Run the tests. """

    from os.path import join
    import time
    
    print "Test the features of the rotCrop module. "

    from instru import * 
    
    fac = Factory("DeviceFactory")
    print "Retrieved factory: " + fac.name
    
    print "Create module from CameraFromFilesFactory"
    try:
        cam = fac.select("camera").select("fromFiles").create("fakeCam")
    except RuntimeError as e:
        print "Runtime error: {0}".format(e.message)
        print "OpenCV is probably not present. Exiting. "
        exit(0)
        
    print "module " + cam.name + " created (" + cam.internalName + ") "

    imgDir = join(baseDir,"resources")
    print "Set image file directory to " + imgDir
    cam.setParameterValue("directory", imgDir)
    print "Set file to be read: rectangles.png"
    cam.setParameterValue("files", "rectangles.png")

    print "Force grayscale images. "
    cam.setParameterValue("forceGrayscale", "ON")

    print "Create a rotCrop module"
    rotCrop = Factory("ImageProcFactory").select("modify").select("rotCrop").create("rotCrop")

    print "Bind the image of the pseudo-camera to the input of rotCrop"
    bind(cam.outPort("image"), rotCrop.inPort("image"))
    
    print "Attaching a data logger to show the image..."
    loggerClasses = dataLoggerClasses() # DataManager::dataLoggerClasses()
    print "Available data logger classes: "
    for loggerClass in loggerClasses:
        print " - " + loggerClass + ": " + loggerClasses[loggerClass]
    
    print 'Logger creation using the constructor: DataLogger("ShowImageLogger")'
    logger = DataLogger("ShowImageLogger") 
    print "Logger description: " + logger.description
    logger.setName("imgShow")

    #cam.outPort("image").register(logger)
    rotCrop.outPort("image").register(logger)

    print "Set rotCrop parameters"
    rotCrop.setParameterValue("xCenter",351)
    rotCrop.setParameterValue("yCenter",243)
    rotCrop.setParameterValue("angle",16.6)
    rotCrop.setParameterValue("width",300)
    rotCrop.setParameterValue("height",200)

    print "Show first image"
    runModule(cam)
    time.sleep(1) # wait 1s in order to show the image

    print "End of script rotCropTest.py"
    
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
