"""
Usage:
   init.py serial [--serial-port PORT] [--storage-dir DIR]
   init.py fs [--storage-dir DIR]

Options:
   -h --help                       show this
   -p PORT --serial-port=PORT      for defining the dev file for the connected Arduino board [default: /dev/ttyACM0]
   -d DIR --storage-dir=DIR        for defining the directory that contains the sensor data to work with e.g. the root directory of the sd card of the Arduino [default: ../data/]

"""

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

from docopt import docopt
from dcollector import serial_read, fs_read


humidity_list = []
moisture_list = []
brightness_list = []
temperature_list = []

if __name__ == "__main__":
    args = docopt(__doc__)

    if args["serial"]:
        print("serial")
    elif args["fs"]:
        print("fs")
        data_entry_list = fs_read(args["--storage-dir"])

        for entry in data_entry_list:
            humidity_list.append(entry.humidity)
            moisture_list.append(entry.moisture)
            brightness_list.append(entry.brightness)
            temperature_list.append(entry.temperature)

            # print("--------------------")
            # print("moisture: \t" + str(entry.moisture))
            # print("brightness: \t" + str(entry.brightness))
            # print("humidity: \t" + str(entry.humidity))
            # print("temperature: \t" + str(entry.temperature))
            # print("Second: " + str(entry.log_time.Second))
            # print("Minute: " + str(entry.log_time.Minute))
            # print("Hour: \t" + str(entry.log_time.Hour))
            # print("Wday: \t" + str(entry.log_time.Wday))
            # print("Day: \t" + str(entry.log_time.Day))
            # print("Month: \t" + str(entry.log_time.Month))
            # print("Year: \t" + str(entry.log_time.Year))

        print("###################################")

        # # Make a data frame
        # df=pd.DataFrame({'x': range(0,len(data_entry_list)), 'y1': humidity_list, 'y2': moisture_list, 'y3': brightness_list, 'y4': temperature_list})

        # # Initialize the figure
        # plt.style.use('seaborn-darkgrid')
        
        # # create a color palette
        # palette = plt.get_cmap('Set1')
        
        # # multiple line plot
        # num=0
        # for column in df.drop('x', axis=1):
        #     num+=1
        
        #     # Find the right spot on the plot
        #     plt.subplot(3,3, num)
        
        #     # Plot the lineplot
        #     plt.plot(df['x'], df[column], marker='', color=palette(num), linewidth=1.9, alpha=0.9, label=column)
        
        #     # Same limits for everybody!
        #     plt.xlim(0,100)
        #     plt.ylim(-2,200)
        
        #     # Not ticks everywhere
        #     if num in range(7) :
        #         plt.tick_params(labelbottom='off')
        #     if num not in [1,4,7] :
        #         plt.tick_params(labelleft='off')
        
        #     # Add title
        #     plt.title(column, loc='left', fontsize=12, fontweight=0, color=palette(num) )
        
        # # general title
        # plt.suptitle("How the 9 students improved\nthese past few days?", fontsize=13, fontweight=0, color='black', style='italic', y=1.02)
        
        # # Axis title
        # plt.text(0.5, 0.02, 'Time', ha='center', va='center')
        # plt.text(0.06, 0.5, 'Note', ha='center', va='center', rotation='vertical')

        # plt.show()

        # Make a data frame
        df=pd.DataFrame({'x': range(0,len(data_entry_list)), 'humidity': humidity_list, 'moisture': moisture_list, 'brightness': brightness_list, 'temperature': temperature_list})
        
        # style
        plt.style.use('seaborn-darkgrid')
        
        # create a color palette
        palette = plt.get_cmap('Set1')
        
        # multiple line plot
        num=0
        for column in df.drop('x', axis=1):
            num+=1
            plt.plot(df['x'], df[column], marker='', color=palette(num), linewidth=1, alpha=0.9, label=column)
        
        # Add legend
        plt.legend(loc=2, ncol=2)
        
        # Add titles
        plt.title("A (bad) Spaghetti plot", loc='left', fontsize=12, fontweight=0, color='orange')
        plt.xlabel("Time")
        plt.ylabel("Score")

        plt.show()

    print(args)
