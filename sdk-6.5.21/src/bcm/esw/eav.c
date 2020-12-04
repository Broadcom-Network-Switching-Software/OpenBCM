/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Ethernet AV BCM APIs
 */

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/eav.h>

#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/switch.h>

#include <bcm_int/esw_dispatch.h>

#define PHASE_FREQUENCY_ADJUSTMENT_MODE_SHIFT  20
#define PHASE_FREQUENCY_ADJUSTMENT_MODE_MASK   0x3
#define PHASE_FREQUENCY_ADJUSTMENT_ATTR_MASK   0xFFFFF

#define HK_BURST_GRANULARITY   64 /* 64bytes */
#define HK_BW_FIELD_MAX        0x3FFFF
#define BCM_EAV_CLASS_A_QUEUE  3
#define BCM_EAV_CLASS_B_QUEUE  2

/*
* Function:
*      bcm_eav_init
* Description:
*      Initialize the Residential Ethernet module and enable
*      the Ethernet AV (EAV) support.
* Parameters:
*      unit - device unit number.
* Returns:
*      BCM_E_XXX
*      
* Notes:
*      1. This function will enable the global EAV functionality
*      2. Decide the way to report egress timestamp info to CPU
*         Either loopback reporting packets
*         or CPU directly read register later.
*/
int 
bcm_esw_eav_init(int unit)
{
    /* Init ether type and DA of TS, SRP and MMRP. */
    if (SOC_IS_HAWKEYE(unit)) {

        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncDestMacOui, 
                                       TS_MAC_OUI_DEFAULT)); 
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncDestMacNonOui, 
                                       TS_MAC_NONOUI_DEFAULT)); 
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncEthertype, 
                                       TS_ETHERTYPE_DEFAULT));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchMMRPEthertype, 
                                       MMRP_ETHERTYPE_DEFAULT)); 
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchMMRPDestMacOui, 
                                       MMRP_MAC_OUI_DEFAULT)); 
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchMMRPDestMacNonOui, 
                                       MMRP_MAC_NONOUI_DEFAULT)); 

        /* Enable CPU 802.1as egress capability to get ingress timestamp. */
        BCM_IF_ERROR_RETURN(bcm_esw_port_control_set(unit, CMIC_PORT(unit),
                            bcmPortControlIEEE8021ASEnableEgress,1));
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
* Function:
*      bcm_eav_port_enable_get
* Description:
*      Get enable status of per port Ethernet AV functionality
* Parameters:
*      unit - device unit number.
*      port - port number
*      enable - (OUT) TRUE, port is enabled for Ethernet AV
*                     FALSE, port is disabled for Ethernet AV
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_port_enable_get(int unit, bcm_port_t port, int *enable)
{
    return bcm_esw_port_control_get(unit, port, 
                                    bcmPortControlTimestampEnable, enable);
}

/*
* Function:
*      bcm_eav_port_enable_set
* Description:
*      Enable or disable per port Ethernet AV functionality
* Parameters:
*      unit - device unit number.
*      port - port number
*      enable - TRUE, port is enabled for Ethernet AV
*               FALSE, port is disabled for Ethernet AV
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
*
* Notes:
*      Need to disable the per port flow control
*/

int 
bcm_esw_eav_port_enable_set(int unit, bcm_port_t port, int enable)
{
    if (SOC_IS_HAWKEYE(unit)) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        /* Enable EAV packets parsing */
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_control_set(unit, port, 
                                     bcmPortControlTimestampEnable, 
                                     enable));
        /* Enable TS protocol packets parsing */
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_control_set(unit, port, 
                                     bcmPortControlIEEE8021ASEnableIngress, 
                                     enable));
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_control_set(unit, port,
                                     bcmPortControlIEEE8021ASEnableEgress,
                                     enable));
        /* Setup switch behavior for TS packets. */
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_port_set(unit, port,
                                            bcmSwitchTimeSyncPktDrop, enable));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_port_set(unit, port,
                                            bcmSwitchTimeSyncPktToCpu, enable));
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
* Function:
*      bcm_eav_link_status_get
* Description:
*      Get link status of per port Ethernet AV functionality
* Parameters:
*      unit - device unit number.
*      port - port number
*      link - (OUT) TRUE, Ethernet AV led is light on
*                     FALSE, Ethernet AV led is light off
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_link_status_get(int unit, bcm_port_t port, int *link)
{
    return bcm_esw_port_link_status_get(unit, port, link);
}

