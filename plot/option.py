import matplotlib as mpl
#mpl.rcParams['hatch.linewidth'] = 0.5

GRAPH_HEIGHT = 5
GRAPH_RANGE_HEIGHT = 4.5
GRAPH_WIDTH = 8

X_LABEL_FONT_SIZE = 24
Y_LABEL_FONT_SIZE = 24

X_TICK_FONT_SIZE = 18
Y_TICK_FONT_SIZE = 18

BORDER_SIZE = 0.5
BORDER_COLOR = 'black'

HATCH = '/' * 8

ARROW_WIDTH = 2
ARROW_HEAD_WIDRH = 8
ARROW_HEAD_LEN = 10

MARKERS = ["o", "^", "v", "s", "d", "p", "h"]
MARKER_SIZE = 150
ANNOTATOR_SIZE = 13

SCALE = 1.2
LABEL_NUM = 4

def getLimit(data):
  return max(data) * SCALE

def getRoundTickGap(gap):
  if gap < 10:
    return int(gap)
  if gap < 100:
    return int(gap/10)*10
  if gap < 500:
    return int(gap/50)*50
  if gap < 2000:
    return int(gap/100)*100
  return int(gap/1000)*1000

def getTicks(lim):
  return list(range(0, int(lim), int(getRoundTickGap(1 + lim / LABEL_NUM))))


#BENCH_TYPE = 'big'
BENCH_TYPE = 'small'
