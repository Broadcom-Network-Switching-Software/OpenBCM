/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        proc_hdr.h
 */

#include <shared/bsl.h>

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/types.h>
#include <soc/proc_hdr.h>
#include <soc/enet.h>
#include <soc/cm.h>
#include <soc/debug.h>

#if defined(BCM_XGS_SUPPORT) && defined(BCM_CPU_TX_PROC_SUPPORT)

static char *soc_proc_hdr_field_names[] = {
    SOC_PROC_HDR_FIELD_NAMES_INIT
};

/*
 * Function Name:
 *         soc_proc_hdr_name_to_field
 * Purpose:
 *         To convert field name to the field
 * Parameters:
 *         [IN] unit - unit number
 *         [IN] name - ascii name of the field
 * Return:
 *         Proc HDR Field or invalid code
 */
soc_proc_hdr_field_t
soc_proc_hdr_name_to_field(int unit, char *name)
{
    soc_proc_hdr_field_t		f;

    assert(COUNTOF(soc_proc_hdr_field_names) - 1 == PH_COUNT);

    COMPILER_REFERENCE(unit);

    for (f = 0; soc_proc_hdr_field_names[f] != NULL; f++) {
	if (sal_strcmp(name, soc_proc_hdr_field_names[f]) == 0) {
	    return f;
	}
    }

    return PH_invalid;
}

/*
 * Function Name:
 *         soc_proc_hdr_field_to_name
 * Purpose:
 *         To convert field to field name
 * Parameters:
 *         [IN] unit - unit number
 *         [IN] name - field in the proc hdr
 * Return:
 *         Field character name or ?? on ERROR
 */
char *
soc_proc_hdr_field_to_name(int unit, soc_proc_hdr_field_t field)
{
    COMPILER_REFERENCE(unit);

    assert(COUNTOF(soc_proc_hdr_field_names) == PH_COUNT);

    if (field < 0 || field >= PH_COUNT) {
	return "??";
    } else {
	return soc_proc_hdr_field_names[field];
    }
}

/*
 * Function Name:
 *         soc_proc_hdr_field_set
 * Purpose:
 *         To set the value of a field in the proc header
 * Parameters:
 *         [IN] unit  - unit number
 *         [INOUT] ph    - proc header to be modified
 *         [IN] field - field to be modified
 *         [IN] val   - value of the field
 * Return:
 *         None
 */
void
soc_proc_hdr_field_set(int unit, soc_proc_hdr_t *ph,
                       soc_proc_hdr_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
        case PH_start:
            ph->overlay1.start = val;
            break;
        case PH_input_pri:
            ph->overlay1.input_pri = val;
            break;
        case PH_header_types:
            ph->overlay1.header_types_4_1 = ((val >> 1) & 0xf);
            ph->overlay1.header_types_0 = (val & 0x1);
            break;
        case PH_source_type:
            ph->overlay1.source_type = val;
            break;
        case PH_source:
            ph->overlay1.source_15_10 = ((val >> 10) & 0x3f);
            ph->overlay1.source_9_2 = ((val >> 2) & 0xff);
            ph->overlay1.source_1_0 = (val & 0x3);
            break;
        case PH_visibility_pkt:
            ph->overlay1.visibility_pkt = val;
            break;
        case PH_pkt_profile:
            ph->overlay1.pkt_profile = val;
            break;
        case PH_mcast_lb_idx_valid:
            ph->overlay1.mcast_lb_idx_valid = val;
            break;
        case PH_mcast_lb_idx:
            ph->overlay1.mcast_lb_idx_7_2 = ((val >> 2) & 0x3f);
            ph->overlay1.mcast_lb_idx_1_0 = (val & 0x3);
            break;
        case PH_zero:
            ph->overlay1.zero = val;
            break;
        case PH_ecmp_member_id:
            ph->overlay1.ecmp_member_id_11_4 = ((val >> 4) & 0xff);
            ph->overlay1.ecmp_member_id_3_0 = (val & 0xf);
            break;
        case PH_dp:
            ph->overlay1.dp = val;
            break;
        case PH_int_pri:
            ph->overlay1.int_pri_3_2 = ((val >> 2) & 0x3);
            ph->overlay1.int_pri_1_0 = (val & 0x3);
            break;
        case PH_int_cn:
            ph->overlay1.int_cn = val;
            break;
        case PH_qos_fields_valid:
            ph->overlay1.qos_fields_valid = val;
            break;
        case PH_routed_pkt:
            ph->overlay1.routed_pkt = val;
            break;
        case PH_destination:
            ph->overlay1.destination_15_10 = ((val >> 10) & 0x3f);
            ph->overlay1.destination_9_2 = ((val >> 2) & 0xff);
            ph->overlay1.destination_1_0 = (val & 0x3);
            break;
        case PH_destination_type:
            ph->overlay1.destination_type = val;
            break;
        case PH_subflow_type:
            ph->overlay1.subflow_type = val;
            break;
        case PH_pp_port:
            ph->overlay1.pp_port = val;
            break;
        default:
	    LOG_WARN(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                    "proc_hdr_set: unit %d: Unknown proc hdr field=%d val=0x%x\n"),
                    unit, field, val));
	    break;
    }
}

