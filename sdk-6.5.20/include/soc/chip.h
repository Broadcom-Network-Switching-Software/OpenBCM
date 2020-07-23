/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        chip.h
 * Purpose:     Defines for chip types, etc.
 * Requires:    soctypes.h, memory, register and feature defs.
 *
 * System on Chip (SOC) basic structures and typedefs
 * Each chip consists of a number of blocks.  Each block can be
 * a port interface controller (PIC) that contains ports.
 * The mcm/bcm*.i files contain definitions of each block and port
 * in the associated chip.  The structures used in those files
 * are defined here.  They are used to build internal use data
 * structures defined in soc/drv.h
 */

#ifndef _SOC_CHIP_H
#define _SOC_CHIP_H

#include <soc/types.h>
#include <soc/memory.h>
#include <soc/register.h>
#include <soc/feature.h>

/*
 * Arrays of soc_block_info_t are built by the registers program.
 * Each array is indexed by block instance number (per chip).
 * The last entry has type -1.
 * Used in bcm*.i files.
 */
typedef struct {
    int    type;    /* block type: SOC_BLK_* */
    int    number;  /* block instance number of block type */
    int    schan;   /* pic number for schan commands */
    int    cmic;    /* pic number for cmic r/w commands */
} soc_block_info_t;

/* Structure describing the instances og a block type. Containing block ID of the first instance and number of block instances */
typedef struct soc_block_instances_s {
    uint32 nof_block_instance;  /* Number of the block instances in a single block */
    int first_blk_instance;  /* Block ID of the first block instance */
}soc_block_instances_t;

/*
 * Arrays of soc_port_info_t are built by the registers program.
 * Each array is indexed by port number (per chip).
 * Unused ports have blk -1 and bindex 0
 * The last entry has blk -1 and bindex -1
 * Used in bcm*.i files.
 */
typedef struct {
    int     blk;    /* index into soc_block_info array */
    int     bindex; /* index of port within block */
} soc_port_info_t;

/*
 * Block types, If you make changes before the last value of the enum, you must re-produce the registers
 */
