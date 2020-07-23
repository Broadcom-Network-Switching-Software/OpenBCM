/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UTILS_HOST_BOARD_H_INCLUDED__
/* { */
#define __UTILS_HOST_BOARD_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif


#include "appl/diag/dcmn/bsp_cards_consts.h"

#if !DUNE_BCM
#else
#ifndef B_HOST_MAX_PARAM_LEN
#define B_HOST_MAX_PARAM_LEN  20
#endif
#endif

void
  init_host_board(
    void
  );
unsigned int
  utils_is_in_crate(
    void
  );

unsigned int
  utils_is_crate_gobi(
    const unsigned int crate_type
  );

unsigned int
  utils_is_crate_negev(
    const unsigned int crate_type
  );

int
  reset_device_on_host_board(
    const unsigned int deassert_not_assert_reset,
    const unsigned int device_location_on_reset_word
  );

#define __ATTRIBUTE_PACKED__  __attribute__ ((packed))

/*
 * App Activate
 * {
 */

typedef enum
{
  SOC_D_USR_APP_FLAVOR_NONE               = 0,
  SOC_D_USR_APP_FLAVOR_NORMAL             = 1,
  SOC_D_USR_APP_FLAVOR_MINIMAL            = 2,
  SOC_D_USR_APP_FLAVOR_WIN_01             = 3,
  SOC_D_USR_APP_FLAVOR_SERVER             = 4,
  SOC_D_USR_APP_FLAVOR_NORMAL_AND_SERVER  = 5,
  SOC_D_USR_APP_FLAVOR_SERVER_WITH_INIT   = 6,
  SOC_D_USR_APP_FLAVOR_MINIMAL_NO_APP     = 7,
  SOC_D_USR_APP_FLAVOR_NOF_ENUMS

} SOC_D_USR_APP_FLAVOR;


void
  d_usr_app_flavor_get(
     SOC_D_USR_APP_FLAVOR* usr_app_flavor
  );

const char*
  d_usr_app_flavor_to_str(
    const SOC_D_USR_APP_FLAVOR usr_app_flavor,
    const unsigned int     short_format
  );

/*
 * }
 */




/*
 * App State
 * {
 */
typedef enum
{
  D_APP_STATE_E_NOT_VALID = 0,        /*Defualt value - Not Valid*/
  D_APP_STATE_E_BOOT_FIN,             /*Boot is Over*/
  D_APP_STATE_E_START_UP_APP_FIN,     /*Start UP App is Over - FE/FAP/NP app is running*/
  D_APP_STATE_E_START_UP_FIN_NO_APP,  /*Start UP is Over - NO FE/FAP/NP app is running*/
  D_APP_STATE_E_APP_ERR,              /*Start UP is Over - Application was running and error was found*/

  /*
   * Last enumerator
   */
  D_APP_STATE_E_NOF_STATES
} D_APP_STATE_ENUM;

typedef struct
{
  D_APP_STATE_ENUM enum_state;

  unsigned long tbd_1;
  unsigned long tbd_2;
  unsigned long tbd_3;
  unsigned long tbd_4;
  unsigned long tbd_5;
  unsigned long tbd_6;
  unsigned long tbd_7;
  unsigned long tbd_8;
  unsigned long tbd_9;
} __ATTRIBUTE_PACKED__ D_APP_STATE;

/*
 * Is used by 'd_app_state_set_state()'
 */
#define D_APP_STATE_SET_STATE_PRINT_SHORT_MAX (6)

void
  d_app_state_get(
     D_APP_STATE* d_app_state
  );

void
  d_app_state_set_state(
     const D_APP_STATE_ENUM e_state
  );

D_APP_STATE_ENUM 
  d_app_state_get_state(void
  );

const char*
  d_app_state_set_state_to_str(
    const D_APP_STATE* d_app_state,
    const unsigned int short_format
  );


/*
 * }
 */




/*
 * {
 * D_BOARD_INFO structure
 */


/*
 * Structure of 'Dune Board Information'
 * ---  Always Change at the END of the structure---
 */
