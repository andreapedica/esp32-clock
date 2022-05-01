
/**
 *  @file       Holtek_prv.h
 *
 *  @brief      Header containing private data definition for the module.
 *
 *
 *  @copyright  Copyright 2022.
 *              Haier Europe. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef HOLTEK_PRV_H
    #define HOLTEK_PRV_H

//-------------------------------------- Include Files ----------------------------------------------------------------
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
#define HOLTEK_SPI_HOST    HSPI_HOST

#define HOLTEK_PIN_NUM_MISO -1
#define HOLTEK_PIN_NUM_MOSI 18
#define HOLTEK_PIN_NUM_CLK  19
#define HOLTEK_PIN_NUM_CS   21

/**
 *
 * SPI driver parameters
 *
 */
#define HMI_SPI_LATCH_PIN GPIO_NUM_21
#define HMI_SPI_MOSI_PIN GPIO_NUM_18
#define HMI_SPI_CLK_PIN GPIO_NUM_19
#define HMI_SPI_CLK_SPEED_HZ 50000

#define HMI_SPI_MEM_RAM_SIZE_BYTES  16

// define digit segments bitmap structure

/**
 * Definition of bitmap to handle 8, 14, 16 segments
 * alphanumeric digits.
 *
 * |-----------------|--------------------|--------------------|
 * | 8 digits layout |  14 digits layout  |  16 digits layout  |
 * |-----------------|--------------------|--------------------|
 * |    ___a___      |    ______a______   |    __a1_____a2___  |
 * |    |     |      |    |\    |    / |  |    |\    |    / |  |
 * |   f|     |b     |   f| h   i   j  |b |   f| h   i   j  |b |
 * |    |     |      |    |  \  |  /   |  |    |  \  |  /   |  |
 * |    ___g___      |    __g1__| __g2__  |    __g1__| __g2__  |
 * |    |     |      |    |  /  |  \   |  |    |  /  |  \   |  |
 * |   e|     |c     |   e| m   l   k  |c |   e| m   l   k  |c |
 * |    |     |      |    |/    |    \ |  |    |/    |    \ |  |
 * |    ___d___      |    ______d______   |    __d1_____d2___  |
 * |                 |                    |                    |
 * |                 |                    |                    |
 * |-----------------|--------------------|--------------------|
 *
 *
 */
typedef union
{
  struct
  {
    uint32_t    a       :1; // used as a1 for 16 segments
    uint32_t    a2      :1;
    uint32_t    b       :1;
    uint32_t    c       :1;
    uint32_t    d       :1; // used as d1 for 16 segments
    uint32_t    d2      :1;
    uint32_t    e       :1;
    uint32_t    f       :1;
    uint32_t    g       :1; // used as g1 for 14/16 segments
    uint32_t    g2      :1;
    uint32_t    h       :1;
    uint32_t    i       :1;
    uint32_t    j       :1;
    uint32_t    k       :1;
    uint32_t    l       :1;
    uint32_t    m       :1;
    uint32_t    free    :15;
    uint32_t    dp      :1;
  }seg;
  uint32_t lword;
}DIGIT_SEG_TYPE;



#define SEG_NO 0x0000
#define SEG_A1 0x0001
#define SEG_A2 0x0002
#define SEG_B  0x0004
#define SEG_C  0x0008
#define SEG_D1 0x0010
#define SEG_D2 0x0020
#define SEG_E  0x0040
#define SEG_F  0x0080
#define SEG_G1 0x0100
#define SEG_G2 0x0200
#define SEG_H  0x0400
#define SEG_J  0x0800
#define SEG_K  0x1000
#define SEG_L  0x2000
#define SEG_M  0x4000
#define SEG_N  0x8000

#define SEG_WIFI_ROW 0x0002

#define    i_C_ALL    0x11

