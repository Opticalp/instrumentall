# -*- coding: utf-8 -*-

## @file   testsuite/python/boxMaskTest.py
## @date   oct. 2017
## @author PhRG / KG / AI
##
## Test the box mask module

#
# Copyright (c) 2017 Ph. Renaud-Goud / Opticalp and contributors
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
    
    print ("Test the features of the box mask module. ")

    from instru import *
    
    fac = Factory("DeviceFactory")
    print ("Retrieved factory: " + fac.name)
    
    print ("Create module from CameraFromFilesFactory")
    try:
        cam = fac.select("camera").select("fromFiles").create("fakeCam")
    except RuntimeError as e:
        print ("Runtime error: {0}".format(e.message))
        print ("OpenCV is probably not present. Exiting. ")
        exit(0)
        
    print ("module " + cam.name + " created (" + cam.internalName + ") ")

    imgDir = join(baseDir,"resources")
    print ("Set image file directory to " + imgDir)
    cam.setParameterValue("directory", imgDir)
    print ("Set file to be read: rectangles.png")
    cam.setParameterValue("files", "rectangles.png")

    print ("Force grayscale images. ")
    cam.setParameterValue("forceGrayscale", "ON")

    print ("Create a mask modules")
    maskFac = Factory("ImageProcFactory").select("maskGen").select("boxMask")
    mask = maskFac.create("mask")
    mask1 = maskFac.create("mask1")

    print ("Bind the image of the pseudo-camera to mask and mask to mask1")
    bind(cam.outPort("image"), mask.inPort("image"))
    bind(mask.outPort("mask"), mask1.inPort("image"))
    
    print ("Attaching a data logger to show the image...")
    logger = DataLogger("ShowImageLogger") 
    print ("Logger description: " + logger.description)
    logger.setName("imgShow")

    mask1.outPort("mask").register(logger)

    print ("Set mask parameters")
    print('mask.setParameterValue("imgInType","ref"): ' +
          'only the size of the input image is considered')
    mask.setParameterValue("imgInType","ref")# ref / min / max
    mask.setParameterValue("inValue",150)
    mask.setParameterValue("outValue",80)
    mask.setParameterValue("boxType","rect")# rect / ellipse 
    mask.setParameterValue("boxWidth",200)
    mask.setParameterValue("boxHeight",200)
    mask.setParameterValue("boxAngle",0)
    mask.setParameterValue("boxXcenter",300)
    mask.setParameterValue("boxYcenter",300)

    print('mask1.setParameterValue("imgInType","max"): ' +
          "the input image and the new mask are OR'ed to the output")
    mask1.setParameterValue("imgInType","max")# ref / min / max
    mask1.setParameterValue("inValue",255)
    mask1.setParameterValue("outValue",80)
    mask1.setParameterValue("boxType","ellipse")# rect / ellipse 
    mask1.setParameterValue("boxWidth",60)
    mask1.setParameterValue("boxHeight",60)
    mask1.setParameterValue("boxAngle",0)
    mask1.setParameterValue("boxXcenter",300)
    mask1.setParameterValue("boxYcenter",300)

    print ("Show first image (OR)")
    runModule(cam)
    time.sleep(1) # wait 1s in order to show the image

    print ("Change mask1 parameters")
    print('mask1.setParameterValue("imgInType","min"): ' +
          "the input image and the new mask are AND'ed to the output")
    mask1.setParameterValue("imgInType","min")# ref / min / max

    print ("Show second image (AND)")
    runModule(cam)
    time.sleep(1)

    print ("Set mask parameters (change height, angle)")
    mask.setParameterValue("boxWidth",200)
    mask.setParameterValue("boxHeight",300)
    mask.setParameterValue("boxAngle",45)

    print('reset mask1 to "OR", change height, angle, x center')
    mask1.setParameterValue("imgInType","max")# ref / min / max
    mask1.setParameterValue("boxWidth",60)
    mask1.setParameterValue("boxHeight",100)
    mask1.setParameterValue("boxAngle",15)
    mask1.setParameterValue("boxXcenter",400)
    mask1.setParameterValue("boxYcenter",300)

    print ("Show third image")
    runModule(cam)
    time.sleep(1) 

    print ("End of script boxMaskTest.py")
    
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
