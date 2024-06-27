#include "parser.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "usbd_cdc_if.h"
#include "flash.h"

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */


static uint8_t compile_date[] = __DATE__;
static uint8_t compile_time[] = __TIME__;

const uint32_t ident_product[8] __attribute__((section(".ARM.__AT_0x0800F000"))) = {0xA5,0xA5,0xA5,0xA5,0x5A,0x5A,0x5A,0x5A};
const uint8_t  ident_str[16] 		__attribute__((section(".ARM.__AT_0x08008000"))) = {0xA5,0xA5,0xA5,0xA5,0x5A,0x5A,0x5A,0x5A};
const uint8_t  version[16]   		__attribute__((section(".ARM.__AT_0x08008010"))) = {0xA5,0xA5,0xA5,0xA5,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5A,0x5A,0x5A,0x5A};

uint8_t  	BOOT_INT[8] = {0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5}; //即将写入到内存的数据
uint32_t 	BOOT_ADDR   = ((uint32_t)0x08020000);														//这是第5扇区的开始地址



char  *care_str[] =
{
    "CareLITE",
    "1.90",
    "OS-RTX V5.5.2",
    "CMSIS V5.7.0",
    "DS18B20",
    "stm32f401ccu6",
};


gpib_config_t cur_cmd, gpib_cmd;
gpib_config_t auto_cmd[MAX_INS];		//队列中
uint8_t 			source_device = 0;
uint8_t 			source_cmd = 0;

uint8_t 	talking;
uint32_t 	timeout = 0;
uint32_t 	query_time = 0;
uint8_t 	param_change_flage = 0;
extern 		gpib_config_struct gpib_config_saved;
extern 		int32_t ds18b20_temp;

//typedef union
//{
//  struct
//  {
//    uint32_t addr:5;
//    uint32_t read_after_write:1;
//    uint32_t eoi:1;
//    uint32_t eos:4;
//    uint32_t eot:1;
//    uint32_t eot_char:8;
//    uint32_t timeout_ms:16;
//    uint32_t srq_status:1;
//    uint32_t debug:1;
//    uint32_t state:1;
//    uint32_t delay:16;
//    uint32_t ms_count:16;
//    uint32_t error:8;
//    uint32_t count:32;
//    uint32_t done:32;
//    uint32_t stop:1;
//    uint32_t listen_only:1;
//    uint32_t is_controller:1;
//    uint32_t cmd_index:8;
//    uint32_t is_controller:1;
//    uint32_t eot_int:4;
//    uint32_t toc:4;
//    uint32_t args:16;
//    uint32_t online:1;

//  } b;
//  uint32_t c;
//  uint32_t d;
//  uint32_t e;
//  uint32_t f;
//  uint32_t c;
//  uint32_t d;
//  uint32_t e;
//  uint32_t f;
//} CONF_SAVE_Type;

char *command_str[] =
{
    "++addr",
    "++auto",
    "++clr",
    "++eoi",
    "++eos",
    "++eot_enable",
    "++eot_char",
    "++ifc",
    "++llo",
    "++loc",
    "++lon",//0x0A
    "++mode",
    "++read",
    "++read_tmo_ms",
    "++rst",
    "++savecfg",
    "++spoll",
    "++srq",
    "++status",
    "++trg",
    "++ver",
    "++help",
    "++debug",
    "++delay",	//延时读取
    "++stop",		//auto_cmd 停止
    "++temp",
    "++count",	//26
    "++online",
    "CARE?",
    "CARE_TEMP?",
    "CARE_TEMP1?",
    "CARE_TEMP2?",
    "CARE_TIMEOUT",
    "CARE_TIMEOUT?",
    "CARE_LOC",
    "CARE_ADDR",
    "CARE_NULL",
    "CARE_READ",
    "CARE_ADDR?",
    "CARE_AUTO",
    "++info",
    "CARE_INFO?",
    "bootloader",
    //CARE_TIME?
};


uint8_t care_protocol(uint8_t device, char *bytes, uint8_t length);
void return_care(char *source_bytes, char *bytes, uint8_t length);




/*
*********************************************************************************************************
*	函 数 名: JumpToBootloader
*	功能说明: 跳转到系统BootLoader
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void JumpToBootloader(void)
{
    uint32_t i=0;
    void (*SysMemBootJump)(void);        /* 声明一个函数指针 */
    __IO uint32_t BootAddr = 0x8000000;  /* BOOT 地址 */

    /* 关闭全局中断 */
    DISABLE_INT();

    /* 关闭滴答定时器，复位到默认值 */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    /* 设置所有时钟到默认状态，使用HSI时钟 */
    HAL_RCC_DeInit();

    /* 关闭所有中断，清除所有中断挂起标志 */
    for (i = 0; i < 8; i++)
    {
        NVIC->ICER[i]=0xFFFFFFFF;
        NVIC->ICPR[i]=0xFFFFFFFF;
    }

    /* 使能全局中断 */
    //ENABLE_INT();

    /* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
    SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

    /* 设置主堆栈指针 */
    __set_MSP(*(uint32_t *)BootAddr);

    /* 在RTOS工程，这条语句很重要，设置为特权级模式，使用MSP指针 */
    __set_CONTROL(0);

    /* 跳转到系统BootLoader */
    SysMemBootJump();

    /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
    while (1)
    {

    }
}

