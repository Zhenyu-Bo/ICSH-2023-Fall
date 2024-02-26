#include <cstdint>
#include <iostream>
#include <fstream>
#include <bitset>


#define LENGTH 1
#define MAXLEN 100
#define STUDENT_ID_LAST_DIGIT 3



int16_t lab1(int16_t n) {
    // initialize

    // calculation

    // return value
}

int16_t lab2(int16_t n) {
    // initialize

    // calculation

    // return value
}

int16_t lab3(char s1[], char s2[], int input_cnt, char my_input[10][MAXLEN]) {
    // initialize

    // calculation

    // return value
}


int16_t lab4(int16_t *memory, int16_t n) {
    // initialize

    // calculation

    // return value
}


int main()
{
    std::fstream file;
    file.open("test.txt", std::ios::in);



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
        std::cout << lab3(passwd, verify , input_cnt, my_input) << std::endl;
    }
    
    // lab4
    std::cout << "===== lab4 =====" << std::endl;
    int16_t memory[MAXLEN], move;
    for (int i = 0; i < LENGTH; ++i) {
        file >> n;
        int16_t state = 0;
        move = lab4(memory, n);
        for(int j = 0; j < move; ++j){
            std::cout << std::bitset<16>(memory[j]) << std::endl;
        }
    }



    return 0;
}