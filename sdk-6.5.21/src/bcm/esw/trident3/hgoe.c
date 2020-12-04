/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Higig Over Ethernet
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
#include <bcm/cosq.h>
#include <bcm/pkt.h>
#include <bcm/port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/control.h>
#include <bcm_int/common/tx.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/trident3.h>

#ifdef BCM_TRIDENT3_SUPPORT
#define _BCM_TD3_HGOE_INITIAL_SHIFT 7

typedef struct bcm_tx_hgoe_cookie_s {
    bcm_pkt_cb_f chain_cb;
    bcm_pkt_cb_f pkt_cb;
    bcm_pkt_t *org_pkt;
    void *org_cookie;
    int encap;
    uint32 org_pkt_flags;
    bcm_pkt_blk_t *org_pkt_blk_addr;
    bcm_pkt_blk_t *new_pkt_blk_addr;
} bcm_tx_hgoe_cookie_t;

typedef struct bcm_tx_hgoe_chain_cb_cookie_s {
    bcm_pkt_cb_f user_given_chain_cb;
    void *user_given_cookie;
    uint32 pkt_cnt;
    bcm_tx_hgoe_cookie_t *pkt_cookie_arr;
} bcm_tx_hgoe_chain_cb_cookie_t;

STATIC void
_bcm_td3_tx_hgoe_pkt_cb(int unit, bcm_pkt_t *pkt, void *cookie) {
    bcm_tx_hgoe_cookie_t *my_cookie = (bcm_tx_hgoe_cookie_t *)cookie;

    if (my_cookie) {

        if (my_cookie->pkt_cb || !my_cookie->chain_cb) {
            /* Revert the changes done to the original pkt blk data */
            pkt->pkt_data = my_cookie->org_pkt_blk_addr;
            pkt->blk_count--;
            pkt->flags |=
                (my_cookie->org_pkt_flags & (BCM_TX_ETHER | BCM_TX_HG_READY));
            pkt->call_back = my_cookie->pkt_cb;
        }

        if (my_cookie->pkt_cb) {
            /* Now call the org call back*/
            my_cookie->pkt_cb(unit, pkt, my_cookie->org_cookie);
        }

        /* If chain cb is not defined then release the pkt blk related
         * memories, otherwise let chain_cb take care of releasing them.
         */
        if (!my_cookie->chain_cb) {
            if (my_cookie->new_pkt_blk_addr) {
                if (my_cookie->new_pkt_blk_addr->data) {
                    sal_dma_free(my_cookie->new_pkt_blk_addr->data);
                }
                sal_free(my_cookie->new_pkt_blk_addr);
            }
            sal_free(my_cookie);
        }
    }
}

STATIC void *
_bcm_td3_tx_hgoe_cb_cookie_set(bcm_pkt_t *pkt,
                                bcm_pkt_cb_f chain_cb,
                                void *org_cookie,
                                uint32 org_flags,
                                bcm_pkt_blk_t *org_blk_addr) {

    if (pkt->call_back || chain_cb) {

        bcm_tx_hgoe_cookie_t *cookie = sal_alloc(sizeof(*cookie), "hgoecookie");

        if (cookie) {
            sal_memset(cookie, 0, sizeof(bcm_tx_hgoe_cookie_t));
            cookie->pkt_cb = pkt->call_back;
            cookie->chain_cb = chain_cb;
            cookie->org_pkt = pkt;
            cookie->org_cookie = org_cookie;
            cookie->org_pkt_flags = org_flags;
            cookie->org_pkt_blk_addr = org_blk_addr;
            cookie->new_pkt_blk_addr = pkt->pkt_data;
            pkt->call_back = pkt->call_back ? _bcm_td3_tx_hgoe_pkt_cb : NULL;
        }
        return (void *)cookie;
    } else {
        return org_cookie;
    }
}

STATIC void
_bcm_td3_tx_hgoe_manual_cleanup(bcm_pkt_t *pkt,
    uint32 org_flags, bcm_pkt_blk_t *org_blk_addr, bcm_pkt_cb_f org_pkt_cb) {

    if (pkt->pkt_data) {
        if (pkt->pkt_data[0].data) {
            sal_dma_free(pkt->pkt_data[0].data);
        }
        sal_free(pkt->pkt_data);
    }
    pkt->pkt_data = org_blk_addr;
    pkt->flags |= (org_flags & (BCM_TX_ETHER | BCM_TX_HG_READY));
    pkt->blk_count--;
    pkt->call_back = org_pkt_cb;
    return;
}

