/*! \file bcmcth_port_util.h
 *
 * PORT component utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PORT_UTIL_H
#define BCMCTH_PORT_UTIL_H

/*******************************************************************************
 * Includes
 */
#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmltd/bcmltd_handler.h>

/*! Port Identifier.  */
typedef int bcmport_id_t;

/*!
 * \brief This macro is used to set a Field present indicator bit status.
 *
 * Sets the Field \c _f present bit indicator in the Field Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Nothing.
 */
#define BCMPORT_LT_FIELD_SET(_m, _f)    \
            do {                        \
                SHR_BITSET(_m, _f);;      \
            } while (0)

/*!
 * \brief This macro is used to get the Field present indicator bit status.
 *
 * Returns the Field \c _f present bit indicator status from the Field
 * Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Field present indicator bit status from the Field bitmap.
 */
#define BCMPORT_LT_FIELD_GET(_m, _f)  (SHR_BITGET(_m, _f))


/*!
 * \brief PORT_MEMBERSHIP_POLICY Logical Table Fields information.
 */
typedef enum bcmport_membership_policy_lt_fields_e {
    PORT_ID = 0,
    ING_VLAN_MEMBERSHIP_CHECK,
    EGR_VLAN_MEMBERSHIP_CHECK,
    SKIP_VLAN_CHECK
} bcmport_membership_policy_fields_t;

/*! Maximum priority count. */
#define MAX_PRI_CNT  8

/*! Maximum TPID count. */
#define MAX_TPID_CNT 4

/*! HA subcomponent ID for port mirror. */
#define BCMPORT_MIRROR_SUB_COMP_ID 1

/*! HA subcomponent ID for port system. */
#define BCMPORT_SYSTEM_SUB_COMP_ID 2


/*! Maximum field count of PORT imm logical tables. */
#define PORT_FIELD_COUNT_MAX 4

/*! Maximum field ID of PORT logical tables. */
#define PORT_FIELD_ID_MAX 32

/*! Maximum  port count. */
#define MAX_PORTS BCMDRD_CONFIG_MAX_PORTS

/*! Data structure to save the info of PORT_ING_MIRRORt entry. */
typedef struct port_imirror_s {

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, PORT_FIELD_ID_MAX);

    /*! Ingress logical port. */
    shr_port_t port;

    /*! Mirror container. */
    uint32_t container;

    /*! Mirror session ID. */
    uint32_t session_id;

    /*! Mirror instance ID. */
    uint32_t instance_id;

    /*! Enable/disable mirror. */
    bool enable;

} port_imirror_t;

/*! Data structure to save the info of PORT_EGR_MIRRORt entry. */
typedef struct port_emirror_s {

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, PORT_FIELD_ID_MAX);

    /*! Ingress logical port. */
    shr_port_t port;

    /*! Egress logical port. */
    shr_port_t egr_port;

    /*! Mirror Container. */
    uint32_t container;

    /*! Mirror session ID. */
    uint32_t session_id;

    /*! Mirror instance ID. */
    uint32_t instance_id;

    /*! Enable/disable mirror. */
    bool enable;

} port_emirror_t;

/*! Data structure to save the info of PORT_MIRRORt entry. */
typedef struct port_mirror_s {

    /*! Ingress logical port. */
    shr_port_t port;

    /*! Enable to mirror to the CPU. */
    bool cpu;

} port_mirror_t;

/*! Data structure to save the info from PORT_BRIDGE LT entry. */
typedef struct port_bridge_info_s {
    /*! Bitmap of valid struct members. */
    uint32_t        member_bmp;

    /*! Bit for the PORT_ID logical table field. */
#define             MEMBER_BMP_PORT_ID         (1 << 0)

    /*! Bit for the BRIDGE logical table field. */
#define             MEMBER_BMP_BRIDGE          (1 << 1)

    /*! Ingress logical port. */
    shr_port_t      port;

    /*! Enable L2 bridging. */
    bool            bridge;
} port_bridge_info_t;

/*!
 * \brief Structure for PORT_MEMBERSHIP_POLICY logical table entry information.
 */
