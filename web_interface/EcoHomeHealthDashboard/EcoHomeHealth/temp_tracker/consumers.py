import json 
from channels.generic.websocket import WebsocketConsumer
import serial
import time
from .models import DH11Sensor

# arduino_port = "COM5"
# baud = 115200
# file_name = "serial_data.csv"
# samples = 5

# try:

#     ser = serial.Serial(arduino_port, baud)
#     print("Connected to Arduino port", arduino_port)

#     line = 0

#     while line <= samples:

#         print("Line " + str(line) + ": writing...")
        
#         get_data = ser.readline().decode('utf-8')

#         # split get_data into a list of words and assign to variables
#         data_list = get_data.split(',')
#         temp, humidity, location = data_list[0], data_list[1], data_list[2]

#         print(type(humidity), temp, type(location))

#         # create instance of database model
#         record = DH11Sensor()

#         # add each row to the database
#         record.humidity = humidity
#         record.temperature = temp
#         record.location = location
#         record.save()
    
#         print("Humidity: ", humidity, "\nTemperature: ", temp, "\nLocation: ", location)
        
#         # write the data to a csv file
#         with open(file_name, "a") as file:
#             file.write(get_data)
#             line += 1

#         # add delay between each reading
#         time.sleep(5)

#     # close the serial port after gathering data
#     ser.close()

#     print("Data collection complete")

# except serial.SerialException:
#     print("\nSerial Connection Failed\n")

##################################################

import paho.mqtt.client as mqtt

# the callback for when the client receives a CONNACK response from the server
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    
    # subscribing in on_connect() means that if we lose the connection and reconnect
    # then the subscriptions will be renewed
    client.subscribe("sensor/temp_celsius")

# the callback for when a PUBLISH message is received from the server
def on_message(client, user_data, msg):
    print(msg.topic + " " + str(msg.payload))

    global data
    data = msg
    
    print(f"Data : {data.payload}")

    # print(f"user data: {msg.payload}")


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("broker.mqtt-dashboard.com", 1883, 60)
# client.loop_forever()
client.loop_start()


class ESPConsumer(WebsocketConsumer):

    def connect(self):
        self.accept()

        # self.send(text_data=json.dumps({
        #     'temp': on_message
        # }))


    def receive(self):
        self.send(text_data=json.dumps({
            'temp': data.payload,
            # 'humidity': humidity,
            # 'location': location
        }))

        


class DatabaseConsumer(WebsocketConsumer):

    def connect(self):
        self.accept()

        self.send(text_data=json.dumps({
            'type': 'connection_established',
            'message': 'You are now connected'
        }))