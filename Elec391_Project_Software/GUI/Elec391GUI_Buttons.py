from tkinter import *
from tkinter import ttk

import glob
import serial
import serial.tools.list_ports

def getSerealPorts():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result



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


        triangleButton = ttk.Button(self,
                                    text="Triangle",
                                    command=self.send_triangle)

        triangleButton.pack()

        self.speedSlider = Scale(self,
                            from_=1,
                            to=1000,
                            tickinterval=100,
                            orient = HORIZONTAL,
                            length=1000,
                            command=self.handle_slider
                            )

        self.speedSlider.set(500)

        self.speedSlider.pack()


    def send_circle(self):
        # ser.write('A'.encode())
        ser.write('c'.encode())

    def send_square(self):
        # ser.write('A'.encode())
        ser.write('s'.encode())

    def send_triangle(self):
        # ser.write('A'.encode())
        ser.write('t'.encode())

    def send_line(self):
        # ser.write('A'.encode())
        ser.write('l'.encode())

    def handle_slider(self, arg):
        val = float(self.speedSlider.get())
        newVal = min(slider_vals, key=lambda x: abs(x-val))
        self.speedSlider.set(newVal)
        print(newVal)
        # ser.write('f'.encode())
        # ser.write(bytes(newVal))


ser = serial.Serial()
ser.port = getSerealPorts()[0]
ser.baudrate = 9600
ser.open()

slider_vals = [10, 12.5, 25, 50, 100, 500, 1000]

app = App()
app.mainloop()
