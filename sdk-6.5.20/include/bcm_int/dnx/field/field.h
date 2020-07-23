/**
 *  \file bcm_int/dnx/field/field.h
 * 
 *  Internal DNX RX APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_FIELD_H_INCLUDED
/* { */
#define DNX_FIELD_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * { Include files
 */

#include <include/bcm/field.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <shared/bsl.h>

/*
 * } Include files
 */

/*
 * { Macros
 */
/**
 * If DBX_FIELD_BRUTE_FORCE_DEINIT_INIT is non-zero then, in order to release
 * all resources, allocated for 'field' tests', the whole field sw state and related
 * resources (sorted list, hash table, occupation bitmap, legacy sw state) are
 * released. On final version, all code under this define should disappear and
 * be replaced by directed release (such as 'filed_group_destroy'.
 */
#define DBX_FIELD_BRUTE_FORCE_DEINIT_INIT  1

/** Set bit in unsigned int*/
#define DNX_FIELD_UINT_SET_BIT(val,bit)  ((val) |= 1u<<(bit))

/** Check if bit is set in unsigned int*/
#define DNX_FIELD_UINT_IS_BIT_SET(val,bit)  (((val)&(1u<<(bit)))? TRUE : FALSE)

/** Set bit in array of uint32*/
#define DNX_FIELD_ARR32_SET_BIT(val,bit)  (((val)[(bit)/32]) |= 1u<<((bit)%32))

/** Check if bit is Set in array of uint32*/
#define DNX_FIELD_ARR32_IS_BIT_SET(val,bit)  (( ((val)[(bit)/32]) & (1u<<((bit)%32)))? TRUE : FALSE)

/** Find number of elements in an static array*/
#define DNX_FIELD_NOF_ELEMENTS_ARRAY(arr)  (sizeof(arr)/sizeof(arr[0]))

/** Print to user, if printing is enabled.*/
#if defined(BROADCOM_DEBUG)
#define DNX_FIELD_PRINT(stuff) (bsl_printf stuff)
#else
#define DNX_FIELD_PRINT(stuff)
#endif

#ifndef MIN
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a)>(b)) ? (a) : (b))
#endif

/*
 * Enums
 * {
 */
/*
 * Enum for the source of the state table as read from the SOC property pmf_state_table_rmw_source.
 */
typedef enum dnx_field_state_table_source_e
{
    DNX_FIELD_STATE_TABLE_SOURCE_INVALID = 0,
    DNX_FIELD_STATE_TABLE_SOURCE_IPMF1_KEY_J = 1,       /* MSB of initial key J in iPMF1 */
    DNX_FIELD_STATE_TABLE_SOURCE_IPMF1_KEY_F = 2,       /* LSB of initial key F in iPMF1 */
    DNX_FIELD_STATE_TABLE_SOURCE_IPMF2_KEY_J = 3,       /* MSB of key J in iPMF2 */
    DNX_FIELD_STATE_TABLE_SOURCE_COUNT = 4
} dnx_field_state_table_source_t;

/**
 * This enum contains compare modes (pairs) values for the first and second compare keys
 */
typedef enum
{
    DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1 = 0,
    DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_2 = 1,
} dnx_field_context_compare_mode_pair_e;

/*
 * Enums
 * }
 */

/*
 * FEM-related definitions.
 * {
 */
/*
 * Verify the fem_bit_format is valid for specified unit
 * 'fem_bit_format' is the format indication for each bit on table,
 * say, FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_2_15_MAP (element FIELD_FEM_BIT_VAL)
 * See dnx_field_fem_bit_info_t (element 'fem_bit_format')
 * See dnx_field_fem_bit_format_e
 */
#define DNX_FIELD_FEM_BIT_FORMAT_VERIFY(_fem_bit_format)  \
    if (((unsigned int) _fem_bit_format >= (unsigned int) (DNX_FIELD_FEM_BIT_FORMAT_NUM)) ||       \
                ((unsigned int) _fem_bit_format < (unsigned int) (DNX_FIELD_FEM_BIT_FORMAT_FIRST))) \
    { \
        SHR_ERR_EXIT(_SHR_E_PARAM, \
                     "fem_bit_format (%d) is out of range (Should be between %d and %d)\r\n", \
                     _fem_bit_format, DNX_FIELD_FEM_BIT_FORMAT_FIRST, DNX_FIELD_FEM_BIT_FORMAT_NUM - 1); \
    }
/*
 * Verify the fem_id is valid for specified unit
 */
#define DNX_FIELD_FEM_ID_VERIFY(_unit,_fem_id)  \
    if ((unsigned int) _fem_id >= (unsigned int) (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id)) \
    { \
        SHR_ERR_EXIT(_SHR_E_PARAM, \
                     "fem_id (%d) is out of range (Should be between %d and %d)\r\n", \
                     _fem_id, 0, dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id - 1); \
    }
/*
 * Verify the fem_condition_entry (of type dnx_field_fem_condition_entry_t) is valid for specified unit
 * - First, verify the the 'fem_action_valid' element is within range
 *   (smaller than DBAL_NOF_ENUM_FIELD_FEM_ACTION_VALID_VALUES
 * - Second, if it is valid (DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE),
 *   verify that the 'fem_map_index'; element is within range (0 - 3).
 */
