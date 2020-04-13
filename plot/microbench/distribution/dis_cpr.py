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
                ha='center', va='bottom')

NAMES = ["Single-Char", "Double-Char", "3-Grams", "4-Grams", "ALM", "ALM-Improved"]
TYPES = ["Dict-A, Email-A", "Dict-B, Email-B", "Dict-A, Email-B", "Dict-B, Email-A"]
REPEAT_TIMES = 2
GROUP_SIZE = 6

COLORS = ['#fee8c8', '#fc8d59', '#d7301f', '#7f0000']

Y_LABEL = "Compression Rate"

X_TICK_FONT_SIZE = 14
Y_TICK_FONT_SIZE = 16

LEGEND_FONT_SIZE = 14
LEGEND_POS = 'upper left'

CSV_X_FILE_PATH = "results/microbench/distribution/dis_cpr.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/distribution/dis_cpr.pdf"

f_in_x = open(CSV_X_FILE_PATH)
reader = csv.reader(f_in_x)
csvrows = list(reader)
data = []
for row in csvrows :
    for item in row :
        data.append(float(item))

single = data[0:4]
double = data[4:8]
three_gram = data[8:12]
four_gram = data[12:16]
alm = data[16:20]
alm_improve = data[20:24]

all_data = [single, double, three_gram, four_gram, alm, alm_improve]
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

width = 1.0 / (GROUP_SIZE * 4 + GROUP_SIZE + 2)
recs = []
pos = []
for i in range(GROUP_SIZE):
    recs.append(ax.bar(width * 1.5 + i * (4 + 1) * width, all_data[i][0], width, color=COLORS[0], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR))
    recs.append(ax.bar(width * 1.5 + i * (4 + 1) * width + width, all_data[i][1], width, color=COLORS[1], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR))
    pos.append(width * 1.5 + i * (4 + 1) * width + width * 2)
    recs.append(ax.bar(width * 1.5 + i * (4 + 1) * width + width * 2, all_data[i][2], width, color=COLORS[2], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, ))
    recs.append(ax.bar(width * 1.5 + i * (4 + 1) * width + width * 3, all_data[i][3], width, color=COLORS[3], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, ))

for rec in recs:
    autolabel(rec)

y_ticks = [0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5]
ax.set_yticks(y_ticks)
ax.set_ylim(0, 3.5)
ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE  - 4)
ax.legend(recs[0:3], ncol=len(TYPES)/2, loc='upper left', labels=TYPES, fontsize='medium')

ax.set_xticks(pos)
ax.set_xticklabels(NAMES, rotation=20)

for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
