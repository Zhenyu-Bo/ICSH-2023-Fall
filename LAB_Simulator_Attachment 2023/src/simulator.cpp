/*
 * @Author       : Chivier Humber
 * @Date         : 2021-09-15 19:41:16
 * @LastEditors  : Chivier Humber
 * @LastEditTime : 2021-11-24 05:33:38
 * @Description  : file content
 */
#include "simulator.h"
#include <bitset>
#include <iomanip>

namespace virtual_machine_nsp {
template <typename T, unsigned B>
inline T SignExtend(const T x) {
    // Extend the number
    // TO BE DONE
    // 用掩码mask来将x的指定位变为1，放到temp中
    T mask = 1 << (B-1); // mask初始值设为最高位为1的B位宽的数值
    T temp = x;
    // 若x的首位为1，则需要在前面补1，如果是0，则不需要处理，因为在内存中会自动填充0
    if((mask & x) != 0)
    {
	    while(mask) // 循环至mask为0结束，此时x前的所有位都填充了1
        {
	    mask = mask << 1;// mask先左移一位
            temp |= mask;// 将temp的指定位变为1
        }
    }
    else
    {
        mask -= 1;
        temp &= mask;
    }
    return temp;
}

void virtual_machine_tp::UpdateCondRegister(int regname) {
    // Update the condition register
    // TO BE DONE
    // 根据reg[regname]的值来设置条件码即可，nzp分别对应4,2,1
    reg[R_COND] = (reg[regname]) ? (reg[regname] > 0 ? 1 : 4) : 2;
}

void virtual_machine_tp::VM_ADD(int16_t inst) {
    int flag = inst & 0b100000;
    int dr = (inst >> 9) & 0x7;
    int sr1 = (inst >> 6) & 0x7;
    if (flag) {
        // add inst number
        int16_t imm = SignExtend<int16_t, 5>(inst & 0b11111);
        reg[dr] = reg[sr1] + imm;
    } else {
        // add register
        int sr2 = inst & 0x7;
        reg[dr] = reg[sr1] + reg[sr2];
    }
    // Update condition register
    UpdateCondRegister(dr);
}

void virtual_machine_tp::VM_AND(int16_t inst) {
    // TO BE DONE
    // AND指令和ADD指令基本相同，仿照上面的ADD指令的操作即可
    int flag = inst & 0b100000; // 取指令的第5位
    int dr = (inst >> 9) & 0x7; // 取目标寄存器的编号
    int sr1 = (inst >> 6) & 0x7; // 取源寄存器1的编号
    if (flag) {
        // and inst number
        int16_t imm = SignExtend<int16_t, 5>(inst & 0b11111);
        reg[dr] = reg[sr1] & imm;
    } else {
        // and register
        int sr2 = inst & 0x7;
        reg[dr] = reg[sr1] & reg[sr2];
    }
    // Update condition register
    UpdateCondRegister(dr);
}

void virtual_machine_tp::VM_BR(int16_t inst) {
    int16_t pc_offset = SignExtend<int16_t, 9>(inst & 0x1FF);
    //std:: cout << pc_offset << std::endl;
    int16_t cond_flag = (inst >> 9) & 0x7;
    if (gIsDetailedMode) {
        std::cout << reg[R_PC] << std::endl;
        std::cout << pc_offset << std::endl;
    }
    if (cond_flag & reg[R_COND]) {
        reg[R_PC] += pc_offset;
    }
}

void virtual_machine_tp::VM_JMP(int16_t inst) {
    // TO BE DONE
    int16_t baseR = (inst >> 6) & 0x7; // 取基址寄存器的编号
    reg[R_PC] = reg[baseR];
}

void virtual_machine_tp::VM_JSR(int16_t inst) {
    // TO BE DONE
    // 这里JSR和JSRR一起处理，所以需要先判断是哪一种指令
    int flag = (inst >> 11) & 1; // 取指令的第11位
    int16_t temp = reg[R_PC]; // 保存R7的值, 返回时需要使用
    if(flag)
    {
        // JSR
        reg[R_PC] += SignExtend<int16_t, 11>(inst & 0x7FF); // PC的值加上偏移量
    }
    else
    {
        // JSRR
        int16_t baseR = (inst >> 6) & 0x7; // 取基址寄存器的编号
        reg[R_PC] = reg[baseR]; // PC的值变为基址寄存器的值
    }
    reg[R_R7] = temp;
}

void virtual_machine_tp::VM_LD(int16_t inst) {
    int16_t dr = (inst >> 9) & 0x7;
    int16_t pc_offset = SignExtend<int16_t, 9>(inst & 0x1FF);
    int16_t addr = reg[R_PC] + pc_offset;
    if (reg[R_PSR] >> 15 && (addr < 0x3000 || addr >= 0xFE00)) {
        std::cout << "--- Access violation ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
    reg[dr] = mem[addr];
    UpdateCondRegister(dr);
}

void virtual_machine_tp::VM_LDI(int16_t inst) {
    // TO BE DONE
    int16_t dr = (inst >> 9) & 0x7; // 取目标寄存器的编号
    int16_t pc_offset = SignExtend<int16_t, 9>(inst & 0x1FF); // 取偏移量
    int16_t addr_1 = reg[R_PC] + pc_offset;
    if (reg[R_PSR] >> 15 && (addr_1 < 0x3000 || addr_1 >= 0xFE00)) {
        std::cout << "--- Access violation ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
    int16_t addr_2 = mem[addr_1];
    if (reg[R_PSR] >> 15 && (addr_2 < 0x3000 || addr_2 >= 0xFE00)) {
        std::cout << "--- Access violation ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
    reg[dr] = mem[addr_2];
    UpdateCondRegister(dr);
}

void virtual_machine_tp::VM_LDR(int16_t inst) {
    // TO BE DONE
    int16_t dr = (inst >> 9) & 0x7; // 取目标寄存器的编号
    int16_t br = (inst >> 6) & 0x7; // 取基址寄存器的编号
    int16_t offset = SignExtend<int16_t,6>(inst & 0x3F); // 取基址偏移量并扩展
    int16_t addr = reg[br] + offset;
    if (reg[R_PSR] >> 15 && (addr < 0x3000 || addr >= 0xFE00)) {
        std::cout << "--- Access violation ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
    reg[dr] = mem[addr];
    UpdateCondRegister(dr);
}

void virtual_machine_tp::VM_LEA(int16_t inst) {
    // TO BE DONE
    int16_t dr = (inst >> 9) & 0x7; // 取目标寄存器的编号
    reg[dr] = reg[R_PC] + SignExtend<int16_t,9>(inst & 0x1FF);
}

void virtual_machine_tp::VM_NOT(int16_t inst) {
    // TO BE DONE
    int16_t dr = (inst >> 9) & 0x7; // 取目标寄存器的编号
    int16_t sr = (inst >> 6) & 0x7; // 取源寄存器的编号
    reg[dr] = ~reg[sr];
    UpdateCondRegister(dr);
}

void virtual_machine_tp::VM_RTI(int16_t inst) {
    ; // PASS
}

void virtual_machine_tp::VM_ST(int16_t inst) {
    // TO BE DONE
    int16_t sr = (inst >> 9) & 0x7; // 取源寄存器的编号
    int16_t pc_offset = SignExtend<int16_t,9>(inst & 0x1FF);
    int16_t addr = reg[R_PC] + pc_offset;
    if (reg[R_PSR] >> 15 && (addr < 0x3000 || addr >= 0xFE00)) {
        std::cout << "--- Access violation ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
    mem[addr] = reg[sr];
}

void virtual_machine_tp::VM_STI(int16_t inst) {
    // TO BE DONE
    int16_t sr = (inst >> 9) & 0x7;
    int16_t pc_offset = SignExtend<int16_t,9>(inst & 0x1FF);
    int16_t addr_1 = reg[R_PC] + pc_offset;
    if (reg[R_PSR] >> 15 && (addr_1 < 0x3000 || addr_1 >= 0xFE00)) {
        std::cout << "--- Access violation ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
    int16_t addr_2 = mem[addr_1];
    if (reg[R_PSR] >> 15 && (addr_2 < 0x3000 || addr_2 >= 0xFE00)) {
        std::cout << "--- Access violation ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
    mem[addr_2] = reg[sr];
}

void virtual_machine_tp::VM_STR(int16_t inst) {
    // TO BE DONE
    int16_t sr = (inst >> 9) & 0x7;
    int16_t br = (inst >> 6) & 0x7;
    int16_t offset = SignExtend<int16_t,6>(inst & 0x3F);
    int16_t addr = reg[br] + offset;
    if (reg[R_PSR] >> 15 && (addr < 0x3000 || addr >= 0xFE00)) {
        std::cout << "--- Access violation ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
    mem[addr] = reg[sr];
}

void virtual_machine_tp::VM_TRAP(int16_t inst) {
    int trap_num = inst & 0xFF;
    // if (trap_num == 0x25)
    //     exit(0);
    // TODO: build trap program
    char temp;
    if(trap_num == 0x20)
    {
        // GETC
        std::cin >> temp;
        reg[R_R0] = temp;
    }
    else if(trap_num == 0x21)
    {
        // OUT
        temp = reg[R_R0] & 0xFF;// 取R0的第八位作为ASCII码输出
        std::cout << temp;
    }
    else if(trap_num == 0x22)
    {
        // PUTS
        int16_t address = reg[R_R0];
        while((temp = (mem[address++] & 0xFF)) != 0)
        {
            std::cout << temp;
        }
        if (gIsDetailedMode) {
            std::cout << std::endl;
        }
        //std::cout << std::endl;
    }
    else if(trap_num == 0x23)
    {
        // IN
        std::cout << "Input a character> ";
        std::cin >> temp;
        std::cout << temp << std::endl;
        reg[R_R0] = temp & 0xFF;
    }
    else if(trap_num == 0x24)
    {
        // PUTSP
        // 与PUTS基本相同，不同的是一个存储单元中存储了两个字符
        int16_t address = reg[R_R0];
        while((temp = (mem[address++] & 0xFF)) != 0)
        {
            std::cout << temp;
            if((temp = (mem[address] >> 8) & 0xFF))
            {
                std::cout<< temp ;
            }
        }
        if (gIsDetailedMode) {
            std::cout << std::endl;
        }
        //std::cout << std::endl;
    }
    else if(trap_num == 0x25)
    {
        // HALT，此时直接打印结束信息并退出程序即可
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
}

virtual_machine_tp::virtual_machine_tp(const int16_t address, const std::string &memfile, const std::string &regfile) {
    // Read memory
    if (memfile != ""){
        mem.ReadMemoryFromFile(memfile);
    }
    
    // Read registers
    std::ifstream input_file;
    input_file.open(regfile);
    if (input_file.is_open()) {
        int line_count = std::count(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>(), '\n');
        input_file.close();
        input_file.open(regfile);
        if (line_count >= 8) {
            for (int index = R_R0; index <= R_R7; ++index) {
                input_file >> reg[index];
            }
        } else {
            for (int index = R_R0; index <= R_R7; ++index) {
                reg[index] = 0;
            }
        }
        input_file.close();
    } else {
        for (int index = R_R0; index <= R_R7; ++index) {
            reg[index] = 0;
        }
    }

    // Set address
    reg[R_PC] = address;
    reg[R_COND] = 0;
}

void virtual_machine_tp::SetReg(const register_tp &new_reg) {
    reg = new_reg;
}

int16_t virtual_machine_tp::NextStep() {
    int16_t current_pc = reg[R_PC];
    reg[R_PC]++;
    if (((reg[R_PSR] >> 15) & 1) && (current_pc < 0x3000 || current_pc >= 0xFE00)) {
        std::cout << "--- Access Violation ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
    }
    int16_t current_instruct = mem[current_pc];
    int opcode = (current_instruct >> 12) & 15;
    
    switch (opcode) {
        case 1101:
        // 考虑opcode非法的情况
        std::cout << "--- Illegal opcode exception ---" << std::endl;
        std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
        exit(0);
        break;
        case O_ADD:
        if (gIsDetailedMode) {
            std::cout << "ADD" << std::endl;
        }
        if (((current_instruct >> 5) & 1 == 0) && (current_instruct >> 3) & 0x3 != 0) {
            std::cout << "--- Illegal opcode exception ---" << std::endl;
            std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
            exit(0);
        }
        VM_ADD(current_instruct);
        break;
        case O_AND:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "AND" << std::endl;
        }
        if ((current_instruct >> 5) & 1 == 0 && (current_instruct >> 3) & 0x3 != 0) {
            std::cout << "--- Illegal opcode exception ---" << std::endl;
            std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
            exit(0);
        }
        VM_AND(current_instruct);
        break;
        case O_BR:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "BR" << std::endl;
        }
        VM_BR(current_instruct);
        break;
        case O_JMP:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "JMP" << std::endl;
        }
        if (current_instruct & 0x3F || (current_instruct >> 9) & 0x7) {
            std::cout << "--- Illegal opcode exception ---" << std::endl;
            std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
            exit(0);
        }
        VM_JMP(current_instruct);
        break;
        case O_JSR:
        // TO BE DONE
        if (gIsDetailedMode) {
            if((current_instruct >> 11) & 1)
                std::cout << "JSR" << std::endl;
            else
                std::cout << "JSRR" << std::endl;
        }
        if (((current_instruct >> 11) & 1 == 0) && ((current_instruct & 0x3F) || ((current_instruct >> 9) &0x3)) ) {
            std::cout << "--- Illegal opcode exception ---" << std::endl;
            std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
            exit(0);
        }
        VM_JSR(current_instruct);
        break;
        case O_LD:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "LD" << std::endl;
        }
        VM_LD(current_instruct);
        break;
        case O_LDI:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "LDI" << std::endl;
        }
        VM_LDI(current_instruct);
        break;
        case O_LDR:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "LDR" << std::endl;
        }
        VM_LDR(current_instruct);
        break;
        case O_LEA:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "LEA" << std::endl;
        }
        VM_LEA(current_instruct);
        break;
        case O_NOT:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "NOT" << std::endl;
        }
        if (current_instruct & 0x3F != 0x3F) {
            std::cout << "--- Illegal opcode exception ---" << std::endl;
            std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
            exit(0);
        }
        VM_NOT(current_instruct);
        break;
        case O_RTI:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "RTI" << std::endl;
        }
        if (reg[R_PSR] >> 15) {
            // 不能在User mode 下使用RTI指令
            std::cout << "--- Privilege mode violation ---" << std::endl;
            std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
            exit(0);
        }
        if (current_instruct & 0xFFF) {
            std::cout << "--- Illegal opcode exception ---" << std::endl;
            std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
            exit(0);
        }
        VM_RTI(current_instruct);
        break;
        case O_ST:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "ST" << std::endl;
        }
        VM_ST(current_instruct);
        break;
        case O_STI:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "STI" << std::endl;
        }
        VM_STI(current_instruct);
        break;
        case O_STR:
        // TO BE DONE
        if (gIsDetailedMode) {
            std::cout << "STR" << std::endl;
        }
        VM_STR(current_instruct);
        break;
        case O_TRAP:
        if (gIsDetailedMode) {
            switch (current_instruct & 0xFF){
                case 0x20:  std::cout << "GETC" << std::endl; break;
                case 0x21:  std::cout << "OUT" << std::endl; break;
                case 0x22:  std::cout << "PUTS" << std::endl; break;
                case 0x23:  std::cout << "IN" << std::endl; break;
                case 0x24:  std::cout << "PUTSP" << std::endl; break;
                case 0x25:  std::cout << "HALT" << std::endl; break;
            }
            //std::cout << "TRAP" << std::endl;
        }
	    if ((current_instruct >> 8) & 0xF) {
            std::cout << "--- Illegal opcode ---" << std::endl;
            std::cout << std::endl << "--- Halting the LC-3 ---" << std::endl;
            exit(0);
        }
        if ((current_instruct & 0xFF) == 0x25) {
            reg[R_PC] = 0;
        }
        VM_TRAP(current_instruct);
        break;
        default:
        VM_RTI(current_instruct);
        break;       
    }

    if (current_instruct == 0) {
        // END
        // TODO: add more detailed judge information
        //std::cout << "--- Halting the LC-3 ---" << std::endl;
        return 0;
    }
    return reg[R_PC];
}

