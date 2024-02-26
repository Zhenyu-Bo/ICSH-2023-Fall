/*
 * @Author       : Chivier Humber
 * @Date         : 2021-08-30 15:10:31
 * @LastEditors  : liuly
 * @LastEditTime : 2022-11-15 21:10:23
 * @Description  : content for samll assembler
 */

#include "assembler.h"
#include <string>
#include <regex>
#include <math.h>

// 检查n位长的立即数是否合法
bool IsValidImmediateNumber(const std::string& operand,int n = 5) {
    int number = RecognizeNumberValue(operand);
    if(number >= -pow(2,n-1) && number <= pow(2,n-1)-1)
        return true;
    else
        return false;
}

// 检查寄存器号是否合法
bool IsValidRegisterNumber(const std::string& operand) {
    // Check if the operand is a valid register number
    auto temp = operand;
    temp = Trim(temp);
    std::regex register_regex("^R[0-7]$");//以R开头，后面接一个0-7之间的数字的正则表达式
    return std::regex_match(temp, register_regex);//检查operand是否为R0~R7中的一个
}

// 检查Label的合法性
bool IsValidLabel(const std::string &label) {
    // Label的要求有：以字母或下划线开头，只包含字母、数字或下划线，不能重复

    // 先检查Label的首字符是否为字母或下划线
    if (!std::isalpha(label[0]) && label[0] != '_') {
        //std::cout << "Label " << label << " is invalid "<< std::endl;
        return false;
    }
    
    // 再检查Label是否只含字母、数字或下划线
    for (char c : label) {
        if (!std::isalnum(c) && c != '_') {
            //std::cout << "Label " << label << " is invalid "<< std::endl;
            return false;
        }
    }
    
    return true;
}

void PrintError(std::vector<std::string> operand_list) {
    int operand_list_size = operand_list.size();
    for(int i = 0;i < operand_list_size;i++)
        std::cout << operand_list.at(i) << " ";
    std::cout << std::endl;
}

// add label and its address to symbol table
void LabelMapType::AddLabel(const std::string &str, const unsigned address) {
    // 先检查Label的合法性
    // 检查Label是否已经被定义过
    if (labels_.find(str) != labels_.end()) {
        printf("Label %s has been defined!\n",str.c_str());
        //throw std::invalid_argument(str + "has been defined!");
    }
    else if (!IsValidLabel(str)) {
        // @ Error Invalid Label
        printf("Invalid Label %s!\n",str.c_str());
        //throw std::invalid_argument("Invalid label: " + str);
    }
    else
        labels_.insert({str, address});
}

unsigned LabelMapType::GetAddress(const std::string &str) const {
    if (labels_.find(str) == labels_.end()) {
        // not found
        //printf("Label %s not found!\n",str.c_str());
        return -1;
    }
    return labels_.at(str);
}

std::string assembler::TranslateOprand(unsigned int current_address, std::string str,int opcode_length,
                                        std::vector<std::string> operand_list) {
    // Translate the oprand
    str = Trim(str);//先去除str首尾的空格
    auto item = label_map.GetAddress(str);
    if (item != -1) {
        // str is a label
        // TO BE DONE
        auto temp = item;
        int offset = item - current_address - 1;//计算偏移量
        if(offset < -pow(2,opcode_length-1) || offset >= pow(2,opcode_length-1)) {
            // @ Error offset too large or too small
            std::cout <<"Error: can not reach label at ";
            PrintError(operand_list);
            exit(-32);
        }
        // 直接调用NumberToAssemble函数，将offset转换为二进制字符串并取它的最后opcode_length位
        return NumberToAssemble(offset).substr(16 - opcode_length);
    }
    if (str[0] == 'R') {
        // str is a register
        // TO BE DONE
        if(!IsValidRegisterNumber(str)) {
            // @ Error invalid register number
            std::cout << "Error: Invalid register number at ";
            PrintError(operand_list);
            exit(-31);
        }
        str[0] = '#';//将str[0]变成"#"，这样就可以调用NumberToAssemble函数来转换str了
        return NumberToAssemble(str).substr(16 - opcode_length);
    } else {
        // str is an immediate number
        // TO BE DONE
        if(!IsValidImmediateNumber(str,opcode_length)) {
            // @ Error invalid immediate number
            std::cout << "Error: Invalid immediate number or offset at ";
            PrintError(operand_list);
            exit(-32);
        }
        return NumberToAssemble(str).substr(16 - opcode_length);
    }
}


