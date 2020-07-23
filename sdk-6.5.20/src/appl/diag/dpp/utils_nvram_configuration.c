/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_nvram_configuration.h>


#if !DUNE_BCM
#include "drv/mem/eeprom.h"
#include "utils_nvram_configuration.h"
#include "utils_line_FTG.h"
#include "utils_line_TGS.h"
#include "utils_line_GFA.h"
#include "utils_line_GFA_FAP21V.h"

#include "UserInterface/include/ui_pure_defi.h"
#include "pub/include/ui_defx.h"

#else
unsigned int
  get_use_par_nv_sem(void){return 0;}
int
  take_par_nv_sem(void){return 0;}
int
  give_back_par_nv_sem(void){return 0;}

#endif

#define NV_RAM_READ_B(x)      ((volatile)eepromReadByte(x))
#define NV_RAM_WRITE_B(x,y,z) eepromWriteBytes(x,(unsigned char *)y,(unsigned int)z)

/******************************************************************************
*
* nvRamGet - get the contents of non-volatile RAM
*
* This routine copies the contents of non-volatile memory into a specified
* string.
* This routine is for use at the application level (at boot,
* use sysNvRamGet).
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: NvRamSet()
* Remarks:
*  If offset is larger than/equal to 'SERIAL_EE_FIRST_OFFSET' then this
*  is offest in the serial eeprom (starting at 'SERIAL_EE_FIRST_OFFSET',
*  which is equivalent to '0').
*
*  Part of this procedure must be carried out uninterrupted by other
*  potential users. Make sure to not invoke any operating system
*  services that might enable interrupts (through enabling task
*  switching).
*/

STATUS
  nvRamGet(
    char *string,    /* where to copy non-volatile RAM    */
    int strLen,      /* maximum number of bytes to copy   */
    int offset       /* byte offset into non-volatile RAM */
    )
{
  STATUS
    ret ;
  int
    err ;
  unsigned
    int
      sem_taken ;
#if !DUNE_BCM
  unsigned
    int
      block_id ;
#endif
  sem_taken = FALSE ;
#if !DUNE_BCM
  err =
    ee_offset_to_block_id(
              (unsigned int)offset,(unsigned int)strLen,&block_id) ;
  if (err)
  {
    ret = ERROR ;
    goto snrg_exit ;
  }
  ret = OK ;
  if (get_use_par_nv_sem())
  {
    take_par_nv_sem() ;
    sem_taken = TRUE ;
  }
  err =
    check_block_crc(block_id) ;
  if (err < 0)
  {
    ret = ERROR ;
    goto snrg_exit ;
  }
  /*
   * If CRC of the block is wrong then load it with
   * default values first.
   */
  if (err)
  {
    err =
      load_defaults(block_id) ;
    if (err)
    {
      /*
       * If loading of defaults into NV ram did not work then
       * just read defaults image.
       */
      get_defaults((char *)string,(int)strLen,(int)offset) ;
      goto snrg_exit ;
    }
  }
#else
  ret = OK ;
#endif
  if (offset < SERIAL_EE_FIRST_OFFSET)
  {
    while (strLen--)
    {
#if !DUNE_BCM
      *string++ = NV_RAM_READ_B(offset);
#else
      *string++ = eepromReadByte(offset);
#endif
      offset++ ;
    }
  }
  else
  {
    char
      err_msg[160] ;
    unsigned
      int
        err_flg ;
    err = ser_ee_high_image_read(
              (unsigned int)(offset - SERIAL_EE_FIRST_OFFSET),
              (unsigned int)strLen,(unsigned char*)string,
              &err_flg,err_msg
          ) ;
    if (err)
    {
      ret = ERROR ;
      goto snrg_exit ;
    }
  }
snrg_exit:
  if (sem_taken)
  {
    give_back_par_nv_sem() ;
  }
  return (ret) ;
}
/*******************************************************************************
*
* nvRamSet - write to non-volatile RAM
*
* This routine copies a specified string into non-volatile RAM.
* This routine is for use at the application level (at boot,
* use sysNvRamGet).
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: sysNvRamGet()
* Remarks:
*  If offset is larger than/equal to 'SERIAL_EE_FIRST_OFFSET' then this
*  is offest in the serial eeprom (starting at 'SERIAL_EE_FIRST_OFFSET',
*  which is equivalent to '0').
*
*  Part of this procedure must be carried out uninterrupted by other potential
*  users. Make sure to not invoke any operating system
*  services that might enable interrupts (through enabling task
*  switching).
*/