#define    C_NULL        0x0000
#define    C_X01        SEG_A1
#define    C_X02        SEG_A2
#define    C_X03        SEG_B
#define    C_X04        SEG_C
#define    C_X05        SEG_D1
#define    C_X06        SEG_D2
#define    C_X07        SEG_E
#define    C_X08        SEG_F
#define    C_X09        SEG_G1
#define    C_X0A        SEG_G2
#define    C_X0B        SEG_H
#define    C_X0C        SEG_J
#define    C_X0D        SEG_K
#define    C_X0E        SEG_L
#define    C_X0F        SEG_M
#define    C_X10        SEG_N
#define    C_ALL       (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_G1 | SEG_G2 | SEG_H | SEG_J | SEG_K | SEG_L | SEG_M | SEG_N)
#define    C_X12       (SEG_A1 | SEG_B | SEG_D1 | SEG_E | SEG_G1 | SEG_H | SEG_K | SEG_M)
#define    C_X13       (SEG_A2 | SEG_C | SEG_D2 | SEG_F | SEG_G2 | SEG_J | SEG_L | SEG_N)
#define    C_X14        0x0020
#define    C_X15        0x0021
#define    C_X16        0x0022
#define    C_X17        0x0023
#define    C_X18        0x0024
#define    C_X19        0x0025
#define    C_X1A        0x0026
#define    C_X1B        0x0027
#define    C_X1C        0x0028
#define    C_X1D        0x0029
#define    C_X1E        0x0030
#define    C_X1F        0x0031
#define    C_Space      SEG_NO
#define    C_X21        0x0033
#define    C_X22        0x0034
#define    C_X23        0x0035
#define    C_X24        0x0036
#define    C_X25        0x0037
#define    C_Ecom     (SEG_A1 | SEG_A2 | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_G1 | SEG_H | SEG_K | SEG_L)
#define    C_X27      (SEG_J) // '
#define    C_X28      (SEG_A1 | SEG_J | SEG_G1 | SEG_F) //°
#define    C_X29      (SEG_C | SEG_E | SEG_D1 | SEG_D2) // u minuscola
#define    C_X2A      (SEG_C | SEG_E | SEG_F | SEG_G1 | SEG_G2) // h minuscola
#define    C_X2B      (SEG_M | SEG_J | SEG_G1 | SEG_G2) //+
#define    C_vir      (SEG_M) //,
#define    C_X2D      (SEG_G1 | SEG_G2) //-
#define    C_X2E      (SEG_D1)
#define    C_X2F      (SEG_N | SEG_K) //
#define    C_0        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_K | SEG_N)
#define    C_1        (SEG_B | SEG_C | SEG_K)
#define    C_2        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_E | SEG_B | SEG_G1 | SEG_G2)
#define    C_3        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_C | SEG_B | SEG_G1 | SEG_G2)
#define    C_4        (SEG_B | SEG_C | SEG_G1 | SEG_G2 | SEG_F)
#define    C_5        (SEG_A1 | SEG_A2 | SEG_C | SEG_D1 | SEG_D2 | SEG_F | SEG_G1 | SEG_G2)
#define    C_6        (SEG_A1 | SEG_A2 | SEG_C | SEG_D1 | SEG_D2 | SEG_F | SEG_G1 | SEG_G2 | SEG_E)
#define    C_7        (SEG_A1 | SEG_A2 | SEG_C | SEG_B)
#define    C_8        (SEG_A1 | SEG_A2 | SEG_C | SEG_D1 | SEG_D2 | SEG_F | SEG_G1 | SEG_G2 | SEG_E | SEG_B)
#define    C_9        (SEG_A1 | SEG_A2 | SEG_C | SEG_D1 | SEG_D2 | SEG_F | SEG_G1 | SEG_G2 | SEG_B)
#define    C_div      (SEG_G2 | SEG_D2)
#define    C_X3B        0x0059
#define    C_X3C        0x0060
#define    C_X3D        0x0061
#define    C_X3E        0x0062
#define    C_X3F        0x0063
#define    C_X40        0x0064
#define    C_A        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_B        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_J | SEG_M | SEG_G2)
#define    C_C        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_D        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_J | SEG_M)
#define    C_E        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_F        (SEG_A1 | SEG_A2 | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_G        (SEG_A1 | SEG_A2 | SEG_G2 | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_H        (SEG_B | SEG_C | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_I        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_J | SEG_M)
#define    C_J        (SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E)
#define    C_K        (SEG_K | SEG_L | SEG_G1 | SEG_F | SEG_E)
#define    C_L        (SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_M        (SEG_B | SEG_C | SEG_H | SEG_K | SEG_E | SEG_F)
#define    C_N        (SEG_F | SEG_E | SEG_H | SEG_L | SEG_B | SEG_C)
#define    C_O        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_P        (SEG_A1 | SEG_A2 | SEG_B | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_Q        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_L)
#define    C_R        (SEG_A1 | SEG_A2 | SEG_B | SEG_L | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_S        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_C | SEG_F | SEG_G1 | SEG_G2)
#define    C_T        (SEG_A1 | SEG_A2 | SEG_J | SEG_M)
#define    C_U        (SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_V        (SEG_F | SEG_E | SEG_N | SEG_K)
#define    C_W        (SEG_B | SEG_C | SEG_L | SEG_N | SEG_E | SEG_F)
#define    C_X        (SEG_H | SEG_L | SEG_N | SEG_K)
#define    C_Y        (SEG_H | SEG_M | SEG_K)
#define    C_Z        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_N | SEG_K)
#define    C_X5B        0x0091
#define    C_X5C        0x0092
#define    C_X5D        0x0093
#define    C_X5E        0x0094
#define    C_X5F        0x0095
#define    C_X60        0x0096
#define    C_a        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_b        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_J | SEG_M | SEG_G2)
#define    C_c        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_d        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_J | SEG_M)
#define    C_e        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_f        (SEG_A1 | SEG_A2 | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_g        (SEG_A1 | SEG_A2 | SEG_G2 | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_h        (SEG_B | SEG_C | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_i        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_J | SEG_M)
#define    C_j        (SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E)
#define    C_k        (SEG_K | SEG_L | SEG_G1 | SEG_F | SEG_E)
#define    C_l        (SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_m        (SEG_B | SEG_C | SEG_H | SEG_K | SEG_E | SEG_F)
#define    C_n        (SEG_F | SEG_E | SEG_H | SEG_L | SEG_B | SEG_C)
#define    C_o        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_p        (SEG_A1 | SEG_A2 | SEG_B | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_q        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_L)
#define    C_r        (SEG_A1 | SEG_A2 | SEG_B | SEG_L | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_s        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_C | SEG_F | SEG_G1 | SEG_G2)
#define    C_t        (SEG_A1 | SEG_A2 | SEG_J | SEG_M)
#define    C_u        (SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_v        (SEG_F | SEG_E | SEG_N | SEG_K)
#define    C_w        (SEG_B | SEG_C | SEG_L | SEG_N | SEG_E | SEG_F)
#define    C_x        (SEG_H | SEG_L | SEG_N | SEG_K)
#define    C_y        (SEG_H | SEG_M | SEG_K)
#define    C_z        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_N | SEG_K)

