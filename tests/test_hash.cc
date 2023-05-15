#include "sylar/util/hash_util.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    std::string data = "Test Message\n";
    std::string rst = sylar::sha1sum(data);

    std::cout.setf(std::ios::hex);
    std::cout.width(2);
    std::cout.fill('0');
    std::cout << rst << std::endl;
    const char* md_value = rst.c_str();
    uint32_t md_len = strlen(md_value);
    printf("Digest is: ");
    for (uint32_t i = 0; i < md_len; i++)
        printf("%02x", md_value[i]);
    printf("\n");

    return 0;
}