/*
* Function:
*      bcm_eav_link_status_set
* Description:
*      Set the EAV link status of  per port Ethernet AV functionality
* Parameters:
*      unit - device unit number.
*      port - port number
*      link - TRUE, Ethernet AV led is light on
*               FALSE, Ethernet AV led is light off
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
*
* Notes:
*/

int 
bcm_esw_eav_link_status_set(int unit, bcm_port_t port, int link)
{
    return BCM_E_UNAVAIL;
}

/*
* Function:
*      bcm_eav_control_get
* Description:
*      Get the configuration for specific type
* Parameters:
*      unit - device unit number.
*      type - configuration type
*      arg1 - (OUT) the pointer buffer to store the returned configuration
*      arg2 - (OUT) the pointer buffer to store the returned configuration
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_control_get(int unit, bcm_eav_control_t type, 
        uint32 *arg1, uint32 *arg2)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval;

    if (SOC_IS_HAWKEYE(unit)) {
        switch (type) {
            case bcmEAVControlTimeBase:
                if (arg1 != NULL) {
                    /* coverity[result_independent_of_operands] */
                    SOC_IF_ERROR_RETURN(READ_CMIC_TIMESYNC_TIMERr(unit, arg1));
                }
                return BCM_E_NONE;
            case bcmEAVControlTimeAdjust:
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(READ_CMIC_TIMESYNC_CONTROLr(unit, &rval));
                if (arg1 != NULL) {
                    *arg1 = (rval >> PHASE_FREQUENCY_ADJUSTMENT_MODE_SHIFT) &
                             PHASE_FREQUENCY_ADJUSTMENT_MODE_MASK;
                }
                if (arg2 != NULL) {
                    *arg2 = (rval & PHASE_FREQUENCY_ADJUSTMENT_ATTR_MASK);
                }
                return BCM_E_NONE;
            default:
                break;
        }
        return BCM_E_PARAM; 
    }
    return rv;
}

/*
* Function:
*      bcm_eav_control_set
* Description:
*      Set the configuration for specific type
* Parameters:
*      unit - device unit number.
*      type - configuration type
*      arg1 - the configuration data to set 
*      arg2 - the configuration data to set 
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_control_set(int unit, bcm_eav_control_t type, 
       uint32 arg1, uint32 arg2)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval = 0;
    if (SOC_IS_HAWKEYE(unit)) {
        switch (type) {
            case bcmEAVControlTimeAdjust:
                /* arg1 = mode, arg2 = phase/freq adjustment value */
                rval = (arg1 << PHASE_FREQUENCY_ADJUSTMENT_MODE_SHIFT) | 
                       (arg2 & PHASE_FREQUENCY_ADJUSTMENT_ATTR_MASK);
                switch (arg1) {
                    case BCM_EAV_TIMEREF_MODE_FREERUN:
                        rval = 0x0;
                    case BCM_EAV_TIMEREF_MODE_PHASE_ADJUSTMENT:
                    case BCM_EAV_TIMEREF_MODE_FREQ_ADJUMENT1:
                    case BCM_EAV_TIMEREF_MODE_FREQ_ADJUMENT2:
                        break;
                    default:
                    return BCM_E_PARAM;
                }
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(WRITE_CMIC_TIMESYNC_CONTROLr(unit, rval));
                break;
            default:
                return BCM_E_PARAM;
        }
        return BCM_E_NONE; 
    }
    return rv;
}

