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

import time

def myMain():
    """Main function. Run the tests. """
    
    print "Test the basic features of seq gen module. "

    seqGen = Factory("DataGenFactory").select("seq").create("seqGen")
    
    fac = Factory("DemoRootFactory").select("branch")
    print "Retrieved factory: " + fac.name
    
    print "Create module from leafForwarder factory"
    mod2 = fac.select("leafForwarder").create("mod2")
    print "module " + mod2.name + " created. "
    
    print "Create module from leafSeqMax factory"
    mod3 = fac.select("leafSeqMax").create("mod3")
    print "module " + mod3.name + " created. "
    
##    # query the possible DataProxy class names for DataProxy creation
##    proxyClasses = dataProxyClasses() # DataManager::dataProxyClasses()
##    print "Available data proxy classes: "
##    for proxyClass in proxyClasses:
##        print " - " + proxyClass + ": " + proxyClasses[proxyClass]
    
    print 'Proxy creation using the constructor: DataProxy("SimpleNumConverterint32")'
    proxy = DataProxy("SimpleNumConverterint32") 
    print " - Name: " + proxy.name
    print " - Description: " + proxy.description

    print "Bind the ports: "
    print " - seqGen output to mod2 input"
    bind(seqGen.outPorts()[0], mod2.inPorts()[0], proxy)
    print " - mod2 output to mod3 input"
    bind(mod2.outPorts()[0], mod3.inPorts()[0])
    print "Data binding done. "

    print "Bind the sequence generator to the sequence combiner (mod3)"
    seqBind(seqGen.outPorts()[0], mod3.inPorts()[0])
    print "Sequence binding done. "
    
    seqGen.setParameterValue("seqSize",5)    
            
    print "launch action: run seqGen"
    runModule(seqGen)
    
    waitAll()

    if (mod3.outPorts()[0].getDataValue() != 4):
        raise RuntimeError("Return value should be \"4\"")
    else:
        print "max value of the seq is 4. OK."
    
    print "Run many times to check the sync of the threads"

    for foo in range(10):
        runModule(seqGen)

    waitAll()

    print "Ok, no deadlock occured..."
    print "Set endless seq"
    seqGen.setParameterValue("seqSize",0)
    print "run"
    runModule(seqGen)
    time.sleep(0.01)
    print "... and stop. "
    cancelAll()

    print "Last seq value is: " + str(seqGen.outPorts()[0].getDataValue())

    print "End of script seqGenTest.py"
    
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
