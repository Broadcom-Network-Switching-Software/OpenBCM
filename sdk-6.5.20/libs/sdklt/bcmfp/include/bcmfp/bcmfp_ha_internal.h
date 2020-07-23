/*! \file bcmfp_ha_internal.h
 *
 * Data structures stored in HA area for BCMFP component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_HA_INTERNAL_H
#define BCMFP_HA_INTERNAL_H

#include <bcmfp/bcmfp_trans_internal.h>

/*! First version number for BCMFP HA blocks. */
#define BCMFP_HA_VERSION_1 0x1

/*! Current version number for BCMFP HA blocks. */
#define BCMFP_HA_STRUCT_VERSION BCMFP_HA_VERSION_1

/*! Signature for BCMFP HA blocks. */
#define BCMFP_HA_STRUCT_SIGN 0x23D61000

/*! Minimum valid block id for BCMFP HA blocks. */
#define BCMFP_HA_BLK_ID_MIN 0x0

/*! Maximum valid block id for BCMFP HA blocks. */
#define BCMFP_HA_BLK_ID_MAX 0xFF

/*! Minimum valid block id for BCMFP fixed HA blocks.
 *  Fixed HA blocks are created during system initialization.
 */
#define BCMFP_HA_BLK_ID_FIXED_MIN  0x4

/*! Maximum valid block id for BCMFP fixed HA blocks.
 *  Fixed HA blocks are created during system initialization.
 */
#define BCMFP_HA_BLK_ID_FIXED_MAX  0x1F

/*! Minimum valid block id for BCMFP dynamic HA blocks.
 *  Dynamic HA blocks are created during run time.
 */
#define BCMFP_HA_BLK_ID_DYNAMIC_MIN 0x80

/*! Maximum valid block id for BCMFP dynamic HA blocks.
 *  Dynamic HA blocks are created during run time.
 */
#define BCMFP_HA_BLK_ID_DYNAMIC_MAX 0xFD

/*! Invalid HA block ID. Used in HA block id validations. */
#define BCMFP_HA_BLK_ID_INVALID    0x0

/*! Since block offset is uint32_t type the max offset is ((2^32) -1).
 *  This macros is used to identify invalid HA block offset.
 */
#define BCMFP_HA_BLK_OFFSET_INVALID 0xFFFFFFFF

/*! Number of HA elements at each HA blk offset is one. */
#define BCMFP_HA_BLK_BUCKET_SIZE_ONE 0x1

/*! Number of HA elements at each HA blk offset is two. */
#define BCMFP_HA_BLK_BUCKET_SIZE_TWO 0x2

/*!
 * Number of HA elements in generic TV block. Different
 * HA block types are defined in bcmfp_ha_blk_type_t.
 */
#define BCMFP_HA_GENERIC_TV_BLK_ELEMENT_COUNT (8 * 1024)

/*!
 * Number of HA elements in HA block of type PDD_ACTION_INFO.
 * Different HA block types are defined in bcmfp_ha_blk_type_t.
 */
#define BCMFP_HA_PDD_ACTION_INFO_BLK_ELEMENT_COUNT 64

/*!
 * Number of HA elements in HA block of type CKEY. Different
 * HA block types are defined in bcmfp_ha_blk_type_t.
 */
#define BCMFP_HA_CKEY_BLK_ELEMENT_COUNT (6 * 1024)

/*!
 * Number of HA elements in HA block of type FID offset info.
 * Different HA block types are defined in bcmfp_ha_blk_type_t.
 */
#define BCMFP_HA_FID_OFFSET_INFO_BLK_ELEMENT_COUNT (1 * 1024)

/*! Reserved BCMFP HA block IDs for each stage. */
typedef enum bcmfp_ha_blk_id_e {
    BCMFP_HA_BLK_ID_ROOT = 2,
    /*! HA block id for ingress stage(IFP). */
    BCMFP_HA_BLK_ID_INGRESS = 4,
    /*! HA block id for egress stage(EFP). */
    BCMFP_HA_BLK_ID_EGRESS = 6,
    /*! HA block id for vlan stage(VFP). */
    BCMFP_HA_BLK_ID_VLAN = 8,
    /*! HA block id for exact match stage(EM). */
    BCMFP_HA_BLK_ID_EM = 10,
    /*! HA block id for flow tracker stage(FT). */
    BCMFP_HA_BLK_ID_FLOW_TRACKER = 12,
} bcmfp_ha_blk_id_t;

