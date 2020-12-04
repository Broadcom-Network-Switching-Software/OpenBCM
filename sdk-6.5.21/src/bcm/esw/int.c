/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains INT functions.
 * INT(Inband Network Telemetry) provides a framework to help detecting 
 * and isolating network faults. This is achieved by sending a INT probe packet
 * from a source node to the destination node. Along the path, every INT-awared 
 * switch device appends the requested data to the probe packet. 
 * The requested data includes switch device's real-time states and should be 
 * processed in the data plane by hardware.
 * This module supports INT probe packet detection and processing configuration 
 * in the INT-awared switch device. This basically is done with various  
 * switch control types and per-port controls.
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/scache.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <bcm_int/esw/switch.h>

#if defined(INCLUDE_L3)
#include <bcm_int/esw/trx.h>
#endif

#define LB_PORT_LPORT_IDX   0
#define INT_MAX_LEN_OFFSET_VALUE    32

#if defined(BCM_TRIDENT3_SUPPORT)
 /*
  * Function:
  *      _bcm_td3_int_loopback_enable
  * Purpose:
  *      Enable IPv4 L3 attribute of the loopback port for INT reply packet
  *   When sending packet to the internal loopback port, the ingress
  *   pp_port(packet process port) will be one of the front panel port. 
  *   Need to enable L3 on the pp_port. In TD3, it references the 
  *   SOURCE_TRUNK_MAP for the port attributes. By default, it uses LPORT 
  *   profile 0.
  * Parameters:
  *   IN : unit
  * Returns:
  *      BCM_E_XXX
  */

STATIC int
_bcm_td3_int_misc_enable(int unit)
{
    soc_field_t reg_fld = HIGIG2_ECN_IN_CC_ENABLEf;
    uint32 fld_value    = 1;

    /* enable IPv4 L3 on loopback port  */
    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, LPORT_TABm, 0,
                                V4L3_ENABLEf, 1));

    /* populate INT_CN in the egress object bus. Editor uses egress object
     * bus to fetch INT_CN from Egress object bus and put into INT metadata.
     */
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, EGR_CONFIG_1_64r, 
               REG_PORT_ANY, 1, &reg_fld, &fld_value));
  
    return SOC_E_NONE;
}

 /*
  * Function:
  *      _bcm_td3_int_loopback_disable
  * Purpose:
  *      Disable L3 on pp_port 
  * Parameters:
  *   IN : unit
  * Returns:
  *      BCM_E_XXX
  */

STATIC int
_bcm_td3_int_misc_disable(int unit)
{
    soc_field_t reg_fld = HIGIG2_ECN_IN_CC_ENABLEf;
    uint32 fld_value    = 0;

    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, LPORT_TABm, 0,
                                V4L3_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, EGR_CONFIG_1_64r, 
               REG_PORT_ANY, 1, &reg_fld, &fld_value));
    return SOC_E_NONE;
}
#endif

int
_bcm_th3_int_reg64_set(int unit, soc_reg_t reg, soc_field_t field, uint64 arg)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint64 reg_value;
    uint64 max_value;
    int field_width;
    int rv;

    if (SOC_REG_FIELD_VALID(unit, reg, field)) {
        /* Check the range of the value to be programmed */
        field_width = soc_reg_field_length(unit, reg, field);
        if (field_width < 64) {
            COMPILER_64_SET(max_value, 0, 1);
            COMPILER_64_SHL(max_value, field_width);
            COMPILER_64_SUB_32(max_value, 1);
        } else {
            COMPILER_64_SET(max_value, -1, -1);
        }
        if (COMPILER_64_GT(arg, max_value)) {
            return BCM_E_PARAM;
        }

        rv = soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        soc_reg64_field_set(unit, reg, &reg_value, field, arg);
        rv = soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_value);
        SOC_IF_ERROR_RETURN(rv);
    }
    return SOC_E_NONE;
#else
    return SOC_E_NONE;
