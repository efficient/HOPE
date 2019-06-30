#!/bin/bash

python gen_load.py randint uniform
python gen_txn.py randint zipfian

python gen_load.py email uniform
python gen_txn.py email zipfian

python gen_load.py url uniform
python gen_txn.py url zipfian

python gen_load.py wiki uniform
python gen_txn.py wiki zipfian

python gen_load.py timestamp uniform
python gen_txn.py timestamp zipfian

