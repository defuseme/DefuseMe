# -*- coding: utf-8 -*-
"""
Created on Fri Jan  1 15:30:12 2016

@author: nerade
"""

from enum import Enum

class Command(Enum):
    ENUMERATE = 0x00
    NEIGHBOUR = 0x01
    SETTINGS = 0x02
    GAMESTATUS = 0x03
