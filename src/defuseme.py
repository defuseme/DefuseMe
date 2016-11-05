from spi.encoder import Encoder
import pprint

class GameManager():
    def __init__(self):
        self.encoder = Encoder()
        self._modules = []

    def get_modules(self):
        return self._modules

    def search_modules(self):
        return self.encoder.discover()

if __name__=='__main__':
    
    print("Starting up")
    gm = GameManager()
    print("Collecting module information")
    gm._modules = gm.search_modules()

    print("Modules found:",gm.get_modules())

    pprint.pprint(gm.get_modules()[0].__dict__)

