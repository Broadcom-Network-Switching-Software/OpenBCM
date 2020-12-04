/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ecmp.h
 * Purpose:     SOC ECMP functions.
 */

#ifndef   _SOC_ESW_ECMP_H_
#define   _SOC_ESW_ECMP_H_

#include <soc/drv.h>

#define SOC_ESW_ECMP_MAX_BANKS    0x4
#define SOC_ESW_ECMP_BANK_SIZE    1024
#define SOC_ESW_ECMP_MAX_GROUPS   4096


#define SOC_ESW_ECMP_IF_ERR_EXIT(rv)        \
    do {                                    \
        if (rv < 0) {                       \
            goto err_exit;                  \
        }                                   \
    } while(0)

typedef enum soc_esw_ecmp_bank_lkup_level_e {
    socEswEcmpBankLkupLevelInvalid  = 0,
    socEswEcmpBankLkupLevelUnderlay = 1,
    socEswEcmpBankLkupLevelOverlay  = 2,
    socEswEcmpBankLkupLevelReserved = 3
} soc_esw_ecmp_bank_lkup_level_t;

typedef struct soc_esw_ecmp_init_info_s {
    int num_levels;       /* Ecmp lookup levels. */
    int num_banks;        /* Number of banks in ecmp member table. */
    int bank_size;        /* Size of each ecmp member bank. */
    int lkup1_size;       /* Ecmp first lookup size. */
    int lkup2_size;       /* Ecmp second lookup size. */
    int ecmp_if_offset;  /* Ecmp interface min offset.  */
#ifdef BCM_WARM_BOOT_SUPPORT
    soc_scache_handle_t scache_handle;
#endif /* BCM_WARM_BOOT_SUPPORT */
} soc_esw_ecmp_init_info_t;

typedef struct soc_esw_ecmp_group_info_s {
    int ecmp_grp;         /* ECMP Group Index. */
    int ecmp_member_base; /* Ecmp member base. */
    int max_paths;        /* Number of ECMP paths in group. */
    int vp_lag;           /* Update for VP Lags */
    uint32 flags;         /* Ecmp group flags. */
} soc_esw_ecmp_group_info_t;

typedef struct soc_esw_ecmp_member_info_s {
    uint32 dvp;
    uint32 nh;
    uint32 prot_grp;
    uint32 prot_nh;
} soc_esw_ecmp_member_info_t;

typedef struct soc_esw_ecmp_bank_state_s {
    int protected_write_disabled;
    soc_esw_ecmp_bank_lkup_level_t level;
    int idx_min;
    int idx_max;
    int tbl_offset;
    int bank_size;
    SHR_BITDCL idx_used[_SHR_BITDCLSIZE(SOC_ESW_ECMP_BANK_SIZE)];
} soc_esw_ecmp_bank_state_t;

typedef struct soc_esw_ecmp_group_state_s {
    int count;
    int base;
    int bank;
} soc_esw_ecmp_group_state_t;

typedef struct soc_esw_ecmp_state_s {
    sal_mutex_t       soc_esw_ecmp_lock;       /* SOC ECMP lock */
#ifdef BCM_WARM_BOOT_SUPPORT
    soc_scache_handle_t scache_handle;
#endif /* BCM_WARM_BOOT_SUPPORT */


    int ecmp_member_banks;
    int ecmp_members_per_banks;
    int ecmp_grp_if_offset;

    soc_esw_ecmp_bank_state_t ecmp_member_bank_info[SOC_ESW_ECMP_MAX_BANKS];
    soc_esw_ecmp_group_state_t ecmp_group_info[SOC_ESW_ECMP_MAX_GROUPS];

    int ecmp_spare_bank_idx;
} soc_esw_ecmp_state_t;


extern soc_esw_ecmp_state_t *soc_esw_ecmp_state[SOC_MAX_NUM_DEVICES];

#define SOC_ESW_ECMP_STATE(_u_) \
    (soc_esw_ecmp_state[unit])

#define SOC_ESW_ECMP_GROUP_INFO(_u_, _gix_) \
    &(SOC_ESW_ECMP_STATE(unit)->ecmp_group_info[_gix_])

#define SOC_ESW_ECMP_BANK_INFO(_u_, _bix_) \
    &(SOC_ESW_ECMP_STATE(unit)->ecmp_member_bank_info[_bix_])

#define SOC_ESW_ECMP_BANK_PROTECTED_ACCESS(_u_, _bix_) \
    ((SOC_ESW_ECMP_STATE(unit)->ecmp_member_bank_info[_bix_].protected_write_disabled) == 0)

#define SOC_ESW_ECMP_BANK_RSVD_GET(_u_) \
    ((SOC_ESW_ECMP_STATE(unit)->ecmp_spare_bank_idx))

#define SOC_ESW_ECMP_LOCK(unit)    soc_esw_ecmp_state_lock(unit)
#define SOC_ESW_ECMP_UNLOCK(unit)  soc_esw_ecmp_state_unlock(unit)


#if 0
#define SOC_ESW_ECMP_LOCK(unit)                      \
    do {                                             \
        int _rv1;                                    \
        _rv1 = soc_esw_ecmp_state_lock(unit);        \
        SOC_IF_ERROR_RETURN(_rv1);                   \
    } while(0)

