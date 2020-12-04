/*! \file bcmfp_group_internal.h
 *
 * Defines, Enums and Structures to represent FP groups
 * related information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_GROUP_INTERNAL_H
#define BCMFP_GROUP_INTERNAL_H

#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_ext_internal.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_pmux_internal.h>
#include <bcmfp/bcmfp_qual_cfg_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmfp/bcmfp_idp_internal.h>

/*! Group resides in a single slice. */
#define BCMFP_GROUP_SPAN_SINGLE_SLICE          (1 << 0)

/*! Group resides in a two paired slices. */
#define BCMFP_GROUP_SPAN_DOUBLE_SLICE          (1 << 1)

/*! Group resides in three paired slices. */
#define BCMFP_GROUP_SPAN_TRIPLE_SLICE          (1 << 2)

/*! Group entries are double wide in each slice. */
#define BCMFP_GROUP_INTRASLICE_DOUBLEWIDE      (1 << 3)

/*! GROUP with default policy */
#define BCMFP_GROUP_WITH_DEFAULT_POLICY        (1 << 4)

/*! GROUP with default presel */
#define BCMFP_GROUP_WITH_DEFAULT_PRESEL        (1 << 5)

/*! GROUP with PDD Profile */
#define BCMFP_GROUP_WITH_PDD_PROFILE           (1 << 6)

/*! GROUP with SBR Profile */
#define BCMFP_GROUP_WITH_SBR_PROFILE           (1 << 7)

/*! GROUP with Dynamic Presel */
#define BCMFP_GROUP_WITH_DYNAMIC_PRESEL        (1 << 8)

/*! GROUP with auto-expanion */
#define BCMFP_GROUP_WITH_AUTO_EXPAND           (1 << 9)

/*! BCMFP_GROUP_INSTALL_XXX flags are used to install
 *  or uninstall different profiles of FP group when
 *  the first entry in FP group is created and last
 *  entry in FP group is deleted respectively.Profiles
 *  to install vary from FP stage to FP stage and also
 *  depends on information provided in FP group by the
 *  application.
 */
/*! Install keygen profiles of FP group in HW. */
#define BCMFP_GROUP_INSTALL_KEYGEN_PROFILE   (0x1 << 0)

/*! Install PDD profiles of FP group in HW. */
#define BCMFP_GROUP_INSTALL_PDD_PROFILE      (0x1 << 1)

/*! Install Default PDD profiles of FP group in HW. */
#define BCMFP_GROUP_INSTALL_DPDD_PROFILE     (0x1 << 2)

/*! Install QOS profiles of FP group in HW. */
#define BCMFP_GROUP_INSTALL_QOS_PROFILE      (0x1 << 3)

/*! Install default QOS profiles of FP group in HW. */
#define BCMFP_GROUP_INSTALL_DQOS_PROFILE     (0x1 << 4)

/*! Install default policy of FP group in HW. */
#define BCMFP_GROUP_INSTALL_DPOLICY          (0x1 << 5)

/*! Install default policy of FP group in HW. */
#define BCMFP_GROUP_INSTALL_KEY_ATTRIB_PROFILE  (0x1 << 6)

/*! Install SBR profiles of FP group in HW. */
#define BCMFP_GROUP_INSTALL_SBR_PROFILE      (0x1 << 7)

/*! Install default SBR profiles of FP group in HW. */
#define BCMFP_GROUP_INSTALL_DSBR_PROFILE     (0x1 << 8)

/*! Install Presel group profile in HW. */
#define BCMFP_GROUP_INSTALL_PSG_PROFILE      (0x1 << 9)

/*! BCMFP group mode string */
#define BCMFP_GROUP_MODE_STRINGS \
{ \
    "HALF", \
    "SINGLE", \
    "SINGLE_ASET_NARROW", \
    "SINGLE_ASET_WIDE", \
    "DBLINTRA", \
    "DBLINTER", \
    "TRIPLE", \
    "QUAD", \
    "AUTO" \
    "INVALID" \
}

