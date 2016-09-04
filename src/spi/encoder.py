# -*- coding: utf-8 -*-
"""
Created on Fri Jan  1 17:06:02 2016

@author: nerade
"""

from .command import Command

CHIP_ENABLE = {
        'CE_0':'',
        'CE_1':'',
        'CE_2':'',
        'CE_3':'',
        'CE_4':'',
        'CE_5':'',
        'CE_6':'',
        'CE_7':'',
        'CE_8':'',
        'CE_9':'',
        'CE_10':'',
        'CE_11':'',
        'CE_12':'',
        'CE_13':'',
        'CE_14':'',
        'CE_15':'',

        }
RASPBERRY=False

class Encoder():

    MAX_SLOT = 16

    def __init__(self):
        self.slot_index = 0
        if RASPBERRY:
            import spidev
            self.spi = spidev.SpiDev()
    
    def _get_slave_select(self):
        return CHIP_ENABLE.get('CE_'+self.slot_index)
    
    def _open_connection(self):
        if RASPBERRY:
            self.spi.open(0,self._get_slave_select())

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

    def _request_SPI(self):
        self._open_connection()

        dummies = []
        request = self.spi.xfer2([Command.ENUMERATE,0x0]

        for i in range(0,request[1]):
            dummies.append(0x0)
        response = self.spi.xfer2(dummies)
        print("Response:",response)
        self._close_connection()



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

    def enumerate(self):
        raw_data = [Command.ENUMERATE]
        if RASPBERRY:
            self._request_SPI()
        else:
            return raw_data

    def send_neighbour(self,data):
        return self.send_packet(Command.NEIGHBOUR.value,data)

    def send_settings(self):
        return self.send_packet(Command.SETTINGS.value)

    def send_gamestatus(self):
        return self.send_packet(Command.GAMESTATUS.value)
