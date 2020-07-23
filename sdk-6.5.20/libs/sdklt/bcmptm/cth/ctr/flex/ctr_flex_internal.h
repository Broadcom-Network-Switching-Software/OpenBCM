/*! \file ctr_flex_internal.h
 *
 * This file contains defines which are internal to ctr_flex.c CTH.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef CTR_FLEX_INTERNAL_H
#define CTR_FLEX_INTERNAL_H

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_ctr_flex_internal.h>
#include <bcmimm/bcmimm_int_comp.h>

#define BSL_LOG_MODULE BSL_LS_BCMPTM_FLEXCTR

#define CTR_FLEX_FIELDS  "bcmptmCtrFlexFields"
#define CTR_FLEX_STATS   "bcmptmCtrFlexStats"

#define MAX_NUM_PIPES     8
#define MAX_INGRESS_POOLS 32
#define MAX_EGRESS_POOLS  32

#define MAX_CTR_INSTANCES 256
#define MAX_CTR_INSTANCES_BIT_WIDTH 8
#define MAX_CTR_TABLE_DEPTH 4096

#define ING_ATTR_PKT_TYPE_ALL        0x7FFFFFFF
#define EGR_ATTR_PKT_TYPE_ALL        0x3

#define LT_INFO_NUM_TABLES 4
#define LT_INFO_MAX_TABLE_NAME_LEN 32

/* Shifts for Ingress Packet Types */
#define PKT_TYPE_UNKNOWN                    0
#define PKT_TYPE_CONTROL                    1
#define PKT_TYPE_OAM                        2
#define PKT_TYPE_BFD                        3
#define PKT_TYPE_BPDU                       4
#define PKT_TYPE_ICNM                       5
#define PKT_TYPE_1588                       6
#define PKT_TYPE_KNOWN_L2UC                 7
#define PKT_TYPE_UNKNOWN_L2UC               8
#define PKT_TYPE_KNOWN_L2MC                 9
#define PKT_TYPE_UNKNOWN_L2MC               10
#define PKT_TYPE_L2BC                       11
#define PKT_TYPE_KNOWN_L3UC                 12
#define PKT_TYPE_UNKNOWN_L3UC               13
#define PKT_TYPE_KNOWN_IPMC                 14
#define PKT_TYPE_UNKNOWN_IPMC               15
#define PKT_TYPE_KNOWN_MPLS_L2              16
#define PKT_TYPE_UNKNOWN_MPLS               17
#define PKT_TYPE_KNOWN_MPLS_L3              18
#define PKT_TYPE_KNOWN_MPLS                 19
#define PKT_TYPE_KNOWN_MPLS_MC              20
#define PKT_TYPE_KNOWN_MIM                  21
#define PKT_TYPE_UNKNOWN_MIM                22
#define PKT_TYPE_KNOWN_TRILL                23
#define PKT_TYPE_UNKNOWN_TRILL              24
#define PKT_TYPE_KNOWN_NIV                  25
#define PKT_TYPE_UNKNOWN_NIV                26
#define PKT_TYPE_KNOWN_L2GRE                27
#define PKT_TYPE_KNOWN_VXLAN                28
#define PKT_TYPE_KNOWN_FCOE                 29
#define PKT_TYPE_UNKNOWN_FCOE               30

/* Shifts for Egress Packet Types */
#define PKT_TYPE_UNICAST                    0
#define PKT_TYPE_MULTICAST                  1

/* Shifts for Vlan Types */
#define VLAN_UT        0
#define VLAN_SIT       1
#define VLAN_SOT       2
#define VLAN_DT        3

/* Shifts for color */
#define ATTR_G_COLOR     0
#define ATTR_R_COLOR     1
#define ATTR_Y_COLOR     2

/* Shifts for VP Type */
#define ATTR_VP_ACCESS    0
#define ATTR_VP_NETWORK   1

