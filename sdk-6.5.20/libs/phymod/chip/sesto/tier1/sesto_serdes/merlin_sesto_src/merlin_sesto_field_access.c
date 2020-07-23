/**************************************************************************************
 **************************************************************************************
 *  File Name     :  merlin_sesto_field_access.c                                        *
 *  Created On    :  29/04/2013                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  APIs to access Serdes IP Registers and Reg fields                *
 *  Revision      :  $Id: merlin_sesto_field_access.c 661 2014-08-14 23:20:49Z kirand $ *
 *                                                                                    *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                              *
 *  No portions of this material may be reproduced in any form without                *
 *  the written permission of:                                                        *
 *      Broadcom Corporation                                                          *
 *      5300 California Avenue                                                        *
 *      Irvine, CA  92617                                                             *
 *                                                                                    *
 *  All information contained in this document is Broadcom Corporation                *
 *  company private proprietary, and trade secret.                                    *
 */

/** @file merlin_sesto_field_access.c
 * Registers and field access
 */

#include "merlin_sesto_field_access.h"
#include "merlin_sesto_dependencies.h"
#include "merlin_sesto_ipconfig.h"
#include "merlin_sesto_internal.h"

err_code_t _merlin_sesto_pmd_rdt_field( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint16_t *val_p) {

  EFUN(merlin_sesto_pmd_rdt_reg(pa,addr,val_p));
  *val_p <<= shift_left;                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  *val_p >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

err_code_t _merlin_sesto_pmd_rdt_field_signed( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, int16_t *val_p) {
  
  EFUN(merlin_sesto_pmd_rdt_reg(pa,addr,(uint16_t *)val_p));
  *val_p <<= shift_left;                   /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
  *val_p >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

/*-------------------------------*/
/* Byte Write and Read Functions */
/*-------------------------------*/

err_code_t _merlin_sesto_pmd_mwr_reg_byte( const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val) {

  EFUN(merlin_sesto_pmd_mwr_reg(pa,addr, mask, lsb, (uint16_t) val)); 

  return(ERR_CODE_NONE);  
} 

  err_code_t _merlin_sesto_pmd_rdt_field_byte( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint8_t *val8_p) {

  uint16_t val;
  
  EFUN(merlin_sesto_pmd_rdt_reg(pa,addr,&val));
  val <<= shift_left;                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  val >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  *val8_p = (uint8_t) val;

  return(ERR_CODE_NONE);
}

  err_code_t _merlin_sesto_pmd_rdt_field_signed_byte( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, int8_t *val8_p) {

  int16_t val;

  EFUN(merlin_sesto_pmd_rdt_reg(pa,addr,(uint16_t *) &val));
  val <<= shift_left;                   /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
  val >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  *val8_p = (int8_t) val;

  return(ERR_CODE_NONE);
}

/*-------------------------------*/
/* MDIO specific Misc functions  */
/*-------------------------------*/



/*-------------------------------*/
/* EVAL specific functions  */
/*-------------------------------*/

uint16_t _merlin_sesto_pmd_rde_reg(const phymod_access_t *pa,uint16_t addr, err_code_t *err_code_p)
{
  uint16_t data;
  EFUN(merlin_sesto_pmd_rdt_reg(pa,addr, &data));
  return data;
}

uint16_t _merlin_sesto_pmd_rde_field(const phymod_access_t *pa,uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p)
{
  uint16_t data;
  EPSTM(data = _merlin_sesto_pmd_rde_reg( pa, addr, err_code_p));

  data <<= shift_left;                 /* Move the sign bit to the left most [shift_left  = (15-msb)] */
  data >>= shift_right;                /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return data;
}

int16_t _merlin_sesto_pmd_rde_field_signed(const phymod_access_t *pa,uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p)
{
  int16_t  data;
  EPSTM(data = (int16_t) _merlin_sesto_pmd_rde_reg( pa, addr, err_code_p));  /* convert it to signed */

  data <<= shift_left;             /* Move the sign bit to the left most    [shift_left  = (15-msb)] */
  data >>= shift_right;            /* Move to the right with sign extension [shift_right = (15-msb+lsb)] */

  return data;
}

uint8_t _merlin_sesto_pmd_rde_field_byte( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
   return ((uint8_t) _merlin_sesto_pmd_rde_field( pa, addr, shift_left, shift_right, err_code_p));
}

int8_t _merlin_sesto_pmd_rde_field_signed_byte( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
  return ((int8_t) _merlin_sesto_pmd_rde_field_signed( pa, addr, shift_left, shift_right, err_code_p));
}
