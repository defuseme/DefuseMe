# -*- coding: utf-8 -*-
"""
Created on Fri Jan  1 17:06:02 2016

@author: nerade
"""
import time
from core.module import Module
from .command import Command

CHIP_ENABLE = {
        'CE_0':'0',
        'CE_1':'0',
        'CE_2':'0',
        'CE_3':'0',
        'CE_4':'0',
        'CE_5':'0',
        'CE_6':'0',
        'CE_7':'0',
        'CE_8':'0',
        'CE_9':'0',
        'CE_10':'0',
        'CE_11':'0',
        'CE_12':'0',
        'CE_13':'0',
        'CE_14':'0',
        'CE_15':'0',

        }
RASPBERRY=True
LOOPBACK=False

class Encoder():

    MAX_SLOT = 1

    def __init__(self):
        self.slot_index = 0
        if RASPBERRY:
            import spidev
            self.spi = spidev.SpiDev()
    
    def _get_slave_select(self,slot):
        #print("CS:",CHIP_ENABLE.get('CE_'+str(slot)))
        return 0
   
    def _open_connection(self,slot):
        if RASPBERRY:
            self.spi.open(0,int(self._get_slave_select(slot)))
            self.spi.max_speed_hz = 61000
            self.spi.max_speed_hz = 244000
            self.spi.mode = 0b01

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

        self.spi.xfer2([Command.ENUMERATE.value,])
        request = self.spi.readbytes(1)

        if LOOPBACK:
            return request

        time.sleep(0.01)
        response =  []
        for i in range(0,request[0]):
            response.append(self.spi.readbytes(1)[0])
        response = str(bytes(response),'ascii')
        module = Module.create_from_string(slot,response)
        self._close_connection()
        return module

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
            if RASPBERRY:
                modules.append(self._request_SPI(slot))
        return modules

    def send_neighbour(self,data):
        return self.send_packet(Command.NEIGHBOUR.value,data)

    def send_settings(self):
        return self.send_packet(Command.SETTINGS.value)

    def send_gamestatus(self):
        return self.send_packet(Command.GAMESTATUS.value)
