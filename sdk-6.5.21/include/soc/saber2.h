/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        saber2.h
 */

#ifndef _SOC_SABER2_H_
#define _SOC_SABER2_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/katana2.h>
#include <soc/shmoo_and28.h>
typedef struct sb2_pbmp_s {
    soc_pbmp_t *pbmp_gport_stack;
    soc_pbmp_t *pbmp_mxq;
    soc_pbmp_t *pbmp_mxq1g;
    soc_pbmp_t *pbmp_mxq2p5g;
    soc_pbmp_t *pbmp_mxq10g;
    soc_pbmp_t *pbmp_xl;
    soc_pbmp_t *pbmp_xl1g;
    soc_pbmp_t *pbmp_xl2p5g;
    soc_pbmp_t *pbmp_xl10g;
    soc_pbmp_t *pbmp_xport_ge;
    soc_pbmp_t *pbmp_xport_xe;
    soc_pbmp_t *pbmp_valid;
    soc_pbmp_t *pbmp_pp;
    soc_pbmp_t *pbmp_linkphy;
}sb2_pbmp_t;


typedef struct  sb2_tdm_pos_info_s {
        uint16 total_slots;
        int16  pos[180];
} sb2_tdm_pos_info_t;

typedef struct bcm56260_tdm_info_s {
        uint8  tdm_freq;
        uint32  tdm_size;

        /* Display purpose only */
        uint8  row;
        uint8  col;
}bcm56260_tdm_info_t;

typedef enum bcmXlPhyPortMode_s {
    bcmXlPhyPortModeQuad=0,
    bcmXlPhyPortModeTri012=1,
    bcmXlPhyPortModeTri023=2,
    bcmXlPhyPortModeDual=3,
    bcmXlPhyPortModeSingle=4,
    bcmXlPhyPortModeTdmDisable=5
}bcmXlPhyPortMode_t;

typedef enum bcmXlCorePortMode_s {
    bcmXlCorePortModeQuad=0,
    bcmXlCorePortModeTri012=1,
    bcmXlCorePortModeTri023=2,
    bcmXlCorePortModeDual=3,
    bcmXlCorePortModeSingle=4,
    bcmXlCorePortModeTdmDisable=5
}bcmXlCorePortMode_t;

#define SB2_BIST_FLAGS_CONS_ADDR_8_BANKS            (0x0001)
#define SB2_BIST_FLAGS_ADDRESS_SHIFT_MODE           (0X0002)
#define SB2_BIST_FLAGS_INFINITE                     (0x0004)
#define SB2_BIST_FLAGS_ALL_ADDRESS                  (0x0008)
#define SB2_BIST_FLAGS_STOP                         (0x0010)
#define SB2_BIST_FLAGS_GET_DATA                     (0x0020)
#define SB2_BIST_FLAGS_TWO_ADDRESS_MODE             (0x0040)
#define SB2_BIST_FLAGS_BG_INTERLEAVE                (0x0080)
#define SB2_BIST_FLAGS_SINGLE_BANK_TEST             (0x0100)
#define SB2_BIST_FLAGS_MPR_STAGGER_MODE             (0x0200)  
#define SB2_BIST_FLAGS_MPR_STAGGER_INCREMENT_MODE   (0x0400)
#define SB2_BIST_FLAGS_MPR_READOUT_MODE             (0x0800)
#define SB2_BIST_FLAGS_ADDRESS_PRBS_MODE            (0x1000)
#define SB2_BIST_FLAGS_USE_RANDOM_DATA_SEED         (0x2000)    
#define SB2_BIST_FLAGS_USE_RANDOM_DBI_SEED          (0x4000) 

#define SB2_BIST_FLAGS_USE_RANDOM_SEED \
    (SB2_BIST_FLAGS_USE_RANDOM_DATA_SEED | SB2_BIST_FLAGS_USE_RANDOM_DBI_SEED)


#define SB2_BIST_NUM_PATTERNS 8
#define SB2_BIST_NUM_SEEDS 8
#define SB2_BIST_MPR_NUM_PATTERNS 4

typedef enum
{
    SB2_BIST_MPR_MODE_SERIAL = 0,
    SB2_BIST_MPR_MODE_PARALLEL = 1,
    SB2_BIST_MPR_MODE_STAGARED = 2,
    SB2_BIST_MPR_DISABLE = 3
} SB2_BIST_MPR_MODE;

typedef struct {
    SB2_BIST_MPR_MODE mpr_mode;
    uint32 mpr_readout_mpr_location;
    uint8 mpr_pattern[SB2_BIST_MPR_NUM_PATTERNS];
} sb2_bist_mpr_info;

typedef enum {
    SB2_DRAM_BIST_DATA_PATTERN_CUSTOM = 0,
    SB2_DRAM_BIST_DATA_PATTERN_RANDOM_PRBS = 1,
    SB2_DRAM_BIST_DATA_PATTERN_DIFF = 2,
    SB2_DRAM_BIST_DATA_PATTERN_ONE = 3,
    SB2_DRAM_BIST_DATA_PATTERN_ZERO = 4,
    SB2_DRAM_BIST_DATA_PATTERN_BIT_MODE = 5,
    SB2_DRAM_BIST_DATA_PATTERN_SHIFT_MODE = 6,
    SB2_DRAM_BIST_DATA_PATTERN_ADDR_MODE = 7,
    SB2_DRAM_BIST_NOF_DATA_PATTERN_MODES = 8
}SB2_DRAM_BIST_DATA_PATTERN_MODE;

