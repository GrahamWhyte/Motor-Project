from tkinter import *
import matplotlib.pyplot as plt
from matplotlib import animation
import matplotlib.style
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import queue
import threading
import serial
import struct
import time

def serial_port(x_queue=None, y_queue=None):

    while (True):
        bytes = ser.read(4)
        val = struct.unpack('<f', bytes)[0]
        x_queue.put(val)
        bytes = ser.read(4)
        val = struct.unpack('<f', bytes)[0]
        y_queue.put(val)

class App(Frame):
    def __init__(self, master):
        Frame.__init__(self)

        master.geometry('1500x700+200+100')
        master.title('Motor Thingymaboop')
        # master.state('zoomed')
        master.config(background = '#fafafa')

        self.xData = []
        self.yData = []

        matplotlib.style.use('ggplot')
        self.fig = plt.figure(figsize=(14, 4.5), dpi=100)
        self.ax1 = self.fig.add_subplot(1, 1, 1)
        self.ax1.set_ylim(-15, 15)
        self.ax1.set_xlim(-15, 15)
        self.line, = self.ax1.plot(self.xData, self.yData, 'r', marker='o')



        self.canvas = FigureCanvasTkAgg(self.fig, root)
        self.canvas.get_tk_widget().grid(column=1, row=1)
        self.canvas.draw()

        self.circleButton = Button(root,
                                   text="Circle",
                                   command=self.send_circle)

        self.circleButton.grid()

        # self.circleButton_window = self.canvas.create_window(250, 250, anchor=SE, window=self.circleButton)

        self.x_queue = queue.Queue()
        self.y_queue = queue.Queue()
        self.thread = threading.Thread(
            target=serial_port,
            kwargs={'x_queue': self.x_queue,
                    'y_queue': self.y_queue}
        )
        self.thread.start()

        self.update_plot()

    def update_plot(self):

        def animate(i):
            # self.yData.append(random.randint(0, 50))
            if (~self.x_queue.empty()):
                yValue = self.y_queue.get()*np.pi/180
                xValue = self.x_queue.get()*np.pi/180

                self.yData.append(np.tan(yValue)*10)
                self.xData.append(np.tan(2*xValue-np.pi/2)*10)
                self.line.set_data(self.xData, self.yData)
                # self.ax1.set_xlim(0, i+1)

        ani = animation.FuncAnimation(self.fig, animate, interval=10, blit=False)
        self.canvas.draw()

    def send_circle(self):
        print('c')


ser = serial.Serial(
    port="COM5",
    baudrate=9600,
    # bytesize=serial.EIGHTBITS,
    # timeout=1
)
ser.close()
ser.open()
time.sleep(1)


root = Tk()

App(root)

root.mainloop()