uint8_t check_auto_cmd(uint8_t addr, uint8_t flag)//检查列表地址
{
    uint8_t i;
    if(flag == 1)
    {
        for(i = 0; i < MAX_INS; i++)
        {
            if(auto_cmd[i].addr == addr) return (i + 1);
        }
    }
    else
    {
        for(i = 0; i < MAX_INS; i++)
        {
            if(auto_cmd[i].addr == 0) return (i + 1);
        }
    }

    if(i == MAX_INS) return 0;
    return 0;
}


uint8_t query_auto_cmd(void)//查询列表地址，count计数值增加，并且给出优先级最高的cmd（按照count最大值）
{
    uint8_t i, query_addr = 0;
    uint16_t d_value = 0;

    query_addr = 0;
    for(i = 0; i < MAX_INS; i++)
    {
        if((auto_cmd[i].addr > 0) && (auto_cmd[i].count - auto_cmd[i].done > 0)  && (auto_cmd[i].count > 0))//addr存在，需要发送大于0，没有完成发送次数
        {
            auto_cmd[i].ms_count += query_time + 1; 													//定时器计数值

            if(auto_cmd[i].ms_count >= auto_cmd[i].read_delay)
            {
                if (d_value < (auto_cmd[i].ms_count - auto_cmd[i].read_delay))//找时间差值最大的
                {
                    d_value = (auto_cmd[i].ms_count - auto_cmd[i].read_delay);
                    query_addr = i + 1;
                }
            }
        }
    }
    if((query_addr >= 1) && ((query_addr < MAX_INS)))
    {
        auto_cmd[query_addr - 1].done += 1;
        auto_cmd[query_addr - 1].ms_count = 0; //计数值清理
        query_time = 0;
        if(auto_cmd[query_addr - 1].count == auto_cmd[query_addr - 1].done)			//发送次数
        {
            auto_cmd[query_addr - 1].count = 0;
            auto_cmd[query_addr - 1].done = 0;
            auto_cmd[query_addr - 1].read_delay = 0;
        }

        return query_addr;
    }
    else return MAX_INS;
}



uint8_t strnicmp (const char * str1, const char * str2, uint8_t n)
{
    uint8_t i;
    for(i = 0; i < n; i++) //去掉结尾的'\n'
    {
        //if (toupper(str1[i]) != str2[i])
        if (str1[i] != str2[i])
        {
            break;
        }
    }
    if (i == (n))
    {
        return i;
    }
    else
    {
        return 0;
    }
}


enum command_t check_cmd(char *str)
{
    uint8_t i, len;
    char str_cmd[CMD_LEN];

    memset(str_cmd, 0, CMD_LEN);
    memcpy(str_cmd, str, strlen(str));

    for(i = 0; i < CMD_LEN; i++)
    {
        if((str_cmd[i] == ' ') || (str_cmd[i] == ':'))
        {
            str_cmd[i] = '\0';
            break;
        }
    }

    len = strlen(str_cmd);

    for(i = 0; i < CMD_INPUTABLE; i++)
    {
        if(len == strlen(command_str[i]))
        {
            if(strnicmp(str, command_str[i], strlen(command_str[i])) > 0)
            {
                return i;
            }
        }

    }
    if(i == CMD_INPUTABLE) return CMD_ERROR;
    return CMD_ERROR;
}


int check_arg(char *str, uint8_t len)
{
    int res;
    char *end;
//    str[len] = '\0';
    res = strtol(str, &end, 0);
//    if((((char *)end) - str) != len) {
//        return ARG_ERR;
//    }
    return res;
}



static void force_end_talking()
{
    if(talking) {
        talking = FALSE;
//    print_terminator(gpib_write_auto_eoi);
    }
}


void TransmissionT_Data(uint8_t *p_data, uint32_t size)
{
    while (CDC_Transmit_FS(p_data, size) != USBD_OK);
}



void add_terminator(char* str, uint8_t eot_int)
{
    uint8_t	len=0;
    len = strlen(str);
    switch (eot_int)
    {
    case 0:
        str[len] = 0x00;
        len += 0;
        break;
    case 1:
        str[len]   = 13;
        str[len+1] = 0x00;
        len += 1;
        break;
    case 2:
        str[len]   = 10;
        str[len+1] = 0x00;
        len += 1;
        break;
    case 3:
        str[len]   = 13;
        str[len+1] = 10;
        str[len+2] = 0x00;
        len += 2;
        break;
    default:
        str[len] = 0x00;
        len += 0;
        break;
    }
}