#define DNX_FIELD_FEM_CONDITION_ENTRY_VERIFY(_unit,_fem_condition_entry)  \
    if ((unsigned int)((_fem_condition_entry).fem_action_valid) < (unsigned int)DBAL_NOF_ENUM_FIELD_FEM_ACTION_VALID_VALUES) \
    { \
        if (((_fem_condition_entry).fem_action_valid) == DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE) \
        { \
            if ((unsigned int) ((_fem_condition_entry).fem_map_index) >= (unsigned int) (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index)) \
            { \
                SHR_ERR_EXIT(_SHR_E_PARAM, \
                     "fem_condition_entry.fem_map_index (%d) is out of range (Should be between %d and %d)\r\n", \
                     ((_fem_condition_entry).fem_map_index), 0, DBAL_NOF_ENUM_FIELD_FEM_ACTION_VALID_VALUES - 1); \
            } \
        } \
    } \
    else    \
    { \
        SHR_ERR_EXIT(_SHR_E_PARAM, \
             "fem_condition_entry.fem_action_valid (%d) is out of range (Should be between %d and %d)\r\n", \
             ((_fem_condition_entry).fem_action_valid), 0, dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index - 1); \
    }
/*
 * Verify the fem_program is valid for specified unit
 */
#define DNX_FIELD_FEM_PROGRAM_VERIFY(_unit,_fem_program)  \
    if ((unsigned int) _fem_program >= (unsigned int) (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs)) \
    { \
        SHR_ERR_EXIT(_SHR_E_PARAM, \
                     "fem_program (%d) is out of range (Should be between %d and %d)\r\n", \
                     _fem_program, 0, dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs - 1); \
    }
/*
 * Verify the fem_program is valid for specified unit
 */
#define DNX_FIELD_FEM_MAP_INDEX_VERIFY(_unit,_fem_map_index)  \
    if ((unsigned int) _fem_map_index >= (unsigned int) (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index)) \
    { \
        SHR_ERR_EXIT(_SHR_E_PARAM, \
                     "fem_map_index (%d) is out of range (Should be between %d and %d)\r\n", \
                     _fem_map_index, 0, dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index - 1); \
    }
/*
 * Step by which 'fem key select' moves along result input.For each step, 32 are specified which
 * become the final input to FEM machine.
 * (See dbal_enum_value_field_field_pmf_a_fem_key_select_e)
 */
#define DNX_FIELD_FEM_INPUT_OFFSET_RESOLUTION dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->fem_key_select_resolution_in_bits
/*
 * Number of meaningful data bits, in the common cases of TCAM and DE, on 'fem key select'.
 * (See dbal_enum_value_field_field_pmf_a_fem_key_select_e)
 */
#define DNX_FIELD_FEM_COMMON_INPUT_SIZE dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_key_select
/*
 * Verify some input parameters for dnx_field_fem_action_add():
 *   input_offset
 *   input_size
 *   fg_id
 */
#define DNX_FIELD_INPUT_SIZE_OFFSET_VERIFY(_unit, _input_offset, _input_size, _fg_id)  \
    { \
        /*    \
         * Reach here if _input_offset is a multiple of DNX_FIELD_FEM_INPUT_OFFSET_RESOLUTION    \
         * (as it should be)    \
         */    \
        dnx_field_action_length_type_e _action_length_type;    \
        dnx_field_key_length_type_e _key_length_type;    \
        unsigned int _block_size;    \
        dnx_field_group_type_e _fg_type;    \
        _block_size = 0;    \
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(_unit, _fg_id, &_fg_type));    \
        /*    \
         */    \
        switch (_fg_type)    \
        {    \
            case (DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION):    \
            {    \
                if ((_input_offset % DNX_FIELD_FEM_INPUT_OFFSET_RESOLUTION) != 0) \
                { \
                    SHR_ERR_EXIT(_SHR_E_PARAM, \
                             "\r\n"  \
                             "For a direct extraction field group, input_offset (%d) should be a multiple of %d. Quit\r\n", \
                             _input_offset, DNX_FIELD_FEM_INPUT_OFFSET_RESOLUTION);    \
                } \
                if (_input_size != DNX_FIELD_FEM_COMMON_INPUT_SIZE) \
                { \
                    SHR_ERR_EXIT(_SHR_E_PARAM, \
                             "\r\n"  \
                             "For a direct extraction field group, input_size (%d) should be %d. Quit\r\n", \
                             _input_size, DNX_FIELD_FEM_COMMON_INPUT_SIZE);    \
                } \
                SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(_unit,_fg_id,&_key_length_type));    \
                /*  \
                 * Input size is 'full key' for both 'half key' and 'full key'. That's how HW works...  \
                 */      \
                if ((_key_length_type == DNX_FIELD_KEY_LENGTH_TYPE_HALF) || (_key_length_type == DNX_FIELD_KEY_LENGTH_TYPE_SINGLE))    \
                {    \
                    _block_size = dnx_data_field.dir_ext.single_key_size_get(unit);    \
                }    \
                else    \
                {    \
                    SHR_ERR_EXIT(_SHR_E_PARAM,    \
                             "\r\n"  \
                             "For a direct extraction field group, this key 'length type' is not allowed, key_length_type=%d (%s)\r\n",    \
                             _key_length_type, dnx_field_key_length_type_e_get_name(_key_length_type));    \
                }    \
                break;    \
            }    \
            case (DNX_FIELD_GROUP_TYPE_TCAM):    \
            case (DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM):    \
            {    \
                if ((_input_offset % DNX_FIELD_FEM_INPUT_OFFSET_RESOLUTION) != 0) \
                { \
                    SHR_ERR_EXIT(_SHR_E_PARAM, \
                             "\r\n"  \
                             "For a TCAM field group, input_offset (%d) should be a multiple of %d. Quit\r\n", \
                             _input_offset, DNX_FIELD_FEM_INPUT_OFFSET_RESOLUTION);    \
                } \
                if (_input_size != DNX_FIELD_FEM_COMMON_INPUT_SIZE) \
                { \
                    SHR_ERR_EXIT(_SHR_E_PARAM, \
                             "\r\n"  \
                             "For a TCAM field group,, input_size (%d) should be %d. Quit\r\n", \
                             _input_size, DNX_FIELD_FEM_COMMON_INPUT_SIZE);    \
                } \
                SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(_unit,_fg_id,&_action_length_type));    \
                /*    \
                 * Load block_size by the number of bits in the payload buffer (that    \
                 * contains all actions plus, potentially, some spare)    \
                 */    \
                SHR_IF_ERR_EXIT(dnx_field_group_action_length_to_bit_size(_unit,_action_length_type,&_block_size));    \
                break;    \
            }    \
            case (DNX_FIELD_GROUP_TYPE_EXEM):    \
            {    \
                  \
                _block_size = 60; \
                break;    \
            }    \
            case (DNX_FIELD_GROUP_TYPE_CONST):    \
            {    \
                if (_input_offset != 0) \
                { \
                    SHR_ERR_EXIT(_SHR_E_PARAM, \
                             "\r\n"  \
                             "For a Const field group, input_offset (%d) should be 0. Quit\r\n", \
                             _input_offset);    \
                } \
                if (_input_size != 0) \
                { \
                    SHR_ERR_EXIT(_SHR_E_PARAM, \
                             "\r\n"  \
                             "For a Const field group,, input_size (%d) should be 0. Quit\r\n", \
                             _input_size);    \
                } \
                break;    \
            }    \
            default:    \
            {    \
                SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(_unit,_fg_id,&_action_length_type));    \
                /*    \
                 * Load block_size by the number of bits in the payload buffer (that    \
                 * contains all actions plus, potentially, some spare    \
                 */    \
                SHR_IF_ERR_EXIT(dnx_field_group_action_length_to_bit_size(_unit,_action_length_type,&_block_size));    \
                break;    \
            }    \
        }    \
        /*    \
         * At this point, 'block_size' contains the number of bits in the buffer that makes    \
         * the 'input' to the FEM.    \
         * Make sure it contains specified '_input_offset' plus the 32 bits required to make    \
         * the 'chunk' that FEM uses.    \
         */    \
        if ((_input_offset + dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_key_select ) > _block_size)    \
        {    \
            SHR_ERR_EXIT(_SHR_E_PARAM,    \
                         "\r\n"  \
                         "Number of bits in 'container buffer' for input to FEM (%d) is smaller than \r\n"    \
                         "required for 'input_offset' (%d) plus 'chunk' size (%d). fg_type is %d (%s). Quit.\r\n",    \
                         _block_size, _input_offset, dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_key_select,    \
                         _fg_type, dnx_field_group_type_e_get_name(_fg_type));    \
        }    \
    }