typedef struct {
    uint32 write_weight;
    uint32 read_weight;
    uint32 bist_num_actions;
    uint32 bist_start_address;
    uint32 bist_end_address;
    SB2_DRAM_BIST_DATA_PATTERN_MODE pattern_mode;
    uint32 pattern[SB2_BIST_NUM_PATTERNS];
    uint32 data_seed[SB2_BIST_NUM_SEEDS];
    uint32 bist_flags;
}sb2_bist_info;

#define SB2_MEM_GRADE_080808        0x080808
#define SB2_MEM_GRADE_090909        0x090909
#define SB2_MEM_GRADE_101010        0x101010
#define SB2_MEM_GRADE_111111        0x111111
#define SB2_MEM_GRADE_121212        0x121212
#define SB2_MEM_GRADE_131313        0x131313
#define SB2_MEM_GRADE_141414        0x141414

#define SB2_MEM_ROWS_16K        16384
#define SB2_MEM_ROWS_32K        32768

#define SB2_DDR_FREQ_667        667
#define SB2_DDR_FREQ_800        800

/* Core PLL clock frequencies */
#define CORE_PLL_CLOCK_130M     130
#define CORE_PLL_CLOCK_118M     118
#define CORE_PLL_CLOCK_75M      75
#define CORE_PLL_CLOCK_37M      37
#define CORE_PLL_CLOCK_24M      24

enum sb2_mem_size_set {
    SB2_MEM_4G,
    SB2_MEM_2G,
    SB2_MEM_1G,
    SB2_MEM_COUNT
};

enum sb2_freq_set {
   SB2_FREQ_667,
   SB2_FREQ_800,
   SB2_FREQ_CNT
};

enum sb2_grade_set {
    SB2_GRADE_DEFAULT,
    SB2_GRADE_080808,
    SB2_GRADE_090909,
    SB2_GRADE_101010,
    SB2_GRADE_111111,
    SB2_GRADE_121212,
    SB2_GRADE_131313,
    SB2_GRADE_141414,
    SB2_GRADE_CNT
};

enum sb2_core_pll_clock {
    SB2_CORE_PLL_CLOCK_130M,
    SB2_CORE_PLL_CLOCK_118M,
    SB2_CORE_PLL_CLOCK_75M,
    SB2_CORE_PLL_CLOCK_37M,
    SB2_CORE_PLL_CLOCK_24M,
    SB2_CORE_PLL_CLOCK_CNT
};

#define SOC_SB2_AND28_INTERFACES_NUM_MAX            8
#define SOC_SB2_AND28_NOF_PER_INTERFACE_BYTES       4
#define SOC_SB2_AND28_NOF_BITS_IN_BYTE              8
#define SOC_SB2_AND28_MRS_NUM_MAX                   16


typedef struct {
    int twl, twr, trc;                                          /* CONFIG0 */
    int rfifo_afull, rfifo_full;                                /* CONFIG1 */
    int trtw, twtr, tfaw, tread_enb;                            /* CONFIG2 */
    int bank_unavail_rd, bank_unavail_wr, rr_read, rr_write;    /* CONFIG3 */
    int refrate;                                                /* CONFIG4 */
    int trp_read, trp_write, trfc[3];                           /* CONFIG6 */
    int tzq;                                                    /* CONFIG7 */
    int cl, cwl, wr;                                            /* PHY_STRAP0 */
    int jedec, mhz;                                             /* PHY_STRAP1 */
    uint32 mr0, mr2;                                            /* MR0, MR2   */
} sb2_req_grade_mem_set_set_t;

typedef struct soc_sb2_and28_info_s {
    uint32 device_core_freq;
    uint32 device_dram_num_max;
    uint8 enable;
    uint32 dram_bitmap;
    soc_pbmp_t ref_clk_bitmap;
    int dram_num;
    int dram_freq;
    int data_rate_mbps;
    int ref_clk_mhz;
    uint32 dram_type;
    uint32 nof_banks;
    uint32 nof_columns;
    uint32 nof_rows;
#if 0
    soc_sb2_and28_info_dram_param_t dram_param;
    soc_sb2_and28_info_dram_internal_param_t dram_int_param;
#endif
    uint32 mr[ SOC_SB2_AND28_MRS_NUM_MAX];
#if 0
    DRC_AND28_DRAM_CLAM_SHELL_MODE
        dram_clam_shell_mode[SOC_SB2_AND28_INTERFACES_NUM_MAX];
#endif
    uint32
           dram_dq_swap[SOC_SB2_AND28_INTERFACES_NUM_MAX][SOC_SB2_AND28_NOF_PER_INTERFACE_BYTES][SOC_SB2_AND28_NOF_BITS_IN_BYTE];

    uint32 gear_down_mode;
    uint32 alert_n_period_thrs;
    uint32 abi;
    uint32 write_dbi;
    uint32 read_dbi;
    uint32 write_crc;
    uint32 read_crc;
    uint32 cmd_par_latency;
    int auto_tune;
    and28_shmoo_config_param_t shmoo_config_param[SOC_SB2_AND28_INTERFACES_NUM_MAX];
    uint8 bist_enable;
    int sim_system_mode;
}soc_sb2_and28_info_t;

