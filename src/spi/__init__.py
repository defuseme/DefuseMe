from enum import Enum

class Commands(Enum):
    ACK = 0x00
    
    STATUS = 0x01
    REGISTER = 0x02
    STRIKE = 0x03
    
    
    NAK = 0x80
    
    