std::string assembler::LineLabelSplit(const std::string &line, int current_address) {
    // label?
    auto first_whitespace_position = line.find(' ');
    auto first_token = line.substr(0, first_whitespace_position);

    if (IsLC3Pseudo(first_token) == -1 && IsLC3Command(first_token) == -1 &&
        IsLC3TrapRoutine(first_token) == -1) {
        // * This is a label
        // save it in label_map
        // TO BE DONE
        // 直接调用AddLabel函数将标签和地址加入到label_map中即可
        label_map.AddLabel(first_token, current_address);

        // remove label from the line
        if (first_whitespace_position == std::string::npos) {
            // nothing else in the line
            return "";
        }
        auto command = line.substr(first_whitespace_position + 1);
        return Trim(command);
    }
    return line;
}

int ORIG[10],END[10];//记录.ORIG，.END出现位置的数组
int ORIG_num = 0, END_num = 0;

// Scan #1: save commands and labels with their addresses
int assembler::firstPass(std::string &input_filename) {
    std::string line;
    std::fstream input_file(input_filename, std::ios::in | std::ios::out);
    if (!input_file.is_open()) {
        std::cout << "Unable to open file" << std::endl;
        // @ Input file read error
        return -1;
    }
    memset(ORIG,-1,sizeof(int)*10);
    memset(END,-1,sizeof(int)*10);

    int orig_address = -1;
    int current_address = -1;
    int stringz_cnt = 0;//记录出现的.STRINGZ的数目
    int flag = 1;//设置一个变量flag，初始化为1，读取到.ORIG时置为0，读取到.END时置为1，这样就可以不处理.END到.ORIG的部分
    int line_number = 0;//记录读取的行号

    while (std::getline(input_file, line)) {

        line_number++;//每读取一行，将line_number加1
        line = FormatLine(line);
        //printf("%s\n",line.c_str());
        if (line.empty()) {
            continue;
        }

        auto command = LineLabelSplit(line, current_address);
        if (command.empty()) {
            continue;
        }

        // OPERATION or PSEUDO?
        auto first_whitespace_position = command.find(' ');
        auto first_token = command.substr(0, first_whitespace_position);
        // Special judge .ORIG and .END
        if (first_token == ".ORIG") {
            flag = 0;
            std::string orig_value =
                command.substr(first_whitespace_position + 1);
            orig_address = RecognizeNumberValue(orig_value);
            
            //std::cout << orig_address<<std::endl;
            if (orig_address == std::numeric_limits<int>::max()) {
                // @ Error address
                // 打印错误提示信息
                std::cout << "Error: Invalid .ORIG value"<< orig_value << " at line:" << line_number << std::endl;
                return -2;
            }
            ORIG[ORIG_num++] = orig_address;
            current_address = orig_address;
            continue;
        }

        if (orig_address == -1) {
            // @ Error Program begins before .ORIG
            // 打印错误提示信息
            std::cout << "Error: Program begins before .ORIG" << "at line:" << line_number << std::endl;
            return -3;
        }

        /*if(flag == 1) {
            continue;//flag = 1时，说明读取到了.END，但还没有读取到.ORIG，此时不需要处理，直接continue即可
        }*/
        if (first_token == ".END") {
            //break;
            commands.push_back({current_address, command, CommandType::PSEUDO});
            END[END_num++] = current_address + stringz_cnt;
            flag = 1;
            orig_address = -1;
            continue;
        }

        // For LC3 Operation
        if (IsLC3Command(first_token) != -1 ||
            IsLC3TrapRoutine(first_token) != -1) {
            commands.push_back(
                {current_address, command, CommandType::OPERATION});
            current_address += 1;
            continue;
        }

        // For Pseudo code
        commands.push_back({current_address, command, CommandType::PSEUDO});
        auto operand = command.substr(first_whitespace_position + 1);
        if (first_token == ".FILL") {
            operand = Trim(operand);
            auto num_temp = RecognizeNumberValue(operand);
            if (operand.length() > 5 || num_temp == std::numeric_limits<int>::max()) {
                // @ Error Invalid Number input @ FILL
                // 打印错误信息
                std::cout << "Error: Invalid .FILL value " << operand << " at line:" << line_number <<std::endl;
                return -4;
            }
            else if (num_temp > 65535 || num_temp < -65536) {
                // @ Error Too large or too small value  @ FILL
                // 打印错误信息
                std::cout << "Error: Too large or too small value " << operand << " at line:" << line_number <<std::endl;
                return -5;
            }
            current_address += 1;
        }
        if (first_token == ".BLKW") {
            // modify current_address
            // TO BE DONE
            // 这与上面的.FILL操作类似，不同的是.FILL只需分配1个存储单元，而.BLKW需要分配n个存储单元
            auto num_temp = RecognizeNumberValue(operand);
            if (num_temp == std::numeric_limits<int>::max()) {
                // @ Error Invalid Number input @ FILL
                // 打印错误信息
                std::cout << "Error: Invalid .FILL value: " << operand << "at line:" << line_number <<std::endl;
                return -4;
            }
            else if (num_temp > 65535 || num_temp < -65536) {
                // @ Error Too large or too small value  @ FILL
                // 打印错误信息
                std::cout << "Error: Too large or too small value: " << operand << "at line:" << line_number <<std::endl;
                return -5;
            }
            current_address += num_temp;

        }
        if (first_token == ".STRINGZ") {
            // modify current_address
            // TO BE DONE
            // 首先判断字符串的首尾是否为"(需要加转义字符\)   
            operand = Trim(operand);
            if (operand[0] != '\"' || operand[operand.size() - 1] != '\"')
            { 
                // 输出operand
                // @ Error String format error
                // 打印错误信息
                std::cout << "Error: String format error at line:" << line_number <<std::endl;
                return -6;
            }
            //然后改变current_address即可，这里需要减去两个双引号和一个空字符，故加上operand.size() - 3
            current_address += operand.size() - 3;
            // 求字符串中转义字符的数量
            int escape_count = 0;
            for (int i = 1; i < operand.size() - 1; i++) {
                if (operand[i] == '\\') {
                    escape_count++;
                }
            }
            // 再将current_address减去转义字符的数量
            current_address -= escape_count;
            stringz_cnt++;
        }
    }
    // OK flag
    return 0;
}

