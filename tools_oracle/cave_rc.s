.intel_syntax noprefix
# Regalloc probe cave (built at 0x10865d50, see patch_ra.py).
# F hook 0x107724f6: per-fn compile start; arms logging when the fn name == env RA_FN.
# W hooks: the 5 `and [sym+0x20],~0x800` sites; log site index + sym name.
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
caveW0:
  and dword ptr [edi+0x20],0xfffff7ff
  pushad
  pushfd
  push edi
  push 0
  call wsite
  add esp,8
  popfd
  popad
  push 0x107ef8fe
  ret
caveW1:
  and dword ptr [esi+0x20],0xfffff7ff
  pushad
  pushfd
  push esi
  push 1
  call wsite
  add esp,8
  popfd
  popad
  push 0x108166c8
  ret
caveW2:
  and dword ptr [esi+0x20],0xfffff7ff
  pushad
  pushfd
  push esi
  push 2
  call wsite
  add esp,8
  popfd
  popad
  push 0x1081673c
  ret
caveW3:
  and dword ptr [eax+0x20],0xfffff7ff
  pushad
  pushfd
  push eax
  push 3
  call wsite
  add esp,8
  popfd
  popad
  push 0x1083d744
  ret
caveW4:
  and dword ptr [eax+0x20],0xfffff7ff
  pushad
  pushfd
  push eax
  push 4
  call wsite
  add esp,8
  popfd
  popad
  push 0x1083dc8a
  ret
# wsite(k, sym): "W<k> <name> <sym+0x20>"
wsite:
  cmp dword ptr ds:CUR,0
  je 1f
  push offset strWk
  call wstr
  add esp,4
  mov eax,[esp+4]
  call hexw
  push offset strSp
  call wstr
  add esp,4
  mov eax,[esp+8]
  push dword ptr [eax+4]
  call wstr
  add esp,4
  push offset strNL
  call wstr
  add esp,4
1:
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
strWk: .ascii "W\0"
strSp: .ascii " \0"
strBar: .ascii " |\0"
strNL: .ascii "\n\0"
hexd: .ascii "0123456789abcdef"
