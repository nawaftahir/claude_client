.intel_syntax noprefix
# Snapshot cave (built at 0x10865d50, see patch_rb.py). F hook 0x107724f6 (compile start):
# when the fn name == env RA_FN, write every committed private RW region to build\ra1.bin;
# at the next compile start write build\ra2.bin. Region record = base, size, bytes.
# stderr gets "S <n> <sym ptr>" per snapshot.
.globl _start
.equ FWRITE, 0x10866108
.equ FOPEN, 0x108660e0
.equ FCLOSE, 0x10866040
.equ IOB, 0x1086610c
.equ GETENV, 0x108660e8
.equ LOADLIB, 0x10866144
.equ GETPROC, 0x10866150
.equ CUR, 0x108b1f00
.equ ENVP, 0x108b1f04
.equ ENVDONE, 0x108b1f08
.equ VQ, 0x108b1f0c
.equ FH, 0x108b1f10
.equ HDR, 0x108b1f14
.equ MBI, 0x108b1f20
_start:
caveF:
  pushad
  pushfd
  cmp dword ptr ds:ENVDONE,0
  jne 1f
  mov dword ptr ds:ENVDONE,1
  push offset strEnv
  call dword ptr ds:GETENV
  add esp,4
  mov ds:ENVP,eax
  push offset strK32
  call dword ptr ds:LOADLIB
  push offset strVQ
  push eax
  call dword ptr ds:GETPROC
  mov ds:VQ,eax
1:
  cmp dword ptr ds:CUR,1
  jne 2f
  mov dword ptr ds:CUR,2
  push offset strF2
  call dump
  add esp,4
2:
  mov edi,ds:ENVP
  test edi,edi
  je 9f
  cmp dword ptr ds:CUR,0
  jne 9f
  mov edx,[ebx]
  mov esi,[edx+4]
  test esi,esi
  je 9f
3:
  mov al,[esi]
  cmp al,[edi]
  jne 9f
  inc esi
  inc edi
  test al,al
  jne 3b
  mov dword ptr ds:CUR,1
  push offset strF1
  call dump
  add esp,4
9:
  popfd
  popad
  mov edx,[ebx]
  movzx eax, word ptr [edx+0x6d]
  push 0x107724fc
  ret
# N hook 0x10784d3b (color node free): first time after arming, write build\ra3.bin
caveN:
  pushad
  pushfd
  cmp dword ptr ds:CUR,1
  jne 1f
  cmp dword ptr ds:CUR+0x60,0
  jne 1f
  mov dword ptr ds:CUR+0x60,1
  push offset strF3
  call dump
  add esp,4
1:
  popfd
  popad
  push esi
  mov esi,[ebx+0x20]
  test esi,esi
  push 0x10784d41
  ret
# dump(path): ebx = fn record (unchanged)
dump:
  push offset strWB
  push dword ptr [esp+8]
  call dword ptr ds:FOPEN
  add esp,8
  mov ds:FH,eax
  mov eax,[ebx]
  mov ds:HDR,eax
  mov dword ptr ds:HDR+4,0
  call whdr
  xor esi,esi
4:
  push 28
  push offset MBI
  push esi
  call dword ptr ds:VQ
  test eax,eax
  je 7f
  mov esi,ds:MBI
  add esi,ds:MBI+12
  cmp dword ptr ds:MBI+16,0x1000
  jne 6f
  cmp dword ptr ds:MBI+24,0x20000
  jne 6f
  cmp dword ptr ds:MBI+20,4
  jne 6f
  mov eax,ds:MBI
  mov ds:HDR,eax
  mov eax,ds:MBI+12
  mov ds:HDR+4,eax
  call whdr
  push dword ptr ds:FH
  push dword ptr ds:MBI+12
  push 1
  push dword ptr ds:MBI
  call dword ptr ds:FWRITE
  add esp,16
6:
  cmp esi,0x7fff0000
  jb 4b
7:
  push dword ptr ds:FH
  call dword ptr ds:FCLOSE
  add esp,4
  ret
whdr:
  push dword ptr ds:FH
  push 8
  push 1
  push offset HDR
  call dword ptr ds:FWRITE
  add esp,16
  ret
strEnv: .ascii "RA_FN\0"
strK32: .ascii "kernel32.dll\0"
strVQ: .ascii "VirtualQuery\0"
strWB: .ascii "wb\0"
strF1: .ascii "build\\ra1.bin\0"
strF2: .ascii "build\\ra2.bin\0"
strF3: .ascii "build\\ra3.bin\0"
