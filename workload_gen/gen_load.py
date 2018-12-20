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

print bcolors.OKGREEN + 'key type = ' + key_type 
print 'distribution = ' + distribution + bcolors.ENDC

ycsb_dir = 'YCSB/bin/'
workload_dir = 'workload_spec/'
dataset_dir = '../datasets/'
output_dir='../workloads/'

email_list = dataset_dir + 'emails.txt'
#email_list_size = 27549660
email_list_size = 25000000
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

out_ycsb_load = output_dir + 'ycsb_load_' + key_type
out_load_ycsbkey = output_dir + 'load_' + 'ycsbkey'
out_load = output_dir + 'load_' + key_type

cmd_ycsb_load = ycsb_dir + 'ycsb load basic -P ' + workload_dir + 'workloadc_' + key_type + '_' + distribution + ' -s > ' + out_ycsb_load

os.system(cmd_ycsb_load)

#####################################################################################

f_load = open (out_ycsb_load, 'r')
f_load_out = open (out_load_ycsbkey, 'w')
for line in f_load :
    cols = line.split()
    if len(cols) > 2 and cols[0] == "INSERT":
        f_load_out.write (cols[2][4:] + '\n')
f_load.close()
f_load_out.close()

cmd = 'rm -f ' + out_ycsb_load
os.system(cmd)

#####################################################################################

if key_type == 'randint' :
    f_load = open (out_load_ycsbkey, 'r')
    f_load_out = open (out_load, 'w')
    for line in f_load :
        f_load_out.write (line)

elif key_type == 'email' :
    email_keymap = {}
    f_email_keymap = open (email_keymap_file, 'w')

    f_email = open (email_list, 'r')
    emails = f_email.readlines()

    f_load = open (out_load_ycsbkey, 'r')
    f_load_out = open (out_load, 'w')

    sample_size = len(f_load.readlines())
    gap = email_list_size / sample_size

    f_load.close()
    f_load = open (out_load_ycsbkey, 'r')
    count = 0
    for line in f_load :
        cols = line.split()
        #email = reverseHostName(emails[count * gap])
        email = emails[count * gap].rstrip()
        f_load_out.write (email + '\n')
        f_email_keymap.write (cols[0] + ' ' + email + '\n')
        count += 1
    f_email_keymap.close()

elif key_type == 'url' :
    url_keymap = {}
    f_url_keymap = open (url_keymap_file, 'w', )

    f_url = open (url_list, 'r')
    urls = f_url.readlines()

    f_load = open (out_load_ycsbkey, 'r')
    f_load_out = open (out_load, 'w')

    sample_size = len(f_load.readlines())
    gap = url_list_size / sample_size

    f_load.close()
    f_load = open (out_load_ycsbkey, 'r')
    count = 0
    for line in f_load :
        cols = line.split()
        url = urls[count * gap].rstrip()
        f_load_out.write (url + '\n')
        f_url_keymap.write (cols[0] + ' ' + url + '\n')
        count += 1
    f_url_keymap.close()

elif key_type == 'wiki' :
    wiki_keymap = {}
    f_wiki_keymap = open (wiki_keymap_file, 'w')

    f_wiki = open (wiki_titles, 'r')
    titles = f_wiki.readlines()

    f_load = open (out_load_ycsbkey, 'r')
    f_load_out = open (out_load, 'w')

    sample_size = len(f_load.readlines())
    gap = wiki_titles_size / sample_size

    f_load.close()
    f_load = open (out_load_ycsbkey, 'r')
    count = 0
    for line in f_load :
        cols = line.split()
        title = titles[count * gap]
        f_load_out.write (title)
        f_wiki_keymap.write (cols[0] + ' ' + title)
        count += 1
    f_wiki_keymap.close()

f_load.close()
f_load_out.close()

cmd = 'rm -f ' + out_load_ycsbkey
os.system(cmd)
