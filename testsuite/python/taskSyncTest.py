# -*- coding: utf-8 -*-

## @file   testsuite/python/taskSyncTest.py
## @date   june 2016
## @author PhRG - opticalp.fr
##
## Test the task synchronization 

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


import time

def myMain(baseDir):
    """Main function. Run the tests. """

    import time

    print("run the task sync test script")

    from instru import *
    
    fac = Factory("DataGenFactory")
    print "Retrieved factory: " + fac.name
    
    print "Create module from floatDataGen factory"
    X = fac.select("dblFloat").create("X")
    print "module " + X.name + " created (" + X.internalName + ") "
    
    print "Set output value to 314"
    X.setParameterValue("value", 314)
    
    print "Create a second module from floatDataGen factory"
    Y = fac.select("dblFloat").create("Y")
    print "module " + Y.name + " created (" + Y.internalName + ") "
    
    print "Set output value to 62.2"
    Y.setParameterValue("value", 62.2)

    task = runModule(Y)
    task.wait()
    
    del task
    
    print "Look for the two inputs demo module factory"
    
    facDemo = Factory("DemoRootFactory").select("branch").select("leafTwoInputs")

    print "Creating demo module. "
    demo2 = facDemo.create("demo2")

    print "Bind the ports"    
    bind(X.outPorts()[0], demo2.inPort("portA"))
    bind(Y.outPorts()[0], demo2.inPort("portB"))
    
    print 'Logger creation using the constructor: DataLogger("DataPocoLogger")'
    logger = DataLogger("DataPocoLogger") 
    
    print "Register the data logger"
    demo2.outPort("portA").register(logger)

    print "run X, run Y"
    runModule(X)
    runModule(Y)

    print "Wait for them to finish"
    waitAll()
    print "OK"
    
    print "run X, wait 0.5 sec, run Y"
    runModule(X)
    time.sleep(0.5)
    runModule(Y)

##    print "hold input port A (from module X)"
##    demo2.inPort("portA").holdData("on")
##    print "change Y to 3.14"
##    Y.setParameterValue("value", 314)
##    print "re-run"
##    runModule(Y)
##
##    waitAll()
##
##    print "change X to 6.22 >> expired"
##    X.setParameterValue("value", 62.2)
##    print "re-run Y"
##    task = runModule(Y)
##
##    # waitAll here freezes the execution since ASI is waiting for X _and_ Y
##    print "wait end task >> demo 2 does not run, X is expired"    
##    task.wait()
##
##    time.sleep(1) # 1 sec sleep
##
##    print "run X >> should run now. "
##    runModule(X)
##    waitAll()
##
##    print "change Y to 6.22"
##    Y.setParameterValue("value", 62.2)
##    print "re-run Y >> X is still held"
##    runModule(Y)
##
##    waitAll()
##    
##    print "port A (X) hold off"
##    demo2.inPort("portA").holdData("off")
##
##    print "re-run Y"
##    task = runModule(Y)
##
##    # waitAll here freezes the execution since ASI is waiting for X _and_ Y
##    print "wait task >> no run, X is not held"    
##    task.wait()
##    
##    print "run X >> should run now. "
##    runModule(X)
##    waitAll()

    print "End of script taskSyncTest.py"
    
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
