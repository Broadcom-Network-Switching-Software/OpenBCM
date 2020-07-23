/*! \file bcmpc_types_internal.h
 *
 * BCMPC internal types.
 *
 * Declaration of the structures, enumerations which are exported by
 * the Port Control (BCMPC) module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_TYPES_INTERNAL_H
#define BCMPC_TYPES_INTERNAL_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpc/bcmpc_types.h>


/*! MAC address type. */
typedef shr_mac_t bcmpc_mac_t;

/*! Bitmap of ports of a particular type or properties. */
typedef bcmdrd_pbmp_t bcmpc_pbmp_t;


/*! \brief Invalid Profile ID.
 *
 * \c BCMPC_INVALID_PROF_ID is a special value, which is not associated with
 * any real profile. Essentially this means that index \c BCMPC_INVALID_PROF_ID
 * cannot be used in any of the profile tables.
 *
 * If the user does not specify the profile ID during an INSERT, the profile ID
 * will be set to \c BCMPC_INVALID_PROF_ID by default, but the configuration
 * will not be submitted to HW (because it is incomplete).
 */
#define BCMPC_INVALID_PROF_ID (0)

/*! The max number of units per system */
#define BCMPC_NUM_UNITS_MAX BCMDRD_CONFIG_MAX_UNITS

/*! The max number of physical ports per chip */
#define BCMPC_NUM_PPORTS_PER_CHIP_MAX BCMDRD_CONFIG_MAX_PORTS

/*! The max number of logical ports per chip */
#define BCMPC_NUM_LPORTS_PER_CHIP_MAX BCMDRD_CONFIG_MAX_PORTS

/*! The maximum number of physical ports per PM */
#define BCMPC_NUM_PPORTS_PER_PM_MAX (16)

/*! The maximum number of lanes per PM */
#define BCMPC_NUM_LANES_PER_PM_MAX (16)

/*! The maximum number of aggreated entities per PM */
#define BCMPC_NUM_AGGR_CORES_PER_PM_MAX (4)

/*! The maximum number of port operating modes per device */
#define BCMPC_NUM_OPMODES_PER_UNIT_MAX (16)

/*! The maximum number of RX DEF TAPs */
#define BCMPC_NUM_RX_DFE_TAPS_MAX (16)

/*! The maximum number of PLLs per PM */
#define BCMPC_NUM_PLLS_PER_PM_MAX (2)

/*! Default port MIB oversize value. */
#define BCMPC_PORT_MIB_OVERSIZE (0x5EE) /* 1516 */

/*! The maximum number of port abilities */
#define BCMPC_NUM_PORT_ABILITIES_MAX (128)

/*!
 * \brief Port Macro operating mode.
 *
 * For some PM types, a physical port may be associated with multiple
 * interfaces. For example, a PM with QTC and GPHY, a physical port can
 * be associated with an SGMII interface (BCMPC_PM_OPMODE_DEFAULT), a QSGMII
 * interface (BCMPC_PM_OPMODE_QSGMII), or a GPHY interface (BCMPC_PM_OPMODE_GPHY
 * ) for that matter, but only one interface at a time. The operating mode is
 * used to define the working mode as well.
 *
 * Note that the order of this enum should be same as PC_PM_OPMODE_T.
 */
typedef enum bcmpc_pm_opmode_e {

    /*! Default port mode. */
    BCMPC_PM_OPMODE_DEFAULT = 0,

    /*! Configure Gigabit core for QSGMII mode. */
    BCMPC_PM_OPMODE_QSGMII,

    /*! Configure Gigabit core to use internal copper PHYs. */
    BCMPC_PM_OPMODE_GPHY,

    /*! Must be the last. */
    BCMPC_PM_OPMODE_COUNT

} bcmpc_pm_opmode_t;

/*!
 * \brief Linkscan modes.
 */
typedef enum bcmpc_ls_mode_e {

    /*! Linkscan is disabled. */
    BCMPC_LS_MODE_DISABLED,

    /*! Linkscan is enabled with software linkscan mode. */
    BCMPC_LS_MODE_SW,

    /*! Linkscan is enabled with other linkscan mode, e.g. HW or FW. */
    BCMPC_LS_MODE_OTHER

} bcmpc_ls_mode_t;

/*!
 * \brief Linkscan port configuration.
 */
typedef struct bcmpc_lm_mode_s {

    /*! Link status is in override mode. */
    bool override_link_state;

    /*! Link UP/DOWN sequence is driven by user application. */
    bool manual_sync;

    /*! Linkscan mode. */
    bcmpc_ls_mode_t ls_mode;

} bcmpc_lm_mode_t;

