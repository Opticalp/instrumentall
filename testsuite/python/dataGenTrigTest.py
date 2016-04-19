# -*- coding: utf-8 -*-

## @file   testsuite/python/dataGenTrigTest.py
## @date   apr. 2016
## @author PhRG - opticalp.fr
##
## Test the trig port with a data gen module 

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
    
    print "Test the basic features of the data generator modules. "
    
    fac = Factory("DataGenFactory")
    print "Retrieved factory: " + fac.name
    
    print "Create module from StringDataGen factory"
    mojo = fac.select("str").create("strGenerator")
    print "module " + mojo.name + " created (" + mojo.internalName + ") "
    
    print 'Set output value to "mojo"'
    mojo.setParameterValue("value", "mojo")
    
#    print "Run module"
#    runModule(mojo)
#    
#    waitAll()
#    print "Return value is: " + mojo.outPort("data").getDataValue()
    
    print "Create module from Int32DataGen factory"
    mod1 = fac.select("int32").create("intGenerator")
    print "module " + mod1.name + " created (" + mod1.internalName + ") "
    
    print "Set output value to 1"
    mod1.setParameterValue("value", 1)
    
    print "Plug the output of strGenerator to the trig of intGenerator"
    bind(mojo.outPort("data"), mod1.inPort("trig"))

    print "Test the data sequence management, using the seqAccu module"
    seqAccu = Factory("DemoRootFactory").select("branch").select("leafSeqAccu").create("seqAccu")
    print "module " + seqAccu.name + " created. "
    print "Binding the ports: data + data seq"
    bind(mod1.outPort("data"), seqAccu.inPorts()[0])
    seqBind(mojo.outPort("data"), seqAccu.inPorts()[0])

    mojo.setParameterValue("seqStart", 1)
    runModule(mojo)
#    waitAll()
    mod1.setParameterValue("value", 10)
    mod1.setParameterValue("value", 2)
    runModule(mojo)
    runModule(mojo)
    waitAll() # waitAll before any seqEnd
    mod1.setParameterValue("value", 3)
    mojo.setParameterValue("seqEnd", 1)
    runModule(mojo)
    waitAll()
    print "Return value is: " + str(seqAccu.outPorts()[0].getDataValue())

    print "End of script dataGenTrigTest.py"
    
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
