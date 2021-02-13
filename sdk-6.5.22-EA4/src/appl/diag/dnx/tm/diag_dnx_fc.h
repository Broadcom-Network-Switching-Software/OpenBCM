/** \file diag_dnx_fc.h
 * 
 * DNX TM Flow Control diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_FC_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FC_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

 /**
  * \brief - Enumerator of the FC types for CFC Flow Control diagnostic command
  */
typedef enum
{
/** Type is invalid */
    DNX_FC_TYPE_INVALID = -1,
/** First FC type */
    DNX_FC_TYPE_FIRST = 0,
/** Type is FC generated by the Global Resources */
    DNX_FC_TYPE_GLB_RSC = DNX_FC_TYPE_FIRST,
/** Type is FC generated by Almost Full NIF */
    DNX_FC_TYPE_AF_NIF = 1,
/** Type is FC generated by ILKN Almost Full NIF */
    DNX_FC_TYPE_ILKN_AF_NIF = 2,
/** Type is FC signal from CFC to an ILKN port in order to transmit LLFC */
    DNX_FC_TYPE_LLFC_ILKN = 3,
/** Type is FC signal from CFC to an NIF port in order to transmit LLFC */
    DNX_FC_TYPE_LLFC_NIF = 4,
/** Type is FC signal from CFC to an NIF port in order to transmit PFC */
    DNX_FC_TYPE_PFC_NIF = 5,
/** Type is FC generated by VSQ_A */
    DNX_FC_TYPE_VSQ_A = 6,
/** Type is FC generated by VSQ_B*/
    DNX_FC_TYPE_VSQ_B = 7,
/** Type is FC generated by VSQ_C */
    DNX_FC_TYPE_VSQ_C = 8,
/** Type is FC generated by VSQ_D */
    DNX_FC_TYPE_VSQ_D = 9,
/** Type is FC generated by VSQ_LLFC */
    DNX_FC_TYPE_VSQ_LLFC = 10,
/** Type is FC generated by VSQ_PFC */
    DNX_FC_TYPE_VSQ_PFC = 11,
/** Type is Q-pair based FC received by the EGQ */
    DNX_FC_TYPE_FC_EGQ = 12,
/** Type is PFC received by the NIF */
    DNX_FC_TYPE_NIF_PFC = 13,
/** Type is Q-pair based FC (to stop a single Q-pair) received by the SPI OOB */
    DNX_FC_TYPE_OOB_SINGLE = 14,
/** Type is Q-pair based FC (to stop all Q-pairs of a port) received by the SPI OOB */
    DNX_FC_TYPE_OOB_PORT_BASED = 15,
/** Type is NIF based FC received by the SPI OOB */
    DNX_FC_TYPE_OOB_LLFC = 16,
/** Type is Generic PFC bitmap used by the SPI OOB */
    DNX_FC_TYPE_OOB_GEN_BMP = 17,
/** Type is FC received by the ILKN OOB */
    DNX_FC_TYPE_ILKN_OOB = 18,
/** Type is FC received by the PP COE OOB */
    DNX_FC_TYPE_PP_COE = 19,
/** Type is FC received by NIF port in order to stop the PM */
    DNX_FC_TYPE_NIF_STOP_PM = 20,
/** Type is FC received by ILKN port in order to stop the PM */
    DNX_FC_TYPE_ILKN_STOP_PM = 21,
/** Type is FC received by the Inband ILKN Calendar */
    DNX_FC_TYPE_INB_ILKN = 22,
/** Number of types */
    DNX_FC_TYPE_COUNT
} dnx_fc_type_e;

/**
 * \brief - Enumerator of the FC types for NIF Flow Control diagnostic command
 */
typedef enum
{
/** Type is invalid */
    DNX_FC_NIF_TYPE_INVALID = -1,
/** First FC type */
    DNX_FC_NIF_TYPE_FIRST = 0,
/** LLFC to Port Macro */
    DNX_FC_NIF_TYPE_LLFC_TO_PM = DNX_FC_NIF_TYPE_FIRST,
/** LLFC was from CFC */
    DNX_FC_NIF_TYPE_LLFC_FROM_CFC = 1,
/** PFC to Port Macro */
    DNX_FC_NIF_TYPE_PFC_TO_PM = 2,
/** PFC  from CFC */
    DNX_FC_NIF_TYPE_PFC_FROM_CFC = 3,
/** PFC from QMLF */
    DNX_FC_NIF_TYPE_PFC_FROM_QMLF = 4,
/** LLFC from QMLF */
    DNX_FC_NIF_TYPE_LLFC_FROM_QMLF = 5,
/** LLFC to Port Macro for reception */
    DNX_FC_NIF_TYPE_STOP_TO_PM = 6,
/** LLFC was from CFC for reception */
    DNX_FC_NIF_TYPE_STOP_FROM_CFC = 7,
/** PFC from Port Macro */
    DNX_FC_NIF_TYPE_PFC_FROM_PM = 8,
/** PFC sent to CFC */
    DNX_FC_NIF_TYPE_PFC_TO_CFC = 9,
/** LLFC sent to ILKN */
    DNX_FC_NIF_TYPE_LLFC_TO_ILKN = 10,
/** Number of types */
    DNX_FC_NIF_TYPE_COUNT
} dnx_fc_nif_type_e;

