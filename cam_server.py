from flask import request
from flask import Flask
import numpy as np
import cv2
import os
from multiprocessing import Value
import requests

import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader
from torchvision import datasets, transforms
from WeatherModel import WeatherModel
import pandas as pd
import pickle
from sklearn.tree import DecisionTreeClassifier

dhtTemp = 0
bmpTemp = 0
humidity = 0
windSpeed = 0
pressure = 0

app = Flask(__name__)
# app.config['DEBUG'] = True
# app.secret_key = 'sdfhj43uop23opjuhjg234jghds8'
# app.jinja_env.globals.update(zip=zip)

# declare counter variable
counter = Value('i', 0)

def save_img(img):
	with counter.get_lock():
		counter.value += 1
		count = counter.value
	img_dir = "esp32_imgs"
	if not os.path.isdir(img_dir):
		os.mkdir(img_dir)
	cv2.imwrite(os.path.join(img_dir,"img_"+str(count)+".jpg"), img)
	# print("Image Saved", end="\n") # debug

@app.route('/')
@app.route('/index', methods=['GET'])
def index():
	return "ESP32-CAM Flask Server", 200

# temporarily stopped saving to demonstrate proof of model
@app.route('/upload_image', methods=['POST','GET'])
def upload():
	received = request
	img = None
	if received.files:
		print(received.files['imageFile'])
		# convert string of image data to uint8
		file  = received.files['imageFile']
		nparr = np.fromstring(file.read(), np.uint8)
		
		# decode image
		img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

		# process image
		device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
		transform = transforms.Compose([
			transforms.ToTensor(),
			transforms.Resize((128, 128)),
			transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5)),
		])
		rgb_img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
		transformedImg = transform(rgb_img)
		model = WeatherModel(num_classes=6).to(device)
		model.load_state_dict(torch.load("WeatherModel.pth", map_location=torch.device("cpu")))
		reshapedImg = torch.reshape(transformedImg, (1, 3, 128, 128))
		reshapedImg.to(device)
		model.eval()
		output = model(reshapedImg)
		_, processed_output = torch.max(output, 1)
		otherData = torch.tensor([windSpeed, bmpTemp, pressure, dhtTemp, humidity])

		# process and combine data for input into decision tree
		inputArray = torch.cat((processed_output, otherData), dim=0)
		df_input = pd.DataFrame(inputArray.reshape((1, 6)))

		# initialise decision tree
		with open('decision_tree.pkl', 'rb') as file:
			decisionTree = pickle.load(file)
		
		output = decisionTree.predict(df_input)
		data = str(output[0]) # should be 1 or 0 based on the model output
		print(data)
		requests.post("http://192.168.58.121/raining", data=data)
		#save_img(img)
		
		return "[SUCCESS] Image Received", 201
	else:
		return "[FAILED] Image Not Received", 204

app.route('upload_data', methods = ['POST'])
def upload_data():
	windSpeedStr = request.args.get('windSpeed')
	bmpTempStr = request.args.get('bmpTemp')
	bmpPressureStr = request.args.get('bmpPressure')
	dhtTempStr = request.args.get('dhtTemp')
	dhtHumidityStr = request.args.get('dhtHumidity')
	
	windSpeed = float(windSpeedStr) if windSpeedStr else None
	bmpTemp = float(bmpTempStr) if bmpTempStr else None
	pressure = float(bmpPressureStr) if bmpPressureStr else None
	dhtTemp = float(dhtTempStr) if dhtTempStr else None
	humidity = float(dhtHumidityStr) if dhtHumidityStr else None

	return "OK", 200

def main():
    app.run(host = "0.0.0.0", port = '3237')

if __name__ == '__main__':
    main()