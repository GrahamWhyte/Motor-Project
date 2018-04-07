# Search FILL THIS IS and fill them in

from getSerialPorts import getSerealPorts

from tkinter import *
from tkinter import ttk
# import matplotlib.pyplot as plt
# from matplotlib import animation
import matplotlib.style
import numpy as np
import queue
import threading
import serial
import struct

import sys
import pandas as pd

import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
import matplotlib.pyplot as plt
from matplotlib import animation

import re


# PARAMS
DEBUG = True
SERIAL = True
KEYBOARD_EVENTS = False

# grid/setpoint cartesian limits
X_LIM_RIGHT = 13
Y_LIM_RIGHT = 19
X_LIM_LEFT = -13
Y_LIM_LEFT = 6

X_MAJOR_GRID_SPACING = 5
Y_MAJOR_GRID_SPACING = 3

MAX_SET_POINTS = 100 # Max number of set points we want the user to be able to add


def arduino_data_prep(type: int, data: list):
    if isinstance(data[0], float):
        bytes = bytearray([type, len(data)*4])
        for val in data:
            temp = bytearray(struct.pack("f", val))
            bytes.extend(temp)
            print("{} -> {} -> {}".format(val, temp, struct.unpack("f", temp)))
    else:
        bytes = bytearray([type, len(data)])
        bytes.extend(bytearray(data))
    return bytes

def cartesion_to_degree(cartesian):

    # do math, indexes in the datafram are "x" and "y"

    # FILL THIS IN
    df_degree = pd.DataFrame(columns=["x", "y"])
    return df_degree

class StdoutRedirector(object):
    def __init__(self,text_widget):
        self.text_space = text_widget

    def write(self,string):
        self.text_space.insert('end', string)
        self.text_space.see('end')