typedef struct port_membership_policy_info_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, PORT_FIELD_ID_MAX);

    /*! Store LT entry Port Identifier key field value. */
    bcmport_id_t      port_id;

    /*! Store LT entry Ingress Vlan Check Field value. */
    bool              ing_vlan_membership_check;

    /*! Store LT entry Egress Vlan Check Field value. */
    bool              egr_vlan_membership_check;

    /*! Store LT entry Vlan Check Skip field value. */
    bool              skip_vlan_check;

} port_membership_policy_info_t;

/*!
 * \brief Structure for PORT_SYSTEM_ING_MIRROR_PROFILE
 *  logical table entry information.
 */
typedef struct port_system_imirror_profile_s {

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, PORT_FIELD_ID_MAX);

    /*! Port system profile instance ID. */
    int      tbl_inst;

    /*! Port system profile ID. */
    uint16_t port_system_profile_id;

    /*! Mirror instance ID. */
    uint32_t instance_id;

    /*! Mirror Container. */
    uint32_t container;

    /*! Enable/disable mirror. */
    bool enable;

} port_system_imirror_profile_t;

/*!
 * The maximum field ID of PORT_SYSTEM_DESTINATION
 * logical tables.
 */
#define PORT_SYSTEM_DEST_FIELD_ID_MAX 7

/*!
 * \brief PORT_SYSTEM_DESTINATION Logical Table
 *  Fields information.
 */
typedef enum bcmport_system_dest_lt_fields_e {

    /*! PORT_SYSTEM_ID logical table field. */
    PORT_SYSTEM_ID = 0,

    /*! TRUNK_VALID logical table field. */
    TRUNK_VALID,

    /*! TRUNK_ID logical table field. */
    TRUNK_ID,

    /*! LOCAL_PORT_ID logical table field. */
    LOCAL_PORT_ID,

    /*! DLB_ID_VALID logical table field. */
    DLB_ID_VALID,

    /*! DLB_ID logical table field. */
    DLB_ID,

    /*! FLEX_CTR_ACTION logical table fiyyeld. */
    FLEX_CTR_ACTION,

} bcmport_system_dest_lt_fields_t;

/*!
 * \brief Structure for PORT_SYSTEM_DESTINATION
 * logical Table entry information.
 */
typedef struct port_system_dest_entry_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, PORT_SYSTEM_DEST_FIELD_ID_MAX);

    /*! System port identifier. */
    uint16_t port_system_id;

    /*! Trunk valid variable. */
    bool trunk_valid;

    /*! Trunk system ID. */
    uint32_t trunk_id;

    /*! logical port identifier. */
    uint32_t port_id;

    /*! DLB ID valid. */
    bool  dlb_id_valid;

    /*! DLB ID. */
    uint16_t  dlb_id;

    /*! Flex counter action. */
    uint8_t flex_ctr_action;

} port_system_dest_entry_t;

/*!
 * The maximum field ID of PORT_SYSTEM_DESTINATION
 * logical tables.
 */
#define PORT_TRUNK_FIELD_ID_MAX 7

/*!
 * \brief PORT_TRUNK Logical Table
 *  Fields information.
 */
typedef enum bcmport_trunk_pctl_lt_fields_e {

    /*! TRUNK_PORT_ID logical table field. */
    TRUNK_PORT_ID = 0,

    /*! TRUNK_SYSTEM_FAILOVER logical table field. */
    TRUNK_SYSTEM_FAILOVER,

} bcmport_trunk_pctl_lt_fields_t;

/*!
 * \brief Structure for PORT_TRUNK
 * logical Table entry information.
 */
typedef struct port_trunk_pctl_entry_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, PORT_TRUNK_FIELD_ID_MAX);

    /*! System port identifier. */
    uint16_t port_id;

    /*! Trunk system failover. */
    bool trunk_system_failover;

} port_trunk_pctl_entry_t;

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Generic hw mem table read for BCMPORT.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_port_pt_read(int unit, const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                    int index, void *entry_data);

/*!
 * \brief Generic hw mem table write for BCMPORT.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_port_pt_write(int unit, const bcmltd_op_arg_t *op_arg,
                     bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                     int index, void *entry_data);

/*!
 * \brief Generic hw reg table read for BCMPORT.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_port_pr_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw reg table write for BCMPORT.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_port_pr_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                     bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw table interactive read for BCMPORT.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_port_ireq_read(int unit, bcmdrd_sid_t pt_id, int index, void *entry_data);

#endif /* BCMCTH_PORT_UTIL_H */
