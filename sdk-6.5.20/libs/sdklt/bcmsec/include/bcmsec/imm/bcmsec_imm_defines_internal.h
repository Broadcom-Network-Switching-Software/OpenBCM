/*! \file bcmsec_imm_defines_internal.h
 *
 * Function defintions for imm implementations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMSEC_IMM_DEFINES_INTERNAL_H
#define BCMSEC_IMM_DEFINES_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmltd/chip/bcmltd_sec_constants.h>
#include <bcmsec/bcmsec_pt_internal.h>

/*! Entry is valid. */
#define VALID                       0

/*! PM configuration for SEC conflicts with existing PM map. */
#define CONFLICTING_PM_PAIR         1

/*! Logical to physical port mapping is missing. */
#define PORT_INFO_UNAVAIL           1

/*! Marks the entry in IMM is invalid. */
#define ENTRY_INVALID             255

/*! Invalid profile. */
#define PROFILE_INVALID             BCMLTD_COMMON_SEC_ENTRY_STATE_T_T##_PROFILE_INVALID

/*! Port update actions. */
typedef enum bcmsec_port_internal_update_e {
    /*! Port add */
    ACTION_PORT_ADD_INT = 0,

    /*! Port delete */
    ACTION_PORT_DELETE_INT = 1
} bcmsec_port_internal_update_t;

/*!
 * \brief Decrypt management configuration values.
 */
typedef struct bcmsec_decrypt_mgmt_cfg_s {
    /*! SEC block id */
    uint8_t     blk_id;

    /*! Detination mac address */
    uint64_t    dst_mac[MGMT_MAX_DST_MAC];

    /*! Ethertype */
    uint32_t    ethertype[MGMT_MAX_ETHERTYPE];

    /*! Lower destination mac address of the range */
    uint64_t    dst_mac_range_low;

    /*! Higher destination mac address of the range */
    uint64_t    dst_mac_range_high;

    /*! Destination mac address for group 0 */
    uint64_t    dst_mac_group_0;

    /*! Ethertype for group 0 */
    uint32_t    ethertype_group_0;

    /*! Destination mac address for group 1 */
    uint64_t    dst_mac_group_1;

    /*! Ethertype for group 1 */
    uint32_t    ethertype_group_1;
} bcmsec_decrypt_mgmt_cfg_t;

/*! PT entry type. */
typedef enum bcmsec_tcam_pt_entry_type_e {
    /*! Key */
    BCMSEC_TCAM_ENTRY_TYPE_KEY = 0,

    /*! Data */
    BCMSEC_TCAM_ENTRY_TYPE_DATA = 1
} bcmsec_tcam_pt_entry_type_t;

/*! The hardware operation entry structure */
typedef struct bcmsec_tcam_pt_entry_info_s {
    /*! Macsec = 0, IPsec = 1 */
    bool ipsec;

    /*! The operation entry type */
    bcmsec_tcam_pt_entry_type_t type;

    /*! The size of array parameter tbl */
    int size;

    /*! Array of buffer pointers  */
    uint32_t **tbl;

} bcmsec_tcam_pt_entry_info_t;

/*!
 * \brief Encrypt port configuration values.
 */
typedef struct bcmsec_encrypt_port_control_s {
    /*! Physical port. */
    int pport;

    /*! Enable port based SC. */
    int port_based_sc;

    /*! Remove CRC in egress. */
    int remove_crc;

    /*! TX Threshold cells. */
    int tx_threshold;
} bcmsec_encrypt_port_control_t;

/*! \brief IMM callback register function.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_imm_cb_reg(int unit);

/*! \brief IMM init function.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_imm_init(int unit, bool warm);

/*! \brief  IMM update updates the SEC cconfiguration based on port updates.
 *
 * \param  [in] unit Unit number.
 * \param  [in] lport Logical port number.
 * \param  [in] action LT update/delete action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmsec_imm_update(int unit, int lport, uint8_t action);

/*! \brief IMM callback register function for SEC port macro control.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_pm_control_imm_register(int unit);

/*! \brief IMM callback register function for SEC Management.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_decrypt_mgmt_imm_register(int unit);

/*! \brief IMM callback register function for Encrypt SA Policy.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_sa_policy_encrypt_imm_register(int unit);

/*! \brief IMM callback register function for Decrypt SA Policy.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_sa_policy_decrypt_imm_register(int unit);

/*! \brief IMM port update function for encrypt port control.
 *
 * \param [in] unit number.
 * \param [in] lport logical port.
 * \param [in] port_op Port operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_port_control_internal_update (int unit, int lport,
                            bcmsec_port_internal_update_t port_op);

/*!
 * \brief Get PT information for SA policy table.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] pt_dyn_info Instance/Index information.
 * \param [in] encrypt Encrypt/Decrypt.
 * \param [out] sa_policy SA policy details.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_sa_policy_pt_get(int unit, bcmltd_sid_t ltid,
                        bcmsec_pt_info_t *pt_dyn_info,
                        bool encrypt,
                        sa_policy_info_t *sa_policy);

/*!
 * \brief Get PT information for SA policy table.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] pt_dyn_info Instance/Index information.
 * \param [in] encrypt Encrypt/Decrypt.
 * \param [in] in SA policy parameters.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_sa_policy_pt_set(int unit, bcmltd_sid_t ltid,
                        bcmsec_pt_info_t *pt_dyn_info,
                        bool encrypt,
                        bcmltd_field_t *in);
#endif /* BCMSEC_IMM_DEFINES_INTERNAL_H */
