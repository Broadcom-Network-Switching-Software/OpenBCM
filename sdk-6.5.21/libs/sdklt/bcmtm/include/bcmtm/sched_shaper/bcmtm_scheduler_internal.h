/*! \file bcmtm_scheduler_internal.h
 *
 * Definitions for Shapers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_SCHEDULER_INTERNAL_H
#define BCMTM_SCHEDULER_INTERNAL_H

#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>

/*! Maximum number of scheduler profile. */
#define MAX_TM_SCHEDULER_PROFILE   8

/*! Operational code for port update. */
#define PORT_UPDATE    255

/*! Scheduler node hierarchy. */
typedef enum
bcmtm_sched_node_e {

    /*! L0 Scheduler node. (COS node). */
    L0_SCHED_COS = 0,

    /*! L1 Scheduler node unicast queue. */
    L1_SCHED_UNICAST_QUEUE = 1,

    /*! L1 Scheduler node multicast queue. */
    L1_SCHED_MULTICAST_QUEUE = 2,
} bcmtm_sched_node_t;

/*! Scheduler mode. */
typedef enum
bcmtm_sched_mode_e {

    /*! Strict priority */
    SP = 0,

    /*! Round Robin */
    RR = 1,
} bcmtm_sched_mode_t;

/*! Default scheduler mode. */
#define DEFAULT_SCHED_MODE RR
/*! Default weight for RR scheduling mode. */
#define DEFAULT_WEIGHT 1

/*!
 * Scheduler profile configuration.
 */
typedef struct bcmtm_scheduler_profile_s {
    /*! Number of unicast queue */
    uint8_t num_ucq;

    /*! Number of multicast queue. */
    uint8_t num_mcq;

    /*! Enable flow control for unicast. */
    uint8_t fc_uc;

    /*! Operational state of the profile node. */
    uint8_t opcode;
}bcmtm_scheduler_profile_t;

/*!
 * Scheduler Configuration.
 */
typedef struct bcmtm_scheduler_cfg_s {
    /*! Scheduling mode. */
    uint8_t sched_mode;

    /*! Weight of the node. */
    uint8_t weight;

    /*! Scheduler Profile ID. */
    uint8_t profile_id;

    /*! WRR enable for the port. */
    uint8_t wrr_enable;

    /*! Parent node ID. */
    uint8_t parent_node;

    /*! Action to be performed on the entry. */
    uint8_t action;

    /*! Operational state of the profile. */
    uint8_t opcode;
} bcmtm_scheduler_cfg_t;


/*!
 * COS to L0 scheduler node profile mappings.
 */
typedef struct bcmtm_scheduler_cos_l0_map_s {
    /*! CoS to L0 map. */
    int map[MAX_TM_SCHEDULER_NODE];
}bcmtm_scheduler_cos_l0_map_t;


/*!
 * Device specific function pointer for physical table configuration
 * for port scheduler.
 */
typedef int
(*bcmtm_scheduler_port_pt_set_f) (int unit,
                                  bcmltd_sid_t ltid,
                                  bcmtm_lport_t lport,
                                  bcmtm_scheduler_profile_t *profile,
                                  bcmtm_scheduler_cfg_t *scheduler_cfg);

/*!
 * Device specific function pointer for physical table configuration
 * for non-cpu node schedulers.
 */
typedef int
(*bcmtm_scheduler_node_pt_set_f) (int unit,
                                  bcmtm_lport_t lport,
                                  uint32_t sched_node_id,
                                  uint32_t sched_level,
                                  bcmltd_sid_t ltid,
                                  bcmtm_scheduler_cfg_t *scheduler_cfg);
/*!
 * Device specific function pointer for physical table configuration
 * for CPU Node schedulers.
 */
typedef int
(*bcmtm_scheduler_cpu_node_pt_set_f) (int unit,
                                      uint32_t sched_node_id,
                                      uint32_t sched_level,
                                      bcmltd_sid_t ltid,
                                      bcmtm_scheduler_cfg_t *scheduler_cfg);

/*!
 * Device specific function pointer for profile configuration
 * for scheduler profiles.
 */
typedef int
(*bcmtm_scheduler_profile_pt_set_t) (int unit,
                                     bcmltd_sid_t ltid,
                                     uint8_t profile_id,
                                     bcmtm_scheduler_profile_t *profile);

/*!
 * Device specific function pointer for node profile validation.
 */
typedef int
(*bcmtm_scheduler_node_profile_validate_t) (int unit,
                                            int node_id,
                                            bcmtm_scheduler_profile_t *profile);

/*!
 * Device specific function pointer for profile validation.
 */
typedef int
(*bcmtm_scheduler_profile_validate_t) (int unit,
                                       uint8_t profile_id,
                                       int node_id,
                                       bcmtm_scheduler_profile_t *profile);

/*!
 * Device specific scheduler profile init function.
 */
