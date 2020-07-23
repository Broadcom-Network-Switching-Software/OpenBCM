/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ISM-HASH management routines
 */

#ifndef _SOC_ISM_HASH_H
#define _SOC_ISM_HASH_H

#include <soc/mem.h>
#include <soc/ism.h>

#ifdef BCM_ISM_SUPPORT

typedef enum soc_mem_view_type_e {
    SOC_MEM_VIEW_VLAN_XLATE_1_XLATE = 1,
    SOC_MEM_VIEW_VLAN_XLATE_1_MPLS,
    SOC_MEM_VIEW_VLAN_XLATE_1_VLAN_MAC,
    SOC_MEM_VIEW_VLAN_XLATE_1_VIF,
    SOC_MEM_VIEW_VLAN_XLATE_2_XLATE,
    SOC_MEM_VIEW_VLAN_XLATE_2_MPLS,
    SOC_MEM_VIEW_VLAN_XLATE_2_MIM_NVP,
    SOC_MEM_VIEW_VLAN_XLATE_2_MIM_ISID,
    SOC_MEM_VIEW_VLAN_XLATE_2_TRILL,
    SOC_MEM_VIEW_VLAN_XLATE_2_MAC_IP_BIND,
    SOC_MEM_VIEW_L2_ENTRY_1_L2,
    SOC_MEM_VIEW_L2_ENTRY_1_VLAN,
    SOC_MEM_VIEW_L2_ENTRY_1_VIF,
    SOC_MEM_VIEW_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG,
    SOC_MEM_VIEW_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT,
    SOC_MEM_VIEW_L2_ENTRY_1_BFD,
    SOC_MEM_VIEW_L2_ENTRY_2_L2,
    SOC_MEM_VIEW_L3_ENTRY_1_IPV4UC,
    SOC_MEM_VIEW_L3_ENTRY_1_LMEP,
    SOC_MEM_VIEW_L3_ENTRY_1_RMEP,
    SOC_MEM_VIEW_L3_ENTRY_1_TRILL,
    SOC_MEM_VIEW_L3_ENTRY_2_IPV4UC,
    SOC_MEM_VIEW_L3_ENTRY_2_IPV4MC,
    SOC_MEM_VIEW_L3_ENTRY_2_IPV6UC,
    SOC_MEM_VIEW_L3_ENTRY_4_IPV6UC,
    SOC_MEM_VIEW_L3_ENTRY_4_IPV6MC,
    SOC_MEM_VIEW_EP_VLAN_XLATE_1_XLATE,
    SOC_MEM_VIEW_EP_VLAN_XLATE_1_MIM_ISID,
    SOC_MEM_VIEW_MPLS_ENTRY_1_MPLS,
    SOC_MEM_VIEW_MPLS_ENTRY_2_MPLS,
    SOC_MEM_VIEW_MPLS_ENTRY_2_MIM_NVP,
    SOC_MEM_VIEW_MPLS_ENTRY_2_MIM_ISID,
    SOC_MEM_VIEW_MPLS_ENTRY_2_TRILL
} soc_mem_view_type_t;