/*
 * }
 */

/*
 * Action priority related definitions.
 * The Action priority is an encoding of the preferred FES.
 * SeeBCM_FIELD_ACTION_POSITION and BCM_FIELD_ACTION_PRIORITY.
 * {
 */

/**
 * The position of the preferred location within the FES array represented in dnx_field_action_priority_t.
 * Parallel to BCM_FIELD_ACTION_ARRAY_OFFSET.
 */
#define DNX_FIELD_ACTION_PRIORITY_FES_POSITION_PRIORITY_OFFSET       0

/**
 * \brief
 *   Retrieve the part of the encoded priority which contains the
 *   position within the array of the corresponding FES or FEM.
 *
 * \param [in] priority -
 *   Encoded action priority, with mandatory position.
 *                        (dnx_field_action_priority_t)
 * \return
 *   FES/FEM position within FES/FEM array (dnx_field_action_position_t).
 */
#define DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(priority)                 \
        (((priority) >> DNX_FIELD_ACTION_PRIORITY_FES_POSITION_PRIORITY_OFFSET)   \
        & BCM_FIELD_ACTION_PRIORITY_MASK)

/**
 * \brief Retrieve the part of the encoded priority which contains the numerical priority.
 * \param [in] priority - Encoded action priority.
 *                        (dnx_field_action_priority_t)
 * \return
 *      Numerical priority (dnx_field_action_numerical_priority_t).
 */
#define DNX_FIELD_ACTION_PRIORITY_FES_PRIORITY_NUMERICAL(priority)                \
        (((priority) >> DNX_FIELD_ACTION_PRIORITY_FES_POSITION_PRIORITY_OFFSET)   \
        & BCM_FIELD_ACTION_PRIORITY_MASK)

/**
 * \brief Retrieve the FES ID
 *
 * \param [in] priority - Encoded action priority
 *                        (dnx_field_action_priority_t)
 * \return
 *      FES array (dnx_field_action_array_t).
 */
#define DNX_FIELD_ACTION_PRIORITY_ARRAY(priority)                \
        (((priority) >> BCM_FIELD_ACTION_ARRAY_OFFSET)           \
        & BCM_FIELD_ACTION_ARRAY_MASK)

/**
 * \brief Retrieve whether encoded priority (dnx_field_action_priority_t) mandated position or not.
 * \param [in] priority - Encoded action priority (dnx_field_action_priority_t).
 * \return
 *      TRUE if position is mandatory.
 *      FALSE if numerical priority.
 */
#define DNX_FIELD_ACTION_PRIORITY_IS_POSITION(priority)                                   \
        ((((priority) & SAL_BIT(BCM_FIELD_ACTION_POSITION_PRIORITY_FLAG_OFFSET)) != 0) &&  \
         (((priority) & SAL_BIT(BCM_FIELD_ACTION_VALID_OFFSET)) != 0))

/**
 * \brief Retrieve whether encoded priority (dnx_field_action_priority_t) has "don't care" bit set.
 * \param [in] priority - Encoded action priority (dnx_field_action_priority_t).
 * \return
 *      TRUE if priority has "don't care" bit set.
 *      FALSE otherwise.
 */
