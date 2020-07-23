/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:     Firebolt L2 function implementations
 */

#include <soc/defs.h>

#ifdef BCM_FIREBOLT_SUPPORT

#include <assert.h>

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/common/multicast.h>

#include <bcm_int/esw_dispatch.h>

typedef struct _bcm_mac_block_info_s {
    bcm_pbmp_t mb_pbmp;
    int ref_count;
} _bcm_mac_block_info_t;

static _bcm_mac_block_info_t *_mbi_entries[BCM_MAX_NUM_UNITS];
static int _mbi_num[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_fb_l2_from_l2x
 * Purpose:
 *      Convert a Firebolt L2X entry to a hardware-independent L2 entry
 * Parameters:
 *      unit - Unit number
 *      l2addr - (OUT) Hardware-independent L2 entry
 *      l2x_entry - Firebolt L2X entry
 */

int
_bcm_fb_l2_from_l2x(int unit, bcm_l2_addr_t *l2addr,
        l2x_entry_t *l2x_entry)
{
    int mb_index, rval;

    sal_memset(l2addr, 0, sizeof (*l2addr));

    /* Valid bit is ignored here; entry is assumed valid */

    soc_L2_ENTRY_ONLYm_mac_addr_get(unit, l2x_entry, MAC_ADDRf, l2addr->mac);

    l2addr->vid = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VLAN_IDf);
    l2addr->cos_dst = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, PRIf);
    l2addr->cos_src = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, PRIf);

    if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, CPUf)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }

    if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, DST_DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }

    if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, SRC_DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }

    if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, SCPf)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }

    if (BCM_MAC_IS_MCAST(l2addr->mac)) {
        l2addr->flags |= BCM_L2_MCAST;
        l2addr->l2mc_group =
            soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, L2MC_PTRf);

        /* Translate l2mc index */
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchL2McIdxRetType, &rval));
        if (rval)  {
           _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group, _BCM_MULTICAST_TYPE_L2,
                                                                l2addr->l2mc_group);
        }
    } else {
        _bcm_gport_dest_t       dest;
        int                     isGport, rv;

        _bcm_gport_dest_t_init(&dest);
        /* Trunk group */
        if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, Tf)) {
            int psc = 0;  /* Dummy not used */
            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                l2addr->tgid = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry,
                                                              TGIDf);
            } else if (soc_mem_field_valid(unit, L2_ENTRY_ONLYm, TGID_LOf)) {
                l2addr->tgid = (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, TGID_LOf)) |
                               (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, TGID_HIf)
                            << BCM_TGID_TRUNK_LO_BITS(unit));
            }
            else {
                l2addr->tgid = (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, TGIDf));
            }
            bcm_esw_trunk_psc_get(unit, l2addr->tgid, &psc);
            if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, REMOTE_TRUNKf)) {
                l2addr->flags |= BCM_L2_REMOTE_TRUNK;
            }
            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
        } else {
            bcm_module_t    mod_in, mod_out;
            bcm_port_t      port_in, port_out;

            if (soc_feature(unit,soc_feature_trunk_group_overlay)) {
                port_in = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry,
                                                         PORT_NUMf);
            } else {
                port_in = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry,
                                                         PORT_TGIDf);
            }
            mod_in = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, MODULE_IDf);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in, &mod_out, &port_out));
            l2addr->modid = mod_out;
            l2addr->port = port_out;

            dest.port = l2addr->port;
            dest.modid = l2addr->modid;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        }
        rv = bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport);

        if (BCM_SUCCESS(rv) && isGport) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_construct(unit, &dest, &(l2addr->port)));
        }
    }

    if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, L3f)) {
        l2addr->flags |= BCM_L2_L3LOOKUP;
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        l2addr->group = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry,
                                                       MAC_BLOCK_INDEXf);
    } else {
        mb_index = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry,
                                                  MAC_BLOCK_INDEXf);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _mbi_entries[unit][mb_index].mb_pbmp);
        }
        l2addr->group = 0;
    }

    if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, RPEf)) {
        l2addr->flags |= BCM_L2_SETPRI;
    }

    if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, STATIC_BITf)) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, MIRRORf)) {
        l2addr->flags |= BCM_L2_MIRROR;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, HITSAf)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, HITDAf)) {
        l2addr->flags |= BCM_L2_DES_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb_l2_to_l2x
 * Purpose:
 *      Convert a hardware-independent L2 entry to a Firebolt L2X entry
 * Parameters:
 *      unit - Unit number
 *      l2x_entry - (OUT) Firebolt L2X entry
 *      l2addr - Hardware-independent L2 entry
 */

int
_bcm_fb_l2_to_l2x(int unit, l2x_entry_t *l2x_entry, bcm_l2_addr_t *l2addr)
{
    VLAN_CHK_ID(unit, l2addr->vid);
    VLAN_CHK_PRIO(unit, l2addr->cos_dst);
    if (l2addr->flags & BCM_L2_PENDING) {
        return BCM_E_PARAM;
    }


    sal_memset(l2x_entry, 0, sizeof (*l2x_entry));

    soc_L2Xm_field32_set(unit, l2x_entry, VALIDf, 1);
    soc_L2Xm_mac_addr_set(unit, l2x_entry, MAC_ADDRf, l2addr->mac);
    soc_L2Xm_field32_set(unit, l2x_entry, VLAN_IDf, l2addr->vid);
    soc_L2Xm_field32_set(unit, l2x_entry, PRIf, l2addr->cos_dst);

    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        soc_L2Xm_field32_set(unit, l2x_entry, CPUf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        soc_L2Xm_field32_set(unit, l2x_entry, DST_DISCARDf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 1);
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        soc_L2Xm_field32_set(unit, l2x_entry, SCPf, 1);
    }

    if (l2addr->flags & BCM_L2_REMOTE_TRUNK) {
        soc_L2Xm_field32_set(unit, l2x_entry, REMOTE_TRUNKf, 1);
    }

    if (l2addr->flags & BCM_L2_MCAST) { /* if (BCM_MAC_IS_MCAST(l2addr->mac)) { */
        if (_BCM_MULTICAST_IS_SET(l2addr->l2mc_group)) {
            if (_BCM_MULTICAST_IS_L2(l2addr->l2mc_group)) {
                soc_L2Xm_field32_set(unit, l2x_entry, L2MC_PTRf, 
                                     _BCM_MULTICAST_ID_GET(l2addr->l2mc_group));
            } else {
                /* No other multicast types in L2 on these devices */
                return BCM_E_PARAM;
            }
        } else {
            soc_L2Xm_field32_set(unit, l2x_entry, L2MC_PTRf,
                                 l2addr->l2mc_group);
        }
    } else {
        bcm_port_t      port = -1;
        bcm_trunk_t     tgid = BCM_TRUNK_INVALID;
        bcm_module_t    modid = -1;

        if (BCM_GPORT_IS_SET(l2addr->port)) {
            _bcm_l2_gport_params_t  g_params;

            if (BCM_GPORT_IS_BLACK_HOLE(l2addr->port)) {
                soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 1);
            } else {
                if (!(l2addr->flags & BCM_L2_DISCARD_SRC)) {
                    soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 0);
                }
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_l2_gport_parse(unit, l2addr, &g_params));

                switch (g_params.type) {
                    case _SHR_GPORT_TYPE_TRUNK: 
                        tgid = g_params.param0;
                        break;
                    case  _SHR_GPORT_TYPE_MODPORT:
                        port = g_params.param0;
                        modid = g_params.param1;
                        break;
                    case _SHR_GPORT_TYPE_LOCAL_CPU:
                        port = g_params.param0;
                        BCM_IF_ERROR_RETURN(
                            bcm_esw_stk_my_modid_get(unit, &modid));
                        break;
                    default:
                        return BCM_E_PORT;
                }
            }
        } else if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
            tgid = l2addr->tgid;

        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                       l2addr->modid, l2addr->port, 
                                       &modid, &port));
            /* Check parameters */
            if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
                return BCM_E_BADID;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port)) {
                return BCM_E_PORT;
            }
        }

        /* Setting l2x_entry fields according to parameters */
        if ( BCM_TRUNK_INVALID != tgid) {
            soc_L2Xm_field32_set(unit, l2x_entry, Tf, 1);
            if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                soc_L2Xm_field32_set(unit, l2x_entry, TGIDf, tgid);
            } else if (soc_mem_field_valid(unit, L2Xm, TGID_LOf)) {
                soc_L2Xm_field32_set(unit, l2x_entry, TGID_LOf,
                                     tgid & BCM_TGID_PORT_TRUNK_MASK(unit));
                soc_L2Xm_field32_set(unit, l2x_entry, TGID_HIf,
                                     tgid >> BCM_TGID_TRUNK_LO_BITS(unit));
            }
            else {
                soc_L2Xm_field32_set(unit, l2x_entry, TGIDf, 
                                     (tgid & BCM_TGID_PORT_TRUNK_MASK(unit)));
            }
            /*
             * Note:  RTAG is ignored here.  Use bcm_trunk_psc_set to
             * to set for a given trunk.
             */
        } else if (-1 != port) {
            soc_L2Xm_field32_set(unit, l2x_entry, MODULE_IDf, modid);
            if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                soc_L2Xm_field32_set(unit, l2x_entry, PORT_NUMf, port);
            } else {
                soc_L2Xm_field32_set(unit, l2x_entry, PORT_TGIDf, port);
            }
        }
    }
    if (l2addr->flags & BCM_L2_L3LOOKUP) {
        soc_L2Xm_field32_set(unit, l2x_entry, L3f, 1);
    }

    if (l2addr->flags & BCM_L2_MIRROR) {
        soc_L2Xm_field32_set(unit, l2x_entry, MIRRORf, 1);
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        soc_L2Xm_field32_set(unit, l2x_entry, MAC_BLOCK_INDEXf, l2addr->group);
    } /* else MAC_BLOCK_INDEXf is handled in the add/remove functions below */

    if (l2addr->flags & BCM_L2_SETPRI) {
        soc_L2Xm_field32_set(unit, l2x_entry, RPEf, 1);
    }

    if (l2addr->flags & BCM_L2_STATIC) {
        soc_L2Xm_field32_set(unit, l2x_entry, STATIC_BITf, 1);
    }

    if ((l2addr->flags & BCM_L2_SRC_HIT) || 
        (l2addr->flags & BCM_L2_HIT)) {
        soc_L2Xm_field32_set(unit, l2x_entry, HITSAf, 1);
    }

    if ((l2addr->flags & BCM_L2_DES_HIT) ||
        (l2addr->flags & BCM_L2_HIT)){
        soc_L2Xm_field32_set(unit, l2x_entry, HITDAf, 1);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mac_block_insert
 * Purpose:
 *      Find or create a MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_pbmp - egress port bitmap for source MAC blocking
 *      mb_index - (OUT) Index of MAC_BLOCK table with bitmap.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No more MAC_BLOCK entries available
 *      BCM_E_PARAM             Bad bitmap supplied
 */
static int
_bcm_mac_block_insert(int unit, bcm_pbmp_t mb_pbmp, int *mb_index)
{
    int cur_index = 0;
    _bcm_mac_block_info_t *mbi = _mbi_entries[unit];
    mac_block_entry_t mbe;
    bcm_pbmp_t temp_pbmp;

    /* Check for reasonable pbmp */
    BCM_PBMP_ASSIGN(temp_pbmp, mb_pbmp);
    BCM_PBMP_AND(temp_pbmp, PBMP_ALL(unit));
    if (BCM_PBMP_NEQ(mb_pbmp, temp_pbmp)) {
        return BCM_E_PARAM;
    }

    for (cur_index = 0; cur_index < _mbi_num[unit]; cur_index++) {
        if (BCM_PBMP_EQ(mbi[cur_index].mb_pbmp, mb_pbmp)) {
            mbi[cur_index].ref_count++;
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Not in table already, see if any space free */
    for (cur_index = 1; cur_index < _mbi_num[unit]; cur_index++) {
        if (mbi[cur_index].ref_count == 0) {
            /* Attempt insert */
            sal_memset(&mbe, 0, sizeof(mac_block_entry_t));
            soc_mem_pbmp_field_set(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                                   &mb_pbmp);
            SOC_IF_ERROR_RETURN(WRITE_MAC_BLOCKm(unit, MEM_BLOCK_ALL,
                                                 cur_index, &mbe));
            mbi[cur_index].ref_count++;
            BCM_PBMP_ASSIGN(mbi[cur_index].mb_pbmp, mb_pbmp);
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Didn't find a free slot, out of table space */
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_mac_block_delete
 * Purpose:
 *      Remove reference to MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_index - Index of MAC_BLOCK table with bitmap.
 */
static void
_bcm_mac_block_delete(int unit, int mb_index)
{
    if (_mbi_entries[unit][mb_index].ref_count > 0) {
        _mbi_entries[unit][mb_index].ref_count--;
    } else if (mb_index) {
        
        /* Someone reran init without flushing the L2 table */
    } /* else mb_index = 0, as expected for learning */
}

/*
 * Function:
 *      bcm_fb_l2_addr_add
 * Description:
 *      Add a MAC address to the Switch Address Resolution Logic (ARL)
 *      port with the given VLAN ID and parameters.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      l2addr - Pointer to bcm_l2_addr_t containing all valid fields
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No MAC_BLOCK entries available
 * Notes:
 *      Use CMIC_PORT(unit) to associate the entry with the CPU.
 *      Use flag of BCM_L2_COPY_TO_CPU to send a copy to the CPU.
 *      Use flag of BCM_L2_TRUNK_MEMBER to set trunking (TGID must be
 *      passed as well with non-zero trunk group ID)
 *      In case the L2X table is full (e.g. bucket full), an attempt
 *      will be made to store the entry in the L2_USER_ENTRY table.
 */

int
bcm_fb_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    l2x_entry_t  l2x_entry, l2ent;
    int          rv, hash_select, bucket, slot;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    int          hash_select2, bucket2 = -1;
#endif
    l2x_entry_t  l2x_lookup;
    int          l2_index, mb_index = 0;
    uint8        key[XGS_HASH_KEY_SIZE];
    uint32       hash_control;
    int          cf_hit, cf_unhit;
    bcm_mac_t    mac;


	if (l2addr->flags & BCM_L2_TRUNK_MEMBER)
	{
	    BCM_IF_ERROR_RETURN(
	        _bcm_trunk_id_validate(unit, l2addr->tgid));
	}

    BCM_IF_ERROR_RETURN(
        _bcm_fb_l2_to_l2x(unit, &l2x_entry, l2addr));

    rv = soc_fb_l2x_lookup(unit, &l2x_entry, &l2x_lookup, &l2_index);
    if (rv < 0) {
         if (rv != BCM_E_NOT_FOUND) {
             return rv;
         }
    } 

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        /* Using MAC_BLOCK_INDEX field for storing the L2 group */
        soc_L2Xm_field32_set(unit, &l2x_entry, MAC_BLOCK_INDEXf, 
                             l2addr->group);
    } else {
        if (rv == BCM_E_NONE) {
            /* Only here if L2 entry was found */
            mb_index = soc_L2Xm_field32_get(unit, &l2x_lookup, MAC_BLOCK_INDEXf);
            _bcm_mac_block_delete(unit, mb_index);
        }
        /* Mac blocking, attempt to associate with bitmap entry */
        BCM_IF_ERROR_RETURN(
            _bcm_mac_block_insert(unit, l2addr->block_bitmap, &mb_index));

        soc_L2Xm_field32_set(unit, &l2x_entry, MAC_BLOCK_INDEXf, mb_index);
    }

    rv = soc_fb_l2x_insert(unit, &l2x_entry);
    if ((rv == BCM_E_FULL) && (l2addr->flags & BCM_L2_REPLACE_DYNAMIC)) {
        rv = READ_HASH_CONTROLr(unit, &hash_control); 
        if (rv < 0 ) {
            goto done;
        }
        hash_select = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                     L2_AND_VLAN_MAC_HASH_SELECTf);

        soc_draco_l2x_param_to_key(l2addr->mac, l2addr->vid, key);
        bucket = soc_fb_l2_hash(unit, hash_select, key);

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
        if (soc_feature(unit, soc_feature_dual_hash)) {
            rv = READ_L2_AUX_HASH_CONTROLr(unit, &hash_control);
            if (rv < 0 ) {
                goto done;
            }
            if (soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                  hash_control, ENABLEf)) {
                hash_select2 = soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                                 hash_control, HASH_SELECTf);
            } else {
                hash_select2 = hash_select;
            }
            bucket2 = soc_fb_l2_hash(unit, hash_select2, key);
        }
#endif

        rv = soc_l2x_freeze(unit);
        if (rv < 0) {
            goto done;
        }

        cf_hit = cf_unhit = -1;
        for (slot = 0; slot < SOC_L2X_BUCKET_SIZE; slot++) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
            if ((bucket2 >=0) && (slot == (SOC_L2X_BUCKET_SIZE/2))) {
                bucket = bucket2;
            }
#endif
            l2_index = bucket * SOC_L2X_BUCKET_SIZE + slot;
            rv = soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, l2_index, &l2ent);
            if ((rv < 0)) {
                (void) soc_l2x_thaw(unit);
                goto done;
            }

            if (!soc_L2Xm_field32_get(unit, &l2ent, VALIDf)) {
                /* Found invalid entry - stop the search victim found */
                cf_unhit = l2_index; 
                break;
            } else {

                 soc_L2Xm_mac_addr_get(unit, &l2ent, MAC_ADDRf, mac);
                /* Skip static entries */
                if ((soc_L2Xm_field32_get(unit, &l2ent, STATIC_BITf)) ||
                    (BCM_MAC_IS_MCAST(mac)) ||
                    (soc_L2Xm_field32_get(unit, &l2ent, L3f))) {
                    continue;
                }
                if (soc_L2Xm_field32_get(unit, &l2ent, HITDAf) || 
                    soc_L2Xm_field32_get(unit, &l2ent, HITSAf) ) {
                    cf_hit =  l2_index;
                } else {
                    /* Found unhit entry - stop search victim found */
                    cf_unhit = l2_index;
                    break;
                }
            }
        }

        if (cf_unhit >= 0) {
            l2_index = cf_unhit;   /* take last unhit dynamic */
        } else if (cf_hit >= 0) {
            l2_index = cf_hit;     /* or last hit dynamic */
        } else {
          rv = BCM_E_FULL;     /* no dynamics to delete */
          (void) soc_l2x_thaw(unit);
          goto done;
        }

        soc_mem_write(unit, L2Xm, MEM_BLOCK_ALL, l2_index, &l2x_entry);
        rv = soc_l2x_thaw(unit);
    }

