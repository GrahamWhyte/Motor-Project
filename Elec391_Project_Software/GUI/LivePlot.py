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
import msvcrt

def serial_port(x_queue=None, y_queue=None, stopFlag = 0):

    if ~stopFlag:
        while (True):
            if ser.in_waiting:
                signal = ser.read()
                print(signal)
                try:
                    if signal.decode() == 'm':
                        bytes = ser.read(4)
                        val = struct.unpack('<f', bytes)[0]
                        x_queue.put(val)

                    if signal.decode() == 'l':
                        bytes = ser.read(4)
                        val = struct.unpack('<f', bytes)[0]
                        y_queue.put(val)

                except AttributeError:
                    pass



class App(Frame):
    def __init__(self, master):
        Frame.__init__(self)

        master.geometry('1200x1200+200+100')
        master.title('Motor Thingymaboop')
        master.state('zoomed')
        master.config(background = '#fafafa')


        matplotlib.style.use('ggplot')
        self.fig = plt.figure(figsize=(20, 10), dpi=100)


        self.canvas = FigureCanvasTkAgg(self.fig, root)
        self.canvas.get_tk_widget().grid(column=1, row=1)
        self.canvas.draw()

        self.circleButton = Button(root,
                                   text="Circle",
                                   command=self.send_circle)

        self.circleButton.grid()

        self.squareButton = Button(root,
                                   text="Square",
                                   command=self.send_square)

        self.squareButton.grid()

        self.x_queue = queue.Queue()
        self.y_queue = queue.Queue()
        self.thread = threading.Thread(
            target=serial_port,
            kwargs={'x_queue': self.x_queue,
                    'y_queue': self.y_queue}
        )
        self.thread.start()

        # self.update_plot()
        self.set_up_plot()

    def set_up_plot(self):

        self.y_queue.queue.clear()
        self.x_queue.queue.clear()

        self.xData = []
        self.yData = []
        self.ax1 = self.fig.add_subplot(1, 1, 1)
        self.ax1.set_ylim(-25, 25)
        self.ax1.set_xlim(-25, 25)
        self.line, = self.ax1.plot(self.xData, self.yData, 'r')
        serial_port(stopFlag=0)
        self.update_plot()

    def update_plot(self):

        def animate(i):
            # self.yData.append(random.randint(0, 50))
            if (~self.x_queue.empty()):
                mirrorAngle = self.y_queue.get()*np.pi/180
                laserAngle = self.x_queue.get()*np.pi/180

                yValue = np.tan(mirrorAngle)*10
                xValue= np.tan(2*laserAngle - np.pi/2)*10

                if (abs(xValue)<100):
                    self.yData.append(yValue)
                    self.xData.append(xValue)
                    self.line.set_data(self.xData, self.yData)
                #     # self.ax1.set_xlim(0, i+1)


        ani = animation.FuncAnimation(self.fig, animate, interval=1, blit=False)
        self.canvas.draw()

    def send_circle(self):
        serial_port(stopFlag=1)
        ser.write('c'.encode())
        plt.cla()
        self.set_up_plot()

    def send_square(self):
        serial_port(stopFlag=1)
        ser.write('s'.encode())
        plt.cla()
        self.set_up_plot()

    def send_triangle(self):
        ser.write('t'.encode())
        plt.cla()
        self.set_up_plot()

    def send_line(self):
        ser.write('l'.encode())
        plt.cla()
        self.set_up_plot()

ser = serial.Serial(
    port="COM5",
    baudrate=9600,
    # bytesize=serial.EIGHTBITS,
    # timeout=1
)
ser.close()
ser.open()

# while True:

    # keyInput = input('Enter a char: ')
    # ser.write(keyInput.encode())
    # print(str(ser.read().decode('utf-8')))

root = Tk()

App(root)

root.mainloop()



