#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define PRU_BASEFREQ    	75000           // PRU Base thread ISR update frequency (hz)
#define PRU_SERVOFREQ       1000            // PRU Servo thread ISR update freqency (hz)  Ideally this is 2x higher than the servo thread frequency of LCNC.

#define BASE_SLICE          0               // IRQ Slice used by the Base thread
#define SERVO_SLICE         1               // IRQ Slice used by the Servo thread

#define STEPBIT     		22            	// bit location in DDS accum
#define STEP_MASK   		(1L<<STEPBIT)

#define JSON_BUFF_SIZE	    10000			// Jason dynamic buffer size

#define JOINTS			    6				// Number of joints - set this the same as LinuxCNC HAL compenent. Max 8 joints
#define VARIABLES           4             	// Number of command values - set this the same as the LinuxCNC HAL compenent

#define PRU_DATA		    0x64617461 	    // "data" SPI payload
#define PRU_READ            0x72656164      // "read" SPI payload
#define PRU_WRITE           0x77726974      // "writ" SPI payload
#define PRU_ESTOP           0x65737470      // "estp" SPI payload
#define PRU_ACKNOWLEDGE		0x61636b6e	    // "ackn" payload
#define PRU_ERR		        0x6572726f	    // "erro" payload

#define DATA_ERR_MAX         40


// Data buffer configuration
#define BUFFER_SIZE 		64            	// Size of recieve buffer - same as HAL component, 64

#define usart(t) usartN(t)
#define usartN(t) USART ## t
#define usartINT(t) usartint(t)
#define usartint(t) USART ## t ## _IRQn
#define usartHANDLER(t) usarthandler(t)
#define usarthandler(t) USART ## t ## _IRQHandler
#define usartCLKEN(t) usartclken(t)
#define usartclken(t) __HAL_RCC_USART ## t ## _CLK_ENABLE

#define UART2            usart(3)
#define UART2_IRQ        usartINT(3)
#define UART2_IRQHandler usartHANDLER(3)
#define UART2_CLK_En     usartCLKEN(3)

#define UART2_CLK HAL_RCC_GetPCLK1Freq()

#define UART2_TX_PIN 10
#define UART2_RX_PIN 5
#define UART2_PORT GPIOC
#define UART2_AF GPIO_AF7_USART3

#define RS485_UDP_PORT 27183

#define PLL_SYS_KHZ (125 * 1000)    // 133MHz
#define SOCKET_MACRAW 0
#define PORT_LWIPERF 5001

// Location for storage of JSON config file in Flash
#define JSON_STORAGE_ADDRESS 0x8060000
#define JSON_UPLOAD_ADDRESS 0x8050000
#define USER_FLASH_LAST_PAGE_ADDRESS  	0x8060000
#define USER_FLASH_END_ADDRESS        	0x807FFFF
#define JSON_SECTOR FLASH_SECTOR_7

#define DEFAULT_CONFIG {0x7b,0x0a,0x09,0x22,0x42,0x6f,0x61,0x72,0x64,0x22,0x3a,0x20,0x22,0x46,0x6c,0x65,0x78,0x69,0x48,0x41,0x4c,0x22,0x2c,0x0a,0x09,0x22,0x4d,0x6f,0x64,0x75,0x6c,0x65,0x73,0x22,0x3a,0x5b,0x0a,0x09,0x7b,0x0a,0x09,0x22,0x54,0x68,0x72,0x65,0x61,0x64,0x22,0x3a,0x20,0x22,0x53,0x65,0x72,0x76,0x6f,0x22,0x2c,0x0a,0x09,0x22,0x54,0x79,0x70,0x65,0x22,0x3a,0x20,0x22,0x42,0x6c,0x69,0x6e,0x6b,0x22,0x2c,0x0a,0x09,0x09,0x22,0x43,0x6f,0x6d,0x6d,0x65,0x6e,0x74,0x22,0x3a,0x09,0x09,0x09,0x22,0x42,0x6c,0x69,0x6e,0x6b,0x79,0x22,0x2c,0x0a,0x09,0x09,0x22,0x50,0x69,0x6e,0x22,0x3a,0x09,0x09,0x09,0x09,0x22,0x50,0x43,0x31,0x33,0x22,0x2c,0x0a,0x09,0x09,0x22,0x46,0x72,0x65,0x71,0x75,0x65,0x6e,0x63,0x79,0x22,0x3a,0x20,0x09,0x09,0x34,0x0a,0x09,0x7d,0x0a,0x09,0x5d,0x0a,0x7d}

/* Default config contents:

{
	"Board": "FlexiHAL",
	"Modules":[
	{
	"Thread": "Servo",
	"Type": "Blink",
		"Comment":			"Blinky",
		"Pin":				"PC13",
		"Frequency": 		4
	}
	]
}

*/

#endif