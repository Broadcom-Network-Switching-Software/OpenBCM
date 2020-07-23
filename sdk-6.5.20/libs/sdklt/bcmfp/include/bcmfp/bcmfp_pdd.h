/*! \file bcmfp_pdd.h
 *
 * PDD(Policy Data Decoder) data structures and APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_PDD_H
#define BCMFP_PDD_H

#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_action_internal.h>

/* PDD FLAGS */

#define BCMFP_COMPUTE_ONLY_PDD_PARTS_REQUIRED (1 << 0)

/*
 * Maximum number of PDD sections a stage supported
 * across all supported devices.
 */
#define BCMFP_PDD_MAX_SECTIONS  (4)

/*
 * Maximum number of PDD Words a stage supported
 * across all supported devices.
 */
#define BCMFP_MAX_PDD_WORDS     (8)

/*
 * Maximum number of offsets a action can have
 * across all supported devices.
 */
#define BCMFP_ACTION_PARAM_SZ           (6)

/*
 * Maximum number of conflicting actions in single
 * wide mode alone an action can have across all
 * supported devices.
 */
#define BCMFP_ACTION_MAX_SINGLEWIDE_CONFLICT (4)



typedef struct bcmfp_action_oper_info_s bcmfp_action_oper_info_t;
typedef struct bcmfp_pdd_oper_s bcmfp_pdd_oper_t;

/*
 * PDD bitmap is divided in to sections in hw.
 * below arrays give the start and end bits for
 * sections s0, s1 etc.
 */
typedef struct bcmfp_pdd_hw_section_info_s {

    uint8_t num_pdd_sections;

    uint8_t  section_id[BCMFP_PDD_MAX_SECTIONS];

    uint8_t section_start_bit[BCMFP_PDD_MAX_SECTIONS];

    uint8_t section_end_bit[BCMFP_PDD_MAX_SECTIONS];

    /*
     * Valid only when pdd_data_mixed_msb_lsb is TRUE.
     */
    bool section_is_lsb[BCMFP_PDD_MAX_SECTIONS];

    /*
     * If there is any alignment criteria between sections
     * the below field give the alignment boundary needed
     * by the hardware when moving from section to section
     * Valid only when pdd_data_mixed_msb_lsb is TRUE.
     */
    uint8_t section_align_bits[BCMFP_PDD_MAX_SECTIONS];

} bcmfp_pdd_hw_section_info_t;

/*
 * Hardware PDD information.
 */
typedef struct bcmfp_pdd_hw_info_s {

    /*
     * Maximum number of hw parts that can be consumed
     * to accomadate the choosen actions in PDD LT.
     * For eg.
     * PDD LT attached to a FP Quad wide mode group
     * can occupy 4 PDD HW parts.
     */
    uint8_t num_max_pdd_parts;

    /* PDD bitmap size. */
    uint16_t pdd_bitmap_size;

    /* Raw policy offset for PDD data. */
    uint8_t pdd_raw_data_start_offset;

    /* Raw policy data width for PDD data. */
    uint16_t raw_policy_width;

    /*
     * Below boolean vairables specifies the details
     * about the policy data encoding.
     *
     * There can be 4 combinations for a given pdd bitmap to generate
     * pdd data.
     *
     * Say PDD bitmap has 10 containers(c1, c2,.. c10) and PDD DATA is 100 bits.
     * c1 size:6 bits, c10 size:5 bits
     *
     * 1: Read pdd bitmap from LSB and write corresponding PDD data at LSB.
     *      Container 1 data is written at bit 0-5 of PDD DATA
     *
     * 2: Read pdd bitmap from MSB and write corresponding PDD data at LSB.
     *      Container 10 data is written at bit 0-5 of PDD DATA
     *
     * 3: Read pdd bitmap from LSB and write corresponding PDD data ar MSB.
     *      container 1 data written at bit 94-99 of PDD DATA
     *
     * 4: Read pdd bitmap from MSB and write corresponding PDD data ar MSB.
     *      container 10 data written at bit 94-99 of PDD DATA
 */

    /*
     * This bit indicates in which direction PDD BITMAP is read
     * while encoding PDD DATA
     */
    bool read_pdd_bitmap_from_lsb;

    /*
     * This bit indicates in which direction PDD DATA is written to hw.
     * PDD DATA can be encoded/decoded in HW from LSB to MSB or MSB to LSB.
     */
    bool write_pdd_data_from_lsb;

    /*
     * This bit indicates in which direction PDD DATA if formed as mix of MSB and LSB.
     */
    bool pdd_data_mixed_msb_lsb;

    /*
     * If there is any alignment criteria between sections
     * the below field give the alignment boundary needed
     * by the hardware when moving from section to section.
     * Ignored when pdd_data_mixed_msb_lsb is TRUE and
     * Align bits in this case will be from section info.
     */
    uint8_t section_align_bits;

    /* Hw pdd section info */
    bcmfp_pdd_hw_section_info_t *section_hw_info;

} bcmfp_pdd_hw_info_t;