/*!
 * Offset to different structures saved in HA from
 * start address of the ha_mem.
 */
typedef struct bcmfp_ha_struct_offset_s {
   /*! offset to bcmfp_ha_blk_hdr_t. */
   uint32_t ha_blk_hdr_offset;

   /*! offset to bcmfp_stage_oper_info_t. */
   uint32_t stage_oper_info_offset;

   /*! Number of groups */
   uint16_t num_groups;

   /*! offset to bcmfp_group_oper_info_t. */
   uint32_t group_oper_info_offset;

   /*! Number of PDD profiles. */
   uint16_t num_pdd_profiles;

   /*! offset to bcmfp_pdd_oper_info_t. */
   uint32_t pdd_oper_info_offset;

   /*! offset to bcmfp_ha_blk_info_t. */
   uint32_t ha_blk_info_offset;

   /*! Size of ha_blk_info array in HA block. */
   uint32_t ha_blk_info_array_size;

    /*! offset to bcmfp_ref_count_t. */
   uint32_t ref_count_offset;

   /*! Size of ref_count array in HA block. */
   uint32_t ref_count_array_size;

   /*! Number of presel groups. */
   uint16_t num_presel_groups;

   /*! Offset to presel group operational info. */
   uint32_t presel_group_oper_info_offset;

   /*! Number of entries in keygen profile table. */
   uint16_t num_keygen_prof;

   /*! Offset to keygen profile reference count. */
   uint32_t keygen_prof_ref_count_offset;

   /*! Number of entries in action profile table. */
   uint16_t num_action_prof;

   /*! Offset to action profile reference count. */
   uint32_t action_prof_ref_count_offset;

   /*! Number of entries in QOS profile table. */
   uint16_t num_qos_prof;

   /*! Offset to QOS profile reference count. */
   uint32_t qos_prof_ref_count_offset;

   /*! Number of entries in EM key attributes profile table. */
   uint16_t num_em_key_attrib_profile;

   /*! Offset to EM key attributes  profile reference count. */
   uint32_t em_key_attrib_prof_ref_count_offset;

   /*! Number of entries in SBR profile table. */
   uint16_t num_sbr_profiles;

   /*! Offset to SBR profile reference count. */
   uint32_t sbr_prof_ref_count_offset;

   /*! Number of entries in PSG profile table. */
   uint16_t num_psg_profiles;

   /*! Offset to PSG profile reference count. */
   uint32_t psg_prof_ref_count_offset;

   /*! Offset to default SBR ID to group ID map segment */
   uint32_t dsbr_group_map_offset;

   /*!
    * Number of HA blk info elements in default SBR ID to
    * group ID map segment.
    */
   uint32_t dsbr_group_map_size;

   /*! Offset to default PDD ID to group ID map segment */
   uint32_t dpdd_group_map_offset;

   /*!
    * Number of HA blk info elements in default PDD ID to
    * group ID map segment.
    */
   uint32_t dpdd_group_map_size;

   /*! Offset to default policy ID to group ID map segment */
   uint32_t dpolicy_group_map_offset;

   /*!
    * Number of HA blk info elements in default policy ID to
    * group ID map segment.
    */
   uint32_t dpolicy_group_map_size;

   /*! Offset to presel entry ID to group ID map segment */
   uint32_t pse_group_map_offset;

   /*!
    * Number of HA blk info elements in presel entry ID to
    * group ID map segment.
    */
   uint32_t pse_group_map_size;

   /*! Offset to presel entry ID to group ID map segment */
   uint32_t psg_group_map_offset;

   /*!
    * Number of HA blk info elements in presel group ID to
    * group ID map segment.
    */
   uint32_t psg_group_map_size;

   /*! Offset to presel entry ID to group ID map segment */
   uint32_t sbr_group_map_offset;

   /*!
    * Number of HA blk info elements in SBR ID to group ID
    * map segment.
    */
   uint32_t sbr_group_map_size;

   /*! Offset to presel group ID to group ID map segment */
   uint32_t pdd_group_map_offset;

   /*!
    * Number of HA blk info elements in PDD ID to group ID
    * map segment.
    */
   uint32_t pdd_group_map_size;


   /*! Offset to group ID to entry ID map segment */
   uint32_t group_entry_map_offset;

   /*!
    * Number of HA blk info elements in group ID to entry ID
    * map segment.
    */
   uint32_t group_entry_map_size;

   /*! Offset to rule ID to entry ID map segment */
   uint32_t rule_entry_map_offset;

   /*!
    * Number of HA blk info elements in rule ID to entry ID
    * map segment.
    */
   uint32_t rule_entry_map_size;

   /*! Offset to policy ID to entry ID map segment */
   uint32_t policy_entry_map_offset;

   /*!
    * Number of HA blk info elements in policy ID to entry ID
    * map segment.
    */
   uint32_t policy_entry_map_size;


   /*! Offset to meter ID to entry ID map segment */
   uint32_t meter_entry_map_offset;

   /*!
    * Number of HA blk info elements in meter ID to entry ID
    * map segment.
    */
   uint32_t meter_entry_map_size;


   /*! Offset to SBR ID to entry ID map segment */
   uint32_t sbr_entry_map_offset;

   /*!
    * Number of HA blk info elements in SBR ID to entry ID
    * map segment.
    */
   uint32_t sbr_entry_map_size;


   /*! Offset to PDD ID to entry ID map segment */
   uint32_t pdd_entry_map_offset;

   /*!
    * Number of counter IDs in legacy stype FP dedicated counters
    * logical table.
    */
   uint16_t num_ctr_ids;

   /*!
    * Number of HA blk info elements in PDD ID to entry ID
    * map segment.
    */
   uint32_t pdd_entry_map_size;

   /*! Offset to counter ID to entry ID map segment */
   uint32_t ctr_entry_map_offset;

   /*!
    * Number of HA blk info elements in counter ID to entry ID
    * map segment.
    */
   uint32_t ctr_entry_map_size;

   /*! Offset to counter hardware index bitmap */
   uint32_t ctr_hw_index_bitmap_offset;

   /*! Size of counter hardware index bitmap */
   uint32_t ctr_hw_index_bitmap_size;

   /*! Offset to counter LT ID to harware index map */
   uint32_t ctr_id_to_hw_index_map_offset;

   /*! Size of counter LT ID to harware index map */
   uint32_t ctr_id_to_hw_index_map_size;

   /*! Offset to counter LT ID bitmap */
   uint32_t ctr_egr_stats_id_bmp_offset;

   /*! Size of counter LT ID bitmap */
   uint32_t ctr_egr_stats_id_bmp_size;

   /*! Offset to counter LT ID to byte count map */
   uint32_t ctr_egr_stat_byte_count_map_offset;

   /*! Size of counter LT ID to byte count map */
   uint32_t ctr_egr_stat_byte_count_map_size;

   /*! Offset to counter LT ID to packet count map */
   uint32_t ctr_egr_stat_pkt_count_map_offset;

   /*! Size of counter LT ID to packet count map */
   uint32_t ctr_egr_stat_pkt_count_map_size;

} bcmfp_ha_struct_offset_t;


