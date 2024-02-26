;本题关键在于direction的变换，可以先判断f(n)是否为8的倍数，再判断其十进制表示的数中是否有一位是8
;在这个过程中如果满足任何一个条件，则diretion翻转，跳出判断，计算f(n+1)
;这里判断是否为8的倍数及位数是否为8都可以通过减法来求对8或10的余数和对10的商
;在判断direction是否要发生改变之前，需要先根据drection的值对f(n)做出相应的改变，并且还要判断f(n)是否越界
;如果超过4096，需要减去4096。因为f(1)=3,每次发现f(n)大于4096时，都会减去4096
;所以f(n)大于4096时，不会比4096的二倍还大，所以只需要减去一次4096即可

;Initialization
;
            .ORIG   x3000
            AND     R1, R1, #0      ;R1用于存储f(n)的值
            AND     R2, R2, #0      ;R2用于存储direction，R2=0时表示向上的箭头，否则表示向下的
            AND     R3, R3, #0      ;R3用于计算f(n)是否为8的倍数
            AND     R4, R4, #0      ;R4用于存储f(n)的十进制表示的每一项，以判断f(n)的十进制表示是否含有8
            AND     R5, R5, #0      ;R5用于存储R4/10的值，用于更新R4中的值
            LD      R6, PTR1        ;R6中存储PTR1(x3103)以备使用
            LD      R7, PTR2        ;R7中存入PTR2(4096)
            LDR     R0, R6, #-1     ;R0用于存储N，假设N存储在x3102中
            ADD     R1, R1, #3      ;R1初始化为3
            
;
;先改变R1的值，得到新的f(n),需要注意的是
;
CHANGER0    ADD     R0, R0, #-1     ;更新R0中的值
            BRz     STF             ;如果R1=0，说明计算完成，转到STF存储f(n)
            ADD     R1, R1, R1      ;R1 = 2*R1
            ADD     R1, R1, #2      ;先将R1加2，后面再根据R2中的值选择是否将R1减4
            ADD     R2, R2, #0      ;进行运算R2=R2,使得此时的nzp与R2中的值相关
            BRz     SubMod          ;如果R2=0,则不需要将R1减4
            ADD     R1, R1, #-4     ;如果R2!=0,则应将R1减2，由于已经将R1加2，所以减4
SubMod      NOT     R7, R7
            ADD     R7, R7, #1      ;将R7取反加1
            ADD     R1, R1, R7
            NOT     R7, R7
            ADD     R7, R7, #1      ;还原R7
            ADD     R1, R1, #0      ;R1 <- R1+0,使得此时nzp与R1相关
            BRzp    PART2           ;如果R1-4096>=0,则现在的R1即为指定范围内的R1,不需要再改变
            ADD     R1, R1, R7   ;如果R1-4096<0，说明原先的R1未越界，还原R1
;因为初始的R1为3，所以上述操作只需要执行一次就可以确保R1在范围内
;
;再判断是否要改变direction的方向
;
PART2       ADD     R4, R1, #0      ;R4 <- R1
            ADD     R3, R1, #0      ;R3 <- R1
JUDGE1      BRnz    JUDGE2
            ADD     R3, R3, #-8
            BRnzp   JUDGE1          ;R3-8后再判断是否为正
JUDGE2      BRz     CHANGER2        ;如果R3=0，说明R1是8的倍数，改变R2中的direction值
CHANGER4    ADD     R4, R4, #-10
            BRn     Add2            ;如果R4<0,判断余数是否为8
            ADD     R5, R5, #1      ;更新R5，用于保存R4/10
            BRnzp   CHANGER4
Add2        ADD     R4, R4, #2      ;如果R4<0,则先将R4加10得到余数，再减去8判断余数是否为8，相当于直接加2
            BRz     CHANGER2
            ADD     R4, R5, #0      ;R4 <- R5,相当于R4 <- R4/10
            BRz     RETURN          ;如果R4=0,说明R4的每一位都判断完了，返回最开始的CHANGER0
            AND     R5, R5, #0      ;R5重新赋值为0，准备计算R4的下一位
            BRnzp   CHANGER4        ;无条件跳转到CHANGER4
        

CHANGER2    NOT     R2, R2
RETURN      BRnzp   CHANGER0

STF         STR     R1, R6, #0      ;将结果存入x3103中
PTR1        .FILL   x3103           ;PTR1存储x3103
PTR2        .FILL   x1000           ;PTR2存储x1000(4096)
            HALT
            .END
