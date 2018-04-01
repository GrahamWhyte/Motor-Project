import tkinter as tk
import serial
import serial.tools.list_ports
import matplotlib as mpl
import numpy as np
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
import random

class App:
    def __init__(self, master):

        # Create Tkinter frame
        frame = tk.Frame(master)

        self.data = SerialPort()

        self.circleButton = tk.Button(frame,
                                      text="Circle",
                                      command=self.send_circle)

        self.circleButton.pack(side="left")

        self.squareButton = tk.Button(frame,
                                      text="Square",
                                      command=self.send_square)

        self.squareButton.pack(side="left")

        # Plot containing setpoints for both laser and mirror
        # self.fig_shape = Figure()
        # self.ax_shape = self.fig_shape.add_subplot(211)

        # fig_setpoint_x = Figure()
        # ax_setpoint_x = fig_setpoint_x.add_subplot(211)

        self.canvas_shape = tk.Canvas(self, background="black")
        self.canvas_shape.pack(side="top", fill="both", expand=1)

        self.laserLine = self.canvas_shape.create_line(0,0,0,0, fill="red")
        self.mirrorLine = self.canvas_shape.create_line(0,0,0,0, fill="blue")

        # self.canvas_x = FigureCanvasTkAgg(fig_setpoint_x, master=master)
        # self.canvas_x.show()
        # self.canvas_x.get_tk_widget().pack(side="top", fill="both", expand=1)

        frame.pack()

    def send_circle(self):
        ser.write(circle.encode())

    def send_square(self):
        ser.write(square.encode())

    def send_triangle(self):
        ser.write(triangle.encode())

    def send_line(self):
        ser.write(line.encode())

    def update_plot(self):
        l = self.data.getLaser()
        m = self.data.getMirror()
        self.addPoint(self.laserLine, l)
        self.addPoint(self.mirrorLine, m)
        self.canvas_shape.xview_moveto(1.0)
        self.after(100, self.update_plot)

    def addPoint(self, line, y):
        coords = self.canvas_shape.coords(y)
        x = coords[-2] + 1
        coords.append(x)
        coords.append(y)
        coords = coords[-200:]
        self.canvas_shape.coords(line, *coords)
        self.canvas_shape.configure(scrollregion=self.canvas_shape.bbox("all"))


class SerialPort:

    # def __init__(self):
    #     self.ser = serial.Serial()
    #     ser.port = find_com_port()
    #     ser.baudrate = 9600
    #     ser.open()
    #
    # def find_com_port(self):
    #     ports = serial.tools.list_ports.comports()
    #     for port in ports:
    #         if 'Arduino' in port.description:
    #             return port
    #     return 0
    #
    # def readFloat(self):
    #     if (ser.in_waiting>=4):
    #         return ser.read(4)

    def getLaser(self): return random.randint(0, 50)
    def getMirror(self): return random.randInt(0, 50)

circle = 'c'
square = 's'
triangle = 't'
line = 'l'

root = tk.Tk()
root.title("Lasers and Stuff")

app = App(root)
root.mainloop()

# serialPort = SerialPort()
# serialPort.find_com_port()

# input = [1, 2, 3, 4, 5]
# output = [10, 20, 30, 40, 50]
# app.plot(input, output)
# while(True):
    # test = serialPort.readFloat()
