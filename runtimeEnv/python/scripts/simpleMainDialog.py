# -*- coding: utf-8 -*-

## @file   runtimeEnv/python/scripts/simpleMainDialog.py
## @date   Jan 2018
## @author PhRG
##
## Create a main control
## Could be used to launch a task (e.g. with App.go() )
## as an example, a camera management is proposed. 

#
# Copyright (c) 2019 Ph. Renaud-Goud
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


class App:
    def __init__(self, top):
        import Tkinter as tk

        tk.Button(top, text="Visu camera", command=self.camera).pack(pady=5)
        tk.Button(top, text="Lancer", command=self.go).pack(pady=5)
        tk.Button(top, text="Quitter", command=top.quit).pack(pady=5)

        self.top = top

    def camera(self):
        import camera
        camera.simpleViewer()
        camera.Dialog(self.top)

    def go(self):
        """do things"""
        pass
   
# main body    
import sys
import os
from os.path import dirname
import Tkinter as tk

if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print "current script: ",os.path.realpath(__file__)
        
        baseDir = dirname(dirname(dirname(__file__)))

        top = tk.Tk()
        top.title("InstrumentAll")
        top.minsize(500,300)
        app = App(top)

        top.mainloop()

        exit(0)

print "Presumably not called from InstrumentAll >> Exiting..."

exit("This script has to be launched from inside InstrumentAll")
