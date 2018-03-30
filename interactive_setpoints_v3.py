"""
This file traces your cursor WHILE LEFT BUTTON IS HELD DOWN

It will snap to the grid, and then draw line segments.

Space bar CLOSES the shape, and turns it green. This is bassically a placeholder for a button in the GUI that will
make the data valid and sent it to the arduino, or whatever you want to do with it.

Erasing is not yes implemented, it can be though. Talk to me about this Graham.

TODO:
-implement a more sophisticated set point algo. Why are there so many in a straight line? not necessary
-erasing?
-moving an existing set point?
-this shits slow as fuck after you press space bar
"""

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# PARAMS
DEBUG = True
# grid/setpoint cartesian limits
X_LIM_RIGHT = 13
Y_LIM_RIGHT = 10
X_LIM_LEFT = -13
Y_LIM_LEFT = -10

X_MAJOR_GRID_SPACING = 5
Y_MAJOR_GRID_SPACING = 5

MAX_SET_POINTS = np.inf # Max number of set points we want the user to be able to add

#GLOBALS
set_points = np.array([np.arange(X_LIM_LEFT, X_LIM_RIGHT, 1), np.arange(Y_LIM_LEFT, Y_LIM_RIGHT, 1)])
x_points = []
y_points = []
lines = []
markers = []

pressed = False

cartesian_setpoints = pd.DataFrame(columns=["x", "y"])


class LineDrawer(object):

    """
        Draws a closed loop of the setpoints selected by the mouse
    """
    def draw_line(self, colour="r"):
        global lines
        print("Entering draw_line method")
        ax = plt.gca()
        if len(x_points) > 1:   # Need at least two markers to draw a line
            for i in range(len(lines), len(x_points)-1): ## Add all new lines, dont redraw all existing ones
                print("Drawing segment {}".format(i))
                x = [x_points[i], x_points[i+1]]
                y = [y_points[i], y_points[i+1]]
                line = plt.plot(x,y, color=colour)
                ax.figure.canvas.draw()

                lines.append(line)

    """
        Adds a marker to each set point on the plot, so the user can see the path
    """
    def add_marker(self, x, y, colour="r"):
        global markers
        print("Adding the marker ({}, {})".format(x, y))
        marker = plt.plot(x, y, color=colour, marker="x")
        ax.figure.canvas.draw()
        markers.append(marker)

    def clear_fig(self):
        global lines, markers
        lines = []
        markers = []
        ax = plt.gca()
        # ax.figure.clf()
        ax.lines = []
        ax.figure.canvas.draw()


"""
This event happens with mouse clicks, and the left click has value 1

It makes a list of points that you have pressed, after snapping it to the grid
"""
def onclick(event):
    global pressed
    if event.button == 1:   # left mouse click
        pressed = True

"""

"""
def onrelease(event):
    global pressed
    if event.button == 1:   # left mouse click
        pressed = False

"""
    This event is called when the space bar is pressed and the plot is our current window (not like chrome or something)
"""
def press(event):
    global set_points, x_points, y_points, cartesian_setpoints, lines, markers
    if event.key == " ":    # Check if was spacebar
        print("Pressed space bar, so should be drawing the shape now")
        ld = LineDrawer()   # Get instance of the line drawer
        ld.clear_fig()  # Erase the red lines
        x_points.append(x_points[0])    # Add this point so it closes the picture
        y_points.append(y_points[0])
        ld.draw_line(colour="g")    # draw the closed loop shape defined by checkpoitns IN GREEN
        x_points.pop(-1)    # Remove that artificial point that was just added
        y_points.pop(-1)
        for x, y in zip(x_points, y_points):    # I think it looks good to still have the markers, but this isn't necessary
            ld.add_marker(x, y, colour="g")
    elif event.key == "j" and DEBUG is True:
        set_points = np.array([np.arange(X_LIM_LEFT, X_LIM_RIGHT, 1), np.arange(Y_LIM_LEFT, Y_LIM_RIGHT, 1)])
        x_points = []
        y_points = []
        cartesian_setpoints = pd.DataFrame(columns=["x", "y"])

        ld = LineDrawer()   # Get instance of the line drawer
        ld.clear_fig()
        lines = []
        markers = []


"""

"""
def onmotion(event):
    global cartesian_setpoints, x_points, y_points, pressed
    if pressed is True:
        # snap the point onto the grid we have defined
        temp_x = find_nearest(set_points[0], event.xdata)
        temp_y = find_nearest(set_points[1], event.ydata)
        if len(x_points) < MAX_SET_POINTS:  # add a set point if we havent exceeded the limit of set points
            is_already_setpoint = ((cartesian_setpoints["x"] == temp_x) & (cartesian_setpoints["y"] == temp_y)).any()
            if not is_already_setpoint: # Check if we have already stored this setpoint
                ld = LineDrawer()   # get an instance of the linedrawer class

                # add the snapped point to the list of all set points
                x_points.append(temp_x)
                y_points.append(temp_y)

                # Mark the set point on the graph
                ld.add_marker(temp_x, temp_y)

                # Update our setpoints dataframe with the new set point
                temp_df = pd.DataFrame([[temp_x, temp_y]], columns=["x", "y"])
                cartesian_setpoints = cartesian_setpoints.append(temp_df, ignore_index=True)

                print(cartesian_setpoints)
                ld.draw_line()    # draw our line segment


"""
    returns the closest point in the array to that of "value"
"""
def find_nearest(array,value):
    idx = (np.abs(array-value)).argmin()
    return array[idx]


"""
    Setsup the plot with a grid and all that shit
"""
def init_plot(fig, ax):
    # set up our figure and axies
    ax.set_xticks(set_points[0], minor=True)
    ax.set_xticks(set_points[0][::X_MAJOR_GRID_SPACING])
    ax.set_yticks(set_points[1], minor=True)
    ax.set_yticks(set_points[1][::Y_MAJOR_GRID_SPACING])
    ax.grid(which='minor', alpha=0.2)
    ax.grid(which='major', alpha=0.5)
    plt.xlim((set_points[0][0], set_points[0][-1]))
    plt.ylim((set_points[1][0], set_points[1][-1]))


if __name__ == "__main__":

    fig, ax = plt.subplots()
    init_plot(fig, ax)

    # Set our events for the interactive plot
    connection_id = fig.canvas.mpl_connect('button_press_event', onclick)
    fig.canvas.mpl_connect('key_press_event', press)
    fig.canvas.mpl_connect('motion_notify_event', onmotion)
    fig.canvas.mpl_connect('button_release_event', onrelease)

    # Tight layout is always better
    plt.tight_layout()

    # Make the plot visible
    plt.show()
