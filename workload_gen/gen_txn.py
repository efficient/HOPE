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

def reverseHostName ( email ) :
    name, sep, host = email.partition('@')
    hostparts = host[:-1].split('.')
    r_host = ''
    for part in hostparts :
        r_host = part + '.' + r_host
    return r_host + sep + name

#####################################################################################

if (len(sys.argv) < 3) :
    print bcolors.FAIL + 'Usage:'
    print 'arg 1, key type: randint, email, url, wiki' 
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
email_keymap_file = output_dir + 'email_keymap.txt'

url_list = dataset_dir + 'urls.txt'
#url_list_size = 105896555
url_list_size = 25000000
url_keymap_file = output_dir + 'url_keymap.txt'

wiki_titles = dataset_dir + 'wikis.txt'
#wiki_titles_size = 14053856
wiki_titles_size = 14000000
wiki_keymap_file = output_dir + 'wiki_keymap.txt'

if key_type != 'randint' and key_type != 'email' and key_type != 'url' and key_type != 'wiki' :
    print bcolors.FAIL + 'Incorrect key_type: please pick from randint, email, url, and wiki' + bcolors.ENDC
    sys.exit()

if distribution != 'uniform' and distribution != 'zipfian' and distribution != 'latest' :
    print bcolors.FAIL + 'Incorrect distribution: please pick from uniform, zipfian and latest' + bcolors.ENDC
    sys.exit()

out_ycsb_txn = output_dir + 'ycsb_txn_' + key_type + '_' + distribution
out_txn_ycsbkey = output_dir + 'txn_' + 'ycsbkey' + '_' + distribution
out_txn = output_dir + 'txn_' + key_type + '_' + distribution

cmd_ycsb_txn = ycsb_dir + 'ycsb run basic -P ' + workload_dir + 'workloadc_' + key_type + '_' + distribution + ' -s > ' + out_ycsb_txn

os.system(cmd_ycsb_txn)

#####################################################################################

f_txn = open (out_ycsb_txn, 'r')
f_txn_out = open (out_txn_ycsbkey, 'w')
for line in f_txn :
    cols = line.split()
    if len(cols) > 2 and cols[0] == 'READ' :
        f_txn_out.write (cols[2][4:] + "\n")
f_txn.close()
f_txn_out.close()

cmd = 'rm -f ' + out_ycsb_txn
os.system(cmd)

#####################################################################################

if key_type == 'randint' :
    f_txn = open (out_txn_ycsbkey, 'r')
    f_txn_out = open (out_txn, 'w')
    for line in f_txn :
        f_txn_out.write (line)

elif key_type == 'email' :
    email_keymap = {}
    f_email_keymap = open (email_keymap_file, 'r')
    for line in f_email_keymap :
        cols = line.split()
        email_keymap[int(cols[0])] = cols[1]

    count = 0
    f_txn = open (out_txn_ycsbkey, 'r')
    f_txn_out = open (out_txn, 'w')
    for line in f_txn :
        cols = line.split()
        if len(cols) > 0 :
            f_txn_out.write (email_keymap[int(cols[0])] + '\n')
    f_email_keymap.close()

elif key_type == 'url' :
    url_keymap = {}
    f_url_keymap = open (url_keymap_file, 'r')
    for line in f_url_keymap :
        cols = line.split()
        url_keymap[int(cols[0])] = cols[1]

    count = 0
    f_txn = open (out_txn_ycsbkey, 'r')
    f_txn_out = open (out_txn, 'w')
    for line in f_txn :
        cols = line.split()
        if len(cols) > 0 :
            f_txn_out.write (url_keymap[int(cols[0])] + '\n')
    f_url_keymap.close()

elif key_type == 'wiki' :
    wiki_keymap = {}
    f_wiki_keymap = open (wiki_keymap_file, 'r')
    for line in f_wiki_keymap :
        cols = line.split()
        wiki_keymap[int(cols[0])] = cols[1]

    count = 0
    f_txn = open (out_txn_ycsbkey, 'r')
    f_txn_out = open (out_txn, 'w')
    for line in f_txn :
        cols = line.split()
        if len(cols) > 0 :
            f_txn_out.write (wiki_keymap[int(cols[0])] + '\n')
    f_wiki_keymap.close()

f_txn.close()
f_txn_out.close()

cmd = 'rm -f ' + out_txn_ycsbkey
os.system(cmd)