class App(Tk):

    def __init__(self):

        Tk.__init__(self)
        # container = Frame(self)
        # container.pack(side="top", fill="both", expand=False)
        # container.grid_rowconfigure(0, weight=1)
        # container.grid_columnconfigure(0, weight=1)

        self.winfo_toplevel().title("B10 Laser Light Show Controller")

        self.commands = {
            "shape"     :   0,
            "speed"     :   1,
            "custom"    :   2
        }

        self.ld = LineDrawer()

        self.canvas = FigureCanvasTkAgg(self.ld.fig, self)
        self.canvas.draw()
        # self.canvas.get_tk_widget().pack(side=BOTTOM, fill=BOTH, expand=True)
        self.canvas.get_tk_widget().grid(row=5, columnspan=4)

        # Set our events for the interactive plot
        self.canvas.mpl_connect('button_press_event', self.onclick)
        if KEYBOARD_EVENTS:
            self.canvas.mpl_connect('key_press_event', self.press)

        circleButton = ttk.Button(self,
                              text="Circle",
                              command=self.send_circle)

        # circleButton.pack(side = BOTTOM)
        circleButton.grid(row = 0, column=0, sticky="ew")

        squareButton = ttk.Button(self,
                              text="Square",
                              command=self.send_square)

        # squareButton.pack(side = BOTTOM)
        squareButton.grid(row=0, column=1, columnspan=2, sticky="ew")

        triangeButton = ttk.Button(self,
                                  text="Triangle",
                                  command=self.send_triangle)

        # triangeButton.pack(side = BOTTOM)
        triangeButton.grid(row=0, column=3, sticky="ew")

        commit_shapeButton = ttk.Button(self,
                              text="Commit Shape",
                              command=self.commit_shape)

        # commit_shapeButton.pack(side = BOTTOM, fill = X)
        commit_shapeButton.grid(row=1, columnspan=2, sticky="ew")

        deleteButton = ttk.Button(self,
                              text="Delete",
                              command=self.delete)

        # deleteButton.pack(side = BOTTOM, fill = X)
        deleteButton.grid(row=1, column=2, columnspan=2, sticky="ew")

        # Serial port drop down
        self.comMenuVar = StringVar(self)
        self.comMenuVar.set("Select COM Port...")  # default value

        ports = getSerealPorts()
        if ports == []:
            ports = ["None Available"]
        serialPorts = OptionMenu(self, self.comMenuVar, ports)
        serialPorts.grid(row=4, column=0, columnspan=3, sticky="ew")

        # When you press the X out button this should kill the python code not just the gui
        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        # Serial port conenct button
        COMButton = ttk.Button(self,
                              text="Connect!",
                              command=self.init_com)

        # deleteButton.pack(side = BOTTOM, fill = X)
        COMButton.grid(row=4, column=3, sticky="ew")

        self.ser = serial.Serial()

        # grahams speed menu
        speedMenu = Label(self, text="Select Speed: ", anchor="w")
        speedMenu.grid(row=2)

        speeds = list(np.arange(0.1, 1, 0.1))

        self.speedMenuVar = StringVar(self)
        self.speedMenuVar.set(str(speeds[0]))  # default value

        speedOptions = OptionMenu(self, self.speedMenuVar, command=self.set_speed, *speeds)
        speedOptions.grid(row=2, column=1, columnspan=3, sticky="ew")


        if DEBUG:
            self.text_box = Text(self, wrap='word', height=11)
            # self.text_box.pack(side = BOTTOM, fill = X)
            self.text_box.grid(row=6, columnspan=4, rowspan=2)
            sys.stdout = StdoutRedirector(self.text_box)


    def set_speed(self, wut):
        print("Preparing to send the speed")
        speed = float(self.speedMenuVar.get())
        self.command = "speed"
        self.data = [speed]
        self.send_shit()

    def init_com(self):
        # Serial stuff
        if SERIAL:
            port = re.search("COM\d+", self.comMenuVar.get())
            port = port.group(0)
            print(port)
            self.ser.port = port
            self.ser.baudrate = 9600
            try:
                self.ser.open()
                print("Connection to {} SUCCESSUL".format(port))
            except:
                print("Connection to {} UNSUCCESSUL".format(port))

    # this should work but doesn't
    def on_closing(self):
        self.destroy()
        sys.exit("Shitty exit")

    def send_circle(self):
        self.command = "shape"
        self.data = [ord("c")]
        self.send_shit()
        x_queue.queue.clear()
        y_queue.queue.clear()
        xData = []
        yData = []


    def send_square(self):
        self.command = "shape"
        self.data = [ord("s")]
        self.send_shit()
        x_queue.queue.clear()
        y_queue.queue.clear()
        xData = []
        yData = []

    def send_triangle(self):
        self.command = "shape"
        self.data = [ord("t")]
        self.send_shit()
        x_queue.queue.clear()
        y_queue.queue.clear()
        xData = []
        yData = []

    def send_line(self):
        self.command = "shape"
        self.data = [ord("l")]
        self.send_shit()

    def press(self, event):
        self.ld.press(event)
        self.canvas.draw()

    def onclick(self, event):
        self.ld.onclick(event)
        self.canvas.draw()

    def commit_shape(self):
        self.ld.commit_shape()
        self.canvas.draw()
        self.command = "custom"
        self.send_shit()

    def delete(self):
        self.ld.delete()
        self.canvas.draw()

    def send_shit(self):
        transmissions = []
        if self.command == "shape":
            transmissions.append(arduino_data_prep(self.commands[self.command], self.data))
        elif self.command == "speed":
            transmissions.append(arduino_data_prep(self.commands[self.command], self.data))
        elif self.command == "custom":
            laser_angles = [l*3.6 for l in self.ld.cartesian_setpoints["x"]]
            mirror_angles = [l*3.6 for l in self.ld.cartesian_setpoints["y"]]
            transmissions.append(arduino_data_prep(self.commands[self.command], laser_angles))
            transmissions.append(arduino_data_prep(self.commands[self.command], mirror_angles))

        for transmission in transmissions:
            print("Preparing to send a transmission of {} -> {}".format(self.command, self.commands[self.command]))
            print("Sending the following string of data: {}".format(transmission))
            if SERIAL:
                self.ser.write(transmission)
                # while(self.ser.in_waiting):
                #     bytes = self.ser.read()
                #     print(bytes)

                # bytes = self.ser.read(4)
                # print(bytes)
                # bytes = self.ser.read()
                # print(bytes)
                # bytes = self.ser.read()
                # print(bytes)


