# -*- coding: utf-8 -*-

## @file   testsuite/python/imgCenterOfMassTest.py
## @date   jul. 2018
## @author PhRG - opticalp.fr
##
## Test the center of mass computation module

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

    print("Test image center of mass module. ")
    
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
    print("Set file to be read: cross.png")
    cam.setParameterValue("files", "noiseDot.png")
    
    print('Logger creation using the constructor: DataLogger("ShowImageLogger")')
    logger = DataLogger("ShowImageLogger") 
    print("Logger description: " + logger.description)
    logger.setName("imgShow")
    
    print('Proxy creation using the constructor: DataProxy("ImageReticle")')
    proxy = DataProxy("ImageReticle") 
    print(" - Name: " + proxy.name)
    print(" - Description: " + proxy.description)

    print("Bind the image source to the image shower via the proxy")
    bind(cam.outPort("image"), DataTarget(logger), proxy)

    x = 32
    y = 44
    dim = 40

    print("Set some proxy parameters")
    proxy.setParameterValue("xPos", x)
    proxy.setParameterValue("yPos", y)

    print("Show the image with centered reticle")
    runModule(cam)
    time.sleep(1) # wait 1s in order to show the image

    print("Changing some proxy parameters")
    proxy.setParameterValue("angle", 0)
    proxy.setParameterValue("xWidth", dim)
    proxy.setParameterValue("yWidth", dim)

    print("Create the center of mass module")
    cOfM = Factory("ImageProcFactory").select("analyze").select("centerOfMass").create("centerOfMass")
    print("module " + cOfM.name + " created (" + cOfM.internalName + ") ")

    print("bind fake cam output to center of mass input")
    bind(cam.outPort("image"), cOfM.inPort("image"))

    print("Analyze (1)")
    runModule(cam)
    time.sleep(1) # wait 1s in order to show the image

    print("x position: " + str(cOfM.outPort("xPosition").getDataValue()))
    print("y position: " + str(cOfM.outPort("yPosition").getDataValue()))

    print ("Create a mask modules")
    maskFac = Factory("ImageProcFactory").select("maskGen").select("boxMask")
    mask = maskFac.create("mask")

    print ("Bind the image of the pseudo-camera to mask and mask to mask1")
    bind(cam.outPort("image"), mask.inPort("image"))
    
    print ("Set mask parameters")
    mask.setParameterValue("imgInType","ref")# ref / min / max
    mask.setParameterValue("inValue",150)
    mask.setParameterValue("outValue",0)
    mask.setParameterValue("boxType","rect")# rect / ellipse 
    mask.setParameterValue("boxWidth",dim)
    mask.setParameterValue("boxHeight",dim)
    mask.setParameterValue("boxAngle",0)
    mask.setParameterValue("boxXcenter",x)
    mask.setParameterValue("boxYcenter",y)

    bind(mask.outPort("mask"), cOfM.inPort("mask"))

    exportWorkflow("imgCenterOfMass.gv")
    
    print("Analyze (2)")
    runModule(cam)
    time.sleep(1) # wait 1s in order to show the image

    print("x position: " + str(cOfM.outPort("xPosition").getDataValue()))
    print("y position: " + str(cOfM.outPort("yPosition").getDataValue()))

    print("Now with thresholding")

    unbind(cOfM.inPort("mask"))

    thres = Factory("ImageProcFactory").select("maskGen").select("threshold").select("population").create("thresPop")

    bind(cam.outPort("image"), thres.inPort("image"))
    bind(mask.outPort("mask"), thres.inPort("mask"))

    print("Set threshold parameters")
    thres.setParameterValue("thresholdValue", 0.95)
    thres.setParameterValue("onValue",128)
    thres.setParameterValue("lowHigh","high")

    bind(thres.outPort("binImage"), DataTarget(proxy))
    bind(thres.outPort("binImage"), cOfM.inPort("mask"))
    
    exportWorkflow("imgCenterOfMass2.gv")
    
    print("Analyze (3)")
    runModule(cam)
    time.sleep(1) # wait 1s in order to show the image

    print("x position: " + str(cOfM.outPort("xPosition").getDataValue()))
    print("y position: " + str(cOfM.outPort("yPosition").getDataValue()))
    
    print("End of script imgCenterOfMassTest.py")
    
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
