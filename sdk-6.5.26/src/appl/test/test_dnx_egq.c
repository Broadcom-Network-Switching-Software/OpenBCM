#ifdef BSL_LOG_MODULE 
#error "_BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_TESTS

#include <sal/types.h>

#if defined (BCM_DNX_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <sal/appl/config.h>
#include <shared/bsl.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/dbal/dbal_external_defines.h>

#include "testlist.h"

static 
shr_error_e test_dnx_egq_global_egq_dbal_configurations(
    int unit,
    uint32 enable,
    int half_q_pairs)
{
    int core;
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit)

    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 8,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE, enable,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_ENA, INST_SINGLE, enable,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_ENA, INST_SINGLE, enable,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_INTERFACE_SPR_ENA, INST_SINGLE, enable,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_NUM_OF_TCG_SPR_B, INST_SINGLE, half_q_pairs,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_NUM_OF_TCG_SPR_A, INST_SINGLE, half_q_pairs,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_NUM_OF_QP_SPR_B, INST_SINGLE, half_q_pairs,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_NUM_OF_QP_SPR_A, INST_SINGLE, half_q_pairs,
                                         GEN_DBAL_FIELD_LAST_MARK));

        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPS_PS_MODEr, core, 0, reg_above_64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/** Global variable for functions related with packet*/


/**
 * \brief - Create a packet
 */
static
shr_error_e test_dnx_egq_create_packet(
    int unit,
    rhhandle_t * packet_header,
    bcm_gport_t src_port)
{
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));

    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, packet_header));

    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit,*packet_header, "PTCH_2"));

    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, *packet_header, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));

    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, *packet_header, "ETH1"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - send a packet
 */
static 
shr_error_e test_dnx_egq_send_packet(
    int unit,
    rhhandle_t * packet_header,
    bcm_gport_t src_port,
    uint32 pcp)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, *packet_header, "ETH1.VLAN.PCP", &pcp, 3));

    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, *packet_header, SAND_PACKET_RESUME));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove HW configuration for qpair
 */
static void
test_dnx_egq_remove_qpair_configuration(
    int unit,
    int base_q_pair,
    int core_id,
    int half_q_pairs,
    int nof_priorities,
    int bit_num,
    int use_second_memory)
{

    int cosq;
    eps_eps_prio_map_entry_t eps_prio_map_entry;
    eps_dwm_entry_t eps_dwm_entry;
    eps_qp_cfg_2_entry_t eps_qp_cfg_2_entry;
    eps_qp_cfg_entry_t eps_qp_cfg_entry;
    pqp_otm_attributes_table_deq_entry_t pqp_otm_attributes_table_deq_entry;

    soc_mem_read(unit, EPS_EPS_PRIO_MAPm, EPS_BLOCK(unit, core_id), bit_num, eps_prio_map_entry.entry_data);
    soc_mem_field32_set(unit, EPS_EPS_PRIO_MAPm, eps_prio_map_entry.entry_data, EPS_PRIO_MAPf, 0);
    soc_mem_write(unit, EPS_EPS_PRIO_MAPm, EPS_BLOCK(unit, core_id), bit_num, eps_prio_map_entry.entry_data);

    for(cosq = 0; cosq < nof_priorities; cosq++)
    {
        soc_mem_read(unit, EPS_DWMm, EPS_BLOCK(unit, core_id), base_q_pair + cosq, eps_dwm_entry.entry_data);
        soc_mem_field32_set(unit, EPS_DWMm, eps_dwm_entry.entry_data, MC_OR_MC_LOW_QUEUE_WEIGHTf, 0);
        soc_mem_write(unit, EPS_DWMm, EPS_BLOCK(unit, core_id), base_q_pair + cosq, eps_dwm_entry.entry_data);
    }

    if (use_second_memory)
    {
        for(cosq = 0; cosq < nof_priorities; cosq++)
        {
            soc_mem_read(unit, EPS_QP_CFG_2m, EPS_BLOCK(unit, core_id), (base_q_pair - half_q_pairs) + cosq, eps_qp_cfg_2_entry.entry_data);
            soc_mem_field32_set(unit, EPS_QP_CFG_2m, eps_qp_cfg_2_entry.entry_data, MAP_QP_TO_IFCf, 0);
            soc_mem_field32_set(unit, EPS_QP_CFG_2m, eps_qp_cfg_2_entry.entry_data, FORCE_OR_IGNORE_FCf, 0);
            soc_mem_write(unit, EPS_QP_CFG_2m, EPS_BLOCK(unit, core_id), base_q_pair - half_q_pairs + cosq, eps_qp_cfg_2_entry.entry_data);
        }
    }
    else
    {
        for(cosq = 0; cosq < nof_priorities; cosq++)
        {
            soc_mem_read(unit, EPS_QP_CFGm, EPS_BLOCK(unit, core_id), base_q_pair + cosq, eps_qp_cfg_entry.entry_data);
            soc_mem_field32_set(unit, EPS_QP_CFGm, eps_qp_cfg_entry.entry_data, MAP_QP_TO_IFCf, 0);
            soc_mem_field32_set(unit, EPS_QP_CFGm, eps_qp_cfg_entry.entry_data, FORCE_OR_IGNORE_FCf, 0);
            soc_mem_write(unit, EPS_QP_CFGm, EPS_BLOCK(unit, core_id), base_q_pair + cosq, eps_qp_cfg_entry.entry_data);
        }
    }

    soc_mem_read(unit, PQP_OTM_ATTRIBUTES_TABLE_DEQm, PQP_BLOCK(unit, core_id), base_q_pair, pqp_otm_attributes_table_deq_entry.entry_data);
    soc_mem_field32_set(unit, PQP_OTM_ATTRIBUTES_TABLE_DEQm, pqp_otm_attributes_table_deq_entry.entry_data, CR_ADJUST_VALUEf, 0);
    soc_mem_field32_set(unit, PQP_OTM_ATTRIBUTES_TABLE_DEQm, pqp_otm_attributes_table_deq_entry.entry_data, CGM_PORT_PROFILEf, 0);
    soc_mem_write(unit, PQP_OTM_ATTRIBUTES_TABLE_DEQm, PQP_BLOCK(unit, core_id), base_q_pair, pqp_otm_attributes_table_deq_entry.entry_data);
}

