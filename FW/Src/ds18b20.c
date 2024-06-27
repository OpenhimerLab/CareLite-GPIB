
#include "ds18b20.h"

#define SENSOR_TEMP_RANGE_MAX (125)
#define SENSOR_TEMP_RANGE_MIN (-55)

#define ds18b20_pin_read() LL_GPIO_IsInputPinSet(DS18B20_GPIO_Port,DS18B20_Pin)
#define DS18B20_CRC

int32_t ds18b20_temp;
uint8_t Deviatvalue = 0;

//void rt_hw_us_delay(uint32_t us)
//{
//    uint32_t delta;

//    us = us * (SysTick->LOAD / (1000000));
//    delta = SysTick->VAL;

//    while (delta - SysTick->VAL < us) continue;
//}

void rt_hw_us_delay(uint32_t us)
{
    uint32_t start, now, delta, reload, us_tick;
    start = SysTick->VAL;
    reload = SysTick->LOAD;
    us_tick = SystemCoreClock / 1000000UL;
    do {
        now = SysTick->VAL;
        delta = start > now ? start - now : reload + start - now;
    } while(delta < us_tick * us);
}


//void rt_hw_us_delay(uint32_t us)
//{
//    uint32_t tick,timeout,now;
//    timeout = osKernelGetSysTimerFreq() / 1000000u;
//    tick = osKernelGetSysTimerCount();
//    do
//    {
//        now = osKernelGetSysTimerCount();
//    } while ((now - tick) < us*timeout);

//    // start interacting with device
//}


//void rt_hw_us_delay(uint32_t us)
//{
//    uint32_t i=0;
//    i = 25 * us;
//    while(i--)
//    {
//        ;    //i=100 4us
//    }
//}

void ds18b20_pin_mode(uint8_t mode)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    if(PIN_MODE_INPUT == mode)
    {
        GPIO_InitStruct.Pin = DS18B20_Pin;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
        LL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);
    }
    else if(PIN_MODE_OUTPUT == mode)
    {
        GPIO_InitStruct.Pin = DS18B20_Pin;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
        LL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);
    }
}

//static uint8_t ds18b20_pin_read()
//{
//    return LL_GPIO_IsInputPinSet(GPIOB,DS18B20_Pin);
//}

void ds18b20_reset(void)
{
    ds18b20_pin_mode(PIN_MODE_OUTPUT);
    LL_GPIO_ResetOutputPin(DS18B20_GPIO_Port,	DS18B20_Pin);
    rt_hw_us_delay(780);               /* 480us - 960us */
    LL_GPIO_SetOutputPin(DS18B20_GPIO_Port,	DS18B20_Pin);
    rt_hw_us_delay(40);                /* 15us - 60us*/
}

uint8_t ds18b20_connect(void)
{
    uint8_t retry = 0;
    ds18b20_pin_mode(PIN_MODE_INPUT);

    while (ds18b20_pin_read() && retry < 200)
    {
        retry++;
        rt_hw_us_delay(1);
    };

    if(retry >= 200)
        return CONNECT_FAILED;
    else
        retry = 0;

    while (!ds18b20_pin_read() && retry < 240)
    {
        retry++;
        rt_hw_us_delay(1);
    };

    if(retry >= 240)
        return CONNECT_FAILED;

    return CONNECT_SUCCESS;
}

uint8_t ds18b20_read_bit(void)
{
    uint8_t data;

    ds18b20_pin_mode(PIN_MODE_OUTPUT);
    LL_GPIO_ResetOutputPin(DS18B20_GPIO_Port,	DS18B20_Pin);
    rt_hw_us_delay(2);
    LL_GPIO_SetOutputPin(DS18B20_GPIO_Port,	DS18B20_Pin);
    ds18b20_pin_mode(PIN_MODE_INPUT);
    /* maybe 12us, maybe 5us, whatever...I have no idea */
    rt_hw_us_delay(15);

    if(ds18b20_pin_read())
        data = 1;
    else
        data = 0;

    rt_hw_us_delay(30);

    return data;
}

uint8_t ds18b20_read_byte()
{
    uint8_t i, j, dat;
    dat = 0;

    for (i = 1; i <= 8; i++)
    {
        j = ds18b20_read_bit();
        dat = (j << 7) | (dat >> 1);
    }
    return dat;
}

void ds18b20_write_byte(uint8_t dat)
{
    uint8_t j;
    uint8_t testb;
    ds18b20_pin_mode(PIN_MODE_OUTPUT);

    for (j = 1; j <= 8; j++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;

        if(testb)
        {
            LL_GPIO_ResetOutputPin(DS18B20_GPIO_Port,	DS18B20_Pin);
            rt_hw_us_delay(2);
            LL_GPIO_SetOutputPin(DS18B20_GPIO_Port,	DS18B20_Pin);
            rt_hw_us_delay(60);
        }
        else
        {
            LL_GPIO_ResetOutputPin(DS18B20_GPIO_Port,	DS18B20_Pin);
            rt_hw_us_delay(60);
            LL_GPIO_SetOutputPin(DS18B20_GPIO_Port,	DS18B20_Pin);
            rt_hw_us_delay(2);
        }
    }
}

void ds18b20_set(void)
{
    ds18b20_reset();
    ds18b20_connect();
    ds18b20_write_byte(0xcc);  /* skip rom */
    ds18b20_write_byte(0x4e);  /* convert */
    ds18b20_write_byte(0x4e);
    ds18b20_write_byte(0x4e);
    ds18b20_write_byte(DS1820_OPTION_1|0x1F);//10bit
}

void ds18b20_start(void)
{
    ds18b20_reset();
    ds18b20_connect();
    ds18b20_write_byte(0xcc);  /* skip rom */
    ds18b20_write_byte(0x44);  /* convert */
}

uint8_t ds18b20_init(void)
{
    uint8_t ret = 0;

    ds18b20_reset();
    ret = ds18b20_connect();

    return ret;
}


uint8_t DS18B20_CaculCRC(uint8_t *_buf, uint8_t _len)
{
    uint8_t crc = 0, i, j;
    for (i = 0; i < _len; i++)
    {
        crc = crc ^ _buf[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8C;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

uint32_t last_tem;

int32_t ds18b20_get_temperature(void)
{
    uint8_t TL, TH;
    int32_t tem;

    uint8_t crc;
    uint8_t spm[10];

//    ds18b20_start();

    ds18b20_init();
    ds18b20_write_byte(0xcc);
    ds18b20_write_byte(0xbe);

    spm[0] = ds18b20_read_byte();
    spm[1] = ds18b20_read_byte();
#ifdef DS18B20_CRC
    spm[2] = ds18b20_read_byte();
    spm[3] = ds18b20_read_byte();
    spm[4] = ds18b20_read_byte();
    spm[5] = ds18b20_read_byte();
    spm[6] = ds18b20_read_byte();
    spm[7] = ds18b20_read_byte();
    spm[8] = ds18b20_read_byte();
    crc = DS18B20_CaculCRC(spm,8);
    if(crc == spm[8])
    {
#endif
        TL = spm[0];
        TH = spm[1];
        if( TH&0xfc)
        {
            tem=(TH<<8)|TL;
            tem = (~tem)+1;
            tem = tem*625/100;
        }
        else
        {
            tem = ((TH<<8)|TL)*625/100;
        }
        tem += Deviatvalue;//Ð£Õý
        last_tem = tem;
        return tem;
    }
    else if((spm[0] == 0xFF) && (spm[1] == 0xFF))
    {
        return 0;
    }
    else return last_tem;
}