static void print_str(char* str, uint8_t eos_code)
{
    uint8_t	len=0;
    static uint8_t temp_str[CMD_LEN];

    memcpy(temp_str, str, CMD_LEN);
    add_terminator((char*)temp_str, eos_code);
    TransmissionT_Data(temp_str, strlen((char*)temp_str));
}


void dump_config()
{
    print_1arg(CMD_ADDR, cur_cmd.addr);
    print_1arg(CMD_AUTO, cur_cmd.read_after_write);
    print_1arg(CMD_EOI, cur_cmd.eoi);
    print_1arg(CMD_EOS, cur_cmd.eos);
    print_1arg(CMD_EOT_ENABLE, cur_cmd.eot);
    print_1arg(CMD_EOT_CHAR, (uint8_t)(cur_cmd.eot_char));
    print_1arg(CMD_LON, care.listen_only);
    print_1arg(CMD_MODE, care.is_controller);
    print_1arg(CMD_READ_TMO_MS, cur_cmd.timeout_max);
    print_1arg(CMD_STATUS, cur_cmd.status);
}


void print_char(enum command_t cmd, uint8_t arg1)
{
    char t[CMD_LEN];
    if(care.debug)
    {
//    print_header();
//    write_func(command_str[cmd], strlen(command_str[cmd]));
//    print_space();
    }

    TransmissionT_Data((uint8_t *)command_str[cmd], strlen(command_str[cmd]));
    sprintf(t, " %d", arg1);
    print_str(t, gpib_cmd.eot_int);
}

static void print_1arg(enum command_t cmd, uint32_t arg1)
{
    char s[40];
    if(care.debug)
    {
//    print_header();
//    write_func(command_str[cmd], strlen(command_str[cmd]));
//    print_space();
    }
    TransmissionT_Data((uint8_t *)command_str[cmd], strlen(command_str[cmd]));
    sprintf(s, " %d", arg1);
    print_str(s, gpib_cmd.eot_int);
}

