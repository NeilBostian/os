#include <Cpu.h>
#include <MultibootInfo.h>
#include <Stack.h>
#include <Terminal.h>
#include <Types.h>

static boot_information *boot_info;
static void print_boot_info();

extern "C" __attribute__((noreturn)) void entry(boot_information *lboot_info)
{
    // This has to be prior to changing the stack, otherwise *lboot_info will be invalid
    MultibootInfo::Initialize(lboot_info);

    // Setup stack
    asm("mov %0, %%ebp" ::"m"(Stack::Top));
    asm("mov %ebp, %esp");

    // Real code begins here
    Terminal::Clear();
    Cpu::Initialize();

    MultibootInfo::PrintHeader();
    Terminal::WriteLine();

    asm("int $0x30");

    while (1)
    {
        asm("hlt");
    }
}