#endif
}

int
_bcm_th3_int_reg32_set(int unit, soc_reg_t reg, soc_field_t field, uint32 arg)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint32 reg_value;
    uint32 max_value;
    int field_width;
    int rv;

    if (SOC_REG_FIELD_VALID(unit, reg, field)) {
        /* Check the range of the value to be programmed */
        field_width = soc_reg_field_length(unit, reg, field);
        max_value = (field_width < 32) ? ((1 << field_width) - 1) : -1;
        if (arg > max_value) {
            return BCM_E_PARAM;
        }

        rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        soc_reg_field_set(unit, reg, &reg_value, field, arg);
        rv = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_value);
        SOC_IF_ERROR_RETURN(rv);
    }
    return SOC_E_NONE;
#else
    return SOC_E_NONE;
#endif
}

int
_bcm_th3_int_reg64_get(int unit, soc_reg_t reg, soc_field_t field, uint64 *arg)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int rv;
    uint64 reg_value;

    if (SOC_REG_FIELD_VALID(unit, reg, field)) {
        COMPILER_64_ZERO(reg_value);
        rv = soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        *arg = soc_reg64_field_get(unit, reg, reg_value, field);
    }
    return SOC_E_NONE;
#else
    return SOC_E_NONE;
#endif
}


int
_bcm_th3_int_reg32_get(int unit, soc_reg_t reg, soc_field_t field, uint32 *arg)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int rv;
    uint32 reg_value;

    if (SOC_REG_FIELD_VALID(unit, reg, field)) {
        rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        *arg = soc_reg_field_get(unit, reg, reg_value, field);
    }
    return SOC_E_NONE;
#else
    return SOC_E_NONE;
#endif
}


/*
 * Function:
 *    bcmi_esw_int_switch_control_set
 * Purpose:
 *    Implement various INT switch control types to perform
 *  system wide configuration to enable INT packet detection 
 *  and processing.
 *
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to modify
 *      arg - The value with which to set the parameter
 * Returns:
 *    BCM_E_XXX
 */

