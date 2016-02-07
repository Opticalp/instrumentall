# -*- coding: utf-8 -*-

## @file   testsuite/python/modPortTest.py
## @date   feb. 2016
## @author PhRG - opticalp.fr
##
## Test the features of the module ports 

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
    
    print "Test the basic features of the module ports. "
    
    fac = Factory("DemoRootFactory")
    print "Retrieved factory: " + fac.name
    
    print "Create module from leafA factory"
    mod1 = fac.select("branch").select("leafA").create("mod1")
    print "module " + mod1.name + " created. "
    
    # test ports
    print "Query input ports"
    inPorts = mod1.inPorts()
    
    for port in inPorts:
        print ( " - module " + port.parent().name +  
            ", port " + port.name + 
            ": " + port.description )
        try:
            source = port.getSourcePort()
            print ( "port source: " +
                source.name + " from module: " +
                source.parent().name )
        except ReferenceError:
            print " This port has no source"
    
    print "Query output ports"
    for port in mod1.outPorts():
        print ( " - module " + port.parent().name +  
            ", port " + port.name + 
            ": " + port.description )
        targets = port.getTargetPorts()
        print "targets: " 
        for target in targets:    
            print ( target.name + ", from module: " +
                target.parent().name )
    
    print "End of script modPortTest.py"
    
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