//FVT TEST DIGIT '{'
#define    C_X7B        (SEG_A1 | SEG_B | SEG_D1 | SEG_E | SEG_G1 | SEG_H | SEG_K | SEG_M)

#define    C_X7C        0x0124

//FVT TEST DIGIT '}'
#define    C_X7D        (SEG_A2 | SEG_C | SEG_D2 | SEG_F | SEG_N | SEG_G2 | SEG_J | SEG_L)

#define    C_X7E        0x0126
#define    C_X7F        0x0127
/*da qui parte il greco*/
#define    C_X80        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G1 | SEG_G2) //(SEG_G1 | SEG_E | SEG_D1 | SEG_D2 | SEG_L | SEG_C)
#define    C_X81        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_J | SEG_M | SEG_G2) //(SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D2 | SEG_E | SEG_F | SEG_G2)
#define    C_X82        (SEG_A1 | SEG_A2 | SEG_E | SEG_F) //(SEG_H | SEG_M | SEG_K)
#define    C_X83        (SEG_F | SEG_E | SEG_H | SEG_L | SEG_D1 | SEG_D2) //(SEG_A1 | SEG_A2 | SEG_H | SEG_G1 | SEG_G2 | SEG_D1 | SEG_D2 | SEG_E | SEG_C)
#define    C_X84        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_G1) //(SEG_A1 | SEG_A2 | SEG_G1 | SEG_G2 | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_X85        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_K | SEG_N) //(SEG_A1 | SEG_A2 | SEG_K | SEG_L)
#define    C_X86        (SEG_B | SEG_C | SEG_E | SEG_F | SEG_G1 | SEG_G2) //(SEG_G2 | SEG_C | SEG_N)
#define    C_X87        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_F | SEG_E | SEG_G1 | SEG_G2) //(SEG_A1 | SEG_A2 | SEG_C | SEG_D1 | SEG_D2 | SEG_F | SEG_G1 | SEG_G2 | SEG_E | SEG_B)
#define    C_X88        (SEG_A1 | SEG_A2 | SEG_J | SEG_M | SEG_D1 | SEG_D2) //(SEG_G1 | SEG_M | SEG_D2 | SEG_D1)
#define    C_X89        (SEG_F | SEG_E | SEG_G1 | SEG_K | SEG_L) //(SEG_K | SEG_L | SEG_G1 | SEG_F | SEG_E)
#define    C_X8A        (SEG_F | SEG_E | SEG_H | SEG_L) //(SEG_H | SEG_L | SEG_N)
#define    C_X8B        (SEG_B | SEG_C | SEG_H | SEG_K | SEG_E | SEG_F) //(SEG_F | SEG_E | SEG_G1 | SEG_G2 | SEG_B)
#define    C_X8C        (SEG_F | SEG_E | SEG_H | SEG_L | SEG_B | SEG_C) //(SEG_N | SEG_E)