/*
 * PDD bitmap is divided into multiple sections to decode
 * the sections of raw policy data simultaneously. Each
 * section will be assigned a start offset in raw policy data.
 */
typedef struct bcmfp_pdd_section_info_s {

    uint8_t num_sections;

    uint8_t section_id[BCMFP_PDD_MAX_SECTIONS];

    uint8_t section_offset[BCMFP_PDD_MAX_SECTIONS];

} bcmfp_pdd_section_info_t;

/*
 * Action, Action offsets and width information
 * in the PDD raw data.
 */
struct bcmfp_action_oper_info_s {

    bcmfp_action_t action;

    /* Number of valid indexes in offset and width arrays. */
    uint8_t num_offsets;

    /* Offset of action within the PDD raw data */
    uint16_t offset[BCMFP_ACTION_PARAM_SZ];

    /* Action width within the PDD raw data */
    uint16_t width[BCMFP_ACTION_PARAM_SZ];

    bcmfp_action_oper_info_t *next;

};

struct bcmfp_pdd_oper_s {

    /* Container Bitmap for the current part/slice generated */
    uint32_t container_bitmap[BCMFP_MAX_PDD_WORDS];

    /* Section info for the corresponding container bitmap */
    bcmfp_pdd_section_info_t section_info;

    /* Action info for the actions enabled in the container bitmap */
    bcmfp_action_oper_info_t *action_oper_info;

    /*
     * Width of policy data. This is acumulated sum of size of
     * all physical containers, section offsets and raw data offset
     */
    uint32_t data_width;

    /* Pdd oper info for the next part/slice if exists */
    bcmfp_pdd_oper_t *next;
};

/*! Action's Offset information */
typedef struct bcmfp_action_offset_cfg_s {
    /*!
     * Physical container ID. Bit number representing the
     * physical container in the PDD bitmap.
     */
    uint16_t     phy_cont_id[BCMFP_ACTION_PARAM_SZ];

    /*!
     * SBR Physical container ID. Bit number representing the
     * physical container in the SBR bitmap.
     */
    uint16_t     sbr_phy_cont_id[BCMFP_ACTION_PARAM_SZ];

    /*! Offset of action within the physical container */
    uint16_t    offset[BCMFP_ACTION_PARAM_SZ];

    /*! Action width within the physical container  */
    uint8_t     width[BCMFP_ACTION_PARAM_SZ];

    /*! Action Value */
    int         value[BCMFP_ACTION_PARAM_SZ];

} bcmfp_action_offset_cfg_t;

/*! Action is part of FLAT policy(Not PDD based) */
#define BCMFP_ACTIONS_IN_FLAT_POLICY      (0x1 << 0)

/*! Action is part of AUX container or AUX data */
#define BCMFP_ACTION_IN_AUX_CONTAINER     (0x1 << 1)

/*! Action is not present in SBR. */
#define BCMFP_ACTION_SBR_NONE   (0x1 << 2)

/*! Action is internally mapped to SBR. */
#define BCMFP_ACTION_SBR_INTERNAL   (0x1 << 3)

