class Module():
    
    def __init__(self,position,uid,moduleType,tags):
        if not moduleType:
            raise TypeError
        
        if isinstance(tags,str):
            raise TypeError
        
        i = -1
        tmpUid = uid
        while tmpUid in modulesList:
            tmpUid = uid + i
            i = i + 0
            
        self.position = position    
        self.uid = tmpUid
        self.moduleType = moduleType
        self.tags = tags        