typedef struct _sb2_cosq_counter_mem_map_s {
    soc_mem_t mem;
    soc_counter_non_dma_id_t non_dma_id;
    char *cname_pkt;
    char *cname_byte;
}_sb2_cosq_counter_mem_map_t;

/* *************************************************************** */
/* SABER2 MMU HELPER  --- START(As per Saber2_MMU_Settings-5.xls */
/* *************************************************************** */
/* Dynamic/SetLater Parameter */
typedef struct _soc_sb2_mmu_params_s {
               uint32 mmu_min_pkt_size;                   /*C5*/
               uint32 mmu_ethernet_mtu_bytes;             /*C6*/
               uint32 mmu_max_pkt_size;                   /*C7*/
               uint32 mmu_jumbo_frame_size;               /*C8*/
               uint32 mmu_int_buf_cell_size;              /*C9*/
               /* mmu_ext_buf_cell_size = (mmu_int_buf_cell_size+2)*15;   */
               uint32 mmu_ext_buf_cell_size;              /*C10=2880*/
               uint32 mmu_pkt_header_size;                /*C11*/
               uint32 packing_mode_d_c;                   /* C12:Dynamic */
               uint32 max_pkt_size_support_packing;        /* C13:Dynamic */
               uint32 lossless_mode_d_c;                  /* C14:Dynamic */
               uint32 pfc_pause_mode_d_c;                 /* C15:Dynamic */
               uint32 mmu_lossless_pg_num;                /*C16*/
               uint32 extbuf_used_d_c;                    /* C17:Dynamic */
               uint32 num_ge_ports_d;                     /* C19:Dynamic */
               uint32 mmu_line_rate_ge;                   /*C20*/
               uint32 num_ge_int_ports_d;                 /* c21:Dynamic */
               uint32 num_egr_queue_per_int_ge_port_d;    /* C22:Dynamic */
               uint32 num_ge_ext_ports_d;                 /* C23:Dynamic */
               uint32 num_egr_queue_per_ext_ge_port_d;    /* C24:Dynamic */
               uint32 num_hg_ports_d;                     /* C25:Dynamic */
               uint32 mmu_line_rate_hg;                   /*C26*/
               uint32 num_hg_int_ports_d;                 /* C27:Dynamic */
               uint32 num_egr_queue_per_int_hg_port_d;    /* C28:Dynamic */
               uint32 num_hg_ext_ports_d;                 /* C29:Dynamic */
               uint32 num_egr_queue_per_ext_hg_port_d;    /* C30:Dynamic */
               uint32 mmu_num_cpu_port;                   /*C31*/
               uint32 mmu_num_cpu_queue;                  /*C32*/
               uint32 cpu_port_int_ext_bounding_d_c;      /* C33:Dynamic */
               uint32 mmu_num_loopback_port;              /*C34*/
               uint32 mmu_num_loopback_queue;             /*C35*/ 
               uint32 mmu_num_ep_redirection_queue;       /*C36*/
               uint32 mmu_num_olp_port_d;                 /*C37*/
               uint32 mmu_num_olp_queue;                  /*C38*/
               uint32 olp_port_int_ext_bounding_d_c;      /*C39:Dynamic*/
               uint32 mmu_total_egress_queue;             /*C40*/
               uint32 mmu_pipeline_lat_cpap_admission;    /*C41*/
               uint32 mmu_int_buf_size;                   /*C42:2048KB=2MB*/
               uint32 mmu_available_int_buf_size_d;       /*C43:1434KB */
               uint32 mmu_reserved_int_buf_cells;         /*C44:100 Cells */
               uint32 mmu_reserved_int_buf_ema_pool_size_d; /*C45:614KB */
               uint32 internal_buffer_reduction_d_c;        /* C46:Dynamic */
               uint32 mmu_ext_buf_size;                   /*C47:737280KB=720MB*/
               uint32 mmu_reserved_ext_buf_space_cfap;    /*C48:737280KB=720MB*/
               uint32 mmu_egress_queue_entries;           /*C49*/
               uint32 mmu_ep_redirect_queue_entries;      /*C50*/
               uint32 mmu_exp_num_of_repl_per_pkt;        /*C51*/
               uint32 mmu_repl_engine_work_queue_entries; /*C52*/
               uint32 mmu_resv_repl_engine_work_queue_entries; /*C53*/
               uint32 mmu_repl_engine_work_queue_in_device;/*C54*/
               uint32 mmu_ema_queues;                     /*C55*/
               uint32 num_cells_rsrvd_ing_ext_buf;        /* C56:Dynamic */     
               uint32 per_cos_res_cells_for_int_buff_d;   /* C58:Dynamic */
               uint32 per_cos_res_cells_for_ext_buff_d;   /* C59:Dynamic */
               uint32 mmu_ing_port_dyn_thd_param;         /*C60*/
               uint32 mmu_ing_pg_dyn_thd_param;           /*C61*/
               uint32 mmu_egr_queue_dyn_thd_param_baf;        /*C62*/
               uint32 mmu_egr_queue_dyn_thd_param_baf_profile_lossy;           /*C63*/
               uint32 mmu_ing_cell_buf_reduction;         /*C64 */
               uint32 mmu_ing_pkt_buf_reduction;          /*C65 */
} _soc_sb2_mmu_params_t;
extern _soc_sb2_mmu_params_t _soc_sb2_mmu_params;