/* Shifts for TCP flags */
#define TCP_FLAG_TYPE_FIN   0
#define TCP_FLAG_TYPE_SYN   1
#define TCP_FLAG_TYPE_RST   2
#define TCP_FLAG_TYPE_PSH   3
#define TCP_FLAG_TYPE_ACK   4
#define TCP_FLAG_TYPE_URG   5
#define TCP_FLAG_TYPE_ECE   6
#define TCP_FLAG_TYPE_CWR   7

#define CTR_FLEX_SET_BITS true
#define CTR_FLEX_CLR_BITS false

#define CTR_FLEX_POOL_ENABLE true
#define CTR_FLEX_POOL_DISABLE false

#define CTR_FLEX_LIST_SUBSCRIBE true
#define CTR_FLEX_LIST_UNSUBSCRIBE false

#define INVALID_CTR_FLEX_ID 0xFFFFFFFF

#define CTR_FLEX_UINT64_TO_UINT32(val32, val64) \
    do {                                        \
        val32[0] = (uint32_t)val64;             \
        val32[1] = (uint32_t)(val64>>32);       \
    } while(0)

/*!
 * \brief Linked List of foreign Table IDs subscribed to an entry.
 */
typedef struct llist_s {
     /*! Stores an ID that is subscribed to this entry. */
    uint32_t val;
     /*! ID reference count. */
    uint32_t ref_count;
     /*! Pointer to the next ID. */
    struct llist_s *next;
} llist_t;

/*!
 * \brief Packet Attribute table entry and its subscribers.
 */
typedef struct ctr_flex_pkt_attribute_s {
     /*! Indicates whether entry (indexed by PKT_ATTR_ID) is active. */
    bool active;

    /*! List of Attribute Table Entries (ATTR_IDs) using this PKT_ATTR_ID. */
    llist_t *ref_entry_list;
} ctr_flex_pkt_attributes_t;

/*!
 * \brief Attribute table entry and its subscribers.
 */
typedef struct ctr_flex_attributes_s {
    /*! Number of instances used by this ID (indexed by ATTR_ID). */
    uint32_t num_instances;

    /*! List of Entry Table Entries (ENTRY_IDs) using this ATTR_ID. */
    llist_t *ref_entry_list;
} ctr_flex_attributes_t;

/*!
 * \brief Structure with opcodes for FLEX_ENTRY notification
 */
typedef struct ctr_flex_notify_entry_opcode_s {
    /*! FLEX ENTRY inserted with LT INSERT */
    bool insert;
    /*! FLEX ENTRY deleted with LT DELETE */
    bool delete;
    /*! FLEX ENTRY updated, either LT UPDATE or via subscription list */
    bool update;
    /*! FLEX ENTRY retry on a waitlisted entry */
    bool retry;
} ctr_flex_notify_entry_opcode_t;

/*!
 * \brief Structure to manage pool memory
 */
typedef struct pool_list_s {
    uint32_t max_track;
    SHR_BITDCL *inuse_bitmap;
} pool_list_t;

/*!
 * \brief Flex counter pool structure
 */
typedef struct ctr_flex_pool_info_s {
    /*! Number of counters in pool */
    size_t num_counters;

    /*!
     * Pool reservation for ingress only.
     * 0 = common pool, 1 = reserved for IFP
     */
    bool pool_reservation;

} ctr_flex_pool_info_t;

/*!
 * \brief Flex counter table info structure
 */
typedef struct ctr_flex_table_info_s {
    /*! LT name */
    char name[LT_INFO_MAX_TABLE_NAME_LEN+1];

    /*! Number of key fields in LT */
    size_t num_key_fields;

    /*! Number of data fields in LT */
    size_t num_data_fields;

    /*! Number of read-only data fields in LT */
    size_t num_read_only_fields;
} ctr_flex_table_info_t;

/*!
 * \brief Flex counter enum values
 */