void run_command(gpib_config_t *cmd)
{
    uint8_t i, auto_addr = 0;
    gpib_config_t last_cmd;
    char t[CMD_LEN];
    uint32_t tick;
    memset(t, 0, CMD_LEN);
    switch(cmd->cmd_index)
    {
    case CMD_ADDR:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 2); //++addr 2   +为空格
        if((cmd->args != -1) && (cmd->args <= 32))
            cur_cmd.addr = cmd->args;

        gpib_config_saved.addr = cur_cmd.addr;
        print_char(cmd->cmd_index, cur_cmd.addr);
        break;
    case CMD_DELAY:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 4); //++addr 2   +为空格
        if((cmd->args != -1) && (cmd->args <= 50000))
            cur_cmd.read_delay = cmd->args;
        print_1arg(cmd->cmd_index, cur_cmd.read_delay);
        break;
    case CMD_AUTO:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 1); //++auto 1   +为空格
        if((cmd->args != -1) && (cmd->args <= 1))
            cur_cmd.read_after_write = cmd->args;
        print_char(cmd->cmd_index, cur_cmd.read_after_write);
        break;
    case CMD_CARE_AUTO:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 1); //++auto 1   +为空格
        if((cmd->args != -1) && (cmd->args <= 1))
            cur_cmd.read_after_write = cmd->args;
        sprintf(t, "CARE_AUTO:%d", cur_cmd.read_after_write);
        print_str(t, cmd->eot_int);
        break;
    case CMD_CLR:
        if(care.is_controller)
        {
            force_end_talking();
        }
        break;
    case CMD_EOI:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 1); //++eoi 1   	+为空格
        if((cmd->args != -1) && (cmd->args <= 1))
            cur_cmd.eoi = cmd->args;
        print_char(cmd->cmd_index, cur_cmd.read_after_write);
        break;
    case CMD_EOS:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 1); //++eos 1   	+为空格
        if((cmd->args != -1) && (cmd->args <= 1))
            cur_cmd.eos = cmd->args;
        print_char(cmd->cmd_index, cur_cmd.eos);
        break;
    case CMD_EOT_ENABLE:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 1); //++eot_enable 1   	+为空格
        if((cmd->args != -1) && (cmd->args <= 1))
            cur_cmd.eot = cmd->args;
        print_char(cmd->cmd_index, cur_cmd.eot);
        break;
    case CMD_EOT_CHAR:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 2); //++eot_char 10   	+为空格
        if((cmd->args != -1) && (cmd->args <= 0x7F))
            cur_cmd.eot_char = cmd->args;
        print_char(cmd->cmd_index, cur_cmd.eot_char);
        break;
    case CMD_INFO:
    case CMD_CARE_INFO:
        print_str(care_str[0], cmd->eot_int);
        print_str(care_str[1], cmd->eot_int);
        print_str(care_str[2], cmd->eot_int);
        print_str(care_str[3], cmd->eot_int);
        print_str(care_str[4], cmd->eot_int);
        print_str(care_str[5], cmd->eot_int);
        print_str((char*)&compile_date, cmd->eot_int);
        print_str((char*)&compile_time, cmd->eot_int);
        break;
    case CMD_IFC:																										//++ifc 1   	+为空格
        if(care.is_controller)
        {
            //force_end_talking();
            gpib_uniline(GPIB_UNI_BUS_CLEAR_START);
            wait_ms(1);
            gpib_uniline(GPIB_UNI_BUS_CLEAR_END);
        }
        break;
    case CMD_LLO:																										//++llo 1   	+为空格
        if(care.is_controller)
        {
            //force_end_talking();
            send_gpib_cmd(GPIB_CMD_LLO, cur_cmd.addr);
        }
        break;
    case CMD_LOC:
        if(care.is_controller)
        {
//        force_end_talking();
            send_gpib_cmd(GPIB_CMD_GTL, cur_cmd.addr);
        }
        break;
    case CMD_STOP:																										//++llo 1   	+为空格
        if(care.is_controller)
        {
            cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 2); //++addr 2   +为空格
            if((cmd->args > 0) && (cmd->args <= 32))//有地址参数
            {
                auto_addr = check_auto_cmd(cmd->args, 1);
            }
            else//无地址参数
            {
                auto_addr = check_auto_cmd(cur_cmd.addr, 1);
            }
            if(auto_addr > 0)
            {
                auto_cmd[auto_addr - 1].stop = 1;
            }
        }
        break;
    case CMD_COUNT:																										//++llo 1   	+为空格
        if(care.is_controller)
        {
            cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 4); //++addr 2   +为空格
            if(cmd->args > 0)
                cur_cmd.count = cmd->args;
            else
                cur_cmd.count = 0;
            print_1arg(cmd->cmd_index, cmd->args);
        }
        break;
    case CMD_LON:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 1); //++eot_enable 1   	+为空格
        if((cmd->args != -1) && (cmd->args <= 1))
            care.listen_only = cmd->args;
        //if(care.listen_only){listening_as_device = TRUE;}
        print_char(cmd->cmd_index, care.listen_only);
        break;
    case CMD_SRQ:
        if(care.is_controller)
        {
            print_char(CMD_SRQ, gpib_uniline(GPIB_UNI_CHECK_SRQ_ASSERT));
        }
        break;
    case CMD_TRG:
        if(!care.is_controller) {
            break;
        }
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 2);
        if((cmd->args != -1) && (cmd->args <= 32))
            send_gpib_cmd(GPIB_CMD_GET, cmd->args);
        break;

    case CMD_READ: // Different from Prologix impl.
    case CMD_CARE_READ:
        if(!care.is_controller) {
            break;
        }
        address_target_talk(gpib_cmd.addr);                                              //put instrument in talk
        gpib_read(gpib_cmd.eoi, gpib_cmd.eos, gpib_cmd.eot, gpib_cmd.eot_char);
