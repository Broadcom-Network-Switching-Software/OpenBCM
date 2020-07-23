/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File for SOC Bond Options Info structures and routines
 */

#ifndef _SOC_BONDOPTIONS_H
#define _SOC_BONDOPTIONS_H

#include <soc/drv.h>


typedef enum {

    /*  Feature Enables */
    socBondInfoFeatureNoL3,           /*  L3  */
    socBondInfoFeatureNoEcmp,           /*  Ecmp */
    socBondInfoFeatureNoAlpm,           /*  Alpm */
    socBondInfoFeatureNoL3Tunnel,       /*  L3 Tunnel */
    socBondInfoFeatureNoStaticNat,      /*  NAT */
    socBondInfoFeatureIpmcDisabled,   /*  IPMC */
    socBondInfoFeatureNoMim,            /*  MIM */
    socBondInfoFeatureNoMpls,           /*  MPLS */
    socBondInfoFeatureNoTrill,          /*  TRILL */
    socBondInfoFeatureNoVxlan,          /*  Vxlan */
    socBondInfoFeatureNoVxlanLite,      /*  Vxlan Lite */
    socBondInfoFeatureNoL2Gre,          /*  L2GRE */
    socBondInfoFeatureNoRiot,           /*  RIOT */
    socBondInfoFeatureNoEtherTm,        /*  Ether TM */
    socBondInfoFeatureNoFcoe,           /*  FCOE */
    socBondInfoFeatureNoTimeSync,       /*  TimeSync */
    socBondInfoFeatureNoHgProxyCoe,     /*  TD2+ COE */
    socBondInfoFeatureNoCoeVlanPause,   /*  COE Vlan Pause */
    socBondInfoFeatureNoCoeSubtag8100,  /*  COE can use 0x8100 */
    socBondInfoFeatureNoFpOam,          /*  FP-Based OAM */
    socBondInfoFeatureNoOsubCT,         /*  OS+CT support */
    socBondInfoFeatureNoCutThru,      /*  Cut-thru support */
    socBondInfoFeatureNoSat,            /*  SAT */
    socBondInfoFeatureNoFpRouting,      /*  IFP Based Routing */
    socBondInfoFeatureNoOam,            /*  OAM */
    socBondInfoFeatureNoLinkPhy,        /*  Link PHY */
    socBondInfoFeatureNoSubTagCoe,      /*  Subtag COE */
    socBondInfoFeatureNoPTP,            /*  PTP */
    socBondInfoFeatureNoServiceMeter,   /*  Service meter */
    socBondInfoFeatureNoAvs,            /*  AVS */
    socBondInfoFeatureNoOamTrueUpMep,   /*  OAM TRUE UPMEP       */
    socBondInfoFeatureNoEpRedirectV2,   /*  EP REDIRECT V2.0     */
    socBondInfoFeatureNoMultiLevelProt, /*  MULTI LEVEL PROT     */
    socBondInfoFeatureNoFpSat,          /*  FP-SAT (SAT Hooks)   */
    socBondInfoFeatureNoSegmentRouting, /*  Segment Routing      */
    socBondInfoFeatureNoCAUI10Support,  /*  CAUI10 Support       */
    socBondInfoFeatureHqos3Levels,    /*  HQOS 3 levels Support*/
    socBondInfoFeatureNoQCN,            /*  QCN                  */
    socBondInfoFeatureNoECN,            /*  ECN                  */
    socBondInfoFeatureNoDMVoq,          /*  DMVOQ                */
    socBondInfoFeatureNoDPVoq,          /*  DPVOQ                */
    socBondInfoFeatureNoPipe_2_3,       /*  PIPE2,3 */
    socBondInfoFeatureNoExactMatch,     /*  UFT ACL Match */
    socBondInfoFeatureNoEfp,            /*  EFP */
    socBondInfoFeatureNoIfp,            /*  IFP */
    socBondInfoFeatureNoVfp,            /*  VFP */
    socBondInfoFeatureNoLpm,            /*  LPM */
    socBondInfoFeatureNoUftBank0,       /*  UFT Bank 0 */
    socBondInfoFeatureNoUftBank1,       /*  UFT Bank 1 */
    socBondInfoFeatureNoUftBank2,       /*  UFT Bank 2 */
    socBondInfoFeatureNoUftBank3,       /*  UFT Bank 3 */
    socBondInfoFeatureNoMplsFrr,        /*  MPLS FRR */
    socBondInfoFeatureNoDynamicLoadBalancing, /*  DLB */
    socBondInfoFeatureNoDynamicAlternatePath, /*  Dynamic Alternate Path */
    socBondInfoFeatureNoPstats,         /*  Packetized Stats */
    socBondInfoFeatureNoTCB,            /*  TCB */
    socBondInfoFeatureNoTimestamp,      /*  Timestamp */
    socBondInfoFeatureNoRRCOE,          /*  RRCOE */
    socBondInfoFeatureNoPreemption,     /*  Preemption */
    socBondInfoFeatureNoTAS,            /*  TAS */
    socBondInfoFeatureNoHSR,            /*  HSR */
    socBondInfoFeatureNoPRP,            /*  PRP */
    socBondInfoFeatureNo8021CB,         /*  802_1CB */
    socBondInfoFeatureNoMacsec,         /*  MACSEC */
    socBondInfoFeatureNoReserveTunnel,  /*  RESERVE TUNNEL */

    /*  Table size updates */
    socBondInfoFeatureVrf1k,         /*  1K VRF */
    socBondInfoFeatureVrf2k,         /*  2K VRF */
    socBondInfoFeatureVrf4k,         /*  4K VRF */
    socBondInfoFeatureL3Entry1k,     /*  L3_ENTRY */
    socBondInfoFeatureL3Iif8k,       /*  8K IIF */
    socBondInfoFeatureL3Iif12k,      /*  12K IIF */
    socBondInfoFeatureUft128k,       /*  Half UFT - 128K */
    socBondInfoFeatureUft256k,       /*  Full UFT - 256K */
    socBondInfoFeatureIpmc64,        /*  64 IPMC */
    socBondInfoFeatureIpmc1k,        /*  1K IPMC */
    socBondInfoFeatureIpmc4k,        /*  4K IPMC */
    socBondInfoFeatureIpmc8k,        /*  8K IPMC */
    socBondInfoFeatureMmu1Mb,        /*  1MB Buffers */
    socBondInfoFeatureMmu2Mb,        /*  2MB Buffers */
    socBondInfoFeatureMmu6Mb,        /*  6MB Buffers */
    socBondInfoFeatureMmu9Mb,        /*  9MB Buffers */
    socBondInfoFeatureMmu12Mb,       /*  12MB Buffers */
    socBondInfoFeatureIfp64HalfSlice,  /*  IFP Half slice (64)*/
    socBondInfoFeatureIfpSliceHalf,   /*  IFP Half slice (512) */
    socBondInfoFeatureEfp64HalfSlice, /* EFP Half Slice (64) */
    socBondInfoFeatureEfp512HalfSlice,/* EFP Half Slice (512) */
    socBondInfoFeatureVfp64HalfSlice,/* VFP Half Slice (64) */
    socBondInfoFeatureVfp512HalfSlice,/* VFP Half Slice (512) */
    socBondInfoFeatureMpls1k,         /* 1K MPLS Table*/
    socBondInfoFeatureMpls2k,         /* 2K MPLS Table*/
    socBondInfoFeatureL2Entry4k,      /* 4K L2_ENTRY Table*/
    socBondInfoFeatureL2Entry8k,      /* 8K L2_ENTRY Table*/
    socBondInfoFeatureL2Entry16k,     /* 16K L2_ENTRY Table*/
    socBondInfoFeatureL2Entry32k,     /* 32K L2_ENTRY Table*/
    socBondInfoFeatureIpVlanXlate256, /* 256 IP VLAN_XLATE Table*/
    socBondInfoFeatureIpVlanXlate1k,  /* 1K IP VLAN_XLATE Table*/
    socBondInfoFeatureIpVlanXlate4k,  /* 4K IP VLAN_XLATE Table*/
    socBondInfoFeatureL3NextHop768,   /* 768 L3_NEXT_HOP Table*/
    socBondInfoFeatureL3NextHop1k,    /* 1K L3_NEXT_HOP Table*/
    socBondInfoFeatureL3NextHop2k,    /* 2K L3_NEXT_HOP Table*/
    socBondInfoFeatureL3NextHop5k,    /* 5K L3_NEXT_HOP Table*/
    socBondInfoFeatureL3DefIpDisable,/*  L3_DEF_IP Table*/
    socBondInfoFeatureL2mc256,       /*  256 L2MC */
    socBondInfoFeatureL2mc4k,        /*  4K L2MC */
    socBondInfoFeatureSVP4k,         /*  4K SVP */
    socBondInfoFeatureL3HostMax512,  /*  512 L3 Max Host */
    socBondInfoFeatureL3HostMax4k,   /*  4K L3 Max Host */
    socBondInfoFeatureVrf16,         /*  16 VRFs */
    socBondInfoFeatureL3NHop8k,      /*  8k NHOPs */
    socBondInfoFeatureL3NHop32k,     /* 32k NHOPs */
    socBondInfoFeatureL3Intf8k,      /*  8k L3_INTF */
    socBondInfoFeatureL3Intf12k,     /* 12k L3_INTF */
    socBondInfoFeatureIfpSlice1k5,    /*  IFP Slice Depth 1.5k Entries */
    socBondInfoFeatureIfpSlice3k,     /*  IFP Slice Depth 3k Entries */
    socBondInfoFeatureIfpSlice6k,     /*  IFP Slice Depth 6k Entries */
    socBondInfoFeatureLpm64,          /*  64 LPM */
    socBondInfoFeatureLpm1k,          /*  1K LPM */
    socBondInfoFeatureLpm2k,          /*  2K LPM */
    socBondInfoFeatureLpm8k,          /*  8K LPM */
    socBondInfoFeatureLpm16k,         /*  16K LPM */
    socBondInfoFeatureNextHop16k,     /*  16K Next Hop */
    socBondInfoFeatureNextHop32k,     /*  32K Next Hop */
    socBondInfoFeatureNextHop48k,     /*  48K Next Hop */
    socBondInfoFeatureStg32,          /*  32  span tree groups */
    socBondInfoFeatureStg256,         /*  256 span tree groups */
    socBondInfoFeatureSRFlowId256,    /*  256 Seamless Redundancy FlowID size */
    socBondInfoFeatureSRFlowId4k,     /*  4k Seamless Redundancy FlowID size */

    /*  iProc Related */
    socBondInfoFeatureUc0Active,     /*  UC0 Active */
    socBondInfoFeatureUc1Active,     /*  UC1 Active */
    socBondInfoFeatureSramPwrDwn,    /*  SRAM Powered Down */
    socBondInfoFeatureSpiCodeSize32KB, /*  SPI - 32KB */
    socBondInfoFeatureSpiCodeSize64KB, /*  SPI - 64KB */
    socBondInfoFeatureSpiCodeSize128KB,/*  SPI - 128KB */
    socBondInfoFeatureTs1588,          /*  TS-1588 */
    socBondInfoNoIHost,                /* iHost Disable */
    socBondInfoFeatureA72Frequency1Ghz, /* A72 Freq 1 Ghz */
    socBondInfoFeatureA72Frequency2Ghz, /* A72 Freq 2 Ghz */

    /* Chip Specific */
    socBondInfoFeatureFamilyApache,
    socBondInfoFeatureFamilyMaverick,
    socBondInfoFeatureFamilyFirebolt5,
    socBondInfoFeatureFamilyRanger2Plus,
    socBondInfoFeatureApacheCoSProfile0,
    socBondInfoFeatureApacheCoSProfile1,
    socBondInfoFeatureApacheCoSProfile2,
    socBondInfoFeatureApachePkg42p5mm,
    socBondInfoFeatureApachePkg50mmTDpCompat,
    socBondInfoFeatureApachePkg50mmTD2pCompat,
    socBondInfoFeatureMetroliteL1SyncE,
    socBondInfoFeatureMetroliteTelecomDPLL,

    /* Broadscan related */
    socBondInfoBskDisable,        /* Broadscan Disable */
    socBondInfoBskNoPktSampling,  /* BROADSCAN packet sampling disable */
    socBondInfoBskNoHWExportPktBuild,  /* BROADSCAN HW export packet build disable */
    socBondInfoBskNoTCPBidir,  /* BROADSCAN TCP bidir processing disable */
    socBondInfoBskNoDoSAttackVector,  /* BROADSCAN DOS attack vector check disable */
    socBondInfoBskNoMicroFlowPolicer, /* BROADSCAN micro flow policer disable */
    socBondInfoBskNoALU32Inst2to11,  /* BROADSCAN ALU32 inst 2_to_11 disable */
    socBondInfoBskSessionDepth3Banks, /* BROADSCAN Session table 3 banks allocated */
    socBondInfoBskSessionDepth5Banks, /* BROADSCAN Session table 5 banks allocated */
    socBondInfoBskSessionDepth9Banks, /* BROADSCAN Session table 9 banks allocated */

    /* PortMacro related */
    socBondInfoPM4x10QInst0UsxgmiiDisable, /* PMQ#0: USXGMII mode is disabled */
    socBondInfoPM4x10QInst1UsxgmiiDisable, /* PMQ#1: USXGMII mode is disabled */
    socBondInfoPM4x10QInst0QsgmiiDisable,  /* PMQ#0: QSGMII mode is disabled */
    socBondInfoPM4x10QInst1QsgmiiDisable,  /* PMQ#1: QSGMII mode is disabled */
    socBondInfoPM4x10QInst0Disable,        /* PMQ#0 block is disabled */
    socBondInfoPM4x10QInst1Disable,        /* PMQ#1 block is disabled */

    /* Broadscan2.0 related */
    socBondInfoBskGroupDepthHalfEntries,   /* BROADSCAN2.0 FT group table depth half entries */
    socBondInfoBskNoDropAnalysis,          /* BROADSCAN2.0 Drop reason tracking disable */
    socBondInfoBskNoLatencyTracking,       /* BROADSCAN2.0 E2E and Chip Delay tracking disable */
    socBondInfoBskNoAluLatencySupport,     /* BROADSCAN2.0 ALU Delay calculation disable */
    socBondInfoBskNoIATIDTTracking,        /* BROADSCAN2.0 IAT & IDT Delay tracking disable */
    socBondInfoBskNoCongestionTracking,    /* BROADSCAN2.0 MMU congestion level delay trakcing disabled */
    socBondInfoBskNoLoadBalancingTracking, /* BROADSCAN2.0 Load Balancing tracking disable */

    /* Broadscan 3.0 related */
    socBondInfoBskAggregateDisable,        /* BROADSCAN3.0 Aggregate Tracking disable */

    /* Channelized switching */
    socBondInfoChannelizedSwitchingDisable,  /* Channelized Switching disable */

    socBondInfoFeatureIpmc16k,              /* 16K IPMC */
    socBondInfoFeatureNextHop64k,           /* 64K Next Hops */

    socBondInfoFeatureCount /* Alyways Last */
} soc_bond_info_features_t;


