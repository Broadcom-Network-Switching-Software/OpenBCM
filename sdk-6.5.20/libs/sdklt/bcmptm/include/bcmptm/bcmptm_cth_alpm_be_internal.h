/*! \file bcmptm_cth_alpm_be_internal.h
 *
 * APIs, defines for PTM to interface with ALPM Custom Handler
 * backend part.
 * This file contains APIs, defines for CTH-ALPM interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_CTH_ALPM_BE_INTERNAL_H
#define BCMPTM_CTH_ALPM_BE_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>

/*******************************************************************************
 * Defines
 */
#define ALPM_L1_BLOCKS_MAX      10
#define ALPM_LN_BANKS_MAX       8
/*! Max L1 banks, applicable when L1 resource can be dynamically assigned */
#define ALPM_L1_BANKS_MAX       16

/*! ALPM modes */
#define ALPM_MODE_COMBINED      0
#define ALPM_MODE_PARALLEL      0x1
#define ALPM_MODE_RPF           0x2
#define ALPM_MODE_FP_COMP       0x4
#define ALPM_MODE_NO_LPM        0x8
#define ALPM_MODE_FIXED_DEFAULT (ALPM_MODE_COMBINED)
#define ALPM_MODE_FLEX_DEFAULT  (ALPM_MODE_NO_LPM)

/*! ALPM Level-1 key inputs */
#define ALPM_KEY_INPUT_LPM_DST_Q         0   /* IPv6-128 DST */
#define ALPM_KEY_INPUT_LPM_DST_D         1   /* IPv6-77 DST */
#define ALPM_KEY_INPUT_LPM_DST_S         2   /* IPv6-32 DST */
#define ALPM_KEY_INPUT_LPM_SRC_Q         3   /* IPv6-128 SRC */
#define ALPM_KEY_INPUT_LPM_SRC_D         4   /* IPv6-77 SRC */
#define ALPM_KEY_INPUT_LPM_SRC_S         5   /* IPv6-32 SRC */
#define ALPM_KEY_INPUT_FP_COMP_DST       6
#define ALPM_KEY_INPUT_FP_COMP_SRC       7
#define ALPM_KEY_INPUT_LPM_L3MC_Q        8
#define ALPM_KEY_INPUT_LPM_L3MC_D        9
#define ALPM_KEY_INPUT_LPM_L3MC_S        10

#define ALPM_ERROR_REASON_NONE          0
#define ALPM_ERROR_REASON_BANKS         1
#define ALPM_ERROR_REASON_IN_USE        2

/*! ALPM instances. */
#define ALPM_0     0
#define ALPM_1     1
#define ALPMS      2

/*******************************************************************************
 * Typedefs
 */
/*!
 * \brief ALPM control bank info
 */
typedef struct bcmptm_cth_alpm_control_bank_s {
    /*!< Level1 banks */
    int num_l1_banks;

    /*!< Level2 banks */
    int num_l2_banks;

    /*!< Level3 banks */
    int num_l3_banks;

    /*!< Level-1 UFT banks bitmap */
    uint32_t l1_bank_bitmap;

    /*!< Level-2 UFT banks bitmap */
    uint32_t l2_bank_bitmap;

    /*!< Level-3 UFT banks bitmap */
    uint32_t l3_bank_bitmap;

    /*!< Bucket bits */
    int l2_bucket_bits;

    /*!< Bucket bits */
    int l3_bucket_bits;
} bcmptm_cth_alpm_control_bank_t;

/*!
 * \brief ALPM compression key type
 */
typedef enum bcmptm_cth_alpm_comp_key_type_e {
    COMP_KEY_L4_PORT = 0,
    COMP_KEY_VRF,
    COMP_KEY_FULL,
    COMP_KEY_TYPES
} bcmptm_cth_alpm_comp_key_type_t;

/*!
 * \brief ALPM controls
 */
