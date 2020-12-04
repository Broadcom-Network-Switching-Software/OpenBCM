/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/drv.h>
#include <bcm_int/esw/port.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <soc/profile_mem.h>
#include <soc/bondoptions.h>
#include <shared/bsl.h>
#include <bcm/port.h>
#include <bcm_int/esw_dispatch.h>

#if defined(BCM_GREYHOUND2_SUPPORT)
int bcmi_gh2_port_mac_type_get(int unit, soc_port_t port,
                               bcmi_port_mac_type_t *port_type)

{
    if (NULL == port_type) {
        return SOC_E_PARAM;
    }
    if (IS_XL_PORT(unit, port)) {
        *port_type = bcmiPortMacTypeXLmac;
        return SOC_E_NONE;
    } else if (IS_GE_PORT(unit, port)) {
        *port_type = bcmiPortMacTypeUnimac;
        return SOC_E_NONE;
    } else if (IS_CL_PORT(unit, port)) {
        *port_type = bcmiPortMacTypeCLmac;
        return SOC_E_NONE;
    } else {
        return SOC_E_PARAM;
    }
}

#define E2EFC_REMOTE_MODE_VALID(_m_)                                        \
    do {                                                                    \
        if ((_m_) != bcmPortE2efcModeTx && (_m_) != bcmPortE2efcModeRx) {   \
            return BCM_E_UNAVAIL;                                           \
        }                                                                   \
    } while (0)
#define GH2_BYTE2CELL(_a_) ((_a_) / 144)

#define GH2_CELL2BYTE(_a_) ((_a_) * 144)

#define GH2_E2EFC_PBM2BINDEX(_a_)   \
    do {                            \
        switch (_a_)                \
        {                           \
            case 1:                 \
                (_a_) = 1;          \
                break;              \
            case 2:                 \
                (_a_) = 2;          \
                break;              \
            case 4:                 \
                (_a_) = 3;          \
                break;              \
            case 8:                 \
                (_a_) = 4;          \
                break;              \
            default:                \
                return BCM_E_PARAM; \
        }                           \
    } while (0)

#define GH2_E2EFC_REMOTE_PORT_NUM 48
#define GH2_E2EFC_RMOD_NUM 4
#define GH2_E2EFC_MODULE_HDR_LEN 2
#define GH2_PORT_BLOCK_ENT_NUM 4
#define GH2_PORT_BLOCK_INIT_NUM (SOC_IS_FIRELIGHT(unit) ? 49:57)

STATIC const soc_field_t e2efc_rmod_id_f[GH2_E2EFC_RMOD_NUM] = {
        RMOD_ID0f, RMOD_ID1f, RMOD_ID2f, RMOD_ID3f};
STATIC const soc_reg_t
e2efc_rmod_id_r[bcmPortE2efcModeCount][GH2_E2EFC_RMOD_NUM] = {
        {E2EFC_TX_RMODID_0r, E2EFC_TX_RMODID_0r,
        E2EFC_TX_RMODID_1r, E2EFC_TX_RMODID_1r},
        {E2EFC_RX_RMODID_0r, E2EFC_RX_RMODID_0r,
        E2EFC_RX_RMODID_1r, E2EFC_RX_RMODID_1r}};
STATIC const soc_field_t e2efc_rmod_enable[GH2_E2EFC_RMOD_NUM] = {
        V0f, V1f, V2f, V3f};

typedef enum port_e2efc_mac_type_c {
    E2efcModuleHdrXlmac = 0,
    E2efcModuleHdrClmac = 1,
    E2efcModuleHdrMacCount = 2
} port_e2efc_mac_type_t;

/* XLMAC : 0, CLMAC:1 */
STATIC const soc_reg_t
e2efc_module_hdr_r[E2efcModuleHdrMacCount][GH2_E2EFC_MODULE_HDR_LEN] = {
        {XLMAC_E2EFC_MODULE_HDR_0r, XLMAC_E2EFC_MODULE_HDR_1r},
        {CLMAC_E2EFC_MODULE_HDR_0r, CLMAC_E2EFC_MODULE_HDR_1r}};

/*
 * Description:
 *      Enable and configure the remote module to apply E2EFC mechanism.
 *      Or disable it.
 * Parameter:
 *      unit           - (IN) unit number
 *      remote_module  - (IN) remote module id
 *      mode           - (IN) selected mode
 *      enable         - (IN) enable or disable selection
 *      e2efc_rmod_cfg - (IN) structure containing the configuration.
 *                       Ignored when enable = 0.
 */
