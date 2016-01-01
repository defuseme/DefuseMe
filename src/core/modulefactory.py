# -*- coding: utf-8 -*-
modulesList = []

class Modules():
    
    def __init__(self,position,uid,moduleType,tags):
        if not moduleType:
            raise TypeError
        
        if isinstance(tags,str):
            raise TypeError
        
        i = 0
        tmpUid = uid
        while tmpUid in modulesList:
            tmpUid = uid + i
            i = i + 1
            
        self.position = position    
        self.uid = tmpUid
        self.moduleType = moduleType
        self.tags = tags        
        

class ModuleFactory():
    
    @staticmethod
    def registerModule(self,uid,moduleType,tags):
        
        module = Module(uid,moduleType,tags)
        modulesList.append(module)
        return module