#!/usr/bin/env python3
"""Reorder objs.rsp by retail address (median of matched fns' retail hits per obj); objs with no hits keep
their slot relative to the previous obj. usage: rorder.py cmp.json objs.rsp > out.rsp"""
import json, sys, statistics
cmp, rsp = sys.argv[1:3]
objs = [l.rstrip('\r\n') for l in open(rsp) if l.strip()]
hits = {}
for f in json.load(open(cmp)):
    if len(f['hits']) == 1 and ':' not in f['obj']:
        hits.setdefault(f['obj'], []).append(int(f['hits'][0], 16))
key, last = [], 0
for i, o in enumerate(objs):
    h = hits.get(o.split('\\')[-1])
    if h: last = statistics.median(h)
    key.append((last, i))
for _, i in sorted(key): print(objs[i])
