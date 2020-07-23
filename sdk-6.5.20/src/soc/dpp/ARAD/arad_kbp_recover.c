/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0) && defined(INCLUDE_KBP)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM


#include <soc/dpp/SAND/Utils/sand_header.h>
#include <shared/bsl.h>
#include <shared/util.h>
#include <bcm/link.h>
#include <soc/mem.h>
#include <soc/phyreg.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_kbp_xpt.h>
#include <soc/dpp/ARAD/arad_kbp_recover.h>

#include <soc/dpp/mbcm.h>



#define ARAD_KBP_RECOVER_NOF_ROP_TESTS_ILKN12   5
#define ARAD_KBP_PORT_MAX_LANES                 12









extern uint32 arad_kbp_blk_lut_set(int unit, uint32 core);

STATIC int arad_kbp_recover_mdio_regs_print(int unit, int mdio_id);


int arad_kbp_recover_rx_enable(int unit, int mdio_id)
{
    int i = 0;
    uint32 flags = SOC_PHY_NOMAP | SOC_PHY_CLAUSE45;
    uint32 kbp_reg_addr, data;
    uint32 port = mdio_id;
    SOCDNX_INIT_FUNC_DEFS;

	kbp_reg_addr =  0X40000;

	data = 0;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    
	SHR_BITSET(&data,0);
	SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, port, flags, kbp_reg_addr, data));
    
    kbp_reg_addr = 0x40021;
	
	while (data != 0x8000 && i < 1000){
		data = 0;
		SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        ++i;
 	}
	
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_kbp_recover_rx_shut_down(int unit, int mdio_id)
{
    uint32 flags = SOC_PHY_NOMAP | SOC_PHY_CLAUSE45;
    uint32 kbp_reg_addr, data;
    uint32 port = mdio_id;
    SOCDNX_INIT_FUNC_DEFS;

    kbp_reg_addr =  0x40000;

	data = 0;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    
	SHR_BITCLR(&data ,0);
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, port, flags, kbp_reg_addr, data));
   
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_kbp_recover_txpcs_soft_reset_toggle(int unit, int core, int mdio_id)
{
    uint32_t flags = SOC_PHY_NOMAP | SOC_PHY_CLAUSE45;
    uint32_t data;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    int kbp_port_id = 0;
    SOCDNX_INIT_FUNC_DEFS;

    kbp_port_id = (elk->kbp_port_core_mapping[kbp_port_id] == core) ? kbp_port_id : kbp_port_id + 1;
 
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, mdio_id, flags, 0x02F00F, &data));
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F00F, data & ~(1 << kbp_port_id)));
    sal_usleep(200000);
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F00F, data));
    
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, mdio_id, flags, 0x02F00F, &data));
    sal_usleep(200000);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_kbp_op_phmf_fifo_reset(int unit, int core, int mdio_id)
{
    uint32_t flags = SOC_PHY_NOMAP | SOC_PHY_CLAUSE45;
    uint32_t data;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    int kbp_port_id = 0;
    SOCDNX_INIT_FUNC_DEFS;

    kbp_port_id = (elk->kbp_port_core_mapping[kbp_port_id] == core) ? kbp_port_id : kbp_port_id + 1;

    if (kbp_port_id) {
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F164, 0x3037));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F165, 0x3037));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F166, 0x3037));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F167, 0x3037));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F168, 0x3037));
        sal_usleep(200000);
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F164, 0x3035));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F165, 0x3035));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F166, 0x3035));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F167, 0x3035));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F168, 0x3035));
    } else {
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F160, 0x3037));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F161, 0x3037));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F162, 0x3037));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F163, 0x3037));
        sal_usleep(200000);
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F160, 0x3035));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F161, 0x3035));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F162, 0x3035));
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, mdio_id, flags, 0x02F163, 0x3035));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, mdio_id, flags, 0x02F00F, &data));

    sal_usleep(400000);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_kbp_op_link_stability_check(int unit, uint32 core, uint32 kbp_mdio_id, uint32 retries)
{
    soc_port_t kbp_port;
    int i, j, lane, ls_interval;
    int ilkn_rate, status, latch_down;
    uint32 eye_margin, ilkn_metaframe, kbp_nof_lanes = 0;
    uint32 hz_l[ARAD_KBP_PORT_MAX_LANES] = {0}, hz_r[ARAD_KBP_PORT_MAX_LANES] = {0};
    uint32 vt_u[ARAD_KBP_PORT_MAX_LANES] = {0}, vt_d[ARAD_KBP_PORT_MAX_LANES] = {0};
    uint32 hz_l_avg, hz_r_avg, vt_u_avg, vt_d_avg;
    uint8 toggle_txpcs = 0, dead_lane = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_kbp_ilkn_interface_param_get(unit, core, &kbp_port, &kbp_nof_lanes, &ilkn_rate, &ilkn_metaframe));

    SOCDNX_IF_ERR_EXIT(bcm_linkscan_enable_get(unit, &ls_interval));

    if (!ls_interval) {
        
        SOCDNX_IF_ERR_EXIT(arad_kbp_recover_txpcs_soft_reset_toggle(unit, core, kbp_mdio_id));
    }

    for (i = 0; i < retries; ++i) {
        
        hz_l_avg = 0;
        hz_r_avg = 0;
        vt_u_avg = 0;
        vt_d_avg = 0;
        toggle_txpcs = 0;
        dead_lane = 0;

        for (lane = 0; lane < kbp_nof_lanes; lane++) {
            
            SOCDNX_IF_ERR_EXIT(
                MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_control_get,
                                         (unit, kbp_port, -1, lane, 0, SOC_PHY_CONTROL_EYE_VAL_HZ_L, &eye_margin)));
            hz_l[lane] = eye_margin;
            hz_l_avg += eye_margin;
            
            SOCDNX_IF_ERR_EXIT(
                MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_control_get,
                                         (unit, kbp_port, -1, lane, 0, SOC_PHY_CONTROL_EYE_VAL_HZ_R, &eye_margin)));
            hz_r[lane] = eye_margin;
            hz_r_avg += eye_margin;
            
            SOCDNX_IF_ERR_EXIT(
                MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_control_get,
                                         (unit, kbp_port, -1, lane, 0, SOC_PHY_CONTROL_EYE_VAL_VT_U, &eye_margin)));
            vt_u[lane] = eye_margin;
            vt_u_avg += eye_margin;
            
            SOCDNX_IF_ERR_EXIT(
                MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_phy_control_get,
                                         (unit, kbp_port, -1, lane, 0, SOC_PHY_CONTROL_EYE_VAL_VT_D, &eye_margin)));
            vt_d[lane] = eye_margin;
            vt_d_avg += eye_margin;

            if (!hz_l[lane] || !hz_r[lane] || !vt_u[lane] || !vt_d[lane]) {
                toggle_txpcs = 1;
                dead_lane = 1;
                break;
            }
            LOG_DEBUG(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                 "arad_kbp_op_link_stability_check: port %d, lane %d, HZ_L %d, HZ_R %d, VT_U %d, VT_D %d, nof_lanes %d\n"),
                                 kbp_port, lane, hz_l[lane], hz_r[lane], vt_u[lane], vt_d[lane], kbp_nof_lanes));
        }
        if (!dead_lane) {
            if (!kbp_nof_lanes) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("The nof lanes for the ilkn port is 0!\n")));
            }
            
            hz_l_avg = (hz_l_avg / kbp_nof_lanes) * 4/5;
            hz_r_avg = (hz_r_avg / kbp_nof_lanes) * 4/5;
            vt_u_avg = (vt_u_avg / kbp_nof_lanes) * 4/5;
            vt_d_avg = (vt_d_avg / kbp_nof_lanes) * 4/5;
            
            for (lane = 0; lane < kbp_nof_lanes; lane++) {
                if ((hz_l[lane] < hz_l_avg) || (hz_r[lane] < hz_r_avg) ||
                    (vt_u[lane] < vt_u_avg) || (vt_d[lane] < vt_d_avg)) {
                    toggle_txpcs = 1;
                }
            }
        }

        if (i < retries - 1) {
            
            if (dead_lane) {
                SOCDNX_IF_ERR_EXIT(arad_kbp_op_phmf_fifo_reset(unit, core, kbp_mdio_id));
                LOG_INFO(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "arad_kbp_op_link_stability_check link recovery iteration %d/%d failed for dead lane, trying again.\n"),
                                     i + 1, retries));
            }
            
            if (toggle_txpcs) {
                SOCDNX_IF_ERR_EXIT(arad_kbp_recover_txpcs_soft_reset_toggle(unit, core, kbp_mdio_id));
                LOG_INFO(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "arad_kbp_op_link_stability_check link recovery iteration %d/%d failed for small eye, trying again.\n"),
                                     i + 1, retries));
            } else {
                
                for (j = 0; j < 100; j++) {
                    sal_usleep(10000);
                    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_link_state_get, 
                                                            (unit, kbp_port, 0, &status, &latch_down)));
                    if (status == 1) {
                        SOC_EXIT;
                    } else {
                        LOG_INFO(BSL_LS_SOC_TCAM,
                                 (BSL_META_U(unit,
                                             "arad_kbp_op_link_stability_check link is NOT up for port %d, trying again.\n"), kbp_port));
                    }
                }
                if (j == 100) {
                    SOCDNX_IF_ERR_EXIT(arad_kbp_recover_txpcs_soft_reset_toggle(unit, core, kbp_mdio_id));
                    LOG_INFO(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                        "arad_kbp_op_link_stability_check link not up after 1 sec for port %d.\n"), kbp_port));
                }
            }
        } else {
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Eyescan Margin is too small for KBP port %d, ROP transmission may fail!\n"), kbp_port));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_kbp_op_recover_txpcs_soft_reset(int unit, int core, soc_port_t port, int mdio_id)
{
    int status, latch_down;
    int result;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_kbp_op_phmf_fifo_reset(unit, core, mdio_id));

    for (i = 0; i < 100; i++) {
        sal_usleep(10000);
        result = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_link_state_get,
                                       (unit, port, 0, &status, &latch_down)));
        if (SOC_FAILURE(result)) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                     "Error arad_kbp_op_recover_txpcs_soft_reset, bcm_port_link_status_get unit %d port %d core %d\n"), unit, port, core));
        }
        if (status == 1) {
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "LINK UP AFTER %d wait iterations for port %d\n"), i + 1, port));
            break;
        }
    }
    if (i == 100) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                             "LINK NOT UP AFTER 1 sec for port %d\n"), port));
    }
