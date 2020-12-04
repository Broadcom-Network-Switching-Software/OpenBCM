/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utility routines for managing loopback headers
 * These are mainly used for debug purposes
 */
#include <shared/bsl.h>
#include <assert.h>
#include <sal/core/libc.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <soc/pbsmh.h>
#include <soc/cm.h>
#include <soc/dcb.h>
#include <soc/loopback.h>

#ifdef BCM_XGS3_SWITCH_SUPPORT
static char *soc_loopback_hdr_field_names[] = {
        /* NOTE: strings must match soc_loopback_hdr_field_t */
        "start",
        "input_pri",
        "visibility",
        "pkt_profile",
        "src",
        "src_type",
        "common_hdr",
        "cng",
        "tc",
        "zero",
        NULL
};

soc_loopback_hdr_field_t
soc_loopback_hdr_name_to_field(int unit, char *name)
{
    soc_loopback_hdr_field_t           f;

    assert(COUNTOF(soc_loopback_hdr_field_names) - 1 == LBMH_COUNT);

    COMPILER_REFERENCE(unit);

    for (f = 0; soc_loopback_hdr_field_names[f] != NULL; f++) {
        if (sal_strcmp(name, soc_loopback_hdr_field_names[f]) == 0) {
            return f;
        }
    }

    return LBMH_invalid;
}

char *
soc_loopback_hdr_field_to_name(int unit, soc_loopback_hdr_field_t field)
{
    COMPILER_REFERENCE(unit);

    assert(COUNTOF(soc_loopback_hdr_field_names) == LBMH_COUNT);

    if (field < 0 || field >= LBMH_COUNT) {
        return "??";
    } else {
        return soc_loopback_hdr_field_names[field];
    }
}


void
soc_loopback_hdr_field_set(int unit, soc_loopback_hdr_t *lh,
                                  soc_loopback_hdr_field_t field, uint32 val)
{
    switch(field) {
        case LBMH_start:      lh->start      =  val; break;
        case LBMH_input_pri:  lh->input_pri  =  val; break;
        case LBMH_common_hdr: 
             lh->common_hdr0 = (val >> 1) & 0xf;
             lh->common_hdr1 = val & 0x1;
             break;
        case LBMH_src_type:   lh->source_type=  val; break;
        case LBMH_src:        
             lh->source0        = ((val >> 10  ) & 0x3f);
             lh->source1        = ((val >> 2   ) & 0xff);
             lh->source2        =  (val & 0x3);
             break;
        case LBMH_visibility: lh->visibility =  val; break;
        case LBMH_pkt_profile:lh->pkt_profile=  val; break;
        case LBMH_pp_port:    lh->pp_port    =  val; break;                    
        default:
            break;
    }
    return;

}

uint32
soc_loopback_hdr_field_get(int unit,soc_loopback_hdr_t * lh,soc_loopback_hdr_field_t field)

{
    switch(field) {
        case LBMH_start:      return lh->start;
        case LBMH_input_pri:  return lh->input_pri;
        case LBMH_visibility: return lh->visibility;
        case LBMH_pkt_profile:return lh->pkt_profile;
        case LBMH_pp_port:    return lh->pp_port;
        case LBMH_src:
            return (((lh->source2) |
                    (lh->source1 << 2) | 
                    (lh->source0 <<10))) ;
        case LBMH_src_type:   return lh->source_type;
        case LBMH_common_hdr: 
            return ((lh->common_hdr0 << 1) | (lh->common_hdr1 & 0x1));
                    
        default:
                return 0;
    }
}


void 
soc_loopback_hdr_dump(int unit, char *pfx, soc_loopback_hdr_t *lh)
{
    LOG_CLI((BSL_META_U(unit,
             "%s "
             "<START=0x%x INPUT_PRI=0x%x HEADER_TYPES=%d>\n"
             "%s "
             "<SOURCE_TYPE=%d SOURCE=%d VISIBILITY_PKT=%d>\n"
             "%s "
             "<PKT_PROFILE=%d PACKET_PROCESSING_PORT = 0x%x>\n"),
             pfx,
             soc_loopback_hdr_field_get(unit, lh, LBMH_start),
             soc_loopback_hdr_field_get(unit, lh, LBMH_input_pri),
             soc_loopback_hdr_field_get(unit, lh, LBMH_common_hdr),
             pfx,
             soc_loopback_hdr_field_get(unit, lh, LBMH_src_type),
             soc_loopback_hdr_field_get(unit, lh, LBMH_src),
             soc_loopback_hdr_field_get(unit, lh, LBMH_visibility),
             pfx,
             soc_loopback_hdr_field_get(unit, lh, LBMH_pkt_profile),
             soc_loopback_hdr_field_get(unit, lh, LBMH_pp_port)));

    return;
}


/*
 * Function:
 *     soc_loopback_lbport_num_get
 * Description:
 *      return FIRST loopback port of the chip
 * Parameters:
 *      unit         device number
 * Return:
 *      first loopbackport on the given device
 */
int 
soc_loopback_lbport_num_get(int unit, uint32 pipe) 
{
    int port, lb_index=0,loopback_port_num=-1;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8  rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
#endif

    PBMP_LB_ITER(unit, port) {
        loopback_port_num = port;
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* For 56983 SKU, pipes are not contiguous so when lb port 139 is
           reached while iteration, we change the lb_index to 6 since it
           belongs to pipe 6
        */
        if ((port == 139) && (dev_id == BCM56983_DEVICE_ID)) {
            lb_index = 6;
        }
#endif
        if (pipe == lb_index) {
            break;
        }
        lb_index++;
    }
    return loopback_port_num;

}
#endif /* BCM_XGS3_SWITCH_SUPPORT*/

