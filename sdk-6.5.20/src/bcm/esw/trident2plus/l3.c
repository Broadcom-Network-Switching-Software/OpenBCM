/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l3.c
 * Purpose:     trident2plus specific L3 function implementations
 */
#include <shared/bsl.h>
#include <soc/defs.h>

#if defined(INCLUDE_L3)
#if defined(BCM_FIREBOLT_SUPPORT)
#include <bcm_int/esw/firebolt.h>
#endif

#ifdef BCM_RIOT_SUPPORT
#include <assert.h>
#include <sal/core/libc.h>
#include <shared/util.h>
#if defined(BCM_FIREBOLT_SUPPORT)
#include <soc/mem.h>
#include <soc/l3x.h>
#include <bcm/l3.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/virtual.h>
#if defined(BCM_TRIDENT_SUPPORT)
#include <bcm_int/esw/trident.h>
#endif /* BCM_TRIDENT_SUPPORT*/
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/nat.h>
#include <bcm_int/esw/qos.h>
#endif /* BCM_TRIDENT_SUPPORT*/
#include <bcm_int/esw/trident2plus.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/failover.h>
#endif
#endif /* BCM_RIOT_SUPPORT */

/* initialize pointer for useful bokkkeeping information */
#define L3_BK_INFO(_unit_)   (&_bcm_l3_bk_info[_unit_])

#ifdef BCM_RIOT_SUPPORT
/* Set ul-ol link params */
#define BCM_L3_NH_NUM_UNDERLAY_BUCKETS    1024     
ul_nh_link_t *ul_nh_assoc_head
    [BCM_MAX_NUM_UNITS][BCM_L3_NH_NUM_UNDERLAY_BUCKETS] = {{NULL}};

/*
 * Function:
 *      bcmi_l3_riot_bank_sel
 * Purpose:
 *      Initialize banks for L3 objects.
 *      The whole l3 table can be divided into 
 *      multiple layers and you can set different banks
 *      for different layers.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int 
bcmi_l3_riot_bank_sel(int unit)
{   
    uint32 bank_sel_reg_val     = 0;
    uint32 num_ol_banks      = 0;
    uint32 l2_tunnel_parse_control = 0;
    _bcm_l3_bookkeeping_t *l3   = L3_BK_INFO(unit);
    uint32 ol_bank_bit_value = 0;
    int iter;
    int num_nh, num_l3_intf;
    int max_nh_banks, max_l3_intf_banks;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    /* The interface entries can only be allocated in terms of 2k
     * banks. if user is allocating in between boundries then we need
     * to throw error as hardware cannot divide overlay and underlay 
     * entries in between bank.
     * If SDK does not return error here then user can start using
     * different entries in same banks for overlay and underlay and may
     * see unextected results.
     */
    if (l3->l3_intf_overlay_entries % BCMI_L3_INTF_ENT_PER_BANK(unit)) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "Please allocate entries in the multiple of %d.\n"), 
            BCMI_L3_INTF_ENT_PER_BANK(unit)));

        return BCM_E_PARAM;
    }

    if (l3->l3_nh_overlay_entries %  BCMI_L3_NH_ENT_PER_BANK(unit)) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "Please allocate entries in the multiple of %d.\n"), 
            BCMI_L3_NH_ENT_PER_BANK(unit)));

        return BCM_E_PARAM;
    }
    num_nh = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    max_nh_banks = num_nh / BCMI_L3_NH_ENT_PER_BANK(unit);
    max_l3_intf_banks = num_l3_intf / BCMI_L3_INTF_ENT_PER_BANK(unit);

    if (max_nh_banks == 0) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "No NH banks in system.\n")));
        return BCM_E_RESOURCE;
    }
    if (max_l3_intf_banks == 0) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "No EGR_L3_INTF banks in system.\n")));
        return BCM_E_RESOURCE;
    }
    /* 
     * set banks for EGR_L3_INTF 
     * By default ("alloc mode" is 0):
     * First set of banks are for overlay.
     * Second set of banks are for underlay.
     * If "alloc mode" is 1:
     * First set of banks are for underlay.
     * Second set of banks are for overlay.
     */
    num_ol_banks = (l3->l3_intf_overlay_entries /
        BCMI_L3_INTF_ENT_PER_BANK(unit)) +
        ((l3->l3_intf_overlay_entries % BCMI_L3_INTF_ENT_PER_BANK(unit)) ? 1 :
        0);

    if (num_ol_banks > max_l3_intf_banks) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "ERROR : Overlay l3 interface(%d) > total interfaces(%d).\n"),
            l3->l3_intf_overlay_entries,
            max_l3_intf_banks * BCMI_L3_INTF_ENT_PER_BANK(unit)));

        return BCM_E_CONFIG;
    }

    /* Setup the bits for overlay banks. */
    for (iter = 0; iter < num_ol_banks; iter++) {
        ol_bank_bit_value |= (1 << iter) ;
    }
    if (l3->l3_intf_overlay_alloc_mode > 0) {
        ol_bank_bit_value <<= (max_l3_intf_banks - num_ol_banks);
    }

    soc_reg_field_set(unit, EGR_L3_INTF_BANK_SELr, &bank_sel_reg_val,
        BANK_SELf, ol_bank_bit_value);
    /* set banks in EGR_L3_INTF_BANK_SELr */
    SOC_IF_ERROR_RETURN(WRITE_EGR_L3_INTF_BANK_SELr
                          (unit, bank_sel_reg_val));

    ol_bank_bit_value = 0;
    bank_sel_reg_val     = 0;
    /* 
     * set banks for ING/EGR/INITIAL_ING_L3_NEXT_HOP. 
     * By default ("alloc mode" is 0):
     * First set of banks are for overlay.
     * Second set of banks are for underlay.
     * If "alloc mode" is 1:
     * First set of banks are for underlay.
     * Second set of banks are for overlay.
     */
    num_ol_banks = (l3->l3_nh_overlay_entries / BCMI_L3_NH_ENT_PER_BANK(unit)) +
        ((l3->l3_nh_overlay_entries % BCMI_L3_NH_ENT_PER_BANK(unit)) ? 1 : 0);
    if (l3->l3_nh_reserve_for_ecmp) {
        max_nh_banks -=
            (BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) / BCMI_L3_ECMP_PER_BANK(unit))
            + ((BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) %
            BCMI_L3_ECMP_PER_BANK(unit)) ? 1 : 0);
    }
    if (num_ol_banks > max_nh_banks) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "ERROR : Overlay l3 Next hop (%d) > total l3 next hop(%d).\n"),
            l3->l3_nh_overlay_entries,
            max_nh_banks * BCMI_L3_NH_ENT_PER_BANK(unit)));

        return BCM_E_CONFIG;
    }
    if (l3->l3_nh_reserve_for_ecmp && (l3->l3_nh_overlay_alloc_mode == 0)) {
        num_ol_banks +=
            (BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) / BCMI_L3_ECMP_PER_BANK(unit))
            + ((BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) %
            BCMI_L3_ECMP_PER_BANK(unit)) ? 1 : 0);
    }
    for (iter=0; iter < num_ol_banks; iter++) {
        ol_bank_bit_value |= (1 << iter) ;
    }
    if (l3->l3_nh_overlay_alloc_mode > 0) {
        if (l3->l3_nh_reserve_for_ecmp) {
            ol_bank_bit_value <<= (max_nh_banks - num_ol_banks +
                (BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) /
                BCMI_L3_ECMP_PER_BANK(unit)) +
                ((BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) %
                BCMI_L3_ECMP_PER_BANK(unit)) ? 1 : 0));
        } else {
            ol_bank_bit_value <<= (max_nh_banks - num_ol_banks);
        }
    }

    /* set banks in INITIAL_ING_L3_NEXT_HOP_BANK_SELr */
    soc_reg_field_set(unit, INITIAL_ING_L3_NEXT_HOP_BANK_SELr,
        &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);

    SOC_IF_ERROR_RETURN(WRITE_INITIAL_ING_L3_NEXT_HOP_BANK_SELr
        (unit, bank_sel_reg_val));

    /* set banks in ING_L3_NEXT_HOP_BANK_SELr */
    soc_reg_field_set(unit, ING_L3_NEXT_HOP_BANK_SELr,
        &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);

    SOC_IF_ERROR_RETURN(WRITE_ING_L3_NEXT_HOP_BANK_SELr
        (unit, bank_sel_reg_val));

    /* set banks in EGR_L3_NEXT_HOP_BANK_SELr */
    soc_reg_field_set(unit, EGR_L3_NEXT_HOP_BANK_SELr,
        &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);
    SOC_IF_ERROR_RETURN(WRITE_EGR_L3_NEXT_HOP_BANK_SELr
        (unit, bank_sel_reg_val));

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit) &&
        (soc_property_get(unit, spn_L3_ECMP_LEVELS, 1) == 2) &&
        (l3->l3_ecmp_group_first_lkup_mem_size ||
         l3->l3_ecmp_member_first_lkup_mem_size)) {
        uint32 rval = 0, rval2 = 0;
        int ecmp_level_sel_reg_val = (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit) ? 1 : 0);
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf,
                          ecmp_level_sel_reg_val);
        soc_reg_field_set(unit, ECMP_CONFIG_2r, &rval2, ECMP_MODEf,
                          ecmp_level_sel_reg_val);

        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIG_2r(unit, rval2));

        if (l3->l3_ecmp_group_first_lkup_mem_size <= BCM_XGS3_L3_ECMP_MAX_GROUPS(unit)) {
            ol_bank_bit_value = (1 <<
                    ((l3->l3_ecmp_group_first_lkup_mem_size / BCMI_L3_ECMP_GROUP_PER_BANK(unit)) +
                     ((l3->l3_ecmp_group_first_lkup_mem_size % BCMI_L3_ECMP_GROUP_PER_BANK(unit)) ?
                      1 : 0))) - 1;
#if defined(BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_fb6_ecmp_group_partitioning)) {
               if (l3->l3_ecmp_group_first_lkup_mem_size ==
                   BCMI_L3_ECMP_GROUP_FIRST_LKUP_MEM_SIZE_B) {
                   /* For 2K ECMP groups in first lookup,
                    * we need to use the upper 4 banks instead
                    * of starting from lowest bank.Also need to set
                    * the two unused banks for first lookup in this case
                    */
                   ol_bank_bit_value = (1 <<
                           ((l3->l3_ecmp_group_first_lkup_mem_size / BCMI_L3_ECMP_GROUP_PER_BANK(unit)) +
                            2)) - 1;
                   ol_bank_bit_value <<= BCMI_L3_ECMP_MAX_BANKS(unit) -
                       ((l3->l3_ecmp_group_first_lkup_mem_size / BCMI_L3_ECMP_GROUP_PER_BANK(unit) + 2));
               }
            }
