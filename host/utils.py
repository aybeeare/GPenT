import numpy as np
import os
from PIL import Image
import matplotlib.pyplot as plt
import base64
import requests
import yaml

with open("config.yml", "r") as ymlfile:
    cfg = yaml.safe_load(ymlfile)
global TOKEN
TOKEN = cfg["TOKEN"]

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
    
    # Return and save image, returning file path to image
    image = Image.fromarray(image_arr, mode='L')
    image_str = 'data_cif.png'
    image.save(image_str)
    cwd = os.getcwd()
    image_path_str = cwd + '/' + image_str

    return image_path_str

def png_to_chatgpt(png_path, question): # png_path as arg in future :)

    #png_path = 'C:/Users/abelk/OneDrive/Desktop/Pen/GPenT/host/mc_question.png'
    #question = "Read this multiple choice question and give an answer as a single letter."

    with open(png_path, "rb") as image_file:
        base64_image = base64.b64encode(image_file.read()).decode('utf-8')

    headers = {
        "Content-Type": "application/json",
        "Authorization": f"Bearer {TOKEN}"
    }

    payload = {
        "model": "gpt-4o-mini",
        "messages": [
            {
                "role": "user",
                "content": [
                    {
                        "type": "text",
                        "text": f"{question}" 
                    },
                    {
                        "type": "image_url",
                        "image_url": {
                            "url": f"data:image/png;base64,{base64_image}"
                        }
                    }
                ]
            }
        ],
        "max_tokens": 1000
    }

    response = requests.post("https://api.openai.com/v1/chat/completions", headers=headers, json=payload)
    answer = response.json()['choices'][0]['message']['content']
    #print('Answer: ', answer)
    return answer

# Call and test functions scratch pad

#debug('debug_logic_analyzer.csv')
#bytes_to_png('data.csv', 240, 320) # QVGA
#bytes_to_png('data.csv', 1200, 1600) # UXGA
#bytes_to_png('data.csv', 320, 480) # HVGA
#bytes_to_png('data.csv', 480, 640) # VGA
#png_to_chatgpt(0, 0)