typedef enum soc_mem_key_type_e {
    SOC_MEM_KEY_VLAN_XLATE_1_XLATE_IVID_OVID = 0,
    SOC_MEM_KEY_VLAN_XLATE_2_XLATE_IVID_OVID = 1,
    SOC_MEM_KEY_VLAN_XLATE_1_XLATE_OVID = 2,
    SOC_MEM_KEY_VLAN_XLATE_2_XLATE_OVID = 3,
    SOC_MEM_KEY_VLAN_XLATE_1_XLATE_IVID = 4,
    SOC_MEM_KEY_VLAN_XLATE_2_XLATE_IVID = 5,
    SOC_MEM_KEY_VLAN_XLATE_1_XLATE_OTAG = 6,
    SOC_MEM_KEY_VLAN_XLATE_2_XLATE_OTAG = 7,
    SOC_MEM_KEY_VLAN_XLATE_1_XLATE_ITAG = 8,
    SOC_MEM_KEY_VLAN_XLATE_2_XLATE_ITAG = 9,
    SOC_MEM_KEY_VLAN_XLATE_1_XLATE_PRI_CFI = 10,
    SOC_MEM_KEY_VLAN_XLATE_2_XLATE_PRI_CFI = 11,
    SOC_MEM_KEY_VLAN_XLATE_1_XLATE_IVID_OVID_SVP = 12,
    SOC_MEM_KEY_VLAN_XLATE_2_XLATE_IVID_OVID_SVP = 13,
    SOC_MEM_KEY_VLAN_XLATE_1_XLATE_OVID_SVP = 14,
    SOC_MEM_KEY_VLAN_XLATE_2_XLATE_OVID_SVP = 15,
    SOC_MEM_KEY_VLAN_XLATE_1_VLAN_MAC_VLAN_MAC = 20,
    SOC_MEM_KEY_VLAN_XLATE_1_VIF_VIF = 21,
    SOC_MEM_KEY_VLAN_XLATE_1_VIF_VIF_VLAN = 22,
    SOC_MEM_KEY_VLAN_XLATE_2_MAC_IP_BIND_HPAE = 27,
    SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE = 0,
    SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE = 1,
    SOC_MEM_KEY_L2_ENTRY_1_L2_VFI = 2,
    SOC_MEM_KEY_L2_ENTRY_2_L2_VFI = 3,
    SOC_MEM_KEY_L2_ENTRY_1_VLAN_SINGLE_CROSS_CONNECT = 6,
    SOC_MEM_KEY_L2_ENTRY_1_VLAN_DOUBLE_CROSS_CONNECT = 7,
    SOC_MEM_KEY_L2_ENTRY_1_VIF_VIF = 8,
    SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS = 9,
    SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG_TRILL_NONUC_NETWORK_LONG = 10,
    SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT = 11,
    SOC_MEM_KEY_L2_ENTRY_1_BFD_BFD = 12,
    SOC_MEM_KEY_L2_ENTRY_1_PE_VID_PE_VID = 13,
    SOC_MEM_KEY_L3_ENTRY_1_IPV4UC_IPV4_UNICAST = 0,
    SOC_MEM_KEY_L3_ENTRY_2_IPV4UC_IPV4_UNICAST = 1,
    SOC_MEM_KEY_L3_ENTRY_2_IPV6UC_IPV6_UNICAST = 2,
    SOC_MEM_KEY_L3_ENTRY_4_IPV6UC_IPV6_UNICAST = 3,
    SOC_MEM_KEY_L3_ENTRY_2_IPV4MC_IPV4_MULTICAST = 6,
    SOC_MEM_KEY_L3_ENTRY_4_IPV6MC_IPV6_MULTICAST = 7,
    SOC_MEM_KEY_L3_ENTRY_1_LMEP_LMEP = 8,
    SOC_MEM_KEY_L3_ENTRY_1_RMEP_RMEP = 9,
    SOC_MEM_KEY_L3_ENTRY_1_TRILL_TRILL = 10,
    SOC_MEM_KEY_EP_VLAN_XLATE_1_XLATE_VLAN_XLATE = 0,
    SOC_MEM_KEY_EP_VLAN_XLATE_1_XLATE_VLAN_XLATE_DVP = 1,
    SOC_MEM_KEY_EP_VLAN_XLATE_1_MIM_ISID_MIM_ISID = 2,
    SOC_MEM_KEY_EP_VLAN_XLATE_1_MIM_ISID_MIM_ISID_DVP = 3,
    SOC_MEM_KEY_MPLS_ENTRY_1_MPLS_MPLS_FIRST_PASS = 16,
    SOC_MEM_KEY_MPLS_ENTRY_2_MPLS_MPLS_FIRST_PASS = 17,
    SOC_MEM_KEY_MPLS_ENTRY_1_MPLS_MPLS_SECOND_PASS = 18,
    SOC_MEM_KEY_MPLS_ENTRY_2_MPLS_MPLS_SECOND_PASS = 19,
    SOC_MEM_KEY_MPLS_ENTRY_2_MIM_NVP_MIM_NVP = 23,
    SOC_MEM_KEY_MPLS_ENTRY_2_MIM_ISID_MIM_ISID = 24,
    SOC_MEM_KEY_MPLS_ENTRY_2_MIM_ISID_MIM_ISID_SVP = 25,
    SOC_MEM_KEY_MPLS_ENTRY_2_TRILL_TRILL = 26
} soc_mem_key_type_t;

#define MAX_FIELDS    5        /* Max fields for any key */

