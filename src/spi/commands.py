# -*- coding: utf-8 -*-
"""
Created on Fri Jan  1 15:30:12 2016

@author: nerade
"""

from enum import Enum

class Commands(Enum):
    STATUS = 0x01
    STATUSREQ = 0x02
    REGISTER = 0x03
    ATTRREQ = 0x04
    BROADCAST = 0x05
    
    EXPLODE = 0x10
    DISARMED = 0x11
    