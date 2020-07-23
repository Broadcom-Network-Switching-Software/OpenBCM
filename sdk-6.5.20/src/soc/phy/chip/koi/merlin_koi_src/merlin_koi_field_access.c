/**************************************************************************************
 **************************************************************************************
 *  File Name     :  merlin_koi_field_access.c                                        *
 *  Created On    :  29/04/2013                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  APIs to access Serdes IP Registers and Reg fields                *
 *  Revision      :  $Id: merlin_koi_field_access.c 1062 2015-06-02 22:12:41Z kirand $ *
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

/** @file merlin_koi_field_access.c
 * Registers and field access
 */

#include "merlin_koi_field_access.h"
#include "merlin_koi_dependencies.h"
#include "merlin_koi_internal.h"

err_code_t _merlin_koi_pmd_rdt_field(uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint16_t *val_p) {
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(merlin_koi_pmd_rdt_reg(addr,val_p));
  *val_p <<= shift_left;                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  *val_p >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

err_code_t _merlin_koi_pmd_rdt_field_signed(uint16_t addr, uint8_t shift_left, uint8_t shift_right, int16_t *val_p) {
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : Signed MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(merlin_koi_pmd_rdt_reg(addr,(uint16_t *)val_p));
  *val_p <<= shift_left;                   /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
  *val_p >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

/*-------------------------------*/
/* Byte Write and Read Functions */
/*-------------------------------*/

err_code_t _merlin_koi_pmd_mwr_reg_byte(uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val) {

  EFUN(merlin_koi_pmd_mwr_reg(addr, mask, lsb, (uint16_t) val));

  return(ERR_CODE_NONE);
}

  err_code_t _merlin_koi_pmd_rdt_field_byte(uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint8_t *val8_p) {

  uint16_t val;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(merlin_koi_pmd_rdt_reg(addr,&val));
  val <<= shift_left;                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  val >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  *val8_p = (uint8_t) val;

  return(ERR_CODE_NONE);
}

  err_code_t _merlin_koi_pmd_rdt_field_signed_byte(uint16_t addr, uint8_t shift_left, uint8_t shift_right, int8_t *val8_p) {

  int16_t val;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : Signed MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(merlin_koi_pmd_rdt_reg(addr,(uint16_t *) &val));
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

uint16_t _merlin_koi_pmd_rde_reg(uint16_t addr, err_code_t *err_code_p)
{
  uint16_t data;
  EPFUN(merlin_koi_pmd_rdt_reg(addr, &data));
  return data;
}

uint16_t _merlin_koi_pmd_rde_field(uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p)
{
  uint16_t data;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif
  EPSTM(data = _merlin_koi_pmd_rde_reg(addr, err_code_p));

  data <<= shift_left;                 /* Move the sign bit to the left most [shift_left  = (15-msb)] */
  data >>= shift_right;                /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return data;
}

int16_t _merlin_koi_pmd_rde_field_signed(uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p)
{
  int16_t  data;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : Signed MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif
  EPSTM(data = (int16_t) _merlin_koi_pmd_rde_reg(addr, err_code_p));  /* convert it to signed */

  data <<= shift_left;             /* Move the sign bit to the left most    [shift_left  = (15-msb)] */
  data >>= shift_right;            /* Move to the right with sign extension [shift_right = (15-msb+lsb)] */

  return data;
}

uint8_t _merlin_koi_pmd_rde_field_byte(uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
   return ((uint8_t) _merlin_koi_pmd_rde_field(addr, shift_left, shift_right, err_code_p));
}

int8_t _merlin_koi_pmd_rde_field_signed_byte(uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
  return ((int8_t) _merlin_koi_pmd_rde_field_signed(addr, shift_left, shift_right, err_code_p));
}