done:
    if (rv < 0) {
        _bcm_mac_block_delete(unit, mb_index);
    }

    return rv;
}

/*
 * Function:
 *      bcm_fb_l2_addr_delete
 * Description:
 *      Delete an L2 address (MAC+VLAN) from the device
 * Parameters:
 *      unit - device unit
 *      mac  - MAC address to delete
 *      vid  - VLAN id
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fb_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_l2_addr_t  l2addr;
    l2x_entry_t    l2x_entry, l2x_lookup;
    int            l2_index, mb_index;
    int            rv;
    soc_control_t   *soc = SOC_CONTROL(unit);

    bcm_l2_addr_t_init(&l2addr, mac, vid);

    BCM_IF_ERROR_RETURN(
        _bcm_fb_l2_to_l2x(unit, &l2x_entry, &l2addr));

    soc_mem_lock(unit, L2Xm);
    rv = soc_fb_l2x_lookup(unit, &l2x_entry, &l2x_lookup, &l2_index);
    if (BCM_E_NONE != rv) {
        soc_mem_unlock(unit, L2Xm);
        return rv;
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        mb_index = soc_L2Xm_field32_get(unit, &l2x_lookup, MAC_BLOCK_INDEXf);
        _bcm_mac_block_delete(unit, mb_index);
    }

    if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
        soc_mem_unlock(unit, L2Xm);
        return BCM_E_RESOURCE;
    }
    if ((rv = soc_fb_l2x_delete(unit, &l2x_entry)) >= 0) {
        rv = soc_l2x_sync_delete(unit, (uint32 *) &l2x_lookup, l2_index, 0);
    }
    SOC_L2_DEL_SYNC_UNLOCK(soc);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Function:
 *      bcm_fb_l2_addr_get
 * Description:
 *      Given a MAC address and VLAN ID, check if the entry is present
 *      in the L2 table, and if so, return all associated information.
 * Parameters:
 *      unit - Device unit number
 *      mac - input MAC address to search
 *      vid - input VLAN ID to search
 *      l2addr - Pointer to bcm_l2_addr_t structure to receive results
 * Returns:
 *      BCM_E_NONE              Success (l2addr filled in)
 *      BCM_E_PARAM             Illegal parameter (NULL pointer)
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_NOT_FOUND Address not found (l2addr not filled in)
 */

