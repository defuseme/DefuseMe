# -*- coding: utf-8 -*-
from collections import OrderedDict
from .encoder import Encoder

import unittest

class TestSPIProtocollEncoder(unittest.TestCase):

    def setUp(self):
        self.neighbour1_data = OrderedDict([
            ('id', 4213),
            ('battery', 2),
            ('display', 1),
            ('active', False)
        ])
        self.neighbour2_data = OrderedDict([
            ('id', 2213),
            ('active', True),
            ('hazard','radio-active'),
            ('needy', True)
        ])

        self.encoder = Encoder()

    def test_enumerate(self):
        result = self.encoder.send_enumerate()
        self.assertEqual(result,'\x00')

    def test_neighbour_broadcast(self):
        result = self.encoder.send_neighbour((self.neighbour1_data,self.neighbour2_data))
        self.assertEqual(result,'\x01id:4213\nbattery:2\ndisplay:1\nactive:False\n\nid:2213\nactive:True\nhazard:radio-active\nneedy:True\n\n\n')

    def test_settings_broadcast(self):
        result = self.encoder.send_settings()
        self.assertEqual(result,'\x02')

    def test_gamestatus_broadcast(self):
        result = self.encoder.send_gamestatus()
        self.assertEqual(result,'\x03')
