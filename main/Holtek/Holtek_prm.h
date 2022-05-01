
/**
 *  @file       Holtek_prm.h
 *
 *  @brief      Header file containing configuration definitions for the module.
 *
 *
 *  @copyright  Copyright 2022. 
 *              Haier Europe. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef HOLTEK_PRM_H
    #define HOLTEK_PRM_H

//-------------------------------------- Include Files ----------------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- MODULE CONFIGURATION DEFINES AND TYPES ---------------------------------------
//=====================================================================================================================

/*
 * Define here the logic digits to be handled
 */
typedef enum
{
  DIGIT_LEFT_2 = 0,
  DIGIT_LEFT_1,
  DIGIT_MIDDLE,
  DIGIT_RIGHT_1,
  DIGIT_RIGHT_2,
  NUM_OF_DIGITS
}DISPLAY_DIGIT_ENUM;

/*
 * Define here the logic icons to be handled
 */
typedef enum
{
  ICON_NONE = 0, /*dummy ID to indicate no mapping*/
  ICON_WIFI,
  ICON_OPEN_DOOR,
  ICON_ADD_DISH,
  ICON_OPTION1,
  ICON_OPTION2,
  ICON_OPTION3,
  ICON_OPTION4,
  ICON_OPTION5,
  ICON_OPTION6,
  ICON_P1,
  ICON_P2,
  ICON_P3,
  ICON_P4,
  ICON_P_TOTAL_CARE,
  ICON_DELAY_3H,
  ICON_DELAY_6H,
  ICON_DELAY_9H,
  ICON_PHASE_PREWASH,
  ICON_PHASE_MAINWASH,
  ICON_PHASE_DRY,
  ICON_SALT,
  ICON_TIME_DOT,
  ICON_LD11,
  ICON_LD12,
  ICON_STD_ENROLL,
  ICON_EASY_ENROLL,
  NUM_OF_ICONS
}DISPLAY_ICON_ENUM;

#endif