typedef struct bcmptm_cth_alpm_control_s {
    /*!< ALPM DBs */
    int alpm_dbs;

    /*!< ALPM mode */
    int alpm_mode;

    /*!< Num of levels for DB0 */
    int db0_levels;

    /*!< Num of levels for DB1 */
    int db1_levels;

    /*!< Num of levels for DB2 */
    int db2_levels;

    /*!< Num of levels for DB3 */
    int db3_levels;

    /*!< Level1 key input on logical blocks */
    int l1_key_input[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 key input on physical blocks */
    int l1_phy_key_input[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 DB select on logical blocks */
    int l1_db[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 DB select on physical blocks */
    int l1_phy_db[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 logical blocks mapping to physical blocks */
    int l1_blocks_l2p[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 physical blocks mapping to logical blocks */
    int l1_blocks_p2l[ALPM_L1_BLOCKS_MAX];

    /*!< Level1 blocks mapping is enabled. */
    int l1_blocks_map_en;

    /*!< Enable/disable uRPF */
    int urpf;

    /*!< Type of hit support */
    uint8_t hit_support;

    /*!< Enable large vrf */
    uint8_t large_vrf;

    /*!< Compression key type */
    bcmptm_cth_alpm_comp_key_type_t comp_key_type;

    /*!< Bank total info */
    bcmptm_cth_alpm_control_bank_t tot;

    /*!< Bank db0 info */
    bcmptm_cth_alpm_control_bank_t db0;

    /*!< Bank db1 info */
    bcmptm_cth_alpm_control_bank_t db1;

    /*!< Bank db2 info */
    bcmptm_cth_alpm_control_bank_t db2;

    /*!< Bank db3 info */
    bcmptm_cth_alpm_control_bank_t db3;

    /*!< Destination. */
    uint16_t destination;

    /*!< Destination mask. */
    uint16_t destination_mask;

    /*!< Destination type if match. */
    uint8_t destination_type_match;

    /*!< Destination type if not match. */
    uint8_t destination_type_non_match;

    /*!< IPv4 unicast global low route strength profile_index */
    uint8_t ipv4_uc_sbr;

    /*!< IPv4 unicast private route strength profile_index */
    uint8_t ipv4_uc_vrf_sbr;

    /*!< IPv4 unicast override route strength profile_index */
    uint8_t ipv4_uc_override_sbr;

    /*!< IPv6 unicast global low route strength profile_index */
    uint8_t ipv6_uc_sbr;

    /*!< IPv6 unicast private route strength profile_index */
    uint8_t ipv6_uc_vrf_sbr;

    /*!< IPv6 unicast override route strength profile_index */
    uint8_t ipv6_uc_override_sbr;

    /*!< IPv4 compression strength profile_index */
    uint8_t ipv4_comp_sbr;

    /*!< IPv6 compression strength profile_index */
    uint8_t ipv6_comp_sbr;

    /*!< Validation status */
    int reason;

    /*!< Return value */
    int rv;
} bcmptm_cth_alpm_control_t;

/*!
 * \brief ALPM resources usage info
 */
typedef struct bcmptm_cth_alpm_usage_info_s {
    /*!< Level1 max entries */
    int l1_max_entries;

    /*!< Level1 used entries */
    int l1_used_entries;

    /*!< Level1 utilization */
    int l1_entries_utilization;

    /*! Leveln max rbkts */
    int ln_max_rbkts;

    /*! Leveln used rbkts */
    int ln_used_rbkts;

    /*! Leveln rbkts utilization*/
    int ln_rbkts_utilization;

    /*! Leveln max entries in used rbkts */
    int ln_max_entries_rbkts;

    /*! Leveln used entries in used rbkts */
    int ln_used_entries_rbkts;

    /*! Leveln entries utilization in used rbkts */
    int ln_entries_utilization_rbkts;

    /*! KEY type */
    uint8_t kt;

    /*! VRF type */
    uint8_t vt;

    /*! ALPM database */
    uint8_t db;
} bcmptm_cth_alpm_usage_info_t;

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Reset alpm control.
 * \param [in] u Logical device id
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_alpm_control_reset(int u, int mtop);

/*!
 * \brief Get alpm control.
 * \param [in] u Logical device id
 * \param [out] ctrl ALPM control.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_alpm_control_get(int unit, int mtop, bcmptm_cth_alpm_control_t *ctrl);

/*!
 * \brief Set alpm control.
 * \param [in] u Logical device id
 * \param [in] ctrl ALPM control.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_alpm_control_set(int unit, int mtop, bcmptm_cth_alpm_control_t *ctrl);

/*!
 * \brief Get default alpm control.
 * \param [in] u Logical device id
 * \param [out] ctrl Default ALPM control.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_alpm_control_default_get(int unit, int mtop, bcmptm_cth_alpm_control_t *ctrl);

/*!
 * \brief Set default alpm control.
 * \param [in] u Logical device id
 * \param [in] ctrl Default ALPM control.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_alpm_control_default_set(int unit, int mtop, bcmptm_cth_alpm_control_t *ctrl);

/*!
 * \brief Initialize the custom handler for ALPM backend.
 * \param [in] u Logical device id
 * \param [in] warm Warm init.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_alpm_be_init(int u, bool warm);

/*!
 * \brief De-initialize the custom handler for ALPM backend.
 * \param [in] u Logical device id
 * \param [in] warm Warm cleanup
 *
 * \retval SHR_E_NONE if success.
 */
extern int
bcmptm_cth_alpm_be_cleanup(int u, bool warm);

#endif /* BCMPTM_CTH_ALPM_BE_INTERNAL_H */

