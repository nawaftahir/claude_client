.intel_syntax noprefix
# Colour-choice probe cave (built at 0x10865d50, patched by patch_ra.py). Hooks:
# F 0x107724f6: per-fn compile start; arms logging when the fn name == env RA_FN.
# P 0x10782a5e: allocator pops node ebp from the worklist (sorted by +0xc desc, +0x44 asc).
# C 0x107834a8: end of the register choice in 0x107833f2; ebx = chosen reg id, [esp+0x20] = node,
#   [esp+0x24] = class, cost array 0x10880680[reg] (see REGALLOC.md round 5).
.globl _start
.equ FWRITE, 0x10866108
.equ IOB, 0x1086610c
.equ GETENV, 0x108660e8
.equ CUR, 0x108b1f00
.equ ENVP, 0x108b1f04
.equ ENVDONE, 0x108b1f08
.equ PREF, 0x10880680
_start:
caveF:
  pushad
  pushfd
  mov dword ptr ds:CUR,0
  cmp dword ptr ds:ENVDONE,0
  jne 1f
  mov dword ptr ds:ENVDONE,1
  push offset strEnv
  call dword ptr ds:GETENV
  add esp,4
  mov ds:ENVP,eax
1:
  mov edi,ds:ENVP
  test edi,edi
  je 9f
  mov edx,[ebx]
  mov esi,[edx+4]
  test esi,esi
  je 9f
  cmp byte ptr [edi],'*'
  je 3f
2:
  mov al,[esi]
  cmp al,[edi]
  jne 9f
  inc esi
  inc edi
  test al,al
  jne 2b
3:
  mov dword ptr ds:CUR,1
  push offset strF
  call wstr
  mov edx,[ebx]
  push dword ptr [edx+4]
  call wstr
  push offset strNL
  call wstr
  add esp,12
9:
  popfd
  popad
  mov edx,[ebx]
  movzx eax, word ptr [edx+0x6d]
  push 0x107724fc
  ret
caveP:
  pushad
  pushfd
  cmp dword ptr ds:CUR,0
  je 9f
  push offset strP
  call wstr
  add esp,4
  xor esi,esi
1:
  mov eax,[ebp+esi*4]
  call hexw
  inc esi
  cmp esi,18
  jne 1b
  mov edi,[ebp]
  test edi,edi
  je 3f
  push offset strBar
  call wstr
  add esp,4
  xor esi,esi
2:
  mov eax,[edi+esi*4]
  call hexw
  inc esi
  cmp esi,14
  jne 2b
  mov edi,ebp
  call wname
3:
  push offset strNL
  call wstr
  add esp,4
9:
  popfd
  popad
  mov eax,[ebp+0x40]
  cmp eax,edi
  push 0x10782a63
  ret
caveC:
  pushad
  pushfd
  cmp dword ptr ds:CUR,0
  je 9f
  mov edi,[esp+0x44]
  push offset strC
  call wstr
  add esp,4
  mov eax,[edi+0x1c]
  call hexw
  mov eax,ebx
  call hexw
  mov eax,[esp+0x48]
  call hexw
  xor eax,eax
  mov edx,[edi+0x20]
  test edx,edx
  je 4f
  mov edx,[edx]
  test edx,edx
  je 4f
  cmp dword ptr [edx],0
  jne 4f
  mov eax,[edx+8]
4:
  call hexw
  mov esi,1
1:
  mov eax,[PREF+esi*4]
  call hexw
  inc esi
  cmp esi,9
  jne 1b
  call wname
  push offset strNL
  call wstr
  add esp,4
9:
  popfd
  popad
  mov eax,[esp+0x20]
  mov edx,[eax+0x20]
  push 0x107834af
  ret
# E 0x107ba622: final emission of one code record (ebx = record, +0 kind, +4 next); logs ptr + 8 dwords
caveE:
  pushad
  pushfd
  push offset strA
  call wstr
  add esp,4
  mov eax,[edi+0x18]
  call hexw
  mov eax,ebx
  call hexw
  xor esi,esi
1:
  mov eax,[ebx+esi*4]
  call hexw
  inc esi
  cmp esi,8
  jne 1b
  push offset strNL
  call wstr
  add esp,4
9:
  popfd
  popad
  mov ecx,[esp+0x14]
  push ecx
  push 0x107ba627
  ret
# wname: edi = node; write " = name" when node->obj->sym->name exists
wname:
  mov edi,[edi]
  test edi,edi
  je 3f
  mov edi,[edi]
  test edi,edi
  je 3f
  mov edi,[edi+4]
  test edi,edi
  je 3f
  push offset strSp2
  call wstr
  push edi
  call wstr
  add esp,8
3:
  ret
# wstr(str): fwrite a 0-terminated string to stderr
wstr:
  mov edx,[esp+4]
  mov ecx,edx
4:
  cmp byte ptr [ecx],0
  je 3f
  inc ecx
  jmp 4b
3:
  sub ecx,edx
  mov eax,ds:IOB
  lea eax,[eax+0x40]
  push eax
  push ecx
  push 1
  push edx
  call dword ptr ds:FWRITE
  add esp,16
  ret
# hexw: write " %08x" of eax
hexw:
  sub esp,12
  mov byte ptr [esp],' '
  mov ecx,8
5:
  mov edx,eax
  and edx,15
  mov dl,[hexd+edx]
  mov [esp+ecx],dl
  shr eax,4
  dec ecx
  jne 5b
  mov byte ptr [esp+9],0
  push esp
  call wstr
  add esp,4
  add esp,12
  ret
strEnv: .ascii "RA_FN\0"
strF: .ascii "F \0"
strP: .ascii "P\0"
strC: .ascii "C\0"
strA: .ascii "A\0"
strS: .ascii "S\0"
strSp2: .ascii " = \0"
strBar: .ascii " |\0"
strNL: .ascii "\n\0"
hexd: .ascii "0123456789abcdef"
