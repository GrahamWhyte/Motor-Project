"""
This file is to choose set points live and have the data prepared to be exported to the arduino.

Left click on the plot to create set points. Press the SPACE BAR to commit the set points, lines will then be drawn
between them. The first and last will also be connected, to close the shape.

Press J to delete all set points
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

        # Set our events for the interactive plot
        self.connection_id = self.fig.canvas.mpl_connect('button_press_event', self.onclick)
        self.fig.canvas.mpl_connect('key_press_event', self.press)

        # Tight layout is always better
        plt.tight_layout()

        # Make the plot visible
        plt.show()

    """
        Draws a closed loop of the setpoints selected by the mouse
    """

    def draw_line(self, colour="r"):
        print("Entering draw_line method")
        if len(self.x_points) > 1:  # Need at least two markers to draw a line
            for i in range(len(self.lines),
                           len(self.x_points) - 1):  ## Add all new lines, dont redraw all existing ones
                print("Drawing segment {}".format(i))
                x = [self.x_points[i], self.x_points[i + 1]]
                y = [self.y_points[i], self.y_points[i + 1]]
                plt.figure(1)
                line = plt.plot(x, y, color=colour)
                self.ax.figure.canvas.draw()

                self.lines.append(line)

    """
        Adds a marker to each set point on the plot, so the user can see the path
    """

    def add_marker(self, x, y, colour="r"):
        print("Adding the marker ({}, {})".format(x, y))
        plt.figure(1)
        marker = plt.plot(x, y, color=colour, marker="x")
        self.ax.figure.canvas.draw()
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
            if len(self.x_points) < MAX_SET_POINTS:  # add a set point if we havent exceeded the limit of set points

                # ld = LineDrawer()   # get an instance of the linedrawer class

                # snap the point onto the grid we have defined
                temp_x = self.find_nearest(self.set_points[0], event.xdata)
                temp_y = self.find_nearest(self.set_points[1], event.ydata)

                if temp_x in self.x_points and temp_y in self.y_points:  # delete a marker if its pressed a second time AND ALL MARKERS AFTER
                    if self.x_points.index(temp_x) == self.y_points.index(temp_y):
                        points_to_remove_start_index = self.x_points.index(
                            temp_x)  # find the index where this point lives in our list of markers

                        # Remove the marker, and everyone after is
                        self.x_points = self.x_points[:points_to_remove_start_index]
                        self.y_points = self.y_points[:points_to_remove_start_index]

                        self.cartesian_setpoints = self.cartesian_setpoints[:points_to_remove_start_index]
                        self.clear_fig()

                        # Put back the markers
                        for x, y in zip(self.x_points,
                                        self.y_points):  # I think it looks good to still have the markers, but this isn't necessary
                            self.add_marker(x, y)

                else:  # otherwise add it and draw the line segment
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
                for x, y in zip(self.x_points,
                                self.y_points):  # I think it looks good to still have the markers, but this isn't necessary
                    self.add_marker(x, y, colour="g")
        elif event.key == "j" and DEBUG is True:
            self.set_points = np.array([np.arange(X_LIM_LEFT, X_LIM_RIGHT, 1), np.arange(Y_LIM_LEFT, Y_LIM_RIGHT, 1)])
            self.x_points = []
            self.y_points = []
            self.cartesian_setpoints = pd.DataFrame(columns=["x", "y"])

            self.clear_fig()
            self.lines = []
            self.markers = []

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
        plt.figure(1)
        plt.xlim((self.set_points[0][0], self.set_points[0][-1]))
        plt.ylim((self.set_points[1][0], self.set_points[1][-1]))


if __name__ == "__main__":

    ld = LineDrawer()

    # # Tight layout is always better
    # plt.tight_layout()
    #
    # # Make the plot visible
    # plt.show()
