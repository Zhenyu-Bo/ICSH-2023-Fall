;
;本题的关键在于使用栈来构造递归子程序REMOVE和PUT
;
            .ORIG   x3000
            LD      R5, BOTTOM          ;R5为frame pointer，初始化为x4000
            LD      R6, BOTTOM          ;R6为stack pointer，初始化为x4000
            LD      R2, START           ;R2为存储state的地址，初始化为x3100
            LDR     R0, R2, #0          ;R0存储n的值
            AND     R3, R3, #0          ;R3用于存储state

            ADD     R6, R6, #-1         ;
            STR     R3, R6, #0          ;state = 0入栈
            ADD     R6, R6, #-1         ;
            STR     R0, R6, #0          ;n入栈
            JSR     REMOVE              ;调用REMOVE子程序
            BRnzp   OVER                ;调用完毕后程序结束，不需要处理返回值
            
            
REMOVE      ADD     R6, R6, #-2         ;为返回值预留空间
            STR     R7, R6, #0          ;保存返回地址
            ADD     R6, R6, #-1         ;
            STR     R5, R6, #0          ;保存caller's frame pointer
            ADD     R5, R6, #-1         ;
            ADD     R6, R6, #-2         ;为局部变量state和n分配空间
            
            LDR     R3, R5, #5          ;R3存储state
            LDR     R0, R5, #4          ;R0存储n
            STR     R3, R5, #0          ;存储局部变量state
            STR     R0, R6, #0          ;存储局部变量n
            BRz     RETURN1             
            ADD     R1, R0, #-1         
            BRz     STORE1              ;n=1时单独处理
            JSR     JMP1                ;做子程序调用前的准备
            JSR     REMOVE              ;递归调用，REMOVE(n-2,state)
            JSR     JMP2                ;做子程序调用后的处理
            AND     R4, R4, #0
            ADD     R4, R4, #1          ;R4 <- 1
            ADD     R1, R0, #-1
JUDGE1      BRnp    LOOP1               ;令R4的第n位为1，后面全为0
            ADD     R3, R3, R4          ;令R3的第n位为1
            ADD     R2, R2, #1          ;存储地址加1
            STR     R3, R2, #0          ;存储state
            JSR     JMP1
            JSR     PUT                 ;PUT(n-2,state)
            JSR     JMP2
            JSR     JMP1
            ADD     R1, R1, #1          ;JMP1中令R1 <- R0-2, 这里需要令R1 <- R0-1, 故还需加1
            STR     R1, R6, #0
            JSR     REMOVE              ;REMOVE(n-1,state)
            JSR     JMP2                ;JMP1中令R1 <- R0-2, 这里需要令R1 <- R0-1, 故还需加1
            BRnzp   RETURN1
            
            
            
            
PUT         ADD     R6, R6, #-2         ;为返回值预留空间
            STR     R7, R6, #0          ;保存返回地址
            ADD     R6, R6, #-1         ;
            STR     R5, R6, #0          ;保存caller's frame pointer
            ADD     R5, R6, #-1         ;
            ADD     R6, R6, #-2         ;为局部变量state和n分配空间
            
            LDR     R3, R5, #5          ;R3存储state
            LDR     R0, R5, #4          ;R0存储n
            STR     R3, R5, #0          ;存储局部变量state
            STR     R0, R6, #0          ;存储局部变量n
            BRz     RETURN1             
            ADD     R1, R0, #-1         ;n=1时单独处理，将最后一位由1变成0
            BRz     STORE2
            JSR     JMP1                ;做子程序调用前的准备
            JSR     PUT                 ;PUT(n-2,state)
            JSR     JMP2
            AND     R4, R4, #0
            ADD     R4, R4, #1
            ADD     R1, R0, #-1
JUDGE2      BRnp    LOOP2               ;R4的第n位为1，后面全为0
            NOT     R4, R4
            ADD     R4, R4, #1          ;将R4取反加1
            ADD     R3, R3, R4          ;将R3的第n位由1变成0
            ADD     R2, R2, #1
            STR     R3, R2, #0
            JSR     JMP1
            JSR     REMOVE              ;REMOVE(n-2,state)
            JSR     JMP2
            JSR     JMP1
            ADD     R1, R1, #1          ;JMP1中令R1 <- R0-2, 这里需要令R1 <- R0-1, 故还需加1
            STR     R1, R6, #0
            JSR     PUT                 ;PUT(n-1,state)
            JSR     JMP2
            BRnzp   RETURN1 
            
            
            
LOOP1       ADD     R4, R4, R4          ;R4左移一位
            ADD     R1, R1, #-1         ;R1自减1
            BRnzp   JUDGE1
            
LOOP2       ADD     R4, R4, R4          ;R4左移一位
            ADD     R1, R1, #-1         ;R1自减1
            BRnzp   JUDGE2
            
            
            
            
STORE1      ADD     R3, R3, #1          ;最后一位由0变为1
            ADD     R2, R2, #1          ;存储地址自加1
            STR     R3, R2, #0          ;
            BRnzp   RETURN1             ;
            
STORE2      ADD     R3, R3, #-1         ;最后一位由1变为0
            ADD     R2, R2, #1          ;存储地址自加1
            STR     R3, R2, #0          ;
            BRnzp   RETURN1             ;
            
            
            
            
RETURN1     STR     R3, R5, #3          ;存储state于R3中
            ADD     R6, R5, #1          ;两个局部变量出栈
            LDR     R5, R6, #0          ;获得caller's frame pointer
            ADD     R6, R6, #1          ;
            LDR     R7, R6, #0          ;获得返回地址存于R7中
            ADD     R6, R6, #1          ;返回地址出栈
            RET
            
            
JMP1        ADD     R6, R6, #-1         ;
            STR     R3, R6, #0          ;存储state
            ADD     R1, R0, #-2         ;R1 = n-2
            ADD     R6, R6, #-1         ;
            STR     R1, R6, #0          ;存储n-2
            RET
            
JMP2        LDR     R3, R6, #0          ;获得返回值
            STR     R3, R5, #0          ;存储返回值
            LDR     R0, R6, #3
            ADD     R6, R6, #3          ;子程序的返回值和局部变量出栈
            RET
            
            
            
START       .FILL   x3100               ;存储n和state的起始地址，x3100处存储n，x3101及以后存储每次操作后的state
BOTTOM      .FILL   x4000               ;栈底地址

OVER        HALT
            .END