int
bcmi_gh2_port_e2efc_remote_module_enable_set(
    int unit,
    bcm_module_t remote_module,
    bcm_port_e2efc_mode_t mode,
    int enable,
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg)
{
    bcm_port_t local_port;
    uint32 val;
    bcm_module_t remote_module_clear = 0;
    int modid, i, blk, bindex, blk_num, port_mode;
    /* XLMAC Related Parameters */
    uint32 hdr_0_lo, hdr_0_hi, hdr_1_lo, hdr_1_hi;
    uint32 src_mod, dest_mod, src_port;
    uint64 val_64;
    int time_unit_sel, time_units, value_a, value_b;
    uint32 val_min, val_max, new_pbm = 0;
    port_e2efc_mac_type_t mac_type;
    int hdr_0 = 0, hdr_1 = 1;

    E2EFC_REMOTE_MODE_VALID(mode);
    /* Check the remote module id is in the valid range or not */
    if (!SOC_MODID_ADDRESSABLE(unit, remote_module)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid remote module ID.\n")));
        return BCM_E_BADID;
    }

    if (e2efc_rmod_cfg == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Null Remote Module Configuration.\n")));
        return BCM_E_PARAM;
    }

    /* Check the logical port is in the right range  */
    if (mode == bcmPortE2efcModeTx &&
        (!SOC_PORT_VALID(unit, e2efc_rmod_cfg->local_port))) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid port range.\n")));
        return BCM_E_PARAM;
    }
    /* Checking whether remote port is HG port */
    if (mode == bcmPortE2efcModeTx) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(
                                unit, e2efc_rmod_cfg->local_port,
                                &local_port));
        if (!IS_HG_PORT(unit, local_port)) {
            LOG_ERROR(
                BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "Error: unit %d port %d is not a Higig port. "
                            "E2EFC messages can only be transmitted to "
                            "or received from Higig ports.\n"),
                 unit, local_port));
            return BCM_E_PARAM;
        }

        if (e2efc_rmod_cfg->pkt_per_second < 0) {
            LOG_ERROR(
                BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "Frequency of sending an E2EFC message "
                            "cannot be less than zero. \n")));
            return BCM_E_PARAM;
        }
    }

    if (enable == 1) {
        /* Search whether the existing remote module ID exist */
        for (i = 0; i < GH2_E2EFC_RMOD_NUM; i++) {
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_r[mode][i],
                                              REG_PORT_ANY, 0, &val));
            if ((remote_module == (soc_reg_field_get(
                                    unit, e2efc_rmod_id_r[mode][i], val,
                                    e2efc_rmod_id_f[i]))) &&
                (soc_reg_field_get(unit, e2efc_rmod_id_r[mode][i], val,
                                   e2efc_rmod_enable[i]) == 1)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                          "Remote Module ID already exists.\n")));
                return BCM_E_EXISTS;
            }
        }
        /* Traverse 4 remote module id and setup the one that is not enabled */
        modid = -1;
        for (i = 0; i < GH2_E2EFC_RMOD_NUM; i++) {
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_r[mode][i],
                                              REG_PORT_ANY, 0, &val));
            if (!(soc_reg_field_get(unit, e2efc_rmod_id_r[mode][i], val,
                                    e2efc_rmod_enable[i]))) {
                soc_reg_field_set(unit, e2efc_rmod_id_r[mode][i], &val,
                                  e2efc_rmod_id_f[i], remote_module);
                soc_reg_field_set(unit, e2efc_rmod_id_r[mode][i], &val,
                                  e2efc_rmod_enable[i], enable);
                BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                                  e2efc_rmod_id_r[mode][i],
                                                  REG_PORT_ANY, 0, val));
                modid = i;
                break;
            }
        }
        if (modid == -1 ) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                      "E2EFC No Available Remote.Module to Setup"
                      "Maximun 4 Remote Module only.\n")));
            return BCM_E_FULL;
        }
        /* Set up Higig Port and Port Speed Information */
        if (mode == bcmPortE2efcModeTx) {
            /* Register E2EFC_RMOD_CFG fill in */
            if (soc_feature(unit, soc_feature_logical_port_num)) {
                blk = SOC_PORT_BLOCK(
                        unit, SOC_INFO(unit).port_l2p_mapping[local_port]);
                bindex = SOC_PORT_BINDEX(
                            unit, SOC_INFO(unit).port_l2p_mapping[local_port]);
            } else {
                blk = SOC_PORT_BLOCK(unit, local_port);
                bindex = SOC_PORT_BINDEX(unit, local_port);
            }
            blk_num = SOC_BLOCK_INFO(unit, blk).number;

            if (IS_CL_PORT(unit, local_port)) {
#if defined(BCM_FIRELIGHT_SUPPORT)
                if (SOC_IS_FIRELIGHT(unit)) {
                    /*
                     * the SOC_BLOCK_INFO will assign block 0 to CL_PORT,
                     * which should be 3 as it follow after XL_PORT block 2
                     */
                    blk_num += 3;
                } else
#endif
                {
                    /*
                     * the SOC_BLOCK_INFO will assign block 0 to CL_PORT,
                     * which should be 7 as it follow after XL_PORT block 6
                     */
                    blk_num += 7;
                }
            }
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_RMOD_CFGr,
                                              REG_PORT_ANY, modid, &val));
            soc_reg_field_set(unit, E2EFC_RMOD_CFGr, &val,
                              PORT_GRP_IDf, blk_num);
            new_pbm |= (1 << bindex);
            soc_reg_field_set(unit, E2EFC_RMOD_CFGr, &val,
                              TX_ENABLE_BMPf, new_pbm);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_RMOD_CFGr,
                                              REG_PORT_ANY, modid, val));
            /* XLMAC Module Header Set up */
            bcm_esw_port_encap_get(unit, local_port, &port_mode);
            /* Higig2 Mode */
            bcm_esw_stk_my_modid_get(unit, (int *)&src_mod);
            if (port_mode == BCM_PORT_ENCAP_HIGIG2) {
                /* default setting K.SOP = 0xfb, MCST = 0(unicast)*/
                hdr_0_hi = 0xfb000000;
                dest_mod = (remote_module & 0xff) << 8;
                hdr_0_hi = hdr_0_hi | dest_mod;
                hdr_0_lo = 0x00000000;
                src_mod = (src_mod & 0xff) << 24;
                src_port = (local_port & 0xff) << 16;
                hdr_0_lo = hdr_0_lo | src_mod | src_port;
            } else {
                /* Higig Mode */
                /* default setting K.SOP = 0xfb, HGI = 8, VID = 1 */
                hdr_0_hi = 0xfb800001;
                src_mod = (src_mod & 0x1f) << 27;
                dest_mod = remote_module & 0xff;
                /* default setting OPCODE = 1, COS = 7 */
                hdr_0_lo = 0x01000700 | src_mod | dest_mod;
            }

            if (IS_XL_PORT(unit, local_port)) {
                mac_type = E2efcModuleHdrXlmac;
            } else if (IS_CL_PORT(unit, local_port)) {
                mac_type = E2efcModuleHdrClmac;
            } else {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                          "Invalid Mac Type.\n")));
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN(
                soc_reg64_get(unit, e2efc_module_hdr_r[mac_type][hdr_0],
                              local_port, 0, &val_64));
            soc_reg64_field32_set(unit, e2efc_module_hdr_r[mac_type][hdr_0],
                                  &val_64, E2EFC_MODULE_HDR_0_LOf, hdr_0_lo);
            soc_reg64_field32_set(unit, e2efc_module_hdr_r[mac_type][hdr_0],
                                  &val_64, E2EFC_MODULE_HDR_0_HIf, hdr_0_hi);
            BCM_IF_ERROR_RETURN(
                soc_reg64_set(unit, e2efc_module_hdr_r[mac_type][hdr_0],
                              local_port, 0, val_64));

            if (port_mode == BCM_PORT_ENCAP_HIGIG2) {
                /* default setting vlan = 1, opcode = 1 */
                hdr_1_lo = 0x00010100;
                hdr_1_hi = 0x00000000;
            } else {
                hdr_1_lo = 0x00000000;
                hdr_1_hi = 0x00000000;
            }
            BCM_IF_ERROR_RETURN(
                soc_reg64_get(unit, e2efc_module_hdr_r[mac_type][hdr_1],
                              local_port, 0, &val_64));
            soc_reg64_field32_set(unit, e2efc_module_hdr_r[mac_type][hdr_1],
                                  &val_64, E2EFC_MODULE_HDR_1_LOf, hdr_1_lo);
            soc_reg64_field32_set(unit, e2efc_module_hdr_r[mac_type][hdr_1],
                                  &val_64, E2EFC_MODULE_HDR_1_HIf,
                                  hdr_1_hi);
            BCM_IF_ERROR_RETURN(
                soc_reg64_set(unit, e2efc_module_hdr_r[mac_type][hdr_1],
                              local_port, 0, val_64));
            /* In the absence of congestion, an E2EFC message is still
            * generated to refresh congestion status when the E2EFC max
            * timer expires. This timer is specified in units of 250ns or
            * 25us, and up to 1023 time units can be specified.
            * The selection of time unit and the number of time units
            * is computed as follows:
             *
             * Let E2EFC message refresh rate = N messages/sec, and N != 0,
             * A = # of 250ns time units = 1 sec / N / 250ns = 4000000 / N,
             * B = # of 25us time units = 1 sec / N / 25us =
             * 40000 / N = A / 100.
             *
             * If A < 1, configure the minimum allowed timer value:
             * time unit = 250ns, # of time units = 1.
             *
             * If A is between 1 and 1023, configure the timer as:
             * time unit = 250ns, # of time units = A.
             *
             * If A > 1023 and B <= 1023, configure the timer as:
             * time unit = 25us, # of time units = B.
             *
             * If B > 1023, configure the maximum allowed timer value:
             * time unit = 25us, # of time units = 1023.
             *
             * Note: If N = 0, # of time units will be set to 0, and E2EFC
             * message refresh is effectively disabled. E2ECC message
             * will be generated only when there is a change in congestion
             * status.
             */

            if (e2efc_rmod_cfg->pkt_per_second == 0) {
                time_unit_sel = 0;
                time_units = 0;
            } else {
                value_a = 4000000 / e2efc_rmod_cfg->pkt_per_second;
                value_b = value_a / 100;
                if (value_a < 1) {
                    time_unit_sel = 0;
                    time_units = 1;
                } else if (value_a <= 1023) {
                    time_unit_sel = 0;
                    time_units = value_a;
                } else if ((value_a > 1023) && (value_b <= 1023)) {
                    time_unit_sel = 1;
                    time_units = value_b;
                } else { /* value_b > 1023 */
                    time_unit_sel = 1;
                    time_units = 1023;
                }
            }
            /* E2EFC meesage trasmit period setup */
            /* For Min TX Timer */
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_HG_MIN_TX_TIMERr,
                                              REG_PORT_ANY, modid, &val_min));
            soc_reg_field_set(unit, E2EFC_HG_MIN_TX_TIMERr, &val_min,
                              LGf, time_unit_sel);
            soc_reg_field_set(unit, E2EFC_HG_MIN_TX_TIMERr, &val_min,
                              TIMERf, time_units);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_HG_MIN_TX_TIMERr,
                                              REG_PORT_ANY, modid, val_min));
            /* For Max TX Timer */
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_HG_MAX_TX_TIMERr,
                                              REG_PORT_ANY, modid, &val_max));
            soc_reg_field_set(unit, E2EFC_HG_MAX_TX_TIMERr, &val_max,
                              LGf, time_unit_sel);
            soc_reg_field_set(unit, E2EFC_HG_MAX_TX_TIMERr, &val_max,
                              TIMERf, time_units);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_HG_MAX_TX_TIMERr,
                                              REG_PORT_ANY, modid, val_max));
        } else {
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                        "E2EFC RX mode not allowed to configure timer"
                        " and XLMAC and HG port setup.\n")));
        }
    } else {
        /* Traverse 4 rmod id and clear the one that match rmod id */
        modid = -1;
        for (i = 0; i < GH2_E2EFC_RMOD_NUM; i++) {
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_r[mode][i],
                                              REG_PORT_ANY, 0, &val));
            if (remote_module == (soc_reg_field_get(
                                  unit, e2efc_rmod_id_r[mode][i], val,
                                  e2efc_rmod_id_f[i]))) {
                soc_reg_field_set(unit, e2efc_rmod_id_r[mode][i], &val,
                                  e2efc_rmod_id_f[i], remote_module_clear);
                soc_reg_field_set(unit, e2efc_rmod_id_r[mode][i], &val,
                                  e2efc_rmod_enable[i], 0);
                BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                                  e2efc_rmod_id_r[mode][i],
                                                  REG_PORT_ANY, 0, val));
                modid = i;
                break;
            }
        }
        if (modid == -1 ) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                      "E2EFC No Existing Remote.Module to Clear"
                      "Maximun 4 Remote Module only.\n")));
            return BCM_E_NOT_FOUND;
        }
    }

    return BCM_E_NONE;
}


