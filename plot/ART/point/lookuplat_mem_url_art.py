import sys
import os
sys.path.append(os.path.abspath('./plot/'))
from option import *
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import matplotlib.cm as cm
import numpy as np

import csv

NAMES = ["Uncompressed", "Single", "Double", "3-Grams, 65536", "4-Grams, 65536", "ALM-Improved, 8192", "ALM-Improved, 65536"]
LABELS = ["Uncompressed", "Single", "Double", "3-Grams", "4-Grams", "ALM-Improved\n(4K)", "ALM-Improved (64K)"]

COLORS = ['#ffffff', '#fff7ec', '#fee8c8', '#fc8d59', '#d7301f', '#7f0000', '#4c0000']

BACKCOLORS = ['#fff7fb', '#ece7f2', '#d0d1e6', '#a6bddb', '#74a9cf', '#3690c0', '#0570b0', '#045a8d', '#023858']

BORDER_SIZE = 0.5
BORDER_COLOR = 'black'

X_LABEL = "Latency (us)"
Y_LABEL = "Memory(MB)"

LEGEND_FONT_SIZE = 10
LEGEND_POS = 'upper left'

CSV_X_FILE_PATH = "results/ART/point/final_lookuplat_url_art.csv"
CSV_Y_FILE_PATH = "results/ART/point/final_mem_url_art.csv"
GRAPH_OUTPUT_PATH = "figures/ART/point/lat_mem_url_art_point.pdf"

f_in_x = open(CSV_X_FILE_PATH)
reader = csv.reader(f_in_x)
csvrows = list(reader)
data_x = []
for row in csvrows :
    for item in row :
        data_x.append(float(item))

f_in_y = open(CSV_Y_FILE_PATH)
reader = csv.reader(f_in_y)
csvrows = list(reader)
data_y = []
for row in csvrows :
    for item in row :
        data_y.append(float(item))
if BENCH_TYPE == 'big':
    X_LIMIT = 12
    Y_LIMIT = 1600
    x_ticks = [0, 400, 800, 1200, 1600]
    y_ticks = [2, 4, 6, 8, 10, 12]
elif BENCH_TYPE == 'small':
    X_LIMIT = 12
    Y_LIMIT = 10
    x_ticks = [2, 4, 6, 8, 10, 12]
    y_ticks = [2, 4, 6, 8, 10]
#========================================================================================
mpl.rcParams['ps.useafm'] = True
mpl.rcParams['pdf.use14corefonts'] = True
mpl.rcParams['text.usetex'] = False

mpl.rcParams['text.latex.preamble'] = [
       r'\usepackage{siunitx}',   # i need upright \micro symbols, but you need...
       r'\sisetup{detect-all}',   # ...this to force siunitx to actually use your fonts
       r'\usepackage{helvet}',    # set the normal font here
       r'\usepackage{sansmath}',  # load up the sansmath so that math -> helvet
       r'\sansmath'               # <- tricky! -- gotta actually tell tex to use!
]
#========================================================================================

fig = plot.figure(figsize=(GRAPH_WIDTH,GRAPH_HEIGHT))
ax = fig.add_subplot(111)

product_max = X_LIMIT * Y_LIMIT * 0.9
product_min = 3
product_diff = product_max - product_min

x_array = []
x = 0.045
while x < X_LIMIT :
    x_array.append(x)
    x += 0.045
x_list = np.array(x_array)

y_lists = []
num_ranges = 9
for i in range(0, num_ranges) :
    y_list = (product_min + (product_diff + 0.0) / num_ranges * i) / x_list
    y_lists.append(y_list)

ax.fill_between(x_list, 0, y_lists[0], facecolor=BACKCOLORS[0], edgecolor=BACKCOLORS[0])
for i in range(0, len(y_lists)-1) :
    ax.fill_between(x_list, y_lists[i], y_lists[i+1], facecolor=BACKCOLORS[i+1], edgecolor=BACKCOLORS[i+1])

ax.scatter(data_x[0], data_y[0], s=MARKER_SIZE, c=COLORS[0], marker=MARKERS[0], linewidths = BORDER_SIZE, edgecolors = BORDER_COLOR, label=NAMES[0], hatch=2*HATCH)
ax.scatter(data_x[1], data_y[1], s=MARKER_SIZE, c=COLORS[1], marker=MARKERS[1], linewidths = BORDER_SIZE, edgecolors = BORDER_COLOR, label=NAMES[1])
ax.scatter(data_x[2], data_y[2], s=MARKER_SIZE, c=COLORS[2], marker=MARKERS[2], linewidths = BORDER_SIZE, edgecolors = BORDER_COLOR, label=NAMES[2])
ax.scatter(data_x[3], data_y[3], s=MARKER_SIZE, c=COLORS[3], marker=MARKERS[3], linewidths = BORDER_SIZE, edgecolors = BORDER_COLOR, label=NAMES[3])
ax.scatter(data_x[4], data_y[4], s=MARKER_SIZE, c=COLORS[4], marker=MARKERS[4], linewidths = BORDER_SIZE, edgecolors = BORDER_COLOR, label=NAMES[4])
ax.scatter(data_x[5], data_y[5], s=MARKER_SIZE, c=COLORS[5], marker=MARKERS[5], linewidths = BORDER_SIZE, edgecolors = BORDER_COLOR, label=NAMES[5])
if BENCH_TYPE == 'big':
    ax.scatter(data_x[6], data_y[6], s=MARKER_SIZE, c=COLORS[6], marker=MARKERS[6], linewidths = BORDER_SIZE, edgecolors = BORDER_COLOR, label=NAMES[6])

ax.set_xlabel(X_LABEL, fontsize=X_LABEL_FONT_SIZE)
ax.set_xlim(0, X_LIMIT)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)
ax.set_ylim(0, Y_LIMIT)

ax.set_xticks(x_ticks)
ax.tick_params(axis='x', labelsize=X_TICK_FONT_SIZE)

ax.set_yticks(y_ticks)
ax.tick_params(axis='y', labelsize=Y_TICK_FONT_SIZE)

#ax.grid()

ax.annotate(LABELS[0], (data_x[0], data_y[0] * 1.09), ha='center', va='center', size=ANNOTATOR_SIZE)
ax.annotate(LABELS[1], (data_x[1] * 1.35, data_y[1]), ha='center', va='center', size=ANNOTATOR_SIZE)
ax.annotate(LABELS[2], (data_x[2] * 0.56, data_y[2]), ha='center', va='center', size=ANNOTATOR_SIZE)
ax.annotate(LABELS[3], (data_x[3], data_y[3] * 0.9), ha='center', va='center', size=ANNOTATOR_SIZE)
ax.annotate(LABELS[4], (data_x[4] * 1.1, data_y[4] * 0.9), ha='center', va='center', size=ANNOTATOR_SIZE)
ax.annotate(LABELS[5], (data_x[5], data_y[5] * 0.88), ha='center', va='center', size=ANNOTATOR_SIZE)
if BENCH_TYPE == 'big':
    ax.annotate(LABELS[6], (data_x[6], data_y[6] * 1.08), ha='center', va='center', size=ANNOTATOR_SIZE)

plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
