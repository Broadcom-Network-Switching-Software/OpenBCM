/*! \file bcmcth_phb_ing_map.h
 *
 * This file contains PHB ing map handler function declarations and
 * macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PHB_ING_MAP_H
#define BCMCTH_PHB_ING_MAP_H

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <shr/shr_bitop.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd_config.h>

/*!
 * PT data entry size: 104 bits
 * LT data entry size: 13 bits
 * Maximum bytes count to hold 13 bits is 3 bytes
 */

/*! Logical table entry length. */
#define PHB_LT_DATA_ENTRY_LEN    13

/*! Mask for 13 bits. */
#define MASK_13_BITS             0x1fff

/*! 8 bits per byte. */
#define BITS_PER_BYTE            8

/*! HA component ID. */
#define PHB_SW_MGMT_COMP_ID      0

/*! HA resource manager ID. */
#define PHB_RESOURCE_MGR_COMP_ID 1

/*! HA entry manager ID. */
#define PHB_ENTRY_MGR_COMP_ID    2

/*! Maximum profile number per map type. */
#define PHB_ING_MAX_PROFILE_PER_TYPE   256

/*! Total supported profile number. */
#define PHB_ING_TOTAL_PROFILE (PHB_ING_MAX_PROFILE_PER_TYPE*PHB_ING_MAX_PROFILE_TYPE)

/*! To derive base_index automatically. */
#define AUTO_MODE                1

/*! To derive base_index manually. */
#define MANUAL_MODE              0

/*! Entry is in use. */
#define IN_USE                   1

/*! Entry is not used. */
#define NOT_IN_USE               0

/*! Total entry per profile for L2 map table. */
#define PHB_ING_L2_PROFILE_ENTRY_SIZE   16

/*! Total entry per profile for L3 map table. */
#define PHB_ING_L3_PROFILE_ENTRY_SIZE   64

/*! Total entry per profile for MPLS map table. */
#define PHB_ING_MPLS_PROFILE_ENTRY_SIZE 8

/*! PHB map type supported. */
typedef enum phb_ing_profile_type_t {
    /*! L2 profile. */
    PHB_ING_PROFILE_TYPE_L2=0,

    /*! L3 profile. */
    PHB_ING_PROFILE_TYPE_L3=1,

    /*! MPLS profile. */
    PHB_ING_PROFILE_TYPE_MPLS=2,

    /*! Type terminator. */
    PHB_ING_MAX_PROFILE_TYPE=3
} phb_ing_profile_type_t;

/*! Software control structure. */
#define PHB_SW_CONTROL(unit) phb_sw_control[unit]

/*!
 * \brief Structure to manage resource usage.
 */
typedef struct resource_bit_map_s {
    /*! Total entry managed. */
    uint32_t total_entry;

    /*! Bit map holder. */
    SHR_BITDCL *usage_bitmap;
} resource_bit_map_t;

/*!
 * \brief Structure to manage profile usage.
 */
typedef struct phb_ing_profile_s {
    /*
     * Bits 0 -   7: Profile index.
     * Bits 8 -  13: Profile type.
     * Bits 14 - 14: Profile is used.
     * Bits 15 - 15: Mode - Auto index or not.
     * Bits 16 - 31: base_index value
     */
    /*! Profile ID. */
    uint32_t  p_id         :    8;

    /*! Profile type. */
    uint32_t  p_type       :    6;

    /*! This profile entry is used. */
    uint32_t  p_used       :    1;

    /*! Auto mode tracker. */
    uint32_t  p_auto_mode  :    1;

    /*! Base_index assigned to this profile. */
    uint32_t  p_base_index :   16;
} phb_ing_profile_t;

/*!
 * \brief Structure to manage software resource.
 */
typedef struct phb_sw_control_s {
   /*! Total profile used. */
   uint32_t total_used;

   /*! Resource Menager to track chunk usage */
   resource_bit_map_t resource_mgr;

   /*! Entry manager to track entry usage within a chunk */
   resource_bit_map_t entry_mgr;

   /*! Profile usage info */
   phb_ing_profile_t profile_info[PHB_ING_TOTAL_PROFILE];
} phb_sw_control_t;

