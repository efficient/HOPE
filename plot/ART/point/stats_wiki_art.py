import matplotlib.pyplot as plt

STATS_DIR = 'results/ART/point/stats_wiki_art.csv'
NAMES = ["Uncompressed", "Single", "Double", "3-Grams, 8192", "3-Grams, 65536", "4-Grams, 8192", "4-Grams, 65536", "ALM, 8192", "ALM, 65536"]
GRAPH_OUTPUT_PATH = "figures/ART/point/stats_wiki_art.pdf"

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8.0 #inches
X_TICK_FONT_SIZE = 8

cnt_N4  = []
cnt_N16 = []
cnt_N48 = []
cnt_N256 = []
with open(STATS_DIR, 'r') as f:
    rows = f.readlines()
    for row in rows:
        cnts = row.split(',')
        cnt_N4.append(cnts[0])
        cnt_N16.append(cnts[1])
        cnt_N48.append(cnts[2])
        cnt_N256.append(cnts[3])

fig = plt.figure(figsize={GRAPH_HEIGHT, GRAPH_WIDTH})
ax = fig.add_subplot(111)

ax.bar(range(len(cnt_N4)), cnt_N4, label='N4', fc='#FADBD8')
ax.bar(range(len(cnt_N16)), cnt_N16, label='N16', fc='#F1948A')
ax.bar(range(len(cnt_N48)), cnt_N48, label='N48', fc='#E74C3C')
ax.bar(range(len(cnt_N256)), cnt_N256, label='N256', fc='#641E16')

#ax.get_xaxis().set_visible(False)

#ax.legend()
ax.set_xticklabels(NAMES)
for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

plt.setp(ax.get_xticklabels(), rotation=30, horizontalalignment='center')
plt.gcf().subplots_adjust(bottom=0.15)
plt.savefig(GRAPH_OUTPUT_PATH, bbox='tight')