#define    C_X8D        (SEG_A1 | SEG_A2 | SEG_G1 | SEG_G2 | SEG_D1 | SEG_D2) //(SEG_A1 | SEG_A2 | SEG_G1 | SEG_G2 | SEG_D1 | SEG_D2 | SEG_B | SEG_C)
#define    C_X8E        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F) //(SEG_G1 | SEG_G2 | SEG_D1 | SEG_D2 | SEG_E | SEG_C)
#define    C_X8F        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_E | SEG_F) //(SEG_G1 | SEG_G2 | SEG_E | SEG_C)
#define    C_X90        (SEG_A1 | SEG_A2 | SEG_B | SEG_E | SEG_F | SEG_G1 | SEG_G2) //(SEG_A1 | SEG_A2 | SEG_G1 | SEG_G2 | SEG_F | SEG_E | SEG_B)
#define    C_X91        (SEG_A1 | SEG_A2 | SEG_H | SEG_N | SEG_D1 | SEG_D2) //(SEG_G1 | SEG_G2 | SEG_E | SEG_M | SEG_D1)
#define    C_X92        (SEG_A1 | SEG_A2 | SEG_J | SEG_M) //(SEG_G1 | SEG_G2 | SEG_D2 | SEG_M)
#define    C_X93        (SEG_F | SEG_G1 | SEG_G2 | SEG_B | SEG_M) //(SEG_D1 | SEG_D2 | SEG_E | SEG_C)
#define    C_X94        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_J | SEG_M) //(SEG_A2 | SEG_B | SEG_G2 | SEG_J | SEG_M | SEG_G1 | SEG_F)
#define    C_X95        (SEG_H | SEG_K | SEG_L | SEG_N) //(SEG_H | SEG_K | SEG_L | SEG_N)
#define    C_X96        (SEG_F | SEG_G1 | SEG_G2 | SEG_J | SEG_B | SEG_M) //(SEG_F | SEG_J | SEG_B | SEG_G1 | SEG_G2 | SEG_M)
#define    C_X97        (SEG_A1 | SEG_A2 | SEG_G1 | SEG_G2 | SEG_D1 | SEG_D2 | SEG_F | SEG_B | SEG_M) //(SEG_E | SEG_M | SEG_C | SEG_D1 | SEG_D2)

// special char AE - code 198
#define    C_X98        (SEG_A2 | SEG_J | SEG_G2 | SEG_M | SEG_D2 | SEG_N)

// special char F - code 1170
#define    C_X99        (SEG_E | SEG_F | SEG_A1 | SEG_G1)

// char e rovesciata - code 1240
#define    C_X9A        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_G1 | SEG_G2 | SEG_D1 | SEG_D2 | SEG_E)

