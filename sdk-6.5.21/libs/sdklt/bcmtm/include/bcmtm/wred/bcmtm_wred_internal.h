/*! \file bcmtm_wred_internal.h
 *
 * Definitions for WRED.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_WRED_INTERNAL_H
#define BCMTM_WRED_INTERNAL_H

#include <bcmltd/bcmltd_types.h>
#include <bcmtm/bcmtm_types.h>

/*! Clear profile. */
#define BCMTM_WRED_CNG_ACTION_CLEAR   0
/*! Set profile. */
#define BCMTM_WRED_CNG_ACTION_SET     1
/*! Default profile. */
#define BCMTM_WRED_CNG_ACTION_DEFAULT 2


/*! ECN mode */
typedef enum
bcmtm_ecn_mode_e {
    /*! Average queue size for congestion calculation. */
    AVG_Q_SIZE = 0,

    /*! Instantaeous queue size for congestion calculation. */
    INSTANT_Q_SIZE = 1,
} bcmtm_ecn_mode_t;

/*!
 * \brief TM WRED config fields.
 */
typedef struct bcmtm_wred_cfg_s {
    /*! Time scale parameter for avg queue size computation. */
    uint8_t weight;

    /*! Current q size enable. */
    uint8_t curr_q_size;

    /*! WRED enable. */
    uint8_t wred;

    /*! ECN enable. */
    uint8_t ecn;

    /*! ECN mode. */
    bcmtm_ecn_mode_t ecn_mode;

    /*! time profile ID. */
    uint8_t time_profile_id;

    /*! mark congestion profile ID. */
    uint8_t mark_cng_profile_id;

    /*! drop congestion profile ID. */
    uint8_t drop_cng_profile_id;

    /*! WCS profile id. */
    uint8_t wcs_profile_id;

    /*! Operational state. */
    uint8_t opcode;

    /*! Action */
    uint8_t action;
} bcmtm_wred_cfg_t;


/*!
 * \brief TM  WRED congestion profile.
 */
typedef struct bcmtm_wred_cng_profile_s {
    /*! Profile ID. */
    uint8_t profile_id;

    /*! Q average. */
    uint8_t q_avg;

    /*! Q min. */
    uint8_t q_min;

    /*! Service pool. */
    uint8_t sp;

    /*! Action. */
    uint8_t action;
} bcmtm_wred_cng_profile_t;


/*!
 * Device specific function pointers for physical table configuration
 * for WRED unicast queue configurations.
 */
typedef int (*bcmtm_wred_ucq_pt_set_f) (int unit,
                                        bcmtm_lport_t lport,
                                        uint32_t ucq_id,
                                        bcmltd_sid_t ltid,
                                        bcmtm_wred_cfg_t *wred_ucq_cfg);

/*!
 * Device specific function pointers for physical table configuration
 * for WRED port servicepool configurations.
 */
typedef int (*bcmtm_wred_portsp_pt_set_f) (int unit,
                                           bcmtm_lport_t lport,
                                           uint32_t portsp_id,
                                           bcmltd_sid_t ltid,
                                           bcmtm_wred_cfg_t *wred_portsp_cfg);

/*!
 * Device specific function pointers for physical table configuration
 * for WRED congestion profile configurations.
 */
typedef int (*bcmtm_wred_cng_profile_pt_set_f) (int unit,
                                                bcmltd_sid_t ltid,
                                                bcmtm_wred_cng_profile_t *cng_profile);


/*! Device specific functions. */
typedef struct bcmtm_wred_drv_s {
    /*! Wred unicast queue physical table configuration. */
    bcmtm_wred_ucq_pt_set_f ucq_set;

    /*! Wred port service pool physical table configuration. */
    bcmtm_wred_portsp_pt_set_f portsp_set;

    /*! Wred Congestion profile configuration. */
    bcmtm_wred_cng_profile_pt_set_f cng_profile_set;
} bcmtm_wred_drv_t;

/*!
 * \brief TM_WRED_UC_Qt internal updates.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port ID.
 * \param [in] action Internal updates action.
 */
extern int
bcmtm_wred_ucq_internal_update (int unit,
                                bcmtm_lport_t lport,
                                uint8_t action);
/*!
 * \brief TM_WRED_PORT_SERVICE_POOLt internal updates.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port ID.
 * \param [in] action Internal updates action.
 */

extern int
bcmtm_wred_portsp_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t action);
#endif /* BCMTM_WRED_INTERNAL_H */