int END_count = 0;

std::string assembler::TranslatePseudo(std::stringstream &command_stream) {
    std::string pseudo_opcode;
    std::string output_line;
    command_stream >> pseudo_opcode;
    if (pseudo_opcode == ".END") {
        if(ORIG[++END_count] != -1) {
            int num_temp = ORIG[END_count] - END[END_count-1];
            for (int i = 0; i < num_temp; i++) {
                output_line += "0000000000000000\n";
            }
            //删除最后一个"\n"，防止出现空行
            output_line = output_line.substr(0, output_line.size() - 1);
        }
        else
            output_line = "";
    }
    else if (pseudo_opcode == ".FILL") {
        std::string number_str;
        command_stream >> number_str;
        output_line = NumberToAssemble(number_str);
        //if (gIsHexMode)//gIsHexMode是一个全局变量，如果它为真，则当前的模式是十六进制模式，输出应该是十六进制
          //  output_line = ConvertBin2Hex(output_line);
    } else if (pseudo_opcode == ".BLKW") {
        // Fill 0 here
        // TO BE DONE
        std::string number_str;
        command_stream >> number_str;
        int num_temp = RecognizeNumberValue(number_str);
        // 因为在first_pass中已经检查过了.BLKW后的数值n的合法性，所以这里不需要再做检查
        for (int i = 0; i < num_temp; i++) {
            output_line += "0000000000000000\n";
        }
        //删除最后一个"\n"，防止出现空行
        output_line = output_line.substr(0, output_line.size() - 1);
    } 
    else if (pseudo_opcode == "FILL_ZERO_LINE") {
        std::string number_str;
        command_stream >> number_str;
        int num_temp = RecognizeNumberValue(number_str);
        // 因为在first_pass中已经检查过了.BLKW后的数值n的合法性，所以这里不需要再做检查
        for (int i = 0; i < num_temp; i++) {
            output_line += "0000000000000000\n";
        }
        // 删除最后一个"\n"，防止出现空行
        output_line = output_line.substr(0, output_line.size() - 1);
    }
    else if (pseudo_opcode == ".STRINGZ") {
        // Fill string here
        // TO BE DONE
        std::string string_value;
        getline(command_stream, string_value);//读取一行，即读取.STRINGZ后的字符串
        string_value = Trim(string_value);
        // 去除首尾双引号（因为在first_pass中已经检查过了.STRINGZ后字符串的合法性，所以这里不需要再做检查）
        string_value = string_value.substr(1, string_value.size() - 2);
        // 将字符串的每个字符添加到output_line中（每个字符单独占一行，故还需添加"\n"）
        for (size_t i = 0; i < string_value.size(); ++i) {
            char c = string_value.at(i);
            // 需要考虑c是否为转义字符
            if (c == '\\' && i + 1 < string_value.size()) {
                char next_c = string_value.at(i+1);
                
                if (next_c == 'n') {
                    c = '\n';
                    ++i;
                } else if (next_c == 't') {
                    c = '\t';
                     ++i;
                } else if (next_c == '\"') {
                    c = '\"';
                    ++i;
                } else if (next_c == '\\') {
                    c = '\\';
                    ++i;
                } else if (next_c == 'r') {
                    c = '\r';
                    ++i;
                } else if (next_c == 'a') {
                    c = '\a';
                    ++i;
                } else if (next_c == 'b') {
                    c = '\b';
                    ++i;
                } else if (next_c == 'f') {
                    c = '\f';
                    ++i;
                } else if (next_c == 'v') {
                    c = '\v';
                    ++i;
                } else if (next_c == '0') {
                    c = '\0';
                    ++i;
                } else if (next_c >= '1' && next_c <= '7') {
                    // 处理八进制转义字符
                    std::string octal_value;
                    while (i < string_value.size() && string_value[i] >= '0' && string_value[i] <= '7') {
                    octal_value += string_value[i++];
                }
                c = static_cast<char>(std::stoi(octal_value, nullptr, 8));
                } else if (next_c == 'x') {
                    // 处理十六进制转义字符
                    std::string hex_value;
                    while (i < string_value.size() && isxdigit(string_value[i])) {
                        hex_value += string_value[i++];
                    }
                    c = static_cast<char>(std::stoi(hex_value, nullptr, 16));
                }
            }
            output_line = output_line + NumberToAssemble(static_cast<int>(c)) + "\n";//需要先将字符c强制转换为整数形式，即它的ASCII码
        }
        // 串尾添加"\0"
        output_line += "0000000000000000";
    }
    if (gIsHexMode)//gIsHexMode是一个全局变量，如果它为真，则当前的模式是十六进制模式，输出应该是十六进制
            output_line = ConvertBin2Hex(output_line);
    return output_line;
}

