# -*- coding: utf-8 -*-
"""
Created on Fri Jan  1 15:30:12 2016

@author: nerade
"""

from enum import Enum

class Command(Enum):
    ENUMERATE = 0x80
    NEIGHBOUR = 0x81
    SETTINGS = 0x82
    GAMESTATUS = 0x83
    RESET = 0x84
