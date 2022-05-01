
/**
 *  @file       Holtek.c
 *
 *  @brief      Basic description of file contents
 *
 *
 *  @copyright  Copyright 2022. 
 *              Haier Europe. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#include <Holtek.h>
#include <Holtek_prv.h>


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
// global structure to set digits state
DISPLAY_DIGIT_TYPE gpx_Display_Digit[NUM_OF_DIGITS];

// global icons bitmap
uint8_t gpu8_Display_Icons_Bitmap[DISPLAY_ICONS_BITMAP_BYTES_NUM];

// global structure to handle blink of icons
DISPLAY_BLINK_TYPE gpx_Display_Blink_Icons[NUM_OF_ICONS];

// global structure to handle blink of digits
DISPLAY_BLINK_TYPE gpx_Display_Blink_Digits[NUM_OF_DIGITS];
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


/**
 * Data structures related to SPI Holtek driver
 * management
 */

typedef enum
{
  SPI_HLTK_INIT_PERIPHERAL,
  SPI_HLTK_SET_CFG_PROTOCOL_FORMAT,
  SPI_HLTK_CFG_SYS_DIS,
  SPI_HLTK_CFG_COM_OPTION,
  SPI_HLTK_CFG_MASTER_MODE,
  SPI_HLTK_CFG_SYS_ON,
  SPI_HLTK_CFG_PWM_MODE,
  SPI_HLTK_CFG_BLINK_MODE,
  SPI_HLTK_CFG_LED_ON,
  SPI_HLTK_CANCEL_CFG_PROTOCOL_FORMAT,
  SPI_HLTK_SET_WRITE_PROTOCOL_FORMAT,
  SPI_HLTK_REFRESH,
  SPI_HLTK_PREPARE_REINIT,
  SPI_HLTK_WAIT_DRIVER_READY,
  SPI_HLTK_CANCEL_CFG_OFF_MODE,
  SPI_HLTK_OFF_MODE,
}SPI_HLTK_ENUM;

static const char* pc_SPI_HLTK_ENUM[] =
{
    [SPI_HLTK_INIT_PERIPHERAL] = "SPI_HLTK_INIT_PERIPHERAL",
    [SPI_HLTK_SET_CFG_PROTOCOL_FORMAT] = "SPI_HLTK_SET_CFG_PROTOCOL_FORMAT",
    [SPI_HLTK_CFG_SYS_DIS] = "SPI_HLTK_CFG_SYS_DIS",
    [SPI_HLTK_CFG_COM_OPTION] = "SPI_HLTK_CFG_COM_OPTION",
    [SPI_HLTK_CFG_MASTER_MODE] = "SPI_HLTK_CFG_MASTER_MODE",
    [SPI_HLTK_CFG_SYS_ON] = "SPI_HLTK_CFG_SYS_ON",
    [SPI_HLTK_CFG_PWM_MODE] = "SPI_HLTK_CFG_PWM_MODE",
    [SPI_HLTK_CFG_BLINK_MODE] = "SPI_HLTK_CFG_BLINK_MODE",
    [SPI_HLTK_CFG_LED_ON] = "SPI_HLTK_CFG_LED_ON",
    [SPI_HLTK_CANCEL_CFG_PROTOCOL_FORMAT] = "SPI_HLTK_CANCEL_CFG_PROTOCOL_FORMAT",
    [SPI_HLTK_SET_WRITE_PROTOCOL_FORMAT] = "SPI_HLTK_SET_WRITE_PROTOCOL_FORMAT",
    [SPI_HLTK_REFRESH] = "SPI_HLTK_REFRESH",
    [SPI_HLTK_PREPARE_REINIT] = "SPI_HLTK_PREPARE_REINIT",
    [SPI_HLTK_WAIT_DRIVER_READY] = "SPI_HLTK_WAIT_DRIVER_READY",
    [SPI_HLTK_CANCEL_CFG_OFF_MODE] = "SPI_HLTK_CANCEL_CFG_OFF_MODE",
    [SPI_HLTK_OFF_MODE] = "SPI_HLTK_OFF_MODE"
};

typedef struct
{
  SPI_HLTK_ENUM state;
  SPI_HLTK_ENUM state_prev;
  SPI_HLTK_ENUM state_next;
  spi_bus_config_t spi_bus_config;
  spi_device_handle_t spi_device_hdl;
  spi_transaction_t spi_transaction;
  spi_transaction_t *trans_desc;
  spi_device_interface_config_t spi_device_interface_config[2];
  bool flag_startup_init;
}SPI_HLTK_HANDLER_TYPE;

