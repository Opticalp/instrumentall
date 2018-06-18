# -*- coding: utf-8 -*-

## @file   testsuite/python/imgHistogramTest.py
## @date   jun. 2018
## @author PhRG - opticalp.fr
##
## Test the histogram computation module

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

    print("Test image histogram module. ")
    
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

    print("Create the histogram module")
    histo = Factory("ImageProcFactory").select("analyze").select("histogram").create("histo")
    print("module " + histo.name + " created (" + histo.internalName + ") ")

    print("bind fake cam output to histogram input")
    bind(cam.outPort("image"), histo.inPort("image"))

    print("Analyze")
    runModule(cam)
    time.sleep(1) # wait 1s in order to show the image

    result = histo.outPort("histogram").getDataValue()
    print("check the result: " + str(result[0]))
    if result[0] > 1./254 or result[0] < 1./256:
        raise RuntimeError("Wrong histogram value for gray level == 0")

    print("level 256 value: " + str(result[256]))
    if result[256] > 0.00001:
        raise RuntimeError("Wrong histogram value for gray level == 256")

    print("End of script imgHistogramTest.py")
    
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
