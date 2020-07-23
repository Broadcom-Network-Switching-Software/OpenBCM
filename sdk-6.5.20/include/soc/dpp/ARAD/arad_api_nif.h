/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_NIF_INCLUDED__

#define __ARAD_API_NIF_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/ARAD/arad_sw_db_tcam_mgmt.h>




#define ARAD_NIF_ID_XAUI_FIRST    ARAD_NIF_ID_XAUI_0
#define ARAD_NIF_ID_XAUI_LAST     ARAD_NIF_ID_XAUI_7

#define ARAD_NIF_ID_RXAUI_FIRST   ARAD_NIF_ID_RXAUI_0
#define ARAD_NIF_ID_RXAUI_LAST    ARAD_NIF_ID_RXAUI_15

#define ARAD_NIF_ID_SGMII_FIRST   ARAD_NIF_ID_SGMII_0
#define ARAD_NIF_ID_SGMII_LAST    ARAD_NIF_ID_SGMII_31


#define ARAD_NIF_ID_ILKN_FIRST    ARAD_NIF_ID_ILKN_0
#define ARAD_NIF_ID_ILKN_LAST     ARAD_NIF_ID_ILKN_1

#define ARAD_NIF_ID_ILKN_TDM_FIRST  ARAD_NIF_ID_ILKN_TDM_0
#define ARAD_NIF_ID_ILKN_TDM_LAST  ARAD_NIF_ID_ILKN_TDM_1

#define ARAD_NIF_ID_CGE_FIRST    ARAD_NIF_ID_CGE_0
#define ARAD_NIF_ID_CGE_LAST     ARAD_NIF_ID_CGE_1

#define ARAD_NIF_ID_XLGE_FIRST    ARAD_NIF_ID_XLGE_0
#define ARAD_NIF_ID_XLGE_LAST     ARAD_NIF_ID_XLGE_7

#define ARAD_NIF_ID_TM_INTERNAL_PKT_FIRST    ARAD_IF_ID_TM_INTERNAL_PKT
#define ARAD_NIF_ID_TM_INTERNAL_PKT_LAST     ARAD_IF_ID_TM_INTERNAL_PKT

#define ARAD_NIF_ID_CPU_FIRST    ARAD_IF_ID_CPU
#define ARAD_NIF_ID_CPU_LAST     ARAD_IF_ID_CPU

#define ARAD_NIF_ID_10GBASE_R_FIRST    ARAD_NIF_ID_10GBASE_R_0
#define ARAD_NIF_ID_10GBASE_R_LAST     ARAD_NIF_ID_10GBASE_R_31


#define  ARAD_NIF_NOF_TYPES (5)

#define ARAD_NIF_NOF_NIFS               32

#define ARAD_NIF_NOF_NIFS_MAX           ARAD_NIF_NOF_NIFS
#define ARAD_MAX_NIF_ID                 (ARAD_NIF_NOF_NIFS_MAX-1)

#define ARAD_NIF_ID_MAX                 ARAD_MAX_NIF_ID

#define ARAD_NIF_INVALID_VAL_INTERN ARAD_NIF_ID_NONE


#define ARAD_NOF_WRAP_CORE              8

#define ARAD_MAX_NIFS_PER_WC            4
#define ARAD_MAX_RXAUIS_PER_WC          2
#define ARAD_NIF2WC_GLBL_ID(nif_id)      ((nif_id)/ARAD_MAX_NIFS_PER_WC)
#define ARAD_NIF_WC2NIF_BASE_ID(wc_id)  ((wc_id)*ARAD_MAX_NIFS_PER_WC)

#define ARAD_SGMII_NIFS_PER_WC          ARAD_MAX_NIFS_PER_WC
#define ARAD_RXAUI_NIFS_PER_WC          (ARAD_MAX_NIFS_PER_WC/2)

#define ARAD_ARAD_STYLE_NOF_IF_CHANNELS_MAX (256)

#define ARAD_NOF_IF_CHANNELS_MAX (ARAD_ARAD_STYLE_NOF_IF_CHANNELS_MAX)

#define ARAD_NIF_IS_TYPE_GMII(nif_type) \
  SOC_SAND_NUM2BOOL((((nif_type) == ARAD_NIF_TYPE_SGMII)))

#define ARAD_NIF_IS_TYPE_XAUI_LIKE(nif_type) \
  SOC_SAND_NUM2BOOL(((nif_type) == ARAD_NIF_TYPE_XAUI) || ((nif_type) == ARAD_NIF_TYPE_RXAUI))

#define ARAD_NIF_IS_TYPE_10GBASE_R(nif_type) \
  SOC_SAND_NUM2BOOL((((nif_type) == ARAD_NIF_TYPE_10GBASE_R)))

