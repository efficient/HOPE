import sys
import matplotlib as mpl
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np
import csv

NUM_LINES = 4
LINE_NAMES = ["3-Grams, Array Dict", "3-Grams, Trie Dict", "4-Grams, Array Dict", "4-Grams, Trie Dict"]
NUM_3_POINTS = 7
NUM_4_POINTS = 9

COLORS = ['#fef0d9', '#fdcc8a', '#fc8d59', '#d7301f']

Y_LABEL = "Latency (ns per char)"
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

CSV_X_FILE_PATH = "results/microbench/array_trie/x_email_array_trie.csv"
CSV_Y_ARRAY_FILE_PATH = "results/microbench/array_trie/lat_email_array.csv"
CSV_Y_TRIE_FILE_PATH = "results/microbench/array_trie/lat_email_trie.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/array_trie/lat_email_array_trie.pdf"

f_in_x = open(CSV_X_FILE_PATH)
reader = csv.reader(f_in_x)
csvrows = list(reader)
data_x = []
for row in csvrows :
    for item in row :
        data_x.append(float(item))

f_in_y_array = open(CSV_Y_ARRAY_FILE_PATH)
reader = csv.reader(f_in_y_array)
csvrows = list(reader)
data_y_array = []
for row in csvrows :
    for item in row :
        data_y_array.append(float(item))

f_in_y_trie = open(CSV_Y_TRIE_FILE_PATH)
reader = csv.reader(f_in_y_trie)
csvrows = list(reader)
data_y_trie = []
for row in csvrows :
    for item in row :
        data_y_trie.append(float(item))

three_x = data_x[0:7]
four_x = data_x[7:16]
three_y_array = data_y_array[0:7]
four_y_array = data_y_array[7:16]
three_y_trie = data_y_trie[0:7]
four_y_trie = data_y_trie[7:16]

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

ax.plot(three_x, three_y_array, 'd-', ms=10, lw = 3, color=COLORS[0], label=LINE_NAMES[0])
ax.plot(three_x, three_y_trie, 'o-', ms=10, lw = 3, color=COLORS[1], label=LINE_NAMES[1])
ax.plot(four_x, four_y_array, 's-', ms=10, lw = 3, color=COLORS[2], label=LINE_NAMES[2])
ax.plot(four_x, four_y_trie, '^-', ms=10, lw = 3, color=COLORS[3], label=LINE_NAMES[3])

ax.set_xscale('log', basex=2)

for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_xlabel(X_LABEL, fontsize=X_LABEL_FONT_SIZE, weight='bold')

X = [2**10, 2**12, 2**14, 2**16, 2**18]
ax.set_xticks(X)
ax.set_xlim(X_START, X_LIMIT)

y_ticks = [0, 20, 40, 60, 80]
ax.set_yticks(y_ticks)
ax.set_ylim(0, 80)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE, weight='bold')

ax.grid()
ax.set_axisbelow(True)

ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE}, ncol=2)

plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
