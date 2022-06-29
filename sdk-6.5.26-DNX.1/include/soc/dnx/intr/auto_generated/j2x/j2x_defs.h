/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * JERICHO_2_X DEFS H
 */

#ifndef _SOC_J_2_X_DEFS_H_
#define _SOC_J_2_X_DEFS_H_

#define SOC_J2X_NOF_BLK 2048

#define SOC_J2X_NOF_INSTANCES_CMIC            1
#define SOC_J2X_NOF_INSTANCES_IPROC           1
#define SOC_J2X_NOF_INSTANCES_MACSEC          2
#define SOC_J2X_NOF_INSTANCES_CDPORT          8
#define SOC_J2X_NOF_INSTANCES_CDMAC           8
#define SOC_J2X_NOF_INSTANCES_CFC             1
#define SOC_J2X_NOF_INSTANCES_ECI             1
#define SOC_J2X_NOF_INSTANCES_EPNI            1
#define SOC_J2X_NOF_INSTANCES_FCR             1
#define SOC_J2X_NOF_INSTANCES_FCT             1
#define SOC_J2X_NOF_INSTANCES_FDR             1
#define SOC_J2X_NOF_INSTANCES_FDA             1
#define SOC_J2X_NOF_INSTANCES_FDT             1
#define SOC_J2X_NOF_INSTANCES_MESH_TOPOLOGY   1
#define SOC_J2X_NOF_INSTANCES_IPS             1
#define SOC_J2X_NOF_INSTANCES_IPT             1
#define SOC_J2X_NOF_INSTANCES_IQM             1
#define SOC_J2X_NOF_INSTANCES_IRE             1
#define SOC_J2X_NOF_INSTANCES_MSW             2
#define SOC_J2X_NOF_INSTANCES_OLP             1
#define SOC_J2X_NOF_INSTANCES_RTP             1
#define SOC_J2X_NOF_INSTANCES_SCH             1
#define SOC_J2X_NOF_INSTANCES_OCB             4
#define SOC_J2X_NOF_INSTANCES_CRPS            1
#define SOC_J2X_NOF_INSTANCES_FMAC            16
#define SOC_J2X_NOF_INSTANCES_CGM             1
#define SOC_J2X_NOF_INSTANCES_OAMP            1
#define SOC_J2X_NOF_INSTANCES_FSRD            8
#define SOC_J2X_NOF_INSTANCES_BRDC_FMAC       1
#define SOC_J2X_NOF_INSTANCES_BRDC_FSRD       1
#define SOC_J2X_NOF_INSTANCES_BRDC_HBC        1
#define SOC_J2X_NOF_INSTANCES_MRPS            1
#define SOC_J2X_NOF_INSTANCES_EDB             1
#define SOC_J2X_NOF_INSTANCES_KAPS            1
#define SOC_J2X_NOF_INSTANCES_PEM             4
#define SOC_J2X_NOF_INSTANCES_SPB             1
#define SOC_J2X_NOF_INSTANCES_SQM             1
#define SOC_J2X_NOF_INSTANCES_DDP             1
#define SOC_J2X_NOF_INSTANCES_DQM             1
#define SOC_J2X_NOF_INSTANCES_ECGM            1
#define SOC_J2X_NOF_INSTANCES_MTM             1
#define SOC_J2X_NOF_INSTANCES_PDM             1
#define SOC_J2X_NOF_INSTANCES_SIF             1
#define SOC_J2X_NOF_INSTANCES_BDM             1
#define SOC_J2X_NOF_INSTANCES_ILU             1
#define SOC_J2X_NOF_INSTANCES_IPPA            1
#define SOC_J2X_NOF_INSTANCES_IPPB            1
#define SOC_J2X_NOF_INSTANCES_IPPC            1
#define SOC_J2X_NOF_INSTANCES_IPPD            1
#define SOC_J2X_NOF_INSTANCES_ITPP            1
#define SOC_J2X_NOF_INSTANCES_ITPPD           1
#define SOC_J2X_NOF_INSTANCES_MACT            1
#define SOC_J2X_NOF_INSTANCES_MCP             1
#define SOC_J2X_NOF_INSTANCES_MDB             1
#define SOC_J2X_NOF_INSTANCES_RQP             1
#define SOC_J2X_NOF_INSTANCES_EPS             1
#define SOC_J2X_NOF_INSTANCES_ERPP            1
#define SOC_J2X_NOF_INSTANCES_ETPPA           1
#define SOC_J2X_NOF_INSTANCES_ETPPB           1
#define SOC_J2X_NOF_INSTANCES_FQP             1
#define SOC_J2X_NOF_INSTANCES_PQP             1
#define SOC_J2X_NOF_INSTANCES_HBMC            1
#define SOC_J2X_NOF_INSTANCES_DDHB            4
#define SOC_J2X_NOF_INSTANCES_EVNT            1
#define SOC_J2X_NOF_INSTANCES_TCAM            1
#define SOC_J2X_NOF_INSTANCES_TDU             1
#define SOC_J2X_NOF_INSTANCES_IPPE            1
#define SOC_J2X_NOF_INSTANCES_HBC             8
#define SOC_J2X_NOF_INSTANCES_ETPPC           1
#define SOC_J2X_NOF_INSTANCES_EPRE            1
#define SOC_J2X_NOF_INSTANCES_IPPF            1
#define SOC_J2X_NOF_INSTANCES_MDB_ARM         1
#define SOC_J2X_NOF_INSTANCES_FDTL            1
#define SOC_J2X_NOF_INSTANCES_DDHA            1
#define SOC_J2X_NOF_INSTANCES_ILE             1
#define SOC_J2X_NOF_INSTANCES_ESB             1
#define SOC_J2X_NOF_INSTANCES_FSAR            1
#define SOC_J2X_NOF_INSTANCES_CDPM            8
#define SOC_J2X_NOF_INSTANCES_OCBM            1
#define SOC_J2X_NOF_INSTANCES_ARB             1
#define SOC_J2X_NOF_INSTANCES_FAWR            1
#define SOC_J2X_NOF_INSTANCES_FAWT            1
#define SOC_J2X_NOF_INSTANCES_FBSW            1
#define SOC_J2X_NOF_INSTANCES_FDCRL           1
#define SOC_J2X_NOF_INSTANCES_FDCRS           1
#define SOC_J2X_NOF_INSTANCES_FDMP            1
#define SOC_J2X_NOF_INSTANCES_FDMX            2
#define SOC_J2X_NOF_INSTANCES_FDMXA           1
#define SOC_J2X_NOF_INSTANCES_FDMXC           1
#define SOC_J2X_NOF_INSTANCES_FDMXT           1
#define SOC_J2X_NOF_INSTANCES_FECRL           1
#define SOC_J2X_NOF_INSTANCES_FECRS           1
#define SOC_J2X_NOF_INSTANCES_FFLXO           1
#define SOC_J2X_NOF_INSTANCES_FFOA            1
#define SOC_J2X_NOF_INSTANCES_FFOB            1
#define SOC_J2X_NOF_INSTANCES_FGDMP           1
#define SOC_J2X_NOF_INSTANCES_FGMAP           1
#define SOC_J2X_NOF_INSTANCES_FLEXMAC         1
#define SOC_J2X_NOF_INSTANCES_FLFR            4
#define SOC_J2X_NOF_INSTANCES_FLFT            2
#define SOC_J2X_NOF_INSTANCES_FLOTN           1
#define SOC_J2X_NOF_INSTANCES_FMACR           1
#define SOC_J2X_NOF_INSTANCES_FMACT           1
#define SOC_J2X_NOF_INSTANCES_FMAP            1
#define SOC_J2X_NOF_INSTANCES_FMX             2
#define SOC_J2X_NOF_INSTANCES_FMXBI           1
#define SOC_J2X_NOF_INSTANCES_FMXBJ           1
#define SOC_J2X_NOF_INSTANCES_FMXC            1
#define SOC_J2X_NOF_INSTANCES_FMXT            1
#define SOC_J2X_NOF_INSTANCES_FOAM            2
#define SOC_J2X_NOF_INSTANCES_FODUO           2
#define SOC_J2X_NOF_INSTANCES_FOSW            1
#define SOC_J2X_NOF_INSTANCES_FPMR            1
#define SOC_J2X_NOF_INSTANCES_FPMT            1
#define SOC_J2X_NOF_INSTANCES_FPTPR           1
#define SOC_J2X_NOF_INSTANCES_FPTPT           1
#define SOC_J2X_NOF_INSTANCES_FRA             2
#define SOC_J2X_NOF_INSTANCES_FRWA            1
#define SOC_J2X_NOF_INSTANCES_FRWBC           1
#define SOC_J2X_NOF_INSTANCES_OFR             1
#define SOC_J2X_NOF_INSTANCES_OFT             1
#define SOC_J2X_NOF_INSTANCES_FECPB           1
#define SOC_J2X_NOF_INSTANCES_FMXBK           1
#define SOC_J2X_NOF_INSTANCES_WB              1
#define SOC_J2X_NOF_INSTANCES_FICPB           1
#define SOC_J2X_NOF_INSTANCES_FDMXBJ          1
#define SOC_J2X_NOF_INSTANCES_W40             1
#define SOC_J2X_NOF_INSTANCES_FMXAJ           1
#define SOC_J2X_NOF_INSTANCES_TSN             1
#define SOC_J2X_NOF_INSTANCES_FMXAI           1
#define SOC_J2X_NOF_INSTANCES_FDMXBI          1

#endif
