import sys
import os
import matplotlib as mpl
sys.path.append(os.path.abspath('./plot/'))
from option import *
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
                ha='center', va='bottom')

GROUP_NUM = 6
GROUP_NAMES = ["Single", "Double", "3-Grams", "4-Grams", "ALM", "ALM-Improved"]

GROUP_SIZE = 6
CATEGORY_NAMES = ["0.001%", "0.01%", "0.1%", "1%", "10%", "100%"]

REPEAT_TIME = 3
ROUND_SIZE = ((GROUP_NUM - 2) * GROUP_SIZE + 2 * (GROUP_SIZE - 1))

CSV_FILE_PATH = "results/microbench/sample_size_sweep/cpr_email_sample_size_sweep.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/sample_size_sweep/cpr_email_sample_size_sweep.pdf"

COLORS = ['#ffffff', '#fee8c8', '#fc8d59', '#d7301f', '#7f0000', '#4c0000']

Y_LABEL = "Compression Rate"

X_TICK_FONT_SIZE = 16
Y_TICK_FONT_SIZE = 16

Y_MODIFIER = 1.0

LEGEND_FONT_SIZE = 18
LEGEND_POS = 'upper left'

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8.0 #inches

f_in = open(CSV_FILE_PATH, 'r')
reader = csv.reader(f_in)
csvrows = list(reader)
csvdata = []

for row in csvrows :
    for item in row :
        csvdata.append((float(item) / Y_MODIFIER))

data = []
for i in range(0, GROUP_SIZE - 1) :
    data.append([0] * GROUP_NUM)

data.append([0] * (GROUP_NUM - 2))

#[[0.01 s, 0.01 d,],[]]
#[0.01 s, 0.01 d, 0.01 3, 0.1 s]
for count, item in enumerate(csvdata):
    idx = count % ROUND_SIZE
    if (idx < (GROUP_SIZE - 1) * GROUP_NUM):
        data[idx / GROUP_NUM][idx % GROUP_NUM] += item
    else:
        group_id = 5
        offset = (idx - (GROUP_SIZE - 1) * GROUP_NUM)
        data[group_id][offset] += item

for i in range(GROUP_SIZE) :
    if i < GROUP_SIZE - 1:
        for j in range(GROUP_NUM):
            data[i][j] = data[i][j] * 1.0 / REPEAT_TIME
    else:
        for j in range(GROUP_NUM - 2):
            data[i][j] = data[i][j] * 1.0 / REPEAT_TIME


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

width = GRAPH_WIDTH  / (ROUND_SIZE + GROUP_NUM + 1)

fig = plot.figure(figsize={GRAPH_HEIGHT, GRAPH_WIDTH})
ax = fig.add_subplot(111)

rect = []

for i in range(0, GROUP_SIZE - 1) :
    pos = []
    for j in range(0, GROUP_NUM - 1) :
        pos.append(width + width * i + width * j * (GROUP_SIZE + 1))
    pos.append(width + width * i + width * (GROUP_NUM - 1) * (GROUP_SIZE) + 4 * width)
    rect.append(ax.bar(pos, data[i], width, color=COLORS[i], label=CATEGORY_NAMES[i]))
#    autolabel(rect[i])

for i in range(GROUP_SIZE-1, GROUP_SIZE):
    pos = []
    for j in range(GROUP_NUM - 2):
        pos.append(width * (GROUP_SIZE - 1) + j * width * (GROUP_SIZE +1) + (i - GROUP_SIZE+2) * width)
    rect.append(ax.bar(pos, data[i], width, color=COLORS[i], label=CATEGORY_NAMES[i]))
#    autolabel(rect[i])

xtick_pos = []
for j in range(0, GROUP_NUM - 1) :
    xtick_pos.append(width + width * (GROUP_SIZE / 2.0) + width * j * (GROUP_SIZE + 1))

xtick_pos.append(width * (GROUP_SIZE / 2.0) + width * (GROUP_NUM - 1) * (GROUP_SIZE + 1))

ax.set_xticks(xtick_pos)
ax.set_xticklabels(GROUP_NAMES, rotation=20)

for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

y_ticks = [0, 1.0, 2.0, 3.0, 4.0]
ax.set_yticks(y_ticks)
ax.set_ylim(0, 4.0)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)

ax.legend(loc=LEGEND_POS, ncol=2, prop={'size':LEGEND_FONT_SIZE})

outFile = GRAPH_OUTPUT_PATH
plot.savefig(outFile, bbox_inches='tight')