#define    C_X9B        0x0155
#define    C_X9C        0x0156
#define    C_X9D        0x0157
#define    C_X9E        0x0158
#define    C_X9F        0x0159
#define    C_XA0        0x0160
#define    C_XA1        0x0161
#define    C_XA2        0x0162
#define    C_XA3        0x0163
#define    C_XA4        0x0164
#define    C_XA5        0x0165
#define    C_XA6        0x0166
#define    C_XA7        0x0167
#define    C_XA8        0x0168
#define    C_XA9        0x0169
#define    C_XAA        0x0170
#define    C_XAB        0x0171
#define    C_XAC        0x0172
#define    C_XAD        0x0173
#define    C_XAE        0x0174
#define    C_XAF        0x0175
/*da qui parte il cyrillic*/
#define    C_XB0        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G1 | SEG_G2) //a
#define    C_XB1        (SEG_A1 | SEG_A2 | SEG_C | SEG_D1 | SEG_D2 | SEG_F | SEG_G1 | SEG_G2 | SEG_E)
#define    C_XB2        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_G2 | SEG_J | SEG_M)
#define    C_XB3        (SEG_A1 | SEG_A2 | SEG_E | SEG_F)
#define    C_XB4        (SEG_A1 | SEG_A2 | SEG_H | SEG_K | SEG_E | SEG_C | SEG_G1 | SEG_G2)
#define    C_XB5        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_G1)
#define    C_XB6        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_G1)
#define    C_XB7        (SEG_H | SEG_J | SEG_K | SEG_L | SEG_M | SEG_N)
#define    C_XB8        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_B | SEG_C | SEG_G1 | SEG_G2)
#define    C_XB9        (SEG_B | SEG_C | SEG_E | SEG_F | SEG_N | SEG_K)
#define    C_XBA        (SEG_B | SEG_C | SEG_E | SEG_F | SEG_N | SEG_K)
#define    C_XBB        (SEG_F | SEG_E | SEG_G1 | SEG_K | SEG_L)
#define    C_XBC        (SEG_D1 | SEG_M | SEG_J | SEG_A2 | SEG_B | SEG_C)
#define    C_XBD        (SEG_B | SEG_C | SEG_H | SEG_K | SEG_E | SEG_F)
#define    C_XBE        (SEG_B | SEG_C | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_XBF        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_XC0        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_E | SEG_F)
#define    C_XC1        (SEG_A1 | SEG_A2 | SEG_B | SEG_E | SEG_F | SEG_G1 | SEG_G2)
#define    C_XC2        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_E | SEG_F)
#define    C_XC3        (SEG_A1 | SEG_A2 | SEG_J | SEG_M)
#define    C_XC4        (SEG_H | SEG_N | SEG_K | SEG_E)
#define    C_XC5        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_J | SEG_M)
#define    C_XC6        (SEG_H | SEG_L | SEG_N | SEG_K)
#define    C_XC7        (SEG_F | SEG_J | SEG_G1 | SEG_G2 | SEG_C)
#define    C_XC8        (SEG_B | SEG_C | SEG_G1 | SEG_G2 | SEG_F)
#define    C_XC9        (SEG_B | SEG_C | SEG_D1 | SEG_D2 | SEG_E | SEG_F | SEG_J | SEG_M)
#define    C_XCA        (SEG_F | SEG_J | SEG_B | SEG_G1 | SEG_G2 | SEG_C)
#define    C_XCB        (SEG_A1 | SEG_J | SEG_M | SEG_G2 | SEG_D2 | SEG_C)
#define    C_XCC        (SEG_F | SEG_E | SEG_D1 | SEG_G1 | SEG_M | SEG_B | SEG_C)
#define    C_XCD        (SEG_F | SEG_E | SEG_D1 | SEG_D2 | SEG_G1 | SEG_G2 | SEG_C)
#define    C_XCE        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_C | SEG_B | SEG_G1 | SEG_G2)
#define    C_XCF        (SEG_A2 | SEG_B | SEG_C | SEG_D2 | SEG_M | SEG_J | SEG_G1 | SEG_E | SEG_F)
#define    C_XD0        (SEG_A1 | SEG_A2 | SEG_B | SEG_C | SEG_G1 | SEG_G2 | SEG_N | SEG_F)
#define    C_XD1        (SEG_A1 | SEG_A2 | SEG_D1 | SEG_D2 | SEG_J | SEG_M)
#define    C_XD2        0x0210
#define    C_XD3        0x0211
#define    C_XD4        0x0212
#define    C_XD5        0x0213
#define    C_XD6        0x0214
#define    C_XD7        0x0215
#define    C_XD8        0x0216
#define    C_XD9        0x0217
#define    C_XDA        0x0218
#define    C_XDB        0x0219
#define    C_XDC        0x0220
#define    C_XDD        0x0221
#define    C_XDE        0x0222
#define    C_XDF        0x0223
#define    C_XE0        0x0224
#define    C_XE1        0x0225
#define    C_XE2        0x0226
#define    C_XE3        0x0227
#define    C_XE4        0x0228
#define    C_XE5        0x0229
#define    C_XE6        0x0230
#define    C_XE7        0x0231
#define    C_XE8        0x0232
#define    C_XE9        0x0233
#define    C_XEA        0x0234
#define    C_XEB        0x0235
#define    C_XEC        0x0236
#define    C_XED        0x0237
#define    C_XEE        0x0238
#define    C_XEF        0x0239
#define    C_XF0        0x0240
#define    C_XF1        0x0241
#define    C_XF2        0x0242
#define    C_XF3        0x0243
#define    C_XF4        0x0244
#define    C_XF5        0x0245
#define    C_XF6        0x0246
#define    C_XF7        0x0247
#define    C_XF8        0x0248
#define    C_XF9        0x0249
#define    C_XFA        0x0250
#define    C_XFB        0x0251
#define    C_XFC        0x0252
#define    C_XFD        0x0253
#define    C_XFE        0x0254
#define    C_XFF        0x0255

