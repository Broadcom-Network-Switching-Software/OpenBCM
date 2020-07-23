/*! \file vlan.h
 *
 * VLAN management header file.
 * This file contains the management for VLAN.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_VLAN_H
#define BCMI_LTSW_VLAN_H

#include <bcm/vlan.h>
#include <sal/sal_types.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcmltd/bcmltd_lt_types.h>

/*!
 * \brief Field bitmap definitions.
 */
/*! VFI Field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_VFI             (1 << 0)
/*! L3_IIF Field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF          (1 << 1)
/*! L2_IIF Field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_L2_IIF          (1 << 2)
/*! Tag action fields to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS     (1 << 3)
/*! Ing xlate pre config info to get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_PRE_CONFIG      (1 << 4)
/*!Qpaque control id field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID     (1 << 5)
/*!svp field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP             (1 << 6)
/*!Qpaque control id 1 field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID_1   (1 << 7)
/*! strength_profile_index field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_STR_PROFILE_IDX (1 << 8)
/*! Pipe field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_PIPE            (1 << 9)
/*! Flex_ctr_action field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION (1 << 10)
/*! Opaque_obj0 field to set or get. */
#define BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0       (1 << 11)

/* Hw opaque control id total contains 6 bits.
 * Field OPAQUE_CTRL_ID[2:3] contains sw higher 2 bits.
 * Field OPAQUE_CTRL_ID_1[0:3] contains sw lower 4 bits.
 */
#define BCMI_LTSW_VLAN_XLATE_OPAQUE_HW2SW(sw, hw_0, hw_1) \
   ((sw) = (hw_1) | (((hw_0) & 0xc) << 2))
#define BCMI_LTSW_VLAN_XLATE_OPAQUE_SW2HW(sw, hw_0, hw_1) \
    do { \
        (hw_1) |= (sw) & 0xf; \
        (hw_0) |= ((sw) & 0x30) >> 2; \
    } while (0)
#define BCMI_LTSW_VLAN_XLATE_OPAQUE_MAX ((1 << 6) - 1)

/*!
 * \brief VLAN translation structure.
 */
typedef struct bcmi_ltsw_vlan_ing_xlate_cfg_s {
    /*! Field bitmap. */
    uint32_t fld_bmp;

    /*! pre config bitmap. */
    uint32_t fld_pre_bmp;

    /*! Virtual forwarding instance. */
    int vfi;

    /*! L3 ingress interface. */
    int l3_iif;

    /*! L2 ingress interface. */
    int l2_iif;

    /*! SVP. */
    int svp;

    /*! Otag action. */
    bcm_vlan_action_t otag_action;

    /*! Itag action. */
    bcm_vlan_action_t itag_action;

    /*! Opaque control id. */
    int opaque_ctrl_id;

    /*! Opaque control id 1. */
    int opaque_ctrl_id_1;

    /*! Pipe. */
    int pipe;

    /*! Flexctr action. */
    uint32_t flex_ctr_action;

    /*! Opaque obj0. */
    uint32_t opaque_obj0;
} bcmi_ltsw_vlan_ing_xlate_cfg_t;

/*!
 * \brief Callback function to handle an ingress VLAN translation entry.
 */
typedef int (*bcmi_ltsw_vlan_ing_xlate_traverse_cb)(
    /* Unit Number.*/
    int unit,

    /* VLAN translation key type. */
    bcm_vlan_translate_key_t key_type,

    /* Port ID. */
    bcm_gport_t port,

    /* Outer VLAN. */
    bcm_vlan_t outer_vlan,

    /* Inner VLAN. */
    bcm_vlan_t inner_vlan,

    /*! Configuration information for VLAN translation. */
    bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg,

    /*! User data of callback. */
    void *user_data
);

/*!
 * \brief VLAN info structure.
 */
typedef struct bcmi_ltsw_vlan_info_s {
    /*! vlan_id. */
    int vid;

    /*! STG. */
    int stg;
} bcmi_ltsw_vlan_info_t;

/*!
 * \brief Callback function to handle a VLAN entry.
 */
typedef int (*bcmi_ltsw_vlan_traverse_cb)(
    /* Unit Number.*/
    int unit,

    /*! Information for VLAN. */
    bcmi_ltsw_vlan_info_t *info,

    /*! User data of callback. */
    void *user_data
);

/*!
 * \brief Application switch control type of vlan for a specific device.
 */
