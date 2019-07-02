import sys
import os
import numpy as np
RESULT_DIR = './results/'
PREFIX = ['ART', 'btree', 'hot', 'microbench/cpr_latency', 'SuRF', 'SuRF_real']
#PREFIX = ['SuRF', 'SuRF_real','ART_single']
TYPE = ['point', 'range']
DATASETS = ['email', 'ts', 'url', 'wiki']
VAR = ['cpr','x','height', 'fpr', 'lat', 'insertlat', 'lookuplat', 'mem']


def generate_result(dirpath, filename):
    full_path = dirpath + filename
    output_path = dirpath + filename
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
#                        print(f,file_prefix)
                        generate_result(cur_dir, f)

