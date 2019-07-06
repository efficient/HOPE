import sys
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
                '%0.2f' % float(height),
#                '%d' % int(height),
                ha='center', va='bottom')

GROUP_NUM = 2
GROUP_NAMES = ["Height", "FPR"]
Y_LABELS = ["Average Trie Height", "False Positive Rate(%)"]

GROUP_SIZE = 7
#CATEGORY_NAMES = ["Uncompressed", "Single", "Double", "3-Grams, 8192", "3-Grams, 65536", "4-Grams, 8192", "4-Grams, 65536"]
CATEGORY_NAMES = ["Uncompressed", "Single", "Double", "3-Grams, 65536", "4-Grams, 65536", "ALM 8192", "ALM 65536"]

CSV_SURF_FPR_FILE_PATH = "results/SuRF/point/final_fpr_url_surf.csv"
CSV_SURF_HEI_FILE_PATH = "results/SuRF/point/final_height_url_surf.csv"
GRAPH_OUTPUT_PATH = "figures/SuRF/point/fpr_url_surf_point.pdf"

COLORS = ['#ffffff', '#fff7ec', '#fee8c8', '#fc8d59', '#d7301f', '#7f0000', '#4c0000']

Y_LABEL = "False Positive Rate (\%)"
Y_LABEL_FONT_SIZE = 20

X_TICK_FONT_SIZE = 20
Y_TICK_FONT_SIZE = 16

LEGEND_FONT_SIZE = 18
LEGEND_POS = 'upper left'

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8.0 #inches

BORDER_SIZE = 0.5
BORDER_COLOR = 'black'

f_in_surf = open(CSV_SURF_FPR_FILE_PATH)
reader = csv.reader(f_in_surf)
csvrows = list(reader)
data_fpr = []
for row in csvrows :
    for item in row :
        data_fpr.append(float(item) * 100)

h_in_surf= open(CSV_SURF_HEI_FILE_PATH)
reader = csv.reader(h_in_surf)
csvrows = list(reader)
data_height = []
for row in csvrows :
    for item in row :
        data_height.append(float(item))

data = [data_height, data_fpr]

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

#fig, ax = plot.subplots(1, 2, figsize=(GRAPH_WIDTH*2, GRAPH_HEIGHT))
#ax1 = plot.axes()  # standard axes
#ax2 = plot.axes([1, 0, 1, 1])

fig = plot.figure(figsize = (GRAPH_WIDTH, GRAPH_HEIGHT))
ax1 = fig.add_axes([0.1, 0.1, 0.4, 0.9])
ax2 = fig.add_axes([0.5, 0.1, 0.4, 0.9])
ax1.set_xlim([0,1])
ax2.set_xlim([0,1])

y1_ticks = [0, 20, 40, 60, 80, 100]
ax1.set_yticks(y1_ticks)
ax1.set_ylim(0, 100)
ax1.set_ylabel(Y_LABELS[0], fontsize=Y_LABEL_FONT_SIZE)

y2_ticks = [0, 12, 24, 36, 48, 60]
ax2.set_yticks(y2_ticks)
ax2.set_ylim(0, 60)
ax2.set_ylabel(Y_LABELS[1], fontsize=Y_LABEL_FONT_SIZE)

ax=[ax1,ax2]
for j in range(0, GROUP_NUM) :
    rect = []
    for i in range(0, GROUP_SIZE) :
        if i == 0: # baseline
            hatch="/"
        else:
            hatch = ""
        pos = width * 1.5 + width * i
        rect.append(ax[j].bar(pos, data[j][i], width, linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, color=COLORS[i], hatch=hatch))
        autolabel(rect[i], ax[j])

ax[0].yaxis.tick_left()
ax[0].yaxis.set_label_position('left')

ax[1].yaxis.tick_right()
ax[1].yaxis.set_label_position('right')
for j in range(0, GROUP_NUM) :
    xtick_pos = [(width * 1.5 + width * GROUP_SIZE) / 2.0]
    ax[j].set_xticks(xtick_pos)
    ax[j].set_xticklabels([GROUP_NAMES[j]])

    for label in ax[j].get_xticklabels():
        label.set_fontsize(X_TICK_FONT_SIZE)

#    y_ticks = [0, 10, 20, 30, 40]
#    ax[j].set_yticks(y_ticks)
#    ax[j].set_ylim(0, 40)
#
    for label in ax[j].get_yticklabels():
        label.set_fontsize(Y_TICK_FONT_SIZE)
#
#    ax1.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)



#ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE})

outFile = GRAPH_OUTPUT_PATH
plot.savefig(outFile, bbox_inches='tight')
