/*
 * @Author       : Chivier Humber
 * @Date         : 2021-08-30 14:36:39
 * @LastEditors  : liuly
 * @LastEditTime : 2022-11-15 21:12:51
 * @Description  : header file for small assembler
 */

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <math.h>

const int kLC3LineLength = 16;

extern bool gIsErrorLogMode;
extern bool gIsHexMode;

const std::vector<std::string> kLC3Pseudos({
    ".ORIG",
    ".END",
    ".STRINGZ",
    ".FILL",
    ".BLKW",
});

const std::vector<std::string> kLC3Commands({
    "ADD",    // 00: "0001" + reg(line[1]) + reg(line[2]) + op(line[3])
    "AND",    // 01: "0101" + reg(line[1]) + reg(line[2]) + op(line[3])
    "BR",     // 02: "0000000" + pcoffset(line[1],9)
    "BRN",    // 03: "0000100" + pcoffset(line[1],9)
    "BRZ",    // 04: "0000010" + pcoffset(line[1],9)
    "BRP",    // 05: "0000001" + pcoffset(line[1],9)
    "BRNZ",   // 06: "0000110" + pcoffset(line[1],9)
    "BRNP",   // 07: "0000101" + pcoffset(line[1],9)
    "BRZP",   // 08: "0000011" + pcoffset(line[1],9)
    "BRNZP",  // 09: "0000111" + pcoffset(line[1],9)
    "JMP",    // 10: "1100000" + reg(line[1]) + "000000"
    "JSR",    // 11: "01001" + pcoffset(line[1],11)
    "JSRR",   // 12: "0100000"+reg(line[1])+"000000"
    "LD",     // 13: "0010" + reg(line[1]) + pcoffset(line[2],9)
    "LDI",    // 14: "1010" + reg(line[1]) + pcoffset(line[2],9)
    "LDR",    // 15: "0110" + reg(line[1]) + reg(line[2]) + offset(line[3])
    "LEA",    // 16: "1110" + reg(line[1]) + pcoffset(line[2],9)
    "NOT",    // 17: "1001" + reg(line[1]) + reg(line[2]) + "111111"
    "RET",    // 18: "1100000111000000"
    "RTI",    // 19: "1000000000000000"
    "ST",     // 20: "0011" + reg(line[1]) + pcoffset(line[2],9)
    "STI",    // 21: "1011" + reg(line[1]) + pcoffset(line[2],9)
    "STR",    // 22: "0111" + reg(line[1]) + reg(line[2]) + offset(line[3])
    "TRAP"    // 23: "11110000" + h2b(line[1],8)
});

const std::vector<std::string> kLC3TrapRoutine({
    "GETC",   // x20
    "OUT",    // x21
    "PUTS",   // x22
    "IN",     // x23
    "PUTSP",  // x24
    "HALT"    // x25
});

const std::vector<std::string> kLC3TrapMachineCode({
    "1111000000100000", //x20
    "1111000000100001", //x21
    "1111000000100010", //x22
    "1111000000100011", //x23
    "1111000000100100", //x24
    "1111000000100101"  //x25
});

enum CommandType { OPERATION, PSEUDO };

static inline void SetErrorLogMode(bool error) {
    gIsErrorLogMode = error;
}

static inline void SetHexMode(bool hex) {
    gIsHexMode = hex;
}

// A wrapper class for std::unordered_map in order to map label to its address
class LabelMapType {
private:
    std::unordered_map<std::string, unsigned> labels_;

public:
    void AddLabel(const std::string &str, unsigned address);
    unsigned GetAddress(const std::string &str) const;
};

//判断字符串是否为伪指令
static inline int IsLC3Pseudo(const std::string &str) {
    int index = 0;
    for (const auto &command : kLC3Pseudos) {
        if (str == command) {
            return index;
        }
        ++index;
    }
    return -1;
}

//判断字符串是否为LC-3的指令
static inline int IsLC3Command(const std::string &str) {
    int index = 0;
    for (const auto &command : kLC3Commands) {
        if (str == command) {
            return index;
        }
        ++index;
    }
    return -1;
}

//判断字符串是否在TRAP矢量表中即是否为TRAP指令
static inline int IsLC3TrapRoutine(const std::string &str) {
    int index = 0;
    for (const auto &trap : kLC3TrapRoutine) {
        if (str == trap) {
            return index;
        }
        ++index;
    }
    return -1;
}

