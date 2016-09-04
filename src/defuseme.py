from core.encoder import Encoder

class GameManager():
    def __init__(self):
        self.encoder = Encoder()

    def get_modules(self):
        self.encoder.send_enumerate()

if __name__=='__main__':
    
    gm = GameManager()
    gm.get_modules()
    


