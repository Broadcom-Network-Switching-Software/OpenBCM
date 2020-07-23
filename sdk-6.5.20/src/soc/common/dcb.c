/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        dcb.c
 * Purpose:     DCB manipulation routines
 *              Provide a uniform means of manipulation of DMA control blocks
 *              that is independent of the actual DCB format used in any
 *              particular chip.
 */

#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/dcbformats.h>
#include <soc/higig.h>
#include <soc/dma.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/rx.h>
#include <shared/bsl.h>


#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)

#define GETHDRFUNCEXPR(_dt, _name, _expr)                                        \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(void *hdr) {            \
                ep_to_cpu_hdr##_dt##_t *h = (ep_to_cpu_hdr##_dt##_t *)hdr;       \
                return _expr;                                                    \
        }
#define GETHDRFUNCFIELD(_dt, _name, _field)                                      \
        GETHDRFUNCEXPR(_dt, _name, h->_field)
#define GETHDRFUNCERR(_dt, _name)                                                \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(void *hdr) {            \
                COMPILER_REFERENCE(hdr);                                         \
                ep_to_cpu_hdr_funcerr(_dt, #_name "_get");                       \
                return 0;                                                        \
        }
#define GETHDRFUNCNULL(_dt, _name)                                               \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(void *hdr) {            \
                COMPILER_REFERENCE(hdr);                                         \
                return 0;                                                        \
        }
#define GETHDRPTREXPR(_dt, _name, _expr)                                         \
        static uint32 * ep_to_cpu_hdr##_dt##_##_name##_get(void *hdr) {          \
                ep_to_cpu_hdr##_dt##_t *h = (ep_to_cpu_hdr##_dt##_t *)hdr;       \
                return _expr;                                                    \
        }
#define GETHDRPTRERR(_dt, _name)                                                 \
        static uint32 * ep_to_cpu_hdr##_dt##_##_name##_get(void *hdr) {          \
                COMPILER_REFERENCE(hdr);                                         \
                ep_to_cpu_hdr_funcerr(_dt, #_name "_get");                       \
                return NULL;                                                     \
        }
#define GETHDRFUNCUNITEXPR(_dt, _name, _expr)                                    \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(int unit, void *hdr) {  \
                ep_to_cpu_hdr##_dt##_t *h = (ep_to_cpu_hdr##_dt##_t *)hdr;       \
                COMPILER_REFERENCE(unit);                                        \
                return _expr;                                                    \
        }
#define GETHDRFUNCUNITFIELD(_dt, _name, _field)                                  \
        GETHDRFUNCUNITEXPR(_dt, _name, h->_field)
#define GETHDRFUNCUNITERR(_dt, _name)                                            \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(int unit, void *hdr) {  \
                COMPILER_REFERENCE(unit);                                        \
                COMPILER_REFERENCE(hdr);                                         \
                ep_to_cpu_hdr_funcerr(_dt, #_name "_get");                       \
                return 0;                                                        \
        }
#define SETHDRFUNCEXPR(_dt, _name, _arg, _expr)                                  \
        static void ep_to_cpu_hdr##_dt##_##_name##_set(void *hdr, _arg) {        \
                ep_to_cpu_hdr##_dt##_t *h = (ep_to_cpu_hdr##_dt##_t *)hdr;       \
                _expr;                                                           \
        }
#define SETHDRFUNCFIELD(_dt, _name, _field, _arg, _expr)                         \
        SETHDRFUNCEXPR(_dt, _name, _arg, h->_field = _expr)
#define SETHDRFUNCERR(_dt, _name, _type)                                         \
        static void ep_to_cpu_hdr##_dt##_##_name##_set(void *hdr, _type val) {   \
                COMPILER_REFERENCE(hdr);                                         \
                COMPILER_REFERENCE(val);                                         \
                ep_to_cpu_hdr_funcerr(_dt, #_name "_set");                       \
        }
#define SETHDRFUNCNULL(_dt, _name)                                                \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_set(void *hdr, uint32 val) { \
                COMPILER_REFERENCE(hdr);                                          \
                return 0;                                                         \
        }
#define SETHDRFUNCEXPRIGNORE(_dt, _name, _arg, _expr)                               \
        SETHDRFUNCEXPR(_dt, _name, _arg, COMPILER_REFERENCE(h))
#if defined(LE_HOST)
#define GETHDRHGFUNCEXPR(_dt, _name, _expr)                                      \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(void *hdr) {            \
                ep_to_cpu_hdr##_dt##_t *hd = (ep_to_cpu_hdr##_dt##_t *)hdr;      \
                uint32  hgh[3];                                                  \
                soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];                 \
                hgh[0] = soc_htonl(hd->mh0);                                     \
                hgh[1] = soc_htonl(hd->mh1);                                     \
                hgh[2] = soc_htonl(hd->mh2);                                     \
                return _expr;                                                    \
        }
#else
#define GETHDRHGFUNCEXPR(_dt, _name, _expr)                                      \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(void *hdr) {            \
                ep_to_cpu_hdr##_dt##_t *hd = (ep_to_cpu_hdr##_dt##_t *)hdr;      \
                soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hd->mh0;                \
                return _expr;                                                    \
        }
#endif
#define GETHDRHGFUNCFIELD(_dt, _name, _field)                                    \
        GETHDRHGFUNCEXPR(_dt, _name, h->hgp_overlay1._field)

#if defined(LE_HOST)
#define GETHDRHG2FUNCEXPR(_dt, _name, _expr)                                     \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(void *hdr) {            \
                ep_to_cpu_hdr##_dt##_t *hd = (ep_to_cpu_hdr##_dt##_t *)hdr;      \
                uint32  hgh[4];                                                  \
                soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&hgh[0];               \
                hgh[0] = soc_htonl(hd->mh0);                                     \
                hgh[1] = soc_htonl(hd->mh1);                                     \
                hgh[2] = soc_htonl(hd->mh2);                                     \
                hgh[3] = soc_htonl(hd->mh3);                                     \
                return _expr;                                                    \
        }
#else
#define GETHDRHG2FUNCEXPR(_dt, _name, _expr)                                     \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(void *hdr) {            \
                ep_to_cpu_hdr##_dt##_t *hd = (ep_to_cpu_hdr##_dt##_t *)hdr;      \
                soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&hd->mh0;              \
                return _expr;                                                    \
        }
#endif
#define GETHDRHG2FUNCFIELD(_dt, _name, _field)                                   \
        GETHDRHG2FUNCEXPR(_dt, _name, h->ppd_overlay1._field)

#if defined(LE_HOST)
#define GETHDRHGFUNCUNITEXPR(_dt, _name, _expr)                                  \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(int unit, void *hdr) {  \
                ep_to_cpu_hdr##_dt##_t *hd = (ep_to_cpu_hdr##_dt##_t *)hdr;      \
                uint32  hgh[3];                                                  \
                soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];                 \
                COMPILER_REFERENCE(unit);                                        \
                hgh[0] = soc_htonl(hd->mh0);                                     \
                hgh[1] = soc_htonl(hd->mh1);                                     \
                hgh[2] = soc_htonl(hd->mh2);                                     \
                return _expr;                                                    \
        }
#else
#define GETHDRHGFUNCUNITEXPR(_dt, _name, _expr)                                  \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(int unit, void *hdr) {  \
                ep_to_cpu_hdr##_dt##_t *hd = (ep_to_cpu_hdr##_dt##_t *)dcb;      \
                soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hd->mh0;                \
                COMPILER_REFERENCE(unit);                                        \
                return _expr;                                                    \
        }
#endif

#if defined(LE_HOST)
#define GETHDRHG2FUNCUNITEXPR(_dt, _name, _expr)                                 \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(int unit, void *hdr) {  \
                ep_to_cpu_hdr##_dt##_t *hd = (ep_to_cpu_hdr##_dt##_t *)hdr;      \
                uint32  hgh[4];                                                  \
                soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&hgh[0];               \
                COMPILER_REFERENCE(unit);                                        \
                hgh[0] = soc_htonl(hd->mh0);                                     \
                hgh[1] = soc_htonl(hd->mh1);                                     \
                hgh[2] = soc_htonl(hd->mh2);                                     \
                hgh[3] = soc_htonl(hd->mh3);                                     \
                return _expr;                                                    \
        }
#else
#define GETHDRHG2FUNCUNITEXPR(_dt, _name, _expr)                                 \
        static uint32 ep_to_cpu_hdr##_dt##_##_name##_get(int unit, void *hdr) {  \
                ep_to_cpu_hdr##_dt##_t *hd = (ep_to_cpu_hdr##_dt##_t *)hdr;      \
                soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&hd->mh0;              \
                COMPILER_REFERENCE(unit);                                        \
                return _expr;                                                    \
        }
#endif

/*
 * Short cuts for generating dcb support functions.
 * Most support functions are just setting or getting a field
 * in the appropriate dcb structure or doing a simple expression
 * based on a couple of fields.
 *      GETFUNCFIELD - get a field from DCB
 *      SETFUNCFIELD - set a field in DCB
 *      SETFUNCERR - dummy handler for field that does not exist for
 *                      a descriptor type
 *      GETFUNCERR - dummy handler for field that does not exist for
 *                      a descriptor type
 */
#define GETFUNCEXPR(_dt, _name, _expr)                                  \
        static uint32 dcb##_dt##_##_name##_get(dcb_t *dcb) {            \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                return _expr;                                           \
        }
#define GETFUNCFIELD(_dt, _name, _field)                                \
        GETFUNCEXPR(_dt, _name, d->_field)
#define GETFUNCERR(_dt, _name)                                          \
        static uint32 dcb##_dt##_##_name##_get(dcb_t *dcb) {            \
                COMPILER_REFERENCE(dcb);                                \
                dcb0_funcerr(_dt, #_name "_get");                       \
                return 0;                                               \
        }
#define GETFUNCNULL(_dt, _name)                                         \
        static uint32 dcb##_dt##_##_name##_get(dcb_t *dcb) {            \
                COMPILER_REFERENCE(dcb);                                \
                return 0;                                               \
        }
#define GETPTREXPR(_dt, _name, _expr)                                   \
        static uint32 * dcb##_dt##_##_name##_get(dcb_t *dcb) {          \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                return _expr;                                           \
        }
#define GETPTRERR(_dt, _name)                                           \
        static uint32 * dcb##_dt##_##_name##_get(dcb_t *dcb) {          \
                COMPILER_REFERENCE(dcb);                                \
                dcb0_funcerr(_dt, #_name "_get");                       \
                return NULL;                                            \
        }
#define GETFUNCUNITEXPR(_dt, _name, _expr)                              \
        static uint32 dcb##_dt##_##_name##_get(int unit, dcb_t *dcb) {  \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                COMPILER_REFERENCE(unit);                               \
                return _expr;                                           \
        }
#define GETFUNCUNITFIELD(_dt, _name, _field)                            \
        GETFUNCUNITEXPR(_dt, _name, d->_field)
#define GETFUNCUNITERR(_dt, _name)                                      \
        static uint32 dcb##_dt##_##_name##_get(int unit, dcb_t *dcb) {  \
                COMPILER_REFERENCE(unit);                               \
                COMPILER_REFERENCE(dcb);                                \
                dcb0_funcerr(_dt, #_name "_get");                       \
                return 0;                                               \
        }
#define SETFUNCEXPR(_dt, _name, _arg, _expr)                            \
        static void dcb##_dt##_##_name##_set(dcb_t *dcb, _arg) {        \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                _expr;                                                  \
        }
#define SETFUNCFIELD(_dt, _name, _field, _arg, _expr)                   \
        SETFUNCEXPR(_dt, _name, _arg, d->_field = _expr)
#define SETFUNCERR(_dt, _name, _type)                                   \
        static void dcb##_dt##_##_name##_set(dcb_t *dcb, _type val) {   \
                COMPILER_REFERENCE(dcb);                                \
                COMPILER_REFERENCE(val);                                \
                dcb0_funcerr(_dt, #_name "_set");                       \
        }
#define SETFUNCNULL(_dt, _name)                                         \
        static uint32 dcb##_dt##_##_name##_set(dcb_t *dcb,uint32 val) { \
                COMPILER_REFERENCE(dcb);                                \
                return 0;                                               \
        }

#define SETFUNCEXPRIGNORE(_dt, _name, _arg, _expr)                      \
        SETFUNCEXPR(_dt, _name, _arg, COMPILER_REFERENCE(d))
#if defined(LE_HOST)
#define GETHGFUNCEXPR(_dt, _name, _expr)                                \
        static uint32 dcb##_dt##_##_name##_get(dcb_t *dcb) {            \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                uint32  hgh[3];                                         \
                soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];        \
                hgh[0] = soc_htonl(d->mh0);                             \
                hgh[1] = soc_htonl(d->mh1);                             \
                hgh[2] = soc_htonl(d->mh2);                             \
                return _expr;                                           \
        }
#else
#define GETHGFUNCEXPR(_dt, _name, _expr)                                \
        static uint32 dcb##_dt##_##_name##_get(dcb_t *dcb) {            \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                soc_higig_hdr_t *h = (soc_higig_hdr_t *)&d->mh0;        \
                return _expr;                                           \
        }
#endif
#define GETHGFUNCFIELD(_dt, _name, _field)                              \
        GETHGFUNCEXPR(_dt, _name, h->hgp_overlay1._field)

#if defined(LE_HOST)
#define GETHG2FUNCEXPR(_dt, _name, _expr)                               \
        static uint32 dcb##_dt##_##_name##_get(dcb_t *dcb) {            \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                uint32  hgh[4];                                         \
                soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&hgh[0];      \
                hgh[0] = soc_htonl(d->mh0);                             \
                hgh[1] = soc_htonl(d->mh1);                             \
                hgh[2] = soc_htonl(d->mh2);                             \
                hgh[3] = soc_htonl(d->mh3);                             \
                return _expr;                                           \
        }
#else
#define GETHG2FUNCEXPR(_dt, _name, _expr)                               \
        static uint32 dcb##_dt##_##_name##_get(dcb_t *dcb) {            \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&d->mh0;      \
                return _expr;                                           \
        }
#endif
#define GETHG2FUNCFIELD(_dt, _name, _field)                             \
        GETHG2FUNCEXPR(_dt, _name, h->ppd_overlay1._field)

#if defined(LE_HOST)
#define GETHGFUNCUNITEXPR(_dt, _name, _expr)                           \
        static uint32 dcb##_dt##_##_name##_get(int unit, dcb_t *dcb) {  \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                uint32  hgh[3];                                         \
                soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];      \
                COMPILER_REFERENCE(unit);                               \
                hgh[0] = soc_htonl(d->mh0);                             \
                hgh[1] = soc_htonl(d->mh1);                             \
                hgh[2] = soc_htonl(d->mh2);                             \
                return _expr;                                           \
        }
#else
#define GETHGFUNCUNITEXPR(_dt, _name, _expr)                           \
        static uint32 dcb##_dt##_##_name##_get(int unit, dcb_t *dcb) {  \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                soc_higig_hdr_t *h = (soc_higig_hdr_t *)&d->mh0;      \
                COMPILER_REFERENCE(unit);                               \
                return _expr;                                           \
        }
#endif

#if defined(LE_HOST)
#define GETHG2FUNCUNITEXPR(_dt, _name, _expr)                           \
        static uint32 dcb##_dt##_##_name##_get(int unit, dcb_t *dcb) {  \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                uint32  hgh[4];                                         \
                soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&hgh[0];      \
                COMPILER_REFERENCE(unit);                               \
                hgh[0] = soc_htonl(d->mh0);                             \
                hgh[1] = soc_htonl(d->mh1);                             \
                hgh[2] = soc_htonl(d->mh2);                             \
                hgh[3] = soc_htonl(d->mh3);                             \
                return _expr;                                           \
        }
#else
#define GETHG2FUNCUNITEXPR(_dt, _name, _expr)                           \
        static uint32 dcb##_dt##_##_name##_get(int unit, dcb_t *dcb) {  \
                dcb##_dt##_t *d = (dcb##_dt##_t *)dcb;                  \
                soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&d->mh0;      \
                COMPILER_REFERENCE(unit);                               \
                return _expr;                                           \
        }
#endif

/*
 * This is a standard function used to generate a debug message whenever 
 * the code tries to access a field not present in the specific DCB
 */
static void
dcb0_funcerr(int dt, char *name)
{
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("ERROR: dcb%d_%s called\n"), dt, name));
}
/* the addr related functions are the same for all dcb types */
void
dcb0_addr_set(int unit, dcb_t *dcb, sal_vaddr_t addr)
{
    uint32      *d = (uint32 *)dcb;

    if (addr == 0) {
        *d = 0;
    } else {
        *d = soc_cm_l2p(unit, (void *)addr);
    }
}

sal_vaddr_t
dcb0_addr_get(int unit, dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    if (*d == 0) {
        return (sal_vaddr_t)0;
    } else {
        return (sal_vaddr_t)soc_cm_p2l(unit, *d);
    }
}

sal_paddr_t
dcb0_paddr_get(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    return (sal_paddr_t)*d;
}

#if defined(BCM_HERCULES_SUPPORT)
SETFUNCERR(0, hg, uint32)
GETFUNCERR(0, hg)
SETFUNCERR(0, stat, uint32)
GETFUNCERR(0, stat)
SETFUNCERR(0, purge, uint32)
GETFUNCERR(0, purge)
GETPTRERR(0, mhp)
GETFUNCERR(0, outer_vid)
GETFUNCERR(0, outer_pri)
GETFUNCERR(0, outer_cfi)
GETFUNCERR(0, rx_outer_tag_action)
GETFUNCERR(0, inner_vid)
GETFUNCERR(0, inner_pri)
GETFUNCERR(0, inner_cfi)
GETFUNCERR(0, rx_inner_tag_action)
GETFUNCNULL(0, rx_bpdu)
GETFUNCNULL(0, rx_l3_intf)
GETFUNCNULL(0, rx_decap_tunnel)
#endif


/*
 * Function:
 *      dcb0_rx_reason_map_get
 * Purpose:
 *      Return the RX reason map for a series of DCB types.
 * Parameters:
 *      dcb_op - DCB operations
 *      dcb    - dma control block
 * Returns:
 *      RX reason map pointer
 * Notes:
 *      Function made global to resolve compiler link issue.
 */
static soc_rx_reason_t *
dcb0_rx_reason_map_get(dcb_op_t *dcb_op, dcb_t *dcb)
{
    COMPILER_REFERENCE(dcb);

    return dcb_op->rx_reason_maps[0];
}

/*
 * Function:
 *      dcb0_rx_reasons_get
 * Purpose:
 *      Map the hardware reason bits from 'dcb' into the set
 *      of "reasons".
 * Parameters:
 *      dcb_op  - DCB operations
 *      dcb     - dma control block
 *      reasons - set of "reasons", socRxReason*
 */
void
dcb0_rx_reasons_get(dcb_op_t *dcb_op, dcb_t *dcb, soc_rx_reasons_t *reasons)
{
    soc_rx_reason_t *map;
    uint32 reason;
    uint32 mask;
    int i;    

    SOC_RX_REASON_CLEAR_ALL(*reasons);

    map = dcb_op->rx_reason_map_get(dcb_op, dcb);
    if (map == NULL) {
        return;
    }

    reason = dcb_op->rx_reason_get(dcb);
    mask = 1;
    for (i = 0; i < 32; i++) {
        if ((mask & reason)) {
            SOC_RX_REASON_SET(*reasons, map[i]);
        }
        mask <<= 1;
    }

    reason = dcb_op->rx_reason_hi_get(dcb);
    mask = 1;
    for (i = 0; i < 32; i++) {
        if ((mask & reason)) {
            SOC_RX_REASON_SET(*reasons, map[i + 32]);
        }
        mask <<= 1;
    }

    /* BPDU bit should be a reason, paste it in here */
    if (dcb_op->rx_bpdu_get(dcb)) {
        SOC_RX_REASON_SET(*reasons, socRxReasonBpdu);
    }

    return;
}

#ifdef  BROADCOM_DEBUG

static char *_dcb_reason_names[socRxReasonCount] = SOC_RX_REASON_NAMES_INITIALIZER;

static void
dcb0_reason_dump(int unit, dcb_t *dcb, char *prefix)
{
    soc_rx_reason_t *map;
    uint32 reason = SOC_DCB_RX_REASON_GET(unit, dcb);
    uint32 mask = 1;
    int i;
    
    map = SOC_DCB(unit)->rx_reason_map_get(SOC_DCB(unit), dcb);
    if (map == NULL) {
        return;
    }

    for (i=0; i < 32; i++) {
        if ((mask & reason)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s\treason bit %d: %s\n"), prefix, i, 
                     _dcb_reason_names[map[i]]));
        }
        mask <<= 1;
    }
    
    reason = SOC_DCB_RX_REASON_HI_GET(unit, dcb);
    mask = 1;

    for (i=0; i < 32; i++) {
        if ((mask & reason)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s\treason bit %d: %s\n"), prefix, i + 32,
                     _dcb_reason_names[map[i + 32]]));
        }
        mask <<= 1;
    }
}


#if defined(BCM_HERCULES_SUPPORT)
static void
dcb0_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %sdone %ssg %schain %sreload\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!"));
    dcb0_reason_dump(unit, dcb, prefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));

    if (tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\tl2pbm %x utbpm %x l3bpm %x %scrc cos %d\n"),
                 prefix,
                 SOC_DCB_TX_L2PBM_GET(unit, dcb),
                 SOC_DCB_TX_UTPBM_GET(unit, dcb),
                 SOC_DCB_TX_L3PBM_GET(unit, dcb),
                 SOC_DCB_TX_CRC_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_TX_COS_GET(unit, dcb)));

        if (SOC_IS_XGS(unit)) {
            if (SOC_IS_XGS_SWITCH(unit)) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s\tdest-modport %d.%d opcode %d\n"),
                         prefix,
                         SOC_DCB_TX_DESTMOD_GET(unit, dcb),
                         SOC_DCB_TX_DESTPORT_GET(unit, dcb),
                         SOC_DCB_TX_OPCODE_GET(unit, dcb)));
            }
        }
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sstart %send %serror %scrc cos %d ingport %d reason %#x\n"),
                 prefix,
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_CRC_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_COS_GET(unit, dcb),
                 SOC_DCB_RX_INGPORT_GET(unit, dcb),
                 SOC_DCB_RX_REASON_GET(unit, dcb)));
        if (SOC_IS_XGS(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s\tsrc-modport %d.%d dest-modport %d.%d\n"),
                     prefix,
                     SOC_DCB_RX_SRCMOD_GET(unit, dcb),
                     SOC_DCB_RX_SRCPORT_GET(unit, dcb),
                     SOC_DCB_RX_DESTMOD_GET(unit, dcb),
                     SOC_DCB_RX_DESTPORT_GET(unit, dcb)));
            LOG_CLI((BSL_META_U(unit,
                                "%s\thg-opcode %d prio %d mcast %d\n"),
                     prefix,
                     SOC_DCB_RX_OPCODE_GET(unit, dcb),
                     SOC_DCB_RX_PRIO_GET(unit, dcb),
                     SOC_DCB_RX_MCAST_GET(unit, dcb)));
        }
    }
}
#endif
#endif  /* BROADCOM_DEBUG */

#if defined(BCM_HERCULES_SUPPORT)
/*
 * DCB Type 2 Support
 */

static soc_rx_reason_t
dcb2_rx_reason_map[] = {
    socRxReasonFilterMatch,         /* Offset 0 */
    socRxReasonCpuLearn,            /* Offset 1 */
    socRxReasonSourceRoute,         /* Offset 2 */
    socRxReasonDestLookupFail,      /* Offset 3 */
    socRxReasonControl,             /* Offset 4 */
    socRxReasonIp,                  /* Offset 5 */
    socRxReasonInvalid,             /* Offset 6 */
    socRxReasonIpOptionVersion,     /* Offset 7 */
    socRxReasonIpmc,                /* Offset 8 */
    socRxReasonTtl,                 /* Offset 9 */
    socRxReasonBroadcast,           /* Offset 10 */
    socRxReasonMulticast,           /* Offset 11 */
    socRxReasonIgmp,                /* Offset 12 */
    socRxReasonInvalid,             /* Offset 13 */
    socRxReasonInvalid,             /* Offset 14 */
    socRxReasonInvalid,             /* Offset 15 */
    socRxReasonInvalid,             /* Offset 16 */
    socRxReasonInvalid,             /* Offset 17 */
    socRxReasonInvalid,             /* Offset 18 */
    socRxReasonInvalid,             /* Offset 19 */
    socRxReasonInvalid,             /* Offset 20 */
    socRxReasonInvalid,             /* Offset 21 */
    socRxReasonInvalid,             /* Offset 22 */
    socRxReasonInvalid,             /* Offset 23 */
    socRxReasonInvalid,             /* Offset 24 */
    socRxReasonInvalid,             /* Offset 25 */
    socRxReasonInvalid,             /* Offset 26 */
    socRxReasonInvalid,             /* Offset 27 */
    socRxReasonInvalid,             /* Offset 28 */
    socRxReasonInvalid,             /* Offset 29 */
    socRxReasonInvalid,             /* Offset 30 */
    socRxReasonInvalid,             /* Offset 31 */
};

static soc_rx_reason_t *dcb2_rx_reason_maps[] = {
    dcb2_rx_reason_map,
    NULL
};

static void
dcb2_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[0] = d[1] = d[2] = d[3] = d[4] = d[5] = d[6] = d[7] = 0;
}

static int
dcb2_addtx(dv_t *dv, sal_vaddr_t addr, uint32 count,
           pbmp_t l2pbm, pbmp_t utpbm, pbmp_t l3pbm, uint32 flags, uint32 *hgh)
{
    dcb2_t      *d;
    uint32      *di;
    uint32      paddr;

    d = (dcb2_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (addr) {
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    } else {
        paddr = 0;
    }

    if (dv->dv_vcnt > 0 && (dv->dv_flags & DV_F_COMBINE_DCB) &&
        (d[-1].c_sg != 0) &&
        (d[-1].addr + d[-1].c_count) == paddr &&
        d[-1].c_count + count <= DCB_MAX_REQCOUNT) {
        d[-1].c_count += count;
        return dv->dv_cnt - dv->dv_vcnt;
    }

    if (dv->dv_vcnt >= dv->dv_cnt) {
        return SOC_E_FULL;
    }
    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = di[5] = di[6] = di[7] = 0;

    d->addr = paddr;
    d->c_count = count;
    d->c_sg = 1;
    d->c_cos = SOC_DMA_COS_GET(flags);
    d->c_crc = SOC_DMA_CRC_GET(flags) ?
                DCB_STRATA_CRC_REGEN : DCB_STRATA_CRC_LEAVE;
    d->l2ports = SOC_PBMP_WORD_GET(l2pbm, 0);
    d->utports = SOC_PBMP_WORD_GET(utpbm, 0);
    d->l3ports = SOC_PBMP_WORD_GET(l3pbm, 0);

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static int
dcb2_addrx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb2_t      *d;
    uint32      *di;

    d = (dcb2_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = di[5] = di[6] = di[7] = 0;

    if (addr) {
        d->addr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    }
    d->c_count = count;
    d->c_sg = 1;
    d->c_cos = SOC_DMA_COS_GET(flags);
    d->c_crc = SOC_DMA_CRC_GET(flags) ?
                DCB_STRATA_CRC_REGEN : DCB_STRATA_CRC_LEAVE;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static uint32
dcb2_intrinfo(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb2_t      *d = (dcb2_t *)dcb;
    uint32      f;

    if (!d->s2valid) {
        return 0;
    }
    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }
    *count = d->count;
    return f;
}

static uint32
dcb2_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb2_t  *d = (dcb2_t *)dcb;

    COMPILER_REFERENCE(dt_mode);
    COMPILER_REFERENCE(ingport_is_hg);

    return d->untagged;
}

SETFUNCFIELD(2, reqcount, c_count, uint32 count, count)
GETFUNCFIELD(2, reqcount, c_count)
GETFUNCFIELD(2, xfercount, count)
SETFUNCFIELD(2, done, s2valid, int val, val ? 1 : 0)
GETFUNCFIELD(2, done, s2valid)
SETFUNCFIELD(2, sg, c_sg, int val, val ? 1 : 0)
GETFUNCFIELD(2, sg, c_sg)
SETFUNCFIELD(2, chain, c_chain, int val, val ? 1 : 0)
GETFUNCFIELD(2, chain, c_chain)
SETFUNCFIELD(2, reload, c_reload, int val, val ? 1 : 0)
GETFUNCFIELD(2, reload, c_reload)
SETFUNCERR(2, desc_intr, int)
GETFUNCERR(2, desc_intr)
SETFUNCFIELD(2, tx_l2pbm, l2ports, pbmp_t pbm, SOC_PBMP_WORD_GET(pbm, 0))
SETFUNCFIELD(2, tx_utpbm, utports, pbmp_t pbm, SOC_PBMP_WORD_GET(pbm, 0))
SETFUNCFIELD(2, tx_l3pbm, l3ports, pbmp_t pbm, SOC_PBMP_WORD_GET(pbm, 0))
SETFUNCFIELD(2, tx_crc, c_crc, int val,
             val ? DCB_STRATA_CRC_REGEN : DCB_STRATA_CRC_LEAVE)
SETFUNCFIELD(2, tx_cos, c_cos, int val, val)
SETFUNCERR(2, tx_destmod, uint32)
SETFUNCERR(2, tx_destport, uint32)
SETFUNCERR(2, tx_opcode, uint32)
SETFUNCERR(2, tx_srcmod, uint32)
SETFUNCERR(2, tx_srcport, uint32)
SETFUNCERR(2, tx_prio, uint32)
SETFUNCERR(2, tx_pfm, uint32)
GETFUNCEXPR(2, rx_crc, d->crc == DCB_STRATA_CRC_REGEN)
GETFUNCFIELD(2, rx_cos, cos)
GETFUNCERR(2, rx_destmod)
GETFUNCERR(2, rx_destport)
GETFUNCERR(2, rx_opcode)
GETFUNCERR(2, rx_classtag)
GETFUNCFIELD(2, rx_matchrule, match_rule)
GETFUNCFIELD(2, rx_start, start)
GETFUNCFIELD(2, rx_end, end)
GETFUNCEXPR(2, rx_error, d->optim == DCB_STRATA_OPTIM_PURGE)
GETFUNCERR(2, rx_prio)
GETFUNCFIELD(2, rx_reason, reason)
GETFUNCNULL(2, rx_reason_hi)
GETFUNCFIELD(2, rx_ingport, srcport)
GETFUNCFIELD(2, rx_srcport, srcport)
GETFUNCERR(2, rx_srcmod)
GETFUNCERR(2, rx_mcast)
GETFUNCERR(2, rx_vclabel)
GETFUNCERR(2, rx_mirror)
GETFUNCERR(2, rx_timestamp)
GETFUNCERR(2, rx_timestamp_upper)
GETFUNCNULL(2, rx_switch_drop)
GETFUNCNULL(2, olp_encap_oam_pkt)
GETFUNCNULL(2, read_ecc_error)
GETFUNCNULL(2, desc_remaining)
SETFUNCNULL(2, desc_remaining)
GETFUNCNULL(2, desc_status)
SETFUNCNULL(2, desc_status)
GETFUNCNULL(2, match_id_lo)
GETFUNCNULL(2, match_id_hi)
GETFUNCNULL(2, forwarding_type)
GETFUNCNULL(2, forwarding_zone_id)

#ifdef BROADCOM_DEBUG
GETFUNCFIELD(2, tx_l2pbm, l2ports)
GETFUNCFIELD(2, tx_utpbm, utports)
GETFUNCFIELD(2, tx_l3pbm, l3ports)
GETFUNCFIELD(2, tx_crc, c_crc)
GETFUNCFIELD(2, tx_cos, c_cos)
GETFUNCERR(2, tx_destmod)
GETFUNCERR(2, tx_destport)
GETFUNCERR(2, tx_opcode)
GETFUNCERR(2, tx_srcmod)
GETFUNCERR(2, tx_srcport)
GETFUNCERR(2, tx_prio)
GETFUNCERR(2, tx_pfm)
#endif /* BROADCOM_DEBUG */

#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb2_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[5] = d[6] = d[7] = 0;
}
SETFUNCFIELD(2, xfercount, count, uint32 count, count)
SETFUNCFIELD(2, rx_start, start, int val, val ? 1 : 0)
SETFUNCFIELD(2, rx_end, end, int val, val ? 1 : 0)
SETFUNCFIELD(2, rx_error, optim, int val, val ? DCB_STRATA_OPTIM_PURGE : 0)
SETFUNCFIELD(2, rx_crc, crc, int val, val ? DCB_STRATA_CRC_REGEN : 0)
#endif  /* PLISIM */

dcb_op_t dcb2_op = {
    2,
    sizeof(dcb2_t),
    dcb2_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb2_init,
    dcb2_addtx,
    dcb2_addrx,
    dcb2_intrinfo,
    dcb2_reqcount_set,
    dcb2_reqcount_get,
    dcb2_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb2_done_set,
    dcb2_done_get,
    dcb2_sg_set,
    dcb2_sg_get,
    dcb2_chain_set,
    dcb2_chain_get,
    dcb2_reload_set,
    dcb2_reload_get,
    dcb2_desc_intr_set,
    dcb2_desc_intr_get,
    dcb2_tx_l2pbm_set,
    dcb2_tx_utpbm_set,
    dcb2_tx_l3pbm_set,
    dcb2_tx_crc_set,
    dcb2_tx_cos_set,
    dcb2_tx_destmod_set,
    dcb2_tx_destport_set,
    dcb2_tx_opcode_set,
    dcb2_tx_srcmod_set,
    dcb2_tx_srcport_set,
    dcb2_tx_prio_set,
    dcb2_tx_pfm_set,
    dcb2_rx_untagged_get,
    dcb2_rx_crc_get,
    dcb2_rx_cos_get,
    dcb2_rx_destmod_get,
    dcb2_rx_destport_get,
    dcb2_rx_opcode_get,
    dcb2_rx_classtag_get,
    dcb2_rx_matchrule_get,
    dcb2_rx_start_get,
    dcb2_rx_end_get,
    dcb2_rx_error_get,
    dcb2_rx_prio_get,
    dcb2_rx_reason_get,
    dcb2_rx_reason_hi_get,
    dcb2_rx_ingport_get,
    dcb2_rx_srcport_get,
    dcb2_rx_srcmod_get,
    dcb2_rx_mcast_get,
    dcb2_rx_vclabel_get,
    dcb2_rx_mirror_get,
    dcb2_rx_timestamp_get,
    dcb2_rx_timestamp_upper_get,
    dcb0_hg_set,
    dcb0_hg_get,
    dcb0_stat_set,
    dcb0_stat_get,
    dcb0_purge_set,
    dcb0_purge_get,
    dcb0_mhp_get,
    dcb0_outer_vid_get,
    dcb0_outer_pri_get,
    dcb0_outer_cfi_get,
    dcb0_rx_outer_tag_action_get,
    dcb0_inner_vid_get,
    dcb0_inner_pri_get,
    dcb0_inner_cfi_get,
    dcb0_rx_inner_tag_action_get,
    dcb0_rx_bpdu_get,
    dcb0_rx_l3_intf_get,
    dcb0_rx_decap_tunnel_get,
    dcb2_rx_switch_drop_get,
    dcb2_olp_encap_oam_pkt_get,
    dcb2_read_ecc_error_get,
    dcb2_desc_remaining_get,
    dcb2_desc_remaining_set,
    dcb2_desc_status_get,
    dcb2_desc_status_set,
    dcb2_match_id_lo_get,
    dcb2_match_id_hi_get,
    dcb2_forwarding_type_get,
    dcb2_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb2_tx_l2pbm_get,
    dcb2_tx_utpbm_get,
    dcb2_tx_l3pbm_get,
    dcb2_tx_crc_get,
    dcb2_tx_cos_get,
    dcb2_tx_destmod_get,
    dcb2_tx_destport_get,
    dcb2_tx_opcode_get,
    dcb2_tx_srcmod_get,
    dcb2_tx_srcport_get,
    dcb2_tx_prio_get,
    dcb2_tx_pfm_get,

    dcb0_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb2_status_init,
    dcb2_xfercount_set,
    dcb2_rx_start_set,
    dcb2_rx_end_set,
    dcb2_rx_error_set,
    dcb2_rx_crc_set,
    NULL,
#endif
    NULL,
};

#endif  /* BCM_HERCULES_SUPPORT */

#if defined(BCM_HERCULES_SUPPORT)
static soc_rx_reason_t
dcb3_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
#if 1
    socRxReasonInvalid,            /* Offset 5 */
    socRxReasonInvalid,            /* Offset 6 */
#endif
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
};

static soc_rx_reason_t *dcb3_rx_reason_maps[] = {
    dcb3_rx_reason_map,
    NULL
};
#endif

#ifdef  BCM_HERCULES_SUPPORT
/*
 * DCB Type 4 Support
 * The trick here is that the tx related functions use the dcb2
 * variations because hercules has a split personality.  Type 2 to
 * transmit but type 4 to receive.  We take advantage of the fact both
 * types are the same length.
 */

static int
dcb4_addrx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb4_t      *d;
    uint32      *di;

    d = (dcb4_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = di[5] = di[6] = di[7] = 0;

    if (addr) {
        d->addr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    }
    d->c_count = count;
    d->c_sg = 1;
    d->c_cos = SOC_DMA_COS_GET(flags);
    d->c_crc = SOC_DMA_CRC_GET(flags) ? DCB_XGS_CRC_REGEN : DCB_XGS_CRC_LEAVE;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static uint32
dcb4_intrinfo(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb4_t      *d = (dcb4_t *)dcb;
    uint32      f;

    if (!d->s2valid) {
        return 0;
    }
    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }
    *count = d->count;
    return f;
}

static uint32
dcb4_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    COMPILER_REFERENCE(dcb);
    COMPILER_REFERENCE(dt_mode);
    COMPILER_REFERENCE(ingport_is_hg);

    dcb0_funcerr(4, "rx_untagged_get");

    return 0;
}

GETFUNCFIELD(4, rx_crc, crc)
GETFUNCFIELD(4, rx_cos, cos)
GETFUNCEXPR(4, rx_destmod, d->destmod_lo)
GETFUNCFIELD(4, rx_destport, destport)
GETFUNCFIELD(4, rx_opcode, opcode)
GETFUNCFIELD(4, rx_classtag, class_tag)
GETFUNCFIELD(4, rx_matchrule, match_rule)
GETFUNCFIELD(4, rx_start, start)
GETFUNCFIELD(4, rx_end, end)
GETFUNCFIELD(4, rx_error, error)
GETFUNCEXPR(4, rx_prio, ((d->prio_hi<<1) | d->prio_lo))
GETFUNCFIELD(4, rx_reason, reason)
GETFUNCNULL(4, rx_reason_hi)
GETFUNCFIELD(4, rx_ingport, ingport)
GETFUNCERR(4, rx_srcport)
GETFUNCFIELD(4, rx_srcmod, srcmod)
GETFUNCEXPR(4, rx_mcast, ((d->destmod_lo<<5) | d->destport))
GETFUNCERR(4, rx_vclabel)
GETFUNCERR(4, rx_mirror)
GETFUNCERR(4, rx_timestamp)
GETFUNCERR(4, rx_timestamp_upper)
GETFUNCNULL(4, rx_switch_drop)
GETFUNCNULL(4, olp_encap_oam_pkt)
GETFUNCNULL(4, read_ecc_error)
GETFUNCNULL(4, desc_remaining)
SETFUNCNULL(4, desc_remaining)
GETFUNCNULL(4, desc_status)
SETFUNCNULL(4, desc_status)
GETFUNCNULL(4, match_id_lo)
GETFUNCNULL(4, match_id_hi)
GETFUNCNULL(4, forwarding_type)
GETFUNCNULL(4, forwarding_zone_id)

#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb4_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[5] = d[6] = d[7] = 0;
}
SETFUNCFIELD(4, rx_start, start, int val, val ? 1 : 0)
SETFUNCFIELD(4, rx_end, end, int val, val ? 1 : 0)
SETFUNCFIELD(4, rx_error, error, int val, val ? 1 : 0)
SETFUNCFIELD(4, rx_crc, crc, int val, val ? 1 : 0)
#endif  /* PLISIM */

dcb_op_t dcb4_op = {
    4,
    sizeof(dcb4_t),
    dcb3_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb2_init,
    dcb2_addtx,
    dcb4_addrx,
    dcb4_intrinfo,
    dcb2_reqcount_set,          /* 2 and 4 are the same */
    dcb2_reqcount_get,          /* 2 and 4 are the same */
    dcb2_xfercount_get,         /* 2 and 4 are the same */
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb2_done_set,              /* 2 and 4 are the same */
    dcb2_done_get,              /* 2 and 4 are the same */
    dcb2_sg_set,                /* 2 and 4 are the same */
    dcb2_sg_get,                /* 2 and 4 are the same */
    dcb2_chain_set,             /* 2 and 4 are the same */
    dcb2_chain_get,             /* 2 and 4 are the same */
    dcb2_reload_set,            /* 2 and 4 are the same */
    dcb2_reload_get,            /* 2 and 4 are the same */
    dcb2_desc_intr_set,
    dcb2_desc_intr_get,
    dcb2_tx_l2pbm_set,
    dcb2_tx_utpbm_set,
    dcb2_tx_l3pbm_set,
    dcb2_tx_crc_set,
    dcb2_tx_cos_set,
    dcb2_tx_destmod_set,
    dcb2_tx_destport_set,
    dcb2_tx_opcode_set,
    dcb2_tx_srcmod_set,
    dcb2_tx_srcport_set,
    dcb2_tx_prio_set,
    dcb2_tx_pfm_set,
    dcb4_rx_untagged_get,
    dcb4_rx_crc_get,
    dcb4_rx_cos_get,
    dcb4_rx_destmod_get,
    dcb4_rx_destport_get,
    dcb4_rx_opcode_get,
    dcb4_rx_classtag_get,
    dcb4_rx_matchrule_get,
    dcb4_rx_start_get,
    dcb4_rx_end_get,
    dcb4_rx_error_get,
    dcb4_rx_prio_get,
    dcb4_rx_reason_get,
    dcb4_rx_reason_hi_get,
    dcb4_rx_ingport_get,
    dcb4_rx_srcport_get,
    dcb4_rx_srcmod_get,
    dcb4_rx_mcast_get,
    dcb4_rx_vclabel_get,
    dcb4_rx_mirror_get,
    dcb4_rx_timestamp_get,
    dcb4_rx_timestamp_upper_get,
    dcb0_hg_set,
    dcb0_hg_get,
    dcb0_stat_set,
    dcb0_stat_get,
    dcb0_purge_set,
    dcb0_purge_get,
    dcb0_mhp_get,
    dcb0_outer_vid_get,
    dcb0_outer_pri_get,
    dcb0_outer_cfi_get,
    dcb0_rx_outer_tag_action_get,
    dcb0_inner_vid_get,
    dcb0_inner_pri_get,
    dcb0_inner_cfi_get,
    dcb0_rx_inner_tag_action_get,
    dcb0_rx_bpdu_get,
    dcb0_rx_l3_intf_get,
    dcb0_rx_decap_tunnel_get,
    dcb4_rx_switch_drop_get,
    dcb4_olp_encap_oam_pkt_get,
    dcb4_read_ecc_error_get,
    dcb4_desc_remaining_get,
    dcb4_desc_remaining_set,
    dcb4_desc_status_get,
    dcb4_desc_status_set,
    dcb4_match_id_lo_get,
    dcb4_match_id_hi_get,
    dcb4_forwarding_type_get,
    dcb4_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb2_tx_l2pbm_get,
    dcb2_tx_utpbm_get,
    dcb2_tx_l3pbm_get,
    dcb2_tx_crc_get,
    dcb2_tx_cos_get,
    dcb2_tx_destmod_get,
    dcb2_tx_destport_get,
    dcb2_tx_opcode_get,
    dcb2_tx_srcmod_get,
    dcb2_tx_srcport_get,
    dcb2_tx_prio_get,
    dcb2_tx_pfm_get,

    dcb0_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb4_status_init,
    dcb2_xfercount_set,         /* 2 and 4 are the same */
    dcb4_rx_start_set,
    dcb4_rx_end_set,
    dcb4_rx_error_set,
    dcb4_rx_crc_set,
    NULL,
#endif
    NULL,
};

#endif  /* BCM_HERCULES_SUPPORT */


#if defined(BCM_XGS3_SWITCH_SUPPORT)

#if defined(BCM_XGS3_SWITCH_SUPPORT) 
/*
 * DCB Type 9 Support
 */

#ifdef BCM_FIREBOLT_SUPPORT
static soc_rx_reason_t
dcb9_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid             /* Offset 31 */
};

static soc_rx_reason_t *dcb9_rx_reason_maps[] = {
    dcb9_rx_reason_map,
    NULL
};
#endif /* BCM_FIREBOLT_SUPPORT */

static void
dcb9_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[0] = d[1] = d[2] = d[3] = d[4] = 0;
    d[5] = d[6] = d[7] = d[8] = d[9] = d[10] = 0;
}

static int
dcb9_addtx(dv_t *dv, sal_vaddr_t addr, uint32 count,
           pbmp_t l2pbm, pbmp_t utpbm, pbmp_t l3pbm, uint32 flags, uint32 *hgh)
{
    dcb9_t      *d;     /* DCB */
    uint32      *di;    /* DCB integer pointer */
    uint32      paddr;  /* Packet buffer physical address */

    d = (dcb9_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (addr) {
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    } else {
        paddr = 0;
    }

    if (dv->dv_vcnt > 0 && (dv->dv_flags & DV_F_COMBINE_DCB) &&
        (d[-1].c_sg != 0) &&
        (d[-1].addr + d[-1].c_count) == paddr &&
        d[-1].c_count + count <= DCB_MAX_REQCOUNT) {
        d[-1].c_count += count;
        return dv->dv_cnt - dv->dv_vcnt;
    }

    if (dv->dv_vcnt >= dv->dv_cnt) {
        return SOC_E_FULL;
    }
    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = 0;
    di[5] = di[6] = di[7] = di[8] = di[9] = di[10] = 0;

    d->addr = paddr;
    d->c_count = count;
    d->c_sg = 1;

    d->c_stat = 1;
    d->c_purge = SOC_DMA_PURGE_GET(flags);
    if (SOC_DMA_HG_GET(flags)) {
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
        soc_higig_hdr_t *mh = (soc_higig_hdr_t *)hgh;
        if (mh->overlay1.start == SOC_HIGIG2_START) {
#if defined(BCM_BRADLEY_SUPPORT)
            dcb11_t *d11 = (dcb11_t *)d;
#elif defined(BCM_RAPTOR_SUPPORT)
            dcb12_t *d11 = (dcb12_t *)d;
#endif
            d11->mh3 = soc_ntohl(hgh[3]);
        }
#endif  /* BCM_BRADLEY_SUPPORT || BCM_RAPTOR_SUPPORT */
        d->c_hg = 1;
        d->mh0 = soc_ntohl(hgh[0]);
        d->mh1 = soc_ntohl(hgh[1]);
        d->mh2 = soc_ntohl(hgh[2]);
    }

    dv->dv_vcnt += 1; 

    return dv->dv_cnt - dv->dv_vcnt;
}

static int
dcb9_addrx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb9_t      *d;     /* DCB */
    uint32      *di;    /* DCB integer pointer */

    d = (dcb9_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = 0;
    di[5] = di[6] = di[7] = di[8] = di[9] = di[10] = 0;

    if (addr) {
        d->addr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    }
    d->c_count = count;
    d->c_sg = 1;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static uint32
dcb9_intrinfo(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb9_t      *d = (dcb9_t *)dcb;     /*  DCB */
    uint32      f;                      /* SOC_DCB_INFO_* flags */

    if (!d->done) {
        return 0;
    }
    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }
    *count = d->count;
    return f;
}

/*
 * Function:
 *      dcb9_rx_untagged_get
 * Description:
 *      When Firebolt/Firebolt_2 are in double tagging mode:
 *      - If incoming front-panel packet is double tagged,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1, d->add_vid = 0.
 *      - If incoming front-panel packet is single outer tagged,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 0, d->add_vid = 0.
 *      - If incoming front-panel packet is single inner tagged,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1, d->add_vid = 1. 
 *      - If incoming front-panel packet is untagged,
 *        d->ingress_untagged = 1,
 *        h->hgp_overlay1.ingress_tagged = 0, d->add_vid = 1. 
 *      - For an incoming Higig packet,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1 if the packet contains
 *        an inner tag, 0 otherwise,
 *        d->add_vid = 1.
 *
 *      In single tagging mode:
 *      - If incoming front-panel packet is tagged,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1, d->add_vid = 0.
 *      - If incoming front-panel packet is untagged,
 *        d->ingress_untagged = 1,
 *        h->hgp_overlay1.ingress_tagged = 0, d->add_vid = 1. 
 *      - For an incoming Higig packet,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1 if the packet originally
 *        ingressed the ingress chip's front-panel port with a tag,
 *        0 otherwise,
 *        d->add_vid = 1.
 */
static uint32
dcb9_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb9_t *d = (dcb9_t *)dcb;
    uint32 hgh[3];
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);

    return (dt_mode ?
            (ingport_is_hg ?
             (h->hgp_overlay1.ingress_tagged ? 0 : 2) :
             (h->hgp_overlay1.ingress_tagged ?
              (d->add_vid ? 1 : 0) :
              (d->add_vid ? 3 : 2))) :
            (h->hgp_overlay1.ingress_tagged ? 2 : 3));
}

SETFUNCFIELD(9, reqcount, c_count, uint32 count, count)
GETFUNCFIELD(9, reqcount, c_count)
GETFUNCFIELD(9, xfercount, count)
/* addr_set, addr_get, paddr_get - Same as DCB 0 */
SETFUNCFIELD(9, done, done, int val, val ? 1 : 0)
GETFUNCFIELD(9, done, done)
SETFUNCFIELD(9, sg, c_sg, int val, val ? 1 : 0)
GETFUNCFIELD(9, sg, c_sg)
SETFUNCFIELD(9, chain, c_chain, int val, val ? 1 : 0)
GETFUNCFIELD(9, chain, c_chain)
SETFUNCFIELD(9, reload, c_reload, int val, val ? 1 : 0)
GETFUNCFIELD(9, reload, c_reload)
SETFUNCERR(9, desc_intr, int)
GETFUNCERR(9, desc_intr)
SETFUNCERR(9, tx_l2pbm, pbmp_t)
SETFUNCERR(9, tx_utpbm, pbmp_t)
SETFUNCERR(9, tx_l3pbm, pbmp_t)
SETFUNCERR(9, tx_crc, int)
SETFUNCERR(9, tx_cos, int)
SETFUNCERR(9, tx_destmod, uint32)
SETFUNCERR(9, tx_destport, uint32)
SETFUNCERR(9, tx_opcode, uint32)
SETFUNCERR(9, tx_srcmod, uint32)
SETFUNCERR(9, tx_srcport, uint32)
SETFUNCERR(9, tx_prio, uint32)
SETFUNCERR(9, tx_pfm, uint32)
GETFUNCFIELD(9, rx_start, start)
GETFUNCFIELD(9, rx_end, end)
GETFUNCFIELD(9, rx_error, error)
/* Fields extracted from MH/PBI */
GETFUNCFIELD(9, rx_cos, cpu_cos)
GETHGFUNCEXPR(9, rx_destmod, (h->overlay1.dst_mod |
                             (h->hgp_overlay1.dst_mod_5 << 5)))
GETHGFUNCFIELD(9, rx_destport, dst_port)
GETHGFUNCFIELD(9, rx_opcode, opcode)
GETHGFUNCFIELD(9, rx_prio, vlan_pri) /* outer_pri */

#ifdef BCM_FIREBOLT_SUPPORT
GETFUNCFIELD(9, rx_matchrule, match_rule)
GETFUNCFIELD(9, rx_reason, reason)
GETFUNCNULL(9, rx_reason_hi)
GETFUNCFIELD(9, rx_ingport, srcport)
GETFUNCERR(9, rx_vclabel)
GETFUNCEXPR(9, rx_mirror, ((d->imirror) | (d->emirror)))
GETFUNCERR(9, rx_timestamp)
GETFUNCERR(9, rx_timestamp_upper)
GETFUNCFIELD(9, outer_vid, outer_vid)
GETFUNCFIELD(9, outer_pri, outer_pri)
GETFUNCFIELD(9, outer_cfi, outer_cfi)
#endif /* BCM_FIREBOLT_SUPPORT */
GETHGFUNCFIELD(9, rx_srcport, src_port)
GETHGFUNCEXPR(9, rx_srcmod, (h->overlay1.src_mod |
                             (h->hgp_overlay1.src_mod_5 << 5)))
/* DCB 9 specific fields */
SETFUNCFIELD(9, hg, c_hg, uint32 hg, hg)
GETFUNCFIELD(9, hg, c_hg)
SETFUNCFIELD(9, stat, c_stat, uint32 stat, stat)
GETFUNCFIELD(9, stat, c_stat)
SETFUNCFIELD(9, purge, c_purge, uint32 purge, purge)
GETFUNCFIELD(9, purge, c_purge)
GETPTREXPR(9, mhp, &(d->mh0))
GETFUNCNULL(9, rx_outer_tag_action)
GETFUNCNULL(9, inner_vid)
GETFUNCNULL(9, inner_pri)
GETFUNCNULL(9, inner_cfi)
GETFUNCNULL(9, rx_inner_tag_action)
GETFUNCFIELD(9, rx_bpdu, bpdu)
GETFUNCFIELD(9, rx_l3_intf, l3_intf)
GETFUNCNULL(9, rx_decap_tunnel)


#ifdef BCM_BRADLEY_SUPPORT
static uint32
dcb11_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb11_t *d = (dcb11_t *)dcb;
    uint32 hgh[4];
    soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&hgh[0];

    COMPILER_REFERENCE(dt_mode);
    COMPILER_REFERENCE(ingport_is_hg);

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    hgh[3] = soc_htonl(d->mh3);

    return (!h->ppd_overlay1.ingress_tagged);
}

GETFUNCFIELD(11, rx_cos, cpu_cos)
GETHG2FUNCFIELD(11, rx_destmod, dst_mod)
GETHG2FUNCFIELD(11, rx_destport, dst_port)
GETHG2FUNCEXPR(11, rx_srcport, h->ppd_overlay1.src_port |
                               ((h->ppd_overlay1.ppd_type <= 1) ?
                                (h->ppd_overlay1.src_t << 5) : 0))
GETHG2FUNCFIELD(11, rx_srcmod, src_mod)
GETHG2FUNCFIELD(11, rx_opcode, opcode)
GETHG2FUNCFIELD(11, rx_prio, vlan_pri) /* outer_pri */
GETFUNCFIELD(11, rx_matchrule, match_rule)
GETFUNCEXPR(11, rx_reason, d->reason)
GETFUNCNULL(11, rx_reason_hi)
GETFUNCFIELD(11, rx_ingport, srcport)
GETFUNCEXPR(11, rx_mirror, ((d->imirror) | (d->emirror)))
GETFUNCERR(11, rx_timestamp)
GETFUNCERR(11, rx_timestamp_upper)
SETFUNCERR(11, stat, uint32)
GETFUNCEXPR(11, stat, d->c_hg ^ d->c_hg) /* 0 */
SETFUNCERR(11, purge, uint32)
GETFUNCEXPR(11, purge, d->c_hg ^ d->c_hg) /* 0 */
GETFUNCFIELD(11, outer_vid, outer_vid)
GETFUNCFIELD(11, outer_pri, outer_pri)
GETFUNCFIELD(11, outer_cfi, outer_cfi)
GETFUNCFIELD(11, rx_bpdu, bpdu)
GETFUNCFIELD(11, rx_l3_intf, l3_intf)
#endif /* BCM_BRADLEY_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) 
static uint32
dcb12_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb12_t *d = (dcb12_t *)dcb;
    uint32 hgh[4];
    soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&hgh[0];

    COMPILER_REFERENCE(dt_mode);
    COMPILER_REFERENCE(ingport_is_hg);

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    hgh[3] = soc_htonl(d->mh3);

    return (!h->ppd_overlay1.ingress_tagged);
}

GETHG2FUNCFIELD(12, rx_destmod, dst_mod)
GETHG2FUNCFIELD(12, rx_destport, dst_port)
GETHG2FUNCFIELD(12, rx_opcode, opcode)
GETHG2FUNCFIELD(12, rx_prio, vlan_pri) /* outer_pri */
GETHG2FUNCFIELD(12, rx_srcport, src_port)
GETHG2FUNCFIELD(12, rx_srcmod, src_mod)
GETFUNCFIELD(12, rx_ingport, srcport)
GETFUNCFIELD(12, rx_crc, regen_crc)
GETFUNCFIELD(12, rx_cos, cpu_cos)
GETFUNCFIELD(12, rx_matchrule, match_rule)
GETFUNCFIELD(12, rx_reason, reason)
GETFUNCNULL(12, rx_reason_hi)
GETHG2FUNCEXPR(12, rx_mcast, ((h->ppd_overlay1.dst_mod << 8) |
                              (h->ppd_overlay1.dst_port)))
GETHG2FUNCEXPR(12, rx_vclabel, ((h->ppd_overlay1.vc_label_19_16 << 16) |
                              (h->ppd_overlay1.vc_label_15_8 << 8) |
                              (h->ppd_overlay1.vc_label_7_0)))
GETHG2FUNCEXPR(12, rx_classtag, (h->ppd_overlay1.ppd_type != 1 ? 0 :
                                 (h->ppd_overlay2.ctag_hi << 8) |
                                 (h->ppd_overlay2.ctag_lo)))
GETFUNCEXPR(12, rx_mirror, ((d->imirror) | (d->emirror)))
GETFUNCERR(12, rx_timestamp)
GETFUNCERR(12, rx_timestamp_upper)
GETFUNCFIELD(12, outer_vid, outer_vid)
GETFUNCFIELD(12, outer_pri, outer_pri)
GETFUNCFIELD(12, outer_cfi, outer_cfi)
GETFUNCFIELD(12, rx_bpdu, bpdu)
#ifdef  PLISIM
SETFUNCFIELD(12, rx_crc, regen_crc, int val, val ? 1 : 0)
SETFUNCFIELD(12, rx_ingport, srcport, int val, val)
#endif /* PLISIM */
GETHG2FUNCFIELD(15, rx_destmod, dst_mod)
GETHG2FUNCFIELD(15, rx_destport, dst_port)
GETHG2FUNCFIELD(15, rx_opcode, opcode)
GETHG2FUNCFIELD(15, rx_prio, vlan_pri) /* outer_pri */
GETHG2FUNCFIELD(15, rx_srcport, src_port)
GETHG2FUNCFIELD(15, rx_srcmod, src_mod)
GETFUNCFIELD(15, rx_ingport, srcport)
GETFUNCFIELD(15, rx_crc, regen_crc)
GETFUNCFIELD(15, rx_cos, cpu_cos)
GETFUNCFIELD(15, rx_matchrule, match_rule)
GETFUNCFIELD(15, rx_reason, reason)
GETFUNCNULL(15, rx_reason_hi)
GETHG2FUNCEXPR(15, rx_mcast, ((h->ppd_overlay1.dst_mod << 8) |
                              (h->ppd_overlay1.dst_port)))
GETHG2FUNCEXPR(15, rx_vclabel, ((h->ppd_overlay1.vc_label_19_16 << 16) |
                              (h->ppd_overlay1.vc_label_15_8 << 8) |
                              (h->ppd_overlay1.vc_label_7_0)))
GETHG2FUNCEXPR(15, rx_classtag, (h->ppd_overlay1.ppd_type != 1 ? 0 :
                                 (h->ppd_overlay2.ctag_hi << 8) |
                                 (h->ppd_overlay2.ctag_lo)))
GETFUNCEXPR(15, rx_mirror, ((d->imirror) | (d->emirror)))
GETFUNCERR(15, rx_timestamp)
GETFUNCERR(15, rx_timestamp_upper)
GETFUNCFIELD(15, outer_vid, outer_vid)
GETFUNCFIELD(15, outer_pri, outer_pri)
GETFUNCFIELD(15, outer_cfi, outer_cfi)
GETFUNCFIELD(15, rx_l3_intf, l3_intf)
#ifdef  PLISIM
SETFUNCFIELD(15, rx_crc, regen_crc, int val, val ? 1 : 0)
SETFUNCFIELD(15, rx_ingport, srcport, int val, val)
#endif /* PLISIM */
#endif  /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
GETFUNCFIELD(13, outer_vid, outer_vid)
GETFUNCFIELD(13, outer_pri, outer_pri)
GETFUNCFIELD(13, outer_cfi, outer_cfi)


/*
 * Function:
 *      dcb13_rx_untagged_get
 * Description:
 *      When Firebolt/Firebolt_2 are in double tagging mode:
 *      - If incoming front-panel packet is double tagged,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1, d->add_vid = 0.
 *      - If incoming front-panel packet is single outer tagged,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 0, d->add_vid = 0.
 *      - If incoming front-panel packet is single inner tagged,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1, d->add_vid = 1. 
 *      - If incoming front-panel packet is untagged,
 *        d->ingress_untagged = 1,
 *        h->hgp_overlay1.ingress_tagged = 0, d->add_vid = 1. 
 *      - For an incoming Higig packet,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1 if the packet contains
 *        an inner tag, 0 otherwise,
 *        d->add_vid = 1.
 *
 *      In single tagging mode:
 *      - If incoming front-panel packet is tagged,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1, d->add_vid = 0.
 *      - If incoming front-panel packet is untagged,
 *        d->ingress_untagged = 1,
 *        h->hgp_overlay1.ingress_tagged = 0, d->add_vid = 1. 
 *      - For an incoming Higig packet,
 *        d->ingress_untagged = 0,
 *        h->hgp_overlay1.ingress_tagged = 1 if the packet originally
 *        ingressed the ingress chip's front-panel port with a tag,
 *        0 otherwise,
 *        d->add_vid = 1.
 *
 *       When ClassTag is configured HiGiG overlay2 format is used and
 *       it doesn't carry ingress_tagged, so use the DCB ingress_untagged
 *       field to determine the tag.
 *
 *       FB2 will have the below limitation in determining  the tag values only 
 *       when ClassTag is configured: 
 *        1. Outer tagged and double tagged will be shown as Doubled tagged packets and
 *        2. For packets coming on HighGig ports, tagging can't be determined.
 */
static uint32
dcb13_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb13_t *d = (dcb13_t *)dcb;
    uint32 hgh[3];
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    if(h->hgp_overlay1.hdr_format == 0)
    {
        return(dt_mode ?
                (ingport_is_hg ?
                 (h->hgp_overlay1.ingress_tagged ? 0 : 2) :
                 (h->hgp_overlay1.ingress_tagged ?
                  (d->add_vid ? 1 : 0) :
                  (d->add_vid ? 3 : 2))) :
                (h->hgp_overlay1.ingress_tagged ? 2 : 3)); 
    }
    else
    {
        return(dt_mode ?
                (d->ingress_untagged ? (d->add_vid ? 3 : 2) :
                 (d->add_vid ? 1 : 0)) :
                (d->ingress_untagged ? 3 : 2));
    }
}

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_FIREBOLT_SUPPORT */

#if defined(BCM_HAWKEYE_SUPPORT)
static uint32
dcb17_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb17_t *d = (dcb17_t *)dcb;

    COMPILER_REFERENCE(dt_mode);
    COMPILER_REFERENCE(ingport_is_hg);

    return d->ingress_untagged;
}

GETFUNCFIELD(17, rx_srcport, srcport)
GETFUNCFIELD(17, rx_reason, reason)
GETFUNCFIELD(17, rx_crc, regen_crc)
GETFUNCFIELD(17, rx_cos, cpu_cos)
GETFUNCNULL(17, rx_destmod)
GETFUNCNULL(17, rx_destport)
GETFUNCNULL(17, rx_opcode)
GETFUNCNULL(17, rx_classtag)
GETFUNCFIELD(17, rx_prio, outer_pri)
GETFUNCNULL(17, rx_srcmod)
GETFUNCNULL(17, rx_mcast)
GETFUNCNULL(17, rx_vclabel)
GETHG2FUNCEXPR(17, rx_timestamp, ((uint32 *)h)[1])
GETFUNCERR(17, rx_timestamp_upper)
#endif /* BCM_HAWKEYE_SUPPORT */

#ifdef BROADCOM_DEBUG
GETFUNCERR(9, tx_l2pbm) 
GETFUNCERR(9, tx_utpbm) 
GETFUNCERR(9, tx_l3pbm)
GETFUNCERR(9, tx_crc) 
GETFUNCERR(9, tx_cos)
GETFUNCERR(9, tx_destmod)
GETFUNCERR(9, tx_destport)
GETFUNCERR(9, tx_opcode) 
GETFUNCERR(9, tx_srcmod)
GETFUNCERR(9, tx_srcport)
GETFUNCERR(9, tx_prio)
GETFUNCERR(9, tx_pfm)
#endif /* BROADCOM_DEBUG */

static uint32 dcb9_rx_crc_get(dcb_t *dcb) {
    return 0;
}

#if defined(LE_HOST)
static uint32 dcb9_rx_classtag_get(dcb_t *dcb)
{
    dcb9_t *d = (dcb9_t *)dcb;
    uint32  hgh[3];
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];
    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    if (h->overlay1.hdr_format != 1) { /* return 0 if not overlay 2 format */
        return 0;
    }
    return((h->overlay2.ctag_hi << 8) | (h->overlay2.ctag_lo << 0));
}

static uint32 dcb9_rx_mcast_get(dcb_t *dcb)
{
    dcb9_t *d = (dcb9_t *)dcb;
    uint32  hgh[3];
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];
    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    return((h->overlay1.dst_mod << 5) | (h->overlay1.dst_port << 0));
}
#else
static uint32 dcb9_rx_classtag_get(dcb_t *dcb)
{
    dcb9_t *d = (dcb9_t *)dcb;
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&d->mh0;
    if (h->overlay1.hdr_format != 1) { /* return 0 if not overlay 2 format */
        return 0;
    }
    return((h->overlay2.ctag_hi << 8) | (h->overlay2.ctag_lo << 0));
}

static uint32 dcb9_rx_mcast_get(dcb_t *dcb)
{
    dcb9_t *d = (dcb9_t *)dcb;
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&d->mh0;
    return((h->overlay1.dst_mod << 5) | (h->overlay1.dst_port << 0));
}
#endif

#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb9_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[5] = d[6] = d[7] = d[8] = d[9] = d[10] = 0;
}
SETFUNCFIELD(9, xfercount, count, uint32 count, count)
SETFUNCFIELD(9, rx_start, start, int val, val ? 1 : 0)
SETFUNCFIELD(9, rx_end, end, int val, val ? 1 : 0)
SETFUNCFIELD(9, rx_error, error, int val, val ? 1 : 0)
SETFUNCEXPRIGNORE(9, rx_crc, int val, ignore)
#ifdef BCM_FIREBOLT_SUPPORT
SETFUNCFIELD(9, rx_ingport, srcport, int val, val)
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_BRADLEY_SUPPORT
SETFUNCFIELD(11, rx_ingport, srcport, int val, val)
#endif /* BCM_BRADLEY_SUPPORT */
#endif  /* PLISIM */

#ifdef BROADCOM_DEBUG
static void
dcb9_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb9_t *d = (dcb9_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
#if defined(LE_HOST)
    uint32  hgh[4];
    uint8 *h = (uint8 *)&hgh[0];

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
#if defined(BCM_BRADLEY_SUPPORT)
            dcb11_t *d11 = (dcb11_t *)d;
#elif defined(BCM_RAPTOR_SUPPORT)
            dcb12_t *d11 = (dcb12_t *)d;
#endif
    hgh[3] = soc_htonl(d11->mh3);
#endif
    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
#else
    uint8 *h = (uint8 *)&d->mh0;
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %ssg %schain %sreload %shg %sstat %spause %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %sstart %send %serror\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!"));
    }
#ifdef BCM_FIREBOLT_SUPPORT
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb)) &&
        (SOC_DCB_TYPE(unit) == 9)) {
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sadd_vid %sbpdu %scell_error %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->add_vid ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3ipmc %sl3only %sl3uc %spkt_aged %spurge_cell %ssrc_hg\n"),
                 prefix,
                 d->l3ipmc ? "" : "!",
                 d->l3only ? "" : "!",
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d mtp_index=%d reason=%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->l3_intf,
                 (d->mtp_index_hi << 2) | d->mtp_index_lo,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 (d->dscp_hi << 4) | d->dscp_lo,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_BRADLEY_SUPPORT
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb)) &&
        (SOC_DCB_TYPE(unit) == 11)) {
        dcb11_t *d = (dcb11_t *)dcb;
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sadd_vid %sbpdu %scell_error %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->add_vid ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3ipmc %sl3only %sl3uc %spkt_aged %spurge_cell %ssrc_hg\n"),
                 prefix,
                 d->l3ipmc ? "" : "!",
                 d->l3only ? "" : "!",
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d mtp_index=%d reason=%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->l3_intf,
                 d->mtp_index,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d hg_type=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index,
                 d->hg_type
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
    }
#endif /* BCM_BRADLEY_SUPPORT */
#if defined(BCM_RAPTOR_SUPPORT)
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb)) &&
        (SOC_DCB_TYPE(unit) == 12)) {
        dcb12_t *d = (dcb12_t *)dcb;
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sdo_not_change_ttl %sadd_vid %sbpdu %scell_error %schg_tos\n"),
                 prefix,
                 d->do_not_change_ttl ? "" : "!",
                 d->add_vid ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3uc %spkt_aged %spurge_cell %ssrc_hg %semirror %simirror\n"),
                 prefix,
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d mtp_index=%d reason=%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->mtp_index,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
    }
#endif /* BCM_RAPTOR_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb)) &&
        (SOC_DCB_TYPE(unit) == 13)) {
        dcb13_t *d = (dcb13_t *)dcb;
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sadd_vid %sbpdu %scell_error %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->add_vid ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3ipmc %sl3only %sl3uc %spkt_aged %spurge_cell %ssrc_hg\n"),
                 prefix,
                 d->l3ipmc ? "" : "!",
                 d->l3only ? "" : "!",
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sivlan_add\n"),
                 prefix,
                 d->inner_vlan_add ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d mtp_index=%d reason=%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->l3_intf,
                 (d->mtp_index_hi << 2) | d->mtp_index_lo,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 (d->dscp_hi << 6) | d->dscp_lo,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_FIREBOLT_SUPPORT */
#ifdef BCM_RAPTOR_SUPPORT
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb)) &&
        (SOC_DCB_TYPE(unit) == 15)) {
        dcb15_t *d = (dcb15_t *)dcb;
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sadd_vid %sbpdu %scell_error %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->add_vid ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3ipmc %sl3only %sl3uc %spkt_aged %spurge_cell %ssrc_hg\n"),
                 prefix,
                 d->l3ipmc ? "" : "!",
                 d->l3only ? "" : "!",
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d mtp_index=%d reason=%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->l3_intf,
                 d->mtp_index,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
    }
#endif /* BCM_RAPTOR_SUPPORT */
#ifdef BCM_SCORPION_SUPPORT
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb)) &&
        (SOC_DCB_TYPE(unit) == 16)) {
        dcb16_t *d = (dcb16_t *)dcb;
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sbpdu %scell_error %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3ipmc %sl3only %sl3uc %spkt_aged %spurge_cell %ssrc_hg\n"),
                 prefix,
                 d->l3ipmc ? "" : "!",
                 d->l3only ? "" : "!",
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d mtp_index=%d reason=%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->l3_intf,
                 d->mtp_index,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d hg_type=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index,
                 d->hg_type
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
    }
#endif /* BCM_SCORPION_SUPPORT */
#ifdef BCM_HAWKEYE_SUPPORT
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb)) &&
        (SOC_DCB_TYPE(unit) == 17)) {
        dcb17_t *d = (dcb17_t *)dcb;
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sadd_vid %sbpdu %scell_error %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->add_vid ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3ipmc %sl3only %sl3uc %spkt_aged %spurge_cell %ssrc_hg\n"),
                 prefix,
                 d->l3ipmc ? "" : "!",
                 d->l3only ? "" : "!",
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d l3_intf=%d mtp_index=%d reason=%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->l3_intf,
                 d->mtp_index,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
    }
#endif /* BCM_HAWKEYE_SUPPORT */
#ifdef BCM_RAPTOR_SUPPORT
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb)) &&
        (SOC_DCB_TYPE(unit) == 18)) {
        dcb18_t *d = (dcb18_t *)dcb;
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sadd_vid %sbpdu %scell_error %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->add_vid ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3ipmc %sl3only %sl3uc %spkt_aged %spurge_cell %ssrc_hg\n"),
                 prefix,
                 d->l3ipmc ? "" : "!",
                 d->l3only ? "" : "!",
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d mtp_index=%d reason=%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->l3_intf,
                 d->mtp_index,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
    }
#endif /* BCM_RAPTOR_SUPPORT */
}
#endif /* BROADCOM_DEBUG */

#ifdef BCM_FIREBOLT_SUPPORT
GETFUNCNULL(9, rx_switch_drop)
GETFUNCNULL(9, olp_encap_oam_pkt)
GETFUNCNULL(9, read_ecc_error)
GETFUNCNULL(9, desc_remaining)
SETFUNCNULL(9, desc_remaining)
GETFUNCNULL(9, desc_status)
SETFUNCNULL(9, desc_status)
GETFUNCNULL(9, match_id_lo)
GETFUNCNULL(9, match_id_hi)
GETFUNCNULL(9, forwarding_type)
GETFUNCNULL(9, forwarding_zone_id)

dcb_op_t dcb9_op = {
    9,
    sizeof(dcb9_t),
    dcb9_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb9_init,
    dcb9_addtx,
    dcb9_addrx,
    dcb9_intrinfo,
    dcb9_reqcount_set,
    dcb9_reqcount_get,
    dcb9_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb9_done_set,
    dcb9_done_get,
    dcb9_sg_set,
    dcb9_sg_get,
    dcb9_chain_set,
    dcb9_chain_get,
    dcb9_reload_set,
    dcb9_reload_get,
    dcb9_desc_intr_set,
    dcb9_desc_intr_get,
    dcb9_tx_l2pbm_set,
    dcb9_tx_utpbm_set,
    dcb9_tx_l3pbm_set,
    dcb9_tx_crc_set,
    dcb9_tx_cos_set,
    dcb9_tx_destmod_set,
    dcb9_tx_destport_set,
    dcb9_tx_opcode_set,
    dcb9_tx_srcmod_set,
    dcb9_tx_srcport_set,
    dcb9_tx_prio_set,
    dcb9_tx_pfm_set,
    dcb9_rx_untagged_get,
    dcb9_rx_crc_get,
    dcb9_rx_cos_get,
    dcb9_rx_destmod_get,
    dcb9_rx_destport_get,
    dcb9_rx_opcode_get,
    dcb9_rx_classtag_get,
    dcb9_rx_matchrule_get,
    dcb9_rx_start_get,
    dcb9_rx_end_get,
    dcb9_rx_error_get,
    dcb9_rx_prio_get,
    dcb9_rx_reason_get,
    dcb9_rx_reason_hi_get,
    dcb9_rx_ingport_get,
    dcb9_rx_srcport_get,
    dcb9_rx_srcmod_get,
    dcb9_rx_mcast_get,
    dcb9_rx_vclabel_get,
    dcb9_rx_mirror_get,
    dcb9_rx_timestamp_get,
    dcb9_rx_timestamp_upper_get,
    dcb9_hg_set,
    dcb9_hg_get,
    dcb9_stat_set,
    dcb9_stat_get,
    dcb9_purge_set,
    dcb9_purge_get,
    dcb9_mhp_get,
    dcb9_outer_vid_get,
    dcb9_outer_pri_get,
    dcb9_outer_cfi_get,
    dcb9_rx_outer_tag_action_get,
    dcb9_inner_vid_get,
    dcb9_inner_pri_get,
    dcb9_inner_cfi_get,
    dcb9_rx_inner_tag_action_get,
    dcb9_rx_bpdu_get,
    dcb9_rx_l3_intf_get,
    dcb9_rx_decap_tunnel_get,
    dcb9_rx_switch_drop_get,
    dcb9_olp_encap_oam_pkt_get,
    dcb9_read_ecc_error_get,
    dcb9_desc_remaining_get,
    dcb9_desc_remaining_set,
    dcb9_desc_status_get,
    dcb9_desc_status_set,
    dcb9_match_id_lo_get,
    dcb9_match_id_hi_get,
    dcb9_forwarding_type_get,
    dcb9_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb9_tx_l2pbm_get,
    dcb9_tx_utpbm_get,
    dcb9_tx_l3pbm_get,
    dcb9_tx_crc_get,
    dcb9_tx_cos_get,
    dcb9_tx_destmod_get,
    dcb9_tx_destport_get,
    dcb9_tx_opcode_get,
    dcb9_tx_srcmod_get,
    dcb9_tx_srcport_get,
    dcb9_tx_prio_get,
    dcb9_tx_pfm_get,

    dcb9_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb9_status_init,
    dcb9_xfercount_set,
    dcb9_rx_start_set,
    dcb9_rx_end_set,
    dcb9_rx_error_set,
    dcb9_rx_crc_set,
    dcb9_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_BRADLEY_SUPPORT
static soc_rx_reason_t
dcb11_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid             /* Offset 31 */
};

static soc_rx_reason_t *dcb11_rx_reason_maps[] = {
    dcb11_rx_reason_map,
    NULL
};

GETFUNCNULL(11, rx_switch_drop)
GETFUNCNULL(11, olp_encap_oam_pkt)
GETFUNCNULL(11, read_ecc_error)
GETFUNCNULL(11, desc_remaining)
SETFUNCNULL(11, desc_remaining)
GETFUNCNULL(11, desc_status)
SETFUNCNULL(11, desc_status)
GETFUNCNULL(11, match_id_lo)
GETFUNCNULL(11, match_id_hi)
GETFUNCNULL(11, forwarding_type)
GETFUNCNULL(11, forwarding_zone_id)

dcb_op_t dcb11_op = {
    11,
    sizeof(dcb11_t),
    dcb11_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb9_init,
    dcb9_addtx,
    dcb9_addrx,
    dcb9_intrinfo,
    dcb9_reqcount_set,
    dcb9_reqcount_get,
    dcb9_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb9_done_set,
    dcb9_done_get,
    dcb9_sg_set,
    dcb9_sg_get,
    dcb9_chain_set,
    dcb9_chain_get,
    dcb9_reload_set,
    dcb9_reload_get,
    dcb9_desc_intr_set,
    dcb9_desc_intr_get,
    dcb9_tx_l2pbm_set,
    dcb9_tx_utpbm_set,
    dcb9_tx_l3pbm_set,
    dcb9_tx_crc_set,
    dcb9_tx_cos_set,
    dcb9_tx_destmod_set,
    dcb9_tx_destport_set,
    dcb9_tx_opcode_set,
    dcb9_tx_srcmod_set,
    dcb9_tx_srcport_set,
    dcb9_tx_prio_set,
    dcb9_tx_pfm_set,
    dcb11_rx_untagged_get,
    dcb9_rx_crc_get,
    dcb11_rx_cos_get,
    dcb11_rx_destmod_get,
    dcb11_rx_destport_get,
    dcb11_rx_opcode_get,
    dcb9_rx_classtag_get,
    dcb11_rx_matchrule_get,
    dcb9_rx_start_get,
    dcb9_rx_end_get,
    dcb9_rx_error_get,
    dcb11_rx_prio_get,
    dcb11_rx_reason_get,
    dcb11_rx_reason_hi_get,
    dcb11_rx_ingport_get,
    dcb11_rx_srcport_get,
    dcb11_rx_srcmod_get,
    dcb9_rx_mcast_get,
    dcb9_rx_vclabel_get,
    dcb11_rx_mirror_get,
    dcb11_rx_timestamp_get,
    dcb11_rx_timestamp_upper_get,
    dcb9_hg_set,
    dcb9_hg_get,
    dcb11_stat_set,
    dcb11_stat_get,
    dcb11_purge_set,
    dcb11_purge_get,
    dcb9_mhp_get,
    dcb11_outer_vid_get,
    dcb11_outer_pri_get,
    dcb11_outer_cfi_get,
    dcb9_rx_outer_tag_action_get,
    dcb9_inner_vid_get,
    dcb9_inner_pri_get,
    dcb9_inner_cfi_get,
    dcb9_rx_inner_tag_action_get,
    dcb11_rx_bpdu_get,
    dcb11_rx_l3_intf_get,
    dcb9_rx_decap_tunnel_get,
    dcb11_rx_switch_drop_get,
    dcb11_olp_encap_oam_pkt_get,
    dcb11_read_ecc_error_get,
    dcb11_desc_remaining_get,
    dcb11_desc_remaining_set,
    dcb11_desc_status_get,
    dcb11_desc_status_set,
    dcb11_match_id_lo_get,
    dcb11_match_id_hi_get,
    dcb11_forwarding_type_get,
    dcb11_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb9_tx_l2pbm_get,
    dcb9_tx_utpbm_get,
    dcb9_tx_l3pbm_get,
    dcb9_tx_crc_get,
    dcb9_tx_cos_get,
    dcb9_tx_destmod_get,
    dcb9_tx_destport_get,
    dcb9_tx_opcode_get,
    dcb9_tx_srcmod_get,
    dcb9_tx_srcport_get,
    dcb9_tx_prio_get,
    dcb9_tx_pfm_get,

    dcb9_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb9_status_init,
    dcb9_xfercount_set,
    dcb9_rx_start_set,
    dcb9_rx_end_set,
    dcb9_rx_error_set,
    dcb9_rx_crc_set,
    dcb11_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_BRADLEY_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT)
static soc_rx_reason_t
dcb12_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonL2LearnLimit,       /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid             /* Offset 31 */
};

static soc_rx_reason_t *dcb12_rx_reason_maps[] = {
    dcb12_rx_reason_map,
    NULL
};

GETFUNCNULL(12, rx_switch_drop)
GETFUNCNULL(12, olp_encap_oam_pkt)
GETFUNCNULL(12, read_ecc_error)
GETFUNCNULL(12, desc_remaining)
SETFUNCNULL(12, desc_remaining)
GETFUNCNULL(12, desc_status)
SETFUNCNULL(12, desc_status)
GETFUNCNULL(12, match_id_lo)
GETFUNCNULL(12, match_id_hi)
GETFUNCNULL(12, forwarding_type)
GETFUNCNULL(12, forwarding_zone_id)

dcb_op_t dcb12_op = {
    12,
    sizeof(dcb12_t),
    dcb12_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb9_init,
    dcb9_addtx,
    dcb9_addrx,
    dcb9_intrinfo,
    dcb9_reqcount_set,
    dcb9_reqcount_get,
    dcb9_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb9_done_set,
    dcb9_done_get,
    dcb9_sg_set,
    dcb9_sg_get,
    dcb9_chain_set,
    dcb9_chain_get,
    dcb9_reload_set,
    dcb9_reload_get,
    dcb9_desc_intr_set,
    dcb9_desc_intr_get,
    dcb9_tx_l2pbm_set,
    dcb9_tx_utpbm_set,
    dcb9_tx_l3pbm_set,
    dcb9_tx_crc_set,
    dcb9_tx_cos_set,
    dcb9_tx_destmod_set,
    dcb9_tx_destport_set,
    dcb9_tx_opcode_set,
    dcb9_tx_srcmod_set,
    dcb9_tx_srcport_set,
    dcb9_tx_prio_set,
    dcb9_tx_pfm_set,
    dcb12_rx_untagged_get,
    dcb12_rx_crc_get,
    dcb12_rx_cos_get,
    dcb12_rx_destmod_get,
    dcb12_rx_destport_get,
    dcb12_rx_opcode_get,
    dcb12_rx_classtag_get,
    dcb12_rx_matchrule_get,
    dcb9_rx_start_get,
    dcb9_rx_end_get,
    dcb9_rx_error_get,
    dcb12_rx_prio_get,
    dcb12_rx_reason_get,
    dcb12_rx_reason_hi_get,
    dcb12_rx_ingport_get,
    dcb12_rx_srcport_get,
    dcb12_rx_srcmod_get,
    dcb12_rx_mcast_get,
    dcb12_rx_vclabel_get,
    dcb12_rx_mirror_get,
    dcb12_rx_timestamp_get,
    dcb12_rx_timestamp_upper_get,
    dcb9_hg_set,
    dcb9_hg_get,
    dcb9_stat_set,
    dcb9_stat_get,
    dcb9_purge_set,
    dcb9_purge_get,
    dcb9_mhp_get,
    dcb12_outer_vid_get,
    dcb12_outer_pri_get,
    dcb12_outer_cfi_get,
    dcb9_rx_outer_tag_action_get,
    dcb9_inner_vid_get,
    dcb9_inner_pri_get,
    dcb9_inner_cfi_get,
    dcb9_rx_inner_tag_action_get,
    dcb12_rx_bpdu_get,
    dcb9_rx_l3_intf_get,
    dcb9_rx_decap_tunnel_get,
    dcb12_rx_switch_drop_get,
    dcb12_olp_encap_oam_pkt_get,
    dcb12_read_ecc_error_get,
    dcb12_desc_remaining_get,
    dcb12_desc_remaining_set,
    dcb12_desc_status_get,
    dcb12_desc_status_set,
    dcb12_match_id_lo_get,
    dcb12_match_id_hi_get,
    dcb12_forwarding_type_get,
    dcb12_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb9_tx_l2pbm_get,
    dcb9_tx_utpbm_get,
    dcb9_tx_l3pbm_get,
    dcb9_tx_crc_get,
    dcb9_tx_cos_get,
    dcb9_tx_destmod_get,
    dcb9_tx_destport_get,
    dcb9_tx_opcode_get,
    dcb9_tx_srcmod_get,
    dcb9_tx_srcport_get,
    dcb9_tx_prio_get,
    dcb9_tx_pfm_get,

    dcb9_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb9_status_init,
    dcb9_xfercount_set,
    dcb9_rx_start_set,
    dcb9_rx_end_set,
    dcb9_rx_error_set,
    dcb12_rx_crc_set,
    dcb12_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
static soc_rx_reason_t
dcb13_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonVlanFilterMatch,    /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid             /* Offset 31 */
};

static soc_rx_reason_t *dcb13_rx_reason_maps[] = {
    dcb13_rx_reason_map,
    NULL
};

GETFUNCERR(13, rx_timestamp)
GETFUNCERR(13, rx_timestamp_upper)
GETFUNCNULL(13, rx_switch_drop)
GETFUNCNULL(13, olp_encap_oam_pkt)
GETFUNCNULL(13, read_ecc_error)
GETFUNCNULL(13, desc_remaining)
SETFUNCNULL(13, desc_remaining)
GETFUNCNULL(13, desc_status)
SETFUNCNULL(13, desc_status)
GETFUNCNULL(13, match_id_lo)
GETFUNCNULL(13, match_id_hi)
GETFUNCNULL(13, forwarding_type)
GETFUNCNULL(13, forwarding_zone_id)

dcb_op_t dcb13_op = {
    13,
    sizeof(dcb13_t),
    dcb13_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb9_init,
    dcb9_addtx,
    dcb9_addrx,
    dcb9_intrinfo,
    dcb9_reqcount_set,
    dcb9_reqcount_get,
    dcb9_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb9_done_set,
    dcb9_done_get,
    dcb9_sg_set,
    dcb9_sg_get,
    dcb9_chain_set,
    dcb9_chain_get,
    dcb9_reload_set,
    dcb9_reload_get,
    dcb9_desc_intr_set,
    dcb9_desc_intr_get,
    dcb9_tx_l2pbm_set,
    dcb9_tx_utpbm_set,
    dcb9_tx_l3pbm_set,
    dcb9_tx_crc_set,
    dcb9_tx_cos_set,
    dcb9_tx_destmod_set,
    dcb9_tx_destport_set,
    dcb9_tx_opcode_set,
    dcb9_tx_srcmod_set,
    dcb9_tx_srcport_set,
    dcb9_tx_prio_set,
    dcb9_tx_pfm_set,
    dcb13_rx_untagged_get,
    dcb9_rx_crc_get,
    dcb9_rx_cos_get,
    dcb9_rx_destmod_get,
    dcb9_rx_destport_get,
    dcb9_rx_opcode_get,
    dcb9_rx_classtag_get,
    dcb9_rx_matchrule_get,
    dcb9_rx_start_get,
    dcb9_rx_end_get,
    dcb9_rx_error_get,
    dcb9_rx_prio_get,
    dcb9_rx_reason_get,
    dcb9_rx_reason_hi_get,
    dcb9_rx_ingport_get,
    dcb9_rx_srcport_get,
    dcb9_rx_srcmod_get,
    dcb9_rx_mcast_get,
    dcb9_rx_vclabel_get,
    dcb9_rx_mirror_get,
    dcb13_rx_timestamp_get,
    dcb13_rx_timestamp_upper_get,
    dcb9_hg_set,
    dcb9_hg_get,
    dcb9_stat_set,
    dcb9_stat_get,
    dcb9_purge_set,
    dcb9_purge_get,
    dcb9_mhp_get,
    dcb13_outer_vid_get,
    dcb13_outer_pri_get,
    dcb13_outer_cfi_get,
    dcb9_rx_outer_tag_action_get,
    dcb9_inner_vid_get,
    dcb9_inner_pri_get,
    dcb9_inner_cfi_get,
    dcb9_rx_inner_tag_action_get,
    dcb9_rx_bpdu_get,
    dcb9_rx_l3_intf_get,
    dcb9_rx_decap_tunnel_get,
    dcb13_rx_switch_drop_get,
    dcb13_olp_encap_oam_pkt_get,
    dcb13_read_ecc_error_get,
    dcb13_desc_remaining_get,
    dcb13_desc_remaining_set,
    dcb13_desc_status_get,
    dcb13_desc_status_set,
    dcb13_match_id_lo_get,
    dcb13_match_id_hi_get,
    dcb13_forwarding_type_get,
    dcb13_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb9_tx_l2pbm_get,
    dcb9_tx_utpbm_get,
    dcb9_tx_l3pbm_get,
    dcb9_tx_crc_get,
    dcb9_tx_cos_get,
    dcb9_tx_destmod_get,
    dcb9_tx_destport_get,
    dcb9_tx_opcode_get,
    dcb9_tx_srcmod_get,
    dcb9_tx_srcport_get,
    dcb9_tx_prio_get,
    dcb9_tx_pfm_get,

    dcb9_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb9_status_init,
    dcb9_xfercount_set,
    dcb9_rx_start_set,
    dcb9_rx_end_set,
    dcb9_rx_error_set,
    dcb9_rx_crc_set,
    dcb9_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_FIREBOLT_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
/*
 * DCB Type 14 Support
 */

static soc_rx_reason_t
dcb14_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL2LearnLimit,       /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t *dcb14_rx_reason_maps[] = {
    dcb14_rx_reason_map,
    NULL
};

static void
dcb14_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[0] = d[1] = d[2] = d[3] = d[4] = 0;
    d[5] = d[6] = d[7] = d[8] = d[9] = d[10] = 0;
    d[11] = d[12] = d[13] = d[14] = d[15] = 0;
}

static int
dcb14_addtx(dv_t *dv, sal_vaddr_t addr, uint32 count,
            pbmp_t l2pbm, pbmp_t utpbm, pbmp_t l3pbm, uint32 flags, uint32 *hgh)
{
    dcb14_t     *d;     /* DCB */
    uint32      *di;    /* DCB integer pointer */
    uint32      paddr;  /* Packet buffer physical address */
    int         unaligned;
    int         unaligned_bytes;
    uint8       *unaligned_buffer;
    uint8       *aligned_buffer;

    d = (dcb14_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (addr) {
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    } else {
        paddr = 0;
    }

    if (dv->dv_vcnt > 0 && (dv->dv_flags & DV_F_COMBINE_DCB) &&
        (d[-1].c_sg != 0) &&
        (d[-1].addr + d[-1].c_count) == paddr &&
        d[-1].c_count + count <= DCB_MAX_REQCOUNT) {
        d[-1].c_count += count;
        return dv->dv_cnt - dv->dv_vcnt;
    }

    /*
     * A few chip revisions do not support 128 byte PCI bursts
     * correctly if the address is not word-aligned. In case
     * we encounter an unaligned address, we consume an extra
     * DCB to correct the alignment.
     */
    do {
        if (dv->dv_vcnt >= dv->dv_cnt) {
            return SOC_E_FULL;
        }
        if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
            d[-1].c_chain = 1;
        }

        di = (uint32 *)d;
        di[0] = di[1] = di[2] = di[3] = di[4] = 0;
        di[5] = di[6] = di[7] = di[8] = di[9] = di[10] = 0;
        di[11] = di[12] = di[13] = di[14] = di[15] = 0;

        d->addr = paddr;
        d->c_count = count;
        d->c_sg = 1;

        d->c_stat = 1;
        d->c_purge = SOC_DMA_PURGE_GET(flags);
        if (SOC_DMA_HG_GET(flags)) {
            soc_higig_hdr_t *mh = (soc_higig_hdr_t *)hgh;
            if (mh->overlay1.start == SOC_HIGIG2_START) {
                d->mh3 = soc_ntohl(hgh[3]);
            }
            d->c_hg = 1;
            d->mh0 = soc_ntohl(hgh[0]);
            d->mh1 = soc_ntohl(hgh[1]);
            d->mh2 = soc_ntohl(hgh[2]);
            d->mh3 = soc_ntohl(hgh[3]);
        }

        unaligned = 0;
        if (soc_feature(dv->dv_unit, soc_feature_pkt_tx_align)) {
            if (paddr & 0x3) {
                unaligned_bytes = 4 - (paddr & 0x3);
                unaligned_buffer = (uint8 *)addr;
                aligned_buffer = SOC_DV_TX_ALIGN(dv, dv->dv_vcnt);
                aligned_buffer[0] = unaligned_buffer[0];
                aligned_buffer[1] = unaligned_buffer[1];
                aligned_buffer[2] = unaligned_buffer[2];
                d->addr = soc_cm_l2p(dv->dv_unit, aligned_buffer);
                if (count > 3) {
                    d->c_count = unaligned_bytes;
                    paddr += unaligned_bytes;
                    count -= unaligned_bytes;
                    unaligned = 1;
                }
            }
        }

        dv->dv_vcnt += 1;

        d = (dcb14_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);
    } while (unaligned);

    return dv->dv_cnt - dv->dv_vcnt;
}

static int
dcb14_addrx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb14_t     *d;     /* DCB */
    uint32      *di;    /* DCB integer pointer */

    d = (dcb14_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = 0;
    di[5] = di[6] = di[7] = di[8] = di[9] = di[10] = 0;
    di[11] = di[12] = di[13] = di[14] = di[15] = 0;

    if (addr) {
        d->addr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    }
    d->c_count = count;
    d->c_sg = 1;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static uint32
dcb14_intrinfo(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb14_t      *d = (dcb14_t *)dcb;     /*  DCB */
    uint32      f;                      /* SOC_DCB_INFO_* flags */

    if (!d->done) {
        return 0;
    }
    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }
    *count = d->count;
    return f;
}


static uint32
dcb14_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb14_t *d = (dcb14_t *)dcb;

    COMPILER_REFERENCE(dt_mode);

    return (ingport_is_hg ?
            ((d->itag_status) ? 0 : 2) :
            ((d->itag_status & 0x2) ?
             ((d->itag_status & 0x1) ? 0 : 2) :
             ((d->itag_status & 0x1) ? 1 : 3)));
}

SETFUNCFIELD(14, reqcount, c_count, uint32 count, count)
GETFUNCFIELD(14, reqcount, c_count)
GETFUNCFIELD(14, xfercount, count)
/* addr_set, addr_get, paddr_get - Same as DCB 0 */
SETFUNCFIELD(14, done, done, int val, val ? 1 : 0)
GETFUNCFIELD(14, done, done)
SETFUNCFIELD(14, sg, c_sg, int val, val ? 1 : 0)
GETFUNCFIELD(14, sg, c_sg)
SETFUNCFIELD(14, chain, c_chain, int val, val ? 1 : 0)
GETFUNCFIELD(14, chain, c_chain)
SETFUNCFIELD(14, reload, c_reload, int val, val ? 1 : 0)
GETFUNCFIELD(14, reload, c_reload)
SETFUNCERR(14, tx_l2pbm, pbmp_t)
SETFUNCERR(14, tx_utpbm, pbmp_t)
SETFUNCERR(14, tx_l3pbm, pbmp_t)
SETFUNCERR(14, tx_crc, int)
SETFUNCERR(14, tx_cos, int)
SETFUNCERR(14, tx_destmod, uint32)
SETFUNCERR(14, tx_destport, uint32)
SETFUNCERR(14, tx_opcode, uint32)
SETFUNCERR(14, tx_srcmod, uint32)
SETFUNCERR(14, tx_srcport, uint32)
SETFUNCERR(14, tx_prio, uint32)
SETFUNCERR(14, tx_pfm, uint32)
GETFUNCFIELD(14, rx_start, start)
GETFUNCFIELD(14, rx_end, end)
GETFUNCFIELD(14, rx_error, error)
GETFUNCFIELD(14, rx_cos, cpu_cos)
/* Fields extracted from MH/PBI */
GETHG2FUNCFIELD(14, rx_destmod, dst_mod)
GETHG2FUNCFIELD(14, rx_destport, dst_port)
GETHG2FUNCFIELD(14, rx_srcmod, src_mod)
GETHG2FUNCFIELD(14, rx_srcport, src_port)
GETHG2FUNCFIELD(14, rx_opcode, opcode)
GETHG2FUNCFIELD(14, rx_prio, vlan_pri) /* outer_pri */
GETHG2FUNCEXPR(14, rx_mcast, ((h->ppd_overlay1.dst_mod << 8) |
                              (h->ppd_overlay1.dst_port)))
GETHG2FUNCEXPR(14, rx_vclabel, ((h->ppd_overlay1.vc_label_19_16 << 16) |
                              (h->ppd_overlay1.vc_label_15_8 << 8) |
                              (h->ppd_overlay1.vc_label_7_0)))
GETHG2FUNCEXPR(14, rx_classtag, (h->ppd_overlay1.ppd_type != 1 ? 0 :
                                 (h->ppd_overlay2.ctag_hi << 8) |
                                 (h->ppd_overlay2.ctag_lo)))
GETFUNCFIELD(14, rx_matchrule, match_rule)
GETFUNCFIELD(14, rx_reason, reason)
GETFUNCFIELD(14, rx_reason_hi, reason_hi)
GETFUNCFIELD(14, rx_ingport, srcport)
GETFUNCEXPR(14, rx_mirror, ((d->imirror) | (d->emirror)))
GETFUNCERR(14, rx_timestamp)
GETFUNCERR(14, rx_timestamp_upper)
SETFUNCFIELD(14, hg, c_hg, uint32 hg, hg)
GETFUNCFIELD(14, hg, c_hg)
SETFUNCFIELD(14, stat, c_stat, uint32 stat, stat)
GETFUNCFIELD(14, stat, c_stat)
SETFUNCFIELD(14, purge, c_purge, uint32 purge, purge)
GETFUNCFIELD(14, purge, c_purge)
GETFUNCFIELD(14, outer_vid, outer_vid)
GETFUNCFIELD(14, outer_pri, outer_pri)
GETFUNCFIELD(14, outer_cfi, outer_cfi)
GETFUNCFIELD(14, rx_outer_tag_action, otag_action)
GETFUNCFIELD(14, rx_inner_tag_action, itag_action)
GETFUNCFIELD(14, rx_bpdu, bpdu)
GETFUNCFIELD(14, rx_l3_intf, l3_intf)
GETFUNCNULL(14, rx_switch_drop)
GETFUNCNULL(14, olp_encap_oam_pkt)
GETFUNCNULL(14, read_ecc_error)
GETFUNCNULL(14, desc_remaining)
SETFUNCNULL(14, desc_remaining)
GETFUNCNULL(14, desc_status)
SETFUNCNULL(14, desc_status)
GETFUNCNULL(14, match_id_lo)
GETFUNCNULL(14, match_id_hi)
GETFUNCNULL(14, forwarding_type)
GETFUNCNULL(14, forwarding_zone_id)

#ifdef BROADCOM_DEBUG
GETFUNCERR(14, tx_l2pbm) 
GETFUNCERR(14, tx_utpbm) 
GETFUNCERR(14, tx_l3pbm)
GETFUNCERR(14, tx_crc) 
GETFUNCERR(14, tx_cos)
GETFUNCERR(14, tx_destmod)
GETFUNCERR(14, tx_destport)
GETFUNCERR(14, tx_opcode) 
GETFUNCERR(14, tx_srcmod)
GETFUNCERR(14, tx_srcport)
GETFUNCERR(14, tx_prio)
GETFUNCERR(14, tx_pfm)
#endif /* BROADCOM_DEBUG */

static uint32 dcb14_rx_crc_get(dcb_t *dcb) {
    return 0;
}

#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb14_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[5] = d[6] = d[7] = d[8] = d[9] = d[10] = 0;
    d[11] = d[12] = d[13] = d[14] = d[15] = 0;
}
SETFUNCFIELD(14, xfercount, count, uint32 count, count)
SETFUNCFIELD(14, rx_start, start, int val, val ? 1 : 0)
SETFUNCFIELD(14, rx_end, end, int val, val ? 1 : 0)
SETFUNCFIELD(14, rx_error, error, int val, val ? 1 : 0)
SETFUNCEXPRIGNORE(14, rx_crc, int val, ignore)
SETFUNCFIELD(14, rx_ingport, srcport, int val, val)
#endif  /* PLISIM */

#ifdef BROADCOM_DEBUG
static void
dcb14_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb14_t *d = (dcb14_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
#if defined(LE_HOST)
    uint32  hgh[4];
    uint8 *h = (uint8 *)&hgh[0];

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    hgh[3] = soc_htonl(d->mh3);
#else
    uint8 *h = (uint8 *)&d->mh0;
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %ssg %schain %sreload %shg %sstat %spause %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %sstart %send %serror\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!"));
    }
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb))) {
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sdo_not_change_ttl %sbpdu %scell_error %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->do_not_change_ttl ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3ipmc %sl3only %sl3uc %spkt_aged %spurge_cell %ssrc_hg\n"),
                 prefix,
                 d->l3ipmc ? "" : "!",
                 d->l3only ? "" : "!",
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d mtp_index=%d reason=%08x_%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->l3_intf,
                 d->mtp_index,
                 d->reason_hi,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d hg_type=%d em_mtp_index=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index,
                 d->hg_type,
                 d->em_mtp_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  hgi=%d itag_status=%d otag_action=%d itag_action=%d\n"),
                 prefix,
                 d->hgi,
                 d->itag_status,
                 d->otag_action,
                 d->itag_action
                 ));
    }
}
#endif /* BROADCOM_DEBUG */
dcb_op_t dcb14_op = {
    14,
    sizeof(dcb14_t),
    dcb14_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb14_init,
    dcb14_addtx,
    dcb14_addrx,
    dcb14_intrinfo,
    dcb14_reqcount_set,
    dcb14_reqcount_get,
    dcb14_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb14_done_set,
    dcb14_done_get,
    dcb14_sg_set,
    dcb14_sg_get,
    dcb14_chain_set,
    dcb14_chain_get,
    dcb14_reload_set,
    dcb14_reload_get,
    dcb9_desc_intr_set,
    dcb9_desc_intr_get,
    dcb14_tx_l2pbm_set,
    dcb14_tx_utpbm_set,
    dcb14_tx_l3pbm_set,
    dcb14_tx_crc_set,
    dcb14_tx_cos_set,
    dcb14_tx_destmod_set,
    dcb14_tx_destport_set,
    dcb14_tx_opcode_set,
    dcb14_tx_srcmod_set,
    dcb14_tx_srcport_set,
    dcb14_tx_prio_set,
    dcb14_tx_pfm_set,
    dcb14_rx_untagged_get,
    dcb14_rx_crc_get,
    dcb14_rx_cos_get,
    dcb14_rx_destmod_get,
    dcb14_rx_destport_get,
    dcb14_rx_opcode_get,
    dcb14_rx_classtag_get,
    dcb14_rx_matchrule_get,
    dcb14_rx_start_get,
    dcb14_rx_end_get,
    dcb14_rx_error_get,
    dcb14_rx_prio_get,
    dcb14_rx_reason_get,
    dcb14_rx_reason_hi_get,
    dcb14_rx_ingport_get,
    dcb14_rx_srcport_get,
    dcb14_rx_srcmod_get,
    dcb14_rx_mcast_get,
    dcb14_rx_vclabel_get,
    dcb14_rx_mirror_get,
    dcb14_rx_timestamp_get,
    dcb14_rx_timestamp_upper_get,
    dcb14_hg_set,
    dcb14_hg_get,
    dcb14_stat_set,
    dcb14_stat_get,
    dcb14_purge_set,
    dcb14_purge_get,
    dcb9_mhp_get,
    dcb14_outer_vid_get,
    dcb14_outer_pri_get,
    dcb14_outer_cfi_get,
    dcb14_rx_outer_tag_action_get,
    dcb9_inner_vid_get,
    dcb9_inner_pri_get,
    dcb9_inner_cfi_get,
    dcb14_rx_inner_tag_action_get,
    dcb14_rx_bpdu_get,
    dcb14_rx_l3_intf_get,
    dcb9_rx_decap_tunnel_get,
    dcb14_rx_switch_drop_get,
    dcb14_olp_encap_oam_pkt_get,
    dcb14_read_ecc_error_get,
    dcb14_desc_remaining_get,
    dcb14_desc_remaining_set,
    dcb14_desc_status_get,
    dcb14_desc_status_set,
    dcb14_match_id_lo_get,
    dcb14_match_id_hi_get,
    dcb14_forwarding_type_get,
    dcb14_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb14_tx_l2pbm_get,
    dcb14_tx_utpbm_get,
    dcb14_tx_l3pbm_get,
    dcb14_tx_crc_get,
    dcb14_tx_cos_get,
    dcb14_tx_destmod_get,
    dcb14_tx_destport_get,
    dcb14_tx_opcode_get,
    dcb14_tx_srcmod_get,
    dcb14_tx_srcport_get,
    dcb14_tx_prio_get,
    dcb14_tx_pfm_get,

    dcb14_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb14_status_init,
    dcb14_xfercount_set,
    dcb14_rx_start_set,
    dcb14_rx_end_set,
    dcb14_rx_error_set,
    dcb14_rx_crc_set,
    dcb14_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT)
GETFUNCNULL(15, rx_switch_drop)
GETFUNCNULL(15, olp_encap_oam_pkt)
GETFUNCNULL(15, read_ecc_error)
GETFUNCNULL(15, desc_remaining)
SETFUNCNULL(15, desc_remaining)
GETFUNCNULL(15, desc_status)
SETFUNCNULL(15, desc_status)
GETFUNCNULL(15, match_id_lo)
GETFUNCNULL(15, match_id_hi)
GETFUNCNULL(15, forwarding_type)
GETFUNCNULL(15, forwarding_zone_id)

dcb_op_t dcb15_op = {
    15,
    sizeof(dcb15_t),
    dcb12_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb9_init,
    dcb9_addtx,
    dcb9_addrx,
    dcb9_intrinfo,
    dcb9_reqcount_set,
    dcb9_reqcount_get,
    dcb9_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb9_done_set,
    dcb9_done_get,
    dcb9_sg_set,
    dcb9_sg_get,
    dcb9_chain_set,
    dcb9_chain_get,
    dcb9_reload_set,
    dcb9_reload_get,
    dcb9_desc_intr_set,
    dcb9_desc_intr_get,
    dcb9_tx_l2pbm_set,
    dcb9_tx_utpbm_set,
    dcb9_tx_l3pbm_set,
    dcb9_tx_crc_set,
    dcb9_tx_cos_set,
    dcb9_tx_destmod_set,
    dcb9_tx_destport_set,
    dcb9_tx_opcode_set,
    dcb9_tx_srcmod_set,
    dcb9_tx_srcport_set,
    dcb9_tx_prio_set,
    dcb9_tx_pfm_set,
    dcb12_rx_untagged_get,
    dcb15_rx_crc_get,
    dcb15_rx_cos_get,
    dcb15_rx_destmod_get,
    dcb15_rx_destport_get,
    dcb15_rx_opcode_get,
    dcb15_rx_classtag_get,
    dcb15_rx_matchrule_get,
    dcb9_rx_start_get,
    dcb9_rx_end_get,
    dcb9_rx_error_get,
    dcb15_rx_prio_get,
    dcb15_rx_reason_get,
    dcb15_rx_reason_hi_get,
    dcb15_rx_ingport_get,
    dcb15_rx_srcport_get,
    dcb15_rx_srcmod_get,
    dcb15_rx_mcast_get,
    dcb15_rx_vclabel_get,
    dcb15_rx_mirror_get,
    dcb15_rx_timestamp_get,
    dcb15_rx_timestamp_upper_get,
    dcb9_hg_set,
    dcb9_hg_get,
    dcb9_stat_set,
    dcb9_stat_get,
    dcb9_purge_set,
    dcb9_purge_get,
    dcb9_mhp_get,
    dcb15_outer_vid_get,
    dcb15_outer_pri_get,
    dcb15_outer_cfi_get,
    dcb9_rx_outer_tag_action_get,
    dcb9_inner_vid_get,
    dcb9_inner_pri_get,
    dcb9_inner_cfi_get,
    dcb9_rx_inner_tag_action_get,
    dcb12_rx_bpdu_get,
    dcb15_rx_l3_intf_get,
    dcb9_rx_decap_tunnel_get,
    dcb15_rx_switch_drop_get,
    dcb15_olp_encap_oam_pkt_get,
    dcb15_read_ecc_error_get,
    dcb15_desc_remaining_get,
    dcb15_desc_remaining_set,
    dcb15_desc_status_get,
    dcb15_desc_status_set,
    dcb15_match_id_lo_get,
    dcb15_match_id_hi_get,
    dcb15_forwarding_type_get,
    dcb15_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb9_tx_l2pbm_get,
    dcb9_tx_utpbm_get,
    dcb9_tx_l3pbm_get,
    dcb9_tx_crc_get,
    dcb9_tx_cos_get,
    dcb9_tx_destmod_get,
    dcb9_tx_destport_get,
    dcb9_tx_opcode_get,
    dcb9_tx_srcmod_get,
    dcb9_tx_srcport_get,
    dcb9_tx_prio_get,
    dcb9_tx_pfm_get,

    dcb9_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb9_status_init,
    dcb9_xfercount_set,
    dcb9_rx_start_set,
    dcb9_rx_end_set,
    dcb9_rx_error_set,
    dcb15_rx_crc_set,
    dcb15_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_RAPTOR_SUPPORT */

#ifdef BCM_SCORPION_SUPPORT
static soc_rx_reason_t
dcb16_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL2LearnLimit,       /* Offset 27 */
    socRxReasonL2NonUnicastMiss,   /* Offset 28 */
    socRxReasonNhop,               /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid             /* Offset 31 */
};

static soc_rx_reason_t *dcb16_rx_reason_maps[] = {
    dcb16_rx_reason_map,
    NULL
};

GETFUNCERR(16, rx_timestamp)
GETFUNCERR(16, rx_timestamp_upper)
GETFUNCNULL(16, rx_switch_drop)
GETFUNCNULL(16, olp_encap_oam_pkt)
GETFUNCNULL(16, read_ecc_error)
GETFUNCNULL(16, desc_remaining)
SETFUNCNULL(16, desc_remaining)
GETFUNCNULL(16, desc_status)
SETFUNCNULL(16, desc_status)
GETFUNCNULL(16, match_id_lo)
GETFUNCNULL(16, match_id_hi)
GETFUNCNULL(16, forwarding_type)
GETFUNCNULL(16, forwarding_zone_id)

dcb_op_t dcb16_op = {
    16,
    sizeof(dcb16_t),
    dcb16_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb14_init,
    dcb14_addtx,
    dcb14_addrx,
    dcb14_intrinfo,
    dcb14_reqcount_set,
    dcb14_reqcount_get,
    dcb14_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb14_done_set,
    dcb14_done_get,
    dcb14_sg_set,
    dcb14_sg_get,
    dcb14_chain_set,
    dcb14_chain_get,
    dcb14_reload_set,
    dcb14_reload_get,
    dcb9_desc_intr_set,
    dcb9_desc_intr_get,
    dcb14_tx_l2pbm_set,
    dcb14_tx_utpbm_set,
    dcb14_tx_l3pbm_set,
    dcb14_tx_crc_set,
    dcb14_tx_cos_set,
    dcb14_tx_destmod_set,
    dcb14_tx_destport_set,
    dcb14_tx_opcode_set,
    dcb14_tx_srcmod_set,
    dcb14_tx_srcport_set,
    dcb14_tx_prio_set,
    dcb14_tx_pfm_set,
    dcb14_rx_untagged_get,
    dcb14_rx_crc_get,
    dcb14_rx_cos_get,
    dcb14_rx_destmod_get,
    dcb14_rx_destport_get,
    dcb14_rx_opcode_get,
    dcb9_rx_classtag_get,
    dcb14_rx_matchrule_get,
    dcb14_rx_start_get,
    dcb14_rx_end_get,
    dcb14_rx_error_get,
    dcb14_rx_prio_get,
    dcb14_rx_reason_get,
    dcb9_rx_reason_hi_get,
    dcb14_rx_ingport_get,
    dcb14_rx_srcport_get,
    dcb14_rx_srcmod_get,
    dcb9_rx_mcast_get,
    dcb14_rx_vclabel_get,
    dcb14_rx_mirror_get,
    dcb16_rx_timestamp_get,
    dcb16_rx_timestamp_upper_get,
    dcb14_hg_set,
    dcb14_hg_get,
    dcb14_stat_set,
    dcb14_stat_get,
    dcb14_purge_set,
    dcb14_purge_get,
    dcb9_mhp_get,
    dcb14_outer_vid_get,
    dcb14_outer_pri_get,
    dcb14_outer_cfi_get,
    dcb14_rx_outer_tag_action_get,
    dcb9_inner_vid_get,
    dcb9_inner_pri_get,
    dcb9_inner_cfi_get,
    dcb14_rx_inner_tag_action_get,
    dcb14_rx_bpdu_get,
    dcb14_rx_l3_intf_get,
    dcb9_rx_decap_tunnel_get,
    dcb16_rx_switch_drop_get,
    dcb16_olp_encap_oam_pkt_get,
    dcb16_read_ecc_error_get,
    dcb16_desc_remaining_get,
    dcb16_desc_remaining_set,
    dcb16_desc_status_get,
    dcb16_desc_status_set,
    dcb16_match_id_lo_get,
    dcb16_match_id_hi_get,
    dcb16_forwarding_type_get,
    dcb16_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb14_tx_l2pbm_get,
    dcb14_tx_utpbm_get,
    dcb14_tx_l3pbm_get,
    dcb14_tx_crc_get,
    dcb14_tx_cos_get,
    dcb14_tx_destmod_get,
    dcb14_tx_destport_get,
    dcb14_tx_opcode_get,
    dcb14_tx_srcmod_get,
    dcb14_tx_srcport_get,
    dcb14_tx_prio_get,
    dcb14_tx_pfm_get,

    dcb14_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb14_status_init,
    dcb14_xfercount_set,
    dcb14_rx_start_set,
    dcb14_rx_end_set,
    dcb14_rx_error_set,
    dcb14_rx_crc_set,
    dcb14_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_SCORPION_SUPPORT */

#if defined(BCM_HAWKEYE_SUPPORT)

static soc_rx_reason_t
dcb17_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonL2LearnLimit,       /* Offset 25 */
    socRxReasonTimeSync,           /* Offset 26 */
    socRxReasonEAVData,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid             /* Offset 31 */
};

static soc_rx_reason_t *dcb17_rx_reason_maps[] = {
    dcb17_rx_reason_map,
    NULL,
};

GETFUNCNULL(17, rx_switch_drop)
GETFUNCNULL(17, olp_encap_oam_pkt)
GETFUNCNULL(17, read_ecc_error)
GETFUNCNULL(17, desc_remaining)
SETFUNCNULL(17, desc_remaining)
GETFUNCNULL(17, desc_status)
SETFUNCNULL(17, desc_status)
GETFUNCNULL(17, match_id_lo)
GETFUNCNULL(17, match_id_hi)
GETFUNCNULL(17, forwarding_type)
GETFUNCNULL(17, forwarding_zone_id)

dcb_op_t dcb17_op = {
    17,
    sizeof(dcb17_t),
    dcb17_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb9_init,
    dcb9_addtx,
    dcb9_addrx,
    dcb9_intrinfo,
    dcb9_reqcount_set,
    dcb9_reqcount_get,
    dcb9_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb9_done_set,
    dcb9_done_get,
    dcb9_sg_set,
    dcb9_sg_get,
    dcb9_chain_set,
    dcb9_chain_get,
    dcb9_reload_set,
    dcb9_reload_get,
    dcb9_desc_intr_set,
    dcb9_desc_intr_get,
    dcb9_tx_l2pbm_set,
    dcb9_tx_utpbm_set,
    dcb9_tx_l3pbm_set,
    dcb9_tx_crc_set,
    dcb9_tx_cos_set,
    dcb9_tx_destmod_set,
    dcb9_tx_destport_set,
    dcb9_tx_opcode_set,
    dcb9_tx_srcmod_set,
    dcb9_tx_srcport_set,
    dcb9_tx_prio_set,
    dcb9_tx_pfm_set,
    dcb17_rx_untagged_get,
    dcb17_rx_crc_get,
    dcb17_rx_cos_get,
    dcb17_rx_destmod_get,
    dcb17_rx_destport_get,
    dcb17_rx_opcode_get,
    dcb17_rx_classtag_get,
    dcb15_rx_matchrule_get,
    dcb9_rx_start_get,
    dcb9_rx_end_get,
    dcb9_rx_error_get,
    dcb17_rx_prio_get,
    dcb17_rx_reason_get,
    dcb15_rx_reason_hi_get,
    dcb15_rx_ingport_get,
    dcb17_rx_srcport_get,
    dcb17_rx_srcmod_get,
    dcb17_rx_mcast_get,
    dcb17_rx_vclabel_get,
    dcb15_rx_mirror_get,
    dcb17_rx_timestamp_get,
    dcb17_rx_timestamp_upper_get,
    dcb9_hg_set,
    dcb9_hg_get,
    dcb9_stat_set,
    dcb9_stat_get,
    dcb9_purge_set,
    dcb9_purge_get,
    dcb9_mhp_get,
    dcb15_outer_vid_get,
    dcb15_outer_pri_get,
    dcb15_outer_cfi_get,
    dcb9_rx_outer_tag_action_get,
    dcb9_inner_vid_get,
    dcb9_inner_pri_get,
    dcb9_inner_cfi_get,
    dcb9_rx_inner_tag_action_get,
    dcb12_rx_bpdu_get,
    dcb15_rx_l3_intf_get,
    dcb9_rx_decap_tunnel_get,
    dcb17_rx_switch_drop_get,
    dcb17_olp_encap_oam_pkt_get,
    dcb17_read_ecc_error_get,
    dcb17_desc_remaining_get,
    dcb17_desc_remaining_set,
    dcb17_desc_status_get,
    dcb17_desc_status_set,
    dcb17_match_id_lo_get,
    dcb17_match_id_hi_get,
    dcb17_forwarding_type_get,
    dcb17_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb9_tx_l2pbm_get,
    dcb9_tx_utpbm_get,
    dcb9_tx_l3pbm_get,
    dcb9_tx_crc_get,
    dcb9_tx_cos_get,
    dcb9_tx_destmod_get,
    dcb9_tx_destport_get,
    dcb9_tx_opcode_get,
    dcb9_tx_srcmod_get,
    dcb9_tx_srcport_get,
    dcb9_tx_prio_get,
    dcb9_tx_pfm_get,

    dcb9_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb9_status_init,
    dcb9_xfercount_set,
    dcb9_rx_start_set,
    dcb9_rx_end_set,
    dcb9_rx_error_set,
    dcb15_rx_crc_set,
    dcb15_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_HAWKEYE_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT)
GETFUNCERR(18, rx_timestamp)
GETFUNCERR(18, rx_timestamp_upper)
GETFUNCFIELD(18, rx_matchrule, match_rule)
GETFUNCNULL(18, rx_switch_drop)
GETFUNCNULL(18, olp_encap_oam_pkt)
GETFUNCNULL(18, read_ecc_error)
GETFUNCNULL(18, desc_remaining)
SETFUNCNULL(18, desc_remaining)
GETFUNCNULL(18, desc_status)
SETFUNCNULL(18, desc_status)
GETFUNCNULL(18, match_id_lo)
GETFUNCNULL(18, match_id_hi)
GETFUNCNULL(18, forwarding_type)
GETFUNCNULL(18, forwarding_zone_id)

dcb_op_t dcb18_op = {
    18,
    sizeof(dcb18_t),
    dcb12_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb9_init,
    dcb9_addtx,
    dcb9_addrx,
    dcb9_intrinfo,
    dcb9_reqcount_set,
    dcb9_reqcount_get,
    dcb9_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb9_done_set,
    dcb9_done_get,
    dcb9_sg_set,
    dcb9_sg_get,
    dcb9_chain_set,
    dcb9_chain_get,
    dcb9_reload_set,
    dcb9_reload_get,
    dcb9_desc_intr_set,
    dcb9_desc_intr_get,
    dcb9_tx_l2pbm_set,
    dcb9_tx_utpbm_set,
    dcb9_tx_l3pbm_set,
    dcb9_tx_crc_set,
    dcb9_tx_cos_set,
    dcb9_tx_destmod_set,
    dcb9_tx_destport_set,
    dcb9_tx_opcode_set,
    dcb9_tx_srcmod_set,
    dcb9_tx_srcport_set,
    dcb9_tx_prio_set,
    dcb9_tx_pfm_set,
    dcb12_rx_untagged_get,
    dcb15_rx_crc_get,
    dcb15_rx_cos_get,
    dcb15_rx_destmod_get,
    dcb15_rx_destport_get,
    dcb15_rx_opcode_get,
    dcb15_rx_classtag_get,
    dcb18_rx_matchrule_get,
    dcb9_rx_start_get,
    dcb9_rx_end_get,
    dcb9_rx_error_get,
    dcb15_rx_prio_get,
    dcb15_rx_reason_get,
    dcb15_rx_reason_hi_get,
    dcb15_rx_ingport_get,
    dcb15_rx_srcport_get,
    dcb15_rx_srcmod_get,
    dcb15_rx_mcast_get,
    dcb15_rx_vclabel_get,
    dcb15_rx_mirror_get,
    dcb18_rx_timestamp_get,
    dcb18_rx_timestamp_upper_get,
    dcb9_hg_set,
    dcb9_hg_get,
    dcb9_stat_set,
    dcb9_stat_get,
    dcb9_purge_set,
    dcb9_purge_get,
    dcb9_mhp_get,
    dcb15_outer_vid_get,
    dcb15_outer_pri_get,
    dcb15_outer_cfi_get,
    dcb9_rx_outer_tag_action_get,
    dcb9_inner_vid_get,
    dcb9_inner_pri_get,
    dcb9_inner_cfi_get,
    dcb9_rx_inner_tag_action_get,
    dcb12_rx_bpdu_get,
    dcb15_rx_l3_intf_get,
    dcb9_rx_decap_tunnel_get,
    dcb18_rx_switch_drop_get,
    dcb18_olp_encap_oam_pkt_get,
    dcb18_read_ecc_error_get,
    dcb18_desc_remaining_get,
    dcb18_desc_remaining_set,
    dcb18_desc_status_get,
    dcb18_desc_status_set,
    dcb18_match_id_lo_get,
    dcb18_match_id_hi_get,
    dcb18_forwarding_type_get,
    dcb18_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb9_tx_l2pbm_get,
    dcb9_tx_utpbm_get,
    dcb9_tx_l3pbm_get,
    dcb9_tx_crc_get,
    dcb9_tx_cos_get,
    dcb9_tx_destmod_get,
    dcb9_tx_destport_get,
    dcb9_tx_opcode_get,
    dcb9_tx_srcmod_get,
    dcb9_tx_srcport_get,
    dcb9_tx_prio_get,
    dcb9_tx_pfm_get,

    dcb9_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb9_status_init,
    dcb9_xfercount_set,
    dcb9_rx_start_set,
    dcb9_rx_end_set,
    dcb9_rx_error_set,
    dcb15_rx_crc_set,
    dcb15_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_RAPTOR_SUPPORT */
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
/*
 * DCB Type 19 Support
 */
static soc_rx_reason_t
dcb19_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL2LearnLimit,       /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonMplsCtrlWordError,  /* Offset 35 */
    socRxReasonIpfixRateViolation, /* Offset 36 */
    socRxReasonWlanDot1xDrop,      /* Offset 37 */
    socRxReasonWlanSlowpath,       /* Offset 38 */
    socRxReasonWlanClientError,    /* Offset 39 */
    socRxReasonEncapHigigError,    /* Offset 40 */
    socRxReasonTimeSync,           /* Offset 41 */
    socRxReasonOAMSlowpath,        /* Offset 42 */
    socRxReasonOAMError,           /* Offset 43 */
    socRxReasonL3AddrBindFail,     /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t
dcb19_rx_egr_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonStp,                /* Offset 1 */
    socRxReasonVlanTranslate,      /* Offset 2 new */
    socRxReasonTunnelError,        /* Offset 3 */
    socRxReasonIpmc,               /* Offset 4 */
    socRxReasonL3HeaderError,      /* Offset 5 */
    socRxReasonTtl,                /* Offset 6 */
    socRxReasonL2MtuFail,          /* Offset 7 */
    socRxReasonHigigHdrError,      /* Offset 8 */
    socRxReasonSplitHorizon,       /* Offset 9 */
    socRxReasonL2Move,             /* Offset 10 */
    socRxReasonFilterMatch,        /* Offset 11 */
    socRxReasonL2SourceMiss,       /* Offset 12 */
    socRxReasonInvalid,            /* Offset 13 */
    socRxReasonInvalid,            /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t *dcb19_rx_reason_maps[] = {
    dcb19_rx_reason_map,
    dcb19_rx_egr_reason_map,
    NULL
};


/*
 * Function:
 *      dcb19_rx_reason_map_get
 * Purpose:
 *      Return the RX reason map for a DCB type 19.
 * Parameters:
 *      dcb_op - DCB operations
 *      dcb    - dma control block
 * Returns:
 *      RX reason map pointer
 */
static soc_rx_reason_t *
dcb19_rx_reason_map_get(dcb_op_t *dcb_op, dcb_t *dcb)
{
    dcb19_t *d = (dcb19_t *)dcb;

    if (d->egr_cpu_copy) {
        return dcb19_rx_egr_reason_map;
    }

    return dcb19_rx_reason_map;
}

static _shr_rx_decap_tunnel_t dcb19_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_ACCESS_SVP,
    _SHR_RX_DECAP_MIM,
    _SHR_RX_DECAP_WTP2AC,
    _SHR_RX_DECAP_AC2AC,
    _SHR_RX_DECAP_AMT,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_L2MPLS_1LABEL,
    _SHR_RX_DECAP_L2MPLS_2LABEL,
    _SHR_RX_DECAP_L2MPLS_1LABELCW,
    _SHR_RX_DECAP_L2MPLS_2LABELCW,
    _SHR_RX_DECAP_L3MPLS_1LABEL,
    _SHR_RX_DECAP_L3MPLS_2LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABELCW,
    _SHR_RX_DECAP_L3MPLS_2LABELCW
};
static uint32 dcb19_rx_decap_tunnel_get(dcb_t *dcb) {
    dcb19_t *d = (dcb19_t *)dcb;
    uint32 decap_tunnel_type = d->decap_tunnel_type;
    /* Check for array bounds */
    if ( (decap_tunnel_type <
    (sizeof (dcb19_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb19_rx_decap_tunnel_map[decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}

static void
dcb19_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[0] = d[1] = d[2] = d[3] = d[4] = 0;
    d[5] = d[6] = d[7] = d[8] = d[9] = d[10] = 0;
    d[11] = d[12] = d[13] = d[14] = d[15] = 0;
}

static int
dcb19_addtx(dv_t *dv, sal_vaddr_t addr, uint32 count,
            pbmp_t l2pbm, pbmp_t utpbm, pbmp_t l3pbm, uint32 flags, uint32 *hgh)
{
    dcb19_t     *d;     /* DCB */
    uint32      *di;    /* DCB integer pointer */
    uint32      paddr;  /* Packet buffer physical address */
    int         unaligned;
    int         unaligned_bytes;
    uint8       *unaligned_buffer;
    uint8       *aligned_buffer;

    d = (dcb19_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (addr) {
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    } else {
        paddr = 0;
    }

    if (dv->dv_vcnt > 0 && (dv->dv_flags & DV_F_COMBINE_DCB) &&
        (d[-1].c_sg != 0) &&
        (d[-1].addr + d[-1].c_count) == paddr &&
        d[-1].c_count + count <= DCB_MAX_REQCOUNT) {
        d[-1].c_count += count;
        return dv->dv_cnt - dv->dv_vcnt;
    }

    /*
     * A few chip revisions do not support 128 byte PCI bursts
     * correctly if the address is not word-aligned. In case
     * we encounter an unaligned address, we consume an extra
     * DCB to correct the alignment.
     */
    do {
        if (dv->dv_vcnt >= dv->dv_cnt) {
            return SOC_E_FULL;
        }
        if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
            d[-1].c_chain = 1;
        }

        di = (uint32 *)d;
        di[0] = di[1] = di[2] = di[3] = di[4] = 0;
        di[5] = di[6] = di[7] = di[8] = di[9] = di[10] = 0;
        di[11] = di[12] = di[13] = di[14] = di[15] = 0;

        d->addr = paddr;
        d->c_count = count;
        d->c_sg = 1;

        d->c_stat = 1;
        d->c_purge = SOC_DMA_PURGE_GET(flags);
        if (SOC_DMA_HG_GET(flags)) {
            soc_higig_hdr_t *mh = (soc_higig_hdr_t *)hgh;
            if (mh->overlay1.start == SOC_HIGIG2_START) {
                d->mh3 = soc_ntohl(hgh[3]);
            }
            d->c_hg = 1;
            d->mh0 = soc_ntohl(hgh[0]);
            d->mh1 = soc_ntohl(hgh[1]);
            d->mh2 = soc_ntohl(hgh[2]);
            d->mh3 = soc_ntohl(hgh[3]);
        }

        unaligned = 0;
        if (soc_feature(dv->dv_unit, soc_feature_pkt_tx_align)) {
            if (paddr & 0x3) {
                unaligned_bytes = 4 - (paddr & 0x3);
                unaligned_buffer = (uint8 *)addr;
                aligned_buffer = SOC_DV_TX_ALIGN(dv, dv->dv_vcnt);
                aligned_buffer[0] = unaligned_buffer[0];
                aligned_buffer[1] = unaligned_buffer[1];
                aligned_buffer[2] = unaligned_buffer[2];
                d->addr = soc_cm_l2p(dv->dv_unit, aligned_buffer);
                if (count > 3) {
                    d->c_count = unaligned_bytes;
                    paddr += unaligned_bytes;
                    count -= unaligned_bytes;
                    unaligned = 1;
                }
            }
        }

        dv->dv_vcnt += 1;

        d = (dcb19_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    } while (unaligned);

    return dv->dv_cnt - dv->dv_vcnt;
}

static int
dcb19_addrx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb19_t     *d;     /* DCB */
    uint32      *di;    /* DCB integer pointer */

    d = (dcb19_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = 0;
    di[5] = di[6] = di[7] = di[8] = di[9] = di[10] = 0;
    di[11] = di[12] = di[13] = di[14] = di[15] = 0;

    if (addr) {
        d->addr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    }
    d->c_count = count;
    d->c_sg = 1;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static uint32
dcb19_intrinfo(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb19_t      *d = (dcb19_t *)dcb;     /*  DCB */
    uint32      f;                      /* SOC_DCB_INFO_* flags */

    if (!d->done) {
        return 0;
    }
    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }
    *count = d->count;
    return f;
}

static uint32
dcb19_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb19_t *d = (dcb19_t *)dcb;

    COMPILER_REFERENCE(dt_mode);

    return (ingport_is_hg ?
            ((d->itag_status) ? 0 : 2) :
            ((d->itag_status & 0x2) ?
             ((d->itag_status & 0x1) ? 0 : 2) :
             ((d->itag_status & 0x1) ? 1 : 3)));
}

SETFUNCFIELD(19, reqcount, c_count, uint32 count, count)
GETFUNCFIELD(19, reqcount, c_count)
GETFUNCFIELD(19, xfercount, count)
/* addr_set, addr_get, paddr_get - Same as DCB 0 */
SETFUNCFIELD(19, done, done, int val, val ? 1 : 0)
GETFUNCFIELD(19, done, done)
SETFUNCFIELD(19, sg, c_sg, int val, val ? 1 : 0)
GETFUNCFIELD(19, sg, c_sg)
SETFUNCFIELD(19, chain, c_chain, int val, val ? 1 : 0)
GETFUNCFIELD(19, chain, c_chain)
SETFUNCFIELD(19, reload, c_reload, int val, val ? 1 : 0)
GETFUNCFIELD(19, reload, c_reload)
SETFUNCERR(19, desc_intr, int)
GETFUNCERR(19, desc_intr)
SETFUNCERR(19, tx_l2pbm, pbmp_t)
SETFUNCERR(19, tx_utpbm, pbmp_t)
SETFUNCERR(19, tx_l3pbm, pbmp_t)
SETFUNCERR(19, tx_crc, int)
SETFUNCERR(19, tx_cos, int)
SETFUNCERR(19, tx_destmod, uint32)
SETFUNCERR(19, tx_destport, uint32)
SETFUNCERR(19, tx_opcode, uint32)
SETFUNCERR(19, tx_srcmod, uint32)
SETFUNCERR(19, tx_srcport, uint32)
SETFUNCERR(19, tx_prio, uint32)
SETFUNCERR(19, tx_pfm, uint32)
GETFUNCFIELD(19, rx_start, start)
GETFUNCFIELD(19, rx_end, end)
GETFUNCFIELD(19, rx_error, error)
GETFUNCFIELD(19, rx_cos, cpu_cos)
/* Fields extracted from MH/PBI */
GETHG2FUNCFIELD(19, rx_destmod, dst_mod)
GETHG2FUNCFIELD(19, rx_destport, dst_port)
GETHG2FUNCFIELD(19, rx_srcmod, src_mod)
GETHG2FUNCFIELD(19, rx_srcport, src_port)
GETHG2FUNCFIELD(19, rx_opcode, opcode)
GETHG2FUNCFIELD(19, rx_prio, vlan_pri) /* outer_pri */
GETHG2FUNCEXPR(19, rx_mcast, ((h->ppd_overlay1.dst_mod << 8) |
                              (h->ppd_overlay1.dst_port)))
GETHG2FUNCEXPR(19, rx_vclabel, ((h->ppd_overlay1.vc_label_19_16 << 16) |
                              (h->ppd_overlay1.vc_label_15_8 << 8) |
                              (h->ppd_overlay1.vc_label_7_0)))
GETHG2FUNCEXPR(19, rx_classtag, (h->ppd_overlay1.ppd_type != 1 ? 0 :
                                 (h->ppd_overlay2.ctag_hi << 8) |
                                 (h->ppd_overlay2.ctag_lo)))
GETFUNCFIELD(19, rx_matchrule, match_rule)
GETFUNCFIELD(19, rx_reason, reason)
GETFUNCFIELD(19, rx_reason_hi, reason_hi)
GETFUNCFIELD(19, rx_ingport, srcport)
GETFUNCEXPR(19, rx_mirror, ((d->imirror) | (d->emirror)))
GETFUNCFIELD(19, rx_timestamp, timestamp)
GETFUNCERR(19, rx_timestamp_upper)
SETFUNCFIELD(19, hg, c_hg, uint32 hg, hg)
GETFUNCFIELD(19, hg, c_hg)
SETFUNCFIELD(19, stat, c_stat, uint32 stat, stat)
GETFUNCFIELD(19, stat, c_stat)
SETFUNCFIELD(19, purge, c_purge, uint32 purge, purge)
GETFUNCFIELD(19, purge, c_purge)
GETPTREXPR(19, mhp, &(d->mh0))
GETFUNCFIELD(19, outer_vid, outer_vid)
GETFUNCFIELD(19, outer_pri, outer_pri)
GETFUNCFIELD(19, outer_cfi, outer_cfi)
GETFUNCFIELD(19, rx_outer_tag_action, otag_action)
GETFUNCFIELD(19, inner_vid, inner_vid)
GETFUNCFIELD(19, inner_pri, inner_pri)
GETFUNCFIELD(19, inner_cfi, inner_cfi)
GETFUNCFIELD(19, rx_inner_tag_action, itag_action)
GETFUNCFIELD(19, rx_bpdu, bpdu)
GETFUNCEXPR(19, rx_l3_intf, ((d->replicated) ? (d->l3_intf) :
                             (((d->l3_intf) & 0x3fff) +
                              _SHR_L3_EGRESS_IDX_MIN)))
GETFUNCNULL(19, rx_switch_drop)
GETFUNCNULL(19, olp_encap_oam_pkt)
GETFUNCNULL(19, read_ecc_error)
GETFUNCNULL(19, desc_remaining)
SETFUNCNULL(19, desc_remaining)
GETFUNCNULL(19, desc_status)
SETFUNCNULL(19, desc_status)
GETFUNCNULL(19, match_id_lo)
GETFUNCNULL(19, match_id_hi)
GETFUNCNULL(19, forwarding_type)
GETFUNCNULL(19, forwarding_zone_id)

#ifdef BROADCOM_DEBUG
GETFUNCERR(19, tx_l2pbm) 
GETFUNCERR(19, tx_utpbm) 
GETFUNCERR(19, tx_l3pbm)
GETFUNCERR(19, tx_crc) 
GETFUNCERR(19, tx_cos)
GETFUNCERR(19, tx_destmod)
GETFUNCERR(19, tx_destport)
GETFUNCERR(19, tx_opcode) 
GETFUNCERR(19, tx_srcmod)
GETFUNCERR(19, tx_srcport)
GETFUNCERR(19, tx_prio)
GETFUNCERR(19, tx_pfm)
#endif /* BROADCOM_DEBUG */

static uint32 dcb19_rx_crc_get(dcb_t *dcb) {
    return 0;
}

#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb19_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[5] = d[6] = d[7] = d[8] = d[9] = d[10] = 0;
    d[11] = d[12] = d[13] = d[14] = d[15] = 0;
}
SETFUNCFIELD(19, xfercount, count, uint32 count, count)
SETFUNCFIELD(19, rx_start, start, int val, val ? 1 : 0)
SETFUNCFIELD(19, rx_end, end, int val, val ? 1 : 0)
SETFUNCFIELD(19, rx_error, error, int val, val ? 1 : 0)
SETFUNCEXPRIGNORE(19, rx_crc, int val, ignore)
SETFUNCFIELD(19, rx_ingport, srcport, int val, val)
#endif  /* PLISIM */

#ifdef BROADCOM_DEBUG
static void
dcb19_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t *d = (dcb19_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
#if defined(LE_HOST)
    uint32  hgh[4];
    uint8 *h = (uint8 *)&hgh[0];

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    hgh[3] = soc_htonl(d->mh3);
#else
    uint8 *h = (uint8 *)&d->mh0;
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %ssg %schain %sreload %shg %sstat %spause %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %sstart %send %serror\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!"));
    }
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb))) {
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sdo_not_change_ttl %sbpdu %sl3routed %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->do_not_change_ttl ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->l3routed ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sreplicated %sl3only %soam_pkt %segr_cpu_copy %strue_egr_mirror %ssrc_hg\n"),
                 prefix,
                 d->replicated ? "" : "!",
                 d->l3only ? "" : "!",
                 d->oam_pkt ? "" : "!",
                 d->egr_cpu_copy ? "" : "!",
                 d->true_egr_mirror ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sservice_tag %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->service_tag ? "" : "!",
                 !(((soc_higig2_hdr_t *)h)->ppd_overlay1.ingress_tagged) ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d mtp_index=%d reason=%08x_%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->l3_intf,
                 d->mtp_index,
                 d->reason_hi,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d hg_type=%d mtp_index=%d decap_tunnel_type=%d\n"),
                 prefix,
                 d->match_rule,
                 d->hg_type,
                 d->mtp_index,
                 d->decap_tunnel_type
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  orig_dstport=%d inner_pri=%d inner_cfi=%d inner_vid=%d\n"),
                 prefix,
                 d->orig_dstport,
                 d->inner_pri,
                 d->inner_cfi,
                 d->inner_vid
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  hgi=%d itag_status=%d otag_action=%d itag_action=%d\n"),
                 prefix,
                 d->hgi,
                 d->itag_status,
                 d->otag_action,
                 d->itag_action
                 ));
    }
}
#endif /* BROADCOM_DEBUG */

dcb_op_t dcb19_op = {
    19,
    sizeof(dcb19_t),
    dcb19_rx_reason_maps,
    dcb19_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb19_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb19_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb19_rx_cos_get,
    dcb19_rx_destmod_get,
    dcb19_rx_destport_get,
    dcb19_rx_opcode_get,
    dcb19_rx_classtag_get,
    dcb19_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb19_rx_prio_get,
    dcb19_rx_reason_get,
    dcb19_rx_reason_hi_get,
    dcb19_rx_ingport_get,
    dcb19_rx_srcport_get,
    dcb19_rx_srcmod_get,
    dcb19_rx_mcast_get,
    dcb19_rx_vclabel_get,
    dcb19_rx_mirror_get,
    dcb19_rx_timestamp_get,
    dcb19_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb19_mhp_get,
    dcb19_outer_vid_get,
    dcb19_outer_pri_get,
    dcb19_outer_cfi_get,
    dcb19_rx_outer_tag_action_get,
    dcb19_inner_vid_get,
    dcb19_inner_pri_get,
    dcb19_inner_cfi_get,
    dcb19_rx_inner_tag_action_get,
    dcb19_rx_bpdu_get,
    dcb19_rx_l3_intf_get,
    dcb19_rx_decap_tunnel_get,
    dcb19_rx_switch_drop_get,
    dcb19_olp_encap_oam_pkt_get,
    dcb19_read_ecc_error_get,
    dcb19_desc_remaining_get,
    dcb19_desc_remaining_set,
    dcb19_desc_status_get,
    dcb19_desc_status_set,
    dcb19_match_id_lo_get,
    dcb19_match_id_hi_get,
    dcb19_forwarding_type_get,
    dcb19_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb19_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb19_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb19_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_SHADOW_SUPPORT */

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
/*
 * DCB Type 20 Support
 */

static soc_rx_reason_t
dcb20_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL2LearnLimit,       /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonMplsCtrlWordError,  /* Offset 35 */
    socRxReasonTimeSync,           /* Offset 36 */
    socRxReasonOAMSlowpath,        /* Offset 37 */
    socRxReasonOAMError,           /* Offset 38 */
    socRxReasonOAMLMDM,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t *dcb20_rx_reason_maps[] = {
    dcb20_rx_reason_map,
    NULL
};


#ifdef BROADCOM_DEBUG
static void
dcb20_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb20_t *d = (dcb20_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
#if defined(LE_HOST)
    uint32  hgh[4];
    uint8 *h = (uint8 *)&hgh[0];

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    hgh[3] = soc_htonl(d->mh3);
#else
    uint8 *h = (uint8 *)&d->mh0;
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %ssg %schain %sreload %shg %sstat %spause %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %sstart %send %serror\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!"));
    }
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb))) {
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sdo_not_change_ttl %sbpdu %sl3routed %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->do_not_change_ttl ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->l3routed ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sreplicated %sl3only %ssrc_hg im_mtp_index=%d em_mtp_index=%d\n"),
                 prefix,
                 d->replicated ? "" : "!",
                 d->l3only ? "" : "!",
                 d->src_hg ? "" : "!",
                 d->im_mtp_index,
                 d->em_mtp_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sservice_tag %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->service_tag ? "" : "!",
                 !(((soc_higig2_hdr_t *)h)->ppd_overlay1.ingress_tagged) ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d reason=%08x_%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->nhop_index,
                 d->reason_hi,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d hg_type=%d decap_tunnel_type=%d\n"),
                 prefix,
                 d->match_rule,
                 d->hg_type,
                 d->decap_tunnel_type
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  inner_pri=%d inner_cfi=%d inner_vid=%d\n"),
                 prefix,
                 d->inner_pri,
                 d->inner_cfi,
                 d->inner_vid
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  hgi=%d itag_status=%d otag_action=%d itag_action=%d\n"),
                 prefix,
                 d->hgi,
                 d->itag_status,
                 d->otag_action,
                 d->itag_action
                 ));
    }
}
#endif /* BROADCOM_DEBUG */

GETFUNCFIELD(20, rx_matchrule, match_rule)
GETFUNCFIELD(20, rx_timestamp, timestamp)
GETFUNCFIELD(20, rx_timestamp_upper, timestamp_upper)
GETFUNCNULL(20, rx_switch_drop)
GETFUNCNULL(20, olp_encap_oam_pkt)
GETFUNCNULL(20, read_ecc_error)
GETFUNCNULL(20, desc_remaining)
SETFUNCNULL(20, desc_remaining)
GETFUNCNULL(20, desc_status)
SETFUNCNULL(20, desc_status)
GETFUNCNULL(20, match_id_lo)
GETFUNCNULL(20, match_id_hi)
GETFUNCNULL(20, forwarding_type)
GETFUNCNULL(20, forwarding_zone_id)

dcb_op_t dcb20_op = {
    20,
    sizeof(dcb20_t),
    dcb20_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb19_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb19_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb19_rx_cos_get,
    dcb19_rx_destmod_get,
    dcb19_rx_destport_get,
    dcb19_rx_opcode_get,
    dcb19_rx_classtag_get,
    dcb20_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb19_rx_prio_get,
    dcb19_rx_reason_get,
    dcb19_rx_reason_hi_get,
    dcb19_rx_ingport_get,
    dcb19_rx_srcport_get,
    dcb19_rx_srcmod_get,
    dcb19_rx_mcast_get,
    dcb19_rx_vclabel_get,
    dcb19_rx_mirror_get,
    dcb20_rx_timestamp_get,
    dcb20_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb19_mhp_get,
    dcb19_outer_vid_get,
    dcb19_outer_pri_get,
    dcb19_outer_cfi_get,
    dcb19_rx_outer_tag_action_get,
    dcb19_inner_vid_get,
    dcb19_inner_pri_get,
    dcb19_inner_cfi_get,
    dcb19_rx_inner_tag_action_get,
    dcb19_rx_bpdu_get,
    dcb9_rx_l3_intf_get,
    dcb19_rx_decap_tunnel_get,
    dcb20_rx_switch_drop_get,
    dcb20_olp_encap_oam_pkt_get,
    dcb20_read_ecc_error_get,
    dcb20_desc_remaining_get,
    dcb20_desc_remaining_set,
    dcb20_desc_status_get,
    dcb20_desc_status_set,
    dcb20_match_id_lo_get,
    dcb20_match_id_hi_get,
    dcb20_forwarding_type_get,
    dcb20_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb20_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb19_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb19_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_ENDURO_SUPPORT || BCM_HURRICANE_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT)
/*
 * DCB Type 21 Support
 */

static soc_rx_reason_t
dcb21_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonMirror,             /* Offset 18 */ 
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL3AddrBindFail,     /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonMplsCtrlWordError,  /* Offset 35 */
    socRxReasonStation,            /* Offset 36 */
    socRxReasonNiv,                /* Offset 37 */
    socRxReasonNiv,                /* Offset 38 */
    socRxReasonNiv,                /* Offset 39 */
    socRxReasonEncapHigigError,    /* Offset 40 */
    socRxReasonTimeSync,           /* Offset 41 */
    socRxReasonOAMSlowpath,        /* Offset 42 */
    socRxReasonOAMError,           /* Offset 43 */
    socRxReasonTrill,              /* Offset 44 */
    socRxReasonTrill,              /* Offset 45 */
    socRxReasonTrill,              /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};
/* From FORMAT NIV_CPU_OPCODE_ENCODING */
static soc_rx_reason_t dcb21_rx_reason_niv_encoding_map[] = {
    socRxReasonNiv,                /* 0: NO_ERRORS
                                    * Base field, must match the entries above */
    socRxReasonNivPrioDrop,        /* 1:DOT1P_ADMITTANCE_DISCARD */
    socRxReasonNivInterfaceMiss,   /* 2:VIF_LOOKUP_MISS */
    socRxReasonNivRpfFail,         /* 3:RPF_LOOKUP_MISS */
    socRxReasonNivTagInvalid,      /* 4:VNTAG_FORMAT_ERROR */
    socRxReasonNivTagDrop,         /* 5:VNTAG_PRESENT_DROP */
    socRxReasonNivUntagDrop,       /* 6:VNTAG_NOT_PRESENT_DROP */
    socRxReasonInvalid,            /* Offset 7 */
    socRxReasonInvalid,            /* Offset 8 */
    socRxReasonInvalid,            /* Offset 9 */
    socRxReasonInvalid,            /* Offset 10 */
    socRxReasonInvalid,            /* Offset 11 */
    socRxReasonInvalid,            /* Offset 12 */
    socRxReasonInvalid,            /* Offset 13 */
    socRxReasonInvalid,            /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};
/* From FORMAT TRILL_CPU_OPCODE_ENCODING */
static soc_rx_reason_t dcb21_rx_reason_trill_encoding_map[] = {
    socRxReasonTrill,              /* 0:NO_ERRORS
                                    * Base field, must match the entries above */
    socRxReasonTrillInvalid,       /* 1:TRILL_HDR_ERROR */
    socRxReasonTrillMiss,          /* 2:TRILL_LOOKUP_MISS */
    socRxReasonTrillRpfFail,       /* 3:TRILL_RPF_CHECK_FAIL */
    socRxReasonTrillSlowpath,      /* 4:TRILL_SLOWPATH */
    socRxReasonTrillCoreIsIs,      /* 5:TRILL_CORE_IS_IS_PKT */
    socRxReasonTrillTtl,           /* 6:TRILL_HOP_COUNT_CHECK_FAIL */
    socRxReasonInvalid,            /* Offset 7 */
    socRxReasonInvalid,            /* Offset 8 */
    socRxReasonInvalid,            /* Offset 9 */
    socRxReasonInvalid,            /* Offset 10 */
    socRxReasonInvalid,            /* Offset 11 */
    socRxReasonInvalid,            /* Offset 12 */
    socRxReasonInvalid,            /* Offset 13 */
    socRxReasonInvalid,            /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};
static soc_rx_reason_t *dcb21_rx_reason_maps[] = {
    dcb21_rx_reason_map,
    dcb21_rx_reason_niv_encoding_map,
    dcb21_rx_reason_trill_encoding_map,
    NULL
};
static _shr_rx_decap_tunnel_t dcb21_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_ACCESS_SVP,
    _SHR_RX_DECAP_MIM,
    _SHR_RX_DECAP_L2GRE,
    _SHR_RX_DECAP_VXLAN,
    _SHR_RX_DECAP_AMT,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_TRILL,
    _SHR_RX_DECAP_L2MPLS_1LABEL,
    _SHR_RX_DECAP_L2MPLS_2LABEL,
    _SHR_RX_DECAP_L2MPLS_1LABELCW,
    _SHR_RX_DECAP_L2MPLS_2LABELCW,
    _SHR_RX_DECAP_L3MPLS_1LABEL,
    _SHR_RX_DECAP_L3MPLS_2LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABELCW,
    _SHR_RX_DECAP_L3MPLS_2LABELCW
}; 
static uint32 dcb21_rx_decap_tunnel_get(dcb_t *dcb) {
    dcb21_t *d = (dcb21_t *)dcb;
    /* Check for array bounds */
    if ( (d->decap_tunnel_type <  
    (sizeof (dcb21_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb21_rx_decap_tunnel_map[d->decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}
GETFUNCEXPR(21, rx_mirror, (d->reason & (1 << 18)))
GETFUNCEXPR(21, rx_l3_intf, ((d->replicated) ? (d->l3_intf) :
                             (((d->l3_intf) & 0x4000) ?  /* TD+ NHI */
                      (((d->l3_intf) & 0x3fff) + _SHR_L3_EGRESS_IDX_MIN) :
                              ((d->l3_intf) & 0x2000) ?  /* TD NHI */
                      (((d->l3_intf) & 0x1fff) + _SHR_L3_EGRESS_IDX_MIN) :
                              (d->l3_intf))))
GETFUNCNULL(21, rx_switch_drop)
GETFUNCNULL(21, olp_encap_oam_pkt)
GETFUNCNULL(21, read_ecc_error)
GETFUNCNULL(21, desc_remaining)
SETFUNCNULL(21, desc_remaining)
GETFUNCNULL(21, desc_status)
SETFUNCNULL(21, desc_status)
GETFUNCNULL(21, match_id_lo)
GETFUNCNULL(21, match_id_hi)
GETFUNCNULL(21, forwarding_type)
GETFUNCNULL(21, forwarding_zone_id)


static void
dcb21_rx_reasons_get(dcb_op_t *dcb_op, dcb_t *dcb, soc_rx_reasons_t *reasons)
{
    soc_rx_reason_t *map, *encoding_map;
    uint32 opcode[2], encoding[3];
    uint32 bit_val;
    int word_idx, bit_idx, opcode_idx, map_idx, word_count;
    int enc_bit = 0;

    SOC_RX_REASON_CLEAR_ALL(*reasons);

    opcode[0] = dcb_op->rx_reason_get(dcb);
    opcode[1] = dcb_op->rx_reason_hi_get(dcb);
    word_count = 2;
    sal_memset(encoding, 0, sizeof(encoding));
    map = dcb_op->rx_reason_map_get(dcb_op, dcb);
    for (word_idx = 0; word_idx < word_count; word_idx++) {
        for (bit_idx = 0; bit_idx < 32; bit_idx++) {
            opcode_idx = word_idx * 32 + bit_idx;
            bit_val = opcode[word_idx] & (1 << bit_idx) ? 1 : 0;
            for (map_idx = 0; ; map_idx++) {
                encoding_map = dcb_op->rx_reason_maps[1 + map_idx];
                if (encoding_map == NULL) {
                    break;
                }
                if (map[opcode_idx] != encoding_map[0]) {
                    continue;
                }
                if (opcode_idx == 0 || map[opcode_idx - 1] != encoding_map[0]) {
                    enc_bit = 0;
                }
                encoding[map_idx] |= bit_val << enc_bit;
                enc_bit++;
                break;
            }
            if (encoding_map == NULL && bit_val) {
                SOC_RX_REASON_SET(*reasons, map[opcode_idx]);
            }
        }
    }

    for (map_idx = 0; ; map_idx++) {
        encoding_map = dcb_op->rx_reason_maps[1 + map_idx];
        if (encoding_map == NULL) {
            break;
        }
        if (encoding[map_idx] != socRxReasonInvalid) {
            SOC_RX_REASON_SET(*reasons, encoding_map[encoding[map_idx]]);
        }
    }

    /* BPDU bit should be a reason, paste it in here */
    if (dcb_op->rx_bpdu_get(dcb)) {
        SOC_RX_REASON_SET(*reasons, socRxReasonBpdu);
    }

    return;
}

#ifdef BROADCOM_DEBUG
static void
dcb21_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb21_t     *d = (dcb21_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
#if defined(LE_HOST)
    uint32      hgh[4];
    uint8       *h = (uint8 *)&hgh[0];

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    hgh[3] = soc_htonl(d->mh3);
#else
    uint8       *h = (uint8 *)&d->mh0;
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %sstat %spause "
             " %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }

    dcb0_reason_dump(unit, dcb, prefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%schg_tos %sregen_crc %schg_ecn %smcq %svfi_valid"
             " %sdvp_nhi_sel\n"),
             prefix,
             d->chg_tos ? "" : "!",
             d->regen_crc ? "" : "!",
             d->chg_ecn ? "" : "!",
             d->mcq ? "" : "!",
             d->vfi_valid ? "" : "!",
             d->dvp_nhi_sel ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sservice_tag %sswitch_pkt %shg_type %ssrc_hg\n"),
             prefix,
             d->service_tag ? "" : "!",
             d->switch_pkt ? "" : "!",
             d->hg_type ? "" : "!",
             d->src_hg ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sl3routed %sl3only %sreplicated %sdo_not_change_ttl"
             " %sbpdu\n"),
             prefix,
             d->l3routed ? "" : "!",
             d->l3only ? "" : "!",
             d->replicated ? "" : "!",
             d->do_not_change_ttl ? "" : "!",
             d->bpdu ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%soam_pkt %seh_tm %shg2_ext_hdr %sall_switch_drop\n"),
             prefix,
             d->oam_pkt ? "" : "!",
             d->eh_tm ? "" : "!",
             d->hg2_ext_hdr ? "" : "!",
             d->all_switch_drop ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\treason=%08x_%08x timestamp=%d srcport=%d\n"),
             prefix,
             d->reason_hi,
             d->reason,
             d->timestamp,
             d->srcport));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tcpu_cos=%d uc_cos=%d mc_cos1=%d mc_cos2=%d hgi=%d\n"),
             prefix,
             d->cpu_cos,
             d->uc_cos,
             d->mc_cos1,
             d->mc_cos2,
             d->hgi));
    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d otag_action=%d"
             " vntag_action=%d\n"),
             prefix,
             d->outer_vid,
             d->outer_cfi,
             d->outer_pri,
             d->otag_action,
             d->vntag_action));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tinner_vid=%d inner_cfi=%d inner_pri=%d itag_action=%d"
             " itag_status=%d\n"),
             prefix,
             d->inner_vid,
             d->inner_cfi,
             d->inner_pri,
             d->itag_action,
             d->itag_status));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdscp=%d ecn=%d\n"),
             prefix,
             d->dscp,
             d->ecn));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tnext_pass_cp=%d eh_seg_sel=%d eh_tag_type=%d "
             " eh_queue_tag=%d\n"),
             prefix,
             d->next_pass_cp,
             d->eh_seg_sel,
             d->eh_tag_type,
             d->eh_queue_tag));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdecap_tunnel_type=%d vfi=%d l3_intf=%d match_rule=%d"
             " mtp_ind=%d\n"),
             prefix,
             d->decap_tunnel_type,
             d->vfi,
             d->l3_intf,
             d->match_rule,
             d->mtp_index));
}
#endif /* BROADCOM_DEBUG */

dcb_op_t dcb21_op = {
    21,
    sizeof(dcb21_t),
    dcb21_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb21_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb19_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb19_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb19_rx_cos_get,
    dcb19_rx_destmod_get,
    dcb19_rx_destport_get,
    dcb19_rx_opcode_get,
    dcb19_rx_classtag_get,
    dcb19_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb19_rx_prio_get,
    dcb19_rx_reason_get,
    dcb19_rx_reason_hi_get,
    dcb19_rx_ingport_get,
    dcb19_rx_srcport_get,
    dcb19_rx_srcmod_get,
    dcb19_rx_mcast_get,
    dcb19_rx_vclabel_get,
    dcb21_rx_mirror_get,
    dcb19_rx_timestamp_get,
    dcb19_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb19_mhp_get,
    dcb19_outer_vid_get,
    dcb19_outer_pri_get,
    dcb19_outer_cfi_get,
    dcb19_rx_outer_tag_action_get,
    dcb19_inner_vid_get,
    dcb19_inner_pri_get,
    dcb19_inner_cfi_get,
    dcb19_rx_inner_tag_action_get,
    dcb19_rx_bpdu_get,
    dcb21_rx_l3_intf_get,
    dcb21_rx_decap_tunnel_get,
    dcb21_rx_switch_drop_get,
    dcb21_olp_encap_oam_pkt_get,
    dcb21_read_ecc_error_get,
    dcb21_desc_remaining_get,
    dcb21_desc_remaining_set,
    dcb21_desc_status_get,
    dcb21_desc_status_set,
    dcb21_match_id_lo_get,
    dcb21_match_id_hi_get,
    dcb21_forwarding_type_get,
    dcb21_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb21_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb19_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb19_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_SHADOW_SUPPORT)
/*
 * DCB Type 22 Support
 */

static soc_rx_reason_t
dcb22_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonFilterMatch,        /* Offset 1 */
    socRxReasonL3HeaderError,      /* Offset 2 */
    socRxReasonProtocol,           /* Offset 3 */
    socRxReasonDosAttack,          /* Offset 4 */
    socRxReasonMartianAddr,        /* Offset 5 */
    socRxReasonParityError,        /* Offset 6 */
    socRxReasonHigigHdrError,      /* Offset 7 */
    socRxReasonTimeSync,           /* Offset 8 */
    socRxReasonHigigHdrError,      /* Offset 9 */
    socRxReasonInvalid,            /* Offset 10 */
    socRxReasonInvalid,            /* Offset 11 */
    socRxReasonInvalid,            /* Offset 12 */
    socRxReasonInvalid,            /* Offset 13 */
    socRxReasonInvalid,            /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t *dcb22_rx_reason_maps[] = {
    dcb22_rx_reason_map,
    NULL
};

#ifdef BROADCOM_DEBUG
static void
dcb22_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb22_t *d = (dcb22_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
#if defined(LE_HOST)
    uint32  hgh[4];
    uint8 *h = (uint8 *)&hgh[0];

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    hgh[3] = soc_htonl(d->mh3);
#else
    uint8 *h = (uint8 *)&d->mh0;
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %ssg %schain %sreload %shg %sstat %spause %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %sstart %send %serror\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!"));
    }
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb))) {
        dcb0_reason_dump(unit, dcb, prefix);
    }
}
#endif /* BROADCOM_DEBUG */

GETFUNCFIELD(22, rx_matchrule, match_rule)
GETFUNCFIELD(22, rx_timestamp_lo, timestamp_lo)
GETFUNCFIELD(22, rx_timestamp_hi, timestamp_hi)
GETFUNCFIELD(22, rx_outer_tag_action, otag_action)
GETFUNCNULL(22, rx_inner_tag_action)
GETFUNCNULL(22, rx_switch_drop)
GETFUNCNULL(22, olp_encap_oam_pkt)
GETFUNCNULL(22, read_ecc_error)
GETFUNCNULL(22, desc_remaining)
SETFUNCNULL(22, desc_remaining)
GETFUNCNULL(22, desc_status)
SETFUNCNULL(22, desc_status)
GETFUNCNULL(22, match_id_lo)
GETFUNCNULL(22, match_id_hi)
GETFUNCNULL(22, forwarding_type)
GETFUNCNULL(22, forwarding_zone_id)

dcb_op_t dcb22_op = {
    22,
    sizeof(dcb22_t),
    dcb22_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb19_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb19_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb19_rx_cos_get,
    dcb19_rx_destmod_get,
    dcb19_rx_destport_get,
    dcb19_rx_opcode_get,
    dcb19_rx_classtag_get,
    dcb22_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb19_rx_prio_get,
    dcb19_rx_reason_get,
    dcb19_rx_reason_hi_get,
    dcb19_rx_ingport_get,
    dcb19_rx_srcport_get,
    dcb19_rx_srcmod_get,
    dcb19_rx_mcast_get,
    dcb19_rx_vclabel_get,
    dcb19_rx_mirror_get,
    dcb22_rx_timestamp_lo_get,
    dcb22_rx_timestamp_hi_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb19_mhp_get,
    dcb19_outer_vid_get,
    dcb19_outer_pri_get,
    dcb19_outer_cfi_get,
    dcb22_rx_outer_tag_action_get,
    dcb19_inner_vid_get,
    dcb19_inner_pri_get,
    dcb19_inner_cfi_get,
    dcb22_rx_inner_tag_action_get,
    dcb19_rx_bpdu_get,
    dcb9_rx_l3_intf_get,
    dcb19_rx_decap_tunnel_get,
    dcb22_rx_switch_drop_get,
    dcb22_olp_encap_oam_pkt_get,
    dcb22_read_ecc_error_get,
    dcb22_desc_remaining_get,
    dcb22_desc_remaining_set,
    dcb22_desc_status_get,
    dcb22_desc_status_set,
    dcb22_match_id_lo_get,
    dcb22_match_id_hi_get,
    dcb22_forwarding_type_get,
    dcb22_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb22_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb19_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb19_rx_ingport_set,
#endif /* PLISIM */
};
#endif /* BCM_SHADOW_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)
/*
 * DCB Type 23 Support
 */
static soc_rx_reason_t
dcb23_rx_reason_map_ip_0[] = { /* IP Overlay 0 */
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonL3HeaderError,      /* Offset 12 */
    socRxReasonProtocol,           /* Offset 13 */
    socRxReasonDosAttack,          /* Offset 14 */
    socRxReasonMartianAddr,        /* Offset 15 */
    socRxReasonTunnelError,        /* Offset 16 */
    socRxReasonMirror,             /* Offset 17 */ 
    socRxReasonIcmpRedirect,       /* Offset 18 */
    socRxReasonL3Slowpath,         /* Offset 19 */
    socRxReasonL3MtuFail,          /* Offset 20 */
    socRxReasonMcastIdxError,      /* Offset 21 */
    socRxReasonVlanFilterMatch,    /* Offset 22 */
    socRxReasonClassBasedMove,     /* Offset 23 */
    socRxReasonL3AddrBindFail,     /* Offset 24 */
    socRxReasonMplsLabelMiss,      /* Offset 25 */
    socRxReasonMplsInvalidAction,  /* Offset 26 */
    socRxReasonMplsInvalidPayload, /* Offset 27 */
    socRxReasonMplsTtl,            /* Offset 28 */
    socRxReasonMplsSequenceNumber, /* Offset 29 */
    socRxReasonL2NonUnicastMiss,   /* Offset 30 */
    socRxReasonNhop,               /* Offset 31 */
    socRxReasonStation,            /* Offset 32 */
    socRxReasonVlanTranslate,      /* Offset 33 */
    socRxReasonTimeSync,           /* Offset 34 */
    socRxReasonOAMSlowpath,        /* Offset 35 */
    socRxReasonOAMError,           /* Offset 36 */
    socRxReasonIpfixRateViolation, /* Offset 37 */
    socRxReasonL2LearnLimit,       /* Offset 38 */
    socRxReasonEncapHigigError,    /* Offset 39 */
    socRxReasonRegexMatch,         /* Offset 40 */
    socRxReasonOAMLMDM,            /* Offset 41 */
    socRxReasonBfd,                /* Offset 42 */
    socRxReasonBfdSlowpath,        /* Offset 43 */
    socRxReasonFailoverDrop,       /* Offset 44 */
    socRxReasonTrillName,          /* Offset 45 */
    socRxReasonTrillTtl,           /* Offset 46 */
    socRxReasonTrillCoreIsIs,      /* Offset 47 */
    socRxReasonTrillSlowpath,      /* Offset 48 */
    socRxReasonTrillRpfFail,       /* Offset 49 */
    socRxReasonTrillMiss,          /* Offset 50 */
    socRxReasonTrillInvalid,       /* Offset 51 */
    socRxReasonNivUntagDrop,       /* Offset 52 */
    socRxReasonNivTagDrop,         /* Offset 53 */
    socRxReasonNivTagInvalid,      /* Offset 54 */
    socRxReasonNivRpfFail,         /* Offset 55 */
    socRxReasonNivInterfaceMiss,   /* Offset 56 */
    socRxReasonNivPrioDrop,        /* Offset 57 */
    socRxReasonParityError,        /* Offset 58 */
    socRxReasonHigigHdrError,      /* Offset 59 */
    socRxReasonFilterMatch,        /* Offset 60 */
    socRxReasonL2GreSipMiss,       /* Offset 61 */
    socRxReasonL2GreVpnIdMiss,     /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t
dcb23_rx_reason_map_ip_1[] = { /* IP Overlay 1 */
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonL3HeaderError,      /* Offset 12 */
    socRxReasonProtocol,           /* Offset 13 */
    socRxReasonDosAttack,          /* Offset 14 */
    socRxReasonMartianAddr,        /* Offset 15 */
    socRxReasonTunnelError,        /* Offset 16 */
    socRxReasonMirror,             /* Offset 17 */ 
    socRxReasonIcmpRedirect,       /* Offset 18 */
    socRxReasonL3Slowpath,         /* Offset 19 */
    socRxReasonL3MtuFail,          /* Offset 20 */
    socRxReasonMcastIdxError,      /* Offset 21 */
    socRxReasonVlanFilterMatch,    /* Offset 22 */
    socRxReasonClassBasedMove,     /* Offset 23 */
    socRxReasonL3AddrBindFail,     /* Offset 24 */
    socRxReasonMplsLabelMiss,      /* Offset 25 */
    socRxReasonMplsInvalidAction,  /* Offset 26 */
    socRxReasonMplsInvalidPayload, /* Offset 27 */
    socRxReasonMplsTtl,            /* Offset 28 */
    socRxReasonMplsSequenceNumber, /* Offset 29 */
    socRxReasonL2NonUnicastMiss,   /* Offset 30 */
    socRxReasonNhop,               /* Offset 31 */
    socRxReasonStation,            /* Offset 32 */
    socRxReasonVlanTranslate,      /* Offset 33 */
    socRxReasonTimeSync,           /* Offset 34 */
    socRxReasonOAMSlowpath,        /* Offset 35 */
    socRxReasonOAMError,           /* Offset 36 */
    socRxReasonIpfixRateViolation, /* Offset 37 */
    socRxReasonL2LearnLimit,       /* Offset 38 */
    socRxReasonEncapHigigError,    /* Offset 39 */
    socRxReasonRegexMatch,         /* Offset 40 */
    socRxReasonOAMLMDM,            /* Offset 41 */
    socRxReasonBfd,                /* Offset 42 */
    socRxReasonBfdSlowpath,        /* Offset 43 */
    socRxReasonFailoverDrop,       /* Offset 44 */
    socRxReasonWlanSlowpathKeepalive, /* Offset 45 */
    socRxReasonWlanTunnelError,    /* Offset 46 */
    socRxReasonWlanSlowpath,       /* Offset 47 */
    socRxReasonWlanDot1xDrop,      /* Offset 48 */
    socRxReasonMplsReservedEntropyLabel, /* Offset 49 */
    socRxReasonCongestionCnmProxy, /* Offset 50 */
    socRxReasonCongestionCnmProxyError, /* Offset 51 */
    socRxReasonCongestionCnm,      /* Offset 52 */
    socRxReasonMplsUnknownAch,     /* Offset 53 */
    socRxReasonMplsLookupsExceeded, /* Offset 54 */
    socRxReasonMplsIllegalReservedLabel, /* Offset 55 */
    socRxReasonMplsRouterAlertLabel, /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonParityError,        /* Offset 58 */
    socRxReasonHigigHdrError,      /* Offset 59 */
    socRxReasonFilterMatch,        /* Offset 60 */
    socRxReasonL2GreSipMiss,       /* Offset 61 */
    socRxReasonL2GreVpnIdMiss,     /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t
dcb23_rx_reason_map_ep[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonStp,                /* Offset 1 */
    socRxReasonVlanTranslate,      /* Offset 2 new */
    socRxReasonTunnelError,        /* Offset 3 */
    socRxReasonIpmc,               /* Offset 4 */
    socRxReasonL3HeaderError,      /* Offset 5 */
    socRxReasonTtl,                /* Offset 6 */
    socRxReasonL2MtuFail,          /* Offset 7 */
    socRxReasonHigigHdrError,      /* Offset 8 */
    socRxReasonSplitHorizon,       /* Offset 9 */
    socRxReasonNivPrune,           /* Offset 10 */
    socRxReasonVirtualPortPrune,   /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonNonUnicastDrop,     /* Offset 13 */
    socRxReasonTrillPacketPortMismatch, /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t
dcb23_rx_reason_map_nlf[] = {
    socRxReasonRegexAction,     /* Offset 0 */
    socRxReasonWlanClientMove,     /* Offset 1 */
    socRxReasonWlanSourcePortMiss, /* Offset 2 */
    socRxReasonWlanClientError,    /* Offset 3 */
    socRxReasonWlanClientSourceMiss, /* Offset 4 */
    socRxReasonWlanClientDestMiss, /* Offset 5 */
    socRxReasonWlanMtu,            /* Offset 6 */
    socRxReasonInvalid,            /* Offset 7 */
    socRxReasonInvalid,            /* Offset 8 */
    socRxReasonInvalid,            /* Offset 9 */
    socRxReasonInvalid,            /* Offset 10 */
    socRxReasonInvalid,            /* Offset 11 */
    socRxReasonInvalid,            /* Offset 12 */
    socRxReasonInvalid,            /* Offset 13 */
    socRxReasonInvalid,            /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t *dcb23_rx_reason_maps[] = {
    dcb23_rx_reason_map_ip_0,
    dcb23_rx_reason_map_ip_1,
    dcb23_rx_reason_map_ep,
    dcb23_rx_reason_map_nlf,
    NULL
};


/*
 * Function:
 *      dcb23_rx_reason_map_get
 * Purpose:
 *      Return the RX reason map for DCB 23 type.
 * Parameters:
 *      dcb_op - DCB operations
 *      dcb    - dma control block
 * Returns:
 *      RX reason map pointer
 */
static soc_rx_reason_t *
dcb23_rx_reason_map_get(dcb_op_t *dcb_op, dcb_t *dcb)
{
    soc_rx_reason_t *map = NULL;
    dcb23_t  *d = (dcb23_t *)dcb;

    switch (d->word4.overlay1.cpu_opcode_type) {
    case SOC_CPU_OPCODE_TYPE_IP_0:
        map = dcb23_rx_reason_map_ip_0;
        break;
    case SOC_CPU_OPCODE_TYPE_IP_1:
        map = dcb23_rx_reason_map_ip_1;
        break;
    case SOC_CPU_OPCODE_TYPE_EP:
        map = dcb23_rx_reason_map_ep;
        break;
    case SOC_CPU_OPCODE_TYPE_NLF:
        map = dcb23_rx_reason_map_nlf;
        break;
    default:
        /* Unknown reason type */
        break;
    }

    return map;
}

static _shr_rx_decap_tunnel_t dcb23_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_ACCESS_SVP,
    _SHR_RX_DECAP_MIM,
    _SHR_RX_DECAP_WTP2AC,
    _SHR_RX_DECAP_AC2AC,
    _SHR_RX_DECAP_AMT,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_TRILL,
    _SHR_RX_DECAP_L2MPLS_1LABEL,
    _SHR_RX_DECAP_L2MPLS_1LABELCW,
    _SHR_RX_DECAP_L3MPLS_1LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABELCW,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_L2GRE
}; 
static uint32 dcb23_rx_decap_tunnel_get(dcb_t *dcb) {
    dcb23_t *d = (dcb23_t *)dcb;
    uint32 decap_tunnel_type = d->word11.overlay1.decap_tunnel_type;
    /* Check for array bounds */
    if ( (decap_tunnel_type <  
    (sizeof (dcb23_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb23_rx_decap_tunnel_map[decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}

static uint32
dcb23_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb23_t *d = (dcb23_t *)dcb;
    uint32 hgh[4];
    soc_higig2_hdr_t *h = (soc_higig2_hdr_t *)&hgh[0];
    COMPILER_REFERENCE(dt_mode);
    
    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    hgh[3] = soc_htonl(d->mh3);

   return ((ingport_is_hg &&
        ((h->ppd_overlay1.ppd_type == 0) ||
        (h->ppd_overlay1.ppd_type == 1))) ?
        ((d->itag_status) ? 0 : 2) :
        ((d->itag_status & 0x2) ?
        ((d->itag_status & 0x1) ? 0 : 2) :
        ((d->itag_status & 0x1) ? 1 : 3)));


}

GETFUNCFIELD(23, xfercount, count)
GETFUNCFIELD(23, rx_cos, word4.overlay1.queue_num)

/* Fields extracted from MH/PBI */
GETHG2FUNCFIELD(23, rx_destmod, dst_mod)
GETHG2FUNCFIELD(23, rx_destport, dst_port)
GETHG2FUNCFIELD(23, rx_srcmod, src_mod)
GETHG2FUNCFIELD(23, rx_srcport, src_port)
GETHG2FUNCFIELD(23, rx_opcode, opcode)
GETHG2FUNCFIELD(23, rx_prio, vlan_pri) /* outer_pri */
GETHG2FUNCEXPR(23, rx_mcast, ((h->ppd_overlay1.dst_mod << 8) |
                              (h->ppd_overlay1.dst_port)))
GETHG2FUNCEXPR(23, rx_vclabel, ((h->ppd_overlay1.vc_label_19_16 << 16) |
                              (h->ppd_overlay1.vc_label_15_8 << 8) |
                              (h->ppd_overlay1.vc_label_7_0)))
GETHG2FUNCEXPR(23, rx_classtag, (h->ppd_overlay1.ppd_type != 1 ? 0 :
                                 (h->ppd_overlay2.ctag_hi << 8) |
                                 (h->ppd_overlay2.ctag_lo)))
GETFUNCFIELD(23, rx_matchrule, match_rule)
GETFUNCFIELD(23, rx_reason, reason)
GETFUNCFIELD(23, rx_reason_hi, reason_hi)
GETFUNCFIELD(23, rx_ingport, srcport)
GETFUNCEXPR(23, rx_mirror, ((SOC_CPU_OPCODE_TYPE_IP_0 ==
                            d->word4.overlay1.cpu_opcode_type) ?
                            (d->reason & (1 << 17)) : 0))
GETFUNCFIELD(23, rx_timestamp, word12.overlay1.timestamp)
GETFUNCFIELD(23, rx_timestamp_upper, word14.overlay1.timestamp_hi)
GETPTREXPR(23, mhp, &(d->mh0))
GETFUNCFIELD(23, outer_vid, word4.overlay1.outer_vid)
GETFUNCFIELD(23, outer_pri, word11.overlay1.outer_pri)
GETFUNCFIELD(23, outer_cfi, word11.overlay1.outer_cfi)
GETFUNCFIELD(23, rx_outer_tag_action, otag_action)
GETFUNCFIELD(23, inner_vid, word11.overlay1.inner_vid)
GETFUNCFIELD(23, inner_pri, inner_pri)
GETFUNCFIELD(23, inner_cfi, word11.overlay1.inner_cfi)
GETFUNCFIELD(23, rx_inner_tag_action, itag_action)
GETFUNCFIELD(23, rx_bpdu, bpdu)
GETFUNCEXPR(23, rx_l3_intf, ((d->replicated) ? (d->repl_nhi) :
                (((d->repl_nhi) & 0x40000) ?  /* TR3 NHI */
                 (((d->repl_nhi) & 0x3ffff) + _SHR_L3_EGRESS_IDX_MIN) :
                 ((d->repl_nhi) & 0x4000) ?   /* HX4 NHI */
                 (((d->repl_nhi) & 0x3fff) + _SHR_L3_EGRESS_IDX_MIN) :
                 (d->repl_nhi))))
GETFUNCFIELD(23, rx_switch_drop, all_switch_drop)
GETFUNCNULL(23, olp_encap_oam_pkt)
GETFUNCNULL(23, read_ecc_error)
GETFUNCNULL(23, desc_remaining)
SETFUNCNULL(23, desc_remaining)
GETFUNCNULL(23, desc_status)
SETFUNCNULL(23, desc_status)
GETFUNCNULL(23, match_id_lo)
GETFUNCNULL(23, match_id_hi)
GETFUNCNULL(23, forwarding_type)
GETFUNCNULL(23, forwarding_zone_id)

#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb23_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[2] = d[3] = d[4] = d[5] = d[10] = 0;
    d[11] = d[12] = d[13] = d[14] = d[15] = 0;
}
SETFUNCFIELD(23, rx_ingport, srcport, int val, val)
#endif  /* PLISIM */

#ifdef BROADCOM_DEBUG
static void
dcb23_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t     *dtx = (dcb19_t *)dcb;  /* Fake out for different TX MH */
    dcb23_t     *d = (dcb23_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
    uint8       *h;
#if defined(LE_HOST)
    uint32      hgh[4];
    h = (uint8 *)&hgh[0];

    if (tx) {
        hgh[0] = soc_htonl(dtx->mh0);
        hgh[1] = soc_htonl(dtx->mh1);
        hgh[2] = soc_htonl(dtx->mh2);
        hgh[3] = soc_htonl(dtx->mh3);
    } else {
        hgh[0] = soc_htonl(d->mh0);
        hgh[1] = soc_htonl(d->mh1);
        hgh[2] = soc_htonl(d->mh2);
        hgh[3] = soc_htonl(d->mh3);
    }
#else
    if (tx) {
        h = (uint8 *)&dtx->mh0;
    } else {
        h = (uint8 *)&d->mh0;
    }
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %sstat %spause "
             " %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }

    dcb0_reason_dump(unit, dcb, prefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%schg_tos %sregen_crc %schg_ecn %svfi_valid"
             " %sdvp_nhi_sel\n"),
             prefix,
             d->word11.overlay1.chg_tos ? "" : "!",
             d->regen_crc ? "" : "!",
             d->chg_ecn ? "" : "!",
             d->vfi_valid ? "" : "!",
             d->dvp_nhi_sel ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sservice_tag %sswitch_pkt %shg_type %ssrc_hg\n"),
             prefix,
             d->service_tag ? "" : "!",
             d->switch_pkt ? "" : "!",
             d->hg_type ? "" : "!",
             d->src_hg ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sl3routed %sl3only %sreplicated %sdo_not_change_ttl"
             " %sbpdu\n"),
             prefix,
             d->l3routed ? "" : "!",
             d->l3only ? "" : "!",
             d->replicated ? "" : "!",
             d->do_not_change_ttl ? "" : "!",
             d->bpdu ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%shg2_ext_hdr\n"),
             prefix,
             d->hg2_ext_hdr ? "" : "!"));
    if (d->hg2_ext_hdr) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\tHigig2 Extension: queue_tag=%04x %stm tag_type=%d"
                 " seg_sel=%d\n"),
                 prefix,
                 d->word14.overlay2.eh_queue_tag,
                 (d->word14.overlay2.eh_tm) ? "" : "!",
                 d->word14.overlay2.eh_tag_type,
                 d->word14.overlay2.eh_seg_sel));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\treason_type=%d reason=%08x_%08x ts_type=%d"
             " timestamp=%08x_%08x"),
             prefix,
             d->word4.overlay1.cpu_opcode_type,
             d->reason_hi,
             d->reason,
             d->timestamp_type,
             d->word14.overlay1.timestamp_hi,
             d->word12.overlay1.timestamp));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tsrcport=%d cpu_cos=%d hgi=%d lb_pkt_type=%d"
             " repl_nhi=%05x\n"),
             prefix,
             d->word4.overlay1.queue_num,
             d->hgi,
             d->loopback_pkt_type,
             d->srcport,
             d->repl_nhi));
    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d otag_action=%d"
             " vntag_action=%d\n"),
             prefix,
             d->word4.overlay1.outer_vid,
             d->word11.overlay1.outer_cfi,
             d->word11.overlay1.outer_pri,
             d->otag_action,
             d->word11.overlay1.vntag_action));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tinner_vid=%d inner_cfi=%d inner_pri=%d itag_action=%d"
             " itag_status=%d\n"),
             prefix,
             d->word11.overlay1.inner_vid,
             d->word11.overlay1.inner_cfi,
             d->inner_pri,
             d->itag_action,
             d->itag_status));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdscp=%d ecn=%d %sspecial_pkt %sall_switch_drop\n"),
             prefix,
             d->word11.overlay1.dscp,
             d->ecn,
             d->special_pkt ? "" : "!",
             d->all_switch_drop ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdecap_tunnel_type=%d vfi=%d match_rule=%d"
             " mtp_ind=%d\n"),
             prefix,
             d->word11.overlay1.decap_tunnel_type,
             d->word12.overlay2.vfi,
             d->match_rule,
             d->mtp_index));
}
#endif /* BROADCOM_DEBUG */

dcb_op_t dcb23_op = {
    23,
    sizeof(dcb23_t),
    dcb23_rx_reason_maps,
    dcb23_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb23_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb23_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb23_rx_cos_get,
    dcb23_rx_destmod_get,
    dcb23_rx_destport_get,
    dcb23_rx_opcode_get,
    dcb23_rx_classtag_get,
    dcb23_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb23_rx_prio_get,
    dcb23_rx_reason_get,
    dcb23_rx_reason_hi_get,
    dcb23_rx_ingport_get,
    dcb23_rx_srcport_get,
    dcb23_rx_srcmod_get,
    dcb23_rx_mcast_get,
    dcb23_rx_vclabel_get,
    dcb23_rx_mirror_get,
    dcb23_rx_timestamp_get,
    dcb23_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb23_mhp_get,
    dcb23_outer_vid_get,
    dcb23_outer_pri_get,
    dcb23_outer_cfi_get,
    dcb23_rx_outer_tag_action_get,
    dcb23_inner_vid_get,
    dcb23_inner_pri_get,
    dcb23_inner_cfi_get,
    dcb23_rx_inner_tag_action_get,
    dcb23_rx_bpdu_get,
    dcb23_rx_l3_intf_get,
    dcb23_rx_decap_tunnel_get,
    dcb23_rx_switch_drop_get,
    dcb23_olp_encap_oam_pkt_get,
    dcb23_read_ecc_error_get,
    dcb23_desc_remaining_get,
    dcb23_desc_remaining_set,
    dcb23_desc_status_get,
    dcb23_desc_status_set,
    dcb23_match_id_lo_get,
    dcb23_match_id_hi_get,
    dcb23_forwarding_type_get,
    dcb23_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb23_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb23_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb23_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_KATANA_SUPPORT)
/*
 * DCB Type 24 Support
 */

static soc_rx_reason_t
dcb24_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonMirror,             /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL3AddrBindFail,     /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonBfdSlowpath,        /* Offset 35 */
    socRxReasonStation,            /* Offset 36 */
    socRxReasonNiv,                /* Offset 37 */
    socRxReasonNiv,                /* Offset 38 */
    socRxReasonNiv,                /* Offset 39 */
    socRxReasonVlanTranslate,      /* Offset 40 */
    socRxReasonTimeSync,           /* Offset 41 */
    socRxReasonOAMSlowpath,        /* Offset 42 */
    socRxReasonOAMError,           /* Offset 43 */
    socRxReasonOAMLMDM,            /* Offset 44 */
    socRxReasonL2LearnLimit,       /* Offset 45 */
    socRxReasonBfd,                /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t *dcb24_rx_reason_maps[] = {
    dcb24_rx_reason_map,
    NULL
};

static int
dcb24_addrx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb24_t     *d;     /* DCB */
    uint32      *di;    /* DCB integer pointer */

    d = (dcb24_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = 0;
    di[5] = di[6] = di[7] = di[8] = di[9] = di[10] = 0;
    di[11] = di[12] = di[13] = di[14] = di[15] = 0;

    if (addr) {
        d->addr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    }
    d->c_count = count;
    d->c_sg = 1;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static _shr_rx_decap_tunnel_t dcb24_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_ACCESS_SVP,
    _SHR_RX_DECAP_MIM,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_L2MPLS_1LABEL,
    _SHR_RX_DECAP_L2MPLS_1LABELCW,
    _SHR_RX_DECAP_L3MPLS_1LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABELCW,
}; 
static uint32 dcb24_rx_decap_tunnel_get(dcb_t *dcb) {
    dcb24_t *d = (dcb24_t *)dcb;
    uint32 decap_tunnel_type = d->word11.overlay1.decap_tunnel_type;
    /* Check for array bounds */
    if ( (decap_tunnel_type <  
    (sizeof (dcb24_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb24_rx_decap_tunnel_map[decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}
GETFUNCEXPR(24, rx_mirror, (d->reason & (1 << 18)))
GETFUNCEXPR(24, rx_l3_intf, ((d->replicated) ? (d->repl_nhi) :
                             (((d->repl_nhi) & 0x1fff) +
                              _SHR_L3_EGRESS_IDX_MIN)))

#ifdef BROADCOM_DEBUG
static void
dcb24_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t     *dtx = (dcb19_t *)dcb;
    dcb24_t     *drx = (dcb24_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
    uint8       *h;
#if defined(LE_HOST)
    uint32      hgh[4];
    h = (uint8 *)&hgh[0];

    if (tx) {
        hgh[0] = soc_htonl(dtx->mh0);
        hgh[1] = soc_htonl(dtx->mh1);
        hgh[2] = soc_htonl(dtx->mh2);
        hgh[3] = soc_htonl(dtx->mh3);
    } else {
        hgh[0] = soc_htonl(drx->mh0);
        hgh[1] = soc_htonl(drx->mh1);
        hgh[2] = soc_htonl(drx->mh2);
        hgh[3] = soc_htonl(drx->mh3);
    }
#else
    if (tx) {
        h = (uint8 *)&dtx->mh0;
    } else {
        h = (uint8 *)&drx->mh0;
    }
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %sstat %spause "
             " %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             dtx->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }
    
    dcb0_reason_dump(unit, dcb, prefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%schg_tos %sregen_crc %schg_ecn %svfi_valid\n"),
             prefix,
             drx->word11.overlay1.chg_tos ? "" : "!",
             drx->regen_crc ? "" : "!",
             drx->chg_ecn ? "" : "!",
             drx->vfi_valid ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sservice_tag %sswitch_pkt %shg_type %ssrc_hg\n"),
             prefix,
             drx->service_tag ? "" : "!",
             drx->switch_pkt ? "" : "!",
             drx->hg_type ? "" : "!",
             drx->src_hg ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sl3routed %sl3only %sreplicated %sdo_not_change_ttl"
             " %sbpdu\n"),
             prefix,
             drx->l3routed ? "" : "!",
             drx->l3only ? "" : "!",
             drx->replicated ? "" : "!",
             drx->do_not_change_ttl ? "" : "!",
             drx->bpdu ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%shg2_ext_hdr\n"),
             prefix,
             drx->hg2_ext_hdr ? "" : "!"));
    if (drx->hg2_ext_hdr) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\tHigig2 Extension: queue_tag=%04x %stm tag_type=%d"
                 " seg_sel=%d\n"),
                 prefix,
                 drx->word14.overlay2.eh_queue_tag,
                 (drx->word14.overlay2.eh_tm) ? "" : "!",
                 drx->word14.overlay2.eh_tag_type,
                 drx->word14.overlay2.eh_seg_sel));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\treason=%08x_%08x ts_type=%d"
             " timestamp=%08x_%08x"),
             prefix,
             drx->reason_hi,
             drx->reason,
             drx->timestamp_type,
             drx->word14.overlay1.timestamp_hi,
             drx->timestamp));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tsrcport=%d queue_num=%d hgi=%d lb_pkt_type=%d"
             " repl_nhi=%05x\n"),
             prefix,
             drx->srcport,
             drx->word4.overlay1.queue_num,
             drx->hgi,
             drx->loopback_pkt_type,
             drx->repl_nhi));
    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d otag_action=%d"
             " vntag_action=%d\n"),
             prefix,
             drx->word4.overlay1.outer_vid,
             drx->word11.overlay1.outer_cfi,
             drx->word11.overlay1.outer_pri,
             drx->otag_action,
             drx->word11.overlay1.vntag_action));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tinner_vid=%d inner_cfi=%d inner_pri=%d itag_action=%d"
             " itag_status=%d\n"),
             prefix,
             drx->word11.overlay1.inner_vid,
             drx->word11.overlay1.inner_cfi,
             drx->inner_pri,
             drx->itag_action,
             drx->itag_status));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdscp=%d ecn=%d %sspecial_pkt %sall_switch_drop\n"),
             prefix,
             drx->word11.overlay1.dscp,
             drx->ecn,
             drx->oam_pkt ? "" : "!",
             drx->all_switch_drop ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdecap_tunnel_type=%d vfi=%d match_rule=%d"
             " mtp_ind=%d\n"),
             prefix,
             drx->word11.overlay1.decap_tunnel_type,
             drx->timestamp & 0x3fff,
             drx->match_rule,
             drx->mtp_index));
}
#endif /* BROADCOM_DEBUG */

GETFUNCFIELD(24, outer_vid, word4.overlay1.outer_vid)

dcb_op_t dcb24_op = {
    24,
    sizeof(dcb24_t),
    dcb24_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb24_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb23_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb23_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb23_rx_cos_get,
    dcb23_rx_destmod_get,
    dcb23_rx_destport_get,
    dcb23_rx_opcode_get,
    dcb23_rx_classtag_get,
    dcb23_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb23_rx_prio_get,
    dcb23_rx_reason_get,
    dcb23_rx_reason_hi_get,
    dcb23_rx_ingport_get,
    dcb23_rx_srcport_get,
    dcb23_rx_srcmod_get,
    dcb23_rx_mcast_get,
    dcb23_rx_vclabel_get,
    dcb24_rx_mirror_get,
    dcb23_rx_timestamp_get,
    dcb23_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb23_mhp_get,
    dcb24_outer_vid_get,
    dcb23_outer_pri_get,
    dcb23_outer_cfi_get,
    dcb23_rx_outer_tag_action_get,
    dcb23_inner_vid_get,
    dcb23_inner_pri_get,
    dcb23_inner_cfi_get,
    dcb23_rx_inner_tag_action_get,
    dcb23_rx_bpdu_get,
    dcb24_rx_l3_intf_get,
    dcb24_rx_decap_tunnel_get,
    dcb23_rx_switch_drop_get,
    dcb23_olp_encap_oam_pkt_get,
    dcb23_read_ecc_error_get,
    dcb23_desc_remaining_get,
    dcb23_desc_remaining_set,
    dcb23_desc_status_get,
    dcb23_desc_status_set,
    dcb23_match_id_lo_get,
    dcb23_match_id_hi_get,
    dcb23_forwarding_type_get,
    dcb23_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb24_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb23_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb23_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_KATANA_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
/*
 * DCB Type 26 Support
 */
/* From FORMAT CPU_OPCODES */
static soc_rx_reason_t
dcb26_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /*  0: CPU_UVLAN */
    socRxReasonL2SourceMiss,       /*  1: CPU_SLF */
    socRxReasonL2DestMiss,         /*  2: CPU_DLF */
    socRxReasonL2Move,             /*  3: CPU_L2MOVE */
    socRxReasonL2Cpu,              /*  4: CPU_L2CPU */
    socRxReasonSampleSource,       /*  5: CPU_SFLOW_SRC */
    socRxReasonSampleDest,         /*  6: CPU_SFLOW_DST */
    socRxReasonL3SourceMiss,       /*  7: CPU_L3SRC_MISS */
    socRxReasonL3DestMiss,         /*  8: CPU_L3DST_MISS */
    socRxReasonL3SourceMove,       /*  9: CPU_L3SRC_MOVE */
    socRxReasonMcastMiss,          /* 10: CPU_MC_MISS */
    socRxReasonIpMcastMiss,        /* 11: CPU_IPMC_MISS */
    socRxReasonFilterMatch,        /* 12: CPU_FFP */
    socRxReasonL3HeaderError,      /* 13: CPU_L3HDR_ERR */
    socRxReasonProtocol,           /* 14: CPU_PROTOCOL_PKT */
    socRxReasonDosAttack,          /* 15: CPU_DOS_ATTACK */
    socRxReasonMartianAddr,        /* 16: CPU_MARTIAN_ADDR */
    socRxReasonTunnelError,        /* 17: CPU_TUNNEL_ERR */
    socRxReasonInvalid,            /* 18: RESERVED_0 */ 
    socRxReasonIcmpRedirect,       /* 19: ICMP_REDIRECT */
    socRxReasonL3Slowpath,         /* 20: L3_SLOWPATH */
    socRxReasonParityError,        /* 21: PARITY_ERROR */
    socRxReasonL3MtuFail,          /* 22: L3_MTU_CHECK_FAIL */
    socRxReasonHigigHdrError,      /* 23: HGHDR_ERROR */
    socRxReasonMcastIdxError,      /* 24: MCIDX_ERROR */
    socRxReasonVlanFilterMatch,    /* 25: VFP */
    socRxReasonClassBasedMove,     /* 26: CBSM_PREVENTED */
    socRxReasonL3AddrBindFail,     /* 27: MAC_BIND_FAIL */
    socRxReasonMplsLabelMiss,      /* 28: MPLS_LABEL_MISS */
    socRxReasonMplsInvalidAction,  /* 29: MPLS_INVALID_ACTION */
    socRxReasonMplsInvalidPayload, /* 30: MPLS_INVALID_PAYLOAD */
    socRxReasonMplsTtl,            /* 31: MPLS_TTL_CHECK_FAIL */
    socRxReasonMplsSequenceNumber, /* 32: MPLS_SEQ_NUM_FAIL */
    socRxReasonL2NonUnicastMiss,   /* 33: PBT_NONUC_PKT */
    socRxReasonNhop,               /* 34: L3_NEXT_HOP */
    socRxReasonMplsUnknownAch,     /* 35: MPLS_UNKNOWN_ACH_ERROR */
    socRxReasonStation,            /* 36: MY_STATION */
    socRxReasonNiv,                /* 37: NIV_DROP_REASON_ENCODING */
    socRxReasonNiv,                /* 38: ->  */
    socRxReasonNiv,                /* 39: 3-bit */
    socRxReasonVlanTranslate,      /* 40: VXLT_MISS */
    socRxReasonTimeSync,           /* 41: TIME_SYNC */
    socRxReasonOAMSlowpath,        /* 42: OAM_SLOWPATH */
    socRxReasonOAMError,           /* 43: OAM_ERROR */
    socRxReasonTrill,              /* 44: TRILL_DROP_REASON_ENCODING */
    socRxReasonTrill,              /* 45: -> */
    socRxReasonTrill,              /* 46: 3-bit */
    socRxReasonL2GreSipMiss,       /* 47: L2GRE_SIP_MISS */
    socRxReasonL2GreVpnIdMiss,     /* 48: L2GRE_VPNID_MISS */
    socRxReasonBfdSlowpath,        /* 49: BFD_SLOWPATH */
    socRxReasonBfd,                /* 50: BFD_ERROR */
    socRxReasonOAMLMDM,            /* 51: OAM_LMDM */
    socRxReasonCongestionCnm,      /* 52: ICNM */
    socRxReasonMplsIllegalReservedLabel, /* 53: MPLS_ILLEGAL_RESERVED_LABEL */
    socRxReasonMplsRouterAlertLabel, /* 54: MPLS_ALERT_LABEL */
    socRxReasonCongestionCnmProxy, /* 55: QCN_CNM_PRP */
    socRxReasonCongestionCnmProxyError, /* 56: QCN_CNM_PRP_DLF */
    socRxReasonVxlanSipMiss,       /* 57: VXLAN_SIP_MISS */
    socRxReasonVxlanVpnIdMiss,     /* 58: VXLAN_VN_ID_MISS */
    socRxReasonFcoeZoneCheckFail,  /* 59: FCOE_ZONE_CHECK_FAIL */
    socRxReasonNat,                /* 60: NAT_DROP_REASON_ENCODING */
    socRxReasonNat,                /* 61: -> */
    socRxReasonNat,                /* 62: 3-bit */
    socRxReasonIpmcInterfaceMismatch /* 63: CPU_IPMC_INTERFACE_MISMATCH */
};
/* From FORMAT TRILL_CPU_OPCODE_ENCODING */
static soc_rx_reason_t dcb26_rx_reason_trill_encoding_map[] = {
    socRxReasonTrill,              /* 0:NO_ERRORS
                                    * Base field, must match the entries above */
    socRxReasonTrillInvalid,       /* 1:TRILL_HDR_ERROR */
    socRxReasonTrillMiss,          /* 2:TRILL_LOOKUP_MISS */
    socRxReasonTrillRpfFail,       /* 3:TRILL_RPF_CHECK_FAIL */
    socRxReasonTrillSlowpath,      /* 4:TRILL_SLOWPATH */
    socRxReasonTrillCoreIsIs,      /* 5:TRILL_CORE_IS_IS_PKT */
    socRxReasonTrillTtl,           /* 6:TRILL_HOP_COUNT_CHECK_FAIL */
    socRxReasonTrillName,          /* 7:NICKNAME_TABLE_COPYTOCPU */
    socRxReasonInvalid,            /* Offset 8 */
    socRxReasonInvalid,            /* Offset 9 */
    socRxReasonInvalid,            /* Offset 10 */
    socRxReasonInvalid,            /* Offset 11 */
    socRxReasonInvalid,            /* Offset 12 */
    socRxReasonInvalid,            /* Offset 13 */
    socRxReasonInvalid,            /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};
/* From FORMAT NAT_CPU_OPCODE_ENCODING */
static soc_rx_reason_t dcb26_rx_reason_nat_encoding_map[] = {
    socRxReasonNat,                /* 0:NOP
                                    * Base field, must match the entries above */
    socRxReasonTcpUdpNatMiss,      /* 1:NORMAL */
    socRxReasonIcmpNatMiss,        /* 2:ICMP */
    socRxReasonNatFragment,        /* 3:FRAGMEMT */
    socRxReasonNatMiss,            /* 4:OTHER */
    socRxReasonInvalid,            /* Offset 5 */
    socRxReasonInvalid,            /* Offset 6 */
    socRxReasonInvalid,            /* Offset 7 */
    socRxReasonInvalid,            /* Offset 8 */
    socRxReasonInvalid,            /* Offset 9 */
    socRxReasonInvalid,            /* Offset 10 */
    socRxReasonInvalid,            /* Offset 11 */
    socRxReasonInvalid,            /* Offset 12 */
    socRxReasonInvalid,            /* Offset 13 */
    socRxReasonInvalid,            /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};
static soc_rx_reason_t *dcb26_rx_reason_maps[] = {
    dcb26_rx_reason_map,
    dcb21_rx_reason_niv_encoding_map,
    dcb26_rx_reason_trill_encoding_map,
    dcb26_rx_reason_nat_encoding_map,
    NULL
};

static _shr_rx_decap_tunnel_t dcb26_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_ACCESS_SVP,
    _SHR_RX_DECAP_MIM,
    _SHR_RX_DECAP_L2GRE,
    _SHR_RX_DECAP_VXLAN,
    _SHR_RX_DECAP_AMT,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_TRILL,
    _SHR_RX_DECAP_L2MPLS_1LABEL,
    _SHR_RX_DECAP_L2MPLS_2LABEL,
    _SHR_RX_DECAP_L2MPLS_1LABELCW,
    _SHR_RX_DECAP_L2MPLS_2LABELCW,
    _SHR_RX_DECAP_L3MPLS_1LABEL,
    _SHR_RX_DECAP_L3MPLS_2LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABELCW,
    _SHR_RX_DECAP_L3MPLS_2LABELCW
};
static uint32 dcb26_rx_decap_tunnel_get(dcb_t *dcb) {
    dcb26_t *d = (dcb26_t *)dcb;
    /* Check for array bounds */
    if ( (d->word11.overlay1.decap_tunnel_type <  
    (sizeof (dcb26_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb26_rx_decap_tunnel_map[d->word11.overlay1.decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}
static uint32
dcb26_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb26_t *d = (dcb26_t *)dcb;

    COMPILER_REFERENCE(dt_mode);

    return (ingport_is_hg ?
            ((d->tag_status) ? 0 : 2) :
            ((d->tag_status & 0x2) ?
             ((d->tag_status & 0x1) ? 0 : 2) :
             ((d->tag_status & 0x1) ? 1 : 3)));
}

static uint32 
dcb26_rx_reason_get(dcb_t *dcb)
{
    dcb26_t *d = (dcb26_t *)dcb;

    return (d->switch_pkt) ? (d->reason) : 0;
}

static uint32 
dcb26_rx_reason_hi_get(dcb_t *dcb)
{
    dcb26_t *d = (dcb26_t *)dcb;

    return (d->switch_pkt) ? (d->reason_hi) : 0;
}

static uint32
dcb26_rx_mirror_get(dcb_t *dcb)
{
    dcb26_t *d = (dcb26_t *)dcb;

    return (d->switch_pkt) ? 0 : 1;
}

GETFUNCFIELD(26, xfercount, count)
GETFUNCFIELD(26, rx_cos, word4.overlay2.cpu_cos)

/* Fields extracted from MH/PBI */
GETHG2FUNCFIELD(26, rx_destmod, dst_mod)
GETHG2FUNCFIELD(26, rx_destport, dst_port)
GETHG2FUNCFIELD(26, rx_srcmod, src_mod)
GETHG2FUNCFIELD(26, rx_srcport, src_port)
GETHG2FUNCFIELD(26, rx_opcode, opcode)
GETHG2FUNCFIELD(26, rx_prio, vlan_pri) /* outer_pri */
GETHG2FUNCEXPR(26, rx_mcast, ((h->ppd_overlay1.dst_mod << 8) |
                              (h->ppd_overlay1.dst_port)))
GETHG2FUNCEXPR(26, rx_vclabel, ((h->ppd_overlay1.vc_label_19_16 << 16) |
                              (h->ppd_overlay1.vc_label_15_8 << 8) |
                              (h->ppd_overlay1.vc_label_7_0)))
GETHG2FUNCEXPR(26, rx_classtag, (h->ppd_overlay1.ppd_type != 1 ? 0 :
                                 (h->ppd_overlay2.ctag_hi << 8) |
                                 (h->ppd_overlay2.ctag_lo)))
GETFUNCFIELD(26, rx_matchrule, match_rule)
GETFUNCFIELD(26, rx_ingport, srcport)
GETFUNCFIELD(26, rx_timestamp, timestamp)
GETFUNCFIELD(26, rx_timestamp_upper, word14.timestamp_hi)
GETPTREXPR(26, mhp, &(d->mh0))
GETFUNCFIELD(26, outer_vid, word4.overlay1.outer_vid)
GETFUNCFIELD(26, outer_pri, word11.overlay1.outer_pri)
GETFUNCFIELD(26, outer_cfi, word11.overlay1.outer_cfi)
GETFUNCFIELD(26, inner_vid, word11.overlay1.inner_vid)
GETFUNCFIELD(26, inner_pri, inner_pri)
GETFUNCFIELD(26, inner_cfi, word11.overlay1.inner_cfi)
GETFUNCFIELD(26, rx_bpdu, bpdu)
GETFUNCEXPR(26, rx_l3_intf,
            (((d->repl_nhi) & 0xffff) + _SHR_L3_EGRESS_IDX_MIN))

#ifdef  PLISIM          /* these routines are only used by pcid */
SETFUNCFIELD(26, rx_ingport, srcport, int val, val)
#endif  /* PLISIM */

#ifdef BROADCOM_DEBUG
static void
dcb26_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t     *dtx = (dcb19_t *)dcb;  /* Fake out for different TX MH */
    dcb26_t     *d = (dcb26_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
    uint8       *h;
#if defined(LE_HOST)
    uint32      hgh[4];
    h = (uint8 *)&hgh[0];

    if (tx) {
        hgh[0] = soc_htonl(dtx->mh0);
        hgh[1] = soc_htonl(dtx->mh1);
        hgh[2] = soc_htonl(dtx->mh2);
        hgh[3] = soc_htonl(dtx->mh3);
    } else {
        hgh[0] = soc_htonl(d->mh0);
        hgh[1] = soc_htonl(d->mh1);
        hgh[2] = soc_htonl(d->mh2);
        hgh[3] = soc_htonl(d->mh3);
    }
#else
    if (tx) {
        h = (uint8 *)&dtx->mh0;
    } else {
        h = (uint8 *)&d->mh0;
    }
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %sstat %spause "
             " %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }

    dcb0_reason_dump(unit, dcb, prefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%schg_tos %sregen_crc %schg_ecn %svfi_valid "
             "%sdvp_nhi_sel\n"),
             prefix,
             d->word11.overlay1.chg_tos ? "" : "!",
             d->regen_crc ? "" : "!",
             d->chg_ecn ? "" : "!",
             d->vfi_valid ? "" : "!",
             d->dvp_nhi_sel ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sservice_tag %sswitch_pkt %shg_type %ssrc_hg\n"),
             prefix,
             d->service_tag ? "" : "!",
             d->switch_pkt ? "" : "!",
             d->hg_type ? "" : "!",
             d->src_hg ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sl3routed %sl3only %sreplicated %sdo_not_change_ttl "
             "%sbpdu\n"),
             prefix,
             d->l3routed ? "" : "!",
             d->l3only ? "" : "!",
             d->replicated ? "" : "!",
             d->do_not_change_ttl ? "" : "!",
             d->bpdu ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%shg2_ext_hdr %sspecial_pkt %sall_switch_drop\n"),
             prefix,
             d->hg2_ext_hdr ? "" : "!",
             d->special_pkt ? "" : "!",
             d->all_switch_drop ? "" : "!"));
    if (d->hg2_ext_hdr) {
        switch (d->word14.overlay1.eh_type) {
        case 0:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 0: queue_tag=0x%04x "
                     "tag_type=%d seg_sel=%d\n"),
                     prefix,
                     d->word14.overlay1.eh_queue_tag,
                     d->word14.overlay1.eh_tag_type,
                     d->word14.overlay1.eh_seg_sel));
            break;
        case 1:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 1: classid=%d l3_iif=%d "
                     "classid_type=%d\n"),
                     prefix,
                     d->word14.overlay2.classid,
                     d->word14.overlay2.l3_iif,
                     d->word14.overlay2.classid_type));
            break;
        case 2:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 2: classid=%d "
                     "classid_type=%d\n"),
                     prefix,
                     d->word14.overlay2.classid,
                     d->word14.overlay2.classid_type));
            break;
        default:
            break;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\treason_type=%d reason=%08x_%08x\n"),
             prefix,
             d->word4.overlay1.cpu_opcode_type,
             d->reason_hi,
             d->reason));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tts_type=%d timestamp=%08x_%08x\n"),
             prefix,
             d->timestamp_type,
             d->word14.timestamp_hi,
             d->timestamp));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tsrcport=%d cpu_cos=%d hgi=%d lb_pkt_type=%d "
             "repl_nhi=%05x\n"),
             prefix,
             d->srcport,
             d->word4.overlay2.cpu_cos,
             d->hgi,
             d->loopback_pkt_type,
             d->repl_nhi));
    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d otag_action=%d "
             "vntag_action=%d\n"),
             prefix,
             d->word4.overlay1.outer_vid,
             d->word11.overlay1.outer_cfi,
             d->word11.overlay1.outer_pri,
             d->otag_action,
             d->word11.overlay1.vntag_action));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tinner_vid=%d inner_cfi=%d inner_pri=%d itag_action=%d "
             "tag_status=%d\n"),
             prefix,
             d->word11.overlay1.inner_vid,
             d->word11.overlay1.inner_cfi,
             d->inner_pri,
             d->itag_action,
             d->tag_status));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdscp=%d ecn=%d decap_tunnel_type=%d match_rule=%d "
             "mtp_ind=%d\n"),
             prefix,
             d->word11.overlay1.dscp,
             d->ecn,
             d->word11.overlay1.decap_tunnel_type,
             d->match_rule,
             d->mtp_index));
}
#endif /* BROADCOM_DEBUG */

dcb_op_t dcb26_op = {
    26,
    sizeof(dcb26_t),
    dcb26_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb21_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb26_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb26_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb26_rx_cos_get,
    dcb26_rx_destmod_get,
    dcb26_rx_destport_get,
    dcb26_rx_opcode_get,
    dcb26_rx_classtag_get,
    dcb26_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb26_rx_prio_get,
    dcb26_rx_reason_get,
    dcb26_rx_reason_hi_get,
    dcb26_rx_ingport_get,
    dcb26_rx_srcport_get,
    dcb26_rx_srcmod_get,
    dcb26_rx_mcast_get,
    dcb26_rx_vclabel_get,
    dcb26_rx_mirror_get,
    dcb26_rx_timestamp_get,
    dcb26_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb26_mhp_get,
    dcb26_outer_vid_get,
    dcb26_outer_pri_get,
    dcb26_outer_cfi_get,
    dcb23_rx_outer_tag_action_get,
    dcb26_inner_vid_get,
    dcb26_inner_pri_get,
    dcb26_inner_cfi_get,
    dcb23_rx_inner_tag_action_get,
    dcb26_rx_bpdu_get,
    dcb26_rx_l3_intf_get,
    dcb26_rx_decap_tunnel_get,
    dcb23_rx_switch_drop_get,
    dcb23_olp_encap_oam_pkt_get,
    dcb23_read_ecc_error_get,
    dcb23_desc_remaining_get,
    dcb23_desc_remaining_set,
    dcb23_desc_status_get,
    dcb23_desc_status_set,
    dcb23_match_id_lo_get,
    dcb23_match_id_hi_get,
    dcb23_forwarding_type_get,
    dcb23_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb26_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb23_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb26_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_TRIDENT2_SUPPORT */

#endif  /* BCM_XGS3_SWITCH_SUPPORT */


#ifdef BCM_PETRA_SUPPORT
#ifdef  PLISIM
static void dcb28_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[5] = d[6] = d[7] = d[8] = d[9] = d[10] = 0;
	d[11] = d[12] = d[13] = d[14] = 0;
}
SETFUNCFIELD(28, xfercount, count, uint32 count, count)
SETFUNCFIELD(28, rx_start, start, int val, val ? 1 : 0)
SETFUNCFIELD(28, rx_end, end, int val, val ? 1 : 0)
SETFUNCFIELD(28, rx_error, error, int val, val ? 1 : 0)
SETFUNCEXPRIGNORE(28, rx_crc, int val, ignore)
#endif
#if defined(LE_HOST)
static uint32 dcb28_rx_classtag_get(dcb_t *dcb)
{
    dcb28_t *d = (dcb28_t *)dcb;
    uint32  hgh[3];
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];
    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    if (h->overlay1.hdr_format != 1) { /* return 0 if not overlay 2 format */
        return 0;
    }
    return((h->overlay2.ctag_hi << 8) | (h->overlay2.ctag_lo << 0));
}

static uint32 dcb28_rx_mcast_get(dcb_t *dcb)
{
    dcb28_t *d = (dcb28_t *)dcb;
    uint32  hgh[3];
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];
    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);
    return((h->overlay1.dst_mod << 5) | (h->overlay1.dst_port << 0));
}

#else
static uint32 dcb28_rx_classtag_get(dcb_t *dcb)
{
    dcb28_t *d = (dcb28_t *)dcb;
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&d->mh0;
    if (h->overlay1.hdr_format != 1) { /* return 0 if not overlay 2 format */
        return 0;
    }
    return((h->overlay2.ctag_hi << 8) | (h->overlay2.ctag_lo << 0));
}

static uint32 dcb28_rx_mcast_get(dcb_t *dcb)
{
    dcb28_t *d = (dcb28_t *)dcb;
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&d->mh0;
    return((h->overlay1.dst_mod << 5) | (h->overlay1.dst_port << 0));
}
#endif
static uint32 dcb28_rx_ingport_get(dcb_t *dcb)
{
    uint32      *p;

	p = (uint32 *)dcb;
	return (p[5] & 0xff);
}

static uint32 dcb28_rx_cos_get(dcb_t *dcb)
{
    uint32      *p;

	p = (uint32 *)dcb;
	return (p[4] & 0x3f);
}


static uint32 dcb28_rx_crc_get(dcb_t *dcb) {
    return 0;
}


static void
dcbArad_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[0] = d[1] = d[2] = d[3] = d[4] = 0;
    d[5] = d[6] = d[7] = d[8] = d[9] = d[10] = 0;
    d[11] = d[12] = d[13] = d[14] = d[15] = 0;
}
 /*uint32      (*rx_error_get)(dcb_t *dcb); */
/*
static uint32 
dcb28_Arad_error_get (dcb_t *dcb)
{
    return 0;
}
*/


static uint32
dcb28_intrinfo(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb28_t      *d = (dcb28_t *)dcb;     /*  DCB */
    uint32      f;                      /* SOC_DCB_INFO_* flags */

    if (!d->done) {
        return 0;
    }
    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }
    *count = d->count;
    return f;
}

static int
dcbArad_addtx(dv_t *dv, sal_vaddr_t addr, uint32 count,
           pbmp_t l2pbm, pbmp_t utpbm, pbmp_t l3pbm, uint32 flags, uint32 *hgh)
{
    dcb28_t      *d;     /* DCB */
    uint32      *di;    /* DCB integer pointer */
    uint32      paddr;  /* Packet buffer physical address */

    d = (dcb28_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (addr) {
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    } else {
        paddr = 0;
    }

    if (dv->dv_vcnt > 0 && (dv->dv_flags & DV_F_COMBINE_DCB) &&
        (d[-1].c_sg != 0) &&
        (d[-1].addr + d[-1].c_count) == paddr &&
        d[-1].c_count + count <= DCB_MAX_REQCOUNT) {
        d[-1].c_count += count;
        return dv->dv_cnt - dv->dv_vcnt;
    }

    if (dv->dv_vcnt >= dv->dv_cnt) {
        return SOC_E_FULL;
    }
    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = 0;
    di[5] = di[6] = di[7] = di[8] = di[9] = di[10] = 0;
    di[11] = di[12] = di[13] = di[14] = di[15] = 0;

    d->addr = paddr;
    d->c_count = count;
    d->c_sg = 1;

    d->c_stat = 1;
    d->c_purge = SOC_DMA_PURGE_GET(flags);
    d->c_hg = SOC_DMA_HG_GET(flags);

    if (SOC_DMA_HG_GET(flags) && (hgh != NULL)) {
        d->mh0 = soc_ntohl(hgh[0]);
        d->mh1 = 0;
        d->mh2 = 0;
        d->mh3 = 0;
    }
	
    dv->dv_vcnt += 1; 

    return dv->dv_cnt - dv->dv_vcnt;
}

static int
dcbArad_addrx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb28_t      *d;     /* DCB */
    uint32      *di;    /* DCB integer pointer */

    d = (dcb28_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = di[4] = 0;
    di[5] = di[6] = di[7] = di[8] = di[9] = di[10] = 0;
    di[11] = di[12] = di[13] = di[14] = di[15] = 0;

    if (addr) {
        d->addr = soc_cm_l2p(dv->dv_unit, (void *)addr);
    }
    d->c_count = count;
    d->c_sg = 1;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

#ifdef BROADCOM_DEBUG
static void
dcb28_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb28_t *d = (dcb28_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
    LOG_CLI((BSL_META_U(unit,
                        "================================================================\n")));
    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    #if 0 
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    #endif
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %ssg %schain %sreload %shg %sstat %spause %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %sstart %send %serror\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!"));
    }

    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb)) &&
        (SOC_DCB_TYPE(unit) == 28)) {
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sadd_vid %sbpdu %scell_error %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->add_vid ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->cell_error ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sl3ipmc %sl3only %sl3uc %spkt_aged %spurge_cell %ssrc_hg\n"),
                 prefix,
                 d->l3ipmc ? "" : "!",
                 d->l3only ? "" : "!",
                 d->l3uc ? "" : "!",
                 d->pkt_aged ? "" : "!",
                 d->purge_cell ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sdecap_iptunnel %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->decap_iptunnel ? "" : "!",
                 d->ingress_untagged ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d l3_intf=%d mtp_index=%d reason=%08x\n"),
                 prefix,
                 d->cpu_cos,
                 d->cos,
                 d->l3_intf,
                 (d->mtp_index_hi << 2) | d->mtp_index_lo,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  match_rule=%d nh_index=%d\n"),
                 prefix,
                 d->match_rule,
                 d->nh_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 (d->dscp_hi << 4) | d->dscp_lo,
                 d->outer_pri,
                 d->outer_cfi,
                 d->outer_vid
                 ));
    }
    LOG_CLI((BSL_META_U(unit,
                        "################################################################\n")));

}
#endif /* BROADCOM_DEBUG */

static uint32
dcb28_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb28_t *d = (dcb28_t *)dcb;
    uint32 hgh[3];
    soc_higig_hdr_t *h = (soc_higig_hdr_t *)&hgh[0];

    hgh[0] = soc_htonl(d->mh0);
    hgh[1] = soc_htonl(d->mh1);
    hgh[2] = soc_htonl(d->mh2);

    return (dt_mode ?
            (ingport_is_hg ? 
             (h->hgp_overlay1.ingress_tagged ? 0 : 2) :
             (h->hgp_overlay1.ingress_tagged ?
              (d->add_vid ? 1 : 0) :
              (d->add_vid ? 3 : 2))) :
            (h->hgp_overlay1.ingress_tagged ? 2 : 3));
}

#ifdef BROADCOM_DEBUG
GETFUNCERR(28, tx_l2pbm) 
GETFUNCERR(28, tx_utpbm) 
GETFUNCERR(28, tx_l3pbm)
GETFUNCERR(28, tx_crc) 
GETFUNCERR(28, tx_cos)
GETFUNCERR(28, tx_destmod)
GETFUNCERR(28, tx_destport)
GETFUNCERR(28, tx_opcode) 
GETFUNCERR(28, tx_srcmod)
GETFUNCERR(28, tx_srcport)
GETFUNCERR(28, tx_prio)
GETFUNCERR(28, tx_pfm)
#endif /* BROADCOM_DEBUG */

SETFUNCFIELD(28, reqcount, c_count, uint32 count, count)
GETFUNCFIELD(28, reqcount, c_count)
GETFUNCFIELD(28, xfercount, count)
/* addr_set, addr_get, paddr_get - Same as DCB 0 */
SETFUNCFIELD(28, done, done, int val, val ? 1 : 0)
GETFUNCFIELD(28, done, done)
SETFUNCFIELD(28, sg, c_sg, int val, val ? 1 : 0)
GETFUNCFIELD(28, sg, c_sg)
SETFUNCFIELD(28, chain, c_chain, int val, val ? 1 : 0)
GETFUNCFIELD(28, chain, c_chain)
SETFUNCFIELD(28, reload, c_reload, int val, val ? 1 : 0)
GETFUNCFIELD(28, reload, c_reload)
SETFUNCERR(28, desc_intr, int)
GETFUNCERR(28, desc_intr)
SETFUNCERR(28, tx_l2pbm, pbmp_t)
SETFUNCERR(28, tx_utpbm, pbmp_t)
SETFUNCERR(28, tx_l3pbm, pbmp_t)
SETFUNCERR(28, tx_crc, int)
SETFUNCERR(28, tx_cos, int)
SETFUNCERR(28, tx_destmod, uint32)
SETFUNCERR(28, tx_destport, uint32)
SETFUNCERR(28, tx_opcode, uint32)
SETFUNCERR(28, tx_srcmod, uint32)
SETFUNCERR(28, tx_srcport, uint32)
SETFUNCERR(28, tx_prio, uint32)
SETFUNCERR(28, tx_pfm, uint32)
GETFUNCFIELD(28, rx_start, start)
GETFUNCFIELD(28, rx_end, end)
GETFUNCFIELD(28, rx_error, error)
/* Fields extracted from MH/PBI */
GETHGFUNCEXPR(28, rx_destmod, (h->overlay1.dst_mod |
                             (h->hgp_overlay1.dst_mod_5 << 5)))
GETHGFUNCFIELD(28, rx_destport, dst_port)
GETHGFUNCFIELD(28, rx_opcode, opcode)
GETHGFUNCFIELD(28, rx_prio, vlan_pri) /* outer_pri */




GETHGFUNCFIELD(28, rx_srcport, src_port)
GETHGFUNCEXPR(28, rx_srcmod, (h->overlay1.src_mod |
                             (h->hgp_overlay1.src_mod_5 << 5)))
/* DCB 9 specific fields */
SETFUNCFIELD(28, hg, c_hg, uint32 hg, hg)
GETFUNCFIELD(28, hg, c_hg)
SETFUNCFIELD(28, stat, c_stat, uint32 stat, stat)
GETFUNCFIELD(28, stat, c_stat)
SETFUNCFIELD(28, purge, c_purge, uint32 purge, purge)
GETFUNCFIELD(28, purge, c_purge)
GETPTREXPR(28, mhp, &(d->mh0))
GETFUNCNULL(28, rx_outer_tag_action)
GETFUNCNULL(28, inner_vid)
GETFUNCNULL(28, inner_pri)
GETFUNCNULL(28, inner_cfi)
GETFUNCNULL(28, rx_inner_tag_action)
GETFUNCFIELD(28, rx_bpdu, bpdu)
GETFUNCFIELD(28, rx_l3_intf, l3_intf)
GETFUNCNULL(28, rx_decap_tunnel)
GETFUNCNULL(28, rx_switch_drop)
GETFUNCNULL(28, olp_encap_oam_pkt)
GETFUNCNULL(28, read_ecc_error)
GETFUNCNULL(28, desc_remaining)
SETFUNCNULL(28, desc_remaining)
GETFUNCNULL(28, desc_status)
SETFUNCNULL(28, desc_status)
GETFUNCNULL(28, match_id_lo)
GETFUNCNULL(28, match_id_hi)
GETFUNCNULL(28, forwarding_type)
GETFUNCNULL(28, forwarding_zone_id)

static soc_rx_reason_t
dcb28_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL2LearnLimit,       /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonMirror,             /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t *dcb28_rx_reason_maps[] = {
    dcb28_rx_reason_map,
    NULL
};

GETFUNCFIELD(28, rx_matchrule, match_rule)
GETFUNCNULL(28, rx_reason_hi)
GETFUNCFIELD(28, rx_reason, reason)
GETFUNCERR(28, rx_vclabel)
GETFUNCERR(28, rx_mirror)
GETFUNCERR(28, rx_timestamp)
GETFUNCERR(28, rx_timestamp_upper)
GETFUNCERR(28, outer_vid)
GETFUNCERR(28, outer_pri)
GETFUNCERR(28, outer_cfi)
#ifdef  PLISIM          /* these routines are only used by pcid */
SETFUNCFIELD(28, rx_ingport, srcport, int val, val)
#endif  /* PLISIM */


dcb_op_t dcb28_op = {
    28,
    sizeof(dcb28_t),
    dcb28_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcbArad_init,
    dcbArad_addtx,
    dcbArad_addrx,
    dcb28_intrinfo,
    dcb28_reqcount_set,
    dcb28_reqcount_get,
    dcb28_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb28_done_set,
    dcb28_done_get,
    dcb28_sg_set,
    dcb28_sg_get,
    dcb28_chain_set,
    dcb28_chain_get,
    dcb28_reload_set,
    dcb28_reload_get,
    dcb28_desc_intr_set,
    dcb28_desc_intr_get,
    dcb28_tx_l2pbm_set,
    dcb28_tx_utpbm_set,
    dcb28_tx_l3pbm_set,
    dcb28_tx_crc_set,
    dcb28_tx_cos_set,
    dcb28_tx_destmod_set,
    dcb28_tx_destport_set,
    dcb28_tx_opcode_set,
    dcb28_tx_srcmod_set,
    dcb28_tx_srcport_set,
    dcb28_tx_prio_set,
    dcb28_tx_pfm_set,
    dcb28_rx_untagged_get,
    dcb28_rx_crc_get,
    dcb28_rx_cos_get,
    dcb28_rx_destmod_get,
    dcb28_rx_destport_get,
    dcb28_rx_opcode_get,
    dcb28_rx_classtag_get,
    dcb28_rx_matchrule_get,
    dcb28_rx_start_get,
    dcb28_rx_end_get,
    dcb28_rx_error_get, 
    /*dcb28_Arad_error_get,*/
    dcb28_rx_prio_get,
    dcb28_rx_reason_get,
    dcb28_rx_reason_hi_get,
    dcb28_rx_ingport_get,
    dcb28_rx_srcport_get,
    dcb28_rx_srcmod_get,
    dcb28_rx_mcast_get,
    dcb28_rx_vclabel_get,
    dcb28_rx_mirror_get,
    dcb28_rx_timestamp_get,
    dcb28_rx_timestamp_upper_get,
    dcb28_hg_set,
    dcb28_hg_get,
    dcb28_stat_set,
    dcb28_stat_get,
    dcb28_purge_set,
    dcb28_purge_get,
    dcb28_mhp_get,
    dcb28_outer_vid_get,
    dcb28_outer_pri_get,
    dcb28_outer_cfi_get,
    dcb28_rx_outer_tag_action_get,
    dcb28_inner_vid_get,
    dcb28_inner_pri_get,
    dcb28_inner_cfi_get,
    dcb28_rx_inner_tag_action_get,
    dcb28_rx_bpdu_get,
    dcb28_rx_l3_intf_get,
    dcb28_rx_decap_tunnel_get,
    dcb28_rx_switch_drop_get,
    dcb28_olp_encap_oam_pkt_get,
    dcb28_read_ecc_error_get,
    dcb28_desc_remaining_get,
    dcb28_desc_remaining_set,
    dcb28_desc_status_get,
    dcb28_desc_status_set,
    dcb28_match_id_lo_get,
    dcb28_match_id_hi_get,
    dcb28_forwarding_type_get,
    dcb28_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb28_tx_l2pbm_get,
    dcb28_tx_utpbm_get,
    dcb28_tx_l3pbm_get,
    dcb28_tx_crc_get,
    dcb28_tx_cos_get,
    dcb28_tx_destmod_get,
    dcb28_tx_destport_get,
    dcb28_tx_opcode_get,
    dcb28_tx_srcmod_get,
    dcb28_tx_srcport_get,
    dcb28_tx_prio_get,
    dcb28_tx_pfm_get,

    dcb28_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb28_status_init,
    dcb28_xfercount_set,
    dcb28_rx_start_set,
    dcb28_rx_end_set,
    dcb28_rx_error_set,
    dcb28_rx_crc_set,
    dcb28_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_PETRA_SUPPORT */


#if defined(BCM_KATANA2_SUPPORT)  
/*
 * DCB Type 29 Support
 */

#ifdef BROADCOM_DEBUG
static void
dcb29_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t     *dtx = (dcb19_t *)dcb;  /* Fake out for different TX MH */
    dcb29_t     *d = (dcb29_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
    uint8       *h;
#if defined(LE_HOST)
    uint32      hgh[4];
    h = (uint8 *)&hgh[0];

    if (tx) {
        hgh[0] = soc_htonl(dtx->mh0);
        hgh[1] = soc_htonl(dtx->mh1);
        hgh[2] = soc_htonl(dtx->mh2);
        hgh[3] = soc_htonl(dtx->mh3);
    } else {
        hgh[0] = soc_htonl(d->mh0);
        hgh[1] = soc_htonl(d->mh1);
        hgh[2] = soc_htonl(d->mh2);
        hgh[3] = soc_htonl(d->mh3);
    }
#else
    if (tx) {
        h = (uint8 *)&dtx->mh0;
    } else {
        h = (uint8 *)&d->mh0;
    }
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %sstat %spause "
             " %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }

    dcb0_reason_dump(unit, dcb, prefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%schg_tos %sregen_crc %schg_ecn %svfi_valid\n"),
             prefix,
             d->word11.overlay1.chg_tos ? "" : "!",
             d->regen_crc ? "" : "!",
             d->chg_ecn ? "" : "!",
             d->vfi_valid ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sservice_tag %sswitch_pkt %shg_type %ssrc_hg\n"),
             prefix,
             d->service_tag ? "" : "!",
             d->switch_pkt ? "" : "!",
             d->hg_type ? "" : "!",
             d->src_hg ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sl3routed %sl3only %sreplicated %sdo_not_change_ttl"
             " %sbpdu\n"),
             prefix,
             d->l3routed ? "" : "!",
             d->l3only ? "" : "!",
             d->replicated ? "" : "!",
             d->do_not_change_ttl ? "" : "!",
             d->bpdu ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%shg2_ext_hdr\n"),
             prefix,
             d->hg2_ext_hdr ? "" : "!"));
    if (d->hg2_ext_hdr) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\tHigig2 Extension: queue_tag=%04x tag_type=%d"
                 " seg_sel=%d\n"),
                 prefix,
                 d->word14.overlay3.eh_queue_tag,
                 d->word14.overlay3.eh_tag_type,
                 d->word14.overlay3.eh_seg_sel));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\treason_type=%d reason=%08x_%08x ts_type=%d"
             " timestamp=%08x_%08x"),
             prefix,
             d->word4.overlay1.cpu_opcode_type,
             d->word2.overlay1.reason_hi,
             d->reason,
             d->timestamp_type,
             d->word14.overlay1.timestamp_hi,
             d->word12.overlay1.timestamp));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tsrcport=%d cpu_cos=%d hgi=%d lb_pkt_type=%d"
             " repl_nhi=%05x\n"),
             prefix,
             d->srcport,
             d->word4.overlay3.queue_num,
             d->hgi,
             d->loopback_pkt_type,
             d->repl_nhi));
    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d otag_action=%d"
             " vntag_action=%d\n"),
             prefix,
             d->word4.overlay1.outer_vid,
             d->word11.overlay1.outer_cfi,
             d->word11.overlay1.outer_pri,
             d->otag_action,
             d->word11.overlay1.vntag_action));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tinner_vid=%d inner_cfi=%d inner_pri=%d itag_action=%d"
             " itag_status=%d\n"),
             prefix,
             d->word11.overlay1.inner_vid,
             d->word11.overlay1.inner_cfi,
             d->inner_pri,
             d->itag_action,
             d->itag_status));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdscp=%d ecn=%d %soam_pkt %solp %sall_switch_drop\n"
                        " ma_ptr=%04x\n"),
             prefix,
             d->word11.overlay1.dscp,
             d->ecn,
             d->oam_pkt ? "" : "!",
             d->word4.overlay3.olp_encap_oam_pkt ? "" : "!",
             d->all_switch_drop ? "" : "!",
             d->word2.overlay2.ma_ptr));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdecap_tunnel_type=%d vfi=%d match_rule=%d"
             " mtp_ind=%d\n"),
             prefix,
             d->word11.overlay1.decap_tunnel_type,
             d->word12.overlay2.vfi,
             d->match_rule,
             d->mtp_index));
}
#endif /* BROADCOM_DEBUG */

/*
 * DCB Type 29 Support
 */
static soc_rx_reason_t
dcb29_rx_reason_map_ip_0[] = { /* IP Overlay 0 */
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonMirror,             /* Offset 18 */ 
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL3AddrBindFail,     /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonBfdSlowpath,        /* Offset 35 */
    socRxReasonStation,            /* Offset 36 */
    socRxReasonVlanTranslate,      /* Offset 37 */
    socRxReasonTimeSync,           /* Offset 38 */
    socRxReasonL2LearnLimit,       /* Offset 39 */
    socRxReasonBfd,                /* Offset 40 */
    socRxReasonFailoverDrop,       /* Offset 41 */
    socRxReasonUnknownSubtendingPort,      /* Offset 42 */
    socRxReasonMplsReservedEntropyLabel,   /* Offset 43 */
    socRxReasonLLTagAbsentDrop,            /* Offset 44 */
    socRxReasonLLTagPresentDrop,           /* Offset 45 */
    socRxReasonMplsLookupsExceeded,        /* Offset 46 */
    socRxReasonMplsIllegalReservedLabel,   /* Offset 47 */
    socRxReasonMplsRouterAlertLabel,       /* Offset 48 */
    socRxReasonMplsUnknownAch,             /* Offset 49 */
    socRxReasonInvalid,                    /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t
dcb29_rx_reason_map_ip_1[] = { /* IP Overlay 1 */
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonMirror,             /* Offset 18 */ 
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL3AddrBindFail,     /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonBfdSlowpath,        /* Offset 35 */
    socRxReasonStation,            /* Offset 36 */
    socRxReasonVlanTranslate,      /* Offset 37 */
    socRxReasonTimeSync,           /* Offset 38 */
    socRxReasonL2LearnLimit,       /* Offset 39 */
    socRxReasonBfd,                /* Offset 40 */
    socRxReasonFailoverDrop,       /* Offset 41 */
    socRxReasonOAMSlowpath,        /* Offset 42 */
    socRxReasonOAMError,           /* Offset 43 */
    socRxReasonOAMLMDM,            /* Offset 44 */
    socRxReasonOAMCCMSlowPath,     /* Offset 45 */
    socRxReasonOAMIncompleteOpcode,/* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonSat,                /* Offset 48 */
    socRxReasonOAMMplsLmDm,        /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};
static soc_rx_reason_t
dcb29_rx_reason_map_ip_3[] = { /* IP Overlay 3 */
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonMirror,             /* Offset 18 */ 
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL3AddrBindFail,     /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonBfdSlowpath,        /* Offset 35 */
    socRxReasonStation,            /* Offset 36 */
    socRxReasonVlanTranslate,      /* Offset 37 */
    socRxReasonTimeSync,           /* Offset 38 */
    socRxReasonL2LearnLimit,       /* Offset 39 */
    socRxReasonBfd,                /* Offset 40 */
    socRxReasonFailoverDrop,       /* Offset 41 */
    socRxReasonNivUntagDrop,       /* Offset 42 */
    socRxReasonNivTagDrop,         /* Offset 43 */
    socRxReasonNivTagInvalid,      /* Offset 44 */
    socRxReasonNivRpfFail,         /* Offset 45 */
    socRxReasonNivInterfaceMiss,   /* Offset 46 */
    socRxReasonNivPrioDrop,        /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
  };
static soc_rx_reason_t
dcb29_rx_reason_map_ep[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonStp,                /* Offset 1 */
    socRxReasonVlanTranslate,      /* Offset 2 new */
    socRxReasonTunnelError,        /* Offset 3 */
    socRxReasonIpmc,               /* Offset 4 */
    socRxReasonL3HeaderError,      /* Offset 5 */
    socRxReasonTtl,                /* Offset 6 */
    socRxReasonL2MtuFail,          /* Offset 7 */
    socRxReasonHigigHdrError,      /* Offset 8 */
    socRxReasonSplitHorizon,       /* Offset 9 */
    socRxReasonNivPrune,           /* Offset 10 */
    socRxReasonVirtualPortPrune,   /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonNonUnicastDrop,     /* Offset 13 */
    socRxReasonTrillPacketPortMismatch, /* Offset 14 */
    socRxReasonOAMError,           /* Offset 15 */
    socRxReasonOAMLMDM,            /* Offset 16 */
    socRxReasonOAMCCMSlowPath,     /* Offset 17 */
    socRxReasonOAMSlowpath,        /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};


static soc_rx_reason_t *dcb29_rx_reason_maps[] = {
    dcb29_rx_reason_map_ip_0,
    dcb29_rx_reason_map_ip_1,
    dcb29_rx_reason_map_ip_3,
    dcb29_rx_reason_map_ep,
    NULL
};
/*
 * Function:
 *      dcb29_rx_reason_map_get
 * Purpose:
 *      Return the RX reason map for DCB 23 type.
 * Parameters:
 *      dcb_op - DCB operations
 *      dcb    - dma control block
 * Returns:
 *      RX reason map pointer
 */
static soc_rx_reason_t *
dcb29_rx_reason_map_get(dcb_op_t *dcb_op, dcb_t *dcb)
{
    soc_rx_reason_t *map = NULL;
    dcb29_t  *d = (dcb29_t *)dcb;

    switch (d->word4.overlay1.cpu_opcode_type) {
    case SOC_CPU_OPCODE_TYPE_IP_0:
        map = dcb29_rx_reason_map_ip_0;
        break;
    case SOC_CPU_OPCODE_TYPE_IP_1:
        map = dcb29_rx_reason_map_ip_1;
        break;
    case SOC_CPU_OPCODE_TYPE_IP_3:
        map = dcb29_rx_reason_map_ip_3;
        break;
    case SOC_CPU_OPCODE_TYPE_EP:
        map = dcb29_rx_reason_map_ep;
        break;
    default:
        /* Unknown reason type */
        break;
    }

    return map;
}

static _shr_rx_decap_tunnel_t dcb29_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_ACCESS_SVP,
    _SHR_RX_DECAP_MIM,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_L2MPLS_1LABEL,
    _SHR_RX_DECAP_L2MPLS_1LABELCW,
    _SHR_RX_DECAP_L3MPLS_1LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABELCW,
}; 
static uint32 dcb29_rx_decap_tunnel_get(dcb_t *dcb) {
    dcb29_t *d = (dcb29_t *)dcb;
    uint32 decap_tunnel_type = d->word11.overlay1.decap_tunnel_type;
    /* Check for array bounds */
    if ( (decap_tunnel_type <  
    (sizeof (dcb29_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb29_rx_decap_tunnel_map[decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}

GETFUNCEXPR(29, rx_l3_intf, ((d->replicated) ? (d->repl_nhi) :
                             (((d->repl_nhi) & 0x3fff) +
                              _SHR_L3_EGRESS_IDX_MIN)))

GETFUNCFIELD(29, outer_vid, word4.overlay1.outer_vid)
GETFUNCFIELD(29, olp_encap_oam_pkt, word4.overlay3.olp_encap_oam_pkt)
GETFUNCNULL(29, read_ecc_error)
GETFUNCNULL(29, desc_remaining)
SETFUNCNULL(29, desc_remaining)
GETFUNCNULL(29, desc_status)
SETFUNCNULL(29, desc_status)
GETFUNCNULL(29, match_id_lo)
GETFUNCNULL(29, match_id_hi)
GETFUNCNULL(29, forwarding_type)
GETFUNCNULL(29, forwarding_zone_id)

dcb_op_t dcb29_op = {
    29,
    sizeof(dcb29_t),
    dcb29_rx_reason_maps,
    dcb29_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb23_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb23_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb23_rx_cos_get,
    dcb23_rx_destmod_get,
    dcb23_rx_destport_get,
    dcb23_rx_opcode_get,
    dcb23_rx_classtag_get,
    dcb23_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb23_rx_prio_get,
    dcb23_rx_reason_get,
    dcb23_rx_reason_hi_get,
    dcb23_rx_ingport_get,
    dcb23_rx_srcport_get,
    dcb23_rx_srcmod_get,
    dcb23_rx_mcast_get,
    dcb23_rx_vclabel_get,
    dcb23_rx_mirror_get,
    dcb23_rx_timestamp_get,
    dcb23_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb23_mhp_get,
    dcb29_outer_vid_get,
    dcb23_outer_pri_get,
    dcb23_outer_cfi_get,
    dcb23_rx_outer_tag_action_get,
    dcb23_inner_vid_get,
    dcb23_inner_pri_get,
    dcb23_inner_cfi_get,
    dcb23_rx_inner_tag_action_get,
    dcb23_rx_bpdu_get,
    dcb29_rx_l3_intf_get,
    dcb29_rx_decap_tunnel_get,
    dcb23_rx_switch_drop_get,
    dcb29_olp_encap_oam_pkt_get,
    dcb29_read_ecc_error_get,
    dcb29_desc_remaining_get,
    dcb29_desc_remaining_set,
    dcb29_desc_status_get,
    dcb29_desc_status_set,
    dcb29_match_id_lo_get,
    dcb29_match_id_hi_get,
    dcb29_forwarding_type_get,
    dcb29_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb29_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb23_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb23_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_KATANA2_SUPPORT */

#if defined(BCM_HURRICANE2_SUPPORT)  
GETFUNCNULL(30, rx_switch_drop)
GETFUNCNULL(30, olp_encap_oam_pkt)
GETFUNCNULL(30, read_ecc_error)
GETFUNCNULL(30, desc_remaining)
SETFUNCNULL(30, desc_remaining)
GETFUNCNULL(30, desc_status)
SETFUNCNULL(30, desc_status)
GETFUNCNULL(30, match_id_lo)
GETFUNCNULL(30, match_id_hi)
GETFUNCNULL(30, forwarding_type)
GETFUNCNULL(30, forwarding_zone_id)

dcb_op_t dcb30_op = {
    30,
    sizeof(dcb20_t),
    dcb20_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb19_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb19_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb19_rx_cos_get,
    dcb19_rx_destmod_get,
    dcb19_rx_destport_get,
    dcb19_rx_opcode_get,
    dcb19_rx_classtag_get,
    dcb20_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb19_rx_prio_get,
    dcb19_rx_reason_get,
    dcb19_rx_reason_hi_get,
    dcb19_rx_ingport_get,
    dcb19_rx_srcport_get,
    dcb19_rx_srcmod_get,
    dcb19_rx_mcast_get,
    dcb19_rx_vclabel_get,
    dcb19_rx_mirror_get,
    dcb20_rx_timestamp_get,
    dcb20_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb19_mhp_get,
    dcb19_outer_vid_get,
    dcb19_outer_pri_get,
    dcb19_outer_cfi_get,
    dcb19_rx_outer_tag_action_get,
    dcb19_inner_vid_get,
    dcb19_inner_pri_get,
    dcb19_inner_cfi_get,
    dcb19_rx_inner_tag_action_get,
    dcb19_rx_bpdu_get,
    dcb9_rx_l3_intf_get,
    dcb19_rx_decap_tunnel_get,
    dcb30_rx_switch_drop_get,
    dcb30_olp_encap_oam_pkt_get,
    dcb30_read_ecc_error_get,
    dcb30_desc_remaining_get,
    dcb30_desc_remaining_set,
    dcb30_desc_status_get,
    dcb30_desc_status_set,
    dcb30_match_id_lo_get,
    dcb30_match_id_hi_get,
    dcb30_forwarding_type_get,
    dcb30_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb20_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb19_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb19_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)  
/*
 * DCB Type 31 Support
 */
static soc_rx_reason_t
dcb31_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL2LearnLimit,       /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonMplsCtrlWordError,  /* Offset 35 */  
    socRxReasonTimeSync,           /* Offset 36 */ 
    socRxReasonOAMSlowpath,        /* Offset 37 */  
    socRxReasonOAMError,           /* Offset 38 */  
    socRxReasonOAMLMDM,            /* Offset 39 */ 
    socRxReasonL3AddrBindFail,     /* Offset 40 */  
    socRxReasonVlanTranslate,      /* Offset 41 */  
    socRxReasonNiv,                /* Offset 42 */ 
    socRxReasonNiv,                /* Offset 43 */ 
    socRxReasonNiv,                /* Offset 44 */ 
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};
static soc_rx_reason_t *dcb31_rx_reason_maps[] = {
    dcb31_rx_reason_map,
    NULL
};

#ifdef BROADCOM_DEBUG
static void
dcb31_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t     *dtx = (dcb19_t *)dcb;  /* Fake out for different TX MH */
    dcb31_t *d = (dcb31_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
    uint8       *h;
#if defined(LE_HOST)
    uint32  hgh[4];
    h = (uint8 *)&hgh[0];

    if (tx) {
        hgh[0] = soc_htonl(dtx->mh0);
        hgh[1] = soc_htonl(dtx->mh1);
        hgh[2] = soc_htonl(dtx->mh2);
        hgh[3] = soc_htonl(dtx->mh3);
    } else {
        hgh[0] = soc_htonl(d->mh0);
        hgh[1] = soc_htonl(d->mh1);
        hgh[2] = soc_htonl(d->mh2);
        hgh[3] = soc_htonl(d->mh3);
    }
#else
    if (tx) {
        h = (uint8 *)&dtx->mh0;
    } else {
        h = (uint8 *)&d->mh0;
    }
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %ssg %schain %sreload %shg %sstat %spause %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %sstart %send %serror\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!"));
    }
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb))) {
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sdo_not_change_ttl %sbpdu %sl3routed %schg_tos %semirror %simirror\n"),
                 prefix,
                 d->do_not_change_ttl ? "" : "!",
                 d->bpdu ? "" : "!",
                 d->l3routed ? "" : "!",
                 d->chg_tos ? "" : "!",
                 d->emirror ? "" : "!",
                 d->imirror ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sreplicated %sl3only %soam_pkt %ssrc_hg\n"),
                 prefix,
                 d->replicated ? "" : "!",
                 d->l3only ? "" : "!",
                 d->oam_pkt ? "" : "!",
                 d->src_hg ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %sswitch_pkt %sregen_crc %sservice_tag %sing_untagged\n"),
                 prefix,
                 d->switch_pkt ? "" : "!",
                 d->regen_crc ? "" : "!",
                 d->service_tag ? "" : "!",
                 !(((soc_higig2_hdr_t *)h)->ppd_overlay1.ingress_tagged) ? "" : "!"
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  %svfi_valid %schg_ecn ecn=%d\n"),
                 prefix,
                 d->vfi_valid ? "" : "!",
                 d->chg_ecn ? "" : "!",
                 d->ecn
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  cpu_cos=%d cos=%d mtp_index=%d reason=%08x_%08x\n"),
                 prefix,
                 d->word4.overlay2.cpu_cos,
                 d->cos,
                 d->mtp_index,
                 d->reason_hi,
                 d->reason
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  reason_type=%d match_rule=%d hg_type=%d mtp_index=%d\n"),
                 prefix,
                 d->word4.overlay1.cpu_opcode_type,
                 d->match_rule,
                 d->hg_type,
                 d->mtp_index
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
                 prefix,
                 d->srcport,
                 d->dscp,
                 d->outer_pri,
                 d->outer_cfi,
                 d->word4.overlay1.outer_vid
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  inner_pri=%d inner_cfi=%d inner_vid=%d\n"),
                 prefix,
                 d->inner_pri,
                 d->inner_cfi,
                 d->inner_vid
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  hgi=%d itag_status=%d otag_action=%d itag_action=%d\n"),
                 prefix,
                 d->hgi,
                 d->itag_status,
                 d->otag_action,
                 d->itag_action
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  repl_nhi=%05x ts_type=%d timestamp=%08x_%08x\n"),
                 prefix,
                 d->repl_nhi,
                 d->timestamp_type,
                 d->timestamp_hi,
                 d->word12.overlay1.timestamp
                 ));
        LOG_CLI((BSL_META_U(unit,
                            "%s  vfi=%d em_mtp_index=%d vntag_action=%d\n"),
                 prefix,
                 d->word12.overlay2.vfi,
                 d->em_mtp_index,
                 d->vntag_action
                 ));
    }
}
#endif /* BROADCOM_DEBUG */

static uint32
dcb31_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb31_t *d = (dcb31_t *)dcb;

    COMPILER_REFERENCE(dt_mode);

    return (ingport_is_hg ?
            ((d->itag_status) ? 0 : 2) :
            ((d->itag_status & 0x2) ?
             ((d->itag_status & 0x1) ? 0 : 2) :
             ((d->itag_status & 0x1) ? 1 : 3)));
}

GETFUNCEXPR(31, rx_mirror, ((d->imirror) | (d->emirror)))

GETFUNCFIELD(31, rx_cos, word4.overlay2.cpu_cos)
GETFUNCFIELD(31, outer_vid, word4.overlay1.outer_vid)
GETFUNCFIELD(31, outer_pri, outer_pri)
GETFUNCFIELD(31, outer_cfi, outer_cfi)
GETFUNCFIELD(31, rx_outer_tag_action, otag_action)
GETFUNCFIELD(31, inner_vid, inner_vid)
GETFUNCFIELD(31, inner_pri, inner_pri)
GETFUNCFIELD(31, inner_cfi, inner_cfi)
GETFUNCFIELD(31, rx_inner_tag_action, itag_action)
GETFUNCFIELD(31, rx_bpdu, bpdu)

GETFUNCFIELD(31, rx_timestamp_upper, timestamp_hi)
GETFUNCEXPR(31, rx_l3_intf, ((d->replicated) ? (d->repl_nhi) :
        (((d->repl_nhi) & 0x3ff) + _SHR_L3_EGRESS_IDX_MIN)))
GETFUNCNULL(31, rx_switch_drop)
GETFUNCNULL(31, olp_encap_oam_pkt)
GETFUNCNULL(31, read_ecc_error)
GETFUNCNULL(31, desc_remaining)
SETFUNCNULL(31, desc_remaining)
GETFUNCNULL(31, desc_status)
SETFUNCNULL(31, desc_status)
GETFUNCNULL(31, match_id_lo)
GETFUNCNULL(31, match_id_hi)
GETFUNCNULL(31, forwarding_type)
GETFUNCNULL(31, forwarding_zone_id)

#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb31_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[2] = d[3] = d[4] = d[5] = d[10] = 0;
    d[11] = d[12] = d[13] = d[14] = d[15] = 0;
}
#endif  /* PLISIM */
    
dcb_op_t dcb31_op = {
    31,
    sizeof(dcb31_t),    
    dcb31_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb19_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb31_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb31_rx_cos_get,
    dcb23_rx_destmod_get,
    dcb23_rx_destport_get,
    dcb23_rx_opcode_get,
    dcb23_rx_classtag_get,
    dcb23_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb23_rx_prio_get,
    dcb23_rx_reason_get,
    dcb23_rx_reason_hi_get,
    dcb23_rx_ingport_get,
    dcb23_rx_srcport_get,
    dcb23_rx_srcmod_get,
    dcb23_rx_mcast_get,
    dcb23_rx_vclabel_get,
    dcb31_rx_mirror_get,
    dcb23_rx_timestamp_get,
    dcb31_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb23_mhp_get,
    dcb31_outer_vid_get,
    dcb31_outer_pri_get,
    dcb31_outer_cfi_get,
    dcb31_rx_outer_tag_action_get,
    dcb31_inner_vid_get,
    dcb31_inner_pri_get,
    dcb31_inner_cfi_get,
    dcb31_rx_inner_tag_action_get,
    dcb31_rx_bpdu_get,
    dcb31_rx_l3_intf_get,
    dcb19_rx_decap_tunnel_get,
    dcb31_rx_switch_drop_get,
    dcb31_olp_encap_oam_pkt_get,
    dcb31_read_ecc_error_get,
    dcb31_desc_remaining_get,
    dcb31_desc_remaining_set,
    dcb31_desc_status_get,
    dcb31_desc_status_set,
    dcb31_match_id_lo_get,
    dcb31_match_id_hi_get,
    dcb31_forwarding_type_get,
    dcb31_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb31_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb31_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb23_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
static uint32
dcb32_rx_hg2_ext_eh_type_get(dcb_t *dcb)
{
   uint32 eh_type = 0;
   dcb32_t *d = (dcb32_t *)dcb;

   if (d->hg2_ext_hdr) {
      switch (d->word14.overlay1.eh_type) {
         case 0:
              eh_type = d->word14.overlay1.eh_queue_tag;
              break;
         default:
              break;
      }
   } 
   return eh_type;
}

/*
 * DCB Type 32 Support
 */
/* From FORMAT CPU_OPCODES */
static soc_rx_reason_t
dcb32_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /*  0: CPU_UVLAN */
    socRxReasonL2SourceMiss,       /*  1: CPU_SLF */
    socRxReasonL2DestMiss,         /*  2: CPU_DLF */
    socRxReasonL2Move,             /*  3: CPU_L2MOVE */
    socRxReasonL2Cpu,              /*  4: CPU_L2CPU */
    socRxReasonSampleSource,       /*  5: CPU_SFLOW_SRC */
    socRxReasonSampleDest,         /*  6: CPU_SFLOW_DST */
    socRxReasonL3SourceMiss,       /*  7: CPU_L3SRC_MISS */
    socRxReasonL3DestMiss,         /*  8: CPU_L3DST_MISS */
    socRxReasonL3SourceMove,       /*  9: CPU_L3SRC_MOVE */
    socRxReasonMcastMiss,          /* 10: CPU_MC_MISS */
    socRxReasonIpMcastMiss,        /* 11: CPU_IPMC_MISS */
    socRxReasonFilterMatch,        /* 12: CPU_FFP */
    socRxReasonL3HeaderError,      /* 13: CPU_L3HDR_ERR */
    socRxReasonProtocol,           /* 14: CPU_PROTOCOL_PKT */
    socRxReasonDosAttack,          /* 15: CPU_DOS_ATTACK */
    socRxReasonMartianAddr,        /* 16: CPU_MARTIAN_ADDR */
    socRxReasonTunnelError,        /* 17: CPU_TUNNEL_ERR */
    socRxReasonSampleSourceFlex,   /* 18: CPU_SFLOW_FLEX */
    socRxReasonIcmpRedirect,       /* 19: ICMP_REDIRECT */
    socRxReasonL3Slowpath,         /* 20: L3_SLOWPATH */
    socRxReasonParityError,        /* 21: PARITY_ERROR */
    socRxReasonL3MtuFail,          /* 22: L3_MTU_CHECK_FAIL */
    socRxReasonHigigHdrError,      /* 23: HGHDR_ERROR */
    socRxReasonMcastIdxError,      /* 24: MCIDX_ERROR */
    socRxReasonVlanFilterMatch,    /* 25: VFP */
    socRxReasonClassBasedMove,     /* 26: CBSM_PREVENTED */
    socRxReasonL3AddrBindFail,     /* 27: MAC_BIND_FAIL */
    socRxReasonMplsLabelMiss,      /* 28: MPLS_LABEL_MISS */
    socRxReasonMplsInvalidAction,  /* 29: MPLS_INVALID_ACTION */
    socRxReasonMplsInvalidPayload, /* 30: MPLS_INVALID_PAYLOAD */
    socRxReasonMplsTtl,            /* 31: MPLS_TTL_CHECK_FAIL */
    socRxReasonMplsSequenceNumber, /* 32: MPLS_SEQ_NUM_FAIL */
    socRxReasonL2NonUnicastMiss,   /* 33: PBT_NONUC_PKT */
    socRxReasonNhop,               /* 34: L3_NEXT_HOP */
    socRxReasonMplsUnknownAch,     /* 35: MPLS_UNKNOWN_ACH_ERROR */
    socRxReasonStation,            /* 36: MY_STATION */
    socRxReasonNiv,                /* 37: NIV_DROP_REASON_ENCODING */
    socRxReasonNiv,                /* 38: ->  */
    socRxReasonNiv,                /* 39: 3-bit */
    socRxReasonVlanTranslate,      /* 40: XLT_MISS */
    socRxReasonTimeSync,           /* 41: TIME_SYNC */
    socRxReasonOAMSlowpath,        /* 42: OAM_SLOWPATH */
    socRxReasonOAMError,           /* 43: OAM_ERROR */
    socRxReasonTrill,              /* 44: TRILL_DROP_REASON_ENCODING */
    socRxReasonTrill,              /* 45: -> */
    socRxReasonTrill,              /* 46: 3-bit */
    socRxReasonL2GreSipMiss,       /* 47: L2GRE_SIP_MISS */
    socRxReasonL2GreVpnIdMiss,     /* 48: L2GRE_VPNID_MISS */
    socRxReasonBfdSlowpath,        /* 49: BFD_SLOWPATH */
    socRxReasonBfd,                /* 50: BFD_ERROR */
#if 1
    socRxReasonInvalid,            /* 51: NOT DEFINED */
#endif
    socRxReasonCongestionCnm,      /* 52: ICNM */
    socRxReasonMplsIllegalReservedLabel, /* 53: MPLS_ILLEGAL_RESERVED_LABEL */
    socRxReasonMplsRouterAlertLabel, /* 54: MPLS_ALERT_LABEL */
    socRxReasonCongestionCnmProxy, /* 55: QCN_CNM_PRP */
    socRxReasonCongestionCnmProxyError, /* 56: QCN_CNM_PRP_DLF */
    socRxReasonVxlanSipMiss,       /* 57: VXLAN_SIP_MISS */
    socRxReasonVxlanVpnIdMiss,     /* 58: VXLAN_VN_ID_MISS */
    socRxReasonFcoeZoneCheckFail,  /* 59: FCOE_ZONE_CHECK_FAIL */
    socRxReasonNat,                /* 60: NAT_DROP_REASON_ENCODING */
    socRxReasonNat,                /* 61: -> */
    socRxReasonNat,                /* 62: 3-bit */
    socRxReasonIpmcInterfaceMismatch /* 63: CPU_IPMC_INTERFACE_MISMATCH */
};

static soc_rx_reason_t *dcb32_rx_reason_maps[] = {
    dcb32_rx_reason_map,
    dcb21_rx_reason_niv_encoding_map,
    dcb26_rx_reason_trill_encoding_map,
    dcb26_rx_reason_nat_encoding_map,
    NULL
};

static _shr_rx_decap_tunnel_t dcb32_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_ACCESS_SVP,
    _SHR_RX_DECAP_MIM,
    _SHR_RX_DECAP_L2GRE,
    _SHR_RX_DECAP_VXLAN,
    _SHR_RX_DECAP_AMT,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_TRILL,
    _SHR_RX_DECAP_L2MPLS_1LABEL,
    _SHR_RX_DECAP_L2MPLS_2LABEL,
    _SHR_RX_DECAP_L2MPLS_1LABELCW,
    _SHR_RX_DECAP_L2MPLS_2LABELCW,
    _SHR_RX_DECAP_L3MPLS_1LABEL,
    _SHR_RX_DECAP_L3MPLS_2LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABELCW,
    _SHR_RX_DECAP_L3MPLS_2LABELCW
};
static uint32 dcb32_rx_decap_tunnel_get(dcb_t *dcb) {
    dcb32_t *d = (dcb32_t *)dcb;
    /* Check for array bounds */
    if ( (d->decap_tunnel_type <
    (sizeof (dcb32_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb32_rx_decap_tunnel_map[d->decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}

/* Fields extracted from MH/PBI */
GETFUNCFIELD(32, rx_ingport, srcport)
#ifdef  PLISIM          /* these routines are only used by pcid */
SETFUNCFIELD(32, rx_ingport, srcport, int val, val)
#endif  /* PLISIM */

#ifdef BROADCOM_DEBUG
static void
dcb32_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t     *dtx = (dcb19_t *)dcb;  /* Fake out for different TX MH */
    dcb32_t     *d = (dcb32_t *)dcb;
    char        ps[((DCB_MAX_SIZE / sizeof(uint32)) * 9) + 1];
    uint8       *h;
#if defined(LE_HOST)
    uint32      hgh[4];
    h = (uint8 *)&hgh[0];

    if (tx) {
        hgh[0] = soc_htonl(dtx->mh0);
        hgh[1] = soc_htonl(dtx->mh1);
        hgh[2] = soc_htonl(dtx->mh2);
        hgh[3] = soc_htonl(dtx->mh3);
    } else {
        hgh[0] = soc_htonl(d->mh0);
        hgh[1] = soc_htonl(d->mh1);
        hgh[2] = soc_htonl(d->mh2);
        hgh[3] = soc_htonl(d->mh3);
    }
#else
    if (tx) {
        h = (uint8 *)&dtx->mh0;
    } else {
        h = (uint8 *)&d->mh0;
    }
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i * 9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %sdescintr %shg "
                        "%sstat %spause %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_DESC_INTR_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }

    dcb0_reason_dump(unit, dcb, prefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%schg_tos %sregen_crc %schg_ecn %svfi_valid "
                        "%sdvp_nhi_sel\n"),
             prefix,
             d->chg_tos ? "" : "!",
             d->regen_crc ? "" : "!",
             d->chg_ecn ? "" : "!",
             d->vfi_valid ? "" : "!",
             d->dvp_nhi_sel ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sservice_tag %sswitch_pkt %shg_type %ssrc_hg\n"),
             prefix,
             d->service_tag ? "" : "!",
             d->switch_pkt ? "" : "!",
             d->hg_type ? "" : "!",
             d->src_hg ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sl3routed %sl3only %sreplicated %sdo_not_change_ttl "
                        "%sbpdu\n"),
             prefix,
             d->l3routed ? "" : "!",
             d->l3only ? "" : "!",
             d->replicated ? "" : "!",
             d->do_not_change_ttl ? "" : "!",
             d->bpdu ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%shg2_ext_hdr %sspecial_pkt %suc_switch_drop\n"),
             prefix,
             d->hg2_ext_hdr ? "" : "!",
             d->special_pkt ? "" : "!",
             d->uc_switch_drop ? "" : "!"));
    if (d->hg2_ext_hdr) {
        switch (d->word14.overlay1.eh_type) {
        case 0:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 0: queue_tag=0x%04x "
                                "tag_type=%d seg_sel=%d\n"),
                     prefix,
                     d->word14.overlay1.eh_queue_tag,
                     d->word14.overlay1.eh_tag_type,
                     d->word14.overlay1.eh_seg_sel));
            break;
        case 1:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 1: classid=%d l3_iif=%d "
                                "classid_type=%d\n"),
                     prefix,
                     d->word14.overlay2.classid,
                     d->word14.overlay2.l3_iif,
                     d->word14.overlay2.classid_type));
            break;
        case 2:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 2: classid=%d "
                                "classid_type=%d\n"),
                     prefix,
                     d->word14.overlay2.classid,
                     d->word14.overlay2.classid_type));
            break;
        default:
            break;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\treason_type=%d reason=%08x_%08x\n"),
             prefix,
             d->cpu_opcode_type,
             d->reason_hi,
             d->reason));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tts_type=%d timestamp=%08x_%08x\n"),
             prefix,
             d->timestamp_type,
             d->word14.timestamp_hi,
             d->timestamp));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tsrcport=%d cos=%d hgi=%d lb_pkt_type=%d "
                        "repl_nhi=%05x\n"),
             prefix,
             d->srcport,
             d->cos,
             d->hgi,
             d->loopback_pkt_type,
             d->repl_nhi));
    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d otag_action=%d "
                        "vntag_action=%d\n"),
             prefix,
             d->outer_vid,
             d->outer_cfi,
             d->outer_pri,
             d->otag_action,
             d->vntag_action));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tinner_vid=%d inner_cfi=%d inner_pri=%d itag_action=%d "
                        "tag_status=%d\n"),
             prefix,
             d->inner_vid,
             d->inner_cfi,
             d->inner_pri,
             d->itag_action,
             d->tag_status));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdscp=%d ecn=%d decap_tunnel_type=%d match_rule=%d "
                        "mtp_ind=%d\n"),
             prefix,
             d->dscp,
             d->ecn,
             d->decap_tunnel_type,
             d->match_rule,
             d->mtp_index));
}
#endif /* BROADCOM_DEBUG */

GETFUNCFIELD(32, rx_cos, cos)
GETFUNCFIELD(32, outer_vid, outer_vid)
GETFUNCFIELD(32, rx_switch_drop, uc_switch_drop)
GETFUNCNULL(32, olp_encap_oam_pkt)
GETFUNCNULL(32, read_ecc_error)
GETFUNCNULL(32, desc_remaining)
SETFUNCNULL(32, desc_remaining)
GETFUNCNULL(32, desc_status)
SETFUNCNULL(32, desc_status)
GETFUNCNULL(32, match_id_lo)
GETFUNCNULL(32, match_id_hi)
GETFUNCNULL(32, forwarding_type)
GETFUNCNULL(32, forwarding_zone_id)

static void
dcb32_desc_intr_set(dcb_t * dcb, int val)
{
    dcb32_t * d = (dcb32_t *)dcb;

    d->c_intr = val;
    return;
}

static uint32
dcb32_desc_intr_get(dcb_t * dcb)
{
    dcb32_t * d = (dcb32_t *)dcb;
    return (d->c_intr);
}

/*
 * DCB Type 32 Support
 */
dcb_op_t dcb32_op = {
    32,
    sizeof(dcb32_t),
    dcb32_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb21_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb26_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb32_desc_intr_set,
    dcb32_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb26_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb32_rx_cos_get,
    dcb26_rx_destmod_get,
    dcb26_rx_destport_get,
    dcb26_rx_opcode_get,
    dcb26_rx_classtag_get,
    dcb26_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb26_rx_prio_get,
    dcb26_rx_reason_get,
    dcb26_rx_reason_hi_get,
    dcb32_rx_ingport_get,
    dcb26_rx_srcport_get,
    dcb26_rx_srcmod_get,
    dcb26_rx_mcast_get,
    dcb26_rx_vclabel_get,
    dcb26_rx_mirror_get,
    dcb26_rx_timestamp_get,
    dcb26_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb26_mhp_get,
    dcb32_outer_vid_get,
    dcb26_outer_pri_get,
    dcb26_outer_cfi_get,
    dcb23_rx_outer_tag_action_get,
    dcb26_inner_vid_get,
    dcb26_inner_pri_get,
    dcb26_inner_cfi_get,
    dcb23_rx_inner_tag_action_get,
    dcb26_rx_bpdu_get,
    dcb26_rx_l3_intf_get,
    dcb32_rx_decap_tunnel_get,
    dcb32_rx_switch_drop_get,
    dcb32_olp_encap_oam_pkt_get,
    dcb32_read_ecc_error_get,
    dcb32_desc_remaining_get,
    dcb32_desc_remaining_set,
    dcb32_desc_status_get,
    dcb32_desc_status_set,
    dcb32_match_id_lo_get,
    dcb32_match_id_hi_get,
    dcb32_forwarding_type_get,
    dcb32_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb32_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb23_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb32_rx_ingport_set,
#endif /* PLISIM */
    dcb32_rx_hg2_ext_eh_type_get,
};
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
 * DCB Type 33 Support
 */
/* From FORMAT CPU_OPCODES */
static soc_rx_reason_t
dcb33_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /*  0: CPU_UVLAN */
    socRxReasonL2SourceMiss,       /*  1: CPU_SLF */
    socRxReasonL2DestMiss,         /*  2: CPU_DLF */
    socRxReasonL2Move,             /*  3: CPU_L2MOVE */
    socRxReasonL2Cpu,              /*  4: CPU_L2CPU */
    socRxReasonSampleSource,       /*  5: CPU_SFLOW_SRC */
    socRxReasonSampleDest,         /*  6: CPU_SFLOW_DST */
    socRxReasonL3SourceMiss,       /*  7: CPU_L3SRC_MISS */
    socRxReasonL3DestMiss,         /*  8: CPU_L3DST_MISS */
    socRxReasonL3SourceMove,       /*  9: CPU_L3SRC_MOVE */
    socRxReasonMcastMiss,          /* 10: CPU_MC_MISS */
    socRxReasonIpMcastMiss,        /* 11: CPU_IPMC_MISS */
    socRxReasonFilterMatch,        /* 12: CPU_FFP */
    socRxReasonL3HeaderError,      /* 13: CPU_L3HDR_ERR */
    socRxReasonProtocol,           /* 14: CPU_PROTOCOL_PKT */
    socRxReasonDosAttack,          /* 15: CPU_DOS_ATTACK */
    socRxReasonMartianAddr,        /* 16: CPU_MARTIAN_ADDR */
    socRxReasonTunnelError,        /* 17: CPU_TUNNEL_ERR */
    socRxReasonFailoverDrop,       /* 18: FAILOVER_DROP */
    socRxReasonIcmpRedirect,       /* 19: ICMP_REDIRECT */
    socRxReasonL3Slowpath,         /* 20: L3_SLOWPATH */
    socRxReasonParityError,        /* 21: PARITY_ERROR */
    socRxReasonL3MtuFail,          /* 22: L3_MTU_CHECK_FAIL */
    socRxReasonHigigHdrError,      /* 23: HGHDR_ERROR */
    socRxReasonMcastIdxError,      /* 24: MCIDX_ERROR */
    socRxReasonVlanFilterMatch,    /* 25: VFP */
    socRxReasonClassBasedMove,     /* 26: CBSM_PREVENTED */
    socRxReasonL3AddrBindFail,     /* 27: MAC_BIND_FAIL */
    socRxReasonMplsLabelMiss,      /* 28: MPLS_LABEL_MISS */
    socRxReasonMplsInvalidAction,  /* 29: MPLS_INVALID_ACTION */
    socRxReasonMplsInvalidPayload, /* 30: MPLS_INVALID_PAYLOAD */
    socRxReasonMplsTtl,            /* 31: MPLS_TTL_CHECK_FAIL */
    socRxReasonMplsSequenceNumber, /* 32: MPLS_SEQ_NUM_FAIL */
    socRxReasonL2NonUnicastMiss,   /* 33: PBT_NONUC_PKT */
    socRxReasonNhop,               /* 34: L3_NEXT_HOP */
    socRxReasonMplsUnknownAch,     /* 35: MPLS_UNKNOWN_ACH_ERROR */
    socRxReasonStation,            /* 36: MY_STATION */
    socRxReasonNiv,                /* 37: NIV_DROP_REASON_ENCODING */
    socRxReasonNiv,                /* 38: ->  */
    socRxReasonNiv,                /* 39: 3-bit */
    socRxReasonVlanTranslate,      /* 40: VXLT_MISS */
    socRxReasonTimeSync,           /* 41: TIME_SYNC */
    socRxReasonOAMSlowpath,        /* 42: OAM_SLOWPATH */
    socRxReasonOAMError,           /* 43: OAM_ERROR */
    socRxReasonTrill,              /* 44: TRILL_DROP_REASON_ENCODING */
    socRxReasonTrill,              /* 45: -> */
    socRxReasonTrill,              /* 46: 3-bit */
    socRxReasonL2GreSipMiss,       /* 47: L2GRE_SIP_MISS */
    socRxReasonL2GreVpnIdMiss,     /* 48: L2GRE_VPNID_MISS */
    socRxReasonBfdSlowpath,        /* 49: BFD_SLOWPATH */
    socRxReasonBfd,                /* 50: BFD_ERROR */
    socRxReasonOAMLMDM,            /* 51: OAM_LMDM */
    socRxReasonCongestionCnm,      /* 52: ICNM */
    socRxReasonMplsIllegalReservedLabel, /* 53: MPLS_ILLEGAL_RESERVED_LABEL */
    socRxReasonMplsRouterAlertLabel, /* 54: MPLS_ALERT_LABEL */
    socRxReasonCongestionCnmProxy, /* 55: QCN_CNM_PRP */
    socRxReasonCongestionCnmProxyError, /* 56: QCN_CNM_PRP_DLF */
    socRxReasonVxlanSipMiss,       /* 57: VXLAN_SIP_MISS */
    socRxReasonVxlanVpnIdMiss,     /* 58: VXLAN_VN_ID_MISS */
    socRxReasonFcoeZoneCheckFail,  /* 59: FCOE_ZONE_CHECK_FAIL */
    socRxReasonNat,                /* 60: NAT_DROP_REASON_ENCODING */
    socRxReasonNat,                /* 61: -> */
    socRxReasonNat,                /* 62: 3-bit */
    socRxReasonIpmcInterfaceMismatch /* 63: CPU_IPMC_INTERFACE_MISMATCH */
};

/* From FORMAT NIV_CPU_OPCODE_ENCODING  */
static soc_rx_reason_t dcb33_rx_reason_niv_encoding_map[] = {
    socRxReasonNiv,                  /* 0: NO_ERRORS
                                      * Base field, must match the entries above */
    socRxReasonNivPrioDrop,          /* 1:DOT1P_ADMITTANCE_DISCARD */
    socRxReasonNivInterfaceMiss,     /* 2:VIF_LOOKUP_MISS */
    socRxReasonNivRpfFail,           /* 3:RPF_LOOKUP_MISS */
    socRxReasonNivTagInvalid,        /* 4:VNTAG_FORMAT_ERROR */
    socRxReasonNivTagDrop,           /* 5:VNTAG_PRESENT_DROP */
    socRxReasonNivUntagDrop,         /* 6:VNTAG_NOT_PRESENT_DROP */
    socRxReasonUnknownSubtendingPort /* 7:UNKOWN SUBTENDING PORT */
};

/* From FORMAT NAT_CPU_OPCODE_ENCODING */
static soc_rx_reason_t dcb33_rx_reason_nat_encoding_map[] = {
    socRxReasonNat,                /* 0:NOP
                                    * Base field, must match the entries above */
    socRxReasonTcpUdpNatMiss,      /* 1:NORMAL */
    socRxReasonIcmpNatMiss,        /* 2:ICMP */
    socRxReasonNatFragment,        /* 3:FRAGMEMT */
    socRxReasonNatMiss,            /* 4:OTHER */
    socRxReasonInvalid,            /* 5:NOT DEFINED */
    socRxReasonInvalid,            /* 6:NOT DEFINED */
#if 1
    socRxReasonInvalid             /* 7:NOT DEFINED */
#endif
};

static soc_rx_reason_t *dcb33_rx_reason_maps[] = {
    dcb33_rx_reason_map,
    dcb33_rx_reason_niv_encoding_map,
    dcb26_rx_reason_trill_encoding_map,
    dcb33_rx_reason_nat_encoding_map,
    NULL
};

static _shr_rx_decap_tunnel_t dcb33_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_ACCESS_SVP,
    _SHR_RX_DECAP_MIM,
    _SHR_RX_DECAP_L2GRE,
    _SHR_RX_DECAP_VXLAN,
    _SHR_RX_DECAP_AMT,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_TRILL,
    _SHR_RX_DECAP_MPLS_1LABELCW,
    _SHR_RX_DECAP_MPLS_2LABELCW,
    _SHR_RX_DECAP_MPLS_1LABEL,
    _SHR_RX_DECAP_MPLS_2LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABEL,
    _SHR_RX_DECAP_L3MPLS_2LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABELCW,
    _SHR_RX_DECAP_L3MPLS_2LABELCW
}; 
static uint32 dcb33_rx_decap_tunnel_get(dcb_t *dcb) {
    dcb33_t *d = (dcb33_t *)dcb;
    /* Check for array bounds */
    if ( (d->word3.overlay1.decap_tunnel_type <  
    (sizeof (dcb33_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb33_rx_decap_tunnel_map[d->word3.overlay1.decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}
GETFUNCFIELD(33, outer_vid, word4.overlay1.outer_vid)

static void
dcb33_desc_intr_set(dcb_t * dcb, int val)
{
    dcb33_t * d = (dcb33_t *)dcb;

    d->c_intr = val;
    return;
}

static uint32
dcb33_desc_intr_get(dcb_t * dcb)
{
    dcb33_t * d = (dcb33_t *)dcb;
    return (d->c_intr);
}

#ifdef BROADCOM_DEBUG
static void
dcb33_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t     *dtx = (dcb19_t *)dcb;  /* Fake out for different TX MH */
    dcb33_t     *d = (dcb33_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
    uint8       *h;
#if defined(LE_HOST)
    uint32      hgh[4];
    h = (uint8 *)&hgh[0];

    if (tx) {
        hgh[0] = soc_htonl(dtx->mh0);
        hgh[1] = soc_htonl(dtx->mh1);
        hgh[2] = soc_htonl(dtx->mh2);
        hgh[3] = soc_htonl(dtx->mh3);
    } else {
        hgh[0] = soc_htonl(d->mh0);
        hgh[1] = soc_htonl(d->mh1);
        hgh[2] = soc_htonl(d->mh2);
        hgh[3] = soc_htonl(d->mh3);
    }
#else
    if (tx) {
        h = (uint8 *)&dtx->mh0;
    } else {
        h = (uint8 *)&d->mh0;
    }
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %sstat %spause "
             " %spurge\n"),
             prefix,
             SOC_DCB_TYPE(unit),
             SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
             SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
             SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
             SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
             d->c_pause ? "" : "!",
             SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }

    dcb0_reason_dump(unit, dcb, prefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%schg_tos %sregen_crc %schg_ecn %svfi_valid "
             "%sdvp_nhi_sel\n"),
             prefix,
             d->word3.overlay1.chg_tos ? "" : "!",
             d->regen_crc ? "" : "!",
             d->chg_ecn ? "" : "!",
             d->vfi_valid ? "" : "!",
             d->dvp_nhi_sel ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sservice_tag %sswitch_pkt %shg_type %ssrc_hg\n"),
             prefix,
             d->service_tag ? "" : "!",
             d->switch_pkt ? "" : "!",
             d->hg_type ? "" : "!",
             d->src_hg ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sl3routed %sl3only %sreplicated %sdo_not_change_ttl "
             "%sbpdu\n"),
             prefix,
             d->l3routed ? "" : "!",
             d->l3only ? "" : "!",
             d->replicated ? "" : "!",
             d->do_not_change_ttl ? "" : "!",
             d->bpdu ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%shg2_ext_hdr %sspecial_pkt %sall_switch_drop\n"),
             prefix,
             d->hg2_ext_hdr ? "" : "!",
             d->special_pkt ? "" : "!",
             d->all_switch_drop ? "" : "!"));
    if (d->hg2_ext_hdr) {
        switch (d->word14.overlay1.eh_type) {
        case 0:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 0: queue_tag=0x%04x "
                     "tag_type=%d seg_sel=%d\n"),
                     prefix,
                     d->word14.overlay1.eh_queue_tag,
                     d->word14.overlay1.eh_tag_type,
                     d->word14.overlay1.eh_seg_sel));
            break;
        case 1:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 1: classid=%d l3_iif=%d "
                     "classid_type=%d\n"),
                     prefix,
                     d->word14.overlay2.classid,
                     d->word14.overlay2.l3_iif,
                     d->word14.overlay2.classid_type));
            break;
        case 2:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 2: classid=%d "
                     "classid_type=%d\n"),
                     prefix,
                     d->word14.overlay2.classid,
                     d->word14.overlay2.classid_type));
            break;
        default:
            break;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\treason_type=%d reason=%08x_%08x\n"),
             prefix,
             d->word4.overlay1.cpu_opcode_type,
             d->reason_hi,
             d->reason));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tts_type=%d timestamp=%08x_%08x\n"),
             prefix,
             d->timestamp_type,
             d->word14.timestamp_hi,
             d->timestamp));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tsrcport=%d cpu_cos=%d hgi=%d lb_pkt_type=%d "
             "repl_nhi=%05x\n"),
             prefix,
             d->srcport,
             d->word4.overlay2.cpu_cos,
             d->hgi,
             d->loopback_pkt_type,
             d->repl_nhi));
    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d otag_action=%d "
             "vntag_action=%d\n"),
             prefix,
             d->word4.overlay1.outer_vid,
             d->word3.overlay1.outer_cfi,
             d->word3.overlay1.outer_pri,
             d->otag_action,
             d->word3.overlay1.vntag_action));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tinner_vid=%d inner_cfi=%d inner_pri=%d itag_action=%d "
             "tag_status=%d\n"),
             prefix,
             d->word3.overlay1.inner_vid,
             d->word3.overlay1.inner_cfi,
             d->inner_pri,
             d->itag_action,
             d->tag_status));
    LOG_CLI((BSL_META_U(unit,
                        "%s\tdscp=%d ecn=%d decap_tunnel_type=%d match_rule=%d "
             "mtp_ind=%d\n"),
             prefix,
             d->word3.overlay1.dscp,
             d->ecn,
             d->word3.overlay1.decap_tunnel_type,
             d->match_rule,
             d->mtp_index));
}
#endif /* BROADCOM_DEBUG */

dcb_op_t dcb33_op = {
    33,
    sizeof(dcb33_t),
    dcb33_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb21_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb26_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb33_desc_intr_set,
    dcb33_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb26_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb26_rx_cos_get,
    dcb26_rx_destmod_get,
    dcb26_rx_destport_get,
    dcb26_rx_opcode_get,
    dcb26_rx_classtag_get,
    dcb26_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb26_rx_prio_get,
    dcb26_rx_reason_get,
    dcb26_rx_reason_hi_get,
    dcb26_rx_ingport_get,
    dcb26_rx_srcport_get,
    dcb26_rx_srcmod_get,
    dcb26_rx_mcast_get,
    dcb26_rx_vclabel_get,
    dcb26_rx_mirror_get,
    dcb26_rx_timestamp_get,
    dcb26_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb26_mhp_get,
    dcb33_outer_vid_get,
    dcb26_outer_pri_get,
    dcb26_outer_cfi_get,
    dcb23_rx_outer_tag_action_get,
    dcb26_inner_vid_get,
    dcb26_inner_pri_get,
    dcb26_inner_cfi_get,
    dcb23_rx_inner_tag_action_get,
    dcb26_rx_bpdu_get,
    dcb26_rx_l3_intf_get,
    dcb33_rx_decap_tunnel_get,
    dcb23_rx_switch_drop_get,
    dcb23_olp_encap_oam_pkt_get,
    dcb23_read_ecc_error_get,
    dcb23_desc_remaining_get,
    dcb23_desc_remaining_set,
    dcb23_desc_status_get,
    dcb23_desc_status_set,
    dcb23_match_id_lo_get,
    dcb23_match_id_hi_get,
    dcb23_forwarding_type_get,
    dcb23_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb33_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb23_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb26_rx_ingport_set,
#endif /* PLISIM */
    NULL,
};
#endif /* BCM_TRIDENT2PLUS_SUPPORT */


#ifdef BCM_APACHE_SUPPORT

/* From FORMAT CPU_OPCODES */
static soc_rx_reason_t
dcb35_rx_reason_map_ip[] = {
    socRxReasonUnknownVlan,        /*  0: CPU_UVLAN */
    socRxReasonL2SourceMiss,       /*  1: CPU_SLF */
    socRxReasonL2DestMiss,         /*  2: CPU_DLF */
    socRxReasonL2Move,             /*  3: CPU_L2MOVE */
    socRxReasonL2Cpu,              /*  4: CPU_L2CPU */
    socRxReasonSampleSource,       /*  5: CPU_SFLOW_SRC */
    socRxReasonSampleDest,         /*  6: CPU_SFLOW_DST */
    socRxReasonL3SourceMiss,       /*  7: CPU_L3SRC_MISS */
    socRxReasonL3DestMiss,         /*  8: CPU_L3DST_MISS */
    socRxReasonL3SourceMove,       /*  9: CPU_L3SRC_MOVE */
    socRxReasonMcastMiss,          /* 10: CPU_MC_MISS */
    socRxReasonIpMcastMiss,        /* 11: CPU_IPMC_MISS */
    socRxReasonFilterMatch,        /* 12: CPU_FFP */
    socRxReasonL3HeaderError,      /* 13: CPU_L3HDR_ERR */
    socRxReasonProtocol,           /* 14: CPU_PROTOCOL_PKT */
    socRxReasonDosAttack,          /* 15: CPU_DOS_ATTACK */
    socRxReasonMartianAddr,        /* 16: CPU_MARTIAN_ADDR */
    socRxReasonTunnelError,        /* 17: CPU_TUNNEL_ERR */
    socRxReasonFailoverDrop,       /* 18: FAILOVER_DROP */
    socRxReasonIcmpRedirect,       /* 19: ICMP_REDIRECT */
    socRxReasonL3Slowpath,         /* 20: L3_SLOWPATH */
    socRxReasonParityError,        /* 21: PARITY_ERROR */
    socRxReasonL3MtuFail,          /* 22: L3_MTU_CHECK_FAIL */
    socRxReasonHigigHdrError,      /* 23: HGHDR_ERROR */
    socRxReasonMcastIdxError,      /* 24: MCIDX_ERROR */
    socRxReasonVlanFilterMatch,    /* 25: VFP */
    socRxReasonClassBasedMove,     /* 26: CBSM_PREVENTED */
    socRxReasonL3AddrBindFail,     /* 27: MAC_BIND_FAIL */
    socRxReasonMplsLabelMiss,      /* 28: MPLS_LABEL_MISS */
    socRxReasonMplsInvalidAction,  /* 29: MPLS_INVALID_ACTION */
    socRxReasonMplsInvalidPayload, /* 30: MPLS_INVALID_PAYLOAD */
    socRxReasonMplsTtl,            /* 31: MPLS_TTL_CHECK_FAIL */
    socRxReasonMplsSequenceNumber, /* 32: MPLS_SEQ_NUM_FAIL */
    socRxReasonL2NonUnicastMiss,   /* 33: PBT_NONUC_PKT */
    socRxReasonNhop,               /* 34: L3_NEXT_HOP */
    socRxReasonMplsUnknownAch,     /* 35: MPLS_UNKNOWN_ACH_ERROR */
    socRxReasonStation,            /* 36: MY_STATION */
    socRxReasonNiv,                /* 37: NIV_DROP_REASON_ENCODING */
    socRxReasonNiv,                /* 38: ->  */
    socRxReasonNiv,                /* 39: 3-bit */
    socRxReasonVlanTranslate,      /* 40: VXLT_MISS */
    socRxReasonTimeSync,           /* 41: TIME_SYNC */
    socRxReasonOAMSlowpath,        /* 42: OAM_SLOWPATH */
    socRxReasonMplsLookupsExceeded,/* 43: MPLS_OUT_OF_LOOKUPS */
    socRxReasonTrill,              /* 44: TRILL_DROP_REASON_ENCODING */
    socRxReasonTrill,              /* 45: -> */
    socRxReasonTrill,              /* 46: 3-bit */
    socRxReasonL2GreSipMiss,       /* 47: L2GRE_SIP_MISS */
    socRxReasonL2GreVpnIdMiss,     /* 48: L2GRE_VPNID_MISS */
    socRxReasonBfdSlowpath,        /* 49: BFD_SLOWPATH */
    socRxReasonBfd,                /* 50: BFD_ERROR */
    socRxReasonMplsReservedEntropyLabel,/* 51: ENTROPY_LABEL_IN_UNALLOWED_RANGE */
    socRxReasonCongestionCnm,      /* 52: ICNM */
    socRxReasonMplsIllegalReservedLabel, /* 53: MPLS_ILLEGAL_RESERVED_LABEL */
    socRxReasonMplsRouterAlertLabel, /* 54: MPLS_ALERT_LABEL */
    socRxReasonCongestionCnmProxy, /* 55: QCN_CNM_PRP */
    socRxReasonCongestionCnmProxyError, /* 56: QCN_CNM_PRP_DLF */
    socRxReasonVxlanSipMiss,       /* 57: VXLAN_SIP_MISS */
    socRxReasonVxlanVpnIdMiss,     /* 58: VXLAN_VN_ID_MISS */
    socRxReasonFcoeZoneCheckFail,  /* 59: FCOE_ZONE_CHECK_FAIL */
    socRxReasonNat,                /* 60: NAT_DROP_REASON_ENCODING */
    socRxReasonNat,                /* 61: -> */
    socRxReasonNat,                /* 62: 3-bit */
    socRxReasonIpmcInterfaceMismatch /* 63: CPU_IPMC_INTERFACE_MISMATCH */
};

/* From format EP_CPU_OPCODES */
static soc_rx_reason_t
dcb35_rx_reason_map_ep[] = {
    socRxReasonSplitHorizon,       /* 0: CPUE_PRUNE */
    socRxReasonNivPrune,           /* 1: CPUE_NIV_DISCARD */
    socRxReasonVirtualPortPrune,   /* 2: CPUE_SPLIT_HORIZON */
    socRxReasonHigigHdrError,      /* 3: CPUE_HIGIG */
    socRxReasonTrillPacketPortMismatch, /* 4: CPUE_TRILL */
    socRxReasonL2MtuFail,          /* 5: CPUE_MTU */
    socRxReasonInvalid,            /* 6: CPUE_PKT_SMALL_OR_BIG */
    socRxReasonUnknownVlan,        /* 7: CPUE_VLAN */
    socRxReasonStp,                /* 8: CPUE_STG */
    socRxReasonVlanTranslate,      /* 9: CPUE_VXLT */
    socRxReasonNonUnicastDrop,     /* 10: CPUE_MULTI_DEST */
    socRxReasonInvalid,            /* 11: CPUE_CNM */
    socRxReasonInvalid,            /* 12: CPUE_FCOE */
    socRxReasonInvalid,            /* 13: CPUE_CFI */
    socRxReasonTtl,                /* 14: CPUE_TTL_DROP */
    socRxReasonIpmc,               /* 15: CPUE_L3ERR */
    socRxReasonTunnelError,        /* 16: CPUE_TUNNEL */
    socRxReasonFilterMatch,        /* 17: CPUE_EFP */
    socRxReasonInvalid,            /* 18: CPUE_PROT */
    socRxReasonInvalid,            /* 19: CPUE_1588 */
    socRxReasonInvalid,            /* 20: CPUE_COMPOSITE_ERROR */
    socRxReasonInvalid,            /* 21: EP_DROP_ORIG */

    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t *dcb35_rx_reason_maps[4][5] = {
    {   /* IP_0 */
        dcb35_rx_reason_map_ip,
        dcb33_rx_reason_niv_encoding_map,
        dcb26_rx_reason_trill_encoding_map,
        dcb33_rx_reason_nat_encoding_map,
        NULL
    },
    {NULL}, /* Index 1 is invalid */
    {   /*  EP */
        dcb35_rx_reason_map_ep,
        NULL
    },
    {NULL} /* Index 3 is invalid */
};


/*
 * Function:
 *      dcb35_rx_reason_map_get
 * Purpose:
 *      Return the RX reason map for DCB 35 type.
 * Parameters:
 *      dcb_op - DCB operations
 *      dcb    - dma control block
 * Returns:
 *      RX reason map pointer
 */
static soc_rx_reason_t *
dcb35_rx_reason_map_get(dcb_op_t *dcb_op, dcb_t *dcb)
{
    soc_rx_reason_t *map = NULL;
    dcb35_t * d = (dcb35_t *)dcb;

    map = dcb35_rx_reason_maps[(d->word4.overlay1.cpu_opcode_type)][0];

    return map;
}

static _shr_rx_decap_tunnel_t dcb35_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_ACCESS_SVP,
    _SHR_RX_DECAP_MIM,
    _SHR_RX_DECAP_L2GRE,
    _SHR_RX_DECAP_VXLAN,
    _SHR_RX_DECAP_AMT,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_TRILL,
    _SHR_RX_DECAP_MPLS_1LABELCW,
    _SHR_RX_DECAP_MPLS_2LABELCW,
    _SHR_RX_DECAP_MPLS_3LABELCW,
    _SHR_RX_DECAP_MPLS_3LABEL_ENTROPYCW,
    _SHR_RX_DECAP_MPLS_1LABEL,
    _SHR_RX_DECAP_MPLS_2LABEL,
    _SHR_RX_DECAP_MPLS_3LABEL,
    _SHR_RX_DECAP_MPLS_3LABEL_ENTROPY
};
static uint32 dcb35_rx_decap_tunnel_get(dcb_t *dcb) {
    dcb35_t *d = (dcb35_t *)dcb;
    /* Check for array bounds */
    if ( (d->word3.overlay1.decap_tunnel_type <
    (sizeof (dcb35_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb35_rx_decap_tunnel_map[d->word3.overlay1.decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}

GETFUNCFIELD(35, olp_encap_oam_pkt, olp_encap_oam_pkt)
GETFUNCNULL(35, read_ecc_error)
GETFUNCNULL(35, desc_remaining)
SETFUNCNULL(35, desc_remaining)
GETFUNCNULL(35, desc_status)
SETFUNCNULL(35, desc_status)
GETFUNCNULL(35, match_id_lo)
GETFUNCNULL(35, match_id_hi)
GETFUNCNULL(35, forwarding_type)
GETFUNCNULL(35, forwarding_zone_id)

dcb_op_t dcb35_op = {
    35,
    sizeof(dcb35_t),
    dcb35_rx_reason_maps[0],
    dcb35_rx_reason_map_get,
    dcb21_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb26_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb33_desc_intr_set,
    dcb33_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb26_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb26_rx_cos_get,
    dcb26_rx_destmod_get,
    dcb26_rx_destport_get,
    dcb26_rx_opcode_get,
    dcb26_rx_classtag_get,
    dcb26_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb26_rx_prio_get,
    dcb26_rx_reason_get,
    dcb26_rx_reason_hi_get,
    dcb26_rx_ingport_get,
    dcb26_rx_srcport_get,
    dcb26_rx_srcmod_get,
    dcb26_rx_mcast_get,
    dcb26_rx_vclabel_get,
    dcb26_rx_mirror_get,
    dcb26_rx_timestamp_get,
    dcb26_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb26_mhp_get,
    dcb33_outer_vid_get,
    dcb26_outer_pri_get,
    dcb26_outer_cfi_get,
    dcb23_rx_outer_tag_action_get,
    dcb26_inner_vid_get,
    dcb26_inner_pri_get,
    dcb26_inner_cfi_get,
    dcb23_rx_inner_tag_action_get,
    dcb26_rx_bpdu_get,
    dcb26_rx_l3_intf_get,
    dcb35_rx_decap_tunnel_get,
    dcb23_rx_switch_drop_get,
    dcb35_olp_encap_oam_pkt_get,
    dcb35_read_ecc_error_get,
    dcb35_desc_remaining_get,
    dcb35_desc_remaining_set,
    dcb35_desc_status_get,
    dcb35_desc_status_set,
    dcb35_match_id_lo_get,
    dcb35_match_id_hi_get,
    dcb35_forwarding_type_get,
    dcb35_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb26_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb23_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb26_rx_ingport_set,
#endif /* PLISIM */
};
#endif /* BCM_APACHE_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
SETFUNCFIELD(36, reqcount, c_count, uint32 count, count)
GETFUNCFIELD(36, reqcount, c_count)
GETFUNCFIELD(36, xfercount, count)
SETFUNCFIELD(36, done, done, int val, val ? 1 : 0)
GETFUNCFIELD(36, done, done)
SETFUNCFIELD(36, sg, c_sg, int val, val ? 1 : 0)
GETFUNCFIELD(36, sg, c_sg)
SETFUNCFIELD(36, chain, c_chain, int val, val ? 1 : 0)
GETFUNCFIELD(36, chain, c_chain)
SETFUNCFIELD(36, reload, c_reload, int val, val ? 1 : 0)
GETFUNCFIELD(36, reload, c_reload)
GETFUNCFIELD(36, rx_start, start)
GETFUNCFIELD(36, rx_end, end)
GETFUNCFIELD(36, rx_error, error)
SETFUNCFIELD(36, hg, c_hg, uint32 hg, hg)
GETFUNCFIELD(36, hg, c_hg)
SETFUNCFIELD(36, purge, c_purge, uint32 purge, purge)
GETFUNCFIELD(36, purge, c_purge)
GETFUNCFIELD(36, read_ecc_error, ecc_error)
GETFUNCFIELD(36, desc_status, c_desc_status)
GETFUNCFIELD(36, desc_intr, c_intr)
GETFUNCFIELD(36, desc_remaining, c_desc)

GETHDRFUNCFIELD(36, rx_timestamp, timestamp)
GETHDRFUNCNULL(36, rx_timestamp_upper)
GETHDRPTREXPR(36, mhp, &(h->mh0))
GETHDRFUNCFIELD(36, outer_vid, outer_vid)
GETHDRFUNCFIELD(36, outer_pri, word6.overlay1.outer_pri)
GETHDRFUNCFIELD(36, outer_cfi, outer_cfi)
GETHDRFUNCFIELD(36, rx_outer_tag_action, otag_action)
GETHDRFUNCFIELD(36, inner_vid, inner_vid)
GETHDRFUNCFIELD(36, inner_pri, word6.overlay1.inner_pri)
GETHDRFUNCFIELD(36, inner_cfi, word6.overlay1.inner_cfi)
GETHDRFUNCFIELD(36, rx_inner_tag_action, itag_action)
GETHDRFUNCFIELD(36, rx_bpdu, bpdu)
GETHDRFUNCFIELD(36, rx_decap_tunnel, decap_tunnel_type)
GETHDRFUNCFIELD(36, match_id_lo, match_id_lo)
GETHDRFUNCFIELD(36, match_id_hi, match_id_hi)
GETHDRFUNCFIELD(36, forwarding_type, forwarding_type)
GETHDRFUNCFIELD(36, forwarding_zone_id, forwarding_zone_id)

static uint32 ep_to_cpu_hdr36_rx_crc_get(void *hdr) {
    return 0;
}

/*
 * In TD3 based devices, tag status is obtained from match ID.
 * Tag status is comprised of otag and itag which is in Zone 1
 * of match ID.
 * itag -> bit  9 of match id.
 * otag -> bit 10 of match id.
 * A right shift of 9 is performed to obtain these 2 bits (otag , itag).
 * Therafter & with 0x3 is done to mask out the other bits.
 */
static uint32
ep_to_cpu_hdr36_rx_untagged_get(void *hdr, int dt_mode, int ingport_is_hg)
{
    ep_to_cpu_hdr36_t *h = (ep_to_cpu_hdr36_t *)hdr;
    uint32 tag_status = ((h->match_id_lo >> 9) & 0x3) ;

    COMPILER_REFERENCE(dt_mode);

    return (ingport_is_hg ?
            ((tag_status) ? 0 : 2) :
            ((tag_status & 0x2) ?
             ((tag_status & 0x1) ? 0 : 2) :
             ((tag_status & 0x1) ? 1 : 3)));
}

static void
ep_to_cpu_hdr36_rx_reasons_get(dcb_op_t *dcb_op, void *hdr, soc_rx_reasons_t *reasons)
{
    soc_rx_reason_t *map, *encoding_map;
    uint32 opcode[2], encoding[3];
    uint32 bit_val;
    int word_idx, bit_idx, opcode_idx, map_idx, word_count;
    int enc_bit = 0;

    SOC_RX_REASON_CLEAR_ALL(*reasons);

    opcode[0] = dcb_op->rx_reason_get(hdr);
    opcode[1] = dcb_op->rx_reason_hi_get(hdr);
    word_count = 2;
    sal_memset(encoding, 0, sizeof(encoding));
    map = dcb_op->rx_reason_maps[0];
    for (word_idx = 0; word_idx < word_count; word_idx++) {
        for (bit_idx = 0; bit_idx < 32; bit_idx++) {
            opcode_idx = word_idx * 32 + bit_idx;
            bit_val = opcode[word_idx] & (1 << bit_idx) ? 1 : 0;
            for (map_idx = 0; ; map_idx++) {
                encoding_map = dcb_op->rx_reason_maps[1 + map_idx];
                if (encoding_map == NULL) {
                    break;
                }
                if (map[opcode_idx] != encoding_map[0]) {
                    continue;
                }
                if (opcode_idx == 0 || map[opcode_idx - 1] != encoding_map[0]) {
                    enc_bit = 0;
                }
                encoding[map_idx] |= bit_val << enc_bit;
                enc_bit++;
                break;
            }
            if (encoding_map == NULL && bit_val) {
                SOC_RX_REASON_SET(*reasons, map[opcode_idx]);
            }
        }
    }

    for (map_idx = 0; ; map_idx++) {
        encoding_map = dcb_op->rx_reason_maps[1 + map_idx];
        if (encoding_map == NULL) {
            break;
        }
        if (encoding[map_idx] != socRxReasonInvalid) {
            SOC_RX_REASON_SET(*reasons, encoding_map[encoding[map_idx]]);
        }
    }

    /* BPDU bit should be a reason, paste it in here */
    if (dcb_op->rx_bpdu_get(hdr)) {
        SOC_RX_REASON_SET(*reasons, socRxReasonBpdu);
    }

    return;
}


GETHDRFUNCFIELD(36, rx_cos, word8.overlay1.cpu_cos)

GETHDRHG2FUNCFIELD(36, rx_destmod, dst_mod)
GETHDRHG2FUNCFIELD(36, rx_destport, dst_port)

GETHDRHG2FUNCFIELD(36, rx_opcode, opcode)
GETHDRFUNCEXPR(36, rx_classtag, (h->word16.overlay2.eh_type == 1 ?
                                (h->word16.overlay2.classid) :
                                (h->word16.overlay2.eh_type == 2 ?
                                (h->word16.overlay3.classid) : 0)))
GETHDRFUNCFIELD(36, rx_matchrule, match_rule)
GETHDRHG2FUNCFIELD(36, rx_prio, vlan_pri) /* outer_pri */

static uint32
ep_to_cpu_hdr36_rx_reason_get(void *hdr)
{
    ep_to_cpu_hdr36_t *h = (ep_to_cpu_hdr36_t *)hdr;

    return (h->switch_pkt) ? (h->reason) : 0;
}

static uint32
ep_to_cpu_hdr36_rx_reason_hi_get(void *hdr)
{
    ep_to_cpu_hdr36_t *h = (ep_to_cpu_hdr36_t *)hdr;

    return (h->switch_pkt) ? (h->reason_hi) : 0;
}

GETHDRFUNCFIELD(36, rx_ingport, word8.overlay1.srcport)
GETHDRHG2FUNCFIELD(36, rx_srcport, src_port)
GETHDRHG2FUNCFIELD(36, rx_srcmod, src_mod)
GETHDRHG2FUNCEXPR(36, rx_mcast, ((h->ppd_overlay1.dst_mod << 8) |
                              (h->ppd_overlay1.dst_port)))
GETHDRHG2FUNCEXPR(36, rx_vclabel, ((h->ppd_overlay1.vc_label_19_16 << 16) |
                              (h->ppd_overlay1.vc_label_15_8 << 8) |
                              (h->ppd_overlay1.vc_label_7_0)))
GETHDRFUNCEXPR(36, rx_mirror, ((h->switch_pkt) ? 0:1))

GETHDRFUNCEXPR(36, rx_l3_intf, h->word6.overlay1.nhi_b ? \
   (h->word6.overlay1.nhi_b + _SHR_L3_EGRESS_IDX_MIN): \
   (((h->nhi_a_hi << 8) | h->word6.overlay1.nhi_a_lo) + _SHR_L3_EGRESS_IDX_MIN))
GETHDRFUNCNULL(36, rx_switch_drop)
GETHDRFUNCNULL(36, olp_encap_oam_pkt)

#ifdef  BROADCOM_DEBUG
GETFUNCERR(36, tx_l2pbm)
GETFUNCERR(36, tx_utpbm)
GETFUNCERR(36, tx_l3pbm)
GETFUNCERR(36, tx_crc)
GETFUNCERR(36, tx_cos)
GETFUNCERR(36, tx_destmod)
GETFUNCERR(36, tx_destport)
GETFUNCERR(36, tx_opcode)
GETFUNCERR(36, tx_srcmod)
GETFUNCERR(36, tx_srcport)
GETFUNCERR(36, tx_prio)
GETFUNCERR(36, tx_pfm)
#endif /* BROADCOM_DEBUG */

#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb36_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[2] = d[3] = 0;
}
SETFUNCFIELD(36, xfercount, count, uint32 count, count)
SETFUNCFIELD(36, rx_start, start, int val, val ? 1 : 0)
SETFUNCFIELD(36, rx_end, end, int val, val ? 1 : 0)
SETFUNCFIELD(36, rx_error, error, int val, val ? 1 : 0)
SETHDRFUNCEXPRIGNORE(36, rx_crc, int val, ignore)
SETHDRFUNCFIELD(36, rx_ingport, word8.overlay1.srcport, int val, val)
#endif  /* PLISIM */

#if 0

static void
ep_to_cpu_hdr_funcerr(int dt, char *name)
{
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("ERROR: ep_to_cpu_hdr%d_%s called\n"), dt, name));
}
#endif

static void
dcb36_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[0] = d[1] = d[2] = d[3] = 0;
}

static int
dcb36_add_rx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb36_t     *d;     /* DCB */
    uint32            *di;    /* DCB integer pointer */
    sal_paddr_t        paddr;

    d = (dcb36_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = 0;

    if (addr) {
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);
        d->addr_lo = PTR_TO_INT(paddr);
        d->addr_hi = PTR_HI_TO_INT(paddr);
        if (soc_cm_get_bus_type(dv->dv_unit) & SOC_PCI_DEV_TYPE) {
            d->addr_hi |= 0x10000000;
        }
    }

    d->c_count = count;
    d->c_sg = 1;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static int
dcb36_add_tx(dv_t *dv, sal_vaddr_t addr, uint32 count,
            pbmp_t l2pbm, pbmp_t utpbm, pbmp_t l3pbm, uint32 flags, uint32 *hgh)
{
    dcb36_t     *d;     /* DCB */
    uint32            *di;    /* DCB integer pointer */
    sal_paddr_t       paddr = 0;  /* Packet buffer physical address */

    d = (dcb36_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (addr)
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);

    if (dv->dv_vcnt > 0 && (dv->dv_flags & DV_F_COMBINE_DCB) &&
        (d[-1].c_sg != 0) &&
        ((d[-1].addr_lo + d[-1].c_count) == paddr) &&
        (d[-1].c_count + count <= DCB_MAX_REQCOUNT)) {
        d[-1].c_count += count;
        return dv->dv_cnt - dv->dv_vcnt;
    }

    if (dv->dv_vcnt >= dv->dv_cnt) {
        return SOC_E_FULL;
    }
    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = 0;

    d->addr_lo = PTR_TO_INT(paddr);
    d->addr_hi = PTR_HI_TO_INT(paddr);
    if (soc_cm_get_bus_type(dv->dv_unit) & SOC_PCI_DEV_TYPE) {
        d->addr_hi |= 0x10000000;
    }
    d->c_count = count;
    d->c_sg = 1;

    d->c_purge = SOC_DMA_PURGE_GET(flags);

    if (SOC_DMA_HG_GET(flags)) {
        d->c_hg = 1;
    }

    dv->dv_vcnt += 1;

    return dv->dv_cnt - dv->dv_vcnt;
}

static uint32
dcb36_intr_info(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb36_t      *d = (dcb36_t *)dcb;     /*  DCB */
    uint32      f;                                    /* SOC_DCB_INFO_* flags */

    if (!d->done) {
        return 0;
    }

    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }

    *count = d->count;
    return f;
}

/* the addr related functions are the same for all dcb types */
static void
dcb36_addr_set(int unit, dcb_t *dcb, sal_vaddr_t addr)
{
    dcb36_t      *d = (dcb36_t *)dcb;     /*  DCB */
    sal_paddr_t        paddr;

    if (addr == 0) {
        d->addr_lo = 0;
        d->addr_hi = 0;
    } else {
        paddr = soc_cm_l2p(unit, (void *)addr);
        d->addr_lo = PTR_TO_INT(paddr);
        d->addr_hi = PTR_HI_TO_INT(paddr);
        if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
            d->addr_hi |= 0x10000000;
        }
    }
}

static sal_vaddr_t
dcb36_addr_get(int unit, dcb_t *dcb)
{
    dcb36_t      *d = (dcb36_t *)dcb;     /*  DCB */
    sal_paddr_t        paddr;

#if defined(COMPILER_OVERRIDE_NO_LONGLONG) || defined(__PEDANTIC__)
    paddr = d->addr_lo;
#else
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        paddr = (((uint64)(d->addr_hi & ~(0x10000000)) << 32) | d->addr_lo);
    } else {
        paddr = (((uint64)(d->addr_hi) << 32) | d->addr_lo);
    }
#endif

    return (sal_vaddr_t)soc_cm_p2l(unit, paddr);
}

static sal_paddr_t
dcb36_paddr_get(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    return (sal_paddr_t)*d;
}

static void
dcb36_desc_intr_set(dcb_t * dcb, int val)
{
    dcb36_t * d = (dcb36_t *)dcb;

    d->c_intr = val;
    return;
}

static uint32
dcb36_desc_status_set(dcb_t * dcb, uint32 val)
{
    dcb36_t * d = (dcb36_t *)dcb;

    d->c_desc_status = val;
    return 0;
}

static uint32
dcb36_desc_remaining_set(dcb_t * dcb, uint32 val)
{
    dcb36_t * d = (dcb36_t *)dcb;

    d->c_desc = val;
    return 0;
}

#ifdef BROADCOM_DEBUG
static void
dcb36_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));

    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %spurge \n"),
                         prefix,
                         SOC_DCB_TYPE(unit),
                         SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));

    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }
}
#endif

/*
 * DCB Type 36 Support
 */
/* From FORMAT CPU_OPCODES */
static soc_rx_reason_t
dcb36_rx_reason_map[] = {
    socRxReasonFilterMatch,        /*  0: CPU_FFP */
    socRxReasonVlanFilterMatch,    /*  1: VFP */
    socRxReasonSampleSourceFlex,   /*  2: CPU_SFLOW_FLEX */
    socRxReasonSampleSource,       /*  3: CPU_SFLOW_SRC */
    socRxReasonSampleDest,         /*  4: CPU_SFLOW_DST */
    socRxReasonL2Move,             /*  5: CPU_L2MOVE */
    socRxReasonL2Cpu,              /*  6: CPU_L2CPU */
    socRxReasonL3SourceMiss,       /*  7: CPU_L3SRC_MISS */
    socRxReasonL3DestMiss,         /*  8: CPU_L3DST_MISS */
    socRxReasonL3SourceMove,       /*  9: CPU_L3SRC_MOVE */
    socRxReasonMcastMiss,          /* 10: CPU_MC_MISS */
    socRxReasonIpMcastMiss,        /* 11: CPU_IPMC_MISS */
    socRxReasonUnknownVlan,        /* 12: CPU_UVLAN */
    socRxReasonL3HeaderMismatch,   /* 13: CPU_L3HDR_MISMATCH */
    socRxReasonProtocol,           /* 14: CPU_PROTOCOL_PKT */
    socRxReasonDosAttack,          /* 15: CPU_DOS_ATTACK */
    socRxReasonMartianAddr,        /* 16: CPU_MARTIAN_ADDR */
    socRxReasonTunnelError,        /* 17: CPU_TUNNEL_ERR */
    socRxReasonL2DestMiss,         /* 18: CPU_DLF */
    socRxReasonIcmpRedirect,       /* 19: ICMP_REDIRECT */
    socRxReasonL3Slowpath,         /* 20: L3_SLOWPATH */
    socRxReasonParityError,        /* 21: PARITY_ERROR */
    socRxReasonL3MtuFail,          /* 22: L3_MTU_CHECK_FAIL */
    socRxReasonHigigHdrError,      /* 23: HGHDR_ERROR */
    socRxReasonMcastIdxError,      /* 24: MCIDX_ERROR */
    socRxReasonL2SourceMiss,       /* 25: CPU_SLF */
    socRxReasonClassBasedMove,     /* 26: CBSM_PREVENTED */
    socRxReasonL3AddrBindFail,     /* 27: MAC_BIND_FAIL */
    socRxReasonMplsLabelMiss,      /* 28: MPLS_LABEL_MISS */
    socRxReasonMplsInvalidAction,  /* 29: MPLS_INVALID_ACTION */
    socRxReasonMplsInvalidPayload, /* 30: MPLS_INVALID_PAYLOAD */
    socRxReasonTunnelObjectValidationFail,  /* 31: TUNNEL_OBJECT_VALIDATION_FAIL */
    socRxReasonMplsSequenceNumber, /* 32: MPLS_SEQ_NUM_FAIL */
    socRxReasonL2NonUnicastMiss,   /* 33: PBT_NONUC_PKT */
    socRxReasonNhop,               /* 34: L3_NEXT_HOP */
    socRxReasonL3Cpu,              /* 35: CPU_L3CPU */
    socRxReasonStation,            /* 36: MY_STATION */
    socRxReasonNivPrioDrop,        /* 37: DOT1P_ADMITTANCE_DISCARD */
    socRxReasonNivRpfFail,         /* 38: RPF_LOOKUP_MISS */
    socRxReasonUnknownSubtendingPort, /* 39: UNKOWN SUBTENDING PORT */
    socRxReasonTunnelAdaptLookupMiss, /* 40: TUNNEL_ADAPT_LOOKUP_MISS_DROP */
    socRxReasonTimeSync,           /* 41: TIME_SYNC */
    socRxReasonPacketFlowSelectMiss,  /* 42: PKT_FLOW_SELECT_MISS */
    socRxReasonTunnelDecapEcnError,/* 43: TUNNEL_DECAP_ECN_ERROR (new) */
    socRxReasonFailoverDrop, /* 44: PROTECTION_DATA_DROP */
    socRxReasonPacketFlowSelect,   /* 45: PKT_FLOW_SELECT */
    socRxReasonOtherLookupMiss,    /* 46: CPU_FORWARDING_OTHER */
    socRxReasonInvalidTpid,        /* 47: INVALID_TPID */
    socRxReasonMplsControlPacket,  /* 48: MPLS_CONTROL_PKT */
    socRxReasonTunnelError,        /* 49: TUNNEL_ERROR */
    socRxReasonTunnelTtlError,     /* 50: TUNNEL_TTL_ERR */
#if 1
    socRxReasonInvalid,            /* 51:NOT DEFINED */
#endif
    socRxReasonCongestionCnm,      /* 52: ICNM */
    socRxReasonMplsIllegalReservedLabel, /* 53: MPLS_ILLEGAL_RESERVED_LABEL */
    socRxReasonMplsRouterAlertLabel, /* 54: MPLS_ALERT_LABEL */
    socRxReasonL3HeaderError,      /* 55: L3_HEADER_ERROR */
    socRxReasonL2HeaderError,      /* 56: L2_HDR_ERROR */
    socRxReasonTtl1,               /* 57: TTL1 */
    socRxReasonTtl,                /* 58: TTL */
    socRxReasonFcoeZoneCheckFail,  /* 59: FCOE_ZONE_CHECK_FAIL */
    socRxReasonNat,                /* 60: NAT_DROP_REASON_ENCODING */
    socRxReasonNat,                /* 61: -> */
    socRxReasonNat,                /* 62: 3-bit */
    socRxReasonIpmcInterfaceMismatch /* 63: CPU_IPMC_INTERFACE_MISMATCH */
};

static soc_rx_reason_t *dcb36_rx_reason_maps[] = {
    dcb36_rx_reason_map,
    dcb26_rx_reason_nat_encoding_map,
    NULL
};

SETFUNCERR(36, tx_l2pbm, pbmp_t)
SETFUNCERR(36, tx_utpbm, pbmp_t)
SETFUNCERR(36, tx_l3pbm, pbmp_t)
SETFUNCERR(36, tx_crc, int)
SETFUNCERR(36, tx_cos, int)
SETFUNCERR(36, tx_destmod, uint32)
SETFUNCERR(36, tx_destport, uint32)
SETFUNCERR(36, tx_opcode, uint32)
SETFUNCERR(36, tx_srcmod, uint32)
SETFUNCERR(36, tx_srcport, uint32)
SETFUNCERR(36, tx_prio, uint32)
SETFUNCERR(36, tx_pfm, uint32)

dcb_op_t dcb36_op = {
    36,
    sizeof(dcb36_t),
    dcb36_rx_reason_maps,
    dcb0_rx_reason_map_get,
    ep_to_cpu_hdr36_rx_reasons_get,
    dcb36_init,
    dcb36_add_tx,
    dcb36_add_rx,
    dcb36_intr_info,
    dcb36_reqcount_set,
    dcb36_reqcount_get,
    dcb36_xfercount_get,
    dcb36_addr_set,
    dcb36_addr_get,
    dcb36_paddr_get,
    dcb36_done_set,
    dcb36_done_get,
    dcb36_sg_set,
    dcb36_sg_get,
    dcb36_chain_set,
    dcb36_chain_get,
    dcb36_reload_set,
    dcb36_reload_get,
    dcb36_desc_intr_set,
    dcb36_desc_intr_get,
    dcb36_tx_l2pbm_set,
    dcb36_tx_utpbm_set,
    dcb36_tx_l3pbm_set,
    dcb36_tx_crc_set,
    dcb36_tx_cos_set,
    dcb36_tx_destmod_set,
    dcb36_tx_destport_set,
    dcb36_tx_opcode_set,
    dcb36_tx_srcmod_set,
    dcb36_tx_srcport_set,
    dcb36_tx_prio_set,
    dcb36_tx_pfm_set,
    ep_to_cpu_hdr36_rx_untagged_get,
    ep_to_cpu_hdr36_rx_crc_get,
    ep_to_cpu_hdr36_rx_cos_get,
    ep_to_cpu_hdr36_rx_destmod_get,
    ep_to_cpu_hdr36_rx_destport_get,
    ep_to_cpu_hdr36_rx_opcode_get,
    ep_to_cpu_hdr36_rx_classtag_get,
    ep_to_cpu_hdr36_rx_matchrule_get,
    dcb36_rx_start_get,
    dcb36_rx_end_get,
    dcb36_rx_error_get,
    ep_to_cpu_hdr36_rx_prio_get,
    ep_to_cpu_hdr36_rx_reason_get,
    ep_to_cpu_hdr36_rx_reason_hi_get,
    ep_to_cpu_hdr36_rx_ingport_get,
    ep_to_cpu_hdr36_rx_srcport_get,
    ep_to_cpu_hdr36_rx_srcmod_get,
    ep_to_cpu_hdr36_rx_mcast_get,
    ep_to_cpu_hdr36_rx_vclabel_get,
    ep_to_cpu_hdr36_rx_mirror_get,
    ep_to_cpu_hdr36_rx_timestamp_get,
    ep_to_cpu_hdr36_rx_timestamp_upper_get,
    dcb36_hg_set,
    dcb36_hg_get,
    NULL,
    NULL,
    dcb36_purge_set,
    dcb36_purge_get,
    ep_to_cpu_hdr36_mhp_get,
    ep_to_cpu_hdr36_outer_vid_get,
    ep_to_cpu_hdr36_outer_pri_get,
    ep_to_cpu_hdr36_outer_cfi_get,
    ep_to_cpu_hdr36_rx_outer_tag_action_get,
    ep_to_cpu_hdr36_inner_vid_get,
    ep_to_cpu_hdr36_inner_pri_get,
    ep_to_cpu_hdr36_inner_cfi_get,
    ep_to_cpu_hdr36_rx_inner_tag_action_get,
    ep_to_cpu_hdr36_rx_bpdu_get,
    ep_to_cpu_hdr36_rx_l3_intf_get,
    ep_to_cpu_hdr36_rx_decap_tunnel_get,
    ep_to_cpu_hdr36_rx_switch_drop_get,
    ep_to_cpu_hdr36_olp_encap_oam_pkt_get,
    dcb36_read_ecc_error_get,
    dcb36_desc_remaining_get,
    dcb36_desc_remaining_set,
    dcb36_desc_status_get,
    dcb36_desc_status_set,
    ep_to_cpu_hdr36_match_id_lo_get,
    ep_to_cpu_hdr36_match_id_hi_get,
    ep_to_cpu_hdr36_forwarding_type_get,
    ep_to_cpu_hdr36_forwarding_zone_id_get,
#ifdef BROADCOM_DEBUG
    dcb36_tx_l2pbm_get,
    dcb36_tx_utpbm_get,
    dcb36_tx_l3pbm_get,
    dcb36_tx_crc_get,
    dcb36_tx_cos_get,
    dcb36_tx_destmod_get,
    dcb36_tx_destport_get,
    dcb36_tx_opcode_get,
    dcb36_tx_srcmod_get,
    dcb36_tx_srcport_get,
    dcb36_tx_prio_get,
    dcb36_tx_pfm_get,
    dcb36_dump,
    dcb0_reason_dump,
#endif
#ifdef  PLISIM
    dcb36_status_init,
    dcb36_xfercount_set,
    dcb36_rx_start_set,
    dcb36_rx_end_set,
    dcb36_rx_error_set,
    ep_to_cpu_hdr36_rx_crc_set,
    ep_to_cpu_hdr36_rx_ingport_set,
#endif /* PLISIM */
};
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)  
/*
 * DCB Type 34 Support
 */
#ifdef BROADCOM_DEBUG
static void
dcb34_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t     *dtx = (dcb19_t *)dcb;  /* Fake out for different TX MH */
    dcb34_t *d = (dcb34_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
    uint8       *h;
#if defined(LE_HOST)
    uint32  hgh[4];
    h = (uint8 *)&hgh[0];

    if (tx) {
        hgh[0] = soc_htonl(dtx->mh0);
        hgh[1] = soc_htonl(dtx->mh1);
        hgh[2] = soc_htonl(dtx->mh2);
        hgh[3] = soc_htonl(dtx->mh3);
    } else {
        hgh[0] = soc_htonl(d->mh0);
        hgh[1] = soc_htonl(d->mh1);
        hgh[2] = soc_htonl(d->mh2);
        hgh[3] = soc_htonl(d->mh3);
    }
#else
    if (tx) {
        h = (uint8 *)&dtx->mh0;
    } else {
        h = (uint8 *)&d->mh0;
    }
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
        "%s\ttype %d %ssg %schain %sreload %shg %sstat %spause %spurge\n"),
        prefix,
        SOC_DCB_TYPE(unit),
        SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
        SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
        SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
        SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
        SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
        d->c_pause ? "" : "!",
        SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
        "%s\taddr %p reqcount %d xfercount %d\n"),
        prefix,
        (void *)SOC_DCB_ADDR_GET(unit, dcb),
        SOC_DCB_REQCOUNT_GET(unit, dcb),
        SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
            "%s\t%sdone %sstart %send %serror\n"),
            prefix,
            SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
            SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
            SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
            SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!"));
    }
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb))) {
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
            "%s  %sdo_not_change_ttl %sbpdu %sl3routed %schg_tos %semirror %simirror\n"),
            prefix,
            d->do_not_change_ttl ? "" : "!",
            d->bpdu ? "" : "!",
            d->l3routed ? "" : "!",
            d->chg_tos ? "" : "!",
            d->emirror ? "" : "!",
            d->imirror ? "" : "!"
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  %sreplicated %sl3only %soam_pkt %ssrc_hg\n"),
            prefix,
            d->replicated ? "" : "!",
            d->l3only ? "" : "!",
            d->oam_pkt ? "" : "!",
            d->src_hg ? "" : "!"
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  %sswitch_pkt %sregen_crc %sservice_tag %sing_untagged\n"),
            prefix,
            d->switch_pkt ? "" : "!",
            d->word4.overlay2.regen_crc ? "" : "!",
            d->service_tag ? "" : "!",
            !(((soc_higig2_hdr_t *)h)->ppd_overlay1.ingress_tagged) ? "" : "!"
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  %svfi_valid %schg_ecn ecn=%d\n"),
            prefix,
            d->vfi_valid ? "" : "!",
            d->chg_ecn ? "" : "!",
            d->ecn
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  cpu_cos=%d cos=%d mtp_index=%d reason=%08x_%08x\n"),
            prefix,
            d->word4.overlay2.cpu_cos,
            d->cos,
            d->mtp_index,
            d->reason_hi,
            d->reason
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  reason_type=%d match_rule=%d hg_type=%d mtp_index=%d\n"),
            prefix,
            d->word4.overlay1.cpu_opcode_type,
            d->match_rule,
            d->hg_type,
            d->mtp_index
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
            prefix,
            d->srcport,
            d->dscp,
            d->outer_pri,
            d->outer_cfi,
            d->word4.overlay1.outer_vid
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  inner_pri=%d inner_cfi=%d inner_vid=%d\n"),
            prefix,
            d->inner_pri,
            d->inner_cfi,
            d->inner_vid
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  hgi=%d itag_status=%d otag_action=%d itag_action=%d\n"),
            prefix,
            d->hgi,
            d->itag_status,
            d->otag_action,
            d->itag_action
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  repl_nhi=%05x ts_type=%d timestamp=%08x_%08x\n"),
            prefix,
            d->repl_nhi,
            d->timestamp_type,
            d->timestamp_hi,
            d->word12.overlay1.timestamp
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  vfi=%d em_mtp_index=%d vntag_action=%d\n"),
            prefix,
            d->word12.overlay2.vfi,
            d->em_mtp_index,
            d->vntag_action
            ));
    }
}
#endif /* BROADCOM_DEBUG */

GETFUNCEXPR(34, rx_l3_intf, ((d->replicated) ? (d->repl_nhi) :
        (((d->repl_nhi) & 0x7ff) + _SHR_L3_EGRESS_IDX_MIN)))
    
dcb_op_t dcb34_op = {
    34,
    sizeof(dcb34_t),    
    dcb31_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb19_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb31_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb31_rx_cos_get,
    dcb23_rx_destmod_get,
    dcb23_rx_destport_get,
    dcb23_rx_opcode_get,
    dcb23_rx_classtag_get,
    dcb23_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb23_rx_prio_get,
    dcb23_rx_reason_get,
    dcb23_rx_reason_hi_get,
    dcb23_rx_ingport_get,
    dcb23_rx_srcport_get,
    dcb23_rx_srcmod_get,
    dcb23_rx_mcast_get,
    dcb23_rx_vclabel_get,
    dcb31_rx_mirror_get,
    dcb23_rx_timestamp_get,
    dcb31_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb23_mhp_get,
    dcb31_outer_vid_get,
    dcb31_outer_pri_get,
    dcb31_outer_cfi_get,
    dcb31_rx_outer_tag_action_get,
    dcb31_inner_vid_get,
    dcb31_inner_pri_get,
    dcb31_inner_cfi_get,
    dcb31_rx_inner_tag_action_get,
    dcb31_rx_bpdu_get,
    dcb34_rx_l3_intf_get,
    dcb19_rx_decap_tunnel_get,
    dcb31_rx_switch_drop_get,
    dcb23_olp_encap_oam_pkt_get,
    dcb23_read_ecc_error_get,
    dcb23_desc_remaining_get,
    dcb23_desc_remaining_set,
    dcb23_desc_status_get,
    dcb23_desc_status_set,
    dcb23_match_id_lo_get,
    dcb23_match_id_hi_get,
    dcb23_forwarding_type_get,
    dcb23_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb34_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb31_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb23_rx_ingport_set,
#endif /* PLISIM */
};
#endif /* BCM_HURRICANE3_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
/*
 * DCB Type 37 Support
 */
/* From FORMAT CPU_OPCODES */
static soc_rx_reason_t
dcb37_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /* Offset 0 */
    socRxReasonL2SourceMiss,       /* Offset 1 */
    socRxReasonL2DestMiss,         /* Offset 2 */
    socRxReasonL2Move,             /* Offset 3 */
    socRxReasonL2Cpu,              /* Offset 4 */
    socRxReasonSampleSource,       /* Offset 5 */
    socRxReasonSampleDest,         /* Offset 6 */
    socRxReasonL3SourceMiss,       /* Offset 7 */
    socRxReasonL3DestMiss,         /* Offset 8 */
    socRxReasonL3SourceMove,       /* Offset 9 */
    socRxReasonMcastMiss,          /* Offset 10 */
    socRxReasonIpMcastMiss,        /* Offset 11 */
    socRxReasonFilterMatch,        /* Offset 12 */
    socRxReasonL3HeaderError,      /* Offset 13 */
    socRxReasonProtocol,           /* Offset 14 */
    socRxReasonDosAttack,          /* Offset 15 */
    socRxReasonMartianAddr,        /* Offset 16 */
    socRxReasonTunnelError,        /* Offset 17 */
    socRxReasonL2MtuFail,          /* Offset 18 */
    socRxReasonIcmpRedirect,       /* Offset 19 */
    socRxReasonL3Slowpath,         /* Offset 20 */
    socRxReasonParityError,        /* Offset 21 */
    socRxReasonL3MtuFail,          /* Offset 22 */
    socRxReasonHigigHdrError,      /* Offset 23 */
    socRxReasonMcastIdxError,      /* Offset 24 */
    socRxReasonVlanFilterMatch,    /* Offset 25 */
    socRxReasonClassBasedMove,     /* Offset 26 */
    socRxReasonL2LearnLimit,       /* Offset 27 */
    socRxReasonMplsLabelMiss,      /* Offset 28 */
    socRxReasonMplsInvalidAction,  /* Offset 29 */
    socRxReasonMplsInvalidPayload, /* Offset 30 */
    socRxReasonMplsTtl,            /* Offset 31 */
    socRxReasonMplsSequenceNumber, /* Offset 32 */
    socRxReasonL2NonUnicastMiss,   /* Offset 33 */
    socRxReasonNhop,               /* Offset 34 */
    socRxReasonMplsCtrlWordError,  /* Offset 35 */
    socRxReasonTimeSync,           /* Offset 36 */
    socRxReasonOAMSlowpath,        /* Offset 37 */
    socRxReasonOAMError,           /* Offset 38 */
    socRxReasonOAMLMDM,            /* Offset 39 */
    socRxReasonL3AddrBindFail,     /* Offset 40 */
    socRxReasonVlanTranslate,      /* Offset 41 */
    socRxReasonNiv,                /* Offset 42 */
    socRxReasonNiv,                /* Offset 43 */
    socRxReasonNiv,                /* Offset 44 */
    socRxReasonStation,            /* Offset 45 */
    socRxReasonL2StuFail,          /* Offset 46 */
    socReasonSrCounterExceeded,    /* Offset 47 */
    socRxReasonSrCopyToCpuBit0,    /* Offset 48 */
    socRxReasonSrCopyToCpuBit1,    /* Offset 49 */
    socRxReasonSrCopyToCpuBit2,    /* Offset 50 */
    socRxReasonSrCopyToCpuBit3,    /* Offset 51 */
    socRxReasonSrCopyToCpuBit4,    /* Offset 52 */
    socRxReasonSrCopyToCpuBit5,    /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};
static soc_rx_reason_t *dcb37_rx_reason_maps[] = {
    dcb37_rx_reason_map,
    NULL
};
#ifdef BROADCOM_DEBUG
static void
dcb37_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    dcb19_t     *dtx = (dcb19_t *)dcb;  /* Fake out for different TX MH */
    dcb37_t *d = (dcb37_t *)dcb;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];
    uint8       *h;
#if defined(LE_HOST)
    uint32  hgh[4];
    h = (uint8 *)&hgh[0];

    if (tx) {
        hgh[0] = soc_htonl(dtx->mh0);
        hgh[1] = soc_htonl(dtx->mh1);
        hgh[2] = soc_htonl(dtx->mh2);
        hgh[3] = soc_htonl(dtx->mh3);
    } else {
        hgh[0] = soc_htonl(d->mh0);
        hgh[1] = soc_htonl(d->mh1);
        hgh[2] = soc_htonl(d->mh2);
        hgh[3] = soc_htonl(d->mh3);
    }
#else
    if (tx) {
        h = (uint8 *)&dtx->mh0;
    } else {
        h = (uint8 *)&d->mh0;
    }
#endif

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));
    if ((SOC_DCB_HG_GET(unit, dcb)) || (SOC_DCB_RX_START_GET(unit, dcb))) {
        soc_dma_higig_dump(unit, prefix, h, 0, 0, NULL);
    }
    LOG_CLI((BSL_META_U(unit,
        "%s\ttype %d %ssg %schain %sreload %shg %sstat %spause %spurge\n"),
        prefix,
        SOC_DCB_TYPE(unit),
        SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
        SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
        SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
        SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
        SOC_DCB_STAT_GET(unit, dcb) ? "" : "!",
        d->c_pause ? "" : "!",
        SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));
    LOG_CLI((BSL_META_U(unit,
        "%s\taddr %p reqcount %d xfercount %d\n"),
        prefix,
        (void *)SOC_DCB_ADDR_GET(unit, dcb),
        SOC_DCB_REQCOUNT_GET(unit, dcb),
        SOC_DCB_XFERCOUNT_GET(unit, dcb)));
    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
            "%s\t%sdone %sstart %send %serror\n"),
            prefix,
            SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
            SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
            SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!",
            SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!"));
#if defined(BCM_TSN_SR_SUPPORT)
    /*
     * Seamless Redundancy(SR) - Parallel Redundancy Protocol 2nd pass port
     * Loopback port:
     *     DCB.MODULE_HEADER.SRC_PID
     *     dcb23_rx_srcport_get
     *
     * ingress port:
     *     EP_TO_CPU_HDR.SOURCE_PORT_NUM
     *     dcb23_rx_ingport_get
     */
        LOG_CLI((BSL_META_U(unit,
                            "%s\t TSN-SR PRP loopback port %d \n"),
                            prefix,
                            SOC_DCB_RX_SRCPORT_GET(unit, dcb)));
        LOG_CLI((BSL_META_U(unit,
                            "%s\t TSN-SR PRP Ingress port %d \n"),
                            prefix,
                            SOC_DCB_RX_INGPORT_GET(unit, dcb)));
#endif /* BCM_TSN_SR_SUPPORT */
    }
    if ((!tx) && (SOC_DCB_RX_START_GET(unit, dcb))) {
        dcb0_reason_dump(unit, dcb, prefix);
        LOG_CLI((BSL_META_U(unit,
            "%s  %sdo_not_change_ttl %sbpdu %sl3routed %schg_tos %semirror %simirror\n"),
            prefix,
            d->do_not_change_ttl ? "" : "!",
            d->bpdu ? "" : "!",
            d->l3routed ? "" : "!",
            d->chg_tos ? "" : "!",
            d->emirror ? "" : "!",
            d->imirror ? "" : "!"
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  %sreplicated %sl3only %soam_pkt %ssrc_hg\n"),
            prefix,
            d->replicated ? "" : "!",
            d->l3only ? "" : "!",
            d->oam_pkt ? "" : "!",
            d->src_hg ? "" : "!"
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  %sswitch_pkt %sregen_crc %sservice_tag %sing_untagged\n"),
            prefix,
            d->switch_pkt ? "" : "!",
            d->word4.overlay2.regen_crc ? "" : "!",
            d->service_tag ? "" : "!",
            !(((soc_higig2_hdr_t *)h)->ppd_overlay1.ingress_tagged) ? "" : "!"
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  %svfi_valid %schg_ecn ecn=%d\n"),
            prefix,
            d->vfi_valid ? "" : "!",
            d->chg_ecn ? "" : "!",
            d->ecn
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  cpu_cos=%d cos=%d mtp_index=%d reason=%08x_%08x\n"),
            prefix,
            d->word4.overlay2.cpu_cos,
            d->cos,
            d->mtp_index,
            d->reason_hi,
            d->reason
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  reason_type=%d match_rule=%d hg_type=%d mtp_index=%d\n"),
            prefix,
            d->word4.overlay1.cpu_opcode_type,
            d->match_rule,
            d->hg_type,
            d->mtp_index
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
            prefix,
            d->srcport,
            d->dscp,
            d->outer_pri,
            d->outer_cfi,
            d->word4.overlay1.outer_vid
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  inner_pri=%d inner_cfi=%d inner_vid=%d\n"),
            prefix,
            d->inner_pri,
            d->inner_cfi,
            d->inner_vid
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  hgi=%d itag_status=%d otag_action=%d itag_action=%d\n"),
            prefix,
            d->hgi,
            d->itag_status,
            d->otag_action,
            d->itag_action
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  repl_nhi=%05x ts_type=%d timestamp=%08x_%08x\n"),
            prefix,
            d->repl_nhi,
            d->timestamp_type,
            d->timestamp_hi,
            d->word12.overlay1.timestamp
            ));
        LOG_CLI((BSL_META_U(unit,
            "%s  vfi=%d em_mtp_index=%d vntag_action=%d\n"),
            prefix,
            d->word12.overlay2.vfi,
            d->em_mtp_index,
            d->vntag_action
            ));
    }
}
#endif /* BROADCOM_DEBUG */

GETFUNCEXPR(37, rx_l3_intf, ((d->replicated) ? (d->repl_nhi) :
        (((d->repl_nhi) & 0x7ff) + _SHR_L3_EGRESS_IDX_MIN)))
GETFUNCNULL(37, read_ecc_error)
GETFUNCNULL(37, desc_remaining)
SETFUNCNULL(37, desc_remaining)
GETFUNCNULL(37, desc_status)
SETFUNCNULL(37, desc_status)
GETFUNCNULL(37, match_id_lo)
GETFUNCNULL(37, match_id_hi)
GETFUNCNULL(37, forwarding_type)
GETFUNCNULL(37, forwarding_zone_id)

dcb_op_t dcb37_op = {
    37,
    sizeof(dcb37_t),
    dcb37_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb19_init,
    dcb19_addtx,
    dcb19_addrx,
    dcb19_intrinfo,
    dcb19_reqcount_set,
    dcb19_reqcount_get,
    dcb19_xfercount_get,
    dcb0_addr_set,
    dcb0_addr_get,
    dcb0_paddr_get,
    dcb19_done_set,
    dcb19_done_get,
    dcb19_sg_set,
    dcb19_sg_get,
    dcb19_chain_set,
    dcb19_chain_get,
    dcb19_reload_set,
    dcb19_reload_get,
    dcb19_desc_intr_set,
    dcb19_desc_intr_get,
    dcb19_tx_l2pbm_set,
    dcb19_tx_utpbm_set,
    dcb19_tx_l3pbm_set,
    dcb19_tx_crc_set,
    dcb19_tx_cos_set,
    dcb19_tx_destmod_set,
    dcb19_tx_destport_set,
    dcb19_tx_opcode_set,
    dcb19_tx_srcmod_set,
    dcb19_tx_srcport_set,
    dcb19_tx_prio_set,
    dcb19_tx_pfm_set,
    dcb31_rx_untagged_get,
    dcb19_rx_crc_get,
    dcb31_rx_cos_get,
    dcb23_rx_destmod_get,
    dcb23_rx_destport_get,
    dcb23_rx_opcode_get,
    dcb23_rx_classtag_get,
    dcb23_rx_matchrule_get,
    dcb19_rx_start_get,
    dcb19_rx_end_get,
    dcb19_rx_error_get,
    dcb23_rx_prio_get,
    dcb23_rx_reason_get,
    dcb23_rx_reason_hi_get,
    dcb23_rx_ingport_get,
    dcb23_rx_srcport_get,
    dcb23_rx_srcmod_get,
    dcb23_rx_mcast_get,
    dcb23_rx_vclabel_get,
    dcb31_rx_mirror_get,
    dcb23_rx_timestamp_get,
    dcb31_rx_timestamp_upper_get,
    dcb19_hg_set,
    dcb19_hg_get,
    dcb19_stat_set,
    dcb19_stat_get,
    dcb19_purge_set,
    dcb19_purge_get,
    dcb23_mhp_get,
    dcb31_outer_vid_get,
    dcb31_outer_pri_get,
    dcb31_outer_cfi_get,
    dcb31_rx_outer_tag_action_get,
    dcb31_inner_vid_get,
    dcb31_inner_pri_get,
    dcb31_inner_cfi_get,
    dcb31_rx_inner_tag_action_get,
    dcb31_rx_bpdu_get,
    dcb37_rx_l3_intf_get,
    dcb19_rx_decap_tunnel_get,
    dcb31_rx_switch_drop_get,
    dcb23_olp_encap_oam_pkt_get,
    dcb37_read_ecc_error_get,
    dcb37_desc_remaining_get,
    dcb37_desc_remaining_set,
    dcb37_desc_status_get,
    dcb37_desc_status_set,
    dcb37_match_id_lo_get,
    dcb37_match_id_hi_get,
    dcb37_forwarding_type_get,
    dcb37_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb19_tx_l2pbm_get,
    dcb19_tx_utpbm_get,
    dcb19_tx_l3pbm_get,
    dcb19_tx_crc_get,
    dcb19_tx_cos_get,
    dcb19_tx_destmod_get,
    dcb19_tx_destport_get,
    dcb19_tx_opcode_get,
    dcb19_tx_srcmod_get,
    dcb19_tx_srcport_get,
    dcb19_tx_prio_get,
    dcb19_tx_pfm_get,

    dcb37_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb31_status_init,
    dcb19_xfercount_set,
    dcb19_rx_start_set,
    dcb19_rx_end_set,
    dcb19_rx_error_set,
    dcb19_rx_crc_set,
    dcb23_rx_ingport_set,
#endif /* PLISIM */
};
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
SETFUNCFIELD(38, reqcount, c_count, uint32 count, count)
GETFUNCFIELD(38, reqcount, c_count)
GETFUNCFIELD(38, xfercount, count)
SETFUNCFIELD(38, done, done, int val, val ? 1 : 0)
GETFUNCFIELD(38, done, done)
SETFUNCFIELD(38, sg, c_sg, int val, val ? 1 : 0)
GETFUNCFIELD(38, sg, c_sg)
SETFUNCFIELD(38, chain, c_chain, int val, val ? 1 : 0)
GETFUNCFIELD(38, chain, c_chain)
SETFUNCFIELD(38, reload, c_reload, int val, val ? 1 : 0)
GETFUNCFIELD(38, reload, c_reload)
GETFUNCFIELD(38, rx_start, start)
GETFUNCFIELD(38, rx_end, end)
GETFUNCFIELD(38, rx_error, error)
SETFUNCFIELD(38, hg, c_hg, uint32 hg, hg)
GETFUNCFIELD(38, hg, c_hg)
SETFUNCFIELD(38, purge, c_purge, uint32 purge, purge)
GETFUNCFIELD(38, purge, c_purge)
GETFUNCFIELD(38, read_ecc_error, ecc_error)
GETFUNCFIELD(38, desc_status, c_desc_status)
GETFUNCFIELD(38, desc_intr, c_intr)
GETFUNCFIELD(38, desc_remaining, c_desc)

GETHDRFUNCFIELD(38, rx_timestamp, timestamp)
GETHDRFUNCFIELD(38, rx_timestamp_upper, timestamp_hi)
GETHDRPTREXPR(38, mhp, &(h->mh0))
GETHDRFUNCFIELD(38, outer_vid, outer_vid)
GETHDRFUNCFIELD(38, outer_pri, outer_pri)
GETHDRFUNCFIELD(38, outer_cfi, outer_cfi)
GETHDRFUNCFIELD(38, rx_outer_tag_action, word3.overlay1.otag_action)
GETHDRFUNCNULL(38, inner_vid)
GETHDRFUNCNULL(38, inner_pri)
GETHDRFUNCNULL(38, inner_cfi)
GETHDRFUNCNULL(38, rx_inner_tag_action)
GETHDRFUNCFIELD(38, rx_bpdu, word0.overlay1.bpdu)
GETHDRFUNCNULL(38, match_id_lo)
GETHDRFUNCNULL(38, match_id_hi)
GETHDRFUNCNULL(38, forwarding_type)
GETHDRFUNCNULL(38, forwarding_zone_id)

static uint32 ep_to_cpu_hdr38_rx_crc_get(void *hdr) {
    return 0;
}

static uint32
ep_to_cpu_hdr38_rx_untagged_get(void *hdr, int dt_mode, int ingport_is_hg)
{
    ep_to_cpu_hdr38_t *h = (ep_to_cpu_hdr38_t *)hdr;
    uint32 tag_status = h->word3.overlay1.tag_status;

    COMPILER_REFERENCE(dt_mode);
    /* We have a case where though the number of bits for the
     * tag status is 2, we support only 2 states
     * 0 = untagged
     * 1 = single outer-tag
     */
    return (ingport_is_hg ?
            ((tag_status) ? 0 : 2) :
            ((tag_status) ? 2 : 3));
}

static void
ep_to_cpu_hdr38_rx_reasons_get(dcb_op_t *dcb_op, void *hdr, soc_rx_reasons_t *reasons)
{
    soc_rx_reason_t *map, *encoding_map;
    uint32 opcode[2], encoding[3];
    uint32 bit_val;
    int word_idx, bit_idx, opcode_idx, map_idx, word_count;
    int enc_bit = 0;

    SOC_RX_REASON_CLEAR_ALL(*reasons);

    opcode[0] = dcb_op->rx_reason_get(hdr);
    opcode[1] = dcb_op->rx_reason_hi_get(hdr);
    word_count = 2;
    sal_memset(encoding, 0, sizeof(encoding));
    map = dcb_op->rx_reason_maps[0];
    for (word_idx = 0; word_idx < word_count; word_idx++) {
        for (bit_idx = 0; bit_idx < 32; bit_idx++) {
            opcode_idx = word_idx * 32 + bit_idx;
            bit_val = opcode[word_idx] & (1 << bit_idx) ? 1 : 0;
            for (map_idx = 0; ; map_idx++) {
                encoding_map = dcb_op->rx_reason_maps[1 + map_idx];
                if (encoding_map == NULL) {
                    break;
                }
                if (map[opcode_idx] != encoding_map[0]) {
                    continue;
                }
                if (opcode_idx == 0 || map[opcode_idx - 1] != encoding_map[0]) {
                    enc_bit = 0;
                }
                encoding[map_idx] |= bit_val << enc_bit;
                enc_bit++;
                break;
            }
            if (encoding_map == NULL && bit_val) {
                SOC_RX_REASON_SET(*reasons, map[opcode_idx]);
            }
        }
    }

    for (map_idx = 0; ; map_idx++) {
        encoding_map = dcb_op->rx_reason_maps[1 + map_idx];
        if (encoding_map == NULL) {
            break;
        }
        if (encoding[map_idx] != socRxReasonInvalid) {
            SOC_RX_REASON_SET(*reasons, encoding_map[encoding[map_idx]]);
        }
    }

    /* BPDU bit should be a reason, paste it in here */
    if (dcb_op->rx_bpdu_get(hdr)) {
        SOC_RX_REASON_SET(*reasons, socRxReasonBpdu);
    }

    return;
}

GETHDRFUNCFIELD(38, rx_cos, word0.overlay2.cpu_cos)
GETHDRHG2FUNCFIELD(38, rx_destmod, dst_mod)
GETHDRHG2FUNCFIELD(38, rx_destport, dst_port)

GETHDRHG2FUNCFIELD(38, rx_opcode, opcode)
GETHDRHG2FUNCEXPR(38, rx_classtag, (h->ppd_overlay1.ppd_type != 1 ? 0 :
                                 (h->ppd_overlay2.ctag_hi << 8) |
                                 (h->ppd_overlay2.ctag_lo)))
GETHDRFUNCFIELD(38, rx_matchrule, match_rule)
GETHDRHG2FUNCFIELD(38, rx_prio, vlan_pri) /* outer_pri */

static uint32
ep_to_cpu_hdr38_rx_reason_get(void *hdr)
{
    ep_to_cpu_hdr38_t *h = (ep_to_cpu_hdr38_t *)hdr;

    return (h->switch_pkt) ? (h->reason) : 0;
}

static uint32
ep_to_cpu_hdr38_rx_reason_hi_get(void *hdr)
{
    ep_to_cpu_hdr38_t *h = (ep_to_cpu_hdr38_t *)hdr;

    return (h->switch_pkt) ? (h->reason_hi) : 0;
}

GETHDRFUNCFIELD(38, rx_ingport, srcport)
GETHDRHG2FUNCFIELD(38, rx_srcport, src_port)
GETHDRHG2FUNCFIELD(38, rx_srcmod, src_mod)
GETHDRHG2FUNCEXPR(38, rx_mcast, ((h->ppd_overlay1.dst_mod << 8) |
                              (h->ppd_overlay1.dst_port)))
GETHDRHG2FUNCEXPR(38, rx_vclabel, ((h->ppd_overlay1.vc_label_19_16 << 16) |
                              (h->ppd_overlay1.vc_label_15_8 << 8) |
                              (h->ppd_overlay1.vc_label_7_0)))
GETHDRFUNCEXPR(38, rx_mirror, ((SOC_CPU_OPCODE_TYPE_IP_0 ==
                          h->word3.overlay1.cpu_opcode_type) ?
                          (h->reason & (1 << 17)) : 0))

static _shr_rx_decap_tunnel_t dcb38_rx_decap_tunnel_map[] = {
    _SHR_RX_DECAP_NONE,
    _SHR_RX_DECAP_IP,
    _SHR_RX_DECAP_L2MPLS_1LABEL,
    _SHR_RX_DECAP_L2MPLS_2LABEL,
    _SHR_RX_DECAP_MPLS_3LABEL,
    _SHR_RX_DECAP_L3MPLS_1LABEL,
    _SHR_RX_DECAP_L3MPLS_2LABEL,
    _SHR_RX_DECAP_MPLS_3LABEL
};

static uint32 ep_to_cpu_hdr38_rx_decap_tunnel_get(void *hdr) {
    ep_to_cpu_hdr38_t *h = (ep_to_cpu_hdr38_t *)hdr;
    /* Check for array bounds */
    if ( (h->decap_tunnel_type <
    (sizeof (dcb38_rx_decap_tunnel_map) / sizeof (_shr_rx_decap_tunnel_t))) ) {
        return (uint32)dcb38_rx_decap_tunnel_map[h->decap_tunnel_type];
    }
    return (uint32)_SHR_RX_DECAP_NONE;
}
GETHDRFUNCFIELD(38, rx_l3_intf, word3.overlay1.ing_l3_intf)
GETHDRFUNCEXPR(38, replication_or_nhi, h->replication_or_nhi)
GETHDRFUNCNULL(38, rx_switch_drop)
GETHDRFUNCNULL(38, olp_encap_oam_pkt)

#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb38_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[2] = d[3] = 0;
}
SETFUNCFIELD(38, xfercount, count, uint32 count, count)
SETFUNCFIELD(38, rx_start, start, int val, val ? 1 : 0)
SETFUNCFIELD(38, rx_end, end, int val, val ? 1 : 0)
SETFUNCFIELD(38, rx_error, error, int val, val ? 1 : 0)
SETHDRFUNCEXPRIGNORE(38, rx_crc, int val, ignore)
SETHDRFUNCFIELD(38, rx_ingport, srcport, int val, val)
#endif  /* PLISIM */

static void
dcb38_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[0] = d[1] = d[2] = d[3] = 0;
}

static int
dcb38_add_rx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb38_t           *d;     /* DCB */
    uint32            *di;    /* DCB integer pointer */
    sal_paddr_t        paddr;

    if(!addr)
        return SOC_E_PARAM;

    d = (dcb38_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = 0;

    if (addr) {
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);
        d->addr_lo = PTR_TO_INT(paddr);
        d->addr_hi = (0x10000000 | PTR_HI_TO_INT(paddr));
    }

    d->c_count = count;
    d->c_sg = 1;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static int
dcb38_add_tx(dv_t *dv, sal_vaddr_t addr, uint32 count,
            pbmp_t l2pbm, pbmp_t utpbm, pbmp_t l3pbm, uint32 flags, uint32 *hgh)
{
    dcb38_t           *d;     /* DCB */
    uint32            *di;    /* DCB integer pointer */
    sal_paddr_t       paddr;  /* Packet buffer physical address */

    if(!addr)
        return SOC_E_PARAM;

    d = (dcb38_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (addr)
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);

    if (dv->dv_vcnt > 0 && (dv->dv_flags & DV_F_COMBINE_DCB) &&
        (d[-1].c_sg != 0) &&
        ((d[-1].addr_lo + d[-1].c_count) == paddr) &&
        (d[-1].c_count + count <= DCB_MAX_REQCOUNT)) {
        d[-1].c_count += count;
        return dv->dv_cnt - dv->dv_vcnt;
    }

    if (dv->dv_vcnt >= dv->dv_cnt) {
        return SOC_E_FULL;
    }
    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = 0;

    d->addr_lo = PTR_TO_INT(paddr);
    d->addr_hi = (0x10000000 | PTR_HI_TO_INT(paddr));
    d->c_count = count;
    d->c_sg = 1;

    d->c_purge = SOC_DMA_PURGE_GET(flags);

    if (SOC_DMA_HG_GET(flags)) {
        d->c_hg = 1;
    }

    dv->dv_vcnt += 1;

    return dv->dv_cnt - dv->dv_vcnt;
}

static uint32
dcb38_intr_info(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb38_t *d = (dcb38_t *)dcb;     /*  DCB */
    uint32  f;                             /* SOC_DCB_INFO_* flags */

    if (!d->done) {
        return 0;
    }

    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }

    *count = d->count;
    return f;
}

/* the addr related functions are the same for all dcb types */
static void
dcb38_addr_set(int unit, dcb_t *dcb, sal_vaddr_t addr)
{
    dcb38_t      *d = (dcb38_t *)dcb;     /*  DCB */
    sal_paddr_t  paddr;

    if (addr == 0) {
        d->addr_lo = 0;
        d->addr_hi = 0;
    } else {
        paddr = soc_cm_l2p(unit, (void *)addr);
        d->addr_lo = PTR_TO_INT(paddr);
        d->addr_hi = (0x10000000 | PTR_HI_TO_INT(paddr));
    }
}

static sal_vaddr_t
dcb38_addr_get(int unit, dcb_t *dcb)
{
    dcb38_t      *d = (dcb38_t *)dcb;     /*  DCB */
    sal_paddr_t        paddr;

#if defined(COMPILER_OVERRIDE_NO_LONGLONG) || defined(__PEDANTIC__)
    paddr = d->addr_lo;
#else
    paddr = (((uint64)(d->addr_hi & ~(0x10000000)) << 32) | d->addr_lo);
#endif

    return (sal_vaddr_t)soc_cm_p2l(unit, paddr);
}

static sal_paddr_t
dcb38_paddr_get(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    return (sal_paddr_t)*d;
}

static void
dcb38_desc_intr_set(dcb_t * dcb, int val)
{
    dcb38_t * d = (dcb38_t *)dcb;

    d->c_intr = val;
    return;
}

static uint32
dcb38_desc_status_set(dcb_t * dcb, uint32 val)
{
    dcb38_t * d = (dcb38_t *)dcb;

    d->c_desc_status = val;
    return 0;
}

static uint32
dcb38_desc_remaining_set(dcb_t * dcb, uint32 val)
{
    dcb38_t * d = (dcb38_t *)dcb;

    d->c_desc = val;
    return 0;
}

#ifdef BROADCOM_DEBUG
static void
dcb38_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));

    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %spurge \n"),
                         prefix,
                         SOC_DCB_TYPE(unit),
                         SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));

    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }
}
#endif

/*
 * DCB Type 38 Support
 */
/* From FORMAT CPU_OPCODES */
static soc_rx_reason_t
dcb38_rx_reason_map[] = {
    socRxReasonUnknownVlan,        /*  0: CPU_UVLAN */
    socRxReasonL2SourceMiss,       /*  1: CPU_SLF */
    socRxReasonL2DestMiss,         /*  2: CPU_DLF */
    socRxReasonL2Move,             /*  3: CPU_L2MOVE */
    socRxReasonL2Cpu,              /*  4: CPU_L2CPU */
    socRxReasonL3SourceMiss,       /*  5: CPU_L3SRC_MISS */
    socRxReasonL3DestMiss,         /*  6: CPU_L3DST_MISS */
    socRxReasonL3SourceMove,       /*  7: CPU_L3SRC_MOVE */
    socRxReasonMcastMiss,          /*  8: CPU_MC_MISS */
    socRxReasonIpMcastMiss,        /*  9: CPU_IPMC_MISS */
    socRxReasonFilterMatch,        /* 10: CPU_FFP */
    socRxReasonL3HeaderError,      /* 11: CPU_L3HDR_ERR */
    socRxReasonProtocol,           /* 12: CPU_PROTOCOL_PKT */
    socRxReasonDosAttack,          /* 13: CPU_DOS_ATTACK */
    socRxReasonMartianAddr,        /* 14: CPU_MARTIAN_ADDR */
    socRxReasonTunnelError,        /* 15: CPU_TUNNEL_ERR */
    socRxReasonSampleSourceFlex,   /* 16: CPU_SFLOW_FLEX */
    socRxReasonSampleSourceFlex,   /* 17: CPU_SFLOW_FLEX */
    socRxReasonIcmpRedirect,       /* 18: ICMP_REDIRECT */
    socRxReasonL3Slowpath,         /* 19: L3_SLOWPATH */
    socRxReasonParityError,        /* 20: PARITY_ERROR */
    socRxReasonL3MtuFail,          /* 21: L3_MTU_CHECK_FAIL */
    socRxReasonMcastIdxError,      /* 22: MCIDX_ERROR */
    socRxReasonVlanFilterMatch,    /* 23: VFP */
    socRxReasonMplsError,          /* 24: MPLS_LABEL_MISS */
    socRxReasonMplsError,          /* 25: MPLS_INVALID_ACTION */
    socRxReasonMplsError,          /* 26: MPLS_INVALID_PAYLOAD */
    socRxReasonL2NonUnicastMiss,   /* 27: PBT_NONUC_PKT */
    socRxReasonNhop,               /* 28: L3_NEXT_HOP */
    socRxReasonStation,            /* 29: MY_STATION */
    socRxReasonTimeSync,           /* 30: TIME_SYNC */
    socRxReasonTunnelDecapEcnError,/* 31: TUNNEL_DECAP_ECN_ERROR (new) */
    socRxReasonBfdSlowpath,        /* 32: BFD SLOWPATH */
    socRxReasonBfd,                /* 33: BFD_ERROR */
#if 1
    socRxReasonInvalid,            /* 34:NOT DEFINED */
#endif
    socRxReasonMplsUnknownAch,     /* 35: MPLS_UNKNOWN_ACH_ERROR */
    socRxReasonMplsRouterAlertLabel, /* 36: MPLS_ALERT_LABEL */
    socRxReasonIpmcInterfaceMismatch, /* 37: CPU_IPMC_INTERFACE_MISMATCH */
    socRxReasonDlbMonitor,         /* 38: DLB_MONITOR */
    socRxReasonIntTurnAround,      /* 39: INBAND_TELEMETRY */
    socRxReasonEtrapMonitor,       /* 40: ETRAP_MONITOR */
    socRxReasonInvalid,            /* 41: Reserved */
    socRxReasonInvalid,            /* 42: Reserved */
    socRxReasonInvalid,            /* 43: Reserved */
    socRxReasonInvalid,            /* 44: Reserved */
    socRxReasonInvalid,            /* 45: Reserved */
    socRxReasonInvalid,            /* 46: Reserved */
    socRxReasonInvalid,            /* 47: Reserved */
    socRxReasonInvalid,            /* 48: Reserved */
    socRxReasonInvalid,            /* 49: Reserved */
    socRxReasonInvalid,            /* 50: Reserved */
    socRxReasonInvalid,            /* 51: Reserved */
    socRxReasonInvalid,            /* 52: Reserved */
    socRxReasonInvalid,            /* 53: Reserved */
    socRxReasonInvalid,            /* 54: Reserved */
    socRxReasonInvalid,            /* 55: Reserved */
    socRxReasonInvalid,            /* 56: Reserved */
    socRxReasonInvalid,            /* 57: Reserved */
    socRxReasonInvalid,            /* 58: Reserved */
    socRxReasonInvalid,            /* 59: Reserved */
    socRxReasonInvalid,            /* 60: Reserved */
    socRxReasonInvalid,            /* 61: Reserved */
    socRxReasonInvalid,            /* 62: Reserved */
    socRxReasonInvalid             /* 63: Reserved */
};

/* From CPU_SFLOW encoding from TH3 regsfile */
static soc_rx_reason_t dcb38_rx_reason_cpu_sflow_encoding_map[] = {
    socRxReasonSampleSourceFlex,   /* 0: NO_ERRORS
                                    * Basefield, must match the entries above */
    socRxReasonSampleSourceFlex,   /* 1: CPU_SFLOW_FLEX */
    socRxReasonSampleDest,         /* 2: CPU_SFLOW_DST */
    socRxReasonSampleSource,       /* 3: CPU_SFLOW_SRC */
    socRxReasonInvalid,            /* Offset 4 */
    socRxReasonInvalid,            /* Offset 5 */
    socRxReasonInvalid,            /* Offset 6 */
    socRxReasonInvalid,            /* Offset 7 */
    socRxReasonInvalid,            /* Offset 8 */
    socRxReasonInvalid,            /* Offset 9 */
    socRxReasonInvalid,            /* Offset 10 */
    socRxReasonInvalid,            /* Offset 11 */
    socRxReasonInvalid,            /* Offset 12 */
    socRxReasonInvalid,            /* Offset 13 */
    socRxReasonInvalid,            /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

/* From MPLS_PROC_ERROR encoding in TH3 regsfile */
static soc_rx_reason_t dcb38_rx_reason_mpls_proc_err_encoding_map[] = {
    socRxReasonMplsError,          /* 0: NO_ERRORS
                                    * Basefield, must match the entries above */
    socRxReasonMplsInvalidPayload, /* 1: Invalid Payload */
    socRxReasonMplsInvalidAction,  /* 2: Invalid Action */
    socRxReasonMplsLabelMiss,      /* 3: Lookup Label Miss */
    socRxReasonMplsTtl,            /* 4: TTL Check Fail */
    socRxReasonInvalid,            /* Offset 5 */
    socRxReasonInvalid,            /* Offset 6 */
    socRxReasonInvalid,            /* Offset 7 */
    socRxReasonInvalid,            /* Offset 8 */
    socRxReasonInvalid,            /* Offset 9 */
    socRxReasonInvalid,            /* Offset 10 */
    socRxReasonInvalid,            /* Offset 11 */
    socRxReasonInvalid,            /* Offset 12 */
    socRxReasonInvalid,            /* Offset 13 */
    socRxReasonInvalid,            /* Offset 14 */
    socRxReasonInvalid,            /* Offset 15 */
    socRxReasonInvalid,            /* Offset 16 */
    socRxReasonInvalid,            /* Offset 17 */
    socRxReasonInvalid,            /* Offset 18 */
    socRxReasonInvalid,            /* Offset 19 */
    socRxReasonInvalid,            /* Offset 20 */
    socRxReasonInvalid,            /* Offset 21 */
    socRxReasonInvalid,            /* Offset 22 */
    socRxReasonInvalid,            /* Offset 23 */
    socRxReasonInvalid,            /* Offset 24 */
    socRxReasonInvalid,            /* Offset 25 */
    socRxReasonInvalid,            /* Offset 26 */
    socRxReasonInvalid,            /* Offset 27 */
    socRxReasonInvalid,            /* Offset 28 */
    socRxReasonInvalid,            /* Offset 29 */
    socRxReasonInvalid,            /* Offset 30 */
    socRxReasonInvalid,            /* Offset 31 */
    socRxReasonInvalid,            /* Offset 32 */
    socRxReasonInvalid,            /* Offset 33 */
    socRxReasonInvalid,            /* Offset 34 */
    socRxReasonInvalid,            /* Offset 35 */
    socRxReasonInvalid,            /* Offset 36 */
    socRxReasonInvalid,            /* Offset 37 */
    socRxReasonInvalid,            /* Offset 38 */
    socRxReasonInvalid,            /* Offset 39 */
    socRxReasonInvalid,            /* Offset 40 */
    socRxReasonInvalid,            /* Offset 41 */
    socRxReasonInvalid,            /* Offset 42 */
    socRxReasonInvalid,            /* Offset 43 */
    socRxReasonInvalid,            /* Offset 44 */
    socRxReasonInvalid,            /* Offset 45 */
    socRxReasonInvalid,            /* Offset 46 */
    socRxReasonInvalid,            /* Offset 47 */
    socRxReasonInvalid,            /* Offset 48 */
    socRxReasonInvalid,            /* Offset 49 */
    socRxReasonInvalid,            /* Offset 50 */
    socRxReasonInvalid,            /* Offset 51 */
    socRxReasonInvalid,            /* Offset 52 */
    socRxReasonInvalid,            /* Offset 53 */
    socRxReasonInvalid,            /* Offset 54 */
    socRxReasonInvalid,            /* Offset 55 */
    socRxReasonInvalid,            /* Offset 56 */
    socRxReasonInvalid,            /* Offset 57 */
    socRxReasonInvalid,            /* Offset 58 */
    socRxReasonInvalid,            /* Offset 59 */
    socRxReasonInvalid,            /* Offset 60 */
    socRxReasonInvalid,            /* Offset 61 */
    socRxReasonInvalid,            /* Offset 62 */
    socRxReasonInvalid             /* Offset 63 */
};

static soc_rx_reason_t *dcb38_rx_reason_maps[] = {
    dcb38_rx_reason_map,
    dcb38_rx_reason_cpu_sflow_encoding_map,
    dcb38_rx_reason_mpls_proc_err_encoding_map,
    NULL
};

SETFUNCERR(38, tx_l2pbm, pbmp_t)
SETFUNCERR(38, tx_utpbm, pbmp_t)
SETFUNCERR(38, tx_l3pbm, pbmp_t)
SETFUNCERR(38, tx_crc, int)
SETFUNCERR(38, tx_cos, int)
SETFUNCERR(38, tx_destmod, uint32)
SETFUNCERR(38, tx_destport, uint32)
SETFUNCERR(38, tx_opcode, uint32)
SETFUNCERR(38, tx_srcmod, uint32)
SETFUNCERR(38, tx_srcport, uint32)
SETFUNCERR(38, tx_prio, uint32)
SETFUNCERR(38, tx_pfm, uint32)
#ifdef  BROADCOM_DEBUG
GETFUNCERR(38, tx_l2pbm)
GETFUNCERR(38, tx_utpbm)
GETFUNCERR(38, tx_l3pbm)
GETFUNCERR(38, tx_crc)
GETFUNCERR(38, tx_cos)
GETFUNCERR(38, tx_destmod)
GETFUNCERR(38, tx_destport)
GETFUNCERR(38, tx_opcode)
GETFUNCERR(38, tx_srcmod)
GETFUNCERR(38, tx_srcport)
GETFUNCERR(38, tx_prio)
GETFUNCERR(38, tx_pfm)
#endif /* BROADCOM_DEBUG */

dcb_op_t dcb38_op = {
    38,
    sizeof(dcb38_t),
    dcb38_rx_reason_maps,
    dcb0_rx_reason_map_get,
    ep_to_cpu_hdr38_rx_reasons_get,
    dcb38_init,
    dcb38_add_tx,
    dcb38_add_rx,
    dcb38_intr_info,
    dcb38_reqcount_set,
    dcb38_reqcount_get,
    dcb38_xfercount_get,
    dcb38_addr_set,
    dcb38_addr_get,
    dcb38_paddr_get,
    dcb38_done_set,
    dcb38_done_get,
    dcb38_sg_set,
    dcb38_sg_get,
    dcb38_chain_set,
    dcb38_chain_get,
    dcb38_reload_set,
    dcb38_reload_get,
    dcb38_desc_intr_set,
    dcb38_desc_intr_get,
    dcb38_tx_l2pbm_set,
    dcb38_tx_utpbm_set,
    dcb38_tx_l3pbm_set,
    dcb38_tx_crc_set,
    dcb38_tx_cos_set,
    dcb38_tx_destmod_set,
    dcb38_tx_destport_set,
    dcb38_tx_opcode_set,
    dcb38_tx_srcmod_set,
    dcb38_tx_srcport_set,
    dcb38_tx_prio_set,
    dcb38_tx_pfm_set,
    ep_to_cpu_hdr38_rx_untagged_get,
    ep_to_cpu_hdr38_rx_crc_get,
    ep_to_cpu_hdr38_rx_cos_get,
    ep_to_cpu_hdr38_rx_destmod_get,
    ep_to_cpu_hdr38_rx_destport_get,
    ep_to_cpu_hdr38_rx_opcode_get,
    ep_to_cpu_hdr38_rx_classtag_get,
    ep_to_cpu_hdr38_rx_matchrule_get,
    dcb38_rx_start_get,
    dcb38_rx_end_get,
    dcb38_rx_error_get,
    ep_to_cpu_hdr38_rx_prio_get,
    ep_to_cpu_hdr38_rx_reason_get,
    ep_to_cpu_hdr38_rx_reason_hi_get,
    ep_to_cpu_hdr38_rx_ingport_get,
    ep_to_cpu_hdr38_rx_srcport_get,
    ep_to_cpu_hdr38_rx_srcmod_get,
    ep_to_cpu_hdr38_rx_mcast_get,
    ep_to_cpu_hdr38_rx_vclabel_get,
    ep_to_cpu_hdr38_rx_mirror_get,
    ep_to_cpu_hdr38_rx_timestamp_get,
    ep_to_cpu_hdr38_rx_timestamp_upper_get,
    dcb38_hg_set,
    dcb38_hg_get,
    NULL,
    NULL,
    dcb38_purge_set,
    dcb38_purge_get,
    ep_to_cpu_hdr38_mhp_get,
    ep_to_cpu_hdr38_outer_vid_get,
    ep_to_cpu_hdr38_outer_pri_get,
    ep_to_cpu_hdr38_outer_cfi_get,
    ep_to_cpu_hdr38_rx_outer_tag_action_get,
    ep_to_cpu_hdr38_inner_vid_get,
    ep_to_cpu_hdr38_inner_pri_get,
    ep_to_cpu_hdr38_inner_cfi_get,
    ep_to_cpu_hdr38_rx_inner_tag_action_get,
    ep_to_cpu_hdr38_rx_bpdu_get,
    ep_to_cpu_hdr38_rx_l3_intf_get,
    ep_to_cpu_hdr38_rx_decap_tunnel_get,
    ep_to_cpu_hdr38_rx_switch_drop_get,
    ep_to_cpu_hdr38_olp_encap_oam_pkt_get,
    dcb38_read_ecc_error_get,
    dcb38_desc_remaining_get,
    dcb38_desc_remaining_set,
    dcb38_desc_status_get,
    dcb38_desc_status_set,
    ep_to_cpu_hdr38_match_id_lo_get,
    ep_to_cpu_hdr38_match_id_hi_get,
    ep_to_cpu_hdr38_forwarding_type_get,
    ep_to_cpu_hdr38_forwarding_zone_id_get,
#ifdef BROADCOM_DEBUG
    dcb38_tx_l2pbm_get,
    dcb38_tx_utpbm_get,
    dcb38_tx_l3pbm_get,
    dcb38_tx_crc_get,
    dcb38_tx_cos_get,
    dcb38_tx_destmod_get,
    dcb38_tx_destport_get,
    dcb38_tx_opcode_get,
    dcb38_tx_srcmod_get,
    dcb38_tx_srcport_get,
    dcb38_tx_prio_get,
    dcb38_tx_pfm_get,

    dcb38_dump,
    dcb0_reason_dump,
#endif
#ifdef  PLISIM
    dcb38_status_init,
    dcb38_xfercount_set,
    dcb38_rx_start_set,
    dcb38_rx_end_set,
    dcb38_rx_error_set,
    ep_to_cpu_hdr38_rx_crc_set,
    ep_to_cpu_hdr38_rx_ingport_set,
#endif /* PLISIM */
    NULL,
    ep_to_cpu_hdr38_replication_or_nhi_get,
};
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef BCM_DNX_SUPPORT
SETFUNCFIELD(39, reqcount, c_count, uint32 count, count)
GETFUNCFIELD(39, reqcount, c_count)
SETFUNCFIELD(39, chain, c_chain, int val, val ? 1 : 0)
GETFUNCFIELD(39, chain, c_chain)
SETFUNCFIELD(39, sg, c_sg, int val, val ? 1 : 0)
GETFUNCFIELD(39, sg, c_sg)
SETFUNCFIELD(39, reload, c_reload, int val, val ? 1 : 0)
GETFUNCFIELD(39, reload, c_reload)
SETFUNCFIELD(39, hg, c_hg, uint32 hg, hg)
GETFUNCFIELD(39, hg, c_hg)
SETFUNCFIELD(39, purge, c_purge, uint32 purge, purge)
GETFUNCFIELD(39, purge, c_purge)

GETFUNCFIELD(39, desc_intr, c_intr)
GETFUNCFIELD(39, desc_remaining, c_desc)
GETFUNCFIELD(39, desc_status, c_desc_status)


GETFUNCFIELD(39, xfercount, count)
GETFUNCFIELD(39, rx_end, end)
GETFUNCFIELD(39, rx_start, start)
GETFUNCFIELD(39, rx_error, error)
GETFUNCFIELD(39, read_ecc_error, ecc_error)
SETFUNCFIELD(39, done, done, int val, val ? 1 : 0)
GETFUNCFIELD(39, done, done)


#ifdef  BROADCOM_DEBUG
GETFUNCERR(39, tx_l2pbm)
GETFUNCERR(39, tx_utpbm)
GETFUNCERR(39, tx_l3pbm)
GETFUNCERR(39, tx_crc)
GETFUNCERR(39, tx_cos)
GETFUNCERR(39, tx_destmod)
GETFUNCERR(39, tx_destport)
GETFUNCERR(39, tx_opcode)
GETFUNCERR(39, tx_srcmod)
GETFUNCERR(39, tx_srcport)
GETFUNCERR(39, tx_prio)
GETFUNCERR(39, tx_pfm)
#endif /* BROADCOM_DEBUG */


#ifdef  PLISIM          /* these routines are only used by pcid */
static void dcb39_status_init(dcb_t *dcb)
{
    return;
}
SETFUNCERR(39, xfercount, uint32)
SETFUNCERR(39, rx_start, int)
SETFUNCERR(39, rx_end, int)
SETFUNCERR(39, rx_error, int)
SETFUNCERR(39, rx_crc,int)
SETFUNCERR(39, rx_ingport, int)
#endif

static void
dcb39_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[0] = d[1] = d[2] = d[3] = 0;
}

static int
dcb39_add_rx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb39_t     *d;     /* DCB */
    uint32            *di;    /* DCB integer pointer */
    sal_paddr_t        paddr;

    d = (dcb39_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = 0;

    if (addr) {
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);
        d->addr_lo = PTR_TO_INT(paddr);
        d->addr_hi = PTR_HI_TO_INT(paddr);
        if (soc_cm_get_bus_type(dv->dv_unit) & SOC_PCI_DEV_TYPE) {
            d->addr_hi |= 0x10000000;
        }
    }

    d->c_count = count;
    d->c_sg = 1;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static int
dcb39_add_tx(dv_t *dv, sal_vaddr_t addr, uint32 count,
            pbmp_t l2pbm, pbmp_t utpbm, pbmp_t l3pbm, uint32 flags, uint32 *hgh)
{
    dcb39_t     *d;     /* DCB */
    uint32            *di;    /* DCB integer pointer */
    sal_paddr_t       paddr = 0;  /* Packet buffer physical address */

    d = (dcb39_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (addr)
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);

    if (dv->dv_vcnt > 0 && (dv->dv_flags & DV_F_COMBINE_DCB) &&
        (d[-1].c_sg != 0) &&
        ((d[-1].addr_lo + d[-1].c_count) == paddr) &&
        (d[-1].c_count + count <= DCB_MAX_REQCOUNT)) {
        d[-1].c_count += count;
        return dv->dv_cnt - dv->dv_vcnt;
    }

    if (dv->dv_vcnt >= dv->dv_cnt) {
        return SOC_E_FULL;
    }
    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = 0;

    d->addr_lo = PTR_TO_INT(paddr);
    d->addr_hi = PTR_HI_TO_INT(paddr);
    if (soc_cm_get_bus_type(dv->dv_unit) & SOC_PCI_DEV_TYPE) {
        d->addr_hi |= 0x10000000;
    }
    d->c_count = count;
    d->c_sg = 1;

    d->c_purge = SOC_DMA_PURGE_GET(flags);

    if (SOC_DMA_HG_GET(flags)) {
        d->c_hg = 1;
    }

    dv->dv_vcnt += 1;

    return dv->dv_cnt - dv->dv_vcnt;
}

static uint32
dcb39_intr_info(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb39_t      *d = (dcb39_t *)dcb;     /*  DCB */
    uint32      f;                                    /* SOC_DCB_INFO_* flags */

    if (!d->done) {
        return 0;
    }

    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }

    *count = d->count;
    return f;
}

/* the addr related functions are the same for all dcb types */
static void
dcb39_addr_set(int unit, dcb_t *dcb, sal_vaddr_t addr)
{
    dcb39_t      *d = (dcb39_t *)dcb;     /*  DCB */
    sal_paddr_t        paddr;

    if (addr == 0) {
        d->addr_lo = 0;
        d->addr_hi = 0;
    } else {
        paddr = soc_cm_l2p(unit, (void *)addr);
        d->addr_lo = PTR_TO_INT(paddr);
        d->addr_hi = PTR_HI_TO_INT(paddr);
        if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
            d->addr_hi |= 0x10000000;
        }
    }
}

static sal_vaddr_t
dcb39_addr_get(int unit, dcb_t *dcb)
{
    dcb39_t      *d = (dcb39_t *)dcb;     /*  DCB */
    sal_paddr_t        paddr;

#if defined(COMPILER_OVERRIDE_NO_LONGLONG) || defined(__PEDANTIC__)
    paddr = d->addr_lo;
#else
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        paddr = (((uint64)(d->addr_hi & ~(0x10000000)) << 32) | d->addr_lo);
    } else {
        paddr = (((uint64)(d->addr_hi) << 32) | d->addr_lo);
    }
#endif

    return (sal_vaddr_t)soc_cm_p2l(unit, paddr);
}

static sal_paddr_t
dcb39_paddr_get(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    return (sal_paddr_t)*d;
}

static void
dcb39_desc_intr_set(dcb_t * dcb, int val)
{
    dcb39_t * d = (dcb39_t *)dcb;

    d->c_intr = val;
    return;
}

static uint32
dcb39_desc_status_set(dcb_t * dcb, uint32 val)
{
    dcb39_t * d = (dcb39_t *)dcb;

    d->c_desc_status = val;
    return 0;
}

static uint32
dcb39_desc_remaining_set(dcb_t * dcb, uint32 val)
{
    dcb39_t * d = (dcb39_t *)dcb;

    d->c_desc = val;
    return 0;
}

#ifdef BROADCOM_DEBUG
static void
dcb39_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_snprintf(&ps[i*9], 9*sizeof(char), "%08x ", p[i]);
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));

    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %spurge \n"),
                         prefix,
                         SOC_DCB_TYPE(unit),
                         SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));

    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }
}
#endif


SETFUNCERR(39, tx_l2pbm, pbmp_t)
SETFUNCERR(39, tx_utpbm, pbmp_t)
SETFUNCERR(39, tx_l3pbm, pbmp_t)
SETFUNCERR(39, tx_crc, int)
SETFUNCERR(39, tx_cos, int)
SETFUNCERR(39, tx_destmod, uint32)
SETFUNCERR(39, tx_destport, uint32)
SETFUNCERR(39, tx_opcode, uint32)
SETFUNCERR(39, tx_srcmod, uint32)
SETFUNCERR(39, tx_srcport, uint32)
SETFUNCERR(39, tx_prio, uint32)
SETFUNCERR(39, tx_pfm, uint32)

static uint32
dcb39_rx_untagged_get(dcb_t *dcb, int dt_mode, int ingport_is_hg)
{
    dcb0_funcerr(39, "rx_untagged_get");
    return 0;
}
GETFUNCERR(39, rx_crc)
GETFUNCERR(39, rx_cos)
GETFUNCERR(39, rx_destmod)
GETFUNCERR(39, rx_destport)
GETFUNCERR(39, rx_opcode)
GETFUNCERR(39, rx_classtag)
GETFUNCERR(39, rx_matchrule)

GETFUNCERR(39, rx_pri)
GETFUNCERR(39, rx_reason)
GETFUNCERR(39, rx_reason_hi)
GETFUNCERR(39, rx_ingport)
GETFUNCERR(39, rx_srcport)
GETFUNCERR(39, rx_srcmod)
GETFUNCERR(39, rx_mcast)
GETFUNCERR(39, rx_vclabel)
GETFUNCERR(39, rx_mirror)
GETFUNCERR(39, rx_timestamp)
GETFUNCERR(39, rx_timestamp_upper)

static uint32*
dcb39_mhp_get(dcb_t *dcb)
{
    dcb0_funcerr(39, "mhp_get");
    return NULL;
}
GETFUNCERR(39, outer_vid)
GETFUNCERR(39, outer_pri)
GETFUNCERR(39, outer_cfi)
GETFUNCERR(39, rx_outer_tag_action)
GETFUNCERR(39, inner_vid)
GETFUNCERR(39, inner_pri)
GETFUNCERR(39, inner_cfi)
GETFUNCERR(39, rx_inner_tag_action)
GETFUNCERR(39, rx_bpdu)
GETFUNCERR(39, rx_l3_intf)
GETFUNCERR(39, rx_decap_tunnel)
GETFUNCERR(39, rx_switch_drop)
GETFUNCERR(39, olp_encap_oam_pkt)

GETFUNCERR(39, match_id_lo)
GETFUNCERR(39, match_id_hi)
GETFUNCERR(39, forwarding_type)
GETFUNCERR(39, forwarding_zone_id)

static soc_rx_reason_t *dcb39_rx_reason_maps[] = {
    NULL
};

dcb_op_t dcb39_op = {
    39,
    sizeof(dcb39_t),
    dcb39_rx_reason_maps,
    dcb0_rx_reason_map_get,
    dcb0_rx_reasons_get,
    dcb39_init,
    dcb39_add_tx,
    dcb39_add_rx,
    dcb39_intr_info,
    dcb39_reqcount_set,
    dcb39_reqcount_get,
    dcb39_xfercount_get,
    dcb39_addr_set,
    dcb39_addr_get,
    dcb39_paddr_get,
    dcb39_done_set,
    dcb39_done_get,
    dcb39_sg_set,
    dcb39_sg_get,
    dcb39_chain_set,
    dcb39_chain_get,
    dcb39_reload_set,
    dcb39_reload_get,
    dcb39_desc_intr_set,
    dcb39_desc_intr_get,
    dcb39_tx_l2pbm_set,
    dcb39_tx_utpbm_set,
    dcb39_tx_l3pbm_set,
    dcb39_tx_crc_set,
    dcb39_tx_cos_set,
    dcb39_tx_destmod_set,
    dcb39_tx_destport_set,
    dcb39_tx_opcode_set,
    dcb39_tx_srcmod_set,
    dcb39_tx_srcport_set,
    dcb39_tx_prio_set,
    dcb39_tx_pfm_set,
    dcb39_rx_untagged_get,
    dcb39_rx_crc_get,
    dcb39_rx_cos_get,
    dcb39_rx_destmod_get,
    dcb39_rx_destport_get,
    dcb39_rx_opcode_get,
    dcb39_rx_classtag_get,
    dcb39_rx_matchrule_get,
    dcb39_rx_start_get,
    dcb39_rx_end_get,
    dcb39_rx_error_get,
    dcb39_rx_pri_get,
    dcb39_rx_reason_get,
    dcb39_rx_reason_hi_get,
    dcb39_rx_ingport_get,
    dcb39_rx_srcport_get,
    dcb39_rx_srcmod_get,
    dcb39_rx_mcast_get,
    dcb39_rx_vclabel_get,
    dcb39_rx_mirror_get,
    dcb39_rx_timestamp_get,
    dcb39_rx_timestamp_upper_get,
    dcb39_hg_set,
    dcb39_hg_get,
    NULL,
    NULL,
    dcb39_purge_set,
    dcb39_purge_get,
    dcb39_mhp_get,
    dcb39_outer_vid_get,
    dcb39_outer_pri_get,
    dcb39_outer_cfi_get,
    dcb39_rx_outer_tag_action_get,
    dcb39_inner_vid_get,
    dcb39_inner_pri_get,
    dcb39_inner_cfi_get,
    dcb39_rx_inner_tag_action_get,
    dcb39_rx_bpdu_get,
    dcb39_rx_l3_intf_get,
    dcb39_rx_decap_tunnel_get,
    dcb39_rx_switch_drop_get,
    dcb39_olp_encap_oam_pkt_get,
    dcb39_read_ecc_error_get,
    dcb39_desc_remaining_get,
    dcb39_desc_remaining_set,
    dcb39_desc_status_get,
    dcb39_desc_status_set,
    dcb39_match_id_lo_get,
    dcb39_match_id_hi_get,
    dcb39_forwarding_type_get,
    dcb39_forwarding_zone_id_get,
#ifdef BROADCOM_DEBUG
    dcb39_tx_l2pbm_get,
    dcb39_tx_utpbm_get,
    dcb39_tx_l3pbm_get,
    dcb39_tx_crc_get,
    dcb39_tx_cos_get,
    dcb39_tx_destmod_get,
    dcb39_tx_destport_get,
    dcb39_tx_opcode_get,
    dcb39_tx_srcmod_get,
    dcb39_tx_srcport_get,
    dcb39_tx_prio_get,
    dcb39_tx_pfm_get,
    dcb39_dump,
    dcb0_reason_dump,
#endif
#ifdef  PLISIM
    dcb39_status_init,
    dcb39_xfercount_set,
    dcb39_rx_start_set,
    dcb39_rx_end_set,
    dcb39_rx_error_set,
    dcb39_rx_crc_set,
    dcb39_rx_ingport_set,
#endif /* PLISIM */
};
#endif /* BCM_DNX_SUPPORT */

#if defined(BCM_FIRELIGHT_SUPPORT)
/*
 * DCB Type 40 Support
 */

static void
dcb40_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[0] = d[1] = d[2] = d[3] = 0;
}

static int
dcb40_add_tx(dv_t *dv, sal_vaddr_t addr, uint32 count,
            pbmp_t l2pbm, pbmp_t utpbm, pbmp_t l3pbm, uint32 flags, uint32 *hgh)
{
    dcb40_t           *d;     /* DCB */
    uint32            *di;    /* DCB integer pointer */
    sal_paddr_t       paddr;  /* Packet buffer physical address */

    if(!addr)
        return SOC_E_PARAM;

    d = (dcb40_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (addr)
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);

    if (dv->dv_vcnt > 0 && (dv->dv_flags & DV_F_COMBINE_DCB) &&
        (d[-1].c_sg != 0) &&
        ((d[-1].addr_lo + d[-1].c_count) == paddr) &&
        (d[-1].c_count + count <= DCB_MAX_REQCOUNT)) {
        d[-1].c_count += count;
        return dv->dv_cnt - dv->dv_vcnt;
    }

    if (dv->dv_vcnt >= dv->dv_cnt) {
        return SOC_E_FULL;
    }
    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = 0;

    d->addr_lo = PTR_TO_INT(paddr);
    d->addr_hi = (0x10000000 | PTR_HI_TO_INT(paddr));
    d->c_count = count;
    d->c_sg = 1;

    d->c_purge = SOC_DMA_PURGE_GET(flags);

    if (SOC_DMA_HG_GET(flags)) {
        d->c_hg = 1;
    }

    dv->dv_vcnt += 1;

    return dv->dv_cnt - dv->dv_vcnt;
}

static int
dcb40_add_rx(dv_t *dv, sal_vaddr_t addr, uint32 count, uint32 flags)
{
    dcb40_t           *d;     /* DCB */
    uint32            *di;    /* DCB integer pointer */
    sal_paddr_t        paddr;

    if(!addr)
        return SOC_E_PARAM;

    d = (dcb40_t *)SOC_DCB_IDX2PTR(dv->dv_unit, dv->dv_dcb, dv->dv_vcnt);

    if (dv->dv_vcnt > 0) {      /* chain off previous dcb */
        d[-1].c_chain = 1;
    }

    di = (uint32 *)d;
    di[0] = di[1] = di[2] = di[3] = 0;

    if (addr) {
        paddr = soc_cm_l2p(dv->dv_unit, (void *)addr);
        d->addr_lo = PTR_TO_INT(paddr);
        d->addr_hi = (0x10000000 | PTR_HI_TO_INT(paddr));
    }

    d->c_count = count;
    d->c_sg = 1;

    dv->dv_vcnt += 1;
    return dv->dv_cnt - dv->dv_vcnt;
}

static uint32
dcb40_intrinfo(int unit, dcb_t *dcb, int tx, uint32 *count)
{
    dcb40_t      *d = (dcb40_t *)dcb;     /*  DCB */
    uint32      f;                      /* SOC_DCB_INFO_* flags */

    if (!d->done) {
        return 0;
    }
    f = SOC_DCB_INFO_DONE;
    if (tx) {
        if (!d->c_sg) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    } else {
        if (d->end) {
            f |= SOC_DCB_INFO_PKTEND;
        }
    }
    *count = d->count;
    return f;
}


/* the addr related functions are the same for all dcb types */
static void
dcb40_addr_set(int unit, dcb_t *dcb, sal_vaddr_t addr)
{
    dcb40_t      *d = (dcb40_t *)dcb;     /*  DCB */
    sal_paddr_t  paddr;

    if (addr == 0) {
        d->addr_lo = 0;
        d->addr_hi = 0;
    } else {
        paddr = soc_cm_l2p(unit, (void *)addr);
        d->addr_lo = PTR_TO_INT(paddr);
        d->addr_hi = (0x10000000 | PTR_HI_TO_INT(paddr));
    }
}

static sal_vaddr_t
dcb40_addr_get(int unit, dcb_t *dcb)
{
    dcb40_t      *d = (dcb40_t *)dcb;     /*  DCB */
    sal_paddr_t        paddr;

#if defined(COMPILER_OVERRIDE_NO_LONGLONG) || defined(__PEDANTIC__)
    paddr = d->addr_lo;
#else
    paddr = (((uint64)(d->addr_hi & ~(0x10000000)) << 32) | d->addr_lo);
#endif

    return (sal_vaddr_t)soc_cm_p2l(unit, paddr);
}

static sal_paddr_t
dcb40_paddr_get(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    return (sal_paddr_t)*d;
}

static void
dcb40_desc_intr_set(dcb_t * dcb, int val)
{
    dcb40_t * d = (dcb40_t *)dcb;

    d->c_intr = val;
    return;
}

static uint32
dcb40_desc_status_set(dcb_t * dcb, uint32 val)
{
    dcb40_t * d = (dcb40_t *)dcb;

    d->c_desc_status = val;
    return 0;
}

static uint32
dcb40_desc_remaining_set(dcb_t * dcb, uint32 val)
{
    dcb40_t * d = (dcb40_t *)dcb;

    d->c_desc = val;
    return 0;
}

static uint32
ep_to_cpu_hdr40_rx_untagged_get(void *hdr, int dt_mode, int ingport_is_hg)
{
    ep_to_cpu_hdr40_t *h = (ep_to_cpu_hdr40_t *)hdr;

    COMPILER_REFERENCE(dt_mode);

    return (ingport_is_hg ?
            ((h->itag_status) ? 0 : 2) :
            ((h->itag_status & 0x2) ?
             ((h->itag_status & 0x1) ? 0 : 2) :
             ((h->itag_status & 0x1) ? 1 : 3)));
}

static void
ep_to_cpu_hdr40_rx_reasons_get(dcb_op_t *dcb_op, void *hdr, soc_rx_reasons_t *reasons)
{
    soc_rx_reason_t *map;
    uint32 reason;
    uint32 mask;
    int i;

    SOC_RX_REASON_CLEAR_ALL(*reasons);

    map = dcb_op->rx_reason_maps[0];
    if (map == NULL) {
        return;
    }

    reason = dcb_op->rx_reason_get(hdr);
    mask = 1;
    for (i = 0; i < 32; i++) {
        if ((mask & reason)) {
            SOC_RX_REASON_SET(*reasons, map[i]);
        }
        mask <<= 1;
    }

    reason = dcb_op->rx_reason_hi_get(hdr);
    mask = 1;
    for (i = 0; i < 32; i++) {
        if ((mask & reason)) {
            SOC_RX_REASON_SET(*reasons, map[i + 32]);
        }
        mask <<= 1;
    }

    /* BPDU bit should be a reason, paste it in here */
    if (dcb_op->rx_bpdu_get(hdr)) {
        SOC_RX_REASON_SET(*reasons, socRxReasonBpdu);
    }

    return;
}

#ifdef BROADCOM_DEBUG
static void
dcb40_dump(int unit, dcb_t *dcb, char *prefix, int tx)
{
    uint32      *p;
    int         i, size;
    char        ps[((DCB_MAX_SIZE/sizeof(uint32))*9)+1];

    p = (uint32 *)dcb;
    size = SOC_DCB_SIZE(unit) / sizeof(uint32);
    for (i = 0; i < size; i++) {
        sal_sprintf(&ps[i*9], "%08x ", p[i]);
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%s\n"), prefix, ps));

    LOG_CLI((BSL_META_U(unit,
                        "%s\ttype %d %schain %ssg %sreload %shg %spurge \n"),
                         prefix,
                         SOC_DCB_TYPE(unit),
                         SOC_DCB_CHAIN_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_SG_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_RELOAD_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_HG_GET(unit, dcb) ? "" : "!",
                         SOC_DCB_PURGE_GET(unit, dcb) ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\taddr %p reqcount %d xfercount %d\n"),
             prefix,
             (void *)SOC_DCB_ADDR_GET(unit, dcb),
             SOC_DCB_REQCOUNT_GET(unit, dcb),
             SOC_DCB_XFERCOUNT_GET(unit, dcb)));

    if (!tx) {
        LOG_CLI((BSL_META_U(unit,
                            "%s\t%sdone %serror %sstart %send\n"),
                 prefix,
                 SOC_DCB_DONE_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_ERROR_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_START_GET(unit, dcb) ? "" : "!",
                 SOC_DCB_RX_END_GET(unit, dcb) ? "" : "!"));
    }

    if (tx || !SOC_DCB_RX_START_GET(unit, dcb)) {
        return;
    }
}
#endif /* BROADCOM_DEBUG */


SETFUNCFIELD(40, reqcount, c_count, uint32 count, count)
GETFUNCFIELD(40, reqcount, c_count)
GETFUNCFIELD(40, xfercount, count)
SETFUNCFIELD(40, done, done, int val, val ? 1 : 0)
GETFUNCFIELD(40, done, done)
SETFUNCFIELD(40, sg, c_sg, int val, val ? 1 : 0)
GETFUNCFIELD(40, sg, c_sg)
SETFUNCFIELD(40, chain, c_chain, int val, val ? 1 : 0)
GETFUNCFIELD(40, chain, c_chain)
SETFUNCFIELD(40, reload, c_reload, int val, val ? 1 : 0)
GETFUNCFIELD(40, reload, c_reload)
GETFUNCFIELD(40, desc_intr, c_intr)
SETFUNCERR(40, tx_l2pbm, pbmp_t)
SETFUNCERR(40, tx_utpbm, pbmp_t)
SETFUNCERR(40, tx_l3pbm, pbmp_t)
SETFUNCERR(40, tx_crc, int)
SETFUNCERR(40, tx_cos, int)
SETFUNCERR(40, tx_destmod, uint32)
SETFUNCERR(40, tx_destport, uint32)
SETFUNCERR(40, tx_opcode, uint32)
SETFUNCERR(40, tx_srcmod, uint32)
SETFUNCERR(40, tx_srcport, uint32)
SETFUNCERR(40, tx_prio, uint32)
SETFUNCERR(40, tx_pfm, uint32)
GETHDRFUNCFIELD(40, rx_cos, word6.overlay2.cpu_cos)
GETHDRHG2FUNCFIELD(40, rx_destmod, dst_mod)
GETHDRHG2FUNCFIELD(40, rx_destport, dst_port)
GETHDRHG2FUNCFIELD(40, rx_opcode, opcode)
GETHDRHG2FUNCEXPR(40, rx_classtag, (h->ppd_overlay1.ppd_type != 1 ? 0 :
                                 (h->ppd_overlay2.ctag_hi << 8) |
                                 (h->ppd_overlay2.ctag_lo)))
GETHDRFUNCFIELD(40, rx_matchrule, match_rule)
GETFUNCFIELD(40, rx_start, start)
GETFUNCFIELD(40, rx_end, end)
GETFUNCFIELD(40, rx_error, error)
GETHDRHG2FUNCFIELD(40, rx_prio, vlan_pri) /* outer_pri */
GETHDRFUNCFIELD(40, rx_reason, reason)
GETHDRFUNCFIELD(40, rx_reason_hi, reason_hi)
GETHDRFUNCFIELD(40, rx_ingport, srcport)
GETHDRHG2FUNCFIELD(40, rx_srcport, src_port)
GETHDRHG2FUNCFIELD(40, rx_srcmod, src_mod)
GETHDRHG2FUNCEXPR(40, rx_mcast, ((h->ppd_overlay1.dst_mod << 8) |
                           (h->ppd_overlay1.dst_port)))
GETHDRHG2FUNCEXPR(40, rx_vclabel, ((h->ppd_overlay1.vc_label_19_16 << 16) |
                           (h->ppd_overlay1.vc_label_15_8 << 8) |
                           (h->ppd_overlay1.vc_label_7_0)))
GETHDRFUNCEXPR(40, rx_mirror, ((h->imirror) | (h->emirror)))
GETHDRFUNCFIELD(40, rx_timestamp, word2.overlay1.timestamp)
GETHDRFUNCFIELD(40, rx_timestamp_upper, timestamp_hi)
SETFUNCFIELD(40, hg, c_hg, uint32 hg, hg)
GETFUNCFIELD(40, hg, c_hg)


SETFUNCFIELD(40, purge, c_purge, uint32 purge, purge)
GETFUNCFIELD(40, purge, c_purge)
GETHDRPTREXPR(40, mhp, &(h->mh0))
GETHDRFUNCFIELD(40, outer_vid, word6.overlay1.outer_vid)
GETHDRFUNCFIELD(40, outer_pri, outer_pri)
GETHDRFUNCFIELD(40, outer_cfi, outer_cfi)
GETHDRFUNCFIELD(40, rx_outer_tag_action, otag_action)
GETHDRFUNCFIELD(40, inner_vid, inner_vid)
GETHDRFUNCFIELD(40, inner_pri, inner_pri)
GETHDRFUNCFIELD(40, inner_cfi, inner_cfi)
GETHDRFUNCFIELD(40, rx_inner_tag_action, itag_action)
GETHDRFUNCFIELD(40, rx_bpdu, bpdu)
GETHDRFUNCEXPR(40, rx_l3_intf, ((h->replicated) ? (h->repl_nhi) :
        (((h->repl_nhi) & 0x7ff) + _SHR_L3_EGRESS_IDX_MIN)))
GETHDRFUNCNULL(40, rx_decap_tunnel)
GETHDRFUNCNULL(40, rx_switch_drop)
GETHDRFUNCNULL(40, olp_encap_oam_pkt)
GETFUNCFIELD(40, read_ecc_error, ecc_error)
GETFUNCFIELD(40, desc_remaining, c_desc)
GETFUNCFIELD(40, desc_status, c_desc_status)
GETHDRFUNCNULL(40, match_id_lo)
GETHDRFUNCNULL(40, match_id_hi)
GETHDRFUNCNULL(40, forwarding_type)
GETHDRFUNCNULL(40, forwarding_zone_id)

#ifdef BROADCOM_DEBUG
GETFUNCERR(40, tx_l2pbm) 
GETFUNCERR(40, tx_utpbm) 
GETFUNCERR(40, tx_l3pbm)
GETFUNCERR(40, tx_crc) 
GETFUNCERR(40, tx_cos)
GETFUNCERR(40, tx_destmod)
GETFUNCERR(40, tx_destport)
GETFUNCERR(40, tx_opcode) 
GETFUNCERR(40, tx_srcmod)
GETFUNCERR(40, tx_srcport)
GETFUNCERR(40, tx_prio)
GETFUNCERR(40, tx_pfm)
#endif /* BROADCOM_DEBUG */

#ifdef  PLISIM
static void dcb40_status_init(dcb_t *dcb)
{
    uint32      *d = (uint32 *)dcb;

    d[2] = d[3] = 0;
}
SETFUNCFIELD(40, xfercount, count, uint32 count, count)
SETFUNCFIELD(40, rx_start, start, int val, val ? 1 : 0)
SETFUNCFIELD(40, rx_end, end, int val, val ? 1 : 0)
SETFUNCFIELD(40, rx_error, error, int val, val ? 1 : 0)
SETHDRFUNCEXPRIGNORE(40, rx_crc, int val, ignore)
SETHDRFUNCFIELD(40, rx_ingport, srcport, int val, val)
#endif /* PLISIM */

dcb_op_t dcb40_op = {
    40,
    sizeof(dcb40_t),
    dcb37_rx_reason_maps,
    dcb0_rx_reason_map_get,
    ep_to_cpu_hdr40_rx_reasons_get,
    dcb40_init,
    dcb40_add_tx,
    dcb40_add_rx,
    dcb40_intrinfo,
    dcb40_reqcount_set,
    dcb40_reqcount_get,
    dcb40_xfercount_get,
    dcb40_addr_set,
    dcb40_addr_get,
    dcb40_paddr_get,
    dcb40_done_set,
    dcb40_done_get,
    dcb40_sg_set,
    dcb40_sg_get,
    dcb40_chain_set,
    dcb40_chain_get,
    dcb40_reload_set,
    dcb40_reload_get,
    dcb40_desc_intr_set,
    dcb40_desc_intr_get,
    dcb40_tx_l2pbm_set,
    dcb40_tx_utpbm_set,
    dcb40_tx_l3pbm_set,
    dcb40_tx_crc_set,
    dcb40_tx_cos_set,
    dcb40_tx_destmod_set,
    dcb40_tx_destport_set,
    dcb40_tx_opcode_set,
    dcb40_tx_srcmod_set,
    dcb40_tx_srcport_set,
    dcb40_tx_prio_set,
    dcb40_tx_pfm_set,
    ep_to_cpu_hdr40_rx_untagged_get,
    dcb19_rx_crc_get,
    ep_to_cpu_hdr40_rx_cos_get,
    ep_to_cpu_hdr40_rx_destmod_get,
    ep_to_cpu_hdr40_rx_destport_get,
    ep_to_cpu_hdr40_rx_opcode_get,
    ep_to_cpu_hdr40_rx_classtag_get,
    ep_to_cpu_hdr40_rx_matchrule_get,
    dcb40_rx_start_get,
    dcb40_rx_end_get,
    dcb40_rx_error_get,
    ep_to_cpu_hdr40_rx_prio_get,
    ep_to_cpu_hdr40_rx_reason_get,
    ep_to_cpu_hdr40_rx_reason_hi_get,
    ep_to_cpu_hdr40_rx_ingport_get,
    ep_to_cpu_hdr40_rx_srcport_get,
    ep_to_cpu_hdr40_rx_srcmod_get,
    ep_to_cpu_hdr40_rx_mcast_get,
    ep_to_cpu_hdr40_rx_vclabel_get,
    ep_to_cpu_hdr40_rx_mirror_get,
    ep_to_cpu_hdr40_rx_timestamp_get,
    ep_to_cpu_hdr40_rx_timestamp_upper_get,
    dcb40_hg_set,
    dcb40_hg_get,
    NULL,
    NULL,
    dcb40_purge_set,
    dcb40_purge_get,
    ep_to_cpu_hdr40_mhp_get,
    ep_to_cpu_hdr40_outer_vid_get,
    ep_to_cpu_hdr40_outer_pri_get,
    ep_to_cpu_hdr40_outer_cfi_get,
    ep_to_cpu_hdr40_rx_outer_tag_action_get,
    ep_to_cpu_hdr40_inner_vid_get,
    ep_to_cpu_hdr40_inner_pri_get,
    ep_to_cpu_hdr40_inner_cfi_get,
    ep_to_cpu_hdr40_rx_inner_tag_action_get,
    ep_to_cpu_hdr40_rx_bpdu_get,
    ep_to_cpu_hdr40_rx_l3_intf_get,
    ep_to_cpu_hdr40_rx_decap_tunnel_get,
    ep_to_cpu_hdr40_rx_switch_drop_get,
    ep_to_cpu_hdr40_olp_encap_oam_pkt_get,
    dcb40_read_ecc_error_get,
    dcb40_desc_remaining_get,
    dcb40_desc_remaining_set,
    dcb40_desc_status_get,
    dcb40_desc_status_set,
    ep_to_cpu_hdr40_match_id_lo_get,
    ep_to_cpu_hdr40_match_id_hi_get,
    ep_to_cpu_hdr40_forwarding_type_get,
    ep_to_cpu_hdr40_forwarding_zone_id_get,
#ifdef  BROADCOM_DEBUG
    dcb40_tx_l2pbm_get,
    dcb40_tx_utpbm_get,
    dcb40_tx_l3pbm_get,
    dcb40_tx_crc_get,
    dcb40_tx_cos_get,
    dcb40_tx_destmod_get,
    dcb40_tx_destport_get,
    dcb40_tx_opcode_get,
    dcb40_tx_srcmod_get,
    dcb40_tx_srcport_get,
    dcb40_tx_prio_get,
    dcb40_tx_pfm_get,

    dcb40_dump,
    dcb0_reason_dump,
#endif /* BROADCOM_DEBUG */
#ifdef  PLISIM
    dcb40_status_init,
    dcb40_xfercount_set,
    dcb40_rx_start_set,
    dcb40_rx_end_set,
    dcb40_rx_error_set,
    ep_to_cpu_hdr40_rx_crc_set,
    ep_to_cpu_hdr40_rx_ingport_set,
#endif /* PLISIM */
};
#endif /* BCM_FIRELIGHT_SUPPORT */

/*
 * Function:
 *      soc_dcb_unit_init
 * Purpose:
 *      Select the appropriate dcb operations set and load it into
 *      SOC_CONTROL
 * Parameters:
 *      unit - device
 */
void
soc_dcb_unit_init(int unit)
{
    soc_control_t       *soc;

    COMPILER_REFERENCE(dcb0_funcerr);
    soc = SOC_CONTROL(unit);

#ifdef  BCM_HERCULES_SUPPORT
    if (soc_feature(unit, soc_feature_dcb_type4)) {
        soc->dcb_op = &dcb4_op;
        return;
    }
#endif  /* BCM_HERCULES_SUPPORT */

#ifdef  BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_FIREBOLT_SUPPORT
    if (soc_feature(unit, soc_feature_dcb_type9)) {
        soc->dcb_op = &dcb9_op;
        return;
    }
#endif
#ifdef BCM_BRADLEY_SUPPORT
    if (soc_feature(unit, soc_feature_dcb_type11)) {
        soc->dcb_op = &dcb11_op;
        return;
    }
#endif
#if defined(BCM_RAPTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type12)) {
        soc->dcb_op = &dcb12_op;
        return;
    }
#endif
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type13)) {
        soc->dcb_op = &dcb13_op;
        return;
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type14)) {
        soc->dcb_op = &dcb14_op;
        return;
    }
#endif
#if defined(BCM_RAPTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type15)) {
        soc->dcb_op = &dcb15_op;
        return;
    }
#endif
#if defined(BCM_SCORPION_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type16)) {
        soc->dcb_op = &dcb16_op;
        return;
    }
#endif
#if defined(BCM_HAWKEYE_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type17)) {
        soc->dcb_op = &dcb17_op;
        return;
    }
#endif
#if defined(BCM_RAPTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type18)) {
        soc->dcb_op = &dcb18_op;
        return;
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type19)) {
        soc->dcb_op = &dcb19_op;
        return;
    }
#endif
#ifdef BCM_ENDURO_SUPPORT
    if (soc_feature(unit, soc_feature_dcb_type20)) {
        soc->dcb_op = &dcb20_op;
        return;
    }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type21)) {
        soc->dcb_op = &dcb21_op;
        return;
    }
#endif
#ifdef BCM_SHADOW_SUPPORT
    if (soc_feature(unit, soc_feature_dcb_type22)) {
        soc->dcb_op = &dcb22_op;
        return;
    }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type23)) {
        soc->dcb_op = &dcb23_op;
        return;
    }
#endif
#if defined(BCM_KATANA_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type24)) {
        soc->dcb_op = &dcb24_op;
        return;
    }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type26)) {
        soc->dcb_op = &dcb26_op;
        return;
    }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type32)) {
        soc->dcb_op = &dcb32_op;
        return;
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type33)) {
        soc->dcb_op = &dcb33_op;
        return;
    }
#endif
#if defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type35)) {
        soc->dcb_op = &dcb35_op;
        return;
    }
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type36)) {
#if defined(BCM_HELIX5_SUPPORT)
        if (soc_feature(unit, soc_feature_hx5_oam_support)) {
        /* For Helix5, OAM_SLOWPATH got added as reason code at offset 52
         * of CPU_OPCODES format in regsfile. It is RESERVED_52_52
         * for Trident3*/
            dcb36_op.rx_reason_maps[0][52] = socRxReasonOAMSlowpath;
        }
#endif
        soc->dcb_op = &dcb36_op;
        return;
    }
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type38) &&
        soc_feature(unit, soc_feature_cmicx)) {
            soc->dcb_op = &dcb38_op;
            return;
    }
#endif
#endif  /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef  BCM_PETRA_SUPPORT
    if (soc_feature(unit, soc_feature_dcb_type28)) {
        soc->dcb_op = &dcb28_op;
        return;
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (soc_feature(unit, soc_feature_dcb_type39) &&
        soc_feature(unit, soc_feature_cmicx))
    {
            soc->dcb_op = &dcb39_op;
            return;
    }
#endif

#if defined(BCM_KATANA2_SUPPORT) 
	if (soc_feature(unit, soc_feature_dcb_type29)) {
		soc->dcb_op = &dcb29_op;
		return;
	}
#endif /* BCM_KATANA2_SUPPORT */

#if defined(BCM_HURRICANE2_SUPPORT) 
	if (soc_feature(unit, soc_feature_dcb_type30)) {
	    soc->dcb_op = &dcb30_op;
	    return;
	}
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type31)) {
         soc->dcb_op = &dcb31_op;
         return;
    }
#endif /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type34)) {
         soc->dcb_op = &dcb34_op;
         return;
    }
#endif /* BCM_HURRICANE3_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_dcb_type37)) {
         soc->dcb_op = &dcb37_op;
         return;
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_FIRELIGHT_SUPPORT)
        if (soc_feature(unit, soc_feature_dcb_type40)) {
             soc->dcb_op = &dcb40_op;
             return;
        }
#endif /* BCM_FIRELIGHT_SUPPORT */

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d has unknown dcb type\n"), unit));
    assert(0);
}

#endif /* defined(BCM_ESW_SUPPORT) */