/*
 * Description:
 *      Get the enable mode and configuration of the remote module that
 *      applies E2EFC mechanism.
 * Parameter:
 *      unit           - (IN) unit number
 *      remote_module  - (IN) remote module id
 *      mode           - (IN) selected mode
 *      enable         - (OUT) enable or disable selection
 *      e2efc_rmod_cfg - (OUT) structure containing the configuration.
 *                       Only valid if the remote module is enabled.
 */
int
bcmi_gh2_port_e2efc_remote_module_enable_get(
    int unit,
    bcm_module_t remote_module,
    bcm_port_e2efc_mode_t mode,
    int *enable,
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg)
{
    uint32 val, val_p, val_t;
    int i, time_unit_sel, time_units;
    int blk_num, pbm, phy_port = 0, log_port = 0;

    E2EFC_REMOTE_MODE_VALID(mode);

    /* Check the remote module id is in the valid range or not */
    if (!SOC_MODID_ADDRESSABLE(unit, remote_module)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid remote module ID.\n")));
        return BCM_E_BADID;
    }

    if (enable == NULL || e2efc_rmod_cfg == NULL) {
        return BCM_E_PARAM;
    }

    /* Check the logical port is in the right range  */
    if (!SOC_PORT_VALID(unit, e2efc_rmod_cfg->local_port)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid port range.\n")));
        return BCM_E_PARAM;
    }

    /* if the matched rmod id is not found then the "enable" is zero */
    *enable = 0;
    /*
     * Only Mode TX has the bcm_port_e2efc_remote_module_config_t
     * configuration, so if mode == RX, the get API will check whether the
     * remote_module exists and return enable value.
     */
    /* Traverse 4 rmod id and search the one that match rmod id */
    for (i = 0; i < GH2_E2EFC_RMOD_NUM; i++) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_r[mode][i],
                                          REG_PORT_ANY, 0, &val));
        if (remote_module == (soc_reg_field_get(unit,
                              e2efc_rmod_id_r[mode][i], val,
                              e2efc_rmod_id_f[i]))) {
            if (mode == bcmPortE2efcModeTx) {
                BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_RMOD_CFGr,
                                                  REG_PORT_ANY, i, &val_p));
                blk_num = soc_reg_field_get(unit, E2EFC_RMOD_CFGr, val_p,
                                            PORT_GRP_IDf);
                pbm = soc_reg_field_get(unit, E2EFC_RMOD_CFGr, val_p,
                                        TX_ENABLE_BMPf);
                GH2_E2EFC_PBM2BINDEX(pbm);
                phy_port = (blk_num * GH2_PORT_BLOCK_ENT_NUM) + pbm
                            + GH2_PORT_BLOCK_INIT_NUM;
                log_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
                e2efc_rmod_cfg->local_port = log_port;
                BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_HG_MIN_TX_TIMERr,
                                                  REG_PORT_ANY, i, &val_t));
                time_unit_sel =
                    soc_reg_field_get(unit, E2EFC_HG_MIN_TX_TIMERr,
                                      val_t, LGf);
                time_units =
                    soc_reg_field_get(unit, E2EFC_HG_MIN_TX_TIMERr,
                                      val_t, TIMERf);
                if (time_units == 0) {
                    e2efc_rmod_cfg->pkt_per_second = 0;
                } else if (time_unit_sel == 1) {
                    /* pkt_per_second = 1 / 25us / time_units */
                    e2efc_rmod_cfg->pkt_per_second = 40000 / time_units;
                } else {
                    /* pkt_per_second = 1 / 250ns / time_units */
                    e2efc_rmod_cfg->pkt_per_second = 4000000 / time_units;
                }
            }
            *enable = 1;
            break;
        }
    }
    return BCM_E_NONE;
}

