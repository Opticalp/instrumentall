# -*- coding: utf-8 -*-

## @file   testsuite/python/parameterSetterTest.py
## @date   Aug. 2016
## @author PhRG - opticalp.fr
##
## Test the parameter setters 

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

    import time
    
    print("Test the basic features of parameterSetter. ")

    from instru import *
    
    fac = Factory("DataGenFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from Int32DataGen factory")
    mod1 = fac.select("int32").create("mod1")
    print("module " + mod1.name + " created (" + mod1.internalName + ") ")
    
    print("Set output value to 1")
    mod1.setParameterValue("value", 1)
    
    print("Run module")
    task = runModule(mod1)

    print(task.name + " state is " + task.state())
    task.wait()
    print(task.name + " state is " + task.state())
    
    print("Return value is: " + str(mod1.outPort("data").getDataValue()))
    if (mod1.outPort("data").getDataValue() != 1):
        raise RuntimeError('Wrong return value: 1 expected. ')

    print("Prepare a second module from Int32DataGen factory")
    mod2 = fac.select("int32").create("mod2")
    print("module " + mod2.name + " created (" + mod2.internalName + ") ")
    
    print("Set mod2 output value to -1")
    mod2.setParameterValue("value", -1)
    
    print("Run module")
    task = runModule(mod2)

    print(task.name + " state is " + task.state())
    task.wait()
    print(task.name + " state is " + task.state())
    
    print("Return value is: " + str(mod2.outPort("data").getDataValue()))
    if (mod2.outPort("data").getDataValue() != -1):
        raise RuntimeError('Wrong return value: -1 expected. ')

    print("Will plug mod1 output to mod2 value parameter...")
    print("1. create parameter setter")
    setter = mod2.buildParameterSetter("value")
    print("parameter name: " + setter.parameterName())
    
    print("2. create a data proxy to convert int32 to int64")
    proxy = DataProxy("SimpleNumConverter") 
    print(" - Name: " + proxy.name)
    print(" - Description: " + proxy.description)
    
    print("3. bind the setter to the mod1 output via the previous proxy")
    bind(mod1.outPort("data"), DataTarget(setter), proxy)
    
    print("Set mod1 output value to 10")
    mod1.setParameterValue("value", 10)
    
    print("Run module mod2")
    task = runModule(mod2)

    print(task.name + " state is " + task.state())
    
    print("wait some... (1 sec)")
    time.sleep(1)
    print(task.name + " state is " + task.state())

    print("Run module mod1")
    task1 = runModule(mod1)
    print("wait mod1 end of execution")
    task1.wait()
    print("ok.")
    
    print(task.name + " state is " + task.state())
    print("and wait")
    task.wait()
    print(task.name + " state is " + task.state())

    print("Return value is: " + str(mod2.outPort("data").getDataValue()))
    if (mod2.outPort("data").getDataValue() != 10):
        raise RuntimeError('Wrong return value: 10 expected. ')    
      
    print("End of script parameterSetterTest.py")
    
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
