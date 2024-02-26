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

            .ORIG   x800
            ; (1) Initialize interrupt vector table.
            LD      R0, VEC
            LD      R1, ISR
            STR     R1, R0, #0
    
            ; (2) Set bit 14 of KBSR.
            LDI     R0, KBSR
            LD      R1, MASK
            NOT     R1, R1
            AND     R0, R0, R1
            NOT     R1, R1
            ADD     R0, R0, R1
            STI     R0, KBSR
    
            ; (3) Set up system stack to enter user space.
            LD      R0, PSR
            ADD     R6, R6, #-1
            STR     R0, R6, #0
            LD      R0, PC
            ADD     R6, R6, #-1
            STR     R0, R6, #0
            ; Enter user space.
            RTI
        
VEC         .FILL   x0180
ISR         .FILL   x1000
KBSR        .FILL   xFE00
MASK        .FILL   x4000
PSR         .FILL   x8002
PC          .FILL   x3000
            .END



            .ORIG   x3000
            ; *** Begin user program code here ***
            ;循环打印学号          
LOOP        LEA     R0, ID                              ;获得字符串首地址
            PUTS                                        ;打印学号+空格
            LD      R1, COUNT                           ;获得COUNT，用于执行DELAY功能            
REP         ADD     R1, R1, #-1                         
            BRp     REP
            BRnzp   LOOP                                ;经过DDELAY后继续打印学号+空格
            
            
COUNT       .FILL       #25000                          ;用于设置延迟时间的长短
ID          .STRINGZ    "PB22081571 "                   ;student id(后面需要有空格来分隔打印的学号)

            ; *** End user program code here ***
            .END



            .ORIG   x3FFF
            ; *** Begin factorial data here ***
FACT_N      .FILL   xFFFF
            ; *** End factorial data here ***
            .END



            .ORIG   x1000
            ; *** Begin interrupt service routine code here ***
            
            LD      R0, NEWLINE
            OUT                                         ;打印换行符
            GETC                                        ;读取输入的字符到R0中
            OUT                                         ;打印读取到的字符
            LD      R3, ASCIININEN
            ADD     R2, R0, R3                          ;判断输入字符的ASCII码是否小于'9'
            BRp     ERROR                               ;若大于'9'，则不是十进制数
            ADD     R2, R2, #9                          ;判断输入的字符是否大于等于'0'
            BRn     ERROR                               ;若小于0，也不是十进制数
            LEA     R0, RIGHTN                          ;否则是十进制数，打印信息
            PUTS
            LD      R3, SAVEn                           ;获得n的存储地址x3FFF放在R3中
            STR     R2, R3, #0                          ;存储读取到的n于x3FFF中
            LD      R3, ASCIIZERO                       ;获得0的ASCII码，由于无法用立即数表示，故存在内存中
            ADD     R0, R2, R3                          ;恢复R0
            OUT                                         ;打印R0
            ADD     R0, R2, #-8                         ;判断输入的数字是否大于等于8
            BRzp    LARGE
            ;BR      FACTIONAL2                          ;如果使用将结果存在内存中的方式，此时可直接跳转
            LEA     R0, EQUAL                           ;打印等式
            PUTS
            BRnzp   FACTIONAL                           ;计算n!
            
            
LARGE       LEA     R0, LARGEN                          
            PUTS                                        ;打印信息
            BRnzp   OVER            

ERROR       LEA     R0, ERRORN
            PUTS                                        ;打印信息
            
RETURNI     RTI
            ; *** End interrupt service routine code here ***
                
                
;            
; 计算n! 
; 其实这里也可以直接将相应的结果存在内存中，再根据n的值取出
; 但是考虑到本次实验应该需要考察这方面的处理，所以这里采用计算的方式获得结果
;
FACTIONAL   AND     R1, R1, #0
            ADD     R1, R1, R2
            BRz     ZERO0                                ;0!单独处理
LOOP1       ADD     R2, R2, #-1                         ;计算R2!
            BRz     OUTPUT
            AND     R3, R3, #0
            ADD     R3, R3, R2                          ;R3存储R2的值
            AND     R4, R4, #0
            ADD     R4, R4, R1                          ;R4用于保存R1的值，因为在计算时R1会改变
