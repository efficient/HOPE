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
                ha='center', va='bottom')

GROUP_SIZE = 9
CATEGORY_NAMES = ["Uncompressed", "Single", "Double", "3-Grams, 8192", "3-Grams, 65536", 
                  "4-Grams, 8192", "4-Grams, 65536", "ALM, 10000", "ALM, 65536"]

CSV_FILE_PATH = "results/SuRF/point/height_email_surf.csv"
GRAPH_OUTPUT_PATH = "figures/SuRF/point/height_email_surf.pdf"

COLORS = ['#fef0d9', '#fdd49e', '#fdbb84', '#fc8d59', '#ef6548', '#d7301f', '#990000', '#5b0006', '#350004']

Y_LABEL = "Average Trie Height"
Y_LABEL_FONT_SIZE = 20

X_TICK_FONT_SIZE = 20
Y_TICK_FONT_SIZE = 16

LEGEND_FONT_SIZE = 18
LEGEND_POS = 'upper left'

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8.0 #inches

f_in = open(CSV_FILE_PATH)
reader = csv.reader(f_in)
csvrows = list(reader)
data = []
for row in csvrows :
    for item in row :
        data.append(float(item))

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

width = GRAPH_WIDTH  / ((GROUP_SIZE + 1) + 1.0)

fig = plot.figure(figsize={GRAPH_HEIGHT, GRAPH_WIDTH})
ax = fig.add_subplot(111)

rect = []

for i in range(0, GROUP_SIZE) :
    pos = []
    pos.append(width + width * i)
    rect.append(ax.bar(pos, [data[i]], width, color=COLORS[i], label=CATEGORY_NAMES[i]))
    autolabel(rect[i])

ax.get_xaxis().set_visible(False)

y_ticks = [0, 5, 10, 15, 20, 25]
ax.set_yticks(y_ticks)
ax.set_ylim(0, 25)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)

#ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE})

outFile = GRAPH_OUTPUT_PATH
plot.savefig(outFile, bbox_inches='tight')