#define DNX_FIELD_ACTION_PRIORITY_FES_PRIORITY_IS_DONT_CARE_BIT_SET(priority)                \
        (((priority) & SAL_BIT(BCM_FIELD_ACTION_DONT_CARE_OFFSET)) != 0)

/**
 * \brief Retrieve whether encoded priority (dnx_field_action_priority_t) has the valid bit set.
 *        That alone does not make the priority valid.
 * \param [in] priority - Encoded action priority (dnx_field_action_priority_t).
 * \return
 *      TRUE if priority has valid bit set.
 *      FALSE otherwise.
 */
#define DNX_FIELD_ACTION_PRIORITY_FES_PRIORITY_IS_VALID_BIT_SET(priority)                \
        (((priority) & SAL_BIT(BCM_FIELD_ACTION_VALID_OFFSET)) != 0)

/**
 * \brief
 *        Verify that the action priority is a legal value
 *        Because we want to only have one "don't care" value and one "invalid" value, we prohibit the use of the
 *        flags for other values.
 * \param [in] priority - Action priority.
 * \return
 *      None.
          */
#define DNX_FIELD_ACTION_PRIORITY_VERIFY(priority)                                                                \
{                                                                                                                 \
    if ((DNX_FIELD_ACTION_PRIORITY_FES_PRIORITY_IS_VALID_BIT_SET(priority)) &&                                    \
        (DNX_FIELD_ACTION_PRIORITY_FES_PRIORITY_IS_DONT_CARE_BIT_SET(priority)) &&                                \
        (priority != BCM_FIELD_ACTION_DONT_CARE))                                                                 \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                \
                     "Action priority 0x%08x has the \"Care\" bit set, but isn't \"don't care\" priority.\r\n",   \
                     priority);                                                                                   \
    }                                                                                                             \
    if ((DNX_FIELD_ACTION_PRIORITY_FES_PRIORITY_IS_VALID_BIT_SET(priority) == FALSE) &&                           \
        (priority != DNX_FIELD_ACTION_PRIORITY_INVALID) && (priority != BCM_FIELD_ACTION_INVALIDATE))             \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                \
                     "Action priority 0x%08x has the \"valid\" bit zero, but isn't the invalid priority.\r\n",    \
                     priority);                                                                                   \
    }                                                                                                             \
}

/**
 * \brief
 *        Get the FES ID from a priority with mandatory position.
 *        We allow only even arrays for EFES, the odd arrays are for FEM.
 * \param [out] _fes_id  - The FES ID encoded by the priority.
 * \param [in] _unit     - Device ID.
 * \param [in] _field_stage    - The field stage.
 * \param [in] _priority - Encoded action priority.
 * \return
 *      None.
 */
#define DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(_fes_id, _unit, _field_stage, _priority)                      \
{                                                                                                                   \
    int _nof_fes_id_in_array;                                                                                       \
    int _nof_fes_id_in_stage;                                                                                       \
    int _nof_fes_array;                                                                                             \
    dnx_field_action_array_t _fes_array;                                                                            \
    dnx_field_action_position_t _fes_position;                                                                      \
    if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(_priority)==FALSE)                                       \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                  \
                     "Action priority 0x%08x Is not a mandatory position priority.\r\n",                            \
                     _priority);                                                                                    \
    }                                                                                                               \
    _fes_array = DNX_FIELD_ACTION_PRIORITY_ARRAY(_priority);                                                        \
    if (dnx_data_field.common_max_val.nof_array_ids_get(_unit) <= _fes_array)                                       \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                  \
                     "\r\n"                                                                                         \
                     "Action priority 0x%08X uses array id %d which is larger than max allowed (%d).\r\n",          \
                     _priority, _fes_array, dnx_data_field.common_max_val.nof_array_ids_get(_unit) - 1);            \
    }                                                                                                               \
    _fes_position = DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(_priority);                                         \
    _nof_fes_id_in_array = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fes_id_per_array;          \
    _nof_fes_array = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fes_array;                       \
    _nof_fes_id_in_stage = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fes_instruction_per_context;\
    if (_nof_fes_array  <= 0)                                                                                       \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage \"%s\" (%d) has no EFES.\r\n", dnx_field_stage_text(_unit, _field_stage), _field_stage);\
    }                                                                                                               \
    if ((dnx_data_field.common_max_val.array_id_type_get(_unit,_fes_array)->is_fes) != 1)                           \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                  \
                     "\r\n"                                                                                         \
                     "Action priority 0x%08X uses array %d which is not a FES array.\r\n",                          \
                     _priority, _fes_array);                                                                        \
    }                                                                                                               \
    if (dnx_data_field.common_max_val.array_id_type_get(_unit, _fes_array)->fes_array_index > (_nof_fes_array - 1)) \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                  \
                     "Action priority 0x%08X encodes FES array %d, while the maximum for stage \"%s\" is %d.\r\n",  \
                     _priority, DNX_FIELD_ACTION_PRIORITY_ARRAY(_priority), dnx_field_stage_text(_unit, _field_stage),\
                     _nof_fes_array-1);                                                                             \
    }                                                                                                               \
    if (_fes_position >= _nof_fes_id_in_array)                                                                      \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                  \
                     "Action priority 0x%08X encodes FES position within array %d, "                                \
                     "while the maximum for stage   \"%s\" is %d.\r\n",                                             \
                     _priority, DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(_priority),                             \
                     dnx_field_stage_text(_unit, _field_stage), _nof_fes_id_in_array-1);                            \
    }                                                                                                               \
    _fes_id = (dnx_data_field.common_max_val.array_id_type_get(_unit, _fes_array)->fes_array_index * _nof_fes_id_in_array) + _fes_position;  \
    if (_fes_id >= _nof_fes_id_in_stage)                                                                            \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                  \
                     "Action priority 0x%08X encodes FES ID %d, while the maximum for stage \"%s\" is %d.\r\n",     \
                     _priority, _fes_id, dnx_field_stage_text(_unit, _field_stage), _nof_fes_id_in_stage-1);        \
    }                                                                                                               \
}

