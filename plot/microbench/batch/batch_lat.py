import sys
import os
sys.path.append(os.path.abspath('./plot/'))
from option import *
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np
import csv

def autolabel(rects):
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., height + 0.01,
                '%0.1f' % float(height),
#                '%d' % int(height),
                fontsize= 14,
                ha='center', va='bottom')

NAMES = ["Single-Char", "Double-Char", "3-Grams", "4-Grams"]
TYPES = ["Batch Size = 1", "Batch Size = 2", "Batch Size = 32"]
REPEAT_TIMES = 2
GROUP_SIZE = 4
BATCH_GROUP_SIZE  = 7
BATCH_SIZEE = [1, 2, 4, 8, 16, 32, 64]

COLORS = ['#fee8c8', '#fc8d59', '#d7301f']

Y_LABEL = "Encode Latency (ns per char)"

X_TICK_FONT_SIZE = 18
Y_TICK_FONT_SIZE = 16

LEGEND_FONT_SIZE = 16
LEGEND_POS = 'upper left'

CSV_X_FILE_PATH = "results/microbench/batch/batch_lat.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/batch/batch_lat.pdf"

f_in_x = open(CSV_X_FILE_PATH)
reader = csv.reader(f_in_x)
csvrows = list(reader)
data = []
for row in csvrows :
    for item in row :
        data.append(float(item))

single = []
double = []
three_gram = []
four_gram = []
for i in range(0, BATCH_GROUP_SIZE * GROUP_SIZE, GROUP_SIZE):
    single.append(data[i])
    double.append(data[i+1])
    three_gram.append(data[i+2])
    four_gram.append(data[i+3])

# remove 1, 2 from batch
#single[0:2] = data[0:2]
#double[0:2] = data[2:4]
#three_gram[0:2] = data[4:6]
#four_gram[0:2] = data[6:8]


all_data = [single, double, three_gram, four_gram]
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

fig = plot.figure(figsize=(8.5, 4))
ax = fig.add_subplot(111)
ax.set_xlim([0,1])

width = 1.0 / (GROUP_SIZE * 3 + GROUP_SIZE + 1)
recs = []
pos = []
for i in range(GROUP_SIZE):
    recs.append(ax.bar(width + i * (3 + 1) * width, all_data[i][0], width, color=COLORS[0], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR))
    recs.append(ax.bar(width + i * (3 + 1) * width + width, all_data[i][1], width, color=COLORS[1], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR))
    pos.append(1.5 * width + i * (3 + 1) * width + width)
    recs.append(ax.bar(width + i * (3 + 1) * width + width * 2, all_data[i][5], width, color=COLORS[2], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, ))

for rec in recs:
    autolabel(rec)

y_ticks = [0, 5, 10, 15, 20]
ax.set_yticks(y_ticks)
ax.set_ylim(0, 20)
ax.set_xticks(pos)
ax.set_xticklabels(NAMES)
ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE  - 8)
ax.legend(recs[0:3], labels=TYPES, loc=LEGEND_POS, fontsize='x-large')

for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