/*
* Function:
*      bcm_eav_egress_timestamp_get
* Description:
*      Get the per port egress timestamp value of TS protocol packets
* Parameters:
*      unit - device unit number
*      port - port number
*      timestamp - (OUT) the pointer buffer to store the returned timestamp  
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_timestamp_get(int unit, bcm_port_t port, uint32 *timestamp)
{
    if (SOC_IS_HAWKEYE(unit)) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        return bcm_esw_port_control_get(unit, port, 
                                        bcmPortControlTimestampTransmit, 
                                        (int *)timestamp);
    }
    return BCM_E_UNAVAIL;
}

/*
* Function:
*      bcm_eav_timesync_mac_get
* Description:
*      Get the Mac address of Time Sync protocol
* Parameters:
*      unit - device unit number
*      eav_mac - the pointer buffer to restorm the mac addrss  
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_timesync_mac_get(int unit, bcm_mac_t eav_mac)
{
    uint32 MacOui, MacNonOui;
    if (SOC_IS_HAWKEYE(unit)) {

        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchTimeSyncDestMacOui, 
                                      (int *)&MacOui));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchTimeSyncDestMacNonOui, 
                                      (int *)&MacNonOui));
        eav_mac[0] = (uint8) (MacOui >> 16);
        eav_mac[1] = (uint8) (MacOui >> 8); 
        eav_mac[2] = (uint8) (MacOui); 
        eav_mac[3] = (uint8) (MacNonOui >> 16);
        eav_mac[4] = (uint8) (MacNonOui >> 8); 
        eav_mac[5] = (uint8) (MacNonOui); 

        return BCM_E_NONE; 
    }
    return BCM_E_UNAVAIL;
}

/*
* Function:
*      bcm_eav_timesync_mac_set
* Description:
*      Set the Mac address of Time Sync protocol
* Parameters:
*      unit - device unit number
*      eav_mac - the pointer buffer to restorm the mac addrss  
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_timesync_mac_set(int unit, bcm_mac_t eav_mac)
{
    uint32 MacOui, MacNonOui;

    if (SOC_IS_HAWKEYE(unit)) {
        MacOui = (((uint32)eav_mac[0]) << 16 | ((uint32)eav_mac[1]) << 8 | ((uint32)eav_mac[2]));
        MacNonOui = (((uint32)eav_mac[3]) << 16 | ((uint32)eav_mac[4]) << 8 | ((uint32)eav_mac[5]));

        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncDestMacOui, 
                                       MacOui));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncDestMacNonOui, 
                                       MacNonOui));

        return BCM_E_NONE; 
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_eav_srp_mac_ethertype_set
 * Description:
 *      Get the Mac address and Ethertype used to trap SRP protocol packets
 * Parameters:
 *      unit - device unit number
 *      mac  - the mac addrss   
 *      ethertype - the EtherType
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_srp_mac_ethertype_set(int unit, bcm_mac_t mac, bcm_port_ethertype_t ethertype)
{
    uint32 MacOui, MacNonOui;

    if (SOC_IS_HAWKEYE(unit)) {
        MacOui = (((uint32)mac[0]) << 16 | ((uint32)mac[1]) << 8 | ((uint32)mac[2]));
        MacNonOui = (((uint32)mac[3]) << 16 | ((uint32)mac[4]) << 8 | ((uint32)mac[5]));

        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchSRPDestMacOui, 
                                       MacOui));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchSRPDestMacNonOui, 
                                       MacNonOui));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchSRPEthertype, ethertype));

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_eav_srp_mac_ethertype_get
 * Description:
 *      Get the Mac address and Ethertype used to trap SRP protocol packets
 * Parameters:
 *      unit - device unit number
 *      mac  - (OUT)the mac addrss   
 *      ethertype - (OUT)the EtherType
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */                               
int 
bcm_esw_eav_srp_mac_ethertype_get(int unit, bcm_mac_t mac, bcm_port_ethertype_t *ethertype)
{
    uint32 MacOui, MacNonOui;
    int    type;
    if (SOC_IS_HAWKEYE(unit)) {

        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchSRPDestMacOui, 
                                      (int *)&MacOui));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchSRPDestMacNonOui, 
                                      (int *)&MacNonOui));
        mac[0] = (uint8) (MacOui >> 16);
        mac[1] = (uint8) (MacOui >> 8); 
        mac[2] = (uint8) (MacOui); 
        mac[3] = (uint8) (MacNonOui >> 16);
        mac[4] = (uint8) (MacNonOui >> 8); 
        mac[5] = (uint8) (MacNonOui); 

        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchSRPEthertype, 
                                       &type));
        *ethertype = type;

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_eav_pcp_mapping_set
 * Description:
 *      Set the PCP(priority) value mapping for each EAV class packets
 * Parameters:
 *      unit - device unit number
 *      type - Class A or Class B stream
 *      pcp  - Priority for the Class   
 *      rempapped_pcp - For NonEAV traffic with PCP=ClassA_PCP or ClassB_PCP
 *      exiting through an egress port configured in EAV mode must be remapped
 *      to another pcp.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_pcp_mapping_set(
    int unit, 
    bcm_eav_stream_class_t type, 
    int pcp, 
    int remapped_pcp)
{
    if (type == bcmEAVCStreamClassA) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncClassAPktPrio, pcp));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncClassAExeptionPktPrio, 
                                   remapped_pcp));
    } else if (type == bcmEAVCStreamClassB) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncClassBPktPrio, pcp));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncClassBExeptionPktPrio, 
                                   remapped_pcp));
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
    }

