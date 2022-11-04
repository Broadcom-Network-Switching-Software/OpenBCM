/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * unified access definitions
 * Macros used to access either new access or old/mcm access
 */

#ifndef _UA_H
#define _UA_H

#include <shared/error.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#endif /* BCM_ACCESS_SUPPORT */

#if !defined(BCM_ACCESS_SUPPORT) || !defined(NO_MCM)
#include <soc/register.h>
#define __UA_HAS_OLD_ACCESS
#endif

/*
 * Unified Access Macro for plain register access:
 * Macros will return success (_SHR_E_NONE / zero) or a _SHR_E_* error code.
 * Register_name, field_name : literals (not  a string) as the macros will concatenate them
 * Register/field Values: uint32 pointers
 * Block_instance number, arrar index: none-negative integer values.
 * Return value: standard _shr_error_t success/failure codes.
 *
 * For per-port register teh blck_instance_number should contain a logical port number
*/


/* internal macros */
#ifdef __UA_HAS_OLD_ACCESS

/* read or write a register */
#define _UA_OLD_REG_READ(unit, register_name, block_instance_number, array_index, reg_value_out, instance_flag) \
    soc_reg_above_64_get_actual_size((unit), register_name##r, (block_instance_number) | instance_flag, (array_index), (uint32*)(reg_value_out))
#define _UA_OLD_REG_WRITE(unit, register_name, block_instance_number, array_index, reg_value_in, instance_flag) \
    soc_reg_above_64_set_actual_size((unit), register_name##r, (block_instance_number) | instance_flag, (array_index), (uint32*)(reg_value_in))
/* set or get a field in/from a register value */
#define _UA_OLD_REG_FIELD_SET(unit, register_name, reg_value_inout, field_name, field_value_in) \
    soc_reg_above_64_field_set_actual_size((unit), register_name##r, (uint32*)(reg_value_inout), field_name##f, (uint32*)(field_value_in))
#define _UA_OLD_REG_FIELD_GET(unit, register_name, reg_value_in, field_name, field_value_out) \
    soc_reg_above_64_field_get_actual_size((unit), register_name##r, (uint32*)(reg_value_in), field_name##f, (uint32*)(field_value_out))
/* check if a register or if a field of a register is valid for the unit */
#define _UA_OLD_REG_IS_VALID(unit, register_name) SOC_REG_IS_VALID((unit), register_name##r)
#define _UA_OLD_REG_FIELD_IS_VALID(unit, register_name, field_name) SOC_REG_FIELD_VALID((unit), register_name##r, field_name##f)
/* Return the width in bits of a register field. Returns zero for a none-vlaid fields */
#define _UA_OLD_REG_FIELD_SIZE(unit, register_name, field_name) soc_reg_field_length((unit), register_name##r, field_name##f)
#endif /* __UA_HAS_OLD_ACCESS */

#ifdef BCM_ACCESS_SUPPORT

/* internal macros */
/* access a register */
#define _UA_NEW_REG_ACCESS(unit, register_name, block_instance_number, array_index, reg_value_out, flags) \
    access_regmem((unit), flags, r##register_name, (block_instance_number), (array_index), 0, (uint32*)(reg_value_out))
/* set a field in a register value */
#define _UA_NEW_REG_FIELD_SET(unit, register_name, reg_value_inout, field_name, field_value_in) \
    access_field_set((unit), r##register_name##_f##field_name, (uint32*)(reg_value_inout), (uint32*)(field_value_in))
/* get a field from a register value */
#define _UA_NEW_REG_FIELD_GET(unit, register_name, reg_value_in, field_name, field_value_out) \
    access_field_get((unit), r##register_name##_f##field_name, (uint32*)(reg_value_in), (uint32*)(field_value_out))
/* check if a register or if a field of a register is valid for the unit */
#define _UA_NEW_REG_IS_VALID(unit, register_name) access_is_regmem_a_valid_register((unit), r##register_name)
#define _UA_NEW_REG_FIELD_IS_VALID(unit, register_name, field_name) access_is_a_valid_register_field((unit), r##register_name##_f##field_name) 
/* Return the width in bits of a register field. Returns zero for a none-vlaid fields */
#define _UA_NEW_REG_FIELD_SIZE(unit, register_name, field_name) access_get_field_size((unit), r##register_name##_f##field_name)


#ifdef __UA_HAS_OLD_ACCESS

/* Unified Access macros that support both new and old access */

/* read a register using a logical port number for per-port registers or a block instance number for other registers */
#define UA_REG_READ(unit, register_name, block_instance_number, array_index, reg_value_out) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_out), 0) : \
    _UA_OLD_REG_READ((unit), register_name, (block_instance_number), (array_index), (reg_value_out), 0))
/* write a register using a logical port number for per-port registers or a block instance number for other registers */
#define UA_REG_WRITE(unit, register_name, block_instance_number, array_index, reg_value_in) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_in), FLAG_ACCESS_IS_WRITE) : \
    _UA_OLD_REG_WRITE((unit), register_name, (block_instance_number), (array_index), (reg_value_in), 0))

/* read a register using a logical port number */
#define UA_REG_READ_PORT(unit, register_name, block_instance_number, array_index, reg_value_out) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_out), FLAG_ACCESS_INST_TYPE_CUSTOM) : \
    _UA_OLD_REG_READ((unit), register_name, (block_instance_number), (array_index), (reg_value_out), 0))
/* write a register using a logical port number*/
#define UA_REG_WRITE_PORT(unit, register_name, block_instance_number, array_index, reg_value_in) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_in), FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_CUSTOM) : \
    _UA_OLD_REG_WRITE((unit), register_name, (block_instance_number), (array_index), (reg_value_in), 0))

/* read a register using a phy port number */
#define UA_REG_READ_PHY_PORT(unit, register_name, block_instance_number, array_index, reg_value_out) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_out), FLAG_ACCESS_INST_TYPE_PHY_PORT) : \
    _UA_OLD_REG_READ((unit), register_name, (block_instance_number), (array_index), (reg_value_out), SOC_REG_ADDR_PHY_ACC_MASK))
/* write a register using a phy port number*/
#define UA_REG_WRITE_PHY_PORT(unit, register_name, block_instance_number, array_index, reg_value_in) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_in), FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_PHY_PORT) : \
    _UA_OLD_REG_WRITE((unit), register_name, (block_instance_number), (array_index), (reg_value_in), SOC_REG_ADDR_PHY_ACC_MASK))

/* read a register using an sbus block instance ID */
#define UA_REG_READ_SBUS_ID(unit, register_name, block_instance_number, array_index, reg_value_out) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_out), FLAG_ACCESS_INST_TYPE_SBUS_ID) : \
    _UA_OLD_REG_READ((unit), register_name, (block_instance_number), (array_index), (reg_value_out), SOC_REG_ADDR_SCHAN_ID_MASK))
