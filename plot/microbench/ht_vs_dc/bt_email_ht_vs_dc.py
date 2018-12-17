import sys
import matplotlib as mpl
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np
import csv

NUM_LINES = 2
LINE_NAMES = ["Fixed-Len Dict Codes", "Hu-Tucker Codes"]
NUM_POINTS = 9

COLORS = ['#fdcc8a', '#d7301f']

Y_LABEL = "Build Time (s)"
Y_LABEL_FONT_SIZE = 20

X_LABEL = "Number of Dictionary Entries"
X_LABEL_FONT_SIZE = 20

X_TICK_FONT_SIZE = 16
Y_TICK_FONT_SIZE = 16

X_START = 2**9
X_LIMIT = 2**18

LEGEND_FONT_SIZE = 16
LEGEND_POS = 'upper left'

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8 #inches

CSV_X_FILE_PATH = "results/microbench/ht_vs_dc/x_email_ht.csv"
CSV_Y_DC_FILE_PATH = "results/microbench/ht_vs_dc/bt_email_dc.csv"
CSV_Y_HT_FILE_PATH = "results/microbench/ht_vs_dc/bt_email_ht.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/ht_vs_dc/bt_email_ht_vs_dc.pdf"

f_in_x = open(CSV_X_FILE_PATH)
reader = csv.reader(f_in_x)
csvrows = list(reader)
data_x = []
for row in csvrows :
    for item in row :
        data_x.append(float(item))

f_in_y_dc = open(CSV_Y_DC_FILE_PATH)
reader = csv.reader(f_in_y_dc)
csvrows = list(reader)
data_y_dc = []
for row in csvrows :
    for item in row :
        data_y_dc.append(float(item))

f_in_y_ht = open(CSV_Y_HT_FILE_PATH)
reader = csv.reader(f_in_y_ht)
csvrows = list(reader)
data_y_ht = []
for row in csvrows :
    for item in row :
        data_y_ht.append(float(item))

#========================================================================================
mpl.rcParams['ps.useafm'] = True
mpl.rcParams['pdf.use14corefonts'] = True
mpl.rcParams['text.usetex'] = True

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

ax.plot(data_x, data_y_dc, 'o-', ms=10, lw = 3, color=COLORS[0], label=LINE_NAMES[0])
ax.plot(data_x, data_y_ht, 's-', ms=10, lw = 3, color=COLORS[1], label=LINE_NAMES[1])

ax.set_xscale('log', basex=2)

for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_xlabel(X_LABEL, fontsize=X_LABEL_FONT_SIZE, weight='bold')

X = [2**10, 2**12, 2**14, 2**16, 2**18]
ax.set_xticks(X)
ax.set_xlim(X_START, X_LIMIT)

y_ticks = [0, 50, 100, 150, 200]
ax.set_yticks(y_ticks)
ax.set_ylim(0, 200)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE, weight='bold')

ax.grid()
ax.set_axisbelow(True)

ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE})

plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