/*! Macro to check if the HA block id is fixed(created during
 *  FP component initialization time) or not.
 */
#define BCMFP_HA_BLK_ID_IS_FIXED(blk_id)       \
     ((blk_id >= BCMFP_HA_BLK_ID_FIXED_MIN) && \
      (blk_id <= BCMFP_HA_BLK_ID_FIXED_MAX))

/*! Macro to check if the HA block id is dynamic(created during
 *  FP component run time) or not.
 */
#define BCMFP_HA_BLK_ID_IS_DYNAMIC(blk_id)       \
     ((blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN) && \
      (blk_id <= BCMFP_HA_BLK_ID_DYNAMIC_MAX))

/*! Macro to get the HA block id reserved for a given stage_id. */
#define BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id)         \
    do {                                                      \
        if (stage_id == BCMFP_STAGE_ID_INGRESS) {             \
           blk_id = BCMFP_HA_BLK_ID_INGRESS;                  \
        } else if (stage_id == BCMFP_STAGE_ID_EGRESS) {       \
           blk_id = BCMFP_HA_BLK_ID_EGRESS;                   \
        } else if (stage_id == BCMFP_STAGE_ID_LOOKUP) {       \
           blk_id = BCMFP_HA_BLK_ID_VLAN;                     \
        } else if (stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {  \
           blk_id = BCMFP_HA_BLK_ID_EM;                       \
        } else if (stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) { \
           blk_id = BCMFP_HA_BLK_ID_FLOW_TRACKER;             \
        } else {                                              \
           SHR_ERR_EXIT(SHR_E_INTERNAL);                      \
        }                                                     \
    } while(0)

/*! Macro to get the stage_id from HA block ID. */
#define BCMFP_HA_BLK_ID_TO_STAGE_ID(blk_id, stage_id)         \
    do {                                                      \
        if (blk_id == BCMFP_HA_BLK_ID_INGRESS ||              \
            blk_id == BCMFP_HA_BLK_ID_INGRESS + 1) {          \
            stage_id = BCMFP_STAGE_ID_INGRESS;                \
        } else if (blk_id == BCMFP_HA_BLK_ID_EGRESS ||        \
             blk_id == BCMFP_HA_BLK_ID_EGRESS + 1) {          \
            stage_id = BCMFP_STAGE_ID_EGRESS;                 \
        } else if (blk_id == BCMFP_HA_BLK_ID_VLAN ||          \
             blk_id == BCMFP_HA_BLK_ID_VLAN + 1) {            \
            stage_id = BCMFP_STAGE_ID_LOOKUP;                 \
        } else if (blk_id == BCMFP_HA_BLK_ID_EM ||            \
             blk_id == BCMFP_HA_BLK_ID_EM + 1) {              \
            stage_id = BCMFP_STAGE_ID_EXACT_MATCH;            \
        } else if (blk_id == BCMFP_HA_BLK_ID_FLOW_TRACKER ||  \
             blk_id == BCMFP_HA_BLK_ID_FLOW_TRACKER + 1) {    \
            stage_id = BCMFP_STAGE_ID_FLOW_TRACKER;           \
        } else {                                              \
           SHR_ERR_EXIT(SHR_E_INTERNAL);                      \
        }                                                     \
    } while(0)


/*! Macro to validate the HA block Id. This will check whether
 *  HA block id is with in the min, max limits of valid block Ids.
 */
#define BCMFP_HA_BLK_ID_VALIDATE(unit, blk_id)    \
    do {                                          \
        SHR_IF_ERR_EXIT(                          \
            (blk_id == BCMFP_HA_BLK_ID_INVALID) ? \
                   SHR_E_INTERNAL : SHR_E_NONE);  \
        SHR_IF_ERR_EXIT(                          \
            (blk_id > BCMFP_HA_BLK_ID_MAX) ?      \
                  SHR_E_INTERNAL : SHR_E_NONE);   \
    } while(0)

/*! Macros to validate the HA block offset. It does following checks
 *- 1. blk_offset is not same as BCMFP_HA_BLK_OFFSET_INVALID.
 *- 2. blk_offset is not more than block size.
 *- 3. blk_offset is not multiple of size of bcmfp_generic_data_t.
 *- This macro doesnot validate blk_id. So before using this macro
 *  BCMFP_HA_BLK_ID_VALIDATE should be preceeded for validating the
 *  block id.
 */
#define BCMFP_HA_BLK_OFFSET_VALIDATE(unit, blk_id, blk_offset) \
    do {                                                       \
        void *_ha_ptr = NULL;                                  \
        uint32_t _size = 0;                                    \
        bcmfp_ha_blk_hdr_t *_blk_hdr = NULL;                   \
        SHR_IF_ERR_EXIT(                                       \
            (blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) ?      \
                                SHR_E_INTERNAL: SHR_E_NONE);   \
        if (((blk_offset - sizeof(bcmfp_ha_blk_hdr_t)) %       \
             sizeof(bcmfp_generic_data_t)) != 0) {             \
            SHR_ERR_EXIT(SHR_E_INTERNAL);                      \
        }                                                      \
        _ha_ptr = BCMFP_HA_PTR(unit, blk_id);                  \
        if (_ha_ptr == NULL) {                                 \
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);                     \
        }                                                      \
        _blk_hdr = (bcmfp_ha_blk_hdr_t *)_ha_ptr;              \
        SHR_IF_ERR_EXIT(                                       \
            (blk_offset > _blk_hdr->blk_size) ?                \
                     SHR_E_INTERNAL : SHR_E_NONE);             \
        _size = blk_offset + sizeof(bcmfp_generic_data_t);     \
        SHR_IF_ERR_EXIT(                                       \
            (_size > _blk_hdr->blk_size) ?                     \
               SHR_E_INTERNAL : SHR_E_NONE);                   \
    } while(0)

