# -*- coding: utf-8 -*-

## @file   testsuite/python/cvMatTest.py
## @date   jan. 2017
## @author PhRG - opticalp.fr
##
## Test the cvMat data generator modules 

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

    import os
    import time
    
    print("Test the basic features of the cvMat data generator modules. ")

    from instru import Factory, DataLogger
    from instru import bind, dataLoggerClasses, runModule, waitAll
    
    fac = Factory("DataGenFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from cvMatDataGen factory")
    try:
        imgGen = fac.select("cvMat").create("imgGen")
        imgGen2 = fac.select("cvMat").create("imgGen2")
    except RuntimeError as e:
        print("Runtime error: " + str(e))
        print("OpenCV is probably not present. Exiting. ")
        exit(0)
        
    print("module " + imgGen.name + " created (" + imgGen.internalName + ") ")
    
    print("Set imgGen output value to 127")
    imgGen.setParameterValue("value", 127)
    print("Set imgGen2 output value to 63")
    imgGen2.setParameterValue("value", 63)
    
    print("Run module")
    task = runModule(imgGen)

    print(task.name + " state is " + task.state())
    task.wait()
    print(task.name + " state is " + task.state())

    print("Attaching a data logger to show the image...")
    loggerClasses = dataLoggerClasses() # DataManager::dataLoggerClasses()
    print("Available data logger classes: ")
    for loggerClass in loggerClasses:
        print(" - " + loggerClass + ": " + loggerClasses[loggerClass])
    
    print('Loggers creation using the constructor: DataLogger("ShowImageLogger")')
    logger = DataLogger("ShowImageLogger")
    logger2 = DataLogger("ShowImageLogger")
    print("Logger description: " + logger.description)

    print("set logger2 imagePanel to 1")
    logger2.setParameterValue("imagePanel",1)

    print("bind the loggers")
    imgGen.outPort("data").register(logger)
    imgGen2.outPort("data").register(logger2)

    print("run imgGen and imgGen2")
    runModule(imgGen)
    waitAll() # to avoid the linux xcb error while not initializing XInitThreads
    runModule(imgGen2)
    time.sleep(1) # wait 1s in order to show the images

    print("Set output value to 128")
    imgGen.setParameterValue("value", 128)

    print("Add a save image logger")
    saver = DataLogger("SaveImageLogger")
    imgGen.outPort("data").register(saver)

    files = os.listdir(".")
    if files.count("img_01.png")>0:
        os.remove("img_01.png")

    runModule(imgGen)
    waitAll()

    print("check if the image is present in the current directory")
    files = os.listdir(".")
    if files.count("img_01.png")!=1:
        raise RuntimeError("image img_01.png not created")

    print("try SaveImageLogger normalization")
    saver.setParameterValue("normalization", "max")
    
    if files.count("img_02.png")>0:
        os.remove("img_02.png")

    runModule(imgGen)
    waitAll()

    print("check if the new image is present in the current directory")
    files = os.listdir(".")
    if files.count("img_02.png")!=1:
        raise RuntimeError("image img_02.png not created")
        
    print("End of script cvMatTest.py")
    
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