std::string assembler::TranslateCommand(std::stringstream &command_stream,
                                        unsigned int current_address) {
    std::string opcode;
    std::string br_immediate;
    command_stream >> opcode;
    auto command_tag = IsLC3Command(opcode);

    std::vector<std::string> operand_list;
    std::string operand;
    while (command_stream >> operand) {
        operand_list.push_back(operand);
    }
    auto operand_list_size = operand_list.size();
    std::string output_line;
    std::vector<std::string> ope;
    ope.push_back(opcode);
    for (const auto& op : operand_list) {
        ope.push_back(op);
    }

    if (command_tag == -1) {
        // This is a trap routine
        command_tag = IsLC3TrapRoutine(opcode);
        output_line = kLC3TrapMachineCode[command_tag];
    } else {
        // This is a LC3 command
        switch (command_tag) {
        case 0:
            // "ADD"
            output_line += "0001";
            if (operand_list_size != 3) {
                // @ Error operand numbers
                printf("Invalid operand numbers at ADD ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1],3,ope);
            if (operand_list[2][0] == 'R') {
                // The third operand is a register                
                output_line += "000";
                output_line += TranslateOprand(current_address, operand_list[2],3,ope);        
            } else {
                // The third operand is an immediate number
                output_line += "1";
                output_line += TranslateOprand(current_address, operand_list[2], 5,ope);                   
            }
            break;
        case 1:
            // "AND"
            // TO BE DONE
            output_line += "0101";
            if (operand_list_size != 3) {
                // @ Error operand numbers
                printf("Invalid operand numbers at AND ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1],3,ope);
            if (operand_list[2][0] == 'R') {
                // The third operand is a register
                output_line += "000";
                output_line += TranslateOprand(current_address, operand_list[2],3,ope);
            } else {
                // The third operand is an immediate number
                output_line += "1";
                output_line += TranslateOprand(current_address, operand_list[2], 5,ope);
            }
            break;
        case 2:
            // "BR"
            // TO BE DONE
            output_line += "0000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at BR ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "111";
            output_line += TranslateOprand(current_address, operand_list[0], 9,ope);
            break;
        case 3:
            // "BRN"
            // TO BE DONE
            output_line += "0000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at BRn ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "100";
            output_line += TranslateOprand(current_address, operand_list[0], 9,ope);
            break;
        case 4:
            // "BRZ"
            // TO BE DONE
            output_line += "0000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at BRz ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "010";
            output_line += TranslateOprand(current_address, operand_list[0], 9,ope);
            break;
        case 5:
            // "BRP"
            // TO BE DONE
            output_line += "0000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at BRp ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "001";
            output_line += TranslateOprand(current_address, operand_list[0], 9,ope);
            break;
        case 6:
            // "BRNZ"
            // TO BE DONE
            output_line += "0000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at BRnz ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "110";
            output_line += TranslateOprand(current_address, operand_list[0], 9,ope);
            break;
        case 7:
            // "BRNP"
            // TO BE DONE
            output_line += "0000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at BRnp ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "101";
            output_line += TranslateOprand(current_address, operand_list[0], 9,ope);
            break;
        case 8:
            // "BRZP"
            // TO BE DONE
            output_line += "0000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at BRzp ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "011";
            output_line += TranslateOprand(current_address, operand_list[0], 9,ope);
            break;
        case 9:
            // "BRNZP"
            output_line += "0000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at BRnzp ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "111";
            output_line += TranslateOprand(current_address, operand_list[0], 9,ope);
            break;
        case 10:
            // "JMP"
            // TO BE DONE
            output_line += "1100";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at JMP ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "000";
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += "000000";
            break;
        case 11:
            // "JSR"
            // TO BE DONE
            output_line += "0100";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at JSR ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "1";
            output_line += TranslateOprand(current_address, operand_list[0], 11,ope);
            break;
        case 12:
            // "JSRR"
            // TO BE DONE
            output_line += "0100";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at JSRR ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += "000";
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += "000000";
            break;
        case 13:
            // "LD"
            // TO BE DONE
            output_line += "0010";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                printf("Invalid operand numbers at LD ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1], 9,ope);
            break;
        case 14:
            // "LDI"
            // TO BE DONE
            output_line += "1010";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                printf("Invalid operand numbers at LDI ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1], 9,ope);
            break;
        case 15:
            // "LDR"
            // TO BE DONE
            output_line += "0110";
            if (operand_list_size != 3) {
                // @ Error operand numbers
                printf("Invalid operand numbers at LDR ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1],3,ope);
            output_line += TranslateOprand(current_address, operand_list[2], 6,ope);
            break;
        case 16:
            // "LEA"
            // TO BE DONE
            output_line += "1110";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                printf("Invalid operand numbers at LDR ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1], 9,ope);
            break;
        case 17:
            // "NOT"
            // TO BE DONE
            output_line += "1001";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                printf("Invalid operand numbers at LDR ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1],3,ope);
            output_line += "111111";
            break;
        case 18:
            // RET
            output_line += "1100000111000000";
            if (operand_list_size != 0) {
                // @ Error operand numbers
                printf("Invalid operand numbers at RET ");
                PrintError(operand_list);
                exit(-30);
            }
            break;
        case 19:
            // RTI
            // TO BE DONE
            output_line += "1000000000000000";
            if (operand_list_size != 0) {
                // @ Error operand numbers
                printf("Invalid operand numbers at RTI ");
                PrintError(operand_list);
                exit(-30);
            }
            break;
        case 20:
            // ST
            output_line += "0011";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                printf("Invalid operand numbers at ST ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1], 9,ope);
            break;
        case 21:
            // STI
            // TO BE DONE
            output_line += "1011";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                printf("Invalid operand numbers at STI ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1], 9,ope);
            break;
        case 22:
            // STR
            // TO BE DONE
            output_line += "0111";
            if (operand_list_size != 3) {
                // @ Error operand numbers
                printf("Invalid operand numbers at STR ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0],3,ope);
            output_line += TranslateOprand(current_address, operand_list[1],3,ope);
            output_line += TranslateOprand(current_address, operand_list[2],6,ope);
            break;
        case 23:
            // TRAP
            // TO BE DONE
            output_line += "11110000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                printf("Invalid operand numbers at TRAP ");
                PrintError(operand_list);
                exit(-30);
            }
            output_line += TranslateOprand(current_address, operand_list[0], 8,ope);
            break;
        default:
            // Unknown opcode
            // @ Error
            printf("Unknown opcode at line %d\n",current_address);
            exit(-40);
            //break;
        }
    }

    if (gIsHexMode)
        output_line = ConvertBin2Hex(output_line);

    return output_line;
}


int assembler::secondPass(std::string &output_filename) {
    // Scan #2:
    // Translate
    std::ofstream output_file;
    // Create the output file
    output_file.open(output_filename);
    if (!output_file) {
        // @ Error at output file
        std::cerr << "Error at output file" << std::endl;
        return -20;
    }

    for (const auto &command : commands) {
        const unsigned address = std::get<0>(command);
        const std::string command_content = std::get<1>(command);
        const CommandType command_type = std::get<2>(command);
        auto command_stream = std::stringstream(command_content);

        if (command_type == CommandType::PSEUDO) {
            // Pseudo
            std::string temp = TranslatePseudo(command_stream);
            if(temp != "")
                output_file << temp << std::endl;
        } else {
            // LC3 command
            output_file << TranslateCommand(command_stream, address) << std::endl;
        }
    }

    // Close the output file
    output_file.close();
    // OK flag
    return 0;
}

// assemble main function
int assembler::assemble(std::string &input_filename, std::string &output_filename) {
    auto first_scan_status = firstPass(input_filename);
    if (first_scan_status != 0) {
        return first_scan_status;
    }
    auto second_scan_status = secondPass(output_filename);
    if (second_scan_status != 0) {
        return second_scan_status;
    }
    // OK flag
    return 0;
}