STATUS
  nvRamSet(
    char *string,     /* string to be copied into non-volatile RAM */
    int strLen,       /* maximum number of bytes to copy           */
    int offset        /* byte offset into non-volatile RAM         */
    )
{
  STATUS
   ret = OK ;
  int
    err ;
  unsigned
    int
      sem_taken ;
#if !DUNE_BCM
  unsigned
    int
      block_id ;
#endif
  sem_taken = FALSE ;
#if !DUNE_BCM
  err =
    ee_offset_to_block_id(
                (unsigned int)offset,(unsigned int)strLen,&block_id) ;
  if (err)
  {
    ret = ERROR ;
    goto snra_exit ;
  }
  if (get_use_par_nv_sem())
  {
    take_par_nv_sem() ;
    sem_taken = TRUE ;
  }
  err =
    check_block_crc(block_id) ;
  if (err < 0)
  {
    ret = ERROR ;
    goto snra_exit ;
  }
  /*
   * If CRC of the block is wrong then load it with
   * default values first.
   */
  if (err)
  {
    err =
      load_defaults(block_id) ;
    if (err)
    {
      ret = ERROR ;
      goto snra_exit ;
    }
  }
#endif
  if (offset < SERIAL_EE_FIRST_OFFSET)
  {
    ret = NV_RAM_WRITE_B(offset,string,strLen) ;
    if (ret == ERROR)
    {
      goto snra_exit ;
    }
  }
  else
  {
    char
      err_msg[160] ;
    unsigned
      int
        err_flg ;
    err = ser_ee_high_image_write(
              (unsigned int)(offset - SERIAL_EE_FIRST_OFFSET),
              (unsigned int)strLen,(unsigned char*)string,
              &err_flg,err_msg
          ) ;
    if (err)
    {
      ret = ERROR ;
      goto snra_exit ;
    }
  }
#if !DUNE_BCM
  err =  update_block_crc(block_id) ;
  if (err)
  {
    ret = ERROR ;
    goto snra_exit ;
  }
#endif
snra_exit:
  if (sem_taken)
  {
    give_back_par_nv_sem() ;
  }
  return ret ;
}
/*****************************************************
*NAME
*   host_eeprom_front_card_type_from_nv
*TYPE: PROC
*DATE: 15/JAN/2002
*FUNCTION:
*  GET the Front card type when working with GFA and with
*      non-Dune front line card from the NVRAM.
*CALLING SEQUENCE:
*  sysNvRamGet
*INPUT:
*  SOC_SAND_DIRECT:
*    HOST_EEPROM_FRONT_CARD_TYPE  *host_eeprom_front_card_type_en_ptr -
*      The Front card type when working with GFA and with
*      non-Dune front line card.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
*   HOST_EEPROM_FRONT_CARD_TYPE
 */
