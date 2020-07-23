/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC UTT Memory (Table) Utilities
 */

#ifndef _SOC_ESW_UTT_H_
#define _SOC_ESW_UTT_H_

#ifdef BCM_UTT_SUPPORT
/************************************************************************
 ************************ ENUMS DEFINITION ******************************
 */
/*
 * Each LPM lookup starts at a fixed address irrespective of number of
 * TCAM unit allocated to the lookup. This results in address holes when
 * max TCAM's are not allocated.
 */
#define SOC_UTT_LPM_FIXED_DATA_ADDRESS       (1 << 0)

/*
 * LPM lookup address is based on the number of TCAM units allocated
 * to the lookup. Doesn't result in address holes.
 */
#define SOC_UTT_LPM_VARIABLE_DATA_ADDRESS    (1 << 1)

/************************************************************************
 *********************** DATA STRUCTURE DEFINITION **********************
 */
typedef enum {
    SOC_UTT_AGENT_IFP = 0,
    SOC_UTT_AGENT_LPM = 1,
    SOC_UTT_AGENT_MAX
} soc_utt_agent_t;

typedef struct {
    int num_tcams;              /* TCAMs per lookup */
    int tcam_start_idx;         /* TCAM start idx to be used */
    uint8 is_paired;            /* Represents if the lookup is paired */
} soc_utt_agent_lookup_info_t;

typedef struct {
    /* Fixed info per chip */
    int flags;
    int min_tcams;              /* Min num of TCAM units supported */
    int max_tcams;              /* Max num of TCAM units supported */
    int max_lookups;            /* Max num of lookups supported */
    int agent_id;               /* HW agent id */
    int max_tcams_per_lookup;   /* Max tcams per lookup. */
    int lookup_base;            /* Start of Lookup index. */

    /* Default value info per chip */
    int def_num_lookups;        /* Default lookups count */
    int def_table_size;         /* Default table size */

    /* Info based on config variables */
    int num_tcams;              /* Num of tcams allocated */
    int num_lookups;            /* Num of lookups for the agent */
    int bank_start;             /* UTT BANK start index */
    int bank_count;             /* UTT BANK count */
    soc_utt_agent_lookup_info_t *lookup_info;
} soc_utt_agent_info_t;

typedef struct {
    int tcam_size;             /* TCAM unit size */
    int max_tcams;             /* Total number of TCAM units */
    int max_agents;            /* Number of stages supported */
    soc_utt_agent_info_t agent_info[SOC_UTT_AGENT_MAX];
} soc_utt_control_t;

extern soc_utt_control_t *soc_utt_control[SOC_MAX_NUM_DEVICES];

/************************************************************************
 *********************** MACROS DEFINITION ******************************
 */
#define SOC_UTT_CONTROL(_u_)       (soc_utt_control[(_u_)])
#define SOC_UTT_TCAM_DEPTH(_u_)    (SOC_UTT_CONTROL(_u_)->tcam_size)
#define SOC_UTT_MAX_TCAMS(_u_)     (SOC_UTT_CONTROL(_u_)->max_tcams)
#define SOC_UTT_MAX_AGENTS(_u_)    (SOC_UTT_CONTROL(_u_)->max_agents)

#define SOC_UTT_AGENT_INFO(_u_, _a_)                                        \
    (SOC_UTT_CONTROL(_u_)->agent_info[(_a_)])

#define SOC_UTT_AGENT_FLAGS(_u_, _a_)                                       \
    (SOC_UTT_AGENT_INFO(_u_, _a_).flags)

#define SOC_UTT_AGENT_MIN_TCAMS(_u_, _a_)                                   \
    (SOC_UTT_AGENT_INFO(_u_, _a_).min_tcams)

#define SOC_UTT_AGENT_MAX_TCAMS(_u_, _a_)                                   \
    (SOC_UTT_AGENT_INFO(_u_, _a_).max_tcams)

#define SOC_UTT_AGENT_MAX_LOOKUPS(_u_, _a_)                                 \
    (SOC_UTT_AGENT_INFO(_u_, _a_).max_lookups)

#define SOC_UTT_AGENT_HW_ID(_u_, _a_)                                       \
    (SOC_UTT_AGENT_INFO(_u_, _a_).agent_id)

#define SOC_UTT_AGENT_LOOKUP_BASE(_u_, _a_)                                 \
    (SOC_UTT_AGENT_INFO(_u_, _a_).lookup_base)

#define SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(_u_, _a_)                        \
    (SOC_UTT_AGENT_INFO(_u_, _a_).max_tcams_per_lookup)

#define SOC_UTT_AGENT_DEF_NUM_LOOKUPS(_u_, _a_)                             \
    (SOC_UTT_AGENT_INFO(_u_, _a_).def_num_lookups)