typedef struct _sb2_general_info_s {
    uint32 max_packet_size_in_cells;                         /* C72 */
    uint32 jumbo_frame_for_int_buff;                         /* C73 */
    uint32 jumbo_frame_for_ext_buff;                         /* C74 */
    uint32 ether_mtu_cells_for_int_buff;                     /* C76 */
    uint32 ether_mtu_cells_for_ext_buff;                     /* C77 */
    uint32 total_num_of_ports;                               /* C79 */
    uint32 total_num_of_ports_excl_lpbk;                     /* C80 */
    uint32 total_num_of_ports_excl_lpbk_olp;                 /* C81 */
    uint32 total_num_of_ports_excl_lpbk_olp_cpu;             /* C82 */
    uint32 port_bw_bound_to_ext_buff;                        /* C83 */
    uint32 total_egr_queues_for_a_int_ge_ports;              /* C84 */
    uint32 total_egr_queues_for_a_ext_ge_ports;              /* C85 */
    uint32 total_egr_queues_for_a_int_hg_ports;              /* C86 */
    uint32 total_egr_queues_for_a_ext_hg_ports;              /* C87 */
    uint32 total_cpu_queues;                                 /* C88 */
    uint32 total_base_queue_int_buff;                       /* C89 */
    uint32 total_base_queue_ext_buff;                        /* C90 */
    uint32 total_ema_queues;                                 /* C91 */
    uint32 total_egr_base_queues_in_device;                  /* C93 */
    uint32 total_egr_queues_in_device;                       /* C94 */
    uint32 max_int_cell_buff_size;                           /* C95 */
    uint32 int_cell_buff_size_after_limitation;              /* C96 */
    uint32 src_packing_fifo;                                 /* C97 */
    uint32 int_buff_pool;                                    /* C98 */
    uint32 ema_pool;                                         /* C99 */
    uint32 max_ext_cell_buff_size;                           /* C100 */
    uint32 repl_engine_work_queue_entries;                   /* C101 */
    uint32 ratio_of_ext_buff_to_int_buff_size;               /* C103 */
    uint32 int_buff_cells_per_avg_size_pkt;                  /* C104 */
    uint32 ext_buff_cells_per_avg_size_pkt;                  /* C105 */
    uint32 max_prop_of_buff_used_by_one_queue_port;          /* C106 */
}_sb2_general_info_t;