/* write a register using an sbus block instance ID*/
#define UA_REG_WRITE_SBUS_ID(unit, register_name, block_instance_number, array_index, reg_value_in) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_in), FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID) : \
    _UA_OLD_REG_WRITE((unit), register_name, (block_instance_number), (array_index), (reg_value_in), SOC_REG_ADDR_SCHAN_ID_MASK))

/* set a field in a register value */
#define UA_REG_FIELD_SET(unit, register_name, reg_value_inout, field_name, field_value_in) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_FIELD_SET((unit), register_name, (reg_value_inout), field_name, (field_value_in)) : \
    _UA_OLD_REG_FIELD_SET((unit), register_name, (reg_value_inout), field_name, (field_value_in)))
/* get a field from a register value */
#define UA_REG_FIELD_GET(unit, register_name, reg_value_in, field_name, field_value_out) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_FIELD_GET((unit), register_name, (reg_value_in), field_name, (field_value_out)) : \
    _UA_OLD_REG_FIELD_GET((unit), register_name, (reg_value_in), field_name, (field_value_out)))

/* check if a register or if a field of a register is valid for the unit */
#define UA_REG_IS_VALID(unit, register_name) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_IS_VALID((unit), register_name) : \
    _UA_OLD_REG_IS_VALID((unit), register_name))
