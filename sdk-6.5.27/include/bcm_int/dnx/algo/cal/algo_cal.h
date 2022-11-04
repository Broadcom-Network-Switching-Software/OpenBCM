/**
 * \file algo_cal.h
 * Reserved.$ 
 */

#ifndef _DNX_ALGO_CAL__H__INCLUDED_
#define _DNX_ALGO_CAL__H__INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/types.h>
#include <shared/utilex/utilex_rand.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>

#define DNX_ALGO_CAL_MAX_LEVEL_DISTANCE_CHECK 3
#define DNX_ALGO_CAL_ILLEGAL_OBJECT_ID  (0xffffffff)

typedef enum
{
    DNX_ALGO_CAL_ALGORITHM_BIG_HOP_SMALL_HOP = 0,
    DNX_ALGO_CAL_ALGORITHM_RATIO_BASED,
    DNX_ALGO_CAL_ALGORITHM_RATIO_BASED_GROUPED,
    DNX_ALGO_CAL_ALGORITHM_RATIO_BASED_GROUPED_RECURSIVE,
    DNX_ALGO_CAL_ALGORITHM_RATIO_BASED_GROUPED_RECURSIVE_WITH_VALIDATE
} dnx_algo_cal_algorithm_t;

typedef struct
{
    int object;
    double optimal_dist;
    int min_allowed_gap;
    int max_allowed_gap;
    int prev_index;
    int next_index;
    int actual_dist_left;
    int actual_dist_right;
    int valid;
} dnx_algo_cal_detailed_entry_t;

typedef struct
{
    int current;

    int valid;
    int min_first_slot;
    int max_first_slot;
} dnx_algo_cal_random_first_constraint_t;

typedef struct
{
    dnx_algo_cal_random_first_constraint_t slot_info[3];
} dnx_algo_cal_random_object_transition_info_t;

typedef struct
{
    int id;
    int orig_id;
    int nof_slots;
    int orig_nof_slots;
} dnx_algo_cal_object_t;

typedef shr_error_e(
    *dnx_algo_cal_build_func_t) (
    int,
    dnx_algo_cal_object_t *,
    int,
    int,
    int,
    void *,
    uint32 *);

typedef int (
    *dnx_algo_cal_compare_func_t) (
    void *,
    void *);
typedef struct
{
    UTILEX_RAND rand_state;
    dnx_algo_cal_compare_func_t compare_cb;
} dnx_algo_cal_random_start_info_t;

/* 
 * \brief -
 *   Given calendar type, rate per each active port, and length of calendar, get
 *   the corresponding calendar with 'calendar_len' slots (but see 'add_dummy_tail').
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use for getting required info..
 * \param [in] cal_info -
 *   Pointer to dnx_ofp_rates_cal_info_t. The following elements are input:
 *     cal_info->cal_type (dnx_ofp_rates_egq_cal_type_e) - May be:
 *       DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 *       All other calendar types
 *   The following are also used,  but only for 'channelized calendar' (CAL_CHAN_ARB):
 *     cal_info->chan_arb_id (used to calculate 'offest', key into table).
 * \param [in] ports_rates -
 *   Pointer to array of uint32. Dimension of array is dnx_data_egr_queuing.params.nof_q_pairs
 *   (currently 512). Each entry indicates the rate assigned to indicated port, in kilo
 *   bits per second. A port, whose rate is '0', is assumed to be inactive.
 * \param [in] calendar_len -
 *   uint32. Number of slots in calendar. Each slot refers to one port.
 * \param [in] add_dummy_tail -
 *   uint8. Flag. If set then number of slots, in 'calendar', loaded by rate, is
 *   not 'calendar_len' but 'calendar_len - 1'.
 * \param [in,out] calendar -
 *   Pointer to dnx_ofp_rates_cal_egq_t. 'calendar' is made out of an array of 'slots'. Each
 *   slot is represented by two parameters: 'base qpair' (= port index) and its corresponding number
 *   of 'credit's that the system is supposed to supply to that port when its turn comes.
 *   Of these two parameters, 'base qpair' is INPUT and 'credit' is OUTPUT of this procedure.
 *   Caller is responsible to set 'calendar' with enough space to accommodate 'calendar_length'
 *   slots.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar
 */
