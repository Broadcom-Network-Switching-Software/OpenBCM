/*! \file bcmtm_shaper_internal.h
 *
 * Definitions for Shapers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_SHAPER_INTERNAL_H
#define BCMTM_SHAPER_INTERNAL_H

#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>

/*!
 * \brief TM Bucket rate encoded values.
 */
typedef struct bcmtm_shaper_bucket_encode_s {
    /*! Refresh rate. */
    uint32_t refresh_rate;
    /*! Granularity */
    uint8_t  granularity;
    /*! Bucket size. */
    uint16_t bucket_sz;
    /*! Bandwidth in kbps. */
    uint32_t bandwidth;
    /*! Burst in kbits. */
    uint32_t burst;

} bcmtm_shaper_bucket_encode_t;

/*!
 * \brief TM shaper configuration values.
 */
typedef struct bcmtm_shaper_cfg_s {
    /*! Shaping mode (byte mode/ packet mode). */
    uint8_t shaping_mode;

    /*! HULL mode. */
    uint8_t hull_mode;

    /*! Ethernet Audio Video settings. */
    uint8_t ethernet_av;

    /*! Max shaper configurations. */
    bcmtm_shaper_bucket_encode_t max_bucket_encode;

    /*! Min shaper configurations. */
    bcmtm_shaper_bucket_encode_t min_bucket_encode;

    /*! Burst auto calculation mode. */
    uint8_t burst_auto;

    /*! Scheduler profile ID. */
    uint8_t profile_id;

    /*! Inter frame gap */
    uint8_t byte_count_inter_frame_gap;

    /*! Encap /Decap for interframe gap */
    uint8_t ifg_encap;

    /*! Inter frame gap higig2 */
    uint8_t byte_count_inter_frame_gap_higig2;

    /*! Action to be performed on the entry. */
    uint8_t action;

    /*! Operational state of the profile. */
    uint8_t opcode;

} bcmtm_shaper_cfg_t;

/*!
 * Device specific function pointer for physical table configuration
 * for port shaper.
 */
typedef int (*bcmtm_shaper_port_pt_set_f) (int unit,
        bcmtm_lport_t lport,
        bcmltd_sid_t ltid,
        bcmtm_shaper_cfg_t *shaper_cfg,
        int enable);

/*!
 * Device specific function pointer for physical table configuration
 * for non-cpu shapers.
 */
typedef int (*bcmtm_shaper_node_pt_set_f) (int unit,
        bcmtm_lport_t lport,
        uint32_t node_level,
        uint32_t node_id,
        bcmltd_sid_t ltid,
        bcmtm_shaper_cfg_t *shaper_cfg);

/*!
 * Device specific function pointer for physical table configuration
 * for CPU Node shapers.
 */
typedef int (*bcmtm_shaper_cpu_node_pt_set_f) (int unit,
        uint32_t sched_node_id,
        uint32_t sched_level,
        bcmltd_sid_t ltid,
        bcmtm_shaper_cfg_t *shaper_cfg);

/*!
 * Device specific function pointer for shaper encoding.
 */
typedef int (*bcmtm_shaper_rate_to_bucket_f) (int unit,
        bcmtm_lport_t lport,
        uint32_t shaping_mode,
        uint8_t burst_auto,
        bcmtm_shaper_bucket_encode_t *bucket_encode);

/*!
 * Device specific function pointer for shaper decoding.
 */
typedef int (*bcmtm_shaper_bucket_to_rate_f) (int unit,
        uint32_t shaping_mode,
        bcmtm_shaper_bucket_encode_t *bucket_encode);


/*! Device specific funtions. */
typedef struct bcmtm_shaper_drv_s {
    /*! Port shaper physical table configuration */
    bcmtm_shaper_port_pt_set_f port_set;
    /*! Scheduler node shaper physical table configuration */
    bcmtm_shaper_node_pt_set_f node_set;
    /*! CPU Scheduler node shaper physical table configuration */
    bcmtm_shaper_cpu_node_pt_set_f cpu_node_set;
    /*! Shaper application readable value  to hardware value conversion. */
    bcmtm_shaper_rate_to_bucket_f rate_to_bucket;
    /*! Shaper hardware value to application readable value conversion. */
    bcmtm_shaper_bucket_to_rate_f bucket_to_rate;
}bcmtm_shaper_drv_t;

/*! \brief Internally updates shaper node in case of internal
 * port updates.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] profile_id Scheduler profile ID.
 * \param [in] profile Scheduler profile.
 * \param [in] action Update action.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_shaper_node_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t profile_id,
                                  bcmtm_scheduler_profile_t *profile,
                                  uint8_t action);

/*! \brief Internally updates shaper port in case of internal
 * port updates.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] action Update action.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_shaper_port_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t action);

#endif /* BCMTM_SHAPER_INTERNAL_H */

