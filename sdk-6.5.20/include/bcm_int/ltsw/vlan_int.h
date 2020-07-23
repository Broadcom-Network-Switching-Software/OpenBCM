/*! \file vlan_int.h
 *
 * VLAN internal management header file.
 * This file contains the management for VLAN module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_VLAN_INT_H
#define BCMI_LTSW_VLAN_INT_H

#include <sal/sal_types.h>

/* EGR_VFI table view. */
typedef enum egr_vfi_view_e {
    EGR_VFI_VIEW_DEFAULT = 0,
    EGR_VFI_VIEW_VSID= 1,
    EGR_VFI_VIEW_CNT
} egr_vfi_view_t;

/* Miss action type. */
typedef enum vlan_miss_action_type_e {
    VLAN_MISS_ACTION_TYPE_FLOOD = 0,
    VLAN_MISS_ACTION_TYPE_DROP = 1,
    VLAN_MISS_ACTION_TYPE_CNT
} vlan_miss_action_type_t;

/* Source Private VLAN port type. */
typedef enum pvlan_port_type_e {
    PVLAN_PORT_TYPE_PROMISCUOUS = 0,
    PVLAN_PORT_TYPE_COMMUNITY = 1,
    PVLAN_PORT_TYPE_ISOLATED = 2,
    PVLAN_PORT_TYPE_CNT
} pvlan_port_type_t;

/*!
 * \brief Get scalar value from symbol.
 *
 * \param [in] unit Unit Number.
 * \param [in] symbol symbol String
 * \param [in] val scalar value
 */
typedef int (*vlan_symbol_to_scalar_f)(
    int unit,
    const char *symbol,
    uint64_t *val);

/*!
 * \brief Get symbol from scalar value.
 *
 * \param [in] unit Unit Number.
 * \param [in] val scalar value
 * \param [in] symbol symbol String
 */
typedef int (*vlan_scalar_to_symbol_f)(
    int unit,
    uint64_t val,
    const char **symbol);

/*!
 * \brief HA subcomponent id for VLAN module.
 */
#define BCMINT_VLAN_SUB_COMP_ID_HA_CFG         0
#define BCMINT_VLAN_SUB_COMP_ID_XLATE_KEY      1
#define BCMINT_VLAN_SUB_COMP_ID_MSHIP          2

/*!
 * \brief VLAN LT field description.
 */
typedef struct bcmint_vlan_fld_s {

    /*! LT field name. */
    const char *name;

     /*! Callback to get scalar value from symbol. */
    vlan_symbol_to_scalar_f symbol_to_scalar;

    /*! Callback to get symbol from scalar value. */
    vlan_scalar_to_symbol_f scalar_to_symbol;

} bcmint_vlan_fld_t;

/*!
 * \brief VLAN LT description.
 */
typedef struct bcmint_vlan_lt_s {

    /*! LT name. */
    const char *name;

    /*! Bitmap of valid fields. */
    uint32_t fld_bmp;

    /*! LT fields. */
    const bcmint_vlan_fld_t *flds;

} bcmint_vlan_lt_t;

/*!
 * \brief VLAN LT field description.
 */
typedef struct bcmint_vlan_lt_db_s {

    /*! LT bitmap. */
    uint32_t lt_bmp;

    /*! LT array. */
    const bcmint_vlan_lt_t *lts;

} bcmint_vlan_lt_db_t;

/*!
 * \brief VLAN LTs.
 */