#endif
            soc_reg_field_set(unit, ING_L3_ECMP_GROUP_BANK_SELr,
                    &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);
            SOC_IF_ERROR_RETURN(WRITE_ING_L3_ECMP_GROUP_BANK_SELr
                    (unit, bank_sel_reg_val));
            soc_reg_field_set(unit, INITIAL_ING_L3_ECMP_GROUP_BANK_SELr,
                    &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);
            SOC_IF_ERROR_RETURN(WRITE_INITIAL_ING_L3_ECMP_GROUP_BANK_SELr
                    (unit, bank_sel_reg_val));
        } else {
            LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
                            "ERROR : l3 ecmp group (%d) > total l3 ecmp group(%d).\n"),
                        l3->l3_ecmp_group_first_lkup_mem_size,
                        BCM_XGS3_L3_ECMP_MAX_GROUPS(unit)));

            return BCM_E_CONFIG;
        }

        if (l3->l3_ecmp_member_first_lkup_mem_size <= (soc_mem_index_count(unit,
            L3_ECMPm))) {
            ol_bank_bit_value = (1 <<
                    ((l3->l3_ecmp_group_first_lkup_mem_size / BCMI_L3_ECMP_PER_BANK(unit)) +
                     ((l3->l3_ecmp_group_first_lkup_mem_size % BCMI_L3_ECMP_PER_BANK(unit)) ?
                      1 : 0))) - 1;
            soc_reg_field_set(unit, ING_L3_ECMP_BANK_SELr,
                    &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);
            SOC_IF_ERROR_RETURN(WRITE_ING_L3_ECMP_BANK_SELr
                    (unit, bank_sel_reg_val));
            soc_reg_field_set(unit, INITIAL_ING_L3_ECMP_BANK_SELr,
                    &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);
            SOC_IF_ERROR_RETURN(WRITE_INITIAL_ING_L3_ECMP_BANK_SELr
                    (unit, bank_sel_reg_val));
        } else {
            LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
                            "ERROR : l3 ecmp group (%d) > total l3 ecmp group(%d).\n"),
                        l3->l3_ecmp_group_first_lkup_mem_size,
                        BCM_XGS3_L3_ECMP_MAX(unit)));

            return BCM_E_CONFIG;
        }
    }
