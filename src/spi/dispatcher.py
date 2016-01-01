# -*- coding: utf-8 -*-
"""
Created on Sun Dec 20 18:30:23 2015

@author: nerade
"""

class Dispatcher():
    
    def __init__():
        self.registeredCommands = {}
        
    def register(commandCode,commandHandler):
        if not commandCode in self.registeredCommands:
            self.registeredCommands[commandCode] = commandHandler
            return True
        else:
            return False
            
    def unregister(commandCode):
        if commandCode in self.registeredCommands:
            del self.registeredCommands[commandCode]
            return True
        else:
            return False
    

    
    