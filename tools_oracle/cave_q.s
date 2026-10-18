.intel_syntax noprefix
.globl _start
.equ FWRITE, 0x10866108
.equ IOB, 0x1086610c
.equ REVT, 0x107284d5
_start:
# cave1: compile start -> "F <fn>"
cave1:
  pushad
  pushfd
  mov edx,[ebx]
  push offset strF
  call logsym
  add esp,4
  popfd
  popad
  mov edx,[ebx]
  movzx eax, word ptr [edx+0x6d]
  push 0x107724fc
  ret
# caveQ: revert-test call site 0x107285bb -> "Q <callee>" + "r <res>"
caveQ:
  mov eax,[esp+0x14]
  push eax
  push esi
  mov eax,edi
  call REVT
  pushad
  pushfd
  mov edx,ebx
  push offset strQ
  call logsym
  add esp,4
  push offset strr
  call wstr
  add esp,4
  mov eax,[esp+0x20]
  call hexw
  push offset strNL
  call wstr
  add esp,4
  popfd
  popad
  push 0x107285c8
  ret
# caveA: 0x10728582 count > thr -> "q 1 count thr"
caveA:
  cmp [esp+0x10],esi
  jg 1f
  push 0x1072858c
  ret
1:
  pushad
  pushfd
  push offset strq1
  call wstr
  add esp,4
  mov eax,[esp+0x34]
  call hexw
  mov eax,[esp+8]
  call hexw
  push offset strNL
  call wstr
  add esp,4
  popfd
  popad
  push 0x10728731
  ret
# caveB: 0x10728596 end of list -> "q 0 count thr"
caveB:
  je 2f
  push 0x1072859c
  ret
2:
  pushad
  pushfd
  push offset strq0
  call wstr
  add esp,4
  mov eax,[esp+0x34]
  call hexw
  mov eax,[esp+0x38]
  call hexw
  push offset strNL
  call wstr
  add esp,4
  popfd
  popad
  push 0x1072540e
  ret
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
logsym:
  push edx
  push dword ptr [esp+8]
  call wstr
  add esp,4
  pop edx
  test edx,edx
  je 6f
  mov edx,[edx+4]
  test edx,edx
  je 6f
  push edx
  call wstr
  add esp,4
6:
  push offset strNL
  call wstr
  add esp,4
  ret
strF: .ascii "F \0"
strQ: .ascii "Q \0"
strr: .ascii "r\0"
strq1: .ascii "q 1\0"
strq0: .ascii "q 0\0"
strNL: .ascii "\n\0"
hexd: .ascii "0123456789abcdef"