int dnx_algo_ofp_rates_fill_shaper_generic_calendar_credits(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 *ports_rates,
    /*
     * Actual Calendar length
     */
    uint32 calendar_len,
    /*
     * Indicate if last entry is dummy or not
     */
    uint8 add_dummy_tail,
    dnx_ofp_rates_cal_egq_t * calendar);
/**
 * \brief
 * build calendar for provided slotes-per-object and provided calendar length.
 * unused slots will receive DNX_ALGO_CAL_ILLEGAL_OBJECT_ID value
 * 
 * \param [in] unit     - unit id
 * \param [in] cal_alg_mode - calendar algorithm mode
 * \param [in] slots_per_object - number of slots to allocate to each object
 * \param [in] nof_objects - total number of objects - size of slots_per_object
 * \param [in] calendar_len - length of the required calendar
 * \param [in] max_calendar_len - maximal possible calendar length
 * \param [out] calendar - the result calendar
 * 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   unused slots will receive DNX_ALGO_CAL_ILLEGAL_OBJECT_ID value
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_simple_fixed_len_cal_build(
    int unit,
    dnx_algo_cal_algorithm_t cal_alg_mode,
    uint32 *slots_per_object,
    uint32 nof_objects,
    uint32 calendar_len,
    uint32 max_calendar_len,
    uint32 *calendar);

/**
 * \brief - 
 * Build fixed length calendar by keeping the ratio between slots for current object and for other objects
 *
 * \param [in] unit - Unit #.
 * \param [in] slots_per_object - Number of slots for each object.
 * \param [in] nof_objects - Array size for slots_per_object.
 * \param [in] calendar_len - calendar length.
 * \param [in] max_calendar_len - Max calendar length.
 * \param [out] calendar - Calendar value for each slots.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_fixed_len_ratio_based_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar);


/**
 * \brief - 
 * Build fixed length calendar when a first slot is selected randomly
 *
 * \param [in] unit - Unit #.
 * \param [in] slots_per_object - Number of slots for each object.
 * \param [in] nof_objects - Array size for slots_per_object.
 * \param [in] calendar_len - calendar length.
 * \param [in] max_calendar_len - Max calendar length.
 * \param [out] calendar - Calendar value for each slots.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_random_start_group_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar);

shr_error_e algo_cal_transition_fix(
    int unit,
    uint32 calendar_len,
    int nof_objects,
    dnx_algo_cal_random_object_transition_info_t * trans_info,
    int nof_tests,
    dnx_algo_cal_detailed_entry_t * cal_dist_info);
/**
 * \brief
 * build best calendar for provided object rates, total bandwidth and maximal calendar length
 *
 * \param [in] unit     - unit id
 * \param [in] alt_cal_alg - use alternative calendar algorithm
 * \param [in] rates_per_object - rate of each object port
 * \param [in] nof_objects - number of objects - length of rates_per_object
 * \param [in] total_bandwidth - total bandwidth of the whole calendar
 * \param [in] max_calendar_len - maximal possible calendar length
 * \param [out] calendar_slots - the result calendar
 * \param [out] calendar_len - the length of the result calendar
 * 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_simple_from_rates_to_calendar(
    int unit,
    int alt_cal_alg,
    uint64 *rates_per_object,
    uint32 nof_objects,
    uint64 total_bandwidth,
    uint32 max_calendar_len,
    uint32 *calendar_slots,
    uint32 *calendar_len);

shr_error_e dnx_algo_cal_group_and_build(
    int unit,
    dnx_algo_cal_build_func_t build_cb,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    void *additional_info,
    uint32 *calendar);

shr_error_e dnx_algo_cal_random_start_build_cb(
    int unit,
    dnx_algo_cal_object_t * slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    void *additional_info,
    uint32 *calendar);

/**
 * \brief
 * get rate of the provided object (object_id) in the provided calendar
 *
 * \param [in] unit     - unit id
 * \param [in] calendar - the input calendar
 * \param [in] calendar_len - the length of the calendar
 * \param [in] total_cal_rate - total bandwidth of the whole calendar
 * \param [in] object_id - object ID
 * \param [out] object_rate - the rate of the object in the calendar
 * 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_simple_object_rate_get(
    int unit,
    uint32 *calendar,
    uint32 calendar_len,
    uint64 total_cal_rate,
    int object_id,
    uint32 *object_rate);

/**
 * \brief -
 * convert unsigned 64 to unsigned 32 bit number.
 * Return error in case of overflow
 *
 */
