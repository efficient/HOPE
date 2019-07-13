import sys
import os

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

#####################################################################################

if (len(sys.argv) < 3) :
    print bcolors.FAIL + 'Usage:'
    print 'arg 1, key type: randint, email, url, wiki, timestamp' 
    print 'arg 2, distribution: uniform, zipfian, latest' + bcolors.ENDC
    sys.exit()

key_type = sys.argv[1]
distribution = sys.argv[2]

print bcolors.OKGREEN +  'key type = ' + key_type
print 'distribution = ' + distribution + bcolors.ENDC

ycsb_dir = 'YCSB/bin/'
workload_dir = 'workload_spec/'
dataset_dir = '../datasets/'
output_dir='../workloads/'

email_list = dataset_dir + 'emails.txt'
#email_list_size = 27549660
email_list_size = 27000000

url_list = dataset_dir + 'urls.txt'
#url_list_size = 105896555
url_list_size = 25000000

wiki_titles = dataset_dir + 'wikis.txt'
#wiki_titles_size = 14053856
wiki_titles_size = 14000000

timestamp_list = dataset_dir + 'poisson_timestamps.csv'
timestamp_list_size = 100000000

if key_type != 'randint' and key_type != 'email' and key_type != 'url' and key_type != 'wiki' and key_type  != 'timestamp' :
    print bcolors.FAIL + 'Incorrect key_type: please pick from randint, email, url, and wiki' + bcolors.ENDC
    sys.exit()

if distribution != 'uniform' and distribution != 'zipfian' and distribution != 'latest' :
    print bcolors.FAIL + 'Incorrect distribution: please pick from uniform, zipfian and latest' + bcolors.ENDC
    sys.exit()

out_ycsb_txn = output_dir + 'ycsb_txn_' + key_type + '_' + distribution
out_txn = output_dir + 'scan_len_' + key_type + '_' + distribution

cmd_ycsb_txn = ycsb_dir + 'ycsb run basic -P ' + workload_dir + 'workloade_' + key_type + '_' + distribution + ' -s > ' + out_ycsb_txn

os.system(cmd_ycsb_txn)

#####################################################################################

f_txn = open (out_ycsb_txn, 'r')
f_txn_out = open (out_txn, 'w')
for line in f_txn :
    cols = line.split()
    if len(cols) > 3 and cols[0] == 'SCAN' :
        f_txn_out.write (cols[3] + "\n")
f_txn.close()
f_txn_out.close()

cmd = 'rm -f ' + out_ycsb_txn
os.system(cmd)
