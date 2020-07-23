/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        traffic_manager.c
 * Purpose:     Tomahawk3 IDB, EDB and MMU routines
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/tomahawk3.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>

void
soc_th3_check_soc_info_dev_port_idb_port_map (int unit)
{
    int dev_port, pipe, local_dev_port;
    int idb_port, exp_idb_port;
    soc_info_t *si = &SOC_INFO(unit);

    PBMP_PORT_ITER(unit, dev_port) {
        if(!IS_MANAGEMENT_PORT(unit, dev_port)) {
            idb_port = si->port_l2i_mapping[dev_port];
            pipe = si->port_pipe[dev_port];
            local_dev_port = dev_port % SOC_TH3_MAX_DEV_PORTS_PER_PIPE;
            if (pipe == 0) {
                exp_idb_port = local_dev_port - 1;
            } else {
                exp_idb_port = local_dev_port;
            }
            if (exp_idb_port != idb_port) {
                LOG_CLI((BSL_META_U(unit,
                    "ERROR: device port to idb port mapping in SOC infor wrong"
                    "for port:%d, exp value:%d actual value:%u\n"),
                    dev_port, exp_idb_port, idb_port ));
            }
        }
    }
}

void
soc_th3_check_idb_portmap(int unit)
{
    int dev_port, local_dev_port;
    int phy_port;
    int idb_port, pipe;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t mem_list[] = {ING_IDB_TO_DEVICE_PORT_MAPm,
                            ING_PHY_TO_IDB_PORT_MAPm};
    soc_mem_t mem;
    int mem_index, num_mems, index;
    soc_field_t field = INVALIDf;
    uint32 value = 0, exp_value;
    uint32 read_entry[SOC_MAX_MEM_WORDS];
    char *mem_name;
    int pm, subport, idb_phy_index;

    PBMP_PORT_ITER(unit, dev_port) {
        if(!IS_MANAGEMENT_PORT(unit, dev_port)) {
            idb_port = si->port_l2i_mapping[dev_port];
            phy_port = si->port_l2p_mapping[dev_port];
            local_dev_port = dev_port % SOC_TH3_MAX_DEV_PORTS_PER_PIPE;
            pipe = si->port_pipe[dev_port];
            pm = ((phy_port - 1) & 0x1f) / _TH3_PORTS_PER_PBLK;
            subport = ((phy_port - 1) & 0x1f) % _TH3_PORTS_PER_PBLK;
            idb_phy_index = pm * _TH3_PORTS_PER_PBLK + subport;

            num_mems = sizeof(mem_list) / sizeof(soc_mem_t);
            for (mem_index = 0;mem_index < num_mems; mem_index++) {
                mem = mem_list[mem_index];
                switch(mem) {
                    case ING_IDB_TO_DEVICE_PORT_MAPm:
                        field = DEVICE_PORTf;
                        index = idb_port;
                        exp_value = local_dev_port;
                        mem_name = "ING_IDB_TO_DEVICE_PORT_MAP";
                        break;
                    case ING_PHY_TO_IDB_PORT_MAPm:
                        field = IDB_PORTf;
                        index = idb_phy_index;
                        exp_value = idb_port;
                        mem_name = "ING_PHY_TO_IDB_PORT_MAP";
                        break;
                    default:
                        field = INVALIDf;
                        index = -1;
                        exp_value = -1;
                        mem_name = "INVALID";
                        break;
                }
                mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, mem_list[mem_index], pipe);
                /* coverity[checked_return:FALSE]*/
                soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, read_entry);
                /* coverity[checked_return:FALSE]*/
                value = soc_mem_field32_get(unit, mem, read_entry, field);
                if(exp_value != value) {
                    LOG_CLI((BSL_META_U(unit,
                        "ERROR: memory:%s port:%d exp value:%d actual value:%u\n"),
                               mem_name, dev_port, exp_value, value));
                    soc_th3_check_soc_info_dev_port_idb_port_map(unit);
                }
            }
        }
    }
}