#define SPI_HLTK_DEVICE_INT_CONFIG_ID   0
#define SPI_HLTK_DEVICE_INT_REFRESH_ID   1

static SPI_HLTK_HANDLER_TYPE x_Spi_Hltk_Handler;

// buffer for data sent via SPI to Holtek
static uint8_t pu8_Hmi_SPI_Mem_Ram[HMI_SPI_MEM_RAM_SIZE_BYTES];

static uint64_t u64_Holtek_Refresh_Period;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void SpiDriverSetup(void);
static void SpiTaskCallback(void *pv_args);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Module initialization method
 *
 */
void Holtek__Initialize(void)
{
  SpiDriverSetup();

  gpx_Display_Digit[DIGIT_LEFT_2].ascii_char = '0';
  gpx_Display_Digit[DIGIT_LEFT_1].ascii_char = '1';
  gpx_Display_Digit[DIGIT_MIDDLE].ascii_char = '2';
  gpx_Display_Digit[DIGIT_RIGHT_1].ascii_char = '3';
  gpx_Display_Digit[DIGIT_RIGHT_2].ascii_char = '4';
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 * @brief   This local method initializes the SPI channel to drive the
 *          HOLTEK chip that handles the DIGITs led matrix.
 *
 */
static void SpiDriverSetup(void)
{
  // setup bus HW configuration
  x_Spi_Hltk_Handler.spi_bus_config.miso_io_num = -1;   // MISO not used
  x_Spi_Hltk_Handler.spi_bus_config.mosi_io_num = HMI_SPI_MOSI_PIN;
  x_Spi_Hltk_Handler.spi_bus_config.sclk_io_num = HMI_SPI_CLK_PIN;
  x_Spi_Hltk_Handler.spi_bus_config.quadwp_io_num = -1;   // WP not used
  x_Spi_Hltk_Handler.spi_bus_config.quadhd_io_num = -1;   // Hold not used
  x_Spi_Hltk_Handler.spi_bus_config.max_transfer_sz = 0; // default

  // setup BUS protocol for CONFIGURATION commands
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].clock_speed_hz = HMI_SPI_CLK_SPEED_HZ;               //Clock out in Hz
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].mode = 0;                                //SPI mode 0
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].spics_io_num = HMI_SPI_LATCH_PIN;               //CS pin
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].queue_size = 10;                          //We want to be able to queue 10 transactions at a time
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].pre_cb = NULL;  //Specify pre-transfer callback to handle D/C line
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].post_cb = NULL;  //Specify post-transfer callback to handle D/C line
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].address_bits = 8;
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].command_bits = 3;
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].cs_ena_pretrans = 16;
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].cs_ena_posttrans = 16;
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID].flags = SPI_DEVICE_HALFDUPLEX;

  // setup BUS protocol for REFRESH commands
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].clock_speed_hz = HMI_SPI_CLK_SPEED_HZ;               //Clock out in Hz
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].mode = 0;                                //SPI mode 0
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].spics_io_num = HMI_SPI_LATCH_PIN;               //CS pin
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].queue_size = 10;                          //We want to be able to queue 10 transactions at a time
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].pre_cb = NULL;  //Specify pre-transfer callback to handle D/C line
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].post_cb = NULL;  //Specify post-transfer callback to handle D/C line
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].address_bits = 7;
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].command_bits = 3;
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].cs_ena_pretrans = 16;
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].cs_ena_posttrans = 16;
  x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID].flags = SPI_DEVICE_HALFDUPLEX;

  /*
   * do not send packet, the first bytes are enough to drive the 2 shift registers.
   *
   * The trick is to use ADDRESS and COMMAND fields as shift registers data bytes.
   *
   */
  x_Spi_Hltk_Handler.spi_transaction.flags = 0;
  x_Spi_Hltk_Handler.spi_transaction.length = 0;
  x_Spi_Hltk_Handler.spi_transaction.rxlength = 0;
  x_Spi_Hltk_Handler.spi_transaction.rx_buffer = NULL;
  x_Spi_Hltk_Handler.spi_transaction.tx_buffer = NULL;
  x_Spi_Hltk_Handler.spi_transaction.user = NULL;

  // set flag for first initialization signal
  x_Spi_Hltk_Handler.flag_startup_init = true;


  u64_Holtek_Refresh_Period = esp_timer_get_time();

  // create task of key events handler
  xTaskCreate(SpiTaskCallback, "SpiCallback", (1024 * 8), NULL, 15, NULL); //was *8
}


/**
 * @brief   This task handles the communication with HOLTEK driver.
 *          Sets initialization commands and periodic refresh
 *          of internal RAM.
 *
 * @param pv_args NULL
 */
