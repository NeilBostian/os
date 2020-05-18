#pragma once

#define STACK_SIZE 1024 * 64

class Stack
{
public:
    static void *Top;
    static void *Bottom;
    static void *GetTopAddress();
    static void *GetBottomAddress();
};