//把字符转换为十进制表示的整数
static inline int CharToDec(const char &ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return -1;
}

static inline char DecToChar(const int &num) {
    if (num <= 9) {
        return num + '0';
    }
    return num - 10 + 'A';
}

// trim string from both left & right
static inline std::string &Trim(std::string &s) {
    // TO BE DONE
    // 寻找第一个非空格字符出现的位置然后删除从0到该位置的所有字符
    auto first = s.find_first_not_of(' ');
    if (first != std::string::npos) {
        s.erase(0, first);
    } else {
        // 如果全是空白字符，则清空字符串
        s.clear();
        return s;
    }

    // 再删除从最后一个非空格字符出现的位置到字符串末尾的所有字符
    auto last = s.find_last_not_of(' ');
    if (last != std::string::npos) {
        s.erase(last + 1);
    }

    return s;
}

// Format one line from asm file, do the following:
// 1. remove comments which begin with ";"
// 2. convert the line into uppercase
// 3. replace all commas with whitespace (for splitting)
// 4. replace all "\t\n\r\f\v" with whitespace
// 5. remove the leading and trailing whitespace chars
// Note: please implement function Trim first
// Function to trim leading and trailing whitespace characters

static std::string FormatLine(const std::string &line) {
    // TO BE DONE
    std::string formattedLine = line;

    // 删除注释
    size_t commentPos = formattedLine.find(";");
    if (commentPos != std::string::npos) {
        formattedLine = formattedLine.substr(0, commentPos);
    }

    // 检查字符串是否含有".STRINGZ"，若含有，则此时直接返回即可
    if (formattedLine.find(".STRINGZ") != std::string::npos) {
        formattedLine = Trim(formattedLine);
        return formattedLine;
    }

    // 把字符串中的小写字母转换成大写字母(使用 std::transform and std:: toupper)
    std::transform(formattedLine.begin(), formattedLine.end(), formattedLine.begin(), ::toupper);

    // 用空格替换","及', '
    std::replace(formattedLine.begin(), formattedLine.end(), ',', ' ');

    // 用空格替换"\t\n\r\f\v"
    std::string whitespaceChars = "\t\n\r\f\v";
    for (char c : whitespaceChars) {
        std::replace(formattedLine.begin(), formattedLine.end(), c, ' ');
    }

    // 使用Trim函数去除字符串前后的空格
    formattedLine = Trim(formattedLine);
    return formattedLine;
}

