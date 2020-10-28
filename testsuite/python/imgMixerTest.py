# -*- coding: utf-8 -*-

## @file   testsuite/python/imgMixerTest.py
## @date   oct. 2020
## @author PhRG - opticalp.fr
##
## Test the image mixer + save image normalization

#
# Copyright (c) 2020 Ph. Renaud-Goud / Opticalp
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

    print("Test the basic features of the image mixer module. ")
    print(" + Demonstrate the normalization feature of SaveImageLogger")
    
    from instru import Factory, DataLogger, DataTarget
    from instru import bind, unbind, runModule, waitAll
    
    fac = Factory("DeviceFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create 2 cameras to read image files")
    try:
        cam1 = fac.select("camera").select("fromFiles").create("fakeCam")
        cam2 = fac.select("camera").select("fromFiles").create("fakeCam2")
    except RuntimeError as e:
        print("Runtime error: " + str(e))
        print("OpenCV is probably not present. Exiting. ")
        exit(0)
        

    imgDir = join(baseDir,"resources")
    print("Set image file directory to " + imgDir)
    cam1.setParameterValue("directory", imgDir)
    cam2.setParameterValue("directory", imgDir)
    print("Set files to be read: 001.png, 003.png")
    cam1.setParameterValue("files", "001.png")
    cam2.setParameterValue("files", "003.png")

    print("Force grayscale images. 001 is already grayscale, " +
          "002 is RGB, 003 is greyscale. ")
    cam1.setParameterValue("forceGrayscale", "ON")
    cam2.setParameterValue("forceGrayscale", "ON")

    print "Create an image mixer module"
    mix = Factory("ImageProcFactory").select("modify").select("mixer").create()

    print "Bind the image of the pseudo-camera to the input of image mixer"
    bind(cam1.outPort("image"), mix.inPort("imageA"))

    print("set mixer parameters")
    params = dict()
    params["a"] = -0.5
    params["b"] = 0.5
    params["offset"] = 0.5
    params["filePath"] = str(join(imgDir, "002.png"))
    mix.setParameterValues(params)

    print('Logger creation using the constructor: DataLogger("ShowImageLogger")')
    logger = DataLogger("ShowImageLogger") 
    print("Logger description: " + logger.description)
    logger.setName("imgShow")
    
    bind(mix.outPort("image"), DataTarget(logger))

    print("Create an img stats module")
    stats = Factory("ImageProcFactory").select("analyze").select("simpleStats").create("stats")
    print("module " + stats.name + " created (" + stats.internalName + ") ")

    print("bind fake cam output to stats input")
    bind(mix.outPort("image"), stats.inPort("image"))

    print("run")
    runModule(cam1)
    waitAll()
    print("max = " + str(stats.outPort("max").getDataValue()) + " = = = = = ")
    print("min = " + str(stats.outPort("min").getDataValue()))
    time.sleep(1) # wait 1s in order to show the image

    print("bind second camera")
    bind(cam2.outPort("image"), mix.inPort("imageB"))    
    runModule(cam1)
    runModule(cam2)
    waitAll()
    print("max = " + str(stats.outPort("max").getDataValue()) + " = = = = = ")
    print("min = " + str(stats.outPort("min").getDataValue()))
    time.sleep(1) # wait 1s in order to show the image

    print("change logger to SaveImageLogger. " +
          "Please check the results in the current working directory")
    unbind(DataTarget(logger))
    logger = DataLogger("SaveImageLogger") 
    bind(mix.outPort("image"), DataTarget(logger))

    print("run with default img save parameters")
    runModule(cam1)
    runModule(cam2)
    waitAll()
    print("max = " + str(stats.outPort("max").getDataValue()) + " = = = = = ")
    print("min = " + str(stats.outPort("min").getDataValue()))

    print("change mixer parameters")
    params["a"] = -1
    params["b"] = 2
    params["offset"] = 0
    params["filePath"] = ""
    mix.setParameterValues(params)

    print("re-run")
    runModule(cam1)
    runModule(cam2)
    waitAll()
    print("max = " + str(stats.outPort("max").getDataValue()) + " = = = = = ")
    print("min = " + str(stats.outPort("min").getDataValue()))

    logger.setParameterValue("normalization", "max")
    print("run with img save parameter normalization = max")
    runModule(cam1)
    runModule(cam2)
    waitAll()
    print("max = " + str(stats.outPort("max").getDataValue()) + " = = = = = ")
    print("min = " + str(stats.outPort("min").getDataValue()))

    logger.setParameterValue("normalization", "min")
    print("run with img save parameter normalization = min")
    runModule(cam1)
    runModule(cam2)
    waitAll()
    print("max = " + str(stats.outPort("max").getDataValue()) + " = = = = = ")
    print("min = " + str(stats.outPort("min").getDataValue()))

    logger.setParameterValue("normalization", "minmax")
    print("run with img save parameter normalization = minmax")
    runModule(cam1)
    runModule(cam2)
    waitAll()
    print("max = " + str(stats.outPort("max").getDataValue()) + " = = = = = ")
    print("min = " + str(stats.outPort("min").getDataValue()))

    print("End of script imgMixerTest.py")
    
    
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
