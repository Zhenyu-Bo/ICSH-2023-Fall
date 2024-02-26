;  
;本题的关键在于compare的实现，可以为此写一个子程序来依次比较两个字符串中的字符，用一个寄存器来记录比较结果
;0表示不同，1表示相同
;此外，读入字符串的功能也经常用到，也可以写一个子程序
;
;Initialzation
;
            .ORIG x3000
            
            AND     R0, R0, #0
            AND     R1, R1, #0
            AND     R2, R2, #0
            AND     R3, R3, #0
            AND     R4, R4, #0
            AND     R5, R5, #0
            AND     R6, R6, #0      ;R6用于记录是第一次输入密码还是1第二次，第二次输入密码时R6赋值为1
            AND     R7, R7, #0      ;R0-R7均初始化为0
;
;主体部分，读取字符串并比较
;
LOOP        LD      R1, S3_ADDR     ;准备读入字符串，将存储字符串的起始地址放入R1中
            JSR     INPUT           ;调用INPUT子程序读入字符串S3
            
            LD      R0, NEWLINE     ;打印换行符
            OUT
            
            LD      R1, S1_ADDR     ;R1中存储密码的存储位置的起始地址
            LD      R2, S3_ADDR     ;R2中存储读入字符串的存储位置的起始地址
            JSR     COMPARE         ;比较输入的字符串与密码是否相同
            ADD     R0, R0, #0      ;设置条件码与R0相关
            BRp     RIGHT           ;R0为1时表示输入的字符串与密码相同，调用子程序打印"righ"并结束程序
            LD      R0, S7_ADDR
            PUTS                    ;打印output:\n
            LD      R0, S5_ADDR     ;若输入的密码错误则打印"wron"
            PUTS                    ;打印wron
            ADD     R6, R6, #0      ;设置条件码与R6相关
            BRp     FINISH          ;若R6等于1则此时为第二次错误输入密码，结束程序
            ADD     R6, R6, #1      ;否则将R6设置为1表示已经输入过一次密码
            LD      R0, S2_ADDR     ;准备输出验证码
            PUTS                    ;输出验证码 
            LD      R0, NEWLINE     
            OUT                     ;打印换行符
            LD      R1, S3_ADDR     ;准备读入字符串
            JSR     INPUT           ;读入字符串
            LD      R0, NEWLINE     ;
            OUT                     ;读入完毕后再次打印换行符以将输入的字符串和后面输出的字符串区分开
            LD      R1, S2_ADDR     ;R1中存储验证码的存储位置的起始地址
            LD      R2, S3_ADDR     ;R2中存储读入字符串的存储位置的起始地址
            JSR     COMPARE         ;比较读入的字符串与验证码是否相等
            ADD     R0, R0, #0      ;设置与R0相关的条件码
            BRp     LOOP            ;若R0为1则说明输入的验证码正确，可以再次输入密码
            LD      R0, S7_ADDR
            PUTS                    ;打印output:\n
            LD      R0, S5_ADDR     ;否则打印"wron"并结束程序
            PUTS
            BRnzp   FINISH
            
            
RIGHT       LD      R0, S7_ADDR
            PUTS                    ;打印output:\n
            LD      R0, S4_ADDR     ;打印"righ"并结束程序
            PUTS
            BRnzp   FINISH
            
            
INPUT       ST      R7, SAVER7      ;因为子程序中调用了TRAP指令会改变R7中的值，所以先保存R7
            LD      R0, S6_ADDR     
            PUTS                    ;打印"input:\n"提示输入字符串
            AND     R2, R2, #0      ;R2<-0
            ADD     R2, R2, #5      ;R2<-5，R2用于计数，因为密码有5位，所以R2从5开始递减计数
LOOP1       BRz     OVER            ;若R2为0，则读入完毕
            GETC                    ;读入字符，存在R0中
            OUT                     ;把读入的字符打印在屏幕上
            STR     R0, R1, #0      ;把读入的字符存在R1指向的内存位置中
            ADD     R1, R1, #1      ;R1 <- R1 + 1
            ADD     R2, R2, #-1     ;R2 <- R2 - 1
            BRnzp   LOOP1
            
OVER        STR     R2, R1, #0      ;字符串末尾添加'\0'，其ASCII码为0，此时R2恰好为0
            LD      R7, SAVER7
            RET


COMPARE     AND     R0, R0, #0      ;R0清零
            AND     R3, R3, #0      ;R3清零
            ADD     R3, R3, #5      ;R3<-5
LOOP2       BRz     CHANGER0        ;若R3等于0，说明5个字符都已比较完毕
            LDR     R4, R1, #0      ;将放在内存中的密码字符读到R4中
            LDR     R5, R2, #0      ;将放在内存中的读入的字符串中的字符读到R5中
            NOT     R4, R4          
            ADD     R4, R4, #1      ;R4取反加1
            ADD     R4, R4, R5      ;比较R4，R5中的值是否相等
            BRnp    RETURN          ;若不等则直接返回，结束比较
            ADD     R1, R1, #1      
            ADD     R2, R2, #1
            ADD     R3, R3, #-1     ;否则R1,R2均后移一位以读取下一位字符，R3自减1
            BRnzp   LOOP2
CHANGER0    ADD     R0, R0, #1      ;若在循环中没有返回，说明每个字符都相等，R0赋值为1再返回表示两字符串相等
RETURN      RET
                
                
FINISH      HALT

S1_ADDR     .FILL   x3100
S2_ADDR     .FILL   x3200
S3_ADDR     .FILL   x3300
S4_ADDR     .FILL   x3400
S5_ADDR     .FILL   x3500
S6_ADDR     .FILL   x3600
S7_ADDR     .FILL   x3700
SAVER7      .FILL   x0000
NEWLINE     .FILL   x000A
            .END
                
            .ORIG x3100
S1          .STRINGZ "hello";password
            .END
            .ORIG x3200
S2          .STRINGZ "world";verification code
            .END
            .ORIG x3400
S4          .STRINGZ "righ\n";
            .END
            .ORIG x3500
S5          .STRINGZ "wron\n";
            .END
            .ORIG x3600
S6          .STRINGZ "input:\n"
            .END
S7          .ORIG x3700
            .STRINGZ "output:\n"
            .END

            