// table of ASCII conversion in digits
//static const uint32_t ASCII_8Digit_Table_Conversion[] = {
///*0x00*/    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
///*0x10*/    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
///*0x20*/    0x00000000, 0x000000C0, 0x00000001, 0x00000111, 0x00000199, 0x00000088, 0x000001D9, 0x00000001, 0x000000D1, 0x0000001D, 0x000001DD, 0x00000000, 0x80000000, 0x00000100, 0x80000000, 0x00000144,
///*0x30*/    0x000000DD, 0x0000000C, 0x00000155, 0x0000011D, 0x0000018C, 0x00000199, 0x000001D9, 0x0000000D, 0x000001DD, 0x0000019D, 0x00000000, 0x00000000, 0x0000010C, 0x00000000, 0x000001C0, 0x00000145,
///*0x40*/    0x000001D5, 0x000001CD, 0x000001DD, 0x000000D1, 0x0000015C, 0x000001D1, 0x000001C1, 0x000000D9, 0x000001CC, 0x0000000C, 0x0000005C, 0x000001CC, 0x000000D0, 0x000000CD, 0x00000148, 0x000000DD,
///*0x50*/    0x000001C5, 0x0000018D, 0x00000140, 0x00000199, 0x0000000D, 0x000000DC, 0x00000058, 0x000000DC, 0x000001CC, 0x0000019C, 0x00000155, 0x000000D1, 0x00000188, 0x0000001D, 0x00000001, 0x00000010,
///*0x60*/    0x00000001, 0x0000015D, 0x000001D8, 0x00000150, 0x0000015C, 0x000001D5, 0x000001C1, 0x0000019D, 0x000001C8, 0x0000000C, 0x0000001C, 0x000001CC, 0x000000D0, 0x00000149, 0x00000148, 0x00000158,
///*0x70*/    0x000001C5, 0x0000018D, 0x00000140, 0x00000199, 0x000001D0, 0x000000DC, 0x00000058, 0x000000DC, 0x000001CC, 0x0000019C, 0x00000155, 0x000000D1, 0x00000188, 0x0000001D, 0x00000001, 0x00000000
//};

#ifdef APPLICATION_ALT_TEST
  //! FOR ALT TEST
  static const uint32_t ASCII_8Digit_Table_Conversion[] =
{
  C_NULL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL, C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_Space,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,      C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,  C_ALL,
};