typedef enum soc_block_type_e {
    SOC_BLK_NONE=0,
    SOC_BLK_EPIC,
    SOC_BLK_GPIC,
    SOC_BLK_HPIC,
    SOC_BLK_IPIC,
    SOC_BLK_XPIC,
    SOC_BLK_CMIC,
    SOC_BLK_CPIC,
    SOC_BLK_ARL,
    SOC_BLK_MMU,
    SOC_BLK_MMU_GLB, /* 10 */
    SOC_BLK_MMU_SC,
    SOC_BLK_MMU_SED,
    SOC_BLK_MMU_XPE,
    SOC_BLK_MCU,
    SOC_BLK_GPORT,
    SOC_BLK_XPORT,
    SOC_BLK_IPIPE,
    SOC_BLK_IPIPE_HI,
    SOC_BLK_EPIPE,
    SOC_BLK_EPIPE_HI, /* 20 */
    SOC_BLK_IGR,
    SOC_BLK_EGR,
    SOC_BLK_BSE,
    SOC_BLK_CSE,
    SOC_BLK_HSE,
    SOC_BLK_BSAFE,
    SOC_BLK_GXPORT,
    SOC_BLK_SPI,
    SOC_BLK_EXP,
    SOC_BLK_SYS, /* 30 */
    SOC_BLK_XGPORT,
    SOC_BLK_SPORT,
    SOC_BLK_INTER,
    SOC_BLK_EXTER,
    SOC_BLK_ESM,
    SOC_BLK_OTPC,
    SOC_BLK_QGPORT,
    SOC_BLK_XQPORT,
    SOC_BLK_XLPORT,
    SOC_BLK_XLPORTB0, /* 40 */
    SOC_BLK_LBPORT,
    SOC_BLK_PORT_GROUP4,
    SOC_BLK_PORT_GROUP5,
    SOC_BLK_PGW_CL,
    SOC_BLK_CPORT,
    SOC_BLK_CLPORT,
    SOC_BLK_CLG2PORT,
    SOC_BLK_XTPORT,
    SOC_BLK_MXQPORT,
    SOC_BLK_TOP, /* 50 */
    SOC_BLK_SER,
    SOC_BLK_AVS,
    SOC_BLK_AXP,
    SOC_BLK_ISM,
    SOC_BLK_ETU,
    SOC_BLK_IBOD,
    SOC_BLK_LLS,
    SOC_BLK_CES,
    SOC_BLK_CI,
    SOC_BLK_TXLP, /* 60 */
    SOC_BLK_RXLP,
    SOC_BLK_IL,
    SOC_BLK_MS_ISEC,
    SOC_BLK_MS_ESEC,
    /* Sirius regs */
    SOC_BLK_BP,
    SOC_BLK_CS,
    SOC_BLK_EB,
    SOC_BLK_EP,
    SOC_BLK_ES,
    SOC_BLK_FD, /* 70 */ 
    SOC_BLK_FF,
    SOC_BLK_FR, 
    SOC_BLK_TX,
    SOC_BLK_QMA,
    SOC_BLK_QMB,
    SOC_BLK_QMC,
    SOC_BLK_QSA,
    SOC_BLK_QSB,
    SOC_BLK_RB,
    SOC_BLK_SC_TOP, /* 80 */
    SOC_BLK_SF_TOP,
    SOC_BLK_TS, 
    SOC_BLK_CW,
    /* Caladan blocks */
    SOC_BLK_CM,
    SOC_BLK_CO,
    SOC_BLK_CX,
    SOC_BLK_ETU_WRAP,
    SOC_BLK_LRA,
    SOC_BLK_LRB,
    SOC_BLK_OC, /* 90 */
    SOC_BLK_PB,
    SOC_BLK_PD,
    SOC_BLK_PP,
    SOC_BLK_PR,
    SOC_BLK_PT,
    SOC_BLK_QM,
    SOC_BLK_RC,
    SOC_BLK_TMA,
    SOC_BLK_TMB,
    SOC_BLK_TP, /* 100 */
    SOC_BLK_TM_QE,
    /* Helix4 */
    SOC_BLK_XWPORT,
    SOC_BLK_IPROC,
    /* Greyhound */
    SOC_BLK_PMQ,
    /* Apache */
    SOC_BLK_BCPORT,
    SOC_BLK_CXXPORT,
    SOC_BLK_CPRI,
    SOC_BLK_RDB,
    /* Monterey */
    SOC_BLK_MACSEC,
    SOC_BLK_CPRI_TP,
    SOC_BLK_TSCF,
    /* Saber2 */
    SOC_BLK_IECELL,
    /* Helix5: Broadscan */
    SOC_BLK_BROADSCAN,
    /* Hurricane4 */
    SOC_BLK_GXFPPORT,
    /* Hurricane4 */
    SOC_BLK_HLA,
    /* Composites: Keeping these at the end */
    SOC_BLK_PORT,
    SOC_BLK_CPU,
    SOC_BLK_ETHER,
    SOC_BLK_HIGIG,
    SOC_BLK_FABRIC,
    SOC_BLK_NET,
    SOC_BLK_HGPORT,
    SOC_BLK_PGW_GE,
    /* Trident3 CCH Virtual Block */
    SOC_BLK_CCH,
    /* Greyhound2 */
    SOC_BLK_CRYPTO,
    SOC_BLK_PGW_GE8P,
    /* Helix5 */
    SOC_BLK_PMQPORT,
    /* Hurricane4 */
    SOC_BLK_QTGPORT,

    /*Tomahawk3*/
    SOC_BLK_MMU_ITM,
    SOC_BLK_MMU_EB,
    SOC_BLK_CEV,
    SOC_BLK_CDPORT,
    SOC_BLK_CDPORT_BST,
    SOC_BLK_CDMAC,
    SOC_BLK_TAF,
    SOC_BLK_END_NOT_DNX,
    
    SOC_BLK_FIRST_DNX, /* all DNX blocks should apear betweenSOC_BLK_FIRST_DNX SOC_BLK_LAST_DNX*/
    SOC_BLK_FIRST_DPP,
    /* Petra-B block info */
    SOC_BLK_CFC,
    SOC_BLK_DPI,
    SOC_BLK_DRC,
    SOC_BLK_ECI,
    SOC_BLK_EGQ,
    SOC_BLK_EPNI,
    SOC_BLK_FCR,
    SOC_BLK_FCT,
    SOC_BLK_FDR,
    SOC_BLK_FDA,
    SOC_BLK_FDT,
    SOC_BLK_MESH_TOPOLOGY,
    SOC_BLK_IDR,
    SOC_BLK_IHB,
    SOC_BLK_IHP,
    SOC_BLK_IPS,
    SOC_BLK_IPT,
    SOC_BLK_IQM,
    SOC_BLK_IRE,
    SOC_BLK_IRR,
    SOC_BLK_MAC,
    SOC_BLK_MBU,
    SOC_BLK_MCC,
    SOC_BLK_MSW,
    SOC_BLK_NBI,
    SOC_BLK_NIF,
    SOC_BLK_NIF_MAL,
    SOC_BLK_OLP,
    SOC_BLK_QDR,
    SOC_BLK_RTP,
    SOC_BLK_SCH,

    /* ARAD */
    SOC_BLK_OCB,
    SOC_BLK_CRPS,
    SOC_BLK_FMAC,
    SOC_BLK_XLP,
    SOC_BLK_CLP,
    SOC_BLK_CXX,
    SOC_BLK_CGM,
    SOC_BLK_OAMP,
    SOC_BLK_FSRD,
    SOC_BLK_DRCA,
    SOC_BLK_DRCB,
    SOC_BLK_DRCC,
    SOC_BLK_DRCD,
    SOC_BLK_DRCE,
    SOC_BLK_DRCF,
    SOC_BLK_DRCG,
    SOC_BLK_DRCH,
    SOC_BLK_BRDC_FMAC,
    SOC_BLK_BRDC_FSRD,
    SOC_BLK_BRDC_HBC,
    SOC_BLK_BRDC_DRC,
    SOC_BLK_DRCBROADCAST,

/* Jericho */
    SOC_BLK_IQMT,
    SOC_BLK_NBIH,
    SOC_BLK_NBIL,
    SOC_BLK_IPST,
    SOC_BLK_PPDB_A,
    SOC_BLK_PPDB_B,
    SOC_BLK_MRPS,
    SOC_BLK_MTRPS_EM,
    SOC_BLK_EDB,
    SOC_BLK_ILKN_PMH,
    SOC_BLK_ILKN_PML,
    SOC_BLK_BRDC_CGM,
    SOC_BLK_BRDC_EGQ,
    SOC_BLK_BRDC_EPNI,
    SOC_BLK_BRDC_IHB,
    SOC_BLK_BRDC_IHP,
    SOC_BLK_BRDC_IPS,
    SOC_BLK_BRDC_IQM,
    SOC_BLK_BRDC_SCH,
    SOC_BLK_BRDC_MRPS,
    SOC_BLK_BRDC_MTRPS_EM,
    SOC_BLK_KAPS,
    SOC_BLK_EMPTY,
    /* for DNX test chips */
    SOC_BLK_PRM,
    SOC_BLK_BLH,
    SOC_BLK_AM_TOP,

/*FE1600*/
    SOC_BLK_OCCG,
    SOC_BLK_DCH,
    SOC_BLK_DCL,
    SOC_BLK_DCMA,
    SOC_BLK_DCMB,
    SOC_BLK_DCMC,
    SOC_BLK_CCS,
    SOC_BLK_BRDC_FMACH,
    SOC_BLK_BRDC_FMACL,
    
/*FE3200*/
    SOC_BLK_DCM,
    SOC_BLK_BRDC_DCH,
    SOC_BLK_BRDC_DCM,
    SOC_BLK_BRDC_DCL,
    SOC_BLK_BRDC_CCS,
    SOC_BLK_BRDC_FMAC_AC,
    SOC_BLK_BRDC_FMAC_BD,

/*RAMON*/
    SOC_BLK_DCML,
    SOC_BLK_MCT,
    SOC_BLK_QRH,
    SOC_BLK_LCM,
    SOC_BLK_BRDC_CCH,
    SOC_BLK_BRDC_DCML,
    SOC_BLK_BRDC_LCM,
    SOC_BLK_BRDC_QRH,

/*QMX*/
    SOC_BLK_BRDC_GPORT,
    SOC_BLK_BRDC_XPL,
    SOC_BLK_BRDC_CPL,
    SOC_BLK_BRDC_PMQ,
/* QAX */
    SOC_BLK_IDB,
    SOC_BLK_IEP,
    SOC_BLK_ILB,
    SOC_BLK_IMP,
    SOC_BLK_IPSEC,
    SOC_BLK_ITE,
    SOC_BLK_PEM,
    SOC_BLK_PMH,
    SOC_BLK_PTS,
    SOC_BLK_SPB,
    SOC_BLK_SQM,
    SOC_BLK_TAR,
    SOC_BLK_TXQ,
    SOC_BLK_DDP,
    SOC_BLK_DEBUG,
    SOC_BLK_DPRCA,
    SOC_BLK_DPRCB,
    SOC_BLK_DPRCC,
    SOC_BLK_DQM,
    SOC_BLK_ECGM,
    SOC_BLK_IPSEC_SPU_WRAPPER_TOP,
    SOC_BLK_BRDC_IPSEC_SPU_WRAPPER_TOP,
    SOC_BLK_KAPS_BBS,
    SOC_BLK_MXQ,
    SOC_BLK_PLL,
    SOC_BLK_HBM,
    SOC_BLK_MTM,
    SOC_BLK_PDM,
    SOC_BLK_SIF,
    SOC_BLK_BDM,
    SOC_BLK_ILU,
    SOC_BLK_IPPA,
    SOC_BLK_IPPB,
    SOC_BLK_IPPC,
    SOC_BLK_IPPD,
    SOC_BLK_ITPP,
    SOC_BLK_ITPPD,
    SOC_BLK_MACT,
    SOC_BLK_MCP,
    SOC_BLK_MDB,
    SOC_BLK_NMG,
    SOC_BLK_RQP,
    SOC_BLK_CDU,
    SOC_BLK_EPS,
    SOC_BLK_EPS_SPR,
    SOC_BLK_ERPP,
    SOC_BLK_ETPPA,
    SOC_BLK_ETPPB,
    SOC_BLK_FQP,
    SOC_BLK_PQP,
    SOC_BLK_HBMC,
    SOC_BLK_BRDC_KAPS_BBS,
    SOC_BLK_DDHB,
    SOC_BLK_EVNT,
    SOC_BLK_TCAM,
    SOC_BLK_DHC,
    SOC_BLK_TDU,
    SOC_BLK_IPPE,
    SOC_BLK_HBC,
    SOC_BLK_ETPPC,
    SOC_BLK_EPRE,
    SOC_BLK_IPPF,
    SOC_BLK_MDB_ARM,
    SOC_BLK_FDTL,
    SOC_BLK_CDUM,
    SOC_BLK_DDHA,
    SOC_BLK_DMU,
    SOC_BLK_ILE,
    SOC_BLK_CLU,
    SOC_BLK_CLUP,
    SOC_BLK_CLMAC,
    SOC_BLK_ESB,
/* Q2A */
    SOC_BLK_DCC,
    SOC_BLK_DPC,
    SOC_BLK_FASIC,
    SOC_BLK_FEU,
    SOC_BLK_FLEXEWP,
    SOC_BLK_FPRD,
    SOC_BLK_FSAR,
    SOC_BLK_FSCL,

/* J2P */
    SOC_BLK_CDB,
    SOC_BLK_CDBM,
    SOC_BLK_CDPM,
    SOC_BLK_OCBM,
    SOC_BLK_MSD,
    SOC_BLK_MSS,
    SOC_BLK_HRC,

    SOC_BLK_ALIGNER,
    SOC_BLK_MSU,
    SOC_BLK_PMU,
    SOC_BLK_NBU,
    SOC_BLK_DDHX,
    SOC_BLK_TCMX,
    SOC_BLK_MKX,
    SOC_BLK_MDBA,
    SOC_BLK_MDBB,
    SOC_BLK_MDBK,
    SOC_BLK_MGU,
    SOC_BLK_DDHC,
    SOC_BLK_DDHAB,
    SOC_BLK_MACTMNG,
    SOC_BLK_FDTM,
    SOC_BLK_EPPS,
    SOC_BLK_CPX,
    SOC_BLK_SAT,
    SOC_BLK_FDTS,
    SOC_BLK_LAST_DNX, /* all DNX blocks should apear betweenSOC_BLK_FIRST_DNX SOC_BLK_LAST_DNX*/
    SOC_BLK_LAST_DPP,
    SOC_BLOCK_TYPE_INVALID = -1
} soc_block_type_t;

