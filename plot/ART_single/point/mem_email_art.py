import sys
import matplotlib as mpl
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
                ha='center', va='bottom', fontsize=6)

GROUP_NUM = 9
GROUP_NAMES = ["Uncompressed", "Single", "Double", "3-Grams, 8192", "3-Grams, 65536", "4-Grams, 8192", "4-Grams, 65536", "ALM, 8129", "ALM, 65536" ]

GROUP_SIZE = 4
CATEGORY_NAMES = ["N4", "N16", "N48", "N256"] 
CSV_SURF_FILE_PATH = "results/ART/point/stats_email_art.csv"
GRAPH_OUTPUT_PATH = "figures/ART/point/stats_email_art.pdf"

COLORS = ['#fef0d9', '#fdd49e', '#fdbb84', '#fc8d59']

Y_LABEL = "Memory Use"
Y_LABEL_FONT_SIZE = 20

X_TICK_FONT_SIZE = 10
Y_TICK_FONT_SIZE = 16

LEGEND_FONT_SIZE = 18
LEGEND_POS = 'upper left'

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8.0 #inches

f_in_mem = open(CSV_SURF_FILE_PATH)
reader = csv.reader(f_in_mem)
csvrows = list(reader)
N4 = []
N16 = []
N48 = []
N256 = []
for row in csvrows :
    record = [int(x) for x in row]
    N4.append(record[0])
    N16.append(record[1])
    N48.append(record[2])
    N256.append(record[3])

data = [N4, N16, N48, N256]
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

width = GRAPH_WIDTH  / (((GROUP_SIZE + 1) * GROUP_NUM) + 1.0)

fig = plot.figure(figsize={GRAPH_HEIGHT, GRAPH_WIDTH})
ax = fig.add_subplot(111)

rect = []
for i in range(0, GROUP_SIZE) :
    pos = []
    for j in range(0, GROUP_NUM) :
        pos.append(width + width * i + width * j * (GROUP_SIZE + 1))
    rect.append(ax.bar(pos, data[i], width, color=COLORS[i], label=CATEGORY_NAMES[i]))
    autolabel(rect[i])

xtick_pos = []
for j in range(0, GROUP_NUM) :
    xtick_pos.append(width + width * (GROUP_SIZE / 2.0) + width * j * (GROUP_SIZE + 1))

ax.set_xticks(xtick_pos)
ax.set_xticklabels(GROUP_NAMES)

for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)
plot.setp(ax.get_xticklabels(), rotation=30, horizontalalignment='center')

y_ticks = [0, 2000000, 4000000, 6000000, 8000000]
ax.set_yticks(y_ticks)
ax.set_ylim(0, 8000000)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE, weight='bold')

#ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE})

outFile = GRAPH_OUTPUT_PATH
plot.savefig(outFile, bbox_inches='tight')
