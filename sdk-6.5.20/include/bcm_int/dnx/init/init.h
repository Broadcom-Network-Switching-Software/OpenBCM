/** \file bcm_int/dnx/init/init.h
 *
 * Internal DNX INIT APIs
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_INIT_INIT_H_INCLUDED
/*
 * {
 */
#define _BCMINT_DNX_INIT_INIT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_seq.h>
#include <sal/core/time.h>

/**
 * Default time threshold (in microseconds) for the time each
 * step is taking during init. This value was chosen because the
 * majority of the steps are below it. For steps that are
 * expected to be above the default, an explicit threshold can be
 * set or DNX Data can be used.
 */
#define BCM_INIT_STEP_TIME_THRESH_DEFAULT       100000

/*
 * Dynamic or internal flag which are recieved by the flag CB. developer can use them to write his flag CBs.
 * when inserting a new sub-list to the init sequence the STEP_IS_LIST flag should be used. in each flag CB the developer
 * is expected to use the SKIP flag according to a logic which is internal to his module, the flag CB should assign those
 * flags according to SOC properties (for example if soc property trunk_disable is set, the CB should set the skip flag to dynamic_flags).
 */
/**
 * Skip step
 */
#define DNX_INIT_STEP_F_SKIP                        UTILEX_SEQ_STEP_F_SKIP

/**
 * Print in verbose level
 */
#define DNX_INIT_STEP_F_VERBOSE                     UTILEX_SEQ_STEP_F_VERBOSE

/**
 * Skip Step due to WB
 */
#define DNX_INIT_STEP_F_WB_SKIP                     UTILEX_SEQ_STEP_F_WB_SKIP
/**
 * Allow writing to registers and changing SW state
 */
#define DNX_INIT_STEP_F_WB_DISABLE_CHECKS           UTILEX_SEQ_STEP_F_WB_DISABLE_CHECKS
/**
 * \brief step required for access only initalization (allow lite inializtion with access only)
 */
#define DNX_INIT_STEP_F_REQUIRED_FOR_ACCESS_ONLY    UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY
/**
 * \brief step required for access only (including per port access) initalization (allow lite with access only)
 */
#define DNX_INIT_STEP_F_REQUIRED_FOR_PORT_ACCESS_ONLY UTILEX_SEQ_STEP_F_REQUIRED_FOR_PORT_ACCESS_ONLY
/**
 * \brief step required for minimal startup supporting phy enable
 */
#define DNX_INIT_STEP_F_REQUIRED_FOR_HEAT_UP UTILEX_SEQ_STEP_F_REQUIRED_FOR_HEAT_UP

/**
 * \brief Allow running this step as a stand-alone
 */
#define DNX_INIT_STEP_F_STANDALONE_EN               UTILEX_SEQ_STEP_F_STANDALONE_EN
/**
 * \brief
 * Time threshold flags - used for testing how much time each step in the Init takes.
 * If flag is set to PER_DEVICE the value will be taken from DNX data. THIS FLAG CAN NOT BE USED FOR STEPS BEFORE DNX DATA IS INITIALIZED
 * If EXPLICIT flag is specified, the value will be taken from an array of the Step IDs and corresponding thresholds. It is user defined
 * and allows to set a specific value for steps before the DNX Data initialization.
 * If no flag is specified, default value will be assumed.
 */
#define DNX_INIT_STEP_F_TIME_THRESH_EXPLICIT     UTILEX_SEQ_STEP_F_TIME_THRESH_EXPLICIT
#define DNX_INIT_STEP_F_TIME_THRESH_PER_DEVICE   UTILEX_SEQ_STEP_F_TIME_THRESH_PER_DEVICE

/**
 * Skip memory test
 */
#define DNX_INIT_STEP_F_MEM_TEST_SKIP                     UTILEX_SEQ_STEP_F_MEM_TEST_SKIP
/**
 * \brief
 * Run the step in parallel on a seperate thread
 */
#define DNX_INIT_STEP_F_RUN_ON_THREAD                 UTILEX_SEQ_STEP_F_RUN_ON_THREAD
/**
 * \brief
 * Join all active seq threads before executing the current step
 */
#define DNX_INIT_STEP_F_JOIN_ALL_THREADS              UTILEX_SEQ_STEP_F_JOIN_ALL_THREADS

/**
 * \brief Maximal step name size
 */
