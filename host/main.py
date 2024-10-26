from coms import ComsClass
from utils import *

if __name__ == "__main__":

    #print("Aaron Steinberg!")
    baud = 115200
    timeout = 10
    #com = str(input("Enter COM Port: "))
    com = 'COM3'
    Coms = ComsClass(com, baud, timeout)

    Coms.cmd1()
    #Coms.cmd2()
    #rx_data_packet = Coms.ReadBufDebug()
    #rx_data_packet = Coms.ReadBuffer(bytes_expected=20)
    Coms.stream_cam_bytes()
    #print('RX Data: ', rx_data_packet)