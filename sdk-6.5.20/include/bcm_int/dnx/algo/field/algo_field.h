/**
 *  \file bcm_int/dnx/algo/field/algo_field.h
 * 
 *  Internal DNX RX APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_FIELD_H_INCLUDED

#define ALGO_FIELD_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * { Include
*/

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <include/bcm_int/dnx/field/field_map.h>
#include <include/bcm_int/dnx/field/field_actions.h>
#include <include/bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/*
 * }
 */

/*
 * { Macros
 */

#ifndef MIN
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a)>(b)) ? (a) : (b))
#endif

/*
 * }
 */

/*
 * { Define
 */

#define DNX_ALGO_FIELD_IPMF_2_FFC              "IPMF_2_FFC"
#define DNX_ALGO_FIELD_IPMF_3_FFC              "IPMF_3_FFC"
#define DNX_ALGO_FIELD_EPMF_FFC                "EPMF_FFC"
#define DNX_ALGO_FIELD_IFWD2_FFC               "IFWD2_FFC"
#define DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_OCC_BMP "IPMF1_INITIAL_KEY_OCC_BMP"

#define DNX_ALGO_FIELD_PMF_A_FES_ID           "IPMF_A_FES_ID"
#define DNX_ALGO_FIELD_PMF_B_FES_ID           "IPMF_B_FES_ID"
#define DNX_ALGO_FIELD_E_PMF_FES_ID           "EPMF_FES_ID"

/*
 * Allocation of FEM program.
 * Currently, this is done on adding FEM to fiewld group.
 */
#define DNX_ALGO_FIELD_PMF_A_FEM_PGM_ID       "IPMF_A_FEM_PGM_ID"

#define DNX_ALGO_FIELD_LINK_PROFILES          "LINK_PROFILES"
#define DNX_ALGO_FIELD_IPMF1_CONTEXT_ID       "IPMF1_CONTEXT_ID"
#define DNX_ALGO_FIELD_IPMF2_CONTEXT_ID       "IPMF2_CONTEXT_ID"
#define DNX_ALGO_FIELD_IPMF3_CONTEXT_ID       "IPMF3_CONTEXT_ID"
#define DNX_ALGO_FIELD_EPMF_CONTEXT_ID        "EPMF_CONTEXT_ID"

#define DNX_ALGO_FIELD_USER_QUAL_ID           "USER_QUAL_ID"
#define DNX_ALGO_FIELD_USER_ACTION_ID         "USER_ACTION_ID"

#define DNX_ALGO_FIELD_GROUP_ID                     "FIELD_GROUP_ID"
/** The resource manager instance name used to allocate APP_DB_IDs to SEXEM-3.*/
#define DNX_ALGO_FIELD_SEXEM_APP_DB_ID              "SEXEM_APP_DB_ID"
/** The resource manager instance name used to allocate APP_DB_IDs to LEXEM.*/
#define DNX_ALGO_FIELD_LEXEM_APP_DB_ID              "LEXEM_APP_DB_ID"
/** The resource manager instance name used to allocate ACE ID (result type).*/
#define DNX_ALGO_FIELD_ACE_ID                       "ACE_ID"
/** The resource manager instance name used to allocate ACE keys (PPMC keys) for MC replication and ACE pointers.*/
#define DNX_ALGO_FIELD_ACE_KEY                      "ACE_KEY"
#define DNX_ALGO_FIELD_ENTRY_TCAM_ACCESS_ID         "ENTRY_TCAM_ACCESS_ID"
/** The resource manager instance name used to allocate profile IDs for iPMF1 context selection based on KBP.*/
#define DNX_ALGO_FIELD_KBP_OPCODE_CS_PROFILE_ID     "KBP_OPCODE_CS_PROFILE_ID"
/** The resource manager instance name used to allocate user defined Apptypes.*/
#define DNX_ALGO_FIELD_KBP_USER_APPTYPE             "KBP_USER_APPTYPE"
/** The resource manager instance name used to allocate Flush Profile ID to SEXEM-3.*/
#define DNX_ALGO_FLUSH_PROFILE_ID                   "FLUSH_PROFILE_ID"

/*
 * Definitions and globals related to 'action' templates (one per FEM).
 * {
 */
/*
 * This is the base string that is used for all templates assigned
 * to 'action's on FEMs. There are 16 FEMs so there are 16 templates.
 */
#define DNX_ALGO_BASE_ACTION_TEMPLATE_RESOURCE      "ACTION TEMPLATE PER FEM"

/*
 * }
 */
/**
 * The value in place_in_alloc in dnx_algo_field_action_fes_alloc_out_fes_quartet_change_t that indicates
 * that the FES quartet is a movement, not a new FES quartet.
 */
#define DNX_ALGO_FIELD_ACTION_MOVEMENT (-1)

/**
 * The value for the first "place_in_fg" for actions added post attach.
 */
#define DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)

/**
 * The context link profile Invalid value
 */
#define DNX_ALGO_FIELD_CONTEXT_LINK_PROFILE_INVALID (0)

/*
 * Defines for 'algo_field_key' (Key allocation)
 * {
 */
/**
 * Defines for SW STATE
 * Shortcuts containing a few steps from root of SW STATE till indicated field.
 * See field_key_alloc.xml
 * {
 */
#define KEYS_AVAILABILITY_STAGE_INFO       dnx_field_keys_per_stage_allocation_sw.keys_availability.keys_availability_stage_info
#define KEYS_AVAILABILITY_GROUP_INFO       KEYS_AVAILABILITY_STAGE_INFO.keys_availability_group_info
#define KEY_STAGE_PROG_OCCUPATION          dnx_field_keys_per_stage_allocation_sw.keys_per_stage.key_stage_prog_occupation
#define KEY_STAGE_SUPPORTS_BITMAP_ALLOCATION    dnx_field_keys_per_stage_allocation_sw.keys_per_stage.key_on_stage_supports_bitmap_allocation
#define KEY_OCCUPATION                     KEY_STAGE_PROG_OCCUPATION.key_occupation
#define KEY_OCCUPATION_BIT_RANGE           KEY_STAGE_PROG_OCCUPATION.key_occupation.bit_range_desc
/*
 * }
 */

/*
 * Enums for 'algo_field_key' (Key allocation)
 * {
 */
typedef enum
{
    /*
     * Allocate key with id that was given by user in API
     */
    DNX_ALGO_FIELD_KEY_FLAG_WITH_ID = 0x1,
    DNX_ALGO_FIELD_KEY_FLAG_NOF
} dnx_algo_field_key_flags_e;
/*
 * }
 */

/*
 * Definitions related to 'bit-range' feature.
 * {
 */
/**
 * \brief A very custom macro to be used to verify that a specified half key is indeed marked
 *   as occupied bit on the 'half key' system and on the 'bit-range' system.
 * \param [in] _unit  -
 *   HW identifier of this unit.
 * \param [in] _field_stage   -
 *   Stage on which investigated key (and half key) resides.
 * \param [in] _context_id   -
 *   Context on stage on which investigated key (and half key) resides.
 * \param [in] _half_key   -
 *   Index of half key, starting from 'first_available_key_on_stage' with even indices for
 *   MS halves and odd for LS halves.
 * \param [in] _key_id   -
 *   Key (A-J) on which specified half key resides. This is required just for display purposes.
 * \param [in] _bit_range_offset_within_key   -
 *   Offset, in bits, from LS bit, within key, where invetigated bit-range is assumed to reside.
 * \param [in] _ms_or_ls   -
 *   String. Either "MS" or "LS" depending which half key it is. For display purposes only.
 * \see
 *   * dnx_algo_field_key_bit_range_free_generic()
 */
