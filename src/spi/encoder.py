# -*- coding: utf-8 -*-
"""
Created on Fri Jan  1 17:06:02 2016

@author: nerade
"""
import time
import random
from core.module import Module,MasterModule
from .command import Command
import RPi.GPIO as GPIO

ADDRESS_PINS = (15,16,13,11)

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

    MAX_SLOT = 16
    master_module = None

    def __init__(self):
        self.slot_index = 0
        if RASPBERRY:
            import spidev

            GPIO.setmode(GPIO.BOARD)
            [GPIO.setup(pin,GPIO.OUT) for pin in ADDRESS_PINS]
            [GPIO.output(pin,1) for pin in ADDRESS_PINS]

            self.spi = spidev.SpiDev()
    
    def register_master_module(self,tags):
        self.master_module=MasterModule(-1,0x0000,tags)
        self.master_module.status=False

    def cleanup(self):
        pass
        #GPIO.cleanup()

    def _get_slave_select(self,slot):
        #print("CS:",CHIP_ENABLE.get('CE_'+str(slot)))
        GPIO.output(ADDRESS_PINS[0],slot & 0x01)
        GPIO.output(ADDRESS_PINS[1],slot & 0x02)
        GPIO.output(ADDRESS_PINS[2],slot & 0x04)
        GPIO.output(ADDRESS_PINS[3],slot & 0x08)
        return 0
   
    def _open_connection(self,slot):
        if slot < 0:
            raise IndexError("Not a valid physical slot")
        if RASPBERRY:
            self.spi.open(0,int(self._get_slave_select(slot)))
            self.spi.max_speed_hz = 61000
            #self.spi.max_speed_hz = 244000
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
        time.sleep(0.01)
        request = self.spi.readbytes(1)

        if LOOPBACK:
            return request

        time.sleep(0.01)
        response =  []
        for i in range(0,request[0]):
            response.append(self.spi.readbytes(1)[0])
        print("Hexdump:",response)
        response = str(bytes(response),'ascii')
        try:
            module = Module.create_from_string(slot,response)
        except (TypeError,KeyError):
            print("Error detecting device on slot {}".format(slot))
            module = None
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
                module = self._request_SPI(slot)
                if module:
                    modules.append(module)
        if self.master_module:
            modules.append(self.master_module)
        return modules
#unschön... TODO
    def initial_reset(self):
         for slot in range(0,self.MAX_SLOT):
            if RASPBERRY:
                self._open_connection(slot)
                self.spi.xfer2([Command.RESET.value,])
                self.spi.xfer2(list(random.randint(0,0xffff).to_bytes(2,byteorder='little')))
                print("reset",slot)
                self._close_connection()


    def send_neighbours(self,module_list):
        for module in module_list:
            for receiver_module in module_list:
                if module.slot==receiver_module.slot:
                    continue
                raw = module.get_byte_repr(receiver_module.get_filter())
                print("Raw:",raw)
                try:
                    self._open_connection(receiver_module.get_slot())
                    self.spi.xfer2([Command.NEIGHBOUR.value,])
                    self.spi.xfer2(list(raw))
                    print("output:",list(raw))
                    self._close_connection()
                except IndexError:
                    pass

    def send_settings(self):
        return self.send_packet(Command.SETTINGS.value)

    def send_reset(self,module_list):
        for module in module_list:
            self._open_connection(module.get_slot())
            self.spi.xfer2([Command.RESET.value,])
            self.spi.xfer2([random.randint(0,0xffff)])
            self._close_connection()

    def send_gamestatus(self,module_list,time,strikes,status):
        trigger_count = 0
        module_list = [module for module in module_list if module.get_slot()>=0]
        for module in module_list:
            self._open_connection(module.get_slot())
            print("Scan")
            self.spi.xfer2([Command.GAMESTATUS.value,])
            self.spi.xfer2(list(bytes(str(time)+'\n','ascii')))
            self.spi.xfer2(list(bytes(str(strikes)+'\n','ascii')))
            resp_raw = self.spi.xfer2(list(bytes(str(status)+'\n','ascii')))
            print(resp_raw)
            resp = resp_raw[1]-48
            if resp == 2:
                trigger_count = trigger_count + 1
                resp = 1
            print(resp)
            self._close_connection()
            module.set_status(bool(resp))
        return trigger_count