/* helper function to search the remote module pointer */
int
bcmi_gh2_e2efc_remote_module_ptr_get(
    int unit,
    bcm_module_t remote_module,
    int *module_ptr)
{
    int i, ptr;
    uint32 val;
    bcm_port_e2efc_mode_t mode = bcmPortE2efcModeTx;

    if (module_ptr == NULL) {
        return BCM_E_PARAM;
    }
    /* Check the remote module id is in the valid range or not */
    if (!SOC_MODID_ADDRESSABLE(unit, remote_module)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid remote module ID.\n")));
        return BCM_E_BADID;
    }

    ptr = -1;
    for (i = 0; i < GH2_E2EFC_RMOD_NUM; i++) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_r[mode][i],
                                          REG_PORT_ANY, 0, &val));
        if (remote_module == (soc_reg_field_get(
                              unit, e2efc_rmod_id_r[mode][i], val,
                              e2efc_rmod_id_f[i]))) {
            ptr = i;
            break;
        }
    }
    if (ptr == -1) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Remote module ID not found.\n")));
        return BCM_E_NOT_FOUND;
    } else {
        *module_ptr = ptr;
    }
    return BCM_E_NONE;
}

/* helper function to search the remote module id */
int
bcmi_gh2_e2efc_remote_module_id_get(
    int unit,
    int module_ptr,
    bcm_module_t *remote_module)
{
    uint32 val;
    bcm_port_e2efc_mode_t mode = bcmPortE2efcModeTx;

    if (remote_module == NULL) {
        return BCM_E_PARAM;
    }

    if (module_ptr < 0 || module_ptr >= GH2_E2EFC_RMOD_NUM) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid remote module pointer. Should be 0~3.\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_r[mode][module_ptr],
                                      REG_PORT_ANY, 0, &val));
    /* the remote moduel is enabled */
    if (soc_reg_field_get(unit,
                          e2efc_rmod_id_r[mode][module_ptr], val,
                          e2efc_rmod_enable[module_ptr])) {
        *remote_module = (soc_reg_field_get(
                            unit, e2efc_rmod_id_r[mode][module_ptr], val,
                            e2efc_rmod_id_f[module_ptr]));
    } else {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/* helper function to init E2EFC remote port */
int
bcmi_gh2_port_e2efc_remote_port_init(int unit)
{
    int i;
    uint32 val;

    for (i = 0; i < GH2_E2EFC_REMOTE_PORT_NUM; i++) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, &val));
        soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val, Vf, 0);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, val));
    }

    return BCM_E_NONE;
}