/*! Group Mode Bitmap */
typedef struct bcmfp_group_mode_bmp_s {
    /*! bitmap of group modes */
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_GROUP_MODE_COUNT)];
} bcmfp_group_mode_bmp_t;


/*! BCMFP group slice mode string */
#define BCMFP_SLICE_MODE_STRINGS \
{ \
    "L2_SINGLE_WIDE", \
    "L3_SINGLE_WIDE", \
    "L3_DOUBLE_WIDE", \
    "L3_ANY_SINGLE_WIDE", \
    "L3_ANY_DOUBLE_WIDE", \
    "L3_ALT_DOUBLE_WIDE", \
    "INVALID", \
}

/*!
 * Typedef:
 *      bcmfp_group_type_t
 * Purpose:
 *      Port and Pkt type provided
 *      in grp_template configuration
 */
typedef enum bcmfp_group_type_e {

    /*! Don't Care. */
    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_ANY         = 0,

    /*! Packet type IPV4, Port Type Don't Care. */
    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4        = 1,

    /*! Packet type IPV6, Port Type Don't Care. */
    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6        = 2,

    /*! Packet type IP, Port Type Don't Care. */
    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IP          = 3,

    /*! Packet type NonIP, Port Type Don't Care. */
    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP       = 4,

    /*! Packet type Don't Care, Port Type HiGiG. */
    BCMFP_GROUP_TYPE_PORT_HIGIG_PACKET_ANY       = 5,

    /*! Packet type Don't Care, Port Type Front Panel. */
    BCMFP_GROUP_TYPE_PORT_FRONT_PACKET_ANY       = 6,

    /*! Packet type Don't Care, Port Type Loopback. */
    BCMFP_GROUP_TYPE_PORT_LOOPBACK_PACKET_ANY    = 7,

    /*! Last in the enum, shouldn't be used. */
    BCMFP_GROUP_TYPE_COUNT                       = 8

} bcmfp_group_type_t;

/*! BCMFP group type strings */
#define BCMFP_GROUP_TYPE_STRINGS \
{ \
    "PORT_ANY_PACKET_ANY", \
    "PORT_ANY_PACKET_IPV4", \
    "PORT_ANY_PACKET_IPV6", \
    "PORT_ANY_PACKET_IP", \
    "PORT_ANY_PACKET_NONIP", \
    "PORT_HIGIG_PACKET_ANY", \
    "PORT_FRONT_PACKET_ANY", \
    "INVALID" \
}

/*!
 * Typedef:
 *     bcmfp_qual_offset_info_t
 * Purpose:
 *     Qualifier offsets in FP tcam.
 */
typedef struct bcmfp_qual_offset_info_s {

    /*! Number of Offsets.      */
    uint8_t       num_offsets;

    /*! Qualifier offset.       */
    uint16_t      offset[BCMFP_QUAL_OFFSETS_MAX];

    /*! Qualifier offset width. */
    uint8_t       width[BCMFP_QUAL_OFFSETS_MAX];

} bcmfp_qual_offset_info_t;

/*!
 * Typedef:
 *     bcmfp_group_qual_offset_info_s
 * Purpose:
 *     Field entry part qualifiers information.
 */
typedef struct bcmfp_group_qual_offset_info_s {

    /*! Qualifier ID array. */
    bcmfp_qualifier_t *qid_arr;

    /*! LT field ID array. */
    bcmltd_fid_t *fid_arr;

    /*! Array LT field ID index array. */
    uint16_t *fidx_arr;

    /*! Offset information array. */
    bcmfp_qual_offset_info_t *offset_arr;

    /*! Size of all the above arrays. */
    uint16_t size;

} bcmfp_group_qual_offset_info_t;

/*! Max group action resolution ids */
#define BCMFP_GROUP_ACTION_RES_IDS_MAX 32

/*! Max Group SBR Ids */
#define BCMFP_GROUP_SBR_IDS_MAX 32

