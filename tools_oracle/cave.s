.intel_syntax noprefix
.globl _start
.equ FWRITE, 0x10866108
.equ IOB, 0x1086610c
_start:
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
cave2:
  pushad
  pushfd
  mov edx,eax
  push offset strX
  call logsym
  add esp,4
  popfd
  popad
  mov ecx,[eax+0x37]
  test ch,0x20
  push 0x107707a0
  ret
cave3:
  pushad
  pushfd
  mov edx,[ecx+4]
  push offset strD
  call logsym
  add esp,4
  popfd
  popad
  push esi
  xor eax,eax
  push edi
  mov edi,ecx
  push 0x1070ce7e
  ret
cave4:
  pushad
  pushfd
  mov edx,[esi+4]
  mov edx,[edx+4]
  push offset strC
  call logsym
  add esp,4
  popfd
  popad
  or eax,1
  mov [esi+0xc],ax
  push 0x1070cea2
  ret
cave5:
  pushad
  pushfd
  mov edx,[edi+4]
  push offset strE
  call logsym
  add esp,4
  popfd
  popad
  xor eax,eax
  mov ax,[edi+0x50]
  push 0x1070cee3
  ret
# logsym(prefix) with edx = sym
logsym:
  push ebp
  mov ebp,esp
  push edx
  mov eax,ds:IOB
  lea esi,[eax+0x40]
  push esi
  push 2
  push 1
  push dword ptr [ebp+8]
  call dword ptr ds:FWRITE
  add esp,16
  pop edx
  test edx,edx
  je 3f
  mov edi,[edx+4]
  test edi,edi
  je 3f
  mov ecx,edi
1:
  cmp byte ptr [ecx],0
  je 2f
  inc ecx
  jmp 1b
2:
  sub ecx,edi
  push esi
  push ecx
  push 1
  push edi
  call dword ptr ds:FWRITE
  add esp,16
3:
  push esi
  push 1
  push 1
  push offset strNL
  call dword ptr ds:FWRITE
  add esp,16
  pop ebp
  ret
strF: .ascii "F \0"
strX: .ascii "X \0"
strNL: .ascii "\n\0"
strD: .ascii "D \0"
strC: .ascii "C \0"
strE: .ascii "E \0"
