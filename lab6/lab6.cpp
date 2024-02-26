#include <cstdint>
#include <iostream>
#include <fstream>
#include <bitset>


#define LENGTH 1
#define MAXLEN 100
#define STUDENT_ID_LAST_DIGIT 1



// 实现求余数
int Mod(int n,int x)
{
    while(n >= 0)
        n = n - x;
    return (n + x);
}

// 实现除法
int Div(int n,int x)
{
    int res = 0;
    while(n >= 0)
    {
        n = n - x;
        res++;
    }
    return (res - 1);
}

// 比较两个字符串是否相同
int compare(char *s1,char *s2)
{
    while(1)
    {
        if(*s1 == '\0') {
            return (*s2 == '\0') ? 1 : 0;
        }
        if(*(s1++) != *(s2++))
            return 0;
    }
}

int16_t lab1(int16_t n) {
    int16_t count = 0,isEven = 0;
    if(Mod(n,2) == 0) {
        n--;//如果n为偶数，计算n-1的原码的1的数目，即为n的补码的0的树目
        isEven=1;
    }
    while(n){
        if(Mod(n,2)) count++;//也可写作count+=n%2;
        n = Div(n,2);
    }
    if(!isEven){
        count=16 - count;
    }
    //count = count + STUDENT_ID_LAST_DIGIT;
    return (count + STUDENT_ID_LAST_DIGIT);
}

int16_t lab2(int16_t n) {
    int16_t f = 3, direction = 0,temp,temp_f;
    for(n--;n > 0;n--)
    {
        f = direction ? (f + f - 2) : (f + f + 2);
        if(f > 4096)
            f = f - 4096;
        if(Mod(f,8) == 0)
        {
            direction = !direction;
            //break;
        }
        else
        {
            temp_f = f;
            do
            {
                temp = Div(temp_f,10);
                temp_f = Mod(temp_f,10);
                if(temp_f == 8)
                {
                    direction = !direction;
                    break;
                }
                else
                {
                    temp_f = temp;
                }
            } while(temp);
        }   
    }
    return f;
}

int16_t lab3(char s1[], char s2[], int input_cnt, char my_input[10][MAXLEN]) {
    if(input_cnt == 0) {
        printf("no input!");
        return 0;
    }
    else if(input_cnt > 3) {
        printf("wrong input number!\n");
        return 0;
    }
    if(compare(s1,my_input[0])) {
        printf("righ\n");
        return 1;
    }
    else {
        printf("wron\n%s\n",s2);
        if(input_cnt == 1) {
            printf("please input verify:\n");
            return 0;
        }
        else if(compare(s2,my_input[1])) {
            if(input_cnt == 2) {
                printf("please input password again:\n");
            }
            else if(compare(s1,my_input[2])) {
                printf("righ\n");
            }
            else {
                printf("wron\n");
            }
        }
        else {
            printf("wron\n");
        }
    }
    return 1;
}


int times;//times表示操作的次数
int REMOVE(int n,int state,int16_t *memory);
int PUT(int n,int state,int16_t *memory);

int REMOVE(int n,int state,int16_t *memory)
{
	if(n==0) return state;
	if(n==1){
		state++;//改变state
		memory[++times] = state;//存储当前操作后的state
		return state;
	}
	state = REMOVE(n-2, state,memory);
    int16_t mark = 1;

    for(int i = 0;i < n-1;i++)
    {
        mark = mark + mark;
    }
        
	state = state + mark;
	memory[++times] = state;
	state = PUT(n-2, state,memory);
	state = REMOVE(n-1, state,memory);
	return state;
}

int PUT(int n,int state,int16_t *memory){
	if(n == 0)	return state;
	if(n == 1)
	{
		state--;//改变state
		memory[++times] = state;//存储当前操作后的state
		return state;
	}
	state = PUT(n-2, state,memory);
	int16_t mark = 1;
    for(int i = 0;i < n-1;i++)
        mark = mark + mark;
    //mark = ~mark;
	state = state - mark;//将state的第n位变成0
	memory[++times] = state;
	state = REMOVE(n-2,state,memory);
	state = PUT(n-1, state,memory);
	return state;
}


int16_t lab4(int16_t *memory, int16_t n) {
    //memory[0] = n;
	times = -1;
	REMOVE(n,0,memory);  
    return times+1;
}


int main()
{
    std::fstream file;
    file.open("F:\\ICSH\\Lab\\lab6\\test.txt", std::ios::in);
    //file.open("test.txt", std::ios::in);


    // lab1
    int16_t n = 0;
    std::cout << "===== lab1 =====" << std::endl;
    for (int i = 0; i < LENGTH; ++i) {
        file >> n;
        std::cout << lab1(n) << std::endl;
    }

    // lab2
    std::cout << "===== lab2 =====" << std::endl;
    for (int i = 0; i < LENGTH; ++i) {
        file >> n;
        std::cout << lab2(n) << std::endl;
    }

    // lab3
    std::cout << "===== lab3 =====" << std::endl;
    char passwd[MAXLEN]; char verify[MAXLEN];
    int input_cnt=-1;
    char my_input[10][MAXLEN];
    for (int i = 0; i < LENGTH; ++i) {
        file >> passwd >> verify;
        file >> input_cnt;
        for (int j=0; j< input_cnt; j++)
        {
            file >> my_input[j];
        }
        //std::cout << lab3(passwd, verify , input_cnt, my_input) << std::endl;
        lab3(passwd, verify , input_cnt, my_input);
    }
    
    // lab4
    std::cout << "===== lab4 =====" << std::endl;
    int16_t memory[MAXLEN], move;
    for (int i = 0; i < LENGTH; ++i) {
        file >> n;
        //int16_t state = 0;
        move = lab4(memory, n);
        for(int j = 0; j < move; ++j){
            std::cout << std::bitset<16>(memory[j]) << std::endl;
        }
    }



    return 0;
}