typedef struct ctr_flex_enum_s {
    /*! OPERATIONAL_STATE_OK */
    uint32_t operational_state_ok;

    /*! PKT_ATTR_TEMPLATE_NOT_PRESENT */
    uint32_t pkt_attr_template_not_present;

    /*! ATTR_TEMPLATE_NOT_PRESENT */
    uint32_t attr_template_not_present;

    /*! ATTR_TEMPLATE_EXCEEDED */
    uint32_t attr_template_exceeded;

    /*! PKT_ATTR_TEMPLATE_EXCEEDED */
    uint32_t pkt_attr_template_exceeded;

    /*! AWAITING_COUNTER_RESOURCE */
    uint32_t awaiting_counter_resource;

    /*! DIRECT_MAP_MODE */
    uint32_t direct_map_mode;

    /*! UNCOMPRESSED_MODE */
    uint32_t uncompressed_mode;
} ctr_flex_enum_t;

/*!
 * \brief Flex counter table depths
 */
typedef struct ctr_flex_table_sizes_s {
    uint32_t max_ing_attr_entries;
    uint32_t max_egr_attr_entries;
    uint32_t max_ing_pkt_attr_entries;
    uint32_t max_egr_pkt_attr_entries;
} ctr_flex_table_sizes_t;

/*!
 * \brief Flex counter control structure
 */
typedef struct ctr_flex_control_s {
    /* Device info */
    ctr_flex_device_info_t *ctr_flex_device_info;

    /* Pool info */
    ctr_flex_pool_info_t ctr_ing_flex_pool[MAX_INGRESS_POOLS];
    ctr_flex_pool_info_t ctr_egr_flex_pool[MAX_EGRESS_POOLS];

    /* Flex Counter Tables */
    ctr_flex_attributes_t *ing_attributes;
    ctr_flex_attributes_t *egr_attributes;

    ctr_flex_pkt_attributes_t *ing_pkt_attributes;
    ctr_flex_pkt_attributes_t *egr_pkt_attributes;

    /* Pool lists */
    pool_list_t ing_pool_list[MAX_INGRESS_POOLS][MAX_NUM_PIPES];
    pool_list_t egr_pool_list[MAX_EGRESS_POOLS][MAX_NUM_PIPES];

    /* Wailist lists */
    llist_t* ing_wait_list;
    llist_t* egr_wait_list;

    /* Table info for LT FLEX_ENTRY and LT FLEX_STATS */
    ctr_flex_table_info_t ctr_flex_field_info[LT_INFO_NUM_TABLES];

    /* List of Flex Counter Field Enum values. */
    ctr_flex_enum_t ctr_flex_enum;

    /* Table depths of LT ATTR and LT PKT_ATTR */
    ctr_flex_table_sizes_t ctr_flex_table_sizes;
} ctr_flex_control_t;

/*!
 * \brief Offset table entry
 */
typedef struct offset_table_entry_s {
    uint8_t    offset;
    uint8_t    count_enable;
} offset_table_entry_t;

/*!
 * \brief Flex attribute structure
 */
typedef struct flex_attribute_s {
    /*! IP */
    uint32_t    ip;
    /*! Drop */
    uint32_t    drop;
    /*! Packet type */
    uint32_t    pkt_type;
    /*! ECN */
    uint32_t    tos_ecn;
    /*! DSCP */
    uint32_t    tos_dscp;
    /*! Port */
    uint32_t    port;
    /*! Internal congestion */
    uint32_t    int_cn;
    /*! INNER DOT1P */
    uint32_t    inner_dot1p;
    /*! OUTER DOT1P */
    uint32_t    outer_dot1p;
    /*! VLAN format */
    uint32_t    vlan;
    /*! INT_PRI */
    uint32_t    int_pri;
    /*! Pre FP color */
    uint32_t    pre_fp_color;
    /* FP color */
    uint32_t    fp_color;
    /* SVP type */
    uint32_t    svp_type;
    /* DVP type */
    uint32_t    dvp_type;
    /* UDF bitmap */
    uint32_t    udf;
    /* TCP flags */
    uint32_t    tcp_flags;
    /* Elephant pkt */
    uint32_t    elephant_pkt;
    /* Congestion marked */
    uint32_t    congestion_marked;
    /* Unicast queueing */
    uint32_t    unicast_queueing;
    /* MMU queue number */
    uint32_t    mmu_queue_num;
} flex_attribute_t;

/*!
 * \brief Flex counter packet attribute template structure
 */