/**
 * \brief - Add HW configuration for qpair
 */
static void
test_dnx_egq_add_qpair_configuration(
    int unit,
    int nif_port,
    int base_q_pair,
    int interface,
    int core_id,
    int nof_q_pairs,
    int nof_priorities,
    int bit_num,
    int use_second_memory)
{

    int cosq;
    int half_q_pairs = nof_q_pairs / 2;
    eps_eps_prio_map_entry_t eps_prio_map_entry;
    eps_dwm_entry_t eps_dwm_entry;
    eps_qp_cfg_2_entry_t eps_qp_cfg_2_entry;
    eps_qp_cfg_entry_t eps_qp_cfg_entry;
    eps_ifc_cfg_entry_t eps_ifc_cfg_entry;
    pqp_otm_attributes_table_deq_entry_t pqp_otm_attributes_table_deq_entry;
    pqp_otm_attributes_table_enq_entry_t pqp_otm_attributes_table_enq_entry;
    eps_qp_credit_table_entry_t eps_qp_credit_table_entry;

    soc_mem_read(unit, EPS_EPS_PRIO_MAPm, EPS_BLOCK(unit, core_id), bit_num, eps_prio_map_entry.entry_data);
    soc_mem_field32_set(unit, EPS_EPS_PRIO_MAPm, eps_prio_map_entry.entry_data, EPS_PRIO_MAPf, 0xfac688);
    soc_mem_write(unit, EPS_EPS_PRIO_MAPm, EPS_BLOCK(unit, core_id), bit_num, eps_prio_map_entry.entry_data);

    if (use_second_memory)
    {
        for(cosq = 0; cosq < nof_priorities; cosq++)
        {
            soc_mem_read(unit, EPS_QP_CFG_2m, EPS_BLOCK(unit, core_id), (base_q_pair - half_q_pairs) + cosq, eps_qp_cfg_2_entry.entry_data);
            soc_mem_field32_set(unit, EPS_QP_CFG_2m, eps_qp_cfg_2_entry.entry_data, MAP_QP_TO_IFCf, interface);
            soc_mem_field32_set(unit, EPS_QP_CFG_2m, eps_qp_cfg_2_entry.entry_data, FORCE_OR_IGNORE_FCf, 1);
            soc_mem_write(unit, EPS_QP_CFG_2m, EPS_BLOCK(unit, core_id), (base_q_pair - half_q_pairs) + cosq, eps_qp_cfg_2_entry.entry_data);
        }
    }
    else
    {
        for(cosq = 0; cosq < nof_priorities; cosq++)
        {
            soc_mem_read(unit, EPS_QP_CFGm, EPS_BLOCK(unit, core_id), base_q_pair + cosq, eps_qp_cfg_entry.entry_data);
            soc_mem_field32_set(unit, EPS_QP_CFGm, eps_qp_cfg_entry.entry_data, MAP_QP_TO_IFCf, interface);
            soc_mem_field32_set(unit, EPS_QP_CFGm, eps_qp_cfg_entry.entry_data, FORCE_OR_IGNORE_FCf, 1);
            soc_mem_write(unit, EPS_QP_CFGm, EPS_BLOCK(unit, core_id), base_q_pair + cosq, eps_qp_cfg_entry.entry_data);
        }
    }

    soc_mem_read(unit, EPS_IFC_CFGm, EPS_BLOCK(unit, core_id), interface, eps_ifc_cfg_entry.entry_data);
    soc_mem_field32_set(unit, EPS_IFC_CFGm, eps_ifc_cfg_entry.entry_data, NONCH_IFC_OTM_PORTf, base_q_pair);
    soc_mem_write(unit, EPS_IFC_CFGm, EPS_BLOCK(unit, core_id), interface, eps_ifc_cfg_entry.entry_data);

    soc_mem_read(unit, PQP_OTM_ATTRIBUTES_TABLE_DEQm, PQP_BLOCK(unit, core_id), base_q_pair, pqp_otm_attributes_table_deq_entry.entry_data);
    soc_mem_field32_set(unit, PQP_OTM_ATTRIBUTES_TABLE_DEQm, pqp_otm_attributes_table_deq_entry.entry_data, CR_ADJUST_VALUEf, 24);
    soc_mem_field32_set(unit, PQP_OTM_ATTRIBUTES_TABLE_DEQm, pqp_otm_attributes_table_deq_entry.entry_data, CGM_PORT_PROFILEf, 1);
    soc_mem_write(unit, PQP_OTM_ATTRIBUTES_TABLE_DEQm, PQP_BLOCK(unit, core_id), base_q_pair, pqp_otm_attributes_table_deq_entry.entry_data);

    soc_mem_read(unit, PQP_OTM_ATTRIBUTES_TABLE_ENQm, PQP_BLOCK(unit, core_id), nif_port, pqp_otm_attributes_table_enq_entry.entry_data);
    soc_mem_field32_set(unit, PQP_OTM_ATTRIBUTES_TABLE_ENQm, pqp_otm_attributes_table_enq_entry.entry_data, QPAIR_BASEf, base_q_pair);
    soc_mem_write(unit, PQP_OTM_ATTRIBUTES_TABLE_ENQm, PQP_BLOCK(unit, core_id), nif_port, pqp_otm_attributes_table_enq_entry.entry_data);

    for(cosq = 0; cosq < nof_priorities; cosq++) {
        soc_mem_read(unit, EPS_DWMm, EPS_BLOCK(unit, core_id), base_q_pair + cosq, eps_dwm_entry.entry_data);
        soc_mem_field32_set(unit, EPS_DWMm, eps_dwm_entry.entry_data, MC_OR_MC_LOW_QUEUE_WEIGHTf, 1);
        soc_mem_write(unit, EPS_DWMm, EPS_BLOCK(unit, core_id), base_q_pair + cosq, eps_dwm_entry.entry_data);

        soc_mem_read(unit, EPS_QP_CREDIT_TABLEm, EPS_BLOCK(unit, core_id), cosq, eps_qp_credit_table_entry.entry_data);
        soc_mem_field32_set(unit, EPS_QP_CREDIT_TABLEm, eps_qp_credit_table_entry.entry_data, QPAIR_INDEXf, base_q_pair + cosq);
        soc_mem_write(unit, EPS_QP_CREDIT_TABLEm, EPS_BLOCK(unit, core_id), cosq, eps_qp_credit_table_entry.entry_data);

        soc_mem_read(unit, EPS_QP_CREDIT_TABLEm, EPS_BLOCK(unit, core_id), nof_q_pairs + cosq, eps_qp_credit_table_entry.entry_data);
        soc_mem_field32_set(unit, EPS_QP_CREDIT_TABLEm, eps_qp_credit_table_entry.entry_data, QPAIR_INDEXf, base_q_pair + cosq);
        soc_mem_write(unit, EPS_QP_CREDIT_TABLEm, EPS_BLOCK(unit, core_id), nof_q_pairs + cosq, eps_qp_credit_table_entry.entry_data);
    }

}

