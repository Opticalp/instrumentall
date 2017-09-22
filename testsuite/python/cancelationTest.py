# -*- coding: utf-8 -*-

## @file   testsuite/python/cancelationTest.py
## @date   feb. 2016
## @author PhRG - opticalp.fr
##
## Test cancelation

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
    
    print "Test the task cancelation management. "

    from instru import *
    
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
    
    print "Bind the sequence generator (mod1) to the sequence combiner (mod3)"
    seqBind(mod1.outPorts()[0], mod3.inPorts()[0])
    print "Sequence binding done. "
    
    print "launch action: run mod1"
    runModule(mod1)

    print "wait 0.5 sec and cancel all. "
    time.sleep(0.5)
    cancelAll()
    
    waitAll()
    
    print "Run 3 times to check the sync of the threads and the cleaning after cancelation"

    runModule(mod1)
    runModule(mod1)
    runModule(mod1)

    print "wait 0.5 sec and cancel all (2) "
    time.sleep(0.5)
    cancelAll()
    waitAll()

    print "launch action: run mod1"
    runModule(mod1)

    time.sleep(0.5)
    waitAll()
    print "mod3 return value is: " + str(mod3.outPorts()[0].getDataValue())

    print "End of script modDataSeqTest.py"
    
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