/*! When LTID is set to this value, the LTID
 * is assigned by the driver internally,
 * else the ltid provided by the user is used.
 */
#define BCMFP_GROUP_AUTO_LTID (BCMFP_GROUP_ACTION_RES_IDS_MAX + 1)

/*! Group Action Resolution ids bitmap */
typedef struct bcmfp_group_action_res_id_bmp_s {
    /*! Group Action Resolution ids bitmap -
     *  Action res Id is used in resolving conflicting actions
     *  between groups
     */
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_GROUP_ACTION_RES_IDS_MAX)];
} bcmfp_group_action_res_id_bmp_t;

/*! Group SBR Id bitmap */
typedef struct bcmfp_group_sbr_id_bmp_s {
    /*! Group SBR Id bitmap */
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_GROUP_SBR_IDS_MAX)];
} bcmfp_group_sbr_id_bmp_t;

/*! Default presel group id. Cannot be used by the application. */
#define BCMFP_DEFAULT_PRESEL_GROUP_ID 0

/*!
 * Group configuration compiled from
 * FP_XXX_GRP_TEMPLATE LT entry.
 */
typedef struct bcmfp_group_cfg_s {

    /*! Group flags BCMFP_GROUP_XXX. */
    uint32_t flags;

    /*! Groups unique, with in a stage, id. */
    bcmfp_group_id_t group_id;

    /*! BCMFP stage to which group_id belongs to. */
    uint32_t stage_id;

    /*!
     * Required bits in a qualifier. This is a
     * linked list and each node in the list is
     * for one qualifier that is part of groups
     * QSET.
     */
    bcmfp_qual_bitmap_t *qual_bitmaps;

    /*! Number of nodes in the qual_bitmaps list */
    uint16_t num_qual_bitmaps;

} bcmfp_group_cfg_t;

/*! Group Structure */
typedef struct bcmfp_group_s {

    /*! Group Flags */
    uint32_t flags;

    /*! Logical Table Id. */
    bcmltd_sid_t ltid;

    /*! Stage id to which the group belongs. */
    uint32_t stage_id;

    /*! Group Id */
    bcmfp_group_id_t group_id;

    /*! Group Mode - SINGLE/DOUBLE/TRIPLE etc. */
    bcmfp_group_mode_t group_mode;

    /*! Group Slice Mode */
    bcmfp_group_slice_mode_t group_slice_mode;

    /*! Group's Port and Pkt type provided
     * in the LT entry. */
    bcmfp_group_type_t  group_port_pkt_type;

    /*! Group's Priority */
    uint32_t group_prio;

    /*! Size of key field */
    uint32_t key_size;

    /*! Size of data field */
    uint32_t data_size;

   /*! Action resolution Id used in resolving conflicting actions b/w groups.
    *  Each action_res_id will be assigned one unique priority.
    */
    uint8_t action_res_id;

    /*! SBR Id of the SBR Profile created for the group. */
    uint8_t sbr_id;

    /*! SBR LT template Id used by the group */
    uint32_t sbr_template_id;

    /*! Number of presels used by the group */
    uint8_t num_presel_ids;

    /*! Pipe in which the group is to be installed.
     * -1 indicated the group operated un global mode.
     */
    int pipe_id;

    /*! Slice Id assigned to the group. */
    uint8_t lookup_id;

    /*! Group's qualifier set. */
    bcmfp_qset_t qset;

    /*! Structure which captures the properties
     * of qualifiers like bitmap,qual id etc.
     */
    bcmfp_qual_bitmap_t *qual_bitmap;

    /*! Extractors(in use) Information in all parts of a group. */
    bcmfp_ext_codes_t ext_codes[BCMFP_ENTRY_PARTS_MAX];

    /*! Arr of Offset and width information of all the qualifiers in
     * different parts with in the group. Number of offset, width pairs
     * of a qualifier depends on the keygen algorithm.
     */
    bcmfp_group_qual_offset_info_t qual_arr[BCMFP_ENTRY_PARTS_MAX];

    /*! Compression enabled/disabled indicator */
    bool compress;

     /*! Types of compressions expected to be applied to
      *  entries associated to this group.
      */
    bool compress_types[BCMFP_COMPRESS_TYPES_MAX];

    /*! Group Hint */
    bcmfp_group_slice_type_t group_slice_type;

} bcmfp_group_t;