typedef enum bcmint_vlan_lt_id_s {

    /*! LT ING_VLAN_XLATE_1_TABLE. */
    BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE = 0,

    /*! LT ING_VLAN_XLATE_2_TABLE. */
    BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE = 1,

    /*! LT ING_VLAN_XLATE_3_TABLE. */
    BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE = 2,

    /*! LT EGR_VLAN_TRANSLATION. */
    BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION = 3,

    /*! LT ING_VFI_TABLE. */
    BCMINT_LTSW_VLAN_ING_VFI_TABLE = 4,

    /*! LT EGR_VFI. */
    BCMINT_LTSW_VLAN_EGR_VFI = 5,

    /*! LT R_VLAN_XLATE_MISS_POLICY. */
    BCMINT_LTSW_VLAN_R_VLAN_XLATE_MISS_POLICY = 6,

    /*! LT R_EGR_VLAN_XLATE_MISS_POLICY. */
    BCMINT_LTSW_VLAN_R_EGR_VLAN_XLATE_MISS_POLICY = 7,

    /*! LT ING_VLAN_XLATE_1_TABLE_PIPE_CONFIG. */
    BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE_PIPE_CONFIG = 8,

    /*! LT ING_VLAN_XLATE_2_TABLE_PIPE_CONFIG. */
    BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE_PIPE_CONFIG = 9,

    /*! LT ING_VLAN_XLATE_3_TABLE_PIPE_CONFIG. */
    BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE_PIPE_CONFIG = 10,

    /*! LT EGR_VLAN_TRANSLATION_PIPE_CONFIG. */
    BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_PIPE_CONFIG = 11,

    /*! LT EGR_VLAN_TRANSLATION_DW. */
    BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW = 12,

    /*! LT EGR_VLAN_TRANSLATION_DW_PIPE_CONFIG. */
    BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW_PIPE_CONFIG = 13,

    /*! VLAN LT count. */
    BCMINT_LTSW_VLAN_LT_CNT

} bcmint_vlan_lt_id_t;

/*
 * \brief VLAN xlate table id.
 */
typedef enum vlan_xlate_table_id_e{
    /* ING_VLAN_XLATE_1_TABLE. */
    XLATE_TABLE_ING_1 = 0,

    /* ING_VLAN_XLATE_2_TABLE. */
    XLATE_TABLE_ING_2 = 1,

    /* ING_VLAN_XLATE_3_TABLE. */
    XLATE_TABLE_ING_3 = 2,

    /* EGR_VLAN_TRANSLATION. */
    XLATE_TABLE_EGR = 3,

    /* EGR_VLAN_TRANSLATION_DW. */
    XLATE_TABLE_EGR_DW = 4,

    /* COUNT. */
    XLATE_TABLE_CNT = 5
} vlan_xlate_table_id_t;

/*!
 * \brief ING_VLAN_XLATE_X_TABLEs LT fields.
 */
typedef enum bcmint_vlan_fld_ing_vlan_xlate_table_s {
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_OVID = 0,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_IVID = 1,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID = 2,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_STRENGTH_PROFILE_INDEX = 3,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF_STRENGTH = 4,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI_STRENGTH = 5,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF = 6,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF = 7,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_SVP = 8,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI = 9,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_VLAN_TAG_PRESERVE_CTRL = 10,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID = 11,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID_1 = 12,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_PIPE = 13,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_FLEX_CTR_ACTION = 14,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPQ_OBJ_0 = 15,
    BCMINT_LTSW_VLAN_FLD_ING_XLATE_FLD_CNT = 16
} bcmint_vlan_fld_ing_vlan_xlate_1_table_t;

/*!
 * \brief EGR_VLAN_TRANSLATION LT fields.
 */
typedef enum bcmint_vlan_fld_egr_vlan_translation_s {
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VFI = 0,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_GROUP_ID = 1,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_0 = 2,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_1 = 3,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_TAG_ACTION = 4,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_DOT1P_REMARK_BASE_PTR = 5,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_STRENGTH_PRFL_IDX = 6,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_OPAQ_CTRL = 7,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_PIPE = 8,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_ACTION = 9,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_INDEX = 10,
    BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLD_CNT = 11
} bcmint_vlan_fld_egr_vlan_translation_t;

/*!
 * \brief ING_VFI_TABLE LT fields.
 */
