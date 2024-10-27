import numpy as np
import os
from PIL import Image
import matplotlib.pyplot as plt

def debug(filename):

    # Quick function that takes exported csv file from logic analyzer and decodes its message (raw bytes to english)

    cwd = os.getcwd()
    new_dir = cwd + '/debug'
    os.chdir(new_dir)
    arr = np.genfromtxt(filename, delimiter=',', dtype=None, encoding='utf-8', names=True)
    arr_list = arr.tolist()
    os.chdir('..')
    #print(arr_list)

    hex_data = []
    for tup in arr_list:
        #print(list(tup))
        hex_data.append(int(list(tup)[-1], 16))

    #print(hex_data)
    ascii_string = ''.join(chr(b) for b in hex_data)
    print(ascii_string)

def bytes_to_png(filename, height, width):

    arr = np.loadtxt(filename, delimiter=',', dtype=str)
    #print(arr.shape)
    arr_flattened = arr.flatten(order='F') # worked once with , order='F'?

    arr_flattened = [int(i,16) for i in arr_flattened.tolist()]
    image_arr = np.array(arr_flattened, dtype='uint8')
   
    # plt.hist(image_arr, bins=10)
    # plt.title("Pixel Histogram")
    # plt.xlabel("Value")
    # plt.ylabel("Frequency")
    # plt.show()
    
    image_arr = image_arr.reshape((height, width)) # worked once with order default?
    

    image = Image.fromarray(image_arr, mode='L')
    #image.save('data.png')
    image.save('data_cif.png')

    print('Image Saved!')

def png_to_chatgpt(png_path):

    pass
    answer = 0

    return answer

# Call and test functions scratch pad

#debug('debug_logic_analyzer.csv')
#bytes_to_png('data.csv', 240, 320) # QVGA
#bytes_to_png('data.csv', 1200, 1600) # UXGA
#bytes_to_png('data.csv', 320, 480) # HVGA
#bytes_to_png('data.csv', 480, 640) # VGA
bytes_to_png('data.csv', 296, 400) # CIF