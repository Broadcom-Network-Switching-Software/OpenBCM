/*! \file bcmtm_oobfc_internal.h
 *
 * Definitions for OOBFC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_OOBFC_INTERNAL_H
#define BCMTM_OOBFC_INTERNAL_H

#include <bcmltd/bcmltd_types.h>
#include <bcmtm/bcmtm_types.h>

/*! Valid configuration. */
#define VALID  0

/*! Invalid OOBFC bit offset. */
#define OOBFC_BIT_OFFSET_INVALID 1

/*! Invalid unicast queue ID. */
#define OOBFC_UC_Q_INVALID  2

/*! Invalid multicast queue ID. */
#define OOBFC_MC_Q_INVALID  2


/*!
 * \brief TM OOBFC queue profile.
 */
typedef struct bcmtm_oobfc_q_profile_s {

    /*! Congestion notification enable. */
    uint8_t cng_notify;

    /*! OOBFC message bit oofset. */
    uint8_t oob_bit_offset;

    /*! Operational state. */
    uint8_t opcode;

} bcmtm_oobfc_q_profile_t;

/*!
 * \brief TM OOBFC port configuation.
 */
typedef
struct bcmtm_oobfc_port_cfg {

    /*! OOBFC message for ingress congestion. */
    bool ingress;

    /*! OOBFC message for egress congestion. */
    bool egress;

    /*! Enable to configure OOBFC reporting. */
    bool cng_report;

    /*! OOBFC queue mapping profile ID. */
    uint8_t q_map_profile_id;

    /*! OOBFC Port ID. */
    int16_t oob_port;

    /*! Operational state */
    uint8_t opcode;

    /*! Action. */
    uint8_t action;
} bcmtm_oobfc_port_cfg_t;

/*!
 * \brief TM OOBFC egress service pool configuration.
 */
typedef
struct bcmtm_oobfc_sp_cfg_s {
    /*! Unicast merge */
    int ucq_merge;

    /*! Multicast merge */
    int mcq_merge;
} bcmtm_oobfc_sp_cfg_t;

/*!
 * Device specific function pointers for physical table configuration
 * for OOBFC unicast profile queue configurations.
 */
typedef int (*bcmtm_oobfc_ucq_profile_pt_set_f) (int unit,
                                        bcmltd_sid_t ltid,
                                        uint8_t profile_id,
                                        uint8_t ucq_id,
                                        bcmtm_oobfc_q_profile_t *ucq_profile);

/*!
 * Device specific function pointers for physical table configuration
 * for OOBFC multicast profile queue configurations.
 */
typedef int (*bcmtm_oobfc_mcq_profile_pt_set_f) (int unit,
                                        bcmltd_sid_t ltid,
                                        uint8_t profile_id,
                                        uint8_t mcq_id,
                                        bcmtm_oobfc_q_profile_t *ucq_profile);

/*!
 * Device specific funcition pointers for physical table configuration
 * for OOBFC port profile configurations.
 */
typedef int (*bcmtm_oobfc_port_profile_pt_set_f) (int unit,
                                        bcmltd_sid_t ltid,
                                        bcmtm_lport_t lport,
                                        bcmtm_oobfc_port_cfg_t *port_cfg);

/*!
 * Device specific function pointers for physical table configuration
 * for OOBFC service pool configurations.
 */
typedef int (*bcmtm_oobfc_egr_sp_pt_set_f) (int unit,
                                            bcmltd_sid_t ltid,
                                            uint8_t sp_id,
                                            bcmtm_oobfc_sp_cfg_t *oobfc_sp_cfg);
/*! Device specific functions. */
typedef struct bcmtm_oobfc_drv_s {
    /*! OOBFC unicast queue profile physical table configuration. */
    bcmtm_oobfc_ucq_profile_pt_set_f ucq_profile_set;
    /*! OOBFC multicast queue profile physical table configuration. */
    bcmtm_oobfc_mcq_profile_pt_set_f mcq_profile_set;
    /*! OOBFC port profile physical table configuration. */
    bcmtm_oobfc_port_profile_pt_set_f port_set;
    /*! OOBFC egress service pool physical table configuration. */
    bcmtm_oobfc_egr_sp_pt_set_f egr_sp_set;
} bcmtm_oobfc_drv_t;

/*!
 * IMM callback register for TM_OOBFC_UC_Q_MAP_PROFILE logical table.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_oobfc_ucq_profile_imm_register(int unit);

/*!
 * IMM callback register for TM_OOBFC_MC_Q_MAP_PROFILE logical table.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_oobfc_mcq_profile_imm_register(int unit);

/*!
 * IMM callback register for TM_OOBFC_PORT logical table.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_imm_oobfc_port_imm_register(int unit);

/*!
 * IMM callback register for OOBFC logical table.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_imm_oobfc_cb_reg(int unit);

/*!
 * OOBFC port internal updates in case of flex operations.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port ID.
 * \param [in] action Update/Delete action.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_oobfc_port_internal_update(int unit,
                                 bcmtm_lport_t lport,
                                 uint8_t action);

#endif /* BCMTM_OOBFC_INTERNAL_H */