/**
 * \brief
 *        Get the EFES array and position within array from a FES ID
 * \param [out] _array  - The EFES array.
 * \param [out] _position  - The EFES position within the array.
 * \param [in] _unit     - Device ID.
 * \param [in] _field_stage  - The field stage.
 * \param [in] _fes_id - EFES ID, which array and position, should be returned.
 * \return
 *      None.
 */
#define DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(_array, _position, _unit, _field_stage, _fes_id)               \
{                                                                                                                 \
    int _nof_fes_id_in_array;                                                                                     \
    int _nof_fes_id_in_stage;                                                                                     \
    int _nof_fes_array;                                                                                           \
    _nof_fes_id_in_array = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fes_id_per_array;        \
    _nof_fes_array = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fes_array;                     \
    _nof_fes_id_in_stage = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fes_instruction_per_context;\
    if (_nof_fes_array  <= 0)                                                                                     \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage \"%s\" (%d) has no EFES. \r\n", dnx_field_stage_text(_unit, _field_stage), _field_stage);\
    }                                                                                                             \
    if (_fes_id >= _nof_fes_id_in_stage)                                                                          \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                \
                     "EFES ID %d is bigger then the maximum for stage \"%s\" is %d.\r\n",                         \
                     _fes_id, dnx_field_stage_text(_unit, _field_stage), _nof_fes_id_in_stage - 1);               \
    }                                                                                                             \
    _array = ((_fes_id / _nof_fes_id_in_array) * 2);                                                              \
    _position = (_fes_id % _nof_fes_id_in_array);                                                                 \
    if ((_array/2) >= _nof_fes_array)                                                                             \
    {                                                                                                             \
     SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                   \
                  "EFES ARRAY %d is bigger then the maximum for stage \"%s\" is %d.\r\n",                         \
                   _array, dnx_field_stage_text(_unit, _field_stage), _nof_fes_array - 1);                        \
    }                                                                                                             \
}
/**
 * \brief
 *   Get the FEM ID from encoded 'action_priority'.
 *   Encoding is assumed to have been carried out using BCM_FIELD_ACTION_POSITION.
 *   We allow only odd array ids for FEM, the even arrays are for FEM.
 *   See 'remark' below.
 * \param [out] _fem_id  -
 *   The FEM ID as extracted from _action_priority.
 * \param [in] _unit     -
 *   Device ID.
 * \param [in] _encoded_position -
 *   Variable of type bcm_field_action_priority_t.
 *   Encoded position of a FEM. Encoding is done using 'array_id' and 'position'.
 *   See 'BCM_FIELD_ACTION_POSITION' and 'remark' below
 * \return
 *   See '_fem_id' above.
 * \remark
 *   'Stage' is not an input parameter here since FEMs are only relevant for IPMF2.
 *
 *   FESes and FEMs are assigned priority, related to their corresponding actions.
 *   The priority layout of FESes and FEMs is as follows:
 *
 * Priority:      LOW        ...       ...      HIGH
 * Array
 *   id:           0          1         2        3
 * Type:          FES        FEM       FES      FEM
 * Identifier:    0-15       0-7       16-31    8-15
 * \see
 *   * DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET
 *   * bcm_field_fem_action_add
 *   * bcm_dnx_field_fem_action_add
 *   * bcm_field_fem_action_info_get
 *   * bcm_dnx_field_fem_action_info_get
 *   * bcm_field_fem_action_delete
 *   * bcm_dnx_field_fem_action_delete
 */
