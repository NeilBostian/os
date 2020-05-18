#include <Stack.h>
#include <Types.h>

__attribute__((aligned(4096))) static uint8 stack[STACK_SIZE];
void *Stack::Top = stack + STACK_SIZE;
void *Stack::Bottom = stack;

void *Stack::GetBottomAddress()
{
    return Stack::Bottom;
}

void *Stack::GetTopAddress()
{
    return Stack::Top;
}