/*
 * Function:
 *      bcm_esw_eav_pcp_mapping_get
 * Description:
 *      Set the PCP(priority) value mapping for each EAV class packets
 * Parameters:
 *      unit - device unit number
 *      type - Class A or Class B stream
 *      pcp  - Priority for the Class   
 *      rempapped_pcp - For NonEAV traffic with PCP=ClassA_PCP or ClassB_PCP
 *      exiting through an egress port configured in EAV mode must be remapped
 *      to another pcp.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_pcp_mapping_get(
    int unit, 
    bcm_eav_stream_class_t type, 
    int *pcp, 
    int *remapped_pcp)
{

    if (type == bcmEAVCStreamClassA) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchTimeSyncClassAPktPrio, pcp));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchTimeSyncClassAExeptionPktPrio, 
                                   remapped_pcp));
        } else if (type == bcmEAVCStreamClassB) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchTimeSyncClassBPktPrio, pcp));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchTimeSyncClassBExeptionPktPrio, 
                                   remapped_pcp));
        } else {
            return BCM_E_PARAM;
        }

        return BCM_E_NONE;
    }

/*
 * Function:
 *      bcm_esw_eav_bandwidth_set
 * Description:
 *      Set the reserved bandwidth for Class A or B stream traffic
 * Parameters:
 *      unit - device unit number
 *      port - port number   
 *      type - Class A or Class B stream
 *      bytes_sec - maximum bytes per second.
 *      bytes_burst - maximum burst size in bytes. 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_bandwidth_set(
    int unit, 
    bcm_port_t port, 
    bcm_eav_stream_class_t type, 
    uint32 bytes_sec,
    uint32 bytes_burst)
{
#if defined(BCM_HAWKEYE_SUPPORT)
    uint32 kbits_sec_max, kbits_sec_burst, burst_size, rval, meter_flags = 0;
    bcm_cos_queue_t cosq;
    
    if (SOC_IS_HAWKEYE(unit)) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        if (type == bcmEAVCStreamClassA) {
            cosq = BCM_EAV_CLASS_A_QUEUE;
        } else if (type == bcmEAVCStreamClassB) {
            cosq = BCM_EAV_CLASS_B_QUEUE;
        } else {
            return BCM_E_PARAM;
        }
        
        /* disable ingress policing and egress metering */
        if (bytes_sec) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_rate_ingress_set(unit, port, 0, 0));
            BCM_IF_ERROR_RETURN(bcm_esw_port_rate_egress_set(unit, port, 0, 0));
        }
        
        kbits_sec_max = bytes_sec * 8 / 1000;
        kbits_sec_burst = bytes_burst * 8 / 1000;

        BCM_IF_ERROR_RETURN(
            bcm_fb_cosq_port_bandwidth_set(unit, port, cosq, kbits_sec_max,
                                           kbits_sec_max, kbits_sec_burst,
                                           meter_flags));

        burst_size = (bytes_burst + (HK_BURST_GRANULARITY - 1)) / HK_BURST_GRANULARITY;
        if (burst_size > HK_BW_FIELD_MAX) {
                burst_size = HK_BW_FIELD_MAX;
        }

        SOC_IF_ERROR_RETURN(READ_MAXBUCKETCONFIGr(unit, port, cosq, &rval));
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &rval,MAX_THD_SELf, 
                          (burst_size >> 6) & 0xFFF);
        SOC_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIGr(unit, port, cosq, rval));
 
        SOC_IF_ERROR_RETURN(READ_EAVBUCKETCONFIG_EXTr(unit, port, &rval));
        soc_reg_field_set(unit, EAVBUCKETCONFIG_EXTr, &rval,
                          (type == bcmEAVCStreamClassA)? CLASSA_THD_SEL_6LSBf:
                              CLASSB_THD_SEL_6LSBf, burst_size & 0x3f);
        SOC_IF_ERROR_RETURN(WRITE_EAVBUCKETCONFIG_EXTr(unit, port, rval));

        return BCM_E_NONE;
    }
