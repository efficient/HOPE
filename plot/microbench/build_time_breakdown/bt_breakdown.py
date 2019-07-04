import sys
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np
import csv

LABELS = ["Single", "Double", "3-Gram", "4-Gram", "AML"]
XLABEL = "Compression Rate"
YLABEL = "Build Time(s)"
TITLE = "Build Time Breakdown (dictionary size = 65536)"

COLORS = ['#fef0d9', '#fc8d59', '#b30000']

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 5 #inches

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

fig = plot.figure(figsize={GRAPH_HEIGHT, GRAPH_WIDTH})
ax = fig.add_subplot(111)

ax.bar(xid, symbol_select_time, width=0.8, label="Symbol Select", color=COLORS[0], bottom=code_assign_time+build_dict_time)
ax.bar(xid, code_assign_time, width=0.8, label="Code Assign(Hu-Tucker/Fixed Size)", color=COLORS[1], bottom=build_dict_time)
ax.bar(xid, build_dict_time, width=0.8, label="Build Dictionary", color=COLORS[2])

plot.xticks(xid, LABELS)
plot.ylabel(YLABEL)
plot.xlabel(XLABEL)
plot.legend(loc="upper left")
plot.title(TITLE)
plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
