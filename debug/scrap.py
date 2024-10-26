from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
import time

# Load the image
image = Image.open("C:/Users/abelk/Documents/Projects/Pen/epic.jpg").convert('L')

# Resize the image to 240x320
#image = image.resize((76800,1))
#image = image.convert("L")
byte_array = image.tobytes()

#print(np.array(list(byte_array), dtype='uint8').shape)

image_arr = np.array(list(byte_array), dtype='uint8')

row_1 = list(image_arr[0:320])
row_2 = list(image_arr[320:640])
#print(row_1)
# print(list(image_arr)[0:320])
# plt.hist(list(byte_array), bins=10)
# plt.title("Pixel Histogram")
# plt.xlabel("Value")
# plt.ylabel("Frequency")
# plt.show()
image_arr = image_arr.reshape((240, 320))
#image_arr = image_arr.reshape((320, 240))
#print(list(image_arr[0, :]))

if row_1 == list(image_arr[0,:]):
    print('Weewoo1!')
    
if row_2 == list(image_arr[1,:]):
    print('Weewoo2!')

image = Image.fromarray(image_arr, mode='L')
image.save('simply_epic.jpg')

print('Image Saved!')

print(int('0x0',16))