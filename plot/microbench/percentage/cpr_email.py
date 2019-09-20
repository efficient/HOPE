import sys
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np
import csv

NUM_LINES = 4
LINE_NAMES = ["Single-Char", "Double-Char", "ALM", "3-Grams", "4-Grams", "ALM-Improved"]
NUM_3_POINTS = 7
NUM_4_EXTRA_POINTS = 2

#COLORS = ['#fef0d9', '#fdcc8a', '#fc8d59', '#d7301f', '#33b3cc', '#3366CC']
COLORS = ['#fff7ec', '#fee8c8', '#fdd49e', '#fc8d59', '#d7301f', '#7f0000']

SHAPE_BORDER = 0.5
EDGE_COLOR = 'black'

Y_LABEL = "Compression Rate"
Y_LABEL_FONT_SIZE = 20

X_LABEL = "Data Percentage"
X_LABEL_FONT_SIZE = 20

X_TICK_FONT_SIZE = 16
Y_TICK_FONT_SIZE = 16

X_START = 10
X_LIMIT = 100

LEGEND_FONT_SIZE = 14
LEGEND_POS = 'upper left'

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8 #inches

CSV_FILE_PATH = "results/microbench/percentage/per_cpr_lat.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/percentage/cpr_email_dict_size.pdf"

data_x = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]

f_in_y = open(CSV_FILE_PATH)
reader = csv.reader(f_in_y)
csvrows = list(reader)
data_y = []
for row in csvrows :
    for item in row :
        data_y.append(float(item))

single_x = []
double_x = []
three_grams_x = []
three_grams_y = []
four_grams_x = []
four_grams_y = []
alm_x = []
alm_y = []
alm_improved_x = []
alm_improved_y = []


def get_add(start_idx, end_idx, step, data):
    part_data = []
    for i in range(start_idx, end_idx, step):
        part_data.append(data[i])
    return part_data

single_y = get_data(0, 20, 2, data)
double_y = get_data(20, 40, 2, data)
three_grams_y = get_data(40, 60, 2, data)
four_grams_y = get_data(60, 80, 2, data)
alm_y = get_data(80, 100, 2, data)
alm_improved_y = get_data(100, 120, 2, data)

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

fig = plot.figure(figsize={GRAPH_HEIGHT, GRAPH_WIDTH})
ax = fig.add_subplot(111)

ax.plot(data_x, single_y, 's-', ms=10, mew = SHAPE_BORDER, mec = EDGE_COLOR, lw = 3, color=COLORS[0], label=LINE_NAMES[0])
ax.plot(double_x, double_y, 'o-', ms=10, mew = SHAPE_BORDER, mec = EDGE_COLOR,lw = 3, color=COLORS[1], label=LINE_NAMES[1])
ax.plot(data_x, alm_y, 'd-', ms=10, lw = 3, mew = SHAPE_BORDER, mec = EDGE_COLOR, color=COLORS[2], label=LINE_NAMES[2])
ax.plot(data_x, three_grams_y, 'p-', mew = SHAPE_BORDER, mec = EDGE_COLOR, ms=10, lw = 3, color=COLORS[3], label=LINE_NAMES[3])
ax.plot(data_x, four_grams_y, '^-', mew = SHAPE_BORDER, mec = EDGE_COLOR, ms=10, lw = 3, color=COLORS[4], label=LINE_NAMES[4])
ax.plot(data_x, alm_improved_y, 'v-', mew = SHAPE_BORDER,mec = EDGE_COLOR, ms=10, lw = 3, color=COLORS[5], label=LINE_NAMES[5])


for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_xlabel(X_LABEL, fontsize=X_LABEL_FONT_SIZE)

ax.set_xticks(data_x)
ax.set_xlim(X_START, X_LIMIT)

y_ticks = [0, 1.0, 2.0, 3.0, 4.0]
ax.set_yticks(y_ticks)
ax.set_ylim(0, 4.0)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)

ax.grid()
ax.set_axisbelow(True)

ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE}, ncol=2)

plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
