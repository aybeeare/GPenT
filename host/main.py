from coms import ComsClass
from utils import *
import ctypes

if __name__ == "__main__":

    baud = 115200
    timeout = 10
    #com = str(input("Enter COM Port: "))
    com = 'COM3'
    Coms = ComsClass(com, baud, timeout)

    Coms.cmd1()
    #Coms.cmd2()
    #rx_data_packet = Coms.ReadBufDebug()
    #rx_data_packet = Coms.ReadBuffer(bytes_expected=20)
    
    Coms.stream_cam_bytes(total_bytes=118400,num_chunks=32)
    image_path_str = bytes_to_png('data.csv', 296, 400) # CIF
    #Coms.stream_cam_bytes(total_bytes=1920000,num_chunks=512)
    #image_path_str = bytes_to_png('data.csv', 1200, 1600) # UXGA
    #question = "Read this multiple choice question and give an answer as a single letter."
    #question = input('Ask a question about this image to ChatGPT')
    question = "Describe what you see in this image in as much detail as possible"
    answer = png_to_chatgpt(image_path_str, str(question))
    print('ChatGPT Response: ', answer)
    #print('RX Data: ', rx_data_packet)