#endif

    /* Enable the control to look into inner l3 header for RioT */
    soc_reg_field_set(unit, ING_L2_TUNNEL_PARSE_CONTROLr,
        &l2_tunnel_parse_control, IFP_L2_TUNNEL_PAYLOAD_FIELD_SELf, 0x1);
    soc_reg_field_set(unit, ING_L2_TUNNEL_PARSE_CONTROLr,
        &l2_tunnel_parse_control, PARSE_IPV4_PAYLOADf, 0x1);
    soc_reg_field_set(unit, ING_L2_TUNNEL_PARSE_CONTROLr,
        &l2_tunnel_parse_control, PARSE_IPV6_PAYLOADf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_ING_L2_TUNNEL_PARSE_CONTROLr
        (unit, l2_tunnel_parse_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_l3_riot_ecmp_level_sel
 * Purpose:
 *      Initialize banks for L3 objects.
 *      The whole l3 table can be divided into 
 *      multiple layers and you can set different banks
 *      for different layers.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int
bcmi_l3_riot_ecmp_level_sel(int unit)
{   
    uint32 ecmp_level_sel_reg_val = 0;
    uint32 rval = 0;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 rval2 = 0;
    uint32 ecmp_bank_sel_reg_val = 0;
#endif

    /* 
     * If ecmp level is multi level then set ecmp config register to 1.
     * else set the ecmp config register to 0. 0 is default case for
     * legacy single level of ecmp.
     */
    ecmp_level_sel_reg_val = (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit) ? 1 : 0);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_riot)) {
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf,
                ecmp_level_sel_reg_val);
        soc_reg_field_set(unit, ECMP_CONFIG_2r, &rval2, ECMP_MODEf,
                ecmp_level_sel_reg_val);

        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIG_2r(unit, rval2));

        if (ecmp_level_sel_reg_val) {
            /* Divide ECMP entries into two equal halves for two level resolution */
            /* Need to provide option for bank selection later */
            /* Maximum number of ECMP group and member banks is 8 in TD3 and 4
             * in HX5.
             */
            soc_reg_field_set(unit, INITIAL_ING_L3_ECMP_BANK_SELr,
                              &ecmp_bank_sel_reg_val, BANK_SELf,
                              ((1 << (soc_reg_field_length(unit,
                              INITIAL_ING_L3_ECMP_BANK_SELr, BANK_SELf) / 2))
                              - 1));
            SOC_IF_ERROR_RETURN(WRITE_INITIAL_ING_L3_ECMP_BANK_SELr
                                (unit, ecmp_bank_sel_reg_val));

            soc_reg_field_set(unit, ING_L3_ECMP_BANK_SELr,
                              &ecmp_bank_sel_reg_val, BANK_SELf,
                              ((1 << (soc_reg_field_length(unit,
                              ING_L3_ECMP_BANK_SELr, BANK_SELf) / 2))
                              - 1));
            SOC_IF_ERROR_RETURN(WRITE_ING_L3_ECMP_BANK_SELr
                                (unit, ecmp_bank_sel_reg_val));

            ecmp_bank_sel_reg_val = 0;

            soc_reg_field_set(unit, INITIAL_ING_L3_ECMP_GROUP_BANK_SELr,
                              &ecmp_bank_sel_reg_val, BANK_SELf,
                              ((1 << (soc_reg_field_length(unit,
                              INITIAL_ING_L3_ECMP_GROUP_BANK_SELr, BANK_SELf)
                              / 2)) - 1));
            SOC_IF_ERROR_RETURN(WRITE_INITIAL_ING_L3_ECMP_GROUP_BANK_SELr
                                (unit, ecmp_bank_sel_reg_val));

            soc_reg_field_set(unit, ING_L3_ECMP_GROUP_BANK_SELr,
                              &ecmp_bank_sel_reg_val, BANK_SELf,
                              ((1 << (soc_reg_field_length(unit,
                              ING_L3_ECMP_GROUP_BANK_SELr, BANK_SELf) / 2))
                              - 1));
            SOC_IF_ERROR_RETURN(WRITE_ING_L3_ECMP_GROUP_BANK_SELr
                                (unit, ecmp_bank_sel_reg_val));

        }
    } else
#endif
    {
    soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_CONFIGURATIONf,
        ecmp_level_sel_reg_val);

    SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *       bcmi_l3_nh_assoc_ol_ul_link_hash_dump 
 * Purpose:
 *       dumps the whole hash table
 * Parameters:
 *           IN :  Unit
 * Returns:
 *        void
 */
void bcmi_l3_nh_assoc_ol_ul_link_hash_dump(int unit)
{   

    ol_nh_link_t *temp;
    ul_nh_link_t *hash;
    int i = 0;

    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
        "printing nh association hash table\n")));

    for (; i< BCM_L3_NH_NUM_UNDERLAY_BUCKETS; i++) {
        hash =  ul_nh_assoc_head[unit][i];
        while (hash != NULL) {
            temp = hash->ol_nh_assoc;
            while (temp != NULL) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "bkt_idx = %d : ol index = %d : ul index = %d \n"),
                    i, hash->u_nh_idx, temp->o_nh_idx));
                temp = temp->next_link;
            }
            hash = hash->ul_nh_link;
        }
    }
}

/*
 * Function:
 *      bcmi_nh_assoc_alloc_ol_link 
 * Purpose:
 *      Allocates link memory and sets overlay nh.
 * Parameters:
 *      ol_nh - overlay nh number
 * Returns:
 *      pointer to allocated link memory              
 */

ol_nh_link_t *
bcmi_nh_assoc_alloc_ol_link(int ol_nh)
{
    ol_nh_link_t *ptr ;

    ptr = (ol_nh_link_t *) sal_alloc(sizeof(ol_nh_link_t), "ol_nh_link" );

    sal_memset(ptr, 0, sizeof(ol_nh_link_t));
    ptr->o_nh_idx = ol_nh;
    return ptr;
}

/*
 * Function:
 *      bcmi_nh_assoc_alloc_ul_link 
 * Purpose:
 *      Allocates link memory and sets underlay nh.
 * Parameters:
 *      ul_nh - underlay nh number
 * Returns:
 *      pointer to allocated link memory              
 */
ul_nh_link_t *
bcmi_nh_assoc_alloc_ul_link(int ul_nh)
{   
    ul_nh_link_t *ptr;

    ptr = (ul_nh_link_t *) sal_alloc(sizeof(ul_nh_link_t), "ul_nh_link");
    sal_memset(ptr, 0, sizeof(ul_nh_link_t));

    ptr->u_nh_idx = ul_nh;
    return ptr;
}