#define DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(_fem_id, _unit, _encoded_position)                          \
{                                                                                                                 \
    int _nof_fem_id_per_array;                                                                                     \
    int _nof_fem_id_in_stage;                                                                                     \
    int _nof_fem_array;                                                                                           \
    dnx_field_action_position_t _fem_position;                                                                    \
    dnx_field_action_array_t _fem_array;                                                                          \
    dnx_field_stage_e _field_stage;                                                                               \
                                                                                                                  \
    if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(_encoded_position) == FALSE)                                        \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                \
                     "\r\n"                                                                                       \
                     "_encoded_position (0x%08X) is not a 'position' one\r\n"                                     \
                     "==> (Bit at offset %d is zero). Quit.",                                                     \
                     _encoded_position, BCM_FIELD_ACTION_POSITION_PRIORITY_FLAG_OFFSET);                          \
    }                                                                                                             \
    _fem_array = DNX_FIELD_ACTION_PRIORITY_ARRAY(_encoded_position);                                              \
    if (dnx_data_field.common_max_val.nof_array_ids_get(_unit) <= _fem_array)                                     \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                \
                     "\r\n"                                                                                       \
                     "_encoded_position 0x%08X uses array id %d which is larger than max allowed (%d).\r\n",      \
                     _encoded_position, _fem_array, dnx_data_field.common_max_val.nof_array_ids_get(_unit) - 1);  \
    }                                                                                                             \
    if ((dnx_data_field.common_max_val.array_id_type_get(_unit, _fem_array)->is_fem) != 1)                        \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                \
                     "\r\n"                                                                                       \
                     "_encoded_position 0x%08X uses array %d which is not a FEM array.\r\n",                      \
                     _encoded_position, _fem_array);                                                              \
    }                                                                                                             \
    _field_stage = DNX_FIELD_STAGE_IPMF2;                                                                         \
    _nof_fem_id_per_array = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fem_id_per_array;        \
    _nof_fem_array = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fem_array;                     \
    if (_nof_fem_array  == 0)                                                                                     \
    {                                                                                                             \
        /*                                                                                                        \
         * This verification is redundant but is left here to cross verify DATA table.                            \
         */                                                                                                       \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                \
            "Stage \"%s\" (%d) has no FEMs. \r\n",                                                                \
            dnx_field_stage_text(_unit, _field_stage), _field_stage);                                             \
    }                                                                                                             \
    if (dnx_data_field.common_max_val.array_id_type_get(_unit, _fem_array)->fem_array_index > (_nof_fem_array - 1)) \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                  \
                     "\r\n"                                                                                         \
                     "_encoded_position 0x%08X contains FEM array id %d,\r\n"                                       \
                     "while the maximum for stage \"%s\" is %d.\r\n",                                               \
                     _encoded_position, _fem_array,                                                                 \
                     dnx_field_stage_text(_unit, _field_stage),                                                     \
                     _nof_fem_array - 1);                                                                           \
    }                                                                                                               \
    _fem_position = DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(_encoded_position);                                 \
    if (_fem_position >= _nof_fem_id_per_array)                                                                      \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                  \
                     "\r\n"                                                                                         \
                     "_encoded_position 0x%08X contains FEM position of %d within array_id %d,\r\n"                 \
                     "while the maximum for stage   \"%s\" is %d.\r\n",                                             \
                     _encoded_position, _fem_position, _fem_array,                                                  \
                     dnx_field_stage_text(_unit, _field_stage), _nof_fem_id_per_array-1);                            \
    }                                                                                                               \
    _fem_id =                                                                                                       \
        (dnx_data_field.common_max_val.array_id_type_get(                                                           \
                _unit, _fem_array)->fem_array_index * _nof_fem_id_per_array) + _fem_position;                        \
    _nof_fem_id_in_stage = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fems_per_context;          \
    if (_fem_id >= _nof_fem_id_in_stage)                                                                            \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                                                  \
                     "_encoded_position 0x%08X encodes FEM ID %d, while the maximum for stage \"%s\" is %d.\r\n",   \
                     _encoded_position, _fem_id, dnx_field_stage_text(_unit, _field_stage), _nof_fem_id_in_stage-1);\
    }                                                                                                               \
}
/**
 * \brief
 *   Get the encoded 'action_priority' from FEM ID.
 *   Encoding is assumed to have been carried out using BCM_FIELD_ACTION_POSITION.
 *   We allow only odd array ids for FEM, the even arrays are for FEM.
 *   See 'remark' below.
 * \param [out] _encoded_position -
 *   Variable of type bcm_field_action_priority_t.
 *   Encoded position of a FEM. Encoding is done using 'array_id' and 'position'.
 *   See 'BCM_FIELD_ACTION_POSITION' and 'remark' below
 * \param [in] _unit     -
 *   Device ID.
 * \param [in] _fem_id  -
 *   The FEM ID.
 * \return
 *   See '_fem_id' above.
 * \remark
 *   'Stage' is not an input parameter here since FEMs are only relevant for IPMF2.
 *
 *   FESes and FEMs are assigned priority, related to their corresponding actions.
 *   The priority layout of FESes and FEMs is as follows:
 *
 * Priority:      LOW        ...       ...      HIGH
 * Array
 *   id:           0          1         2        3
 * Type:          FES        FEM       FES      FEM
 * Identifier:    0-15       0-7       16-31    8-15
 * \see
 *   * DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET
 *   * DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET
 */
#define DNX_FIELD_ACTION_FEM_ID_ENCODED_POSITION_GET(_encoded_position, _unit, _fem_id)                           \
{                                                                                                                 \
    int _nof_fem_id_per_array;                                                                                    \
    int _nof_fem_array;                                                                                           \
    int _nof_fem_id;                                                                                              \
    int _array;                                                                                                   \
    int _position;                                                                                                \
    dnx_field_stage_e _field_stage = DNX_FIELD_STAGE_IPMF2;                                                       \
    _nof_fem_id_per_array = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fem_id_per_array;       \
    _nof_fem_array = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fem_array;                     \
    _nof_fem_id = dnx_data_field.stage.stage_info_get(_unit, _field_stage)->nof_fem_id;                           \
    if (_fem_id >= _nof_fem_id)                                                                                   \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEM ID %d is bigger then the maximum %d.\r\n", _fem_id, _nof_fem_id - 1);     \
    }                                                                                                             \
    _array = (((_fem_id / _nof_fem_id_per_array) * 2) + 1);                                                       \
    _position = (_fem_id % _nof_fem_id_per_array);                                                                \
    if (((_array - 1) / 2) >= _nof_fem_array)                                                                     \
    {                                                                                                             \
     SHR_ERR_EXIT(_SHR_E_INTERNAL, "FEM ARRAY %d is bigger then the maximum %d.\r\n", _array, _nof_fem_array - 1);\
    }                                                                                                             \
    _encoded_position = BCM_FIELD_ACTION_POSITION(_array, _position);                                             \
}
/*
 * }
 */
/*
 * Verify the _context_id is allocated for a specified _stage
 * If 'context_id' is not allocated for that '_stage', go to exit with error _SHR_E_NOT_FOUND
 * If '_no_print' is non-zero then just set error code and go to exit. Otherwise,
 * also print error message.
 */
#define DNX_FIELD_CONTEXT_IS_ALLOCATED(_stage, _context_id, _no_print) \
    do \
    { \
        uint8 _is_allocated = 0; \
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, _stage, _context_id, &_is_allocated)); \
        if (!_is_allocated) \
        { \
            if (_no_print != 0) \
            { \
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND); \
            } \
            else \
            { \
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, \
                         "context_id (%d) is not allocated to stage %s \r\n", \
                         _context_id, dnx_field_stage_text(unit, _stage)); \
            } \
        } \
    } while (0)