typedef struct ctr_flex_pkt_attribute_template_s {
    /*! Select IP Packet */
    bool ip;

    /*! Select drop */
    bool drop;

    /*! Following selects Packet type */
    /*! Select packet type all */
    bool pkt_type_all;

    /*! Select packet type unknown */
    bool pkt_type_unknown;

    /*! Select control packet type */
    bool pkt_type_control;

    /*! Select OAM packet type */
    bool pkt_type_oam;

    /*! Select BFP packet type */
    bool pkt_type_bfd;

    /*! Select BPDU packet type */
    bool pkt_type_bpdu;

    /*! Select ICNM packet type */
    bool pkt_type_icnm;

    /*! Select 1588 packet type */
    bool pkt_type_1588;

    /*! Select known l2uc packet type */
    bool pkt_type_known_l2uc;

    /*! Select unknown l2uc packet type */
    bool pkt_type_unknown_l2uc;

    /*! Select l2bc packet type */
    bool pkt_type_l2bc;

    /*! Select known l2mc packet type */
    bool pkt_type_known_l2mc;

    /*! Select unknown l2mc packet type */
    bool pkt_type_unknown_l2mc;

    /*! Select known l3uc packet type */
    bool pkt_type_known_l3uc;

    /*! Select unknown l3uc packet type */
    bool pkt_type_unknown_l3uc;

    /*! Select known ipmc packet type */
    bool pkt_type_known_ipmc;

    /*! Select unknown ipmc packet type */
    bool pkt_type_unknown_ipmc;

    /*! Select known L2 MPLS packet type */
    bool pkt_type_known_mpls_l2;

    /*! Select unknown L3 MPLS packet type */
    bool pkt_type_known_mpls_l3;

    /*! Select known MPLS packet type */
    bool pkt_type_known_mpls;

    /*! Select unknown MPLS packet type */
    bool pkt_type_unknown_mpls;

    /*! Select known MPLS MC packet type */
    bool pkt_type_known_mpls_mc;

    /*! Select known mim packet type */
    bool pkt_type_known_mim;

    /*! Select unknown mim packet type */
    bool pkt_type_unknown_mim;

    /*! Select known trill packet type */
    bool pkt_type_known_trill;

    /*! Select unknown trill packet type */
    bool pkt_type_unknown_trill;

    /*! Select known niv packet type */
    bool pkt_type_known_niv;

    /*! Select unknown niv packet type */
    bool pkt_type_unknown_niv;

    /*! Select egress multicast packet type */
    bool pkt_type_multicast;

    /*! Select egress unicast packet type */
    bool pkt_type_unicast;

    /*! Select network-facing DVP type */
    bool network_dvp;

    /*! Select network-facing DVP type */
    bool network_svp;

    /*! Select access-facing DVP type */
    bool access_dvp;

    /*! Select access-facing DVP type */
    bool access_svp;

    /*! Select Explicit Congestion Notification (ECN) */
    uint8_t tos_ecn;

    /*! Select Differentiated Services (DSCP) */
    uint8_t tos_dscp;

    /*! Port bitmap */
    uint8_t port;

    /*! Internal Congestion */
    uint8_t int_cn;

    /*! Outer 801.1p */
    uint8_t outer_dot1p;

    /*! Inner 801.1p */
    uint8_t inner_dot1p;

    /*! VLAN format selections */
    /*! Selects untagged vlan (VLAN_UT) */
    bool vlan_ut;

    /*! Selects inner tagged vlan (VLAN_SIT) */
    bool vlan_sit;

    /*! Selects outer tagged vlan (VLAN_SIT) */
    bool vlan_sot;

    /*! Selects inner and outer tagged vlan (VLAN_DT) */
    bool vlan_dt;

    /*! Specifies internal priority */
    uint8_t int_pri;

    /*! FP color selections */
    /*! Selects FP color green */
    bool fp_g_color;

    /*! Selects FP color yellow */
    bool fp_y_color;

    /*! Selects FP red yellow */
    bool fp_r_color;

    /*! Pre FP color selections */
    /*! Selects Pre FP color green */
    bool pre_fp_g_color;

    /*! Selects Pre FP color yellow */
    bool pre_fp_y_color;

    /*! Selects Pre FP red yellow */
    bool pre_fp_r_color;

    /*! Selects Elephant packet */
    bool elephant_pkt;

    /*! Specifies TCP flag FIN */
    bool tcp_flag_type_fin;

    /*! Specifies TCP flag SYN */
    bool tcp_flag_type_syn;

    /*! Specifies TCP flag RST */
    bool tcp_flag_type_rst;

    /*! Specifies TCP flag PSH */
    bool tcp_flag_type_psh;

    /*! Specifies TCP flag ACK */
    bool tcp_flag_type_ack;

    /*! Specifies TCP flag URG */
    bool tcp_flag_type_urg;

    /*! Specifies TCP flag ECE */
    bool tcp_flag_type_ece;

    /*! Specifies TCP flag CWR */
    bool tcp_flag_type_cwr;

    /*! Specifies user defined field (UDF) */
    uint32_t udf;

    /*! Select Congestion marked packets */
    bool congestion_marked;

    /*! Select based on Unicast queueing value */
    bool unicast_queueing;

    /*! Select based on MMU QUEUE NUMBER */
    uint32_t mmu_queue_num;
} ctr_flex_pkt_attribute_template_t;