int
  host_eeprom_front_card_type_from_nv(
    HOST_EEPROM_FRONT_CARD_TYPE  *host_eeprom_front_card_type_en_ptr
  )
{
  int             ret;
  SERIAL_EE_AREA  *serial_ee_area;
  char            block_data[sizeof(short) + 1];
  unsigned short  eeprom_front_card_type;
  unsigned int    offset;
  STATUS          status;
  ret = 0 ;
  serial_ee_area = (SERIAL_EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(serial_ee_area->
              host_ser_ee_block_02.un_host_serial_ee_block_02.
              host_serial_ee_block_02.eeprom_front_card_type)) +
              SERIAL_EE_FIRST_OFFSET ;
  status = nvRamGet(block_data,sizeof(eeprom_front_card_type),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  eeprom_front_card_type = *((unsigned short *)block_data) ;
  *host_eeprom_front_card_type_en_ptr = (HOST_EEPROM_FRONT_CARD_TYPE)eeprom_front_card_type ;
tifn_exit:
  return (ret) ;
}

/*****************************************************
*NAME
*   host_eeprom_front_card_type_from_nv
*TYPE: PROC
*DATE: 15/JAN/2002
*FUNCTION:
*  GET the Front card type when working with GFA and with
*      non-Dune front line card from the NVRAM's run time parameters.
*CALLING SEQUENCE:
*  sysNvRamGet
*INPUT:
*  SOC_SAND_DIRECT:
*    HOST_EEPROM_FRONT_CARD_TYPE  *host_eeprom_front_card_type_en_ptr -
*      The Front card type when working with GFA and with
*      non-Dune front line card.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
*   HOST_EEPROM_FRONT_CARD_TYPE
 */
int
  host_run_val_of_front_card_type(
    HOST_EEPROM_FRONT_CARD_TYPE  *host_eeprom_front_card_type_en_ptr
  )
{
  int             ret;
  SERIAL_EE_AREA  *serial_ee_area;
  char            block_data[sizeof(short) + 1];
  unsigned short  eeprom_front_card_type;
  unsigned int    offset;
  STATUS          status;
  ret = 0 ;
  serial_ee_area = (SERIAL_EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(serial_ee_area->
              host_ser_ee_block_02.un_host_serial_ee_block_02.
              host_serial_ee_block_02.eeprom_front_card_type)) +
              SERIAL_EE_FIRST_OFFSET ;
  status = get_run_vals(block_data,sizeof(eeprom_front_card_type),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  eeprom_front_card_type = *((unsigned short *)block_data) ;
  *host_eeprom_front_card_type_en_ptr = (HOST_EEPROM_FRONT_CARD_TYPE)eeprom_front_card_type ;
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   host_eeprom_front_card_type_to_nv
*TYPE: PROC
*DATE: 15/JAN/2002
*FUNCTION:
*  SET the Front card type when working with GFA and with
*      non-Dune front line card to the NVRAM.
*CALLING SEQUENCE:
*  sysNvRamSet
*INPUT:
*  SOC_SAND_DIRECT:
*    HOST_EEPROM_FRONT_CARD_TYPE  host_eeprom_front_card_type_en_ptr -
*      The Front card type when working with GFA and with
*      non-Dune front line card.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
*   HOST_EEPROM_FRONT_CARD_TYPE
 */
int
  host_eeprom_front_card_type_to_nv(
    HOST_EEPROM_FRONT_CARD_TYPE  host_eeprom_front_card_type_en
  )
{
  int             ret;
  SERIAL_EE_AREA  *serial_ee_area;
  unsigned int    offset;
  unsigned short  eeprom_front_card_type;
  STATUS          status;
  ret = 0 ;

  eeprom_front_card_type = (unsigned short)host_eeprom_front_card_type_en;
  serial_ee_area = (SERIAL_EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(serial_ee_area->
              host_ser_ee_block_02.un_host_serial_ee_block_02.
              host_serial_ee_block_02.eeprom_front_card_type)) +
              SERIAL_EE_FIRST_OFFSET ;
  status =
    nvRamSet(
      (char *)&eeprom_front_card_type,
      sizeof(eeprom_front_card_type),
      (int)offset
    );

  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}

/*****************************************************
*NAME
*   host_eeprom_front_card_type_to_nv
*TYPE: PROC
*DATE: 15/JAN/2002
*FUNCTION:
*  Return front card type name
*CALLING SEQUENCE:
*  sysNvRamSet
*INPUT:
*  SOC_SAND_DIRECT:
*    HOST_EEPROM_FRONT_CARD_TYPE  host_eeprom_front_card_type_en_ptr -
*      The Front card type when working with GFA and with
*      non-Dune front line card.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Pointer to const string.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
*   HOST_EEPROM_FRONT_CARD_TYPE
 */
const char *
  get_front_card_type_name(
    HOST_EEPROM_FRONT_CARD_TYPE  host_eeprom_front_card_type_en
  )
{
  const char
    *str;

  switch(host_eeprom_front_card_type_en){
  case HOST_EEPROM_FRONT_CARD_TYPE_NONE:
    str = "Alone";
  break;
  case HOST_EEPROM_FRONT_CARD_TYPE_X10:
    str = "X10";
  break;
  case HOST_EEPROM_FRONT_CARD_TYPE_IXP2800:
    str = "IXP2800";
  break;
  default:
    str = "Unknown";
  }

  return str;
}


/*****************************************************
*NAME
*   host_eeprom_front_card_type_to_nv
*TYPE: PROC
*DATE: 15/JAN/2002
*FUNCTION:
*  Check if IXP2800 front card is actually connected.
*  For that, it is not enough that the NVRAM indicate
*  it is, but it also need to check that the card is
*  GFA, and that no Dune's front line card is connected.
*CALLING SEQUENCE:
*  sysNvRamSet
*INPUT:
*  SOC_SAND_DIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int -
*     TRUE: IXP2800 is connected
*     TRUE: IXP2800 is not connected
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
*   HOST_EEPROM_FRONT_CARD_TYPE
 */
unsigned int
  is_front_end_ixp2800_connected(
    void
  )
{
  unsigned int
    is_it = FALSE;
  HOST_EEPROM_FRONT_CARD_TYPE
    card_type;

  host_eeprom_front_card_type_from_nv(&card_type);

#if LINK_FAP20V_LIBRARIES
  if((is_line_card_gfa_connected()                    ) &&
     (card_type == HOST_EEPROM_FRONT_CARD_TYPE_IXP2800) &&
     (!is_line_card_tgs_connected()                   ) &&
     (!is_line_card_ftg_connected()                   )
    )
  {
    is_it = TRUE;
  }
#endif
#if LINK_FAP21V_LIBRARIES
  if((gfa_fap21v_is_line_card_gfa_connected()    ) &&
     (card_type == HOST_EEPROM_FRONT_CARD_TYPE_IXP2800) &&
     (!is_line_card_tgs_connected()                   ) &&
     (!is_line_card_ftg_connected()                   )
    )
  {
    is_it = TRUE;
  }
#endif
  return is_it;
}
/*****************************************************
*NAME
*   host_eeprom_front_card_type_to_nv
*TYPE: PROC
*DATE: 15/JAN/2002
*FUNCTION:
*  Check if IXP2800 front card is actually connected.
*  For that, it is not enough that the NVRAM indicate
*  it is, but it also need to check that the card is
*  GFA, and that no Dune's front line card is connected.
*CALLING SEQUENCE:
*  sysNvRamSet
*INPUT:
*  SOC_SAND_DIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int -
*     TRUE: IXP2800 is connected
*     TRUE: IXP2800 is not connected
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
*   HOST_EEPROM_FRONT_CARD_TYPE
 */
unsigned int
  is_front_end_x10_connected(
    void
  )
{
  unsigned int
    is_it = FALSE;
  HOST_EEPROM_FRONT_CARD_TYPE
    card_type;

  host_eeprom_front_card_type_from_nv(&card_type);
#if LINK_FAP20V_LIBRARIES

  if(
     is_line_card_gfa_connected()                 &&
     card_type == HOST_EEPROM_FRONT_CARD_TYPE_X10 &&
     !is_line_card_tgs_connected()                &&
     !is_line_card_ftg_connected()
    )
  {
    is_it = TRUE;
  }
#endif /*#if LINK_FAP20V_LIBRARIES*/
#if LINK_FAP21V_LIBRARIES
  if(
     gfa_fap21v_is_line_card_gfa_connected() &&
     card_type == HOST_EEPROM_FRONT_CARD_TYPE_X10 &&
     !is_line_card_tgs_connected()                &&
     !is_line_card_ftg_connected()
    )
  {
    is_it = TRUE;
  }
#endif /*#if LINK_FAP21V_LIBRARIES*/

  return is_it;
}

/*
 * HOST_DB-board related proccedures
 * {
 */
/*****************************************************
*NAME
*   host_db_board_cat_number_from_nv
*TYPE: PROC
*DATE: 03/DEC/2004
*FUNCTION:
*  GET HOST_DB BOARD'S HARDWARE CATALOG NUMBER FROM NV RAM.
*CALLING SEQUENCE:
*  hoat_board_cat_number_from_nv(board_serial_num_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    char  *board_serial_num_ptr -
*      This procedure loads the pointed address with
*      value of serial number of board as stored
*      in serial NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of serial nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  host_db_board_cat_number_from_nv(
    char  *board_serial_num_ptr
    )
{
  int             ret;
  SERIAL_EE_AREA  *serial_ee_area;
  unsigned int    offset;
  STATUS          status;
  ret = 0 ;
  serial_ee_area = (SERIAL_EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(serial_ee_area->
              host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
              host_db_serial_ee_block_02.host_db_board_catalog_number[0])) +
              SERIAL_EE_FIRST_OFFSET ;
  status =
    nvRamGet(
      board_serial_num_ptr,B_HOST_DB_MAX_CATALOG_NUMBER_LEN,(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   host_db_board_description_from_nv
*TYPE: PROC
*DATE: 03/DEC/2004
*FUNCTION:
*  GET HOST_DB BOARD'S HARDWARE DESCRIPTION FROM NV RAM.
*CALLING SEQUENCE:
*  host_db_board_name_from_nv(board_description_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    char  *board_description_ptr -
*      This procedure loads the pointed address with
*      value of string describing hardware of board as
*      stored in serial NV ram.
*      Caller must prepare an array of size
*      'B_HOST_DB_MAX_BOARD_DESCRIPTION_LEN + 1' or more.
*  SOC_SAND_INDIRECT:
*    internal structure of serial nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  host_db_board_description_from_nv(
    char  *board_description_ptr
    )
{
  int             ret;
  SERIAL_EE_AREA  *serial_ee_area;
  unsigned int    offset;
  STATUS          status;
  ret = 0 ;
  serial_ee_area = (SERIAL_EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(serial_ee_area->
              host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
              host_db_serial_ee_block_02.host_db_board_description[0])) +
              SERIAL_EE_FIRST_OFFSET ;
  status =
    nvRamGet(
      board_description_ptr,B_HOST_DB_MAX_BOARD_DESCRIPTION_LEN,(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit;
  }
exit:
  return (ret);
}
/*****************************************************
*NAME
*   host_db_board_version_from_nv
*TYPE: PROC
*DATE: 03/DEC/2004
*FUNCTION:
*  GET HOST_DB BOARD'S HARDWARE VERSION FROM NV RAM.
*CALLING SEQUENCE:
*  host_db_board_version_from_nv(board_version_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    char  *board_version_ptr -
*      This procedure loads the pointed address with
*      value of version of board as stored
*      in serial NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of serial nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  host_db_board_version_from_nv(
    char *board_version_ptr
    )
{
  int             ret ;
  SERIAL_EE_AREA  *serial_ee_area ;
  unsigned int    offset ;
  STATUS          status ;
  ret = 0 ;
  serial_ee_area = (SERIAL_EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(serial_ee_area->
              host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
              host_db_serial_ee_block_02.host_db_board_version[0])) +
              SERIAL_EE_FIRST_OFFSET ;
  status =
    nvRamGet(
      board_version_ptr,B_HOST_DB_MAX_DEFAULT_BOARD_VERSION_LEN,(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   host_db_board_sn_from_nv
*TYPE: PROC
*DATE: 03/DEC/2004
*FUNCTION:
*  GET HOST_DB BOARD'S SOFTWARE VERSION FROM NV RAM.
*CALLING SEQUENCE:
*  host_db_board_sn_from_nv(host_db_board_serial_number_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned short  *host_db_board_serial_number_ptr -
*      This procedure loads the pointed address with
*      value of serial number of board as stored in serial NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of serial nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  host_db_board_sn_from_nv(
    unsigned short  *host_db_board_serial_number_ptr
    )
{
  int             ret;
  SERIAL_EE_AREA  *serial_ee_area;
  char            block_data[sizeof(short) + 1];
  unsigned short  serial_number;
  unsigned int    offset;
  STATUS          status;
  ret = 0 ;
  serial_ee_area = (SERIAL_EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(serial_ee_area->
              host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
              host_db_serial_ee_block_02.host_db_board_serial_number)) +
              SERIAL_EE_FIRST_OFFSET ;
  status = nvRamGet(block_data,sizeof(serial_number),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  serial_number = *((unsigned short *)block_data) ;
  *host_db_board_serial_number_ptr = serial_number ;
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   host_db_board_param_i_from_nv
*TYPE: PROC
*DATE: 03/DEC/2004
*FUNCTION:
*  GET HOST_DB BOARD'S HARDWARE TYPE FROM NV RAM.
*CALLING SEQUENCE:
*  host_db_board_param_i_from_nv(i, board_name_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int i - 1 to 10
*    char  *board_param_i_ptr
*      This procedure loads the pointed address with
*      value of string specifying type of board as
*      stored in serial NV ram.
*      Caller must prepare an array of size
*      'B_HOST_DB_MAX_PARAM_LEN + 1' or more.
*  SOC_SAND_INDIRECT:
*    internal structure of serial nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  host_db_board_param_i_from_nv(
    unsigned int i,
    char  *board_param_i_ptr
    )
{
  int             ret;
  SERIAL_EE_AREA  *serial_ee_area;
  unsigned int    offset;
  STATUS          status;
  ret = 0;
  if (i==0 || i>10)
  {
    ret = 1;
    goto exit;
  }
  serial_ee_area = (SERIAL_EE_AREA *)0;
  offset = (unsigned int)((char *)&(serial_ee_area->
              host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
              host_db_serial_ee_block_02.host_db_board_param_1[0])) +
              + ( (i-1) * B_HOST_DB_MAX_PARAM_LEN ) + SERIAL_EE_FIRST_OFFSET;
  status = nvRamGet(board_param_i_ptr,B_HOST_DB_MAX_PARAM_LEN,(int)offset) ;
  if (status == ERROR)
  {
    ret = 2 ;
    goto exit;
  }
exit:
  return (ret);
}
/*****************************************************
*NAME
*   host_db_board_type_from_nv
*TYPE: PROC
*DATE: 03/DEC/2004
*FUNCTION:
*  GET HOST_DB BOARD'S HARDWARE TYPE FROM NV RAM.
*CALLING SEQUENCE:
*  host_db_board_type_from_nv(board_type_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned short  *board_type_ptr -
*      This procedure loads the pointed address with
*      the type of board as stored in serial NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of serial nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  host_db_board_type_from_nv(
    unsigned short  *board_type_ptr
    )
{
  int             ret;
  SERIAL_EE_AREA  *serial_ee_area ;
  char            block_data[sizeof(short) + 1] ;
  unsigned short  board_type;
  unsigned int    offset;
  STATUS          status;
  ret = 0 ;
  serial_ee_area = (SERIAL_EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(serial_ee_area->
              host_db_ser_ee_block_01.un_host_db_serial_ee_block_01.
              host_db_serial_ee_block_01.host_db_board_type)) +
              SERIAL_EE_FIRST_OFFSET ;
  status = nvRamGet(block_data,sizeof(board_type),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
  board_type = *((unsigned short *)block_data) ;
  *board_type_ptr = board_type ;
exit:
  return (ret) ;
}


/*
 * }
 */


/*****************************************************
*NAME
*   host_front_end_type_from_nv
*TYPE: PROC
*DATE: 08/Mar/2007
*FUNCTION:
*  Read front end type from NvRam
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_BSP_CARDS_DEFINES  *board_type_ptr -
*      This procedure loads the pointed address with
*      the type of board as stored in serial NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of serial nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  host_front_end_type_from_nv(
    SOC_BSP_CARDS_DEFINES  *board_type_ptr
    )
{
  int
    ret;
  unsigned short
    nv_ram_val;
  unsigned int
    offset,
    size;
  STATUS
    status;

  ret = 0 ;

  offset =
   (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            front_ser_ee_block_01.un_front_serial_ee_block_01.
            front_serial_ee_block_01.front_board_type)) +
            SERIAL_EE_FIRST_OFFSET ; /*need to be 1024 */
  size =
    sizeof(((SERIAL_EE_AREA *)0)->
            front_ser_ee_block_01.un_front_serial_ee_block_01.
            front_serial_ee_block_01.front_board_type) ;

  status = nvRamGet((char *)(&nv_ram_val),(int)size,(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }

  *board_type_ptr = nv_ram_val;

exit:
  return (ret) ;
}