typedef struct _sb2_input_port_threshold_s {
    uint32 global_hdrm_cells_for_int_buff_ingress_pool;      /* C110 */
    uint32 global_hdrm_cells_for_int_buff_ema_pool;          /* C111 */
    uint32 global_hdrm_cells_for_ext_buff_pool;              /* C112 */
    uint32 global_hdrm_cells_for_RE_WQEs;                    /* C113 */
    uint32 global_hdrm_cells_for_EQEs;                       /* C114 */

    uint32 hdrm_int_buff_cells_for_10G_PG;                   /* C115 */
    uint32 hdrm_ema_buff_cells_for_10G_PG;                   /* C116 */
    uint32 hdrm_ext_buff_cells_for_10G_PG;                   /* C117 */
    uint32 hdrm_RE_WQEs_pkts_for_10G_PG;                     /* C118 */
    uint32 hdrm_EQEs_pkts_for_10G_PG;                        /* C119 */

    uint32 hdrm_int_buff_cells_for_10G_total_PG;             /* C120 */
    uint32 hdrm_ema_buff_cells_for_10G_total_PG;             /* C121 */
    uint32 hdrm_ext_buff_cells_for_10G_total_PG;             /* C122 */
    uint32 hdrm_RE_WQEs_pkts_for_10G_total_PG;               /* C123 */
    uint32 hdrm_EQEs_pkts_for_10G_total_PG;                  /* C124 */

    uint32 hdrm_int_buff_cells_for_1G_PG;                    /* C125 */
    uint32 hdrm_ema_buff_cells_for_1G_PG;                    /* C126 */
    uint32 hdrm_ext_buff_cells_for_1G_PG;                    /* C127 */
    uint32 hdrm_RE_WQEs_pkts_for_1G_PG;                      /* C128 */
    uint32 hdrm_EQEs_pkts_for_1G_PG;                         /* C129 */

    uint32 hdrm_int_buff_cells_for_1G_total_PG;              /* C130 */
    uint32 hdrm_ema_buff_cells_for_1G_total_PG;              /* C131 */
    uint32 hdrm_ext_buff_cells_for_1G_total_PG;              /* C132 */
    uint32 hdrm_RE_WQEs_pkts_for_1G_total_PG;                /* C133 */
    uint32 hdrm_EQEs_pkts_for_1G_total_PG;                   /* C134 */

    uint32 hdrm_int_buff_cells_for_olp_port;                 /* C135 */
    uint32 hdrm_ema_buff_cells_for_olp_port;                 /* C136 */
    uint32 hdrm_ext_buff_cells_for_olp_port;                 /* C137 */
    uint32 hdrm_RE_WQEs_pkts_for_olp_port;                   /* C138 */
    uint32 hdrm_EQEs_pkts_for_olp_port;                      /* C139 */

    uint32 hdrm_int_buff_cells_for_lpbk_port;                /* C140 */
    uint32 hdrm_ema_buff_cells_for_lpbk_port;                /* C141 */
    uint32 hdrm_ext_buff_cells_for_lpbk_port;                /* C142 */
    uint32 hdrm_RE_WQEs_pkts_for_lpbk_port;                  /* C143 */
    uint32 hdrm_EQEs_pkts_for_lpbk_port;                     /* C144 */

    uint32 hdrm_int_buff_cells_for_cpu_port;                 /* C145 */
    uint32 hdrm_ema_buff_cells_for_cpu_port;                 /* C146 */
    uint32 hdrm_ext_buff_cells_for_cpu_port;                 /* C147 */
    uint32 hdrm_RE_WQEs_pkts_for_cpu_port;                   /* C148 */
    uint32 hdrm_EQEs_pkts_for_cpu_port;                      /* C149 */

    uint32 total_hdrm_int_buff_cells;                        /* C150 */
    uint32 total_hdrm_int_buff_ema_pool_cells;               /* C151 */
    uint32 total_hdrm_ext_buff_cells;                        /* C152 */
    uint32 total_hdrm_RE_WQEs_pkts;                          /* C153 */
    uint32 total_hdrm_EQEs_pkts;                             /* C154 */

    uint32 min_int_buff_cells_per_PG;                        /* C156 */
    uint32 min_int_buff_ema_pool_cells_per_PG;               /* C157 */
    uint32 min_ext_buff_cells_per_PG;                        /* C158 */
    uint32 min_RE_WQEs_pkt_per_PG;                           /* C159 */
    uint32 min_EQEs_pkt_per_PG;                              /* C160 */

    uint32 min_int_buff_cells_for_total_PG;                  /* C162 */
    uint32 min_int_buff_ema_pool_cells_for_total_PG;         /* C163 */
    uint32 min_ext_buff_cells_for_total_PG;                  /* C164 */
    uint32 min_RE_WQEs_pkts_for_total_PG;                    /* C165 */
    uint32 min_EQEs_pkts_for_total_PG;                       /* C166 */

    uint32 min_int_buff_cells_for_a_port;                    /* C168 */
    uint32 min_int_buff_ema_pool_cells_for_a_port;           /* C169 */
    uint32 min_ext_buff_cells_for_a_port;                    /* C170 */
    uint32 min_RE_WQEs_pkts_for_a_port;                      /* C171 */
    uint32 min_EQEs_pkts_for_a_port;                         /* C172 */

    uint32 min_int_buff_cells_for_total_port;                /* C174 */
    uint32 min_int_buff_ema_pool_cells_for_total_port;       /* C175 */
    uint32 min_ext_buff_cells_for_total_port;                /* C176 */
    uint32 min_RE_WQEs_pkts_for_total_port;                  /* C177 */
    uint32 min_EQEs_pkts_for_total_port;                     /* C178 */

    uint32 total_min_int_buff_cells;                         /* C180 */
    uint32 total_min_int_buff_ema_pool_cells;                /* C181 */
    uint32 total_min_ext_buff_cells;                         /* C182 */
    uint32 total_min_RE_WQEs_pkts;                           /* C183 */
    uint32 total_min_EQEs_pkts;                              /* C184 */

    uint32 total_shared_ing_buff_pool;                       /* C186 */
    uint32 total_shared_EMA_buff;                            /* C187 */
    uint32 total_shared_ext_buff;                            /* C188 */
    uint32 total_shared_RE_WQEs_buff;                        /* C189 */
    uint32 total_shared_EQEs_buff;                           /* C190 */

    uint32 ingress_burst_cells_size_for_one_port;            /* C192 */
    uint32 ingress_burst_pkts_size_for_one_port;             /* C193 */
    uint32 ingress_burst_cells_size_for_all_ports;           /* C194 */
    uint32 ingress_total_shared_cells_use_for_all_port;      /* C195 */
    uint32 ingress_burst_pkts_size_for_all_port;             /* C196 */
    uint32 ingress_total_shared_pkts_use_for_all_port;       /* C197 */
    uint32 ingress_total_shared_hdrm_cells_use_for_all_port; /* C198 */
    uint32 ingress_total_shared_hdrm_pkts_use_for_all_port;  /* C199 */

}_sb2_input_port_threshold_t;

