
#include "util.h"
//#include "stm32f10x.h"


void wait_8n6clk(unsigned char i)
{
    while(i--);
}

void _wait_us(unsigned int count)
{
    while(count--) wait_8n6clk(50); // 46clk + about 45clk * n
}

void wait_ms(unsigned int count)
{
    while(count--) _wait_us(1000);
}

