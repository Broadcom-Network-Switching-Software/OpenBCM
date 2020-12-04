/**************************************************************************************
 **************************************************************************************
 *  File Name     :  eagle_phy_1_field_access.c                                        *
 *  Created On    :  29/04/2013                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  APIs to access Serdes IP Registers and Reg fields                *
 *  Revision      :  $Id: eagle_phy_1_field_access.c 1374 2016-03-12 01:19:55Z cvazquez $ *
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

/** @file eagle_phy_1_field_access.c
 * Registers and field access
 */

#include "eagle_phy_1_field_access.h"
#include "eagle_phy_1_dependencies.h"
#include "eagle_phy_1_ipconfig.h"
#include "eagle_phy_1_internal.h"

err_code_t _eagle_phy_1_pmd_rdt_field(uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint16_t *val_p) {
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(eagle_phy_1_pmd_rdt_reg(addr,val_p));
  *val_p <<= shift_left;                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  *val_p >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

err_code_t _eagle_phy_1_pmd_rdt_field_signed(uint16_t addr, uint8_t shift_left, uint8_t shift_right, int16_t *val_p) {
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : Signed MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(eagle_phy_1_pmd_rdt_reg(addr,(uint16_t *)val_p));
  *val_p <<= shift_left;                   /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
  *val_p >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

/*-------------------------------*/
/* Byte Write and Read Functions */
/*-------------------------------*/

err_code_t _eagle_phy_1_pmd_mwr_reg_byte(uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val) {

  EFUN(eagle_phy_1_pmd_mwr_reg(addr, mask, lsb, (uint16_t) val));

  return(ERR_CODE_NONE);
}

  err_code_t _eagle_phy_1_pmd_rdt_field_byte(uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint8_t *val8_p) {

  uint16_t val;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(eagle_phy_1_pmd_rdt_reg(addr,&val));
  val <<= shift_left;                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  val >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  *val8_p = (uint8_t) val;

  return(ERR_CODE_NONE);
}

err_code_t _eagle_phy_1_pmd_rdt_field_signed_byte(uint16_t addr, uint8_t shift_left, uint8_t shift_right, int8_t *val8_p) {

  int16_t val;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : Signed MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(eagle_phy_1_pmd_rdt_reg(addr,(uint16_t *) &val));
  val <<= shift_left;                   /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
  val >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  *val8_p = (int8_t) val;

  return(ERR_CODE_NONE);
}

/*-------------------------------*/
/* MDIO specific Misc functions  */
/*-------------------------------*/

err_code_t eagle_phy_1_pmd_mdio_mwr_reg(uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {

  /* Can't use "wrc_mdio_maskdata(~mask)" to avoid circular function calls */
#if              defined(MDIO_MMDSEL_AER_COM_MDIO_MASKDATA)
  EFUN(eagle_phy_1_pmd_wr_reg(MDIO_MMDSEL_AER_COM_MDIO_MASKDATA, ~mask));
#elif            defined(DIG_COM_MASKDATA_REG)
  EFUN(eagle_phy_1_pmd_wr_reg(DIG_COM_MASKDATA_REG, ~mask));
#else
#error "SERDES_MDIO_MASKED_WRITE enabled without mask register specified."
#endif
  EFUN(eagle_phy_1_pmd_wr_reg(addr, val<<lsb));


  return(ERR_CODE_NONE);
}


/*--------------------------*/
/* EVAL specific functions  */
/*--------------------------*/

uint16_t _eagle_phy_1_pmd_rde_reg(uint16_t addr, err_code_t *err_code_p)
{
  uint16_t data;
  EPFUN(eagle_phy_1_pmd_rdt_reg(addr, &data));
  return data;
}

uint16_t _eagle_phy_1_pmd_rde_field(uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p)
{
  uint16_t data;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif
  EPSTM(data = _eagle_phy_1_pmd_rde_reg(addr, err_code_p));

  data <<= shift_left;                 /* Move the sign bit to the left most [shift_left  = (15-msb)] */
  data >>= shift_right;                /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return data;
}

int16_t _eagle_phy_1_pmd_rde_field_signed(uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p)
{
  int16_t  data;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%04x : Signed MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif
  EPSTM(data = (int16_t) _eagle_phy_1_pmd_rde_reg(addr, err_code_p));  /* convert it to signed */

  data <<= shift_left;             /* Move the sign bit to the left most    [shift_left  = (15-msb)] */
  data >>= shift_right;            /* Move to the right with sign extension [shift_right = (15-msb+lsb)] */

  return data;
}

uint8_t _eagle_phy_1_pmd_rde_field_byte(uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
   return ((uint8_t) _eagle_phy_1_pmd_rde_field(addr, shift_left, shift_right, err_code_p));
}

int8_t _eagle_phy_1_pmd_rde_field_signed_byte(uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
  return ((int8_t) _eagle_phy_1_pmd_rde_field_signed(addr, shift_left, shift_right, err_code_p));
}



/*--------------------------*/
/* 32-bit-address Functions */
/*--------------------------*/

err_code_t _eagle_phy_1_pmd_rdt_field_addr32(uint32_t addr, uint8_t shift_left, uint8_t shift_right, uint16_t *val_p) {
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%08lx : MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(eagle_phy_1_pmd_rdt_reg_addr32(addr,val_p));
  *val_p <<= shift_left;                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  *val_p >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

err_code_t _eagle_phy_1_pmd_rdt_field_signed_addr32(uint32_t addr, uint8_t shift_left, uint8_t shift_right, int16_t *val_p) {
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%08lx : Signed MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(eagle_phy_1_pmd_rdt_reg_addr32(addr,(uint16_t *)val_p));
  *val_p <<= shift_left;                   /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
  *val_p >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

/*-------------------------------*/
/* Byte Write and Read Functions */
/*-------------------------------*/

err_code_t _eagle_phy_1_pmd_mwr_reg_byte_addr32(uint32_t addr, uint16_t mask, uint8_t lsb, uint8_t val) {

  EFUN(eagle_phy_1_pmd_mwr_reg_addr32(addr, mask, lsb, (uint16_t) val));

  return(ERR_CODE_NONE);
}

err_code_t _eagle_phy_1_pmd_rdt_field_byte_addr32(uint32_t addr, uint8_t shift_left, uint8_t shift_right, uint8_t *val8_p) {

  uint16_t val;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%08lx : MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(eagle_phy_1_pmd_rdt_reg_addr32(addr,&val));
  val <<= shift_left;                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  val >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  *val8_p = (uint8_t) val;

  return(ERR_CODE_NONE);
}

err_code_t _eagle_phy_1_pmd_rdt_field_signed_byte_addr32(uint32_t addr, uint8_t shift_left, uint8_t shift_right, int8_t *val8_p) {

  int16_t val;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%08lx : Signed MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif

  EFUN(eagle_phy_1_pmd_rdt_reg_addr32(addr,(uint16_t *) &val));
  val <<= shift_left;                   /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
  val >>= shift_right;                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  *val8_p = (int8_t) val;

  return(ERR_CODE_NONE);
}

/*-------------------------*/
/* EVAL specific functions */
/*-------------------------*/

uint16_t _eagle_phy_1_pmd_rde_reg_addr32(uint32_t addr, err_code_t *err_code_p)
{
  uint16_t data;
  EPFUN(eagle_phy_1_pmd_rdt_reg_addr32(addr, &data));
  return data;
}

uint16_t _eagle_phy_1_pmd_rde_field_addr32(uint32_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p)
{
  uint16_t data;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%08lx : MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif
  EPSTM(data = _eagle_phy_1_pmd_rde_reg_addr32(addr, err_code_p));

  data <<= shift_left;                 /* Move the sign bit to the left most [shift_left  = (15-msb)] */
  data >>= shift_right;                /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return data;
}

int16_t _eagle_phy_1_pmd_rde_field_signed_addr32(uint32_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p)
{
  int16_t  data;
#ifdef ATE_LOG
    EFUN_PRINTF(("// ATE_LOG Addr x%08lx : Signed MASK x%04x : Right Shift %d\n",addr,(((uint16_t)(0xFFFF<<shift_left))>>shift_right)<<(shift_right-shift_left),shift_right-shift_left));
#endif
  EPSTM(data = (int16_t) _eagle_phy_1_pmd_rde_reg_addr32(addr, err_code_p));  /* convert it to signed */

  data <<= shift_left;             /* Move the sign bit to the left most    [shift_left  = (15-msb)] */
  data >>= shift_right;            /* Move to the right with sign extension [shift_right = (15-msb+lsb)] */

  return data;
}

uint8_t _eagle_phy_1_pmd_rde_field_byte_addr32(uint32_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
   return ((uint8_t) _eagle_phy_1_pmd_rde_field_addr32(addr, shift_left, shift_right, err_code_p));
}

int8_t _eagle_phy_1_pmd_rde_field_signed_byte_addr32(uint32_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
  return ((int8_t) _eagle_phy_1_pmd_rde_field_signed_addr32(addr, shift_left, shift_right, err_code_p));
}
