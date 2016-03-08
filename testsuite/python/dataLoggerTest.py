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
    
#    logger = DataLogger("DataLoggerClassName") # creation using the factory
#    # OR
#    logger = dataLoggers()[N] # query the data manager about existing data loggers
#    
#    # register a logger to some data
#    module.OutPorts()[P].data().registerLogger(logger)
#    
#    # check the registered loggers
#    loggers = module.OutPorts()[P].data().loggers()
#
#    for logger in loggers:
#        print "Logger: " + logger.name + "(" + logger.description + ") "
#              " on port: " + logger.parent().parent().name + 
#              " of module: " + logger.parent().parent().parent().name  
#    
#    # remove a logger from a data
#    detachDataLogger(logger)
#
#    # remove a logger 
#    removeDataLogger(logger)
   
    # set the poco logger to mod2 output data
    #mod2.outPorts()[0].data().registerLogger(DataLogger("DataPocoLogger"))
    
    print "launch action: run mod1"
    runModule(mod1)
    
    waitAll()
    
    print "End of script dataLoggerTest.py"
    
# main body    
import sys
import os
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall":
        print "current script: ",os.path.realpath(__file__)
        
        from instru import *

        myMain()
        exit(0)

print "Presumably not called from InstrumentAll >> Exiting..."

exit("This script has to be launched from inside InstrumentAll")