typedef enum bcmi_ltsw_vlan_control_s {

    /*! Unknown VLAN ID to CPU. */
    bcmiVlanControlUnknownVlanToCpu = 0,

    /*! Private VLAN VID mismatch. */
    bcmiVlanControlSharedVlanMismatchToCpu = 1,

    /*! BPDU packets are dropped upon invalid VLAN. */
    bcmiVlanControlBpduInvalidVlanDrop = 2,

    /*! Enable shared vlan. */
    bcmiVlanControlSharedVlanEnable = 3,

    /*! Decode SNAP packets with non-zero OUI for protocol-based VLAN. */
    bcmiVlanControlSnapNonZeroOui = 4,

    /*! Enable egress port's spanning tree check before enqueuing the packet to the MMU for
        L2 forwarding. */
    bcmiVlanControlEgressStgCheckIngressEnable = 5
} bcmi_ltsw_vlan_control_t;

/*!
 * \brief Default profile entry index on VFI creation.
 */
/*! Index to the entry which has empty port members. */
#define BCMI_LTSW_VLAN_PROFILE_ING_MSHP_CHK_IDX_DEF   1
/*! Index to the entry which has empty port members. */
#define BCMI_LTSW_VLAN_PROFILE_EGR_MSHP_CHK_IDX_DEF   1
/*! Index to the entry which has empty port members. */
#define BCMI_LTSW_VLAN_PROFILE_EGR_UNTAG_IDX_DEF      0
/*! Index to the entry which is not valid to block. */
#define BCMI_LTSW_VLAN_PROFILE_ING_BLOCK_MASK_IDX_DEF 0

/*!
 * \brief Vlan table operation code.
 */
typedef enum bcmi_ltsw_vlan_table_opcode_s {

    /*! Insert. */
    bcmiVlanTableOpInsert = 0,

    /*! Delete. */
    bcmiVlanTableOpDelete = 1,

    /*! Set, insert/delete based on compare result. */
    bcmiVlanTableOpSet = 2,
} bcmi_ltsw_vlan_table_opcode_t;

/*!
 * \brief The directions of VLAN check.
 */
/*! VLAN membership check in IPIPE. */
#define BCMI_LTSW_VLAN_CHK_ING      (1 << 0)
/*! Egress VLAN membership check in IPIPE. */
#define BCMI_LTSW_VLAN_CHK_ING_EGR  (1 << 1)
/*! VLAN membership check in EPIPE. */
#define BCMI_LTSW_VLAN_CHK_EGR      (1 << 2)