class LineDrawer(object):

    def __init__(self):

        self.set_points = np.array([np.arange(X_LIM_LEFT, X_LIM_RIGHT, 1), np.arange(Y_LIM_LEFT, Y_LIM_RIGHT, 1)])
        self.x_points = []
        self.y_points = []

        self.markers = []
        self.lines = []

        self.cartesian_setpoints = pd.DataFrame(columns=["x", "y"])

        self.fig, self.ax = plt.subplots()
        self.init_plot()


    """
        Draws a closed loop of the setpoints selected by the mouse
    """

    def draw_line(self, colour="r"):
        print("Entering draw_line method")
        if len(self.x_points) > 1:  # Need at least two markers to draw a line
            for i in range(len(self.lines), len(self.x_points) - 1):  ## Add all new lines, dont redraw all existing ones
                print("Drawing segment {}".format(i))
                x = [self.x_points[i], self.x_points[i + 1]]
                y = [self.y_points[i], self.y_points[i + 1]]
                line = self.ax.plot(x, y, color=colour)
                # self.ax.figure.canvas.draw()

                self.lines.append(line)

    """
        Adds a marker to each set point on the plot, so the user can see the path
    """

    def add_marker(self, x, y, colour="r"):
        print("Adding the marker ({}, {})".format(x, y))
        marker = self.ax.plot(x, y, color=colour, marker="x")
        # self.ax.figure.canvas.draw()
        self.markers.append(marker)

    def clear_fig(self):
        self.lines = []
        self.markers = []
        self.ax.lines = []
        self.ax.figure.canvas.draw()

    """
    This event happens with mouse clicks, and the left click has value 1

    It makes a list of points that you have pressed, after snapping it to the grid
    """

    def onclick(self, event):
        if event.button == 1:  # left mouse click
            print("left mouse click")
            if len(self.x_points) < MAX_SET_POINTS:  # add a set point if we havent exceeded the limit of set points

                # snap the point onto the grid we have defined
                temp_x = self.find_nearest(self.set_points[0], event.xdata)
                temp_y = self.find_nearest(self.set_points[1], event.ydata)

                # add the snapped point to the list of all set points
                self.x_points.append(temp_x)
                self.y_points.append(temp_y)

                # Mark the set point on the graph
                self.add_marker(temp_x, temp_y)

                # Update our setpoints dataframe with the new set point
                temp_df = pd.DataFrame([[temp_x, temp_y]], columns=["x", "y"])
                self.cartesian_setpoints = self.cartesian_setpoints.append(temp_df, ignore_index=True)

                print(self.cartesian_setpoints)
                self.draw_line()  # draw our line segment


            else:  # draw the shape if the limit was hit
                self.draw_line()  # draw our closed loop shape
        elif event.button == 3:
            print("right mouse click")
            # snap the point onto the grid we have defined
            temp_x = self.find_nearest(self.set_points[0], event.xdata)
            temp_y = self.find_nearest(self.set_points[1], event.ydata)
            deleted = False

            if ((self.cartesian_setpoints["x"] == temp_x) & (self.cartesian_setpoints[
                                                                 "y"] == temp_y)).any():  # delete a marker if its pressed a second time AND ALL MARKERS AFTER
                deleted = True
                points_to_remove_start_index = \
                self.cartesian_setpoints.index[self.cartesian_setpoints["x"] == temp_x].tolist()[
                    -1]  # find the index where this point lives in our list of markers

                # Remove the marker, and everyone after is
                self.x_points = self.x_points[:points_to_remove_start_index]
                self.y_points = self.y_points[:points_to_remove_start_index]

                self.cartesian_setpoints = self.cartesian_setpoints[:points_to_remove_start_index]
                self.clear_fig()

                # Put back the markers
                for x, y in zip(self.x_points,
                                self.y_points):  # I think it looks good to still have the markers, but this isn't necessary
                    self.add_marker(x, y)


    """
        This event is called when the space bar is pressed and the plot is our current window (not like chrome or something)
    """

    def press(self, event):
        if event.key == " ":  # Check if was spacebar
            if len(self.x_points) > 0:
                print("Pressed space bar, so should be drawing the shape now")
                self.clear_fig()  # Erase the red lines
                self.x_points.append(self.x_points[0])  # Add this point so it closes the picture
                self.y_points.append(self.y_points[0])
                self.draw_line(colour="g")  # draw the closed loop shape defined by checkpoitns IN GREEN
                self.x_points.pop(-1)  # Remove that artificial point that was just added
                self.y_points.pop(-1)
                for x, y in zip(self.x_points, self.y_points):  # I think it looks good to still have the markers, but this isn't necessary
                    self.add_marker(x, y, colour="g")
        elif event.key == "d" and DEBUG is True:
            self.set_points = np.array([np.arange(X_LIM_LEFT, X_LIM_RIGHT, 1), np.arange(Y_LIM_LEFT, Y_LIM_RIGHT, 1)])
            self.x_points = []
            self.y_points = []
            self.cartesian_setpoints = pd.DataFrame(columns=["x", "y"])

            self.clear_fig()
            self.lines = []
            self.markers = []

    def delete(self):
        self.set_points = np.array([np.arange(X_LIM_LEFT, X_LIM_RIGHT, 1), np.arange(Y_LIM_LEFT, Y_LIM_RIGHT, 1)])
        self.x_points = []
        self.y_points = []
        self.cartesian_setpoints = pd.DataFrame(columns=["x", "y"])

        self.clear_fig()
        self.lines = []
        self.markers = []

    def commit_shape(self):
        if len(self.x_points) > 0:
            print("Pressed space bar, so should be drawing the shape now")
            self.clear_fig()  # Erase the red lines
            self.x_points.append(self.x_points[0])  # Add this point so it closes the picture
            self.y_points.append(self.y_points[0])
            self.draw_line(colour="g")  # draw the closed loop shape defined by checkpoitns IN GREEN
            self.x_points.pop(-1)  # Remove that artificial point that was just added
            self.y_points.pop(-1)
            for x, y in zip(self.x_points,
                            self.y_points):  # I think it looks good to still have the markers, but this isn't necessary
                self.add_marker(x, y, colour="g")


    """
        returns the closest point in the array to that of "value"
    """

    def find_nearest(self, array, value):
        idx = (np.abs(array - value)).argmin()
        return array[idx]

    """
        Setsup the plot with a grid and all that shit
    """

    def init_plot(self):
        # set up our figure and axies
        self.ax.set_xticks(self.set_points[0], minor=True)
        self.ax.set_xticks(self.set_points[0][::X_MAJOR_GRID_SPACING])
        self.ax.set_yticks(self.set_points[1], minor=True)
        self.ax.set_yticks(self.set_points[1][::Y_MAJOR_GRID_SPACING])
        self.ax.grid(which='minor', alpha=0.2)
        self.ax.grid(which='major', alpha=0.5)
        # plt.figure(1)
        self.ax.set_xlim((self.set_points[0][0], self.set_points[0][-1]))
        self.ax.set_ylim((self.set_points[1][0], self.set_points[1][-1]))
        # plt.autoscale(False)

    def test(self):
        self.add_marker(0, 0)

# Plotting stuff
xData = []
yData = []
fig = plt.figure(figsize=(5, 5), dpi=100)
ax = fig.add_subplot(111)
#
# # Serial stuff
# ser = serial.Serial()
# if SERIAL:
#     ser.port = "COM5"
#     ser.baudrate = 9600
#     ser.open()

# Threading stuff
x_queue = queue.Queue()
y_queue = queue.Queue()

# threadQueue.start()
# threadSerial = threading.Thread(target=read_serial_port)

app = App()
app.mainloop()