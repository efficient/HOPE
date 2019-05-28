import matplotlib.pyplot as plt

STATS_DIR = 'results/SuRF/point/stats_email_surf.csv'
NAMES = ["Uncompressed", "Single", "Double", "3-Grams, 8192", "3-Grams, 65536", "4-Grams, 8192", "4-Grams, 65536", "ALM, 8192", "ALM, 65536"]
GRAPH_OUTPUT_PATH = "figures/SuRF/point/stats_email_surf.pdf"

GRAPH_HEIGHT = 4.5 #inches
GRAPH_WIDTH = 8.0 #inches
X_TICK_FONT_SIZE = 8

filter_mem  = []
encoder_mem = []
with open(STATS_DIR, 'r') as f:
    rows = f.readlines()
    for row in rows:
        row = row.split('\n')[0]
        cnts = row.split(',')
        filter_mem.append(float(cnts[1]))
        encoder_mem.append(float(cnts[2]))

fig = plt.figure(figsize={GRAPH_HEIGHT, GRAPH_WIDTH})
ax = fig.add_subplot(111)
print(filter_mem)
print(encoder_mem) 
encoder_mem[-1] = 100
ax.bar(range(len(filter_mem)), filter_mem, label='filter', fc='#FADBD8')
ax.bar(range(len(encoder_mem)), encoder_mem, label='encoder', fc='#F1948A')
#ax.get_xaxis().set_visible(False)

#ax.legend()
ax.set_xticklabels(NAMES)
for label in ax.get_xticklabels():
    label.set_fontsize(X_TICK_FONT_SIZE)

plt.setp(ax.get_xticklabels(), rotation=30, horizontalalignment='center')
plt.gcf().subplots_adjust(bottom=0.15)
plt.savefig(GRAPH_OUTPUT_PATH, bbox='tight')


