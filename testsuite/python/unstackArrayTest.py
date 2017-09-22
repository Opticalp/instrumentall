# -*- coding: utf-8 -*-

## @file   testsuite/python/unstackArrayTest.py
## @date   jul. 2017
## @author PhRG - opticalp.fr
##
## Test the unstack array control module

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

    import time
    
    print("Test the features of the unstack array module. ")

    from instru import *
    
    fac = Factory("DataGenFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Retrieve a vector generator")
    vectGen = fac.select("int32Vect").create("vectGen")

    print("prepare the data buffer with range(10)")
    for value in range(10):
        vectGen.setParameterValue("value", value)
        
    fac = Factory("ControlFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create the unstack module")
    spliter = fac.select("dataShaping").select("unstack").select("int32").create("spliter")
    
    print("bind")
    bind(vectGen.outPort("data"), spliter.inPort("array"))

    print("Test the data sequence management, using the seqAccu module")
    seqAccu = Factory("DemoRootFactory").select("branch").select("leafSeqAccu").create("seqAccu")
    print("module " + seqAccu.name + " created. ")
    print("Binding the ports: data + data seq")
    bind(spliter.outPort("elements"), seqAccu.inPorts()[0])
    seqBind(spliter.outPort("elements"), seqAccu.inPorts()[0])

    print("Run")
    runModule(vectGen)
    
    waitAll()

    print("Return value is: " + str(seqAccu.outPorts()[0].getDataValue()))
    if ( seqAccu.outPorts()[0].getDataValue() != range(10) ):
        raise RuntimeError("Wrong return value")

    print("Replace the demo seqAccu module by the generic seqAccumulator")
    print("Create the seqAccumulator module")
    accuGen = fac.select("dataShaping").select("accu").select("int32").create("seqAccuGen")

    print("unbind the previous connexions")
    unbind(seqAccu.inPorts()[0])
    seqUnbind(seqAccu.inPorts()[0])

    print("bind the new module")
    bind(spliter.outPort("elements"), accuGen.inPort("elements"))
    seqBind(spliter.outPort("elements"), accuGen.inPort("elements"))

    print("prepare the data buffer with range(10)")
    for value in range(10):
        vectGen.setParameterValue("value", value)
        
    print("Run")
    runModule(vectGen)
    
    waitAll()

    print("Return value is: " + str(accuGen.outPort("array").getDataValue()))
    if ( accuGen.outPort("array").getDataValue() != range(10) ):
        raise RuntimeError("Wrong return value")

    print "End of script unstackArrayTest.py"
    
# main body    
import sys
import os
from os.path import dirname
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print "current script: ",os.path.realpath(__file__)
        
        baseDir = dirname(dirname(__file__))
        
        myMain(baseDir)
        exit(0)

print "Presumably not called from InstrumentAll >> Exiting..."

exit("This script has to be launched from inside InstrumentAll")

