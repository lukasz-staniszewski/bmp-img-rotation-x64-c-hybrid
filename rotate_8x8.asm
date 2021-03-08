; =================================================================
; ARKO x86-64b
; rotate_8x8 function
; function rotates a 8x8 square of pixels and returns new-position pointer of ouput pixels
; =================================================================

section	.text
global  rotate_8x8

rotate_8x8:
prolog:
	push	rbp
	mov	    rbp,    rsp
read_square_to_register:
    ; W RSI PTR_OUTPUT
    ; W RDI ADRES STARTOWY KWADRATU - PTR_INPUT
    ; W RDX LINE_BYTES_INPUT
    mov     r8,     rcx         ; CL sie przyda wiec w R8 LINE_BYTES_OUTPUT
    mov     cl,     8           ; CL PETLA DLA 8 BAJTOW
r_bytes:
    shl     rax,    8
    mov     al,     [rdi]
    add     rdi,    rdx
    dec     cl
    jnz     r_bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; WSZYSTKIE REJESTRY WOLNE POZA R9 (8 bajtow obrazka), R8 (line_bytes_output) i RSI (ptr_output);;;;;;;;;;;;;;;
body:
    mov    cl,     1            ; W cl iterator po kolumnach (od 1 do 8)

;10101010|11111111|00000000|10101010|10101010|11111111|00000000|10101010
column_loop:
    mov     rdi,    rax        ; W RDI KOPIA 8 BAJTOW 
    rol     rax,    cl
    mov     dl,    8        
w_bytes:
    and     al,     0x1			; aktualnie w bl 00000000 lub 00000001
    shl     ch,     1
    or      ch,     al			; w CH tworzony bajt
    rol     rax,    8			; bierzemy kolejny bajt
    dec     dl
    jnz     w_bytes
    mov     rax,    rdi         ; przywrocenie EDX
finish_row_loop:
    mov     byte [rsi], ch
    sub     rsi,    r8
    inc     cl
    cmp     cl,     9
    jne     column_loop
finish_rot:
    mov     rax,    rsi        ; zwracamy ptr_output
epilog:
    pop     rbp
    ret     
;============================================
; CONVENTIONS
;============================================
; kolejnosc parametrow calkowitych - rejestry: rdi, rsi, rdx, rcx, r8, r9
; kolejne parametry: na stosie od PRAWEJ do LEWEJ
; pierwszy parametr pod adresem [esp + 8]
; rejestr stosu (rsp) musi c wyrownany do 16 bajtow (aprzed call
; (adres powrotu natychmiast psuje to wyrownanie - push rej  lub  sub rsp, 8
; trzeba zachowac: rbp, rbx, r12, r13, r14, r15
; wartosc funkcji: rax (lub rdx:rax)

; Rejestry 64-bitowe
; R0  R1  R2  R3  R4  R5  R6  R7  R8  R9  R10  R11  R12  R13  R14  R15
; RAX RCX RDX RBX RSP RBP RSI RDI

; Rejestry 32-bitowe (mlodsze bity rejestrow wyzej)
; R0D R1D R2D R3D R4D R5D R6D R7D R8D R9D R10D R11D R12D R13D R14D R15D
; EAX ECX EDX EBX ESP EBP ESI EDI

; Rejestry 16-bitowe (mlodsze bity rejestrow wyzej)
; R0W R1W R2W R3W R4W R5W R6W R7W R8W R9W R10W R11W R12W R13W R14W R15W
; AX  CX  DX  BX  SP  BP  SI  DI

; Rejestry 8-bitowe (mlodsze bity rejestrow wyzej)
; R0B R1B R2B R3B R4B R5B R6B R7B R8B R9B R10B R11B R12B R13B R14B R15B
; AL  CL  DL  BL  SPL BPL SIL DIL

; Rejestry 8-bitowe (brzemie historii)
; AH  CH  DH  BH
;============================================


;============================================
; REGISTERS
;============================================
;
; larger addresses
;
;  | long line_bytes_output        | RCX        
;  ---------------------------------
;  | long line_bytes_input         | RDX        
;  ---------------------------------
;  | unsigned char* ptr_output     | RSI        
;  ---------------------------------
;  | unsigned char* adress_start   | RDI         
;  ---------------------------------
;  | return address                | EBP+4
;  ---------------------------------
;  | saved ebp                     | EBP, ESP
;  ---------------------------------
;  | ... here local variables      | EBP-x
;  |     when needed               |
;
; \/                              \/
; \/ the stack grows in this      \/
; \/ direction                    \/
;
; lower addresses
;
;
;============================================