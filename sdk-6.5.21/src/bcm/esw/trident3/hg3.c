/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/higig.h>
#include <soc/trident3.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/pkt.h>
#include <bcm/port.h>
#include <bcm_int/control.h>
#include <bcm_int/common/tx.h>
#include <bcm_int/esw/trident3.h>

/*
 * Following are the configurations that are to be done on HG3 ports
 *
 *  rCLMAC_MODE.HDR_MODE=0
 *
 *  tING_DEVICE_PORT.PORT_TYPE=0
 *  tING_DEVICE_PORT.PARSE_CONTEXT_ID_0=3
 *  tING_DEVICE_PORT.INITIAL_SHIFT_AMOUNT_0=6
 *
 *  tEGR_PORT.PORT_TYPE=1
 *  tEGR_PORT.EGR_PORT_CTRL_ID=8
 *  tEGR_PORT.EDIT_CTRL_ID=13
 *
 *  tEGR_ING_PORT.PORT_TYPE=1
 */

#ifdef BCM_TRIDENT3_SUPPORT

#define _BCM_TD3_HG3_INITIAL_SHIFT 6
#define _BCM_TD3_HG3_PARSE_CONTEXT_ID 3

int _bcm_td3_port_hg3_mode_set(int unit, bcm_port_t port,
                              bcm_port_encap_config_t *encap_config)
{
    int rv        = BCM_E_NONE;
    soc_mem_t mem = INVALIDm;
    ing_device_port_entry_t ient;
    egr_ing_port_entry_t eent;
    egr_port_entry_t epent;

    if (!(SOC_PORT_VALID(unit, port))) {
        return BCM_E_PORT;
    }
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if (soc_feature(unit, soc_feature_higig3)) {

        /* configure etherype */
        if (SOC_REG_IS_VALID(unit, HG3_ETHERTYPEr)) {
            /* Switch control Higig3Ethertype can also set this ethertype */
            if (encap_config->higig_ethertype) {
                soc_reg_field32_modify(unit, HG3_ETHERTYPEr, REG_PORT_ANY,
                                       ETHERTYPEf, encap_config->higig_ethertype);
            }
        }

        /* config ingress port */
        mem = ING_DEVICE_PORTm;
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &ient));
        soc_mem_field32_set(unit, mem, &ient, PARSE_CONTEXT_ID_0f,
                            _BCM_TD3_HG3_PARSE_CONTEXT_ID);
        soc_mem_field32_set(unit, mem, &ient, PORT_TYPEf, 0);
        soc_mem_field32_set(unit, mem, &ient, INITIAL_SHIFT_AMOUNT_0f,
                            _BCM_TD3_HG3_INITIAL_SHIFT);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &ient));

        /* configure EGR_ING_PORT port */
        mem = EGR_ING_PORTm;
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &eent));
        soc_mem_field32_set(unit, mem, &eent, PORT_TYPEf, 1);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &eent));

        /* configure EGR_PORT port */
        mem = EGR_PORTm;
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &epent));
        soc_mem_field32_set(unit, mem, &epent, EDIT_CTRL_IDf, 13);
        soc_mem_field32_set(unit, mem, &epent, PORT_TYPEf, 1);
        soc_mem_field32_set(unit, mem, &epent, EGR_PORT_CTRL_IDf, 8);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &epent));
        /* configure CLMAC_MODE */
        rv = soc_reg_field32_modify(unit, CLMAC_MODEr, port, HDR_MODEf, 0);

        return rv;
    } else {
        return BCM_E_UNAVAIL;
    }
}

int _bcm_td3_port_hg3_mode_get(int unit, bcm_port_t port, uint8 *is_hg3)
{
    ing_device_port_entry_t ient;
    soc_mem_t mem = ING_DEVICE_PORTm;
    uint32 hdr_type, initial_shift, parse_context_id;
    *is_hg3 = 0;

    if (IS_CPU_PORT(unit, port)) {
        return BCM_E_NONE;
    }
    if (soc_feature(unit, soc_feature_higig3)) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &ient));
        hdr_type = soc_mem_field32_get(unit, mem, &ient, PORT_TYPEf);
        initial_shift =
            soc_mem_field32_get(unit, mem, &ient, INITIAL_SHIFT_AMOUNT_0f);
        parse_context_id = soc_mem_field32_get(unit, mem, &ient, PARSE_CONTEXT_ID_0f);
        if (hdr_type == 0 && initial_shift == _BCM_TD3_HG3_INITIAL_SHIFT &&
            parse_context_id == _BCM_TD3_HG3_PARSE_CONTEXT_ID) {
            *is_hg3 = 1;
        }
    }

    return BCM_E_NONE;
}

