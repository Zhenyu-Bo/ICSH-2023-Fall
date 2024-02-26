/*
 * @Author       : Chivier Humber
 * @Date         : 2021-09-15 09:27:50
 * @LastEditors  : Chivier Humber
 * @LastEditTime : 2021-09-22 19:36:46
 * @Description  : file content
 */
#pragma once

#include "common.h"
#include "register.h"
#include "memory.h"

namespace virtual_machine_nsp {

enum kOpcodeList {
    O_ADD = 0b0001,
    O_AND = 0b0101,
    O_BR  = 0b0000,
    O_JMP = 0b1100,
    O_JSR = 0b0100,
    O_LD  = 0b0010,
    O_LDI = 0b1010,
    O_LDR = 0b0110,
    O_LEA = 0b1110,
    O_NOT = 0b1001,
    O_RTI = 0b1000,
    O_ST  = 0b0011,
    O_STI = 0b1011,
    O_STR = 0b0111,
    O_TRAP = 0b1111
};

enum kTrapRoutineList {
};

class virtual_machine_tp {
    public:
    register_tp reg;
    memory_tp mem;
    
    // Instructions
    void VM_ADD(int16_t inst);
    void VM_AND(int16_t inst);
    void VM_BR(int16_t inst);
    void VM_JMP(int16_t inst);
    void VM_JSR(int16_t inst);
    void VM_LD(int16_t inst);
    void VM_LDI(int16_t inst);
    void VM_LDR(int16_t inst);
    void VM_LEA(int16_t inst);
    void VM_NOT(int16_t inst);
    void VM_RTI(int16_t inst);
    void VM_ST(int16_t inst);
    void VM_STI(int16_t inst);
    void VM_STR(int16_t inst);
    void VM_TRAP(int16_t inst);

    // Managements
    virtual_machine_tp() {}
    virtual_machine_tp(const int16_t address, const std::string &memfile, const std::string &regfile);
    void UpdateCondRegister(int reg);
    void SetReg(const register_tp &new_reg);
    int16_t NextStep();
    void ClearMem(); // 清空内存
    void ShowMem(int begin, int end); // 显示内存从begin到end范围的值
    void ChangeMem(int address, int16_t value); // 改变地址为address的内存单元中的值为value
    void ChangeReg(int x, int16_t value); // 改变寄存器x中的值为value
    void ShowReg(); // 显示各寄存器的值
    void ClearReg(); // 清空寄存器，其中reg[R_COND]清空为2（010）,reg[R_PSR]清空为0x8002
    void ShowMode(); // 显示当前处于的模式（User mode/Supervisor mode）
    void ChangeMode(); // 改变当前处于的模式
};

}; // virtual machine namespace