typedef struct
{
  /*
   * On return, the finger load here the host board type.
   * As return from "host_board_type_from_nv()".
   */
  SOC_BSP_CARDS_DEFINES host_board_type ;

  /*
   * On return, the finger load here the host slot-id.
   * As return from "slot_id()".
   */
  unsigned char  slot_id ;

  /*
   * On return, the finger load here the host board device version.
   * As return from "host_board_type_from_nv()".
   */
  unsigned int   device_ver ;

  /*
   * On return, the finger load here the host board serial number.
   * As return from "host_board_sn_from_nv()".
   */
  unsigned short host_board_serial_number ;

  /*
   * On return, the finger load here the Crate Type.
   * As return from "get_crate_type()".
   */
  unsigned char crate_type ;

  /*
   * On return, the finger load here the SW version.
   * As return from "get_prog_ver()".
   */
  unsigned long prog_version;

  /*
   * On return, the finger load here the CPU board serial number.
   * As return from "host_board_sn_from_nv()".
   */
  unsigned short cpu_board_serial_number ;

  /*
   * On return, the finger load here the Chassis crate-id.
   * As return from "get_crate_id()".
   * If the CPU is not in crate - it will be indicated in
   * 'slot_id' as ZERO.
   */
  unsigned long crate_id;

  /*
   * Application state, as return from "d_app_state_get()"
   * This is run-time state.
   */
  D_APP_STATE   app_state;

  /*
   * Application Activate, as return from "d_usr_app_flavor_get()".
   * This is user request in the NvRam
   */
  SOC_D_USR_APP_FLAVOR  usr_app_flavor;

  /*
   * On return, the finger load here the front-host board serial number.
   * As return from "host_board_sn_from_nv()".
   */
  unsigned short front_host_board_serial_number ;

  /*
   * On return, the finger loads here the host daughter-board serial number.
   * As return from "host_board_sn_from_nv()".
   */
  unsigned short host_db_serial_number ;

  /*
   * On return, the finger load here the host board device debug version (device subversion).
   * As return from "host_board_type_from_nv()".
   */
  unsigned int   device_dbg_ver ;

  /*
   * Adding fields here only
   */

} __ATTRIBUTE_PACKED__ D_BOARD_INFO_STRCT;



#define D_BOARD_INFO_SIZE_IN_LONG 100
typedef union
{
  D_BOARD_INFO_STRCT  inf;
  unsigned long       FILLER[D_BOARD_INFO_SIZE_IN_LONG];

} __ATTRIBUTE_PACKED__ D_BOARD_INFO;



int
  board_information_get(
    D_BOARD_INFO* b_info
  );
void
  board_information_print(
    const D_BOARD_INFO* b_info,
    const unsigned int  short_format
  );
void
  board_information_clear(
    D_BOARD_INFO* b_info
  );
/*
 * }
 */


/*
 * {
 * D_BOARD_NV_APP_INFO structure
 */


typedef struct
{
  char  host_board_param_1[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_2[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_3[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_4[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_5[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_6[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_7[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_8[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_9[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_10[B_HOST_MAX_PARAM_LEN];
} __ATTRIBUTE_PACKED__ D_BOARD_NV_APP_INFO;


int
  board_nv_app_information_get(
    D_BOARD_NV_APP_INFO* b_app
  );
int
  board_nv_app_information_set(
    const D_BOARD_NV_APP_INFO* b_app
  );

void
  board_nv_app_information_print(
    const D_BOARD_NV_APP_INFO* b_app,
    const unsigned int         short_format
  );
void
  board_nv_app_information_clear(
    D_BOARD_NV_APP_INFO* b_app
  );

/*
 * }
 */





/*
 * {
 * D_BOARD_NV_CPU_INFO -- CPU board - NvRam values
 */


typedef struct
{
  unsigned short  board_serial_number ;
  unsigned short  board_version ;
  char            board_description[40] ;
  char            board_name[12] ;
} D_BOARD_NV_CPU_INFO;


int
  board_nv_cpu_info_get(
    D_BOARD_NV_CPU_INFO* b_cpu
  );

void
  board_nv_cpu_info_print(
    const D_BOARD_NV_CPU_INFO* b_cpu,
    const unsigned int         short_format
  );

void
  board_nv_cpu_info_get_clear(
    D_BOARD_NV_CPU_INFO* b_cpu
  );


/*
 * }
 */

#ifdef  __cplusplus
}
#endif

/* } __UTILS_HOST_BOARD_H_INCLUDED__*/
#endif

