/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS Queue Management
 * Purpose: API to set different cosq, priorities, and scheduler registers.
 */

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/cosq.h>

#define HR2_PKT_REFRESH_MAX   0xfffff
#define HR2_PKT_THD_MAX   0xfff

STATIC int
_bcm_hr2_cosq_port_packet_bandwidth_set(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       int pps, int burst)
{
    uint32 regval;
    soc_reg_t maxbucket_config_reg, maxbucket_reg;
    soc_field_t refresh_f, thd_sel_f, bucket_f;

    if (cosq < 0) {
        maxbucket_config_reg = PKTPORTMAXBUCKETCONFIGr;
        maxbucket_reg = PKTPORTMAXBUCKETr;
        refresh_f = PKT_PORT_MAX_REFRESHf;
        thd_sel_f = PKT_PORT_MAX_THD_SELf;
        bucket_f = PKT_PORT_MAX_BUCKETf;
        cosq = 0;
    } else {
        maxbucket_config_reg = PKTMAXBUCKETCONFIGr;
        maxbucket_reg = PKTMAXBUCKETr;
        refresh_f = PKT_MAX_REFRESHf;
        thd_sel_f = PKT_MAX_THD_SELf;
        bucket_f = PKT_MAX_BUCKETf;
    }

    /*
     * To set the new Bandwidth settings, the procedure adopted is
     * a. reset MAXBUCKETCONFIG and MAXBUCKET
     * b. update MAXBUCKETCONFIG with new values passed
     * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
     */

    /* Disable egress metering */
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, maxbucket_config_reg, REG_PORT_ANY, cosq, &regval));
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, refresh_f, 0);
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, thd_sel_f, 0);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, maxbucket_config_reg, REG_PORT_ANY, cosq, regval));

    /* reset the MAXBUCKET register*/
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, maxbucket_reg, REG_PORT_ANY, cosq, &regval));
    soc_reg_field_set(unit, maxbucket_reg, &regval, bucket_f, 0);
    soc_reg_field_set(unit, maxbucket_reg, &regval, IN_PROFILE_FLAGf, 0);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, maxbucket_reg, REG_PORT_ANY, cosq, regval));

    /* Check packets-per second upper limit */
    if (pps > HR2_PKT_REFRESH_MAX) {
        pps = HR2_PKT_REFRESH_MAX;
    }

    /* Check burst upper limit */
    if (burst > HR2_PKT_THD_MAX) {
        burst = HR2_PKT_THD_MAX;
    }

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, maxbucket_config_reg, REG_PORT_ANY, cosq, &regval));
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, refresh_f, pps);
    soc_reg_field_set(unit, maxbucket_config_reg, 
                                &regval, thd_sel_f, burst);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, maxbucket_config_reg, REG_PORT_ANY, cosq, regval));

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    SOC_IF_ERROR_RETURN(READ_PKTSHAPECONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, PKTSHAPECONFIGr, regval, ENABLEf) == 0) {
        soc_reg_field_set(unit, PKTSHAPECONFIGr, &regval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_PKTSHAPECONFIGr(unit, regval));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_hr2_cosq_port_packet_bandwidth_get(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       int *pps, int *burst)
{
    uint32 regval;
    soc_reg_t maxbucket_config_reg;
    soc_field_t refresh_f, thd_sel_f;

    if (cosq < 0) {
        maxbucket_config_reg = PKTPORTMAXBUCKETCONFIGr;
        refresh_f = PKT_PORT_MAX_REFRESHf;
        thd_sel_f = PKT_PORT_MAX_THD_SELf;
        cosq = 0;
    } else {
        maxbucket_config_reg = PKTMAXBUCKETCONFIGr;
        refresh_f = PKT_MAX_REFRESHf;
        thd_sel_f = PKT_MAX_THD_SELf;
    }

    /* Disable egress metering */
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, maxbucket_config_reg, REG_PORT_ANY, cosq, &regval));
    *pps = soc_reg_field_get(unit, maxbucket_config_reg, regval, refresh_f);
    *burst = soc_reg_field_get(unit, maxbucket_config_reg, 
                                         regval, thd_sel_f);

    return BCM_E_NONE;
}

int
bcm_hr2_cosq_port_pps_set(int unit, bcm_port_t port,
                         bcm_cos_queue_t cosq, int pps)
{
    int temp_pps, burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    } else if (pps < 0) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (_bcm_hr2_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                            &temp_pps, &burst));

    /* Replace the current PPS setting, keep BURST the same */
    return _bcm_hr2_cosq_port_packet_bandwidth_set(unit, port, cosq,
                                              pps, burst);
}

int
bcm_hr2_cosq_port_pps_get(int unit, bcm_port_t port,
                         bcm_cos_queue_t cosq, int *pps)
{
    int burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }
          
    return _bcm_hr2_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                              pps, &burst);
}

int
bcm_hr2_cosq_port_burst_set(int unit, bcm_port_t port,
                           bcm_cos_queue_t cosq, int burst)
{
    int pps, temp_burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    } else if (burst < 0) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (_bcm_hr2_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                            &pps, &temp_burst));

    /* Replace the current BURST setting, keep PPS the same */
    return _bcm_hr2_cosq_port_packet_bandwidth_set(unit, port, cosq,
                                              pps, burst);
}

int
bcm_hr2_cosq_port_burst_get(int unit, bcm_port_t port,
                           bcm_cos_queue_t cosq, int *burst)
{
    int pps;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }
             
    return _bcm_hr2_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                              &pps, burst);
}
