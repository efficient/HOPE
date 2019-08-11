import sys
import os
import numpy as np
RESULT_DIR = './results/'
#PREFIX = ['ART', 'btree', 'hot', 'microbench/cpr_latency', 'SuRF', 'SuRF_real']
PREFIX = ['microbench/cpr_latency', 'SuRF', 'SuRF_real']
#TYPE = ['point', 'range']
TYPE = ['point']
DATASETS = ['email', 'ts', 'url', 'wiki']
VAR = ['cpr','x','height', 'fpr', 'lat', 'insertlat', 'lookuplat', 'stats']
BT_OUTFILE = "results/microbench/build_time_breakdown/bt_breakdown.csv"

def generate_result_single(dirpath, filename):
    full_path = dirpath + filename
    output_path = dirpath + 'final_' + filename
    results = []
    with open(full_path, 'r') as f:
#        print(full_path)
        lines = f.readlines()
        cnt = 0
        for line in lines:
            line = line.strip('\n')
            if line == '-':
               break
            cnt += 1
        results = [[] for i in range(cnt)]
        idx = 0
        for i,line in enumerate(lines):
            line = line.strip(',\n')
            #print line, line == '-'
            if line == '-':
                idx = 0
                continue
            #print([float(x) for x in line.split(',')])
            results[idx].append(np.array([float(x) for x in line.split(',')]))
            idx += 1
        results = (np.mean(np.asarray(results), axis=1))

    # Output results to file
    with open(output_path, 'w') as of:
        for row in results:
            line_result = ''
            for col in row:
                line_result += str(col) + ','
            line_result = line_result.strip(',')
            line_result += '\n'
            of.write(line_result)

def microtree():
    for pre in PREFIX:
        for t in TYPE:
            if pre == 'microbench/cpr_latency':
               cur_dir = RESULT_DIR + pre + '/'
            else:
                cur_dir = RESULT_DIR + pre + '/' + t + '/'
            for v in  VAR:
                for d in DATASETS:
                    file_prefix = v + '_' + d
                    files = os.listdir(cur_dir)
                    for f in files:
                        if f.startswith(file_prefix):
                            generate_result_single(cur_dir, f)


def buildtime():
    ss_time = []
    encode_time = []
    build_dict_time = []

    with open("bt") as f:
        lines = f.readlines()
        for line in lines:
            wl = line.split("=")
            key = wl[0].strip()
            if (key == "Symbol Select time"):
                ss_time.append(wl[1].strip())
            if (key == "Code Assign(Hu-Tucker) time"):
                encode_time.append(wl[1].strip())
            if (key == "Build Dictionary time"):
                build_dict_time.append(wl[1].strip())

#    print(ss_time)
#    print(encode_time)
#    print(build_dict_time)
    with open(BT_OUTFILE, 'w') as f:
        for i in range(0, len(ss_time)):
            f.write(ss_time[i] + "," + encode_time[i] + "," + build_dict_time[i]+"\n")


def triearray():
    pass


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(sys.argv)
        print("Did not generate any results, return")
        exit(0)

    exp_type = sys.argv[1]

    if exp_type == "micro-tree":
        microtree()
    elif exp_type == "bt":
        buildtime()
    elif exp_type == "ta":
        triearray()
    else:
        print("Unkown experiment type")
