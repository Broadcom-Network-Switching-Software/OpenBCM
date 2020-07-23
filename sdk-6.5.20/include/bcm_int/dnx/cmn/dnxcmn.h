/** \file dnxcmn.h
 *  
 *  Common Utils for dnx only
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DNXCMN_H_INCLUDED
/*
 * {
 */
#define DNXCMN_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Includes 
 * { 
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
/*
 * }
 */

/*
 * MACROs
 * {  
 */
/** 
 * \brief - iterate over the relevant cores 
 * if core_id is BCM_CORE_ALL will iterate over all supported cores. 
 * Otherwise the iteration will be over the given core. 
 */
#define DNXCMN_CORES_ITER(unit, core_id, core_index) \
    for(core_index = ((core_id == BCM_CORE_ALL) ? 0 : core_id);\
        core_index < ((core_id == BCM_CORE_ALL) ?  dnx_data_device.general.nof_cores_get(unit) : (core_id + 1));\
        core_index++)

/**
 * \brief - convert core id to index
 * if core_id is BCM_CORE_ALL return 0 assuming cores are symmetrical.
 */
#define DNXCMN_CORE_ID2INDEX(unit, core_id) ((core_id == BCM_CORE_ALL) ? 0 : core_id)

/**
 * \brief - validate core is legal 
 *  if allow_all == true - validate that core is either valid core_id or BCM_CORE_ALL
 *  Otherwise - validate that core is valid core_id
 */
#define DNXCMN_CORE_VALIDATE(unit, core, allow_all) \
        if ((core >= dnx_data_device.general.nof_cores_get(unit) || (core < 0)) && (!(core == _SHR_CORE_ALL && allow_all)))\
        {\
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Core %d\n", core);\
        }

/**
 * \brief - indicates whether chip has more than one core or not.
 */
#define DNXCMN_CHIP_IS_MULTI_CORE(unit) \
        (dnx_data_device.general.nof_cores_get(unit) > 1)

/**
 * Get core clock frequency (supported units are Hz, KHz and MHz)
 */
#define DNXCMN_CORE_CLOCK_HZ_GET(unit) (dnx_data_device.general.core_clock_khz_get(unit) * 1000)
#define DNXCMN_CORE_CLOCK_KHZ_GET(unit) (dnx_data_device.general.core_clock_khz_get(unit))
/*
 * }
 */

/*
 * TYPEDEFs
 * {  
 */

#define DNXCMN_TIMEOUT                            (20*1000)
#define DNXCMN_MIN_POLLS                          (100)

/*
 * }
 */

/*
 * TYPEDEFs
 * {  
 */
/**
 * \brief - time units
 */
typedef enum
{
  /**
   * Invalid time unit
   */
    DNXCMN_TIME_UNIT_INVALID = -1,
  /**
   * Second
   */
    DNXCMN_TIME_UNIT_SEC = 0,
  /**
   * Mili Second
   */
    DNXCMN_TIME_UNIT_MILISEC = 1,
  /**
   * Micro Second
   */
    DNXCMN_TIME_UNIT_USEC = 2,

  /**
   * Nano Second
   */
    DNXCMN_TIME_UNIT_NSEC = 3
} dnxcmn_time_unit_e;

/**
 * Represet time which composed from amount of time and time units
 */
typedef struct
{
    dnxcmn_time_unit_e time_units;
    uint64 time;
} dnxcmn_time_t;
/**
 * Enumeration of field types acceptable by dnx_dbal_gen_set/get
 * See:
 *   dnx_dbal_gen_set()
 *   dnx_dbal_gen_get()
 */
typedef enum
{
    FIRST_DNX_GEN_DBAL_FIELD = 0,
    /**
     * Field is of type uint32.
     */
    GEN_DBAL_FIELD32 = FIRST_DNX_GEN_DBAL_FIELD,
    /**
     * Field is of type uint64.
     */
    GEN_DBAL_FIELD64,
    /**
     * Field is an array. All array elements are of type uint32.
     */
    GEN_DBAL_ARR32,
    /**
     * Field is a range. Only relevant for 'key' and only for 'set' operation.
     */
    GEN_DBAL_RANGE32,
    /**
     * Field is the last, following all other fields and their parameters.
     * Used for verification.
     */
    GEN_DBAL_FIELD_LAST_MARK,
    NUM_DNX_GEN_DBAL_FIELD
} dnx_gen_dbal_field_e;
/*
 * }
 */
/*
 * DBAL utils functions
 * {
 */

/**
 * \brief
 *   Carry out DBAL access (of 'get') on one specified table
 *   with one or more keys and with one or more fields.
 *   All fields must be of a type specified in 'dnx_gen_dbal_field_e'.
 *   In most cases, they would be of type 'uint32' for input and
 *   'uint32 *' for output
 * \param [in] unit -
 *   uint32. Identifier of HW platform.
 * \param [in] dbal_table_id -
 *   dbal_tables_e. Identifier of the DBAL table to write to.
 *   For example: DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION
 * \param [in] num_keys -
 *   uint32. Number of keys required for access of this table. The code,
 *   below, will expect 'num_keys' triplets at the beginning of the variables
 *   list (va_arg_params) as follows: (Field Type, Key Id, Key Value).
 *   For example:
 *     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core
 *   Note that all values are expected to be specified in in 'dnx_gen_dbal_field_e'.
 * \param [in] num_fields -
 *   uint32. Number of fields required for access of this table. The code,
 *   below, will expect 'num_fields' quartet, following the 'num_keys' triplets on
 *   variables list (va_arg_params) as follows:
 *   (Field Type, Field Id, Field Instance, Pointer to Field Value).
 *   For example:
 *     GEN_DBAL_FIELD32, DBAL_FIELD_FQP_CALENDAR_SET_SEL, INST_SINGLE, &current_cal
 *   Notes:
 *     * For 'get' operation, all field values are expected be pointers to types
 *       specified in in 'dnx_gen_dbal_field_e'.
 *       In most cases, the field type would be 'uint32 *'
 *     * In most cases, the value of 'Field Instance' would be 'INST_SINGLE'. However,
 *       when the field is actually an array of fields, with index, this value would be
 *       the index.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   * The last field identifier, following the ekys and the variables, MUST be GEN_DBAL_FIELD_LAST_MARK.
 *     This is a verification tool.
 *   * In parameter 'va_arg_params' (represented by '...') -
 *   List of triplets followed by quartets as described above. Sum length of both lists
 *   is expected to be
 *   'num_keys' + 'num_fields'. 
 *   * If field type is GEN_DBAL_ARR32, then the 'field value' is expected to be 'uint32 *'
 *   which is a pointer to the array, in memory, which is to contain data as read from HW.
*    User is expected to fill that area with '0' before calling this procedure.
 * \see
 *    * dnx_egr_recycle_bandwidth_get
 */
shr_error_e dnx_dbal_gen_get(
    uint32 unit,
    dbal_tables_e dbal_table_id,
    uint32 num_keys,
    uint32 num_fields,
    ...);
/**
 * \brief
 *   Carry out DBAL access (of 'set') on one specified table
 *   with one or more keys and with one or more fields.
 *   All fields must be of a type specified in 'dnx_gen_dbal_field_e'.
 *   In most cases, they would be of type 'uint32'
 * \param [in] unit -
 *   uint32. Identifier of HW platform.
 * \param [in] dbal_table_id -
 *   dbal_tables_e. Identifier of the DBAL table to write to.
 *   For example: DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION
 * \param [in] num_keys -
 *   uint32. Number of keys required for access of this table. The code,
 *   below, will expect 'num_keys' triplets at the beginning of the variables
 *   list (va_arg_params) as follows: (Field Type, Key Id, Key Value).
 *   For example:
 *     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core
 *   Note that all values are expected to be specified in in 'dnx_gen_dbal_field_e'.
 * \param [in] num_fields -
 *   uint32. Number of fields required for access of this table. The code,
 *   below, will expect 'num_fields' quartet, following the 'num_keys' triplets on
 *   variables list (va_arg_params) as follows (but see notes below):
 *   (Field Type, Field Id, Field Instance, Field Value).
 *   For example:
 *     GEN_DBAL_FIELD32, DBAL_FIELD_FQP_CALENDAR_SET_SEL, INST_SINGLE, current_cal
 *   Notes:
 *     * For 'set' operation, all values are expected be specified in in 'dnx_gen_dbal_field_e'.
 *       In most cases, the type would be 'uint32'
 *     * In most cases, the value of 'Field Instance' would be 'INST_SINGLE'. However,
 *       when the field is actually an array of fields, with index, this value would be
 *       the index.
 *     * For 'set' operation, on the 'key' section, if the key is a 'range' then the
 *       number of following parameters is '4' (instead of the standard triplet) and the last
 *       two are the range.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   * The last field identifier, following the ekys and the variables, MUST be GEN_DBAL_FIELD_LAST_MARK.
 *     This is a verification tool.
 *   * In parameter 'va_arg_params' (represented by '...') -
 *   List of triplets followed by quartets as described above. Sum length of both lists
 *   is expected to be
 *   'num_keys' + 'num_fields'. 
 *   * If field type is GEN_DBAL_ARR32, then the 'field value' is expected to be 'uint32 *'
 *   which is a pointer to the array, in memory, which is to be written into HW.
 * \see
 *    * dnx_egr_q_nif_cal_set
 */
shr_error_e dnx_dbal_gen_set(
    uint32 unit,
    dbal_tables_e dbal_table_id,
    uint32 num_keys,
    uint32 num_fields,
    ...);
/*
 * }
 */

/*
 * TIME Functions
 * {  
 */
/**
 * \brief - Convert time to required time_units
 *          The result will be round off 
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] time_src - time to convert
 *   \param [in] required_time_units - required time units
 *   \param [in] time_dest - destination of the converted tiem
 * \par INDIRECT INPUT:
 *   * None
 */
shr_error_e dnxcmn_time_units_convert(
    int unit,
    dnxcmn_time_t * time_src,
    dnxcmn_time_unit_e required_time_units,
    dnxcmn_time_t * time_dest);

/**
 * \brief - Convert device clock cycles to time (according to required time units) 
 *          The result will be round down 
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] nof_clock_cycles - number of clock cycles to convert to time
 *   \param [in] required_time_unit - required time units
 *   \param [in] time - pointer to converted time (output)
 * \par INDIRECT INPUT:
 *   * dnx_data_device (to get the core clock)
 */
shr_error_e dnxcmn_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxcmn_time_unit_e required_time_unit,
    dnxcmn_time_t * time);

/**
 * \brief - Convert system reference clock cycles to time (according to required time units) 
 *          The result will be round down 
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] nof_clock_cycles - number of clock cycles to convert to time
 *   \param [in] required_time_unit - required time units
 *   \param [in] time - pointer to converted time (output)
 * \par INDIRECT INPUT:
 *   * dnx_data_device (to get the core clock)
 */
shr_error_e dnxcmn_system_ref_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxcmn_time_unit_e required_time_unit,
    dnxcmn_time_t * time);