int
bcmi_esw_int_switch_control_set(int unit, 
                           bcm_switch_control_t type,
                           int arg)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        uint32 reg_value;
        uint64 reg_val64;
        soc_field_t field;
        soc_field_t field1;

        switch (type) {
            case bcmSwitchIntL4DestPort1:
            case bcmSwitchIntMaxPayloadLength:
                if (type == bcmSwitchIntL4DestPort1) {
                    if ((arg < 0) || (arg > 0xffff)) {
                        return BCM_E_PARAM;
                    }
                    field = UDP_DEST_PORTf; 
                    if (arg) {
                        rv = _bcm_td3_int_misc_enable(unit);
                    } else {
                        rv = _bcm_td3_int_misc_disable(unit);
                    }
                    SOC_IF_ERROR_RETURN(rv);
                } else {
                    field = MAX_PAYLOAD_LENGTHf; 
                    if (arg < INT_MAX_LEN_OFFSET_VALUE) {
                        return BCM_E_UNAVAIL;
                    }
                    arg -= INT_MAX_LEN_OFFSET_VALUE;
                }
                /* call cancun cch interface function */
                if (SOC_REG_FIELD_VALID(unit, INT_CONTROLr, field)) {
                    rv = soc_reg32_get(unit, INT_CONTROLr, REG_PORT_ANY,0,
                                             &reg_value);
                    SOC_IF_ERROR_RETURN(rv);
                    soc_reg_field_set(unit,INT_CONTROLr,&reg_value,
                               field,arg);
                    rv = soc_reg32_set(unit, INT_CONTROLr, REG_PORT_ANY, 0, 
                               reg_value);
                    SOC_IF_ERROR_RETURN(rv);
                    return SOC_E_NONE;
                } 
                break;
            case bcmSwitchIntProbeMarker1:
            case bcmSwitchIntProbeMarker2:
                if (SOC_REG_IS_VALID(unit, INT_CONTROL_1r)) {
                    rv = soc_reg64_get(unit, INT_CONTROL_1r, REG_PORT_ANY, 0, 
                         &reg_val64);
                    SOC_IF_ERROR_RETURN(rv);
                    if (type == bcmSwitchIntProbeMarker1) {
                        field  = PROBE_MARKER_1_BYTES_1_0f;
                        field1 = PROBE_MARKER_1_BYTES_3_2f;
                    } else {
                        field  = PROBE_MARKER_2_BYTES_1_0f;
                        field1 = PROBE_MARKER_2_BYTES_3_2f;
                    }
                    soc_reg64_field32_set(unit, INT_CONTROL_1r, &reg_val64, 
                         field, arg & 0xffff);
                    soc_reg64_field32_set(unit, INT_CONTROL_1r, &reg_val64, 
                         field1, (arg >> 16) & 0xffff);
                    rv = soc_reg64_set(unit, INT_CONTROL_1r, REG_PORT_ANY, 0, 
                         reg_val64);
                    SOC_IF_ERROR_RETURN(rv);
                    return SOC_E_NONE;
                }
                break;

            case bcmSwitchIntL4DestPortEnable:
                if (SOC_REG_IS_VALID(unit, INT_PARSE_CONTROLr)) {
                    rv = soc_reg64_get(unit, INT_PARSE_CONTROLr, REG_PORT_ANY, 0,
                         &reg_val64);
                    SOC_IF_ERROR_RETURN(rv);

                    field  = UDP_BASED_ENABLEf;
                    soc_reg64_field32_set(unit, INT_PARSE_CONTROLr, &reg_val64,
                         field, arg & 0x1);
                    rv = soc_reg64_set(unit, INT_PARSE_CONTROLr, REG_PORT_ANY, 0,
                         reg_val64);
                    SOC_IF_ERROR_RETURN(rv);
                    return SOC_E_NONE;
                }
                break;

            case bcmSwitchIntProbeMarkerEnable:
                if (SOC_REG_IS_VALID(unit, INT_PARSE_CONTROLr)) {
                    rv = soc_reg64_get(unit, INT_PARSE_CONTROLr, REG_PORT_ANY, 0,
                         &reg_val64);
                    SOC_IF_ERROR_RETURN(rv);

                    field  = PROBE_MARKER_BASED_ENABLEf;
                    soc_reg64_field32_set(unit, INT_PARSE_CONTROLr, &reg_val64,
                         field, arg & 0x1);
                    rv = soc_reg64_set(unit, INT_PARSE_CONTROLr, REG_PORT_ANY, 0,
                         reg_val64);
                    SOC_IF_ERROR_RETURN(rv);
                    return SOC_E_NONE;
                }
                break;

            case bcmSwitchIntL4DestPort2:
            case bcmSwitchIntVersion:
            case bcmSwitchIntRequestVectorValue:
            case bcmSwitchIntRequestVectorMask:
            case bcmSwitchIntSwitchId:
            case bcmSwitchIntEgressTimeDelta:
            case bcmSwitchIntHopLimitCpuEnable:
            case bcmSwitchIntTurnAroundCpuEnable:
            default:
                return BCM_E_UNAVAIL;
                break;
        }
        return BCM_E_UNAVAIL;
    }