//      force_end_talking();
//      gpib_uniline(GPIB_UNI_CMD_START);
//      gpib_putchar(GPIB_CMD_UNL, 0);
//      gpib_putchar(GPIB_CMD_LAD(0), 0); // listener, it's me.
//      gpib_putchar(GPIB_CMD_TAD(gpib_config.address.item[0][0]), 0); // talker
//      gpib_uniline(GPIB_UNI_CMD_END);
//      if((cmd->args > 0) && (info->arg[0] == ARG_EOI)){
//        gpib_read(push_func, GPIB_READ_UNTIL_EOI);
//      }else{
//        gpib_read(push_func, 0);
//      }
//      sys_state |= SYS_GPIB_LISTENED;
        break;
    case CMD_READ_TMO_MS:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 4);
        if((cmd->args > 0) && (cmd->args <= 9999))
            cur_cmd.timeout_max = cmd->args;
        print_1arg(CMD_READ_TMO_MS, cur_cmd.timeout_max);
        break;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    case CMD_CARE_QUERY://CARE?
        tick = osKernelGetSysTimerCount();
        sprintf(t, "+9.9999%04dE+03", (tick&0xFFFF)%10000);
        print_str(t, cmd->eot_int);
        break;

    case CMD_TEMP:
    case CMD_CARE_TEMP:
    case CMD_CARE_TEMP1:
    case CMD_CARE_TEMP2:
        sprintf(t, "%.2f", (ds18b20_temp / 100.0)); //DS18B20:
        print_str(t, cmd->eot_int);
        break;
    case CMD_CARE_TIMEOUT://CARE_TIMEOUT:1000
        cmd->args = check_arg(cmd->cmd + strlen(command_str[cmd->cmd_index]), 4);
        if((cmd->args > 0) && (cmd->args <= 9999))
            cur_cmd.timeout_max = cmd->args;
        sprintf(t, "CARE_TIMEOUT:%d", cur_cmd.timeout_max);
        print_str(t, cmd->eot_int);
        break;
    case CMD_CARE_TIMEOUT_QUERY://CARE_TIMEOUT?
        sprintf(t, "CARE_TIMEOUT:%d", cur_cmd.timeout_max);
        print_str(t, cmd->eot_int);
        break;
    case CMD_CARE_LOC://CARE_LOC
        if(care.is_controller)
        {
            send_gpib_cmd(GPIB_CMD_GTL, cur_cmd.addr);
        }
        break;
    case CMD_CARE_ADDR://CARE_ADDR:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 2);
        if((cmd->args != -1) && (cmd->args <= 32))
            cur_cmd.addr = cmd->args;
        gpib_config_saved.addr = cur_cmd.addr;
        sprintf(t, "CARE_ADDR:%d", cur_cmd.addr);
        print_str(t, cmd->eot_int);
        break;
    case CMD_CARE_ADDR_QUERY:
        sprintf(t, "CARE_ADDR:%d", cur_cmd.addr);
        print_str(t, cmd->eot_int);
        break;
    case CMD_CARE_NULL://CARE_NULL:
        memcpy(&last_cmd, &cur_cmd, sizeof(cur_cmd));
        memcpy(&cur_cmd, &gpib_config_saved, sizeof(gpib_config_saved));

        memcpy(&cur_cmd.cmd, cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), strlen(cmd->cmd + strlen(command_str[cmd->cmd_index])));
        cur_cmd.read_after_write = 0;
        memcpy(&gpib_cmd, &cur_cmd, sizeof(cur_cmd));
        gpib_execute();
        memcpy(&cur_cmd, &last_cmd, sizeof(last_cmd));
        break;
    case CMD_RST:
        NVIC_SystemReset();
        break;
    case CMD_BOOT:
        erase_flash(5, 6); //擦除第5扇区
        writedata_to_flash(BOOT_INT, 8, BOOT_ADDR);//将数组写入
        //HAL_Delay(100);
        osDelay(1000);
        NVIC_SystemReset();
				//JumpToBootloader();
        break;
    case CMD_LINE:
        cmd->args = check_arg(cmd->cmd + 1 + strlen(command_str[cmd->cmd_index]), 2); //++addr 2   +为空格
        if((cmd->args != -1) && (cmd->args != 0) && (cmd->args <= 32))
        {
            cur_cmd.addr = cmd->args;
            cur_cmd.online = address_target_state(cur_cmd.addr);
            sprintf(t, "addr:%d online:%d", cur_cmd.addr, cur_cmd.online);
            print_str(t, cmd->eot_int);
        }
        else
        {
            for(i = 0; i < MAX_INS; i++)
            {
                if (address_target_state(i+1) == 1)
                {
                    sprintf(t, "%d online", i+1);
                    print_str(t, cmd->eot_int);
                    memset(t, 0, CMD_LEN);
                }
            }
        }
        break;

    case CMD_MODE:
//      if(renew_arg0_u8(info, &gpib_config.is_controller, 1)){
//        force_end_talking();
//        gpib_io_init();
//        if(!gpib_config.is_controller){device_init();}
//      }
//      if(not_query){break;}
        print_char(CMD_MODE, care.is_controller); // return current mode
        break;

    case CMD_SAVECFG:
        // Different from Prologic command, savecfg is performing one-shot save.
//      flash_update((flash_address_t)(void __code *)&gpib_config_saved,
//          (u8 *)&gpib_config, sizeof(gpib_config));
//        if(gpib_config.debug & DEBUG_VERBOSE)
    {
        dump_config();
    }
    break;
//    case CMD_SPOLL: {
//      if(gpib_config.is_controller){
//        u8 buf[2];
//        int stb;

//        force_end_talking();
//        gpib_uniline(GPIB_UNI_CMD_START);
//        gpib_putchar(GPIB_CMD_SPE, 0);
//        memcpy(buf, gpib_config.address.item[0], sizeof(gpib_config.address.item[0]));
//        check_address(&(info->arg[0]), (u8)cmd->args, buf);
//        buf[0] = GPIB_CMD_TAD(buf[0]);
//        gpib_write(buf, (buf[1] == 0 ? 1 : 2), 0);
//        gpib_uniline(GPIB_UNI_CMD_END);

//        stb = gpib_getchar();
//        if(!GPIB_GETCHAR_IS_ERROR(stb)){
//          print_1arg(CMD_SPOLL, GPIB_GETCHAR_TO_DATA(stb));
//        }

