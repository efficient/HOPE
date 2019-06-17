import sys
import os
import numpy as np
RESULT_DIR = './results/'
PREFIX = ['ART', 'btree', 'hot', 'microbench/cpr_latency', 'SuRF', 'SuRF_real']
#PREFIX = ['hot']
TYPE = ['point', 'range']
DATASETS = ['email', 'ts', 'url', 'wiki']
VAR = ['cpr','x','height', 'insertlat', 'lookuplat', 'mem']


def generate_result(dirpath, filename):
    full_path = dirpath + filename
    output_path = dirpath + filename
    results = []
    with open(full_path, 'r') as f:
        print(full_path)
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
            results[idx].append([float(x) for x in line.split(',')])
            idx += 1
        results = (np.mean(np.array(results), axis=1)).tolist()

    # Output results to file
    with open(output_path, 'w') as of:
        for row in results:
            line_result = ''
            for col in row:
                line_result += str(col) + ','
            line_result = line_result.strip(',')
            line_result += '\n'
            of.write(line_result)

for pre in PREFIX:
    for t in TYPE:
        if pre == 'microbench/cpr_latency':
            t = ''
        cur_dir = RESULT_DIR + pre + '/' + t + '/'
        for v in  VAR:
            for d in DATASETS:
                file_prefix = v + '_' + d
                files = os.listdir(cur_dir)
                for f in files:
                    if f.startswith(file_prefix):
                        generate_result(cur_dir, f)