#endif  /* defined(BCM_TRIDENT3_SUPPORT) */

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        uint64 arg64;
        uint64 enable64;
        int field_len;
        uint64 max_val;

        if (!soc_feature(unit, soc_feature_inband_network_telemetry)) {
            return BCM_E_UNAVAIL;
        }
        
        COMPILER_64_SET(arg64, 0, (uint32)arg);
        COMPILER_64_SET(enable64, 0, (uint32)1);

        switch (type) {
            /* XXX add code for TH3 handling */
            case bcmSwitchIntL4DestPort1:
                field_len = soc_reg_field_length(unit, INT_DETECT_CONFIG_0r, UDP_DST_PORT1f);
                COMPILER_64_SET(max_val, 0, (uint32)((1 << field_len) - 1));
                if (COMPILER_64_GT(arg64, max_val)) {
                    return BCM_E_PARAM;
                }
                rv =_bcm_th3_int_reg64_set(unit, INT_DETECT_CONFIG_0r, UDP_DST_PORT1f, arg64);
                if (BCM_SUCCESS(rv)) {
                    rv = _bcm_th3_int_reg64_set(unit, INT_DETECT_CONFIG_0r, UDP_DST_PORT1_ENf, enable64);
                }
                break;
            case bcmSwitchIntL4DestPort2:
                field_len = soc_reg_field_length(unit, INT_DETECT_CONFIG_0r, UDP_DST_PORT2f);
                COMPILER_64_SET(max_val, 0, (uint32)((1 << field_len) - 1));
                if (COMPILER_64_GT(arg64, max_val)) {
                    return BCM_E_PARAM;
                }
                rv = _bcm_th3_int_reg64_set(unit, INT_DETECT_CONFIG_0r, UDP_DST_PORT2f, arg64);
                if (BCM_SUCCESS(rv)) {
                    rv = _bcm_th3_int_reg64_set(unit, INT_DETECT_CONFIG_0r, UDP_DST_PORT2_ENf, enable64);
                }
                break;
            case bcmSwitchIntProbeMarker1:
                rv = _bcm_th3_int_reg64_set(unit, INT_DETECT_CONFIG_1r, PROBE_MARKER1f, arg64);
                if (BCM_SUCCESS(rv)) {
                    rv = _bcm_th3_int_reg64_set(unit, INT_DETECT_CONFIG_1r, PROBE_MARKER1_ENf, enable64);
                }
                break;
            case bcmSwitchIntProbeMarker2:
                rv = _bcm_th3_int_reg64_set(unit, INT_DETECT_CONFIG_2r, PROBE_MARKER2f, arg64);
                if (BCM_SUCCESS(rv)) {
                    rv = _bcm_th3_int_reg64_set(unit, INT_DETECT_CONFIG_2r, PROBE_MARKER2_ENf, enable64);
                }
                break;
            case bcmSwitchIntVersion:
                field_len = soc_reg_field_length(unit, INT_DETECT_CONFIG_0r, VERSIONf);
                COMPILER_64_SET(max_val, 0, (uint32)((1 << field_len) - 1));
                if (COMPILER_64_GT(arg64, max_val)) {
                    return BCM_E_PARAM;
                }
                rv = _bcm_th3_int_reg64_set(unit, INT_DETECT_CONFIG_0r, VERSIONf, arg64);
                break;
            case bcmSwitchIntRequestVectorValue:
                rv = _bcm_th3_int_reg64_set(unit, INT_REQUEST_VECTOR_CONFIGr, REQUEST_VECTOR_VALUEf, arg64);
                break;
            case bcmSwitchIntRequestVectorMask:
                rv = _bcm_th3_int_reg64_set(unit, INT_REQUEST_VECTOR_CONFIGr, REQUEST_VECTOR_MASKf, arg64);
                break;
            case bcmSwitchIntSwitchId:
                rv = _bcm_th3_int_reg32_set(unit, EGR_INT_SWITCH_IDr, SWITCHIDf, arg);
                break;
            case bcmSwitchIntEgressTimeDelta:
                rv = _bcm_th3_int_reg32_set(unit, EGR_INT_EGRESS_TIME_DELTAr, DELTAf, arg);
                break;
            case bcmSwitchIntHopLimitCpuEnable:
                rv = _bcm_th3_int_reg32_set(unit, INT_PROCESS_CONFIGr, HOPLIMIT_COPYTO_CPUf, arg);
                break;
            case bcmSwitchIntTurnAroundCpuEnable:
                rv = _bcm_th3_int_reg32_set(unit, INT_PROCESS_CONFIGr, TURNAROUND_COPYTO_CPUf, arg);
                break;
            default:
                break;
        }
    }
