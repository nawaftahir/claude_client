#!/usr/bin/env python3
"""Paren-aware comparison variants of one C source line (operand swap, integer bound shift, f-suffix)."""
import re
FLIP = {'<': '>', '>': '<', '<=': '>=', '>=': '<=', '==': '==', '!=': '!='}
OPS = re.compile(r'<=|>=|==|!=|(?<![-<>])<(?![<=])|(?<![-<>])>(?![>=])')
STOP_L = ('&&', '||', '?', ':', ',', ';', '(', '=', 'return', '!')
STOP_R = ('&&', '||', '?', ':', ',', ';', ')')

def operand_left(s, i):
    """start index of the operand ending at s[i] (exclusive end)"""
    j, dep = i, 0
    while j > 0:
        c = s[j - 1]
        if c == ')': dep += 1
        elif c == '(':
            if dep == 0: break
            dep -= 1
        elif dep == 0:
            if s[:j].endswith(('&&', '||')) or c in '?:,;' or (c == '=' and s[j-2:j-1] not in '<>=!') or (c == '!' and s[j:j+1] != '='):
                break
            if s[:j].endswith('return') and (j == 6 or not s[j-7].isalnum()): break
        j -= 1
    return j

def operand_right(s, i):
    j, dep = i, 0
    while j < len(s):
        c = s[j]
        if c == '(': dep += 1
        elif c == ')':
            if dep == 0: break
            dep -= 1
        elif dep == 0 and (s[j:j+2] in ('&&', '||') or c in '?:,;'):
            break
        j += 1
    return j

def variants(line):
    if line.lstrip().startswith(('#', '//', '/*', '*')) or 'assert' in line:
        return []
    mask = re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*\'''', lambda m: 'x' * len(m.group()), line)  # hide literals
    vs = set()
    for m in OPS.finditer(mask):
        op = m.group()
        a0, b1 = operand_left(mask, m.start()), operand_right(mask, m.end())
        a, b = line[a0:m.start()].strip(), line[m.end():b1].strip()
        am, bm = mask[a0:m.start()], mask[m.end():b1]
        if not a or not b or '<' in am + bm or '>' in am.replace('->', '') + bm.replace('->', ''): continue
        lead = line[a0:m.start()][:len(line[a0:m.start()]) - len(line[a0:m.start()].lstrip())]
        trail = line[m.end():b1][len(line[m.end():b1].rstrip()):]
        pre, post = line[:a0] + lead, trail + line[b1:]
        vs.add('%s%s %s %s%s' % (pre, b, FLIP[op], a, post))
        if re.fullmatch(r'-?\d+', b):
            k = int(b)
            nb = {'>': ('>=', k + 1), '>=': ('>', k - 1), '<': ('<=', k - 1), '<=': ('<', k + 1)}.get(op)
            if nb: vs.add('%s%s %s %d%s' % (pre, a, nb[0], nb[1], post))
    lit = re.compile(r'(?<![\w.])(\d+\.\d*(?:e-?\d+)?)(f?)(?![\w.])')
    if lit.search(line):
        vs.add(lit.sub(lambda m: m.group(1) + ('' if m.group(2) else 'f'), line))
    vs.discard(line)
    return sorted(vs)

NEG = {'==': '!=', '!=': '==', '<': '>=', '>=': '<', '>': '<=', '<=': '>'}

def negate(c):
    m = [x for x in OPS.finditer(c)]
    if len(m) == 1 and '&&' not in c and '||' not in c and '?' not in c:
        x = m[0]
        return c[:x.start()] + NEG[x.group()] + c[x.end():]
    if c.startswith('!') and not c.startswith('!=') and operand_right(c, 1) == len(c) and re.fullmatch(r'!\s*[\w.>\[\]-]+|!\s*\(.*\)', c):
        return c[1:].strip()
    if re.fullmatch(r'[\w.\[\]]+(?:->[\w.\[\]]+)*', c):
        return '!' + c
    return '!(' + c + ')'

def ternaries(line):
    """`c ? a : b` -> `!c ? b : a` (arm order decides fallthrough)"""
    if line.lstrip().startswith(('#', '//', '/*', '*')) or 'assert' in line:
        return []
    mask = re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*\'''', lambda m: 'x' * len(m.group()), line)
    vs = set()
    for q in (i for i, ch in enumerate(mask) if ch == '?'):
        j, dep = q, 0  # condition start: like operand_left but spans && / ||
        while j > 0:
            c = mask[j - 1]
            if c == ')': dep += 1
            elif c == '(':
                if dep == 0: break
                dep -= 1
            elif dep == 0 and (c in '?:,;{' or (c == '=' and mask[j-2:j-1] not in '<>=!' and mask[j:j+1] != '=')):
                break
            elif dep == 0 and mask[:j].endswith('return') and (j == 6 or not mask[j-7].isalnum()):
                break
            j -= 1
        k, dep = q + 1, 0  # matching ':'
        while k < len(mask) and not (dep == 0 and mask[k] in ':?'):
            dep += mask[k] in '(['; dep -= mask[k] in ')]'
            if dep < 0: break
            k += 1
        if k >= len(mask) or mask[k] != ':': continue
        e = operand_right(mask, k + 1)
        if mask[e:e + 1] in ('?', ':'): continue  # nested ternary
        cond, a, b = line[j:q].strip(), line[q + 1:k].strip(), line[k + 1:e].strip()
        if not cond or not a or not b: continue
        lead = line[j:q][:len(line[j:q]) - len(line[j:q].lstrip())]
        trail = line[k + 1:e][len(line[k + 1:e].rstrip()):]
        vs.add('%s%s%s ? %s : %s%s%s' % (line[:j], lead, negate(cond), b, a, trail, line[e:]))
    vs.discard(line)
    return sorted(vs)

if __name__ == '__main__':
    for t, want in [('\tb = sourceBuf ? &buf[l] : NULL;', '\tb = !sourceBuf ? NULL : &buf[l];'),
                    ('\t\tline[i] = c != \'\\t\' ? c : \' \';', '\t\tline[i] = c == \'\\t\' ? \' \' : c;'),
                    ('\treturn yy_is_jam ? 0 : yy_current_state;', '\treturn !yy_is_jam ? yy_current_state : 0;'),
                    ('\tf(a ? X : Y, 0);', '\tf(!a ? Y : X, 0);'),
                    ('\tx = a && b->c ? 1 : 2;', '\tx = !(a && b->c) ? 2 : 1;'),
                    ('\tx = !scr.dev ? 1 : 2;', '\tx = scr.dev ? 2 : 1;')]:
        v = ternaries(t)
        assert want in v, (t, v)
    for t, want in [('if (*cursorX < 0)', 'if (0 > *cursorX)'),
                    ('if ( Q_stricmp( cmd, "mr" ) == 0 )', 'if ( 0 == Q_stricmp( cmd, "mr" ) )'),
                    ('if ( ( sourcePosLookup->type & type ) != type )', 'if ( type != ( sourcePosLookup->type & type ) )'),
                    ('for ( i = 0; i < numBones; ++i )', 'for ( i = 0; numBones > i; ++i )'),
                    ('if (!st->lbr_48k) {', None), ('stack=st->stack;', None),
                    ('return a->b < c;', 'return c > a->b;'), ('x = y >= 3 ? 1 : 2;', 'x = 3 <= y ? 1 : 2;')]:
        v = variants(t)
        assert (want in v) if want else not v, (t, v)
        for x in v: assert '-;' not in x and x.count('(') == t.count('('), (t, x)
    print('ok')