#define DNX_INIT_STEP_NAME_SIZE              (256)

/**
 * The additional Header is UDP 1-7
 */
#define ADDITIONAL_HEADERS_UDP_SESSION_INDEX_MIN (1)
#define ADDITIONAL_HEADERS_UDP_SESSION_INDEX_MAX (8)
#define ADDITIONAL_HEADERS_UDP_USER_DEFINED(udp_index) (DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_8 + udp_index - 1)

/**
 *  \brief From where to take the data from FFC meta-data
 */
#define FFC_OFFSET 0
 /**
  *  \brief Build PRT FFC_INSTRUCTION values
  */
#define BUILD_PRT_FFC(offset,width,ffc_width,input_offset,ffc_input_offset)  \
 (((offset) << FFC_OFFSET) | \
 ((width) << (ffc_width)) | \
 ((input_offset) << (ffc_input_offset)))

/**
 * \brief Describes the IDs for the different steps and
 *        sub-steps of the Init. each entry here needs to be
 *        aligned with DNX_INIT_STEP_NAME_STR below.
 */
typedef enum
{
    /**
     * Invalid Value, used to indicate to APIs that this input is
     * not relevant
     */
    DNX_INIT_STEP_INVALID_STEP = UTILEX_SEQ_STEP_INVALID,
    /**
     * Value used get the first step of the enum
     */
    DNX_INIT_STEP_FIRST_STEP = UTILEX_SEQ_STEP_INVALID + 1,
    /**
     * Sub-list containing all TM modules
     */
    DNX_INIT_STEP_TM_MODULES = DNX_INIT_STEP_FIRST_STEP,
    /**
     * Sub-list containing all PP modules
     */
    DNX_INIT_STEP_PP_MODULES,
    /**
     * Init Schan
     */
    DNX_INIT_STEP_SCHAN,
    /**
     * Init adapter reg access
     */
    DNX_INIT_STEP_ADAPTER_REG_ACC,
    /**
     * Init device feature list
     */
    DNX_INIT_STEP_FEATURE,
    /**
     * Init Info config
     */
    DNX_INIT_STEP_INFO_CONFIG,
    /**
     * Init contorl structures.
     */
    DNX_INIT_STEP_CONTROL,
    /**
     * Init needed mutexes.
     */
    DNX_INIT_STEP_MUTEXES,
    /**
     * Init memory mutexes.
     */
    DNX_INIT_STEP_MEM_MUTEX,
    /**
     * Init time analyzer.
     */
    DNX_INIT_STEP_TIME_ANALYZER,
    /**
     * Init the MBCM.
     */
    DNX_INIT_STEP_MBCM_INIT,
    /**
     * Init the dnx_defines - legacy step should be removed in the future.
     */
    DNX_INIT_STEP_DEFINES_INIT,
    /**
     * Init the dnx_imp_defines - legacy step should be removed in the future.
     */
    DNX_INIT_STEP_IMP_DEFINES_INIT,
    /**
     * Sub-list containing all Legacy Soc Steps,
     * eventually this should be empty and canceled.
     */
    DNX_INIT_STEP_LEGACY_SOC,
    /**
     * Init Dnx Data.
     */
    DNX_INIT_STEP_DNX_DATA,
    /**
     * Init Error Recovery.
     */
    DNX_INIT_STEP_ERROR_RECOVERY,
    /**
     * Init SW State.
     */
    DNX_INIT_STEP_SW_STATE,
    /**
     * Init DBAL.
     */
    DNX_INIT_STEP_DBAL,
    /**
     * Init Common Modules.
     */
    DNX_INIT_STEP_COMMON_MODULES,
    /**
     * Init RX.
     */
    DNX_INIT_STEP_RX,
    /**
     * Init TX.
     */
    DNX_INIT_STEP_TX,
    /**
     * Restore Soc Control.
     */
    DNX_INIT_STEP_RESTORE_SOC_CONTROL,
    /**
     * mark driver as not inited
     */
    DNX_INIT_STEP_MARK_NOT_INITED,
    /**
     * init Done step, used to update relevant modules that the init
     * was done
     */
    DNX_INIT_STEP_INIT_DONE,
    /**
     * Configuration of the endianness
     */
    DNX_INIT_STEP_ENDIANNESS_CONFIG,
    /**
     * Use polling for s-bus access
     */
    DNX_INIT_STEP_POLLING,
    /**
     * Rings s-bus and broadcast blocks
     */
    DNX_INIT_STEP_RING_CONFIG,
    /**
     * Configuring s-bus timeout and clear errors
     */
    DNX_INIT_STEP_SBUS_TIMEOUT,
    /**
     * Iproc/PAXB configuration not configured earlier
     */
    DNX_INIT_STEP_IPROC,
    /**
     * Making Hard Reset
     */
    DNX_INIT_STEP_HARD_RESET,
    /**
     * Verifying core clock values.
     */
    DNX_INIT_STEP_CORE_CLOCK_VERIFY,
            /**
     * Configuration of SBUS
     */
    DNX_INIT_STEP_SBUS,
    /**
     * Configuration of the Soft reset
     */
    DNX_INIT_STEP_SOFT_RESET,
    /**
     * DMA init/deinit
     */
    DNX_INIT_STEP_DMA,
    /**
     * Configuration of the PLLs
     */
    DNX_INIT_STEP_PLL_CONFIG,
    /**
     * Move s-bus access interrupts to be polled
     */
    DNX_INIT_STEP_SBUS_INTERRUPTS,
    /**
     * Init of the BIST tests
     */
    DNX_INIT_STEP_BIST_TESTS,
    /**
     * Set memory write masks to not mask
     */
    DNX_INIT_STEP_MEM_MASKS,
    /**
     * Set the Broadcast configuration
     */
    DNX_INIT_STEP_BRDC_CONFIG,
    /**
     * Pre-Sofr inializtion
     */
    DNX_INIT_STEP_PRE_SOFT_INIT,
    /**
     * Making the Soft Init
     */
    DNX_INIT_STEP_SOFT_INIT,
    /**
     * init access step list
     */
    DNX_INIT_STEP_ACCESS,
    /**
     * init TS, BS, NIF and Fabric PLL
     */
    DNX_INIT_STEP_PLL,
    /**
     * init Vlan Algo Res
     */
    DNX_INIT_STEP_VLAN_ALGO_RES,
    /**
     * init STG Algo Res
     */
    DNX_INIT_STEP_ALGO_STG,
    /**
     * init QOS Algo
     */
    DNX_INIT_STEP_ALGO_QOS,
    /**
     * init TUNNEL Algo
     */
    DNX_INIT_STEP_ALGO_TUNNEL,
    /**
     * Init algo instru
     */
    DNX_INIT_STEP_ALGO_INSTRU,
    /**
     * init PP Multicast Algo
     */
    DNX_INIT_STEP_ALGO_PPMC,
    /**
     * init SAT Algo Res
    */
    DNX_INIT_STEP_ALGO_SAT,
    /**
     * Init algo template.
     */
    DNX_INIT_STEP_ALGO_TEMPLATE,
    /**
     * init HW overwrites
     */
    DNX_INIT_STEP_HW_OVERWRITE,
    /**
     * init PEMLA
     */
    DNX_INIT_STEP_PEMLA,
    /**
    * init ARR
    */
    DNX_INIT_STEP_ARR,
    /**
     * Init Utilties modules
     */
    DNX_INIT_STEP_UTILITIES,
    /**
     * Init Algo modules
     */
    DNX_INIT_STEP_ALGO,
    /**
     * Legacy soc modules list
     */
    DNX_INIT_STEP_SOC_MODULES,
    /**
     * Init algo LIF module.
     */
    DNX_INIT_STEP_ALGO_LIF,
    /**
     * init L3 Algo Res
     */
    DNX_INIT_STEP_ALGO_L3,
    /**
     * init L2 Algo
     */
    DNX_INIT_STEP_ALGO_L2,
    /**
     * init IN_LIF_PROFILE Algo Res
     */
    DNX_INIT_STEP_ALGO_IN_LIF_PROFILE,
    /**
     * init OUT_LIF_PROFILE Algo Res
     */
    DNX_INIT_STEP_ALGO_OUT_LIF_PROFILE,
    /**
     * init L2 module
     */
    DNX_INIT_STEP_L2_MODULE,
    /**
     * init L3 module
     */
    DNX_INIT_STEP_L3_MODULE,
    /**
     * init switch module
     */
    DNX_INIT_STEP_SWITCH_MODULE,
    /**
     * init in_lif_profile module
     */
    DNX_INIT_STEP_IN_LIF_PROFILE_MODULE,
    /**
     * init out_lif_profile module
     */
    DNX_INIT_STEP_OUT_LIF_PROFILE_MODULE,
    /**
     * init PORT_INGRESS module
     */
    DNX_INIT_STEP_PORT_INGRESS_MODULE,
    /**
     * init MIRROR module
     */
    DNX_INIT_STEP_MIRROR_MODULE,
    /**
     * Init LIF module.
     */
    DNX_INIT_STEP_LIF,
    /**
     * Appl properties - used to set properties values through APIs.
     */
    DNX_INIT_STEP_APPL_PROPERTIES,
    /**
     * init PP PORT module
     */
    DNX_INIT_STEP_PP_PORT,
    /**
     * init misc. access data structures - signals, regs/mems mcm ad-ons
     */
    DNX_INIT_STEP_AUX_ACCESS,
    /**
     * initialize visibility settings
     */
    DNX_INIT_STEP_VISIBILITY,
    /**
     * init all UTILEX services
     */
    DNX_INIT_STEP_UTILEX,
    /**
     * init VLAN module
     */
    DNX_INIT_STEP_VLAN,
    /**
     * init SRv6 module
     */
    DNX_INIT_STEP_SRV6,
    /**
     * init STG module
     */
    DNX_INIT_STEP_STG,
    /**
     * init QOS module
     */
    DNX_INIT_STEP_QOS,
    /**
     * init Time module
     */
    DNX_INIT_STEP_TIME,

    /**
     * init PTP module
     */
    DNX_INIT_STEP_PTP,
    /**
     * init OAM module
     */
    DNX_INIT_STEP_OAM,
    /**
     * init BFD module
     */
    DNX_INIT_STEP_BFD,
    /**
     * init PP module
     */
    DNX_INIT_STEP_PP_GENERAL,

    /**
    * init MPLS module
     */
    DNX_INIT_MPLS,
    /**
    * init CRPS module
     */
    DNX_INIT_STEP_CRPS_MODULE,
    /**
    * init Meter module
     */
    DNX_INIT_STEP_POLICER_MODULE,
    /**
    * init SAT module
    */
    DNX_INIT_STEP_SAT_MODULE,
    /**
    * init STIF module
     */
    DNX_INIT_STEP_STIF_MODULE,
    /**
    * init Ingress Congestion module
     */
    DNX_INIT_STEP_COSQ_INGRESS_MODULE,
    /**
    * init cosq latency module
     */
    DNX_INIT_STEP_COSQ_LATENCY_MODULE,
    /**
    * init Flow Control module
     */
    DNX_INIT_STEP_FLOW_CONTROL_MODULE,
    /**
    * init PFC Deadlock module
     */
    DNX_INIT_STEP_PFC_DEADLOCK_MODULE,
    /**
    * init E2E scheduler module
     */
    DNX_INIT_STEP_SCH_MODULE,
    /**
    * init Fabric transmit module
     */
    DNX_INIT_STEP_FABRIC_TRANSMIT_MODULE,
    /**
    * init Fabric module
     */
    DNX_INIT_STEP_FABRIC_MODULE,
    /**
    * init Mib module
     */
    DNX_INIT_STEP_MIB_MODULE,
    /**
    * init Ecgm module
    */
    DNX_INIT_STEP_EGRESS_CONGESTION_MODULE,
    /**
    * init Trunk module
     */
    DNX_INIT_STEP_TRUNK_MODULE,
    /**
    * init MACSec/IPSec module
     */
    DNX_INIT_STEP_SEC_MODULE,
    /**
     * Algo Lane Map module
     */
    DNX_INIT_STEP_ALGO_LANE_MAP,
    /**
     * Algo Port module
     */
    DNX_INIT_STEP_ALGO_PORT,
    /**
     * Algo Port IMB module
     */
    DNX_INIT_STEP_ALGO_PORT_IMB,
    /**
     * Algo Port PP module
     */
    DNX_INIT_STEP_ALGO_PORT_PP,
    /**
     * init DBAL general Module
     */
    DNX_INIT_STEP_DBAL_GENERAL,
    /**
     * init DBAL fields
     */
    DNX_INIT_STEP_DBAL_FIELDS,
    /**
     * init DBAL tables
     */
    DNX_INIT_STEP_DBAL_TABLES,
    /**
     * init DBAL MDB access layer
     */
    DNX_INIT_STEP_DBAL_ACCESS_MDB,
    /**
     * init DBAL PEMLA access layer
     */
    DNX_INIT_STEP_DBAL_ACCESS_PEMLA,
    /**
     * init DBAL SW access layer
     */
    DNX_INIT_STEP_DBAL_ACCESS_SW,
    /**
     * Set lane map (convert ucode_port property to BCM api call)
     */
    DNX_INIT_STEP_DYN_PORT_LANE_MAP_SET,
    /**
     * Operations to do after DBAL is done with init
     */
    DNX_INIT_STEP_DBAL_POST_OPERATIONS,
    /**
     * Power Up PHYs (for Heat up mode only)
     */
    DNX_INIT_STEP_PHY_POWER_UP,
    /**
     * Add ports (convert ucode_port property to BCM api call)
     */
    DNX_INIT_STEP_DYN_PORT_ADD,
    /**
     * Set port properties (convert soc properties to BCM api call)
     */
    DNX_INIT_STEP_DYN_PORT_PROP_SET,
    /**
     * Used as a temp step to add ports until dynamic port will be implemented
     */
    DNX_INIT_STEP_DYN_PORT_TEMP,
    /**
     * init STK_SYS module
     */
    DNX_INIT_STEP_STK_SYS_MODULE,
    /**
     * init IPQ module
     */
    DNX_INIT_STEP_IPQ_MODULE,
     /**
     * init IQS module
     */
    DNX_INIT_STEP_IQS_MODULE,
     /**
     * init PVT mon
     */
    DNX_INIT_STEP_PVT_MON,
    /**
     * init Instrumentation mon
     */
    DNX_INIT_STEP_INSTRU,
    /**
     * init Reflector
     */
    DNX_INIT_STEP_REFLECTOR,
    /**
     * init Eventor
     */
    DNX_INIT_STEP_EVENTOR,
    /**
     * init MULTICAST module
     */
    DNX_INIT_STEP_MULTICAST_MODULE,
    /**
     * init TDM module
     */
    DNX_INIT_STEP_TDM_MODULE,
    /**
     * init BIER module
     */
    DNX_INIT_STEP_BIER_MODULE,
    /**
     * init RX Algo Res
     */
    DNX_INIT_STEP_ALGO_RX,
    /**
     * init Field module
     */
    DNX_INIT_STEP_FIELD,
    /**
     * init Field module
     */
    DNX_INIT_STEP_ALGO_FIELD,
    /**
     * init TDM module
     */
    DNX_INIT_STEP_ALGO_TDM,
    /**
     * init oam module
     */
    DNX_INIT_STEP_ALGO_OAM,
    /**
     * init BFD module
     */
    DNX_INIT_STEP_ALGO_BFD,
    /**
     * init OAMP module
     */
    DNX_INIT_STEP_ALGO_OAMP,
    /**
     * init MPLS Algo module
     */
    DNX_INIT_STEP_ALGO_MPLS,
    /**
     * init VLAN Algo module
     */
    DNX_INIT_STEP_ALGO_VLAN,
    /**
     * init EGR_POINTED Algo module
     */
    DNX_INIT_STEP_ALGO_EGR_POINTED,
    /**
     * init Shell Commands Data
     */
    DNX_INIT_STEP_SH_CMD,
    /**
     * init Shell Commands Data for dnx tree
     */
    DNX_INIT_STEP_SH_CMD_DNX,
    /**
     * init Shell Commands Data for ctest tree
     */
    DNX_INIT_STEP_SH_CMD_CTEST,
    /**
     * init IMB (Interface Managment Block) module. include NIF and
     * Fabric modules.
     */
    DNX_INIT_STEP_PORT_MODULE,
    /**
     * init OFP rate module.
     * Fabric modules.
     */
    DNX_INIT_STEP_OFP_RATES_MODULE,
    /**
     * init EGR queuing module.
     * Fabric modules.
     */
    DNX_INIT_STEP_EGR_QUEUING_MODULE,
    /**
     * Configuration Tune step
     */
    DNX_INIT_STEP_TUNE,
    /**
     * Set mem defaults
     */
    DNX_INIT_STEP_MEM_DEFAULTS,
    /**
     * PEM init sequence
     */
    DNX_INIT_STEP_PEM_SEQ_INIT,
    /**
     * Set DTQs Contexts sizes
     */
    DNX_INIT_STEP_DTQS_CONTEXTS_INIT,
    /**
     * Disable traffic. Has to be done before HW access.
     * Currently it's called right after DBAL init since this step
     * Calls for APIs use DBAL (technically this step should be
     * called after access init).
     */
    DNX_INIT_STEP_DISABLE_TRAFFIC,
    /**
     * Disable data traffic
     */
    DNX_INIT_STEP_DISABLE_DATA_TRAFFIC,
    /**
     * Disable fabric control cells
     */
    DNX_INIT_STEP_DISABLE_CONTROL_CELLS,
    /**
     * Configuration of 'legacy' structures within DNX FIELD
     */
    DNX_INIT_STEP_FLD_LEGACY,
    /**
     * Configuration of 'mapping' structures (e.g., BCM actions to DNX actions) within DNX FIELD
     */
    DNX_INIT_STEP_FLD_MAPPING,
    /**
     * Configuration of 'sw state' structures (e.g., allocated keys)  within DNX FIELD. Mostly related to 'algo'.
     */
    DNX_INIT_STEP_FLD_SWSTATE,
    /**
     * Configuration of HW tables (e.g., FES tables, such as
     * DBAL_TABLE_FIELD_PMF_A_FES_2ND_INSTRUCTION) within DNX FIELD.
     */
    DNX_INIT_STEP_FLD_TABLES,
    /**
     * Configuration of default PMF programs within DNX FIELD.
     */
    DNX_INIT_STEP_FLD_PGMSET,
    /**
     * Configuration of the Field range tables.
     */
    DNX_INIT_STEP_FLD_RANGE,
    /**
     * Configuration of resources for TCAM manager and within DNX  FIELD.
     */
    DNX_INIT_STEP_FLD_TCAMFW,
    /**
     * Configuration of resources for TCAM Handlers and within DNX  FIELD.
     */
    DNX_INIT_STEP_FLD_HANDLERS,
    /**
     * Configuration of resources for Field Context.
     */
    DNX_INIT_STEP_FLD_CONTEXT,
    /**
     * Configuration of resources for Field Presel.
     */
    DNX_INIT_STEP_FLD_PRESEL,
    /**
     * Configuration of the pre-defined UDH types length.
     */
    DNX_INIT_STEP_FLD_UDH,
    /**
     * Configuration of the pre-defined system headers.
     */
    DNX_INIT_STEP_FLD_SYS_HDRS,
    /**
     * Configuration of resources for TCAM manager and within DNX  FIELD.
     */
    DNX_INIT_STEP_KBP_MNGR,
    /**
     * consistent hashing manager algo.
     */
    DNX_INIT_STEP_ALGO_CHM,
    /**
     * SRAM packet buffer.
     */
    DNX_INIT_STEP_SPB,
    /**
     * Dram init.
     */
    DNX_INIT_STEP_DRAM,
    /**
     * Failover (Protection) module
     */
    DNX_INIT_STEP_FAILOVER,
    /**
     * Algo for Failover module
     */
    DNX_INIT_STEP_ALGO_FAILOVER,
    /**
     * Family
     */
    DNX_INIT_STEP_FAMILY,
    /**
     * Algo for pp stat
     */
    DNX_INIT_STEP_ALGO_STAT_PP,
    /**
     * Algo for ptp
     */
    DNX_INIT_STEP_ALGO_PTP,
    /**
     * Initialize the descriptor DMA based on the soc properties.
     */
    DNX_INIT_SBUSDMA_DESC,
    /**
     * Initialize all the MDB registers according to the MDB profile, architecture specifications and device parameters.
     */
    DNX_INIT_MDB,
    /**
     * Dram buffers step.
     */
    DNX_INIT_STEP_DRAM_BUFFERS,
    /**
     * Interrupts init
     */
    DNX_INIT_STEP_INTERRUPT,
    /**
     * SER interrupt handler
     */
    DNX_INIT_STEP_SER,
    /**
     * Shadow memory init
     */
    DNX_INIT_STEP_SHADOW_MEMORY,
    /*
     *  Set memory cacheable
     */
    DNX_INIT_STEP_MEM_CACHEABLE,
    /*
     *  Linkscan
     */
    DNX_INIT_STEP_LINKSCAN,
    /*
     *  Dram temperature monitoring
     */
    DNX_INIT_STEP_TEMPERATURE_MONITOR,
    /*
     * Coounter Processor Eviction
     */
    DNX_INIT_STEP_CRPS_EVICTION,
    /*
     *  MIB thread
     */
    DNX_INIT_STEP_MIB_THREAD,
    /*
     *  PortMod thread
     */
    DNX_INIT_STEP_PORTMOD_THREAD,
    /*
     *  Fabric load balance thread
     */
    DNX_INIT_STEP_FABRIC_LOAD_BALANCE,
    /*
     *  RCPU
     */
    DNX_INIT_STEP_RCPU,
    /**
     * stat_pp init.
     */
    DNX_INIT_STEP_STAT_PP,
    /**
     * SyncE init.
     */
    DNX_INIT_STEP_SYNCE,
    /**
     * ALGO SyncE.
     */
    DNX_INIT_STEP_ALGO_SYNCE,
    /**
     * Threads init.
     */
    DNX_INIT_STEP_THREADS,
    /**
     * Kleap info init.
     */
    DNX_INIT_KLEAP_INFO,
    /**
     * Configure mem monitor mask.
     */
    DNX_INIT_STEP_MEM_MASK,
    /**
     * ALGO FlexE Group.
     */
    DNX_INIT_STEP_ALGO_FLEXE_GENERAL,
    /**
     * ALGO FlexE core.
     */
    DNX_INIT_STEP_ALGO_FLEXE_CORE,
    /**
     * GDDR6 dram.
     */
    DNX_INIT_STEP_GDDR6,
    /**
     * FlexE init.
     */
    DNX_INIT_STEP_FLEXE,
    /**
     * ALGO Switch init
     */
    DNX_INIT_STEP_ALGO_SWITCH,
    /**
     *  L2 Flush Traverse thread
     */
    DNX_INIT_STEP_L2_FLUSH_TRAVERSE_THREAD,
    /**
     *  Enable DRAM blocks
     */
    DNX_INIT_STEP_DRAM_BLOCKS_ENABLE,
    /**
    * init Port Interrupts
     */
    DNX_INIT_STEP_PORT_INTERRUPTS,
    /**
     * Interrupt event thread
     */
    DNX_INIT_STEP_INTR_EVT_THREAD,
    /**
     * Clear interrupts and counter
     */
    DNX_INIT_STEP_CLEAR_COUNTER,
    /**
     * Number of init steps, must be before last.
     */
    DNX_INIT_STEP_COUNT,
    /**
     * flow init.
     */
    DNX_INIT_STEP_FLOW,
    /**
     * Dummy step, used to indicate the last step for a step list.
     */
    DNX_INIT_STEP_LAST_STEP = UTILEX_SEQ_STEP_LAST
} dnx_init_step_id_e;

