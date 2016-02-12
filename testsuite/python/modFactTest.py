# -*- coding: utf-8 -*-

## @file   testsuite/python/modFactTest.py
## @date   jan. 2016
## @author PhRG - opticalp.fr
##
## Test the features of the module factories 

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
    
    print "Test the basic features of the module factories. "
    
    facList = getRootFactories()
    
    print "Available factories: "
    for facStr in facList:
        print " - " + facStr
    
    print " --- Factory detail "
    fac = Factory("DemoRootFactory")
    print "Factory name: " + fac.name
    print "Factory description: " + fac.description
    
    ## select
    print fac.selectDescription()
    print "Possible selectors: "
    for val in fac.selectValueList():
        print " - " + val + " >> " + fac.select(val).name
        print "   " + fac.select(val).description
    
    print "Create module from leaf factory"
    fac.select("branch").select("leaf").create()
    
    if (fac.countRemain()>0):
        print "Creating module from " + fac.name + " without arg" 
        mod = fac.create()
        print "  module " + mod.name + " created: " + mod.description
        print mod.name + " parent is: " + mod.parent().name

    if (fac.countRemain()>0):
        print "Creating module from " + fac.name + ' with arg: "mojo"' 
        mod = fac.create("mojo")
        print "  module " + mod.name + " created. "
    
    if (fac.countRemain()>0):
        print "Trying to create a module from " + fac.name + ' with arg: "mojo"'
        try:  
            fac.create("mojo")
        except RuntimeError:
            print "Error caught. OK. Duplicate name. "
        else:
            raise RuntimeError("duplicate module name should have been detected")
        
    if (fac.countRemain()>0):
        print "Creating module from " + fac.name + ' with arg: ".all-chars_"' 
        mod = fac.create(".all-chars_")
        print "  module " + mod.name + " created. "
    
    if (fac.countRemain()>0):
        print "Trying to create a module from " + fac.name + ' with arg: "josé"'
        try:  
            fac.create("josé")
        except RuntimeError:
            print "Error caught. OK. unauthorized character. "
        else:
            raise RuntimeError("unauthorized character in module name should have been detected")
        
    print "End of script modFactTest.py"
    
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