/*!
 * \brief User Data from the entry LT.
 */
typedef struct ctr_flex_entry_data_s {
    uint32_t entry_id;
    uint32_t pool_id;
    uint32_t pipe_num;
    bool     pipe_unique;
    bool     pipe_unique_valid;
    uint32_t base_index;
    bool     base_index_mode_auto;
    bool     base_index_mode_auto_valid;
    uint32_t max_instances;
    bool     max_instances_mode_auto;
    bool     max_instances_mode_auto_valid;
    uint32_t offset_mode;
    uint32_t attr_id;
    uint32_t op_state;
} ctr_flex_entry_data_t;

/*!
 * \brief flex counter active entry structure
 */
 typedef struct active_entry_s {
    /*! LT Entry data input by user */
    ctr_flex_entry_data_t *entry_data;

    /*! Number of active instances */
    uint32_t active_instances;

    /*! Counter instance id array. */
    uint32_t *ctr_instance_id;

    /*! Packet attribute template id array. */
    uint32_t *pkt_attribute_template_id;

    /*! Combined Packet attribute structures */
    flex_attribute_t flex_attribute;

    /*! Offset Table map */
    offset_table_entry_t *offset_table_map;

    /*! Ingress or egress entry */
    bool ingress;
} active_entry_t;

/*!
 * \brief Source LT of user data.
 */
typedef enum ctr_flex_data_type_e {
   CTR_FLEX_PKT_ATTR_DATA = 0,
   CTR_FLEX_ATTR_DATA = 1,
   CTR_FLEX_ENTRY_DATA = 2
} ctr_flex_data_type_t;

/*!
 * \brief User Data from the packet attribute LT.
 */
typedef struct ctr_flex_pkt_attr_data_s {
    uint32_t pkt_attr_id;
    ctr_flex_pkt_attribute_template_t *template;
} ctr_flex_pkt_attr_data_t;

/*!
 * \brief User Data from the attribute LT.
 */
typedef struct ctr_flex_attr_data_s {
    uint32_t attr_id;
    uint32_t instance_id;
    uint32_t pkt_attr_id;
} ctr_flex_attr_data_t;

/*!
 * \brief User Data and type from LT insert or LT update.
 */
typedef struct ctr_flex_user_data_s {
    bool ingress;
    ctr_flex_data_type_t data_type;
    bcmimm_entry_event_t event_reason;
    ctr_flex_pkt_attr_data_t pkt_attr_data;
    ctr_flex_attr_data_t attr_data;
    ctr_flex_entry_data_t entry_data;
} ctr_flex_user_data_t;

/*!
 * Routine to read entry from hardware
 */
int
bcmptm_ctr_flex_ireq_read(int unit,
                          bcmltd_sid_t lt_id,
                          bcmdrd_sid_t pt_id,
                          int index,
                          void *entry_data);

