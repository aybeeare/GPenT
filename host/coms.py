import serial
import time
from time import strftime
from datetime import datetime
import os
import sys
import csv
from utils import *
import ctypes



class ComsClass():

    def __init__(self, com, baud, timeout):

        # com = str(input("Enter COM Port: "))
        self.ser = serial.Serial(port=com, baudrate=baud, timeout=timeout)
        self.timeout = timeout
        
        # Byte Codes
        self.TX_SYNC = 'CB'
        self.RX_SYNC = '77'
        self.TX_PACKET = 'AB'
        self.RX_PACKET = 'BA'

        # commands
        self.STREAM1_CMD = '76'
        self.STREAM2_CMD = '69'

        # Clear PC buffers of stale data
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

    #     # Logging Timestamped Data to Text and CSV Files
    #     path = os.getcwd()
    #     self.base_path = 'logs\\'
    #     self.txt_path = 'txt\\'
    #     self.csv_path = 'csv\\' 

    #     if not os.path.isdir(path + self.base_path):
    #         os.mkdir(path + self.base_path)

    #     if not os.path.isdir(path + self.base_path + self.txt_path):
    #         os.mkdir(path + self.base_path + self.txt_path)

    #     if not os.path.isdir(path + self.base_path + self.csv_path):
    #         os.mkdir(path + self.base_path + self.csv_path)
        

    #     self.filename = filename
    #     self.out_file_txt = self.base_path + self.txt_path + self.filename + '.txt'
    #     self.out_file_csv = self.base_path + self.csv_path + self.filename + '.csv'
    
    # def log_txt(self, log_str, term_print):

    #     log_str = '[' + str(time.strftime('%m/%d %X')) + '] ' + log_str

    #     with open(self.out_file_txt, "a+", newline='') as fname:

    #         fname.write(log_str)

    #     fname.close()

    #     if term_print:

    #         print(log_str)

    # def log_csv(self, data_to_log):

    #     timestamp = f"{datetime.now().strftime("%Y-%m-%d %H:%M:%S")}, "

    #     with open(self.out_file_csv, 'a+', newline='') as fname:

    #         log_str = timestamp + data_to_log
    #         fname.write(log_str)
    
    def ReadBufDebug(self):

        start_time = time.time()

        while 1:

            # wait for header bytes
            if self.ser.inWaiting() >= 2:
                
                if self.ser.read(1).hex() == hex(int(self.RX_SYNC, 16))[2:]:
                    
                    if self.ser.read(1).hex() == hex(int(self.RX_PACKET, 16))[2:]:
                        print('Received Frame Sync and Header!')
                        break
            
            elif (time.time() - start_time) > 50:
                sys.exit('Timed Out!')
            
            else:
                pass

        rx_data_packet = [hex(i) for i in list(self.ser.read(self.ser.inWaiting()))]
        return rx_data_packet

        # while 1:

        #     if self.ser.inWaiting() > 0:
        #         time.sleep(10)
        #         num_bytes_rxd = self.ser.inWaiting()
        #         print('Bytes in Buffer: ', self.ser.read(num_bytes_rxd))
                # print('Bytes in Buffer: ', self.ser.inWaiting())
                # #print('First Byte: ', bytes.fromhex(self.ser.read(1)))#.hex()))
                # #print('Second Byte: ', bytes.fromhex(self.ser.read(1)))
                # print('First Byte: ', self.ser.read(1).hex())
                # print('Second Byte: ', self.ser.read(1).hex())
                # print('Third Byte: ', self.ser.read(1).hex())
                # print('Fourth Byte: ', self.ser.read(1).hex())
    
    def ReadBuffer(self, bytes_expected):

        smallest_trans_size = 50000 # smallest transaction rx'd in num of bytes

        start_time = time.time()

        while 1:

            # wait for header bytes
            if self.ser.inWaiting() >= 2:
                
                if self.ser.read(1).hex() == hex(int(self.RX_SYNC, 16))[2:]:
                    
                    if self.ser.read(1).hex() == hex(int(self.RX_PACKET, 16))[2:]:
                        #print('Received Frame Sync and Header!')
                        break
            
            elif (time.time() - start_time) > 50:
                sys.exit('Timed Out!')
            
            else:
                pass

        # programmed delay to wait for large chunks of data...
        if bytes_expected > smallest_trans_size:

            for i in range(30):
                heartbeat_str = i*'*'
                sys.stdout.write(heartbeat_str) # print heartbeat signal to screen
                #sys.stdout.flush()
                time.sleep(0.5)


        start_time = time.time()
        
        while self.ser.inWaiting() < bytes_expected:
            
            if (time.time() - start_time) > self.timeout: 
                break
                sys.exit('Timed Out!')
                
        rx_data_list = list(self.ser.read(bytes_expected))
        rx_data_packet = [hex(i) for i in rx_data_list]

        return rx_data_packet


    def send_command(self, command):

        sent_com = self.ser.write(bytes.fromhex(command))

    def cmd1(self):

        self.ser.reset_output_buffer()
        self.ser.reset_input_buffer()

        command_packet = self.TX_SYNC + self.TX_PACKET + self.STREAM1_CMD
        #print(type(bytes.fromhex(command_packet)))
        self.send_command(command_packet)
        # Read Buffer...

    def cmd2(self):

        self.ser.reset_output_buffer()
        self.ser.reset_input_buffer()

        command_packet = self.TX_SYNC + self.TX_PACKET + self.STREAM2_CMD
        self.send_command(command_packet)

    def stream_cam_bytes(self, total_bytes, num_chunks):

        chunk_size = total_bytes // num_chunks

        buf_length = self.ReadBuffer(bytes_expected=4)
        image_width = self.ReadBuffer(bytes_expected=4)
        image_height = self.ReadBuffer(bytes_expected=4)

        print('Buffer Length: ', buf_length)
        print('Image Width: ', image_width)
        print('Image Height: ', image_height)

        # Total payload is 76800 bytes, PC serial buffer maxes out at 3950, send in 32 chunks of 2400 bytes
        payload_list = []

        for i in list(range(num_chunks)):

            # print('Bytes in Buffer Before Read: ', self.ser.inWaiting())
            heartbeat_str = str((i/num_chunks)*100) + '%' + ' completed\n'
            sys.stdout.write(heartbeat_str)
            payload_chunk = self.ReadBuffer(bytes_expected=chunk_size)
            payload_list.append(payload_chunk)
            # print('Bytes in Buffer After Read: ', self.ser.inWaiting())

        # print('Payload List Length: ', len(payload_list))
        # print('Length Payload Chunk: ', len(payload_list[0]))

        num_bytes_missing = 0
        total_bytes = 0
        for i in list(range(num_chunks)): # list of 32 lists, each 2400 long

            for j in list(range(chunk_size)):

                if len(payload_list[i][j]) >= 3:
                    
                    total_bytes += 1

        # print('Total Bytes: ', total_bytes) 

        transposed_list = zip(*payload_list)
        with open("data.csv", "w", newline='') as file: # TODO: Images are ODDLY ZOOMED!
            writer = csv.writer(file)
            #writer.writerows(payload_list)
            writer.writerows(transposed_list)