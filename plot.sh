!bin/bash

############################################################
# Sample Size Sweep
############################################################
#python plot/microbench/sample_size_sweep/cpr_email.py
#python plot/microbench/sample_size_sweep/cpr_wiki.py
#python plot/microbench/sample_size_sweep/cpr_url.py

#python plot/microbench/sample_size_sweep/bt_email.py
#python plot/microbench/sample_size_sweep/bt_wiki.py
#python plot/microbench/sample_size_sweep/bt_url.py

############################################################
# CPR Latency
############################################################
python plot/microbench/cpr_latency/cpr_email.py
python plot/microbench/cpr_latency/cpr_wiki.py
python plot/microbench/cpr_latency/cpr_url.py

python plot/microbench/cpr_latency/lat_email.py
python plot/microbench/cpr_latency/lat_wiki.py
python plot/microbench/cpr_latency/lat_url.py

python plot/microbench/cpr_latency/mem_email.py
python plot/microbench/cpr_latency/mem_wiki.py
python plot/microbench/cpr_latency/mem_url.py

############################################################
# Hu-Tucker
############################################################
#python plot/microbench/hu_tucker/hu_tucker_email.py

############################################################
# Array vs. Trie
############################################################
#python plot/microbench/array_trie/array_trie_email.py

############################################################
# Hu-Tucker vs. Fixed-Len Dict
############################################################
#python plot/microbench/ht_vs_dc/cpr_email_ht_vs_dc.py
#python plot/microbench/ht_vs_dc/lat_email_ht_vs_dc.py
#python plot/microbench/ht_vs_dc/bt_email_ht_vs_dc.py

############################################################
# Build Time Breakdown
############################################################
#python plot/microbench/build_time_breakdown/bt_100_breakdown.py
#python plot/microbench/build_time_breakdown/bt_10_breakdown.py
#python plot/microbench/build_time_breakdown/bt_1_breakdown.py

############################################################
# SuRF point
############################################################
python plot/SuRF/point/lat_mem_email_surf.py
python plot/SuRF/point/lat_mem_wiki_surf.py
python plot/SuRF/point/lat_mem_url_surf.py

python plot/SuRF/point/fpr_email_surf.py
python plot/SuRF/point/fpr_wiki_surf.py
python plot/SuRF/point/fpr_url_surf.py

python plot/SuRF/point/height_email_surf.py
python plot/SuRF/point/height_wiki_surf.py
python plot/SuRF/point/height_url_surf.py

############################################################
# SuRF range
############################################################
python plot/SuRF/range/lat_mem_email_surf.py
python plot/SuRF/range/lat_mem_wiki_surf.py
python plot/SuRF/range/lat_mem_url_surf.py

python plot/SuRF/range/fpr_email_surf.py
python plot/SuRF/range/fpr_wiki_surf.py
python plot/SuRF/range/fpr_url_surf.py

############################################################
# SuRF-Real8 point
############################################################
#python plot/SuRF_real/point/lat_mem_email_surfreal.py
#python plot/SuRF_real/point/lat_mem_wiki_surfreal.py
#python plot/SuRF_real/point/lat_mem_url_surfreal.py

############################################################
# SuRF-Real8 range
############################################################
#python plot/SuRF_real/range/lat_mem_email_surfreal.py
#python plot/SuRF_real/range/lat_mem_wiki_surfreal.py
#python plot/SuRF_real/range/lat_mem_url_surfreal.py

############################################################
# ART point
############################################################
python plot/ART/point/lat_mem_email_art.py
python plot/ART/point/lat_mem_wiki_art.py
python plot/ART/point/lat_mem_url_art.py

python plot/ART/point/height_email_art.py
python plot/ART/point/height_wiki_art.py
python plot/ART/point/height_url_art.py

############################################################
# ART range
############################################################
python plot/ART/range/lat_mem_email_art.py
python plot/ART/range/lat_mem_wiki_art.py
python plot/ART/range/lat_mem_url_art.py

############################################################
# ART Node Stats
############################################################
python plot/ART/point/mem_email_art.py
python plot/ART/point/mem_wiki_art.py
python plot/ART/point/mem_url_art.py

############################################################
# SuRF Memory Stats
############################################################
python plot/SuRF/point/mem_email_surf.py
python plot/SuRF/point/mem_wiki_surf.py
python plot/SuRF/point/mem_url_surf.py

############################################################
# Btree point
############################################################
python plot/btree/point/lat_mem_email_btree.py
python plot/btree/point/lat_mem_wiki_btree.py
python plot/btree/point/lat_mem_url_btree.py

############################################################
# Btree range
############################################################
python plot/btree/range/lat_mem_email_btree.py
python plot/btree/range/lat_mem_wiki_btree.py
python plot/btree/range/lat_mem_url_btree.py