/*
 * Description:
 *      Add remote module-port and set configuration.
 *      Return a handle id for the bcm_port_e2efc_remote_port_xxx APIs.
 *      The API of remote module-port configuration is only for E2EFC
 *      transmission.
 * Parameter:
 *      unit            - (IN) unit number
 *      e2efc_rport_cfg - (IN) structure containing the configuration
 *      rport_handle_id - (OUT) remote port handle id, to be used for the rest
 *                        of bcm_port_e2efc_remote_port_xxx APIs
 */
int
bcmi_gh2_port_e2efc_remote_port_add(
    int unit,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg,
    int *rport_handle_id)
{
    uint32 val_p, val_off, val_on, val_drop;
    int i, enable, rmod_ptr;

    if (e2efc_rport_cfg == NULL || rport_handle_id == NULL) {
        return BCM_E_PARAM;
    }

    /* Check the logical port is in the right range  */
    if (!SOC_PORT_VALID(unit, e2efc_rport_cfg->remote_port)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid port range.\n")));
        return BCM_E_PARAM;
    }

    if ((e2efc_rport_cfg->xoff_threshold_packets < 0) ||
        (e2efc_rport_cfg->xoff_threshold_bytes < 0) ||
        (e2efc_rport_cfg->xon_threshold_packets < 0) ||
        (e2efc_rport_cfg->xon_threshold_bytes < 0) ||
        (e2efc_rport_cfg->drop_threshold_packets < 0) ||
        (e2efc_rport_cfg->drop_threshold_bytes < 0)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid threshhold value set, should not less than 0.\n")));
        return BCM_E_PARAM;
    }

    /* Checking if the duplicated Counter is being set */
    for (i = 0; i < GH2_E2EFC_REMOTE_PORT_NUM; i++) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, &val_p));
        if ((soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val_p, Vf))) {
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_e2efc_remote_module_ptr_get(
                    unit, e2efc_rport_cfg->remote_module, &rmod_ptr));
            if (e2efc_rport_cfg->remote_port ==
                    (soc_reg_field_get(unit, E2EFC_CNT_ATTRr,
                                       val_p, RMT_SRC_PORT_IDf))&&
                    rmod_ptr == (soc_reg_field_get(
                                unit, E2EFC_CNT_ATTRr, val_p,
                                RMOD_PTRf))) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                          "Each remote source port should be"
                          "mapped to only one counter.\n")));
                return BCM_E_FULL;
            }
        }
    }

    enable = 0;
    for (i = 0; i < GH2_E2EFC_REMOTE_PORT_NUM; i++) {
        /* check whether the E2EFC_CNT_ATTRr Vf is enabled */
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, &val_p));
        if (!(soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val_p, Vf))) {
            /* Setting remote port, module id and enable counter */
            enable = 1;
            soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p, Vf, enable);
            BCM_IF_ERROR_RETURN(
            bcmi_gh2_e2efc_remote_module_ptr_get(
                unit, e2efc_rport_cfg->remote_module, &rmod_ptr));
            soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                              RMOD_PTRf, rmod_ptr);

            soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                              RMT_SRC_PORT_IDf, e2efc_rport_cfg->remote_port);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_ATTRr,
                                              REG_PORT_ANY, i, val_p));
            /* Setting xoff theshhold */
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_SET_LIMITr,
                                              REG_PORT_ANY, i, &val_off));
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &val_off,
                              PKT_SET_LIMITf,
                              e2efc_rport_cfg->xoff_threshold_packets);
            soc_reg_field_set(
                unit, E2EFC_CNT_SET_LIMITr, &val_off, CELL_SET_LIMITf,
                GH2_BYTE2CELL(e2efc_rport_cfg->xoff_threshold_bytes));
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_SET_LIMITr,
                                              REG_PORT_ANY, i, val_off));
            /* Setting xon theshhold */
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                              E2EFC_CNT_RESET_LIMITr,
                                              REG_PORT_ANY, i, &val_on));
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &val_on,
                              PKT_RESET_LIMITf,
                              e2efc_rport_cfg->xon_threshold_packets);
            soc_reg_field_set(
                unit, E2EFC_CNT_RESET_LIMITr, &val_on, CELL_RESET_LIMITf,
                GH2_BYTE2CELL(e2efc_rport_cfg->xon_threshold_bytes));
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_RESET_LIMITr,
                                              REG_PORT_ANY, i, val_on));
            /* Setting drop theshhold */
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                              E2EFC_CNT_DISC_LIMITr,
                                              REG_PORT_ANY, i, &val_drop));
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &val_drop,
                              PKT_DISC_LIMITf,
                              e2efc_rport_cfg->drop_threshold_packets);
            soc_reg_field_set(
                unit, E2EFC_CNT_DISC_LIMITr, &val_drop, CELL_DISC_LIMITf,
                GH2_BYTE2CELL(e2efc_rport_cfg->drop_threshold_bytes));
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_DISC_LIMITr,
                                              REG_PORT_ANY, i, val_drop));
            break;
        }
    }
    if (enable == 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                  "Can't Port Config Because the 48 entries is all full")));
        return BCM_E_FULL;
    }
    *rport_handle_id = i;

    return BCM_E_NONE;
}