int dnx_algo_cal_u64_to_long(
    int unit,
    uint64 u64,
    uint32 *u32);

/**
 * \brief -
 * divide unsigned 64 by unsigned 32 and round up
 */
int dnx_algo_cal_u64_div_long_and_round_up(
    int unit,
    uint64 dividend,
    uint32 divisor,
    uint64 *result);

/**
 * \brief
 * Check the distribution of a given element in the provided calendar
 *
 * \param [in] unit     - unit id
 * \param [in] object - the element its distribution is being tested
 * \param [in] nof_slots - the elements nof_instances in the calendar
 * \param [in] deviation - the deviation of the check
 * \param [in] calendar - the input calendar
 * \param [in] calendar_len - the length of the calendar
 * \param [out] passed - the result of the test
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_distance_cyclic_check(
    int unit,
    int object,
    int nof_slots,
    float deviation,
    uint32 *calendar,
    int calendar_len,
    int *passed);

/**
 * \brief
 * Check the second-order distribution of a given element in the provided calendar
 *
 * \param [in] unit     - unit id
 * \param [in] object - the element its distribution is being tested
 * \param [in] nof_slots - the elements nof_instances in the calendar
 * \param [in] deviation - the lower limit of the test
 * \param [in] calendar - the input calendar
 * \param [in] calendar_len - the length of the calendar
 * \param [out] passed - the result of the test
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_double_distance_cyclic_check(
    int unit,
    int object,
    int nof_slots,
    float deviation,
    uint32 *calendar,
    int calendar_len,
    int *passed);

/**
 * \brief
 * Check the third-order distribution of a given element in the provided calendar
 *
 * \param [in] unit     - unit id
 * \param [in] object - the element its distribution is being tested
 * \param [in] nof_slots - the elements nof_instances in the calendar
 * \param [in] deviation - the lower limit of the test
 * \param [in] calendar - the input calendar
 * \param [in] calendar_len - the length of the calendar
 * \param [out] passed - the result of the test
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_triple_distance_cyclic_check(
    int unit,
    int object,
    int nof_slots,
    float deviation,
    uint32 *calendar,
    int calendar_len,
    int *passed);

/**
 * \brief
 * Check the minimal distance between clients instances is not under given value more then given maximal occurrences
 *
 * \param [in] unit     - unit id
 * \param [in] objects - bitmap of objects to be checked for min distance
 * \param [in] calendar - the input calendar
 * \param [in] calendar_length - the length of the calendar
 * \param [in] limit_value - the minimal valid distance between object occurrences
 * \param [in] max_occurrences - maximal occurrences of meeting the limit value for this test
 * \param [out] passed - the result of the test
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_min_distance_cyclic_check(
    int unit,
    bcm_pbmp_t objects,
    uint32 *calendar,
    int calendar_length,
    int limit_value,
    int max_occurrences,
    int *passed);

/**
 * \brief
 * Check that there aren't any consecutive occurrences of a given client
 *
 * \param [in] unit     - unit id
 * \param [in] client - the input client
 * \param [in] nof_slots - the elements nof_instances in the calendar
 * \param [in] calendar - the input calendar
 * \param [in] calendar_length - the length of the calendar
 * \param [out] passed - the result of the test
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_consecutive_cyclic_check(
    int unit,
    uint32 client,
    int nof_slots,
    uint32 *calendar,
    int calendar_length,
    int *passed);

/**
 * \brief
 * Check the "level"-order distribution of a given element in the transition from old to new calendar
 *
 * \param [in] unit     - unit id
 * \param [in] client - the input client
 * \param [in] level - the level of the check
 * \param [in] nof_slots - the elements nof_instances in the calendar
 * \param [in] deviation - the deviation of the check
 * \param [in] calendar - the input new calendar
 * \param [in] active_calendar_last_occurrences - the input active calendar last occurrences
 * \param [in] calendar_length - the length of the calendar
 * \param [out] passed - the result of the test
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_distance_transition_check(
    int unit,
    int client,
    int nof_slots,
    int level,
    float deviation,
    uint32 *calendar,
    uint32 *active_calendar_last_occurrences,
    int calendar_length,
    int *passed);

/**
 * \brief
 * Check the minimal distance between clients instances is not under given value more then given maximal occurrences
 *
 * \param [in] unit     - unit id
 * \param [in] objects - bitmap of objects to be checked for min distance
 * \param [in] calendar - the input calendar
 * \param [in] active_calendar_last_occurrences - the input active calendar last occurrences
 * \param [in] calendar_length - the length of the calendar
 * \param [in] limit_value - the minimal valid distance between two clients occurrences
 * \param [in] max_occurrences - maximal occurrences of meeting the limit value for this test
 * \param [out] passed - the result of the test
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_min_distance_transition_check(
    int unit,
    bcm_pbmp_t objects,
    uint32 *calendar,
    uint32 **active_calendar_last_occurrences,
    int calendar_length,
    int limit_value,
    int max_occurrences,
    int *passed);

/**
 * \brief
 * Build cal_dist_info meta-data array for the fix function algo_cal_fix()
 *
 * \param [in] unit     - unit id
 * \param [in] nof_objects - number of objects in calendar
 * \param [in] calendar_length - the length of the calendar
 * \param [in] calendar - the input calendar
 * \param [in] level - the level of the check
 * \param [in] deviation - the deviation of the check
 * \param [in] extra - extra gap/value added to the deviation
 * \param [in] check_abs_dist - indicates if checking absolute distance is needed
 * \param [in] is_valid_test - array of valid object to be fixed per-test
 * \param [out] cal_dist_info - meta-data array for the fix function algo_cal_fix()
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e algo_cal_dist_calc(
    int unit,
    uint32 nof_objects,
    uint32 calendar_length,
    uint32 *calendar,
    int level,
    double deviation,
    int extra,
    int check_abs_dist,
    int *is_valid_test,
    dnx_algo_cal_detailed_entry_t * cal_dist_info);

/**
 * \brief
 * Fix calendar according to cal_dist_info meta-data
 *
 * \param [in] unit     - unit id
 * \param [in] calendar_length - the length of the calendar
 * \param [in] nof_tests - number of tests
 * \param [in] nof_tests_to_fix - number of tests to fix
 * \param [in] complete_fix - boolean which state fix off all needed tests
 * \param [out] cal_dist_info - holding the meta-data for the fix and the fixed calendar itself
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e algo_cal_fix(
    int unit,
    uint32 calendar_length,
    int nof_tests,
    int nof_tests_to_fix,
    int complete_fix,
    dnx_algo_cal_detailed_entry_t * cal_dist_info);

/**
 * \brief
 * Build fixed length calendar by keeping the ratio between slots for current object and for other objects, usinf  random starting point
 *
 * \param [in] unit     - unit id
 * \param [in] slots_per_object - number of slots to allocate to each object
 * \param [in] nof_objects - total number of objects - size of slots_per_object
 * \param [in] calendar_length - the length of the calendar
 * \param [in] max_calendar_len - maximal possible calendar length
 * \param [in] info - additional info for calendar algorithm
 * \param [in] trans_info - transition info
 * \param [out] calendar - the calendar that was built by the function
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_random_start_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_length,
    int max_calendar_len,
    dnx_algo_cal_random_start_info_t * info,
    dnx_algo_cal_random_object_transition_info_t * trans_info,
    uint32 *calendar);

#endif