/*!
 * \brief Allocate and Initialize the HA memory(HA blocks)
 *  required for BCMFP during system initialization time.
 *
 * \param [in] unit Logical device id
 * \param [in] warm Falg indicating cold/warm boot
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int bcmfp_ha_init(int unit, bool warm);
/*!
 * \brief De allocate the HA memory(HA blocks) allocated for
 *  BCMFP during system initialization time and run time.
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int bcmfp_ha_cleanup(int unit);

/*!
 * \brief Get the pointer to the HA memory for the given
 *  HA block ID.
 *
 * \param [in] unit Logical device id
 * \param [in] sub_id HA block id.
 * \param [out] ha_mem Pointer to HA block.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_mem_get(int unit,
                 uint8_t sub_id,
                 void **ha_mem);
/*!
 * \brief Set the pointer to the HA memory for the given
 *  HA block ID.
 *
 * \param [in] unit Logical device id
 * \param [in] sub_id HA block id.
 * \param [in] ha_mem Pointer to HA block.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_mem_set(int unit,
                 uint8_t sub_id,
                 void *ha_mem);
/*!
 * \brief Get the pointer to the HA element in the given
 *  HA block id at the specified offset. This function can
 *  be used only if HA block is created run time.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id.
 * \param [in] blk_offset HA block offset.
 * \param [in] ha_element Pointer to HA element.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_fid_offset_info_get(int unit,
                          uint8_t blk_id,
                          uint32_t blk_offset,
                          bcmfp_fid_offset_info_t **fid_offset_info);

/*!
 * \brief Acquire the required number of ha elements from the
 *  free pool. Each HA block(run time created) maintains linked
 *  list of free HA elements in it. While acquiring the free HA
 *  elements, following sequence of steps will be executed.
 *  - Go through all existing HA blocks(run time created) until
 *    required number of HA elements are acquired.
 *  - If free HA elements in all existing HA blocks(run time created),
 *    Then create a new HA block and repeat above step.
 *  - While creating a new HA block in the above step, if no block ids
 *    are left free(Each HA block will be assigned one unique blocj id),
 *    then fail.
 *
 * \param [in] unit Logical device id
 * \param [in] req_ha_elements Required number of HA elements.
 * \param [in] bucket_size Number of HA elemenst at each blk offset.
 * \param [out] ha_elements Pointer to linked list of acquired HA elements.
 * \param [out] first_ha_element_blk_id HA block id of first HA element
 *              in the linked list.
 * \param [out] first_ha_element_blk_offset HA block offset of first
 *              HA element in the linked list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_fid_offset_infos_acquire(int unit,
                               uint16_t req_ha_elements,
                               bcmfp_fid_offset_info_t **ha_elements,
                               uint8_t *first_ha_element_blk_id,
                               uint32_t *first_ha_element_blk_offset);

/*!
 * \brief Release the linked list of HA elements(belongs to different
 *  block ids) back to free HA elements of its parent HA block id.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id of first HA element in linked list.
 * \param [in] blk_offset HA block offset first HA element in linked list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_fid_offset_infos_release(int unit,
                               uint8_t blk_id,
                               uint32_t blk_offset);
/*!
 * \brief Get the pointer to the HA element in the given
 *  HA block id at the specified offset. This function can
 *  be used only if HA block is created run time.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id.
 * \param [in] blk_offset HA block offset.
 * \param [in] ha_element Pointer to HA element.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_element_get(int unit,
               uint8_t blk_id,
               uint32_t blk_offset,
               bcmfp_generic_data_t **ha_element);

/*!
 * \brief Acquire the required number of ha elements from the
 *  free pool. Each HA block(run time created) maintains linked
 *  list of free HA elements in it. While acquiring the free HA
 *  elements, following sequence of steps will be executed.
 *  - Go through all existing HA blocks(run time created) until
 *    required number of HA elements are acquired.
 *  - If free HA elements in all existing HA blocks(run time created),
 *    Then create a new HA block and repeat above step.
 *  - While creating a new HA block in the above step, if no block ids
 *    are left free(Each HA block will be assigned one unique blocj id),
 *    then fail.
 *
 * \param [in] unit Logical device id
 * \param [in] req_ha_elements Required number of HA elements.
 * \param [in] bucket_size Number of HA elemenst at each blk offset.
 * \param [out] ha_elements Pointer to linked list of acquired HA elements.
 * \param [out] first_ha_element_blk_id HA block id of first HA element
 *              in the linked list.
 * \param [out] first_ha_element_blk_offset HA block offset of first
 *              HA element in the linked list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_elements_acquire(int unit,
                          uint16_t req_ha_elements,
                          uint32_t bucket_size,
                          bcmfp_generic_data_t **ha_elements,
                          uint8_t *first_ha_element_blk_id,
                          uint32_t *first_ha_element_blk_offset);

/*!
 * \brief Release the linked list of HA elements(belongs to different
 *  block ids) back to free HA elements of its parent HA block id.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id of first HA element in linked list.
 * \param [in] blk_offset HA block offset first HA element in linked list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_elements_release(int unit,
                          uint8_t blk_id,
                          uint32_t blk_offset);
/*!
 * \brief Get the header of an HA block.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id.
 * \param [out] blk_hdr HA block header.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_blk_hdr_get(int unit,
             uint8_t sub_id,
             bcmfp_ha_blk_hdr_t **blk_hdr);

/*!
 * \brief Get the pointer to Fp root HA block.
 *
 * \param [in] unit Logical device id
 * \param [out] root Pointer to FP root HA block.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_root_blk_get(int unit,
                      bcmfp_ha_root_blk_t **root);

/*!
 * \brief Get the backup HA block ID for the given Ha block id.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id.
 * \param [out] _backup_blk_id Backup HA block id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_backup_blk_id_get(int unit,
                           uint8_t blk_id,
                           uint8_t *backup_blk_id);

extern int
bcmfp_ha_free_blk_id_get(int unit, uint8_t *blk_id);

/*!
 * \brief Get the total size of stage operational info HA block of a
 *  stage. This function also keeps track of all individual segements
 *  sizes and those segments offsets in the HA block. This information
 *  is used when HA blocks are intialized(bcmfp_stage_oper_info_init_cb)
 *  or when HA block is updated(bcmfp_stage_oper_info_init_wb).
 *
 * \param [in] unit         Logical device id
 * \param [in] stage_id     BCMFP stage ID.
 * \param [out] size        Total size of the HA block.
 * \param [out] issu_offset Offset and size info of different
 *                          segments in HA block.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_stage_oper_size_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *size,
                             bcmfp_ha_struct_offset_t *issu_offset);
/*!
 * \brief Report offsets of different segemenst in HA block to ISSU.
 *
 * \param [in] unit         Logical device id
 * \param [in] stage_id     BCMFP stage ID.
 * \param [out] blk_id      HA block ID.
 * \param [out] issu_offset Offset and size info of different
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_ha_block_report(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint8_t blk_id,
                            bcmfp_ha_struct_offset_t *issu_offset);
#endif /* BCMFP_HA_INTERNAL_H */