#endif  /* defined(BCM_TOMAHAWK3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *    bcmi_esw_int_control_get
 * Purpose:
 *    Get the value of various INT switch control types.
 *
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to modify
 *      arg - The value with which to set the parameter
 * Returns:
 *    BCM_E_XXX
 */

int
bcmi_esw_int_switch_control_get(int unit, 
                           bcm_switch_control_t type,
                           int *arg)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        uint32 reg_value;
        soc_field_t field;
        soc_field_t field1;
        uint64 reg_val64;

        switch (type) {
            case bcmSwitchIntL4DestPort1:
            case bcmSwitchIntMaxPayloadLength:
                /* call cancun cch interface function */
                if (type == bcmSwitchIntL4DestPort1) {
                    field = UDP_DEST_PORTf;
                } else {
                    field = MAX_PAYLOAD_LENGTHf;
                }
                /* call cancun cch interface function */
                if (SOC_REG_FIELD_VALID(unit, INT_CONTROLr, field)) {
                    rv = soc_reg32_get(unit, INT_CONTROLr, REG_PORT_ANY,0,
                                             &reg_value);
                    SOC_IF_ERROR_RETURN(rv);
                    *arg = soc_reg_field_get(unit,INT_CONTROLr,reg_value,
                               field);
                    if (type == bcmSwitchIntMaxPayloadLength) {
                        *arg += INT_MAX_LEN_OFFSET_VALUE;
                    }
                    return SOC_E_NONE;
                }
                break;

            case bcmSwitchIntProbeMarker1:
            case bcmSwitchIntProbeMarker2:
                if (SOC_REG_IS_VALID(unit, INT_CONTROL_1r)) {
                    rv = soc_reg64_get(unit, INT_CONTROL_1r, REG_PORT_ANY, 0, 
                         &reg_val64);
                    SOC_IF_ERROR_RETURN(rv);
                    if (type == bcmSwitchIntProbeMarker1) {
                        field  = PROBE_MARKER_1_BYTES_1_0f;
                        field1 = PROBE_MARKER_1_BYTES_3_2f;
                    } else {
                        field  = PROBE_MARKER_2_BYTES_1_0f;
                        field1 = PROBE_MARKER_2_BYTES_3_2f;
                    }
                    *arg = soc_reg64_field32_get(unit, INT_CONTROL_1r, 
                           reg_val64, field);
                    *arg &= 0xffff;

                    reg_value =  soc_reg64_field32_get(unit, INT_CONTROL_1r, 
                           reg_val64, field1);
                    reg_value <<= 16;
                    *arg |= reg_value;
                    return SOC_E_NONE;
                }
                break;
                
            case bcmSwitchIntL4DestPortEnable:
                if (SOC_REG_IS_VALID(unit, INT_PARSE_CONTROLr)) {
                    rv = soc_reg64_get(unit, INT_PARSE_CONTROLr, REG_PORT_ANY, 0,
                                       &reg_val64);
                    SOC_IF_ERROR_RETURN(rv);

                    field  = UDP_BASED_ENABLEf;
                    *arg = soc_reg64_field32_get(unit, INT_PARSE_CONTROLr, reg_val64,
                                                 field);
                    *arg &= 0x1;
                    return SOC_E_NONE;
                }
                break;

            case bcmSwitchIntProbeMarkerEnable:
                if (SOC_REG_IS_VALID(unit, INT_PARSE_CONTROLr)) {
                    rv = soc_reg64_get(unit, INT_PARSE_CONTROLr, REG_PORT_ANY, 0,
                                       &reg_val64);
                    SOC_IF_ERROR_RETURN(rv);

                    field  = PROBE_MARKER_BASED_ENABLEf;
                    *arg = soc_reg64_field32_get(unit, INT_PARSE_CONTROLr, reg_val64,
                                                 field);
                    *arg &= 0x1;
                    return SOC_E_NONE;
                }
                break;

            case bcmSwitchIntL4DestPort2:
            case bcmSwitchIntVersion:
            case bcmSwitchIntRequestVectorValue:
            case bcmSwitchIntRequestVectorMask:
            case bcmSwitchIntSwitchId:
            case bcmSwitchIntEgressTimeDelta:
            case bcmSwitchIntHopLimitCpuEnable:
            case bcmSwitchIntTurnAroundCpuEnable:
            default:
                return BCM_E_UNAVAIL;
        }
        return BCM_E_UNAVAIL;
    }