#define NOF_DNX_BLOCKS   ((SOC_BLK_LAST_DNX) - (SOC_BLK_FIRST_DNX))


#define NOF_BCM_BLOCKS   ((NOF_DNX_BLOCKS) + SOC_BLK_END_NOT_DNX)

#define SOC_DNX_BLK_PORT_INITIALIZER \
    SOC_BLK_EPIC,\
    SOC_BLK_GPIC,\
    SOC_BLK_HPIC,\
    SOC_BLK_IPIC,\
    SOC_BLK_XPIC,\
    SOC_BLK_GPORT,\
    SOC_BLK_XPORT,\
    SOC_BLK_GXPORT,\
    SOC_BLK_GXFPPORT,\
    SOC_BLK_XGPORT,\
    SOC_BLK_QGPORT,\
    SOC_BLK_SPORT,\
    SOC_BLK_XLPORT,\
    SOC_BLK_XLPORTB0,\
    SOC_BLK_MXQPORT,\
    SOC_BLK_XQPORT,\
    SOC_BLK_CPIC,\
    SOC_BLK_CPORT,\
    SOC_BLK_CLPORT,\
    SOC_BLK_CDPORT,\
    SOC_BLK_CLG2PORT,\
    SOC_BLK_XTPORT,\
    SOC_BLK_XWPORT,\
    SOC_BLK_AXP,\
    SOC_BLK_MXQPORT,\
    SOC_BLK_XLP,\
    SOC_BLK_CLP,\
    SOC_BLK_PGW_CL, \
    SOC_BLK_RXLP,\
    SOC_BLK_TXLP,\
    SOC_BLK_CXXPORT,\
    SOC_BLK_CPRI,\
    SOC_BLK_RDB,\
    SOC_BLK_BCPORT,\
    SOC_BLK_PGW_GE,\
    SOC_BLK_PGW_GE8P,\
    SOC_BLK_IECELL,\
    SOC_BLK_BROADSCAN, \
    SOC_BLK_MXQ,\
    SOC_BLK_CDMAC,\
    SOC_BLK_CLMAC,\
    SOC_BLOCK_TYPE_INVALID

#define SOC_BLK_PORT_INITIALIZER \
    SOC_BLK_EPIC,\
    SOC_BLK_GPIC,\
    SOC_BLK_HPIC,\
    SOC_BLK_IPIC,\
    SOC_BLK_XPIC,\
    SOC_BLK_GPORT,\
    SOC_BLK_XPORT,\
    SOC_BLK_GXPORT,\
    SOC_BLK_GXFPPORT,\
    SOC_BLK_XGPORT,\
    SOC_BLK_QGPORT,\
    SOC_BLK_SPORT,\
    SOC_BLK_XLPORT,\
    SOC_BLK_XLPORTB0,\
    SOC_BLK_MXQPORT,\
    SOC_BLK_XQPORT,\
    SOC_BLK_CPIC,\
    SOC_BLK_CPORT,\
    SOC_BLK_CLPORT,\
    SOC_BLK_CDPORT,\
    SOC_BLK_CLG2PORT,\
    SOC_BLK_XTPORT,\
    SOC_BLK_XWPORT,\
    SOC_BLK_AXP,\
    SOC_BLK_MXQPORT,\
    SOC_BLK_XLP,\
    SOC_BLK_CLP,\
    SOC_BLK_PGW_CL, \
    SOC_BLK_RXLP,\
    SOC_BLK_TXLP,\
    SOC_BLK_CXXPORT,\
    SOC_BLK_CPRI,\
    SOC_BLK_RDB,\
    SOC_BLK_BCPORT,\
    SOC_BLK_PGW_GE,\
    SOC_BLK_PGW_GE8P,\
    SOC_BLK_IECELL,\
    SOC_BLK_BROADSCAN, \
    SOC_BLK_MXQ,\
    SOC_BLK_MACSEC,\
    SOC_BLK_CDMAC,\
    SOC_BLK_CLMAC,\
    SOC_BLOCK_TYPE_INVALID

#define SOC_BLK_CPU_INITIALIZER \
    SOC_BLK_CMIC,\
    SOC_BLK_CPIC,\
    SOC_BLOCK_TYPE_INVALID\

