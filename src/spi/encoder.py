# -*- coding: utf-8 -*-
"""
Created on Fri Jan  1 17:06:02 2016

@author: nerade
"""

from .command import Command

CHIP_ENABLE = {
        'CE_0':'1',
        'CE_1':'1',
        'CE_2':'1',
        'CE_3':'1',
        'CE_4':'1',
        'CE_5':'1',
        'CE_6':'1',
        'CE_7':'1',
        'CE_8':'1',
        'CE_9':'1',
        'CE_10':'1',
        'CE_11':'1',
        'CE_12':'1',
        'CE_13':'1',
        'CE_14':'1',
        'CE_15':'1',

        }
RASPBERRY=True
LOOPBACK=True

class Encoder():

    MAX_SLOT = 2

    def __init__(self):
        self.slot_index = 0
        if RASPBERRY:
            import spidev
            self.spi = spidev.SpiDev()
    
    def _get_slave_select(self,slot):
        print("CS:",CHIP_ENABLE.get('CE_'+str(slot)))
        return 1
    
    def _open_connection(self,slot):
        if RASPBERRY:
            self.spi.open(0,int(self._get_slave_select(slot)))

    def _close_connection(self):
        if RASPBERRY:
            self.spi.close()

    def _write_SPI(self,raw_data):

        self._open_connection()
        
        if isinstance(raw_data,str):
            raise ValueError("Must send list of bytes")
        for byte in raw_data:
                self.spi.xfer2([byte])
                print(chr(byte),end='')
        
        self._close_connection()

    def _request_SPI(self,slot):
        self._open_connection(slot)

        dummies = []
        request = self.spi.xfer2([Command.ENUMERATE.value,0])
        if LOOPBACK:
            return request
        for i in range(0,request[1]):
            dummies.append(0x0)
        response = self.spi.xfer2(dummies)
        print("Response:",response)
        self._close_connection()
        return response

    def send_packet(self,command,data=None):
        raw_data = []
        payload = []
        payload.append(chr(command))
        #print(repr(data))
        if data:
            for module in data:
                for key,value in module.items():
                    payload.append(key)
                    payload.append(":")
                    payload.append(str(value))
                    payload.append("\n")
                payload.append('\n')
            payload.append('\n')
        raw_data=''.join(payload)
        if RASPBERRY:
            self._write_SPI(bytes(raw_data,'ascii'))
        else:
            return raw_data

    def discover(self):
        modules = []
        for slot in range(0,self.MAX_SLOT):
            raw_data = [Command.ENUMERATE]
            if RASPBERRY:
                modules.append(self._request_SPI(slot))
            else:
                modules.append(raw_data)
        return modules

    def send_neighbour(self,data):
        return self.send_packet(Command.NEIGHBOUR.value,data)

    def send_settings(self):
        return self.send_packet(Command.SETTINGS.value)

    def send_gamestatus(self):
        return self.send_packet(Command.GAMESTATUS.value)
