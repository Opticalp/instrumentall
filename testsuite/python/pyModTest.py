# -*- coding: utf-8 -*-

## @file   testsuite/python/pyModTest.py
## @date   Feb 2017
## @author PhRG - opticalp.fr
##
## Test the features of the python module 

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

from os.path import *

def myMain():
    """Main function. Run the tests. """
    
    print("Test the basic features of the python module. ")
    
    fac = Factory("ExternFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from python factory")
    pyMod0 = fac.select("python").select("trigA;trigB").create("pyMod0")
    print("module " + pyMod0.name + " created. ")
    
    if len(pyMod0.inPorts()) is not 2:
        raise RuntimeError("Bad trig port count")
    
    global pyMod
    pyMod = fac.select("python").select("trig").create("pyMod")
    print("module " + pyMod.name + " created. ")
    
    if len(pyMod.inPorts()) is not 1:
        raise RuntimeError("Bad trig port count")
    
    scriptFile = join(join(dirname(dirname(realpath(__file__))),"resources"),"pyModScript.py")
    print("Load script file: " + scriptFile)
    pyMod.setParameterValue("scriptFilePath",scriptFile)

    print("Create a module to trig pyMod")
    floatGen = Factory("DataGenFactory").select("float").create("floatGenerator")
    print("set generated value to 3.14")
    floatGen.setParameterValue("value",3.14)
    
    bind(floatGen.outPort("data"), pyMod.inPort("trig"))
    
    global Na 
    Na = 6.022e23
    
    runModule(floatGen)
        
    print("End of script pyModTest.py")
    
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