#define SOC_BLK_ETHER_INITIALIZER \
    SOC_BLK_EPIC,\
    SOC_BLK_GPIC,\
    SOC_BLK_XPIC,\
    SOC_BLK_GPORT,\
    SOC_BLK_XPORT,\
    SOC_BLK_GXPORT,\
    SOC_BLK_GXFPPORT,\
    SOC_BLK_XGPORT,\
    SOC_BLK_XQPORT,\
    SOC_BLK_QGPORT,\
    SOC_BLK_SPORT,\
    SOC_BLK_XLPORT,\
    SOC_BLK_XLPORTB0,\
    SOC_BLK_MXQPORT,\
    SOC_BLK_CPORT,\
    SOC_BLK_CLPORT,\
    SOC_BLK_CDPORT,\
    SOC_BLK_CLG2PORT,\
    SOC_BLK_XTPORT,\
    SOC_BLK_XWPORT,\
    SOC_BLK_CXXPORT,\
    SOC_BLK_CPRI,\
    SOC_BLK_PGW_GE,\
    SOC_BLK_PGW_GE8P,\
    SOC_BLOCK_TYPE_INVALID\

#define SOC_BLK_HIGIG_INITIALIZER \
    SOC_BLK_HPIC,\
    SOC_BLK_IPIC,\
    SOC_BLK_XPORT,\
    SOC_BLK_GXPORT,\
    SOC_BLK_GXFPPORT,\
    SOC_BLK_XGPORT,\
    SOC_BLK_XQPORT,\
    SOC_BLK_XLPORT,\
    SOC_BLK_XLPORTB0,\
    SOC_BLK_MXQPORT,\
    SOC_BLK_QGPORT,\
    SOC_BLK_CPORT,\
    SOC_BLK_CLPORT,\
    SOC_BLK_CDPORT,\
    SOC_BLK_CLG2PORT,\
    SOC_BLK_XTPORT,\
    SOC_BLK_XWPORT,\
    SOC_BLK_CXXPORT,\
    SOC_BLK_CPRI,\
    SOC_BLK_PGW_GE,\
    SOC_BLK_PGW_GE8P,\
    SOC_BLOCK_TYPE_INVALID\

#define SOC_BLK_FABRIC_INITIALIZER \
    SOC_BLK_SC_TOP,\
    SOC_BLK_SF_TOP,\
    SOC_BLK_SPI,\
    SOC_BLOCK_TYPE_INVALID\

#define SOC_BLK_NET_INITIALIZER \
    SOC_BLK_EPIC,\
    SOC_BLK_GPIC,\
    SOC_BLK_XPIC,\
    SOC_BLK_GPORT,\
    SOC_BLK_XPORT,\
    SOC_BLK_GXPORT,\
    SOC_BLK_GXFPPORT,\
    SOC_BLK_XGPORT,\
    SOC_BLK_XQPORT,\
    SOC_BLK_QGPORT,\
    SOC_BLK_SPORT,\
    SOC_BLK_XLPORT,\
    SOC_BLK_XLPORTB0,\
    SOC_BLK_MXQPORT,\
    SOC_BLK_CPORT,\
    SOC_BLK_CLPORT,\
    SOC_BLK_CDPORT,\
    SOC_BLK_CLG2PORT,\
    SOC_BLK_XTPORT,\
    SOC_BLK_XWPORT,\
    SOC_BLK_HPIC,\
    SOC_BLK_IPIC,\
    SOC_BLK_SC_TOP,\
    SOC_BLK_SF_TOP,\
    SOC_BLK_SPI,\
    SOC_BLK_IL,\
    SOC_BLK_FSRD,\
    SOC_BLK_HBC,\
    SOC_BLK_XLP,\
    SOC_BLK_CLP,\
    SOC_BLK_PGW_CL,\
    SOC_BLK_CXXPORT,\
    SOC_BLK_CPRI,\
    SOC_BLK_RDB,\
    SOC_BLK_BCPORT,\
    SOC_BLK_PGW_GE,\
    SOC_BLK_PGW_GE8P,\
    SOC_BLK_FMAC,\
    SOC_BLK_MACSEC,\
    SOC_BLK_MXQ,\
    SOC_BLK_FEU,\
    SOC_BLOCK_TYPE_INVALID\

#define SOC_BLK_HGPORT_INITIALIZER \
    SOC_BLK_IPIPE_HI,\
    SOC_BLOCK_TYPE_INVALID\

/*
 * Naming of blocks (there are two such arrays, one for
 * block based naming and one for port based naming)
 * Last entry has blk of SOC_BLK_NONE.
 */
typedef struct {
    soc_block_t     blk;        /* block bits to match */
    int             isalias;    /* this name is an alias */
    char            *name;      /* printable name */
} soc_block_name_t;

/* used to intialize soc_block_name_t soc_block_port_names[] */
#define SOC_BLOCK_PORT_NAMES_INITIALIZER    {  \
    /*    blk  , isalias, name */      \
    { SOC_BLK_EPIC,     0,  "fe"    }, \
    { SOC_BLK_GPIC,     0,  "ge"    }, \
    { SOC_BLK_GPORT,    0,  "ge"    }, \
    { SOC_BLK_GXPORT,   0,  "hg"    }, \
    { SOC_BLK_GXFPPORT, 0,  "ge"    }, \
    { SOC_BLK_XGPORT,   0,  "ge"    }, \
    { SOC_BLK_QGPORT,   0,  "ge"    }, \
    { SOC_BLK_XQPORT,   0,  "ge"    }, \
    { SOC_BLK_SPORT,    0,  "ge"    }, \
    { SOC_BLK_HPIC,     0,  "hg"    }, \
    { SOC_BLK_IPIC,     0,  "hg"    }, \
    { SOC_BLK_XPIC,     0,  "xe"    }, \
    { SOC_BLK_XPORT,    0,  "hg"    }, \
    { SOC_BLK_CMIC,     0,  "cpu"   }, \
    { SOC_BLK_CPIC,     0,  "cpu"   }, \
    { SOC_BLK_SPI,      0,  "spi"   }, \
    { SOC_BLK_EXP,      0,  "exp"   }, \
    { SOC_BLK_LBPORT,   0,  "lb"    }, \
    { SOC_BLK_CPU,      0,  "cpu"   }, \
    { SOC_BLK_AXP,      0,  "ax"    }, \
    { SOC_BLK_XLPORT,   0,  "xl"    }, \
    { SOC_BLK_XLPORTB0, 0,  "xlb0"  }, \
    { SOC_BLK_CPORT,    0,  "ce"    }, \
    { SOC_BLK_CLPORT,   0,  "cl"    }, \
    { SOC_BLK_CDPORT,   0,  "cd"    }, \
    { SOC_BLK_CLG2PORT, 0,  "clg2"  }, \
    { SOC_BLK_XTPORT,   0,  "xt"    }, \
    { SOC_BLK_MXQPORT,  0,  "mxq"   }, \
    { SOC_BLK_XWPORT,   0,  "xw"    }, \
    { SOC_BLK_MXQPORT,  0,  "mxq"    }, \
    { SOC_BLK_PGW_CL,   0,  "pgw_cl"}, \
    { SOC_BLK_CXXPORT,  0,  "cxx"   }, \
    { SOC_BLK_CPRI,     0,  "cpri"  }, \
    { SOC_BLK_RDB,      0,  "rdb"   }, \
    { SOC_BLK_BCPORT,   0,  "bcport"}, \
    { SOC_BLK_PMQ,      0,  "pmq"   }, \
    { SOC_BLK_PGW_GE,   0,  "pgw_ge"}, \
    { SOC_BLK_MACSEC,   0,  "msec"   }, \
    { SOC_BLK_PGW_GE8P, 0,  "pgw_ge8p"}, \
    { SOC_BLK_MACSEC,   0,  "msec"   }, \
    { SOC_BLK_NONE,     0,  ""  } }

