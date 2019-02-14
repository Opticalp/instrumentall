# -*- coding: utf-8 -*-

## @file   python/embed/camera.py
## @date   Jan 2019
## @author PhRG - opticalp.fr
##
## Manage a genicam camera
## Use camera from files if the requested camera is not available

#
# Copyright (c) 2019 Ph. Renaud-Goud / Opticalp
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

class Camera:
    def __init__(self, baseDir):
        """find the camera"""

        from instru import Factory, DataLogger
        from os.path import join, isfile
        from os import listdir

        try:
            fact = Factory("DeviceFactory").select("camera").select("genicam")
            # TODO: modify here:
##            ctiPath = "C:\\Program Files\\Allied Vision\\Vimba_2.1\\VimbaUSBTL\\Bin\\Win64\\VimbaUSBTL.cti"
            ctiPath = "C:\\Program Files\\Baumer\\Baumer GAPI SDK\\Components\\Bin\\x64\\bgapi2_usb.cti"
            print("Please, check the GenTL.cti path before use: " + ctiPath)
            fact = fact.select(ctiPath)
            print(fact.selectValueList())
            # TODO: modify here:
##            interfaceId = "VimbaUSBInterface_0x0"
            interfaceId = "USB"
            print("Please, check the interface ID from the list above... Using " + interfaceId)
            genFact = fact.select(interfaceId)
            print(genFact.selectValueList())
            # TODO: modify here:
##            devId = "DEV_1AB2280005D5"
            devId = "199E43710690"
            print("Please, check the device ID from the list above... Using " + devId)
            genFact = genFact.select(devId)

##            confFile = join(join(baseDir,"conf"),"mako.yml")
            confFile = "None"
            print("Loading conf file: " + confFile)
            self.cam = genFact.select(confFile).create("camera")
        except:
            print("Genicam camera not found, using camera from files. ")
            imgDir = join(baseDir,"img")
            print("please check that the directory " + imgDir + " contains some images")
            self.cam = Factory("DeviceFactory").select("camera").select("fromFiles").create("camera")
            self.cam.setParameterValue("directory", imgDir)

            files = ""
            for file in listdir(imgDir):
                if isfile(join(imgDir,file)):
                    files = files + file + "\n"
            if files is not "":
                files = files[:-1] # remove trailing \n
            print("Image(s) found: \n" + files)
            self.cam.setParameterValue("files", files)

        print("Create fullscreen logger")
        self.logger = None
        try:
            self.logger = DataLogger("FullscreenLogger")
            self.loggerIsFS = True
        except:
            self.loggerIsFS = False
            self.logger = DataLogger("ShowImageLogger")

    def monitor(self,monitorID):
        if self.loggerIsFS:
            self.logger.setParameterValue("monitor", monitorID)

    def plugViewer(self):
        from instru import bind, DataTarget, runModule

        if self.logger:
            bind(self.cam.outPort("image"),DataTarget(self.logger))
            runModule(self.cam)
        else:
            print("Img shower initialization failed. Can not plug the viewer. ")
        

def logger():
    return camera.logger

def simpleViewer():
    camera.plugViewer()

class Dialog:
    def __init__(self, top):
        import Tkinter as tk
        
        dlg = tk.Toplevel(top)
        dlg.title("Contrôle camera")
        dlg.minsize(300,300)

        tk.Label(dlg, text="moniteur").pack()

        self.eMon = tk.Entry(dlg)
        self.eMon.pack()

        tk.Button(dlg, text="Appliquer", command=self.monitor).pack()

    def monitor(self):
        camera.monitor(int(self.eMon.get()))       

import os
from os.path import dirname

print("Find the camera")
baseDir = dirname(dirname(dirname(__file__)))

camera = Camera(baseDir)
