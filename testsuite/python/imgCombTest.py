# -*- coding: utf-8 -*-

## @file   testsuite/python/imgCombTest.py
## @date   feb. 2019
## @author PhRG - opticalp.fr
##
## Test the fake camera module

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

    print("Test the basic features of the cameraFromFiles module. ")
    
    from instru import *
    
    fac = Factory("DeviceFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create 2 cameras to read image files")
    try:
        cam1 = fac.select("camera").select("fromFiles").create("fakeCam")
        cam2 = fac.select("camera").select("fromFiles").create("fakeCam2")
    except RuntimeError as e:
        print("Runtime error: {0}".format(e.message))
        print("OpenCV is probably not present. Exiting. ")
        exit(0)
        

    imgDir = join(baseDir,"resources")
    print("Set image file directory to " + imgDir)
    cam1.setParameterValue("directory", imgDir)
    cam2.setParameterValue("directory", imgDir)
    print("Set files to be read: 001.png, 002.png")
    cam1.setParameterValue("files", """001.png
                                      002.png""")

    print("Set file to be read: rectangles.png")
    cam2.setParameterValue("files", "rectangles.png")

    print("Force grayscale images. 001 is already grayscale, 002 is RGB. ")
    cam1.setParameterValue("forceGrayscale", "ON")
    cam2.setParameterValue("forceGrayscale", "ON")

    print "Create a rotCrop module"
    rotCrop2 = Factory("ImageProcFactory").select("modify").select("rotCrop").create()

    print "Bind the image of the pseudo-camera to the input of rotCrop"
    bind(cam2.outPort("image"), rotCrop2.inPort("image"))

    print("set rotCrop size parameter")
    params = dict()
    params["width"] = 640
    params["height"] = 400
    rotCrop2.setParameterValues(params)

    print("Create a bin image combiner")
    imComb = Factory("ImageProcFactory").select("maskGen").select("minMax").create("imComb")
    
    print("bind the image sources to the image combiner")
    bind(cam1.outPort("image"), imComb.inPort("imgA"))
    bind(rotCrop2.outPort("image"), imComb.inPort("imgB"))
    
    print('Logger creation using the constructor: DataLogger("ShowImageLogger")')
    logger = DataLogger("ShowImageLogger") 
    print("Logger description: " + logger.description)
    logger.setName("imgShow")
    
    bind(imComb.outPort("image"), DataTarget(logger))

    print("set the imCombiner to \"max\"")
    imComb.setParameterValue("combType","max")

    print("run")
    for ind in range(2):
        runModule(cam1)
        runModule(cam2)
        waitAll()
        time.sleep(1) # wait 1s in order to show the image

    
    print("set the imCombiner to \"max\"")
    imComb.setParameterValue("combType","min")

    print("run")
    for ind in range(2):
        runModule(cam1)
        runModule(cam2)
        waitAll()
        time.sleep(1) # wait 1s in order to show the image

    print("End of script imgCombTest.py")
    
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