typedef struct _bcm_td3_tx_hg3_cookie_s {
    bcm_pkt_cb_f chain_cb;
    bcm_pkt_cb_f pkt_cb;
    bcm_pkt_t *orig_pkt;
    void *orig_cookie;
    int encap;
    uint32 orig_flags;
    bcm_pkt_blk_t *orig_pkt_blk;
    bcm_pkt_blk_t *pkt_blk;
} _bcm_td3_tx_hg3_cookie_t;

typedef struct _bcm_td3_tx_hg3_chain_cb_cookie_s {
    bcm_pkt_cb_f chain_cb;
    void *user_cookie;
    uint32 pkt_cnt;
    _bcm_td3_tx_hg3_cookie_t *cookies;
} _bcm_td3_tx_hg3_chain_cb_cookie_t;


uint16 bcm_td3_tx_hg3_ethertype_get(int unit)
{
    int rv;
    uint32 val, ether_type;
    rv = soc_reg32_get(unit, HG3_ETHERTYPEr, REG_PORT_ANY, 0, &val);
    if (rv == 0) {
        ether_type = soc_reg_field_get(unit, HG3_ETHERTYPEr, val, ETHERTYPEf);
        return (uint16)ether_type;
    } else {
        return 0;
    }
}

STATIC int _bcm_td3_port_encap_config_get(int unit, bcm_gport_t gport,
                                          bcm_port_encap_config_t *encfg)
{
    uint8 is_hg3 = 0;
    bcm_port_t port;

    if (NULL == encfg) {
        return (BCM_E_PARAM);
    }

    sal_memset(encfg, 0, sizeof(bcm_port_encap_config_t));

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &port));
    BCM_IF_ERROR_RETURN(_bcm_td3_port_hg3_mode_get(unit, port, &is_hg3));
    if (is_hg3) {
        encfg->encap           = BCM_PORT_ENCAP_HIGIG3;
        encfg->higig_ethertype = bcm_td3_tx_hg3_ethertype_get(unit);
    }
    return BCM_E_NONE;
}

STATIC void _bcm_td3_tx_hg3_pkt_cb(int unit, bcm_pkt_t *pkt, void *cookie)
{
    _bcm_td3_tx_hg3_cookie_t *ck = (_bcm_td3_tx_hg3_cookie_t *)cookie;

    if (ck) {
        if (ck->pkt_cb || !ck->chain_cb) {
            /* Revert the changes done to the original pkt addr */
            pkt->pkt_data = ck->orig_pkt_blk;
            pkt->blk_count--;
            pkt->flags |= (ck->orig_flags & (BCM_TX_ETHER | BCM_TX_HG_READY));
            pkt->call_back = ck->pkt_cb;
        }

        if (ck->pkt_cb) {
            /* Now call the orig call back */
            ck->pkt_cb(unit, pkt, ck->orig_cookie);
        }

        /* If chain cb is not defined then release the pkt blk related memories,
         * otherwise let chain_cb take care of releasing them. */
        if (!ck->chain_cb) {
            if (ck->pkt_blk) {
                if (ck->pkt_blk->data) {
                    sal_dma_free(ck->pkt_blk->data);
                }
                sal_free(ck->pkt_blk);
            }
            sal_free(ck);
        }
    }
}

STATIC void *_bcm_td3_tx_hg3_cb_cookie_set(bcm_pkt_t *pkt,
                                           bcm_pkt_cb_f chain_cb,
                                           void *orig_cookie, uint32 orig_flags,
                                           bcm_pkt_blk_t *orig_pkt_blk)
{
    _bcm_td3_tx_hg3_cookie_t *cookie;

    if (!(pkt->call_back || chain_cb)) {
        return orig_cookie;
    }

    cookie = sal_alloc(sizeof(*cookie), "hg3cookie");
    if (cookie) {
        sal_memset(cookie, 0, sizeof(_bcm_td3_tx_hg3_cookie_t));
        cookie->pkt_cb       = pkt->call_back;
        cookie->chain_cb     = chain_cb;
        cookie->orig_pkt     = pkt;
        cookie->orig_cookie  = orig_cookie;
        cookie->orig_flags   = orig_flags;
        cookie->orig_pkt_blk = orig_pkt_blk;
        cookie->pkt_blk      = pkt->pkt_data;
        pkt->call_back       = pkt->call_back ? _bcm_td3_tx_hg3_pkt_cb : NULL;
    }
    return (void *)cookie;
}