typedef struct _sb2_output_port_threshold_s {
    uint32 min_grntd_res_queue_cells_int_buff;               /* C204 */
    uint32 min_grntd_res_queue_cells_ext_buff;               /* C205 */
    uint32 min_grntd_res_queue_cells_EQEs;                   /* C206 */
    uint32 min_grntd_res_EMA_queue_cells;                    /* C207 */
    uint32 min_grntd_res_RE_WQs_cells;                       /* C208 */
    uint32 min_grntd_res_RE_WQs_queue_cells_for_int_buff;    /* C209 */
    uint32 min_grntd_res_RE_WQs_queue_cells_for_ext_buff;    /* C210 */
    uint32 min_grntd_res_EP_redirect_queue_entry_cells;      /* C211 */

    uint32 min_grntd_tot_res_queue_cells_int_buff;           /* C213 */
    uint32 min_grntd_tot_res_queue_cells_ext_buff;           /* C214 */
    uint32 min_grntd_tot_res_queue_cells_EQEs;               /* C215 */
    uint32 min_grntd_tot_res_EMA_queue_cells;                /* C216 */
    uint32 min_grntd_tot_res_RE_WQs_cells;                   /* C217 */
    uint32 min_grntd_tot_res_RE_WQs_queue_cells_for_int_buff;/* C218 */
    uint32 min_grntd_tot_res_RE_WQs_queue_cells_for_ext_buff;/* C219 */
    uint32 min_grntd_tot_res_EP_redirect_queue_entry_cells;  /* C220 */

    uint32 min_grntd_tot_shr_queue_cells_int_buff;           /* C222 */
    uint32 min_grntd_tot_shr_queue_cells_ext_buff;           /* C223 */
    uint32 min_grntd_tot_shr_queue_cells_EQEs;               /* C224 */
    uint32 min_grntd_tot_shr_EMA_queue_cells;                /* C225 */
    uint32 min_grntd_tot_shr_RE_WQs_cells;                   /* C226 */
    uint32 min_grntd_tot_shr_RE_WQs_queue_cells_for_int_buff;/* C227 */
    uint32 min_grntd_tot_shr_RE_WQs_queue_cells_for_ext_buff;/* C228 */
    uint32 min_grntd_tot_shr_EP_redirect_queue_entry_cells;  /* C229 */

    uint32 egress_queue_dynamic_threshold_parameter;         /* C230 */
    uint32 egress_burst_cells_size_for_one_queue;            /* C231 */
    uint32 egress_burst_pkts_size_for_one_queue;             /* C232 */
    uint32 egress_burst_cells_size_for_all_ports;            /* C233 */
    uint32 egress_burst_pkts_size_for_all_ports;             /* C234 */
    uint32 egress_burst_cells_size_for_all_queues;           /* C235 */
    uint32 egress_burst_pkts_size_for_all_queues;            /* C236 */
    uint32 egress_total_use_in_cells_for_all_queues;         /* C237 */
    uint32 egress_total_use_in_pkts_for_all_queues;          /* C238 */
    uint32 egress_remaining_cells_for_all_queues;            /* C239 */
    uint32 egress_remaining_pkts_for_all_queues;             /* C240 */

}_sb2_output_port_threshold_t;

typedef struct _soc_sb2_mmu_intermediate_results_s {
    _sb2_general_info_t           general_info;
    _sb2_input_port_threshold_t   input_port_threshold;
    _sb2_output_port_threshold_t  output_port_threshold;
}_soc_sb2_mmu_intermediate_results_t;
extern _soc_sb2_mmu_intermediate_results_t _soc_sb2_mmu_intermediate_results;

extern int soc_sb2_linkphy_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index);
extern int soc_sb2_linkphy_port_blk_idx_get(
    int unit, int port, int *block, int *index);
extern int soc_sb2_linkphy_get_portid(int unit, int block, int index);

extern void _saber2_phy_addr_default(int unit, int port,
    uint16 *phy_addr, uint16 *phy_addr_int);
extern soc_error_t soc_saber2_get_mxq_phy_port_mode(
        int unit, soc_port_t port,int speed, bcmMxqPhyPortMode_t *mode);
extern soc_error_t soc_saber2_get_xl_phy_core_port_mode(
        int unit, soc_port_t port, bcmXlPhyPortMode_t *phy_mode, 
        bcmXlCorePortMode_t *core_mode);

extern int soc_sb2_iecell_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index);
extern int soc_sb2_iecell_port_blk_idx_get(
    int unit, int port, int *block, int *index);
extern int soc_sb2_iecell_get_portid(int unit, int block, int index);
extern soc_error_t soc_saber2_port_enable_set(
       int unit, soc_port_t port, int enable);
extern soc_error_t soc_saber2_get_port_block(
       int unit, soc_port_t port,uint8 *block);

extern soc_functions_t soc_saber2_drv_funs;
extern int
soc_saber2_num_cosq_init(int unit);
extern int soc_sb2_dump_default_mmu(int unit); 
extern int soc_sb2_mem_config(int unit, int dev_id);
extern int _soc_saber2_mmu_reconfigure(int unit, int port);