/*! Action is not valid, physical container is not mapped. */
#define BCMFP_ACTION_NOT_VALID   (0x1 << 4)

/*! Action is added as place holder in policy template
 * This is used specifically for meter and counter
 * actions ( as they actions are dervied from the
 * entry LT). This soft action is initialized in action_cfg_db
 * to extract the offsets of the action_data in the
 * policy data. Also with this the 1:1 correspondance
 * between the PDD template actions and the policy
 * template actions will be maintained.
 */
#define BCMFP_ACTION_SOFT (0x1 << 5)

/*
 * Action Data base structures
 */
typedef struct bcmfp_action_cfg_s {

    /* Action flags BCMFP_ACTION_XXX. */
    uint32_t flags;

    const char *action_name;

    /* Action offset information. */
    bcmfp_action_offset_cfg_t offset_cfg;

    /* Next offset information of same action. */
    struct bcmfp_action_cfg_s *next;

    /*! Action view type at this offset */
    bcmfp_action_viewtype_t viewtype;

    /*!
     * Pointer to an array of fids of the actions
     * that are conflicting with the given action
     * only in single wide mode
     */
    uint32_t single_wide_conflict_fid[BCMFP_ACTION_MAX_SINGLEWIDE_CONFLICT];

} bcmfp_action_cfg_t;

typedef struct bcmfp_action_cfg_db_s {

    /* Total number of actions supported */
    uint32_t num_actions;

    /* Total number of containers supported */
    uint16_t num_containers;

    /* Action configutation for each action supported */
    bcmfp_action_cfg_t *action_cfg[BCMFP_ACTIONS_COUNT];

    /*
     * Array of container, container size mapping.
     * Index refers to container ID.
     */
    uint16_t *container_size;

} bcmfp_action_cfg_db_t;

typedef struct bcmfp_pdd_cfg_s {

    /* PDD flags BCMFP_PDD_XXX. */
    uint32_t flags;

    bcmfp_action_cfg_db_t  *action_cfg_db;

    bcmfp_pdd_hw_info_t *pdd_hw_info;

    uint16_t raw_data_offset;

    uint32_t num_actions;

    bcmfp_action_t *pdd_action;

} bcmfp_pdd_cfg_t;

/*!
 * \brief Calcualte the least possible group mode to accomodate the
 *  actions added in PDD profile LT. Also update the part container
 *  bitmap and section offsets to the PDD oper info corresponding LT entry.
 *
 * \param [in] unit Logical device id
 * \param [in] PDD Config Info
 * \param [out] Number of parts needed to accomodate all PDD LT entry actions
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */

extern int
bcmfp_pdd_oper_num_parts_get(int unit,
                                  bcmfp_pdd_cfg_t *pdd_cfg_info,
                                  uint8_t *num_parts);

/*!
 * \brief Calcualte Container bitmap, Section Offsets,
 *  and the final bus offsets for the actions added
 *  in PDD LT entry. Also update the action offsets to the
 *  PDD oper info corresponding to PDD LT entry.
 *
 * \param [in] unit Logical device id
 * \param [in] PDD Config Info
 * \param [out] PDD oper info
 * \param [out] Number of PDD parts required.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 *
 * NOTE: Memory for pdd_oper_info is allocated in the
 * below function. Caller function is responsible for
 * freeing the memory after their use of it. Below fucntion
 * needs to be called for freeing the memory used for pdd_oper_info.
 * Function: bcmfp_pdd_oper_info_free
 */
extern int
bcmfp_pdd_cfg_process(int unit,
                      bcmfp_pdd_cfg_t *pdd_cfg_info,
                      bcmfp_pdd_oper_t **pdd_oper_info,
                      uint8_t *num_parts);
/*!
 * \brief Free the memory allocated for
 *  PDD oper info.
 *
 * \param [in] unit Logical device id
 * \param [in] Number PDD parts.
 * \param [in] PDD oper info
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_oper_info_free(int unit,
                         uint8_t num_parts,
                         bcmfp_pdd_oper_t *pdd_operational_info);
#endif /* BCMFP_PDD_H */
