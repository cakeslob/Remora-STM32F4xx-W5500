/**
 * Copyright (c) 2022 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdbool.h>

extern "C" {
#include "port_common.h"
#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "spi.h"
}

#include "drivers/pin/pin.h"

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */

#ifndef WIZCHIP_SPI_PRESCALER
#define WIZCHIP_SPI_PRESCALER SPI_BAUDRATEPRESCALER_2
#endif

static void (*irq_callback)(void);
static volatile bool spin_lock = false;

// We've abstracted away platform specific logic using the Pin driver
Pin* wiz_cs_pin = nullptr;
char wiz_cs_storage[sizeof(Pin)];

Pin* wiz_rst_pin = nullptr;
char wiz_rst_storage[sizeof(Pin)];


/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
static inline void wizchip_select(void)
{
    wiz_cs_pin->set(GPIO_PIN_RESET);
}

static inline void wizchip_deselect(void)
{
    wiz_cs_pin->set(GPIO_PIN_SET);
}

void wizchip_reset()
{
    // not sure if needed, doesnt seem to effect using spi1 pa5 sck, unsure what to do. -cakeslob
    wiz_cs_pin->set(GPIO_PIN_RESET);
    wiz_rst_pin->set(GPIO_PIN_RESET);
    HAL_Delay(250);                 // Todo - abstract away any HAL_Delays. May need a little driver unit for timing
    wiz_cs_pin->set(GPIO_PIN_SET);
    wiz_rst_pin->set(GPIO_PIN_SET);
    HAL_Delay(250);
}


static void wizchip_critical_section_lock(void)
{
    while(spin_lock);

    spin_lock = true;
}

static void wizchip_critical_section_unlock(void)
{
    spin_lock = false;
}

void wizchip_spi_initialize(void)
{   
    /* 
    Your chosen SPI_PORT, WIZ_CS_PORT_AND_PIN and WIZ_RST_PORT_AND_PIN all need to be initialised in your env in platformio.ini, for example. Note the string escape formatting "\" and \""
    build_flags = 
        ${common.build_flags}
        -D SPI_PORT=1
        -D WIZ_CS_PORT_AND_PIN="\"PB_06\""
        -D WIZ_RST_PORT_AND_PIN="\"PB_05\""
    */
    wiz_cs_pin = new (wiz_cs_storage) Pin(WIZ_CS_PORT_AND_PIN, OUTPUT, NONE); 
    wiz_rst_pin = new (wiz_rst_storage) Pin(WIZ_RST_PORT_AND_PIN, OUTPUT, NONE);   

    //__HAL_RCC_GPIOB_CLK_ENABLE(); // note to self - we can tell this is initialised in main_init.c, but if anything changes and this stops working at some point we may have a race condition where this port is not initialised until later in the code. 

    // reset the device pre-emptively
    wizchip_reset();

    // initialise the SPI bus
    wizchip_select();
    spi_init();
    wizchip_deselect();
    spi_set_speed(WIZCHIP_SPI_PRESCALER);
}

void wizchip_cris_initialize(void)
{
    spin_lock = false;
    reg_wizchip_cris_cbfunc(wizchip_critical_section_lock, wizchip_critical_section_unlock);
}

void wizchip_initialize(void)
{
    wizchip_reset();

    reg_wizchip_cris_cbfunc(wizchip_critical_section_lock, wizchip_critical_section_unlock);
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
    reg_wizchip_spi_cbfunc(spi_get_byte, spi_put_byte);
    reg_wizchip_spiburst_cbfunc(spi_read, spi_write);

    /* W5x00 initialize */
    uint8_t temp;
#if (_WIZCHIP_ == W5100S)
    uint8_t memsize[2][4] = {{8, 0, 0, 0}, {8, 0, 0, 0}};
#elif (_WIZCHIP_ == W5500)
    uint8_t memsize[2][8] = {{8, 0, 0, 0, 0, 0, 0, 0}, {8, 0, 0, 0, 0, 0, 0, 0}};
#endif

    if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
    {
        printf(" W5x00 initialized fail\n");

        return;
    }

    /* Check PHY link status */
    do
    {
        if (ctlwizchip(CW_GET_PHYLINK, (void *)&temp) == -1)
        {
            printf(" Unknown PHY link status\n");

            return;
        }
    } while (temp == PHY_LINK_OFF);
}

void wizchip_check(void)
{
#if (_WIZCHIP_ == W5100S)
    /* Read version register */
    if (getVER() != 0x51)
    {
        printf(" ACCESS ERR : VERSION != 0x51, read value = 0x%02x\n", getVER());

        while (1)
            ;
    }
#elif (_WIZCHIP_ == W5500)
    /* Read version register */
    if (getVERSIONR() != 0x04)
    {
        printf(" ACCESS ERR : VERSION != 0x04, read value = 0x%02x\n", getVERSIONR());

        while (1)
            ;
    }
#endif
}

/* Network */
void network_initialize(wiz_NetInfo net_info)
{
    ctlnetwork(CN_SET_NETINFO, (void *)&net_info);
}

void print_network_information(wiz_NetInfo net_info)
{
    uint8_t tmp_str[8] = {
        0,
    };

    ctlnetwork(CN_GET_NETINFO, (void *)&net_info);
    ctlwizchip(CW_GET_ID, (void *)tmp_str);

    if (net_info.dhcp == NETINFO_DHCP)
    {
        printf("====================================================================================================\n");
        printf(" %s network configuration : DHCP\n\n", (char *)tmp_str);
    }
    else
    {
        printf("====================================================================================================\n");
        printf(" %s network configuration : static\n\n", (char *)tmp_str);
    }

    printf(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    printf(" IP          : %d.%d.%d.%d\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    printf(" Subnet Mask : %d.%d.%d.%d\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    printf(" Gateway     : %d.%d.%d.%d\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    printf(" DNS         : %d.%d.%d.%d\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
    printf("====================================================================================================\n\n");
}