extern void soc_saber2_pbmp_init(int unit, sb2_pbmp_t sb2_pbmp);
extern void soc_saber2_subport_init(int unit);
extern void soc_saber2_block_reset(int unit, uint8 block,int active_low);
extern void soc_saber2_save_tdm_pos(int unit, uint32 new_tdm_size,uint32 *new_tdm);
extern soc_error_t soc_saber2_reconfigure_tdm(int unit,uint32 new_tdm_size,uint32 *new_tdm);

extern int _soc_saber2_flexio_scache_allocate(int unit);
extern int _soc_saber2_flexio_scache_retrieve(int unit);
extern int _soc_saber2_flexio_scache_sync(int unit);

extern int soc_sb2_temperature_monitor_get(int unit,
            int temperature_max,
            soc_switch_temperature_monitor_t *temperature_array,
            int *temperature_count);
extern int soc_sb2_show_voltage(int unit);
extern int _soc_saber2_hw_reset_control_init(int unit);

#define SB2_TDM_MAX_SIZE           284 

#define SB2_MAX_BLOCKS             7
#define SB2_MAX_PORTS_PER_BLOCK    4

#define SB2_MAX_LOGICAL_PORTS           30 
#define SB2_MAX_PHYSICAL_PORTS          28 

#define SB2_LPBK                        29 
#define SB2_CMIC                        0
#define SB2_IDLE                        SB2_MAX_LOGICAL_PORTS

#define SB2_PORT_MAC_MAX       96

#define SB2_MAX_SERVICE_POOLS           4
#define SB2_MAX_PRIORITY_GROUPS         8


#define SOC_SB2_MIN_SUBPORT_INDEX             30
#define SOC_SB2_MAX_SUBPORTS                  64
#define SOC_SB2_MAX_LINKPHY_SUBPORTS          32
#define SOC_SB2_SUBPORT_GROUP_MAX             SOC_SB2_MAX_SUBPORTS
#define SOC_SB2_SUBPORT_PP_PORT_INDEX_MIN     SOC_SB2_MIN_SUBPORT_INDEX
#define SOC_SB2_SUBPORT_PP_PORT_INDEX_MAX     (\
                SOC_SB2_SUBPORT_PP_PORT_INDEX_MIN + SOC_SB2_MAX_SUBPORTS - 1)

#define SOC_SB2_LINKPHY_TX_DATA_BUF_START_ADDR_MAX  3392
#define SOC_SB2_LINKPHY_TX_DATA_BUF_END_ADDR_MIN    31
#define SOC_SB2_LINKPHY_TX_DATA_BUF_END_ADDR_MAX    3423
#define SOC_SB2_LINKPHY_TX_STREAM_START_ADDR_OFFSET 0x80

/* Max 32 streams per linkphy port.
 * So we can support max of 16 subports per linkphy port.
 */
#define SOC_SB2_MAX_LINKPHY_SUBPORTS_PER_PORT     32
#define SOC_SB2_MAX_LINKPHY_STREAMS_PER_PORT      32
#define SOC_SB2_MAX_STREAMS_PER_SLICE             32
#define SOC_SB2_MAX_STREAMS_PER_SUBPORT           BCM_SUBPORT_CONFIG_MAX_STREAMS

#define SOC_SB2_LINKPHY_BLOCK_MAX                 1
#define SOC_SB2_MAX_LINKPHY_PORTS                 4

/* Configs for BCM56460,461,465, 466 */
#define BCM56460_DEVICE_ID_OFFSET_CFG 0
#define BCM56460_DEVICE_ID_DEFAULT_CFG 1
#define BCM56460_DEVICE_ID_MAX_CFG 6

/* Configs for BCM56462,467 */
#define BCM56462_DEVICE_ID_OFFSET_CFG 6
#define BCM56462_DEVICE_ID_DEFAULT_CFG 7
#define BCM56462_DEVICE_ID_MAX_CFG 8

/* Configs for BCM56463,468 */
#define BCM56463_DEVICE_ID_OFFSET_CFG 8
#define BCM56463_DEVICE_ID_DEFAULT_CFG 9
#define BCM56463_DEVICE_ID_MAX_CFG 11

/* Configs for BCM56260,261,265,266 */
#define BCM56260_DEVICE_ID_OFFSET_CFG 11
#define BCM56260_DEVICE_ID_DEFAULT_CFG 12
#define BCM56260_DEVICE_ID_MAX_CFG 13

/* Configs for BCM56262,267 */
#define BCM56262_DEVICE_ID_OFFSET_CFG 13
#define BCM56262_DEVICE_ID_DEFAULT_CFG 14
#define BCM56262_DEVICE_ID_MAX_CFG 14

/* Configs for BCM56263,268 */
#define BCM56263_DEVICE_ID_OFFSET_CFG 14
#define BCM56263_DEVICE_ID_DEFAULT_CFG 15
#define BCM56263_DEVICE_ID_MAX_CFG 16

