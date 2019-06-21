import sys
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np
import csv

NUM_LINES = 4
LINE_NAMES = ["Single", "Double", "3-Grams", "4-Grams", "ALM"]
NUM_3_POINTS = 7
NUM_4_EXTRA_POINTS = 2

COLORS = ['#fef0d9', '#fdcc8a', '#fc8d59', '#d7301f', '#3366CC']

Y_LABEL = "Memory (KB)"
Y_LABEL_FONT_SIZE = 20

X_LABEL = "Number of Dictionary Entries"
X_LABEL_FONT_SIZE = 20

X_TICK_FONT_SIZE = 16
Y_TICK_FONT_SIZE = 16

X_START = 2**7
X_LIMIT = 2**19

Y_MODIFIER = 1000
Y_START = 2
Y_LIMIT = 2**18

LEGEND_FONT_SIZE = 14
LEGEND_POS = 'upper left'

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8 #inches

CSV_X_FILE_PATH = "results/microbench/cpr_latency/x_wiki_dict_size.csv"
CSV_Y_FILE_PATH = "results/microbench/cpr_latency/mem_wiki_dict_size.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/cpr_latency/mem_wiki_dict_size.pdf"

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
        data_y.append(float(item) / Y_MODIFIER)

single_x = [data_x[0]]
single_y = [data_y[0]]
double_x = [data_x[1]]
double_y = [data_y[1]]
three_grams_x = []
three_grams_y = []
four_grams_x = []
four_grams_y = []
alm_x = []
alm_y = []

for i in range(2, 2 + NUM_3_POINTS * 3, 3) :
    three_grams_x.append(data_x[i])
    three_grams_y.append(data_y[i])
    four_grams_x.append(data_x[i+1])
    four_grams_y.append(data_y[i+1])
    alm_x.append(data_x[i+2])
    alm_y.append(data_y[i+2])

for i in range(2 + NUM_3_POINTS * 3, 2 + NUM_3_POINTS * 3 + 2 * NUM_4_EXTRA_POINTS, 2) :
    four_grams_x.append(data_x[i])
    four_grams_y.append(data_y[i])
    alm_x.append(data_x[i+1])
    alm_y.append(data_y[i+1])

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

ax.plot(single_x, single_y, 'd-', ms=10, lw = 3, color=COLORS[0], label=LINE_NAMES[0])
ax.plot(double_x, double_y, 'o-', ms=10, lw = 3, color=COLORS[1], label=LINE_NAMES[1])
ax.plot(three_grams_x, three_grams_y, 's-', ms=10, lw = 3, color=COLORS[2], label=LINE_NAMES[2])
ax.plot(four_grams_x, four_grams_y, '^-', ms=10, lw = 3, color=COLORS[3], label=LINE_NAMES[3])
ax.plot(alm_x, alm_y, 'v-', ms=10, lw = 3, color=COLORS[4], label=LINE_NAMES[4])

ax.set_xscale('log', basex=2)
ax.set_yscale('log', basey=2)

for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_xlabel(X_LABEL, fontsize=X_LABEL_FONT_SIZE)

X = [2**8, 2**10, 2**12, 2**14, 2**16, 2**18]
ax.set_xticks(X)
ax.set_xlim(X_START, X_LIMIT)

Y = [2**3, 2**6, 2**9, 2**12, 2**15, 2**18]
ax.set_yticks(Y)
ax.set_ylim(Y_START, Y_LIMIT)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)

ax.grid()
ax.set_axisbelow(True)

ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE}, ncol=2)

plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')

