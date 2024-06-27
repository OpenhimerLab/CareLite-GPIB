
#ifndef __GPIB_IO_H__
#define __GPIB_IO_H__

#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"
#include "util.h"


#define REN_Pin 						LL_GPIO_PIN_2
#define REN_GPIO_Port 			GPIOA
#define IFC_Pin 						LL_GPIO_PIN_3
#define IFC_GPIO_Port 			GPIOA
#define NDAC_Pin 						LL_GPIO_PIN_4
#define NDAC_GPIO_Port 			GPIOA
#define NRFD_Pin 						LL_GPIO_PIN_5
#define NRFD_GPIO_Port 			GPIOA
#define DAV_Pin 						LL_GPIO_PIN_6
#define DAV_GPIO_Port 			GPIOA
#define EOI_Pin 						LL_GPIO_PIN_7
#define EOI_GPIO_Port 			GPIOA
#define TE_Pin 							LL_GPIO_PIN_8
#define TE_GPIO_Port 				GPIOA

#define ATN_Pin 						LL_GPIO_PIN_0
#define ATN_GPIO_Port 			GPIOB
#define SRQ_Pin 						LL_GPIO_PIN_1
#define SRQ_GPIO_Port 			GPIOB
#define SC_Pin 							LL_GPIO_PIN_2
#define SC_GPIO_Port 				GPIOB
#define DC_Pin 							LL_GPIO_PIN_13
#define DC_GPIO_Port 				GPIOB

#define PE_Pin 							LL_GPIO_PIN_14
#define PE_GPIO_Port 				GPIOC

#define DIO1_Pin 						LL_GPIO_PIN_15
#define DIO1_GPIO_Port 			GPIOA
#define DIO2_Pin 						LL_GPIO_PIN_3
#define DIO2_GPIO_Port 			GPIOB
#define DIO3_Pin 						LL_GPIO_PIN_4
#define DIO3_GPIO_Port 			GPIOB
#define DIO4_Pin 						LL_GPIO_PIN_13
#define DIO4_GPIO_Port 			GPIOC
#define DIO5_Pin 						LL_GPIO_PIN_6
#define DIO5_GPIO_Port 			GPIOB
#define DIO6_Pin 						LL_GPIO_PIN_7
#define DIO6_GPIO_Port 			GPIOB
#define DIO7_Pin 						LL_GPIO_PIN_8
#define DIO7_GPIO_Port 			GPIOB
#define DIO8_Pin 						LL_GPIO_PIN_9
#define DIO8_GPIO_Port 			GPIOB

#define DS18B20_Pin 						LL_GPIO_PIN_5
#define DS18B20_GPIO_Port 			GPIOB


#define SC_HIGH								SC_GPIO_Port->BSRR = SC_Pin
#define SC_LOW								SC_GPIO_Port->BSRR = (SC_Pin<<16)

#define REN_HIGH							REN_GPIO_Port->BSRR = REN_Pin
#define REN_LOW								REN_GPIO_Port->BSRR = (REN_Pin<<16)

#define IFC_HIGH							IFC_GPIO_Port->BSRR = IFC_Pin
#define IFC_LOW								IFC_GPIO_Port->BSRR = (IFC_Pin<<16)

#define NDAC_HIGH							NDAC_GPIO_Port->BSRR = NDAC_Pin
#define NDAC_LOW							NDAC_GPIO_Port->BSRR = (NDAC_Pin<<16)

#define NRFD_HIGH							NRFD_GPIO_Port->BSRR = NRFD_Pin
#define NRFD_LOW							NRFD_GPIO_Port->BSRR = (NRFD_Pin<<16)

#define DAV_HIGH							DAV_GPIO_Port->BSRR = DAV_Pin
#define DAV_LOW								DAV_GPIO_Port->BSRR = (DAV_Pin<<16)

#define EOI_HIGH							EOI_GPIO_Port->BSRR = EOI_Pin
#define EOI_LOW								EOI_GPIO_Port->BSRR = (EOI_Pin<<16)

#define ATN_HIGH							ATN_GPIO_Port->BSRR = ATN_Pin
#define ATN_LOW								ATN_GPIO_Port->BSRR = (ATN_Pin<<16)

#define TE_HIGH								TE_GPIO_Port->BSRR = TE_Pin
#define TE_LOW								TE_GPIO_Port->BSRR = (TE_Pin<<16)

#define PE_HIGH								PE_GPIO_Port->BSRR = PE_Pin
#define PE_LOW								PE_GPIO_Port->BSRR = (PE_Pin<<16)

#define DC_HIGH								DC_GPIO_Port->BSRR = DC_Pin
#define DC_LOW								DC_GPIO_Port->BSRR = (DC_Pin<<16)

#define SRQ_HIGH							SRQ_GPIO_Port->BSRR = SRQ_Pin
#define SRQ_LOW								SRQ_GPIO_Port->BSRR = (SRQ_Pin<<16)


#define GPIB_READ_UNTIL_EOI 0x01
#define GPIB_WRITE_USE_EOI 	0x01


#define GPIB_GETCHAR_IS_ERROR(x) (x <= 0)
#define GPIB_GETCHAR_IS_EOI(x) (x >= 0x100)
#define GPIB_GETCHAR_IS_CMD(x) (x >= 0x200)
#define GPIB_GETCHAR_TO_DATA(x) ((uint8_t)(x & 0xFF))

#define LISTEN 	0x20
#define TALK 		0x40

#define GPIB_CMD_LAD(x) 							(x | LISTEN)
#define GPIB_CMD_IS_LAD_OR_UNL(x) 		((x & TALK) == LISTEN)
#define GPIB_CMD_TAD(x) 							(x | TALK)
#define GPIB_CMD_IS_TAD_OR_UNT(x) 		((x & TALK) == TALK)


enum uniline_message_t
{
    GPIB_UNI_CMD_START,
    GPIB_UNI_CMD_END,
    GPIB_UNI_BUS_CLEAR_START,
    GPIB_UNI_BUS_CLEAR_END,
    GPIB_UNI_CHECK_SRQ_ASSERT,
    GPIB_UNI_SRQ_ASSERT,
    GPIB_UNI_SRQ_DEASSERT,
};

enum gpib_command_t
{
    GPIB_CMD_DCL = 0x14,
    GPIB_CMD_UNL = 0x3F,
    GPIB_CMD_UNT = 0x5F,
    GPIB_CMD_GET = 0x08,
    GPIB_CMD_SDC = 0x04,
    GPIB_CMD_LLO = 0x11,
    GPIB_CMD_GTL = 0x01,
    GPIB_CMD_SPE = 0x18,
    GPIB_CMD_SPD = 0x19,
};

uint8_t send_gpib_cmd(uint8_t cmd,  uint8_t addr);
void gpib_io_init(void);
void gpib_io_set_timeout(uint32_t set_time);
int8_t putchar_internal(uint8_t c, uint8_t use_eoi);
int16_t gpib_write(char *buf, uint16_t length, uint8_t cmd);
int gpib_getchar(void);
void GPIB_REN(uint8_t state);
void GPIB_IFC(uint8_t state);
uint8_t address_target_listen(uint8_t address);
uint8_t address_target_talk(uint8_t address);
uint8_t address_target_state(uint8_t address);
uint8_t gpib_read(uint8_t use_eoi, uint8_t eos_code, uint8_t eot_enable, uint8_t eot_char);
uint8_t gpib_uniline(enum uniline_message_t msg);


#endif /* __GPIB_IO_H__ */