/*
 * Description:
 *      Set (modify) configuration of the remote port.
 * Parameter:
 *      unit            - (IN) unit number
 *      rport_handle_id - (IN) remote port handle id
 *      e2efc_rport_cfg - (IN) structure containing the configuration
 */
int
bcmi_gh2_port_e2efc_remote_port_set(
    int unit,
    int rport_handle_id,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg)
{
    uint32 val_p, val_off, val_on, val_drop;
    int rmod_ptr;

    /* check whehter the handle id is valid */
    if ( rport_handle_id < 0 || rport_handle_id >= GH2_E2EFC_REMOTE_PORT_NUM) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Hadle ID.\n")));
        return BCM_E_PARAM;
    }

    if (e2efc_rport_cfg == NULL) {
        return BCM_E_PARAM;
    }

    /* Check the logical port is in the right range  */
    if (!SOC_PORT_VALID(unit, e2efc_rport_cfg->remote_port)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid port range.\n")));
        return BCM_E_PARAM;
    }

    if ((e2efc_rport_cfg->xoff_threshold_packets < 0) ||
        (e2efc_rport_cfg->xoff_threshold_bytes < 0) ||
        (e2efc_rport_cfg->xon_threshold_packets < 0) ||
        (e2efc_rport_cfg->xon_threshold_bytes < 0) ||
        (e2efc_rport_cfg->drop_threshold_packets < 0) ||
        (e2efc_rport_cfg->drop_threshold_bytes < 0)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid threshhold value set, should not less than 0.\n")));
        return BCM_E_PARAM;
    }
    /* check whether the parameter is added or not */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      E2EFC_CNT_ATTRr,
                                      REG_PORT_ANY, rport_handle_id, &val_p));
    if (!(soc_reg_field_get(unit, E2EFC_CNT_ATTRr,
                            val_p, Vf))) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "The port config is not added, thus can't set")));
        return BCM_E_PARAM;
    }

    /* Setting remote port and module id */
    BCM_IF_ERROR_RETURN(
    bcmi_gh2_e2efc_remote_module_ptr_get(
        unit, e2efc_rport_cfg->remote_module, &rmod_ptr));
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p, RMOD_PTRf, rmod_ptr);
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                      RMT_SRC_PORT_IDf, e2efc_rport_cfg->remote_port);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_ATTRr, REG_PORT_ANY,
                                      rport_handle_id, val_p));
    /* Setting xoff theshhold */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      E2EFC_CNT_SET_LIMITr, REG_PORT_ANY,
                                      rport_handle_id, &val_off));
    soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &val_off,
                      PKT_SET_LIMITf,
                      e2efc_rport_cfg->xoff_threshold_packets);
    soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &val_off,
                      CELL_SET_LIMITf,
                      GH2_BYTE2CELL(e2efc_rport_cfg->xoff_threshold_bytes));
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_SET_LIMITr,
                                      REG_PORT_ANY, rport_handle_id, val_off));
    /* Setting xon theshhold */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      E2EFC_CNT_RESET_LIMITr,
                                      REG_PORT_ANY, rport_handle_id, &val_on));
    soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr,
                      &val_on, PKT_RESET_LIMITf,
                      e2efc_rport_cfg->xon_threshold_packets);
    soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr,
                      &val_on, CELL_RESET_LIMITf,
                      GH2_BYTE2CELL(e2efc_rport_cfg->xon_threshold_bytes));
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_RESET_LIMITr,
                                      REG_PORT_ANY, rport_handle_id, val_on));
    /* Setting drop theshhold */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      E2EFC_CNT_DISC_LIMITr, REG_PORT_ANY,
                                      rport_handle_id, &val_drop));
    soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr,
                      &val_drop, PKT_DISC_LIMITf,
                      e2efc_rport_cfg->drop_threshold_packets);
    soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr,
                      &val_drop, CELL_DISC_LIMITf,
                      GH2_BYTE2CELL(e2efc_rport_cfg->drop_threshold_bytes));
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_DISC_LIMITr,
                                      REG_PORT_ANY, rport_handle_id, val_drop));

    return BCM_E_NONE;
}