typedef enum bcmint_vlan_fld_ing_vfi_table_s {
    BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI = 0,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_MAPPED_VFI = 1,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION = 2,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION_TYPE = 3,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_L3_IIF = 4,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_FLEX_CTR_ACTION = 5,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_STRENGTH_PROFILE_INDEX = 6,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_SPANNING_TREE_GROUP = 7,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_MEMBERSHIP_PROFILE_PTR = 8,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_BITMAP_PROFILE_INDEX = 9,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_EXPECTED_OUTER_TPID_BITMAP = 10,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_OPAQUE_CTRL_ID = 11,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_SRC_PVLAN_PORT_TYPE = 12,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV4MC_L2_ENABLE = 13,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV6MC_L2_ENABLE = 14,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_L2MC_MISS_ACTION = 15,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_L2UC_MISS_ACTION = 16,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_DO_NOT_LEARN = 17,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_L2_HOST_NARROW_LOOKUP_ENABLE = 18,
    BCMINT_LTSW_VLAN_FLD_ING_VFI_FLD_CNT = 19
} bcmint_vlan_fld_ing_vfi_table_t;

/*!
 * \brief EGR_VFI LT fields.
 */
typedef enum bcmint_vlan_fld_egr_vfi_s {
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI = 0,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_0 = 1,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_1 = 2,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_TAG_ACTION = 3,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_UNTAG_PROFILE_INDEX = 4,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_STG = 5,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_MEMBERSHIP_PROFILE_IDX = 6,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_FLEX_CTR_ACTION = 7,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_EFP_CTRL_ID = 8,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_CLASS_ID = 9,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_VIEW_T = 10,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_VSID = 11,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_STRENGTH_PRFL_IDX = 12,
    BCMINT_LTSW_VLAN_FLD_EGR_VFI_FLD_CNT = 13
} bcmint_vlan_fld_egr_vfi_t;

/*!
 * \brief R_VLAN_XLATE_MISS_POLICY LT fields.
 */
typedef enum bcmint_vlan_fld_vt_miss_policy_s {
    BCMINT_LTSW_VLAN_FLD_VT_MISS_POLICY_VT_CTRL = 0,
    BCMINT_LTSW_VLAN_FLD_VT_MISS_POLICY_FLD_CNT
} bcmint_vlan_fld_ing_vt_miss_policy_t;

/*!
 * \brief R_EGR_VLAN_XLATE_MISS_POLICY LT fields.
 */
typedef enum bcmint_vlan_fld_egr_vt_miss_policy_s {
    BCMINT_LTSW_VLAN_FLD_EGR_VT_MISS_POLICY_VT_CTRL = 0,
    BCMINT_LTSW_VLAN_FLD_EGR_VT_MISS_POLICY_FLD_CNT
} bcmint_vlan_fld_egr_vt_miss_policy_t;

/*!
 * \brief VLAN_XLATE_TABLE_PIPE_CONFIG LT fields.
 */
typedef enum bcmint_vlan_fld_vlan_xlate_table_pipe_config_s {
    BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_PIPEUNIQUE = 0,
    BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_CNT
} bcmint_vlan_fld_vlan_xlate_table_pipe_config_t;


/*!
 * \brief Get maximum number of outer TPID.
 *
 * This function is used to get maximum number of outer TPID only for
 * VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  max_num              Max TPID Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmint_vlan_otpid_max_num(int unit, int *max_num);

/*!
 * \brief Set outer TPID value.
 *
 * This function is used to set outer TPID value only for VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] index                Outer TPID entry index.
 * \param [in]  tpid                 Outer TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmint_vlan_otpid_info_tpid_set(int unit, int index, uint16_t tpid);

/*!
 * \brief Add reference count of outer TPID.
 *
 * This function is used to set reference count of outer TPID only for
 * VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] index                Outer TPID entry index.
 * \param [in]  count                Reference count.

 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmint_vlan_otpid_info_ref_count_add(int unit, int index, int count);

/*!
 * \brief Get maximum number of payload outer TPID.
 *
 * This function is used to get maximum number of payload outer TPID only for
 * VLAN module.
 *
 * \param [in]  unit                      Unit Number.
 * \param [in]  max_num              Max TPID Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmint_vlan_payload_otpid_max_num(int unit, int *max_num);

/*!
 * \brief Set payload outer TPID value.
 *
 * This function is used to set payload outer TPID value only for VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] index              Payload outer TPID entry index.
 * \param [in]  tpid                 Payload outer TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmint_vlan_payload_otpid_info_tpid_set(int unit, int index, uint16_t tpid);

/*!
 * \brief Add reference count of payload outer TPID.
 *
 * This function is used to set reference count of payload outer TPID only for
 * VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] index               Payload outer TPID entry index.
 * \param [in]  count                Reference count.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmint_vlan_payload_otpid_info_ref_count_add(int unit, int index, int count);

/*!
 * \brief Set defualt VLAN information.
 *
 * This function is used to set default vlan into VLAN information.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  def_vlan             Defualt VLAN.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmint_vlan_info_default_vlan_set(int unit, bcm_vlan_t def_vlan);

/*!
 * \brief Get VLAN auto stack flag.
 *
 * This function is used to get VLAN auto stack flag.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval VLAN auto stack flag.
 */