/**
 * \brief ptr to step cb function returning shr_error_e, to be
 * used in each init/deinit step. the step_list_info and the
 * flags are used for internal purposes, and are managed by the
 * mechanism.
 */
typedef shr_error_e(
    *dnx_step_cb) (
    int unit);

/**
 * \brief ptr to step flag cb function returning
 * shr_error_e, to be used in each init step to decode flags
 * according to a given step logic ( for example Step X should
 * be skipped if SOC property Y is set to value Z etc). to do
 * so, one would need to return in dynamic flags the
 * DNX_INIT_STEP_F_SKIP_DYNAMIC flag.
 */
typedef shr_error_e(
    *dnx_step_flag_cb) (
    int unit,
    int *dynamic_flags);

/**
 * \brief dnx init arguments structure.
 *
 * This structure should contain user's information.
 * most of the needed info however should be passed as SOC properties.
 * currently it is used to define a stopping and continuation points
 */
typedef struct
{
  /**
   * this is used to start the init sequence from a certain step.
   * this will be used after stopping the init sequence with last
   * step. the default for this should be the
   * DNX_INIT_STEP_INVALID_STEP step id.
   */
    dnx_init_step_id_e first_step;

  /**
   * this is used to stop the init sequence at a certain step.
   * to continue use the init function with first step defined as
   * next step of this. the default for this should be the
   * DNX_INIT_STEP_INVALID_STEP step id.
   */
    dnx_init_step_id_e last_step;
} dnx_init_info_t;
/*
 * Typedefs:
 * {
 */
