# -*- coding: utf-8 -*-

## @file   python/scripts/meca500Init.py
## @date   Oct. 2018
## @author PhRG - opticalp.fr
##
## Test the Mecademic Meca500

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

def myMain(baseDir):
    """Main function. Run the tests. """

    import Tkinter as tk
    import tkMessageBox as msg

    window = tk.Tk()
    window.wm_withdraw()

    #centre screen message
    window.geometry("1x1+"+str(window.winfo_screenwidth()/2)+"+"+str(window.winfo_screenheight()/2))
    resp = msg.showinfo(title="Connexion", message="""
        Initialization of the Meca500.

        Please, power on and home the robot first.
        Press the window button when done. """)

    print(resp)

    from instru import * 
    
    fac = Factory("DeviceFactory").select("motion")
    NIClist = fac.selectValueList()

    for nic in NIClist:
        if nic.find("192.168.0.") is 0:
            fac = fac.select(nic)
            break

    print("Retrieve robot factory at the default robot IP address... ")
    
    try:
        fac = fac.select("192.168.0.100")
    except:
        print "Unexpected error:", sys.exc_info()[0]
        raise
    
    print("Create robot from its factory")
    robo = fac.create("robo")
    print("Robot module: " + robo.name + " was created. (" + robo.internalName + ") ")

    msg.showwarning(title="Set references", message="""Redefining TRF and WRF.

        Please, take care of the robot movements. """)

    robo.setParameterValue("query","SetTRF(0,0,55,0,-45,0)")
    robo.setParameterValue("query","SetWRF(300,0,250,0,0,0)")
    robo.setParameterValue("query","SetJointVel(20)")

    if not msg.askokcancel("Movement", "Ready to move? "):
        return

    robo.setParameterValue("query","SetPose(0,0,0,0,0,0)")

    resp = msg.showinfo(title="Init", message="Init done. ")
    window.destroy()
    
# main body    
import sys
import os
from os.path import dirname
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print("current script: ",os.path.realpath(__file__))
        
        baseDir = dirname(dirname(__file__))
        
        myMain(baseDir)
        exit(0)

print("Presumably not called from InstrumentAll >> Exiting...")

exit("This script has to be launched from inside InstrumentAll")
