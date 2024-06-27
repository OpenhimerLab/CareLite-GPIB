#ifndef __PARSER_H__
#define __PARSER_H__

#include "stdint.h"
#include <ctype.h>
#include <string.h>
#include "gpib_io.h"
#include "gpib.h"

#include "stm32f4xx.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"


#define 	ARG_ERR 			-1
#define 	ARG_EOI 			256

#define 	CMD_LEN 			128
#define 	MAX_INS				32  //最大仪表数量
#define 	SELF_ADDR			21	//CARE地址	
#define 	GPIB_MASTER 	1

typedef enum {FALSE = 0,TRUE = 1} bool;




enum command_t {
    CMD_ADDR=0,
    CMD_AUTO,
    CMD_CLR,
    CMD_EOI,
    CMD_EOS,
    CMD_EOT_ENABLE,
    CMD_EOT_CHAR,
    CMD_IFC,
    CMD_LLO,
    CMD_LOC,
    CMD_LON,//0x0A
    CMD_MODE,
    CMD_READ,
    CMD_READ_TMO_MS,
    CMD_RST,
    CMD_SAVECFG,
    CMD_SPOLL,
    CMD_SRQ,
    CMD_STATUS,
    CMD_TRG,
    CMD_VER,
    CMD_HELP,
    CMD_DEBUG,
    CMD_DELAY,     //延时
    CMD_STOP,
    CMD_TEMP,
    CMD_COUNT,
		CMD_LINE,
    CMD_CARE_QUERY,
    CMD_CARE_TEMP,
    CMD_CARE_TEMP1,
    CMD_CARE_TEMP2,
    CMD_CARE_TIMEOUT,
    CMD_CARE_TIMEOUT_QUERY,
    CMD_CARE_LOC,
    CMD_CARE_ADDR,
    CMD_CARE_NULL,
		CMD_CARE_READ,
		CMD_CARE_ADDR_QUERY,
		CMD_CARE_AUTO,
		CMD_INFO,
		CMD_CARE_INFO,
    CMD_BOOT,
    CMD_INPUTABLE,
    CMD_ERROR = CMD_INPUTABLE,
    CMD_TALK,
};


typedef struct {
    uint8_t buffer[CMD_LEN];
    uint8_t len;
		uint8_t type;
} cdc_msg;


typedef struct {
    uint8_t item[15][2];
    uint8_t valid_items;
} address_t;

typedef struct {										//GPIB命令
    char cmd[CMD_LEN];
    uint8_t addr;											//address_t address;
    uint8_t read_after_write;					//直接读取数据;
    uint8_t eoi;											// Read until EOI,eoi结束
    uint8_t eos;											// Read until EOS char found
    uint8_t eot;											//传输结束模式，talker结束，EOI使能
    char eot_char;										//程序附加结束字符
    uint32_t timeout_max;
    uint8_t srq_status;
    uint8_t status;										//收发状态
    uint32_t read_delay;							//read_after_write,		auto=0时，经过delay再去读取数据,执行一次;auto=1时，经过delay再去读取数据，自动循环执行
    uint32_t ms_count;								//计时
    uint8_t error;										//出错计数
    uint32_t count;										//需要发送次数
    uint32_t done;										//已经发送次数
    uint8_t stop;											//下次停止
    uint8_t cmd_index;								//care设置命令序号
    uint8_t eot_int;									//care返回命令结束符0,1,2,3,
    uint8_t toc;											//care返回命令结束符0,1,2,3,
    int args;
		uint8_t online;
} gpib_config_struct;

typedef struct {
    uint8_t addr;
    uint8_t listen_only;
    uint8_t is_controller;
	  uint8_t debug;
} care_config;

//typedef struct {										//控制命令
//  enum command_t cmd;
//	char str[CMD_LEN];
//  int args;
//  int arg[30];
//} parsed_info_t;


extern uint32_t count_time;
extern care_config care;
extern uint8_t talking;
//extern uint8_t 	param_change_flage;

typedef gpib_config_struct gpib_config_t;

extern gpib_config_t cur_cmd, gpib_cmd;							//当前指令修改
extern gpib_config_t auto_cmd[MAX_INS];		//队列中

extern char *command_str[CMD_INPUTABLE];

extern uint32_t timeout;

//void change_param(void);
void print_char(enum command_t cmd, uint8_t arg1);
uint8_t query_auto_cmd(void);
uint8_t check_auto_cmd(uint8_t addr, uint8_t flag);
void parser_reset(void);
void RingBuff_Init(void);
uint8_t Write_RingBuff(char *buf);
uint8_t Read_RingBuff(char *buf);
enum command_t check_cmd(char *str);
int check_arg(char *str, uint8_t len);
void TransmissionT_Data(uint8_t *p_data,uint32_t size);
void run_command(gpib_config_t *cmd);
void parse(char c);
static void print_eos(uint8_t eos_code);
static void print_1arg(enum command_t cmd, uint32_t arg1);

uint8_t care_protocol(uint8_t device, char *bytes, uint8_t length);
void add_terminator(char* str, uint8_t eos_code);



#endif /* __PARSER_H__ */
