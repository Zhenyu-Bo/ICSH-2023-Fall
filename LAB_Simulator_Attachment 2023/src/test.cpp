#include <iostream>
#include <iomanip>

using namespace std;
template <typename T, unsigned B>
T SignExtend(const T x) {
    T mask = 1 << (B-1); // mask初始值设为最高位为1的B位宽的数值
    T temp = x;
    // 若x的首位为1，则需要在前面补1，如果是0，则不需要处理，因为在内存中会自动填充0
    T temp_out = mask & x;
    if((mask & x) != 0)
    {
        while(mask) // 循环至mask为0结束，此时x前的所有位都填充了1
        {
            mask = mask << 1;// mask先左移一位
            temp |= mask;// 将temp的指定位变为1
        }
    }
    return temp;
}

int main()
{
    int16_t mem[0xFFFF] = {0};
    int16_t value;
    unsigned int address;
    std::cin >> std::hex >> address;
    std::cin >> std::dec >> value;
    mem[address] = value;
    std::cout << mem[address] << std::endl;
}