/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <bcm_int/common/debug.h>
#include <bcm/l3.h>
#include <bcm/module.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/common/link.h>
#include <bcm/error.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <bcm_int/dpp/error.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

#ifdef BCM_ARAD_SUPPORT
#endif /* BCM_ARAD_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_VLAN_REMOVED
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT_VLAN_REMOVED */ 

#if defined(BCM_WARM_BOOT_SUPPORT_VLAN_REMOVED)
/* this flag is lowered by scache.c commit function after syncing the SW state*/
#define MARK_SCACHE_DIRTY(unit) \
    SOC_CONTROL_LOCK(unit);\
    SOC_CONTROL(unit)->scache_dirty = 1;\
    SOC_CONTROL_UNLOCK(unit);

#else
#define MARK_SCACHE_DIRTY(unit) \
    do { \
    } while(0)

#endif /*BCM_WARM_BOOT_SUPPORT_VLAN_REMOVED */



#define BCM_SW_DB_CELL_ID_MAX                        ((1L<<11) - 1)




/* global variables - AKA the SW state */
bcm_dpp_l3_sw_db_t _l3_sw_db[BCM_MAX_NUM_UNITS];
bcm_dpp_switch_sw_db_t _switch_sw_db[BCM_MAX_NUM_UNITS];
int _cell_id_curr[BCM_MAX_NUM_UNITS];
bcm_dpp_mirror_sw_db_t _mirror_sw_db[BCM_MAX_NUM_UNITS];

/************************************************************************/
/*                       set/get functions                              */
/************************************************************************/

