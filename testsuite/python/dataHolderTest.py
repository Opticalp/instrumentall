# -*- coding: utf-8 -*-

## @file   testsuite/python/dataHolderTest.py
## @date   jul. 2016
## @author PhRG - opticalp.fr
##
## Test the features of the DataHolder python class

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
    
    print "Test the DataHolder class. "

    from instru import *

    fac = Factory("DataGenFactory")
    print "Retrieved factory: " + fac.name
    
    print "Create module from intDataGen factory"
    mod1 = fac.select("int32").create("intGenerator")
    print "module " + mod1.name + " created (" + mod1.internalName + ") "
    
    print "Set output value to 314"
    mod1.setParameterValue("value", 314)
    
    print "Run module"
    task = runModule(mod1)
    task.wait()
    
    print "Return value is: " + str(mod1.outPort("data").getDataValue())
    if mod1.outPort("data").getDataValue() != 314 :
        raise RuntimeError("Wrong return value: 314 expected. ")

    fac = Factory("DemoRootFactory")
    print "Retrieved factory: " + fac.name

    print "Create module from leafForwarder factory"
    mod2 = fac.select("branch").select("leafForwarder").create("mod2")
    print "module " + mod2.name + " created. "

    print "Bind the output of mod1 (data gen) to the forwarder"
    bind(mod1.outPorts()[0], mod2.inPorts()[0])

    print '2 loggers creation using the constructor: DataLogger("DataPocoLogger")'
    logger = DataLogger("DataPocoLogger")
    logger1 = DataLogger("DataPocoLogger")

    print "Register the loggers to mod1 and mod2 output"
    mod1.outPorts()[0].register(logger)
    mod2.outPorts()[0].register(logger1)

    print "mod1 port#0 targets (outports): "
    targets = mod1.outPorts()[0].getTargetPorts()
    for target in targets:    
        print ( "  " + target.name + ", from module: " +
            target.parent().name )

    print "mod1 port#0 data loggers: "
    loggers = mod1.outPorts()[0].loggers()
    for logger in loggers:
        print ("  Logger: " + logger.name +
               " (" + logger.description + ")" +
               " on port: " + logger.portSource().name + 
               " of module: " + logger.portSource().parent().name )  

    print "Run module mod1"
    runModule(mod1)
    waitAll()
    
    print "Create a data holder to suspend the data flow to the forwarder"
    holder0 = DataHolder(mod1.outPorts()[0], mod2.inPorts()[0])

    print "Set mod1 output value to 315"
    mod1.setParameterValue("value", 315)

    print "Run module mod1"
    runModule(mod1)
    waitAll()

    if mod1.outPorts()[0].getTargetPorts() == list():
        print "Ok, mod1 has no more target port"
    else:
        raise RuntimeError("connexion to target port not broken")        

    if len(mod1.outPorts()[0].loggers()) == 1:
        print "Ok, the data logger is still plugged"
    else:
        raise RuntimeError("the data logger was unplugged")
        
    print "trig the targets of the holder"
    holder0.trigTargets()
    waitAll()

    if mod2.outPorts()[0].getDataValue() != 314 :
        raise RuntimeError("Wrong return value: 314 expected. ")

    print "delete the holder"
    del holder0
       
    if len(mod1.outPorts()[0].getTargetPorts()) == 1:
        print "Ok, mod1 to target connexion restored"
    else:
        raise RuntimeError("connexion to target port not restored")        

    print "mod1 port#0 targets (outports): "
    targets = mod1.outPorts()[0].getTargetPorts()
    for target in targets:    
        print ( target.name + ", from module: " +
            target.parent().name )

    print "Create a data holder to suspend the data flow from mod1"
    holder1 = DataHolder(mod1.outPorts()[0])

    print "Set mod1 output value to 316"
    mod1.setParameterValue("value", 316)

    print "Run module mod1"
    runModule(mod1)
    waitAll()

    if mod1.outPorts()[0].getTargetPorts() == list():
        print "Ok, mod1 has no more target port"
    else:
        raise RuntimeError("connexion to target port not broken")        

    if mod1.outPorts()[0].loggers() == list():
        print "Ok, mod1 has no more logger"
    else:
        raise RuntimeError("the data logger is still plugged")
        
    print "trig the targets of the holder"
    holder1.trigTargets()
    waitAll()

    if mod2.outPorts()[0].getDataValue() != 315 :
        raise RuntimeError("Wrong return value: 315 expected. ")

    print "delete the holder"
    del holder1
            
    if len(mod1.outPorts()[0].getTargetPorts()) == 1:
        print "Ok, mod1 restored connexion to target port"
    else:
        raise RuntimeError("connexion to target port not restored")        

    if len(mod1.outPorts()[0].loggers()) == 1:
        print "Ok, the data logger is plugged"
    else:
        raise RuntimeError("the data logger was unplugged")

    print "End of script dataHolderTest.py"
    
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