/*
 * Bond Options structure.
 *
 * New elements that cannot be fit into the 'features' definition should be
 * defined as spearate elements in the structure.
 */
typedef struct soc_bond_info_s {
    uint16 dev_id;                  /*  16b:   DEVICE_ID               */
    uint16 rev_id;                  /*   8b:   REV_ID                  */

    uint32 device_skew;             /*   4b:   DEVICE_SKEW             */
    uint32 max_core_freq;           /*   3b:   MAX_CORE_FREQUENCY      */
    uint32 default_core_freq;       /*   3b:   DEFAULT_CORE_FREQUENCY  */
    uint32 mb_bank_num;             /*   4b:   MMU_MB_NUM_BANKS        */
    uint32 dpp_clk_ratio;           /*   2b:   DPP TO CORE RATIO       */
    uint32 avs_status;              /*   8b:   AVS_STATUS              */
    uint32 cbp_buffer_size;         /*   2b:   CBP_BUFFER_SIZE         */
    uint32 max_xqs_number;          /*   1b:   MAX_XQS_NUMBER          */

    uint32 defip_cam_map;           /*   8b:   DEFIP_CAM_ENABLE        */
    uint32 ifp_slice_map;           /*  12b:   IFP_SLICE_ENABLE        */
    uint32 efp_slice_map;           /*   4b:   EFP_SLICE_ENABLE        */
    uint32 vfp_slice_map;           /*   4b:   VFP_SLICE_ENABLE        */
    uint32 tsce_max_2p5g;           /*   14b:  FORCE_2P5G_MAX          */
    uint32 tscf_max_11g;            /*   4b:   FORCE_11G_MAX           */
    uint32 tsc_force_hg;            /*   18b:  FORCE_HG                */
    uint32 tsc_enable;              /*   18b:  TSC_ENABLE              */
    uint32 clmac_map;               /*   6b:   CLMAC_ENABLE            */
    uint32 xlmac_map;               /*  18b:   XLMAC_ENABLE            */
    uint32 utt_banks_map;           /*  16b:   UTT_TCAM_BANK_ENABLE    */

    SHR_BITDCL tsc_disabled[_SHR_BITDCLSIZE(65)];   /* 65b: TSC_DISABLE         */
    SHR_BITDCL force_hg[_SHR_BITDCLSIZE(64)];       /* 64b: FORCE_HG            */
    SHR_BITDCL tsc_max_speed[_SHR_BITDCLSIZE(128)]; /* 128b: TSC_MAX_LANE_SPEED */
    SHR_BITDCL tsc_sr_only[_SHR_BITDCLSIZE(64)];    /* 64b: TSC_SR_ONLY         */
    SHR_BITDCL tsc_in_loop[_SHR_BITDCLSIZE(128)];   /* 128b: TSC_FORCE_LOOPBACK */
    SHR_BITDCL cpri_enable[_SHR_BITDCLSIZE(48)];    /* 48b: TSC_CPRI_ENABE      */
    SHR_BITDCL ic_id[_SHR_BITDCLSIZE(64)];          /* 64b: ICID                */

    /*  Bitmap of bondoption features */
    SHR_BITDCL features[_SHR_BITDCLSIZE(socBondInfoFeatureCount)];
} soc_bond_info_t;

