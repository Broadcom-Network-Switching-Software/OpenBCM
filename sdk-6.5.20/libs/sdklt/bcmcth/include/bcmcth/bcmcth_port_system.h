/*! \file bcmcth_port_system.h
 *
 * This file contains port system
 * related data structures and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PORT_SYSTEM_H
#define BCMCTH_PORT_SYSTEM_H

#include <shr/shr_bitop.h>
#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>

/*! Port system status. */
typedef struct bcmcth_port_system_s {
    /*! Operation Mode. */
    uint8_t     operation_mode;
    /*! L3_IIF. */
    uint32_t    l3_iif_id;
    /*! VRF. */
    uint32_t    vrf_id;
    /*! VLAN. */
    uint32_t    vlan_id;
} bcmcth_port_system_t;

/*! No Operation. */
#define    BCMCTH_PORT_SYSTEM_OP_MODE_NONE    0
/*! L3_IIF mode. */
#define    BCMCTH_PORT_SYSTEM_OP_MODE_L3_IIF    2
/*! VRF mode. */
#define    BCMCTH_PORT_SYSTEM_OP_MODE_VRF    3
/*! VLAN mode. */
#define    BCMCTH_PORT_SYSTEM_OP_MODE_VLAN    4


/*!
 * PORT_SYSTEM operation mode sw state.
 */
typedef struct bcmcth_port_system_state_s {
    /*! Operation mode state table size. */
    uint32_t                tbl_sz;
    /*! Operation mode sw state. */
    bcmcth_port_system_t        *info;
    /*! Operation mode backup sw state. */
    bcmcth_port_system_t        *info_ha;
} bcmcth_port_system_state_t;

/*!
 * \brief Get port system state pointer.
 *
 * Get port system state pointer for the unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [out] state         Returned state pointer.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_port_system_state_get(int unit, bcmcth_port_system_state_t **state);

/*!
 * \brief Port system sw resources init.
 *
 * Initialize Port system SW data structures for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  warm          Cold/Warm boot.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_port_system_sw_state_init(int unit, bool warm);

/*!
 * \brief Port system sw resources cleanup.
 *
 * Cleanup Port system SW data structures allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_port_system_sw_state_cleanup(int unit);

#endif /* BCMCTH_PORT_SYSTEM_H */
