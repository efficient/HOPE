import sys
import matplotlib as mpl
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np
import csv

LABELS = ["Collect Symbol Freq Stats", "Generate Encoding Intervals", "Compute Interval Freq", "Hu-Tucker Algorithm", "Build Trie Dict"]

COLORS = ['#fef0d9', '#fdcc8a', '#fc8d59', '#e34a33', '#b30000']

#EXPLODE = [0.1, 0.1, 0.1, 0.1, 0.1]
EXPLODE = [0.05, 0.05, 0.05, 0.05, 0.05]

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 5 #inches

CSV_FILE_PATH = "results/microbench/build_time_breakdown/bt_breakdown.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/build_time_breakdown/bt_10_breakdown.pdf"

f_in = open(CSV_FILE_PATH)
reader = csv.reader(f_in)
csvrows = list(reader)
data = []
for row in csvrows :
    for item in row :
        data.append(float(item))

data_100 = data[0:5]
data_10 = data[5:10]
data_1 = data[10:15]

sizes_100 = []
for t in data_100 :
    s = t / sum(data_100) * 100.0
    sizes_100.append(s)

sizes_10 = []
for t in data_10 :
    s = t / sum(data_10) * 100.0
    sizes_10.append(s)

sizes_1 = []
for t in data_1 :
    s = t / sum(data_1) * 100.0
    sizes_1.append(s)

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

fig = plot.figure(figsize={GRAPH_HEIGHT, GRAPH_WIDTH})
ax = fig.add_subplot(111)

#ax.pie(sizes_10, explode=EXPLODE, labels=LABELS, colors=COLORS,
#       autopct='%1.1f\%%', shadow=True, startangle=90)

ax.pie(sizes_10, explode=EXPLODE, colors=COLORS, startangle=90)

ax.axis('equal')

plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