/*
 * Description:
 *      Get configuration of the remote port.
 *      Return the remote module, remote port and configuration of the
 *      handle id.
 * Parameter:
 *      unit            - (IN) unit number
 *      rport_handle_id - (IN) remote port handle id
 *      e2efc_rport_cfg - (OUT) structure containing the configuration
 */
int
bcmi_gh2_port_e2efc_remote_port_get(
    int unit,
    int rport_handle_id,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg)
{
    uint32 val_p, val_off, val_on, val_drop;
    int rmod_ptr, rmod_id;
    int xoff_threshold_cell, xon_threshold_cell, drop_threshold_cell;

    if (e2efc_rport_cfg == NULL) {
        return BCM_E_PARAM;
    }

    /* check whehter the handle id is valid */
    if ( rport_handle_id < 0 || rport_handle_id >= GH2_E2EFC_REMOTE_PORT_NUM) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Hadle ID.\n")));
        return BCM_E_PARAM;
    }
    /* check whether the parameter is added or not */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      E2EFC_CNT_ATTRr,
                                      REG_PORT_ANY, rport_handle_id, &val_p));
    if (!(soc_reg_field_get(unit, E2EFC_CNT_ATTRr,
                            val_p, Vf))) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                  "The port config is not added, thus can't get")));
        return BCM_E_NOT_FOUND;
    }

    /* Getting remote port and module id */
    rmod_ptr = soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val_p, RMOD_PTRf);
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_e2efc_remote_module_id_get(unit, rmod_ptr, &rmod_id));
    e2efc_rport_cfg->remote_module = rmod_id;
    e2efc_rport_cfg->remote_port =
    soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val_p,
                      RMT_SRC_PORT_IDf);
    /* Getting xoff theshhold */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_SET_LIMITr,
                                 REG_PORT_ANY, rport_handle_id, &val_off));
    e2efc_rport_cfg->xoff_threshold_packets =
        soc_reg_field_get(unit, E2EFC_CNT_SET_LIMITr, val_off,
                          PKT_SET_LIMITf);
    xoff_threshold_cell = soc_reg_field_get(unit, E2EFC_CNT_SET_LIMITr,
                                            val_off, CELL_SET_LIMITf);
    e2efc_rport_cfg->xoff_threshold_bytes = GH2_CELL2BYTE(xoff_threshold_cell);
    /* Getting xon theshhold */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_RESET_LIMITr,
                                      REG_PORT_ANY, rport_handle_id, &val_on));
    e2efc_rport_cfg->xon_threshold_packets =
        soc_reg_field_get(unit, E2EFC_CNT_RESET_LIMITr,
                          val_on, PKT_RESET_LIMITf);
    xon_threshold_cell = soc_reg_field_get(unit, E2EFC_CNT_RESET_LIMITr,
                                           val_on, CELL_RESET_LIMITf);
    e2efc_rport_cfg->xon_threshold_bytes = GH2_CELL2BYTE(xon_threshold_cell);
    /* Getting drop theshhold */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_DISC_LIMITr,
                                      REG_PORT_ANY,
                                      rport_handle_id, &val_drop));
    e2efc_rport_cfg->drop_threshold_packets =
        soc_reg_field_get(unit, E2EFC_CNT_DISC_LIMITr,
                          val_drop, PKT_DISC_LIMITf);
    drop_threshold_cell = soc_reg_field_get(unit, E2EFC_CNT_DISC_LIMITr,
                                            val_drop, CELL_DISC_LIMITf);
    e2efc_rport_cfg->drop_threshold_bytes = GH2_CELL2BYTE(drop_threshold_cell);

    return BCM_E_NONE;
}


/*
 * Description:
 *      Delete remote port.
 * Parameter:
 *      unit            - (IN) unit number
 *      rport_handle_id - (IN) remote port handle id
 */
