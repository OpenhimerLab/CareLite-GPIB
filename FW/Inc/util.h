#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>
#include <string.h>
#include <limits.h>

void wait_8n6clk(unsigned char i);
void _wait_us(unsigned int count);
void wait_ms(unsigned int count);

void delay_us(uint32_t nus);

//#define wait_us(n) { \
//  ((n) <= 100) ? wait_8n6clk((n) * 5) : _wait_us((n) - 1); \
//}

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#endif /* __UTIL_H__ */