STATIC void
_bcm_td3_tx_hgoe_chain_cb(int unit, bcm_pkt_t *pkt, void *cookie) {
    uint32 pkt_idx;
    bcm_tx_hgoe_cookie_t *per_pkt_info = NULL;
    bcm_tx_hgoe_chain_cb_cookie_t *my_cookie =
                (bcm_tx_hgoe_chain_cb_cookie_t *) cookie;

    if (my_cookie) {
        per_pkt_info = my_cookie->pkt_cookie_arr;
        if (per_pkt_info) {
            /* Revert the pkt data blk changes for eack packet in the chain */
            for (pkt_idx = 0 ; pkt_idx < my_cookie->pkt_cnt; pkt_idx++) {
                if (per_pkt_info[pkt_idx].chain_cb
                    && per_pkt_info[pkt_idx].encap == BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET) {
                    _bcm_td3_tx_hgoe_manual_cleanup(
                                per_pkt_info[pkt_idx].org_pkt,
                                per_pkt_info[pkt_idx].org_pkt_flags,
                                per_pkt_info[pkt_idx].org_pkt_blk_addr,
                                per_pkt_info[pkt_idx].pkt_cb);
                }
            }

            /* Call any user given pkt cb for eack packet */
            for (pkt_idx = 0 ; pkt_idx < my_cookie->pkt_cnt ; pkt_idx++) {
                if (per_pkt_info[pkt_idx].pkt_cb) {
                    (per_pkt_info[pkt_idx].pkt_cb)(unit,
                                            per_pkt_info[pkt_idx].org_pkt,
                                            per_pkt_info[pkt_idx].org_cookie);
                }
            }

            /* Remove the per packet info/cookie */
            sal_free(per_pkt_info);
        }

        /* Call user given chain cb, if any */
        if (my_cookie->user_given_chain_cb) {
            my_cookie->user_given_chain_cb(unit, pkt,
                                           my_cookie->user_given_cookie);
        }

        /* Remove the master cookie */
        sal_free(my_cookie);
    }
    return;
}