#define UA_REG_FIELD_IS_VALID(unit, register_name, field_name) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_FIELD_IS_VALID((unit), register_name, field_name) : \
    _UA_OLD_REG_FIELD_IS_VALID((unit), register_name, field_name))

/* Return the width in bits of a register field. Returns zero for a none-vlaid fields */
#define UA_REG_FIELD_SIZE(unit, register_name, field_name) \
    (ACCESS_IS_INITIALIZED(unit) ? _UA_NEW_REG_FIELD_SIZE((unit), register_name, field_name) : \
    _UA_OLD_REG_FIELD_SIZE((unit), register_name, field_name))

#else /* __UA_HAS_OLD_ACCESS */

/* macro implementation using new access only */

/* read a register using a logical port number for per-port registers or a block instance number for other registers */
#define UA_REG_READ(unit, register_name, block_instance_number, array_index, reg_value_out) \
    _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_out), 0)
/* write a register using a logical port number for per-port registers or a block instance number for other registers */
#define UA_REG_WRITE(unit, register_name, block_instance_number, array_index, reg_value_in) \
    _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_in), FLAG_ACCESS_IS_WRITE)

/* read a register using a logical port number */
#define UA_REG_READ_PORT(unit, register_name, block_instance_number, array_index, reg_value_out) \
    _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_out), FLAG_ACCESS_INST_TYPE_CUSTOM)
/* write a register using a logical port number*/
#define UA_REG_WRITE_PORT(unit, register_name, block_instance_number, array_index, reg_value_in) \
    _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_in), FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_CUSTOM)

/* read a register using a phy port number */
#define UA_REG_READ_PHY_PORT(unit, register_name, block_instance_number, array_index, reg_value_out) \
    _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_out), FLAG_ACCESS_INST_TYPE_PHY_PORT)
/* write a register using a phy port number*/
#define UA_REG_WRITE_PHY_PORT(unit, register_name, block_instance_number, array_index, reg_value_in) \
    _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_in), FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_PHY_PORT)

/* read a register using an sbus block instance ID */
#define UA_REG_READ_SBUS_ID(unit, register_name, block_instance_number, array_index, reg_value_out) \
    _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_out), FLAG_ACCESS_INST_TYPE_SBUS_ID)
/* write a register using an sbus block instance ID*/
#define UA_REG_WRITE_SBUS_ID(unit, register_name, block_instance_number, array_index, reg_value_in) \
    _UA_NEW_REG_ACCESS((unit), register_name, (block_instance_number), (array_index), (reg_value_in), FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID)

/* set a field in a register value */
#define UA_REG_FIELD_SET(unit, register_name, reg_value_inout, field_name, field_value_in) \
    _UA_NEW_REG_FIELD_SET((unit), register_name, (reg_value_inout), field_name, (field_value_in))
/* get a field from a register value */
#define UA_REG_FIELD_GET(unit, register_name, reg_value_in, field_name, field_value_out) \
    _UA_NEW_REG_FIELD_GET((unit), register_name, (reg_value_in), field_name, (field_value_out))
/* check if a register or if a field of a register is valid for the unit */
#define UA_REG_IS_VALID(unit, register_name) _UA_NEW_REG_IS_VALID((unit), register_name)
#define UA_REG_FIELD_IS_VALID(unit, register_name, field_name) _UA_NEW_REG_FIELD_IS_VALID((unit), register_name, field_name)
/* Return the width in bits of a register field. Returns zero for a none-vlaid fields */
#define UA_REG_FIELD_SIZE(unit, register_name, field_name) _UA_NEW_REG_FIELD_SIZE((unit), register_name, field_name)