#define SOC_UTT_AGENT_DEF_TABLE_SIZE(_u_, _a_)                              \
    (SOC_UTT_AGENT_INFO(_u_, _a_).def_table_size)

#define SOC_UTT_AGENT_NUM_TCAMS(_u_, _a_)                                   \
    (SOC_UTT_AGENT_INFO(_u_, _a_).num_tcams)

#define SOC_UTT_AGENT_BANK_START(_u_, _a_)                                  \
    (SOC_UTT_AGENT_INFO(_u_, _a_).bank_start)

#define SOC_UTT_AGENT_BANK_COUNT(_u_, _a_)                                  \
    (SOC_UTT_AGENT_INFO(_u_, _a_).bank_count)

#define SOC_UTT_AGENT_NUM_LOOKUPS(_u_, _a_)                                 \
    (SOC_UTT_AGENT_INFO(_u_, _a_).num_lookups)

#define SOC_UTT_AGENT_LOOKUP_INFO(_u_, _a_)                                 \
    (SOC_UTT_AGENT_INFO(_u_, _a_).lookup_info)

#define SOC_UTT_AGENT_LOOKUP_INFO_IDX(_u_, _a_, _idx_)                      \
    (SOC_UTT_AGENT_INFO(_u_, _a_).lookup_info[(_idx_)])

#define SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(_u_, _a_, _idx_)                \
    SOC_UTT_AGENT_LOOKUP_INFO_IDX(_u_, _a_, _idx_).num_tcams

#define SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(_u_, _a_, _idx_)           \
    SOC_UTT_AGENT_LOOKUP_INFO_IDX(_u_, _a_, _idx_).tcam_start_idx

#define SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(_u_, _a_, _idx_)                \
    SOC_UTT_AGENT_LOOKUP_INFO_IDX(_u_, _a_, _idx_).is_paired

#define SOC_UTT_IFP_TCAM_START  (0)

/************************************************************************
 *********************** EXTERN DECLARATIONS ****************************
 */
extern int soc_utt_init(int unit);
extern int soc_utt_deinit(int unit);

extern int soc_utt_ifp_bank_config(int unit);
extern int soc_utt_ifp_lookup_depth_get(int unit, int num_lookups,
                                        int *depth_arr);
extern void
soc_utt_ifp_index_get(int unit, int wide, int *index, int *index2, int m);
extern int soc_utt_lpm_mem_config_validate(int unit, int config_v6);
extern int soc_utt_lpm_bank_config(int unit, int urpf, int tcam_pair);

extern int soc_utt_lpm_lookup_info_update(int unit);
extern int soc_utt_lpm_paired_tcams(int unit, int config_ipv6, int *p_tcam);
extern int soc_utt_lpm_urpf_paired_tcams(int unit, int *p_tcam);

extern int soc_utt_l3_defip_index_map(int unit, int wide, int index);
extern int soc_utt_l3_defip_index_remap(int unit, int wide, int index);
extern int soc_utt_l3_defip_urpf_index_map(int unit, int wide, int index);
extern int soc_utt_l3_defip_urpf_index_remap(int unit, int wide, int index);
extern int soc_utt_l3_defip_alpm_urpf_index_map(int unit, int wide, int index);
extern int soc_utt_l3_defip_alpm_urpf_index_remap(int unit, int wide, int index);
extern int soc_utt_l3_defip_aacl_index_map(int unit, int wide, int urpf, int index);
extern int soc_utt_l3_defip_aacl_index_remap(int unit, int wide, int urpf, int index);

#if defined(INCLUDE_L3)
extern int soc_utt_lpm_entry_in_paired_tcam(int unit, int index);
extern int soc_utt_lpm_max_64bv6_route_get(int u, int paired, uint16 *entries);
extern int soc_utt_lpm_unreserved_entry_count(int unit, int *count);
extern int soc_utt_lpm128_logical_tcam_start_index(int unit, int index);
extern int soc_utt_lpm128_logical_tcam_num(int unit, int index);
extern int soc_utt_lpm128_is_logical_tcam_first_entry(int unit, int index);
extern int soc_utt_lpm128_paired_offset(int unit, int index);
extern int soc_utt_lpm128_move_v4_down_for_v6_get_fent(int unit,
                                                       int fent_start,
                                                       int fent,
                                                       int *fent_for_move);
extern int soc_utt_lpm128_multiple_lookup_v4_domain_odd_start(int unit, int start,
                                                              int fent, int rem ,
                                                              int *v4_domain_odd_start);
extern int soc_utt_lpm_scaled_unreserved_start_index(int unit, int *start);
#endif /* INCLUDE_L3 */


#endif /* BCM_UTT_SUPPORT */
#endif /* _SOC_ESW_UTT_H_ */

