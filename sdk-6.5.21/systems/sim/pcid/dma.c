/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *	dma.c
 * Requires:
 *	soc_internal_memory_fetch/store
 *	soc_internal_send_int
 * Provides:
 *	pcid_dcb_fetch
 *	pcid_dcb_store
 *	pcid_dma_tx_start
 *	pcid_dma_rx_start
 *	pcid_dma_rx_check
 *	pcid_dma_start_chan
 *	pcid_dma_stat_write
 *	pcid_dma_ctrl_write
 */

#include <shared/bsl.h>

#include <sys/types.h>
#include <soc/dcb.h>
#include <soc/dma.h>
#include <soc/higig.h>
#include <soc/pbsmh.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include "pcid.h"
#include "cmicsim.h"

#ifdef BCM_CMICM_SUPPORT
void pcid_dma_cmicm_rx_check(pcid_info_t *pcid_info, int chan);
#endif

uint32
pcid_reg_read(pcid_info_t *pcid_info, uint32 addr)
{
   uint32 value;

   if ( (pcid_info->regmem_cb)  && 
        (pcid_info->regmem_cb(pcid_info, BCMSIM_PCI_MEM_READ,
                              addr, &value,
                              BYTES2BITS(sizeof(uint32))) == 0) ) {
      return value;
   } 

   return PCIM(pcid_info, addr);
}

void
pcid_reg_write(pcid_info_t *pcid_info, uint32 addr, uint32 value)
{
   if ( (pcid_info->regmem_cb) &&
        (pcid_info->regmem_cb(pcid_info, BCMSIM_PCI_MEM_WRITE,
                               addr, &value,
                               BYTES2BITS(sizeof(uint32))) == 0) ){
      return;
   }

   PCIM(pcid_info, addr) = value;
}

void
pcid_reg_or_write(pcid_info_t* pcid_info, uint32 addr, uint32 or_value)
{
   uint32 v = pcid_reg_read(pcid_info, addr);
   v |= or_value;
   pcid_reg_write(pcid_info, addr, v);
}

void
pcid_reg_and_write(pcid_info_t* pcid_info, uint32 addr, uint32 and_value)
{
   uint32 v = pcid_reg_read(pcid_info, addr);
   v &= ~and_value;
   pcid_reg_write(pcid_info, addr, v);
}


uint32
pcid_dcb_fetch(pcid_info_t *pcid_info, uint32 addr, dcb_t *dcb)
{
    int     size, i;
    uint32  *temp;

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("Reading descriptor at 0x%08x\n"), addr));

    size = SOC_DCB_SIZE(pcid_info->unit);
    soc_internal_bytes_fetch(pcid_info, addr, (uint8 *)dcb, size);

    /* Convert to chip(same as host for sim mode) endian-ness */
    temp = (uint32 *)dcb;
    for (i=0; i < (size/sizeof(uint32)); i++) {
        *(temp + i) = soc_internal_endian_swap(pcid_info, *(temp + i), 
                                               MF_ES_DMA_OTHER);
    }

#ifdef BROADCOM_DEBUG
    if (LOG_CHECK(BSL_LS_SOC_DMA | BSL_INFO)) {
	SOC_DCB_DUMP(pcid_info->unit, dcb, "PCID Fetch", 0);
    }
#endif /* BROADCOM_DEBUG */

    return addr + size;
}

uint32
pcid_dcb_store(pcid_info_t *pcid_info, uint32 addr, dcb_t *dcb)
{
    int	    size, i;
    uint32  *temp;

    size = SOC_DCB_SIZE(pcid_info->unit);
    
    /* Convert to CPU endian-ness */
    temp = (uint32 *)dcb;
    for (i=0; i < (size/sizeof(uint32)); i++) {
        *(temp + i) = soc_internal_endian_swap(pcid_info, *(temp + i), 
                                               MF_ES_DMA_OTHER);
    }

    soc_internal_bytes_store(pcid_info, addr, (void *)dcb, size);
    
    /* Convert back to chip(host) endian-ness */
    for (i=0; i < (size/sizeof(uint32)); i++) {
        *(temp + i) = soc_internal_endian_swap(pcid_info, *(temp + i), 
                                               MF_ES_DMA_OTHER);
    }

#ifdef BROADCOM_DEBUG
    if (LOG_CHECK(BSL_LS_SOC_DMA | BSL_INFO)) {
	SOC_DCB_DUMP(pcid_info->unit, dcb, "PCID Store", 1);
    }
#endif /* BROADCOM_DEBUG */

    return addr + size;
}

/*
 * Function:
 *	_pcid_loop_tx_cb
 * Purpose:
 *	Loop back packets transmitted from CPU
 * Parameters:
 *      pcid_info - Pointer to pcid control structure
 *      dcb - Pointer to DCB describing packet data
 *      chan - Channel on which packet is being transmitted
 */

void
_pcid_loop_tx_cb(pcid_info_t *pcid_info, dcb_t *dcb, int chan)
{
    int			cnt;		/* Bytes in this piece of packet */
    int			eop;		/* End of packet indicator */
    uint8		*buf;		/* alloc'd for this piece of packet */
    int			unit;
    int                 pkt_pos;
    int                 word_count=8; /* FB/ER 3 + 5 words of HG + PBE */
    static uint8        pkt_data[4][PKT_SIZE_MAX];
    static uint32       dcbd[4][13];
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32              hgf;
    uint32              opcode = 0xff;
    uint32              hgh_store[4];
    soc_higig_hdr_t     *hgh;
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    /* Accumulate length for each channel */
    static int len[N_DMA_CHAN] = {0, 0, 0, 0};
    static int tx_pkt_no = 1;
    static int tx_dcb_no = 0;

    unit = pcid_info->unit;
    eop = !SOC_DCB_SG_GET(unit, dcb);
    cnt = SOC_DCB_REQCOUNT_GET(unit, dcb);

    if ((SOC_DCB_TYPE(pcid_info->unit) >= 9) &&
        (SOC_DCB_TYPE(pcid_info->unit) <=31)) {
        /* Copy module Header */
        if (len[chan] == 0) {
            uint32      *up;
            int         i;
            up = (uint32 *)dcb;
            up +=2; /* skip 2 words */

#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(pcid_info->unit)) {
                word_count = 13; /* 4 + 7 words of HG + PBE */
            }
#endif /* BCM_TRX_SUPPORT */

            for (i = 0; i < word_count; i++) {
                dcbd[chan][i] = up[i];
            }
#ifdef BCM_XGS3_SWITCH_SUPPORT
            hgf = CMIC_PORT(unit);
            if (SOC_DCB_HG_GET(unit, dcb)) {
                hgh_store[0] = soc_htonl(dcbd[chan][0]);
                hgh_store[1] = soc_htonl(dcbd[chan][1]);
                hgh_store[2] = soc_htonl(dcbd[chan][2]);
                hgh_store[3] = soc_htonl(dcbd[chan][3]);
                hgh =  (soc_higig_hdr_t *)&hgh_store[0];

                switch (soc_higig_field_get(unit, hgh, HG_start)) {
                case 0xff:
                    hgf = soc_pbsmh_field_get(unit,
                                              (soc_pbsmh_hdr_t *)hgh,
                                              PBSMH_dst_port);
                    break;
                case 0xfb:
                case 0xfc:
                    hgf = soc_higig_field_get(unit, hgh, HG_dst_port);
                    soc_higig_field_set(unit, hgh, HG_src_port, hgf);
                    for(i = 0; i < 4; i++) {
                        dcbd[chan][i] = soc_ntohl(hgh_store[i]);
                    }
                    break;
                default:
                    break;
                }
            }
            SOC_DCB_RX_INGPORT_SET(unit, &dcbd[chan][-2], hgf);
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        }
    }
    pkt_pos = len[chan];
    len[chan] += cnt;

    buf = &pkt_data[chan][pkt_pos];
    if ((len[chan]) >= PKT_SIZE_MAX) {
	return;
    }

    soc_internal_bytes_fetch(pcid_info,
                             SOC_DCB_PADDR_GET(unit, dcb),
                             buf, cnt);
    tx_dcb_no++;

    if (eop) {
	tx_pkt_no++;
	tx_dcb_no = 0;
#ifdef BCM_XGS3_SWITCH_SUPPORT
        /*
         * Not a generic implementation. Intended only for LB test
         */
        if (SOC_IS_XGS3_SWITCH(unit)) {
            buf = &pkt_data[chan][0];
            hgh_store[0] = soc_htonl(dcbd[chan][0]);
            hgh_store[1] = soc_htonl(dcbd[chan][1]);
            hgh_store[2] = soc_htonl(dcbd[chan][2]);
            hgh_store[3] = soc_htonl(dcbd[chan][3]);
            hgh =  (soc_higig_hdr_t *)&hgh_store[0];

            if ((soc_higig_field_get(unit, hgh, HG_start) == 0xfb) ||
                (soc_higig_field_get(unit, hgh, HG_start) == 0xfc)) {
                opcode = soc_higig_field_get(unit, hgh, HG_opcode);
            } else {
                opcode = 0xff;
            }
            if ((opcode == 0) ||
                ((buf[12] != 0x81) || (buf[13] != 0x00))) {
                uint16 vtag;
                int i;

                vtag = soc_higig_field_get(pcid_info->unit,
                                           hgh,
                                           HG_vlan_tag);
                for(i = len[chan] - 1; i >= 12; i--) {
                    buf[i + 4] = buf[i];
                }
                buf[12] = 0x81;
                buf[13] = 0x00;
                buf[14] = vtag >> 8;
                buf[15] = vtag & 0xff;
                len[chan] += 4;
            }
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        if (!(SOC_IS_XGS3_SWITCH(unit) &&
             SOC_DCB_PURGE_GET(unit, dcb))) {
            pcid_add_pkt(pcid_info,
                         &pkt_data[chan][0],
                         len[chan],
                         &dcbd[chan][0]);
        }
        len[chan] = 0;
    }
}

/*
 * Function:
 *	_pcid_default_tx_cb
 * Purpose:
 *	Default function for transmit callback.
 * Parameters:
 *      pcid_info - Pointer to pcid control structure
 *      dcb - Pointer to DCB describing packet data
 *      chan - Channel on which packet is being transmitted
 */

static void
_pcid_default_tx_cb(pcid_info_t *pcid_info, dcb_t *dcb, int chan)
{
    int			cnt;		/* Bytes in this piece of packet */
    int			i, j;
    int			eop;		/* End of packet indicator */
    uint8		*buf;		/* alloc'd for this piece of packet */
    int			unit;
    char		prefix[64];

    /* Accumulate length for each channel */
    static int len[N_DMA_CHAN] = {0, 0, 0, 0};
    static int tx_pkt_no = 1;
    static int tx_dcb_no = 0;

    unit = pcid_info->unit;
    eop = !SOC_DCB_SG_GET(unit, dcb);
    cnt = SOC_DCB_REQCOUNT_GET(unit, dcb);

    len[chan] += cnt;

    if ((buf = sal_alloc(cnt, "txcbb")) == NULL) {
	return;		/* D'oh! */
    }

    soc_internal_bytes_fetch(pcid_info,
                             SOC_DCB_PADDR_GET(unit, dcb),
                             buf, cnt);
    /* coverity[secure_coding] */
    sprintf(prefix, "TX %d\tDCB[%d]", tx_pkt_no, tx_dcb_no);
#ifdef BROADCOM_DEBUG
    SOC_DCB_DUMP(pcid_info->unit, dcb, prefix, 1);
#endif /* BROADCOM_DEBUG */

    tx_dcb_no++;

    for (i = 0; i < cnt; i += 16) {
	LOG_CLI((BSL_META_U(unit,
                            "\tdata[%04x]="), i));

	for (j = 0; j < 16 && i + j < cnt; j++) {
	    LOG_CLI((BSL_META_U(unit,
                                "%02x "), buf[i + j]));
	}

	LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    if (eop) {
        LOG_CLI((BSL_META_U(unit,
                            "TX %d\tEnd %d-byte packet\n"
                            "---------------------------------"
                            "---------------------------------\n"),
                 tx_pkt_no, len[chan]));
	tx_pkt_no++;
	tx_dcb_no = 0;
        len[chan] = 0;
    }

    sal_free(buf);
}

void
pcid_dma_tx_start(pcid_info_t *pcid_info, int ch)
{
    uint32 dcb_addr, dcb_addr_next;
    dcb_t *dcb;

    assert(ch >= 0 && ch < N_DMA_CHAN);

/*    dcb_addr = PCIM(pcid_info, CMIC_DMA_DESC(ch)); */
    dcb_addr = pcid_reg_read(pcid_info, CMIC_DMA_DESC(ch));

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("pcid_dma_tx_start: dcb_addr=0x%08x\n"), dcb_addr));

    dcb = sal_alloc(SOC_DCB_SIZE(pcid_info->unit), "txs_dcb");
    if (dcb == NULL) {
	return;		/* Yow! */
    }
    do {
	dcb_addr_next = pcid_dcb_fetch(pcid_info, dcb_addr, dcb);

	if (SOC_DCB_RELOAD_GET(pcid_info->unit, dcb) &&
	    (pcid_reg_read(pcid_info, CMIC_CONFIG) & CC_RLD_OPN_EN)) {
	    dcb_addr_next = SOC_DCB_PADDR_GET(pcid_info->unit, dcb);
	} else { /* Check for call back function */
            if (pcid_info->tx_cb) {
                (pcid_info->tx_cb)(pcid_info, dcb, ch);
            } else { /* Call back not defined; use default */
                _pcid_default_tx_cb(pcid_info, dcb, ch);
            }
        }

	/* Update descriptor in memory */

	SOC_DCB_DONE_SET(pcid_info->unit, dcb, 1);
        SOC_DCB_XFERCOUNT_SET(pcid_info->unit, dcb,
                              SOC_DCB_REQCOUNT_GET(pcid_info->unit, dcb));
	pcid_dcb_store(pcid_info, dcb_addr, dcb);

	dcb_addr = dcb_addr_next;

/*	PCIM(pcid_info, CMIC_DMA_STAT) |= DS_DESC_DONE_TST(ch); */
        pcid_reg_or_write(pcid_info, CMIC_DMA_STAT, DS_DESC_DONE_TST(ch));

/*	PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_DESC_DONE(ch); */
        pcid_reg_or_write(pcid_info, CMIC_IRQ_STAT, IRQ_DESC_DONE(ch));
	/* send_int(); */
    } while (SOC_DCB_CHAIN_GET(pcid_info->unit, dcb));

    /* Notify completion */

/*    PCIM(pcid_info, CMIC_DMA_STAT) |= DS_CHAIN_DONE_TST(ch); */
    pcid_reg_or_write(pcid_info, CMIC_DMA_STAT, DS_CHAIN_DONE_TST(ch));

/*    PCIM(pcid_info, CMIC_DMA_STAT) &= ~DS_DMA_ACTIVE(ch); */
    pcid_reg_and_write(pcid_info, CMIC_DMA_STAT, DS_DMA_ACTIVE(ch));

/*    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_CHAIN_DONE(ch); */
    pcid_reg_or_write(pcid_info, CMIC_IRQ_STAT, IRQ_CHAIN_DONE(ch));

    soc_internal_send_int(pcid_info);
    sal_free(dcb);
}

#ifdef BCM_CMICM_SUPPORT
void
pcid_dma_cmicm_tx_start(pcid_info_t *pcid_info, int ch)
{
    uint32 dcb_addr, dcb_addr_next;
    dcb_t *dcb;

    assert(ch >= 0 && ch < N_DMA_CHAN);

/*    dcb_addr = PCIM(pcid_info, CMIC_CMCx_DMA_DESCy_OFFSET(CMC0, ch)); */
    dcb_addr = pcid_reg_read(pcid_info, CMIC_CMCx_DMA_DESCy_OFFSET(CMC0, ch));

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("pcid_dma_cmicm_tx_start: dcb_addr=0x%08x\n"), dcb_addr));

    dcb = sal_alloc(SOC_DCB_SIZE(pcid_info->unit), "txs_dcb");
    if (dcb == NULL) {
	return;		/* Yow! */
    }
    do {
	dcb_addr_next = pcid_dcb_fetch(pcid_info, dcb_addr, dcb);

	if (SOC_DCB_RELOAD_GET(pcid_info->unit, dcb)) {
	    dcb_addr_next = SOC_DCB_PADDR_GET(pcid_info->unit, dcb);
	} else { /* Check for call back function */
            if (pcid_info->tx_cb) {
                (pcid_info->tx_cb)(pcid_info, dcb, ch);
            } else { /* Call back not defined; use default */
                _pcid_default_tx_cb(pcid_info, dcb, ch);
            }
        }

	/* Update descriptor in memory */

	SOC_DCB_DONE_SET(pcid_info->unit, dcb, 1);
        SOC_DCB_XFERCOUNT_SET(pcid_info->unit, dcb,
                              SOC_DCB_REQCOUNT_GET(pcid_info->unit, dcb));
	pcid_dcb_store(pcid_info, dcb_addr, dcb);

	dcb_addr = dcb_addr_next;

/*/ PCIM(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET) |= DS_CMCx_DMA_DESC_DONE(ch); */
        pcid_reg_or_write(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET, DS_CMCx_DMA_DESC_DONE(ch));

/*	PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) |= IRQ_CMCx_DESC_DONE(ch); */
        pcid_reg_or_write(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET, IRQ_CMCx_DESC_DONE(ch));

	/* send_int(); */
    } while (SOC_DCB_CHAIN_GET(pcid_info->unit, dcb));

    /* Notify completion */

/*    PCIM(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET) |= DS_CMCx_DMA_CHAIN_DONE(ch); */
    pcid_reg_or_write(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET, DS_CMCx_DMA_CHAIN_DONE(ch));

/*    PCIM(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET) &= ~DS_CMCx_DMA_ACTIVE(ch); */
    pcid_reg_and_write(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET, DS_CMCx_DMA_ACTIVE(ch));

/*    PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) |= IRQ_CMCx_CHAIN_DONE(ch); */
    pcid_reg_or_write(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET, IRQ_CMCx_CHAIN_DONE(ch));

    soc_internal_cmicm_send_int(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK0_OFFSET);
    sal_free(dcb);
}
#endif

static uint32 		rx_dcb_addr[N_DMA_CHAN];
static dcb_t		*rx_dcb[N_DMA_CHAN];


void
pcid_dma_rx_start(pcid_info_t *pcid_info, int ch)
{
    /*
     * This notifies the pcid_dma_rx_check polling routine that a DCB is
     * executing.
     */

/*   rx_dcb_addr[ch] = PCIM(pcid_info, CMIC_DMA_DESC(ch)); */
   rx_dcb_addr[ch] = pcid_reg_read(pcid_info, CMIC_DMA_DESC(ch));

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("pcid_dma_rx_start: ch=%d dcb=0x%08x\n"),
              ch, rx_dcb_addr[ch]));
}

#ifdef BCM_CMICM_SUPPORT
void
pcid_dma_cmicm_rx_start(pcid_info_t *pcid_info, int ch)
{
    /*
     * This notifies the pcid_dma_rx_check polling routine that a DCB is
     * executing.
     */

/*    rx_dcb_addr[ch] = PCIM(pcid_info, CMIC_CMCx_DMA_DESCy_OFFSET(CMC0, ch)); */
   rx_dcb_addr[ch] = pcid_reg_read(pcid_info, CMIC_CMCx_DMA_DESCy_OFFSET(CMC0, ch));

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("pcid_dma_cmicm_rx_start: ch=%d dcb=0x%08x\n"),
              ch, rx_dcb_addr[ch]));
}
#endif

void
pcid_dma_rx_check(pcid_info_t *pcid_info, int chan)
{
    uint32 		rx_dcb_addr_next;
    int 		cnt, len, xfer;
    int 		eop, drop_pkt = 0;
    uint32		dma_ctrl;
    int                 word_count=8; /* FB/ER 3 + 5 words of HG + PBE */

    assert(chan >= 0 && chan < N_DMA_CHAN);
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(pcid_info->unit, soc_feature_cmicm)) {
        pcid_dma_cmicm_rx_check(pcid_info, chan);
        return;
    }
#endif

    PCID_MEM_LOCK(pcid_info);

/*    dma_ctrl = PCIM(pcid_info, CMIC_DMA_CTRL); */
    dma_ctrl = pcid_reg_read(pcid_info, CMIC_DMA_CTRL);

    if ((dma_ctrl & DC_DIRECTION_MASK(chan)) != DC_SOC_TO_MEM(chan)) {
        PCID_MEM_UNLOCK(pcid_info);
        return;		/* Channel not configured for RX */
    }

    if ((pcid_reg_read(pcid_info, CMIC_DMA_STAT) & DS_DMA_EN_TST(chan)) == 0) {
        PCID_MEM_UNLOCK(pcid_info);
        return;		/* Channel not enabled */
    }

    if (rx_dcb_addr[chan] == 0) {
        PCID_MEM_UNLOCK(pcid_info);
        return;		/* Should not happen */
    }

    

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("pcid_dma_rx_check: dcb=0x%08x\n"), rx_dcb_addr[chan]));

    if (rx_dcb[chan] == NULL) {
	rx_dcb[chan] = sal_alloc(SOC_DCB_SIZE(pcid_info->unit), "rxc_dcb");
	if (rx_dcb[chan] == NULL) {
        PCID_MEM_UNLOCK(pcid_info);
        return;	/* Yow! */
	}
    }
    rx_dcb_addr_next = pcid_dcb_fetch(pcid_info, rx_dcb_addr[chan],
                                      rx_dcb[chan]);

    if (SOC_DCB_RELOAD_GET(pcid_info->unit, rx_dcb[chan]) &&
        (pcid_reg_read(pcid_info, CMIC_CONFIG) & CC_RLD_OPN_EN)) {
	rx_dcb_addr_next = SOC_DCB_PADDR_GET(pcid_info->unit,
					     rx_dcb[chan]);

	LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META("pcid_dma_rx_check: reload to 0x%08x\n"),
                  rx_dcb_addr_next));
    } else {
        sal_mutex_take(pcid_info->pkt_mutex, sal_mutex_FOREVER);
        if (pcid_info->pkt_list == 0) {
            sal_mutex_give(pcid_info->pkt_mutex);
            PCID_MEM_UNLOCK(pcid_info);
            return;		/* No pending packets anyway */
        }
	eop = !SOC_DCB_SG_GET(pcid_info->unit, rx_dcb[chan]);
	cnt = SOC_DCB_REQCOUNT_GET(pcid_info->unit, rx_dcb[chan]);

	len = pcid_info->pkt_list->length - pcid_info->pkt_list->consum;

	LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META("pcid_dma_rx_check: eop=%d cnt=%d length=%d consum=%d len=%d\n"),
                  eop, cnt, pcid_info->pkt_list->length,
                  pcid_info->pkt_list->consum, len));

	/* Clear status */
	SOC_DCB_STATUS_INIT(pcid_info->unit, rx_dcb[chan]);

	if (len <= cnt) { /* Room to receive whole packet in current DCB */
	    xfer = len;
	    SOC_DCB_RX_END_SET(pcid_info->unit, rx_dcb[chan], 1);
	    drop_pkt = 1;
            eop = 1;
	} else {	/* Part of packet is received */
	    xfer = cnt;
	    drop_pkt = eop;
	}
	SOC_DCB_XFERCOUNT_SET(pcid_info->unit, rx_dcb[chan], xfer);
	SOC_DCB_RX_START_SET(pcid_info->unit, rx_dcb[chan],
				    (pcid_info->pkt_list->consum == 0));
	SOC_DCB_RX_ERROR_SET(pcid_info->unit, rx_dcb[chan], 0);
	SOC_DCB_RX_CRC_SET(pcid_info->unit, rx_dcb[chan], 1);
        SOC_DCB_RX_END_SET(pcid_info->unit, rx_dcb[chan], eop);
        if ((SOC_DCB_TYPE(pcid_info->unit) >= 9) &&
            (SOC_DCB_TYPE(pcid_info->unit) <=31)) {
            uint32      *up;
            int         i;
            up = rx_dcb[chan];
            up += 2; /* skip 2 words */

#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(pcid_info->unit)) {
                word_count = 13; /* 4 + 7 words of HG + PBE */
            }
#endif /* BCM_TRX_SUPPORT */

            for (i = 0; i < word_count; i++) {
                up[i] = pcid_info->pkt_list->dcbd[i];
            }
        }

	soc_internal_memory_store(pcid_info,
                      SOC_DCB_PADDR_GET(pcid_info->unit, rx_dcb[chan]),
		      &pcid_info->pkt_list->data[pcid_info->pkt_list->consum],
		      xfer,
		      MF_ES_DMA_PACKET);
        /* Now indicate this part of packet is consumed */
        pcid_info->pkt_list->consum += xfer;
        sal_mutex_give(pcid_info->pkt_mutex);
    }

    SOC_DCB_DONE_SET(pcid_info->unit, rx_dcb[chan], 1);

    pcid_dcb_store(pcid_info, rx_dcb_addr[chan], rx_dcb[chan]);

/*    PCIM(pcid_info, CMIC_DMA_STAT) |= DS_DESC_DONE_TST(chan); */
    pcid_reg_or_write(pcid_info, CMIC_DMA_STAT, DS_DESC_DONE_TST(chan));

/*    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_DESC_DONE(chan); */
    pcid_reg_or_write(pcid_info, CMIC_IRQ_STAT, IRQ_DESC_DONE(chan));

    if (SOC_DCB_CHAIN_GET(pcid_info->unit, rx_dcb[chan])) {
	rx_dcb_addr[chan] = rx_dcb_addr_next;
    } else {
        if (SOC_DCB_RELOAD_GET(pcid_info->unit, rx_dcb[chan])) {
            rx_dcb_addr[chan] = rx_dcb_addr_next;
        } else {
            rx_dcb_addr[chan] = 0;
        }

	/* Notify completion */

/*	PCIM(pcid_info, CMIC_DMA_STAT) |= DS_CHAIN_DONE_TST(chan); */
        pcid_reg_or_write(pcid_info, CMIC_DMA_STAT, DS_CHAIN_DONE_TST(chan));
/*	PCIM(pcid_info, CMIC_DMA_STAT) &= ~DS_DMA_ACTIVE(chan); */
        pcid_reg_and_write(pcid_info, CMIC_DMA_STAT, DS_DMA_ACTIVE(chan));

/*	PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_CHAIN_DONE(chan); */
        pcid_reg_or_write(pcid_info, CMIC_IRQ_STAT, IRQ_CHAIN_DONE(chan));
/*	PCIM(pcid_info, CMIC_IRQ_STAT) &= ~(IRQ_DESC_DONE(chan)); */
        pcid_reg_and_write(pcid_info, CMIC_IRQ_STAT, IRQ_DESC_DONE(chan));
    }

    soc_internal_send_int(pcid_info);

    if (drop_pkt) {
	packet_t		*tmp;

        sal_mutex_take(pcid_info->pkt_mutex, sal_mutex_FOREVER);
	tmp = pcid_info->pkt_list;
	pcid_info->pkt_list = pcid_info->pkt_list->next;
        pcid_info->pkt_count--;
        sal_mutex_give(pcid_info->pkt_mutex);
	sal_free(tmp);
    }
    PCID_MEM_UNLOCK(pcid_info);
}

#ifdef BCM_CMICM_SUPPORT
void
pcid_dma_cmicm_rx_check(pcid_info_t *pcid_info, int chan)
{
    uint32 		rx_dcb_addr_next;
    int 		cnt, len, xfer;
    int 		eop, drop_pkt = 0;
    uint32		dma_ctrl;
    int                 word_count=8; /* FB/ER 3 + 5 words of HG + PBE */

    assert(chan >= 0 && chan < N_DMA_CHAN);

    PCID_MEM_LOCK(pcid_info);

/*    dma_ctrl = PCIM(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, chan)); */
    dma_ctrl = pcid_reg_read(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, chan));

    if (dma_ctrl & PKTDMA_DIRECTION) {
        PCID_MEM_UNLOCK(pcid_info);
        return;		/* Channel not configured for RX */
    }

    if ((dma_ctrl & PKTDMA_ENABLE) == 0) {
        PCID_MEM_UNLOCK(pcid_info);
        return;		/* Channel not enabled */
    }

    if (rx_dcb_addr[chan] == 0) {
        PCID_MEM_UNLOCK(pcid_info);
        return;		/* Should not happen */
    }

    

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("pcid_dma_cmicm_rx_check: dcb=0x%08x\n"), rx_dcb_addr[chan]));

    if (rx_dcb[chan] == NULL) {
	rx_dcb[chan] = sal_alloc(SOC_DCB_SIZE(pcid_info->unit), "rxc_dcb");
	if (rx_dcb[chan] == NULL) {
        PCID_MEM_UNLOCK(pcid_info);
        return;	/* Yow! */
	}
    }
    rx_dcb_addr_next = pcid_dcb_fetch(pcid_info, rx_dcb_addr[chan],
                                      rx_dcb[chan]);

    if (SOC_DCB_RELOAD_GET(pcid_info->unit, rx_dcb[chan])) {
	rx_dcb_addr_next = SOC_DCB_PADDR_GET(pcid_info->unit,
					     rx_dcb[chan]);

	LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META("pcid_dma_cmicm_rx_check: reload to 0x%08x\n"),
                  rx_dcb_addr_next));
    } else {
        sal_mutex_take(pcid_info->pkt_mutex, sal_mutex_FOREVER);
        if (pcid_info->pkt_list == 0) {
            sal_mutex_give(pcid_info->pkt_mutex);
            PCID_MEM_UNLOCK(pcid_info);
            return;		/* No pending packets anyway */
        }
	eop = !SOC_DCB_SG_GET(pcid_info->unit, rx_dcb[chan]);
	cnt = SOC_DCB_REQCOUNT_GET(pcid_info->unit, rx_dcb[chan]);

	len = pcid_info->pkt_list->length - pcid_info->pkt_list->consum;

	LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META("pcid_dma_cmicm_rx_check: eop=%d cnt=%d length=%d consum=%d len=%d\n"),
                  eop, cnt, pcid_info->pkt_list->length,
                  pcid_info->pkt_list->consum, len));

	/* Clear status */
	SOC_DCB_STATUS_INIT(pcid_info->unit, rx_dcb[chan]);

	if (len <= cnt) { /* Room to receive whole packet in current DCB */
	    xfer = len;
	    SOC_DCB_RX_END_SET(pcid_info->unit, rx_dcb[chan], 1);
	    drop_pkt = 1;
            eop = 1;
	} else {	/* Part of packet is received */
	    xfer = cnt;
	    drop_pkt = eop;
	}
	SOC_DCB_XFERCOUNT_SET(pcid_info->unit, rx_dcb[chan], xfer);
	SOC_DCB_RX_START_SET(pcid_info->unit, rx_dcb[chan],
				    (pcid_info->pkt_list->consum == 0));
	SOC_DCB_RX_ERROR_SET(pcid_info->unit, rx_dcb[chan], 0);
	SOC_DCB_RX_CRC_SET(pcid_info->unit, rx_dcb[chan], 1);
        SOC_DCB_RX_END_SET(pcid_info->unit, rx_dcb[chan], eop);
        if ((SOC_DCB_TYPE(pcid_info->unit) >= 9) &&
            (SOC_DCB_TYPE(pcid_info->unit) <= 31)) {
            uint32      *up;
            int         i;
            up = rx_dcb[chan];
            up += 2; /* skip 2 words */

#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(pcid_info->unit)) {
                word_count = 13; /* 4 + 7 words of HG + PBE */
            }
#endif /* BCM_TRX_SUPPORT */

            for (i = 0; i < word_count; i++) {
                up[i] = pcid_info->pkt_list->dcbd[i];
            }
        }

	soc_internal_memory_store(pcid_info,
                      SOC_DCB_PADDR_GET(pcid_info->unit, rx_dcb[chan]),
		      &pcid_info->pkt_list->data[pcid_info->pkt_list->consum],
		      xfer,
		      MF_ES_DMA_PACKET);
        /* Now indicate this part of packet is consumed */
        pcid_info->pkt_list->consum += xfer;
        sal_mutex_give(pcid_info->pkt_mutex);
    }

    SOC_DCB_DONE_SET(pcid_info->unit, rx_dcb[chan], 1);

    pcid_dcb_store(pcid_info, rx_dcb_addr[chan], rx_dcb[chan]);

/*    PCIM(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET) |= DS_CMCx_DMA_DESC_DONE(chan); */
    pcid_reg_or_write(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET, DS_CMCx_DMA_DESC_DONE(chan));

/*    PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) |= IRQ_CMCx_DESC_DONE(chan); */
    pcid_reg_or_write(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET, IRQ_CMCx_DESC_DONE(chan));

    if (SOC_DCB_CHAIN_GET(pcid_info->unit, rx_dcb[chan])) {
	rx_dcb_addr[chan] = rx_dcb_addr_next;
    } else {
        if (SOC_DCB_RELOAD_GET(pcid_info->unit, rx_dcb[chan])) {
            rx_dcb_addr[chan] = rx_dcb_addr_next;
        } else {
            rx_dcb_addr[chan] = 0;
        }

	/* Notify completion */

/*	PCIM(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET) |= DS_CMCx_DMA_CHAIN_DONE(chan); */
        pcid_reg_or_write(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET, DS_CMCx_DMA_CHAIN_DONE(chan));
/*	PCIM(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET) &= ~DS_CMCx_DMA_ACTIVE(chan); */
        pcid_reg_and_write(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET, DS_CMCx_DMA_ACTIVE(chan));

/*	PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) |= IRQ_CMCx_CHAIN_DONE(chan); */
        pcid_reg_or_write(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET, IRQ_CMCx_CHAIN_DONE(chan));
/*	PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) &= ~(IRQ_CMCx_DESC_DONE(chan)); */
        pcid_reg_or_write(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET, IRQ_CMCx_DESC_DONE(chan));
    }

    soc_internal_cmicm_send_int(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK0_OFFSET);

    if (drop_pkt) {
	packet_t		*tmp;

        sal_mutex_take(pcid_info->pkt_mutex, sal_mutex_FOREVER);
	tmp = pcid_info->pkt_list;
	pcid_info->pkt_list = pcid_info->pkt_list->next;
        pcid_info->pkt_count--;
        sal_mutex_give(pcid_info->pkt_mutex);
	sal_free(tmp);
    }
    PCID_MEM_UNLOCK(pcid_info);
}
#endif

void
pcid_dma_start_chan(pcid_info_t *pcid_info, int ch)
{
    uint32 dma_ctrl;
    int tx, modbmp, abort, intrdesc, drop_tx;

/*    PCIM(pcid_info, CMIC_DMA_STAT) |= DS_DMA_ACTIVE(ch); */
    pcid_reg_or_write(pcid_info, CMIC_DMA_STAT, DS_DMA_ACTIVE(ch));

/*    dma_ctrl = PCIM(pcid_info, CMIC_DMA_CTRL); */
    dma_ctrl = pcid_reg_read(pcid_info, CMIC_DMA_CTRL);

    tx = ((dma_ctrl & DC_DIRECTION_MASK(ch)) == DC_MEM_TO_SOC(ch));
    modbmp = ((dma_ctrl & DC_MOD_BITMAP_MASK(ch)) == DC_MOD_BITMAP(ch));
    abort = ((dma_ctrl & DC_ABORT_DMA_MASK(ch)) == DC_ABORT_DMA(ch));
    intrdesc = ((dma_ctrl & DC_INTR_ON_MASK(ch)) == DC_INTR_ON_DESC(ch));
    drop_tx = ((dma_ctrl & DC_DROP_TX_MASK(ch)) == DC_DROP_TX(ch));

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("Starting DMA on channel %d\n"), ch));

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("Mode: %s %s %s %s %s\n"),
              tx ? "TX" : "RX",
              modbmp ? "MOD_BMP" : "NO_MOD_BMP",
              abort ? "ABORT" : "NO_ABORT",
              intrdesc ? "INTR_ON_DESC" : "INTR_ON_PKT",
              drop_tx ? "DROP_TX" : "NO_DROP_TX"));

    if (tx) {
	pcid_dma_tx_start(pcid_info, ch);
    } else {
	pcid_dma_rx_start(pcid_info, ch);
    }
}

#ifdef BCM_CMICM_SUPPORT
void
pcid_dma_cmicm_start_chan(pcid_info_t *pcid_info, int ch)
{
    uint32 dma_ctrl;
    int tx, en, abort, intrdesc;

/*    PCIM(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET) |= DS_CMCx_DMA_ACTIVE(ch); */
    pcid_reg_or_write(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET, DS_CMCx_DMA_ACTIVE(ch));

/*    dma_ctrl = PCIM(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, ch)); */
    dma_ctrl = pcid_reg_read(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, ch));

    tx = (dma_ctrl & PKTDMA_DIRECTION);
    en = (dma_ctrl & PKTDMA_ENABLE);
    abort = (dma_ctrl & PKTDMA_ABORT);
    intrdesc = (dma_ctrl & PKTDMA_SEL_INTR_ON_DESC_OR_PKT);

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("Starting DMA on channel %d\n"), ch));

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META("Mode: %s %s %s %s\n"),
              tx ? "TX" : "RX",
              en ? "ENABLE" : "NO_ENABLE",
              abort ? "ABORT" : "NO_ABORT",
              intrdesc ? "INTR_ON_DESC" : "INTR_ON_PKT"));

    if (tx) {
	pcid_dma_cmicm_tx_start(pcid_info, ch);
    } else {
	pcid_dma_cmicm_rx_start(pcid_info, ch);
    }
}
#endif

void
pcid_dma_stat_write(pcid_info_t *pcid_info, uint32 value)
{
    uint32	old_stat = pcid_reg_read(pcid_info, CMIC_DMA_STAT); /* Initial value */
    uint32	new_stat;
    int		ch;

    /*
     * In the hardware, the DMA is triggered by a couple conditions:
     *	1) DMA Enable == 1
     *	2) Chain_done == 0
     *
     * DMA must be triggered as soon as all of these conditions are true.
     */
    if (value & 0x80) {			/* Set a bit */
/*	new_stat = PCIM(pcid_info, CMIC_DMA_STAT) |= 1 << (value & 0x1f); */
        pcid_reg_or_write(pcid_info, CMIC_DMA_STAT, 1 << (value & 0x1f));
        new_stat = pcid_reg_read(pcid_info, CMIC_DMA_STAT);
    } else {
/*	new_stat = PCIM(pcid_info, CMIC_DMA_STAT) &= ~(1 << (value & 0x1f)); */
        pcid_reg_and_write(pcid_info, CMIC_DMA_STAT, 1 << (value & 0x1f));
        new_stat = pcid_reg_read(pcid_info, CMIC_DMA_STAT);
    }

    /*
     * Now look at differences and descide what action to take. This
     * could be optimized since only one channel changed, but for now take
     * the easy approach.
     */

    for (ch = 0; ch < N_DMA_CHAN; ch++) {
	int	old_enable, old_chain_done, old_desc_done;
	int	new_enable, new_chain_done, new_desc_done;

	old_enable = DS_DMA_EN_TST(ch) & old_stat;
	old_chain_done = DS_CHAIN_DONE_TST(ch) & old_stat;
	old_desc_done = DS_DESC_DONE_TST(ch) & old_stat;

	new_enable = DS_DMA_EN_TST(ch) & new_stat;
	new_chain_done = DS_CHAIN_DONE_TST(ch) & new_stat;
	new_desc_done = DS_DESC_DONE_TST(ch) & new_stat;

	/*
	 * Check for interrupt changes - this MUST be before the DMA
	 * check below to avoid clearing status from a potentially
	 * started DMA.
	 */

	if (old_desc_done && !new_desc_done) { /* Clear interrupt */
/*	    PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_DESC_DONE(ch); */
           pcid_reg_and_write(pcid_info, CMIC_IRQ_STAT, IRQ_DESC_DONE(ch));
	}

	if (old_chain_done && !new_chain_done) { /* Clear interrupt */
/*	    PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_CHAIN_DONE(ch); */
           pcid_reg_and_write(pcid_info, CMIC_IRQ_STAT, IRQ_CHAIN_DONE(ch));
	}

	/* Trigger a DMA ? */

	if ((!old_enable || old_chain_done) && /* Not running before */
	    (new_enable && !new_chain_done)) { /* Should start */
	    pcid_dma_start_chan(pcid_info, ch);
	}
    }
}

void
pcid_dma_ctrl_write(pcid_info_t *pcid_info, uint32 value)
{
    int		ch;

    pcid_reg_write(pcid_info, CMIC_DMA_CTRL, value);

    for (ch = 0; ch < N_DMA_CHAN; ch++) {
	if (value & DC_ABORT_DMA(ch)) {
/*           PCIM(pcid_info, CMIC_DMA_STAT) &= ~DS_DMA_ACTIVE(ch); */
           pcid_reg_and_write(pcid_info, CMIC_DMA_STAT, DS_DMA_ACTIVE(ch));
	}
    }
}

#ifdef BCM_CMICM_SUPPORT
void
pcid_dma_cmicm_ctrl_write(pcid_info_t *pcid_info, uint32 reg, uint32 value)
{
    int    ch; 
    uint32 old_stat;
    uint32 new_ctrl, old_ctrl;
    int	old_enable, old_chain_done;
    int	new_enable;
    int active = 0;

    switch (reg) {
    case CMIC_CMC0_CH0_DMA_CTRL_OFFSET:
        ch = 0;
        break;
    case CMIC_CMC0_CH1_DMA_CTRL_OFFSET:
        ch = 1;
        break;
    case CMIC_CMC0_CH2_DMA_CTRL_OFFSET:
        ch = 2;
        break;
    case CMIC_CMC0_CH3_DMA_CTRL_OFFSET:
        ch = 3;
        break;
    default:
        assert(0);
        return;
    }

/*    old_stat = PCIM(pcid_info, CMIC_CMCx_DMA_STAT_OFFSET(CMC0)); */
    old_stat = pcid_reg_read(pcid_info, CMIC_CMCx_DMA_STAT_OFFSET(CMC0));
/*    old_ctrl = PCIM(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, ch)); */
    old_ctrl = pcid_reg_read(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, ch));
/*    PCIM(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, ch)) = value; */
    pcid_reg_write(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, ch), value);
/*    new_ctrl = PCIM(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, ch)); */
    new_ctrl = pcid_reg_read(pcid_info, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(CMC0, ch));

    /* handle enable and abort */
    active = DS_CMCx_DMA_ACTIVE(ch) & old_stat;
    old_enable = PKTDMA_ENABLE & old_ctrl;
    old_chain_done = DS_CMCx_DMA_CHAIN_DONE(ch) & old_stat;
    new_enable = PKTDMA_ENABLE & new_ctrl;
 
    if (!new_enable) {
/*        PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) &= ~(IRQ_CMCx_DESC_DONE(ch)); */
       pcid_reg_and_write(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET, (IRQ_CMCx_DESC_DONE(ch)));
/*        PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) &= ~(IRQ_CMCx_CHAIN_DONE(ch)); */
       pcid_reg_and_write(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET, IRQ_CMCx_CHAIN_DONE(ch));
    }

    /* Trigger a DMA ? */
    if ((!old_enable || old_chain_done) && /* Not running before */
	 !active && new_enable) { /* Should start */
	 pcid_dma_cmicm_start_chan(pcid_info, ch);
    } else if (active && (new_ctrl & PKTDMA_ABORT)) { /* or Abort DMA */
/*         PCIM(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET) &= ~DS_CMCx_DMA_ACTIVE(ch); */
       pcid_reg_and_write(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET, DS_CMCx_DMA_ACTIVE(ch));
         LOG_CLI((BSL_META("Abort on ch: %d !!\n"), ch));
    }
}
#endif

void
pcid_dma_stop_all(pcid_info_t *pcid_info) 
{
    int ch;

    LOG_CLI((BSL_META("Abort and Disable  DMA\n")));
    
    /* 
     * Abort and disable all DMA channels when the client exits so that
     * the PCID does not poll for the old DMA when the client connects again.
     */ 
#ifdef BCM_CMICM_SUPPORT
    if (pcid_info->cmicm >= CMC0) {
/*        PCIM(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET) = 0; */
       pcid_reg_write(pcid_info, CMIC_CMC0_DMA_STAT_OFFSET, 0);
    }
#endif
    for (ch = 0; ch < N_DMA_CHAN; ch++) {
#ifdef BCM_CMICM_SUPPORT
        if (pcid_info->cmicm >= CMC0) {
            pcid_dma_cmicm_ctrl_write(pcid_info, 
                   CMIC_CMCx_CHy_DMA_CTRL_OFFSET(pcid_info->cmicm, ch), PKTDMA_ABORT);
/*            PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) &= ~(IRQ_CMCx_DESC_DONE(ch)); */
            pcid_reg_and_write(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET, IRQ_CMCx_DESC_DONE(ch));
/*            PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) &= ~(IRQ_CMCx_CHAIN_DONE(ch)); */
            pcid_reg_and_write(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET, IRQ_CMCx_CHAIN_DONE(ch));
        } else 
#endif
        {
            pcid_dma_ctrl_write(pcid_info, DC_ABORT_DMA(ch)); /* Abort current DMA */
            pcid_dma_stat_write(pcid_info, (0x00 | ch));      /* Disable DMA */
        }
        rx_dcb_addr[ch] = 0x0;                           
    }
}
