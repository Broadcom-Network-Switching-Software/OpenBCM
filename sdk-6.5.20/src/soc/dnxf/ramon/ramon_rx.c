/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON RX
 */


#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_RX

#include <bcm/types.h>

#include <shared/bsl.h>

#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/defs.h>
#include <soc/types.h>
#include <soc/error.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

#include <soc/dnxf/ramon/ramon_rx.h>
#include <soc/dnxf/ramon/ramon_stack.h>

shr_error_e
soc_ramon_rx_cpu_address_modid_set(int unit, int num_of_cpu_addresses, int *cpu_addresses)
{
    int i;
    bcm_module_t alrc_max_base_index, update_base_index;
    uint32 mem_val[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 reg_val32;
    soc_dnxc_isolation_status_t isolation_status = soc_dnxc_isolation_status_active;

    soc_reg_t eci_fe_global_fe_dest_ids[] = {ECI_FE_GLOBAL_FE_DEST_IDS_1r, ECI_FE_GLOBAL_FE_DEST_IDS_2r, ECI_FE_GLOBAL_FE_DEST_IDS_3r, ECI_FE_GLOBAL_FE_DEST_IDS_4r};
    soc_field_t fe_dest_id[] = {FE_DEST_ID_0f, FE_DEST_ID_1f, FE_DEST_ID_2f, FE_DEST_ID_3f, FE_DEST_ID_4f, FE_DEST_ID_5f, FE_DEST_ID_6f, FE_DEST_ID_7f};
    soc_field_t fe_dest_id_en[] = {FE_DEST_ID_0_ENf, FE_DEST_ID_1_ENf, FE_DEST_ID_2_ENf, FE_DEST_ID_3_ENf, FE_DEST_ID_4_ENf, FE_DEST_ID_5_ENf, FE_DEST_ID_6_ENf, FE_DEST_ID_7_ENf};
    int nof_fields_in_reg = 0;
    int reg_index = 0;
    int cpu_addr_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    if ((num_of_cpu_addresses < 0) || (num_of_cpu_addresses > dnxf_data_fabric.cell.rx_max_nof_cpu_buffers_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "num_of_cpu_addresses invalid \n");
    }

    
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_topology_isolate_get,(unit, &isolation_status)));
    if (isolation_status == soc_dnxc_isolation_status_active) {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bcm_rx_start/stop should not be called when traffic is enabled.\nTo disable traffic use bcm_fabric_control_set with bcmFabricIsolate parameter.\n ");
    }

    

    SHR_IF_ERR_EXIT(soc_ramon_stk_module_max_all_reachable_get(unit, &alrc_max_base_index));
    SHR_IF_ERR_EXIT(soc_ramon_stk_module_max_fap_get(unit, &update_base_index));

    for (i = 0 ; i < num_of_cpu_addresses ; i++)
    {
        if (!((cpu_addresses[i] >= alrc_max_base_index) && (cpu_addresses[i] <= update_base_index)))
        {
            LOG_CLI((BSL_META_U(unit,
                "warning: cpu address is not in the interval [AlrcMaxBaseIndex,UpdateBaseIndex] (the ignored interval for calculating all reachable vector)\n")));
        }
    }

    nof_fields_in_reg = COUNTOF(fe_dest_id) / COUNTOF(eci_fe_global_fe_dest_ids);

    for (cpu_addr_index = 0; cpu_addr_index < num_of_cpu_addresses; ++cpu_addr_index)
    {
        reg_index = cpu_addr_index / nof_fields_in_reg;

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, eci_fe_global_fe_dest_ids[reg_index], REG_PORT_ANY, 0, &reg_val32));
        soc_reg_field_set(unit, eci_fe_global_fe_dest_ids[reg_index], &reg_val32, fe_dest_id[cpu_addr_index], cpu_addresses[cpu_addr_index]);
        soc_reg_field_set(unit, eci_fe_global_fe_dest_ids[reg_index], &reg_val32, fe_dest_id_en[cpu_addr_index], 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, eci_fe_global_fe_dest_ids[reg_index], REG_PORT_ANY, 0, reg_val32));
    }

    for (cpu_addr_index = num_of_cpu_addresses; cpu_addr_index < dnxf_data_fabric.cell.rx_max_nof_cpu_buffers_get(unit); ++cpu_addr_index)
    {
        reg_index = cpu_addr_index / nof_fields_in_reg;

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, eci_fe_global_fe_dest_ids[reg_index], REG_PORT_ANY, 0, &reg_val32));
        soc_reg_field_set(unit, eci_fe_global_fe_dest_ids[reg_index], &reg_val32, fe_dest_id_en[cpu_addr_index], 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, eci_fe_global_fe_dest_ids[reg_index], REG_PORT_ANY, 0, reg_val32));
    }

    if (SOC_DNXF_IS_FE13(unit) && !(dnxf_data_fabric.general.local_routing_enable_uc_get(unit)))
    {
        int nof_qrh, j;
        for (i=0 ; i < dnxf_data_device.blocks.nof_instances_qrh_get(unit)/2; i++) { 
            SHR_IF_ERR_EXIT(READ_QRH_FE_GLOBAL_FE_DEST_IDS_1r(unit, i, &reg_val32));
            soc_reg_field_set(unit, QRH_FE_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_0_ENf, 0x0);
            soc_reg_field_set(unit, QRH_FE_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_1_ENf, 0X0);
            SHR_IF_ERR_EXIT(WRITE_QRH_FE_GLOBAL_FE_DEST_IDS_1r(unit, i, reg_val32));

            SHR_IF_ERR_EXIT(READ_QRH_FE_GLOBAL_FE_DEST_IDS_2r(unit, i, &reg_val32));
            soc_reg_field_set(unit, QRH_FE_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_2_ENf, 0x0);
            soc_reg_field_set(unit, QRH_FE_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_3_ENf, 0X0);
            SHR_IF_ERR_EXIT(WRITE_QRH_FE_GLOBAL_FE_DEST_IDS_2r(unit, i, reg_val32));
        }

        
        SHR_BITSET_RANGE(mem_val, 0, dnxf_data_port.general.nof_links_get(unit));
        nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
        for (i = 0 ; i < num_of_cpu_addresses ; i++)
        {
            SHR_IF_ERR_EXIT(WRITE_RTP_RMHMTm(unit, MEM_BLOCK_ALL, cpu_addresses[i], mem_val));
            for (j = 0; j < nof_qrh; j++) {
                SHR_IF_ERR_EXIT(WRITE_QRH_DUCTm(unit, QRH_BLOCK(unit, j), cpu_addresses[i], mem_val));
            }
        }                  
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_rx_cpu_address_modid_init(int unit)
{
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_ECI_FE_GLOBAL_FE_DEST_IDS_1r(unit, &reg_val));
    soc_reg_field_set(unit, ECI_FE_GLOBAL_FE_DEST_IDS_1r, &reg_val, FE_DEST_ID_0_ENf, 0);
    soc_reg_field_set(unit, ECI_FE_GLOBAL_FE_DEST_IDS_1r, &reg_val, FE_DEST_ID_1_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_ECI_FE_GLOBAL_FE_DEST_IDS_1r(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_ECI_FE_GLOBAL_FE_DEST_IDS_2r(unit, &reg_val));
    soc_reg_field_set(unit, ECI_FE_GLOBAL_FE_DEST_IDS_2r, &reg_val, FE_DEST_ID_2_ENf, 0);
    soc_reg_field_set(unit, ECI_FE_GLOBAL_FE_DEST_IDS_2r, &reg_val, FE_DEST_ID_3_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_ECI_FE_GLOBAL_FE_DEST_IDS_2r(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_ECI_FE_GLOBAL_FE_DEST_IDS_3r(unit, &reg_val));
    soc_reg_field_set(unit, ECI_FE_GLOBAL_FE_DEST_IDS_3r, &reg_val, FE_DEST_ID_4_ENf, 0);
    soc_reg_field_set(unit, ECI_FE_GLOBAL_FE_DEST_IDS_3r, &reg_val, FE_DEST_ID_5_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_ECI_FE_GLOBAL_FE_DEST_IDS_3r(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_ECI_FE_GLOBAL_FE_DEST_IDS_4r(unit, &reg_val));
    soc_reg_field_set(unit, ECI_FE_GLOBAL_FE_DEST_IDS_4r, &reg_val, FE_DEST_ID_6_ENf, 0);
    soc_reg_field_set(unit, ECI_FE_GLOBAL_FE_DEST_IDS_4r, &reg_val, FE_DEST_ID_7_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_ECI_FE_GLOBAL_FE_DEST_IDS_4r(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}


#undef BSL_LOG_MODULE