#define ARAD_NIF_IS_TYPE_CGE(nif_type) \
  SOC_SAND_NUM2BOOL((((nif_type) == ARAD_NIF_TYPE_100G_CGE)))
#define ARAD_NIF_IS_TYPE_XLGE(nif_type) \
  SOC_SAND_NUM2BOOL((((nif_type) == ARAD_NIF_TYPE_40G_XLGE)))


#define ARAD_NIF_NOF_SYNCE_CLK_IDS (2)





#define ARAD_NIF_ID(tp,id) \
  (ARAD_NIF_ID_##tp##_FIRST + (id))


#define ARAD_NIF_IS_TYPE_ID(tp,id) \
  SOC_SAND_NUM2BOOL(SOC_SAND_IS_VAL_IN_RANGE(id, ARAD_NIF_ID_##tp##_FIRST, ARAD_NIF_ID_##tp##_LAST))


#define ARAD_NIF_ID_OFFSET(tp,id) \
  SOC_SAND_DELTA((int)(id), (int)ARAD_NIF_ID_##tp##_FIRST)


#define ARAD_NIF_TYPE_MAX(tp) \
  SOC_SAND_RNG_COUNT(ARAD_NIF_ID_##tp##_FIRST, ARAD_NIF_ID_##tp##_LAST)


#define ARAD_NIF_IS_ARAD_ID(id)\
  SOC_SAND_NUM2BOOL(ARAD_NIF_IS_TYPE_ID(XAUI,id)  || ARAD_NIF_IS_TYPE_ID(RXAUI,id)  || \
      ARAD_NIF_IS_TYPE_ID(SGMII,id) || \
      ARAD_NIF_IS_TYPE_ID(XLGE,id) || ARAD_NIF_IS_TYPE_ID(CGE,id) || \
      ARAD_NIF_IS_TYPE_ID(ILKN,id)|| ARAD_NIF_IS_TYPE_ID(10GBASE_R,id) || ARAD_NIF_IS_TYPE_ID(TM_INTERNAL_PKT,id))

#define ARAD_NIF_IS_VALID_ID(id) \
  SOC_SAND_NUM2BOOL(ARAD_NIF_IS_ARAD_ID(id))





#define ARAD_NIF_BASE_MAL(mal_id)         ((mal_id) & ~0x1)

#define ARAD_NIF_IS_BASE_MAL(mal_id)      SOC_SAND_NUM2BOOL((mal_id) == ARAD_NIF_BASE_MAL(mal_id))

#define ARAD_NIF_SGMII_LANE(if_id) \
  ((if_id) % ARAD_MAX_NIFS_PER_WC)

#define ARAD_NIF_RXAUI_LANE(if_id) \
  (((if_id) % ARAD_MAX_RXAUIS_PER_WC)*2)

typedef enum
{
  
  ARAD_NIF_MODE_NONE = 0,
  
  ARAD_NIF_MODE_XAUI_STANDARD,
  
  ARAD_NIF_MODE_XAUI_CX4,
  
  ARAD_NIF_MODE_XAUI_KX4,
  
  ARAD_NIF_MODE_XAUI_PLUS_16G,
  
  ARAD_NIF_MODE_XAUI_PLUS_20G,
  
  ARAD_NIF_MODE_XAUI_PLUS_PLUS_25G,
  
  ARAD_NIF_MODE_XAUI_PLUS_PLUS_40G,
  
  ARAD_NIF_MODE_SGMII_100M,
  
  ARAD_NIF_MODE_SGMII_1G,
  
  ARAD_NIF_MODE_SGMII_1000_BASEX,
  
  ARAD_NIF_MODE_SGMII_2_5G,
  
  ARAD_NIF_MODE_RXAUI_10G_STANDARD,
  
  ARAD_NIF_MODE_RXAUI_11G,
  
  ARAD_NIF_MODE_RXAUI_16G,
  
  ARAD_NIF_MODE_RXAUI_18G,
  
  ARAD_NIF_MODE_RXAUI_20G,

    
  ARAD_NIF_MODE_ILKN_FIRST,
  
  ARAD_NIF_MODE_ILKN_SERDES_6_25 = ARAD_NIF_MODE_ILKN_FIRST,
  
  ARAD_NIF_MODE_ILKN_SERDES_10_3,
  
  ARAD_NIF_MODE_ILKN_SERDES_10_9,
  
  ARAD_NIF_MODE_ILKN_SERDES_11_5,
  
  ARAD_NIF_MODE_ILKN_SERDES_12_5,
    
  ARAD_NIF_MODE_ILKN_LAST = ARAD_NIF_MODE_ILKN_SERDES_12_5,


    
    ARAD_NIF_MODE_10GBASE_R_XFI_10G,
    
    ARAD_NIF_MODE_10GBASE_R_SFI_10G,
    
    ARAD_NIF_MODE_10GBASE_R_KR_10G,
    
    ARAD_NIF_MODE_10GBASE_R_XFI_11G,
    
    ARAD_NIF_MODE_10GBASE_R_SFI_11G,
    
    ARAD_NIF_MODE_10GBASE_R_KR_11G,
    
    ARAD_NIF_MODE_10GBASE_R_XFI_12G,
    
    ARAD_NIF_MODE_10GBASE_R_SFI_12G,
    
    ARAD_NIF_MODE_10GBASE_R_KR_12G,
    
    ARAD_NIF_MODE_XLGE_XLAUI_40G,
    
    ARAD_NIF_MODE_XLGE_CR4_40G,
    
    ARAD_NIF_MODE_XLGE_KR4_40G,
    
    ARAD_NIF_MODE_XLGE_XLPPI_40G,
    
    ARAD_NIF_MODE_XLGE_XLPPI_42G,
    
    ARAD_NIF_MODE_XLGE_XLAUI_45G,
    
    ARAD_NIF_MODE_XLGE_CR4_45G,
    
    ARAD_NIF_MODE_XLGE_KR4_45G,
    
    ARAD_NIF_MODE_XLGE_XLPPI_45G,
    
    ARAD_NIF_MODE_XLGE_XLAUI_48G,
    
    ARAD_NIF_MODE_XLGE_CR4_48G,
    
    ARAD_NIF_MODE_XLGE_KR4_48G,
    
    ARAD_NIF_MODE_XLGE_XLPPI_48G,


        
    ARAD_NIF_MODE_CGE_FIRST,
    
    ARAD_NIF_MODE_CGE_CAUI_SERDES_10_3 = ARAD_NIF_MODE_CGE_FIRST,
    
    ARAD_NIF_MODE_CGE_KR10_SERDES_10_3,
    
    ARAD_NIF_MODE_CGE_CR10_SERDES_10_3,
    
    ARAD_NIF_MODE_CGE_CAUI_SERDES_10_9,
    
    ARAD_NIF_MODE_CGE_KR10_SERDES_10_9,
    
    ARAD_NIF_MODE_CGE_CR10_SERDES_10_9,
    
    ARAD_NIF_MODE_CGE_CAUI_SERDES_11_5,
    
    ARAD_NIF_MODE_CGE_KR10_SERDES_11_5,
    
    ARAD_NIF_MODE_CGE_CR10_SERDES_11_5,
    
    ARAD_NIF_MODE_CGE_CAUI_SERDES_12_5,
    
    ARAD_NIF_MODE_CGE_KR10_SERDES_12_5,
    
    ARAD_NIF_MODE_CGE_CR10_SERDES_12_5,
        
    ARAD_NIF_MODE_CGE_LAST = ARAD_NIF_MODE_CGE_CR10_SERDES_12_5,

    ARAD_NIF_NOF_MODES

}ARAD_NIF_MODE;


typedef enum
{
  
  ARAD_NIF_TYPE_NONE = 0,
  
  ARAD_NIF_TYPE_XAUI,
  
   ARAD_NIF_TYPE_SGMII,
  
  ARAD_NIF_TYPE_RXAUI,
  
  ARAD_NIF_TYPE_ILKN,
    
    ARAD_NIF_TYPE_10GBASE_R,
    
    ARAD_NIF_TYPE_40G_XLGE,
    
    ARAD_NIF_TYPE_100G_CGE,
    
    ARAD_NIF_TYPE_TM_INTERNAL_PKT,
    
    ARAD_NIF_TYPE_CPU

}ARAD_NIF_TYPE;

typedef enum
{
  
  ARAD_NIF_ILKN_ID_A = ARAD_NIF_ID_ILKN_0,
  
  ARAD_NIF_ILKN_ID_B = ARAD_NIF_ID_ILKN_1,
  
  ARAD_NIF_NOF_ILKN_IDS = 2
}ARAD_NIF_ILKN_ID;

typedef enum
{
  
  ARAD_NIF_CGE_ID_A = ARAD_NIF_ID_CGE_0,
    
  ARAD_NIF_CGE_ID_B = ARAD_NIF_ID_CGE_1,
  
  ARAD_NIF_NOF_CGE_IDS = 2
}ARAD_NIF_CGE_ID;

typedef enum
{
  
  ARAD_NIF_RX_BCAST_PACKETS,
  
  ARAD_NIF_RX_MCAST_BURSTS,

  
  ARAD_NIF_RX_ERR_PACKETS,

  

  
  ARAD_NIF_RX_LEN_BELOW_MIN,
  
  ARAD_NIF_RX_LEN_MIN_59,
  
  ARAD_NIF_RX_LEN_60,
  
  ARAD_NIF_RX_LEN_61_123,
  
  ARAD_NIF_RX_LEN_124_251,
  
  ARAD_NIF_RX_LEN_252_507,
  
  ARAD_NIF_RX_LEN_508_1019,
  
  ARAD_NIF_RX_LEN_1020_1514,
  
  ARAD_NIF_RX_LEN_1515_1518,
  
  ARAD_NIF_RX_LEN_1519_2043,
  
  ARAD_NIF_RX_LEN_2044_4091,
  
  ARAD_NIF_RX_LEN_4092_9212,
  
  ARAD_NIF_RX_LEN_9213CFG_MAX,

  
  ARAD_NIF_RX_LEN_1515CFG_MAX,

  
  ARAD_NIF_RX_LEN_ABOVE_MAX,


  

  
  ARAD_NIF_TX_LEN_BELOW_MIN,
  
  ARAD_NIF_TX_LEN_MIN_59,
  
  ARAD_NIF_TX_LEN_60,
  
  ARAD_NIF_TX_LEN_61_123,
  
  ARAD_NIF_TX_LEN_124_251,
  
  ARAD_NIF_TX_LEN_252_507,
  
  ARAD_NIF_TX_LEN_508_1019,
  
  ARAD_NIF_TX_LEN_1020_1514,
  
  ARAD_NIF_TX_LEN_1515_1518,
  
  ARAD_NIF_TX_LEN_1519_2043,
  
  ARAD_NIF_TX_LEN_2044_4091,
  
  ARAD_NIF_TX_LEN_4092_9212,
  
  ARAD_NIF_TX_LEN_9213CFG_MAX,
  
  ARAD_NIF_TX_BCAST_PACKETS,
  
  ARAD_NIF_TX_MCAST_BURSTS,

  
  ARAD_NIF_TX_ERR_PACKETS,
  
  ARAD_NIF_RX_OK_OCTETS,
  
  ARAD_NIF_TX_OK_OCTETS,
  
  ARAD_NIF_RX_OK_PACKETS,
  
  ARAD_NIF_TX_OK_PACKETS,
  
  ARAD_NIF_RX_NON_UNICAST_PACKETS,
  
  ARAD_NIF_TX_NON_UNICAST_PACKETS,

   
  ARAD_NIF_RX_ILKN_PER_CHANNEL,
  
  ARAD_NIF_TX_ILKN_PER_CHANNEL,


  
  ARAD_NIF_NOF_COUNTER_TYPES
}ARAD_NIF_COUNTER_TYPE;


typedef enum
{
   
  ARAD_NIF_SYNCE_MODE_TWO_DIFF_CLK = 0,
  
  ARAD_NIF_SYNCE_MODE_TWO_CLK_AND_VALID = 1,
  
  ARAD_NIF_NOF_SYNCE_MODES = 2
}ARAD_NIF_SYNCE_MODE;

#if defined(INCLUDE_KBP)

typedef enum
{
   
  ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE = 0,
  
  ARAD_NIF_ELK_TCAM_DEV_TYPE_NL88650A = 1,
  
  ARAD_NIF_ELK_TCAM_DEV_TYPE_NL88650B = 2,

  ARAD_NIF_ELK_TCAM_DEV_TYPE_NL88675 = 3,

  
   ARAD_NIF_ELK_TCAM_DEV_TYPE_BCM52311 = 4,
  
   ARAD_NIF_ELK_TCAM_DEV_TYPE_BCM52321 = 5,
   
  ARAD_NIF_ELK_NOF_TCAM_DEV_TYPE = 5
}ARAD_NIF_ELK_TCAM_DEV_TYPE;

typedef enum
{
   
  ARAD_NIF_ELK_TCAM_CONNECT_MODE_SINGLE = 0,
  
  ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED = 1,
  
  ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT = 2,
   
  ARAD_NIF_ELK_TCAM_NOF_CONNECT_MODE = 3
} ARAD_NIF_ELK_TCAM_CONNECT_MODE;

#endif


typedef enum
{
    ARAD_KBP_LUT_MDIO_WRITE = 0,
    ARAD_KBP_LUT_ROP_WRITE
} ARAD_NIF_ELK_TCAM_LUT_WRITE_MODE;


typedef struct
{
    int device;
    uint32 kbp_mdio_id;
} ARAD_NIF_ELK_USER_DATA;

typedef enum
{
  
  ARAD_NIF_SYNCE_CLK_DIV_1 = 0,
  
  ARAD_NIF_SYNCE_CLK_DIV_2 = 1,
  
  ARAD_NIF_SYNCE_CLK_DIV_4 = 2,
  
  ARAD_NIF_NOF_SYNCE_CLK_DIVS = 3
}ARAD_NIF_SYNCE_CLK_DIV;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint32 port_id;
  
  ARAD_NIF_SYNCE_CLK_DIV clk_divider;
  
  uint8 squelch_enable;

} ARAD_NIF_SYNCE_CLK;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  ARAD_NIF_SYNCE_MODE mode;
  
  ARAD_NIF_SYNCE_CLK conf[ARAD_NIF_NOF_SYNCE_CLK_IDS];

}ARAD_INIT_SYNCE;

#if defined(INCLUDE_KBP)
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;

  ARAD_NIF_ELK_USER_DATA kbp_user_data[SOC_DPP_DEFS_MAX(NOF_CORES)];

#ifdef BCM_88660_A0
  
  uint8 ext_interface_mode;
  
  uint8 msb_trigger_on[2];
	
  uint32 last_opcode_set[2];

  int kbp_rop_enable_dma;
  int kbp_rop_dma_trans_len;
  int kbp_rop_dma_total_trans_count;
  int kbp_rop_dma_msg_count;

  uint8 *kbp_rop_dma_buffer[2];
  uint32 kbp_rop_dma_buf_idx_in_use;

  uint32 kbp_rop_dma_gathered_trans_count;
  uint8 *kbp_rop_dma_buffer_curr;

  uint32 kbp_rop_dma_ack_pending;
  uint32 kbp_rop_dma_trans_sent_in_lst_msg;

#endif
  
  ARAD_NIF_ELK_TCAM_DEV_TYPE  tcam_dev_type;
  
  uint32  fwd_table_size[ARAD_KBP_FRWRD_IP_NOF_TABLES];

  uint32 kbp_recover_enable;

  uint32 kbp_recover_iter;

  uint32 kbp_no_fwd_ipv6_dip_sip_sharing_disable;

  uint32 kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header;

  ARAD_NIF_ELK_TCAM_CONNECT_MODE kbp_connect_mode;

  uint32 kbp_start_lane[4];
  
  uint8  is_master;

  uint8 kbp_24bit_fwd;

  uint8 kbp_ipv6_mc_invert_group_source;
  
  uint8 kbp_port_core_mapping[4];
  
  ARAD_NIF_ELK_TCAM_LUT_WRITE_MODE kbp_lut_write_mode;
  
  uint8 kbp_serdes_pcie_init;
}ARAD_INIT_ELK;
#endif

#define ARAD_NIF_NOF_COUNTERS   ARAD_NIF_NOF_COUNTER_TYPES


uint32
  arad_nif_counter_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t                  port,
    SOC_SAND_IN  ARAD_NIF_COUNTER_TYPE       counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT              *counter_val
  );


uint32
  arad_nif_all_counters_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_IN  uint8                  non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT         counter_val[ARAD_NIF_NOF_COUNTERS]
  );