#define DNX_FIELD_KEY_VERIFY_HALF_KEY_OCCUPATION(_unit, _field_stage, _context_id, _half_key, _key_id, _bit_range_offset_within_key,_ms_or_ls) \
{ \
    uint8 _is_half_key_range_occupied; \
    dnx_field_key_half_occupation_state_e _key_occupation_state; \
    SHR_IF_ERR_EXIT(KEY_OCCUPATION.  \
                    is_half_key_range_occupied.get(_unit, _field_stage, _context_id, _half_key,&_is_half_key_range_occupied));  \
    if (_is_half_key_range_occupied != TRUE)  \
    {  \
        LOG_ERROR_EX(BSL_LOG_MODULE,  \
            "\r\n"  \
            "Fail on free bit-range on key: field_stage %s context_id %d key_id %c bit_range_offset_within_key %d\r\n",  \
            dnx_field_stage_e_get_name(_field_stage), _context_id, 'A' + _key_id.id[0], _bit_range_offset_within_key);  \
        SHR_ERR_EXIT(_SHR_E_PARAM,  \
             "\r\n"  \
             "%s half of input key_id is marked as 'not occupied' on bit-range system. Illegal. Quit\r\n",_ms_or_ls);  \
    }  \
    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get  \
                                (_unit, _field_stage, _context_id, _half_key, &_key_occupation_state));  \
    if (_key_occupation_state == DNX_FIELD_KEY_OCCUPY_NONE)  \
    {  \
        LOG_ERROR_EX(BSL_LOG_MODULE,  \
            "\r\n"  \
            "Fail on free bit-range on key: field_stage %s context_id %d key_id %c bit_range_offset_within_key %d\r\n",  \
            dnx_field_stage_e_get_name(_field_stage), _context_id, 'A' + _key_id.id[0], _bit_range_offset_within_key);  \
        SHR_ERR_EXIT(_SHR_E_PARAM,  \
             "\r\n"  \
             "%s half of input key_id is marked as 'not occupied' on half key system. Illegal. Quit\r\n",_ms_or_ls);  \
    }  \
}
/**
 * \brief A very custom macro to be used to verify that a specified half key, at a specified
 *   offset, is allocated and is indeed marked with 'filed group type' which is the same as on input.
 * \param [in] _unit  -
 *   HW identifier of this unit.
 * \param [in] _field_stage   -
 *   Stage on which investigated key (and half key) resides.
 * \param [in] _context_id   -
 *   Context on stage on which investigated key (and half key) resides.
 * \param [in] _fg_type   -
 *   Field group type which is to be compared with the one stored in swstate.
 * \param [in] _half_key   -
 *   Index of half key, starting from 'first_available_key_on_stage' with even indices for
 *   MS halves and odd for LS halves.
 * \param [in] _key_id   -
 *   Key (A-J) on which specified half key resides. This is required just for display purposes.
 * \param [in] _bit_range_offset_within_key   -
 *   Offset, in bits, from LS bit, within key, where invetigated bit-range is assumed to reside.
 * \param [in] _ms_or_ls   -
 *   String. Either "MS" or "LS" depending which half key it is. For display purposes only.
 * \param [out] _key_length_local   -
 *   This variable is supplied by the caller and is loaded, by this macro, by the number of bits
 *   in the pointed 'bit-range'.
 * \see
 *   * dnx_algo_field_key_bit_range_free_generic()
 */
#define DNX_FIELD_KEY_VERIFY_FG_TYPE_FOR_BIT_RANGE(_unit, _field_stage, _context_id, _fg_type, _half_key, _key_id, _bit_range_offset_within_key,_ms_or_ls,_key_length_local) \
{ \
    uint8 _local_fg_type; \
    /*  \
     * Verify bit-range was indeed allocated at this offset.  \
     */  \
    SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.bit_range_size.get  \
                    (_unit, _field_stage, _context_id, _half_key,  \
                    (uint32) ((_bit_range_offset_within_key >= DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)?  \
                        (_bit_range_offset_within_key - DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF):_bit_range_offset_within_key),  \
                     &_key_length_local));  \
    if (_key_length_local == 0)  \
    {  \
        SHR_ERR_EXIT(_SHR_E_INTERNAL,  \
                     "\r\n"  \
                     "Stage %s context_id %d\r\n"  \
                     "Bit %d on key %c (%s half) is marked as 'free'. Cannot release a 'free' chunk.\r\n"  \
                     "This is probably the wrong offset.\r\n",  \
                     dnx_field_stage_e_get_name(_field_stage),_context_id,  \
                    (unsigned int) ((_bit_range_offset_within_key >= DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)?  \
                        (_bit_range_offset_within_key - DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF):_bit_range_offset_within_key),  \
                     'A' + (unsigned char) (_key_id.id[0]),_ms_or_ls );  \
    }   \
    /*  \
     * '_key_length_local' now contains the number of bits that have been allocated at  \
     * offset 'bit_range_offset_within_key'.  \
     */  \
    SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.fg_type.get \
                    (_unit, _field_stage, _context_id, _half_key, \
                    (uint32)((_bit_range_offset_within_key >= DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)? \
                        (_bit_range_offset_within_key - DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF):_bit_range_offset_within_key), \
                    &_local_fg_type)); \
    if ((dnx_field_group_type_e)_local_fg_type != _fg_type) \
    { \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, \
                     "\r\n" \
                     "Stage %s context_id %d\r\n" \
                     "Field group type (%s) on this key (%c, %s half) and offset (%d) does not\r\n" \
                     "match field group type specified by the caller (%s).\r\n" \
                     "Probably caller's mistake.\r\n", \
                     dnx_field_stage_e_get_name(_field_stage),_context_id, \
                     dnx_field_group_type_e_get_name(_local_fg_type), \
                     'A' + (unsigned char) (_key_id.id[0]),_ms_or_ls,(unsigned int) _bit_range_offset_within_key, \
                     dnx_field_group_type_e_get_name(_fg_type)); \
    } \
}
/*
 * Related to 'bit-range':
 * Minimal number of bits that may be required to be allocated on 'bit-range'
 * (via dnx_algo_field_key_bit_range_alloc_generic()). See dnx_algo_field_key_bit_range_alloc_generic_verify
 */
#define DNX_FIELD_KEY_BIT_RANGE_MIN_LENGTH 1
/*
 * Related to 'bit-range':
 * Maximal value for the number of bit, relative to the first bit on 'key template', which is
 * required to be '16 bit aligned' when allocating 'bit-range'
 * See dnx_algo_field_key_bit_range_alloc_generic().
 */
#define DNX_FIELD_KEY_MAX_VALUE_FOR_ALIGNED_BIT (DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)

/*
 * }
 */

/*
 * Enums
 * {
 */
/**
 * Collection of values out of which, one is loaded into 'failed_to_find_p'
 * (output of dnx_algo_field_key_alloc_generic())
 */
typedef enum
{
    DNX_FIELD_KEY_FAILED_FIRST = 0,
    /**
     * There was NO FAILURE.
     * Key was found in a place which has not, previously, been occupied.
     */
    DNX_FIELD_KEY_FAILED_NO_FAILURE = DNX_FIELD_KEY_FAILED_FIRST,
    /**
     * No key was found because all available space was already occupied.
     */
    DNX_FIELD_KEY_FAILED_NO_PLACE,
    /**
     * No key was found because no available space was set for specified input combination
     * (field group type, PMF program, field stage).
     */
    DNX_FIELD_KEY_FAILED_NOT_INITIALIZED,
    /**
     * No key was found because configuration has not yet been assigned for specified
     * field group type.
     */
    DNX_FIELD_KEY_FAILED_NOT_ASSIGNED_TO_FG,
    /**
     * No key was found because configuration is that specified field group type has zero
     * keys.
     */
    DNX_FIELD_KEY_FAILED_FG_HAS_ZERO_KEYS,
    /**
     * Number of types in dnx_field_key_failed_to_find_e
     */
    DNX_FIELD_KEY_FAILED_NOF
} dnx_field_key_failed_to_find_e;

/*
 * }
 * Enums
 */

/*
 * } Define
*/

/*
 * typedefs
 * {
 */

/**
 * Structure containing all the information provided to the FES allocation algorithm about the current
 * state of a single FES ID of a single context ID (the FES quartets that is is allocated).
 * see dnx_algo_field_action_fes_alloc_single_context_state_in_t.
 * see dnx_algo_field_action_fes_alloc_state_in_t.
 * see dnx_algo_field_action_fes_allocate()
 */