int
bcm_fb_l2_addr_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
                      bcm_l2_addr_t *l2addr)
{
    l2x_entry_t l2fb_search, l2fb_entry;
    int         index, rv;

    VLAN_CHK_ID(unit, vid);

    /* Set up entry for query */
    sal_memset(&l2fb_search, 0, sizeof(l2fb_search));
    sal_memset(&l2fb_entry, 0, sizeof(l2fb_entry));
    soc_L2Xm_mac_addr_set(unit, &l2fb_search, MAC_ADDRf, mac);
    soc_L2Xm_field32_set(unit, &l2fb_search, VLAN_IDf, vid);

    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &index,
                        &l2fb_search, &l2fb_entry, 0);

    if (rv == SOC_E_NONE) {
        BCM_IF_ERROR_RETURN(_bcm_fb_l2_from_l2x(unit, l2addr, &l2fb_entry));
        return BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_fb_l2_reload_mbi
 * Description:
 *      Load MAC block info from hardware into software data structures.
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

STATIC int
_bcm_fb_l2_reload_mbi(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_mac_block_info_t *mbi = _mbi_entries[unit];
    l2x_entry_t         *l2x_entry, *l2x_table;
    mac_block_entry_t   mbe;
    int                 index, mb_index, l2x_size;

    /*
     * Refresh MAC Block information from the hardware tables.
     */

    for (mb_index = 0; mb_index < _mbi_num[unit]; mb_index++) {
        SOC_IF_ERROR_RETURN
            (READ_MAC_BLOCKm(unit, MEM_BLOCK_ANY, mb_index, &mbe));
        soc_mem_pbmp_field_get(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                               &mbi[mb_index].mb_pbmp);
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        l2x_size = sizeof(l2x_entry_t) * soc_mem_index_count(unit, L2Xm);
        l2x_table = soc_cm_salloc(unit, l2x_size, "l2 reload");
        if (l2x_table == NULL) {
            return BCM_E_MEMORY;
        }

        memset((void *)l2x_table, 0, l2x_size);
        if (soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                               soc_mem_index_min(unit, L2Xm),
                               soc_mem_index_max(unit, L2Xm),
                               l2x_table) < 0) {
            soc_cm_sfree(unit, l2x_table);
            return SOC_E_INTERNAL;
        }

        for (index = soc_mem_index_min(unit, L2Xm);
             index <= soc_mem_index_max(unit, L2Xm); index++) {

             l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm,
                                                      l2x_entry_t *,
                                                      l2x_table, index);
             if (!soc_L2Xm_field32_get(unit, l2x_entry, VALIDf)) {
                 continue;
             }
  
             mb_index = soc_L2Xm_field32_get(unit, l2x_entry, MAC_BLOCK_INDEXf);
             mbi[mb_index].ref_count++;
        }
        soc_cm_sfree(unit, l2x_table);
    } /* Else, MAC Block table is used for L2 group instead, so no reload */

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

