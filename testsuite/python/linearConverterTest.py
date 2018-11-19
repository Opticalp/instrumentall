# -*- coding: utf-8 -*-

## @file   testsuite/python/linearConverterTest.py
## @date   aug. 2018
## @author PhRG - opticalp.fr
##
## Test the features of the DataProxy

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
    
    print("Test the linear converter DataProxy. ")

    from instru import *

    fac = Factory("DataGenFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from intDataGen factory")
    intGen = fac.select("int32").create("intGen")
    print("module " + intGen.name + " created (" + intGen.internalName + ") ")
    
    print("Set output value to 314")
    intGen.setParameterValue("value", 314)
    
    print("Run module")
    task = runModule(intGen)
    task.wait()
    
    print("Return value is: " + str(intGen.outPort("data").getDataValue()))
    if intGen.outPort("data").getDataValue() != 314 :
        raise RuntimeError("Wrong return value: 314 expected. ")

##    print("Create module from dblFloatDataGen factory")
##    dblGen = fac.select("dblFloat").create("dblGen")
##    print("module " + dblGen.name + " created (" + dblGen.internalName + ") ")
##    
##    print("Set output value to 6.022")
##    dblGen.setParameterValue("value", 6.022)
##    
##    print("Run module")
##    task = runModule(dblGen)
##    task.wait()
##    
##    print("Return value is: " + str(dblGen.outPort("data").getDataValue()))
##    if abs(dblGen.outPort("data").getDataValue() - 6.022) > 0.01 :
##        raise RuntimeError("Wrong return value: 6.022 expected. ")

    fac = Factory("DemoRootFactory")
    print("Retrieved factory: " + fac.name)

    print("Create module from leafForwarder factory")
    forwarder = fac.select("branch").select("leafForwarder").create("forwarder")
    print("module " + forwarder.name + " created. ")

    # query the possible DataProxy class names for DataProxy creation
    proxyClasses = dataProxyClasses() # DataManager::dataProxyClasses()
    print("Available data proxy classes: ")
    for proxyClass in proxyClasses:
        print(" - " + proxyClass + ": " + proxyClasses[proxyClass])
    
    print('Proxy creation using the constructor: DataProxy("LinearConverter")')
    linearProxy = DataProxy("LinearConverter") 
    print(" - Name: " + linearProxy.name)
    print(" - Description: " + linearProxy.description)

    print("Bind the output of intGen to the forwarder via the proxy")
    bind(intGen.outPort("data"), forwarder.inPorts()[0], linearProxy)

    print("Run module intGen")
    runModule(intGen)
    waitAll()
    
    print("Return value is: " + str(forwarder.outPorts()[0].getDataValue()))
    if forwarder.outPorts()[0].getDataValue() != 314 :
        raise RuntimeError("Wrong return value: 314 expected. ")

    print("Set linear converter scaling factor to -1")
    linearProxy.setParameterValue("scale", -1)

    print("Run module intGen")
    runModule(intGen)
    waitAll()
    
    print("Return value is: " + str(forwarder.outPorts()[0].getDataValue()))
    if forwarder.outPorts()[0].getDataValue() != -314 :
        raise RuntimeError("Wrong return value: -314 expected. ")

    print("Set linear converter scaling factor to 1/314")
    linearProxy.setParameterValue("scale", 1./314)

    print("Run module intGen")
    runModule(intGen)
    waitAll()
    
    print("Return value is: " + str(forwarder.outPorts()[0].getDataValue()))
    if forwarder.outPorts()[0].getDataValue() != 1 :
        raise RuntimeError("Wrong return value: 1 expected. ")

    print("Set linear converter scaling factor to 1/3")
    linearProxy.setParameterValue("scale", 1./3)
    print("Set linear converter offset to -104.6")
    linearProxy.setParameterValue("offset", -104.7)

    print("Run module intGen")
    runModule(intGen)
    waitAll()
    
    print("Return value is: " + str(forwarder.outPorts()[0].getDataValue()))
    if forwarder.outPorts()[0].getDataValue() != 0 :
        raise RuntimeError("Wrong return value: 0 expected. ")

    print("End of script linearConverterTest.py")
    
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