/* used to intialize soc_block_name_t soc_block_names[] */
#define SOC_BLOCK_NAMES_INITIALIZER {  \
    /*    blk  , isalias, name */      \
    { SOC_BLK_EPIC,     0,  "epic"  }, \
    { SOC_BLK_GPIC,     0,  "gpic"  }, \
    { SOC_BLK_HPIC,     0,  "hpic"  }, \
    { SOC_BLK_IPIC,     0,  "ipic"  }, \
    { SOC_BLK_XPIC,     0,  "xpic"  }, \
    { SOC_BLK_CMIC,     0,  "cmic"  }, \
    { SOC_BLK_CPIC,     0,  "cpic"  }, \
    { SOC_BLK_ARL,      0,  "arl"   }, \
    { SOC_BLK_MMU,      0,  "mmu"   }, \
    { SOC_BLK_MMU_SED,  0,  "mmu_sed" }, \
    { SOC_BLK_MMU_GLB,  0,  "mmu_glb" }, \
    { SOC_BLK_MMU_SC,   0,  "mmu_sc"  }, \
    { SOC_BLK_MMU_XPE,  0,  "mmu_xpe" }, \
    { SOC_BLK_MMU_SED,  0,  "mmu_sed" }, \
    { SOC_BLK_MMU_ITM,  0,  "mmu_itm" }, \
    { SOC_BLK_MMU_EB,   0,  "mmu_eb" }, \
    { SOC_BLK_MCU,      0,  "mcu"   }, \
    { SOC_BLK_GPORT,    0,  "gport" }, \
    { SOC_BLK_XPORT,    0,  "xport" }, \
    { SOC_BLK_GXPORT,   0,  "gxport" }, \
    { SOC_BLK_XLPORT,   0,  "xlport" }, \
    { SOC_BLK_XLPORTB0, 0,  "xlb0port" }, \
    { SOC_BLK_MXQPORT,  0,  "mxqport" }, \
    { SOC_BLK_XGPORT,   0,  "xgport" }, \
    { SOC_BLK_QGPORT,   0,  "qgport" }, \
    { SOC_BLK_XQPORT,   0,  "xqport" }, \
    { SOC_BLK_SPORT,    0,  "sport" }, \
    { SOC_BLK_IPIPE,    0,  "ipipe" }, \
    { SOC_BLK_IPIPE_HI, 0,  "ipipe_hi" }, \
    { SOC_BLK_EPIPE,    0,  "epipe" }, \
    { SOC_BLK_EPIPE_HI, 0,  "epipe_hi" }, \
    { SOC_BLK_IGR,      0,  "igr"  }, \
    { SOC_BLK_EGR,      0,  "egr"  }, \
    { SOC_BLK_BSE,      0,  "bse"  }, \
    { SOC_BLK_IL,       0,  "intl"  }, \
    { SOC_BLK_MS_ISEC,  0,  "ms_isec"  }, \
    { SOC_BLK_MS_ESEC,  0,  "ms_esec"  }, \
    { SOC_BLK_CW,       0,  "cw"  }, \
    { SOC_BLK_CSE,      0,  "cse"  }, \
    { SOC_BLK_HSE,      0,  "hse"  }, \
    { SOC_BLK_BSAFE,    0,  "bsafe" }, \
    { SOC_BLK_ESM,      0,  "esm"}, \
    { SOC_BLK_EPIC,     1,  "e"     }, \
    { SOC_BLK_GPIC,     1,  "g"     }, \
    { SOC_BLK_HPIC,     1,  "h"     }, \
    { SOC_BLK_IPIC,     1,  "i"     }, \
    { SOC_BLK_XPIC,     1,  "x"     }, \
    { SOC_BLK_CMIC,     1,  "cpu"   }, \
    { SOC_BLK_OTPC,     1,  "otpc"  }, \
    { SOC_BLK_SPI,      0,  "spi"   }, \
    { SOC_BLK_LBPORT,   0,  "lb"    }, \
    { SOC_BLK_TOP,      0,  "top"   }, \
    { SOC_BLK_SER,      0,  "ser"   }, \
    { SOC_BLK_AVS,      0,  "avs"   }, \
    { SOC_BLK_ISM,      0,  "ism"   }, \
    { SOC_BLK_ETU,      0,  "etu"   }, \
    { SOC_BLK_IBOD,     0,  "ibod"   }, \
    { SOC_BLK_AXP,      0,  "ax"    }, \
    { SOC_BLK_XLPORT,   0,  "xlport" }, \
    { SOC_BLK_XLPORTB0, 0,  "xlb0port" }, \
    { SOC_BLK_CLPORT,   0,  "clport" }, \
    { SOC_BLK_CLMAC,   0,  "clmac" }, \
    { SOC_BLK_CDPORT,   0,  "cdport" }, \
    { SOC_BLK_CDMAC,    0,  "cdmac" }, \
    { SOC_BLK_CLG2PORT, 0,  "clg2port" }, \
    { SOC_BLK_CPORT,    0,  "cport" }, \
    { SOC_BLK_XTPORT,   0,  "xtport" }, \
    { SOC_BLK_XWPORT,   0,  "xwport" }, \
    { SOC_BLK_PORT_GROUP4, 0, "port_group4" }, \
    { SOC_BLK_PORT_GROUP5, 0, "port_group5" }, \
    { SOC_BLK_PGW_CL,   0,  "pgw_cl" }, \
    { SOC_BLK_CPU,      0,  "cpu"   }, \
    { SOC_BLK_CI,       0,  "ci"   }, \
    { SOC_BLK_TXLP,     0,  "txlp" }, \
    { SOC_BLK_RXLP,     0,  "rxlp" }, \
    { SOC_BLK_IECELL,   0,  "iecell" }, \
    { SOC_BLK_BROADSCAN, 0, "broadscan" }, \
    { SOC_BLK_LLS,      0,  "lls"   }, \
    { SOC_BLK_CES,      0,  "ces"   }, \
    { SOC_BLK_CEV,      0,  "cev"   }, \
    { SOC_BLK_MS_ISEC,  0,  "ms_isec"  }, \
    { SOC_BLK_MS_ESEC,  0,  "ms_esec"  }, \
    { SOC_BLK_PGW_CL,   0,  "pgw_cl" }, \
    { SOC_BLK_IPROC,    0,  "iproc"  }, \
    { SOC_BLK_PMQ,      0,  "pmq"  }, \
    { SOC_BLK_CXXPORT,  0,  "cxxport" }, \
    { SOC_BLK_CPRI,     0,  "cpri" }, \
    { SOC_BLK_TSCF,     0,  "cpri" }, \
    { SOC_BLK_RDB,      0,  "rdb"    }, \
    { SOC_BLK_BCPORT,   0,  "bcport"}, \
    { SOC_BLK_PGW_GE,   0,  "pgw_ge" }, \
    { SOC_BLK_MACSEC,      0,  "msec"    }, \
    { SOC_BLK_CCH,      0,  "cch_pseudo"}, \
    { SOC_BLK_CRYPTO,   0,  "crypto" }, \
    { SOC_BLK_PGW_GE8P, 0,  "pgw_ge8p" }, \
    { SOC_BLK_PMQPORT,  0,  "pmqport" }, \
    { SOC_BLK_QTGPORT,  0,  "qtgport" }, \
    { SOC_BLK_GXFPPORT, 0,  "gxfpport" }, \
    { SOC_BLK_TAF,      0,  "taf" }, \
    { SOC_BLK_NONE,     0,  ""      } }