#else
static const uint32_t ASCII_8Digit_Table_Conversion[] =
{
  C_NULL, C_X01,  C_X02,  C_X03,  C_X04,  C_X05,  C_X06,      C_X07,  C_X08,  C_X09,  C_X0A,  C_X0B,  C_X0C,  C_X0D,  C_X0E,  C_X0F,
  C_X10,  C_ALL,  C_X12,  C_X13,  C_X14,  C_X15,  C_X16,      C_X17,  C_X18,  C_X19,  C_X1A,  C_X1B,  C_X1C,  C_X1D,  C_X1E,  C_X1F,
  C_Space,C_X21,  C_X22,  C_X23,  C_X24,  C_X25,  C_Ecom,     C_X27,  C_X28,  C_X29,  C_X2A,  C_X2B,  C_vir,  C_X2D,  C_X2E,  C_X2F,
  C_0,    C_1,    C_2,    C_3,    C_4,    C_5,    C_6,        C_7,    C_8,    C_9,    C_div,  C_X3B,  C_X3C,  C_X3D,  C_X3E,  C_X3F,
  C_X40,  C_A,    C_B,    C_C,    C_D,    C_E,    C_F,        C_G,    C_H,    C_I,    C_J,    C_K,    C_L,    C_M,    C_N,    C_O,
  C_P,    C_Q,    C_R,    C_S,    C_T,    C_U,    C_V,        C_W,    C_X,    C_Y,    C_Z,    C_X5B,  C_X5C,  C_X5D,  C_X5E,  C_X5F,
  C_X60,  C_a,    C_b,    C_c,    C_d,    C_e,    C_f,        C_g,    C_h,    C_i,    C_j,    C_k,    C_l,    C_m,    C_n,    C_o,
  C_p,    C_q,    C_r,    C_s,    C_t,    C_u,    C_v,        C_w,    C_x,    C_y,    C_z,    C_X7B,  C_X7C,  C_X7D,  C_X7E,  C_X7F,
  C_X80,  C_X81,  C_X82,  C_X83,  C_X84,  C_X85,  C_X86,      C_X87,  C_X88,  C_X89,  C_X8A,  C_X8B,  C_X8C,  C_X8D,  C_X8E,  C_X8F,
  C_X90,  C_X91,  C_X92,  C_X93,  C_X94,  C_X95,  C_X96,      C_X97,  C_X98,  C_X99,  C_X9A,  C_X9B,  C_X9C,  C_X9D,  C_X9E,  C_X9F,
  C_XA0,  C_XA1,  C_XA2,  C_XA3,  C_XA4,  C_XA5,  C_XA6,      C_XA7,  C_XA8,  C_XA9,  C_XAA,  C_XAB,  C_XAC,  C_XAD,  C_XAE,  C_XAF,
  C_XB0,  C_XB1,  C_XB2,  C_XB3,  C_XB4,  C_XB5,  C_XB6,      C_XB7,  C_XB8,  C_XB9,  C_XBA,  C_XBB,  C_XBC,  C_XBD,  C_XBE,  C_XBF,
  C_XC0,  C_XC1,  C_XC2,  C_XC3,  C_XC4,  C_XC5,  C_XC6,      C_XC7,  C_XC8,  C_XC9,  C_XCA,  C_XCB,  C_XCC,  C_XCD,  C_XCE,  C_XCF,
  C_XD0,  C_XD1,  C_XD2,  C_XD3,  C_XD4,  C_XD5,  C_XD6,      C_XD7,  C_XD8,  C_XD9,  C_XDA,  C_XDB,  C_XDC,  C_XDD,  C_XDE,  C_XDF,
  C_XE0,  C_XE1,  C_XE2,  C_XE3,  C_XE4,  C_XE5,  C_XE6,      C_XE7,  C_XE8,  C_XE9,  C_XEA,  C_XEB,  C_XEC,  C_XED,  C_XEE,  C_XEF,
  C_XF0,  C_XF1,  C_XF2,  C_XF3,  C_XF4,  C_XF5,  C_XF6,      C_XF7,  C_XF8,  C_XF9,  C_XFA,  C_XFB,  C_XFC,  C_XFD,  C_XFE,  C_XFF
};

#endif

// bit manipulation fast macros
#define BIT_TEST(mem,bit)   ((mem)&(1ULL<<(bit)))
#define BIT_SET(mem,bit)    ((mem)|=(1ULL<<(bit)))
#define BIT_CLR(mem,bit)    ((mem)&=~(1ULL<<(bit)))
#define BIT_TOGGLE(mem,bit) ((mem)^=(1ULL<<(bit)))
#define BITMAP_SET(bit)    ((1ULL<<(bit)))
#define BITMAP_CLR(bit)    (~(1ULL<<(bit)))
// macro to convert a number to ASCII char
#define CONVERT_NUM_TO_ASCII(n) (uint8_t)('0'+n)

// macro to improve readibility of code when long timeouts have to be defined in small units like usec
#define MSEC_TO_USEC(x) (uint64_t)((uint64_t)x * 1000ULL)
#define SEC_TO_USEC(x)  (uint64_t)((uint64_t)x * 1000000ULL)
#define SEC_TO_MSEC(x)  (uint64_t)((uint64_t)x * 1000ULL)

#endif
