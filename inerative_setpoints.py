import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# PARAMS
DEBUG = True
X_LIM = 26  # this is the number of x positions we have aka x resolution
Y_LIM = 19  # this is the number of y positions we have aka y resolution
MAX_SET_POINTS = np.inf # Max number of set points we want the user to be able to add

#GLOBALS
set_points = np.array([np.arange(-X_LIM, X_LIM, 1), np.arange(-Y_LIM, Y_LIM, 1)])
x_points = []
y_points = []

cartesian_setpoints = pd.DataFrame(columns=["x", "y"])


class LineDrawer(object):
    lines = []

    """
        Draws a closed loop of the setpoints selected by the mouse
    """
    def draw_line(self, startx,starty):
        print("Entering draw_line method")
        ax = plt.gca()
        for i in range(len(x_points)):
            print("Drawing segment {}".format(i))
            x = [x_points[i-1], x_points[i]]
            y = [y_points[i-1], y_points[i]]
            line = plt.plot(x,y, color="r")
            ax.figure.canvas.draw()

            self.lines.append(line)

    """
        Adds a marker to each set point on the plot, so the user can see the path
    """
    def add_marker(self, x, y):
        print("Adding the marker ({}, {})".format(x, y))
        line = plt.plot(x, y, color="r", marker="x")
        ax.figure.canvas.draw()
        self.lines.append(line)

    def clear_fig(self):
        self.lines = []
        ax = plt.gca()
        # ax.figure.clf()
        ax.lines = []
        ax.figure.canvas.draw()


"""
This event happens with mouse clicks, and the left click has value 1

It makes a list of points that you have pressed, after snapping it to the grid
"""
def onclick(event):
    if event.button == 1:   # left mouse click
        if len(x_points) < MAX_SET_POINTS:  # add a set point if we havent exceeded the limit of set points
            global cartesian_setpoints

            ld = LineDrawer()   # get an instance of the linedrawer class

            # snap the point onto the grid we have defined
            temp_x = find_nearest(set_points[0], event.xdata)
            temp_y = find_nearest(set_points[1], event.ydata)

            # add the snapped point to the list of all set points
            x_points.append(temp_x)
            y_points.append(temp_y)

            # Mark the set point on the graph
            ld.add_marker(temp_x, temp_y)

            # Update our setpoints dataframe with the new set point
            temp_df = pd.DataFrame([[temp_x, temp_y]], columns=["x", "y"])
            cartesian_setpoints = cartesian_setpoints.append(temp_df, ignore_index=True)
            print(cartesian_setpoints)

        else:   #   draw the shape if the limit was hit
            ld = LineDrawer()   # Get instance of the line drawer
            ld.draw_line(x_points, y_points)    # draw our closed loop shape

"""
    This event is called when the space bar is pressed and the plot is our current window (not like chrome or something)
"""
def press(event):
    global set_points, x_points, y_points, cartesian_setpoints
    if event.key == " ":    # Check if was spacebar
        print("Pressed space bar, so should be drawing the shape now")
        ld = LineDrawer()   # Get instance of the line drawer
        ld.draw_line(x_points, y_points)    # draw the closed loop chape defined by checkpoitns
    elif event.key == "j" and DEBUG == True:
        set_points = np.array([np.arange(-X_LIM, X_LIM, 1), np.arange(-Y_LIM, Y_LIM, 1)])
        x_points = []
        y_points = []
        cartesian_setpoints = pd.DataFrame(columns=["x", "y"])

        ld = LineDrawer()   # Get instance of the line drawer
        ld.clear_fig()


"""
    returns the closest point in the array to that of "value"
"""
def find_nearest(array,value):
    idx = (np.abs(array-value)).argmin()
    return array[idx]

# set up our figure and axies
fig, ax = plt.subplots()
ax.set_xticks(set_points[0], minor=True)
ax.set_xticks(set_points[0][::5])
ax.set_yticks(set_points[1], minor=True)
ax.set_yticks(set_points[1][::5])
ax.grid(which='minor', alpha=0.2)
ax.grid(which='major', alpha=0.5)
plt.xlim((set_points[0][0], set_points[0][-1]))
plt.ylim((set_points[1][0], set_points[1][-1]))

# Set our events for the interactive plot
connection_id = fig.canvas.mpl_connect('button_press_event', onclick)
fig.canvas.mpl_connect('key_press_event', press)

# Tight layout is always better
plt.tight_layout()

# Make the plot visible
plt.show()