STATIC int
_bcm_td3_hgoe_pkt_prepare(int unit,
                                bcm_pkt_t *pkt,
                                uint32 *org_flags,
                                bcm_pkt_blk_t **org_blk_start,
                                bcm_port_encap_config_t *encap_cfg)
{
    int rv = BCM_E_NONE;
    int blk_idx = 0;
    uint8 vlan_adj = 0;
    bcm_pkt_blk_t *tmp_pkt_blk_arr = NULL;
    uint8 *dma_buffer = NULL;
    uint16 htons_val = 0;

    if (!pkt || !org_blk_start || !org_flags) {
        return BCM_E_PARAM;
    }

    /* Re-Adjust the size for VLAN tag */
    if (!BCM_PKT_NO_VLAN_TAG(pkt)) {
        vlan_adj = 4;
    }

    /* Blk 0 should have DMAC, SMAC and VLAN (if VLAN tag exists) */
    if (pkt->pkt_data[0].len < (12 + vlan_adj)) {
        return (BCM_E_FAIL);
    }

    /* Create a HG2 hdr if not provided */
    if(!BCM_PKT_TX_HG_READY(pkt)) {
        rv = bcm_common_tx_pkt_setup(unit, pkt);

        /* If HG Hdr build fails then bail out as HGoE requires the HG Hdr. */
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        /* If HG Hdr was not built return back error. */
        if (SOC_HIGIG2_START != *BCM_PKT_HG_HDR(pkt)) {
            return (BCM_E_FAIL);
        }
    }

    /* Create a dma'able block of memory */
    dma_buffer = sal_dma_alloc(42, "HgoeHeader");
    if (!dma_buffer) {
        return BCM_E_MEMORY;
    }

    /* Create a pkt data block */
    tmp_pkt_blk_arr =
        sal_alloc(sizeof(bcm_pkt_blk_t) * (pkt->blk_count + 1), "HgoeBlocks");
    if (!tmp_pkt_blk_arr) {
        sal_dma_free(dma_buffer);
        return BCM_E_MEMORY;
    }

    /* Fill in the block details */
    tmp_pkt_blk_arr[0].data = dma_buffer;
    tmp_pkt_blk_arr[0].len = 42;
    for (blk_idx = 0 ; blk_idx < pkt->blk_count; blk_idx++ ) {
        tmp_pkt_blk_arr[blk_idx + 1] = pkt->pkt_data[blk_idx];
    }

    /* Create and copy 42 byte HGoE HDR (DA, SA, EtherType, HG, DA, SA) */
    sal_memcpy(dma_buffer, pkt->pkt_data[0].data, 12);
    htons_val = bcm_htons(encap_cfg->higig_ethertype);
    sal_memcpy(dma_buffer + 12, &htons_val, 2);

    /* Copy 16 byte  HG2 HDR */
    sal_memcpy(dma_buffer + 14, BCM_PKT_HG_HDR(pkt), 16);
    if (!BCM_PKT_TX_HG_READY(pkt)) {
        /* Rewrite the SOP as HGoE requires the SOP to be 0xFB */
        dma_buffer[14] = SOC_HIGIG_START;
    }
    sal_memcpy(dma_buffer + 30, pkt->pkt_data[0].data, 12);
    /* Keep hold of the start of the org blk address */
    *org_blk_start = pkt->pkt_data;

    /* Replace the block list with new blk list */
    pkt->pkt_data = tmp_pkt_blk_arr;
    pkt->blk_count++;

    /* Readjust the len of org payload as SMAC/DMAC are already in HGoE Hdr */
    tmp_pkt_blk_arr[1].len -= (12 + vlan_adj);
    tmp_pkt_blk_arr[1].data += (12 + vlan_adj);

    /* Store the org flags */
    *org_flags = pkt->flags;

    /* Mark the packet as SoBMH */
    pkt->flags &= ~BCM_TX_ETHER;
    pkt->flags |= BCM_TX_HG_READY; /* HG header is ready */

    return rv;
}

int
bcm_td3_port_hgoe_mode_set(int unit, bcm_port_t port) {

    soc_mem_t mem = INVALIDm;
    ing_device_port_entry_t ing_entry;
    egr_ing_port_entry_t egr_ing_entry;
    egr_port_entry_t egr_entry;
    if (!(SOC_PORT_VALID(unit, port))) {
        return BCM_E_PORT;
    }
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    /* config ingress port */
    mem = ING_DEVICE_PORTm;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                            MEM_BLOCK_ANY, port, &ing_entry));
    soc_mem_field32_set(unit, mem, &ing_entry, PARSE_CONTEXT_ID_0f, 2);
    soc_mem_field32_set(unit, mem, &ing_entry, HDR_TYPE_0f,
                                            BCM_TD3_HGOE_HDR_TYPE);
    soc_mem_field32_set(unit, mem, &ing_entry, INITIAL_SHIFT_AMOUNT_0f,
                                            _BCM_TD3_HGOE_INITIAL_SHIFT);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                                port, &ing_entry));

    mem = EGR_ING_PORTm;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                            MEM_BLOCK_ANY, port, &egr_ing_entry));
    soc_mem_field32_set(unit, mem, &egr_ing_entry, PORT_TYPEf, 1);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                            port, &egr_ing_entry));

   mem = EGR_PORTm;
   SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                            MEM_BLOCK_ANY, port, &egr_entry));
   soc_mem_field32_set(unit, mem, &egr_entry,  EDIT_CTRL_IDf, 4);
   soc_mem_field32_set(unit, mem, &egr_entry, PORT_TYPEf, 1);
   soc_mem_field32_set(unit, mem, &egr_entry, EGR_PORT_CTRL_IDf, 8);
   SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                                port, &egr_entry));
   return BCM_E_NONE;
}