#endif  /* defined(BCM_TRIDENT3_SUPPORT) */

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit) &&
        soc_feature(unit, soc_feature_inband_network_telemetry)) {
        uint64 arg64;

        COMPILER_64_ZERO(arg64);
        switch (type) {
            case bcmSwitchIntL4DestPort1:
                rv = _bcm_th3_int_reg64_get(unit, INT_DETECT_CONFIG_0r, UDP_DST_PORT1f, &arg64);
                *arg = (int)COMPILER_64_LO(arg64);
                break;
            case bcmSwitchIntL4DestPort2:
                rv = _bcm_th3_int_reg64_get(unit, INT_DETECT_CONFIG_0r, UDP_DST_PORT2f, &arg64);
                *arg = (int)COMPILER_64_LO(arg64);
                break;
            case bcmSwitchIntProbeMarker1:
                rv = _bcm_th3_int_reg64_get(unit, INT_DETECT_CONFIG_1r, PROBE_MARKER1f, &arg64);
                *arg = (int)COMPILER_64_LO(arg64);
                break;
            case bcmSwitchIntProbeMarker2:
                rv = _bcm_th3_int_reg64_get(unit, INT_DETECT_CONFIG_2r, PROBE_MARKER2f, &arg64);
                *arg = (int)COMPILER_64_LO(arg64);
                break;
            case bcmSwitchIntVersion:
                rv = _bcm_th3_int_reg64_get(unit, INT_DETECT_CONFIG_0r, VERSIONf, &arg64);
                *arg = (int)COMPILER_64_LO(arg64);
                break;
            case bcmSwitchIntRequestVectorValue:
                rv = _bcm_th3_int_reg64_get(unit, INT_REQUEST_VECTOR_CONFIGr, REQUEST_VECTOR_VALUEf, &arg64);
                *arg = (int)COMPILER_64_LO(arg64);
                break;
            case bcmSwitchIntRequestVectorMask:
                rv = _bcm_th3_int_reg64_get(unit, INT_REQUEST_VECTOR_CONFIGr, REQUEST_VECTOR_MASKf, &arg64);
                *arg = (int)COMPILER_64_LO(arg64);
                break;
            case bcmSwitchIntSwitchId:
                rv = _bcm_th3_int_reg32_get(unit, EGR_INT_SWITCH_IDr, SWITCHIDf, (uint32 *)arg);
                break;
            case bcmSwitchIntEgressTimeDelta:
                rv = _bcm_th3_int_reg32_get(unit, EGR_INT_EGRESS_TIME_DELTAr, DELTAf, (uint32 *)arg);
                break;
            case bcmSwitchIntHopLimitCpuEnable:
                rv = _bcm_th3_int_reg32_get(unit, INT_PROCESS_CONFIGr, HOPLIMIT_COPYTO_CPUf, (uint32 *)arg);
                break;
            case bcmSwitchIntTurnAroundCpuEnable:
                rv = _bcm_th3_int_reg32_get(unit, INT_PROCESS_CONFIGr, TURNAROUND_COPYTO_CPUf, (uint32 *)arg);
                break;

            default:
                rv = BCM_E_UNAVAIL;
                break;
        }
    }
#endif  /* defined(BCM_TOMAHAWK3_SUPPORT) */

    return rv;
}
