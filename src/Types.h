#pragma once

// Integer types
typedef unsigned char uint8;
typedef signed char int8;

typedef unsigned short uint16;
typedef signed short int16;

typedef unsigned int uint32;
typedef signed int int32;

typedef unsigned long long uint64;
typedef signed long long int64;

// String type
typedef const char *string;

class String
{
public:
    // Returns length of a string
    static uint32 Len(string str);

    // Compare two strings for equality
    static bool Cmp(string a, string b);

    // Converts `x` to a binary string, placing the result in *res.
    // *res should be a char[9].
    static void ConvertToBin(uint8 x, char *res);

    // Converts `x` to a hex string, placing the result in *res.
    // *res should be a char[3].
    static void ConvertToHex(uint8 x, char *res);

    // Converts `x` to a hex string, placing the result in *res.
    // *res should be a char[5].
    static void ConvertToHex(uint16 x, char *res);

    // Converts `x` to a hex string, placing the result in *res.
    // *res should be a char[9].
    static void ConvertToHex(uint32 x, char *res);
};
