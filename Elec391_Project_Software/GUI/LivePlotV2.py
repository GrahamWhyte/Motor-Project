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

import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
import matplotlib.pyplot as plt
from matplotlib import animation

def serialPort(x_queue = None, y_queue = None):
    while (True):
        if ser.in_waiting:
            signal = ser.read()
            # print(signal)
            try:
                if signal.decode() == 'm':
                    bytes = ser.read(4)
                    val = struct.unpack('<f', bytes)[0]
                    y_queue.put(val)

                if signal.decode() == 'l':
                    bytes = ser.read(4)
                    val = struct.unpack('<f', bytes)[0]
                    x_queue.put(val)

            except (AttributeError, UnicodeDecodeError):
                pass

        else:
            pass



def animate(i):
    if (~x_queue.empty()):
        mirrorAngle = y_queue.get()
        laserAngle = x_queue.get()

        yValue = np.tan(mirrorAngle)*10
        xValue= np.tan(2*laserAngle - np.pi/2)*10


        if abs(xValue) <100:
            yData.append(mirrorAngle)
            xData.append(laserAngle)

    ax.clear()
    ax.plot(xData, yData)



class App(Tk):

    def __init__(self):

        Tk.__init__(self)
        container = Frame(self)
        container.pack(side="top", fill="both", expand=False)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        circleButton = ttk.Button(self,
                              text="Circle",
                              command=self.send_circle)

        circleButton.pack()

        squareButton = ttk.Button(self,
                              text="Square",
                              command=self.send_square)

        squareButton.pack()


        triangeButton = ttk.Button(self,
                                  text="Triangle",
                                  command=self.send_triangle())

        triangeButton.pack()

        canvas = FigureCanvasTkAgg(fig, self)
        # canvas.show()
        canvas.draw()
        canvas.get_tk_widget().pack(side=BOTTOM, fill=BOTH, expand=True)

        self.protocol("WM_DELETE_WINDOW", self.on_closing)


    # this should work but doesn't
    def on_closing(self):
        self.destroy()
        sys.exit("Shitty exit")

    def send_circle(self):
        ser.write('c'.encode())
        x_queue.queue.clear()
        y_queue.queue.clear()
        xData = []
        yData = []


    def send_square(self):
        ser.write('s'.encode())
        x_queue.queue.clear()
        y_queue.queue.clear()
        xData = []
        yData = []

    def send_triangle(self):
        ser.write('t'.encode())
        x_queue.queue.clear()
        y_queue.queue.clear()
        xData = []
        yData = []

    def send_line(self):
        ser.write('l'.encode())

# Plotting stuff
xData = []
yData = []
fig = plt.figure(figsize=(5, 5), dpi=100)
ax = fig.add_subplot(111)

# Serial stuff
ser = serial.Serial()
ser.port = "COM5"
ser.baudrate = 9600
ser.open()

# Threading stuff
x_queue = queue.Queue()
y_queue = queue.Queue()
threadQueue = threading.Thread(
    target = serialPort,
    kwargs = {'x_queue': x_queue,
              'y_queue': y_queue}
)
# threadQueue.start()
# threadSerial = threading.Thread(target=read_serial_port)

app = App()
ani = animation.FuncAnimation(fig, animate, interval=10)
app.mainloop()