typedef struct
{
    /*
     * The FES program ID (and therefore FES quartet) used by the context ID for the FES ID.
     */
    dnx_field_fes_pgm_id_t fes_pgm_id;
    /*
     * The masks used by the context ID for each instruction in the FES quartet.
     */
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    /*
     * The action types used by each fes2msb instruction in the FES quartet.
     */
    dnx_field_action_type_t fes_action_type[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    /*
     * Whether or not another context uses the same FES program ID.
     */
    uint8 is_shared;
} dnx_algo_field_action_fes_alloc_single_context_single_fes_quartet_state_in_t;

/**
 * Structure containing all the information provided to the FES allocation algorithm about the current
 * state of the FES configuration for one FES ID.
 * see dnx_algo_field_action_fes_alloc_fes_state_in_t.
 * see dnx_algo_field_action_fes_alloc_state_in_t.
 * see dnx_algo_field_action_fes_allocate()
 */
typedef struct
{
    /*
     * The number of FES quartets in use by the FES ID by all contexts.
     */
    unsigned int nof_fes_quartets;
    /*
     * The number of masks in use by the FES ID by all contexts.
     */
    unsigned int nof_masks;
} dnx_algo_field_action_fes_alloc_single_fes_if_fes_state_in_t;

/**
 * Structure containing all the information provided to the FES allocation algorithm about the current
 * state of a single context ID (the FES quartets that is is allocated).
 * Used to provide the current allocation status of the context ID we are working on.
 * Also used to find the priority of FES quartets.
 * see dnx_algo_field_action_fes_alloc_state_in_t.
 * see dnx_algo_field_action_fes_allocate()
 */
typedef struct
{
    /*
     * The current state for each FES ID.
     */
    dnx_algo_field_action_fes_alloc_single_context_single_fes_quartet_state_in_t
        fes_id_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_algo_field_action_fes_alloc_single_context_state_in_t;

/**
 * Structure containing all the information provided to the FES allocation algorithm about the current
 * state of the FES configuration.
 * see dnx_algo_field_action_fes_alloc_state_in_t.
 * see dnx_algo_field_action_fes_allocate()
 */
typedef struct
{
    /*
     * The current state for each FES ID.
     */
    dnx_algo_field_action_fes_alloc_single_fes_if_fes_state_in_t
        fes_id_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_algo_field_action_fes_alloc_fes_state_in_t;

/**
 * Structure containing all the information provided to the FES allocation algorithm about the current state.
 * see dnx_algo_field_action_fes_context_state_in_t.
 * dnx_algo_field_action_fes_allocate()
 * see dnx_field_actions_fes_set().
 */
typedef struct
{
    /*
     * The current state given for a specific context ID
     */
    dnx_algo_field_action_fes_alloc_single_context_state_in_t context_state;
    /*
     * The current state of each FES ID.
     */
    dnx_algo_field_action_fes_alloc_fes_state_in_t fes_state;
} dnx_algo_field_action_fes_alloc_state_in_t;

/**
 * Structure containing all the information provided to the FES allocation algorithm about a
 * specific FES quartet to be added.
 * Used in an array to provide, for each FES ID, the priorities of each fes program.
 * see dnx_algo_field_action_fes_alloc_in_t.
 */
typedef struct
{
    /*
     * The action's priority.
     */
    dnx_field_action_priority_t priority;
    /*
     * A possible mask ID allocation, ignoring all other FES quartets 
     * (used to provide number of distinct non zero masks.
     */
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    /*
     * The action types used by each fes2msb instruction in the FES quartet..
     */
    dnx_field_action_type_t fes_action_type[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} dnx_algo_field_action_fes_alloc_in_fes_quartet_t;

/**
 * Structure containing all the information provided to the FES allocation algorithm about the
 * fes quartets to be added.
 * see dnx_algo_field_actions_context_state_t.
 * dnx_algo_field_action_fes_allocate()
 * see dnx_field_actions_fes_set().
 */
typedef struct
{
    /*
     * the number of FES quartets that need to be allocated.
     */
    unsigned int nof_fes_quartets;
    /*
     * The information given for each fes quartet.
     */
        dnx_algo_field_action_fes_alloc_in_fes_quartet_t
        fes_quartet[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
} dnx_algo_field_action_fes_alloc_in_t;

/**
 * Structure containing the movements of existing FES quartets.
 * dnx_algo_field_action_fes_allocate()
 * see dnx_field_actions_fes_set().
 * see dnx_algo_field_action_fes_alloc_out_t.
 */
typedef struct
{
    /*
     * The index of new FES quartet in dnx_algo_field_action_fes_alloc_in_t fes quartet.
     * If it is a movement and not a new FES quartet, the value is DNX_ALGO_FIELD_ACTION_MOVEMENT.
     */
    int place_in_alloc;
    /*
     * The FES ID from which we move.
     */
    dnx_field_fes_id_t source_fes_id;
    /*
     * The FES PROGRAM ID from which we move (if we move).
     */
    dnx_field_fes_pgm_id_t source_fes_pgm_id;
    /*
     * The list of source mask IDs for the FES instructions (if we move).
     */
    dnx_field_fes_mask_id_t source_fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    /*
     * If the source FES quartet is shared (if we move).
     */
    uint8 source_is_shared;
    /*
     * The new FES ID of the FES quartet.
     */
    dnx_field_fes_id_t dest_fes_id;
    /*
     * The new FES program ID of the FES quartet.
     */
    dnx_field_fes_pgm_id_t dest_fes_pgm_id;
    /*
     * The list of newly allocated masks
     * Invalid if shared.
     */
    dnx_field_fes_mask_id_t dest_fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    /*
     * If the FES quartet is shared.
     */
    uint8 dest_is_shared;
} dnx_algo_field_action_fes_alloc_out_fes_quartet_change_t;

/**
 * Structure containing the results of the allocation algorithm.
 * see dnx_algo_field_action_fes_allocate()
 * see dnx_field_actions_fes_set().
 */
typedef struct
{
    
    dnx_algo_field_action_fes_alloc_out_fes_quartet_change_t
        fes_quartet_change[(1 +
                            DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_ID_PER_ARRAY) *
                           DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_ID_PER_ARRAY * (2 / 2) *
                           DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_ARRAY];
    /*
     * The number of elements in the 'fes_quartet_change'array.
     */
    unsigned int nof_fes_quartet_changes;

} dnx_algo_field_action_fes_alloc_out_t;

/**
 * Structure containing all the information provided to the FES deallocation algorithm about the current
 * state of each context ID (at this point information about only one context ID is passed).
 * Used in an array to provide the current allocation status of the FES quartets
 * see dnx_algo_field_action_fes_dealloc_state_in_t.
 */
typedef struct
{
    /*
     * The FES program ID (and therefore FES quartet) used by the context ID for each FES ID.
     * Zero FES program indicates that no FES quartet is allocated for the FES_ID.
     */
    dnx_field_fes_pgm_id_t fes_pgm_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    /*
     * The Number of context IDs that use each FES quartet.
     * Note that we use uint32 because it is the type used for context IDs, even though we have only 64 programs.
     */
    uint8 fes_quartet_nof_refs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    /*
     * The Number of context IDs that use each FES action mask.
     * Zero indicated the the context ID that we deallocate for doesn't use that mask (whether or not others use it).
     */
    uint8
        mask_nof_refs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
        [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
} dnx_algo_field_action_fes_dealloc_context_state_in_t;

/**
 * Structure containing all the information provided to the FES deallocation algorithm about the current state.
 * See dnx_algo_field_action_fes_context_state_in_t.
 * See dnx_algo_field_action_fes_dealloc().
 * See dnx_field_actions_fes_context_group_delete().
 */
typedef struct
{
    /*
     * The current state given for the specific context ID we deallocate from.
     */
    dnx_algo_field_action_fes_dealloc_context_state_in_t context_state;

} dnx_algo_field_action_fes_dealloc_state_in_t;

/**
 * Structure containing the results of the deallocation algorithm.
 * Provides the list of FES quartets to be deletes from HW.
 * dnx_algo_field_action_fes_dealloc().
 * dnx_field_actions_fes_context_group_delete().
 */
typedef struct
{
    /*
     * Marks if the FES ID is being used by the field group that we delete from the context ID.
     */
    int belongs_to_fg[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    /*
     * The FES program ID of the FES quartet to delete in each FES ID.
     * Zero FES program ID indicates not to delete.
     * We could have used only an indication of whether to delete or not the FES quartet belonging to the context ID.
     */
    dnx_field_fes_pgm_id_t fes_pgm_id_to_delete[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    /*
     * Indication on whether to delete or not each fes action mask.
     */
    int delete_mask[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
        [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
} dnx_algo_field_action_fes_dealloc_out_t;

/**
 * Structure containing all the information provided to the ACE ACR FES allocation algorithm about the current state.
 * Since the only resource that has contention in the ACE ACR in the mask (and we do not take priority into
 * consideration as there is no contention between groups) the only information we care about is the action masks.
 * Technically, we can omit the zero indexed action mask if we assume the convention (that we use) that the mask with
 * ID DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID is always DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK.
 * dnx_algo_field_action_ace_fes_allocate()
 * see dnx_field_actions_ace_fes_set().
 */
typedef struct
{
    /*
     * The current state of the action masks in the ACE ACR FESes.
     */
    dnx_field_fes_mask_t
        masks[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT][DNX_DATA_MAX_FIELD_ACE_NOF_MASKS_PER_FES];
} dnx_algo_field_action_ace_fes_alloc_state_in_t;

/**
 * Structure containing all the information provided to the FES allocation algorithm about the
 * actions to be added. Includes the number of actions and their required masks.
 * dnx_algo_field_action_ace_fes_allocate()
 * see dnx_field_actions_ace_fes_set().
 */
typedef struct
{
    /*
     * the number of FES actions that need to be allocated.
     */
    unsigned int nof_actions;
    /*
     * The masks that the actions require.
     */
    dnx_field_fes_mask_t masks[DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT];
} dnx_algo_field_action_ace_fes_alloc_in_t;

/**
 * Structure containing the results of the allocation algorithm for ACE ACR FES configuration.
 * see dnx_algo_field_action_ace_fes_allocate()
 * see dnx_field_actions_ace_fes_set().
 */
typedef struct
{
    /*
     * The FES ID for each action.
     */
    dnx_field_fes_id_t fes_id[DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT];
    /*
     * The mask ID for each action. Using DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID indicates no mask to write.
     */
    dnx_field_fes_id_t mask_id[DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT];
    /*
     * The number of elements in the arrays above. Not necessary.
     */
    unsigned int nof_actions;
} dnx_algo_field_action_ace_fes_alloc_out_t;

/**
 * Structure containing the information about the SW state of a FES quartet.
 * See dnx_algo_field_action_fes_quartet_sw_state_info_get()
 */
typedef struct
{
    /*
     * The field group ID that the FES quartet belongs to.
     */
    dnx_field_group_t fg_id;
    /*
     * The ordinal number (the index) of the FES quartet within the field group.
     */
    uint8 place_in_fg;
    /*
     * The FES quartet's priority for the context ID.
     */
    dnx_field_action_priority_t priotrity;
} dnx_algo_field_action_fes_quartet_sw_state_get_info_t;

/**
 * Structure containing the alignment requirements for allocating a bit range on key.
 */
typedef struct
{
    /**
     * Boolean flag. If 'TRUE' then bit range was allocated while aligning the bit, specified
     * in 'bit_range_aligned_offset' below to bit_range_align_lsb_step.
     * See parameter 'do_align' in 'dnx_algo_field_key_id_bit_range_allocate()'.
     * This is used for FEM handling.
     */
    int align_lsb;
    /**
     * The granularity/resolution in bits to align the lsb to, when using bit_range_align_lsb.
     */
    int align_lsb_step;
    /**
     * Only meaningful if 'bit_range_align_lsb' is 'TRUE'. This is the bit, within allocated bit-range,
     * which has been aligned. Bit is identified by its index, starting from '0', which is the first (LS)
     * bit of the bit range. So, for example, a value of '2' would mean that the third bit has been aligned
     * (i.e., placed on, say, bit(32) of the key.
     * This is only relevant for FEM handling.
     *   Number of bit, within the allocated 'key_length' bits, which
     *   should be aligned to 16 on the physical key. Could be positive or negative
     *   (between -16 to 'maximal key size' - 1). Numbering of bits starts from '0' at
     *   the LS bit.
     *   For example, if 'key_length' is 22 and we need BIT(5) to be aligned then
     *   the result may be:
     *                         MS                                              LS
     *                                                       BIT(5)
                                                                V
     *      Allocated 22 bits:                  ++++++++++++++++++++++
     *      Key              :   ------------------------------------------------
     *      Aligned bits     :                  |               |               |
     *                                                          ^
     *                                                       BIT(16)
     */
    int lsb_aligned_bit;
    /**
     * Boolean flag. If 'TRUE' then bit range was allocated while aligning the bit after the MSB
     * to bit_range_align_msb_step.
     * See parameter 'do_align' in 'dnx_algo_field_key_id_bit_range_allocate()'.
     * This is used for adding EFES with condition bits.
     */
    int align_msb;
    /**
     * The granularity/resolution in bits to align the msb to, when using bit_range_align_msb.
     */
    int align_msb_step;
} dnx_algo_field_key_bit_range_align_t;

/*
 * }
 */

/*
 * { Prototypes
*/

/**
* \brief
*   Print an entry of the 'FEM action' template. See
*       \ref dnx_algo_template_print_data_cb for more details.
*  
* \param [in] unit -
*  Identifier of the device to access.
* \param [in] data -
*   Pointer of the struct to be printed.
*   \b As \b input - \n
*    The pointer is expected to hold a struct of type dnx_field_fem_action_entry_t. \n
*     It's the user's responsibility to verify it beforehand.
* \return
*   None
* \remark
*    None
* \see
*   dnx_algo_template_print_data_cb
 */
void dnx_algo_fem_action_template_cb(
    int unit,
    const void *data);

/**
 * \brief
 *   Get the null-terminated string representing the template assigned
 *   to specified 'fem_id'.
 * \param [in] unit -
 *   Device ID
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * DBAL table, say, FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_2_15_MAP
 */
const char *dnx_algo_fem_action_template_id_get(
    int unit,
    dnx_field_fem_id_t fem_id);

/**
 * \brief
 *  Init all need algorithm for Field module functionality
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_field_init(
    int unit);

/**
 * \brief
 *  Deinit all need algorithm for Field module functionality
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_field_deinit(
    int unit);

/*
 * Procedures related to allocation of keys.
 * {
 */
/**
 * \brief
 *  Allocate Key id after the the logical construction of key was completed
 * \param [in] unit             -  Device ID
 * \param [in] flags          -  please look at dnx_algo_field_key_flags_e
 * \param [in] field_stage      -  Field Stage to allocate key for (PMFA,PMFB...)
 * \param [in] fg_type          -  Database type that being created (TCAM/DE/DT)
 * \param [in] context_id       -  Context ID to allocate the key for
 * \param [in] key_length       -
 *   Enum of type dnx_field_key_length_type_e. Key size - 80b/160b/320b
 * \param [in,out] key_id_p        -
 *   dnx_field_key_id_t *. Structure to hold the allocated key ids, A -> J,
 *   to contain single keys as per 'key_length'. (Currently, maximal size is 320
 *   bits corresponding to DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE elements on key_id_p->id[])
 *   also contains key_part -represents the allocated key part (MSB or LSB),
 *   Only meaningful if 'key_length' is 80b.
 *   Caller is assumed to have loaded the value of DBAL_NOF_ENUM_FIELD_KEY_VALUES on
 *   all key_id_p->id[] to indicate no free key was found. This is changed by this
 *   procedure if free key IS found.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * The allocation is done per Field stage, per PMF program id and per
 *     field group type
 * \see
 *   * dnx_algo_field_key_alloc_generic
 *   * dnx_algo_field_key_id_free
 */
shr_error_e dnx_algo_field_key_id_allocate(
    int unit,
    dnx_algo_field_key_flags_e flags,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    dnx_field_key_length_type_e key_length,
    dnx_field_key_id_t *key_id_p);

/**
 * \brief
 *  Given 'field stage', 'field group type' and 'context id' (= 'triplet'), allocate
 *  a range of bits within the keys assigned to this 'triplet'.
 *  This is only done for 'triplet's that were assigned as supporting the 'bit-range'
 *  feature.
 *  Allocation is only done within within one single key and NOT across keys.
 *  Allocation results in a bit-range of size 'key_length' within 'key id'
 *  at offset 'bit_range_offset_within_key'. The latter two are the output of
 *  this procedure.
 * \param [in] unit             -
 *   HW identifier of device
 * \param [in] field_stage      -
 *   Field Stage to allocate key for (PMFA,PMFB...)
 * \param [in] fg_type          -
 *   Type of requiring 'field group'. (Currently, only DE (direct extraction)
 *   is supported. This is enforced at init. See dnx_algo_field_key_alloc_sw_state_init())
 * \param [in] context_id       -
 *   Context ID to allocate the bit-range (on a key) for
 * \param [in] key_length       -
 *   uint8. Size of required bit-range. May not be larger than the size of
 *   a single key (DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)
 * \param [in] align_info_p      -
 *   Information for forced alignment of bit range.
 * \param [in] force_bit_selection      -
 *   If TRUE, allocate the bits requested in key_id_p
 * \param [in,out] key_id_p        -
 *   dnx_field_key_id_t *. Structure to hold the allocated key ids, A -> J,
 *   to contain containing keys as per 'key_length'. Only the first element
 *   is updated by this procedure since, currently, maximal size is 160
 *   bits corresponding to DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE element
 *   on key_id_p->id[0]. Caller is assumed to have loaded the value of
 *   DBAL_NOF_ENUM_FIELD_KEY_VALUES on all key_id_p->id[] to indicate
 *   no free bit-range was found on any key. This is changed by this procedure if
 *   free bit-range is found.
 *   Also contains key_part which represents the allocated key part (MSB or LSB).
 *   The latter is only meaningful if 'key_length' is less than 80 bits and
 *   this procedure managed to locate the bits within half-key (without extending
 *   from one half key to another). If returned value is DNX_FIELD_KEY_PART_TYPE_INVALID
 *   then a full key was required.
 *   If force_bit_selection is TRUE, this is the input of the key.
 * \param [in, out] bit_range_offset_within_key_p -
 *   uint32 *. This procedure loads pointed memory by the offset of the required
 *   bit-range within selected key.
 *   If force_bit_selection is TRUE, this is the input of the offset on key.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * The allocation is done per Field stage, per PMF program id (context id) and per
 *     field group type
 * \see
 *   * dnx_algo_field_key_alloc_generic
 *   * dnx_algo_field_key_id_free
 */
shr_error_e dnx_algo_field_key_id_bit_range_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    uint8 key_length,
    dnx_algo_field_key_bit_range_align_t * align_info_p,
    int force_bit_selection,
    dnx_field_key_id_t *key_id_p,
    uint32 *bit_range_offset_within_key_p);

/**
 * \brief
 *  Free (deallocate) an already allocated Key id.
 * \param [in]  unit              -
 *   Device ID
 * \param [in] fg_type -
 *   dnx_field_group_type_e. Type of field group requiring the key.
 * \param [in]  field_stage -
 *   dnx_field_stage_e. Type of stage for which the key was allocated.
 * \param [in]  context_id -
 *   Identifier of Context ID for which the key was allocated.
 * \param [in]  key_id_p -
 *   structure holds an array of key Identifier.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * The free is done per Field stage, per PMF program id.
 * \see
 *   * dnx_algo_field_key_free_generic_detailed
 */
shr_error_e dnx_algo_field_key_id_free(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p);

/**
 * \brief
 *  Free (deallocate) an already allocated bit-range on specified Key id.
 * \param [in]  unit              -
 *   Device ID
 * \param [in] field_stage      -
 *   Field Stage to free bit-range (on key) from (IPMF1,IPMF2...)
 * \param [in] fg_type          -
 *   Type of 'field group' which wishes to free this bit-range from.
 *   See dnx_algo_field_key_alloc_sw_state_init()).
 * \param [in] context_id       -
 *   Context ID to free the bit-range (on a key) from.
 * \param [in] key_id        -
 *   dnx_field_key_id_t. Structure hold the key to free bit-range from, A -> J.
 *   Only the first element is used by this procedure since, currently, maximal
 *   size is 160 bits corresponding to DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE
 *   element on key_id.id[0].
 *   Also contains 'key_part' element which represents the allocated key part (MSB or LSB).
 *   This is ignored by this procedure.
 * \param [in] bit_range_offset_within_key -
 *   uint32. The offset of the first bit (LS bit) of the bit-range (within specified key)
 *   which is required to be released. Must match exactly the value supplied by
 *   dnx_algo_field_key_id_bit_range_allocate()
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * The free is done per Field stage, per PMF program id.
 * \see
 *   * dnx_algo_field_key_id_bit_range_allocate()
 */
shr_error_e dnx_algo_field_key_id_bit_range_free(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    dnx_field_key_id_t key_id,
    uint32 bit_range_offset_within_key);

/**
 * \brief
 *   Initialize SW STATE for the 'key allocation' sub-module of the 'KEY' module.
 *   See remarks.
 * \param [in] unit     - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * See file field_key_alloc.xml where all definitions, related to \n
 *     SW STATE of key allocation, are stored. Root is 'dnx_field_keys_per_stage_allocation_sw_data'
 *   * Set all configuration into into 'keys_availability_stage_info' and 'keys_availability_group_info'
 *   * Initialize all 'key_occupation' structures to indicate 'free entry'
 * \see
 *   * dnx_field_keys_per_stage_allocation_t
 *   * dnx_field_keys_per_stage_allocation_sw
 *   * dnx_field_keys_per_stage_allocation_sw_data
 *
 *   * dnx_algo_field_key_id_allocate
 *   * dnx_algo_field_key_id_free
 */
shr_error_e dnx_algo_field_key_alloc_sw_state_init(
    int unit);
/*
 * }
 */

/**
* \brief
*  Allocate FFC id for IPMF 1
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [in] context_id   - Context ID
* \param [in] ranges       - Bitmap of the ranges available for allocation
* \param [out] alloc_id_p   - Allocated id of FFC
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ipmf_1_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    uint8 ranges,
    int *alloc_id_p);

/**
* \brief
*  De-Allocate FFC id for IPMF 1
* \param [in] unit         - Device Id
* \param [in] context_id   - Context ID
* \param [in] ffc_id   - FFC ID to Deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ipmf_1_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id);
/**
* \brief
*  De-Allocate FFC id for IPMF 2
* \param [in] unit         - Device Id
* \param [in] context_id   - Context ID
* \param [in] ffc_id   - FFC ID to Deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ipmf_2_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id);
/**
* \brief
*  De-Allocate FFC id for IPMF 3
* \param [in] unit         - Device Id
* \param [in] context_id   - Context ID
* \param [in] ffc_id   - FFC ID to Deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ipmf_3_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id);

/**
* \brief
*  De-Allocate FFC id for EPMF
* \param [in] unit         - Device Id
* \param [in] context_id   - Context ID
* \param [in] ffc_id   - FFC ID to Deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_epmf_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id);

/**
* \brief
*  De-Allocate FFC id for IFWD2
* \param [in] unit         - Device Id
* \param [in] context_id   - Context ID
* \param [in] ffc_id   - FFC ID to Deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ifwd2_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id);

/**
* \brief
*  Allocate FFC id for IPMF 2
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [in] context_id   - Context ID
* \param [out] alloc_id_p   - Allocated id of FFC
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ipmf_2_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p);

/**
* \brief
*  Allocate FFC id for IPMF 3
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [in] context_id   - Context ID
* \param [out] alloc_id_p   - Allocated id of FFC
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ipmf_3_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p);

/**
* \brief
*  Allocate FFC id for EPMF
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [in] context_id   - Context ID
* \param [out] alloc_id_p   - Allocated id of FFC
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_epmf_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p);

/**
* \brief
*  Allocate FFC id for IFWD2
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [in] context_id   - Context ID
* \param [out] alloc_id_p   - Allocated id of FFC
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ifwd2_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p);

/**
* \brief
*  This function will check if given ffc_id for a specific context in stage iFWD2 is allocated.
* \param [in] unit         - Device Id
* \param [in] context_id   - Context ID
* \param [in] ffc_id       - FFC ID
* \param [out] is_allocated_p  - TRUE if entry is allocated, FALSE otherwise.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ifwd2_ffc_is_allocated(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id,
    uint8 *is_allocated_p);

/**
* \brief
*  Get the number of free FFC id for specific context in IFWD2
* \param [in] unit         - Device Id
* \param [in] context_id   - Context ID
* \param [out] nof_elements_p   - Number of free FFC
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ifwd2_ffc_num_of_elements_get(
    int unit,
    dnx_field_context_t context_id,
    int *nof_elements_p);

/**
* \brief
*  Allocate placeholder in the initial key in iPMF1.
* \param [in] unit           - Device Id
* \param [in] context_id     - Context ID to allocate the given range
* \param [in] size           - Number of bits that we try to allocate
* \param [in] force_position - If true the place to allocate is given.
* \param [in,out] key_id_p   - If force_position is true input argument for the key to allocate in.
*                              If force_position is false output argument of what key was allocated in.
* \param [in,out] bit_lsb_p  - If force_position is true input argument for the lsb on key to allocate.
*                              If force_position is false output argument of the lsb on key that was allocated.
* \return
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref shr_error_e
* \remark
*   None* \remark
* * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_allocate(
    int unit,
    dnx_field_context_t context_id,
    uint32 size,
    int force_position,
    dbal_enum_value_field_field_key_e * key_id_p,
    int *bit_lsb_p);

/**
* \brief
*  De-Allocate placeholder in the initial key for a given qualifier in IPMF1
* \param [in] unit         - Device Id.
* \param [in] context_id   - Context ID to allocate the given range.
* \param [in] size         - qualifier size that we try to allocate.
* \param [in] key_id       - The key to deallocate in.
* \param [in] initial_offset - Starting index of the allocated range.
* \return
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref shr_error_e
* \remark
*   None* \remark
* * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_deallocate(
    int unit,
    dnx_field_context_t context_id,
    uint32 size,
    dbal_enum_value_field_field_key_e key_id,
    int initial_offset);

/**
* \brief
*  Allocate CONTEXT_ID for PMFA
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [out] alloc_id_p   - Allocated id of CONTEXT_ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_presel_pmf_a_context_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t * alloc_id_p);

/**
* \brief
*  Allocate CONTEXT_ID for PMFB
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [out] alloc_id_p   - Allocated id of CONTEXT_ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_presel_pmf_b_context_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t * alloc_id_p);

/**
* \brief
*  Allocate CONTEXT_ID for E_PMF
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [out] alloc_id_p   - Allocated id of CONTEXT_ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_presel_e_pmf_context_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t * alloc_id_p);

/**
* \brief
*  Allocate User Qual_ID
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [out] user_qual_id_p - Pointer to user qual id to be allocated by the function
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_user_qual_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_qual_id_t * user_qual_id_p);

/**
* \brief
*  Allocate User Action_ID
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [out] user_action_id_p  - Pointer to action id to be allocated by the function
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_user_action_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_action_id_t * user_action_id_p);

/**
* \brief
*  Allocate field group id for all PMF's
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (DNX_ALGO_TEMPLATE_ALLOCATE..)
* \param [out] fg_id_p   - Allocated id for Field Group (DNX_DATA_MAX_FIELD_GROUP_NOF_FGS)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_group_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_group_t * fg_id_p);

/**
* \brief
*  Allocate APP_DB_ID for small EXEM.
*  APP_DB_ID is the ID of the application database, a logical division of the physical databases in the MDB.
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (currently only WITH_ID is supported)
* \param [out] app_db_id_p  - Allocated APP_DB_ID. Can also be an input parameter if WITH_ID flag is set.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_lexem_app_db_id_alloc
*/
shr_error_e dnx_algo_field_sexem_app_db_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_app_db_id_t * app_db_id_p);

/**
* \brief
*  Allocate APP_DB_ID for large EXEM.
*  APP_DB_ID is the ID of the application database, a logical division of the physical databases in the MDB.
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (currently only WITH_ID is supported)
* \param [out] app_db_id_p  - Allocated APP_DB_ID. Can also be an input parameter if WITH_ID flag is set.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_sexem_app_db_id_alloc
*/
shr_error_e dnx_algo_field_lexem_app_db_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_app_db_id_t * app_db_id_p);

/**
* \brief
*  Allocate ACE_ID for ACE formats (also serves as result type of the PPMC table and context for ACE ACR).
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (currently only WITH_ID is supported)
* \param [out] ace_id_p    - Allocated ACE ID. Can also be an input parameter if WITH_ID flag is set.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_ace_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_ace_id_t * ace_id_p);

/**
* \brief
*  Allocate ACE key for the ace table (PPMC table for MC replication and ACE pointers).
* \param [in] unit         - Device Id
* \param [in] alloc_flags  - allocation flags (currently only WITH_ID is supported)
* \param [out] ace_key_p    - Allocated ACE key. Can also be an input parameter if WITH_ID flag is set.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_ace_key_allocate(
    int unit,
    int alloc_flags,
    dnx_field_ace_key_t * ace_key_p);
/*
 * Procedures related to 'FEM machine'
 * {
 */
/**
 * \brief
 *  Allocate FEM_PGM_ID (FEM program) for PMFA
 * \param [in] unit         - Device Id
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' on which this fem program
 *    is to be allocated. (Allocation, then, is per 'FEM id')
 *    See DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] alloc_flags  -
 *    Allocation flags (Currently, only DNX_ALGO_RES_ALLOCATE_WITH_ID is meaningful)
 * \param [out] fem_program_p  -
 *    Allocated id of FEM_PGM_ID (FEM program)
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Currently, this is done when FEM is added to a field group
 * \see
 *   * dnx_field_fem_action_add()
 */
shr_error_e dnx_algo_field_action_pmf_a_fem_pgm_id_allocate(
    int unit,
    dnx_field_fem_id_t fem_id,
    int alloc_flags,
    dnx_field_fem_program_t * fem_program_p);

/**
 * \brief
 *  Deallocate FEM_PGM_ID (FEM program) for PMFA
 * \param [in] unit   - Device id
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' from which this fem program
 *    is to be deallocated.
 *    See DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' this HW setup is intended.
 *    See, for example, DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Currently, this is done when FEM is removed from a field group
 * \see
 *   * dnx_field_fem_action_remove()
 */
shr_error_e dnx_algo_field_action_pmf_a_fem_pgm_id_deallocate(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program);
/**
 * \brief
 *   Given 'fem_id', find a free 'fem_program'. Also, use scanned info to get a bitmap
 *   of all 'action's used by this FEM. Use SWSTATE of specified 'fem_id'
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' this operation is intended.
 * \param [out] available_fem_program_p -
 *    This procedure loads pointed memory by the identifier of a free 'fem_program',
 *    provided one is found. Remember that if this procedure returns without an
 *    error then a free 'fem_program' has been found.
 *    See dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs
 * \param [out] sum_allocated_actions_p -
 *    This procedure loads pointed memory by a bit map of all the 'action's
 *    which are currently being used by this 'fem_id'. This is a bitmap such that
 *    BIT(0) stands for the first 'action', BIT(1) for the second, etc.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * If no free 'fem_program' is found, an error, _SHR_E_RESOURCE, is returned.
 * \see
 *   * dnx_field_fem_condition_entry_t
 *   * dnx_field_fem_action_entry_t
 */
shr_error_e dnx_algo_field_action_find_available_fem_program(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t * available_fem_program_p,
    uint8 *sum_allocated_actions_p);
/**
 * \brief
 *   Inspect input 'condition's array (fem_condition_entry[]):
 *   Check whether any of the specified actions (See fem_action_entry[]) is
 *   the same as 'action's currently in HW.
 *   If so, update conditions' array (fem_condition_entry_destination) accordingly
 *   and remove from 'ignore_actions'
 *   See remarks and references below.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_condition_entry -
 *   Array of '16' elements (size taken from DATA). Each element contains all info corresponding
 *   to one 'condition'. Among these parameters is an indication on which 'action' (one of the
 *   4 available) is to be activated when this condition is hit. The available 4 'action's are
 *   detailed on 'fem_action_entry' below. See DBAL table: FIELD_PMF_A_FEM_MAP_INDEX
 *   Note that only 'action' indices, that are referred to here, are referenced on 'fem_action_entry'.
 *   For example, if, on all 16 conditions, only indices '0' and '1' are specified then only
 *   fem_action_entry[0] and fem_action_entry[1] will be looked up. The othres will be
 *   ignored.
 * \param [in] fem_encoded_actions -
 *    Array of '4' elements (size taken from DATA). Each element contains encoded value of
 *    action corresponding to one 'action type' on 'fem_action_entry[]' below.
 *    These entries are being 'reshuffled' into fem_encoded_actions_destination[] so
 *    that the indices match the reordering of action in fem_action_entry_destination[].
 * \param [in] fem_action_entry -
 *    Array of '4' elements (size taken from DATA). Each element contains all info corresponding
 *    to one 'action' (= all 4/24 bit descriptors).
 *    See, for example, DBAL tables: FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_*_MAP
 *    Note that, for 'FEM id's 0/1, only 4 bits may be specified. In that case, the per-bit element
 *    'fem_bit_format' on 'dnx_field_fem_action_entry_t' should be marked DNX_FIELD_FEM_BIT_FORMAT_INVALID
 *    for bits 4 to 23. Otherwise, an error will be ejected.
 *    Note that the 'fem_action' element is, at this stage, a valid input parameter. It is not
 *    encoded! This is 'action_type' as written to HW. The input encoded actions are on
 *    'fem_encoded_actions' above.
 *    This array is 'reshuffled' by this procedure so as to match the actions pointed by
 *    fem_condition_entry_destination[] so that it is ready to be written into HW.
 *    Note the 'fem_adder'  element -
 *    Each element contains a 24-bits value to add to action value after all bit-extractions
 *    from 'key select'.
 *    See DBAL table: FIELD_PMF_A_FEM_ADDER_FEMS_2_15_MAP
 * \param [in] sum_allocated_actions -
 *    Bitmap indicating which 'action's are already occupied in HW. (e.g., If BIT(0) is set then
 *    first 'action' is loaded into HW and is being used by some 'condition' on some 'fem program'
 * \param [out] fem_condition_entry_destination -
 *   See 'fem_condition_entry' above. This array of '16' elements is modified to point to
 *   'action's which are actually in HW and is loaded into fem_condition_entry_destination[].
 *   So, bottom line, 'fem_condition_entry_destination' may actually be loaded into HW.
 * \param [out] ignore_actions_p -
 *   Bitmap indicating which 'action's should be ignored when loading values into HW. This
 *   bitmap marks as 'do not ignore' even 'action's which are already in HW (See 'already_in_hw_p').
 *   See also 'sum_allocated_actions' above.
 * \param [out] already_in_hw_p -
 *   Bitmap indicating which 'action's are already loaded into HW. This is used by the caller
 *   to not reload (i.e., 'write') 'action's which are already in HW.
 * \param [out] fem_encoded_actions_destination -
 *    See fem_encoded_actions[] above.
 * \param [out] fem_action_entry_destination -
 *    See fem_action_entry[] above.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * 'FEM program' is selected automatically by this procedure.
 * \see
 *   * dnx_field_fem_action_add()
 *   * dnx_field_fem_condition_entry_t
 *   * dnx_field_fem_action_entry_t
 */
shr_error_e dnx_algo_field_action_update_conditions(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    uint8 sum_allocated_actions,
    dnx_field_fem_condition_entry_t
    fem_condition_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    uint8 *ignore_actions_p,
    uint8 *already_in_hw_p,
    dnx_field_action_t fem_encoded_actions_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX]);
/*
 * }
 */
/**
* \brief
*  Deallocate CONTEXT_ID for PMFA
* \param [in] unit   - Device id
* \param [in] context_id  - CONTEXT_ID to deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_presel_pmf_a_pmf_pgm_id_deallocate(
    int unit,
    dnx_field_context_t context_id);

/**
* \brief
*  Deallocate PMF_PGM_ID for PMFB
* \param [in] unit   - Device id
* \param [in] context_id  - CONTEXT_ID to deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_presel_pmf_b_pmf_pgm_id_deallocate(
    int unit,
    dnx_field_context_t context_id);    
/**
* \brief
*  Deallocate PMF_PGM_ID for E_PMF
* \param [in] unit   - Device id
* \param [in] context_id  - CONTEXT_ID to deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_presel_e_pmf_pmf_pgm_id_deallocate(
    int unit,
    dnx_field_context_t context_id);
/**
* \brief
*  Deallocate user qual id
* \param [in] unit   - Device id
* \param [in] user_qual_id  - user qual id for deallocation
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_user_qual_id_deallocate(
    int unit,
    dnx_field_qual_id_t user_qual_id);
/**
* \brief
*  Deallocate user action id
* \param [in] unit   - Device id
* \param [in] user_action_id  - user action id for deallocation
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_user_action_id_deallocate(
    int unit,
    dnx_field_action_id_t user_action_id);

/**
* \brief
*  Deallocate APP_DB_ID for small EXEM.
* \param [in] unit         - Device Id
* \param [in] app_db_id    - APP_DB_ID to deallocate.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_lexem_app_db_id_dealloc
*/
shr_error_e dnx_algo_field_sexem_app_db_id_deallocate(
    int unit,
    dnx_field_app_db_id_t app_db_id);

/**
* \brief
*  Deallocate APP_DB_ID for large EXEM.
* \param [in] unit         - Device Id
* \param [in] app_db_id    - APP_DB_ID to deallocate.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_sexem_app_db_id_dealloc
*/
shr_error_e dnx_algo_field_lexem_app_db_id_deallocate(
    int unit,
    dnx_field_app_db_id_t app_db_id);

/**
* \brief
*  Deallocate ACE ID.
* \param [in] unit         - Device Id
* \param [in] ace_id       - ACE ID to deallocate.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_ace_id_deallocate(
    int unit,
    dnx_field_ace_id_t ace_id);

/**
* \brief
*  Deallocate ACE KEY.
* \param [in] unit         - Device Id
* \param [in] ace_key       - ACE key to deallocate.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_ace_key_deallocate(
    int unit,
    dnx_field_ace_key_t ace_key);

/**
* \brief
*  Deallocate Database for field module
* \param [in] unit   - Device id
* \param [in] fg_id  - Field group ID to deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_group_id_deallocate(
    int unit,
    dnx_field_group_t fg_id);

/**
* \brief
*  This function will allocate entry_access_id by using simple allocation manager
*  entry_access_id is used by entry_add function to identify specific entry and to handle inside TCAM..
*  the id being allocated per field stage
* \param [in] unit         - Device ID
* \param [in] alloc_flags  - Alloc flag with/without ID
* \param [out] alloc_id_p   - Allocated id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_entry_tcam_access_id_allocate(
    int unit,
    int alloc_flags,
    int *alloc_id_p);

/**
* \brief
*  This function will check if given entry_access_id is allocated.
*  entry_access_id is used by entry_add function to identify specific entry and to handle inside TCAM.
* \param [in] unit            - Device ID
* \param [in] entry_access_id - Entry access ID to check whether allocated
* \param [out] is_allocated_p - TRUE if entry is allocated, FALSE otherwise.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_entry_tcam_access_id_is_allocated(
    int unit,
    int entry_access_id,
    uint8 *is_allocated_p);

/**
* \brief
*  Deallocate entry_access id
* \param [in] unit             - Device id
* \param [in] entry_access_id  - id to deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_algo_field_entry_tcam_access_id_allocate() description
*/
shr_error_e dnx_algo_field_entry_tcam_access_id_deallocate(
    int unit,
    int entry_access_id);

/**
* \brief
*  Allocate a KBP profile ID to be used by iPMF1 context selection.
* \param [in] unit         - Device ID
* \param [in] alloc_flags  - Alloc flag with/without ID
* \param [out] cs_prodfile_id_p - Allocated id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_kbp_opcode_cs_profile_id_allocate(
    int unit,
    int alloc_flags,
    int *cs_prodfile_id_p);

/**
* \brief
*  Deallocate KBP profile ID.
* \param [in] unit             - Device id
* \param [in] cs_prodfile_id   - id to deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_algo_field_kbp_opcode_cs_profile_id_allocate()
*/
shr_error_e dnx_algo_field_kbp_opcode_cs_profile_id_deallocate(
    int unit,
    int cs_prodfile_id);

/**
* \brief
*  This function will check if given KBP profile ID is allocated.
* \param [in] unit            - Device ID
* \param [in] cs_prodfile_id - ID to check
* \param [out] is_allocated_p - TRUE if entry is allocated, FALSE otherwise.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_kbp_opcode_cs_profile_id_is_allocated(
    int unit,
    int cs_prodfile_id,
    uint8 *is_allocated_p);

/**
* \brief
*  This function will check if given flush profile ID is allocated.
* \param [in] unit            - Device ID
* \param [in] flush_profile_id - ID to check
* \param [out] is_allocated_p - TRUE if entry is allocated, FALSE otherwise.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_flush_profile_is_allocated(
    int unit,
    int flush_profile_id,
    uint8 *is_allocated_p);

/**
* \brief
*  Initialize resource allocation for FFC and Keys management
* \param [in] unit             - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_algo_field_key_res_mngr_init() description
*/
shr_error_e dnx_algo_field_key_res_mngr_init(
    int unit);

/**
* \brief
*  De-Allocate resources for FFC and Keys management
* \param [in] unit             - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_algo_field_key_res_mngr_deinit() description
*/ shr_error_e
    dnx_algo_field_key_res_mngr_deinit(
    int unit);

/**
* \brief
*  Initialize resource allocation for Context management
* \param [in] unit             - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_context_res_mngr_init(
    int unit);

/**
* \brief
*  De-Allocate resources for Context management
* \param [in] unit             - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/ shr_error_e
    dnx_algo_field_context_res_mngr_deinit(
    int unit);

/**
* \brief
*  Initialize the SW state for FES management.
* \param [in] unit             - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_algo_field_init()
*/
shr_error_e dnx_algo_field_action_init(
    int unit);

/**
* \brief
*  Deinit the SW state for FES management.
* \param [in] unit             - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_algo_field_deinit()
*/
shr_error_e dnx_algo_field_action_sw_state_deinit(
    int unit);

/**
 * \brief
 *   This procedure allocates FES quartets and FES action masks for a context ID and field group,
 *   and returns if the allocation was successful, and if so which quartets need to be written to the FES HW.
 * \param [in] unit -
 *   Device ID.
 * \param [in] field_stage -
 *   For which of the PMF blocks the configuration is done (iPMF-1, iPMF2, iPMF-3, ePMF-1).
 * \param [in] fg_id -
 *   HW identifier of field group.
 * \param [in] context_id -
 *   The context ID.
 * \param [in] current_state_in_p -
 *   The current state of the allocation given to the allocation algorithm from the outside
 *   (as opposed to the current state in algo field SW state).
 *   At the moment contains the allocated FES quartets to the context ID we allocate for.
 * \param [in] fes_fg_in_p -
 *   The information about the new field group and context ID and what allocations they require.
 * \param [in] allow_fes_pgm_sharing -
 *   Whether to enable fes program sharing for this allocation. This is disabled for direct extraction.
 * \param [in] post_attach -
 *   If we add more EFES after context attach.
 * \param [out] alloc_result_p -
 *   Pointer to the memory to be loaded with the result of the allocation.
 *   The data is only valid if alloc_successful_p points to TRUE.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_actions_convert_fes_instructions_from_alloc_to_write()
 *   * Fes allocation description on ALGORITHM DESCRIPTIONS section in algo_field_action.c.
 */
shr_error_e dnx_algo_field_action_fes_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p,
    int allow_fes_pgm_sharing,
    int post_attach,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p);

/**
 * \brief
 *   This procedure deallocates FES quartets and FES action masks for a context ID and field group.
 *   It's main use is to clear the relevant SW state in algo_field_action, but is also marks all
 *   Fes quartets and FES action masks owned by the context ID and no other context ID, and related
 *   to the field group to be deleted.
 * \param [in] unit -
 *   Device ID.
 * \param [in] field_stage -
 *   For which of the PMF blocks the configuration is done (iPMF-1, iPMF2, iPMF-3, ePMF-1).
 * \param [in] fg_id -
 *   HW identifier of field group to remove.
 * \param [in] context_id -
 *   The context ID that we remove the field group from.
 * \param [in] fes_id -
 *   If DNX_FIELD_EFES_ID_INVALID ignoring this parameter. Otherwise, only delete this sepcific FES ID.
 * \param [in] current_state_dealloc_in_p -
 *   The current state of the allocation given to the allocation algorithm from the outside
 *   (as opposed to the current state in algo field SW state).
 *   At the moment contains the allocated FES quartets to the context ID we allocate for,
 *   and the number of context IDs that are allocated each FES quartet in each FES.
 * \param [out] dealloc_result_p -
 *   Pointer to the memory to be loaded with the result of the deallocation.
 *   For each FES ID, indicates whether to delete FES quartets and FES action masks.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_actions_fes_context_group_delete()
 */
shr_error_e dnx_algo_field_action_fes_dealloc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_algo_field_action_fes_dealloc_state_in_t * current_state_dealloc_in_p,
    dnx_algo_field_action_fes_dealloc_out_t * dealloc_result_p);

/**
 * \brief
 *  This procedure allocated FES IDs and mask IDs for the ACE ACR.
 * \param [in] unit -
 *  Device ID.
 * \param [in] ace_id -
 *  Identifier of ACE Format. Also serves as result type in the ACE table and context for the ACE ACR.
 *  Not used in this algorithm.
 * \param [in] current_state_in_p -
 *  The current state of the allocation given to the allocation algorithm from the outside
 *  (as opposed to the current state in algo field SW state, which doesn't exist for the ACE ACR).
 *  At the moment contains the action masks being used by the ACE ACR FESes.
 * \param [in] fes_fg_in_p -
 *   The information about the action that we want to write.
 *   Contains the number of fes instructions to be written and their action masks.
 * \param [in] alloc_result_p -
 *   Pointer to the memory to be loaded with the results of the allocation.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Note that this algorithm assumes that all FES instructions that use masks that aren't all zeros
 *     use the zero mask, and that an all zero mask that isn't the zero mask means it's free for allocation.
 * \see
 *   * dnx_field_actions_fes_ace_set()
 */
shr_error_e dnx_algo_field_action_ace_fes_allocate(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_algo_field_action_ace_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_ace_fes_alloc_in_t * fes_fg_in_p,
    dnx_algo_field_action_ace_fes_alloc_out_t * alloc_result_p);

/**
 * \brief
 *   This procedure provides the SW state saved in algo_field_action for the FES quartets of
 *   a specific context ID and FES ID. It also receives the FES program ID as that information isn't saved to SW state,
 *   and needs to be read from HW.
 * \param [in] unit -
 *   Device ID.
 * \param [in] field_stage -
 *   For which of the PMF blocks the configuration is done (iPMF-1, iPMF2, iPMF-3, ePMF-1).
 * \param [in] context_id -
 *   Context ID.
 * \param [in] fes_id -
 *   FES ID.
 * \param [in] fes_pgm_id -
 *   The FES program ID of the FEQ quartet for the context ID and FES ID.
 *   Note that algo_field_action module cannot verify that the FES program ID given is indeed allocated
 *   to the context ID.
 * \param [out] fes_quartet_sw_state_info_p -
 *   Pointer to an array of structures dnx_algo_field_action_fes_quartet_sw_state_get_info_t with the number of elements
 *   DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT.
 *   to be loaded with the SW state info for the FES quartet.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_actions_context_fes_info_get()
 */
shr_error_e dnx_algo_field_action_fes_quartet_sw_state_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    dnx_algo_field_action_fes_quartet_sw_state_get_info_t * fes_quartet_sw_state_info_p);

/**
* \brief
*  Allocate field Context for Field module per Stage
* \param [in] unit          - Device id
* \param [in] alloc_flags   - Alloc Flags (WITH_ID)
* \param [in] stage         - Field Stage to allocate context to
* \param [in,out] context_id_p  - pointer to dnx_field_context_t
*                   as input -  the context id to allocate when WITH_ID flag is set
*                   as output - allocated Context id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_context_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_stage_e stage,
    dnx_field_context_t * context_id_p);

/**
* \brief
*  Deallocate the Context ID for Field module per Stage
* \param [in] unit        - Device id
* \param [in] stage        - Field Stage to deallocate context in
* \param [in] context_id  - Context id to deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_context_id_deallocate(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id);

/**
* \brief
*  Deallocate the Context ID for Field module per Stage
* \param [in] unit        - Device id
* \param [in] stage        - Field Stage the context is in
* \param [in] context_id  - Context id to check
* \param [out] is_allocated_p  - TRUE if context was allocated, false otherwise
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_context_id_is_allocated(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    uint8 *is_allocated_p);

/**
* \brief
*  Allocate a user defined Apptype.
* \param [in] unit          - Device id
* \param [in] alloc_flags   - Alloc Flags (WITH_ID)
* \param [in,out] apptype_p  - pointer to dnx_field_context_t
*                   as input -  the Apptype id to allocate when WITH_ID flag is set
*                   as output - allocated Apptype id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_context_apptype_allocate(
    int unit,
    int alloc_flags,
    bcm_field_AppType_t * apptype_p);

/**
* \brief
*  Deallocate a user defined Apptype.
* \param [in] unit        - Device id
* \param [in] apptype     - Apptype to deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_context_apptype_deallocate(
    int unit,
    bcm_field_AppType_t apptype);

/**
* \brief
*  Deallocate the Context ID for Field module per Stage
* \param [in] unit        - Device id
* \param [in] apptype     - Apptype to check
* \param [out] is_allocated_p  - TRUE if the apptype was allocated, false otherwise
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_context_apptype_is_allocated(
    int unit,
    bcm_field_AppType_t apptype,
    uint8 *is_allocated_p);

/**
* \brief
*   Check whether specific field group was already allocated
*   in DNX_ALGO_FIELD_GROUP_ID
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit ID
*   \param [in] fg_id           -  Field Group id to check
*   \param [in] is_allocated_p  -  Pointer of type uint8.This procedure
*                                  This procedure loads pointed memory by
*                                  a non-zero value if specified fg_id is allocated.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_group_is_allocated(
    int unit,
    dnx_field_group_t fg_id,
    uint8 *is_allocated_p);

/**
* \brief
*   Check whether specific ACE ID was already allocated in DNX_ALGO_FIELD_ACE_ID
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit ID
*   \param [in] ace_id          -  ACE ID to check
*   \param [in] is_allocated_p  -  Pointer of type uint8.This procedure
*                                  This procedure loads pointed memory by
*                                  a non-zero value if specified ace_id is allocated.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_ace_id_is_allocated(
    int unit,
    dnx_field_ace_id_t ace_id,
    uint8 *is_allocated_p);

/**
* \brief
*   Check whether specific ACE key was already allocated in DNX_ALGO_FIELD_ACE_KEY
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit ID
*   \param [in] ace_key         -  ACE key to check
*   \param [in] is_allocated_p  -  Pointer of type uint8.This procedure
*                                  This procedure loads pointed memory by
*                                  a non-zero value if specified ace_id is allocated.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_ace_key_is_allocated(
    int unit,
    dnx_field_ace_key_t ace_key,
    uint8 *is_allocated_p);

/**
* \brief
*  Increases the number of links for the given context ID, the number of links for a given context in
*  iPMF1 stage indicates how many iPMF2 contexts are "attached" or in other words being "cascaded"
*  from the iPMF1 context.
*  This function allocates a new link profile number for the iPMF1 contextfor the first link that
*  gets created.
*
* \param[in] unit       - Device ID
* \param[in] context_id - iPMF1 Context ID to increase number of links for (possibly allocating a
*                         new link profile number for)
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_context_links_inc(
    int unit,
    dnx_field_context_t context_id);

/**
* \brief
*  Decreases the number of links for the given context ID, the number of links for a given context in
*  iPMF1 stage indicates how many iPMF2 contexts are "attached" or in other words being "cascaded"
*  from the iPMF1 context.
*  This function deallocates the link profile number for the iPMF1 context if no more links are left.
*
* \param[in] unit       - Device ID
* \param[in] context_id - iPMF1 Context ID to decrease number of links for (possibly deallocating its
*                         link profile number)
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_context_links_dec(
    int unit,
    dnx_field_context_t context_id);

/**
* \brief
*  Returns the link profile number for the given context.
*  Links profile numbers are given to contexts in iPMF1 stage and are required to perform cascading
*  between foreign contexts.
*
* \param[in] unit           - Device ID
* \param[in] context_id     - Context ID to get link profile number for
* \param[out] profile_num_p - Link profile number for the given context
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_field_link_profile_get(
    int unit,
    dnx_field_context_t context_id,
    int *profile_num_p);

/*
 * } Prototypes
*/

/*
 * Utility procedures to be used by diag
 * {
 */
/**
 * \brief
 *  This function will indicate key allocation state: Is there any occupied key
 *  on specified pair - stage and context
 * \param [in] unit               - Device ID
 * \param [in] field_stage        -
 *   dnx_field_stage_e. Stage to use for searching key occupation state
 * \param [in] context_id            -
 *   dnx_field_context_t. Context id (program) to use for searching key occupation state
 * \param [in] group_type            -
 *   dnx_field_group_type_e. Group type to use for searching key occupation state
 *   If set to DNX_FIELD_GROUP_TYPE_INVALID then this input is ignored (i.e., group
 *   type is 'do not care').
 * \param [in] bit_range_only            -
 *   int. If this input is 'zero' then ignore it. If non-zero then search only
 *   'field group types' which support the 'bit-range' feature. If feature is
 *   not soppurted, this procedure will flag 'not occupied'
 * \param [out] none_occupied_p            -
 *   int pointer. This procedure loads pointed memory by a non-zero value
 *   if no key is occupied for this stage/context pair.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Output of this procedure is based on keys occupation as stored on SW state.
 * \see
 *   * None
 */
shr_error_e dnx_algo_field_key_is_any_key_occupied(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e group_type,
    int bit_range_only,
    int *none_occupied_p);
/*
 * }
 */

#endif/*_FIELD_API_INCLUDED__*/
