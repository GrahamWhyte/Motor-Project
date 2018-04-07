import tkinter as tk
import threading
# from multiprocessing import Queue
# from Queue import Empty
import queue
import serial
import random

# class SerialPort(threading.Thread):
#
#     def __init__(self, queue):
#         threading.Thread.__init__(self)
#         self.queue = queue
#     def run(self):
#         ser = serial.Serial("COM3", 9600)
        # while(True):
        #     if (ser.in_waiting)>=4:
        #         data = float(ser.read(4))
        #         self.queue.put(data)
            # self.queue.put(random.randint(0, 50))

    # simulate values
    # def get_laser_setpoint(self): return random.randint(0,50)
    # def get_mirror_setpoint(self): return random.randint(50,100)

def thread_test(thread_queue=None):
    thread_queue.put(random.randint(0, 50))

class App(tk.Frame):
    def __init__(self, master):
        tk.Frame.__init__(self)
        self.canvas = tk.Canvas(self)
        self.canvas.pack(side="top", fill="both", expand=True)

        # create lines for laser/mirror
        self.laser_line = self.canvas.create_line(0,0,0,0, fill="red")
        # self.mirror_line = self.canvas.create_line(0,0,0,0, fill="blue")


        self.circleButton = tk.Button(self.canvas,
                                      text="Circle",
                                      command=self.send_circle)

        self.circleButton_window = self.canvas.create_window(250, 250, anchor=tk.SE, window=self.circleButton)

        # Initialize Serial communication
        self.thread_queue = queue.Queue()
        self.thread = threading.Thread(
            target=thread_test,
            kwargs={'thread_queue': self.thread_queue}
        )
        self.thread.start()

        # start the update process
        self.update_plot()

    def update_plot(self):
        while (self.thread_queue.qsize()>0):
            try:
                l = self.thread_queue.get()
                # m = self.servo.get_mirror_setpoint()
                self.add_point(self.laser_line, l)
                # self.add_point(self.mirror_line, m)
                self.canvas.xview_moveto(1.0)
            except queue.Empty:
                pass
        self.after(100, self.update_plot)

    def add_point(self, line, y):
        coords = self.canvas.coords(line)
        x = coords[-2] + 1
        coords.append(x)
        coords.append(y)
        coords = coords[-200:] # keep # of points to a manageable size
        self.canvas.coords(line, *coords)
        self.canvas.configure(scrollregion=self.canvas.bbox("all"))

    def send_circle(self):
        # ser.write(circle.encode())
        print('c\n')

if __name__ == "__main__":

    root = tk.Tk()
    App(root).pack(side="top", fill="both", expand=False)

    root.mainloop()