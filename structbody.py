#!/usr/bin/env python3
"""Print a struct's body from a header: structbody.py <file> <struct-name>"""
import re, sys
s = open(sys.argv[1], errors='replace').read()
m = re.search(r'struct\s+%s\s*\{' % re.escape(sys.argv[2]), s)
i = m.end(); depth = 1
while depth:
    depth += {'{': 1, '}': -1}.get(s[i], 0); i += 1
print(re.sub(r'[ \t]+', ' ', s[m.start():i]))