/**
 * \brief - Enumerator of the FC types for IPS Flow Control diagnostic command
 */
typedef enum
{
/** Type is invalid */
    DNX_FC_IPS_TYPE_INVALID = -1,
/** First FC type */
    DNX_FC_IPS_TYPE_FIRST = 0,
/** Type is FC to SRAM Delete Dequeue Command Queues */
    DNX_FC_IPS_TYPE_DELS = DNX_FC_IPS_TYPE_FIRST,
/** Type is FC to DRAM Delete Dequeue Command Queue */
    DNX_FC_IPS_TYPE_DELD = 1,
/** Type is FC to SRAM-To-DRAM Dequeue Command Queue */
    DNX_FC_IPS_TYPE_S2D = 2,
/** Type is FC to OCB-Only Dequeue Command Queues*/
    DNX_FC_IPS_TYPE_OCBO = 3,
/** Type is FC to Dequeue Command Queues of traffic SRAM to fabric that is not mixed with DRAM */
    DNX_FC_IPS_TYPE_SRAM = 4,
/** Type is FC to Dequeue Command Queues of traffic SRAM to fabric that is mixed with DRAM */
    DNX_FC_IPS_TYPE_MIXS = 5,
/** Type is FC to Dequeue Command Queues of traffic DRAM to fabric that is mixed with SRAM */
    DNX_FC_IPS_TYPE_MIXD = 6,
/** Number of types */
    DNX_FC_IPS_TYPE_COUNT
} dnx_fc_ips_type_e;

/**
* \brief - Enumerator of the OOB interfaces
*/
typedef enum
{
/** Type is invalid */
    DNX_FC_OOB_IF_INVALID = -1,
/** First OOB interface */
    DNX_FC_OOB_IF_FIRST = 0,
/** OOB interface is 0 */
    DNX_FC_OOB_IF_0 = DNX_FC_OOB_IF_FIRST,
/** OOB interface is 1 */
    DNX_FC_OOB_IF_1 = 1,
/** Number of OOB interfaces */
    DNX_FC_OOB_IF_COUNT
} dnx_fc_oob_if_e;

/**
* \brief - Enumerator of the directions of FC
*/
typedef enum
{
/** Type is invalid */
    DNX_FC_DIRECTION_INVALID = -1,
/** Direction is Generation */
    DNX_FC_DIRECTION_FIRST = 0,
/** OOB interface is 0 */
    DNX_FC_DIRECTION_GENERATION = DNX_FC_DIRECTION_FIRST,
/** OOB interface is 1 */
    DNX_FC_DIRECTION_RECEPTION = 1,
/** Number of OOB interfaces */
    DNX_FC_DIRECTION_COUNT
} dnx_fc_direction_e;

/**
* \brief - Enumerator of the Global resources indexes
*/
typedef enum
{
/** Global resource is invalid */
    DNX_FC_GLB_RSC_INVALID = -1,
/** First Global resource */
    DNX_FC_GLB_RSC_FIRST = 0,
/** Global resource is Total DRAM low priority */
    DNX_FC_DRAM_TOTAL_LOW_PRIO = DNX_FC_GLB_RSC_FIRST,
/** Global resource is Total DRAM high priority */
    DNX_FC_DRAM_TOTAL_HIGH_PRIO = 1,
/** Global resource is Total SRAM low priority */
    DNX_FC_SRAM_TOTAL_LOW_PRIO = 2,
/** Global resource is Total SRAM high priority */
    DNX_FC_SRAM_TOTAL_HIGH_PRIO = 3,
/** Global resource is DRAM Pool 0 low priority */
    DNX_FC_DRAM_POOL_0_LOW_PRIO = 4,
/** Global resource is DRAM Pool 0 high priority */
    DNX_FC_DRAM_POOL_0_HIGH_PRIO = 5,
/** Global resource is DRAM Pool 1 low priority */
    DNX_FC_DRAM_POOL_1_LOW_PRIO = 6,
/** Global resource is DRAM Pool 1 high priority */
    DNX_FC_DRAM_POOL_1_HIGH_PRIO = 7,
/** Global resource is SRAM Pool 0 low priority */
    DNX_FC_SRAM_POOL_0_LOW_PRIO = 8,
/** Global resource is SRAM Pool 0 high priority */
    DNX_FC_SRAM_POOL_0_HIGH_PRIO = 9,
/** Global resource is SRAM Pool 1 low priority */
    DNX_FC_SRAM_POOL_1_LOW_PRIO = 10,
/** Global resource is SRAM Pool 1 high priority */
    DNX_FC_SRAM_POOL_1_HIGH_PRIO = 11,
/** Global resource is Total HDRM high priority */
    DNX_FC_HDRM_TOTAL_HIGH_PRIO = 12,
/** Number of Global Resources */
    DNX_FC_GLB_RSC_COUNT
} dnx_fc_glb_rsc_e;

extern sh_sand_cmd_t sh_dnx_fc_cmds[];
extern sh_sand_man_t sh_dnx_fc_man;

#endif /** DIAG_DNX_FC_H_INCLUDED */