/*
 * Function Name:
 *         soc_proc_hdr_field_get
 * Purpose:
 *         To get the value of a field in the proc header
 * Parameters:
 *         [IN] unit  - unit number
 *         [IN] ph    - proc header to be modified
 *         [IN] field - field to be modified
 * Return:
 *         value of the field, if field is not found then Log Warning
 */
uint32
soc_proc_hdr_field_get(int unit, soc_proc_hdr_t *ph,
                       soc_proc_hdr_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
        case PH_start:
            return ph->overlay1.start;
        case PH_input_pri:
            return ph->overlay1.input_pri;
        case PH_header_types:
            return ((ph->overlay1.header_types_4_1 << 1) | ph->overlay1.header_types_0);
        case PH_source_type:
            return ph->overlay1.source_type;
        case PH_source:
            return ((ph->overlay1.source_15_10 << 10) |
                    (ph->overlay1.source_9_2 << 2) |
                    (ph->overlay1.source_1_0));
        case PH_visibility_pkt:
            return ph->overlay1.visibility_pkt;
        case PH_pkt_profile:
            return ph->overlay1.pkt_profile;
        case PH_mcast_lb_idx_valid:
            return ph->overlay1.mcast_lb_idx_valid;
        case PH_mcast_lb_idx:
            return ((ph->overlay1.mcast_lb_idx_7_2 << 2) |
                    (ph->overlay1.mcast_lb_idx_1_0));
        case PH_zero:
            return ph->overlay1.zero;
        case PH_ecmp_member_id:
            return ((ph->overlay1.ecmp_member_id_11_4 << 4) |
                    (ph->overlay1.ecmp_member_id_3_0));
        case PH_dp:
            return ph->overlay1.dp;
        case PH_int_pri:
            return ((ph->overlay1.int_pri_3_2 << 2) |
                    (ph->overlay1.int_pri_1_0));
        case PH_int_cn:
            return ph->overlay1.int_cn;
        case PH_qos_fields_valid:
            return ph->overlay1.qos_fields_valid;
        case PH_routed_pkt:
            return ph->overlay1.routed_pkt;
        case PH_destination:
            return ((ph->overlay1.destination_15_10 << 10) |
                   (ph->overlay1.destination_9_2 << 2) |
                   (ph->overlay1.destination_1_0));
        case PH_destination_type:
            return ph->overlay1.destination_type;
        case PH_subflow_type:
            return ph->overlay1.subflow_type;
        case PH_pp_port:
            return ph->overlay1.pp_port;
        default:
	    LOG_WARN(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                    "proc_hdr_get: unit %d: Unknown proc hdr field=%d\n"),
                    unit, field));
	    return 0;
    }
}

/*
 * Function Name:
 *         soc_proc_hdr_field_dump
 * Purpose:
 *         to output the entire proc header in a user friendly format
 * Parameters:
 *         [IN] unit - unit number
 *         [IN] pfx  - character prefix to be appended to the header output.
 *         [IN] ph   - proc header to be modified
 * Return:
 *         None
 */