static int RecognizeNumberValue(const std::string &str) {
    auto temp = str;
    temp = Trim(temp);
    int number;
    int temp_int;
    // 首先判断temp是否为空串，如果是，则说明输入的字符串为空或空格串，返回最大的int值表示出错
    if(temp.empty()) {
        printf("Error: empty string!\n");
        return std::numeric_limits<int>::max();
    }

    try {
        if(temp.at(0) == '#') {
            //如果str以#开头，则说明它后面接的是十进制数，直接用stoi函数转换即可（需要从下一位开始，不能包含'#'）
            if(temp.at(1) != '-' && temp.at(1) != '+' && !(temp.at(1) >= '0' && temp.at(1) <= '9'))
            {
                //printf("Error: invalid number format: ");
                //std::cout<< temp <<std::endl;
                return std::numeric_limits<int>::max();
            }
            number = 0;
            int begin = (temp.at(1) == '-' || temp.at(1) == '+') ? 2 : 1;
            for(int i = begin; i <= temp.length()-1; i++) {
                //std::cout << temp.at(temp.length()-i) << "\n";
                //std::cout<< temp.at(i) << " " <<temp.length()-1-i <<" "<< number <<std::endl;
                temp_int = CharToDec(temp.at(i));
                if(temp_int == -1 || temp_int > 9)
                {
                    //printf("Error: invalid number format: ");
                    //std::cout<< temp <<std::endl;
                    return std::numeric_limits<int>::max();
                }
                number = number + pow(10,temp.length()-1-i)*temp_int;
            }
            if(temp.at(1) == '-')
                number = -number;
            return number;
            //return std::stoi(str.substr(1),nullptr,10);
        }
        else if(temp.at(0) == 'x' || temp.at(0) == 'X') {
            //如果str以x或X开头，则说明它后面接的是16进制数，也是调用stoi函数转换，此时进制为16
            //std::cout << temp.substr(1) << ":" <<std::endl;
            /*if(temp.at(1) != '-' && temp.at(1) != '+' && !(temp.at(1) >= '0' && temp.at(1) <= 'F'))
            {
                printf("Error: invalid number format: ");
                std::cout<< temp <<std::endl;
                return std::numeric_limits<int>::max();
            }*/
            number = 0;
            //int begin = (temp.at(1) == '-' || temp.at(1) == '+') ? 2 : 1;
            for(int i = 1; i <= temp.length()-1; i++) {
                //std::cout << temp.at(temp.length()-i) << "\n";
                //std::cout<< temp.at(i) << " " <<temp.length()-1-i <<" "<< number <<std::endl;
                temp_int = CharToDec(temp.at(i));
                if(temp_int == -1)
                {
                    //printf("Error: invalid number format: ");
                    //std::cout<< temp <<std::endl;
                    return std::numeric_limits<int>::max();
                }
                if(temp.at(1) > '7') {
                    temp_int = 15 - temp_int;
                }
                number = number + pow(16,temp.length()-1-i)*temp_int;
            }
            //if(temp.at(1) == '-')
              //  number = -number;
            if(temp.at(1) > '7') {
                number = -number - 1;
            } 
            return number;
            //return std::stoi(str.substr(1),nullptr,16);
            //std::cout<< number <<std::endl;            
        }
        else {
            //当以上两种情况均不成立时，说明输入的字符串错误，返回最大的int值表示出错
            printf("Error: unrecognized number format!\n");
            return std::numeric_limits<int>::max();
        }
    } catch (std::invalid_argument&) {
        printf("Error: invalid number format!\n");
        return std::numeric_limits<int>::max();
    } catch (std::out_of_range&) {
        printf("Error: number out of range!\n");
        return std::numeric_limits<int>::max();
    }
}

static std::string NumberToAssemble(const int &number) {
    // Convert `number` into a 16 bit binary string
    // TO BE DONE
    std::string binaryString;
    int bit = 0;
    for (int i = 15; i >= 0; i--) {
        bit = (number >> i) & 1;//每次将number右移i位再与1进行与运算，这样可以判断number的第i位是否为1
        binaryString += std::to_string(bit);//因为i是从15开始递减的，所以将bit添加到binaryString的后面
    }
    return binaryString;
}

static std::string NumberToAssemble(const std::string &number) {
    // Convert `number` into a 16 bit binary string
    // You might use `RecognizeNumberValue` in this function
    // TO BE DONE
    //先调用RecognizeNumberValue函数将字符串转换为int类型的数
    int new_num = RecognizeNumberValue(number);
    //然后直接返回上一个参数为int类型的NumberToAssemble函数即可
    return NumberToAssemble(new_num);
}

static std::string ConvertBin2Hex(const std::string &bin) {
    // Convert the binary string `bin` into a hex string
    // TO BE DONE
    std::string hex;
    int size = 4;
    int len = bin.length();
    for (int i = 0; i < len; i += size) {
        // 如果剩余的字符数量少于4，只处理剩余的字符
        if (i + size > len) {
            size = len - i;
        }
        std::string nibble = bin.substr(i, size);//从第i个字符开始，提取长为size的子串
        int decimal = std::stoi(nibble, nullptr, 2);//将nibble转换成10进制的形式
        std::stringstream ss;
        // 将十进制数以16进制的形式写入ss中，并且使用大写字母的形式
        ss << std::hex << std::uppercase << decimal;
        hex += ss.str();//将ss中的内容添加到hex的后面
    }
    return hex;
}

class assembler {
    using Commands = std::vector<std::tuple<unsigned, std::string, CommandType>>;

private:
    LabelMapType label_map;
    Commands commands;

    static std::string TranslatePseudo(std::stringstream &command_stream);
    std::string TranslateCommand(std::stringstream &command_stream,
                                 unsigned int current_address);
    std::string TranslateOprand(unsigned int current_address, std::string str,
                                int opcode_length,std::vector<std::string> operand_list);
    std::string LineLabelSplit(const std::string &line, int current_address);
    int firstPass(std::string &input_filename);
    int secondPass(std::string &output_filename);

public:
    int assemble(std::string &input_filename, std::string &output_filename);
};
