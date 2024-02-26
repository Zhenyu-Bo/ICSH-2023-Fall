; Unfortunately we have not YET installed Windows or Linux on the LC-3,
; so we are going to have to write some operating system code to enable
; keyboard interrupts. The OS code does three things:
;
;    (1) Initializes the interrupt vector table with the starting
;        address of the interrupt service routine. The keyboard
;        interrupt vector is x80 and the interrupt vector table begins
;        at memory location x0100. The keyboard interrupt service routine
;        begins at x1000. Therefore, we must initialize memory location
;        x0180 with the value x1000.
;    (2) Sets bit 14 of the KBSR to enable interrupts.
;    (3) Pushes a PSR and PC to the system stack so that it can jump
;        to the user program at x3000 using an RTI instruction.

        .ORIG x800
        ; (1) Initialize interrupt vector table.
        LD R0, VEC
        LD R1, ISR
        STR R1, R0, #0

        ; (2) Set bit 14 of KBSR.
        LDI R0, KBSR
        LD R1, MASK
        NOT R1, R1
        AND R0, R0, R1
        NOT R1, R1
        ADD R0, R0, R1
        STI R0, KBSR

        ; (3) Set up system stack to enter user space.
        LD R0, PSR
        ADD R6, R6, #-1
        STR R0, R6, #0
        LD R0, PC
        ADD R6, R6, #-1
        STR R0, R6, #0
        ; Enter user space.
        RTI
        
VEC     .FILL x0180
ISR     .FILL x1000
KBSR    .FILL xFE00
MASK    .FILL x4000
PSR     .FILL x8002
PC      .FILL x3000
        .END

        .ORIG x3000
        
        ; *** Begin user program code here ***
LOOP	LEA R0, STUID		; 打印字符串1
        PUTS
	AND R0, R0, #0
	LD  R0, DELAY
SLEEP	ADD R0, R0, #-1
	BRp SLEEP
		
	BRnzp LOOP


DELAY	.FILL #25000
STUID	.STRINGZ "PB22081571 "
        ; *** End user program code here ***
        .END

        .ORIG x3FFF
        ; *** Begin honoi data here ***
HONOI_N .FILL xFFFF
        ; *** End honoi data here ***
        .END

        .ORIG x1000
        ; *** Begin interrupt service routine code here ***
        LD R6,STACK;压栈

        LD R0, NEWLINE
        OUT
        ;输入汉诺塔阶数

        GETC ;input n
        ;从键盘读入n
        OUT
        ADD R5, R0, #0
        LD  R3, ASCIIZ
        ADD R0, R0, R3    ;n-'0'
        ADD R4, R0, #0
        BRn ERROR
        LD  R3, ASCIIN     
        ADD R0, R5, R3    ;n-'9'
        BRp ERROR
        LEA R0, OUTPUT2
        PUTS
        LD  R0, NEWLINE
        OUT
        ADD R0, R4, #0
        BR MAIN
ERROR   LEA R0, OUTPUT1
        PUTS
FINISH  LD  R0, NEWLINE
        OUT
        RTI
       
MAIN    ADD R6,R6,#-1
        STR R0,R6,#0
        JSR HANOI
        ;output
        LEA R0,OUTPUT3
        PUTS
        LD  R0, OFF
        LD  R2, NEGH
LOOPH   ADD R1, R2, R1
        BRn ENDH
        ADD R0, R0, #1
        BRnzp LOOPH
ENDH    OUT     ; 百位
        LD  R3, POSH
        ADD R1, R1, R3
        LD  R0, OFF
        LD  R2, NEGS
LOOPS   ADD R1, R2, R1
        BRn ENDS
        ADD R0, R0, #1
        BRnzp LOOPS
ENDS    OUT     ; 十位
        ADD R1, R1, #10
        LD  R0, OFF
        ADD R0, R0, R1
        OUT     ; 个位
        LEA R0,OUTPUT4
        PUTS
        HALT

HANOI   ADD R6, R6, #-1
        STR R7, R6, #0 ; Push R7, the return linkage
        ADD R6, R6, #-1
        STR R0, R6, #0 ; Push R0, the value of n
        ADD R6, R6, #-1
        STR R2, R6, #0 ; Push R2, which is needed in the subroutine
        ; Check for base case
        AND R2, R0, #-2
        BRnp SKIP ; H(n)=n if n=0,1
        ADD R1, R0, #0 ; R0=n is the answer
        BRnzp DONE
        ; Not a base case, do the recursion
SKIP    ADD R0, R0, #-1  ; R0 = n-1
        JSR HANOI ; R1 = H(n-1)
        ADD R1, R1, R1; R1 = 2H(n-1)
        ADD R1, R1, #1 ; R1 = 2H(n-1) + 1
        ; Restore registers and return
DONE    LDR R2, R6, #0

        ADD R6, R6, #1
        LDR R0, R6, #0
        ADD R6, R6, #1
        LDR R7, R6, #0
        ADD R6, R6, #1
        RET

;
ASCIIZ      .FILL       xFFD0
ASCIIN      .FILL       xFFC7 
OFF         .FILL       x0030
STACK       .FILL       x6000
NEWLINE     .FILL       x000A
OUTPUT1     .STRINGZ    " is not a decimal digit. "
OUTPUT2     .STRINGZ    " is a decimal digit. "
OUTPUT3     .STRINGZ    "Tower of hanoi needs "
OUTPUT4     .STRINGZ    " moves."
NEGH        .FILL	xFF9C
POSH        .FILL	x0064
NEGS        .FILL   	xFFF6
; *** End interrupt service routine code here ***
        .END
      


