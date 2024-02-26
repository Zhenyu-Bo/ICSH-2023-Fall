            .ORIG x800
            ; (1) Initialize interrupt vector table.
            LD  R0, VEC
            LD  R1, ISR
            STR R1, R0, #0

            ; (2) Set bit 14 of KBSR.
            LDI R0, KBSR
            LD  R1, MASK
            NOT R1, R1
            AND R0, R0, R1
            NOT R1, R1
            ADD R0, R0, R1
            STI R0, KBSR

            ; (3) Set up system stack to enter user space.
            LD  R0, PSR
            ADD R6, R6, #-1
            STR R0, R6, #0
            LD  R0, PC
            ADD R6, R6, #-1
            STR R0, R6, #0
            ; Enter user space.
            RTI
        
VEC         .FILL x0180
ISR         .FILL x1000
KBSR        .FILL xFE00
MASK        .FILL x4000
PSR         .FILL x8002
PC          .FILL x3000
            .END
            
            
            
            
            
            
            .ORIG x3000

            ;循环打印学号          
LOOP        LEA     R0, ID                                  ;获得字符串首地址
            PUTS
            LD      R1, COUNT
REP         ADD     R1, R1, #-1
            BRp     REP
            BRnzp   LOOP
            
            
COUNT       .FILL       #25000
ID          .STRINGZ    "PB22081571 "                   ;student id(后面需要有空格来分隔打印的学号)
            .END
            
            
            ;中断程序
            .ORIG   x1000
            LD      R0, NEWLINE
            OUT                                         ;打印换行符
            GETC                                        ;读取输入的字符到R0中
            OUT                                         ;打印读取到的字符
            LD      R3, ASCIININEN
            ADD     R2, R0, R3                          ;判断输入字符的ASCII码是否小于'9'
            BRp     ERROR
            ADD     R2, R2, #9                          ;判断输入的字符是否大于'0'
            BRn     ERROR
            LEA     R0, RIGHTN
            PUTS
            LD      R3, SAVEn
            STR     R2, R3, #0                          ;存储读取到的n于x3FFF中
            LD      R3, ASCIIZERO
            ADD     R0, R2, R3                          ;恢复R0
            OUT                                         ;打印R0
            ADD     R0, R2, #-8                         ;判断输入的数字是否大于等于8
            BRzp    LARGE
            ;LEA     R0, EQUAL                           ;打印等式
            ;PUTS
            BRnzp   FACTIONAL                           ;计算n!
            ;JSR     FACTIONAL
            ;OUT                                        ;打印存储在R0中的运算结果
            ;LEA     NEWLINE
            ;OUT
            ;BRnzp   RETURN1
            
LARGE       LEA     R0, LARGEN
            PUTS
            BRnzp   OVER            

ERROR       LEA     R0, ERRORN
            PUTS
            
RETURNI     RTI
                
            
;计算n!   
FACTIONAL   ADD     R2, R2, #0
            BRp     ONE!
            LEA     R0, ZERO
            BR      OUTPUT
ONE!        ADD     R2, R2, #-1
            BRp     TWO!
            LEA     R0, ONE
            BR      OUTPUT
TWO!        ADD     R2, R2, #-1
            BRp     THREE!
            LEA     R0, TWO
            BR      OUTPUT  
THREE!      ADD     R2, R2, #-1
            BRp     FOUR!
            LEA     R0, THREE
            BR      OUTPUT   
FOUR!       ADD     R2, R2, #-1
            BRp     FIVE!
            LEA     R0, FOUR
            BR      OUTPUT   
FIVE!       ADD     R2, R2, #-1
            BRp     SIX!
            LEA     R0, FIVE
            BR      OUTPUT 
SIX!        ADD     R2, R2, #-1
            BRp     SEVEN!
            LEA     R0, SIX
            BR      OUTPUT  
SEVEN!      LEA     R0, SEVEN
            
            
OUTPUT      PUTS              
            
OVER        HALT

            
NEWLINE     .FILL       x000A
ERRORN      .STRINGZ    " is not a decimal digit.\n"    ;输入的字符不是十进制数
RIGHTN      .STRINGZ    " is a decimal digit.\n"        ;输入的字符是十进制数
LARGEN      .STRINGZ    "! is too large for LC-3.\n"    ;
EQUAL       .STRINGZ    "! = "                          ;打印等式的中间部分
THOUSANDN   .FILL       #-1000
HUNDREDN    .FILL       #-100
TENN        .FILL       #-10
ASCIININEN  .FILL       #-57                            ;9的ASCII码的负值
ASCIIZERO   .FILL       #48                             ;0的ASCII码
SAVEn       .FILL       x3FFF
RES         .FILL       x1100
ZERO        .STRINGZ    "! = 1\n"
ONE         .STRINGZ    "! = 1\n"
TWO         .STRINGZ    "! = 2\n"
THREE       .STRINGZ    "! = 6\n"
FOUR        .STRINGZ    "! = 24\n"
FIVE        .STRINGZ    "! = 120\n"
SIX         .STRINGZ    "! = 720\n"
SEVEN       .STRINGZ    "! = 5040\n"

            .END