/*!
 * \brief Add an entry to ingress VLAN translation table.
 *
 * This function is used to add an entry to ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_ing_xlate_add(int unit,
                             bcm_vlan_translate_key_t key_type,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg);

/*!
 * \brief Update an entry in ingress VLAN translation table.
 *
 * This function is used to update an entry in ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_ing_xlate_update(int unit,
                                bcm_vlan_translate_key_t key_type,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan,
                                bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg);

/*!
 * \brief Get an entry from ingress VLAN translation table.
 *
 * This function is used to get an entry from ingress VLAN translation table.
 * If fld_bmp is not specified (zero), all fields will be retrieved.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] cfg              Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_ing_xlate_get(int unit,
                             bcm_vlan_translate_key_t key_type,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg);

/*!
 * \brief Delete an entry from ingress VLAN translation table.
 *
 * This function is used to delete an entry from ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_ing_xlate_delete(int unit,
                                bcm_vlan_translate_key_t key_type,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan);
/*!
 * \brief Traverse over all entries in ingress translation table.
 *
 * This function is used to traverse over all ingress translation entries and
 * call given callback with action structure.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_ing_xlate_traverse(int unit,
                                  bcmi_ltsw_vlan_ing_xlate_traverse_cb cb,
                                  void *user_data);

/*!
 * \brief De-initialize the VLAN module.
 *
 * This function is used to de-initialize the VLAN module.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_vlan_detach(int unit);

/*!
 * \brief De-initialize the VLAN outer TPID information.
 *
 * This function is used to de-initialize the VLAN information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_init(int unit);

/*!
 * \brief De-initialize VLAN outer TPID information.
 *
 * This function is used to de-initialize VLAN outer TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_detach(int unit);

/*!
 * \brief initialize the VLAN inner TPID information.
 *
 * This function is used to initialize the VLAN information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_vlan_itpid_init(int unit);

/*!
 * \brief De-initialize VLAN inner TPID information.
 *
 * This function is used to de-initialize VLAN inner TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_vlan_itpid_detach(int unit);

/*!
 * \brief Get dafault TPID.
 *
 * This function is used to get dafault TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Default tpid.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_default_get(int unit, uint16_t *tpid);

/*!
 * \brief Add a new outer TPID entry.
 *
 * This function is used to add a new outer TPID entry. If a same TPID
 * already exists, simplely increase the reference count of the cached entry.
 * Otherwise, add the entry to the cached table and write the new entry into LT.
 * Only four distinct TPID values are currently supported.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be added.
 * \param [out] index                Index where the the new TPID is added.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_entry_add(int unit, uint16_t tpid, int *index);

/*!
 * \brief Get outer TPID value for a specified entry index.
 *
 * This function is used to get outer TPID value for a specified entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Index where the the outer TPID is gotten.
 * \param [out] tpid                 Outer TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_entry_get(int unit, int index, uint16_t *tpid);

/*!
 * \brief Delete outer TPID entry by index.
 *
 * This function is used to delete outer TPID entry by index. For legacy
 * behavior,  LT/HW TPID enties are not touched during the deletion.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                TPID entry Index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_entry_delete(int unit, int index);

/*!
 * \brief Get outer entry index for a specified TPID value.
 *
 * This function is used to get outer entry index for a specified TPID value.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Outer TPID.
 * \param [out] index                Outer TPID entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_entry_lookup(int unit, uint16_t tpid, int *index);

/*!
 * \brief Add reference count of outer TPID.
 *
 * This function is used to add reference count of outer TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Reference count depth.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_entry_ref_count_add(int unit, int index, int count);

/*!
 * \brief Lock outer VLAN TPID table.
 *
 * This function is used to lock outer VLAN TPID table.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_lock(int unit);

/*!
 * \brief Unlock outer VLAN TPID table.
 *
 * This function is used to unlock outer VLAN TPID table.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_otpid_unlock(int unit);

/*!
 * \brief Initialize the payload outer TPID information.
 *
 * This function is used to initialize the payload outer TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_vlan_payload_otpid_init(int unit);

/*!
 * \brief De-initialize payload outer TPID information.
 *
 * This function is used to de-initialize payload outer TPID information.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_vlan_payload_otpid_detach(int unit);

/*!
 * \brief Get dafault payload outer TPID.
 *
 * This function is used to get dafault payload outer TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Default tpid.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_payload_otpid_default_get(int unit, uint16_t *tpid);

/*!
 * \brief Add a new payload outer TPID entry.
 *
 * This function is used to add a new payload outer TPID entry. If a same TPID
 * already exists, simplely increase the reference count of the cached entry.
 * Otherwise, add the entry to the cached table and write the new entry into LT.
 * Only four distinct TPID values are currently supported.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be added.
 * \param [out] index                Index where the the new TPID is added.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_payload_otpid_entry_add(int unit, uint16_t tpid, int *index);

/*!
 * \brief Get payload outer TPID value for a specified entry index.
 *
 * This function is used to get payload outer TPID value for a specified entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Index where the the payload outer TPID is gotten.
 * \param [out] tpid                 Payload outer TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_payload_otpid_entry_get(int unit, int index, uint16_t *tpid);

/*!
 * \brief Delete outer TPID entry by index.
 *
 * This function is used to delete payload outer TPID entry by index. For legacy
 * behavior,  LT/HW TPID enties are not touched during the deletion.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                TPID entry Index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_payload_otpid_entry_delete(int unit, int index);

/*!
 * \brief Get out the entry index for a specified payload outer TPID value.
 *
 * This function is used to get outer entry index for a specified payload outer TPID value.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Payload outer TPID.
 * \param [out] index              Payload outer TPID entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_payload_otpid_entry_lookup(int unit, uint16_t tpid, int *index);

/*!
 * \brief Add reference count of payload outer TPID.
 *
 * This function is used to add reference count of payload outer TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 Reference count depth.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_payload_otpid_entry_ref_count_add(int unit, int index, int count);

/*!
 * \brief Set global inner TPID.
 *
 * This function is used to set global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be set.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_itpid_set(int unit, uint16_t tpid);

/*!
 * \brief Get global inner TPID.
 *
 * This function is used to get global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] tpid                 Inner TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_itpid_get(int unit, uint16_t *tpid);

/*!
 * \brief Set force VLAN port type into VLAN table.
 *
 * This function is used to set force VLAN port type into VLAN table.
 *
 * \param [in] unit              Unit number.
 * \param [in] vlan              VLAN ID.
 * \param [in] port_type         Force VLAN port type -
 *                               BCM_PORT_FORCE_VLAN_*_PORT.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_force_port_set(int unit, int vlan, int port_type);

/*!
 * \brief Get force VLAN port type from VLAN table.
 *
 * This function is used to get force VLAN port type into VLAN table.
 *
 * \param [in] unit              Unit number.
 * \param [in] vlan              VLAN ID.
 * \param [out]port_type         Force VLAN port type -
 *                               BCM_PORT_FORCE_VLAN_*_PORT.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_force_port_get(int unit, int vlan, int *port_type);

/*!
 * \brief Set vlan default flood mode.
 *
 * This function is used to set vlan create default flood mode.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  mode                 VLAN multicast flood mode.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_flood_default_set(int unit, bcm_vlan_mcast_flood_t mode);

/*!
 * \brief Get vlan default flood mode.
 *
 * This function is used to get vlan create default flood mode.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] mode                 VLAN multicast flood mode.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_flood_default_get(int unit, bcm_vlan_mcast_flood_t *mode);

/*!
 * \brief Callback function to update the VLAN information in L2 station.
 */
