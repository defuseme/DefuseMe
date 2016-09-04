# -*- coding: utf-8 -*-
from module import Module
modulesList = []

class ModuleFactory():
    
    @staticmethod
    def registerModule(self,uid,moduleType,tags):
        
        module = Module(uid,moduleType,tags)
        modulesList.append(module)
        return module
