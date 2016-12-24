import random
from core.module import MasterModule
from spi.encoder import Encoder
import pprint
import sys
import time
from core.utils import do_every

INTERVAL = 0.2

class GameManager():

    def __init__(self):
        self.encoder = Encoder()
        self.encoder.register_master_module({'SEED':random.randint(0,0xffff)})
        self._modules = []
        self._master_module = None
        self._running = False
        self._strikes = 0

    def get_strikes(self):
        return self._strikes

    def set_strikes(self,strikes):
        self._strikes = strikes

    def inc_strikes(self,inc):
        self.set_strikes(self.get_strikes() + inc)

    def get_modules(self):
        return self._modules

    def set_modules(self,modules):
        self._modules = modules

    def search_modules(self):
        self.encoder.initial_reset()
        time.sleep(2)
        all_modules = self.encoder.discover()
        modules = []
        for module in all_modules:
            if isinstance(module,MasterModule):
                self._master_module = module
            else:
                modules.append(module)
        return modules

    def send_neighbours(self):
        return self.encoder.send_neighbours(self._modules+[self._master_module])

    def check_module_states(self):
        states = []
        for module in self.get_modules():
            states.append(module.get_status())
            print(module.get_slot(),module.get_status())
        return not any(states)

    def run_check(self):
        timeout = not self._running if hasattr(self,'game_time') else False
        strike_out = self.max_strikes < self.get_strikes()
        defused = self.check_module_states()
        print("Strikes:",self.get_strikes())
        print("Is defused?",defused)
        print(strike_out,timeout)
        if defused:
            print("SUCCESS")
        elif any((timeout,strike_out)):
            print("BOOOOOOOM")
        return not any((timeout,strike_out))

    def _update(self):
        self.game_time = self.exploding_time-int(time.time()*1000)
        if not self._running:
            self.encoder.send_gamestatus(self._modules,self.game_time,strikes=0,status=0)
            self._running = True
            return
        if (self.check_module_states()):
            self.encoder.send_gamestatus(self._modules,
                    time=0 if self.game_time <=0 else self.game_time,
                    strikes=self.get_strikes(),
                    status=0
            )
            print("sendsend")
            self._running = False
        elif (self.game_time > 0) and (self.get_strikes() < self.max_strikes):
            trigger_new = self.encoder.send_gamestatus(self._modules,
                    self.game_time,
                    strikes=self.get_strikes(),
                    status=1
            )
            self.inc_strikes(trigger_new)
            
        else:
            self.encoder.send_gamestatus(self._modules,
                    time=0 if self.game_time <=0 else self.game_time,
                    strikes=self.get_strikes(),
                    status=2
            )
            self._running = False

    def run(self,limit,max_strikes):
        self.max_strikes = max_strikes
        self.exploding_time = int(time.time()+int(limit))*1000
        do_every(INTERVAL,self._update,self.run_check)

if __name__=='__main__':
    
    if len(sys.argv) != 3:
        print("You failed, BITCH!")
        sys.exit()
    print("Starting up")
    gm = GameManager()
    print("Collecting module information")
    gm.set_modules(gm.search_modules())
    if not gm.get_modules():
        print("There are no modules attached to the Frame!")
        sys.exit()
    print("Modules found:",gm.get_modules())

    [pprint.pprint(module.__dict__) for module in gm.get_modules() if module]
    [pprint.pprint(module.get_byte_repr()) for module in gm.get_modules() if module]
    #pprint.pprint(gm.get_modules()[0].get_byte_repr())
    gm.send_neighbours()

    gm.run(sys.argv[1],int(sys.argv[2]))
    gm.encoder.cleanup()
