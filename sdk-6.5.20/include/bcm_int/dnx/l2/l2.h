/*
 * ! \file bcm_int/dnx/l2/l2.h Internal DNX L2 APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef L2_H_INCLUDED
/*
 * {
 */
#define L2_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/algo/l2/algo_l2.h>
#include <bcm_int/dnx/l2/l2_learn.h>

#include <bcm/l2.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/** Strength of a static transplantable entry in the MACT. Entry that do not age out but can be transplanted */
#define STATIC_TRANSPLANTABLE_MACT_ENTRY_STRENGTH 0

/** Strength of a dynamic entry in the MACT */
#define DYNAMIC_MACT_ENTRY_STRENGTH 1

/** Strength of a static entry in the MACT */
#define STATIC_MACT_ENTRY_STRENGTH 2

/** Number of maximum registered callbacks per unit */
#define DNX_L2_CB_MAX 4

#define DEFAULT_NOF_AGE_CYCLES 8

/** Callbacks handling */
typedef struct
{
    /** Callback routine */
    bcm_l2_addr_callback_t callback;
    /** User defined data */
    void *userdata;
} dnx_l2_cb_entry_t;

typedef struct
{
    /**Callback entry */
    dnx_l2_cb_entry_t entry[DNX_L2_CB_MAX];
    /** Number of callback entries */
    int count;
} dnx_l2_cbs_t;

extern dnx_l2_cbs_t dnx_l2_cbs[BCM_MAX_NUM_UNITS];

/*
 * }
 */

/**
 * \brief - Initialize L2 module.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_module_init(
    int unit);

/**
 * \brief - Deinitialize L2 module.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_module_deinit(
    int unit);

/**
 * \brief - Update the number of meta cycles until an entry with the given vlan is aged out. 
 * Possible number of meta cycles are: 0 (no aging), 1, 2, ..., 8, 10, 12, 14, 16, 32, 64, 128
 *
 * \param [in] unit - unit id
 * \param [in] vsi - fid to update its number of aging cycles before age out.
 * \param [in] is_lif - age configuration per LIF.
 * \param [in] nof_meta_cycles_for_ageout - time in units of meta cycles until the vsi's entries are aged out. 
 * e.g. if the meta cycle was configured to 10 seconds and nof_meta_cycles_for_ageout=2, the entries will be delete/aged out 
 * after 20 seconds. 
 *
 *
 * \return
 *   shr_error_e 
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_nof_age_cycles_before_ageout_set(
    int unit,
    int vsi,
    int is_lif,
    int nof_meta_cycles_for_ageout);

/**
 * \brief - Retrieve the number of meta cycles until an entry with the given vlan is aged out.
 * Possible number of meta cycles are: 0 (no aging), 1, 2, ..., 8, 10, 12, 14, 16, 32, 64, 128
 *
 * \param [in] unit - unit id
 * \param [in] vsi - vsi to fetch its number of aging cycles before age out.
 * \param [out] nof_meta_cycles_for_ageout - time in units of meta cycles until the vsi's entries are aged out.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_nof_age_cycles_before_ageout_get(
    int unit,
    int vsi,
    int *nof_meta_cycles_for_ageout);

/**
 * \brief - Set the aging profiles to default values according to the learn mode.
 *
 * \param [in] unit - unit id
 * \param [in] learn_mode - type of learn that is configured
 *
 * \return
 *   shr_error_e
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_aging_profile_init(
    int unit,
    dnx_learn_mode_t learn_mode);

/**
 * \brief - Scan all MAC addresses and decrement age-status in case that aging is disabled and age machine is not still working.
 *     Manual triggering of the age machine is possible only when automatic aging is disabled.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_age_scan_set(
    int unit);
/**
 * \brief - Enable/Disable the limit check when the CPU is adding a MACT entry.
 *          Both limits per FID and the global MACT limit are enabled/disabled.
 *
 * \param [in] unit - Relevant unit
 * \param [in] enable - '1' means limits are checked. '0' means limits are forced for CPU and MACT entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_learn_limit_to_cpu_set(
    int unit,
    uint32 enable);

/**
 * \brief - Return whether the limits on the MACT are forced on CPU MACT additions.
 *
 * \param [in] unit - Relevant unit
 * \param [out] is_enabled - '1' means limits are checked. '0' means limits are not forced for CPU add MACT entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_learn_limit_to_cpu_get(
    int unit,
    int *is_enabled);

/**
 * \brief - Set the properties of a VSI to default values
 * 
 * \param [in] unit - unit id
 * \param [in] vsi - vsi to update
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vsi_profile_set_to_default(
    int unit,
    int vsi);

/*
 * }
 */
#endif /* L2_H_INCLUDED */