int
_bcm_sw_db_cell_id_curr_get(int unit, int *cell_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    *cell_id = _cell_id_curr[unit];
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_sw_db_cell_id_curr_set(int unit, int *cell_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    if(*cell_id > BCM_SW_DB_CELL_ID_MAX)
    {
        _cell_id_curr[unit] = 0;
    }
    else
    {
        sal_memcpy(&(_cell_id_curr[unit]), &cell_id, sizeof(int));  
    }
    MARK_SCACHE_DIRTY(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* switch urpf_mode set/get*/
int 
_bcm_sw_db_switch_urpf_mode_set(int unit, int urpf_mode)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(urpf_mode != bcmL3IngressUrpfLoose && urpf_mode != bcmL3IngressUrpfStrict && urpf_mode != bcmL3IngressUrpfDisable) {
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.urpf_mode.set(unit, urpf_mode));

    MARK_SCACHE_DIRTY(unit);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_sw_db_switch_urpf_mode_get(int unit, int *urpf_mode)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.urpf_mode.get(unit, urpf_mode));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* switch dos attack set/get*/
int 
_bcm_sw_db_switch_dos_attack_info_set(int unit, bcm_switch_control_t bcm_type, int enable)
{
    BCMDNX_INIT_FUNC_DEFS;
    switch (bcm_type)
    {
        case bcmSwitchDosAttackToCpu:
          _switch_sw_db[unit].dos_attack.tocpu = enable;
          break;
        case bcmSwitchDosAttackSipEqualDip:
          _switch_sw_db[unit].dos_attack.sipequaldip = enable;
          break;
        case bcmSwitchDosAttackMinTcpHdrSize:
          _switch_sw_db[unit].dos_attack.mintcphdrsize = enable;
          break;
        case bcmSwitchDosAttackV4FirstFrag:
          _switch_sw_db[unit].dos_attack.v4firstfrag = enable;
          break;
        case bcmSwitchDosAttackTcpFlags:
          _switch_sw_db[unit].dos_attack.tcpflags = enable;
          break;
        case bcmSwitchDosAttackL4Port:
          _switch_sw_db[unit].dos_attack.l4port = enable;
          break;
        case bcmSwitchDosAttackTcpFrag:
          _switch_sw_db[unit].dos_attack.tcpfrag = enable;
          break;
        case bcmSwitchDosAttackIcmp:
          _switch_sw_db[unit].dos_attack.icmp = enable;
          break;
        case bcmSwitchDosAttackIcmpPktOversize:
          _switch_sw_db[unit].dos_attack.icmppktoversize = enable;
          break;
        case bcmSwitchDosAttackMACSAEqualMACDA:
          _switch_sw_db[unit].dos_attack.macsaequalmacda = enable;
          break;
        case bcmSwitchDosAttackIcmpV6PingSize:
          _switch_sw_db[unit].dos_attack.icmpv6pingsize = enable;
          break;
        case bcmSwitchDosAttackIcmpFragments:
          _switch_sw_db[unit].dos_attack.icmpfragments = enable;
          break;
        case bcmSwitchDosAttackTcpOffset:
          _switch_sw_db[unit].dos_attack.tcpoffset = enable;
          break;
        case bcmSwitchDosAttackUdpPortsEqual:
          _switch_sw_db[unit].dos_attack.udpportsequal = enable;
          break;
        case bcmSwitchDosAttackTcpPortsEqual:
          _switch_sw_db[unit].dos_attack.tcpportsequal = enable;
          break;
        case bcmSwitchDosAttackTcpFlagsSF:
          _switch_sw_db[unit].dos_attack.tcpflagssf = enable;
          break;
        case bcmSwitchDosAttackTcpFlagsFUP:
          _switch_sw_db[unit].dos_attack.tcpflagsfup = enable;
          break;
        case bcmSwitchDosAttackTcpHdrPartial:
          _switch_sw_db[unit].dos_attack.tcphdrpartial = enable;
          break;
        case bcmSwitchDosAttackPingFlood:
          _switch_sw_db[unit].dos_attack.pingflood = enable;
          break;
        case bcmSwitchDosAttackSynFlood:
          _switch_sw_db[unit].dos_attack.synflood = enable;
          break;
        case bcmSwitchDosAttackTcpSmurf:
          _switch_sw_db[unit].dos_attack.tcpsmurf = enable;
          break;
        case bcmSwitchDosAttackTcpXMas:
          _switch_sw_db[unit].dos_attack.tcpxmas = enable;
          break;
        case bcmSwitchDosAttackL3Header:
          _switch_sw_db[unit].dos_attack.l3header = enable;
          break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable switch control type")));
    }

    MARK_SCACHE_DIRTY(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_sw_db_switch_dos_attack_info_get(int unit, bcm_switch_control_t bcm_type, int *enable)
{

    BCMDNX_INIT_FUNC_DEFS;
    switch (bcm_type)
    {
        case bcmSwitchDosAttackToCpu:
          *enable = _switch_sw_db[unit].dos_attack.tocpu;
          break;
        case bcmSwitchDosAttackSipEqualDip:
          *enable = _switch_sw_db[unit].dos_attack.sipequaldip;
          break;
        case bcmSwitchDosAttackMinTcpHdrSize:
          *enable = _switch_sw_db[unit].dos_attack.mintcphdrsize;
          break;
        case bcmSwitchDosAttackV4FirstFrag:
          *enable = _switch_sw_db[unit].dos_attack.v4firstfrag;
          break;
        case bcmSwitchDosAttackTcpFlags:
          *enable = _switch_sw_db[unit].dos_attack.tcpflags;
          break;
        case bcmSwitchDosAttackL4Port:
          *enable = _switch_sw_db[unit].dos_attack.l4port;
          break;
        case bcmSwitchDosAttackTcpFrag:
          *enable = _switch_sw_db[unit].dos_attack.tcpfrag;
          break;
        case bcmSwitchDosAttackIcmp:
          *enable = _switch_sw_db[unit].dos_attack.icmp;
          break;
        case bcmSwitchDosAttackIcmpPktOversize:
          *enable = _switch_sw_db[unit].dos_attack.icmppktoversize;
          break;
        case bcmSwitchDosAttackMACSAEqualMACDA:
          *enable = _switch_sw_db[unit].dos_attack.macsaequalmacda;
          break;
        case bcmSwitchDosAttackIcmpV6PingSize:
          *enable = _switch_sw_db[unit].dos_attack.icmpv6pingsize;
          break;
        case bcmSwitchDosAttackIcmpFragments:
          *enable = _switch_sw_db[unit].dos_attack.icmpfragments;
          break;
        case bcmSwitchDosAttackTcpOffset:
          *enable = _switch_sw_db[unit].dos_attack.tcpoffset;
          break;
        case bcmSwitchDosAttackUdpPortsEqual:
          *enable = _switch_sw_db[unit].dos_attack.udpportsequal;
          break;
        case bcmSwitchDosAttackTcpPortsEqual:
          *enable = _switch_sw_db[unit].dos_attack.tcpportsequal;
          break;
        case bcmSwitchDosAttackTcpFlagsSF:
          *enable = _switch_sw_db[unit].dos_attack.tcpflagssf;
          break;
        case bcmSwitchDosAttackTcpFlagsFUP:
          *enable = _switch_sw_db[unit].dos_attack.tcpflagsfup;
          break;
        case bcmSwitchDosAttackTcpHdrPartial:
          *enable = _switch_sw_db[unit].dos_attack.tcphdrpartial;
          break;
        case bcmSwitchDosAttackPingFlood:
          *enable = _switch_sw_db[unit].dos_attack.pingflood;
          break;
        case bcmSwitchDosAttackSynFlood:
          *enable = _switch_sw_db[unit].dos_attack.synflood;
          break;
        case bcmSwitchDosAttackTcpSmurf:
          *enable = _switch_sw_db[unit].dos_attack.tcpsmurf;
          break;
        case bcmSwitchDosAttackTcpXMas:
          *enable = _switch_sw_db[unit].dos_attack.tcpxmas;
          break;
        case bcmSwitchDosAttackL3Header:
          *enable = _switch_sw_db[unit].dos_attack.l3header;
          break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable switch control type")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* given real FEC return pointer to copy FECs and ECMPs */
int
bcm_sw_db_l3_fec_to_ecmps_get(int unit, int l3a_intf, fec_to_ecmps_t *fec_to_ecmps)
{
    uint32 fec_id;
    BCMDNX_INIT_FUNC_DEFS;

    _bcm_l3_intf_to_fec(unit, l3a_intf, &fec_id);

    *fec_to_ecmps = _l3_sw_db[unit].fec_to_ecmp[fec_id];
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* given copy FEC return pointer to real FEC and used ECMP */
int
bcm_sw_db_l3_copy_fec_to_ecmps_get(int unit, int l3a_intf, fec_copy_info_t *fec_copy_info)
{
    uint32 fec_id;
    BCMDNX_INIT_FUNC_DEFS;

    _bcm_l3_intf_to_fec(unit, l3a_intf, &fec_id);

    fec_copy_info->ecmp = _l3_sw_db[unit].fec_copy_info[fec_id].ecmp;
    fec_copy_info->real_fec = _l3_sw_db[unit].fec_copy_info[fec_id].real_fec;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
* intf: real-FEC to be added to the ECMP
* ecmp: ECMP-id, with encoding
* cor_fec: copy-FEC, FEC-index where to put the copy.
*/
int
bcm_sw_db_l3_add_fec_mapping_to_ecmp(int unit, uint32 flags, int intf, int ecmp, int cor_fec)
{
    int *tmp_cors = NULL;
    int *tmp_ecmps = NULL;
    int i;
    SOC_PPC_FEC_ID fec; /* real-FEC */

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_l3_intf_to_fec(unit, intf, &fec);

    BCMDNX_ALLOC(tmp_cors, sizeof(int) * (_l3_sw_db[unit].fec_to_ecmp[fec].nof_ecmps + 1), "tmp_cors");
    if (tmp_cors == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    BCMDNX_ALLOC(tmp_ecmps, sizeof(int) * (_l3_sw_db[unit].fec_to_ecmp[fec].nof_ecmps + 1), "tmp_ecmps");
    if (tmp_ecmps == NULL) {
        BCM_FREE(tmp_cors);
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    /*  _l3_sw_db.fec_to_ecmp holds two kinds of mappings (what if they are conflicting?):
        for indices representing real fecs, corresponding_fecs is an array of the copies of the real fec in
        the ecmps which have that fec as a member (i.e. corresponding_fecs[i] will be ecmps[i] + some offset within the ecmp),
        and ecmps is an array of the ecmps which have that fec as a member.
        For indices representing fec copies in the ecmp, corresponding_fecs will have just one entry, pointing to the real fec
        (and ecmps will also have one entry, pointing to the ecmp index which has that copy as a member)
    */

   /*
    * 1. add pointer from real-FEC to new copy (cor-FEC) and new ECMP
    */

    /* for real-FEC save side copy-FECs and include-ECMPs use this real FEC */
    for (i=0; i < _l3_sw_db[unit].fec_to_ecmp[fec].nof_ecmps; i++)
    {
        tmp_cors[i] = _l3_sw_db[unit].fec_to_ecmp[fec].corresponding_fecs[i];
        tmp_ecmps[i] = _l3_sw_db[unit].fec_to_ecmp[fec].ecmps[i];
    }

    /* free old SW state */
    if (_l3_sw_db[unit].fec_to_ecmp[fec].nof_ecmps > 0) {
        BCM_FREE(_l3_sw_db[unit].fec_to_ecmp[fec].ecmps);
        _l3_sw_db[unit].fec_to_ecmp[fec].ecmps = NULL;
       BCM_FREE(_l3_sw_db[unit].fec_to_ecmp[fec].corresponding_fecs);
        _l3_sw_db[unit].fec_to_ecmp[fec].corresponding_fecs = NULL;
    }

    /* back to org status but with larges arrays to include new members */
    _l3_sw_db[unit].fec_to_ecmp[fec].ecmps = tmp_ecmps;
    _l3_sw_db[unit].fec_to_ecmp[fec].corresponding_fecs = tmp_cors;

    /* for real-FEC add pointer to new ECMP and new-copy*/
    _l3_sw_db[unit].fec_to_ecmp[fec].ecmps[(_l3_sw_db[unit].fec_to_ecmp[fec].nof_ecmps)] = ecmp;
    _l3_sw_db[unit].fec_to_ecmp[fec].corresponding_fecs[(_l3_sw_db[unit].fec_to_ecmp[fec].nof_ecmps)] = cor_fec;
    _l3_sw_db[unit].fec_to_ecmp[fec].nof_ecmps++;



    /*
     * 1. add pointer from copy-FEC to real-FEC, and new-ECMP
     */
    if(_l3_sw_db[unit].fec_copy_info[cor_fec].real_fec != -1)
    {
        /*this corresponding fec was just created so it should have no mapping
          (and should never have more than one ancestor)*/

        if (!(flags & BCM_L3_REPLACE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Fec exists already - can\'t replace")));
        }
    }

    /* store mapping from copy to use place */
    _l3_sw_db[unit].fec_copy_info[cor_fec].ecmp = ecmp;
    _l3_sw_db[unit].fec_copy_info[cor_fec].real_fec = fec;
    

    MARK_SCACHE_DIRTY(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * intf: copy fec
 */
int
bcm_sw_db_l3_fec_remove_fec(int unit, int intf)
{
    int i;
    int cor_fec;
    int flag = 0;
    int *tmp_cors = NULL;
    int *tmp_ecmps = NULL;
    SOC_PPC_FEC_ID fec;

    BCMDNX_INIT_FUNC_DEFS;

    _bcm_l3_intf_to_fec(unit, intf, &fec);

    /* only fecs that were created internally should be deleted using this function,
     thus only one cor_dec should exist for them (their ancestor) */
    cor_fec = _l3_sw_db[unit].fec_copy_info[fec].real_fec;

    /*
    if (_l3_sw_db[unit].fec_to_ecmp[cor_fec].nof_ecmps > 1)
    {
        return BCM_E_BUSY;
    }
    */

    if (cor_fec == -1)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("delete unexist FEC")));
    }

    _l3_sw_db[unit].fec_copy_info[fec].real_fec = -1;
    _l3_sw_db[unit].fec_copy_info[fec].ecmp = -1;


    /* remove mapping from the fecs ancestor
       if multiple copies of the same fec are in the ecmp, only one will be removed
       (flag can only be incremented once) */
    for (i=0; i < _l3_sw_db[unit].fec_to_ecmp[cor_fec].nof_ecmps; i++)
    {
        if (flag > 0)
        {
            _l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs[i-flag] = _l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs[i];
            _l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps[i-flag] = _l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps[i];
            _l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs[i] = 0;
            _l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps[i] = 0;        
        }
        if (i >= (_l3_sw_db[unit].fec_to_ecmp[cor_fec].nof_ecmps - flag))
        {
            /* clean the last spots */
            _l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs[i] = 0;
            _l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps[i] = 0;
        }
        if (_l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs[i] == fec)
        {
            flag++;
        }    
    }

    _l3_sw_db[unit].fec_to_ecmp[cor_fec].nof_ecmps -= flag;

    /*  if this fec is not contained in any more ecmps its mappings may be deleted, otherwise the mappings need to be realloced
        after the removal */
    if (_l3_sw_db[unit].fec_to_ecmp[cor_fec].nof_ecmps == 0)
    {

       BCM_FREE(_l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps);
        _l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps = NULL;
       BCM_FREE(_l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs);
        _l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs = NULL;
    } 
    else
    {
        /*
        BCMDNX_ALLOC(tmp_cors, sizeof(int) * (_l3_sw_db[unit].fec_to_ecmp[fec].nof_ecmps), "tmp_cors");
        BCMDNX_ALLOC(tmp_ecmps, sizeof(int) * (_l3_sw_db[unit].fec_to_ecmp[fec].nof_ecmps), "tmp_ecmps");
        */

        BCMDNX_ALLOC(tmp_cors, sizeof(int) * (_l3_sw_db[unit].fec_to_ecmp[cor_fec].nof_ecmps), "tmp_cors");
        if (tmp_cors == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        BCMDNX_ALLOC(tmp_ecmps, sizeof(int) * (_l3_sw_db[unit].fec_to_ecmp[cor_fec].nof_ecmps), "tmp_ecmps");
        if (tmp_ecmps == NULL) {
            BCM_FREE(tmp_cors);
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        for (i=0; i < _l3_sw_db[unit].fec_to_ecmp[cor_fec].nof_ecmps; i++)
        {
            tmp_cors[i] = _l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs[i];
            tmp_ecmps[i] = _l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps[i];
        }

       BCM_FREE(_l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps);
        _l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps = NULL;
       BCM_FREE(_l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs);
        _l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs = NULL;

        _l3_sw_db[unit].fec_to_ecmp[cor_fec].ecmps = tmp_ecmps;
        _l3_sw_db[unit].fec_to_ecmp[cor_fec].corresponding_fecs = tmp_cors;
    } 

    MARK_SCACHE_DIRTY(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
} 

int
bcm_sw_db_l3_set_ecmp_sizes(int unit, int mpintf, int max_size, int curr_size)
{
    SOC_PPC_FEC_ID ecmp;
    BCMDNX_INIT_FUNC_DEFS;

    _bcm_l3_intf_to_fec(unit, mpintf, &ecmp);

#ifdef BCM_ARAD_SUPPORT
    BCM_SAND_IF_ERR_EXIT(arad_pp_sw_db_ecmp_cur_size_set(unit, ecmp, curr_size));
    BCM_SAND_IF_ERR_EXIT(arad_pp_sw_db_ecmp_max_size_set(unit, ecmp, max_size));
#endif

    MARK_SCACHE_DIRTY(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_sw_db_l3_set_ecmp_is_successive(int unit, int mpintf, uint8 successive)
{
    SOC_PPC_FEC_ID ecmp;
    BCMDNX_INIT_FUNC_DEFS;

    _bcm_l3_intf_to_fec(unit, mpintf, &ecmp);

    _l3_sw_db[unit].ecmp_successive[ecmp] = successive;
    MARK_SCACHE_DIRTY(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_sw_db_l3_get_ecmp_is_successive(int unit, int mpintf, uint8 *successive)
{
    SOC_PPC_FEC_ID ecmp;
    BCMDNX_INIT_FUNC_DEFS;

    _bcm_l3_intf_to_fec(unit, mpintf, &ecmp);

    *successive = _l3_sw_db[unit].ecmp_successive[ecmp];

    MARK_SCACHE_DIRTY(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
} 



int
bcm_sw_db_l3_set_ecmp_is_protected(int unit, int mpintf, uint8 protected)
{
    SOC_PPC_FEC_ID ecmp;
    BCMDNX_INIT_FUNC_DEFS;

    _bcm_l3_intf_to_fec(unit, mpintf, &ecmp);

    _l3_sw_db[unit].ecmp_protected[ecmp] = protected;
    MARK_SCACHE_DIRTY(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_sw_db_l3_get_ecmp_is_protected(int unit, int mpintf, uint8 *protected)
{
    SOC_PPC_FEC_ID ecmp;
    BCMDNX_INIT_FUNC_DEFS;

    _bcm_l3_intf_to_fec(unit, mpintf, &ecmp);

    *protected = _l3_sw_db[unit].ecmp_protected[ecmp];

    MARK_SCACHE_DIRTY(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
} 



int
bcm_sw_db_l3_get_ecmp_sizes(int unit, int mpintf, int *max_size, int *curr_size)
{
    SOC_PPC_FEC_ID fec_ndx;
#ifdef BCM_ARAD_SUPPORT
    uint32 soc_sand_rv;
    uint32 max_size_lcl, curr_size_lcl;
#endif /* BCM_ARAD_SUPPORT */
    BCMDNX_INIT_FUNC_DEFS;

    /* convert the intf to fec */
    _bcm_l3_intf_to_fec(unit, mpintf, &fec_ndx);  

#ifdef BCM_ARAD_SUPPORT
    soc_sand_rv = arad_pp_sw_db_ecmp_cur_size_get(unit, fec_ndx, &curr_size_lcl);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *curr_size = curr_size_lcl;
    soc_sand_rv = arad_pp_sw_db_ecmp_max_size_get(unit, fec_ndx, &max_size_lcl);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *max_size = max_size_lcl;
#endif /* BCM_ARAD_SUPPORT */

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
} 


/************************************************************************/
/*                      init\reinit functions                           */
/*  (reinit functions can be used to take out warmboot init seq logic)  */
/************************************************************************/
int
_bcm_sw_db_l3_init(int unit)
{
    int i;

    BCMDNX_INIT_FUNC_DEFS;

    /* ECMP info */
    BCMDNX_ALLOC(_l3_sw_db[unit].ecmp_size, sizeof(ecmp_size_t) * _BCM_DPP_L3_NOF_FECS(unit), "ecmp-size-info");
    if (_l3_sw_db[unit].ecmp_size == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    sal_memset(_l3_sw_db[unit].ecmp_size,0x0, sizeof(ecmp_size_t) * _BCM_DPP_L3_NOF_FECS(unit));

    BCMDNX_ALLOC(_l3_sw_db[unit].fec_to_ecmp, sizeof(fec_to_ecmps_t) * _BCM_DPP_L3_NOF_FECS(unit), "fec-ecmp-map-info");
    if (_l3_sw_db[unit].fec_to_ecmp == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    sal_memset(_l3_sw_db[unit].fec_to_ecmp,0x0, sizeof(fec_to_ecmps_t) * _BCM_DPP_L3_NOF_FECS(unit));

    BCMDNX_ALLOC(_l3_sw_db[unit].fec_copy_info, sizeof(fec_copy_info_t) * _BCM_DPP_L3_NOF_FECS(unit), "copy-fec-map-info");
    if (_l3_sw_db[unit].fec_copy_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    sal_memset(_l3_sw_db[unit].fec_copy_info,0xFF, sizeof(fec_copy_info_t) * _BCM_DPP_L3_NOF_FECS(unit));

    BCMDNX_ALLOC(_l3_sw_db[unit].ecmp_successive, sizeof(uint8) * _BCM_DPP_L3_NOF_FECS(unit), "ecmp-successive");
    if (_l3_sw_db[unit].ecmp_successive == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    sal_memset(_l3_sw_db[unit].ecmp_successive,0x0, sizeof(uint8) * _BCM_DPP_L3_NOF_FECS(unit));

    BCMDNX_ALLOC(_l3_sw_db[unit].ecmp_protected, sizeof(uint8) * _BCM_DPP_L3_NOF_FECS(unit), "ecmp-protected");
    if (_l3_sw_db[unit].ecmp_protected == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    sal_memset(_l3_sw_db[unit].ecmp_protected,0x0, sizeof(uint8) * _BCM_DPP_L3_NOF_FECS(unit));

    for (i=0; i<_BCM_DPP_L3_NOF_FECS(unit); i++)
    {    
        _l3_sw_db[unit].fec_copy_info[i].ecmp = -1;
        _l3_sw_db[unit].fec_copy_info[i].real_fec = -1;
    }  

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_sw_db_l3_deinit(int unit)
{

    BCMDNX_INIT_FUNC_DEFS;

    /* ECMP info */
    BCM_FREE(_l3_sw_db[unit].ecmp_size);

    BCM_FREE(_l3_sw_db[unit].fec_to_ecmp);

    BCM_FREE(_l3_sw_db[unit].fec_copy_info);

    BCM_FREE(_l3_sw_db[unit].ecmp_successive);

    BCM_FREE(_l3_sw_db[unit].ecmp_protected);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_WARM_BOOT_SUPPORT_VLAN_REMOVED
int
_bcm_sw_db_l3_reinit(int unit)
{ 
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_sw_db_l3_init(unit));
exit:
    BCMDNX_FUNC_RETURN;
}

#endif

#ifdef BCM_WARM_BOOT_SUPPORT_VLAN_REMOVED

int
_bcm_sw_db_switch_reinit(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


#endif /*BCM_WARM_BOOT_SUPPORT_VLAN_REMOVED */






/************************************************************************/
/*               init/sync functions for the whole module               */
/************************************************************************/

int _bcm_sw_db_init(int unit)
{
    int rv = BCM_E_NONE;
    int pp_enable=0;

    BCMDNX_INIT_FUNC_DEFS;

    pp_enable = SOC_DPP_PP_ENABLE(unit); 

#ifdef BCM_WARM_BOOT_SUPPORT_VLAN_REMOVED

    rv = _bcm_sw_db_l3_reinit(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_sw_db_switch_reinit(unit);
    BCMDNX_IF_ERR_EXIT(rv);
#else /*BCM_WARM_BOOT_SUPPORT_VLAN_REMOVED*/
    if (pp_enable) {
        rv = _bcm_sw_db_l3_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif

exit:
BCMDNX_FUNC_RETURN;
}

int _bcm_sw_db_deinit(int unit)
{
    int rv = BCM_E_NONE;
    int pp_enable=0;

    BCMDNX_INIT_FUNC_DEFS;
    pp_enable = SOC_DPP_PP_ENABLE(unit); 

    if (pp_enable) {
        rv = _bcm_sw_db_l3_deinit(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
BCMDNX_FUNC_RETURN;
}

int
_bcm_sw_db_petra_mirror_mode_set(int unit, int mode)
{

  BCMDNX_INIT_FUNC_DEFS;

  _mirror_sw_db[unit].mirror_mode= mode;


  BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_sw_db_petra_mirror_mode_get(int unit, int *mode)
{

  BCMDNX_INIT_FUNC_DEFS;

  *mode = _mirror_sw_db[unit].mirror_mode;
  
  BCM_EXIT;
  exit:
      BCMDNX_FUNC_RETURN;

}

int
_bcm_sw_db_outbound_mirror_port_profile_set(int unit, int port, uint8 profile)
{

  BCMDNX_INIT_FUNC_DEFS;

  _mirror_sw_db[unit].egress_port_profile[port] = profile;


  BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_sw_db_outbound_mirror_port_profile_get(int unit, int port, uint8 *profile)
{

  BCMDNX_INIT_FUNC_DEFS;

  *profile = _mirror_sw_db[unit].egress_port_profile[port];
  
  BCM_EXIT;
  exit:
      BCMDNX_FUNC_RETURN;

}


