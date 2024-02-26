/*
 * @Author       : Chivier Humber
 * @Date         : 2021-09-14 21:41:49
 * @LastEditors  : Chivier Humber
 * @LastEditTime : 2021-11-24 05:33:45
 * @Description  : file content
 */
#include "simulator.h"

using namespace virtual_machine_nsp;
namespace po = boost::program_options;

bool gIsSingleStepMode = false;
bool gIsDetailedMode = false;
std::string gInputFileName = "input.txt";
std::string gRegisterStatusFileName = "register.txt";
std::string gOutputFileName = "";
int gBeginningAddress = 0x3000;

void menu(virtual_machine_tp virtual_machine);

int main(int argc, char **argv) {
    po::options_description desc{"\e[1mLC3 SIMULATOR\e[0m\n\n\e[1mOptions\e[0m"};
    desc.add_options()                                                                             //
        ("help,h", "Help screen")                                                                  //
        ("file,f", po::value<std::string>()->default_value("input.txt"), "Input file")             //
        ("register,r", po::value<std::string>()->default_value("register.txt"), "Register Status") //
        ("single,s", "Single Step Mode")                                                           //
        ("begin,b", po::value<int>()->default_value(0x3000), "Begin address (0x3000)")
        ("output,o", po::value<std::string>()->default_value(""), "Output file")
        ("detail,d", "Detailed Mode");

    po::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }
    if (vm.count("file")) {
        gInputFileName = vm["file"].as<std::string>();
    }
    if (vm.count("register")) {
        gRegisterStatusFileName = vm["register"].as<std::string>();
    }
    if (vm.count("single")) {
        gIsSingleStepMode = true;
    }
    if (vm.count("begin")) {
        gBeginningAddress = vm["begin"].as<int>();
    }
    if (vm.count("output")) {
        gOutputFileName = vm["output"].as<std::string>();
    }
    if (vm.count("detail")) {
        gIsDetailedMode = true;
    }

    virtual_machine_tp virtual_machine(gBeginningAddress, gInputFileName, gRegisterStatusFileName);
    int halt_flag = true;
    int time_flag = 0;
    char c;
    virtual_machine.ClearReg();
    while(halt_flag) {
        // Single step
        // TO BE DONE
        if (virtual_machine.NextStep() == 0)
            halt_flag = false; //如果后面没有要执行的程序, 就可以结束了
        if (gIsDetailedMode)
            //std::cout << virtual_machine.reg << std::endl;
	        virtual_machine.ShowReg();
        if (gIsSingleStepMode) 
        {
            std::cout << "Do you want to print menu?[y/...]" << std::endl;
            //c = getchar();
	        std::cin >> c;
            if( c == 'y')
                menu(virtual_machine);
            std::cout << "Enter any key to continue..." << std::endl;
	        //std::cout << "If you want to exit SingleStep mode, please enter 'e'" << std::endl;
		    //c =  getchar();
		    //std::cin >> c;
	        //if (c == 'e')
		      //  gIsSingleStepMode = false;
       	}
        ++time_flag;
    }

    std::cout << virtual_machine.reg << std::endl;
    std::cout << "cycle = " << time_flag << std::endl;
    return 0;
}

void menu(virtual_machine_tp virtual_machine)
{
    std::cout << "Please choose the function:" << std::endl;
    std::cout << "0. Clear the memory" << std::endl;
    std::cout << "1. Show the value in memory" << std::endl;
    std::cout << "2. Change the value in memory" << std::endl;
    std::cout << "3. Show the value in register" << std::endl;
    std::cout << "4. Change the value in register" << std::endl;
    std::cout << "5. Show your mode" << std::endl;
    std::cout << "6. Change your mode" << std::endl;
    std::cout << "7. Exit SingleStep mode" << std::endl;
    std::cout << "8. Exit" << std::endl;
    std::cout << "Your choice: ";
    int choice;
    int16_t value;
    char c;
    std::cin >> choice;
    switch (choice)
    {
        case 0:
            virtual_machine.ClearMem();
            break;
        case 1:
            unsigned int begin, end;
            std::cout << "Enter the beginning address in hexadecimal: ";
            std::cin >> std::hex >> begin;
            std::cout << "Enter the end address in hexadecimal: ";
            std::cin >> std::hex >> end;
            virtual_machine.ShowMem(begin, end);
            break;
        case 2:
            unsigned int address;
            //int value;
            std::cout << "Enter the address in hexadecimal: ";
            std::cin >> std::hex >> address;
            std::cout << "Press 'd' to enter the value in decimal or 'h' to enter the value in hexadecimal: " << std::endl;
            std::cin >> c;
            if(c == 'd')
            {
		    std::cin >> std::dec >> value;
            }
            else if(c == 'h')
            {
                std::cin >> std::hex >> value;
            }
            else
            {
                std::cout << "Invalid input!" << std::endl;
                menu(virtual_machine);
                return ;
            }
            virtual_machine.ChangeMem(address, value);
            break;
        case 3:
		    virtual_machine.ShowReg();
            break;
        case 4:
            int reg;
            std::cout << "Enter the register number: ";
            std::cin >> reg;
            std::cout << "Press 'd' to enter the value in decimal or 'h' to enter the value in hexadecimal: " << std::endl;
            std::cin >> c;
            if(c == 'd')
            {
                std::cin >> std::dec >> value;
            }
            else if(c == 'h')
            {
                std::cin >> std::hex >> value;
            }
            else
            {
                std::cout << "Invalid input!" << std::endl;
                menu(virtual_machine);
                return ;
            }
            virtual_machine.ChangeReg(reg, value);    
            break;
        case 5:
            virtual_machine.ShowMode();
            break;
        case 6:
            virtual_machine.ChangeMode();
            break;
        case 7:
            gIsSingleStepMode = false;
            break;
        case 8:
            return ;
            break;
        default:
            std::cout << "Invalid input!" << std::endl;
            menu(virtual_machine);
	    return ;
    }
    std::cout << "Do you want to show this menu again?[y/...]" << std::endl;
    std::cin >> c;
    if(c == 'y')
        menu(virtual_machine);
}