/* used to initialize soc_block_name_t soc_dpp_block_names[] */
#define SOC_BLOCK_DPP_NAMES_INITIALIZER { \
    /*    blk  , isalias, name */      \
    {SOC_BLK_CFC,       0,  "CFC"}, \
    {SOC_BLK_DPI,       0,  "DPI"}, \
    {SOC_BLK_DRC,       0,  "DRC"}, \
    {SOC_BLK_ECI,       0,  "ECI"}, \
    {SOC_BLK_EGQ,       0,  "EGQ"}, \
    {SOC_BLK_EPNI,      0,  "EPNI"}, \
    {SOC_BLK_FCR,       0,  "FCR"}, \
    {SOC_BLK_MMU,       0,  "MMU"}, \
    {SOC_BLK_MMU_SED,   0,  "MMU_SED"}, \
    {SOC_BLK_MMU_GLB,   0,  "MMU_GLB"}, \
    {SOC_BLK_MMU_SC,    0,  "MMU_SC"}, \
    {SOC_BLK_MMU_XPE,   0,  "MMU_XPE"}, \
    {SOC_BLK_MMU_SED,   0,  "MMU_SED"}, \
    {SOC_BLK_FCT,       0,  "FCT"}, \
    {SOC_BLK_FDR,       0,  "FDR"}, \
    {SOC_BLK_FDA,       0,  "FDA"}, \
    {SOC_BLK_FDT,       0,  "FDT"}, \
    {SOC_BLK_MESH_TOPOLOGY, 0,  "MESH_TOPOLOGY"}, \
    {SOC_BLK_IDR,       0,  "IDR"}, \
    {SOC_BLK_IHB,       0,  "IHB"}, \
    {SOC_BLK_IHP,       0,  "IHP"}, \
    {SOC_BLK_IPS,       0,  "IPS"}, \
    {SOC_BLK_IPT,       0,  "IPT"}, \
    {SOC_BLK_IQM,       0,  "IQM"}, \
    {SOC_BLK_IRE,       0,  "IRE"}, \
    {SOC_BLK_IRR,       0,  "IRR"}, \
    {SOC_BLK_MAC,       0,  "MAC"}, \
    {SOC_BLK_MBU,       0,  "MBU"}, \
    {SOC_BLK_MCC,       0,  "MCC"}, \
    {SOC_BLK_MSW,       0,  "MSW"}, \
    {SOC_BLK_NBI,       0,  "NBI"}, \
    {SOC_BLK_NIF,       0,  "NIF"}, \
    {SOC_BLK_NIF_MAL,   0,  "NIF_MAL"}, \
    {SOC_BLK_OLP,       0,  "OLP"}, \
    {SOC_BLK_QDR,       0,  "QDR"}, \
    {SOC_BLK_RTP,       0,  "RTP"}, \
    {SOC_BLK_SCH,       0,  "SCH"}, \
    {SOC_BLK_OCB,       0,  "OCB"}, \
    {SOC_BLK_CRPS,      0,  "CRPS"}, \
    {SOC_BLK_FMAC,      0,  "FMAC"}, \
    {SOC_BLK_XLP,       0,  "XLP"}, \
    {SOC_BLK_CLP,       0,  "CLP"}, \
    {SOC_BLK_CGM,       0,  "CGM"}, \
    {SOC_BLK_OAMP,      0,  "OAMP"}, \
    {SOC_BLK_FSRD,      0,  "FSRD"}, \
    {SOC_BLK_DRCA,      0,  "DRCA"}, \
    {SOC_BLK_DRCB,      0,  "DRCB"}, \
    {SOC_BLK_DRCC,      0,  "DRCC"}, \
    {SOC_BLK_DRCD,      0,  "DRCD"}, \
    {SOC_BLK_DRCE,      0,  "DRCE"}, \
    {SOC_BLK_DRCF,      0,  "DRCF"}, \
    {SOC_BLK_DRCG,      0,  "DRCG"}, \
    {SOC_BLK_DRCH,      0,  "DRCH"}, \
    {SOC_BLK_BRDC_FMAC ,0,  "BRDC_FMAC"}, \
    {SOC_BLK_BRDC_FSRD,0,  "BRDC_FSRD"}, \
    {SOC_BLK_BRDC_HBC ,0,  "BRDC_HBC"}, \
    {SOC_BLK_BRDC_DRC,0,  "SOC_BLK_BRDC_DRC"}, \
    {SOC_BLK_DRCBROADCAST, 0,  "DRCBROADCAST"}, \
    {SOC_BLK_BRDC_CGM ,0,  "BRDC_CGM"}, \
    {SOC_BLK_BRDC_EGQ ,0,  "BRDC_EGQ"}, \
    {SOC_BLK_BRDC_EPNI ,0, "BRDC_EPNI"}, \
    {SOC_BLK_BRDC_IHB ,0,  "BRDC_IHB"}, \
    {SOC_BLK_BRDC_IHP ,0,  "BRDC_IHP"}, \
    {SOC_BLK_BRDC_IPS ,0,  "BRDC_IPS"}, \
    {SOC_BLK_BRDC_IQM ,0,  "BRDC_IQM"}, \
    {SOC_BLK_BRDC_SCH ,0,  "BRDC_SCH"}, \
    {SOC_BLK_BRDC_MRPS, 0, "BRDC_MRPS"}, \
    {SOC_BLK_BRDC_MTRPS_EM, 0, "BRDC_MTRPS_EM"}, \
    {SOC_BLK_PRM,      0,  "PRM"}, \
    {SOC_BLK_BLH,      0,  "BLH"}, \
    {SOC_BLK_AM_TOP,    0,  "AM_TOP"}, \
    {SOC_BLK_OCCG,      0,  "OCCG"}, \
    {SOC_BLK_CCH,       0,  "CCH"}, \
    {SOC_BLK_DCH,       0,  "DCH"}, \
    {SOC_BLK_DCL,       0,  "DCL"}, \
    {SOC_BLK_DCMA,      0,  "DCMA"}, \
    {SOC_BLK_DCMB,      0,  "DCMB"}, \
    {SOC_BLK_DCMC,      0,  "DCMC"}, \
    {SOC_BLK_CCS,       0,  "CCS"}, \
    {SOC_BLK_BRDC_FMACH,0,  "BRDC_FMACH"}, \
    {SOC_BLK_BRDC_FMACL,0,  "BRDC_FMACL"}, \
    {SOC_BLK_CMIC,      0,  "CMIC"}, \
    {SOC_BLK_IPROC,     0,  "IPROC"}, \
	{SOC_BLK_DCM,     	0,  "DCM"}, \
	{SOC_BLK_BRDC_DCH,  0,  "BRDC_DCH"}, \
	{SOC_BLK_BRDC_DCM,  0,  "BDRC_DCM"}, \
	{SOC_BLK_BRDC_DCL,  0,  "BRDC_DCL"}, \
	{SOC_BLK_BRDC_CCS,  0,  "BRDC_CCS"}, \
	{SOC_BLK_BRDC_FMAC_AC, 0,"BRDC_FMAC_AC"}, \
	{SOC_BLK_BRDC_FMAC_BD, 0,"BRDC_FMAC_BD"}, \
    {SOC_BLK_DCML, 0, "DCML"}, \
    {SOC_BLK_MCT, 0, "MCT"}, \
    {SOC_BLK_QRH, 0, "QRH"}, \
    {SOC_BLK_LCM, 0, "LCM"},\
    {SOC_BLK_BRDC_DCML, 0, "BRDC_DCML"}, \
    {SOC_BLK_BRDC_CCH, 0, "BRDC_CCH"}, \
    {SOC_BLK_BRDC_LCM, 0, "BRDC_LCM"}, \
    {SOC_BLK_BRDC_QRH, 0, "BRDC_QRH"}, \
    /* JERICHO */ {SOC_BLK_EDB,        0,  "EDB"}, \
    {SOC_BLK_MRPS,       0,  "MRPS"}, \
    {SOC_BLK_MTRPS_EM,       0,  "MRPS_EM"}, \
    {SOC_BLK_ILKN_PMH,   0,  "ILKN_PMH"}, \
    {SOC_BLK_IPST,       0,  "IPST"}, \
    {SOC_BLK_ILKN_PML,   0,  "ILKN_PML"}, \
    {SOC_BLK_IQMT,       0,  "IQMT"}, \
    {SOC_BLK_PPDB_A,       0,  "PPDB_A"}, \
    {SOC_BLK_PPDB_B,       0,  "PPDB_B"}, \
    {SOC_BLK_NBIL,       0,  "NBIL"}, \
    {SOC_BLK_NBIH,       0,  "NBIH"}, \
    {SOC_BLK_GPORT,   0,  "GPORT"},\
    {SOC_BLK_PMQ,    0,  "PMQ"},\
    {SOC_BLK_BRDC_GPORT,0, "BRDC_GPORT"},\
    {SOC_BLK_BRDC_XPL, 0, "BRDC_XPL"},\
    {SOC_BLK_BRDC_CPL, 0, "BRDC_CPL"},\
    {SOC_BLK_BRDC_PMQ, 0, "BRDC_PMQ"},\
    {SOC_BLK_KAPS, 0, "KAPS"},\
    /* QAX */ {SOC_BLK_ILB, 0, "ILB"},\
    {SOC_BLK_IEP, 0, "IEP"},\
    {SOC_BLK_IMP, 0, "IMP"},\
    {SOC_BLK_PTS, 0, "PTS"},\
    {SOC_BLK_SPB, 0, "SPB"},\
    {SOC_BLK_ITE, 0, "ITE"},\
    {SOC_BLK_DDP, 0, "DDP"},\
    {SOC_BLK_TXQ, 0, "TXQ"},\
    {SOC_BLK_TAR, 0, "TAR"},\
    {SOC_BLK_PEM, 0, "PEM"},\
    {SOC_BLK_SQM, 0, "SQM"},\
    {SOC_BLK_DQM, 0, "DQM"},\
    {SOC_BLK_ECGM, 0, "ECGM"},\
    {SOC_BLK_IDB, 0, "IDB"},\
    {SOC_BLK_IPSEC, 0, "IPSEC"},\
    {SOC_BLK_IPSEC_SPU_WRAPPER_TOP, 0, "IPSEC_SWT"},\
    {SOC_BLK_BRDC_IPSEC_SPU_WRAPPER_TOP, 0, "BRDC_IPSEC_SWT"},\
    {SOC_BLK_KAPS_BBS, 0, "KAPS_BBS"},\
    {SOC_BLK_MTM, 0, "MTM"},\
    {SOC_BLK_PDM, 0, "PDM"},\
    {SOC_BLK_SIF, 0, "SIF"},\
    {SOC_BLK_BDM, 0, "BDM"},\
    {SOC_BLK_ILU, 0, "ILU"},\
    {SOC_BLK_IPPA, 0, "IPPA"},\
    {SOC_BLK_IPPB, 0, "IPPB"},\
    {SOC_BLK_IPPC, 0, "IPPC"},\
    {SOC_BLK_IPPD, 0, "IPPD"},\
    {SOC_BLK_ITPP, 0, "ITPP"},\
    {SOC_BLK_ITPPD, 0, "ITPPD"},\
    {SOC_BLK_MACT, 0, "MACT"},\
    {SOC_BLK_MCP, 0, "MCP"},\
    {SOC_BLK_MDB, 0, "MDB"},\
    {SOC_BLK_NMG, 0, "NMG"},\
    {SOC_BLK_RQP, 0, "RQP"},\
    {SOC_BLK_CDU, 0, "CDU"},\
    {SOC_BLK_EPS, 0, "EPS"},\
    {SOC_BLK_EPS_SPR, 0, "EPS_SPR"},\
    {SOC_BLK_ERPP, 0, "ERPP"},\
    {SOC_BLK_ETPPA, 0, "ETPPA"},\
    {SOC_BLK_ETPPB, 0, "ETPPB"},\
    {SOC_BLK_FQP, 0, "FQP"},\
    {SOC_BLK_HBM, 0, "HBM"},\
    {SOC_BLK_PQP, 0, "PQP"},\
    {SOC_BLK_HBMC, 0, "HBMC"},\
    {SOC_BLK_BRDC_KAPS_BBS, 0, "BRDC_KAPS_BBS"},\
    {SOC_BLK_CDUM, 0, "CDUM"},\
    {SOC_BLK_CXX, 0, "CXX"},\
    {SOC_BLK_DDHA, 0, "DDHA"},\
    {SOC_BLK_DDHB, 0, "DDHB"},\
    {SOC_BLK_DEBUG, 0, "DEBUG"},\
    {SOC_BLK_DHC, 0, "DHC"},\
    {SOC_BLK_DMU, 0, "DMU"},\
    {SOC_BLK_DPRCA, 0, "DPRCA"},\
    {SOC_BLK_DPRCB, 0, "DPRCB"},\
    {SOC_BLK_DPRCC, 0, "DPRCC"},\
    {SOC_BLK_EDB, 0, "EDB"},\
    {SOC_BLK_EMPTY, 0, "EMPTY"},\
    {SOC_BLK_ETPPC, 0, "ETPPC"},\
    {SOC_BLK_EVNT, 0, "EVNT"},\
    {SOC_BLK_HBC, 0, "HBC"},\
    {SOC_BLK_ILB, 0, "ILB"},\
    {SOC_BLK_ILE, 0, "ILE"},\
    {SOC_BLK_CLU, 0, "CLU"},\
    {SOC_BLK_CLUP, 0, "CLUP"},\
    {SOC_BLK_CLMAC, 0, "CLMAC"},\
    {SOC_BLK_ESB, 0, "ESB"},\
    {SOC_BLK_IPPE, 0, "IPPE"},\
    {SOC_BLK_MXQ, 0, "MXQ"},\
    {SOC_BLK_PLL, 0, "PLL"},\
    {SOC_BLK_PMH, 0, "PMH"},\
    {SOC_BLK_EPRE, 0, "EPRE"},\
    {SOC_BLK_IPPF, 0, "IPPF"},\
    {SOC_BLK_CDPORT, 0, "CDPORT"},\
    {SOC_BLK_CDMAC, 0, "CDMAC"},\
    {SOC_BLK_CLPORT, 0, "CLPORT"},\
    {SOC_BLK_MDB_ARM, 0, "MDB_ARM"},\
    {SOC_BLK_FDTL, 0, "FDTL"},\
    {SOC_BLK_TCAM, 0, "TCAM"},\
    {SOC_BLK_TDU, 0, "TDU"},\
    {SOC_BLK_CLPORT, 0, "CLPORT"},\
     /* QUX */{SOC_BLK_MXQ, 0, "MXQ"},\
    /* FLAIR */{SOC_BLK_PLL, 0, "PLL"},\
    /* Q2A */{SOC_BLK_DCC, 0, "DCC"},\
    {SOC_BLK_DPC, 0, "DPC"},\
    {SOC_BLK_FASIC, 0, "FASIC"},\
    {SOC_BLK_FEU, 0, "FEU"},\
    {SOC_BLK_FLEXEWP, 0, "FLEXEWP"},\
    {SOC_BLK_FPRD, 0, "FPRD"},\
    {SOC_BLK_FSAR, 0, "FSAR"},\
    {SOC_BLK_FSCL, 0, "FSCL"},\
    {SOC_BLK_CDB, 0, "CDB"},\
    {SOC_BLK_CDBM, 0, "CDBM"},\
    {SOC_BLK_CDPM, 0, "CDPM"},\
    {SOC_BLK_OCBM, 0, "OCBM"},\
    {SOC_BLK_MSD, 0, "MSD"},\
    {SOC_BLK_MSS, 0, "MSS"},\
    {SOC_BLK_HRC, 0, "HRC"},\
    {SOC_BLK_MACSEC, 0, "MACSEC"},\
    {SOC_BLK_ALIGNER, 0, "ALIGNER"},\
    {SOC_BLK_MSU, 0, "MSU"},\
    {SOC_BLK_PMU, 0, "PMU"},\
    {SOC_BLK_NBU, 0, "NBU"},\
    {SOC_BLK_DDHX, 0, "DDHX"},\
    {SOC_BLK_TCMX, 0, "TCMX"},\
    {SOC_BLK_MKX, 0, "MKX"},\
    {SOC_BLK_MDBA, 0, "MDBA"},\
    {SOC_BLK_MDBB, 0, "MDBB"},\
    {SOC_BLK_MDBK, 0, "MDBK"},\
    {SOC_BLK_MGU, 0, "MGU"},\
    {SOC_BLK_DDHC, 0, "DDHC"},\
    {SOC_BLK_DDHAB, 0, "DDHAB"},\
    {SOC_BLK_MACTMNG, 0, "MACTMNG"},\
    {SOC_BLK_FDTM, 0, "FDTM"},\
    {SOC_BLK_EPPS, 0, "EPPS"},\
    {SOC_BLK_CPX, 0, "CPX"},\
    {SOC_BLK_SAT, 0, "SAT"},\
    {SOC_BLK_FDTS, 0, "FDTS"},\
    {SOC_BLK_NONE,       0,  ""      }}