//        buf[0] = GPIB_CMD_UNT;
//        buf[1] = GPIB_CMD_SPD;
//        gpib_uniline(GPIB_UNI_CMD_START);
//        gpib_write(buf, 2, 0);
//        gpib_uniline(GPIB_UNI_CMD_END);
//      }
//      break;
//    }

    case CMD_STATUS:
//      if(renew_arg0_u8(info, &gpib_config.status, 0xFF)){
//        if(!gpib_config.is_controller){
//          gpib_uniline((gpib_config.status & 0x40)
//              ? GPIB_UNI_SRQ_ASSERT : GPIB_UNI_SRQ_DEASSERT); // SRQ
//        }
//      }
//      if(not_query){break;}
        print_char(CMD_STATUS, cur_cmd.status);
        break;

//#define print_str(str) write_func(str, sizeof(str) - 1)
//    case CMD_VER:
//      if(gpib_config.debug & DEBUG_VERBOSE){
//        print_header();
//        write_func(command_str[CMD_VER], strlen(command_str[CMD_VER]));
//        print_space();
//      }
//      print_str("Fenrir GPIB-USB 1.0 (Prologix version 6.0 compatible)");
//      print_terminator(write_func);
//      break;
//#undef print_str
//    case CMD_HELP: {
//      u8 i;
//      if(gpib_config.debug & DEBUG_VERBOSE){
//        print_header();
//        write_func(command_str[CMD_HELP], strlen(command_str[CMD_HELP]));
//        print_space();
//      }
//      for(i = 0; i < sizeof(command_str) / sizeof(command_str[0]); ++i){
//        if(i > 0){print_space();}
//        write_func(command_str[i], strlen(command_str[i]));
//      }
//      print_terminator(write_func);
//      break;
//    }
//    case CMD_DEBUG:
//      renew_arg0_u8(info, &gpib_config.debug, 0xFF);
//      if(not_query){break;}
//      print_1arg(CMD_DEBUG, gpib_config.debug); // return current debug
//      break;
//    case CMD_TALK: {
//      static __xdata char buf;
//      if(talking){
//        if(gpib_config.debug & DEBUG_GPIB_ECHO){
//          push_func(buf); // print character to be tried to write
//        }
//        if(cmd->args == 0){ // terminator
//          if(gpib_config.eos == 3){
//            gpib_putchar(buf, gpib_config.eoi ? GPIB_WRITE_USE_EOI : 0);
//          }else{
//            gpib_putchar(buf, 0);
//            print_terminator(gpib_write_auto_eoi);
//          }
//          talking = FALSE;
//          if(gpib_config.read_after_write){
//            cmd->cmd_index = CMD_READ;
//            // cmd->args = 0;
//            run_command(info); // valid only for controller.
//          }
//        }else{
//          talking = (gpib_putchar(buf, 0) > 0);
//          buf = (u8)(info->arg[0]);
//        }
//        sys_state |= SYS_GPIB_TALKED;
//      }else if(cmd->args > 0){ // ignore terminator when not talking.
//        if(gpib_config.is_controller){ // controller
//          gpib_cmd(GPIB_CMD_TAD(0), &gpib_config.address); // talker, it's me.
//          talking = TRUE;
//        }else if(talkable_as_device){ // device
//          talking = TRUE;
//        }
//        buf = (u8)(info->arg[0]);
//      }
//      break;
    default:
        break;
    }
}