#endif /* BCM_HAWKEYE_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_eav_bandwidth_get
 * Description:
 *      Get the reserved bandwidth for Class A or B stream traffic
 * Parameters:
 *      unit - device unit number
 *      port - port number   
 *      type - Class A or Class B stream
 *      bytes_sec - bytes per second.
 *      bytes_burst - max burst bytes.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_bandwidth_get(
    int unit, 
    bcm_port_t port, 
    bcm_eav_stream_class_t type, 
    uint32 *bytes_sec,
    uint32 *bytes_burst)
{
#if defined(BCM_HAWKEYE_SUPPORT)
    uint32 regval, kbits_sec_max, kbits_sec_min, kbits_sec_burst;
    uint32 burst_hi, burst_lo, meter_flags = 0;
    bcm_cos_queue_t cosq;
    
    if (SOC_IS_HAWKEYE(unit)) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        if (type == bcmEAVCStreamClassA) {
            cosq = BCM_EAV_CLASS_A_QUEUE;
        } else if (type == bcmEAVCStreamClassB) {
            cosq = BCM_EAV_CLASS_B_QUEUE;
        } else {
            return BCM_E_PARAM;
        }
        
        BCM_IF_ERROR_RETURN(
            bcm_fb_cosq_port_bandwidth_get(unit, port, cosq, &kbits_sec_min, 
                                           &kbits_sec_max, &kbits_sec_burst, 
                                           &meter_flags));

        
        *bytes_sec = (kbits_sec_max * 1000) / 8;
        SOC_IF_ERROR_RETURN(READ_MAXBUCKETCONFIGr(unit, port, cosq, &regval));
        burst_hi = soc_reg_field_get(unit, MAXBUCKETCONFIGr, regval, MAX_THD_SELf);
        SOC_IF_ERROR_RETURN(READ_EAVBUCKETCONFIG_EXTr(unit, port, &regval));
        if (type == bcmEAVCStreamClassA) {
            burst_lo = soc_reg_field_get(unit, EAVBUCKETCONFIG_EXTr, regval, CLASSA_THD_SEL_6LSBf);
        } else {
            burst_lo = soc_reg_field_get(unit, EAVBUCKETCONFIG_EXTr, regval, CLASSB_THD_SEL_6LSBf);
        }
        *bytes_burst = ((burst_hi << 6) | burst_lo)*HK_BURST_GRANULARITY;
        return BCM_E_NONE;
    }
#endif /* BCM_HAWKEYE_SUPPORT */
    return BCM_E_UNAVAIL;
}