/*
 * Function:
 *      bcmi_nh_assoc_bkt_get
 * Purpose:
 *      Adds a overlay/underlay association.
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_nh_assoc_bkt_get(int nh_idx)
{


    return( _shr_crc16b(0, (uint8 *)(&nh_idx),
        BYTES2BITS(sizeof(nh_idx))) %
        BCM_L3_NH_NUM_UNDERLAY_BUCKETS);

}
/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_sw_add
 * Purpose:
 *      Adds a overlay/underlay association.
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_sw_add(int unit, int ol_nh, int ul_nh)
{   

    ul_nh_link_t *ul_assoc, *prev_assoc, *ul_alloc;
    ol_nh_link_t *ol_link, *prev_link, *ol_alloc;
    int bkt_idx = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    bkt_idx = bcmi_nh_assoc_bkt_get(ul_nh);

    prev_assoc = ul_assoc = ul_nh_assoc_head[unit][bkt_idx];
    while (ul_assoc && (ul_assoc->u_nh_idx != ul_nh)) {
        prev_assoc = ul_assoc;
        ul_assoc = ul_assoc->ul_nh_link;
    }

    if (ul_assoc) {
        prev_link = ol_link = ul_assoc->ol_nh_assoc;
        while (ol_link && (ol_link->o_nh_idx != ol_nh)) {
            prev_link = ol_link;
            ol_link = ol_link->next_link;
        }
        if (!ol_link) {
            ol_alloc =  bcmi_nh_assoc_alloc_ol_link(ol_nh);
            if (ol_alloc == NULL) {
                return BCM_E_MEMORY;
            }
            prev_link->next_link = ol_alloc;
        }
    } else {

        ul_alloc = bcmi_nh_assoc_alloc_ul_link(ul_nh);
        if (ul_alloc == NULL) {
            return BCM_E_MEMORY;
        }
        ol_alloc = bcmi_nh_assoc_alloc_ol_link(ol_nh);
        if (ol_alloc == NULL) {
            sal_free(ul_alloc);
            return BCM_E_MEMORY;
        }

        if (prev_assoc) {
            prev_assoc->ul_nh_link = ul_alloc;
            prev_assoc->ul_nh_link->ol_nh_assoc = ol_alloc;
        } else {
            ul_nh_assoc_head[unit][bkt_idx] = ul_alloc;
            ul_nh_assoc_head[unit][bkt_idx]->ol_nh_assoc = ol_alloc;
        }
    }
    /* 
     * increase the ref count of second next hop so that it
     * not deleted in case someone tries to delete it. 
     */
    BCM_XGS3_L3_ENT_REF_CNT_INC 
        (BCM_XGS3_L3_TBL_PTR(unit, next_hop), ul_nh,
        _BCM_SINGLE_WIDE); 

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_l3_nh_del_nh_nh_assoc 
 * Purpose:
 *      Encode the dstination value based on the gport.
 * Parameters:
 *      unit - SOC device unit number
 *      nh_idx - next hop index
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int bcmi_l3_nh_assoc_ol_ul_link_delete(int unit, int nh_idx)
{
    soc_mem_t mem;                        /* Table location memory. */
    egr_l3_next_hop_entry_t  egr_entry;   /* Egress next hop entry. */
    int ent_type, next_ptr_type, next_nh_idx;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    mem = EGR_L3_NEXT_HOPm;
    sal_memset(&egr_entry, 0, sizeof(egr_l3_next_hop_entry_t));

    SOC_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, EGR_L3_NEXT_HOPm,
                                          nh_idx, &egr_entry));

    if (soc_feature(unit, soc_feature_mem_access_data_type)) {
        ent_type = soc_mem_field32_get(unit, mem, &egr_entry, DATA_TYPEf);
    } else {
    ent_type = soc_mem_field32_get(unit, mem, &egr_entry, ENTRY_TYPEf);
    }

    /* chk if the entry is L3MC */
    if (ent_type == BCMI_L3_EGR_NH_MCAST_ENTRY_TYPE) {
        next_ptr_type = soc_mem_field32_get
            (unit, mem, &egr_entry, L3MC__NEXT_PTR_TYPEf);

        if (next_ptr_type  == BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH) {
            next_nh_idx = soc_mem_field32_get(unit, mem, &egr_entry,
                              L3MC__NEXT_PTRf);

            BCM_IF_ERROR_RETURN(
                bcmi_l3_nh_assoc_ol_ul_link_sw_delete(unit, nh_idx, 
                next_nh_idx));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_sw_delete
 * Purpose:
 *      Deletes a overlay/underlay association.
 *      If there is only one underlay nh link, 
 *      then remove the overlay link also.
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_sw_delete(int unit, int ol_nh, int ul_nh)
{
    ul_nh_link_t *ul_assoc, *prev_assoc;
    ol_nh_link_t *link_nh, *prev_link;
    int bkt_idx = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    bkt_idx = bcmi_nh_assoc_bkt_get(ul_nh);

    prev_assoc = ul_assoc = ul_nh_assoc_head[unit][bkt_idx];

    while (ul_assoc && (ul_assoc->u_nh_idx != ul_nh)) {
        prev_assoc = ul_assoc;
        ul_assoc = ul_assoc->ul_nh_link;
    }

    if (ul_assoc) {
        prev_link = link_nh = ul_assoc->ol_nh_assoc;
        while (link_nh && (link_nh->o_nh_idx != ol_nh)) {
            prev_link = link_nh;
            link_nh = link_nh->next_link;
        }

        if (link_nh) {
            if (link_nh == ul_assoc->ol_nh_assoc) {
                ul_assoc->ol_nh_assoc = link_nh->next_link;
            } else {
                prev_link->next_link = link_nh->next_link;
            }
            sal_free(link_nh);
        } else {
            /* send an error here */
            return BCM_E_NOT_FOUND;
        }
    }

    if (ul_assoc && (ul_assoc->ol_nh_assoc == NULL)) {
        if (ul_assoc == ul_nh_assoc_head[unit][bkt_idx]) {
            ul_nh_assoc_head[unit][bkt_idx] = ul_assoc->ul_nh_link;
        } else {
            prev_assoc->ul_nh_link = ul_assoc->ul_nh_link;
        }
        sal_free(ul_assoc);
    } else if (ul_assoc == NULL) {
        return BCM_E_NOT_FOUND;
    }
    /* 
     * decrease the ref count of second next hop so that it
     * not deleted in case someone tries to delete it. 
     */
    BCM_XGS3_L3_ENT_REF_CNT_DEC 
        (BCM_XGS3_L3_TBL_PTR(unit, next_hop), ul_nh,
        _BCM_SINGLE_WIDE); 

 return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_replace
 * Purpose:
 *      Replace overlay/underlay association.
 *
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_replace(int unit, int old_ul, int new_ul)
{   

    int entry_type, next_ptr_type;
    egr_l3_next_hop_entry_t egr_nh;
    ul_nh_link_t *ul_assoc;
    ol_nh_link_t *ol_link;
    int bkt_idx = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    bkt_idx = bcmi_nh_assoc_bkt_get(old_ul);

    if (ul_nh_assoc_head[unit][bkt_idx] == NULL) {
        /* There is nothing to replace */
        return BCM_E_NONE;
    }
    ul_assoc = ul_nh_assoc_head[unit][bkt_idx];
    while (ul_assoc && (ul_assoc->u_nh_idx != old_ul)) {
        ul_assoc = ul_assoc->ul_nh_link;
    }
    /* replace underlay nh index */
    if (ul_assoc) {
        ol_link = ul_assoc->ol_nh_assoc;
        while (ol_link) {

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                ol_link->o_nh_idx, &egr_nh));
            if (soc_feature(unit, soc_feature_mem_access_data_type)) {
                entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                 &egr_nh, DATA_TYPEf);
            } else {
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, ENTRY_TYPEf);
            }
            if (entry_type == BCMI_L3_EGR_NH_MCAST_ENTRY_TYPE) {
                next_ptr_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &egr_nh, L3MC__NEXT_PTR_TYPEf);
                if (next_ptr_type == BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH) {

                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__NEXT_PTR_TYPEf, BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__NEXT_PTRf, new_ul);

                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                        MEM_BLOCK_ALL, ol_link->o_nh_idx, &egr_nh));
                }
            }
        /* For all overlay NHs, replace the underlay NH. */
        bcmi_l3_nh_assoc_ol_ul_link_sw_delete(unit, ol_link->o_nh_idx, 
            old_ul);
        bcmi_l3_nh_assoc_ol_ul_link_sw_add(unit, ol_link->o_nh_idx, 
            new_ul);
        ol_link = ol_link->next_link;
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_is_required 
 * Purpose:
 *      check if nh-nh association is needed.
 * Parameters:
 *      unit        - unit number
 *      vp nh index - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_is_required(int unit, int vp_nh_index)
{ 
   /* int entry_type = 0;
    egr_l3_next_hop_entry_t egr_nh;
    int action_present=0, action_not_present=0, vntag_actions = 0;*/

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }
#if 0
    /*For TD3, the overlay nh can't be updated easily when underlay nh is changed*/
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                  vp_nh_index, &egr_nh));
    if (soc_feature(unit, soc_feature_mem_access_data_type)) {
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf);
    } else {
    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
    }
    /* only TD3 can have no underlay sd_tag. For TD2+, SRC MAC will not be modified */
    if (SOC_IS_TRIDENT3X(unit) && (entry_type == BCMI_L3_EGR_NH_SD_TAG_ENTRY_TYPE)) {
        /* check if sdtag egr nh has some tag actions */
        action_present =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
        action_not_present =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
        vntag_actions =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__VNTAG_ACTIONSf);

        if (vntag_actions || action_present || action_not_present) {
            /* 
             * this next hop can do some modifications and therefore
             * we should set nh-nh association for this NH.
             */
            return 1;
        } else {
            return 0;
        }
    }
