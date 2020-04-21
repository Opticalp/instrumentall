# -*- coding: utf-8 -*-

## @file   runtimeEnv/python/scripts/variopticLensTest.py
## @date   apr. 2020
## @author PhRG
##
## Test the asi motion module 

#
# Copyright (c) 2020 Ph. Renaud-Goud
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

    print("Test the varioptic lens module")
    from instru import *
    
    fac = Factory("DataGenFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from floatDataGen factory")
    Z = fac.select("dblFloat").create("Z")
    print("module " + Z.name + " created (" + Z.internalName + ") ")
    
    print("Set output value to 31.4")
    Z.setParameterValue("value", 31.4)
    
    task = runModule(Z)
    task.wait()
    
    print("Look for varioptic lens device")

    try:
        print("Trying to open the port: COM6")
        fac = Factory("DeviceFactory").select("motion").select("COM6").select("Varioptic")
    except RuntimeError as e:
        print(str(e))
        print("Exiting...")
        return 

    if fac.countRemain()==0:
        print("no Varioptic lens device available on port COM6, exiting. ")
        return
    
    print("Creating the Varioptic lens module. ")
    vario = fac.create("vario")

    print("Bind the ports")
    bind(Z.outPorts()[0], vario.inPort("zAxis"))
        
    print('Loggers creation using the constructor: DataLogger("DataPocoLogger")')
    loggerZ = DataLogger("DataPocoLogger") 
    
    print("Register the data loggers")
    bind(vario.outPort("zAxis"), DataTarget(loggerZ))

    print("update current position")
    runModule(vario)
    waitAll()

    print("run Z")
    runModule(Z)

    print("Wait for them to finish")
    waitAll()

    print("Send query: read register 0x03")
    vario.setParameterValue("query", "0x023803013E")

    print("Command sent")
    print("retrieved response size: " + str(len(vario.getParameterValue("query"))))
        
    print("End of script variopticLensTest.py")
    
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