#define SOC_BLOCK_DPP_PORT_NAMES_INITIALIZER {  \
    /*    blk  , isalias, name */ \
    { SOC_BLK_NONE,     0,  ""  }}

typedef union {
    soc_reg_t en_reg;
    soc_mem_t en_mem;
} _soc_ser_ctrl_type_t;

typedef struct _soc_ser_ctrl_info_s{
    _soc_ser_ctrl_type_t ctrl_type;
    uint8 flag_reg_mem;
    soc_field_t en_fld;
    int en_fld_position;
} _soc_ser_ctrl_info_t;

typedef struct {
    soc_mem_t mem;
    _soc_ser_ctrl_info_t en_ctrl;
    _soc_ser_ctrl_info_t ecc1b_ctrl;
} _soc_mem_ser_en_info_t;

typedef struct {
    soc_reg_t reg;
    _soc_ser_ctrl_info_t en_ctrl;
    _soc_ser_ctrl_info_t ecc1b_ctrl;
    int action; /* clear 0, restore 1 */
} _soc_reg_ser_en_info_t;

typedef struct {
    char bus_name[64];
    soc_reg_t en_reg;
    soc_field_t en_fld;
    soc_reg_t ecc1b_en_reg;
    soc_field_t ecc1b_en_fld;
} _soc_bus_ser_en_info_t;