extern soc_bond_info_t *soc_bond_info[SOC_MAX_NUM_DEVICES];

#define SOC_BOND_INFO(unit) (soc_bond_info[unit])

/* Bond Info Features cache  - Access macros */
#define SOC_BOND_INFO_FEATURE_GET(unit, feat)   \
        SHR_BITGET(SOC_BOND_INFO(unit)->features, feat)
#define SOC_BOND_INFO_FEATURE_SET(unit, feat)   \
        SHR_BITSET(SOC_BOND_INFO(unit)->features, feat)
#define SOC_BOND_INFO_FEATURE_CLEAR(unit, feat) \
        SHR_BITCLR(SOC_BOND_INFO(unit)->features, feat)

#define SOC_BOND_INFO_GET(unit, attrib)     (SOC_BOND_INFO(unit)->attrib)
#define SOC_BOND_INFO_SET(unit, attrib)     (SOC_BOND_INFO(unit)->attrib)

#define SOC_BOND_INFO_REG_MAX       16
#define SOC_BOND_INFO_REG_WIDTH     32
#define SOC_BOND_INFO_MAX_BITS      (SOC_BOND_INFO_REG_MAX * SOC_BOND_INFO_REG_WIDTH)

#define SOC_BOND_OPTIONS_COPY(_toval_, __cast__, _fromval_, _sbit_, _ebit_)  \
    do {                                                                     \
        {                                                                    \
            uint32 bitrange_val = 0;                                         \
            SHR_BITCOPY_RANGE(&bitrange_val, 0,                              \
                              (CONST SHR_BITDCL *)(_fromval_),               \
                              (_sbit_), ((_ebit_) - (_sbit_) + 1));          \
            _toval_ = (__cast__)bitrange_val;                                \
        }                                                                    \
    } while (0)


extern int soc_bond_info_registers[SOC_BOND_INFO_REG_MAX];

#endif /* !_SOC_BONDOPTIONS_H */