#endif
    return 1;
}

/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_reinit
 * Purpose:
 *      re-initializes overlay/underlay association.
 * Parameters:
 *      unit     - unit number
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_reinit(int unit, int ol_nh_idx)
{
    int entry_type, next_ptr_type, ul_nh_idx;
    egr_l3_next_hop_entry_t egr_nh;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, ol_nh_idx, &egr_nh));

    if (soc_feature(unit, soc_feature_mem_access_data_type)) {
       entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf);
    } else {
    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
    }

    if (entry_type == BCMI_L3_EGR_NH_MCAST_ENTRY_TYPE) { 
        next_ptr_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
            &egr_nh, L3MC__NEXT_PTR_TYPEf);
        if (next_ptr_type == BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH) {
        
            ul_nh_idx =  soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, L3MC__NEXT_PTRf);

            BCM_IF_ERROR_RETURN(
                bcmi_l3_nh_assoc_ol_ul_link_sw_add(unit, ol_nh_idx, 
                ul_nh_idx));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_nh_destination_set
 * Purpose:
 *      Encode the dstination value based on the gport.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */

int bcmi_l3_nh_dest_set(int unit, bcm_l3_egress_t *nh_entry, uint32 *nh_dest)
{
    int gport_type = 0, port = -1, gport = -1;
    int mc_group = 0;

    if(nh_entry->flags2 & BCM_L3_FLAGS2_MC_GROUP){
        if (!_BCM_MULTICAST_IS_L3(nh_entry->mc_group)) {
            return BCM_E_PARAM;
        }
        mc_group = _BCM_MULTICAST_ID_GET(nh_entry->mc_group);
        *nh_dest = BCMI_L3_NH_DEST_IPMC_GET(mc_group);
        return BCM_E_NONE;
    }

    gport = nh_entry->port;
    gport_type = BCMI_GPORT_TYPE_GET(gport);

    if (BCMI_RIOT_IS_ENABLED(unit)) {

        switch(gport_type) {
        case BCM_GPORT_VXLAN_PORT:
            port = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
            *nh_dest =  BCMI_L3_NH_DEST_DVP_GET(port);
        break;
        case BCM_GPORT_L2GRE_PORT:
            port = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
            *nh_dest =  BCMI_L3_NH_DEST_DVP_GET(port);
        break;
        case BCM_GPORT_MPLS_PORT:
            port = BCM_GPORT_MPLS_PORT_ID_GET(gport);
            *nh_dest =  BCMI_L3_NH_DEST_DVP_GET(port);
        break;
        case BCM_GPORT_MIM_PORT:
            port = BCM_GPORT_MIM_PORT_ID_GET(gport);
            *nh_dest =  BCMI_L3_NH_DEST_DVP_GET(port);
        break;
        default:
            if (nh_entry->flags & BCM_L3_TGID) {
                int tid_is_vp_lag = 0;
                if (soc_feature(unit, soc_feature_vp_lag)) {
                    (void)_bcm_esw_trunk_id_is_vp_lag(unit, gport, &tid_is_vp_lag);
                }

                if (tid_is_vp_lag) {
                    int vp_lag_vp;
                    /* Get the VP value representing VP LAG */
                    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_tid_to_vp_lag_vp(unit,
                        gport, &vp_lag_vp));

                    *nh_dest = BCMI_L3_NH_DEST_DVP_GET(vp_lag_vp);
                } else {
                    *nh_dest = BCMI_L3_NH_DEST_LAG_GET(nh_entry->port);
                }
            } else {
                *nh_dest =  BCMI_L3_NH_DEST_DGLP_GET
                    (BCM_L3_DGLP_GET(nh_entry->module, nh_entry->port));
            }
        }
    } else {
        if (nh_entry->flags & BCM_L3_TGID) {
            *nh_dest = BCMI_L3_NH_DEST_LAG_GET(nh_entry->port);
        } else {
            *nh_dest =  BCMI_L3_NH_DEST_DGLP_GET
                (BCM_L3_DGLP_GET(nh_entry->module, nh_entry->port));
        }
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit) &&
            soc_feature(unit, soc_feature_xflow_macsec) &&
            ((nh_entry->flags2 & BCM_L3_FLAGS2_XFLOW_MACSEC_ENCRYPT) ||
            (nh_entry->flags2 & BCM_L3_FLAGS2_XFLOW_MACSEC_DECRYPT))) {
            int local_modid;
            BCM_IF_ERROR_RETURN
                (bcm_esw_stk_my_modid_get (unit, &local_modid));
            *nh_dest =  BCMI_L3_NH_DEST_DGLP_GET
                        (BCM_L3_DGLP_GET(local_modid, 66));
        }
