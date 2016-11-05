import re


META_FIELDS = ('AUTHOR','VERSION','DESC','REPO','URL')

class Module():
    
    def __init__(self,position,uid,tags={}):

        if not isinstance(tags,dict):
            raise TypeError
        
        self.position = position    
        self.uid = uid
        [setattr(self,field,tags.pop(field)) for field in META_FIELDS if (field in tags)]
        self.tags = tags

    @staticmethod
    def create_from_string(position,s):
        match_obj = re.findall("""(?P<tag>^\w+):(?P<value>.+$)""",s,re.MULTILINE)
        attr = { i[0]:i[1] for i in match_obj}
        print(attr)
        module = Module(position,attr.pop('ID'),tags=attr)
        return module


