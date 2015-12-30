#
from enum import Enum
class Commands(Enum):
    register = 0x00
    statusreq = 0x01
    status = 0x02
    tickup = 0x03
    
    
    