/*!
 * Routine to write entry to hardware
 */
int
bcmptm_ctr_flex_ireq_write(int unit,
                          bcmltd_sid_t lt_id,
                          bcmdrd_sid_t pt_id,
                          int index,
                          void *entry_data);

/*!
 * Create a pool manager to manage specified number of counters.
 */
int
bcmptm_ctr_flex_pool_mgr_create (pool_list_t *pool_list,
                                 uint32_t max_counters);

/*!
 * Allocate specified number of counters and return base index.
 */
int
bcmptm_ctr_flex_pool_mgr_allocate (pool_list_t *pool_list,
                                  uint32_t num_counters,
                                  uint32_t *base_index);

/*!
 * Reserve specified number of counters at requested base index.
 */
int
bcmptm_ctr_flex_pool_mgr_reserve (pool_list_t *pool_list,
                                  uint32_t base_index,
                                  uint32_t num_counters);

/*!
 * Free specified number of counters at specified base index.
 */
int
bcmptm_ctr_flex_pool_mgr_free (pool_list_t *pool_list,
                               uint32_t base_index,
                               uint32_t num_counters);

/*!
 * Allocate a field struct with requested number of fields
 */
int
bcmptm_ctr_flex_flds_allocate(int unit, bcmltd_fields_t* in, size_t num_fields);

/*!
 * Check if the specified base and num_counters are available.
 * Useful during unit testing.
 */
int
bcmptm_ctr_flex_pool_mgr_is_base_free (int unit,
                                   bool ingress,
                                   uint32_t pipe,
                                   uint32_t pool,
                                   uint32_t num_counters,
                                   uint32_t base_index);
/*!
 * Check if the specified base and num_counters are used.
 * Useful during unit testing.
 */
int
bcmptm_ctr_flex_pool_mgr_is_base_full (int unit,
                                   bool ingress,
                                   uint32_t pipe,
                                   uint32_t pool,
                                   uint32_t num_counters,
                                   uint32_t base_index);
/*!
 * Check if the specified pool is entirely full
 */
bool
bcmptm_ctr_flex_pool_mgr_is_pool_full (int unit,
                                   bool ingress,
                                   uint32_t pipe,
                                   uint32_t pool);

/*!
 * Free a field struct
 */
int
bcmptm_ctr_flex_flds_free(int unit, bcmltd_fields_t* in);

/*!
 * Destroy the specified pool manager.
 */
void
bcmptm_ctr_flex_pool_mgr_destroy (pool_list_t *pool_list);

/*!
 * IMM insert/update callback handler for ING_FLEX_STATS LT.
 */
int
bcmptm_flex_ing_ctr_stats_lt_stage_cb(int unit,
                                      bcmltd_sid_t sid,
                                      uint32_t trans_id,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key_fields,
                                      const bcmltd_field_t *data_fields,
                                      void *context,
                                      bcmltd_fields_t *output_fields);
/*!
 * IMM insert/update callback handler for EGR_FLEX_STATS LT.
 */
int
bcmptm_flex_egr_ctr_stats_lt_stage_cb(int unit,
                                      bcmltd_sid_t sid,
                                      uint32_t trans_id,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key_fields,
                                      const bcmltd_field_t *data_fields,
                                      void *context,
                                      bcmltd_fields_t *output_fields);
/*!
 * IMM lookup callback handler for EGR_FLEX_STATS LT.
 */
int
bcmptm_flex_egr_ctr_stats_lt_lookup_cb (int unit,
                                        bcmltd_sid_t sid,
                                        uint32_t trans_id,
                                        void *context,
                                        bcmimm_lookup_type_t lkup_type,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out);
/*!
 * IMM lookup callback handler for ING_FLEX_STATS LT.
 */
int
bcmptm_flex_ing_ctr_stats_lt_lookup_cb (int unit,
                                        bcmltd_sid_t sid,
                                        uint32_t trans_id,
                                        void *context,
                                        bcmimm_lookup_type_t lkup_type,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out);

/*!
 * Write the device offset table
 */