void
soc_proc_hdr_dump(int unit, char *pfx, soc_proc_hdr_t *ph)
{
    LOG_CLI((BSL_META_U(unit,
                        "%s0x%02x%02x%02x%02x%02x "
             "<START=0x%x INPUT_PRI=0x%x HEADER_TYPES=%d SOURCE_TYPE=%d SOURCE=%d VISIBILITY_PKT=%d PKT_PROFILE=%d\n"),
             pfx,
             ph->overlay0.bytes[0],
             ph->overlay0.bytes[1],
             ph->overlay0.bytes[2],
             ph->overlay0.bytes[3],
             ph->overlay0.bytes[4],
             soc_proc_hdr_field_get(unit, ph, PH_start),
             soc_proc_hdr_field_get(unit, ph, PH_input_pri),
             soc_proc_hdr_field_get(unit, ph, PH_header_types),
             soc_proc_hdr_field_get(unit, ph, PH_source_type),
             soc_proc_hdr_field_get(unit, ph, PH_source),
             soc_proc_hdr_field_get(unit, ph, PH_visibility_pkt),
             soc_proc_hdr_field_get(unit, ph, PH_pkt_profile)));

    LOG_CLI((BSL_META_U(unit,
                        "%s0x%02x%02x%02x "
             "<MCAST_LB_IDX_VALID=%d MCAST_LB_IDX=%d ZERO=%d>\n"),
             pfx,
             ph->overlay0.bytes[5],
             ph->overlay0.bytes[6],
             ph->overlay0.bytes[7],
             soc_proc_hdr_field_get(unit, ph, PH_mcast_lb_idx_valid),
             soc_proc_hdr_field_get(unit, ph, PH_mcast_lb_idx),
             soc_proc_hdr_field_get(unit, ph, PH_zero)));

    LOG_CLI((BSL_META_U(unit,
                        "%s0x%02x%02x%02x%02x "
             "<ECMP_MEMBER_ID=%d DP=%d INT_PRI=%d INT_CN=%d QOS_FIELD_VALID=%d ROUTED_PKT=%d>\n"),
             pfx,
             ph->overlay0.bytes[8],
             ph->overlay0.bytes[9],
             ph->overlay0.bytes[10],
             ph->overlay0.bytes[11],
             soc_proc_hdr_field_get(unit, ph, PH_ecmp_member_id),
             soc_proc_hdr_field_get(unit, ph, PH_dp),
             soc_proc_hdr_field_get(unit, ph, PH_int_pri),
             soc_proc_hdr_field_get(unit, ph, PH_int_cn),
             soc_proc_hdr_field_get(unit, ph, PH_qos_fields_valid),
             soc_proc_hdr_field_get(unit, ph, PH_routed_pkt)));

    LOG_CLI((BSL_META_U(unit,
                        "%s0x%02x%02x%02x%02x "
             "<DESTINATION=%d DESTINATION_TYPE=%d SUBFLOW_TYPE=%d PP_PORT=%d>\n"),
             pfx,
             ph->overlay0.bytes[12],
             ph->overlay0.bytes[13],
             ph->overlay0.bytes[14],
             ph->overlay0.bytes[15],
             soc_proc_hdr_field_get(unit, ph, PH_destination),
             soc_proc_hdr_field_get(unit, ph, PH_destination_type),
             soc_proc_hdr_field_get(unit, ph, PH_subflow_type),
             soc_proc_hdr_field_get(unit, ph, PH_pp_port)));
}

/*
 * Function:
 *     soc_lbport_num_get
 * Description:
 *      return FIRST loopback port of the chip
 * Parameters:
 *      unit         device number
 * Return:
 *      first loopback port on the given device
 */
int
soc_lbport_num_get(int unit, uint32 pipe)
{
    int port, lb_index = 0, loopback_port_num = -1;
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

/*
 * Function:
 *     soc_pipe_get
 * Description:
 *      return the pipe of the chip
 * Parameters:
 *      unit             device number
 *      logical_src_port source port
 * Return:
 *      pipe number based on logical src port
 */
int
soc_pipe_get(int unit, uint32 logical_src_port)
{
    soc_info_t  *si;
    si = &SOC_INFO(unit);

    return si->port_pipe[logical_src_port];
}

#endif /* BCM_XGS_SUPPORT && BCM_CPU_TX_PROC_SUPPORT */
