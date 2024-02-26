/*
 * @Author       : Chivier Humber
 * @Date         : 2021-09-15 21:15:24
 * @LastEditors  : Chivier Humber
 * @LastEditTime : 2021-11-23 16:08:51
 * @Description  : file content
 */
#include "common.h"
#include "memory.h"

namespace virtual_machine_nsp {
    void memory_tp::ReadMemoryFromFile(std::string filename, int beginning_address) {
        // Read from the file
        // TO BE DONE
        // 这个函数的作用是依次读取文件的每一行，将字符串转化为指令存到内存中
        std::ifstream file(filename);
        std::string line;
        int address = beginning_address;
        while (std::getline(file, line)) {
            memory[address++] = TranslateInstruction(line);    
        }
        
    }

    int16_t memory_tp::GetContent(int address) const {
        // get the content
        // TO BE DONE
        return memory[address];// 直接返回内存中的内容即可
    }

    int16_t& memory_tp::operator[](int address) {
        // get the content
        // TO BE DONE
        return memory[address];// 也是直接返回内存中的内容即可
    }    
}; // virtual machine namespace