typedef int (*bcmi_ltsw_vlan_l2_station_update_cb) (
    /*! Unit Number. */
    int unit,

    /*! VLAN ID. */
    bcm_vlan_t vlan,

    /*! Update flags. */
    int flags,

    /*! VLAN information. */
    bcmi_ltsw_l2_station_vlan_t *vlan_info,

    /*! User data of callback. */
    void *user_data
);

/*!
 * \brief Update all VLAN information to L2 station.
 *
 * This function is used to update all VLAN information to L2 station, and it is
 * only called by VLAN or L2 STATION during init.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_l2_station_update_all(int unit,
                                     bcmi_ltsw_vlan_l2_station_update_cb cb,
                                     void *user_data);

/*!
 * \brief Callback function to update the VLAN information in L3 station.
 */
typedef int (*bcmi_ltsw_vlan_l3_intf_update_cb) (
    /*! Unit Number. */
    int unit,

    /*! VFI ID. */
    int vid,

    /*! VLAN information. */
    bcmi_ltsw_l3_intf_vfi_t *vfi_info,

    /*! User data of callback. */
    void *user_data
);

/*!
 * \brief Update all VLAN information to L3 interface.
 *
 * This function is used to update all VLAN information to L3 interface, and it
 * is only called by L3 INTF module during init.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_l3_intf_update_all(int unit,
                                  bcmi_ltsw_vlan_l3_intf_update_cb cb,
                                  void *user_data);

/*!
 * \brief Add a port into default vlan.
 *
 * This function is used to add a port into default vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_port_attach(int unit, bcm_port_t port);

/*!
 * \brief Remove a port into default vlan.
 *
 * This function is used to remove a port into default vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_port_detach(int unit, bcm_port_t port);

/*!
 * \brief Set the state of VLAN membership check on a given port.
 *
 * This function is used to set the state of VLAN membership check on a given
 * port.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 * \param [in] disable           Disable or enable.
 * \param [in] flags             Direction flags.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_port_check_set(int unit, bcm_port_t port,
                              int disable, uint16_t flags);

/*!
 * \brief Delete VLAN profile entry per index.
 *
 * This function is used to delete VLAN profile entry per index.
 *
 * \param [in] unit              Unit number.
 * \param [in] profile_hdl       Profile handle.
 * \param [in] index             Profile entry index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_profile_delete(int unit,
                              bcmi_ltsw_profile_hdl_t profile_hdl,
                              int index);

/*!
 * \brief Display VLAN software structure information.
 *
 * This function is used to display VLAN software structure information.
 *
 * \retval None
 */
extern void
bcmi_ltsw_vlan_sw_dump(int unit);

/*!
 * \brief Check if VLAN exists.
 *
 * This function is used to check if VLAN exists.
 *
 * \param [in]   unit                 Unit Number.
 * \param [in]   vid                  VLAN ID.
 * \param [out]  exist                Exist.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_exist_check(int unit, bcm_vlan_t vid, int *exist);

/*!
 * \brief Retrieves the number of VLAN created in the system.
 *
 * This function is used to retrieves the number of VLAN created in the system.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  Vlan Count.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_vlan_count_get(int unit, int *arg);

/*!
 * \brief Get port bitmap of default VLAN.
 *
 * This function can only be called during init to get pbmp of default VLAN.
 *
 * \param [in]   unit               Unit Number.
 * \param [in]   pbmp               Port bitmap.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_vlan_def_pbmp_get(int unit, bcm_pbmp_t *pbmp);

/*!
 * \brief Get port bitmap of reserved default VLAN flooding group.
 *
 * This function is used to get port pbmp of default VLAN flooding group.
 *
 * \param [in]   unit               Unit Number.
 * \param [in]   pbmp               Port bitmap.
 *
 * \retval SHR_E_NONE               No errors.
 * \retval !SHR_E_NONE              Failure.
 */
