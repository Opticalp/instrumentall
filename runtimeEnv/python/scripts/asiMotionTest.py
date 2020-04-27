# -*- coding: utf-8 -*-

## @file   testsuite/python/asiMotionTest.py
## @date   mar. 2016
## @author PhRG - opticalp.fr
##
## Test the asi motion module 

#
# Copyright (c) 2016 Ph. Renaud-Goud / Opticalp
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

    print("Test the ASI motion module")
    from instru import *
    
    fac = Factory("DataGenFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from floatDataGen factory")
    X = fac.select("dblFloat").create("X")
    print("module " + X.name + " created (" + X.internalName + ") ")
    
    print("Set output value to 314")
    X.setParameterValue("value", 314)
    
    print("Create a second module from floatDataGen factory")
    Y = fac.select("dblFloat").create("Y")
    print("module " + Y.name + " created (" + Y.internalName + ") ")
    
    print("Set output value to 62.2")
    Y.setParameterValue("value", 62.2)

    task = runModule(Y)
    task.wait()
    
    print("Look for ASI Motion device")

    asiFac = Factory("DeviceFactory").select("motion")

    ports = asiFac.selectValueList()
    for port in ports:
        if "COM" in port:
            if asiFac.select(port).select("ASI").countRemain() > 0:
                asiFac = asiFac.select(port).select("ASI")
                print("ASI motion controler (MS-2000) detected on port " + port)
                break
            elif asiFac.select(port).select("AsiTiger").countRemain() > 0:
                asiFac = asiFac.select(port).select("AsiTiger")
                print("ASI motion controler (Tiger) detected on port " + port)
                break
    
    if asiFac.countRemain()==0:
        print("no ASI Motion device available, exiting. ")
        return
    
    print("Creating the ASI Motion device module. ")
    asi = asiFac.create("asi")

    asi.setParameterValue("xAxisPos", 0)
    asi.setParameterValue("yAxisPos", 0)

    print("Bind the ports")
    bind(X.outPorts()[0], asi.inPort("xAxis"))
    bind(Y.outPorts()[0], asi.inPort("yAxis"))
    
    print('Loggers creation using the constructor: DataLogger("DataPocoLogger")')
    loggerX = DataLogger("DataPocoLogger") 
    loggerY = DataLogger("DataPocoLogger")
    
    print("Register the data loggers")
    asi.outPort("xAxis").register(loggerX)
    asi.outPort("yAxis").register(loggerY)

    print("update current position")
    runModule(asi)
    waitAll()

    print("run X, run Y")
    runModule(X)
    runModule(Y)

    print("Wait for them to finish")
    waitAll()
    
    asi.setParameterValue("xAxisPos", 0)
    asi.setParameterValue("yAxisPos", 0)

    print("End of script asiMotionTest.py")
    
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