typedef struct {
    char buffer_name[64];
    soc_reg_t en_reg;
    soc_field_t en_fld;
    soc_reg_t ecc1b_en_reg;
    soc_field_t ecc1b_en_fld;
} _soc_buffer_ser_en_info_t;

typedef struct {
    soc_mem_t   mem;
    soc_reg_t   enable_reg;
    soc_field_t enable_field;
    soc_field_t parity_field; /* used to sw inject test */
} _soc_tcam_ser_en_info_t;

typedef struct {
    char name[64];
    soc_reg_t force_reg;
    soc_field_t force_fld;
} _soc_bus_buffer_ser_force_info_t;
/*
 * soc_feature_fun_t: boolean function indicating if feature is supported
 * Used in bcm*.i files.
 */
typedef int (*soc_feature_fun_t) (int unit, soc_feature_t feature);

/*
 * soc_init_chip_fun_t: chip initialization function
 * Used in bcm*.i files.
 */
typedef void (*soc_init_chip_fun_t) (void);

/* Use macros to access */
extern soc_chip_groups_t soc_chip_type_map[SOC_CHIP_TYPES_COUNT];
extern char *soc_chip_type_names[SOC_CHIP_TYPES_COUNT];
extern char *soc_chip_group_names[SOC_CHIP_GROUPS_COUNT];
extern uint32 soc_ports_list[][8];
extern soc_numelport_set_t soc_numelports_list[][5];

#define SOC_CHIP_NAME(type)    (soc_chip_type_names[(type)])

#endif  /* !_SOC_CHIP_H */