extern int
bcmi_ltsw_vlan_def_flood_port_get(int unit, bcm_pbmp_t *pbmp);

/*!
 * \brief Update VLAN membership when port stack mode changes.
 *
 * This function is used to update VLAN member when port stack mode changes.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  flags                Flags.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_vlan_stk_update(int unit, uint32_t flags);

/*!
 * \brief Update VLAN range index when trunk member changes.
 *
 * This function is used to update VLAN range index when trunk member changes.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  gport                Original trunk member gport.
 * \param [in]  num_joining          Number of joining ports in trunk.
 * \param [in]  joining_arr          Joining ports in trunk.
 * \param [in]  num_leaving          Number of leaving ports in trunk.
 * \param [in]  leaving_arr          Leaving ports in trunk.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_vlan_range_update(int unit, bcm_gport_t gport,
                            int num_joining, bcm_gport_t *joining_arr,
                            int num_leaving, bcm_gport_t *leaving_arr);

/*!
 * \brief Set ingress vlan qos id.
 *
 * This function is used to set ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_ing_qos_set(int unit, bcm_vlan_t vid, int id);

/*!
 * \brief Get ingress vlan qos id.
 *
 * This function is used to get ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_ing_qos_get(int unit, bcm_vlan_t vid, int *id);

/*!
 * \brief Set egress vlan qos id.
 *
 * This function is used to set egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_egr_qos_set(int unit, bcm_vlan_t vid, int id);

/*!
 * \brief Get egress vlan qos id.
 *
 * This function is used to get egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_egr_qos_get(int unit, bcm_vlan_t vid, int *id);

/*!
 * \brief Set vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value to set the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_control_set(int unit,
                           bcmi_ltsw_vlan_control_t type,
                           int value);

/*!
 * \brief Get vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value pointer to get the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_control_get(int unit,
                           bcmi_ltsw_vlan_control_t type,
                           int *value);

/*!
 * \brief Update vlan xlate pipe sw info.
 *
 * \param [in] unit              Unit number.
 * \param [in] pg                Port group.
 * \param [in] ingress           Ingress/egress.
 * \param [in] pipe              Pipe.
 * \param [in] op                Op code: Insert/Delete.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_xlate_pipe_update(int unit, bcm_port_t pg, int ingress,
                                 int pipe, bcmi_ltsw_vlan_table_opcode_t op);

/*!
 * \brief Set STG into a specified VLAN entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID.
 * \param [in] ingress   Ingress or egress.
 * \param [in] stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
bcmi_ltsw_vlan_stg_set(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t stg);

/*!
 * \brief Get STG from a specified VLAN entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID.
 * \param [in] ingress   Ingress or egress.
 * \param [out]stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
bcmi_ltsw_vlan_stg_get(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t *stg);

/*!
 * \brief Notify VLAN membership check information for L2 tunnel case.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN ID.
 * \param [in] add       Add or Delete.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
extern int
bcmi_ltsw_vlan_check_info_notify(int unit, bcm_vlan_t vid, int add);

/*!
 * \brief Check if egr xlate pipe mode is supported.
 *
 * \param [in] unit              Unit number.
 * \param [in] support           Support or not.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_xlate_egr_pipe_check(int unit, int *support);

/*!
 * \brief Get default vlan membership ports profile id.
 *
 * This function is used to get default vlan membership ports profile id.
 *
 * \param [in] unit              Unit number.
 * \param [in] type             Default vlan membership ports profile type.
 * \param [out] profile_id    Default vlan membership ports profile id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_mshp_default_profile_get(int unit,
                                        bcmi_ltsw_profile_hdl_t type,
                                        int *profile_id);

/*!
 * \brief Traverse vlan table to get needed info.
 *
 * This function is used to traverse vlan table without lock.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
bcmi_ltsw_vlan_traverse(int unit,
                        bcmi_ltsw_vlan_traverse_cb cb, void *user_data);
#endif /* BCM_LTSW_VLAN_H */
