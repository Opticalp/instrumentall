# -*- coding: utf-8 -*-
##
# Some convenience tools to be imported by InstrumentAll 
#
# This module should be imported either in console mode 
# or via a script into InstrumentAll to get some general
# tools to deal with InstrumentAll raw features 
#   
# @file   python/embed/instruTools.py
# @brief  General python tools for InstrumentAll
# @date   14/06/2017
# @author PhRG - kevin GARDETTE
#

"""
Created on june 14 2017

General python tools for InstrumentAll

@author: PhRG - KG
"""

#
# Copyright (c) 2017 kevin GARDETTE / HEF
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


# tests could be inserted here 
if __name__ == "__main__":
    exit("Stand-alone call: nothing to be done")

# the ImportError exception is forwarded if any
import sys
import os
from instru import *

# general information 
def generalInfo():
    """Display the general information of instrumentall"""
    
    print about()
    print "instrumentall version: " + version()

# call to createFactories()
def discoverUntilLeaves(factory):
    """Descend into the child factories of the given factory until the leaves are reached. """

    if factory.isLeaf():
        return
    
    selectors = factory.selectValueList()
    for sel in selectors:
        if sel: # sel is not an empty string: empty string means free selector
            print "discovering with selector: " + sel
            discoverUntilLeaves(factory.select(sel))

## Create all the possible factories, then export.
def createFactories():
    """Main function. Create the factories, and export the corresponding graph. """
   
    print "Test the factory tree export graph feature"

    print "Retrieve and explore each root factories: "
    factories = getRootFactories()
    for fac in factories:
        print " - " + fac.name
        discoverUntilLeaves(fac)
    
    print "Export factories tree to fullFacTree.gv"
    exportFactoriesTree("fullFacTree.gv")
            
    print "End of script factoryTreeExportTest.py"

## Print the root factories with the description
def printRootFactories():
    """Display the root factories with the description"""
    roots = getRootFactories()
    for fact in roots:
        print fact.name + ": " + fact.description

## print information you have after create
def printInPorts(module):
    """Display the InPort of the module"""
    
    print " \n Print in port"
    inPorts = module.inPorts()
    for port in inPorts:
        print port.name + ": " + port.description

def printOutPorts(module):
    """Display the OutPort of the module"""
    
    print " \n Print out port"
    outPorts = module.outPorts()
    for port in outPorts:
        print port.name + ": " + port.description

def printParameters(module):
    """Display the module parameter and its value"""
    
    print " \n Print parameters "
    params = module.getParameterSet()
    #for param in params:
    #    print "param index: " + str(param['index']) + " > " + param['name'] + "(" + param['descr'] + ")"
    for param in params:
        value = module.getParameterValue(param["name"])
        if not isinstance(value, basestring):
            value = str(value)
        print ( " - " + param["name"] + ": " + param["descr"] +
                " ; value = " + value )

def printModuleDetails(module):
    """Display all details of module: InPort; OutPort and Parameter"""
    
    print " Module name: " + module.name
    print " Module internal name: " + module.internalName
    print " Module description: " + module.description
    printOutPorts(module)
    printInPorts(module)
    printParameters(module)