static void SpiTaskCallback(void *pv_args)
{
 spi_transaction_t x_trans;
 uint8_t u8_bit_idx;
 DIGIT_SEG_TYPE px_digit_status[NUM_OF_DIGITS];
 DISPLAY_DIGIT_ENUM e_digit;

 // task loop
 while (true)
 {
   switch (x_Spi_Hltk_Handler.state)
   {

     case SPI_HLTK_INIT_PERIPHERAL:
       //Initialize the SPI bus, avoid usage of DMA (not needed)
       if (spi_bus_initialize(HSPI_HOST, &x_Spi_Hltk_Handler.spi_bus_config, 0) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_SET_CFG_PROTOCOL_FORMAT;
       }
       else
       {
         ESP_LOGE("SpiTaskCallback", "%s", pc_SPI_HLTK_ENUM[x_Spi_Hltk_Handler.state]);
       }
       break;

     case SPI_HLTK_SET_CFG_PROTOCOL_FORMAT:
       //Attach the Holtek to the SPI bus with CONFIGURATION protocol format
       if (spi_bus_add_device( HSPI_HOST,
                               &x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_CONFIG_ID],
                               &x_Spi_Hltk_Handler.spi_device_hdl) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_CFG_SYS_DIS;
       }
       else
       {
         ESP_LOGE("SpiTaskCallback", "%s", pc_SPI_HLTK_ENUM[x_Spi_Hltk_Handler.state]);
       }
       break;

     case SPI_HLTK_CFG_SYS_DIS:
       if (x_Spi_Hltk_Handler.flag_startup_init == true)
       {
         //! SYS DIS - 100 0000 0000
         x_Spi_Hltk_Handler.spi_transaction.cmd = 4;
         x_Spi_Hltk_Handler.spi_transaction.addr = 0x00;
         x_Spi_Hltk_Handler.spi_transaction.length = 0;
         x_Spi_Hltk_Handler.spi_transaction.tx_buffer = NULL;

         if (spi_device_queue_trans( x_Spi_Hltk_Handler.spi_device_hdl,
                                     &x_Spi_Hltk_Handler.spi_transaction,
                                     portMAX_DELAY) == ESP_OK)
         {
           x_Spi_Hltk_Handler.state = SPI_HLTK_WAIT_DRIVER_READY;
           x_Spi_Hltk_Handler.state_next = SPI_HLTK_CFG_COM_OPTION;
         }
       }
       else
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_CFG_COM_OPTION;
       }
       break;

     case SPI_HLTK_CFG_COM_OPTION:
       //! COM OPTION - 100 0010 abXX
       // ab = 00 -> N-MOS open drain output and 8 COM option
       x_Spi_Hltk_Handler.spi_transaction.cmd = 4;
       x_Spi_Hltk_Handler.spi_transaction.addr = 0x20;
       x_Spi_Hltk_Handler.spi_transaction.length = 0;
       x_Spi_Hltk_Handler.spi_transaction.tx_buffer = NULL;

       if (spi_device_queue_trans( x_Spi_Hltk_Handler.spi_device_hdl,
                                   &x_Spi_Hltk_Handler.spi_transaction,
                                   portMAX_DELAY) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_WAIT_DRIVER_READY;
         x_Spi_Hltk_Handler.state_next = SPI_HLTK_CFG_MASTER_MODE;
       }
       break;

     case SPI_HLTK_CFG_MASTER_MODE:
       //! MASTER MODE - 100 0001 10XX
       x_Spi_Hltk_Handler.spi_transaction.cmd = 4;
       x_Spi_Hltk_Handler.spi_transaction.addr = 0x18;
       x_Spi_Hltk_Handler.spi_transaction.length = 0;
       x_Spi_Hltk_Handler.spi_transaction.tx_buffer = NULL;

       if (spi_device_queue_trans( x_Spi_Hltk_Handler.spi_device_hdl,
                                   &x_Spi_Hltk_Handler.spi_transaction,
                                   portMAX_DELAY) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_WAIT_DRIVER_READY;
         x_Spi_Hltk_Handler.state_next = SPI_HLTK_CFG_SYS_ON;
       }
       break;

     case SPI_HLTK_CFG_SYS_ON:
       //! SYS ON - 100 0000 0001
       x_Spi_Hltk_Handler.spi_transaction.cmd = 4;
       x_Spi_Hltk_Handler.spi_transaction.addr = 0x01;
       x_Spi_Hltk_Handler.spi_transaction.length = 0;
       x_Spi_Hltk_Handler.spi_transaction.tx_buffer = NULL;

       if (spi_device_queue_trans( x_Spi_Hltk_Handler.spi_device_hdl,
                                   &x_Spi_Hltk_Handler.spi_transaction,
                                   portMAX_DELAY) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_WAIT_DRIVER_READY;
         x_Spi_Hltk_Handler.state_next = SPI_HLTK_CFG_PWM_MODE;
       }
       break;

     case SPI_HLTK_CFG_PWM_MODE:
       //! PWM DUTY - 100 101X 1111
       x_Spi_Hltk_Handler.spi_transaction.cmd = 4;

       x_Spi_Hltk_Handler.spi_transaction.addr = 0xAF;

       x_Spi_Hltk_Handler.spi_transaction.length = 0;
       x_Spi_Hltk_Handler.spi_transaction.tx_buffer = NULL;

       if (spi_device_queue_trans( x_Spi_Hltk_Handler.spi_device_hdl,
                                   &x_Spi_Hltk_Handler.spi_transaction,
                                   portMAX_DELAY) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_WAIT_DRIVER_READY;
         x_Spi_Hltk_Handler.state_next = SPI_HLTK_CFG_BLINK_MODE;
       }
       break;

     case SPI_HLTK_CFG_BLINK_MODE:
       //! BLINK OFF - 100 0000 1000
       x_Spi_Hltk_Handler.spi_transaction.cmd = 4;
       x_Spi_Hltk_Handler.spi_transaction.addr = 0x08;
       x_Spi_Hltk_Handler.spi_transaction.length = 0;
       x_Spi_Hltk_Handler.spi_transaction.tx_buffer = NULL;

       if (spi_device_queue_trans( x_Spi_Hltk_Handler.spi_device_hdl,
                                   &x_Spi_Hltk_Handler.spi_transaction,
                                   portMAX_DELAY) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_WAIT_DRIVER_READY;
         x_Spi_Hltk_Handler.state_next = SPI_HLTK_CFG_LED_ON;
       }
       break;

     case SPI_HLTK_CFG_LED_ON:
       //! LED ON - 100 0000 0011
       x_Spi_Hltk_Handler.spi_transaction.cmd = 4;
       x_Spi_Hltk_Handler.spi_transaction.addr = 0x03;
       x_Spi_Hltk_Handler.spi_transaction.length = 0;
       x_Spi_Hltk_Handler.spi_transaction.tx_buffer = NULL;

       if (spi_device_queue_trans( x_Spi_Hltk_Handler.spi_device_hdl,
                                   &x_Spi_Hltk_Handler.spi_transaction,
                                   portMAX_DELAY) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_WAIT_DRIVER_READY;
         x_Spi_Hltk_Handler.state_next = SPI_HLTK_CANCEL_CFG_PROTOCOL_FORMAT;
       }
       break;

     case SPI_HLTK_CANCEL_CFG_PROTOCOL_FORMAT:
       // reset the protocol format for CONFIGURATION
       if (spi_bus_remove_device(x_Spi_Hltk_Handler.spi_device_hdl) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_SET_WRITE_PROTOCOL_FORMAT;
       }
       else
       {
         ESP_LOGE("SpiTaskCallback", "%s", pc_SPI_HLTK_ENUM[x_Spi_Hltk_Handler.state]);
       }
       break;

     case SPI_HLTK_SET_WRITE_PROTOCOL_FORMAT:
       //Attach the Holtek to the SPI bus with REFRESH protocol format
       if (spi_bus_add_device( HSPI_HOST,
                               &x_Spi_Hltk_Handler.spi_device_interface_config[SPI_HLTK_DEVICE_INT_REFRESH_ID],
                               &x_Spi_Hltk_Handler.spi_device_hdl) == ESP_OK)
       {
         u64_Holtek_Refresh_Period = esp_timer_get_time();

         x_Spi_Hltk_Handler.state = SPI_HLTK_REFRESH;
       }
       break;

     case SPI_HLTK_REFRESH:

       // clear flag, at least one config done since statup
       x_Spi_Hltk_Handler.flag_startup_init = false;

       // check timeout for Holtek configuration refresh
       if ((esp_timer_get_time() - u64_Holtek_Refresh_Period) >= SEC_TO_USEC(5))
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_PREPARE_REINIT;

         u64_Holtek_Refresh_Period = esp_timer_get_time();
       }
       else
       {

         // refresh periodically the Holtek RAM

         // clear buffer
         memset(pu8_Hmi_SPI_Mem_Ram, 0x00, HMI_SPI_MEM_RAM_SIZE_BYTES);    //ALL OFF

         // loop to assign ASCII segments to physical leds in digits
         for (e_digit = 0; e_digit < NUM_OF_DIGITS; ++e_digit)
         {
           // convert ascii char to digits bitmap
           px_digit_status[e_digit].lword = (ASCII_8Digit_Table_Conversion[gpx_Display_Digit[e_digit].ascii_char] * gpx_Display_Blink_Digits[e_digit].toggle);
           px_digit_status[e_digit].seg.dp = (gpx_Display_Digit[e_digit].dp * gpx_Display_Blink_Digits[e_digit].toggle);

           // per each segment, set proper digit bit
           for (u8_bit_idx = 0; u8_bit_idx < HMI_SPI_MEM_RAM_SIZE_BYTES; ++u8_bit_idx)
           {
             //
             //step 1 - test the bit in the ASCII bitmap starting from Aa to N
             //step 2 - use the segment index as index in the holtek memory with offset 2 (first 2 bits are linked to digits not mounted)
             //
             //NOTE: each BYTE in the holtek memory refers to a SEGMENT (i.e. Aa), each BIT (starting from bit 2) refers to a DIGIT starting from LEFT2 -> RIGHT2
             //
             if(BIT_TEST(px_digit_status[e_digit].lword, u8_bit_idx) != 0)
             {
               BIT_SET(pu8_Hmi_SPI_Mem_Ram[u8_bit_idx], e_digit+2);
             }
             else
             {
               BIT_CLR(pu8_Hmi_SPI_Mem_Ram[u8_bit_idx], e_digit+2);
             }
           }
         }

         /**
           * Special management of the "WIFI" icon that is actually mapped on Holtek display
           *
           */
         if ((px_digit_status[DIGIT_LEFT_1].seg.dp == 1) ||
             ((gpx_Display_Blink_Icons[ICON_WIFI].state == true) && (gpx_Display_Blink_Icons[ICON_WIFI].toggle == true)))
         {
           BIT_SET(pu8_Hmi_SPI_Mem_Ram[1], 7);
         }
         else
         {
           BIT_CLR(pu8_Hmi_SPI_Mem_Ram[1], 7);
         }


         x_trans.flags = 0;
         x_trans.length = 0;
         x_trans.rxlength = 0;
         x_trans.rx_buffer = NULL;
         x_trans.tx_buffer = NULL;
         x_trans.user = NULL;

         x_trans.addr = 0;
         x_trans.cmd = 5;
         x_trans.length = (HMI_SPI_MEM_RAM_SIZE_BYTES * 8);
         x_trans.tx_buffer = pu8_Hmi_SPI_Mem_Ram;

         spi_device_queue_trans(x_Spi_Hltk_Handler.spi_device_hdl, &x_trans, portMAX_DELAY);

         x_Spi_Hltk_Handler.state = SPI_HLTK_WAIT_DRIVER_READY;
         x_Spi_Hltk_Handler.state_next = SPI_HLTK_REFRESH;
       }
       break;

     case SPI_HLTK_PREPARE_REINIT:
       // reset the protocol format for REFRESH
       if (spi_bus_remove_device(x_Spi_Hltk_Handler.spi_device_hdl) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_SET_CFG_PROTOCOL_FORMAT;
       }
       else
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_WAIT_DRIVER_READY;
         x_Spi_Hltk_Handler.state_next = SPI_HLTK_PREPARE_REINIT;
         ESP_LOGE("SpiTaskCallback", "%s", pc_SPI_HLTK_ENUM[x_Spi_Hltk_Handler.state]);
       }
       break;

     case SPI_HLTK_WAIT_DRIVER_READY:
       if (spi_device_get_trans_result(x_Spi_Hltk_Handler.spi_device_hdl,
                                       &x_Spi_Hltk_Handler.trans_desc,
                                       portMAX_DELAY) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = x_Spi_Hltk_Handler.state_next;
       }
       break;

     case SPI_HLTK_CANCEL_CFG_OFF_MODE:
       // reset the protocol format for CONFIGURATION
       if (spi_bus_remove_device(x_Spi_Hltk_Handler.spi_device_hdl) == ESP_OK)
       {
         x_Spi_Hltk_Handler.state = SPI_HLTK_OFF_MODE;
       }
       else
       {
         ESP_LOGE("SpiTaskCallback", "%s", pc_SPI_HLTK_ENUM[x_Spi_Hltk_Handler.state]);
       }
       break;

     case SPI_HLTK_OFF_MODE:
       break;

     default:
       break;
   }

   vTaskDelay(100 / portTICK_RATE_MS);
 }

 vTaskDelete(NULL);
}