exit:
    SOCDNX_FUNC_RETURN;
}



int arad_kbp_recover_run_recovery_sequence(
    int unit,
    uint32 core,
    ARAD_NIF_ELK_USER_DATA user_data[SOC_DPP_DEFS_MAX(NOF_CORES)],
    uint32 retries,
    void *buf,
    int option)
{
    int i, j, k, rv, nof_rop_tests, kbp_nof_lanes = 0;
    uint32 reg_val, flags;
    soc_port_t port, kbp_port = -1;
    soc_pbmp_t ilkn_pbmp, kbp_phys;
    int tmp_core;
    soc_reg_above_64_val_t data;
    int is_broadcast = (core == 2) ? 1 : 0;
    int core_iter = (core == 2) ? 2 : 1;
    int mdio_id, core_id;
    uint32 field_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (!is_broadcast) {
        ilkn_pbmp = PBMP_IL_ALL(unit);
        SOC_PBMP_ITER(ilkn_pbmp, port) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &tmp_core));
            if (SOC_PORT_IS_ELK_INTERFACE(flags) && (core == tmp_core)) {
                kbp_port = port;
                break;
            }
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, kbp_port, &kbp_phys));
        SOC_PBMP_COUNT(kbp_phys, kbp_nof_lanes);
    }

    for (i = 0; i < retries; ++i) {
        for (k = 0; k < core_iter; k ++) {
            mdio_id = is_broadcast ? user_data[k].kbp_mdio_id: user_data[core].kbp_mdio_id;
            if (ARAD_KBP_IS_OP_OR_OP2) {
                SOCDNX_IF_ERR_EXIT(arad_kbp_op_recover_txpcs_soft_reset(unit, core, kbp_port, mdio_id));
                sal_usleep(100000);
            } else {
                SOCDNX_IF_ERR_EXIT(arad_kbp_recover_rx_shut_down(unit, mdio_id));
                sal_usleep(10000);
                SOCDNX_IF_ERR_EXIT(arad_kbp_recover_rx_enable(unit, mdio_id));
                if (bsl_fast_check(BSL_LS_SOC_TCAM | BSL_DEBUG)) {
                    SOCDNX_IF_ERR_EXIT(arad_kbp_recover_mdio_regs_print(unit, mdio_id));
                }
            }
            for (j = 0; j < retries; ++j) {
                if (SOC_IS_JERICHO(unit)) {
                    core_id = is_broadcast ? k : core;
                    arad_kbp_cpu_lookup_reply(unit, core_id, data);
                    field_val = soc_reg_above_64_field32_get(unit, IHB_LOOKUP_REPLYr, data, LOOKUP_REPLY_VALIDf);
                    if (field_val == 0) {
                        break;
                    }
                } else {
                    SOCDNX_IF_ERR_EXIT(READ_IHB_LOOKUP_REPLYr_REG32(unit, core, &reg_val));
                    if (reg_val == 0) {
                        break;
                    }
                }
            }
        }
        
        if (option == 1) {
            nof_rop_tests = (kbp_nof_lanes == 12) ? ARAD_KBP_RECOVER_NOF_ROP_TESTS_ILKN12 : 1;
            rv = 0;
            for (k = 0; k < nof_rop_tests; ++k) {
                
                
                rv += arad_kbp_init_rop_test(unit, core); 
            }
        } else if (option == 2) {
            
            
            rv = arad_kbp_blk_lut_set(unit, core);
        } else if (option == 3) {
            
            
            rv = arad_kbp_rop_write(unit, core, (arad_kbp_rop_write_t *)buf);
        } else if (option == 4) {
            
            
            rv = arad_kbp_rop_read(unit, core, (arad_kbp_rop_read_t *)buf);
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("KBP recover doesn't support option %d"), option));
        }
        if(rv == SOC_E_NONE){
            
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "KBP recovery succeeded within %d iterations for option %d.\n"), i + 1, option));
            break;
        } else {
            if (i < retries - 1) {
                LOG_INFO(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "KBP recovery iteration %d/%d failed, trying again for option %d.\n"), i + 1, retries, option));
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL,(_BSL_SOC_MSG("KBP recovery failed.\n")));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC 
int arad_kbp_recover_mdio_regs_print(int unit, int mdio_id){
    int i;
    uint32 flags, kbp_reg_addr, data;
    uint32 port = mdio_id;
    SOCDNX_INIT_FUNC_DEFS;

    flags = BCM_PORT_PHY_NOMAP | BCM_PORT_PHY_CLAUSE45;
    kbp_reg_addr = 0x80006;

    
    data = 0x1111;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x\n"), kbp_reg_addr, data));
    data += 1;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, port, flags, kbp_reg_addr, data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> write\n"), kbp_reg_addr, data));
    data = 0x1111;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40000;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Link Control [15:0] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40001;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Link Control [31:16] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40010;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Transmit Lane Enable [15:0].\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40011;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Transmit Lane Enable [31:16].\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40012;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Receive Lane Enable [15:0].\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40013;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Receive Lane Enable [31:16].\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4001a;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Receive Elastic FIFO Parity Error [15:0] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4001b;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Receive Elastic FIFO Parity Error [31:16] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40021;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> link up (0x8000).\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40022;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Link Error Summary [15:0] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40023;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Link Error Summary [31:16] Register\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40024;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Receive Lane Error [15:0] Register\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x8049C;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> ROP Error Status [15:0] Register\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x8049D;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> ROP Error Status [31:16] Register\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40238;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Packet Header [15:0] Register\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40239;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Packet Header [31:16] Register\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4023A;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Packet Header [47:32] Register\n)"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4023B;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Packet Header [63:48] Register\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4023C;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Packet Header [79:64] Register\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4023D;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Packet Header [95:80] Register\n"), kbp_reg_addr, data));

    for (i = 0 ; i < 4; ++i)
    {
        kbp_reg_addr =  0x40200 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 0 [15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40204 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 1 [15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40204 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 1 [15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40208 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 2[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x4020c + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 3[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40210 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 4[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40214 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 5[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40218 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 6[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x4021C + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 7[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40220 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 8[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40224 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 9[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40228 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 10[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x4022C + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 11[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40230 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 12[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40234 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Data Word 13[15:0] Register\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40040 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> recive counters\n"), kbp_reg_addr, data));

        kbp_reg_addr =  0x40044 + i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> transmit counters\n"), kbp_reg_addr, data));
    }

    kbp_reg_addr =  0x40100;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Word Align Done [15:0]  Register - (12 lanes - 0xfff)\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40101;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Word Align Done [31:16] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40102;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Scrambler Synchronization [15:0]  Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40103;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Scrambler Synchronization [31:16] Register\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40104;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Elastic FIFO Full [15:0]  Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40105;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Elastic FIFO Full [31:16] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40106;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Word Align Fail [15:0]  Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40107;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Word Align Fail [31:16] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40108;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Illegal Framing Bit [15:0]  Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40109;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Illegal Framing Bit [31:16] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4010c;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Scrambler Word Error [15:0]  Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4010d;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Scrambler Word Error [31:16] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4010e;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Skip Word Error [15:0]  Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x4010f;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  Receive PCS Skip Word Error [31:16] Register.\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40112;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Receive PCS Metaframe Length Error [15:0]\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x40112;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> Receive PCS Metaframe Length Error [31:16]\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x10000 | (0x1 << 3) | 0x6;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==>  DCR .\n"), kbp_reg_addr, data));

    kbp_reg_addr =  0x10000 | (0x1 << 3) | 0x7;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> DCR.\n"), kbp_reg_addr, data));

    for (i = 3 ; i < 8 ; ++i) {
        kbp_reg_addr =  0x10000 | (0x102 << 3) | i;
        SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        LOG_DEBUG(BSL_LS_SOC_TCAM,(BSL_META_U(unit, "kbp_reg_addr=0x%08x, data=0x%08x ==> read scratch using mdio i=%d.\n"), kbp_reg_addr, data, i));
    }

exit:
    SOCDNX_FUNC_RETURN;    
}


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

