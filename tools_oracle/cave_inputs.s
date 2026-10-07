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
  mov ebx,[esp+0x1c]
  mov ebx,[ebx+0xc]
9:
  test ebx,ebx
  je 8f
  mov edx,[ebx+4]
  mov edx,[edx+4]
  push offset strT
  call logsym
  add esp,4
  mov ebx,[ebx]
  jmp 9b
8:
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
# cave6: hook 0x1078d026 (call inline-decision 0x1078cf54; test eax,eax) -> log
# "I <callee>\n" + "V <result> <instrs> <p2 cost> <p3 budget> <p4 thresh>\n"
cave6:
  push dword ptr [esp+0xc]
  push dword ptr [esp+0xc]
  push dword ptr [esp+0xc]
  push dword ptr [esp+0xc]
  mov eax,esi
  mov ecx,0x1078cf54
  call ecx
  pushad
  pushfd
  call symof
  push edx
  push offset strI
  call logsym
  add esp,4
  pop edx
  push offset strV
  call wstr
  add esp,4
  mov eax,[esp+0x20]
  call hexw
  mov esi,[esp+8]
  call symof
  movzx eax,word ptr [edx+0x6d]
  call hexw
  mov eax,[esp+0x28]
  call hexw
  mov eax,[esp+0x2c]
  call hexw
  mov eax,[esp+0x30]
  call hexw
  mov esi,[esp+8]
  call symof
  push edx
  mov eax,[edx+0x81]
  call hexw
  mov edx,[esp]
  mov eax,[edx+0xc5]
  call hexw
  pop edx
  mov eax,[edx+0x20]
  call hexw
  mov eax,ds:0x10882b58
  call hexw
  push offset strNL
  call wstr
  add esp,4
  popfd
  popad
  add esp,16
  test eax,eax
  jne 6f
  push 0x1078d033
  ret
6:
  push 0x1078d36a
  ret
# symof: edx = callee sym of call site esi
symof:
  mov edx,[esi+4]
  mov edx,[edx+0x28]
  cmp byte ptr [edx+8],4
  jne 7f
  mov edx,[edx+0x18]
  ret
7:
  mov edx,[edx+0x18]
  mov edx,[edx+8]
  mov edx,[edx]
  ret
# wstr(str): fwrite a 0-terminated string to stderr-ish stream
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
strT: .ascii "T \0"
hexd: .ascii "0123456789abcdef"
strI: .ascii "I \0"
strV: .ascii "V\0"