uint32
  arad_nif_all_nifs_all_counters_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                     counters_val[ARAD_NIF_NOF_NIFS][ARAD_NIF_NOF_COUNTERS]
  );


uint32 arad_nif_synce_clk_sel_port_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  soc_port_t                  port);

uint32 arad_nif_synce_clk_sel_port_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_OUT soc_port_t                  *port);

uint32 arad_nif_synce_clk_squelch_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   synce_cfg_num,
    SOC_SAND_IN  int                   enable);

uint32 arad_nif_synce_clk_squelch_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   synce_cfg_num,
    SOC_SAND_OUT int                   *enable);


ARAD_NIF_TYPE
  arad_nif_id2type(
    SOC_SAND_IN  ARAD_INTERFACE_ID  arad_nif_id
  );


ARAD_INTERFACE_ID
  arad_nif_type2id(
    SOC_SAND_IN ARAD_NIF_TYPE arad_nif_type,
    SOC_SAND_IN uint32 internal_id
  );


ARAD_INTERFACE_ID
  arad_nif_offset2nif_id(
    SOC_SAND_IN  ARAD_NIF_TYPE       nif_type,
    SOC_SAND_IN  uint32         nif_offset
  );


#if ARAD_DEBUG_IS_LVL1
const char*
  ARAD_NIF_TYPE_to_string(
    SOC_SAND_IN  ARAD_NIF_TYPE enum_val
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


