# -*- coding: utf-8 -*-

## @file   testsuite/python/dataGenTest.py
## @date   mar. 2016
## @author PhRG - opticalp.fr
##
## Test the data generator modules 

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
    
    print("Test the basic features of the data generator modules. ")

    from instru import *
    import instruTools
    
    fac = Factory("DataGenFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from floatDataGen factory")
    mod1 = fac.select("float").create("floatGenerator")
    print("module " + mod1.name + " created (" + mod1.internalName + ") ")

    instruTools.printModuleDetails(mod1)
    
    print("Set output value to 3.14")
    mod1.setParameterValue("value", 3.14)
    
    print("Run module")
    task = runModule(mod1)

    print(task.name + " state is " + task.state())
    task.wait()
    print(task.name + " state is " + task.state())
    
    print("Return value is: " + str(mod1.outPort("data").getDataValue()))
    if ( abs(mod1.outPort("data").getDataValue()-3.14) > 0.01 ):
        raise RuntimeError("Wrong return value: 3.14 expected. ")

    print("Create module from StringDataGen factory")
    mod1 = fac.select("str").create("strGenerator")
    print("module " + mod1.name + " created (" + mod1.internalName + ") ")
    
    print('Set output value to "mojo"')
    mod1.setParameterValue("value", "mojo")
    
    print("Run module")
    runModule(mod1)
    
    waitAll()
    print("Return value is: " + mod1.outPort("data").getDataValue())
    if ( mod1.outPort("data").getDataValue() != "mojo" ):
        raise RuntimeError('Wrong return value: "mojo" expected. ')
    
    print("Create module from Int32DataGen factory")
    mod1 = fac.select("int32").create("intGenerator")
    print("module " + mod1.name + " created (" + mod1.internalName + ") ")
    
    print("Set output value to 1")
    mod1.setParameterValue("value", 1)
    
    print("Run module")
    runModule(mod1)
    
    waitAll()
    print("Return value is: " + str(mod1.outPort("data").getDataValue()))
    if (mod1.outPort("data").getDataValue() != 1):
        raise RuntimeError('Wrong return value: 1 expected. ')

    print("Test the data sequence management, using the seqAccu module")
    seqAccu = Factory("DemoRootFactory").select("branch").select("leafSeqAccu").create("seqAccu")
    print("module " + seqAccu.name + " created. ")
    print("Binding the ports: data + data seq")
    bind(mod1.outPort("data"), seqAccu.inPorts()[0])
    seqBind(mod1.outPort("data"), seqAccu.inPorts()[0])

    print('setParameterValue("value", 50)')
    mod1.setParameterValue("value", 50)
    print('setParameterValue("seqStart", 1)')
    mod1.setParameterValue("seqStart", 1)
    print("runModule many times: Twice...")
    runModule(mod1)
    runModule(mod1)
    waitAll() # if we do not wait here, the data is de-synced because
            # we did not stack the value again with setParameterValue

    print("done, and then in a 'for' loop")
    
    for value in range(20):
        mod1.setParameterValue("value", value)
        runModule(mod1)

    print("wait for all threads to terminate, and then set sequence end. ")
    waitAll() # waitAll before any seqEnd
    mod1.setParameterValue("seqEnd", 1)
    mod1.setParameterValue("value", 100)

    runModule(mod1)
    waitAll()
    print("Return value is: " + str(seqAccu.outPorts()[0].getDataValue()))
    if ( seqAccu.outPorts()[0].getDataValue() != [50, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 100] ):
        raise RuntimeError("Wrong return value")
    
    print("Test the vector generation")
    mod1 = fac.select("dblFloatVect").create("vectGen")
    
    for value in range(10):
        mod1.setParameterValue("value", value)

    runModule(mod1)
    waitAll() # mandatory!! If not done, there is no way to know where multiple calls are splited 
    print("Return value is: " + str(mod1.outPort("data").getDataValue()))
    
    print("End of script dataGenTest.py")
    
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