int bcm_td3_port_hgoe_mode_get(int unit, bcm_port_t port, uint8 *is_hgoe) {

    ing_device_port_entry_t ing_entry;
    soc_mem_t mem = ING_DEVICE_PORTm;
    uint32 hdr_type, initial_shift;
    *is_hgoe = 0;

    if (IS_CPU_PORT(unit, port)) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                            MEM_BLOCK_ANY, port, &ing_entry));
    hdr_type = soc_mem_field32_get(unit, mem, &ing_entry, HDR_TYPE_0f);
    initial_shift = soc_mem_field32_get(unit, mem,
                                            &ing_entry, INITIAL_SHIFT_AMOUNT_0f);
    if (hdr_type == BCM_TD3_HGOE_HDR_TYPE &&
            initial_shift == _BCM_TD3_HGOE_INITIAL_SHIFT) {
        *is_hgoe = 1;
    }
    return BCM_E_NONE;
}

int bcm_td3_tx_hgoe_pkt(int unit, bcm_pkt_t *pkt,
                bcm_port_encap_config_t *encap_cfg, void *cookie,
                bcm_tx_f tx_f)
{
    void *my_cookie = NULL;
    int rv = BCM_E_NONE;
    uint32 org_pkt_flags = 0;
    bcm_pkt_cb_f pkt_org_cb = NULL;
    bcm_pkt_blk_t *org_pkt_blk_addr = NULL;
    rv = _bcm_td3_hgoe_pkt_prepare(unit, pkt,
                &org_pkt_flags, &org_pkt_blk_addr, encap_cfg);
    if (!BCM_FAILURE(rv)) {
        pkt_org_cb = pkt->call_back;

        /* cookie setup*/
        my_cookie =
            _bcm_td3_tx_hgoe_cb_cookie_set(pkt, NULL,
                  cookie, org_pkt_flags, org_pkt_blk_addr);

        /* Send the pkt if the cookie creation succeeded */
        if (my_cookie || !pkt_org_cb) {
            rv = tx_f(unit, pkt, my_cookie);
        } else { /* cookie alloc err */
            rv = BCM_E_MEMORY;
        }

        /* Cookie cleanup in an event of failure */
        if (BCM_FAILURE(rv)
            && my_cookie && cookie != my_cookie) {
            sal_free(my_cookie);
        }

        /* Pkt cleanup if no callback or failure to send */
        if (BCM_FAILURE(rv) || !pkt_org_cb) {
            _bcm_td3_tx_hgoe_manual_cleanup(pkt,
                                        org_pkt_flags,
                                        org_pkt_blk_addr,
                                        pkt_org_cb);
        }
    }
    return rv;
}

int
bcm_td3_tx_hgoe_array(int unit, bcm_pkt_t **pkt_arr, int count,
                    bcm_pkt_cb_f all_done_cb, void *cookie)
{
    int rv = BCM_E_NONE;
    int port_cnt = 0;
    bcm_pbmp_t pbmp;
    uint32 pkt_idx = 0;
    uint32 prt_idx = 0;
    uint8 is_pkt_cb = 0;
    uint8 use_hgoe = 0;
    bcm_port_t tx_port = 0;
    uint32 org_pkt_flags = 0;
    uint8 use_dflt_method = 1;
    bcm_pkt_t *cur_pkt = NULL;
    bcm_pkt_blk_t *org_pkt_blk_addr = NULL;
    bcm_port_encap_config_t encapcfg = {0};
    bcm_tx_hgoe_cookie_t *pkt_info_arr = NULL;
    bcm_tx_hgoe_chain_cb_cookie_t *master_cookie = NULL;
    typedef struct {
        bcm_port_t  port;
        uint16 ether_type;
        uint16 encap;
    } hgoe_port_info_t;
    hgoe_port_info_t *port_info_arr = NULL;

    /* Input paramter check */
    if (!pkt_arr || count < 1 || !*pkt_arr) {
        return BCM_E_PARAM;
    }

    if (!BCM_UNIT_VALID(unit) || !SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (!BCM_IS_LOCAL(unit)) { /* Tunnel the packet to the remote CPU */
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "bcm_tx_array ERROR:  Cannot tunnel\n")));
        return BCM_E_PARAM;
    }

    BCM_PBMP_CLEAR(pbmp);
    /* Do fast check if the HGoE logic can be invoked or not */
    for ( pkt_idx = 0 ; pkt_idx < count; pkt_idx++) {

        /* Get hold of the current packet */
        cur_pkt = pkt_arr[pkt_idx];

        /* If even a single pkt is smart switched then go default route */
        if (BCM_PKT_TX_ETHER(cur_pkt)) {
            break;
        }
        /* If pkt has to be sent on multiple ports then go default route */
        BCM_PBMP_COUNT(cur_pkt->tx_pbmp, port_cnt);
        if (1 != port_cnt) {
            break;
        }

        BCM_PBMP_OR(pbmp, cur_pkt->tx_pbmp);

        /* Mark if individual pkt call back exists. */
        if (!is_pkt_cb && cur_pkt->call_back) {
            is_pkt_cb = 1;
        }
    }

    /* If previous loop did not complete successfully or there is a case
     * where individual pkt callback esists but chain done callback not then
     * the HGoE logic cannot be invoked as it wont handle the said scenario.
     * Hence send the pkt using default way and hope TX happens successfully
     */
    if ((pkt_idx >= count) && !(is_pkt_cb && !all_done_cb) ) {
        use_dflt_method = 0;
    }

    if (use_dflt_method) {
        return bcm_common_tx_array(unit, pkt_arr, count, all_done_cb, cookie);
    }

    /*** Check for port encapsulation type ***/
    /* Create a buffer to hold HGoE tag to be used later */
    BCM_PBMP_COUNT(pbmp, port_cnt);
    port_info_arr = sal_alloc(sizeof(hgoe_port_info_t) * port_cnt, "HGoEPrtInfo");
    if (!port_info_arr) {
        return (BCM_E_MEMORY);
    }

    /* If all tx ports are HGoE then go through the trouble of
     * forming a sobmh pkt otherwise just use the default way
     */
    prt_idx = 0;
    rv = BCM_E_NONE;
    BCM_PBMP_ITER(pbmp, tx_port)
    {
        if (prt_idx >= port_cnt) {
            rv = BCM_E_FAIL;
            break;
        }

        rv = bcm_esw_port_encap_config_get(unit, tx_port, &encapcfg);

        if (BCM_FAILURE(rv)) {
            rv = BCM_E_FAIL;
            break;
        } else {
            if (encapcfg.encap == BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET) {
                use_hgoe = 1;
            }
            port_info_arr[prt_idx].port = tx_port;
            port_info_arr[prt_idx].ether_type = encapcfg.higig_ethertype;
            port_info_arr[prt_idx++].encap = encapcfg.encap;
        }
    }

    if (BCM_FAILURE(rv) || !use_hgoe) {
        /* Error in getting the port encap. info or port is non HGoE */
        sal_free(port_info_arr);
        return bcm_common_tx_array(unit, pkt_arr, count, all_done_cb, cookie);
    }

    /*** Past this point the pkt tx has to go through HGoE logic ***/

    /* Allocate memory to hold per pkt cookie */
    pkt_info_arr =
        sal_alloc(sizeof(bcm_tx_hgoe_cookie_t) * count, "PerPktCkArr");
    if (!pkt_info_arr) {
        sal_free(port_info_arr);
        return BCM_E_MEMORY;
    }

    /* Allocate memory for master cookie */
    master_cookie =
        sal_alloc(sizeof(bcm_tx_hgoe_chain_cb_cookie_t), "ChainCBCk");
    if (!master_cookie) {
        sal_free(port_info_arr);
        sal_free(pkt_info_arr);
        return BCM_E_MEMORY;
    }
    /* Fill in master cookie */
    master_cookie->pkt_cnt = count;
    master_cookie->user_given_chain_cb = all_done_cb;
    master_cookie->user_given_cookie = cookie;
    master_cookie->pkt_cookie_arr = pkt_info_arr;

    /* Initialize the per pkt cookie arr */
    sal_memset(pkt_info_arr, 0, sizeof(bcm_tx_hgoe_cookie_t) * count);

    for (pkt_idx = 0, rv = BCM_E_NONE; pkt_idx < count; pkt_idx++) {
        uint8 is_hgoe = 0;
        org_pkt_flags = 0;
        org_pkt_blk_addr = NULL;

        /* Get hold of the current packet */
        cur_pkt = pkt_arr[pkt_idx];

        /* Find out the appr. HGoE tag from the cached copy. It is sure that the
         * information will be retrieved succesfully every time as the logic
         * dictates no failure, hence no need to worry if hgoe tag is not found
         */
        sal_memset(&encapcfg, 0, sizeof(encapcfg));
        BCM_PBMP_ITER(cur_pkt->tx_pbmp, tx_port)
        {
            for (prt_idx = 0; prt_idx < port_cnt; prt_idx++) {
                if (port_info_arr[prt_idx].port == tx_port) {
                    if (port_info_arr[prt_idx].encap == BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET) {
                        is_hgoe = 1;
                    }
                    encapcfg.higig_ethertype =port_info_arr[prt_idx].ether_type;
                    encapcfg.encap = port_info_arr[prt_idx].encap;
                    break;
                }
            }
            break; /* Only one TX port limitation */
        }
        if (!is_hgoe) {
            pkt_info_arr[pkt_idx].chain_cb = _bcm_td3_tx_hgoe_chain_cb;
            pkt_info_arr[pkt_idx].org_cookie = cookie;
            pkt_info_arr[pkt_idx].pkt_cb = cur_pkt->call_back;
            pkt_info_arr[pkt_idx].org_pkt = cur_pkt;
            continue;
        }
        /* Prepare the  packet */
        rv = _bcm_td3_hgoe_pkt_prepare(
            unit, cur_pkt, &org_pkt_flags, &org_pkt_blk_addr, &encapcfg);

        if (BCM_FAILURE(rv)) {
            break;
        }

        /* Fill in the per packet callback info*/
        pkt_info_arr[pkt_idx].encap = BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET;
        pkt_info_arr[pkt_idx].chain_cb = _bcm_td3_tx_hgoe_chain_cb;
        pkt_info_arr[pkt_idx].org_cookie = cookie;
        pkt_info_arr[pkt_idx].org_pkt = cur_pkt;
        pkt_info_arr[pkt_idx].pkt_cb = cur_pkt->call_back;
        pkt_info_arr[pkt_idx].org_pkt_flags = org_pkt_flags;
        pkt_info_arr[pkt_idx].org_pkt_blk_addr = org_pkt_blk_addr;
        pkt_info_arr[pkt_idx].new_pkt_blk_addr = cur_pkt->pkt_data;

        /* Remove the call back from the pkt */
        cur_pkt->call_back = NULL;
    }

    if (!BCM_FAILURE(rv)) {
        /* All packets processed, now send the call down */
        if (all_done_cb) {
            rv = bcm_common_tx_array(unit, pkt_arr,
                    count, _bcm_td3_tx_hgoe_chain_cb, master_cookie);
        } else {
            rv = bcm_common_tx_array(unit, pkt_arr, count, NULL, cookie);
        }
    }

    if (BCM_FAILURE(rv) || !all_done_cb) {
        /* Something went wrong or no callback requested, cleanup */
        for (pkt_idx=0 ; pkt_idx<count ; pkt_idx++) {
            if (pkt_info_arr[pkt_idx].chain_cb) {
                if (pkt_info_arr[pkt_idx].encap == BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET) {
                    _bcm_td3_tx_hgoe_manual_cleanup(
                            pkt_info_arr[pkt_idx].org_pkt,
                            pkt_info_arr[pkt_idx].org_pkt_flags,
                            pkt_info_arr[pkt_idx].org_pkt_blk_addr,
                            pkt_info_arr[pkt_idx].pkt_cb);
                }
            }
        }
        sal_free(pkt_info_arr);
        sal_free(master_cookie);
    }
    sal_free(port_info_arr);

    return rv;
}

int
bcm_td3_tx_hgoe_list(int unit, bcm_pkt_t *pkt,
                   bcm_pkt_cb_f all_done_cb, void *cookie) {

    int rv = BCM_E_NONE;
    int port_cnt = 0;
    bcm_pbmp_t pbmp;
    uint32 pkt_cnt = 0;
    uint32 pkt_idx = 0;
    uint32 prt_idx = 0;
    uint8 is_pkt_cb = 0;
    uint8 use_hgoe = 0;
    bcm_port_t tx_port = 0;
    uint32 org_pkt_flags = 0;
    uint8 use_dflt_method = 1;
    bcm_pkt_t *cur_pkt = NULL;
    bcm_pkt_blk_t *org_pkt_blk_addr = NULL;
    bcm_port_encap_config_t encapcfg = {0};
    bcm_tx_hgoe_cookie_t *pkt_info_arr = NULL;
    bcm_tx_hgoe_chain_cb_cookie_t *master_cookie = NULL;
    typedef struct {
        bcm_port_t  port;
        uint16 ether_type;
        uint16 encap;
    } hgoe_port_info_t;
    hgoe_port_info_t *port_info_arr = NULL;

    /* Input parameter check */
    if (!pkt) {
        return BCM_E_PARAM;
    }

    if (!BCM_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (!BCM_IS_LOCAL(unit)) { /* Tunnel the packet to the remote CPU */
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "bcm_tx_list ERROR:  Cannot tunnel\n")));
        return BCM_E_PARAM;
    }


    BCM_PBMP_CLEAR(pbmp);
    /* Do fast check if the HGoE logic can be invoked or not */
    for (cur_pkt = pkt; cur_pkt; cur_pkt = cur_pkt->next, pkt_cnt++) {

        /* If even a single pkt is smart switched then go default route */
        if (BCM_PKT_TX_ETHER(cur_pkt)) {
            break;
        }

        /* If pkt has to be sent on multiple ports then go default route */
        BCM_PBMP_COUNT(cur_pkt->tx_pbmp, port_cnt);
        if (1 != port_cnt) {
            break;
        }

        BCM_PBMP_OR(pbmp, cur_pkt->tx_pbmp);

        /* Mark if individual pkt call back exists. */
        if (!is_pkt_cb && cur_pkt->call_back) {
            is_pkt_cb = 1;
        }
    }

    /* If previous loop did not complete successfully or there is a case
     * where individual pkt callback esists but chain done callback not then
     * the HGoE logic cannot be invoked as it wont handle the said scenario.
     * Hence send the pkt using default way and hope TX happens successfully
     */
    if (!cur_pkt && !(is_pkt_cb && !all_done_cb) ) {
        use_dflt_method = 0;
    }

    if (use_dflt_method) {
        rv = bcm_common_tx_list(unit, pkt, all_done_cb, cookie);
        return rv;
    }
    /* Check for port encapsulation type */

    /* Create a buffer to hold HGoE tag to be used later */
    BCM_PBMP_COUNT(pbmp, port_cnt);
    port_info_arr=sal_alloc(sizeof(hgoe_port_info_t) * port_cnt, "HGoEPrtInfo");
    if (!port_info_arr) {
        return (BCM_E_MEMORY);
    }

    /* If all tx ports are HGoE then go through the trouble of
     * forming a sobmh pkt otherwise just use the default way
     */
    prt_idx = 0;
    rv = BCM_E_NONE;
    BCM_PBMP_ITER(pbmp, tx_port)
    {
        if (prt_idx >= port_cnt) {
            rv = BCM_E_FAIL;
            break;
        }

        rv = bcm_esw_port_encap_config_get(unit, tx_port, &encapcfg);

        if (BCM_FAILURE(rv)) {
            rv = BCM_E_FAIL;
            break;
        } else {
            if (encapcfg.encap == BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET) {
                use_hgoe = 1;
            }
            port_info_arr[prt_idx].port = tx_port;
            port_info_arr[prt_idx].ether_type = encapcfg.higig_ethertype;
            port_info_arr[prt_idx++].encap = encapcfg.encap;
        }
    }

    if (BCM_FAILURE(rv) || !use_hgoe) {
        /* Error in getting the port encap. info or port is non HGoE */
        sal_free(port_info_arr);
        return(bcm_common_tx_list(unit, pkt, all_done_cb, cookie));
    }

    /* Past this point the pkt tx has to go through HGoE logic */

    /* Allocate memory to hold per pkt cookie */
    pkt_info_arr =
        sal_alloc(sizeof(bcm_tx_hgoe_cookie_t) * pkt_cnt, "PerPktCkArr");
    if (!pkt_info_arr) {
        sal_free(port_info_arr);
        return (BCM_E_MEMORY);
    }

    /* Allocate memory for master cookie */
    master_cookie =
        sal_alloc(sizeof(bcm_tx_hgoe_chain_cb_cookie_t), "ChainCBCk");
    if (!master_cookie) {
        sal_free(port_info_arr);
        sal_free(pkt_info_arr);
        return (BCM_E_MEMORY);
    }

    /* Fill in master cookie */
    master_cookie->pkt_cnt = pkt_cnt;
    master_cookie->user_given_chain_cb = all_done_cb;
    master_cookie->user_given_cookie = cookie;
    master_cookie->pkt_cookie_arr = pkt_info_arr;

    /* Initialize the per pkt cookie arr */
    sal_memset(pkt_info_arr, 0, sizeof(bcm_tx_hgoe_cookie_t) * pkt_cnt);

    for (cur_pkt = pkt, rv = BCM_E_NONE, pkt_idx=0;
            cur_pkt; cur_pkt=cur_pkt->next, pkt_idx++) {
        uint8 is_hgoe = 0;
        org_pkt_flags = 0;
        org_pkt_blk_addr = NULL;

        /* Find out the appr. HGoE tag from the cached copy. It is sure that the
         * information will be retrieved succesfully every time as the logic
         * dictates no failure, hence no need to worry if hgoe tag is not found
         */
        sal_memset(&encapcfg, 0, sizeof(encapcfg));
        BCM_PBMP_ITER(cur_pkt->tx_pbmp, tx_port)
        {
            for (prt_idx=0 ; prt_idx<port_cnt ;prt_idx++) {
                if (port_info_arr[prt_idx].port == tx_port) {
                    if (port_info_arr[prt_idx].encap == BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET) {
                        is_hgoe = 1;
                    }
                    encapcfg.higig_ethertype = port_info_arr[prt_idx].ether_type;
                    encapcfg.encap = port_info_arr[prt_idx].encap;
                    break;
                }
            }
            break; /* Only one TX port limitation */
        }
        if (!is_hgoe) {
            pkt_info_arr[pkt_idx].chain_cb = _bcm_td3_tx_hgoe_chain_cb;
            pkt_info_arr[pkt_idx].org_cookie = cookie;
            pkt_info_arr[pkt_idx].org_pkt = cur_pkt;
            pkt_info_arr[pkt_idx].pkt_cb = cur_pkt->call_back;
            continue;
        }

        /* Prepare the  packet */
        rv = _bcm_td3_hgoe_pkt_prepare(
            unit, cur_pkt, &org_pkt_flags, &org_pkt_blk_addr, &encapcfg);

        if (BCM_FAILURE(rv)) {
            break;
        }

        /* Fill in the per packet callback info*/
        pkt_info_arr[pkt_idx].encap = BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET;
        pkt_info_arr[pkt_idx].chain_cb = _bcm_td3_tx_hgoe_chain_cb;
        pkt_info_arr[pkt_idx].org_cookie = cookie;
        pkt_info_arr[pkt_idx].org_pkt = cur_pkt;
        pkt_info_arr[pkt_idx].pkt_cb = cur_pkt->call_back;
        pkt_info_arr[pkt_idx].org_pkt_flags = org_pkt_flags;
        pkt_info_arr[pkt_idx].org_pkt_blk_addr = org_pkt_blk_addr;
        pkt_info_arr[pkt_idx].new_pkt_blk_addr = cur_pkt->pkt_data;

        /* Remove the call back from the pkt */
        cur_pkt->call_back = NULL;
    }

    if (!BCM_FAILURE(rv)) {
        /* All packets processed, now send the call down */
        if (all_done_cb) {
            rv = bcm_common_tx_list(unit,
                    pkt, _bcm_td3_tx_hgoe_chain_cb, master_cookie);
        } else {
            rv = bcm_common_tx_list(unit, pkt, NULL, cookie);
        }
    }

    if (BCM_FAILURE(rv) || !all_done_cb) {
        /* Something went wrong or no callback requested, cleanup */
        for (pkt_idx=0 ; pkt_idx<pkt_cnt ; pkt_idx++) {
            if (pkt_info_arr[pkt_idx].chain_cb
                && pkt_info_arr[pkt_idx].encap == BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET) {
                _bcm_td3_tx_hgoe_manual_cleanup(
                            pkt_info_arr[pkt_idx].org_pkt,
                            pkt_info_arr[pkt_idx].org_pkt_flags,
                            pkt_info_arr[pkt_idx].org_pkt_blk_addr,
                            pkt_info_arr[pkt_idx].pkt_cb);
            }
        }
        sal_free(pkt_info_arr);
        sal_free(master_cookie);
    }
    sal_free(port_info_arr);

    return rv;
}
#endif