#define SOC_ESW_ECMP_UNLOCK(unit)                    \
    do {                                             \
        int _rv1;                                    \
        _rv1 = soc_esw_ecmp_state_unlock(unit);      \
        SOC_IF_ERROR_RETURN(_rv1);                   \
    } while(0)
#endif

extern void
soc_esw_ecmp_init_info_t_init(soc_esw_ecmp_init_info_t *info);

extern void
soc_esw_ecmp_group_info_t_init(soc_esw_ecmp_group_info_t *info);


/*
 * Function:
 *      soc_esw_ecmp_protected_init
 * Purpose:
 *      Initialize state for ecmp protected accesses
 * Parameters:
 *      unit            - (IN)SOC unit number.
 *      init_info       - (IN)ECMP init info.
 * Returns:
 *      SOC_E_XXX
 */
extern int
soc_esw_ecmp_protected_init(int unit,
                            soc_esw_ecmp_init_info_t *init_info);

/*
 * Function:
 *      soc_esw_ecmp_protected_detach
 * Purpose:
 *      De-Initialize state for ecmp protected accesses
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      SOC_E_XXX
 */
extern int
soc_esw_ecmp_protected_detach(int unit);



extern int
soc_esw_ecmp_state_lock(int unit);

extern int
soc_esw_ecmp_state_unlock(int unit);

/*
 * Function:
 *      soc_esw_ecmp_protected_enabled
 * Purpose:
 *      Checks whether protected ecmp is required or not.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      TRUE: Protected ECMP access routines should be used
 *      FALSE: Protected ECMP access routines are not required
 */
extern int
soc_esw_ecmp_protected_enabled(int unit);


extern int
soc_esw_ecmp_protected_banks_get(int unit,
                                 soc_esw_ecmp_bank_lkup_level_t lvl,
                                 uint32 *bank_bitmap);

extern int
soc_esw_ecmp_protected_idx_free_get(int unit,
                                    int bix,
                                    int count,
                                    int *free_idx);
/*
 * Function:
 *      soc_esw_ecmp_protected_bank_defragment
 * Purpose:
 *      Defragment an ecmp bank to fill up holes in
 *      the middle of the ecmp member table and compress.
 * Parameters:
 *      unit            - (IN)  SOC unit number.
 *      bank_idx        - (IN)  bank idx to compress.
 *      grp_idx_move    - (IN)  ecmp group id to write.
 *      free_count      - (OUT) Free count in bank after compression.
 * Returns:
 *      SOC_E_XXX
 */
extern int
soc_esw_ecmp_protected_bank_defragment(int unit,
                                       int bank_idx,
                                       int grp_idx_move,
                                       int *free_count);


/*
 * Function:
 *      soc_esw_ecmp_protected_ser_update
 * Purpose:
 *      Free the bank corresponding to index of errored ECMP memory.
 * Parameters:
 *      unit            - (IN)  SOC unit number.
 *      mem             - (IN)  SOC memory.
 *      ecmp_idx        - (IN)  ECMP memory index.
 * Returns:
 *      SOC_E_XXX
 */
extern int
soc_esw_ecmp_protected_ser_update(int unit,
                                  soc_mem_t mem,
                                  int ecmp_idx);

/*
 * Function:
 *      soc_esw_ecmp_bank_free_count_get
 * Purpose:
 *      Get the number of free entries in this bank.
 * Parameters:
 *      unit            - (IN)  SOC unit number.
 *      bank_idx        - (IN)  Bank index.
 *      count           - (OUT) Free entry count.
 * Returns:
 *      SOC_E_XXX
 */
extern int
soc_esw_ecmp_bank_free_count_get(int unit, int bank_idx, int *count);

/*
 * Function:
 *      soc_esw_ecmp_protected_grp_add
 * Purpose:
 *      Add ecmp group next hop members, or reset ecmp group entry.
 *      NOTE: Function only updates the INITIAL_L3_ECMP and L3_ECMP tables only.
 *            Group tables and other associated tables are updated by the callee.
 *            Function may update group tables of other groups as part of this API though.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      grp_info   - (IN)ecmp group id to write.
 *      nh_list    - (IN)Next hop indexes or NULL for entry reset.
 * Returns:
 *      SOC_E_XXX
 */
extern int
soc_esw_ecmp_protected_grp_add(int unit,
                               soc_esw_ecmp_group_info_t *grp_info,
                               void *nh_list);

/*
 * Function:
 *      soc_esw_ecmp_protected_grp_delete
 * Purpose:
 *      Delete ECMP members
 *      NOTE: Function does not update the INITIAL_L3_ECMP and L3_ECMP tables.
 *            Group tables need to be updated to reflect group delete.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      grp_info   - (IN)ecmp group id to write.
 * Returns:
 *      SOC_E_XXX
 */
extern int
soc_esw_ecmp_protected_grp_delete(int unit,
                                  soc_esw_ecmp_group_info_t *grp_info);


/*
 * Function:
 *      soc_esw_ecmp_protected_grp_get
 * Purpose:
 *      Fetch ecmp members info for a given group
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      grp_info   - (IN)ecmp group id to write.
 *      nh_list    - (OUT)Next hop indexes or NULL for entry reset.
 * Returns:
 *      SOC_E_XXX
 */
extern int
soc_esw_ecmp_protected_grp_get(int unit,
                               soc_esw_ecmp_group_info_t *grp_info,
                               void *nh_list);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
soc_esw_ecmp_protected_state_warmboot_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _SOC_ESW_ECMP_H_ */