/*
 * Function:
 *      bcm_fb_l2_init
 * Description:
 *      Initialize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

int
bcm_fb_l2_init(int unit)
{
    int         was_running = FALSE;
    uint32      flags;
    sal_usecs_t interval;

    if (soc_l2x_running(unit, &flags, &interval)) { 	 
        was_running = TRUE; 	 
        BCM_IF_ERROR_RETURN(soc_l2x_stop(unit)); 	 
    }

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        if (!SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM) {
            soc_fb_l2x_delete_all(unit);
        }
    }

    if (_mbi_entries[unit] != NULL) {
        sal_free(_mbi_entries[unit]);
        _mbi_entries[unit] = NULL;
    }

    _mbi_num[unit] = (SOC_MEM_INFO(unit, MAC_BLOCKm).index_max -
                      SOC_MEM_INFO(unit, MAC_BLOCKm).index_min + 1);
    _mbi_entries[unit] = sal_alloc(_mbi_num[unit] *
                                   sizeof(_bcm_mac_block_info_t),
                                   "BCM L2X MAC blocking info");
    if (!_mbi_entries[unit]) {
        return BCM_E_MEMORY;
    }

    sal_memset(_mbi_entries[unit], 0,
               _mbi_num[unit] * sizeof(_bcm_mac_block_info_t));

    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_fb_l2_reload_mbi(unit));
    }

    /* bcm_l2_register clients */
    
    soc_l2x_register(unit,
            _bcm_l2_register_callback,
            NULL);

    if (was_running || SAL_BOOT_BCMSIM) {
        interval = (SAL_BOOT_BCMSIM)? BCMSIM_L2XMSG_INTERVAL : interval;
        soc_l2x_start(unit, flags, interval);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_l2_term
 * Description:
 *      Finalize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

int
bcm_fb_l2_term(int unit)
{
    if (_mbi_entries[unit] != NULL) {
        sal_free(_mbi_entries[unit]);
        _mbi_entries[unit] = NULL;
    }

    return BCM_E_NONE;
}

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
int
_bcm_dual_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                          bcm_l2_addr_t *cf_array, int cf_max,
                          int *cf_count)
{
    l2x_entry_t         l2ent;
    uint8               key[XGS_HASH_KEY_SIZE];
    int                 hash_select, bucket, bucket_chunk, slot, bank;

    *cf_count = 0;

    for (bank = 0; bank < 2; bank++) {
        /* Get L2 hash select */
        SOC_IF_ERROR_RETURN
            (soc_fb_l2x_entry_bank_hash_sel_get(unit, bank, &hash_select));
        soc_draco_l2x_param_to_key(addr->mac, addr->vid, key);
        bucket = soc_fb_l2_hash(unit, hash_select, key);

        bucket_chunk = SOC_L2X_BUCKET_SIZE / 2;

        for (slot = bucket_chunk * bank;
             (slot < (bucket_chunk * (bank + 1))) && (*cf_count < cf_max);
             slot++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY,
                              bucket * SOC_L2X_BUCKET_SIZE + slot,
                              &l2ent));
            if (soc_L2Xm_field32_get(unit, &l2ent, VALIDf)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb_l2_from_l2x(unit, &cf_array[*cf_count], &l2ent));
                *cf_count += 1;
            }
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */

int
bcm_fb_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                          bcm_l2_addr_t *cf_array, int cf_max,
                          int *cf_count)
{
    l2x_entry_t         l2ent;
    uint8               key[XGS_HASH_KEY_SIZE];
    int                 hash_select, bucket, slot;
    uint32              hash_control;

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_dual_hash)) {
        return _bcm_dual_l2_conflict_get(unit, addr, cf_array,
                                         cf_max, cf_count);
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */

    *cf_count = 0;

    /* Get L2 hash select */
    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
    hash_select = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                 L2_AND_VLAN_MAC_HASH_SELECTf);
    soc_draco_l2x_param_to_key(addr->mac, addr->vid, key);
    bucket = soc_fb_l2_hash(unit, hash_select, key);

    for (slot = 0;
         slot < SOC_L2X_BUCKET_SIZE && *cf_count < cf_max;
         slot++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY,
                          bucket * SOC_L2X_BUCKET_SIZE + slot,
                          &l2ent));
        if (soc_L2Xm_field32_get(unit, &l2ent, VALIDf)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb_l2_from_l2x(unit, &cf_array[*cf_count], &l2ent));
            *cf_count += 1;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb_age_reg_config
 * Description:
 *     Helper function to _bcm_fb_l2_addr_replace_by_XXX functions to 
 *     configure PER_PORT_AGE_CONTROL register for Firebolt
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       structure with information of what to replace 
 * Return:
 *     BCM_E_XXX
 */
