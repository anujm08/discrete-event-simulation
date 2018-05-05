import numpy as np
import sys
import os
import matplotlib
import scipy.stats as st
matplotlib.use('Agg')
import matplotlib.pyplot as plt

def throughput_plot(data):
    plt.figure()
    plot_lines = []
    x = [val[0] for val in data]
    y1 = [val[1] for val in data]
    y2 = [val[2] for val in data]
    y3 = [val[3] for val in data]
    line, = plt.plot(x, y3, marker = "+", label = "Throughput")
    plot_lines.append(line)

    line, = plt.plot(x, y1, marker = "*", label = "Goodput")
    plot_lines.append(line)

    line, = plt.plot(x, y2, marker = "o", label = "Badput")
    plot_lines.append(line)

    plt.xlim(xmax=180)
    plt.xlabel("Num of Users")
    plt.ylabel("Req Completed / second")
    plt.legend(handles = plot_lines, loc = 'best')
    plt.savefig("graphs/throughput.png", bbox_inches = "tight")

def droprate_plot(data):
    plt.figure()
    plot_lines = []
    x = [val[0] for val in data]
    y1 = [val[4] for val in data]
    line, = plt.plot(x, y1, marker = "*", label = "Drop Rate")
    plot_lines.append(line)

    plt.xlabel("Num of Users")
    plt.ylabel("Drop Rate (req / sec)")
    plt.savefig("graphs/droprate.png", bbox_inches = "tight")

def lossprob_plot(data):
    plt.figure()
    plot_lines = []
    x = [val[0] for val in data]
    y1 = [val[5] for val in data]
    line, = plt.plot(x, y1, marker = "*", label = "Loss Probability")
    plot_lines.append(line)

    plt.xlabel("Num of Users")
    plt.ylabel("Loss Probability")
    plt.savefig("graphs/lossprob.png", bbox_inches = "tight")


def resptime_plot(data):
    plt.figure()
    plot_lines = []
    # print CI, z_val
    x = [val[0] for val in data]
    y1 = [val[6] for val in data]

    plt.plot(x, y1, marker = "*")
    plot_lines.append(line)

    plt.xlim(xmax=180)
    plt.xlabel("Num of Users")
    plt.ylabel("Response Time (in s)")
    plt.savefig("graphs/resptime.png", bbox_inches = "tight")

def resptime_CI_plot(data, CI = .95, count = 10):
    plt.figure()
    plot_lines = []
    z_val = st.norm.ppf(CI + (1-CI)/2)
    # print CI, z_val
    x = [val[0] for val in data]
    y1 = [val[6] for val in data]
    yerr = [z_val * ((val[7] / count) ** 0.5) for val in data]
    plt.errorbar(x, y1, yerr = yerr)
    plot_lines.append(line)

    plt.xlim(xmax=180)
    plt.xlabel("Num of Users")
    plt.ylabel("Response Time (in s)")
    plt.savefig("graphs/resptime_{}.png".format(CI), bbox_inches = "tight")

def cores_plot(data):
    plt.figure()
    plot_lines = []
    x = [val[0] for val in data]
    y1 = [val[8] for val in data]
    line, = plt.plot(x, y1, marker = "*", label = "Average Core Utilization")
    plot_lines.append(line)

    plt.xlim(xmax=180)
    plt.xlabel("Num of Users")
    plt.ylabel("Average Core Utilization (out of 8)")
    plt.savefig("graphs/cores.png", bbox_inches = "tight")

def numreqs_plot(data):
    plt.figure()
    plot_lines = []
    x = [val[0] for val in data]
    y1 = [val[9] for val in data]
    line, = plt.plot(x, y1, marker = "*", label = "Average Num of Requests")
    plot_lines.append(line)

    plt.xlim(xmax=180)
    plt.xlabel("Num of Users")
    plt.ylabel("Average Num of Requests")
    # plt.legend(handles = plot_lines, loc = 'best')
    plt.savefig("graphs/numreqs.png", bbox_inches = "tight")

def util_law_plot(data):
    plt.figure()
    plot_lines = []
    x = [val[0] for val in data]
    y1 = [val[8] / val[3] for val in data]
    line, = plt.plot(x, y1, marker = "*", label = "Average Service Time")
    plot_lines.append(line)

    plt.xlim(xmax=180)
    plt.xlabel("Num of Users")
    # plt.ylabel("Average Service Time (Utilization law)")
    plt.legend(handles = plot_lines, loc = 'best')
    plt.savefig("graphs/utillaw.png", bbox_inches = "tight")

def little_law_plot(data):
    plt.figure()
    plot_lines = []
    x = [val[0] for val in data]
    y1 = [val[9] for val in data]
    y2 = [val[6] * val[3] for val in data]

    line, = plt.plot(y1, y2, marker = "*")
    plot_lines.append(line)
    plt.xlabel("Number of Requests (N)")
    plt.ylabel("Resp Time * Throughput (R * " + u"\u03BB" + ")")
    plt.savefig("graphs/littlelaw.png", bbox_inches = "tight")

if __name__ == '__main__':
    if not os.path.exists('graphs'):
        os.makedirs('graphs')
    results = []
    query = ''
    size = 0

    gput  = 0.0
    bput = 0.0
    tput = 0.0
    droprate = 0.0
    lossprob = 0.0
    grtime = 0.0
    brtime = 0.0
    rtime = 0.0
    rtime_sq = 0.0
    cores = 0.0
    reqs = 0.0
    count = 0

    with open('results.txt', 'r') as resultsFile:
        for line in resultsFile:
            values = [val for val in line.split()]
            # print values

            if size != int(values[0]):
                if size != 0:
                    rtime_var = 1.0 * count / (count - 1) * ((rtime_sq / count) - ( (rtime / count) ** 2))
                    results.append([size, gput/count, bput/count, tput/count, droprate/count, lossprob/count, rtime/count, rtime_var, cores/count, reqs/count])
                size = 0
                gput = 0.0
                bput = 0.0
                tput = 0.0
                droprate = 0.0
                lossprob = 0.0
                grtime = 0.0
                brtime = 0.0
                rtime = 0.0
                rtime_sq = 0.0
                cores = 0.0
                reqs = 0.0
                count = 0
            size = int(values[0])
            gput += float(values[7])
            bput += float(values[8])
            tput += float(values[9])
            droprate += float(values[10])
            lossprob += float(values[11])
            # grtime += float(values[10])
            # brtime += float(values[11])
            rtime += float(values[14])
            rtime_sq += (float(values[14]) ** 2)
            cores += float(values[15])
            reqs += float(values[16])

            count += 1
        rtime_var = (rtime_sq / count) - ( (rtime / count) ** 2)
        results.append([size, gput/count, bput/count, tput/count, droprate/count, lossprob/count, rtime/count, rtime_var, cores/count, reqs/count])
    throughput_plot(results)
    droprate_plot(results)
    lossprob_plot(results)
    resptime_plot(results)
    resptime_CI_plot(results, 0.95)
    resptime_CI_plot(results, 0.9)
    resptime_CI_plot(results, 0.99)
    resptime_CI_plot(results, 0.999)
    cores_plot(results)
    numreqs_plot(results)
    util_law_plot(results)
    little_law_plot(results)
