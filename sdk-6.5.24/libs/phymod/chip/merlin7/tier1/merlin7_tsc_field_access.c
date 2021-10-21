/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/**************************************************************************************
 **************************************************************************************
 *  File Name     :  merlin7_tsc_field_access.c                                        *
 *  Created On    :  29/04/2013                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  APIs to access Serdes IP Registers and Reg fields                *
 *  Revision      :   *
 */

/** @file merlin7_tsc_field_access.c
 * Registers and field access
 */

#include "merlin7_tsc_field_access.h"
#include "merlin7_tsc_functions.h"
#include "merlin7_tsc_internal.h"
#include "merlin7_tsc_internal_error.h"
#include "merlin7_tsc_dependencies.h"

/* If SERDES_EVAL is defined, then is_ate_log_enabled() is queried to *\
\* know whether to log ATE.  merlin7_tsc_access.h provides that function.  */

err_code_t _merlin7_tsc_pmd_rdt_field(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint16_t *val_p) {
    INIT_SRDS_ERR_CODE

  EFUN(merlin7_tsc_pmd_rdt_reg(sa__, addr,val_p));
  *val_p = (uint16_t)(*val_p << shift_left);                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  *val_p = (uint16_t)(*val_p >> shift_right);                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

err_code_t _merlin7_tsc_pmd_rdt_field_signed(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, int16_t *val_p) {
    INIT_SRDS_ERR_CODE

  EFUN(merlin7_tsc_pmd_rdt_reg(sa__, addr,(uint16_t *)val_p));
  *val_p = (int16_t)(*val_p << shift_left);                   /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
  *val_p = (int16_t)(*val_p >> shift_right);                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return(ERR_CODE_NONE);
}

/*-------------------------------*/
/* Byte Write and Read Functions */
/*-------------------------------*/

err_code_t _merlin7_tsc_pmd_mwr_reg_byte(srds_access_t *sa__, uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val) {
    INIT_SRDS_ERR_CODE


  EFUN(merlin7_tsc_pmd_mwr_reg(sa__, addr, mask, lsb, (uint16_t) val));

  return(ERR_CODE_NONE);
}

err_code_t _merlin7_tsc_pmd_rdt_field_byte(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint8_t *val8_p) {
    INIT_SRDS_ERR_CODE
  uint16_t val = 0;

  EFUN(merlin7_tsc_pmd_rdt_reg(sa__, addr,&val));

  val = (uint16_t)(val << shift_left);                   /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
  val = (uint16_t)(val >> shift_right);                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  *val8_p = (uint8_t) val;

  return(ERR_CODE_NONE);
}

err_code_t _merlin7_tsc_pmd_rdt_field_signed_byte(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, int8_t *val8_p) {
    INIT_SRDS_ERR_CODE
  int16_t val = 0;

  EFUN(merlin7_tsc_pmd_rdt_reg(sa__, addr,(uint16_t *) &val));

  val = (int16_t)(val << shift_left);                   /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
  val = (int16_t)(val >> shift_right);                  /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  *val8_p = (int8_t) val;
  return(ERR_CODE_NONE);
}


/*-------------------------------*/
/* EVAL specific functions  */
/*-------------------------------*/

uint16_t _merlin7_tsc_pmd_rde_reg(srds_access_t *sa__, uint16_t addr, err_code_t *err_code_p) {
  INIT_SRDS_ERR_CODE
  uint16_t data = 0;

  EPFUN(merlin7_tsc_pmd_rdt_reg(sa__, addr, &data));

    return data;
}

uint16_t _merlin7_tsc_pmd_rde_field(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
  INIT_SRDS_ERR_CODE
  uint16_t data;

  EPSTM(data = _merlin7_tsc_pmd_rde_reg(sa__, addr, err_code_p));

  data = (uint16_t)(data << shift_left);                 /* Move the sign bit to the left most [shift_left  = (15-msb)] */
  data = (uint16_t)(data >> shift_right);                /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

  return data;
}

int16_t _merlin7_tsc_pmd_rde_field_signed(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
  INIT_SRDS_ERR_CODE
  int16_t  data;

  EPSTM(data = (int16_t) _merlin7_tsc_pmd_rde_reg(sa__, addr, err_code_p));  /* convert it to signed */

  data = (int16_t)(data << shift_left);             /* Move the sign bit to the left most    [shift_left  = (15-msb)] */
  data = (int16_t)(data >> shift_right);            /* Move to the right with sign extension [shift_right = (15-msb+lsb)] */

  return data;
}

uint8_t _merlin7_tsc_pmd_rde_field_byte(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
    return ((uint8_t) _merlin7_tsc_pmd_rde_field(sa__, addr, shift_left, shift_right, err_code_p));
}

int8_t _merlin7_tsc_pmd_rde_field_signed_byte(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p) {
    return ((int8_t) _merlin7_tsc_pmd_rde_field_signed(sa__, addr, shift_left, shift_right, err_code_p));
}
