.intel_syntax noprefix
# Regalloc probe cave (built at 0x10865d50, see patch_ra.py).
# F hook 0x107724f6: per-fn compile start; arms logging when the fn name == env RA_FN.
# N hook 0x10784d3b: color.c node free (ebx = 0x48-byte node); dumps node + its [0] object.
.globl _start
.equ FWRITE, 0x10866108
.equ IOB, 0x1086610c
.equ GETENV, 0x108660e8
.equ CUR, 0x108b1f00
.equ ENVP, 0x108b1f04
.equ ENVDONE, 0x108b1f08
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
2:
  mov al,[esi]
  cmp al,[edi]
  jne 9f
  inc esi
  inc edi
  test al,al
  jne 2b
  mov dword ptr ds:CUR,1
  mov edx,[ebx]
  push offset strF
  call wstr
  add esp,4
  mov edx,[ebx]
  push dword ptr [edx+4]
  call wstr
  add esp,4
  push offset strNL
  call wstr
  add esp,4
9:
  popfd
  popad
  mov edx,[ebx]
  movzx eax, word ptr [edx+0x6d]
  push 0x107724fc
  ret
caveN:
  pushad
  pushfd
  cmp dword ptr ds:CUR,0
  je 9f
  push offset strN
  call wstr
  add esp,4
  xor esi,esi
1:
  mov eax,[ebx+esi*4]
  call hexw
  inc esi
  cmp esi,18
  jne 1b
  mov edi,[ebx]
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
  cmp esi,24
  jne 2b
  mov edi,[edi+0x28]
  test edi,edi
  je 3f
  push offset strBar
  call wstr
  add esp,4
  xor esi,esi
6:
  mov eax,[edi+esi*4]
  call hexw
  inc esi
  cmp esi,16
  jne 6b
3:
  push offset strNL
  call wstr
  add esp,4
9:
  popfd
  popad
  push esi
  mov esi,[ebx+0x20]
  test esi,esi
  push 0x10784d41
  ret
# P hook 0x10782a5e: allocator pops node ebp from the worklist (sorted by +0xc desc, +0x44 asc)
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
  mov edi,[edi]
  test edi,edi
  je 3f
  mov edi,[edi+4]
  test edi,edi
  je 3f
  push offset strSp2
  call wstr
  add esp,4
  push edi
  call wstr
  add esp,4
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
strN: .ascii "N\0"
strP: .ascii "P\0"
strSp2: .ascii " = \0"
strBar: .ascii " |\0"
strNL: .ascii "\n\0"
hexd: .ascii "0123456789abcdef"