/*
 * Verify the context_id is valid for specified unit
 */
#define DNX_FIELD_CONTEXT_ID_VERIFY(_unit,_context_id)                                                   \
    if ((unsigned int) (_context_id) >= (unsigned int) (dnx_data_field.common_max_val.nof_contexts_get(_unit))) \
    { \
        SHR_ERR_EXIT(_SHR_E_PARAM, \
                     "context_id (%d) is out of range (Should be between %d and %d)\r\n", \
                     _context_id, 0, dnx_data_field.common_max_val.nof_contexts_get(_unit) - 1); \
    }

/**
 * Verify the field group ID is in range for current unit
 */
#define DNX_FIELD_GROUP_ID_VERIFY(_fg_id)                                                   \
    if ((unsigned int) (_fg_id) >= (unsigned int) (dnx_data_field.group.nof_fgs_get(unit))) \
    { \
        SHR_ERR_EXIT(_SHR_E_PARAM, \
                     "_fg_id (%d) is out of range (Should be between %d and %d)\r\n", \
                     _fg_id, 0, dnx_data_field.group.nof_fgs_get(unit) - 1); \
    }
/**
 * Verify the field group ID is in range for current unit
 */
#define DNX_FIELD_GROUP_ID_KBP_VERIFY(_fg_id)                                                   \
    if ((unsigned int) (_fg_id) >= (unsigned int) (dnx_data_field.kbp.nof_fgs_get(unit))) \
    { \
        SHR_ERR_EXIT(_SHR_E_PARAM, \
                     "_fg_id (%d) is out of range (Should be between %d and %d)\r\n", \
                     _fg_id, 0, dnx_data_field.kbp.nof_fgs_get(unit) - 1); \
    }

/**
 * Verify the stage is valid.
 */
#define DNX_FIELD_STAGE_VERIFY(_stage)                                                   \
    if (((int) (_stage) < DNX_FIELD_STAGE_FIRST) || (_stage >= DNX_FIELD_STAGE_NOF))  \
    {  \
        SHR_ERR_EXIT(_SHR_E_PARAM,  \
                     "\r\n" \
                     "field_stage (%d) is out of range (Should be between %d and %d)\r\n",  \
                     _stage, DNX_FIELD_STAGE_FIRST, DNX_FIELD_STAGE_NOF - 1);  \
    }
/**
 * Verify that 'field group type' is valid.
 * See dnx_field_group_type_e
 */
#define DNX_FIELD_FG_TYPE_VERIFY(_fg_type)                                                   \
    if (((int) (_fg_type) < DNX_FIELD_GROUP_TYPE_FIRST) || (_fg_type >= DNX_FIELD_GROUP_TYPE_NOF))  \
    {  \
        SHR_ERR_EXIT(_SHR_E_PARAM,  \
                     "\r\n" \
                     "fg_type (%d) is out of range (Should be between %d and %d)\r\n",  \
                     _fg_type, DNX_FIELD_GROUP_TYPE_FIRST, DNX_FIELD_GROUP_TYPE_NOF - 1);  \
    }
/**
 * Verify that 'key id' is valid.
 * See dbal_enum_value_field_field_key_e
 */
#define DNX_FIELD_KEY_ID_VERIFY(_key_id)                                                   \
    if (((int) (_key_id) < DBAL_ENUM_FVAL_FIELD_KEY_A) || ((_key_id) >= DBAL_NOF_ENUM_FIELD_KEY_VALUES))  \
    {  \
        SHR_ERR_EXIT(_SHR_E_PARAM,  \
                     "\r\n" \
                     "key_id (%d) is out of range (Should be between %d and %d)\r\n",  \
                     _key_id, DBAL_ENUM_FVAL_FIELD_KEY_A, DBAL_NOF_ENUM_FIELD_KEY_VALUES - 1);  \
    }
/**
 * Verify the field group ID is valid for specified unit
 */
#define DNX_FIELD_FG_ID_VERIFY(_unit,_fg_id)                                                   \
        if ((unsigned int) (_fg_id) >= (unsigned int) (dnx_data_field.group.nof_fgs_get(_unit))) \
        { \
            SHR_ERR_EXIT(_SHR_E_PARAM, \
                         "\r\n" \
                         "Field Group ID (%d) is out of range (Should be between %d and %d)\r\n", \
                         _fg_id, 0, dnx_data_field.group.nof_fgs_get(_unit) - 1); \
        }
/**
 * Verify the compare pair ID is valid for specified unit
 */
#define DNX_FIELD_COMPARE_PAIR_VERIFY(_unit,_pair_id)                                                   \
        if ((unsigned int) (_pair_id) >= (unsigned int) (dnx_data_field.common_max_val.nof_compare_pairs_get(_unit))) \
        { \
            SHR_ERR_EXIT(_SHR_E_PARAM, \
                         "\r\n" \
                         "Compare pair ID (%d) is out of range (Should be %d or %d)\r\n", \
                         _pair_id, 0, (dnx_data_field.common_max_val.nof_compare_pairs_get(_unit) - 1)); \
        }
/**
 * Verify the ACE group ID is valid for specified unit
 */
#define DNX_FIELD_ACE_ID_VERIFY(_unit,_ace_id)                                                      \
        if ((unsigned int) (_ace_id) >= (unsigned int) (dnx_data_field.ace.nof_ace_id_get(_unit))) \
        { \
            SHR_ERR_EXIT(_SHR_E_PARAM, \
                         "ACE ID (%d) is out of range (Should be between %d and %d)\r\n", \
                         _ace_id, 0, dnx_data_field.ace.nof_ace_id_get(_unit) - 1); \
        }

 /**
 * Check if an ACE format ID is within the range of ACE (ACL extension) or not (for example if it is a PPMC ACE format
 * or operates on outlifs).
 */
