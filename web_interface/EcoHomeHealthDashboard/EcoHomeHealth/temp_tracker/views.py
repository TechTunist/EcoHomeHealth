from turtle import title
from django.shortcuts import render
from django.http import HttpResponse
from .models import DH11Sensor
import plotly.express as px
import plotly.graph_objs as go
from plotly.subplots import make_subplots
import time

import serial




def home(request):

   

    return render(request, 'home.html')

# def home(request):

#     arduino_port = "COM5"
#     baud = 115200
#     file_name = "serial_data.csv"
#     samples = 5

#     try:

#         ser = serial.Serial(arduino_port, baud)
#         print("Connected to Arduino port", arduino_port)

#         line = 0

#         while line <= samples:

#             print("Line " + str(line) + ": writing...")
            
#             get_data = ser.readline().decode('utf-8')

#             # split get_data into a list of words and assign to variables
#             data_list = get_data.split(',')
#             temp, humidity, location = data_list[0], data_list[1], data_list[2]

#             print(type(humidity), temp, type(location))

#             # create instance of database model
#             record = DH11Sensor()

#             # add each row to the database
#             record.humidity = humidity
#             record.temperature = temp
#             record.location = location
#             record.save()
        
#             print("Humidity: ", humidity, "\nTemperature: ", temp, "\nLocation: ", location)
            
#             # write the data to a csv file
#             with open(file_name, "a") as file:
#                 file.write(get_data)
#                 line += 1

#             # add delay between each reading
#             time.sleep(5)

#         # close the serial port after gathering data
#         ser.close()

#         print("Data collection complete")

#     except serial.SerialException:
#         print("\nSerial Connection Failed\n")

    
#     # access database fields
#     database = DH11Sensor.objects.all()

#     # living_room_temps = DH11Sensor.objects.filter(location__startswith='1\n')
#     # for i in range(len(living_room_temps)):
#     #     print(living_room_temps[i].temperature)

#     # query data for each location

#     # living_room = DH11Sensor.objects.filter(location__startswith='1\n').values(
#     # 'temperature', 'humidity', 'time_of_record')

#     # living_room = DH11Sensor.objects.filter(location__startswith='1')
#     # kitchen = DH11Sensor.objects.filter(location__startswith='1')
#     # master_bedroom = DH11Sensor.filter(location__startswith='1')

# ############### chart 1 ###################

#     # plotly.graph_objects
#     fig1 = make_subplots(specs=[[{"secondary_y": True}]])

#     # \n added to end of data sent by ESP01s autmatically
#     fig1.add_trace(
#         go.Scatter(x = [records.time_of_record for records in database if records.location == '1\n'],
#                 y = [records.temperature for records in database if records.location == '1\n'],
#                 name="Temperature"), secondary_y = False
#     )

#     fig1.add_trace(
#         go.Scatter(x = [records.time_of_record for records in database if records.location == '1\n'],
#                 y = [records.humidity for records in database if records.location == '1\n'],
#                 name = "Humidity"), secondary_y = True
#     )

#     # add figure title
#     fig1.update_layout(
#         title_text = "Living Room (Ground Floor)"
#     )

#     # set x-axis title
#     fig1.update_xaxes(
#         title_text = "Time of Measurement"
#     )

#     # set primary y-axis title
#     fig1.update_yaxes(
#         title_text = "Temperature",
#         secondary_y = False
#     )

#     # set secondary y-axis title
#     fig1.update_yaxes(
#         title_text = "Temperature",
#         secondary_y = False
#     )

#     chart1 = fig1.to_html()

#     ############### chart 2 ###################

#         # plotly.graph_objects
#     fig2 = make_subplots(specs=[[{"secondary_y": True}]])

#     fig2.add_trace(
#         go.Scatter(x = [records.time_of_record for records in database if records.location == '2\n'],
#                 y = [records.temperature for records in database if records.location == '2\n'],
#                 name="Temperature"), secondary_y = False
#     )

#     fig2.add_trace(
#         go.Scatter(x = [records.time_of_record for records in database if records.location == '2\n'],
#                 y = [records.humidity for records in database if records.location == '2\n'],
#                 name = "Humidity"), secondary_y = True
#     )

#     # add figure title
#     fig2.update_layout(
#         title_text = "Kitchen (Ground Floor)"
#     )

#     # set x-axis title
#     fig2.update_xaxes(
#         title_text = "Time of Measurement"
#     )

#     # set primary y-axis title
#     fig2.update_yaxes(
#         title_text = "Temperature",
#         secondary_y = False
#     )

#     # set secondary y-axis title
#     fig2.update_yaxes(
#         title_text = "Temperature",
#         secondary_y = False
#     )

#     chart2 = fig2.to_html()

#     ############### chart 3 ###################

#          # plotly.graph_objects
#     fig3 = make_subplots(specs=[[{"secondary_y": True}]])

#     fig3.add_trace(
#         go.Scatter(x = [records.time_of_record for records in database if records.location == '3\n'],
#                 y = [records.temperature for records in database if records.location == '3\n'],
#                 name="Temperature"), secondary_y = False
#     )

#     fig3.add_trace(
#         go.Scatter(x = [records.time_of_record for records in database if records.location == '3\n'],
#                 y = [records.humidity for records in database if records.location == '3\n'],
#                 name = "Humidity"), secondary_y = True
#     )

#     # add figure title
#     fig3.update_layout(
#         title_text = "Master Bedroom (First Floor)"
#     )

#     # set x-axis title
#     fig3.update_xaxes(
#         title_text = "Time of Measurement"
#     )

#     # set primary y-axis title
#     fig3.update_yaxes(
#         title_text = "Temperature",
#         secondary_y = False
#     )

#     # set secondary y-axis title
#     fig3.update_yaxes(
#         title_text = "Temperature",
#         secondary_y = False
#     )

#     chart3 = fig3.to_html()

#     context = {"chart1": chart1,
#                 "chart2": chart2,
#                 "chart3": chart3,
#                 }

#     return render(request, 'home.html', context)
    

####### Add a live stream from the sensors on separate view #######

def liveStream(request):

    context = {}

    return render(request, 'live.html', context)



def lobby(request):

    context = {}

    return render(request, 'lobby.html')