STATIC int 
_bcm_fb_age_reg_config(int unit, uint32 flags, _bcm_l2_replace_t *rep_st)
{
    uint32          age_val = 0, op, match_mod, match_port;
    uint32          sync_op, soc_flags;
    soc_control_t   *soc = SOC_CONTROL(unit);
    int             rv;

    BCM_IF_ERROR_RETURN(
        _bcm_get_op_from_flags(flags, &op, &sync_op));

    if (rep_st->match_dest.trunk != -1) {
        match_mod = BCM_TRUNK_TO_MODIDf(unit, rep_st->match_dest.trunk);
        match_port = BCM_TRUNK_TO_TGIDf(unit, rep_st->match_dest.trunk);
        if (sync_op == SOC_L2X_PORTMOD_DEL) {
            sync_op = SOC_L2X_TRUNK_DEL;
        }
    } else {
        match_mod = rep_st->match_dest.module;
        match_port = rep_st->match_dest.port;
    }

    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &age_val,
                      EXCL_STATICf,
                      (flags & BCM_L2_REPLACE_MATCH_STATIC) ? 0 : 1);
    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &age_val, 
                          PPA_MODEf, op);
    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &age_val, 
                      VLAN_IDf, rep_st->key_vlan);
    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &age_val,
                      MODULE_IDf, match_mod);
    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &age_val,
                      TGID_PORTf, match_port);
    BCM_IF_ERROR_RETURN(WRITE_PER_PORT_AGE_CONTROLr(unit, age_val));

    /* OK not to lock L2Xm before since it is locked by upper layer */
    if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
        return BCM_E_RESOURCE;
    }
    rv = soc_l2x_port_age(unit, PER_PORT_AGE_CONTROLr, INVALIDr);
    if (BCM_FAILURE(rv)) {
        SOC_L2_DEL_SYNC_UNLOCK(soc);
        return rv;
    }
    if ((flags & BCM_L2_REPLACE_DELETE)) {
        soc_flags = (flags & BCM_L2_REPLACE_MATCH_STATIC) ? SOC_L2X_INC_STATIC : 0;
        if (flags & BCM_L2_REPLACE_NO_CALLBACKS) {
            soc_flags |= SOC_L2X_NO_CALLBACKS;
        }
        rv = _soc_l2x_sync_delete_by(unit, rep_st->match_dest.module,
                                     rep_st->match_dest.port,
                                     rep_st->key_vlan,
                                     rep_st->match_dest.trunk, 0,
                                     soc_flags, sync_op);
    }
    SOC_L2_DEL_SYNC_UNLOCK(soc);
    return rv;
}

