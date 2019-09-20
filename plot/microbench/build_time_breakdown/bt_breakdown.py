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
        height = rect.get_height() + rect.get_y()
        ax.text(rect.get_x() + rect.get_width()/2., height + 0.01,
                '%0.2f' % float(height),
#                '%d' % int(height),
                ha='center', va='bottom')

GROUP_SIZE = 6
GROUP_NUM = 2
LABELS = ["Single", "Double", "3-Gram", "4-Gram", "ALM", "ALM-Improved"]
TYPES = ["Symbol Select","Code Assign", "Build Dictionary"]
XLABEL = ["Fixed Size", "Dictionary Size = $2^{12}$", "Dictionary Size = $2^{16}$"]
YLABEL = "Build Time(s)"
X_TICK_FONT_SIZE = 8
Y_TICK_FONT_SIZE = 10

TITLE = "Build Time Breakdown (dictionary size = 65536)"

COLORS = ['#fef0d9', '#fc8d59', '#b30000']

LEGEND_FONT_SIZE = 10
LEGEND_POS = 'upper left'

CSV_FILE_PATH = "results/microbench/build_time_breakdown/bt_breakdown.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/build_time_breakdown/bt_breakdown.pdf"

data = np.genfromtxt(CSV_FILE_PATH, delimiter=',')
symbol_select_time = np.array([data[:, 0], data[:,3]]).T
code_assign_time = np.array([data[:, 1], data[:, 4]]).T
build_dict_time = np.array([data[:, 2], data[:, 5]]).T

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

fig = plot.figure(figsize=(GRAPH_WIDTH, GRAPH_HEIGHT))
ax = fig.add_subplot(111)

ax.set_xlim([0,1])
ax.set_ylim([0,60])
width =  1/((GROUP_SIZE - 2) * GROUP_NUM + 2 + 4) #Width of bars
xids = []
bars = []

for i in range(2):
    xids.append(width + width * i)
    bars.append(ax.bar(width + width * i, symbol_select_time[i].mean(), width, color=COLORS[0], bottom = 0, linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, label = TYPES[0]))
    bars.append(ax.bar(width + width * i, code_assign_time[i].mean(), width, color=COLORS[1], bottom = symbol_select_time[i].mean(), linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, label = TYPES[1]))
    bars.append(ax.bar(width + width * i, build_dict_time[i].mean(), width, color=COLORS[2], bottom = symbol_select_time[i].mean() + code_assign_time[i].mean(), linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, label = TYPES[2]))

for i in range(2, GROUP_SIZE) :
    pos = []
    for j in range(0, GROUP_NUM) :
        cur_pos = width * 3 + width * (i-1) + width * j * (GROUP_SIZE - 1)
        pos.append(cur_pos)
        xids.append(cur_pos)
    bars.append(ax.bar(pos, symbol_select_time[i], width, color=COLORS[0], bottom = 0, linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, label = TYPES[0]))
    bars.append(ax.bar(pos, code_assign_time[i], width, color=COLORS[1], bottom = symbol_select_time[i], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, label = TYPES[1]))
    bars.append(ax.bar(pos, build_dict_time[i], width, color=COLORS[2], bottom = symbol_select_time[i] + code_assign_time[i], linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, label = TYPES[2]))

xids.sort()
xids.append(width + width - width/2)
xids.append(width * 4 + (width * (GROUP_SIZE-2))/2 - width/2)
xids.append(width * 4 + width * (GROUP_SIZE-2) + width + (width * (GROUP_SIZE-2))/2 - width/2)

ax.tick_params(axis='y', labelsize=Y_TICK_FONT_SIZE)
ax.set_xticks(xids)
ax.set_xticklabels(LABELS[:2] + LABELS[2:] * 2  + XLABEL)
rotations = [20] * len(xids)
xlabel_ypos = [0] * len(xids)
xlabel_sizes = [X_TICK_FONT_SIZE] * len(xids)
xlabel_font_type = ['normal'] * len(xids)
xlabel_ypos[-1] = -0.1
xlabel_ypos[-2] = -0.1
xlabel_ypos[-3] = -0.11
rotations[-1] = 0
rotations[-2] = 0
rotations[-3] = 0
xlabel_sizes[-1] = X_TICK_FONT_SIZE * 1.2
xlabel_sizes[-2] = X_TICK_FONT_SIZE * 1.2
xlabel_sizes[-3] = X_TICK_FONT_SIZE * 1.2
xlabel_font_type[-1] = 'semibold'
xlabel_font_type[-2] = 'semibold'
xlabel_font_type[-3] = 'semibold'
for i, t in enumerate(ax.get_xticklabels()):
    t.set_y(xlabel_ypos[i])
    t.set_rotation(rotations[i])
    t.set_size(xlabel_sizes[i])
    t.set_horizontalalignment('center')
    t.set_fontweight(xlabel_font_type[i])

for i in range(2,len(bars), 3):
    autolabel(bars[i])

plot.ylabel(YLABEL, fontsize=Y_LABEL_FONT_SIZE)
#plot.xlabel(XLABEL, fontsize=16)
plot.legend(handles=bars[:len(TYPES)], loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE})
#plot.title(TITLE)
plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