STATIC void _bcm_td3_tx_hg3_cleanup(bcm_pkt_t *pkt, uint32 orig_flags,
                                    bcm_pkt_blk_t *orig_blk_addr,
                                    bcm_pkt_cb_f orig_pkt_cb)
{
    if (pkt->pkt_data) {
        if (pkt->pkt_data[0].data) {
            sal_dma_free(pkt->pkt_data[0].data);
        }
        sal_free(pkt->pkt_data);
    }
    pkt->pkt_data = orig_blk_addr;
    pkt->flags |= (orig_flags & (BCM_TX_ETHER | BCM_TX_HG_READY));
    pkt->blk_count--;
    pkt->call_back = orig_pkt_cb;
    return;
}

STATIC void _bcm_td3_tx_hg3_chain_cb(int unit, bcm_pkt_t *pkt, void *cookie)
{
    uint32 pkt_idx;
    _bcm_td3_tx_hg3_cookie_t *pktck       = NULL;
    _bcm_td3_tx_hg3_chain_cb_cookie_t *ck = (_bcm_td3_tx_hg3_chain_cb_cookie_t *)cookie;

    if (ck) {
        pktck = ck->cookies;
        if (pktck) {
            /* Revert the pkt data blk changes for eack packet in the chain */
            for (pkt_idx = 0; pkt_idx < ck->pkt_cnt; pkt_idx++) {
                if (pktck[pkt_idx].chain_cb &&
                    pktck[pkt_idx].encap == BCM_PORT_ENCAP_HIGIG3) {
                    _bcm_td3_tx_hg3_cleanup(
                        pktck[pkt_idx].orig_pkt, pktck[pkt_idx].orig_flags,
                        pktck[pkt_idx].orig_pkt_blk, pktck[pkt_idx].pkt_cb);
                }
            }

            /* Call any user given pkt cb for eack packet */
            for (pkt_idx = 0; pkt_idx < ck->pkt_cnt; pkt_idx++) {
                if (pktck[pkt_idx].pkt_cb) {
                    (pktck[pkt_idx].pkt_cb)(unit, pktck[pkt_idx].orig_pkt,
                                            pktck[pkt_idx].orig_cookie);
                }
            }

            /* Remove the per packet info/cookie */
            sal_free(pktck);
        }

        /* Call user given chain cb, if any */
        if (ck->chain_cb) {
            ck->chain_cb(unit, pkt, ck->user_cookie);
        }

        /* Remove the master cookie */
        sal_free(ck);
    }
    return;
}

