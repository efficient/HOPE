import sys
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np
import csv

LABELS = ["Single", "Double", "3-Gram", "4-Gram", "ALM", "ALM-Improved"]
XLABEL = "Compression Rate"
YLABEL = "Build Time(s)"
X_LABEL_FONT_SIZE = 15
Y_LABEL_FONT_SIZE = 15
X_TICK_FONT_SIZE = 8
Y_TICK_FONT_SIZE = 10

TITLE = "Build Time Breakdown (dictionary size = 65536)"

COLORS = ['#fef0d9', '#fc8d59', '#b30000']

LEGEND_FONT_SIZE = 10
LEGEND_POS = 'upper left'

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 5 #inches

BORDER_SIZE = 0.5
BORDER_COLOR = 'black'
WIDTH = 0.8 #Width of bars
CSV_FILE_PATH = "results/microbench/build_time_breakdown/bt_breakdown.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/build_time_breakdown/bt_breakdown.pdf"

data = np.genfromtxt(CSV_FILE_PATH, delimiter=',')
symbol_select_time = data[:, 0]
code_assign_time = data[:, 1]
build_dict_time = data[:, 2]

xid = [x for x in range(len(build_dict_time))]

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

ax.bar(xid, symbol_select_time, width=WIDTH, linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, label="Symbol Select", color=COLORS[0], bottom=code_assign_time+build_dict_time)
ax.bar(xid, code_assign_time, width=WIDTH, linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, label="Code Assign(Hu-Tucker/Fixed Size)", color=COLORS[1], bottom=build_dict_time)
ax.bar(xid, build_dict_time, width=WIDTH, linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, label="Build Dictionary", color=COLORS[2])

ax.tick_params(axis='x', labelsize=X_TICK_FONT_SIZE)
ax.tick_params(axis='y', labelsize=Y_TICK_FONT_SIZE)

plot.xticks(xid, LABELS)
plot.ylabel(YLABEL, fontsize=Y_LABEL_FONT_SIZE)
plot.xlabel(XLABEL, fontsize=X_LABEL_FONT_SIZE)
plot.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE})
#plot.title(TITLE)
plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