/**
 * \brief - internal function that frees the packet if something fails
 */
static 
shr_error_e test_dnx_egq_qpair_test_internal(
    int unit)
{
    int success = 0;

    int core_id;
    int cpu_port;
    int nif_port;
    int cosq;
    int base_q_pair;
    int interface;
    int nof_q_pairs = dnx_data_egr_queuing.params.nof_q_pairs_get(unit);
    int half_q_pairs = nof_q_pairs / 2;
    int nof_priorities;
    uint32 pd, db;
    int use_second_memory;
    int bit_num;
    bcm_port_t port0 = 0, port1 = 1, port2 = 2, port3 = 3;
    rhhandle_t packet_header = NULL;
    uint64 tx_packet_count;
#ifdef INCLUDE_CTEST
    int nof_cores = dnx_data_device.general.nof_cores_get(unit);
    char ucode_port0_name[RHNAME_MAX_SIZE], ucode_port0_value[RHNAME_MAX_SIZE];
    char ucode_port1_name[RHNAME_MAX_SIZE], ucode_port1_value[RHNAME_MAX_SIZE];
    char ucode_port2_name[RHNAME_MAX_SIZE], ucode_port2_value[RHNAME_MAX_SIZE];
    char ucode_port3_name[RHNAME_MAX_SIZE], ucode_port3_value[RHNAME_MAX_SIZE];
    char tm_header_name[RHNAME_MAX_SIZE], tm_header_value[RHNAME_MAX_SIZE];
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_soc_property_t ctest_soc_property[20];
#endif /** INCLUDE_CTEST */

    SHR_FUNC_INIT_VARS(unit);

#ifdef INCLUDE_CTEST
    sprintf(ucode_port0_name, "ucode_port_%d", port0);
    sprintf(ucode_port0_value, "CPU.%d:core_0.%d", port0, port0);
    sprintf(ucode_port1_name, "ucode_port_%d", port1);
    sprintf(ucode_port1_value, "CGE1:core_0.%d", port1);

    ctest_soc_property[0].property = "ucode_port_*";
    ctest_soc_property[0].value = NULL;
    ctest_soc_property[1].property = "port_priorities*";
    ctest_soc_property[1].value = NULL;
    ctest_soc_property[2].property = "ilkn_use_fabric_links_1*";
    ctest_soc_property[2].value = NULL;
    ctest_soc_property[3].property = "port_priorities";
    ctest_soc_property[3].value = "8";
    ctest_soc_property[4].property = "otm_base_q_pair_1";
    ctest_soc_property[4].value = "8";
    ctest_soc_property[5].property = "otm_base_q_pair_2";
    ctest_soc_property[5].value = "8";
    ctest_soc_property[6].property = "dram_temperature_monitor_enable";
    ctest_soc_property[6].value = "0";
    ctest_soc_property[7].property = "ext_ram_enabled_bitmap*";
    ctest_soc_property[7].value = "0";
    ctest_soc_property[8].property = ucode_port0_name;
    ctest_soc_property[8].value = ucode_port0_value;
    ctest_soc_property[9].property = ucode_port1_name;
    ctest_soc_property[9].value = ucode_port1_value;
    /** Last member must be NULL*/
    ctest_soc_property[10].property = NULL;
    ctest_soc_property[10].value = NULL;

    if (nof_cores > 1)
    {
        sprintf(ucode_port2_name, "ucode_port_%d", port2);
        sprintf(ucode_port2_value, "CPU.%d:core_1.%d", port2, port2);
        sprintf(tm_header_name, "tm_port_header_type_in_%d", port2);
        sprintf(tm_header_value, "INJECTED_2_PP");
        sprintf(ucode_port3_name, "ucode_port_%d", port3);
        sprintf(ucode_port3_value, "CGE20:core_1.%d", port3);

        ctest_soc_property[10].property = ucode_port2_name;
        ctest_soc_property[10].value = ucode_port2_value;
        ctest_soc_property[11].property = tm_header_name;
        ctest_soc_property[11].value = tm_header_value;
        ctest_soc_property[12].property = ucode_port3_name;
        ctest_soc_property[12].value = ucode_port3_value;
        /** Last member must be NULL*/
        ctest_soc_property[13].property = NULL;
        ctest_soc_property[13].value = NULL;
    }

    SHR_IF_ERR_EXIT_WITH_LOG(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h),"Error - couldn't configure the soc properties %s%s%s\n", EMPTY, EMPTY, EMPTY);