int
update_offset_table(int unit,
                    bool ingress,
                    uint32_t pool,
                    uint8_t offset_mode,
                    offset_table_entry_t offset_table_map[256]);

/*!
 * Read the device packet selector register
 */
int
read_pkt_selector_regvalue(int unit,
                           bool ingress,
                           uint32_t pool,
                           uint32_t *pkt_selector_reg_value);

/*!
 * Update the device packet selector register
 */
int
update_pkt_attr_selector_bits(int unit,
                              bool ingress,
                              bool set_bit,
                              uint32_t pool,
                              uint32_t pkt_attr_bit_position,
                              uint32_t pkt_attr_total_bits,
                              uint8_t  *current_bit_position,
                              uint32_t *pkt_selector_reg_value);

/*!
 * Write the device packet selector register
 */
int
write_pkt_attr_selectors(int unit,
                        bool ingress,
                        uint32_t pool,
                        uint32_t *pkt_selector_reg_value);
/*!
 * Enable a counter pool on the device.
 * Call the CCI interface to enable eviction on the pool.
 */
int
enable_counter_pool(int unit,
                    bool ingress,
                    bool all_pools,
                    uint32_t pool,
                    bool enable);

/*!
 * Get Table SID from pool ID.
 */
int
get_counter_sid (int unit,
                 bool ingress,
                 uint32_t pool,
                 bcmdrd_sid_t *table_sid);

/*!
 * Enable a counter offset in active entry offset-map.
 */
void
update_offset_map(active_entry_t *active_entry,
                      uint8_t offset,
                      uint8_t counter);

/*!
 * Update the packet selector registers and offset map tables.
 */
int
update_packet_selectors(int unit,
                        bool set_bit,
                        active_entry_t *active_entry,
                        flex_attribute_t *combined_flex_attribute);

/*!
 * Set packet type and other attributes in active entry.
 */
int
process_per_counter_attributes(int unit,
            active_entry_t *active_entry,
            ctr_flex_pkt_attribute_template_t *pkt_attribute_template);

/*!
 * Compute offsets for all active entry instances.
 */
int
process_offset_table(int unit,
                     active_entry_t *active_entry,
                     ctr_flex_user_data_t *user_data,
                     flex_attribute_t *combined_flex_attribute);

/*!
 * Lookup all intances for this entry and combine the attributes.
 */
int
generate_combined_attributes(int unit,
                             active_entry_t *active_entry,
                             ctr_flex_user_data_t *user_data,
                             flex_attribute_t *combined_flex_attribute);
/*!
 * Computes the offset map and programs the OFFET_TABLE with counter offsets
 * and counter enable. Also programs the packet key selectors bits.
 */
int
process_attributes(int unit,
                   active_entry_t *active_entry,
                   ctr_flex_user_data_t *user_data);

/*!
 * Lookup PKT_ATTR_TEMPLATE table given pkt_attr_template_id.
 * Return packet attributes in pkt_attribute_template.
 * Return SHR_E if entry not found.
 * user_data, if any, is entry still being processed in stage_cb.
 */
int
lookup_flex_pkt_attr_table (int unit, bool ingress, bool lookup_local,
                uint32_t pkt_attr_template_id,
                ctr_flex_user_data_t *user_data,
                ctr_flex_pkt_attribute_template_t *pkt_attribute_template);

/*!
 * Lookup FLEX_ENTRY table given entry_id.
 * Return data fields. Return SHR_E_ if entry not found.
 */
int
lookup_flex_entry_table (int unit, bool ingress, uint32_t entry_id,
                         ctr_flex_entry_data_t *entry_data);

/*!
 * Parse FLEX_ENTRY from imm_field list.
 */
int
parse_flex_entry (int unit, bool ingress, bcmltd_fields_t *imm_fields,
                  ctr_flex_entry_data_t *entry_data);

/*!
 * Lookup ATTR_TEMPLATE table given attr_template_id and instance_id.
 * Return pkt_attr_template_id. Return SHR_E_ if entry not found.
 * user_data, if any, is entry still being processed in stage_cb.
 */