/*!
 * \brief Port macro configuration.
 *
 * This structure determines how a port macro is divided into logical
 * ports. Please refer to the documentation for the specific port
 * macro to see which lane configurations are supported.
 *
 * Note that some port macro contain multiple cores (e.g. PM12X10) in
 * which case the operating mode must be specified for all the cores.
 *
 * Each logical port must also be assigned a maximum speed, which is
 * used by the MMU to reserve packet (cell) buffers.
 *
 * The \c SPEED_MAX array is indexed by the port macro physical port
 * number associated with a logical port. For example for a PM4X10
 * port macro in DUAL mode (2 x 20 Gbps), the array would be assigned
 * like this:
 *
 * \verbatim
 *     SPEED_MAX[0] = 20000
 *     SPEED_MAX[1] = 0
 *     SPEED_MAX[2] = 20000
 *     SPEED_MAX[3] = 0
 *     LANE_MAP[0] = 0x3
 *     LANE_MAP[1] = 0
 *     LANE_MAP[2] = 0xc
 *     LANE_MAP[3] = 0
 * \endverbatim
 */
typedef struct bcmpc_pm_mode_s {

    /*! Port macro operating mode. */
    bcmpc_pm_opmode_t opmode[BCMPC_NUM_AGGR_CORES_PER_PM_MAX];

    /*! PM port maximum speed. */
    uint32_t speed_max[BCMPC_NUM_PPORTS_PER_PM_MAX];

    /*! PM port lane map. */
    uint32_t lane_map[BCMPC_NUM_PPORTS_PER_PM_MAX];

} bcmpc_pm_mode_t;

/*!
 * A special value to indicate the param is don't care to the operation,
 * /ref bcmpc_operation_t.
 */
#define BCMPC_OP_PARAM_NONE ((uint32_t)-1)

/*!
 * \brief BCMPC operation description.
 *
 * This structure is used to specify a driver operation which is used in \ref
 * bcmpc_cfg_profile_t.
 *
 * For a operation, in addition to the name of the operation, it also contains
 * a driver name and an integer parameter for the operation.
 */
typedef struct bcmpc_operation_s {

    /*! Drive name. */
    char *drv;

    /*! Operation name. */
    char *name;

    /*! Operation parameter. */
    uint32_t param;

} bcmpc_operation_t;

/*!
 * \brief Port interrupt types.
 *
 * This enum defines the interrupt types supported.
 */
typedef enum bcmpc_intr_type_s {
    /*! FDR interrupt. */
    BCMPC_INTR_FDR,

    /*! One bit ECC errors */
    BCMPC_INTR_ECC_1B_ERR_RSFEC_RBUF_MPP0,
    BCMPC_INTR_ECC_1B_ERR_RSFEC_RBUF_MPP1,
    BCMPC_INTR_ECC_1B_ERR_RSFEC_MPP0,
    BCMPC_INTR_ECC_1B_ERR_RSFEC_MPP1,
    BCMPC_INTR_ECC_1B_ERR_DESKEW,
    BCMPC_INTR_ECC_1B_ERR_SPEED,
    BCMPC_INTR_ECC_1B_ERR_AM,
    BCMPC_INTR_ECC_1B_ERR_UM,
    BCMPC_INTR_ECC_1B_ERR_TX_1588,
    BCMPC_INTR_ECC_1B_ERR_RX_1588_MPP0,
    BCMPC_INTR_ECC_1B_ERR_RX_1588_MPP1,


    /*! Two bit ECC errors */
    BCMPC_INTR_ECC_2B_ERR_RSFEC_RBUF_MPP0,
    BCMPC_INTR_ECC_2B_ERR_RSFEC_RBUF_MPP1,
    BCMPC_INTR_ECC_2B_ERR_RSFEC_MPP0,
    BCMPC_INTR_ECC_2B_ERR_RSFEC_MPP1,
    BCMPC_INTR_ECC_2B_ERR_DESKEW,
    BCMPC_INTR_ECC_2B_ERR_SPEED,
    BCMPC_INTR_ECC_2B_ERR_AM,
    BCMPC_INTR_ECC_2B_ERR_UM,
    BCMPC_INTR_ECC_2B_ERR_TX_1588,
    BCMPC_INTR_ECC_2B_ERR_RX_1588_MPP0,
    BCMPC_INTR_ECC_2B_ERR_RX_1588_MPP1,

    /*! Should be last. */
    BCMPC_INTR_COUNT
} bcmpc_intr_type_t;

#endif /* BCMPC_TYPES_INTERNAL_H */