#endif /** INCLUDE_CTEST */

    SHR_IF_ERR_EXIT_WITH_LOG(test_dnx_egq_global_egq_dbal_configurations(unit, 0, half_q_pairs),"Error - couldn't configure global test config %s%s%s\n", EMPTY, EMPTY, EMPTY);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        packet_header = NULL;
        /** Determine which ports to use depending on core */
        if (core_id == 0) {
            cpu_port = port0;
            nif_port = port1;
        } else {
            cpu_port = port2;
            nif_port = port3;
        }

        /** Get egress interface */
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_port_egr_if_get(unit, nif_port, &interface),"Error - couldn't get egress interface %s%s%s\n", EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_port_priorities_nof_get(unit, nif_port, &nof_priorities),"Error - couldn't get nof priorities %s%s%s\n", EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT_WITH_LOG(bcm_port_force_forward_set(unit, cpu_port, nif_port, 1),"Error - couldn't configure force forward %s%s%s\n", EMPTY, EMPTY, EMPTY);

        /** create packet structure that will be used for the test */
        SHR_IF_ERR_EXIT_WITH_LOG(test_dnx_egq_create_packet(unit, &packet_header, cpu_port),"Error - creating the packet failed %s%s%s\n", EMPTY, EMPTY, EMPTY);

        /** Iter all qpairs, configure specific queues , send a packet to each one and check that it is received */
        for (base_q_pair = 0; base_q_pair < nof_q_pairs; base_q_pair = base_q_pair + 8) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "Testing from Qpair %d to %d on core %d\n"), base_q_pair, base_q_pair + 7, core_id));

            bit_num = base_q_pair / 8;
            if (base_q_pair >= half_q_pairs)
            {
                use_second_memory = 1;
            }
            else
            {
                use_second_memory = 0;
            }

            /** Configure HW */
            test_dnx_egq_add_qpair_configuration(unit, nif_port, base_q_pair, interface, core_id, nof_q_pairs, nof_priorities, bit_num, use_second_memory);

            for(cosq  =  0; cosq  <  nof_priorities; cosq++) {
                SHR_IF_ERR_EXIT_WITH_LOG(test_dnx_egq_send_packet(unit, &packet_header, cpu_port, cosq),"Error - something wrong when sending a packet %s%s%s\n", EMPTY, EMPTY, EMPTY);
            }
            /** Wait packets to be proceed*/
            sal_usleep(2000000);

            for(cosq  =  0; cosq  <  nof_priorities; cosq++) {
                /** Get statistics for queue */
                SHR_IF_ERR_EXIT_WITH_LOG(dnx_ecgm_dbal_queue_resources_statistics_max_get(unit, core_id, 0, base_q_pair + cosq, &pd, &db),"Error - couldn't get statistics %s%s%s\n", EMPTY, EMPTY, EMPTY);
                /** Verify we have one packet received on each queue*/
                if ((pd != 1) || (db != 1))
                {
                    LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error - a packet is missing for queue %d.\n"), base_q_pair + cosq));
                    SHR_IF_ERR_EXIT(_SHR_E_FAIL);
                }
            }

            SHR_IF_ERR_EXIT_WITH_LOG(bcm_stat_get(unit, nif_port, snmpEtherStatsTXNoErrors, &tx_packet_count),"Error - couldn't get stats for packets %s%s%s\n", EMPTY, EMPTY, EMPTY);
            printf("packet count is %d \n", COMPILER_64_LO(tx_packet_count));
            /** Verify all the packets are received */
            if (COMPILER_64_LO(tx_packet_count) != base_q_pair + 8)
            {
                LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error - a packet is missing.\n")));
                SHR_IF_ERR_EXIT(_SHR_E_FAIL);
            }

            /** Remove HW configuration */
            test_dnx_egq_remove_qpair_configuration(unit, base_q_pair, core_id, half_q_pairs, nof_priorities, bit_num, use_second_memory);

            LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "Finish successfully test for qpairs in range %d - %d on core %d \n"), base_q_pair, base_q_pair + 7, core_id));
        }
        /** free packet structure */
        diag_sand_packet_free(unit, packet_header);
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META_U(unit, "Please reset device after test is done! \n")));
    success = 1;
exit:
    if (!success)
    {
        diag_sand_packet_free(unit, packet_header);
    }
    SHR_FUNC_EXIT;
}

/** The main function for TR test for EGQ qpairs*/
int
test_dnx_egq_qpair_test(int unit, args_t *a, void *pa)
{
    int rv = 0;

    rv = test_dnx_egq_qpair_test_internal(unit);
    if (rv != 0)
    {
         LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error - test failed.\n")));
         return BCM_E_FAIL;
    }
    return rv;
}
#endif

#undef _ERR_MSG_MODULE_NAME