void virtual_machine_tp::ClearMem() {
    for(int i = 0;i < 0xFFFF;i++)
        mem[i] = 0;
}

void virtual_machine_tp::ShowMem(int begin, int end) {
    for(;begin <= end;begin++)
    {
	    std::cout << "Address: x" << std::setw(4) << std::setfill('0') << std::hex << begin;
        std::cout << " Value: x" << std::setw(4) << std::setfill('0') << std::hex << mem[begin] << "/b" << std::bitset<16>(mem[begin]) <<std::endl;
    }
}

void virtual_machine_tp::ChangeMem(int address, int16_t value) {
    mem[address] = value;
}

void virtual_machine_tp::ChangeReg(int x,int16_t value)
{
    reg[x] = value;
}

void virtual_machine_tp::ShowReg()
{
    std::cout << "\e[1mR0\e[0m = " << std::setw(4) << std::setfill('0') << std::hex << reg[R_R0] << ", ";
    std::cout << "\e[1mR1\e[0m = " << std::setw(4) << std::setfill('0') << std::hex << reg[R_R1] << ", ";
    std::cout << "\e[1mR2\e[0m = " << std::setw(4) << std::setfill('0') << std::hex << reg[R_R2] << ", ";
    std::cout << "\e[1mR3\e[0m = " << std::setw(4) << std::setfill('0') << std::hex << reg[R_R3] << std::endl;
    std::cout << "\e[1mR4\e[0m = " << std::setw(4) << std::setfill('0') << std::hex << reg[R_R4] << ", ";
    std::cout << "\e[1mR5\e[0m = " << std::setw(4) << std::setfill('0') << std::hex << reg[R_R5] << ", ";
    std::cout << "\e[1mR6\e[0m = " << std::setw(4) << std::setfill('0') << std::hex << reg[R_R6] << ", ";
    std::cout << "\e[1mR7\e[0m = " << std::setw(4) << std::setfill('0') << std::hex << reg[R_R7] << std::endl;
    std::cout << "\e[1mCOND[NZP]\e[0m = " << std::bitset<3>(reg[R_COND]) << std::endl;
    std::cout << "\e[1mPC\e[0m = " << std::setw(4) << std::setfill('0') << std::hex << reg[R_PC] << std::endl;
}

void virtual_machine_tp::ClearReg()
{
    reg[R_R0] = 0;
    reg[R_R1] = 0;
    reg[R_R2] = 0;
    reg[R_R3] = 0;
    reg[R_R4] = 0;
    reg[R_R5] = 0;
    reg[R_R6] = 0;
    reg[R_R7] = 0;
    reg[R_PC] = 0x3000;
    reg[R_COND] = 2;
    reg[R_PSR] = 0x8002;
}

void virtual_machine_tp::ShowMode()
{
    std::cout << "Current mode: ";
    if (reg[R_PSR] >> 15) {
        std::cout << "User mode" << std::endl;
    } else {
        std::cout << "Supervisor mode" << std::endl;
    }
}

void virtual_machine_tp::ChangeMode()
{
    std::cout << "You have successfully changed from ";
    if (reg[R_PSR] >> 15) {
        std::cout << "User mode to Supervisor mode" << std::endl;
        reg[R_PSR] &= 0x7FFF; // 最高位清零，后面的保持不变
    } else {
        std::cout << "Supervisor mode to User mode" << std::endl;
        reg[R_PSR] |= 0x8000; // 最高位置1，后面的保持不变
    }
}

} // namespace virtual_machine_nsp
