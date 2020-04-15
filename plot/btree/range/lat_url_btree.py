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

def autolabel(rects, ax):
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., height + 0.01,
                '%0.1f' % float(height),
#                '%d' % int(height),
                fontsize=14,
                ha='center', va='bottom')

GROUP_NUM = 2
GROUP_NAMES = ["Range", "Insert"]
Y_LABEL = "Latency(us)"

GROUP_SIZE = 7
CATEGORY_NAMES = ["Uncompressed", "Single", "Double", "3-Grams, 65536", "4-Grams, 65536", "ALM 8192", "ALM 65536"]

CSV_ART_LOOKUP_FILE_PATH = "results/btree/range/final_lookuplat_url_btree_range.csv"
CSV_ART_INS_FILE_PATH = "results/btree/range/final_insertlat_url_btree_range.csv"
GRAPH_OUTPUT_PATH = "figures/btree/range/lat_url_btree.pdf"

COLORS = ['#ffffff', '#fff7ec', '#fee8c8', '#fc8d59', '#d7301f', '#7f0000', '#4c0000']

LEGEND_FONT_SIZE = 18
LEGEND_POS = 'upper left'

lookup_in_art = open(CSV_ART_LOOKUP_FILE_PATH)
reader = csv.reader(lookup_in_art)
csvrows = list(reader)
data_lookup = []
for row in csvrows :
    for item in row :
        data_lookup.append(float(item))

insert_in_art = open(CSV_ART_INS_FILE_PATH)
reader = csv.reader(insert_in_art)
csvrows = list(reader)
data_insert = []
for row in csvrows :
    for item in row :
        data_insert.append(float(item))

data = [data_lookup, data_insert]

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

width = 1.0 / ((GROUP_SIZE + 2))

fig = plot.figure(figsize = (GRAPH_WIDTH, GRAPH_RANGE_HEIGHT))
ax1 = fig.add_axes([0.1, 0.1, 0.4, 0.9])
ax2 = fig.add_axes([0.5, 0.1, 0.4, 0.9])
ax1.set_xlim([0,1])
ax2.set_xlim([0,1])

ax1.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)
y1_limit = getLimit(data_lookup)
y1_ticks = getTicks(y1_limit)
ax1.set_yticks(y1_ticks)
ax1.set_ylim(0, y1_limit)
ax1.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)

y2_limit = getLimit(data_insert)
y2_ticks = getTicks(y2_limit)
ax2.set_ylim(0, y2_limit)
ax2.yaxis.set_ticks(y2_ticks)

ax1.yaxis.tick_left()
ax2.yaxis.tick_right()

ax=[ax1,ax2]
for j in range(0, GROUP_NUM) :
    rect = []
    for i in range(0, GROUP_SIZE) :
        if i == 0: # baseline
            hatch = HATCH
        else:
            hatch = ""
        pos = width * 1 + width * i
        rect.append(ax[j].bar(pos, data[j][i], width, linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, color=COLORS[i], hatch=hatch))
        autolabel(rect[i], ax[j])

for j in range(0, GROUP_NUM) :
    xtick_pos = [width + width * GROUP_SIZE / 2.0]
    ax[j].set_xticks(xtick_pos)
    ax[j].set_xticklabels([GROUP_NAMES[j]])

    for label in ax[j].get_xticklabels():
        label.set_fontsize(X_LABEL_FONT_SIZE)

    for label in ax[j].get_yticklabels():
        label.set_fontsize(Y_TICK_FONT_SIZE)

#ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE})

outFile = GRAPH_OUTPUT_PATH
plot.savefig(outFile, bbox_inches='tight')