/**
 * \brief step/sub-step structure.
 *
 * each step in the Init/Deinit process should be well defined.
 * each step should have an Init and Deinit function.
 * the Deinit function should not access the HW, just free SW resources.
 * a flag CB can also be used, so each step can activate flags according
 * to dedicated SOC properties or other logic.
 */
typedef struct dnx_init_step_s dnx_init_step_t;
struct dnx_init_step_s
{
  /**
   * Step ID, used to uniquely identify a step.
   */
    dnx_init_step_id_e step_id;

  /**
   * Step name
   */
    char *step_name;

  /**
   * Init function CB that will run during the Init Sequence.
   */
    dnx_step_cb init_function;

  /**
   * Deinit function CB that will run during the Deinit Sequence.
   */
    dnx_step_cb deinit_function;

  /**
   * Flag function CB that will run prior to the init function of
   * this step to determine which flags are needed to the step
   * according to the CB logic - could be looking for certain SOC
   * properties for example.
   */
    dnx_step_flag_cb flag_function;

  /**
   * Step Flags, internal flags used by the system's logic
   */
    int step_flags;

  /**
   * Pointer to a sub list of steps to be used if current step
   * represents a sub list of steps.
   */
    const dnx_init_step_t *step_sub_array;

    /*
     * Dependency marker for the step. It is used if multithread_en property is equal to 1.
     */
    dnx_init_step_id_e depends_on_step_id;
};

