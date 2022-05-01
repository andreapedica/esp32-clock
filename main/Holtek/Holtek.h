
/**
 *  @file       Holtek.h
 *
 *  @brief      Header of the module, containing interfaces and global data definition.
 *
 *
 *  @copyright  Copyright 2022. 
 *              Haier Europe. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef HOLTEK_H
    #define HOLTEK_H

//-------------------------------------- Include Files ----------------------------------------------------------------
#include <Holtek_prm.h>


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

// define size of the icons bitmap array
#define DISPLAY_ICONS_BITMAP_BYTES_NUM  ((NUM_OF_ICONS / 8) + 1)

// define logic digit structure
typedef struct
{
  uint8_t ascii_char;
  uint8_t dp;
}DISPLAY_DIGIT_TYPE;

// icons blink management structure
typedef struct
{
  uint32_t duty_on_set;
  uint32_t duty_off_set;
  uint32_t duty_ctr;
  bool state;
  bool toggle;
  bool endless;
  uint8_t repetitions;
}DISPLAY_BLINK_TYPE;

typedef struct
{
  uint32_t duty_on;
  uint32_t duty_off;
  bool endless;
  uint8_t repeats;
}BLINKING_TIMING_TYPE;

// define macro to calculate offsets in icons bitmap
#define DISPLAY_ICON_GET_BYTE_BIT(e_icon, out_u8_byte, out_u8_bit)  {out_u8_byte = (e_icon / 8); out_u8_bit = (e_icon % 8);}

// global structure to set digits state
extern DISPLAY_DIGIT_TYPE gpx_Display_Digit[NUM_OF_DIGITS];

// global icons bitmap
extern uint8_t gpu8_Display_Icons_Bitmap[DISPLAY_ICONS_BITMAP_BYTES_NUM];

// global structure to handle blink of icons
extern DISPLAY_BLINK_TYPE gpx_Display_Blink_Icons[NUM_OF_ICONS];

// global structure to handle blink of digits
extern DISPLAY_BLINK_TYPE gpx_Display_Blink_Digits[NUM_OF_DIGITS];

void Holtek__Initialize(void);

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif
