/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        socdma.h
 * Purpose:     Maps out structures used for DMA operations and
 *              exports routines.
 */

#ifndef _SOC_DMA_H
#define _SOC_DMA_H

#include <sal/types.h>
#include <sal/core/sync.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>

#include <shared/types.h>

#include <soc/types.h>
#include <soc/cmic.h>
#include <soc/dcb.h>

#if defined(BE_HOST) && defined(LE_HOST)
#error "Define either BE_HOST or LE_HOST. Both BE_HOST and LE_HOST should not be defined at the same time"
#endif

#define SOC_DMA_ROUND_LEN(x)    (((x) + 3) & ~3)

/* Type for DMA channel number */

typedef _shr_dma_chan_t    dma_chan_t;

/*
 * Defines:
 *      SOC_DMA_DV_FREE_CNT/SOC_DMA_DV_FREE_SIZE
 * Purpose:
 *      Defines the number of free DV's that the DMA code will cache
 *      to avoid calling alloc/free routines. FREE_CNT is number of 
 *      dv_t's, and FREE_SIZE is the number of DCBs in the DV. 
 * Notes:
 *      Allocation requests for DV's with < FREE_SIZE dcbs MAY result in 
 *      an allocation of a DV with FREE_SIZE dcbs. Allocations with 
 *      > FREE_SIZE will always result in calls to memory allocation
 *      routines.
 */

#define SOC_DMA_DV_FREE_CNT     32
#define SOC_DMA_DV_FREE_SIZE    160    /*Common DCBs *(1 + Extra DCBs)*/

#define SOC_DV_PKTS_MAX         32     /* Determines alloc size of
                                           DMA'able data in DVs */

typedef int8    dvt_t;                  /* DV Type definition */

#define         DV_NONE         0       /* Disable/Invalid */
#define         DV_TX           1       /* Transmit data */
#define         DV_RX           2       /* Receive data  */

#define SOC_DMA_DV_TX_FREE_SIZE 3
#define SOC_DMA_DV_RX_FREE_SIZE 1

#define CMICX_DMA_CONTIGUOUS_DESCS    8 /* Max no. of contiguous descriptors */

/*
 * Typedef:
 *      soc_tx_param_t
 * Purpose:
 *      Strata XGS hardware specific info related to packet TX;
 *      Will be used as a vehicle for passing information to
 *      the DCB-Add-TX operation.
 *      Information is per packet.  The data buffer pointer and
 *      byte count is passed separately.
 */

typedef struct soc_tx_param_s {
    uint8             cos;          /* The local COS# to use */
    uint8             prio_int;     /* Internal priority of the packet */
    uint8             src_port;     /* Header/tag ONLY.  Use rx_port below */
    uint8             src_mod;
    uint8             dest_port;    /* Header/tag ONLY.  Use tx_pbmp below */
    uint8             dest_mod;
    uint8             opcode;
    uint8             pfm;          /* see BCM_PORT_PFM_* */

    soc_pbmp_t        tx_pbmp;      /* Target ports */
    soc_pbmp_t        tx_upbmp;     /* Untagged ports */
    soc_pbmp_t        tx_l3pbmp;    /* L3 ports */

    /* Uses SOC_DMA and SOC_DMA_F flags from below */
    uint32            flags;

#if 0 /* Not yet */
    /* Representations for HiGig header data (12 bytes) */
    union {
        uint8         higig8[12];
        uint32        higig32[3];
        /* Add higig_hdr_t here */
    } higig_u;

/* Accessors for the HiGig header in the soc_tx_param_t structure */
#define SOC_TXP_HIGIG8(data)     ((data).higig_u.higig8)
#define SOC_TXP_HIGIG32(data)    ((data).higig_u.higig32)
#define SOC_TXP_HG_HDR(data)     ((data).higig_u.hg_hdr)

#endif

    /* Other new headers/tags may be added here */
} soc_tx_param_t;

/*
 * Typedef:
 *      dv_t
 * Purpose:
 *      Maps out the I/O Vec structure used to pass DMA chains to the
 *      the SOC DMA routines "DMA I/O Vector".
 * Notes:
 *      To start a DMA request, the caller must fill in:
 *              dv_op: operation to perform (DV_RX or DV_TX).
 *              dv_flags: Set DV_F_NOTIFY_DSC for descriptor done callout
 *                        Set DV_F_NOTIFY_CHN for chain done callout
 *                        Set DV_F_WAIT to suspend in dma driver
 *              dv_valid: # valid DCB entries (this field is initialized
 *                      by soc_dma_dv_alloc, and set properly if
 *                      soc_dma_add_dcb is called to build chain).
 *              dv_done_chain: NULL if no callout for chain done, or the
 *                      address of the routine to call when chain done
 *                      is seen. It is called with 2 parameters, the
 *                      unit # and a pointer to the DV chain done has been
 *                      seen on.
 *              dv_done_desc: NULL for synchronous call, or the address of
 *                      the function to call on descriptor done. The function
 *                      is with 3 parameters, the unit #, a pointer to
 *                      the DV structure, and a pointer to the DCB completed.
 *                      One call is made for EVERY DCB, and only if the
 *                      DCB is DONE.
 *              dv_done_packet: NULL if no callout for packet done, or the
 *                      address of the routine to call when packet done
 *                      is seen. It has the same prototype as dv_done_desc.
 *              dv_public1 - 4: Not used by DMA routines,
 *                      for use by caller.
 *
 *     Scatter/gather is implemented through multiple DCBs pointing to
 *     different buffers with the S/G bit set.  End of S/G chain (end of
 *     packet) is indicated by having S/G bit clear in the DCB.
 *
 *     Chains of packets can be associated with a single DV.  This
 *     keeps the HW busy DMA'ing packets even as interrupts are
 *     processed.  DVs can be chained (a software construction)
 *     which will start a new DV from this file rather than calling
 *     back.  This is not done much in our code.
 */

typedef struct dv_s {
    struct dv_s *dv_next,               /* Queue pointers if required */
                *dv_chain;              /* Pointer to next DV in chain */
    int         dv_unit;                /* Unit dv is allocated on */
    uint32      dv_magic;               /* Used to indicate valid */
    dvt_t       dv_op;                  /* Operation to be performed */
    dma_chan_t  dv_channel;             /* Channel queued on */
    int         dv_flags;               /* Flags for operation */
    /* Fix soc_dma_dv_reset if you add flags */
#   define      DV_F_NOTIFY_DSC         0x01    /* Notify on dsc done */
#   define      DV_F_NOTIFY_CHN         0x02    /* Notify on chain done */
#   define      DV_F_COMBINE_DCB        0x04    /* Combine DCB where poss. */
#   define      DV_F_NEEDS_REFILL       0x10    /* Needs to be refilled */
    int         dv_cnt;                 /* # descriptors allocated */
    int         dv_vcnt;                /* # descriptors valid */
    int         dv_dcnt;                /* # descriptors done */
    void        (*dv_done_reload)(int u, struct dv_s *dv_chain);
    void        (*dv_done_chain)(int u, struct dv_s *dv_chain);
    void        (*dv_done_desc)(int u, struct dv_s *dv, dcb_t *dcb);
    void        (*dv_done_desc_subs)(int u, struct dv_s *dv, dcb_t *dcb);
    void        (*dv_done_packet)(int u, struct dv_s *dv, dcb_t *dcb);
    any_t       dv_public1;             /* For caller */
    any_t       dv_public2;             /* For caller */
    any_t       dv_public3;             /* For caller */
    any_t       dv_public4;             /* For caller */

    /*
     * Information for SOC_TX_PKT_PROPERTIES.
     * Normally, packets are completely independent across a DMA TX chain.
     * In order to program the cmic dma tx register effeciently, the data
     * in soc_tx_param must be consistent for all packets in the chain.
     */
    soc_tx_param_t   tx_param;

    /*
     * Buffer for gather-inserted data.  Possibly includes:
     *     HiGig hdr     (12 bytes)
     *     HiGig2 hdr    (16 bytes) (HG2/SL mutually exclusive)
     *     SL tag        (4 bytes)
     *     VLAN tag      (4 bytes)
     *     Dbl VLAN tag  (4 bytes)
     */
    uint8       *dv_dmabuf;
    uint32      dv_dma_buf_size;
#if defined(BCM_CMICX_SUPPORT) && defined(BCM_CMICX_TX_PREALLOC_SUPPORT)
#define SOC_DMA_BUF_LEN         44
#else
#define SOC_DMA_BUF_LEN         24      /* Maximum header size from above */
#endif /* BCM_CMICX_SUPPORT && BCM_CMICX_TX_PREALLOC_SUPPORT */
#define SOC_DV_DMABUF_SIZE      (SOC_DV_PKTS_MAX * SOC_DMA_BUF_LEN)
#define SOC_DV_HG_HDR(dv, i)    (&((dv)->dv_dmabuf[SOC_DMA_BUF_LEN*i+0]))
#define SOC_DV_SL_TAG(dv, i)    (&((dv)->dv_dmabuf[SOC_DMA_BUF_LEN*i+12]))
#define SOC_DV_VLAN_TAG(dv, i)  (&((dv)->dv_dmabuf[SOC_DMA_BUF_LEN*i+16]))
#define SOC_DV_VLAN_TAG2(dv, i) (&((dv)->dv_dmabuf[SOC_DMA_BUF_LEN*i+20]))

#if defined(BCM_CMICX_SUPPORT) && defined(BCM_CMICX_TX_PREALLOC_SUPPORT)
#define SOC_DV_TX_HDR(dv, i)     (&((dv)->dv_dmabuf[SOC_DMA_BUF_LEN*i+24]))
#define SOC_DV_EXT_HG_HDR(dv, i) (&((dv)->dv_dmabuf[SOC_DMA_BUF_LEN*i+40]))
#endif /* BCM_CMICX_SUPPORT && BCM_CMICX_TX_PREALLOC_SUPPORT */

/* Optionally allocated for Tx alignment */
#define SOC_DV_TX_ALIGN(dv, i) (&((dv)->dv_dmabuf[dv->dv_dma_buf_size + 4 * i]))

    dcb_t       *dv_dcb;
} dv_t;

/* Poll type */
typedef enum soc_dma_poll_type_e {
    SOC_DMA_POLL_DESC_DONE = 0,
    SOC_DMA_POLL_CTRLD_DESC_DONE = 1,
    SOC_DMA_POLL_CHAIN_DONE = 2
} soc_dma_poll_type_t;

/*
 * Typedef:
 *      sdc_t (SOC DMA Control)
 * Purpose:
 *      Each DMA channel on each SOC has one of these structures to
 *      track the currently active or queued operations.
 */

typedef struct sdc_s {
    dma_chan_t      sc_channel;        /* Channel # for reverse lookup */
    dvt_t           sc_type;           /* DV type that we accept */
    uint8           sc_flags;          /* See SDMA_CONFIG_XXX */
    dv_t            *sc_q;             /* Request queue head */
    dv_t            *sc_q_tail;        /* Request queue tail */
    dv_t            *sc_dv_active;     /* Pointer to individual active DV */
    int             sc_q_cnt;          /* # requests queued + active */
    int             sc_dma_started;    /* Continuous DMA chan started */
} sdc_t;

/* CMIC DMA Driver Interface for CMIC type abstraction */
typedef struct soc_cmic_dma_drv_s {
    int (*init)(int unit);
    int (*ctrl_init)(int unit);
    int (*chan_abort)(int unit, int chan);
    int (*chan_tx_purge)(int unit, int chan, dv_t *dv);
    int (*chan_config)(int unit, int chan, dvt_t type, uint32 flags);
    int (*chan_dv_start)(int unit, sdc_t *sc, dv_t *dv);
    int (*chan_start)(int unit, sdc_t *sc);
    int (*chan_poll)(int unit, int chan, soc_dma_poll_type_t type, int *detected);
    int (*chan_desc_done_intr_enable)(int unit, int chan);
    int (*chan_desc_done)(int unit, int chan);
    int (*chan_chain_done)(int unit, int chan, int mitigation);
    int (*chan_reload_done)(int unit, int chan, dcb_t *dcb, int *rld_done);
    int (*chan_counter_get)(int unit, int chan, uint32 *tx_pkt, uint32 *rx_pkt);
    int (*chan_counter_clear)(int unit, int chan, uint32 mask);
    int (*chan_cos_ctrl_get)(int unit, int chan, uint32 mask, uint32 *cos_ctrl);
    int (*chan_cos_ctrl_set)(int unit, int chan, uint32 mask, uint32 cos_ctrl);
    int (*chan_cos_ctrl_queue_get)(int unit, int cmc, int chan, int queue, uint32 *cos_ctrl);
    int (*chan_cos_ctrl_reg_addr_get)(int unit, int cmc, int chan, int queue, uint32 *reg_addr);
    int (*chan_status_get)(int unit, int chan, uint32 mask, int *status);
    int (*chan_status_clear)(int unit, int chan);
    int (*chan_halt_get)(int unit, int chan, uint32 mask, int *active);
    int (*chan_ctrl_reg_get)(int unit, int chan, uint32 *val);
    int (*chan_ctrl_reg_set)(int unit, int chan, uint32 val);
    int (*chan_rxbuf_threshold_cfg)(int unit, int chan, uint32 val);
    int (*cmc_rxbuf_threshold_cfg)(int unit, int cmc, uint32 val);
    int (*cmc_counter_get)(int unit, int cmc, uint32 *tx_pkt, uint32 *rx_pkt);
    int (*cmc_counter_clear)(int unit, int cmc, uint32 mask);
    int (*masks_get)(int unit, uint32 *pkt_endian, uint32 *desc_endian, uint32 *header_endian);
    int (*loopback_config)(int unit, uint32 cfg);
    int (*header_size_get)(int unit, uint32 *hdr_size);
    int (*max_rx_channels_get)(int unit, uint32 *max_rx_channels);
    int (*pci_timeout_handle)(int unit);
} soc_cmic_dma_drv_t;

#define SOC_DMA_MODE(unit)(SOC_CONTROL(unit)->dma_mode)
#define SOC_DMA_DESC_PREFETCH(unit)(SOC_CONTROL(unit)->desc_prefetch)

/* Do not alter ext_dcb flag below */
#define SET_NOTIFY_CHN_ONLY(flags) do { \
    (flags) |=  DV_F_NOTIFY_CHN; \
    (flags) &= ~DV_F_NOTIFY_DSC; \
} while (0)

#define SET_NOTIFY_DSC_ONLY(flags) do { \
    (flags) |=  DV_F_NOTIFY_DSC; \
    (flags) &= ~DV_F_NOTIFY_CHN; \
} while (0)

/* Try to avoid all other flags */
#define         SOC_DMA_F_PKT_PROP      0x10000000    /* 1 << 28 */

/*
 * Note DMA_F_INTR is NOT a normal flag.
 *    Interrupt mode is the default behavior and is ! POLLED mode.
 */
#define         SOC_DMA_F_INTR          0x00 /* Interrupt Mode */

#define         SOC_DMA_F_MBM           0x01 /* Modify bit MAP */
#define         SOC_DMA_F_POLL          0x02 /* POLL mode */
#define         SOC_DMA_F_TX_DROP       0x04 /* Drop if no ports */
#define         SOC_DMA_F_JOIN          0x08 /* Allow low level DV joins */
#define         SOC_DMA_F_DEFAULT       0x10 /* Default channel for type */
#define         SOC_DMA_F_CLR_CHN_DONE  0x20 /* Clear Chain-done on start */
#define         SOC_DMA_F_INTR_ON_DESC  0x40 /* Interrupt per descriptor */

extern int      soc_dma_init(int unit);
extern int      soc_dma_attach(int unit, int reset);
extern int      soc_dma_detach(int unit);
extern void     soc_dma_chan_started_clear(int unit, dma_chan_t c);
extern int      soc_dma_chan_config(int unit, dma_chan_t chan, dvt_t type,
                                    uint32 flags);
extern dvt_t    soc_dma_chan_dvt_get(int unit, dma_chan_t chan);

extern dv_t     *soc_dma_dv_alloc(int unit, dvt_t, int cnt);
extern dv_t     *soc_dma_dv_alloc_by_port(int unit, dvt_t op, int cnt,
                                          int pkt_to_ports);
extern void     soc_dma_dv_free(int unit, dv_t *);
extern void     soc_dma_dv_reset(dvt_t, dv_t *);
extern int      soc_dma_dv_join(dv_t *dv_list, dv_t *dv_add);

extern int      soc_dma_desc_add(dv_t *, sal_vaddr_t, uint16, pbmp_t,
                                 pbmp_t, pbmp_t, uint32 flags, uint32 *hgh);
extern void     soc_dma_dump_dv_dcb(int unit, char *pfx,
                                    dv_t *dv_chain, int index);

extern int      soc_cmicx_dma_ctrl_release_cpu_credits(int unit);

/* CMIC driver functions */
extern int      soc_cmice_dma_drv_init(int unit, soc_cmic_dma_drv_t *drv);
extern int      soc_cmicm_dma_drv_init(int unit, soc_cmic_dma_drv_t *drv);
extern int      soc_cmicd_dma_drv_init(int unit, soc_cmic_dma_drv_t *drv);
extern int      soc_cmicx_dma_drv_init(int unit, soc_cmic_dma_drv_t *drv);
extern int      soc_pktio_dma_drv_init(int unit, soc_cmic_dma_drv_t *drv);

/* abort and disable previous DMA operations */
extern int soc_cmicm_dma_abort(int unit, uint32 flags);
extern int soc_cmicx_dma_abort(int unit, uint32 flags);
/* Handle DMA aborting and the iproc hot swap mechanism at startup */
extern int soc_cmicx_handle_hotswap_and_dma_abort(int unit);

/* flags specifying not to abort and disable some DMA types */
#define SOC_DMA_ABORT_SKIP_SBUS        1
#define SOC_DMA_ABORT_SKIP_PACKET      2
#define SOC_DMA_ABORT_SKIP_FIFO        4
#define SOC_DMA_ABORT_SKIP_CCM         8
#define SOC_DMA_ABORT_SKIP_SCHAN_FIFO 16
#define SOC_DMA_ABORT_CMC0            32
#define SOC_DMA_ABORT_DISCONNECT_PACKET 64 /* Before aborting, prevent receiving packet DMA from the device */


#define         SOC_DMA_COS(_x)         ((_x) << 0)
#define         SOC_DMA_COS_GET(_x)     (((_x) >> 0) & 7)
#define         SOC_DMA_CRC_REGEN       (1 << 3)
#define         SOC_DMA_CRC_GET(_x)     (((_x) >> 3) & 1)
#define         SOC_DMA_RLD             (1 << 4)
#define         SOC_DMA_HG              (1 << 22)
#define         SOC_DMA_STATS           (1 << 23)
#define         SOC_DMA_PURGE           (1 << 24)

/* Max 7 bits for mod, 6 bits for port and 1 for trunk indicator */
#define _SDP_MSK 0x3f           /* 10:5 */
#define _SDP_S 5                /* 10:5 */
#define _SDM_MSK 0x7f           /* 17:11 */
#define _SDM_S 11               /* 17:11 */
#define _SDT_MSK 0x1            /* 18:18 */
#define _SDT_S 18               /* 18:18 */
#define _SMHOP_MSK 0x7          /* 21:19 */
#define _SMHOP_S 19             /* 21:19 */

/* 
 * type 9 descriptor, Higig, stats, purge bits
 */

#define _SHG_MSK 0x1            /* 22:22 */
#define _SHG_S 22               /* 22:22 */
#define _SSTATS_MSK 0x1         /* 23:23 */
#define _SSTATS_S 23            /* 23:23 */
#define _SPURGE_MSK 0x1         /* 24:24 */
#define _SPURGE_S 24            /* 24:24 */

#define SOC_DMA_LOOPBACK_SET   0x1
#define SOC_DMA_LOOPBACK_RESET 0x2
#define SOC_DMA_COS_RX_0       0x1
#define SOC_DMA_COS_RX_1       0x2
#define SOC_DMA_TXPKT          0x1
#define SOC_DMA_RXPKT          0x2
#define SOC_DMA_RXPKT_DROP     0x4
#define SOC_DMA_STATUS_LOW     0x1
#define SOC_DMA_STATUS_HI      0x2

#define SOC_DMA_DPORT_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SDP_S, _SDP_MSK)
#define SOC_DMA_DPORT_SET(flags, val)  \
            SOC_SM_FLAGS_SET(flags, val, _SDP_S, _SDP_MSK)
       
#define SOC_DMA_DMOD_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SDM_S, _SDM_MSK)
#define SOC_DMA_DMOD_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SDM_S, _SDM_MSK)

#define SOC_DMA_DTGID_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SDT_S, _SDT_MSK)
#define SOC_DMA_DTGID_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SDT_S, _SDT_MSK)

#define SOC_DMA_MHOP_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SMHOP_S, _SMHOP_MSK)
#define SOC_DMA_MHOP_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SMHOP_S, _SMHOP_MSK)

#define SOC_DMA_HG_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SHG_S, _SHG_MSK)
#define SOC_DMA_HG_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SHG_S, _SHG_MSK)

#define SOC_DMA_STATS_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SSTATS_S, _SSTATS_MSK)
#define SOC_DMA_STATS_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SSTATS_S, _SSTATS_MSK)

#define SOC_DMA_PURGE_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SPURGE_S, _SPURGE_MSK)
#define SOC_DMA_PURGE_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SPURGE_S, _SPURGE_MSK)

extern int      soc_dma_rld_desc_add(dv_t *, sal_vaddr_t);
extern void     soc_dma_desc_end_packet(dv_t *);

extern int      soc_dma_contiguous_desc_add(dv_t *);

extern int      soc_dma_start(int unit, dma_chan_t, dv_t *);

extern int      soc_dma_abort_channel_total(int unit, dma_chan_t channel);
extern int      soc_dma_abort_dv(int unit, dv_t *);
extern int      soc_dma_abort(int unit);

extern int      soc_dma_chan_counter_get(int unit, dma_chan_t channel,
                                         uint32 *tx_pkt, uint32 *rx_pkt);
extern int      soc_dma_chan_counter_clear(int unit, dma_chan_t channel,
                                           uint32 mask);
extern int      soc_dma_loopback_config(int unit, uint32 cfg);
extern int      soc_dma_header_size_get(int unit, uint32 *hdr_size);
extern int      soc_dma_chan_cos_ctrl_get(int unit, dma_chan_t channel,
                                     uint32 cfg, uint32 *cos_ctrl);
extern int      soc_dma_chan_cos_ctrl_set(int unit, dma_chan_t channel,
                                     uint32 cfg, uint32 cos_ctrl);
extern int      soc_dma_cos_ctrl_queue_get(int unit, int cmc, int chan,
                                     int queue, uint32 *cos_ctrl);
extern int      soc_dma_cos_ctrl_reg_addr_get(int unit, int cmc, int chan,
                                              int queue, uint32 *reg_addr);
extern int      soc_dma_chan_poll_done(int unit, dma_chan_t channel,
                                        soc_dma_poll_type_t type,
                                        int *detected);
extern int      soc_dma_chan_status_get(int unit, dma_chan_t channel,
                                        uint32 mask, int *status);
extern int      soc_dma_chan_status_clear(int unit, dma_chan_t channel);
extern int      soc_dma_chan_halt_get(int unit, dma_chan_t channel,
                                        uint32 mask, int *active);
extern int      soc_dma_chan_rxbuf_threshold_cfg(int unit, dma_chan_t channel,
                                                 uint32 val);
extern int      soc_dma_cmc_rxbuf_threshold_cfg(int unit, int cmc,
                                                uint32 val);
extern int      soc_dma_chan_ctrl_reg_get(int unit, dma_chan_t channel,
                                          uint32 *val);
extern int      soc_dma_chan_ctrl_reg_set(int unit, dma_chan_t channel,
                                          uint32 val);
extern int      soc_dma_cmc_counter_get(int unit, int cmc,
                                         uint32 *tx_pkt, uint32 *rx_pkt);
extern int      soc_dma_cmc_counter_clear(int unit, int cmc,
                                           uint32 mask);
extern int      soc_dma_masks_get(int unit, uint32 *pkt_endian,
                                  uint32 *desc_endian, uint32 *header_endian);
extern int      soc_dma_max_rx_channels_get(int unit, uint32 *max_rx_channels);


/* Wait on synchronous send - requires a context */
extern int      soc_dma_wait(int unit, dv_t *dv_chain);
extern int      soc_dma_wait_timeout(int unit, dv_t *dv_chain, int usec);

extern void     soc_dma_higig_dump(int unit, char *pfx, uint8 *addr,
                                   int len, int pkt_len, int *ether_offset);
extern void     soc_dma_ether_dump(int unit, char *pfx, uint8 *addr,
                                   int len, int offset);
extern void     soc_dma_ep_to_cpu_hdr_dump(int unit, char *pfx, uint8 *addr,
                                          int len, int offset);
extern void     soc_dma_ep_to_cpu_hdr_decoded_dump(int unit, char *pfx, void *addr,
                                          int skip_mhdr, int hdr_size);
extern void     soc_dma_usage_get(int unit);

extern int      soc_dma_dv_valid(dv_t *dv);
extern void     soc_dma_dump_dv(int unit, char *pfx, dv_t *);
extern void     soc_dma_dump_pkt(int unit, char *pfx, uint8 *addr, int len,
                                 int decode);

/* Interrupt Routines */

extern void     soc_dma_done_desc(int unit, uint32 chan);
extern void     soc_dma_cmicx_done_desc(int unit, uint32 chan);
extern void     soc_dma_done_chain(int unit, uint32 chan);
extern void     soc_dma_pci_timeout_handle(int unit,  uint32 data);

#ifdef  BCM_XGS3_SWITCH_SUPPORT
int soc_dma_tx_purge(int unit, dma_chan_t c);
#endif  /* BCM_XGS3_SWITCH_SUPPORT */

/*
 * Simplified API for ROM TX/RX polled mode.
 * See $SDK/doc/dma_rom.txt for more information.
 */

#ifndef SOC_DMA_ROM_TX_CHANNEL
#define SOC_DMA_ROM_TX_CHANNEL 2
#endif

#ifndef SOC_DMA_ROM_RX_CHANNEL
#define SOC_DMA_ROM_RX_CHANNEL 3
#endif

extern int      soc_dma_rom_init(int unit, int max_packet_rx, int tx, int rx); 
extern int      soc_dma_rom_detach(int unit);
extern dcb_t    *soc_dma_rom_dcb_alloc(int unit, int psize); 
extern int      soc_dma_rom_dcb_free(int unit, dcb_t *dcb); 
extern int      soc_dma_rom_tx_start(int unit, dcb_t *dcb); 
extern int      soc_dma_rom_tx_poll(int unit, int *done); 
extern int      soc_dma_rom_tx_abort(int unit); 
extern int      soc_dma_rom_rx_poll(int unit, dcb_t **dcb); 

extern void     soc_cmicx_pktdma_desc_endian_set(int unit, int cmc, int channel, int big_endian);
extern void     soc_cmicx_pktdma_hdr_endian_set(int unit, int cmc, int channel, int big_endian);
extern void     soc_cmicx_pktdma_data_endian_set(int unit, int cmc, int channel, int big_endian);
extern uint32   soc_cmicx_pktdma_header_size_get(int unit);
extern uint32   soc_cmicx_pktdma_cos_ctrl_rx0_get(int unit, int cmc, int channel);
extern uint32   soc_cmicx_pktdma_cos_ctrl_rx1_get(int unit, int cmc, int channel);
extern uint32   soc_cmicx_pktdma_cos_ctrl_rx0_reg_addr_get(int unit, int cmc, int channel);
extern uint32   soc_cmicx_pktdma_cos_ctrl_rx1_reg_addr_get(int unit, int cmc, int channel);
extern uint32   soc_cmicx_pktdma_cos_ctrl_queue_id_get(int unit, int cmc, int channel, int queue);
extern sal_vaddr_t soc_cmicx_pktdma_desc_addr_get(int unit, int cmc, int channel);

/*
 * Atomic memory allocation functions that are safe to call
 * from interrupt context when BCM_TX_FROM_INTR is defined.
 * This memory does not necessarily need to be DMA-safe.
 *
 * Currently the assumption is that soc_cm_salloc/sfree are
 * atomic if BCM_TX_FROM_INTR is defined.
 */
#ifdef BCM_TX_FROM_INTR
#define SOC_DMA_NEEDS_ATOMIC_ALLOC
#endif

#if defined(SAL_HAS_ATOMIC_ALLOC)
/* Dedicated functions provided by SAL */
extern void *soc_at_alloc(int dev, int size, const char *name);
extern void soc_at_free(int dev, void *ptr);
#elif defined(SOC_DMA_NEEDS_ATOMIC_ALLOC) && !defined(SAL_ALLOC_IS_ATOMIC)
/* Use CM DMA allocator for atomic allocation */
#define soc_at_alloc(_u, _sz, _nm) soc_cm_salloc(_u, _sz, _nm)
#define soc_at_free(_u, _ptr) soc_cm_sfree(_u, _ptr)
#else
/* */
#define soc_at_alloc(_u, _sz, _nm) sal_alloc(_sz, _nm)
#define soc_at_free(_u, _ptr) sal_free(_ptr)
#endif

/* microsec */
#ifdef BCM_ICS
/* Same time on Quickturn as well */
#define DMA_ABORT_TIMEOUT 10000
#else
#define DMA_ABORT_TIMEOUT \
    (SAL_BOOT_SIMULATION ? 10000000 : 10000)
#endif

#endif  /* !_SOC_DMA_H */