#endif /* __UA_HAS_OLD_ACCESS */

#else /* BCM_ACCESS_SUPPORT */

/* Unified Access macros that only support old access */

/* read or write a register using a logical port number for per-port registers or a block instance number for other registers */
#define UA_REG_READ(unit, register_name, block_instance_number, array_index, reg_value_out) \
    _UA_OLD_REG_READ((unit), register_name, (block_instance_number), (array_index), (reg_value_out), 0)
#define UA_REG_WRITE(unit, register_name, block_instance_number, array_index, reg_value_in) \
    _UA_OLD_REG_WRITE((unit), register_name, (block_instance_number), (array_index), (reg_value_in), 0)

/* read a register using a logical port number */
#define UA_REG_READ_PORT(unit, register_name, block_instance_number, array_index, reg_value_out) \
    _UA_OLD_REG_READ((unit), register_name, (block_instance_number), (array_index), (reg_value_out), 0)
/* write a register using a logical port number*/
#define UA_REG_WRITE_PORT(unit, register_name, block_instance_number, array_index, reg_value_in) \
    _UA_OLD_REG_WRITE((unit), register_name, (block_instance_number), (array_index), (reg_value_in), 0)

/* read a register using a phy port number */
#define UA_REG_READ_PHY_PORT(unit, register_name, block_instance_number, array_index, reg_value_out) \
    _UA_OLD_REG_READ((unit), register_name, (block_instance_number), (array_index), (reg_value_out), SOC_REG_ADDR_PHY_ACC_MASK)
/* write a register using a phy port number*/
#define UA_REG_WRITE_PHY_PORT(unit, register_name, block_instance_number, array_index, reg_value_in) \
    _UA_OLD_REG_WRITE((unit), register_name, (block_instance_number), (array_index), (reg_value_in), SOC_REG_ADDR_PHY_ACC_MASK)

/* read a register using an sbus block instance ID */
#define UA_REG_READ_SBUS_ID(unit, register_name, block_instance_number, array_index, reg_value_out) \
    _UA_OLD_REG_READ((unit), register_name, (block_instance_number), (array_index), (reg_value_out), SOC_REG_ADDR_SCHAN_ID_MASK)
/* write a register using an sbus block instance ID*/
#define UA_REG_WRITE_SBUS_ID(unit, register_name, block_instance_number, array_index, reg_value_in) \
    _UA_OLD_REG_WRITE((unit), register_name, (block_instance_number), (array_index), (reg_value_in), SOC_REG_ADDR_SCHAN_ID_MASK)


/* set or get a field in/from a register value */
#define UA_REG_FIELD_SET(unit, register_name, reg_value_inout, field_name, field_value_in) \
    _UA_OLD_REG_FIELD_SET((unit), register_name, (reg_value_inout), field_name, (field_value_in))
#define UA_REG_FIELD_GET(unit, register_name, reg_value_in, field_name, field_value_out) \
    _UA_OLD_REG_FIELD_GET((unit), register_name, (reg_value_in), field_name, (field_value_out))
/* check if a register or if a field of a register is valid for the unit */
#define UA_REG_IS_VALID(unit, register_name) _UA_OLD_REG_IS_VALID((unit), register_name)
#define UA_REG_FIELD_IS_VALID(unit, register_name, field_name) _UA_OLD_REG_FIELD_IS_VALID((unit), register_name, field_name)
/* Return the width in bits of a register field. Returns zero for a none-vlaid fields */
#define UA_REG_FIELD_SIZE(unit, register_name, field_name) _UA_OLD_REG_FIELD_SIZE((unit), register_name, field_name)

#endif /* BCM_ACCESS_SUPPORT */
     

#endif /* _UA_H */