#endif
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_get_gport_from_destination
 * Purpose:
 *      Encode the dstination value based on the gport.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int bcmi_get_port_from_destination(int unit, uint32 dest,
                                        bcm_l3_egress_t *nh)
{
    int vp;

    if (BCMI_L3_NH_DEST_IS_LAG(dest)) {
        nh->port = BCMI_L3_NH_PORT_LAG_GET(dest);
        nh->flags |= BCM_L3_TGID;
    } else if (BCMI_L3_NH_DEST_IS_DGLP(dest)) {
        nh->port = BCMI_L3_NH_PORT_DGLP_GET(dest);
    } else if (BCMI_L3_NH_DEST_IS_DVP(dest)) {
        vp = BCMI_L3_NH_PORT_DVP_GET(dest);
        _bcm_vp_encode_gport(unit, vp, &(nh->port));
    } else if (BCMI_L3_NH_DEST_IS_IPMC(dest)) {
        _BCM_MULTICAST_GROUP_SET(nh->mc_group, _BCM_MULTICAST_TYPE_L3,
                                 BCMI_L3_NH_PORT_IPMC_GET(dest));
        nh->flags2 |= BCM_L3_FLAGS2_MC_GROUP;
    } else {
        nh->port = dest;
    }

    return BCM_E_NONE;

}

static soc_profile_mem_t *_bcm_td2p_macda_oui_profile[BCM_MAX_NUM_UNITS] = {NULL};

/*
 * Function:
 *      _bcm_td2p_macda_oui_profile_init
 * Purpose:
 *      Allocate and initialize _bcm_td2p_macda_oui_profile_init
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_td2p_macda_oui_profile_init(int unit)
{
    soc_mem_t mem_profile;
    int entry_words[1];

    _bcm_td2p_macda_oui_profile_deinit(unit);

    /* Create profile for MACDA_OUI_PROFILE table*/
    _bcm_td2p_macda_oui_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "MACDA OUI Profile Mem");
    if (_bcm_td2p_macda_oui_profile[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    soc_profile_mem_t_init(_bcm_td2p_macda_oui_profile[unit]);
    mem_profile = EGR_MACDA_OUI_PROFILEm;
    entry_words[0] = BYTES2WORDS(sizeof(egr_macda_oui_profile_entry_t));
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem_profile, entry_words, 1,
                                _bcm_td2p_macda_oui_profile[unit]));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_macda_oui_profile_deinit
 * Purpose:
 *      Deallocate _bcm_td2p_macda_oui_profile
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_X_XXX
 */