#define DNX_FIELD_ACE_FORMAT_ID_IS_ACE(_unit, _ace_format_id)                                            \
    ((_ace_format_id) >= dnx_data_field.ace.ace_id_pmf_alloc_first_get(_unit) &&                           \
     (_ace_format_id) <= dnx_data_field.ace.ace_id_pmf_alloc_last_get(_unit))

/**
 * Check if an ACE key ID is within the range of ACE (ACL extension) or not (for example if it is a PPMC ACE format
 * or operates on outlifs).
 */
#define DNX_FIELD_ACE_KEY_IS_ACE(_unit, _ace_key)                                            \
    ((_ace_key) >= dnx_data_field.ace.min_key_range_pmf_get(_unit) &&                          \
     (_ace_key) <= dnx_data_field.ace.max_key_range_pmf_get(_unit))

/**
 * Check if any of the input types of the supplied _qual_info(dnx_field_qual_attach_info_t) is cascaded.
 * If yes, sets the return parameter '_cascaded' to TRUE, else it stays FALSE.
 */
#define DNX_FIELD_IS_ANY_QUAL_CASCADED(_unit, _qual_info, _cascaded)                                                            \
    do                                                                                                                          \
    {                                                                                                                           \
        int _qual_index;                                                                                                         \
        _cascaded = FALSE;                                                                                                      \
        for (_qual_index = 0; _qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&                                          \
                 _qual_info[_qual_index].input_type != DNX_FIELD_INPUT_TYPE_INVALID; _qual_index++)                               \
        {                                                                                                                       \
            if (_qual_info[_qual_index].input_type == DNX_FIELD_INPUT_TYPE_CASCADED)                                             \
            {                                                                                                                   \
                _cascaded = TRUE;                                                                                              \
                break;                                                                                                          \
            }                                                                                                                   \
        }                                                                                                                       \
    }                                                                                                                           \
    while(0)

/*
 * } Macros
 */

/*
 * { Define
 */

/*
 * 'Invalid' indication for FEM_KEY_SELECT field (in IPPC_PMF_FEM_PROGRAM)
 * This should occupy the same number of bits as FEM_KEY_SELECT field,
 * all set to '1' (Maximal value within this field).
 */
#define DNX_FIELD_FEM_KEY_SELECT_INVALID 127
/*
 * 'Invalid' indication for range min/max value
 */
#define DNX_FIELD_RANGE_VALUE_INVALID (-1)

/**
 * Default context is created for case when TCAM CS is miss and context 0 is chosen by defualt
 * If user dont want to configure preselectors he can use default context for grou_context_attach
 * The default context is created on init for all stages
 */
#define DNX_FIELD_CONTEXT_ID_DEFAULT(_unit) (dnx_data_field.context.default_context_get(_unit))

/** The FES program ID that is the default FES program ID and performs only invalid actions.*/
#define DNX_ALGO_FIELD_ZERO_FES_PROGRAM           0
/** The FES action mask ID that is the default mask.*/
#define DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID    0
/** The default FES action mask, defined as the one that does nothing to the action output.*/
#define DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK   0
/**
 * The FES action mask ID that is the default mask for the ACE ACR.
 * Different from DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID because the ACE ACR has a different number of action masks
 * than the PMF EFESes.
 */
#define DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID    0

/**
 * The default value of the 'input argument' parameter in attach info.
 * At the moment serves only as the default layer index.
 */
#define DNX_FIELD_ATTACH_INFO_FIELD_INPUT_ARG_DEFAULT  (0)
/**
 * The default value of the 'offset' parameter in attach info.
 */
#define DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT (0)
/**
 * Determines if we allow the user to disable the valid bit for non-void actions in a
 * lookup field group (TCAM, EXEM, ACE...).
 * The reason we prohibit users from not nosing valid bits for non void actions on all field groups except
 * direct exraction and KBP, is that we cannot disable them if we have a miss.
 * Once we have the use of the EFES 2msb feature we could disable those actions if we have a miss,
 * and we expect to enable this constant (or tight it altogether.)
 */
#define DNX_FIELD_ALLOW_VALID_BIT_CHOICE_FOR_LOOKUP_FG (0)
/**
 * In order to determine if the MDB profile supports EXEM, we determine the minimum capacity for EXEM
 * for which we allow an EXEM field group to be configured.
 */
#define DNX_FIELD_EXEM_MINIMUM_CAPACITY (3)
/**
 * The maximum number of context selection qualifiers for a single stage.
 */
#define DNX_FIELD_CS_QUAL_NOF_MAX (42)
/**
 * The minimum number of ACL keys for to be reserved in KBP for each FWD context.
 */
#define DNX_FIELD_KBP_ACL_KEYS_MIN (2)
/**
 * The minimum number of bits required for VMV.
 * At least 2 bits to support up to one quarter entries, plus 2 bits for EFES 2MSB feature.
 */
#define DNX_FIELD_EXEM_MIN_VMV_SIZE (2+2)

/*
 * } Define Constant
 */

/*
 * { Define Constant Invalids
 */
/**
 * Invalid indication for 'FEM program'.
 * See, for example, dnx_field_actions_fem_set() or dnx_field_fem_action_add()
 */
#define DNX_FIELD_FEM_PROGRAM_INVALID      ((dnx_field_fem_program_t)(-1))
/*
 * } Define
 */

#endif /* DNX_FIELD_H_INCLUDED */