/**
 * \brief Structure holding information for steps and their relative time thresholds.
 *
 * This structure is used for steps with DNX_INIT_STEP_F_TIME_THRESH_EXPLICIT
 * flag in order to link the Step ID to the specific user defined time threshold.
 */
typedef struct
{
  /**
   * Step ID, used to uniquely identify a step.
   */
    dnx_init_step_id_e step_id;

  /**
   * Time threshold - the maximum time the step should take in
   * microseconds.
   */
    sal_time_t time_thresh;
} dnx_init_time_thresh_info_t;

/*
 * }
 */
/**
 * \brief - Deep conversion of dnx_init_step_t to utilex_seq_step_t
 *          The conversion allocate memory that should be freed using 'dnx_step_list_destroy()'
 */
shr_error_e dnx_init_step_list_convert(
    int unit,
    const dnx_init_step_t * dnx_step_list,
    utilex_seq_step_t ** step_list);

/**
 * \brief - Deep free of utilex_seq_step_t
 *          Should be called after 'dnx_init_step_list_convert()'
 */
shr_error_e dnx_init_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list);
/*
 * }
 */

/**
 * \brief - Get the time threshold of a given step in BCM init according to the
 *          step ID and the step fags. If called with flag UTILEX_SEQ_STEP_INVALID
 *          will return the overall threshold for the full sequence. In this case
 *          flags are not relevant.
 *
 * \param [in] unit - Unit #
 * \param [in] step_id - Step ID
 * \param [in] flags - Step flags
 * \param [out] time_thresh - Time threshold for the specified step
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_time_thresh_cb(
    int unit,
    int step_id,
    uint32 flags,
    sal_usecs_t * time_thresh);

/**
 * \brief - Get whether DNX BCM init sequence finished.
 *
 * \param [in] unit - Unit #
 * \return
 *   int - boolean value which denotes if init sequence finished.
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_init_is_init_done_get(
    int unit);

/**
 * \brief - re run init steps according to dnx_init_info.
 *
 * \param [in] unit - Unit #
 * \param [in] flags - dnx_init_run flags are supported.
 * \param [in] dnx_init_info - init info
 * \return
 *   shr_error_e.
 * \remark
 *   * the first_step provided in dnx_init_info provided has to be after the DNX_INIT_STEP_DBAL step
 * \see
 *   * None
 */
shr_error_e dnx_init_rerun(
    int unit,
    int flags,
    dnx_init_info_t * dnx_init_info);

/**
 * \brief - skip the step when performing wb test
 *
 * \param [in] unit - Unit #
 * \param [out] dynamic_flags - sequence dynamic flags
 * \return
 *   shr_error_e.
 * \remark
 * \see
 *   * None
 */
shr_error_e wb_test_mode_skip(
    int unit,
    int *dynamic_flags);

/*
 * }
 */
#endif /* _BCMINT_DNX_INIT_INIT_H_INCLUDED */