int
lookup_flex_attr_table (int unit, bool ingress, uint32_t attr_template_id,
                        uint32_t instance, ctr_flex_user_data_t *user_data,
                        uint32_t *pkt_attr_template_id);

/*!
 * Notify all subscibers of the FLEX_PKT_ATTR table entry.
 * This notifies each member of the list of FLEX_ATTR entries that
 * want to be notified of any changes to this FLEX_PKT_ATTR table entry.
 */
int
ctr_flex_notify_pkt_attr_template (int unit, ctr_flex_user_data_t *user_data);

/*!
 * Notify all subscibers of the FLEX_ATTR table entry.
 * This notifies each member of the list of FLEX_ENTRY entries that
 * want to be notified of any changes to this FLEX_ATTR table entry.
 */
int
ctr_flex_notify_attr_template (int unit, ctr_flex_user_data_t *user_data);

/*!
 * Process the requested operation on an FLEX_ENTRY table entry.
 * This operation may be an insert, update or delete.
 * Insert and Deletes were triggered by LT insert or delete
 * on the FLEX_ENTRY table.
 * Update may be triggered by LT update on the FLEX_ENTRY table.
 * Update may also be triggered by changes in the FLEX_ATTR or
 * FLEX_PKT_ATTR tables, resulting in notifications.
 */
int
ctr_flex_notify_entry_template (int unit,
                                ctr_flex_notify_entry_opcode_t *entry_opcode,
                                ctr_flex_user_data_t *user_data,
                                ctr_flex_entry_data_t *new_entry_data,
                                ctr_flex_entry_data_t *current_entry_data);

/*!
 * Get LT fields default values
 */
int
ctr_flex_entry_get_field_default(int unit,
                                 bool ingress,
                                 ctr_flex_entry_data_t *default_value);
/*!
 * Decode the PKT_ATTR fields
 */
void
extract_flex_ing_pkt_attribute_fields(
    ctr_flex_pkt_attribute_template_t *ing_pkt_attribute_template,
    const bcmltd_field_t *data_field);

/*!
 * Update the OPERATIONAL_STATUSf in the FLEX_ENTRY table.
 */
int
oper_state_update (int unit, bool ingress, uint32_t entry_id,
                   ctr_flex_entry_data_t *entry_data);

/*!
 * Populate the Read-Only Fields of ING_POOL_INFOt
 */
int
ctr_flex_populate_ing_pool_info(int unit, bool warm);

/*!
 * Populate the Read-Only Fields of EGR_POOL_INFOt
 */
int
ctr_flex_populate_egr_pool_info(int unit, bool warm);

/*!
 * Get the field info for LT ENTRY and LT STATS
 */
int
ctr_flex_get_lt_field_info(int unit);

/*!
 * Register callback handlers for the in-memory logical tables
 */
int
ctr_flex_imm_lt_register(int unit, bool warm);

/*!
 * Subscribe an ID to a notification list.
 * Notification lists may be in the FLEX_ATTR or FLEX_PKT_ATTR tables.
 */
int
ctr_flex_subscribe_list (int unit, bool subscribe, llist_t **llist, uint32_t id) ;

/*!
 * Allocate counters for the specified active entry.
 * Use supplied base_index or allocate a base_index depending on mode_auto.
 */
int
allocate_counters (int unit, active_entry_t *active_entry);

/*!
 * Free counters for the specified active entry, and release the base_index.
 */
int
free_counters (int unit, active_entry_t *active_entry);

/*!
 * Resize counters for the specified active entry.
 * The max number of instances currently allocated is provided.
 */
int
resize_counters (int unit, active_entry_t *active_entry, uint32_t current_instances);

/*!
 * Get flex counter control structure.
 */
int
bcmptm_ctr_flex_ctrl_get(int unit,
                         ctr_flex_control_t **ctrl);

/*!
 * Get flex counter device-specific driver callback.
 */
int
bcmptm_ctr_flex_data_driver_get(int unit,
                                const bcmptm_ctr_flex_data_driver_t **cb);

#endif /* CTR_FLEX_INTERNAL_H */
