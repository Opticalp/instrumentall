# -*- coding: utf-8 -*-

## @file   testsuite/python/dataLoggerTest.py
## @date   Mar 2016
## @author PhRG - opticalp.fr
##
## Test the data logging system 

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


def myMain():
    """Main function. Run the tests. """
    
    print "Test the basic features of the data logging system. "
    
    fac = Factory("DemoRootFactory")
    print "Retrieved factory: " + fac.name
    
    print "Create module from leafDataSeq factory"
    mod1 = fac.select("branch").select("leafDataSeq").create("mod1")
    print "module " + mod1.name + " created. "
    
    print "Create module from leafForwarder factory"
    mod2 = fac.select("branch").select("leafForwarder").create("mod2")
    print "module " + mod2.name + " created. "
    
    print "Create module from leafSeqAccu factory"
    mod3 = fac.select("branch").select("leafSeqAccu").create("mod3")
    print "module " + mod3.name + " created. "
    
    print "Bind the ports: "
    print " - mod1 output to mod2 input"
    bind(mod1.outPorts()[0], mod2.inPorts()[0])
    print " - mod2 output to mod3 input"
    bind(mod2.outPorts()[0], mod3.inPorts()[0])
    
    print "Bind the sequence generator (mod1) to the sequence combiners (mod3)"
    seqBind(mod1.outPorts()[0], mod3.inPorts()[0])
    
    # query the possible DataLogger class names for DataLogger creation
    loggerClasses = dataLoggerClasses() # DataManager::dataLoggerClasses()
    print "Available data logger classes: "
    for loggerClass in loggerClasses:
        print " - " + loggerClass + ": " + loggerClasses[loggerClass]
    
    print 'Logger creation using the constructor: DataLogger("DataPocoLogger")'
    logger = DataLogger("DataPocoLogger") 
    print " - Name: " + logger.name
    print " - Description: " + logger.description
    
    print 'And once again: DataLogger("DataPocoLogger")'
    logger1 = DataLogger("DataPocoLogger") 
    
    print "Retrieve the existing loggers list"
    loggers = dataLoggers()
    print str(len(loggers)) + " loggers available"
    
    print "logger#0 is of class: " + loggers[0].name + " (" + loggers[0].description + ")"
    
    print "Register the first logger to mod2 output"
    mod2.outPorts()[0].register(logger)
    
    print "Check the registered loggers at the mod2 output port" 
    loggers = mod2.outPorts()[0].loggers()

    for lolo in loggers:
        print ("Logger: " + lolo.name + " (" + lolo.description + ")" +
               " on port: " + lolo.portSource().name + 
               " of module: " + lolo.portSource().parent().name )  
    
    print "Register the second logger to mod3 output"
    mod3.outPorts()[0].register(logger1)
    
    print "launch action: run mod1"
    runModule(mod1)
    waitAll()

    print "Detach the 1st logger and re-run"
    logger.detach()

    runModule(mod1)
    waitAll()
    
    print "Re-register the 1st logger to mod2 output"
    mod2.outPorts()[0].register(logger)

    print "Remove the logger" 
    removeDataLogger(logger)
   
    print "Check the registered loggers at the mod2 output port" 
    if mod2.outPorts()[0].loggers() is None:
        print "Ok, no more logger is registered"

    print "And re-run..."
    runModule(mod1)
    waitAll()

    print "End of script dataLoggerTest.py"
    
# main body    
import sys
import os
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print "current script: ",os.path.realpath(__file__)
        
        from instru import *

        myMain()
        exit(0)

print "Presumably not called from InstrumentAll >> Exiting..."

exit("This script has to be launched from inside InstrumentAll")
