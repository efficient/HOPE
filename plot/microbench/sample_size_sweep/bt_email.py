import sys
import matplotlib as mpl
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np
import csv

def autolabel(rects):
    count = 0
    for rect in rects:
        height = rect.get_height()
        if count == 0 :
            ax.text(rect.get_x() + rect.get_width()/2., height + 0.01,
                    '%.2f' % float(height),
                    #'%d' % int(height),
                    ha='center', va='bottom')
        else :
            ax.text(rect.get_x() + rect.get_width()/2., height + 0.01,
                    '%0.1f' % float(height),
                    #'%d' % int(height),
                    ha='center', va='bottom')
        count += 1

GROUP_NUM = 4
GROUP_NAMES = ["Single", "Double", "3-Grams", "4-Grams"]

GROUP_SIZE = 3
CATEGORY_NAMES = ["100\%", "10\%", "1\%"]

CSV_FILE_PATH = "results/microbench/sample_size_sweep/bt_email_sample_size_sweep.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/sample_size_sweep/bt_email_sample_size_sweep.pdf"

COLORS = ['#fee8c8', '#fdbb84', '#e34a33'] #for GROUP_SIZE = 3
#COLORS = ['#fdcc8a', '#fc8d59', '#d7301f'] #for GROUP_SIZE = 3(2)

Y_LABEL = "Built Time (s)"
Y_LABEL_FONT_SIZE = 20

X_TICK_FONT_SIZE = 20
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
for i in range(0, GROUP_SIZE) :
    data.append([])

count = 0
for item in csvdata :
    data[count / GROUP_NUM].append(item)
    count += 1

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

y_ticks = [0, 50, 100, 150]
ax.set_yticks(y_ticks)
ax.set_ylim(0, 150)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE, weight='bold')

ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE})

outFile = GRAPH_OUTPUT_PATH
plot.savefig(outFile, bbox_inches='tight')