int
bcmi_gh2_port_e2efc_remote_port_delete(
    int unit,
    int rport_handle_id)
{
    uint32 val_p, val_off, val_on, val_drop;
    uint32 clear = 0x00000000;

    /* check whehter the handle id is valid */
    if ( rport_handle_id < 0 || rport_handle_id >= GH2_E2EFC_REMOTE_PORT_NUM) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Hadle ID.\n")));
        return BCM_E_PARAM;
    }
    /* check whether the parameter is added or not */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                 REG_PORT_ANY, rport_handle_id, &val_p));
    if (!(soc_reg_field_get(unit, E2EFC_CNT_ATTRr,
                            val_p, Vf))) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                  "The port config is not added, thus can't delete")));
        return BCM_E_NOT_FOUND;
    }

    /* Set to zero remote port and module id */
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                      Vf, clear);
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                      RMOD_PTRf, clear);
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                      RMT_SRC_PORT_IDf, clear);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_ATTRr,
                                      REG_PORT_ANY, rport_handle_id, val_p));
    /* Set to zero xoff theshhold */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_SET_LIMITr,
                                 REG_PORT_ANY, rport_handle_id, &val_off));
    soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &val_off,
                      PKT_SET_LIMITf, clear);
    soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &val_off,
                      CELL_SET_LIMITf, clear);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_SET_LIMITr,
                                  REG_PORT_ANY, rport_handle_id, val_off));
    /* Set to zero xon theshhold */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_RESET_LIMITr,
                                 REG_PORT_ANY, rport_handle_id, &val_on));
    soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr,
                      &val_on, PKT_RESET_LIMITf, clear);
    soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr,
                      &val_on, CELL_RESET_LIMITf, clear);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_RESET_LIMITr,
                                      REG_PORT_ANY, rport_handle_id, val_on));
    /* Set to zero drop theshhold */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_DISC_LIMITr,
                                      REG_PORT_ANY, rport_handle_id,
                                      &val_drop));
    soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr,
                      &val_drop, PKT_DISC_LIMITf, clear);
    soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr,
                      &val_drop, CELL_DISC_LIMITf, clear);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_DISC_LIMITr,
                                      REG_PORT_ANY, rport_handle_id,
                                      val_drop));

    return BCM_E_NONE;
}
/*
 * Description:
 *      Traverse enabled remote modules that apply E2EFC mechanism.
 * Parameter:
 *      unit      - (IN) unit number
 *      cb        - (IN) call back function to execute
 *      user_data - (IN) pointer to user data
*/
int
bcmi_gh2_port_e2efc_remote_module_traverse(
    int unit,
    bcm_port_e2efc_remote_module_traverse_cb cb,
    void *user_data)
{
    int i, rv, rmod_id;
    bcm_port_e2efc_mode_t mode;
    uint32 val;

    if (user_data == NULL) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < GH2_E2EFC_RMOD_NUM; i++) {
        for (mode = bcmPortE2efcModeTx; mode < bcmPortE2efcModeCount; mode++) {
            soc_reg32_get(unit, e2efc_rmod_id_r[mode][i],
                          REG_PORT_ANY, 0, &val);
            if (soc_reg_field_get(unit, e2efc_rmod_id_r[mode][i],
                                  val, e2efc_rmod_enable[i])) {
                rmod_id = soc_reg_field_get(unit, e2efc_rmod_id_r[mode][i],
                                            val, e2efc_rmod_id_f[i]);
                rv = cb(unit, rmod_id, mode, user_data);
                if (rv != BCM_E_NONE) {
                    return rv;
                }
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Description:
 *      Traverse remote ports.
 * Parameter:
 *      unit      - (IN) unit number
 *      cb        - (IN) call back function to execute
 *      user_data - (IN) pointer to user data
 */
int
bcmi_gh2_port_e2efc_remote_port_traverse(
    int unit,
    bcm_port_e2efc_remote_port_traverse_cb cb,
    void *user_data)
{
    int i, rv;
    uint32 val;

    if (user_data == NULL) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < GH2_E2EFC_REMOTE_PORT_NUM; i++) {
        /* check whether the E2EFC_CNT_ATTRr Vf is enabled */
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, &val));
        if (soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val, Vf)) {
            rv = cb(unit, i, user_data);
            if (rv != BCM_E_NONE) {
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}

#define BCMI_GH2_MAX_PHY_PORTS  (90)

#if defined(BCM_FIRELIGHT_SUPPORT)
#define BCMI_FL_MAX_PHY_PORTS  (78)
#endif

/*
 * Description:
 *      Force MAC loopback for Riot loopback port.
 * Parameter:
 *      unit      - (IN) unit number
 */
int
bcmi_gh2_port_force_lb_set(int unit)
{
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    soc_info_t *si = &SOC_INFO(unit);
    int pport, port, loopback, pport_num = BCMI_GH2_MAX_PHY_PORTS;

    if (SOC_WARM_BOOT(unit)) {
        return BCM_E_NONE;
    }

    if (!soc_feature(unit, soc_feature_untethered_otp)) {
        return SOC_E_NONE;
    }

#if defined(BCM_FIRELIGHT_SUPPORT)
    if (soc_feature(unit, soc_feature_fl)) {
        pport_num = BCMI_FL_MAX_PHY_PORTS;
    }
#endif

    /* Check Riot lb port */
    for (pport = 0; pport < pport_num; pport++) {
        if (SHR_BITGET(bond_info->tsc_in_loop, pport)) {
            port = si->port_p2l_mapping[pport];
            if (port == -1) {
                continue;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_loopback_get(unit, port, &loopback));
            if (loopback == BCM_PORT_LOOPBACK_NONE) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_loopback_set(
                        unit, port, BCM_PORT_LOOPBACK_MAC));
                /* Enable MAC credit generation from TSC */
                BCM_IF_ERROR_RETURN(
                    soc_phyctrl_control_set(
                        unit, port, SOC_PHY_CONTROL_BOND_IN_PWRDN_OVERRIDE, 1));
                LOG_DEBUG(BSL_LS_BCM_PORT,
                          (BSL_META_UP(unit, port,
                                       "Set port as RIOT LB port\n")));
            } else if (loopback != BCM_PORT_LOOPBACK_MAC) {
                LOG_WARN(BSL_LS_BCM_PORT,
                         (BSL_META_UP(unit, port,
                                     "port should be in LB NONE/MAC.\n")));
                return BCM_E_INTERNAL;
            }
        }
    }
    return BCM_E_NONE;
}

#endif /* BCM_GREYHOUND2_SUPPORT */
