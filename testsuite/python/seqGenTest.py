# -*- coding: utf-8 -*-

## @file   testsuite/python/seqGenTest.py
## @date   may 2017
## @author PhRG - opticalp.fr
##
## Test the sequence generation module

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
    from instru import *
    
    print("Test the basic features of seq gen module. ")

    seqGen = Factory("DataGenFactory").select("seq").create("seqGen")
    
    fac = Factory("DemoRootFactory").select("branch")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from leafForwarder factory")
    mod2 = fac.select("leafForwarder").create("mod2")
    print("module " + mod2.name + " created. ")
    
    print("Create module from leafSeqMax factory")
    mod3 = fac.select("leafSeqMax").create("mod3")
    print("module " + mod3.name + " created. ")
    
    # query the possible DataProxy class names for DataProxy creation
    proxyClasses = dataProxyClasses() # DataManager::dataProxyClasses()
    print "Available data proxy classes: "
    for proxyClass in proxyClasses:
        print " - " + proxyClass + ": " + proxyClasses[proxyClass]
    
    print('Proxy creation using the constructor: DataProxy("SimpleNumConverter")')
    proxy = DataProxy("SimpleNumConverter") 
    print(" - Name: " + proxy.name)
    print(" - Description: " + proxy.description)

    print("Bind the ports: ")
    print(" - seqGen output to mod2 input")
    bind(seqGen.outPorts()[0], mod2.inPorts()[0], proxy)
    print(" - mod2 output to mod3 input")
    bind(mod2.outPorts()[0], mod3.inPorts()[0])
    print("Data binding done. ")

    print("Bind the sequence generator to the sequence combiner (mod3)")
    seqBind(seqGen.outPorts()[0], mod3.inPorts()[0])
    print("Sequence binding done. ")
    
    seqGen.setParameterValue("seqSize",5)    
            
    print("launch action: run seqGen")
    runModule(seqGen)
    
    waitAll()

    if (mod3.outPorts()[0].getDataValue() != 4):
        raise RuntimeError("Return value should be \"4\"")
    else:
        print("max value of the seq is 4. OK.")
    
    print( "Run many times to check the sync of the threads")

    for foo in range(10):
        runModule(seqGen)

    waitAll()

    print("Ok, no deadlock occured...")
    
    print("Test the clocked generation. Period is 1s")
    seqGen.setParameterValue("delay", 1000)
    
    print("and run seqGen once. ")
    t0 = time.time()
    
    runModule(seqGen)
    waitAll()
    
    elapsed = time.time()-t0
    print("elapsed time was: " + str(elapsed) + " seconds")
    if elapsed < 3: # 4 data generated <=> 3 intervals
        raise RuntimeError("the delay was not effective")
    elif elapsed > 4: 
        raise RuntimeError("the delay setting seems wrong. duration was too long. ")  
    
    print("Reset the delay to zero to reset the clocked generation")
    seqGen.setParameterValue("delay",0)
    
    print("Set endless seq")
    seqGen.setParameterValue("seqSize",0)
    print("run")
    runModule(seqGen)
    time.sleep(0.01)
    print("... and stop. ")
    cancelAll()

    print("Last seq value is: " + str(seqGen.outPorts()[0].getDataValue()))

    print("And now, testing the trigged version.")

    print("Creating a trigger source")
    trig = Factory("DataGenFactory").select("int64").create("trig")

    print("Create a parameter setter to trig AND set the parameter seqSize simultaneously")
    setter = seqGen.buildParameterSetter("seqSize")

    print("Create module from leafSeqAccu factory")
    accu = fac.select("leafSeqAccu").create("accu")
    print("module " + accu.name + " created. ")

    print("Make the bindings")
    bind(trig.outPort("data"),seqGen.inPort("trig"))

    bind(mod3.outPorts()[0], accu.inPort("inPortA"))
    seqBind(trig.outPort("data"), accu.inPort("inPortA"))
    bind(trig.outPort("data"),DataTarget(setter))

    print("Export the workflow: seqGenWorkflow.gv")
    exportWorkflow("seqGenWorkflow.gv")

    print("Set trig value to [3 6 2]")
    print('setParameterValue("value", 3)')
    trig.setParameterValue("value", 3)
    print('setParameterValue("seqStart", 1)')
    trig.setParameterValue("seqStart", 1)
    print("runModule")
    runModule(trig)
    trig.setParameterValue("value", 6)
    runModule(trig)
    waitAll() # waitAll before any seqEnd

    print("set seq end")
    trig.setParameterValue("seqEnd", 1)
    trig.setParameterValue("value", 2)

    runModule(trig)
    waitAll() 

    print("seqAccu result is: " + str(accu.outPorts()[0].getDataValue()))
    if accu.outPorts()[0].getDataValue() != [2,5,1]:
           raise RuntimeError("result should have been [2,5,1]")

    print("End of script seqGenTest.py")
    
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
