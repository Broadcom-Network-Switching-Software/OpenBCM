/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/**************************************************************************************
 **************************************************************************************
 *  File Name     :  merlin7_tsc_field_access.h                                        *
 *  Created On    :  29/04/2013                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Serdes IP Register and Field access APIs                         *
 *  Revision      :   *
 */

/** @file merlin7_tsc_field_access.h
 * Registers and field access
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MERLIN7_TSC_API_FIELD_ACCESS_H
#define MERLIN7_TSC_API_FIELD_ACCESS_H

#include "merlin7_tsc_ipconfig.h"
#include "common/srds_api_err_code.h"
#include "merlin7_tsc_dependencies.h"

/* Extract a bitfield from a register value. */
#define extract_field(reg_value,msb,lsb) (((uint16_t)(reg_value) & ((1 << ((msb) + 1)) - 1)) >> (lsb))

/** Read a register field as an unsigned value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param shift_left  Number of bits sign_bit should be moved to the left [shift_left  = (15 - msb)]
 * @param shift_right Number of right shifts to move field to bit0        [shift_right = (15 - msb + shift_right)]
 * @param *val_p 16-bit unsigned value read from the field
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _merlin7_tsc_pmd_rdt_field(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint16_t *val_p);

/** Read a register field as an signed value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param shift_left  Number of bits sign_bit should be moved to the left [shift_left  = (15 - msb)]
 * @param shift_right Number of right shifts to move field to bit0        [shift_right = (15 - msb + lsb)]
 * @param *val_p 16-bit signed value read from the field
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _merlin7_tsc_pmd_rdt_field_signed(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, int16_t *val_p);


/*-------------------------------*/
/* Byte Write and Read Functions */
/*-------------------------------*/

/** Write a contiguous bit field in a register.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param mask 16-bit mask indicating the position of the field with bits of 1s
 * @param lsb  LSB of the field, the width of the field is implied by mask.
 * @param val  8bit value to write into the field.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _merlin7_tsc_pmd_mwr_reg_byte(srds_access_t *sa__, uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val);

/** Read a register field as an 8-bit unsigned value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param shift_left  Number of bits sign_bit should be moved to the left [shift_left  = (15 - msb)]
 * @param shift_right Number of right shifts to move field to bit0        [shift_right = (15 - msb + lsb)]
 * @param *val_p 8-bit unsigned value read from the field
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _merlin7_tsc_pmd_rdt_field_byte(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint8_t *val_p);

/** Read a register field as an 8-bit signed value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param shift_left  Number of bits sign_bit should be moved to the left [shift_left  = (15 - msb)]
 * @param shift_right Number of right shifts to move field to bit0        [shift_right = (15 - msb + lsb)]
 * @param *val_p 8-bit signed value read from the field
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _merlin7_tsc_pmd_rdt_field_signed_byte(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, int8_t *val_p);


/*****************************************************************************
   The following functions are being retained for legacy purposes and
   for Serdes DVT, characterization backwards compatability
 */

/** Read a register.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param err_code_p error code returned through pointer
 * @return 16-bit value read from the register
 */
uint16_t _merlin7_tsc_pmd_rde_reg(srds_access_t *sa__, uint16_t addr, err_code_t *err_code_p);

/** Read a register field as an unsigned value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param shift_left  Number of bits sign_bit should be moved to the left [shift_left  = (15 - msb)]
 * @param shift_right Number of right shifts to move field to bit0        [shift_right = (15 - msb + lsb)]
 * @param err_code_p error code returned through pointer
 * @return 16-bit unsigned value read from the field
 */
uint16_t _merlin7_tsc_pmd_rde_field(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);

/** Read a register field as a signed value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param shift_left  Number of bits sign_bit should be moved to the left [shift_left  = (15 - msb)]
 * @param shift_right Number of right shifts with sign extension          [shift_right = (15 - msb + lsb)]
 * @param err_code_p error code returned through pointer
 * @return 16-bit signed value read from the field
 */
int16_t  _merlin7_tsc_pmd_rde_field_signed(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);

/** Read a bit field as an unsigned value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param shift_left  Number of bits sign_bit should be moved to the left [shift_left  = (15 - msb)]
 * @param shift_right Number of right shifts to move field to bit0        [shift_right = (15 - msb + lsb)]
 * @param err_code_p error code returned through pointer
 * @return 8-bit unsigned value read from the field
 */
uint8_t _merlin7_tsc_pmd_rde_field_byte(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);

/** Read a bit field as a signed value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr 16-bit register address.
 * @param shift_left  Number of bits sign bit should be moved to the left [shift_left  = (15 - msb)]
 * @param shift_right Number of right shifts with sign extension          [shift_right = (15 - msb + lsb)]
 * @param err_code_p error code returned through pointer
 * @return 8-bit signed value read from the field
 */
int8_t  _merlin7_tsc_pmd_rde_field_signed_byte(srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);

#endif /* FIELD_ACCESS_H_ */
#ifdef __cplusplus
}
#endif