/*
 * Function:
 *     _bcm_fb_repl_reg_config
 * Description:
 *     Helper function to _bcm_fb_l2_addr_replace_by_XXX functions to 
 *     configure PER_PORT_REPL_CONTROL register for Firebolt
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       structure with information of what to replace 
 * Return:
 *     BCM_E_XXX
 */
STATIC int 
_bcm_fb_repl_reg_config(int unit, _bcm_l2_replace_t *rep_st)
{
    uint32 repl_val = 0, mod_val, port_val;

    if (rep_st->new_dest.trunk != -1) {
        mod_val = BCM_TRUNK_TO_MODIDf(unit, rep_st->new_dest.trunk);
        port_val = BCM_TRUNK_TO_TGIDf(unit, rep_st->new_dest.trunk);
    } else {
        mod_val = rep_st->new_dest.module;
        port_val = rep_st->new_dest.port;
    }
    soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &repl_val,
                      MODULE_IDf, mod_val);
    soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &repl_val,
                      PORT_TGIDf, port_val);
    SOC_IF_ERROR_RETURN(WRITE_PER_PORT_REPL_CONTROLr(unit, repl_val));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb_l2_addr_replace_by_vlan
 * Description:
 *     Helper function to _bcm_l2_addr_replace_by_vlan API to replace l2 entries 
 *      by vlan for Firebolt family 
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       structure with information of what to replace
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_fb_l2_addr_replace_by_vlan_dest(int unit, uint32 flags, _bcm_l2_replace_t *rep_st)
{

    if (NULL == rep_st) {
        return BCM_E_PARAM;
    }
    if (flags & BCM_L2_REPLACE_MATCH_VLAN) {
        VLAN_CHK_ID(unit, rep_st->key_vlan);
    }
  
    if (0 == (flags & BCM_L2_REPLACE_DELETE)) {
        BCM_IF_ERROR_RETURN(
            _bcm_fb_repl_reg_config(unit, rep_st));
    }
    BCM_IF_ERROR_RETURN(
        _bcm_fb_age_reg_config(unit, flags, rep_st));

    return BCM_E_NONE;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_fb_l2_sw_dump
 * Purpose:
 *     Displays L2 information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_fb_l2_sw_dump(int unit)
{
    _bcm_mac_block_info_t *mbi;
     char                 pfmt[SOC_PBMP_FMT_LEN];
     int                  i;

    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    LOG_CLI((BSL_META_U(unit,
                        "  FB L2 MAC Blocking Info -\n")));
    LOG_CLI((BSL_META_U(unit,
                        "      Number : %d\n"), _mbi_num[unit]));

    mbi = _mbi_entries[unit];
    LOG_CLI((BSL_META_U(unit,
                        "      Entries (index: pbmp-count) :\n")));
    if (mbi != NULL) {
        for (i = 0; i < _mbi_num[unit]; i++) {
            SOC_PBMP_FMT(mbi[i].mb_pbmp, pfmt);
            LOG_CLI((BSL_META_U(unit,
                                "          %5d: %s-%d\n"), i, pfmt, mbi[i].ref_count));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif  /* BCM_FIREBOLT_SUPPORT */