STATIC int _bcm_td3_pkt_hg3_prepare(int unit, bcm_pkt_t *pkt,
                                    uint32 *orig_flags,
                                    bcm_pkt_blk_t **orig_blk_start)
{
    int rv                         = BCM_E_NONE;
    int blk_idx                    = 0;
    uint8 vlan_adj                 = 0;
    bcm_pkt_blk_t *tmp_pkt_blk_arr = NULL;
    uint8 *dma_buffer              = NULL;

    if (!pkt || !orig_blk_start || !orig_flags) {
        return BCM_E_PARAM;
    }

    /* Blk 0 should have DMAC, SMAC and VLAN (if VLAN tag exists) */
    if (pkt->pkt_data[0].len < (12 + vlan_adj)) {
        return (BCM_E_FAIL);
    }

    /* Create a HG3 hdr if not provided */
    if (!BCM_PKT_TX_HG_READY(pkt)) {
        uint16 pkt_hg3_etype;

        /* If HG3 Hdr build fails then bail out as HG3 requires the HG Hdr. */
        rv = bcm_common_tx_pkt_setup(unit, pkt);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        pkt_hg3_etype = soc_higig3_field_get(unit, (soc_higig3_hdr_t *)BCM_PKT_HG_HDR(pkt), HG_ether_type);
        /* If HG Hdr was not built return back error. */
        /* SOC_HIGIG_START */
        if (bcm_td3_tx_hg3_ethertype_get(unit) != pkt_hg3_etype) {
            return (BCM_E_FAIL);
        }
    }

    /* Create a dma'able block of memory */
    /* 42 = sizeof(da)+sizeof(sa)+sizeof(hg3_header) = 6+6+16 */
    dma_buffer = sal_dma_alloc(28, "Hg3Header");
    if (!dma_buffer) {
        return BCM_E_MEMORY;
    }

    /* Create a pkt data block */
    tmp_pkt_blk_arr =
        sal_alloc(sizeof(bcm_pkt_blk_t) * (pkt->blk_count + 1), "Hg3Blocks");
    if (!tmp_pkt_blk_arr) {
        sal_dma_free(dma_buffer);
        return BCM_E_MEMORY;
    }

    /* Fill in the block details */
    tmp_pkt_blk_arr[0].data = dma_buffer;
    tmp_pkt_blk_arr[0].len  = 28;
    for (blk_idx = 0; blk_idx < pkt->blk_count; blk_idx++) {
        tmp_pkt_blk_arr[blk_idx + 1] = pkt->pkt_data[blk_idx];
    }

    
    /* Create and copy 28 byte HG3 HDR (DA, SA, HG3-HDR) */
    sal_memcpy(dma_buffer, pkt->pkt_data[0].data, 12);

    /* Copy 16 bytes HG3 HDR */
    sal_memcpy(dma_buffer + 12, BCM_PKT_HG_HDR(pkt), 16);
#if 0
    if (!BCM_PKT_TX_HG_READY(pkt)) {
        dma_buffer[14] = BCM_TD3_HG3_HDR_TYPE; /* SOC_HIGIG_START */
    }
    sal_memcpy(dma_buffer + 30, pkt->pkt_data[0].data, 12);
#endif
    /* Keep hold of the start of the orig blk address */
    *orig_blk_start = pkt->pkt_data;

    /* Replace the block list with new blk list */
    pkt->pkt_data = tmp_pkt_blk_arr;
    pkt->blk_count++;

    /* Readjust the len of orig payload as SMAC/DMAC are already in HG3 Hdr */
    tmp_pkt_blk_arr[1].len -= (12 + vlan_adj);
    tmp_pkt_blk_arr[1].data += (12 + vlan_adj);

    /* Store the orig flags */
    *orig_flags = pkt->flags;

    /* Mark the packet as SoBMH */
    pkt->flags &= ~BCM_TX_ETHER;
    pkt->flags |= BCM_TX_HG_READY; /* HG header is ready */

    return rv;
}

int bcm_td3_is_hg3_enabled_port(int unit, bcm_port_t port)
{
    bcm_port_encap_config_t encfg;

    if (!soc_feature(unit, soc_feature_higig3)) {
        return 0;
    }
    
    _bcm_td3_port_encap_config_get(unit, port, &encfg);
    return encfg.encap == BCM_PORT_ENCAP_HIGIG3;
}

int bcm_td3_tx_hg3_pkt(int unit, bcm_pkt_t *pkt, void *cookie, bcm_tx_f tx_f)
{
    void *ck                    = NULL;
    int rv                      = BCM_E_NONE;
    uint32 orig_flags           = 0;
    bcm_pkt_cb_f pkt_orig_cb    = NULL;
    bcm_pkt_blk_t *orig_pkt_blk = NULL;

    rv = _bcm_td3_pkt_hg3_prepare(unit, pkt, &orig_flags, &orig_pkt_blk);
    if (BCM_SUCCESS(rv)) {
        pkt_orig_cb = pkt->call_back;

        /* cookie setup */
        ck = _bcm_td3_tx_hg3_cb_cookie_set(pkt, NULL, cookie, orig_flags,
                                           orig_pkt_blk);

        /* Send the pkt if the cookie creation succeeded */
        if (ck || !pkt_orig_cb) {
            rv = tx_f(unit, pkt, ck);
        } else { /* cookie alloc err */
            rv = BCM_E_MEMORY;
        }

        /* Cookie cleanup in an event of failure */
        if (BCM_FAILURE(rv) && ck && cookie != ck) {
            sal_free(ck);
        }

        /* Pkt cleanup if no callback or failure to send */
        if (BCM_FAILURE(rv) || !pkt_orig_cb) {
            _bcm_td3_tx_hg3_cleanup(pkt, orig_flags, orig_pkt_blk, pkt_orig_cb);
        }
    }
    return rv;
}

