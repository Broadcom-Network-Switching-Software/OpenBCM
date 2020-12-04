/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef _UTILS_NVRAM_CONFIGURATION_
#define _UTILS_NVRAM_CONFIGURATION_


#if !DUNE_BCM
#include "eeprom.h"
#endif

/********************************************************
* Documentation at utils_nvram_configuration.c
 */
STATUS
  nvRamGet(
    char *string,    /* where to copy non-volatile RAM    */
    int strLen,      /* maximum number of bytes to copy   */
    int offset       /* byte offset into non-volatile RAM */
    );
/********************************************************
* Documentation at utils_nvram_configuration.c
 */
STATUS
  nvRamSet(
    char *string,     /* string to be copied into non-volatile RAM */
    int strLen,       /* maximum number of bytes to copy           */
    int offset        /* byte offset into non-volatile RAM         */
    );

/********************************************************
* Documentation at utils_nvram_configuration.c
 */
int
  host_eeprom_front_card_type_from_nv(
    HOST_EEPROM_FRONT_CARD_TYPE  *host_eeprom_front_card_type_en_ptr
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
 */
int
  host_eeprom_front_card_type_to_nv(
    HOST_EEPROM_FRONT_CARD_TYPE  host_eeprom_front_card_type_en
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
 */
int
  host_run_val_of_front_card_type(
    HOST_EEPROM_FRONT_CARD_TYPE  *host_eeprom_front_card_type_en_ptr
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
 */
const char *
  get_front_card_type_name(
    HOST_EEPROM_FRONT_CARD_TYPE  host_eeprom_front_card_type_en
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
 */
unsigned int
  is_front_end_ixp2800_connected(
    void
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
 */
unsigned int
  is_front_end_x10_connected(
    void
  );

/*
 * HOST_DB board related procedures.
 * {
 */
int
  host_db_board_cat_number_from_nv(
    char  *board_serial_num_ptr
    ) ;
int
  host_db_board_description_from_nv(
    char  *board_description_ptr
    ) ;
int
  host_db_board_version_from_nv(
    char  *board_version_ptr
    ) ;
int
  host_db_board_sn_from_nv(
    unsigned short  *board_sn_ptr
    ) ;
int
  host_db_board_param_i_from_nv(
    unsigned int i,
    char  *board_param_i_ptr
    ) ;
int
  host_db_board_type_from_nv(
    unsigned short  *board_board_type_ptr
    ) ;
/*
 * }
 */

int
  host_front_end_type_from_nv(
    SOC_BSP_CARDS_DEFINES  *board_type_ptr
    );

#endif /* _UTILS_NVRAM_CONFIGURATION_ */