struct _soc_hash_bank;
struct _soc_hash_mem_set;
struct _soc_hash_mem;
struct _soc_hash_mem_view;
struct _soc_hash_key;

/* Physical bank representation */
typedef struct _soc_hash_bank {
    uint32 num_bkts;               /* banks physical attribute */
    uint32 bkt_size;               /* in terms of smallest entry (always 4 for ISM) */
    uint32 num_entries;            /* product of num_bkts and bkt_size */
    uint16 hash_offset;            /* offset of first bit of the key */
    uint32 hash_mask;              /* used to pick particular number of bits for hashing */
    uint32 base_entry;             /* starting location of this bank in a memory view */
    int8 bank_org;                 /* 0: dual 0, 1: dual 1, -1: ism */
    uint8 my_id;                   /* who am i */
    struct _soc_hash_mem_set *hms; /* pointer to mem set mapped to this bank */
} soc_hash_bank_t;

#define SOC_HASH_MEM_MAX_KEY_BUFFER_SIZE_BYTES 64
/* Memory set / feature */
typedef struct _soc_hash_mem_set {
    uint8 mem_set;              /* from soc_mem_set_type_t */
    struct _soc_hash_mem *shm;  /* pointer to logical memory tables */
    uint8 num_mems;             /* num of mem tables belonging to this set */
    struct _soc_hash_bank *shb; /* pointer to first bank in an ordered set of banks 
                                   used by memories of this set */
    uint8 num_banks;            /* number of banks in this memory */ 
    struct _soc_hash_key *shk;  /* ptr to key types */
    uint8 num_keys;             /* number of key types */    
    uint8 zero_lsb;             /* Use lsb or 0 when offset is 48 */
    uint16 max_key_bits;        /* used for preparing the key(with padding) */
} soc_hash_mem_set_t;

/* Logical memory definition */
typedef struct _soc_hash_mem {
    soc_mem_t mem;                  /* memory type (L2_ENTRY_1 etc) */
    struct _soc_hash_mem_view *hmv; /* pointer to mem views */
    uint8 num_views;                /* number of views belonging to this mem */
    struct _soc_hash_mem_set *shms; /* pointer to parent set */
} soc_hash_mem_t;

/* Memory view definition */
typedef struct _soc_hash_mem_view {
    uint8 mem_view;                     /* from soc_mem_view_type_t */
    soc_field_t key_fields[MAX_FIELDS]; /* list of key fields */
    uint32 key_size;                    /* key size */
    soc_field_t lsb_field;              /* field for hash lsb */
    struct _soc_hash_mem *shm;          /* pointer to logical memory table */
} soc_hash_mem_view_t;

/* Key type definition */
typedef struct _soc_hash_key {
    uint16 key_type;                /* from soc_mem_key_type_t */
    struct _soc_hash_mem_view *hmv; /* pointer to the mem view */
} soc_hash_key_t;

extern soc_hash_bank_t _soc_ism_shb[SOC_MAX_NUM_DEVICES][_SOC_ISM_MAX_BANKS];
extern soc_hash_mem_set_t _soc_ism_shms[];
extern soc_hash_mem_t _soc_ism_shm[];
extern soc_hash_mem_view_t _soc_ism_shmv[];
extern soc_hash_key_t _soc_ism_shk[];

typedef struct _soc_ism_hash_s {
    soc_hash_bank_t *shb;      /* List of all banks ordered by allocation */
    soc_hash_mem_set_t *shms;  /* List of all mem sets / feature */
    soc_hash_mem_t *shm;       /* List of logical memories within the mem sets */
    soc_hash_mem_view_t *shmv; /* List of views within a logical memory */
    soc_hash_key_t *shk;       /* All the key types in a set and/or view */
} _soc_ism_hash_t;

#define _SOC_ISM_BANKS(unit) SOC_ISM_HASH_INFO(unit)->shb
#define _SOC_ISM_SETS(unit)  SOC_ISM_HASH_INFO(unit)->shms
#define _SOC_ISM_MEMS(unit)  SOC_ISM_HASH_INFO(unit)->shm
#define _SOC_ISM_VIEWS(unit) SOC_ISM_HASH_INFO(unit)->shmv
#define _SOC_ISM_KEYS(unit)  SOC_ISM_HASH_INFO(unit)->shk

#endif /* BCM_ISM_SUPPORT */

#endif  /* _SOC_ISM_HASH_H */

