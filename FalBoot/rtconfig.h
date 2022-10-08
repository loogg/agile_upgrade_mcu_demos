#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define IDLE_THREAD_STACK_SIZE 256

/* kservice optimization */

#define RT_DEBUG
#define RT_DEBUG_COLOR

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX

/* Memory Management */

#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart1"
#define RT_VER_NUM 0x40100
#define ARCH_ARM
#define RT_USING_CPU_FFS
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M4

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 1024
#define RT_MAIN_THREAD_PRIORITY 10
#define RT_USING_FAL
#define FAL_DEBUG_CONFIG
#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_PIN

/* Using USB */


/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* POSIX (Portable Operating System Interface) layer */


/* Interprocess Communication (IPC) */


/* Socket is in the 'Network' category */


/* Network */


/* Utilities */


/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */


/* XML: Extensible Markup Language */


/* multimedia packages */

/* LVGL: powerful and easy-to-use embedded GUI library */


/* u8g2: a monochrome graphic library */


/* PainterEngine: A cross-platform graphics application framework written in C language */


/* tools packages */


/* system packages */

/* enhanced kernel services */


/* acceleration: Assembly language or algorithmic acceleration packages */


/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */


/* Micrium: Micrium software products porting for RT-Thread */

#define PKG_USING_AGILE_UPGRADE
#define AGILE_UPGRADE_ENABLE_LOG
#define AGILE_UPGRADE_USING_CRC_TAB_CONST
#define AGILE_UPGRADE_ENABLE_QUICKLZ
#define AGILE_UPGRADE_ENABLE_FAL
#define AGILE_UPGRADE_ENABLE_VERSION_COMPARE
#define PKG_USING_AGILE_UPGRADE_LATEST_VERSION
#define PKG_AGILE_UPGRADE_VER_NUM 0x99999

/* peripheral libraries and drivers */


/* Kendryte SDK */


/* AI packages */


/* miscellaneous packages */

/* project laboratory */


/* samples: kernel and components samples */


/* entertainment: terminal games and other interesting software packages */

#define PKG_USING_QUICKLZ
#define QLZ_COMPRESSION_LEVEL 3
#define PKG_USING_QUICKLZ_V101

/* Arduino libraries */


/* Projects */


/* Sensors */


/* Display */


/* Timing */


/* Data Processing */


/* Data Storage */

/* Communication */


/* Device Control */


/* Other */

/* Signal IO */


/* Uncategorized */

#define SOC_FAMILY_STM32
#define SOC_SERIES_STM32F4

/* Hardware Drivers Config */

#define SOC_STM32F407ZG
#define BOARD_STM32F407_ATK_EXPLORER

/* Onboard Peripheral Drivers */

#define BSP_USING_USB_TO_USART

/* Enable File System */


/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_UART
#define BSP_USING_UART1
#define BSP_USING_ON_CHIP_FLASH

/* Board extended module Drivers */


#endif
