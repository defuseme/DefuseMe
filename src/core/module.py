import re


META_FIELDS = ('AUTHOR','VERSION','DESC','REPO','URL','REGISTER')

class Module():
    _status = True
    
    def __init__(self,slot,uid,tags={}):

        if not isinstance(tags,dict):
            raise TypeError
        self.slot = slot    
        self.uid = uid
        self._active = tags.pop('ACTIVE',True)
        [setattr(self,field.lower(),tags.pop(field)) for field in META_FIELDS if (field in tags)]
        if hasattr(self,'register'):
            self.register = [tag for tag in self.register.split(',')]
        else:
            self.register = []
        self.tags = tags

    def get_byte_repr(self,filter=[]):
        return bytes('\n'.join(['{}:{}'.format(k,v) for k,v in self.tags.items() if k in filter])+'\n','ASCII')

    def get_slot(self):
        return self.slot

    def get_filter(self):
        return self.register

    def get_status(self):
        return self._status

    def set_status(self,status):
        self._status = status

    def is_active(self):
        return self._active

    @staticmethod
    def create_from_string(position,s):
        match_obj = re.findall("""(?P<tag>^\w+):(?P<value>.+$)""",s,re.MULTILINE)
        attr = { match[0]:match[1] for match in match_obj }
        print(attr)
        module = Module(position,attr.pop('ID'),tags=attr)
        return module


class MasterModule(Module):
    def get_status(self):
        return False