void
soc_th3_check_mmu_portmap(int unit)
{
    int dev_port;
    int phy_port, local_phy_port;
    int chip_port_num, pipe;
    int mmu_port;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t mem_list[] = {MMU_THDO_DEVICE_PORT_MAPm,
                            MMU_RQE_DEVICE_TO_MMU_PORT_MAPPINGm};
    soc_reg_t reg_list[]={MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr,
                          MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPINGr,
                          MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPINGr,
                          MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPINGr,
                          MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPINGr};
    soc_mem_t mem;
    soc_reg_t reg;
    int mem_index, num_mems, index;
    int reg_index, num_regs;
    soc_field_t field=INVALIDf;
    uint32 value = 0, exp_value;
    uint32 read_entry[SOC_MAX_MEM_WORDS];
    char *mem_name;
    uint32 rval=0;

    PBMP_PORT_ITER(unit, dev_port) {
        if(!IS_MANAGEMENT_PORT(unit, dev_port)) {
            phy_port = si->port_l2p_mapping[dev_port];
            mmu_port = si->port_p2m_mapping[phy_port];
            chip_port_num = SOC_TH3_MMU_CHIP_PORT(unit, dev_port);
            pipe = si->port_pipe[dev_port];
            if (pipe) {
                local_phy_port = (phy_port - 1) % SOC_TH3_MMU_PORT_STRIDE;
            } else {
                local_phy_port = (phy_port) % SOC_TH3_MMU_PORT_STRIDE;
            }

            num_mems = sizeof(mem_list) / sizeof(soc_mem_t);
            for (mem_index = 0;mem_index < num_mems; mem_index++) {
                mem = mem_list[mem_index];
                switch(mem) {
                    case MMU_THDO_DEVICE_PORT_MAPm:
                        field = DEVICE_PORTf;
                        index = chip_port_num;
                        exp_value = dev_port;
                        mem_name = "MMU_THDO_DEVICE_PORT_MAP";
                        break;
                    case MMU_RQE_DEVICE_TO_MMU_PORT_MAPPINGm:
                        field = MMU_PORTf;
                        index = dev_port;
                        exp_value = mmu_port;
                        mem_name = "MMU_RQE_DEVICE_TO_MMU_PORT_MAPPING";
                        break;
                    default:
                        field = INVALIDf;
                        index = -1;
                        exp_value = -1;
                        mem_name = "INVALID";
                        break;
                }
                /* coverity[checked_return:FALSE]*/
                soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, read_entry);
                /* coverity[checked_return:FALSE]*/
                value = soc_mem_field32_get(unit, mem, read_entry, field);
                if(exp_value != value) {
                    LOG_CLI((BSL_META_U(unit,
                        "ERROR: memory:%s port:%d exp value:%d actual value:%u\n"),
                               mem_name, dev_port, exp_value, value));
                    soc_th3_check_soc_info_dev_port_idb_port_map(unit);
                }
            }

            num_regs = sizeof(reg_list) / sizeof(soc_reg_t);
            for (reg_index = 0;reg_index < num_regs; reg_index++) {
                reg = reg_list[reg_index];
                switch(reg) {
                    case MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr:
                        field = MMU_PORTf;
                        index = dev_port;
                        exp_value = mmu_port;
                        mem_name = "MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPING";
                        break;
                    case MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPINGr:
                        field = PHY_PORT_NUMf;
                        index = dev_port;
                        exp_value = phy_port;
                        mem_name = "MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPING";
                        break;
                    case MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPINGr:
                        field = PHY_PORT_NUMf;
                        index = dev_port;
                        exp_value = local_phy_port;
                        mem_name = "MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPING";
                        break;
                    case MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPINGr:
                        field = PHY_PORT_NUMf;
                        index = dev_port;
                        exp_value = local_phy_port;
                        mem_name = "MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPING";
                        break;
                    case MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPINGr:
                        field = DEVICE_PORTf;
                        index = dev_port;
                        exp_value = dev_port;
                        mem_name = "MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPING";
                        break;
                    default:
                        field = INVALIDf;
                        index = -1;
                        exp_value = -1;
                        mem_name = "INVALID";
                        break;
                }
                soc_reg32_get(unit, reg, index, 0, &rval);
                value = soc_reg_field_get(unit, reg, rval, field);
                if(exp_value != value) {
                    LOG_CLI((BSL_META_U(unit,
                        "ERROR: register:%s port:%d exp value:%d actual value:%u\n"),
                               mem_name, dev_port, exp_value, value));
                    soc_th3_check_soc_info_dev_port_idb_port_map(unit);
                }
            }
        }
    }
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