void
_bcm_td2p_macda_oui_profile_deinit(int unit)
{
    if (_bcm_td2p_macda_oui_profile[unit]) {
        (void)soc_profile_mem_destroy(unit, _bcm_td2p_macda_oui_profile[unit]);
        sal_free(_bcm_td2p_macda_oui_profile[unit]);
        _bcm_td2p_macda_oui_profile[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_td2p_l3_macda_oui_profile_entry_del
 * Purpose:
 *      Delete entry from profile table
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      ipv6     - (IN) IPV4 or IPv6 flag
 *      l3cfg    - (IN) L3 entry info.
 *      bufentry - (IN) hw buffer
 * Returns:
 */
bcm_error_t
_bcm_td2p_l3_macda_oui_profile_entry_del(int unit, _bcm_l3_cfg_t *l3cfg,
                                         int macda_oui_profile_index)
{
    /* delete an entry or decrement ref count from EGR_MACDA_OUI_PROFILE table */
    if (macda_oui_profile_index != -1) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_delete(unit, _bcm_td2p_macda_oui_profile[unit],
                                                   macda_oui_profile_index));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_l3_macda_oui_profile_entry_parse
 * Purpose:
 *      TD2P helper routine used to parse hw l3 entry to api format
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN) L3 entry info.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 */
bcm_error_t
_bcm_td2p_l3_macda_oui_profile_entry_parse(int unit, soc_mem_t mem,
                                           _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{
    uint32 profile_index;
    uint32 mac_field;
    int rv = BCM_E_NONE;
    int ipv6;                     /* Entry is IPv6 flag.         */
    _bcm_l3_fields_t *fld;        /* L3 table common fields.     */
    uint32 *buf_p;                /* HW buffer address.          */
    egr_macda_oui_profile_entry_t macda_oui_profile_entry;

    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    buf_p = (uint32 *)l3x_entry;

    /* Set table fields */
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    soc_mem_mac_address_get(unit, mem, buf_p, fld->macda_lsb,
                            l3cfg->l3c_mac_addr,
                            SOC_MEM_MAC_LOWER_ONLY);
    /* Read upper 3 bytes of MAC address from profile table */
    profile_index = soc_mem_field32_get(unit, mem, buf_p,
                                              fld->oui_profile_index);
    SOC_IF_ERROR_RETURN
        (READ_EGR_MACDA_OUI_PROFILEm(unit, MEM_BLOCK_ANY, profile_index,
                                     &macda_oui_profile_entry));
    soc_EGR_MACDA_OUI_PROFILEm_field_get(unit, &macda_oui_profile_entry,
                                         MACDA_OUIf, &mac_field);
    l3cfg->l3c_mac_addr[0] = (uint8) (mac_field >> 16 & 0xff);
    l3cfg->l3c_mac_addr[1] = (uint8) (mac_field >> 8 & 0xff);
    l3cfg->l3c_mac_addr[2] = (uint8) (mac_field & 0xff);

    return rv;
}


/*
 * Function:
 *      _bcm_td2p_l3_macda_oui_profile_entry_add
 * Purpose:
 *      TD2P helper routine used to set some fields in hw l3 entry
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN) L3 entry info.
 *      buf_p     - (IN) hw buffer.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_td2p_l3_macda_oui_profile_entry_add(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                                         uint32 *buf_p, int *macda_oui_profile_index)
{
    int rv = BCM_E_NONE;
    int ipv6;                     /* Entry is IPv6 flag.         */
    _bcm_l3_fields_t *fld;        /* L3 table common fields.     */
    uint32 *bufp;                 /* HW buffer address.          */
    uint32 profile_idx;
    uint32 mac_oui;
    void *entries[1];
    egr_macda_oui_profile_entry_t macda_oui_profile_entry;

    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    /* Set table fields */
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    bufp = buf_p;

    /* Set MACDA_OUI_PROFILE_VALID to 1 */
    soc_mem_field32_set(unit, mem, bufp, fld->oui_profile_valid, 1);

    /* Set the MACDA_LSB field */
    soc_mem_mac_address_set(unit, mem, bufp, fld->macda_lsb,
                            l3cfg->l3c_mac_addr,
                            SOC_MEM_MAC_LOWER_ONLY);
    /* Setup EGR_MACDA_OUI_PROFILE table with MACDA_OUI */
    mac_oui = ((l3cfg->l3c_mac_addr[0] << 16) |
               (l3cfg->l3c_mac_addr[1] << 8)  |
               (l3cfg->l3c_mac_addr[2] << 0));
    sal_memset(&macda_oui_profile_entry, 0, sizeof(macda_oui_profile_entry));
    soc_EGR_MACDA_OUI_PROFILEm_field_set(unit, &macda_oui_profile_entry,
                                         MACDA_OUIf, &mac_oui);
    entries[0] = &macda_oui_profile_entry;
    rv = soc_profile_mem_add(unit, _bcm_td2p_macda_oui_profile[unit],
                             entries, 1, &profile_idx);
    SOC_IF_ERROR_RETURN(rv);

    *macda_oui_profile_index = profile_idx;
    /* Set the MACDA_OUI_PROFILE_INDEX field */
    soc_mem_field32_set(unit, mem, bufp, fld->oui_profile_index, profile_idx);

    return rv;
}
/*
 * Function:
 *      _bcm_td2p_l3_macda_oui_profile_entry_replace
 * Purpose:
 *      TD2P helper routine used to replace hw L3 entry
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN) L3 entry info.
 *      buf_p     - (IN) hw buffer.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_td2p_l3_macda_oui_profile_entry_replace(int unit, soc_mem_t mem,
                                             _bcm_l3_cfg_t *l3cfg, uint32 *bufp)
{
    /* For BCM_L3_REPLACE, get profile index for EGR_MACDA_OUI_PROFILE table.
     * Delete the profile entry if it's not used by any other host.
     */
    int macda_oui_profile_index = -1;
    int macda_oui_profile_index_new = -1;
    int ref_count = 0;
    int rv = BCM_E_NONE;
    rv = _bcm_td2p_l3_get_macda_oui_profile_index(unit, mem, l3cfg,
                                                  &macda_oui_profile_index,
                                                  &ref_count);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }
    if (macda_oui_profile_index == -1) {
        /* Host entry was done in regular L3_ENTRY table view, add it to extended view */
        rv = _bcm_td2p_l3_macda_oui_profile_entry_add(
                unit, mem, l3cfg, bufp, &macda_oui_profile_index_new);
    } else {
        /* Add new entry, ref_count is automatically incremented */
        rv = _bcm_td2p_l3_macda_oui_profile_entry_add(unit, mem, l3cfg, bufp,
                                                      &macda_oui_profile_index_new);
        if (BCM_SUCCESS(rv)) {
            /* Delete old entry from profile table */
            BCM_IF_ERROR_RETURN
            (_bcm_td2p_l3_macda_oui_profile_entry_del(unit, l3cfg,
                                                      macda_oui_profile_index));
        } else {
            return rv;
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2p_l3_get_macda_oui_profile_index
 * Purpose:
 *      TD2P helper routine used to get profile index for EGR_MACDA_OUI_PROFILE
 * Parameters:
 *      unit                      - (IN) SOC unit number.
 *      mem                       - (IN) L3 Extended table memory.
 *      l3cfg                     - (IN) L3 entry info.
 *      macda_oui_profile_index   - (out) EGR_MACDA_OUI profile index.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_td2p_l3_get_macda_oui_profile_index(int unit, soc_mem_t mem_ext, _bcm_l3_cfg_t *l3cfg,
                                         int *macda_oui_profile_index, int *ref_count)
{
    int ipv6;                          /* IPv6 entry indicator.*/
    int rv = BCM_E_NONE;               /* Operation return status. */
    uint32 *buf_key, *buf_entry;       /* Key and entry buffer ptrs*/
    l3_entry_ipv4_unicast_entry_t l3v4_key, l3v4_entry;            /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_key, l3v4_ext_entry; /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_key, l3v6_entry;           /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_key, l3v6_ext_entry; /* IPv6-Embedded */
    soc_field_t macda_oui_index, macda_oui_valid;
    uint32 macda_oui_profile_valid = 0;
    soc_mem_t mem;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    if (ipv6) {
        macda_oui_index  = IPV6UC_EXT__MACDA_OUI_PROFILE_INDEXf;
        macda_oui_valid  = IPV6UC_EXT__MACDA_OUI_PROFILE_VALIDf;
        mem = BCM_XGS3_L3_MEM(unit, v6);
    } else {
        macda_oui_index  = IPV4UC_EXT__MACDA_OUI_PROFILE_INDEXf;
        macda_oui_valid  = IPV4UC_EXT__MACDA_OUI_PROFILE_VALIDf;
        mem = BCM_XGS3_L3_MEM(unit, v4);
    }

    /* Assign entry-key buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_key,
                              l3v4_key,
                              l3v4_ext_key,
                              l3v6_key,
                              l3v6_ext_key);

    /* Assign entry buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_entry,
                              l3v4_entry,
                              l3v4_ext_entry,
                              l3v6_entry,
                              l3v6_ext_entry);
    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN
    (_bcm_td2_l3_ent_init(unit, mem_ext, l3cfg, buf_key));

    /* Perform lookup hw. to get full entry in ext-view */
    rv = soc_mem_generic_lookup(unit, mem_ext, MEM_BLOCK_ANY,
                                _BCM_TD2_L3_MEM_BANKS_ALL,
                                buf_key, buf_entry, &l3cfg->l3c_hw_index);
    if (SOC_SUCCESS(rv)) {
        /* If entry found, get value of MACDA_OUI_PROFILE_VALIDf */
        macda_oui_profile_valid = soc_mem_field32_get(unit, mem_ext, buf_entry,
                                                      macda_oui_valid);
        if (macda_oui_profile_valid) {
            /* Only if MACDA_OUI_PROFILE_VALIDf is set */
            *macda_oui_profile_index = soc_mem_field32_get(unit, mem_ext, buf_entry,
                                                           macda_oui_index);
            rv = soc_profile_mem_ref_count_get(unit,
                                               _bcm_td2p_macda_oui_profile[unit],
                                               *macda_oui_profile_index,
                                               ref_count);
        }
    } else if (rv == SOC_E_NOT_FOUND) {
        /* Check if entry was added in regular L3_ENTRY table view */

        /* Assign entry-key buf based on table being used */
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_key,
                                  l3v4_key,
                                  l3v4_ext_key,
                                  l3v6_key,
                                  l3v6_ext_key);

        /* Assign entry buf based on table being used */
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                  l3v4_entry,
                                  l3v4_ext_entry,
                                  l3v6_entry,
                                  l3v6_ext_entry);
        /* Prepare lookup key. */
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_init(unit, mem, l3cfg, buf_key));

        /* Perform lookup */
        rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY,
                                    _BCM_TD2_L3_MEM_BANKS_ALL,
                                    buf_key, buf_entry, &l3cfg->l3c_hw_index);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2p_l3_extended_required_scache_size_get
 * Purpose:
 *      Get required MACDA_OUI_PROFILE scache size for Level 2 Warm Boot in TD2P
 *      L3_ENTRY extended view
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      size - (OUT) Required scache size.
 */
void
bcm_td2p_l3_extended_required_scache_size_get(int unit, int *size)
{
    /* Sync the ref_count of EGR_MACDA_OUI_PROFILEm */
    *size += SOC_MEM_SIZE(unit, EGR_MACDA_OUI_PROFILEm) * sizeof(uint32);
}

/*
 * Function:
 *      bcm_td2p_l3_extended_view_sync
 * Purpose:
 *      Record persistent info into the scache for the MACDA_OUI_PROFILE
 *      for L3_ENTRY extended view.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
bcm_error_t
bcm_td2p_l3_extended_view_sync(int unit, uint8 **scache_ptr)
{
    int rv = BCM_E_NONE;
    int i;
    int ref_count;

    if ((scache_ptr == NULL) || (*scache_ptr == NULL)) {
        return BCM_E_PARAM;
    }

    /* Record EGR_MACDA_OUI_PROFILEm ref_count in scache */
    for (i = 0; i < SOC_MEM_SIZE(unit, EGR_MACDA_OUI_PROFILEm); i++) {
        rv = soc_profile_mem_ref_count_get(unit,
                                           _bcm_td2p_macda_oui_profile[unit],
                                           i,
                                           &ref_count);
        if (!(rv == SOC_E_NONE)) {
            return rv;
        }

        sal_memcpy((*scache_ptr), &ref_count, sizeof(uint32));
        (*scache_ptr) += sizeof(ref_count);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2p_l3_extended_view_reinit
 * Purpose:
 *      Warm boot recovery for the MACDA_OUI_PROFILE
 *      extended view
 * Parameters:
 *      unit - Device Number
 *      defaul_ver - Warm boot default version
 * Returns:
 *      BCM_E_XXX
 */
bcm_error_t
bcm_td2p_l3_extended_view_reinit(int unit, uint8 **scache_ptr)
{
    int rv = BCM_E_NONE;
    soc_profile_mem_t *profile_mem;
    int i, j, size;
    uint32 ref_count;

    if ((scache_ptr == NULL) || (*scache_ptr == NULL)) {
        return BCM_E_PARAM;
    }

    /* Update EGR_MACDA_OUI_PROFILE memory profile reference counter */
    profile_mem = _bcm_td2p_macda_oui_profile[unit];
    size = SOC_MEM_SIZE(unit, EGR_MACDA_OUI_PROFILEm);
    for (i = 0; i < size; i++) {
        sal_memcpy(&ref_count, (*scache_ptr), sizeof(uint32));
        (*scache_ptr) += sizeof(ref_count);
        for (j = 0; j < ref_count; j++) {
            SOC_IF_ERROR_RETURN(
            soc_profile_mem_reference(unit, profile_mem, i, 1));
        }
    }
    return rv;
}
#endif /* BCM_RIOT_SUPPORT */

/*
 * Function:
 *      bcm_td2p_l3_tables_init
 * Purpose:
 *      Initialize internal L3 tables and enable L3.
 *      TD2P has configurable multi domain banks for
 *      various l3 objects. This routine sets up h/w
 *      tables for corresponding objects and then calls
 *      common tables init routine.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td2p_l3_tables_init(int unit)
{

#ifdef BCM_TRIDENT3_SUPPORT
    _bcm_l3_bookkeeping_t *l3   = L3_BK_INFO(unit);
#endif
#ifdef BCM_RIOT_SUPPORT
    /* Selects L3 banks based on user inputs */
    BCM_IF_ERROR_RETURN(bcmi_l3_riot_bank_sel(unit));
#endif
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    if ((!SOC_IS_TRIDENT3X(unit))
#ifdef BCM_TRIDENT3_SUPPORT
             || ((l3->l3_ecmp_group_first_lkup_mem_size == 0)
             && (l3->l3_ecmp_member_first_lkup_mem_size == 0))
#endif
    ) {
#if defined(BCM_RIOT_SUPPORT)
        /* Non-default ecmp partitions not configured */
        /* selects ecmp level */
        BCM_IF_ERROR_RETURN(bcmi_l3_riot_ecmp_level_sel(unit));
#endif
    }
#endif
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_tables_init(unit));

    return BCM_E_NONE;
}
#endif /* INCLUDE_L3 */
