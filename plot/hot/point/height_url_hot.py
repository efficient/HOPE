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
                '%0.2f' % float(height),
#                '%d' % int(height),
                ha='center', va='bottom')

GROUP_SIZE = 7
CATEGORY_NAMES = ["Uncompressed", "Single", "Double", "3-Grams, 65536", "4-Grams, 65536", "ALM, 8192", "ALM, 65536"]

CSV_FILE_PATH = "results/hot/point/final_height_url_hot.csv"
GRAPH_OUTPUT_PATH = "figures/hot/point/height_url_hot.pdf"

COLORS = ['#ffffff', '#fff7ec', '#fee8c8', '#fc8d59', '#d7301f', '#7f0000', '#4c0000']

Y_LABEL = "Average Trie Height"

X_TICK_FONT_SIZE = 20
Y_TICK_FONT_SIZE = 16

LEGEND_FONT_SIZE = 18
LEGEND_POS = 'upper left'

f_in = open(CSV_FILE_PATH)
reader = csv.reader(f_in)
csvrows = list(reader)
data = []
for row in csvrows :
    sum_h = 0
    num_nodes = 0
    for i,item in enumerate(row):
        if item == '':
            continue
        num_nodes += float(item)
        sum_h  += (i+1) * float(item)
    data.append(sum_h/num_nodes)

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

width = 1  / (GROUP_SIZE + 1.0)

fig = plot.figure(figsize=(GRAPH_WIDTH, GRAPH_HEIGHT))
ax = fig.add_subplot(111)

rect = []

for i in range(0, GROUP_SIZE) :
    if i == 0: # baseline
        hatch="/"
    else:
        hatch = ""
    pos = []
    pos.append(width + width * i)
    rect.append(ax.bar(pos, [data[i]], width, color=COLORS[i], label=CATEGORY_NAMES[i], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, hatch=hatch))
    autolabel(rect[i])

ax.get_xaxis().set_visible(False)

y_limit = getLimit(data)
y_ticks = getTicks(y_limit)
ax.set_yticks(y_ticks)
ax.set_ylim(0, y_limit)
ax.set_xlim([0,1])

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)
#ax.set_xlabel('Test',  fontsize=Y_LABEL_FONT_SIZE)
#ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE})

outFile = GRAPH_OUTPUT_PATH
plot.savefig(outFile, bbox_inches='tight')