LOOP2       ADD     R3, R3, #-1                         ;计算R4*R2,结果存于R1中
            BRz     LOOP1
            ADD     R1, R1, R4
            BRnzp   LOOP2
ZERO0       ADD     R1, R1, #1                          ;若R1 = 0，则直接将其加1再输出即可
            
            ;计算完成后打印结果，因为n! <= 7! = 5040，所以只需依次打印结果的千百个十位即可 
OUTPUT      AND     R4, R4, #0                          ;R4用于标记当前位前面有没有非零位，以判断要不要打印0
            LD      R2, THOUSANDN                       ;获得-1000
            JSR     TRANS                               ;计算字符的千位，转换成ASCII码并输出
            LD      R2, HUNDREDN                        ;获得-100
            JSR     TRANS                               ;计算字符的百位，转换成ASCII码并输出
            LD      R2, TENN                            ;获得-10
            JSR     TRANS                               ;计算字符的十位，转换成ASCII码并输出
;因为这时R1为个位，不管是否为0，是第几个0，都需要打印，将其加上0的ASCII码放到R0中打印即可
            LD      R3, ASCIIZERO
            ADD     R0, R1, R3
            OUT
            ;JSR     OUT1
            LD      R0, NEWLINE                         ;最后打印换行符
            OUT
            
OVER        HALT



;将数字转化为ASCII码并输出            
TRANS       AND     R0, R0, #0
LOOP3       ADD     R1, R1, R2                          ;R1 <- R1 + R2
            BRn     JUDGE1                              ;当R1为负时，此时R0中存储的即为所求位的数值
            ADD     R0, R0, #1
            BRnzp   LOOP3
JUDGE1      ADD     R0, R0, #0                          ;设置条件码与R0相关
            BRnz    JUDGE2                              ;如果R0等于0则需要判断要不要打印  
            ADD     R4, R4, #1
OUT1        LD      R3, ASCIIZERO                       ;获得0的ASCII码
            ADD     R0, R0, R3                          ;将R0转换成对应的ASCII码以输出
            OUT
            BRnzp   RESTORE                             ;因为前面将R1减到了负值，即多加了一个R2，所以需要再减去R2
            
JUDGE2      ADD     R4, R4, #0                          ;设置条件码
            BRp     OUT1                                ;如果R4不为0，则说明当前位前面有非零位，需要打印0
            
RESTORE     NOT     R2, R2
            ADD     R2, R2, #1                          ;将R2取反加1获得相反数
            ADD     R1, R1, R2                          ;前面将R1减到了负值，故还需要再加上-R2
            RET 


            
FACTIONAL2  ADD     R2, R2, #0
            BRp     ONE!
            LEA     R0, ZERO
            BR      OUTRES
ONE!        ADD     R2, R2, #-1
            BRp     TWO!
            LEA     R0, ONE
            BR      OUTRES
TWO!        ADD     R2, R2, #-1
            BRp     THREE!
            LEA     R0, TWO
            BR      OUTRES  
THREE!      ADD     R2, R2, #-1
            BRp     FOUR!
            LEA     R0, THREE
            BR      OUTRES   
FOUR!       ADD     R2, R2, #-1
            BRp     FIVE!
            LEA     R0, FOUR
            BR      OUTRES   
FIVE!       ADD     R2, R2, #-1
            BRp     SIX!
            LEA     R0, FIVE
            BR      OUTRES 
SIX!        ADD     R2, R2, #-1
            BRp     SEVEN!
            LEA     R0, SIX
            BR      OUTRES  
SEVEN!      LEA     R0, SEVEN
            
            
OUTRES      PUTS
            BRnzp   OVER
        
        
        
            
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
ZERO        .STRINGZ    "! = 1\n"
ONE         .STRINGZ    "! = 1\n"
TWO         .STRINGZ    "! = 2\n"
THREE       .STRINGZ    "! = 6\n"
FOUR        .STRINGZ    "! = 24\n"
FIVE        .STRINGZ    "! = 120\n"
SIX         .STRINGZ    "! = 720\n"
SEVEN       .STRINGZ    "! = 5040\n"

            .END
