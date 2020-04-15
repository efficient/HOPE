#!/bin/bash

run_microbench=$1
run_surf=$2
run_art=$3
run_hot=$4
run_btree=$5
run_prefixbtree=$6
run_breakdown=$7

PYTHON='python'
if [[ ${run_microbench} == 1 ]]
then
    ############################################################
    # Sample Size Sweep
    ############################################################
    #${PYTHON} ./plot/microbench/sample_size_sweep/cpr_email.py
    #${PYTHON} ./plot/microbench/sample_size_sweep/cpr_wiki.py
    #${PYTHON} ./plot/microbench/sample_size_sweep/cpr_url.py

    #${PYTHON} ./plot/microbench/sample_size_sweep/bt_email.py
    #${PYTHON} ./plot/microbench/sample_size_sweep/bt_wiki.py
    #${PYTHON} ./plot/microbench/sample_size_sweep/bt_url.py

    ############################################################
    # CPR Latency
    ############################################################
    ${PYTHON} ./plot/microbench/cpr_latency/cpr_email.py
    ${PYTHON} ./plot/microbench/cpr_latency/cpr_wiki.py
    ${PYTHON} ./plot/microbench/cpr_latency/cpr_url.py

    ${PYTHON} ./plot/microbench/cpr_latency/lat_email.py
    ${PYTHON} ./plot/microbench/cpr_latency/lat_wiki.py
    ${PYTHON} ./plot/microbench/cpr_latency/lat_url.py

    ${PYTHON} ./plot/microbench/cpr_latency/mem_email.py
    ${PYTHON} ./plot/microbench/cpr_latency/mem_wiki.py
    ${PYTHON} ./plot/microbench/cpr_latency/mem_url.py

    ############################################################
    # Hu-Tucker
    ############################################################
    #${PYTHON} ./plot/microbench/hu_tucker/hu_tucker_email.py

    ############################################################
    # Array vs. Trie
    ############################################################
    #${PYTHON} ./plot/microbench/array_trie/array_trie_email.py

    ############################################################
    # Hu-Tucker vs. Fixed-Len Dict
    ############################################################
    #${PYTHON} ./plot/microbench/ht_vs_dc/cpr_email_ht_vs_dc.py
    #${PYTHON} ./plot/microbench/ht_vs_dc/lat_email_ht_vs_dc.py
    #${PYTHON} ./plot/microbench/ht_vs_dc/bt_email_ht_vs_dc.py

    ############################################################
    # Build Time Breakdown
    ############################################################
    #${PYTHON} ./plot/microbench/build_time_breakdown/bt_100_breakdown.py
    #${PYTHON} ./plot/microbench/build_time_breakdown/bt_10_breakdown.py
    #${PYTHON} ./plot/microbench/build_time_breakdown/bt_1_breakdown.py
fi

if [[ ${run_surf} == 1 ]]
then
    ############################################################
    # SuRF point
    ############################################################
    ${PYTHON} ./plot/SuRF/point/lat_mem_email_surf.py
    ${PYTHON} ./plot/SuRF/point/lat_mem_wiki_surf.py
    ${PYTHON} ./plot/SuRF/point/lat_mem_url_surf.py

    ${PYTHON} ./plot/SuRF/point/height_email_surf.py
    ${PYTHON} ./plot/SuRF/point/height_wiki_surf.py
    ${PYTHON} ./plot/SuRF/point/height_url_surf.py

    ${PYTHON} ./plot/SuRF/point/fprcompare_email_surf.py
    ############################################################
    # SuRF range
    ############################################################
    ${PYTHON} ./plot/SuRF/range/lat_email_surf.py
    ${PYTHON} ./plot/SuRF/range/lat_wiki_surf.py
    ${PYTHON} ./plot/SuRF/range/lat_url_surf.py

fi

if [[ ${run_art} == 1 ]]
then
    ############################################################
    # ART point
    ############################################################
    ${PYTHON} ./plot/ART/point/lookuplat_mem_email_art.py
    ${PYTHON} ./plot/ART/point/lookuplat_mem_wiki_art.py
    ${PYTHON} ./plot/ART/point/lookuplat_mem_url_art.py

    ${PYTHON} ./plot/ART/point/height_email_art.py
    ${PYTHON} ./plot/ART/point/height_wiki_art.py
    ${PYTHON} ./plot/ART/point/height_url_art.py

    ############################################################
    # ART range
    ############################################################
    ${PYTHON} ./plot/ART/range/lat_email.py
    ${PYTHON} ./plot/ART/range/lat_wiki.py
    ${PYTHON} ./plot/ART/range/lat_url.py
fi

if [[ ${run_btree} == 1 ]]
then
    ############################################################
    # Btree point
    ############################################################
    ${PYTHON} ./plot/btree/point/lookuplat_mem_email_btree.py
    ${PYTHON} ./plot/btree/point/lookuplat_mem_wiki_btree.py
    ${PYTHON} ./plot/btree/point/lookuplat_mem_url_btree.py

    ############################################################
    # Btree range
    ############################################################
    ${PYTHON} ./plot/btree/range/lat_email_btree.py
    ${PYTHON} ./plot/btree/range/lat_wiki_btree.py
    ${PYTHON} ./plot/btree/range/lat_url_btree.py
fi


if [[ ${run_prefixbtree} == 1 ]]
then
    ############################################################
    # Btree point
    ############################################################
    ${PYTHON} ./plot/prefixbtree/point/lookuplat_mem_email_btree.py
    ${PYTHON} ./plot/prefixbtree/point/lookuplat_mem_wiki_btree.py
    ${PYTHON} ./plot/prefixbtree/point/lookuplat_mem_url_btree.py

    ############################################################
    # Btree range
    ############################################################
    ${PYTHON} ./plot/prefixbtree/range/lat_email_btree.py
    ${PYTHON} ./plot/prefixbtree/range/lat_wiki_btree.py
    ${PYTHON} ./plot/prefixbtree/range/lat_url_btree.py
fi


if [[ ${run_hot} == 1 ]]
then
    ############################################################
    # hot point
    ############################################################
    ${PYTHON} ./plot/hot/point/lookuplat_mem_email_hot.py
    ${PYTHON} ./plot/hot/point/lookuplat_mem_wiki_hot.py
    ${PYTHON} ./plot/hot/point/lookuplat_mem_url_hot.py

    ############################################################
    # hot range
    ############################################################
    ${PYTHON} ./plot/hot/range/lookuplat_mem_email_hot.py
    ${PYTHON} ./plot/hot/range/lookuplat_mem_wiki_hot.py
    ${PYTHON} ./plot/hot/range/lookuplat_mem_url_hot.py

    ############################################################
    # hot height
    ############################################################
    ${PYTHON} ./plot/hot/point/height_email_hot.py
    ${PYTHON} ./plot/hot/point/height_wiki_hot.py
    ${PYTHON} ./plot/hot/point/height_url_hot.py
fi


if [[ ${run_breakdown} == 1 ]]
then
    ${PYTHON} ./plot/microbench/build_time_breakdown/bt_breakdown.py
    ${PYTHON} ./plot/microbench/array_trie/array_trie_email_bar.py
fi
