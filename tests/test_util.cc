#include "sylar/macro.h"



void asset_test()
{
    SYLAR_ASSERT2( 0 == 1 ,"abcdef xx");
}


int main()
{
    asset_test();
    return 0;
}