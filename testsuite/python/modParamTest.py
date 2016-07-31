# -*- coding: utf-8 -*-

## @file   testsuite/python/modParamTest.py
## @date   Mar 2016
## @author PhRG - opticalp.fr
##
## Test the features of the module parameters 

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

from os.path import *

def myMain():
    """Main function. Run the tests. """
    
    print "Test the basic features of the module parameters. "
    
    fac = Factory("DemoRootFactory")
    print "Retrieved factory: " + fac.name
    
    print "Create module from leafParam factory"
    mod1 = fac.select("branch").select("leafParam").create("mod1")
    print "module " + mod1.name + " created. "
    
    print "Retrieve the module parameters: "
    params = mod1.getParameterSet()

    for param in params:
        value = mod1.getParameterValue(param["name"])
        if not isinstance(value, basestring):
            value = str(value)
        print ( " - " + param["name"] + ": " + param["descr"] +
                " ; value = " + value )
        
    print "Trying to set some parameter values: "
    print " - set intParam to 666"
    mod1.setParameterValue("intParam", 666)
    print " - set floatParam to 0"
    mod1.setParameterValue("floatParam", 0)
    print " - set strParam to mojo"
    mod1.setParameterValue("strParam", "mojo")

    print "Get the new values: "    
    for param in params:
        value = mod1.getParameterValue(param["name"])
        if not isinstance(value, basestring):
            value = str(value)
        print " - " + param["name"] + ": " +  value 

    cfgFile = join(join(dirname(dirname(realpath(__file__))),"resources"),"modParamTest.properties")
    print "Load test config file: modParamTest.properties from " + cfgFile

    loadConfiguration(cfgFile)
    print 'Config file loaded. Creating module "testParamMod" with default value for param' 
    
    val = fac.select("branch").select("leafParam").create("testParamMod").getParameterValue("intParam")
    print "intParam value is: " + str(val)
        
    print "End of script modParamTest.py"
    
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