/* Configs for BCM56233 */
#define BCM56233_DEVICE_ID_OFFSET_CFG 16
#define BCM56233_DEVICE_ID_DEFAULT_CFG 17
#define BCM56233_DEVICE_ID_MAX_CFG 17

/* SAT port for Saber2 */
#define SOC_SB2_SAT_OAMP_PHY_PORT_NUMBER 5

/* Max CoE modules */
#define SOC_SB2_MAX_COE_MODULES 3

typedef uint32 sb2_port_speeds_t[SB2_MAX_BLOCKS][SB2_MAX_PORTS_PER_BLOCK];
typedef uint32 sb2_block_ports_t[SB2_MAX_BLOCKS][SB2_MAX_PORTS_PER_BLOCK];

typedef uint32 sb2_speed_t[SB2_MAX_PHYSICAL_PORTS];
typedef uint32 sb2_port_to_block_t[SB2_MAX_PHYSICAL_PORTS];
typedef uint32 sb2_port_to_block_subports_t[SB2_MAX_PHYSICAL_PORTS];

extern sb2_block_ports_t *sb2_block_ports[SOC_MAX_NUM_DEVICES];
extern sb2_port_speeds_t *sb2_port_speeds[SOC_MAX_NUM_DEVICES];

extern uint32 sb2_current_tdm[SB2_TDM_MAX_SIZE];
extern uint32 sb2_current_tdm_size;
extern uint8  sb2_tdm_update_flag;
extern sb2_tdm_pos_info_t sb2_tdm_pos_info[SB2_MAX_BLOCKS];

#ifdef INCLUDE_AVS
extern int soc_saber2_avs_init(int unit);
#endif /* INCLUDE_AVS */
extern int soc_saber2_and28_info_config(int unit, soc_sb2_and28_info_t *drc_info,
        and28_shmoo_dram_info_t *sdi);
extern int soc_sb2_and28_dram_init_reset(int unit);
extern int soc_sb2_and28_dram_init_config(int unit);
extern int soc_sb2_and28_dram_savecfg(int unit, and28_shmoo_config_param_t *config_param);
extern int soc_sb2_and28_dram_restorecfg(int unit, and28_shmoo_config_param_t *config_param);
extern int soc_saber2_hw_reset_control_init(int unit);
extern int soc_saber2_ser_init(int unit,int enable);
extern soc_error_t soc_saber2_mem_parity_control(int unit, soc_mem_t mem, int copyno, int enable);
extern soc_error_t soc_saber2_ser_mem_clear(int unit, soc_mem_t mem);

extern int soc_sb2_cosq_max_bucket_set(int unit, int port,
                        int index, uint32 level);
extern int
soc_sb2_cosq_min_bucket_get(int unit, int port, int index, int level,
                        uint32 *min_quantum, uint32 *mantissa,
                        uint32 *exp, uint32 *cycle);
extern int soc_sb2_cosq_min_clear (int unit, int port,
                        int index, uint32 level);

extern int
soc_katana_compute_shaper_rate(int unit, uint32 rate_mantissa,
                               uint32 rate_exponent, uint32 *rate);

extern int
_soc_ddr_sb2_phy_freqtoloc(uint32 freq);
extern int soc_saber2_xl_port_speed_get(int unit, int port, int *speed);
int
soc_sb2_mmu_config_shared_buf_recalc(int unit, int delta_size, bcm_kt2_cosq_recalc_type_t flags);
extern void soc_sb2_xport_type_update(int unit, soc_port_t port, int mode);

#define BCM_SB2_COSQ_FLEX_COUNT_SUPPORTED 3
#define BCM_SB2_COSQ_FLEX_COUNT_REGS 6
#define BCM_SB2_COSQ_WRED_DROP_MASK    0x037f
#define BCM_SB2_COSQ_TOTAL_DROP_MASK   0x0000 
#define BCM_SB2_COSQ_DEFAULT_DROP_MASK 0x03ff 
#define BCM_SB2_COSQ_CHECK_STAT_TYPE(drop_type,type) (drop_type ^ type) 

extern void soc_sb2_cosq_counter_mem_map_get(int unit, int *num_elem, _sb2_cosq_counter_mem_map_t **mem_map);
extern int soc_saber2_perq_flex_counters_init(int unit, uint32 drop_mask);

#define SABER2_5626X_NGROUPS_FP    32
#define SABER2_5626X_NPORTS_FP     4
#define SABER2_5626X_NGROUPS_HG    4
#define SABER2_5626X_NPORTS_HG     4

extern int soc_sb2_5626x_devtype (int unit);
int
_soc_saber2_mdio_reg_read(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 *phy_data);
int
_soc_saber2_tsce_firmware_set(int unit, int port, uint8 *array, int datalen);
int
_soc_saber2_mdio_reg_write(int unit, uint32 phy_addr,
                              uint32 phy_reg, uint32 phy_data);
int
soc_saber2_port_is_unused(int unit, int port_no);
extern int
soc_sb2_get_max_buffer_size(int unit, int external, int value);
extern void
soc_sb2_get_queue_min_size(int unit, int *internal_size, int *external_size);
#endif  /* !_SOC_SABER2_H_ */