/*!
 * \brief Key to access l2 map table.
 */
typedef struct l2_dot1p_key_s {
    /*! Key packet CFI. */
    unsigned int cfi: 1;

    /*! Key packet PRI. */
    unsigned int pkt_pri: 3;
} l2_dot1p_key_t;

/*!
 * \brief Key to access l2 map table.
 */
typedef union l2_index_key_t {
    /*! Key in union format. */
    l2_dot1p_key_t sub_key;

    /*! Key in int format. */
    unsigned int key;
} l2_index_key_t;

/*!
 * \brief Key to access l3 map table.
 */
typedef struct l3_dscp_key_s {
    /*! Key packet IP DSCP. */
    unsigned int dscp: 6;
} l3_dscp_key_t;

/*!
 * \brief Key to access l3 map table.
 */
typedef union l3_index_key_t {
    /*! Key in union format. */
    l3_dscp_key_t sub_key;

    /*! key in int format */
    unsigned int key;
} l3_index_key_t;

/*!
 * \brief Key to access mpls map table.
 */
typedef struct mpls_exp_key_s {
    /*! key packet mpls exp. */
    unsigned int exp: 3;
} mpls_exp_key_t;

/*!
 * \brief Key to access mpls map table.
 */
typedef union mpls_index_key_t {
    /*! Key in union format. */
    mpls_exp_key_t sub_key;

    /*! Key in int format. */
    unsigned int key;
} mpls_index_key_t;

/*!
 * \brief Data field to hold map entry.
 */
typedef struct phb_ing_map_sub_entry_s {
    /*! Data field internal priority. */
    uint32_t int_pri: 4;

    /*! Data field color. */
    uint32_t cng: 2;

    /*! Data field dscp is valid. */
    uint32_t dscp_valid: 1;

    /*! Data field dscp. */
    uint32_t dscp: 6;

    /*! Reserved. */
    uint32_t reserved: 19;
} phb_ing_map_sub_entry_t;

/*!
 * \brief Data field to hold map entry.
 */
typedef union phb_ing_map_entry_t {
    /*! Data field in union format. */
    phb_ing_map_sub_entry_t sub_entry;

    /*! Data field in int format. */
    uint32_t entry;
} phb_ing_map_entry_t;

/*!
 * \brief Data structure to hold l2 lt table data.
 */
typedef struct phb_ing_l2_lt_s {
    /*! Key from packet for l2 map. */
    l2_index_key_t key_pkt;

    /*! Key from profile id. */
    uint16_t key_pid;

    /*! Data field read back from hardware. */
    phb_ing_map_entry_t hw_field;

    /*! Data field of indexing mode. */
    uint32_t auto_mode;

    /*! Data field of base_index. */
    uint32_t base_index;

    /*! Data field of auto created base_index. */
    uint32_t base_index_oper;
} phb_ing_l2_lt_t;

/*!
 * \brief Data structure to hold l3 lt table data.
 */
typedef struct phb_ing_l3_lt_s {
    /*! Key from packet for l3 map. */
    l3_index_key_t key_pkt;

    /*! Key from profile id. */
    uint16_t key_pid;

    /*! Data field read back from hardware. */
    phb_ing_map_entry_t hw_field;

    /*! Data field of indexing mode. */
    uint32_t auto_mode;

    /*! Data field of base_index. */
    uint32_t base_index;

    /*! Data field of auto created base_index. */
    uint32_t base_index_oper;
} phb_ing_l3_lt_t;

/*!
 * \brief Data structure to hold mpls lt table data.
 */