extern int
bcmint_vlan_info_auto_stack_get(int unit);

/*!
 * \brief Get port type in default VLAN.
 *
 * This function is used to get port type in default VLAN.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval Port type in default VLAN.
 */
extern int
bcmint_vlan_info_default_port_type_get(int unit);

/*!
 * \brief Check if the VLAN is pre-configured.
 *
 * This function is used to check if the VLAN is pre-configured.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID.
 *
 * \retval Zero                      Not pre-configured VLAN.
 * \retval None-zero                 Pre-configured VLAN.
 */
extern int
bcmint_vlan_info_pre_cfg_get(int unit, bcm_vlan_t vid);

/*!
 * \brief Set the bitmap for pre-configured VLAN.
 *
 * This function is used to set the bitmap for pre-configured VLAN.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID.
 *
 * \retval None
 */
extern void
bcmint_vlan_info_pre_cfg_set(int unit, bcm_vlan_t vid);

/*!
 * \brief Check if VLAN exists.
 *
 * This function is used to check if VLAN exists.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID.
 *
 * \retval Zero                      VLAN doesn't exist.
 * \retval None-zero                 VLAN exists.
 */
extern int
bcmint_vlan_info_exist_check(int unit, bcm_vlan_t vid);

/*!
 * \brief Set the bitmap of existing VLANs.
 *
 * This function is used to set the bitmap of existing VLANs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID.
 *
 * \retval None
 */
extern void
bcmint_vlan_info_set(int unit, bcm_vlan_t vid);

/*!
 * \brief Set the vlan entry init state
 *
 * This function is used to set the vlan entry init state.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  inited               Init state.
 *
 * \retval None
 */
extern void
bcmint_vlan_entry_init_state_set(int unit, uint8_t inited);

/*!
 * \brief Get the vlan entry init state
 *
 * This function is used to get the vlan entry init state.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval Zero                      VLAN entry is not inited.
 * \retval None-zero                 VLAN enty is inited.
 */
extern int
bcmint_vlan_entry_init_state_get(int unit);

/*!
 * \brief Get per npl  LT info.
 *
 * This function is used to get vlan lt info based on NPL version.
 *
 * \param [in]    unit                 Unit Number.
 * \param [in]    lt_id                VLAN LT ID.
 * \param [out]   lt_info              LT info.
 *
 * \retval None
 */
extern int
bcmint_vlan_lt_get(int unit,
                   bcmint_vlan_lt_id_t lt_id,
                   const bcmint_vlan_lt_t **lt_info);

/*!
 * \brief Check if vlan xlate tables share same pt resource.
 *
 * This function is used to check if vlan xlate tables share same pt resource.
 *
 * \param [in]   unit                 Unit Number.
 * \param [in]   lt_id                VLAN LT ID.
 * \param [out]  share                share.
 *
 * \retval None
 */
extern int
bcmint_vlan_xlate_table_share(int unit,
                              vlan_xlate_table_id_t lt_id, int *share);
#endif /* BCMI_LTSW_VLAN_INT_H */