int bcm_td3_tx_hg3_array(int unit, bcm_pkt_t **pkt_arr, int count,
                         bcm_pkt_cb_f all_done_cb, void *cookie)
{
    int rv   = BCM_E_NONE;
    int pcnt = 0;
    bcm_pbmp_t pbmp;
    uint32 pkti                                = 0;
    uint32 pti                                 = 0;
    uint8 has_pkt_cb                           = 0;
    uint8 use_hg3                              = 0;
    bcm_port_t tx_port                         = 0;
    uint32 orig_flags                          = 0;
    bcm_pkt_t *cur_pkt                         = NULL;
    bcm_pkt_blk_t *orig_pkt_blk                = NULL;
    bcm_port_encap_config_t encapcfg           = {0};
    _bcm_td3_tx_hg3_cookie_t *cookies          = NULL;
    _bcm_td3_tx_hg3_chain_cb_cookie_t *ccookie = NULL;
    typedef struct {
        bcm_port_t port;
        uint16 ether_type;
        uint16 encap;
    } hg3_port_info_t;
    hg3_port_info_t *pinfos = NULL;

    /* Input parameter check */
    if (!pkt_arr || count < 1 || !*pkt_arr) {
        return BCM_E_PARAM;
    }

    if (!BCM_UNIT_VALID(unit) || !SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (!BCM_IS_LOCAL(unit)) { /* Tunnel the packet to the remote CPU */
        LOG_ERROR(BSL_LS_BCM_TX, (BSL_META_U(unit, "Error:  Cannot do tunnel\n")));
        return BCM_E_PARAM;
    }

    BCM_PBMP_CLEAR(pbmp);
    /* Do fast check if the HG3 logic can be invoked or not */
    for (pkti = 0; pkti < count; pkti++) {
        /* Get hold of the current packet */
        cur_pkt = pkt_arr[pkti];

        /* If even a single pkt is smart switched then go default route */
        if (BCM_PKT_TX_ETHER(cur_pkt)) {
            break;
        }

        /* If pkt has to be sent on multiple ports then go default route */
        BCM_PBMP_COUNT(cur_pkt->tx_pbmp, pcnt);
        if (1 != pcnt) {
            break;
        }

        BCM_PBMP_OR(pbmp, cur_pkt->tx_pbmp);

        /* Mark if individual pkt call back exists. */
        if (cur_pkt->call_back) {
            has_pkt_cb = 1;
        }
    }

    /* If previous loop did not complete successfully or there is a case
     * where individual pkt callback esists but chain done callback not then
     * the HG3 logic cannot be invoked as it wont handle the said scenario.
     * Hence send the pkt using default way and hope TX happens successfully.
     */
    if ((pkti < count) || (has_pkt_cb && !all_done_cb)) {
        return bcm_common_tx_array(unit, pkt_arr, count, all_done_cb, cookie);
    }

    /* Check for port encapsulation type */
    /* Create a buffer to hold HG3 tag to be used later */
    BCM_PBMP_COUNT(pbmp, pcnt);
    pinfos = sal_alloc(sizeof(hg3_port_info_t) * pcnt, "HG3PtInfo");
    if (!pinfos) {
        return (BCM_E_MEMORY);
    }

    /* If all tx ports are HG3 then go through the trouble of
     * forming a sobmh pkt otherwise just use the default way
     */
    pti = 0;
    rv  = BCM_E_NONE;
    BCM_PBMP_ITER(pbmp, tx_port)
    {
        if (pti >= pcnt) {
            rv = BCM_E_FAIL;
            break;
        }

        rv = _bcm_td3_port_encap_config_get(unit, tx_port, &encapcfg);

        if (bcm_td3_is_hg3_enabled_port(unit, tx_port)) {
            use_hg3             = 1;
            pinfos[pti].encap   = BCM_PORT_ENCAP_HIGIG3;
        } else {
            pinfos[pti].encap   = BCM_PORT_ENCAP_IEEE;
        }
        pinfos[pti++].port = tx_port;
    }

    if (BCM_FAILURE(rv) || !use_hg3) {
        /* Error in getting the port encap. info or port is non HG3 */
        sal_free(pinfos);
        return bcm_common_tx_array(unit, pkt_arr, count, all_done_cb, cookie);
    }

    /* Past this point the pkt tx has to go through HG3 logic */

    /* Allocate memory to hold per pkt cookie */
    cookies =
        sal_alloc(sizeof(_bcm_td3_tx_hg3_cookie_t) * count, "PerPktCkArr");
    if (!cookies) {
        sal_free(pinfos);
        return (BCM_E_MEMORY);
    }

    /* Allocate memory for master cookie */
    ccookie =
        sal_alloc(sizeof(_bcm_td3_tx_hg3_chain_cb_cookie_t), "ChainCBCk");
    if (!ccookie) {
        sal_free(pinfos);
        sal_free(cookies);
        return (BCM_E_MEMORY);
    }

    /* Fill in master cookie */
    ccookie->pkt_cnt     = count;
    ccookie->chain_cb    = all_done_cb;
    ccookie->user_cookie = cookie;
    ccookie->cookies     = cookies;

    /* Initialize the per pkt cookie arr */
    sal_memset(cookies, 0, sizeof(_bcm_td3_tx_hg3_cookie_t) * count);

    for (pkti = 0, rv = BCM_E_NONE; pkti < count; pkti++) {
        uint8 is_hg3 = 0;
        orig_flags   = 0;
        orig_pkt_blk = NULL;

        /* Get hold of the current packet */
        cur_pkt = pkt_arr[pkti];

        /* Find out the appr. HG3 tag from the cached copy. It is sure that the
         * information will be retrieved succesfully every time as the logic
         * dictates no failure, hence no need to worry if hg3 tag is not found
         */
        BCM_PBMP_ITER(cur_pkt->tx_pbmp, tx_port)
        {
            for (pti = 0; pti < pcnt; pti++) {
                if (pinfos[pti].port == tx_port) {
                    if (pinfos[pti].encap == BCM_PORT_ENCAP_HIGIG3) {
                        is_hg3 = 1;
                    }
                    break;
                }
            }
            break; /* Only one TX port limitation */
        }
        if (!is_hg3) {
            cookies[pkti].chain_cb    = _bcm_td3_tx_hg3_chain_cb;
            cookies[pkti].orig_cookie = cookie;
            cookies[pkti].pkt_cb      = cur_pkt->call_back;
            cookies[pkti].orig_pkt    = cur_pkt;
            continue;
        }

        /* Prepare the  packet */
        rv = _bcm_td3_pkt_hg3_prepare(unit, cur_pkt, &orig_flags, &orig_pkt_blk);

        if (BCM_FAILURE(rv)) {
            break;
        }

        /* Fill in the per packet callback info*/
        cookies[pkti].encap        = BCM_PORT_ENCAP_HIGIG3;
        cookies[pkti].chain_cb     = _bcm_td3_tx_hg3_chain_cb;
        cookies[pkti].orig_cookie  = cookie;
        cookies[pkti].orig_pkt     = cur_pkt;
        cookies[pkti].pkt_cb       = cur_pkt->call_back;
        cookies[pkti].orig_flags   = orig_flags;
        cookies[pkti].orig_pkt_blk = orig_pkt_blk;
        cookies[pkti].pkt_blk      = cur_pkt->pkt_data;

        /* Remove the call back from the pkt */
        cur_pkt->call_back = NULL;
    }

    if (BCM_SUCCESS(rv)) {
        /* All packets processed, now send the call down */
        if (all_done_cb) {
            rv = bcm_common_tx_array(unit, pkt_arr, count, _bcm_td3_tx_hg3_chain_cb,
                                     ccookie);
        } else {
            rv = bcm_common_tx_array(unit, pkt_arr, count, NULL, cookie);
        }
    }

    if (BCM_FAILURE(rv) || !all_done_cb) {
        /* Something went wrong or no callback requested, cleanup */
        for (pkti = 0; pkti < count; pkti++) {
            if (cookies[pkti].chain_cb &&
                cookies[pkti].encap == BCM_PORT_ENCAP_HIGIG3) {
                _bcm_td3_tx_hg3_cleanup(
                    cookies[pkti].orig_pkt, cookies[pkti].orig_flags,
                    cookies[pkti].orig_pkt_blk, cookies[pkti].pkt_cb);
            }
        }
        sal_free(cookies);
        sal_free(ccookie);
    }
    sal_free(pinfos);

    return rv;
}

int bcm_td3_tx_hg3_list(int unit, bcm_pkt_t *pkt, bcm_pkt_cb_f all_done_cb,
                        void *cookie)
{
    int rv = BCM_E_MEMORY;
    int i, count = 0;
    bcm_pkt_t **pkt_arr;
    bcm_pkt_t *cur_pkt = NULL;

    /* Get packet count */
    cur_pkt = pkt;
    while (cur_pkt) {
        count++;
        cur_pkt = cur_pkt->next;
    }

    pkt_arr = sal_alloc(sizeof(cur_pkt) * count, "HG3PktArr");
    if (pkt_arr != NULL) {
        cur_pkt = pkt;
        for (i = 0; i < count; i++) {
            pkt_arr[i] = cur_pkt;
            cur_pkt    = cur_pkt->next;
        }

        rv = bcm_td3_tx_hg3_array(unit, pkt_arr, count, all_done_cb, cookie);
        sal_free(pkt_arr);
    }

    return rv;
}

#endif