typedef struct phb_ing_mpls_t_s {
    /*! Key from packet for mpls map. */
    mpls_index_key_t key_pkt;

    /*! Key from profile id. */
    uint16_t key_pid;

    /*! Data field read back from hardware. */
    phb_ing_map_entry_t hw_field;

    /*! Data field of indexing mode. */
    uint32_t auto_mode;

    /*! Data field of base_index. */
    uint32_t base_index;

    /*! Data field of auto created base_index. */
    uint32_t base_index_oper;
} phb_ing_mpls_lt_t;

/*!
 * \brief Data structure to hold hardware information.
 */
typedef struct bcmcth_phb_ing_map {
    /*! Maximum hardware entries supported. */
    uint16_t max_hw_entries;

    /*! Number sub-entries per hardware entry. */
    uint16_t max_sub_entries;

    /*! Number of totoal profile supported. */
    uint16_t max_profiles;

    /*! Hardware map table ID. */
    uint32_t map_table_sid;

    /*! Hardware entry field ID. */
    uint32_t map_field_fid;
} bcmcth_phb_ing_map_t;

/*!
 * \brief Data structure to hold hardware information.
 */
typedef struct bcmcth_phb_egr_botp_config_s {
    /*! Hardware policy table ID. */
    uint32_t policy_table_sid;

    /*! Hardware policy table entry index. */
    uint32_t policy_table_index;

    /*! Hardware field 0 ID. */
    uint32_t map_field_0_fid;

    /*! Hardware field 0 config data. */
    uint32_t map_field_0_config;

    /*! Hardware field 1 ID. */
    uint32_t map_field_1_fid;

    /*! Hardware field 1 config data. */
    uint32_t map_field_1_config;

    /*! Hardware field 2 ID. */
    uint32_t map_field_2_fid;

    /*! Hardware field 2 config data. */
    uint32_t map_field_2_config;

    /*! Hardware field 3 ID. */
    uint32_t map_field_3_fid;

    /*! Hardware field 3 config data. */
    uint32_t map_field_3_config;

    /*! Hardware field 4 ID. */
    uint32_t map_field_4_fid;

    /*! Hardware field 4 config data. */
    uint32_t map_field_4_config;

    /*! Hardware field 5 ID. */
    uint32_t map_field_5_fid;

    /*! Hardware field 5 config data. */
    uint32_t map_field_5_config;

    /*! Hardware field 6 ID. */
    uint32_t map_field_6_fid;

    /*! Hardware field 6 config data. */
    uint32_t map_field_6_config;
} bcmcth_phb_egr_botp_config_t;