/*! Used to Convert the FP Group Mode to Group Mode
 * used by the PTM.
 */
#define BCMFP_GROUP_MODE_TO_BCMPTM_GROUP_MODE(bcmfp_group_mode,               \
                                              bcmptm_group_mode)              \
     do {                                                                     \
         if (bcmfp_group_mode == BCMFP_GROUP_MODE_HALF) {                     \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_HALF;                \
         } else if (bcmfp_group_mode == BCMFP_GROUP_MODE_SINGLE ||            \
             bcmfp_group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW ||       \
             bcmfp_group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {         \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;              \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_DBLINTRA) {           \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA;            \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_DBLINTER) {           \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_DBLINTER;            \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_TRIPLE) {             \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_TRIPLE;              \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_QUAD) {               \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_QUAD;                \
         } else {                                                             \
             SHR_ERR_EXIT(SHR_E_UNAVAIL);                              \
         }                                                                    \
     } while (0)

/*! Used to Convert the EM group mode to the group mode used
 * by PTM.
 */
#define BCMFP_EM_GROUP_MODE_TO_BCMPTM_EM_GROUP_MODE(group_oper,                     \
                                              bcmptm_group_mode)                    \
     do {                                                                           \
         if (group_oper->flex_mode == true) {                                       \
             bcmptm_group_mode = BCMPTM_RM_HASH_ENTRY_MODE_FLEX;                    \
         } else if (group_oper->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW) {\
             bcmptm_group_mode = BCMPTM_RM_HASH_ENTRY_MODE_EM_160;                  \
         } else if (group_oper->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {  \
             bcmptm_group_mode = BCMPTM_RM_HASH_ENTRY_MODE_EM_128;                  \
         } else if (group_oper->group_mode == BCMFP_GROUP_MODE_DBLINTER) {          \
             bcmptm_group_mode = BCMPTM_RM_HASH_ENTRY_MODE_EM_320;                  \
         } else {                                                                   \
             SHR_ERR_EXIT(SHR_E_UNAVAIL);                                    \
         }                                                                          \
      } while (0)


/*!
 * \brief Check whether group template id is associated to any
 *  entry id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] group_template_id Index to FP_XXX_GROUP_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = group template id not mapped to entry id(s).
 *             FLASE = group template id mapped to entry id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_group_entry_map_check(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t group_template_id,
                            bool *not_mapped);
/*!
 * \brief Add entry id to given group template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] group_template_id Index to FP_XXX_GROUP_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_group_entry_map_add(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t group_template_id,
                          uint32_t entry_id);

/*!
 * \brief Delete entry id from given group template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] group_template_id Index to FP_XXX_GROUP_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_group_entry_map_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t group_template_id,
                             uint32_t entry_id);

/*!
 * \brief Loop through all entry ids associated to a given
 *  group template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] group_template_id Index to FP_XXX_GROUP_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_group_entry_map_traverse(int unit,
                               bcmfp_stage_id_t stage_id,
                               uint32_t group_template_id,
                               bcmfp_entry_id_traverse_cb cb,
                               void *user_data);

/*!
 * \brief Loop through all entry ids associated to a given
 *  group template id till required condition is met.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] group_template_id Index to FP_XXX_GROUP_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 * \param [in] cond_met status to indicate traverse stop when set to TRUE.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_group_entry_map_cond_traverse(int unit,
                                    bcmfp_stage_id_t stage_id,
                                    uint32_t group_template_id,
                                    bcmfp_entry_id_cond_traverse_cb cb,
                                    void *user_data,
                                    bool *cond_met);


#endif /* BCMFP_GROUP_INTERNAL_H */