uint8_t care_protocol(uint8_t device, char *bytes, uint8_t length)
{
    uint8_t return_flag, gpib_str_state = 0;
    uint8_t gpib_address;
    gpib_config_t last_cmd;
    uint8_t len;
    uint32_t tick;
    char t[CMD_LEN];

    gpib_str_state = 0x01;
    source_device = 0;

    while ((gpib_str_state != 0))
    {
        switch(gpib_str_state)
        {
        case 0x01:
            switch ((unsigned char)bytes[3])
            {
            case 0xAA://发送地址bytes[1]GPIB仪表无返回指令
                gpib_str_state = 0x05;
                gpib_address = bytes[1] & 0x1F;
                source_device = device;
                source_cmd = (unsigned char)bytes[3];
                return_flag = 1;
                break;
            case 0xAB:
                gpib_str_state = 0x05;
                gpib_address = bytes[1] & 0x1F;
                source_cmd = (unsigned char)bytes[3];
                source_device = device;
                return_flag = 0;
                break;
            case 0xAF://读取bytes[1] GPIB仪表采集数据 增加仪表读指令，配合0xAB实现多机采集
                gpib_str_state = 0x09;
                gpib_address = bytes[1] & 0x1F;
                source_cmd = (unsigned char)bytes[3];
                source_device = device;
                return_flag = 0;
                break;
            case 0xA0://设备参数查询
                gpib_str_state = 0x10;
                break;
            case 0xB0://设备参数设置
                gpib_str_state = 0x11;
                break;
            case 0xAE://温湿度查询
                gpib_str_state = 0x40;
                break;
//            case 0xCA://读取自动采集配置
//                gpib_str_state = 0x60;
//                break;
//            case 0xCB://设置自动采集配置
//                gpib_str_state = 0x70;
//                break;
//            case 0xCC://读取自动采集数据
//                gpib_str_state = 0x80;
//                break;
            case 0xDD://模拟采集
                gpib_str_state = 0x90;
                break;
            case 0xAC://设置SCPI等待时间
                gpib_str_state = 0x92;
                break;
            case 0xAD://查询SCPI等待时间
                gpib_str_state = 0x93;
                break;
            case 0xB1://重启设备
                gpib_str_state = 0x99;
                break;
            default:
                gpib_str_state = 0x00;
                break;
            }
            break;
        case 0x05:
            memcpy(&gpib_cmd, &gpib_config_saved, sizeof(gpib_config_saved));
            memcpy(&gpib_cmd.cmd, bytes + 5, bytes[2] - 2);
            gpib_cmd.addr = gpib_address;
            gpib_cmd.read_after_write = return_flag;
            gpib_execute();
            gpib_str_state = 0;
            break;
        case 0x09:																									//GPIB是否返回仪器数据
            memcpy(&gpib_cmd, &gpib_config_saved, sizeof(gpib_config_saved));
            //memcpy(&gpib_cmd.cmd, bytes + 5, bytes[2] - 2);
            gpib_cmd.addr = gpib_address;
            //gpib_cmd.read_after_write = return_flag;
            address_target_talk(gpib_cmd.addr);												//put instrument in talk
            if(gpib_read(gpib_cmd.eoi, gpib_cmd.eos, gpib_cmd.eot, gpib_cmd.eot_char) != 0) gpib_cmd.error++;
            gpib_str_state = 0;
            break;
        case 0x10:												//08 16 07 A0 00 31 32 33 34 35
            switch ((unsigned char)bytes[4])
            {
            case 0xD0:
                gpib_str_state = 0x20;
                break;//查询CARE编译日期
            case 0xD1:
                gpib_str_state = 0x21;
                break;//查询CARE信息
            case 0xD2:
                gpib_str_state = 0x22;
                break;//查询设备版本
            case 0xD3:
                gpib_str_state = 0x23;
                break;//查询DHCP是否激活
            case 0xD4: 														 //查询CARE IP
            case 0xD5: 														 //查询CARE GW
            case 0xD6:
                gpib_str_state = 0x26;
                break;//查询CARE MSK
            case 0xD7:
                gpib_str_state = 0x27;
                break;//查询CARE TCP_PORT
            case 0xD9:
                gpib_str_state = 0x29;
                break;//查询CARE当前时间
            case 0xDA:
                gpib_str_state = 0x2A;
                break;//查询温湿度传感器类型
            case 0xDB:
                gpib_str_state = 0x2B;
                break;//查询透明协议时各端口对应GPIB地址
            case 0xDC:
                gpib_str_state = 0x2C;
                break;//查询MAC地址
            case 0xDD:
                gpib_str_state = 0x2D;
                break;//查询串口0波特率
            case 0xDE:
                gpib_str_state = 0x2E;
                break;//查询串口1波特率
            case 0xDF:
                gpib_str_state = 0x2F;
                break;//查询串口转发

            default:
                gpib_str_state = 0;
                break;
            }
            break;
        case 0x11:												//08 16 07 B0 00 31 32 33 34 35
            switch ((unsigned char)bytes[4])
            {
            case 0xD0:
                gpib_str_state = 0x30;
                break;//设置LISTEN ONLY模式
            case 0xD1:
                gpib_str_state = 0x31;
                break;//设置LISTEN ONLY模式
            case 0xD3:
                gpib_str_state = 0x33;
                break;//设置DHCP是否激活
            case 0xD4:
                gpib_str_state = 0x34;
                break;//设置CARE IP
            case 0xD5:
                gpib_str_state = 0x35;
                break;//设置CARE GW
            case 0xD6:
                gpib_str_state = 0x36;
                break;//设置CARE MSK
            case 0xD7:
                gpib_str_state = 0x37;
                break;//设置CARE TCP_PORT
            case 0xD8:
                gpib_str_state = 0x38;
                break;//care参数还原默认
            case 0xD9:
                gpib_str_state = 0x39;
                break;//设置CARE当前时间
            case 0xDF:
                gpib_str_state = 0x3F;
                break;//设置串口转发
            case 0xE0:
                gpib_str_state = 0x60;
                break;//设置GPIB透传地址
            case 0xE1:
                gpib_str_state = 0x61;
                break;//设置离线采集
            case 0xE2:
                gpib_str_state = 0x62;
                break;//设置CARE自动模式
            case 0xE3:
                gpib_str_state = 0x63;
                break;//设置wifi
            case 0xE4:
                gpib_str_state = 0x64;
                break;//还原wifi出厂设置
            case 0xE5:
                gpib_str_state = 0x65;
                break;//设置WIFI SSID
            case 0xE6:
                gpib_str_state = 0x66;
                break;//设置WIFI KEY
            case 0xE7:
                gpib_str_state = 0x67;
                break;//释放仪表到LOCAL状态
            case 0xE8:
                gpib_str_state = 0x68;
                break;//设置wifi连接远程仪表，默认0无远程仪表，支持域名解析
            case 0xE9:
                gpib_str_state = 0x69;
                break;//查询WIFI IP地址
            default:
                gpib_str_state = 0;
                break;
            }
            break;
        case 0x20:				//CARE 编译信息
            strcpy(t, (char*)compile_date);
            strcat(t, " ");
            strcat(t, (char*)compile_time);
            len = strlen((char*)t);
            return_care(bytes, t, len);
            gpib_str_state = 0;
            break;
        case 0x21:				//CARE 信息
            if((unsigned char)bytes[2] == 0x02)
            {
                strcpy(t, care_str[0]);
            }
            else if(((unsigned char)bytes[2] == 0x03) && ((unsigned char)bytes[5] <= 0x05))
            {
                strcpy(t, care_str[bytes[5]]);
            }
            len = strlen((char*)t);
            return_care(bytes, t, len);
            gpib_str_state = 0;
            break;
        case 0x22:				//CARE版本
            strcpy(t, care_str[1]);
            len = strlen((char*)t);
            return_care(bytes, t, len);
            gpib_str_state = 0;
            break;
        case 0x29:				//RTC
            sprintf(t, "%04d%02d%02d%02d%02d%02d",
                    2022,
                    2,
                    2,
                    22,
                    06,
                    16);
            return_care(bytes, t, strlen(t));
            gpib_str_state = 0;
            break;
        case 0x39:																//设置CARE当前时间
            t[0] = 0x01;
            return_care(bytes, t, 1);
            gpib_str_state = 0;
            break;
        case 0x40:
            sprintf(t, "%.2f\r\n", (ds18b20_temp / 100.0));
            return_care(bytes, t, strlen(t));
            gpib_str_state = 0;
            break;
        case 0x90://08 00 02 DD 00								//模拟数据
            tick = osKernelGetSysTimerCount();
            sprintf(t, "+9.9999%04dE+03", (tick&0xFFFF)%10000);
            return_care(bytes, t, strlen(t));
            gpib_str_state = 0;
            break;
        case 0x92://08 00 06 AC 00 00 00 00 01		//设置SCPI等待时间
            t[0] = 0x01;
            return_care(bytes, t, 1);
            gpib_str_state = 0;
            break;
        case 0x93://08 00 02 AD 00								//查询SCPI等待时间
            t[0] = 0xFF;
            t[1] = 0xFF;
            t[2] = 0xFF;
            t[3] = 0xFF;
            return_care(bytes, t, 4);
            gpib_str_state = 0;
            break;
        case 0x99://08 00 02 B1 00								//重启CARE
            NVIC_SystemReset();
            gpib_str_state = 0;
            break;
        default:
            return_care(bytes, bytes, len);
            gpib_str_state = 0;
            break;
        }
    }
    return 1;
}

void return_care(char *source_bytes, char *bytes, uint8_t length)
{   //08 16 07 B0 00 31 32 33 34 35
    uint8_t i;
    uint8_t send_char[256];

    memset(send_char, 0, 256);

    if (source_bytes[0] == 0x08)
    {
        for(i = length + 4; i >= 5; i--)
        {
            send_char[i] = bytes[i - 5];
        }
        send_char[0] = source_bytes[0] + 1;
        send_char[1] = 0;
        send_char[2] = 2 + length;
        send_char[3] = source_bytes[3];
        send_char[4] = source_bytes[4];
        length = length + 5;
    }
    else if(source_device == 0x01)
    {
        for(i = length + 4; i >= 5; i--)
        {
            send_char[i] = bytes[i - 5];
        }
        send_char[0] = 0x09;
        send_char[1] = gpib_cmd.addr;
        send_char[2] = 2 + length;
        send_char[3] = source_cmd;//0xAA,0xAB,0xAF采集指令数据返回
        send_char[4] = 0x01;
        length = length + 5;
    }
    else
    {
        for(i = 0; i < length; i++)
        {
            send_char[i] = bytes[i];
        }
    }
    source_device = 0x00;
    TransmissionT_Data(send_char, length);
}