/*! Data pointer for software control structure. */
extern phb_sw_control_t *phb_sw_control[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief PHB map entry PT write.
 *
 * Write PHB map entry into physical table.
 *
 * \param [in] unit               Unit number.
 * \param [in] trans_id           LT  Transaction identifier.
 * \param [in] pt_entry_index     PT table index.
 * \param [in] pt_sub_entry_index PT table sub-entry index.
 * \param [in] pt_sub_entry_data  PT table sub-entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int phb_ing_map_pt_write(int unit,
                     uint32_t trans_id,
                     uint32_t pt_entry_index,
                     uint32_t pt_sub_entry_index,
                     uint32_t pt_sub_entry_data);

/*!
 * \brief PHB map entry PT read.
 *
 * Read PHB map entry from physical table.
 *
 * \param [in]  unit               Unit number.
 * \param [in]  trans_id           LT  Transaction identifier.
 * \param [in]  pt_entry_index     PT table index.
 * \param [in]  pt_sub_entry_index PT table sub-entry index.
 * \param [out] pt_sub_entry_data  PT table sub-entry data read back.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int phb_ing_map_pt_read(int unit,
                               uint32_t trans_id,
                               uint32_t pt_entry_index,
                               uint32_t pt_sub_entry_index,
                               uint32_t *pt_sub_entry_data);

/*!
 * \brief PHB resource(chunk) manager creation.
 *
 * \param [in] resource_mgr    Resource holder.
 * \param [in] total_entry     Total entries managed.
 * \param [in] warm            Is this warmboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */

extern int
bcmcth_phb_resource_mgr_create (resource_bit_map_t *resource_mgr,
                                           uint32_t total_entry,
                                           bool warm);

/*!
 * \brief PHB resource(chunk) manager checks whether an entry is used.
 *
 * \param [in] resource_mgr    Resource holder.
 * \param [in] entry_count     Total count of entries to be reserved.
 * \param [in] base_index      Starting index of current profile.
 * \param [in] auto_mode       To create base_index automatically or not.
 *
 * \retval NOT_IN_USE       Entry is not in use.
 * \retval IN_USE           Entry is in use.
 */
extern int
bcmcth_phb_resource_mgr_allocate (resource_bit_map_t *resource_mgr,
                                  uint32_t entry_count,
                                  uint32_t *base_index,
                                  bool auto_mode);

/*!
 * \brief PHB resource(chunk) manager reserves an chunk.
 *
 * \param [in] resource_mgr    Resource holder.
 * \param [in] base_index      Starting index of current profile.
 * \param [in] entry_count     Total count of entries to be reserved.
 *
 * \retval SHR_E_NONE       No error.
 */
extern int
bcmcth_phb_resource_mgr_reserve (resource_bit_map_t *resource_mgr,
                                 uint32_t base_index,
                                 uint32_t entry_count);

/*!
 * \brief PHB resource(chunk) manager frees a chunk.
 *
 * \param [in] resource_mgr    Resource holder.
 * \param [in] base_index      Starting index of current profile.
 * \param [in] entry_count     Total count of entries to be reserved.
 *
 * \retval SHR_E_NONE       No error.
 */
extern int
bcmcth_phb_resource_mgr_free (resource_bit_map_t *resource_mgr,
                              uint32_t base_index,
                              uint32_t entry_count);


/*!
 * \brief PHB resource(chunk) manager destroy.
 * \param [in] resource_mgr    Resource holder.
 *
 * Use HA memory as place holder - no operation needed.
 */
extern void
bcmcth_phb_resource_mgr_destroy (resource_bit_map_t *resource_mgr);

/*!
 * \brief PHB profile manager to look up a particular profile.
 *
 * \param [in] phb_control    Resource holder.
 * \param [in] p_type         profile_type.
 * \param [in] p_id           Profile ID.
 * \param [out] p_index       Matched profile index in database.
 *
 * \retval SHR_E_NONE       No error.
 */
extern int
bcmcth_phb_profile_lookup_entry(uint32_t p_type,
                         uint32_t p_id,
                         uint32_t *p_index,
                         phb_sw_control_t *phb_control);

/*!
 * \brief PHB profile manager to look for a free profile.
 *
 * \param [in] phb_control    Resource holder.
 * \param [in] p_index        Profile index in database.
 *
 * \retval SHR_E_NONE       No error.
 */
extern int
bcmcth_phb_profile_get_free_entry(uint32_t *p_index,
                                  phb_sw_control_t *phb_control);

/*!
 * \brief PHB profile manager to free one profile spot.
 *
 * \param [in] phb_control    Resource holder.
 * \param [in] p_type         profile_type.
 * \param [in] p_id           Profile ID.
 *
 * \retval SHR_E_NONE       No error.
 */
extern int
bcmcth_phb_profile_delete(uint32_t p_type,
                          uint32_t p_id,
                          phb_sw_control_t *phb_control);

/*!
 * \brief PHB profile manager to update a profile with usage info.
 *
 * \param [in] p_index        Profile index in database.
 * \param [in] p_type         profile type.
 * \param [in] p_id           Profile ID per type.
 * \param [in] p_auto_mode    Profile mode to derive base_index.
 * \param [in] p_base_index   Profile base_index to map to hardware entry.
 * \param [in] phb_control    Resource holder.
 *
 * \retval SHR_E_NONE         No error.
 */
extern int
bcmcth_phb_profile_update_entry(uint32_t p_index,
                                uint32_t p_type,
                                uint32_t p_id,
                                uint32_t p_auto_mode,
                                uint32_t p_base_index,
                                phb_sw_control_t *phb_control);

/*!
 * \brief PHB profile manager to read back base_index from a profile.
 *
 * \param [in] p_index        Profile index in database.
 * \param [out] p_auto_mode   Profile mode to derive base_index.
 * \param [out] p_base_index  Profile base_index to map to hardware entry.
 * \param [in] phb_control    Resource holder.
 *
 * \retval SHR_E_NONE         No error.
 */
extern int
bcmcth_phb_profile_read_entry(uint32_t p_index,
                                uint32_t *p_auto_mode,
                                uint32_t *p_base_index,
                                phb_sw_control_t *phb_control);

/*!
 * \brief PHB entry manager destroy.
 * \param [in] entry_mgr    Resource holder.
 *
 * Use HA memory as place holder - no operation needed.
 */

extern void
bcmcth_phb_entry_mgr_destroy (resource_bit_map_t *entry_mgr);

/*!
 * \brief PHB entry manager frees an entry.
 *
 * \param [in] entry_mgr    Resource holder.
 * \param [in] base_index   Starting index of current profile.
 * \param [in] offset       Offset to an entry in the profile.
 *
 * \retval SHR_E_NONE       No error.
 */
extern int
bcmcth_phb_entry_mgr_free (resource_bit_map_t *entry_mgr,
                           uint32_t base_index,
                           uint32_t offset);

/*!
 * \brief PHB entry manager reserves an entry.
 *
 * \param [in] entry_mgr    Resource holder.
 * \param [in] base_index   Starting index of current profile.
 * \param [in] offset       Offset to an entry in the profile.
 *
 * \retval SHR_E_NONE       No error.
 */
extern int
bcmcth_phb_entry_mgr_reserve (resource_bit_map_t *entry_mgr,
                              uint32_t base_index,
                              uint32_t offset);

/*!
 * \brief PHB entry manager checks whether an entry is used.
 *
 * \param [in] entry_mgr    Resource holder.
 * \param [in] base_index   Starting index of current profile.
 * \param [in] offset       Offset to an entry in the profile.
 *
 * \retval NOT_IN_USE       Entry is not in use.
 * \retval IN_USE           Entry is in use.
 */
extern int
bcmcth_phb_entry_mgr_check (resource_bit_map_t *entry_mgr,
                              uint32_t base_index,
                              uint32_t offset);

/*!
 * \brief PHB entry manager creation.
 *
 * \param [in] entry_mgr    Resource holder.
 * \param [in] total_entry  Total entries managed.
 * \param [in] warm         Is this warmboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_phb_entry_mgr_create (resource_bit_map_t *entry_mgr,
                             uint32_t total_entry,
                             bool warm);


/*!
 * \brief Initialize device-specific data.
 *
 * \param [in] unit Unit nunmber.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_phb_ing_map_init(int unit);

/*!
 * \brief Initialize device-specific data.
 *
 * \param [in] unit Unit nunmber.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_phb_ing_imm_init(int unit);

/*!
 * \brief Initialize PHB s/software data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Cold/Warmboot flag.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_phb_sw_resources_alloc(int unit, bool warm);

/*!
 * \brief Cleanup PHB software data structures.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_phb_sw_resources_free(int unit);

/*!
 * \brief Allocate PHB table field data structures.
 *
 * \param [in] unit          Unit number.
 * \param [in] in            Allocation place holder.
 * \param [in] num_fields    Total table fields.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_phb_table_entry_allocate(int unit, bcmltd_fields_t* in, size_t num_fields);

/*!
 * \brief Cleanup PHB table data structures.
 *
 * \param [in] unit          Unit number.
 * \param [in] in            Allocation place holder.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_phb_table_entry_free(int unit, bcmltd_fields_t* in);

/*!
 * \brief Register IMM call back functions.
 *
 * \param [in] unit          Unit number.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_phb_ing_imm_cb_register(int unit);

#endif /* BCMCTH_PHB_ING_MAP_H */