typedef int
(*bcmtm_scheduler_profile_init_t) (int unit,
                                   uint8_t profile_id,
                                   bcmtm_scheduler_profile_t *profile);

/*! Device specific funtions. */
typedef struct bcmtm_scheduler_drv_s{
    /*! Port scheduler physical table configuration. */
    bcmtm_scheduler_port_pt_set_f port_set;
    /*! Scheduler node physical table configuration. */
    bcmtm_scheduler_node_pt_set_f node_set;
    /*! CPU scheduler node physical table configuration. */
    bcmtm_scheduler_cpu_node_pt_set_f cpu_node_set;
    /*! Scheduler profile physical table configuration. */
    bcmtm_scheduler_profile_pt_set_t profile_set;
    /*! Scheduler node profile validation. */
    bcmtm_scheduler_node_profile_validate_t node_profile_validate;
    /*! Scheduler profile validation. */
    bcmtm_scheduler_profile_validate_t profile_validate;
    /*! Scheduler profile initialization. */
    bcmtm_scheduler_profile_init_t profile_init;
}bcmtm_scheduler_drv_t;

/*!
 * \brief Gets the scheduler profile for a given profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Profile ID.
 * \param [out] profile  Scheduler profile.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_MEMORY Memory not available for creating field list.
 */
extern int
bcmtm_scheduler_profile_get(int unit,
                            uint8_t profile_id,
                            bcmtm_scheduler_profile_t *profile);

/*!
 * \brief Validates the scheduler profile.
 *
 * \param [in] unit Unit number.
 * \param [in] node_id Scheduler node id.
 * \param [in] profile_id Scheduler profile ID.
 * \param [in] profile Scheduler profile.
 * \param [out] status Profile status.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
extern int
bcmtm_scheduler_profile_validate(int unit,
                                 int node_id,
                                 uint8_t profile_id,
                                 bcmtm_scheduler_profile_t *profile,
                                 uint8_t *status);

/*!
 * \brief Get the scheduler profile for the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [out] profile_id Scheduler profile ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
extern int
bcmtm_scheduler_port_profile_get(int unit,
                                 bcmtm_lport_t lport,
                                 uint8_t *profile_id);

/*!
 * \brief Get the scheduler port profile update.
 *
 * Updates scheduler profile for all logical ports for a given profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Scheduler profile ID.
 * \param [in] profile Scheduler profile.
 * \param [in] action Update/delete action for the profile.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
extern int
bcmtm_scheduler_port_profile_update(int unit,
                                    uint8_t profile_id,
                                    bcmtm_scheduler_profile_t *profile,
                                    uint8_t action);
/*!
 * \brief Get the scheduler port profile update.
 *
 * Updates scheduler profile for all logical ports for a given profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] profile_id Scheduler profile ID.
 * \param [in] profile Scheduler profile.
 * \param [in] action Update action for the entry.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
extern int
bcmtm_scheduler_node_internal_update(int unit,
                                    bcmtm_lport_t lport,
                                    uint8_t profile_id,
                                    bcmtm_scheduler_profile_t *profile,
                                    uint8_t action);
/*!
 * \brief Get the scheduler port update.
 *
 * Updates scheduler logical ports for a given profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] profile_id Scheduler profile ID.
 * \param [in] profile Scheduler profile.
 * \param [in] action Update/delete  action for the profile.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
extern int
bcmtm_scheduler_port_update(int unit,
                            bcmtm_lport_t lport,
                            uint8_t profile_id,
                            bcmtm_scheduler_profile_t *profile,
                            uint8_t action);


/*! \brief Internal Scheduler SP profile updates.
 *
 * Updates the Scheduler SP profile for a given profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Scheduler profile ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
extern int
bcmtm_scheduler_sp_profile_internal_update(int unit,
                                           uint8_t profile_id);

/*! \brief Scheduler node ID validate
 *
 * Scheduler node ID validation for a given scheduler level.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical Port ID.
 * \param [in] level Scheduler level.
 * \param [in] node_id Scheduler node ID.
 * \param [in] profile Scheduler profile.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */

extern int
bcmtm_scheduler_node_id_validate(int unit,
                                 bcmtm_lport_t lport,
                                 uint32_t level,
                                 uint32_t node_id,
                                 bcmtm_scheduler_profile_t *profile);

/*!
 * \brief Check if the profile is valid.
 *
 * Checks for the software profile state obtained from IMM lookup.
 * This function doesnot perform hardware validations.
 *
 * \param [in] unit Unit number.
 * \param [in] profile Scheduler profile.
 *
 * \retval 1 Profile is valid.
 * \retval 0 Profile is invalid.
 */
extern int
bcmtm_scheduler_profile_is_valid(int unit,
                                 bcmtm_scheduler_profile_t *profile);

#endif /* BCMTM_SCHEDULER_INTERNAL_H */