/**
 * \brief - Convert time to device clock cycles (according to specified time units) 
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] time - time to convert to clock cycles
 *   \param [in] nof_clock_cycles - pointer to converted number of clock cycles (output)
 * \par INDIRECT INPUT:
 *   * dnx_data_device (to get the core clock)
 */
shr_error_e dnxcmn_time_to_clock_cycles_get(
    int unit,
    dnxcmn_time_t * time,
    uint32 *nof_clock_cycles);

/**
 * \brief - Convert time to system reference clock cycles (according to specified time units) 
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] time - time to convert to clock cycles
 *   \param [in] nof_clock_cycles - pointer to converted number of clock cycles (output)
 * \par INDIRECT INPUT:
 *   * dnx_data_device (to get the core clock)
 */
shr_error_e dnxcmn_time_to_system_ref_clock_cycles_get(
    int unit,
    dnxcmn_time_t * time,
    uint32 *nof_clock_cycles);

/**
 * \brief - Doing polling till the result matches the expected, 
 *        or we reach the time out
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] time_out - time out value, maximal time for
 *          polling
 *   \param [in] min_polls - minimum polls
 *   \param [in] entry_handle_id - DBAL handle to the table, need to 
 *          set Key field before passsing the handle
 *   \param [in] field - DBAL field name
 *   \param [in] expected_value - the expected value
 */
shr_error_e dnxcmn_polling(
    int unit,
    sal_usecs_t time_out,
    int32 min_polls,
    uint32 entry_handle_id,
    dbal_fields_e field,
    uint32 expected_value);

/*
 * }
 */
#endif /* !DNXCMN_H_INCLUDED */
