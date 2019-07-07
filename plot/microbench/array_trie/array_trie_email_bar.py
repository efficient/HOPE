import sys
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plot
import matplotlib.ticker as ticker
import numpy as np

LABELS = ['3-Gram', '4-Gram']
TYPES = ['Array', 'Trie']

COLORS = ['#fdcc8a', '#d7301f']
WIDTH = 0.2

Y_LABEL = "Encode Latency (ns per char)"
Y_LABEL_FONT_SIZE = 20

X_LABEL = "Encode Scheme"
X_LABEL_FONT_SIZE = 20

X_TICK_FONT_SIZE = 16
Y_TICK_FONT_SIZE = 16

X_START = 2**9
X_LIMIT = 2**18

LEGEND_FONT_SIZE = 16
LEGEND_POS = 'upper left'

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8 #inches

BORDER_SIZE = 0.5
BORDER_COLOR = 'black'

CSV_FILE_PATH = "results/microbench/array_trie/lat_email_array.csv"
GRAPH_OUTPUT_PATH = "figures/microbench/array_trie/lat_email_array_trie.pdf"
DICT_ID = 5

data = np.genfromtxt(CSV_FILE_PATH, delimiter=',')
print(data)
array = [data[DICT_ID,0], data[DICT_ID,2]]
trie = [data[DICT_ID,1], data[DICT_ID,3]]

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

idx = np.arange(2) / 1.5
ax.bar(idx, array, WIDTH, linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, color=COLORS[0], label=TYPES[0])
ax.bar(idx + WIDTH, trie, WIDTH, linewidth = BORDER_SIZE, edgecolor = BORDER_COLOR, color=COLORS[1], label=TYPES[1])

ax.set_xticks(idx + WIDTH/ 2)
ax.set_xticklabels(LABELS)

for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

for label in ax.get_yticklabels():
    label.set_fontsize(Y_TICK_FONT_SIZE)

ax.set_xlabel(X_LABEL, fontsize=X_LABEL_FONT_SIZE)
ax.set_ylabel(Y_LABEL, fontsize=Y_LABEL_FONT_SIZE)

ax.legend(loc=LEGEND_POS, prop={'size':LEGEND_FONT_SIZE}, ncol=2)

plot.savefig(GRAPH_OUTPUT_PATH, bbox_inches='tight')
