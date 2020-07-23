/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_EG_ENCAP_ACCESS_INCLUDED__

#define __ARAD_PP_EG_ENCAP_ACCESS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/drv.h>





#define ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE                          ( 3)


#define ARAD_PP_EG_ENCAP_ACCESS_1ST_ENTRY_START_LOCATION (0)
#define ARAD_PP_EG_ENCAP_ACCESS_2ND_ENTRY_START_LOCATION (37)


#define ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit)                     (SOC_DPP_DEFS_GET(unit, eg_encap_access_prefix_lsb))
#define ARAD_PP_EG_ENCAP_ACCESS_PREFIX_MSB(unit)                     (SOC_DPP_DEFS_GET(unit, eg_encap_access_prefix_msb))
#define ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit)                SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_MSB(unit))



#define ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_LSB                     (0)
#define ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_MSB                     (1)
#define ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_NOF_BITS                SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_LSB, ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_MSB)



#define ARAD_PP_EG_ENCAP_ACCESS_MPLS_ENTRY_PREFIX_MASK         (0x8) 


#define ARAD_PP_EG_ENCAP_ACCESS_IP_ENTRY_PREFIX_MASK         (0xf) 


#define ARAD_PP_EG_ENCAP_ACCESS_LL_ENTRY_PREFIX_MASK          (0xe) 


#define ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_PREFIX_MASK          (0xe) 


#define ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_2_ENT_PREFIX_MASK     (0x3) 


#define ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_2_ENT_PREFIX_MASK     (0x3) 

#define ARAD_PP_EG_ENCAP_ACCESS_OTHER_ENTRY_PREFIX_MASK       (0xf) 

#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_PREFIX_MASK       (0xf) 




#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_2_ENT_PREFIX_MASK       (0x4) 





#define ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(_unit, _outlif)  \
    ((SOC_IS_JERICHO_PLUS(unit)) ? ((_outlif) - SOC_DPP_CONFIG(unit)->l3.nof_rifs) : (_outlif))

#define ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(_unit, _outlif)  \
    ((SOC_IS_JERICHO_PLUS(unit)) ? ((_outlif) + SOC_DPP_CONFIG(unit)->l3.nof_rifs) : (_outlif))


#define ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, outlif, err_e, err_num, err_exit_label) \
    SOC_SAND_ERR_IF_ABOVE_NOF(outlif, ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_DPP_DEFS_GET(unit, nof_out_lifs)), \
    err_e, err_num, err_exit_label);


#define SOC_PPC_OUTLIF_SW_STATE_INFO_GET(_unit, _lif_id, _outlif_info_ptr) \
     (sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.outlif_info.data_base. \
            get(_unit, ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(_unit, _lif_id), _outlif_info_ptr))



#define ARAD_PP_EG_ENCAP_LIF_ENTRY_TO_LOCAL_LIF(unit, bank_id, offset)  \
    (((bank_id >> 1) << (SOC_DPP_DEFS_GET(unit, nof_eg_encap_rsvs) + SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs) + 1)) +  ((bank_id & 1) <<  SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs)) + offset)


#define ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(_unit, outlif) \
    (ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(_unit, outlif) >> 1)


#define ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset) \
    (entry_offset >> (SOC_DPP_DEFS_GET(unit, nof_eg_encap_rsvs) + SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs)))


#define ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset) \
    (entry_offset & ((1 << SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs)) - 1))


#define ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_BANK_ID(unit, outlif)    \
    (outlif >> ((SOC_DPP_DEFS_GET(unit, nof_eg_encap_rsvs) + SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs)) + 1))
#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset)  \
    (entry_offset < SOC_DPP_DEFS_GET(unit, nof_eedb_lines))


#define ARAD_PP_EG_ENCAP_ACCESS_NOF_ALIGNED_EEDB_ENTRIES(unit)          ( ARAD_PP_EG_ENCAP_NOF_REGULAR_EEDB_BANKS(unit) * SOC_DPP_NOF_EEDB_LINES_PER_BANK_ARAD )
#define ARAD_PP_EG_ENCAP_ACCESS_NOF_EEDB_ENTRIES(unit)                   ( ARAD_PP_EG_ENCAP_NOF_REGULAR_EEDB_BANKS(unit) * ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_HALF_EEDB_BANK(unit) ) 
 
#define ARAD_PP_EG_ENCAP_ACCESS_NOF_TOP_BANK_ENTRIES(unit)                     ( ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_TOP_BANK(unit) )

#define ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_ID(unit, entry_offset) \
      ( ( entry_offset - ARAD_PP_EG_ENCAP_ACCESS_NOF_ALIGNED_EEDB_ENTRIES(unit) ) / ARAD_PP_EG_ENCAP_ACCESS_NOF_TOP_BANK_ENTRIES(unit))

#define ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_INTERNAL_OFFSET(unit, entry_offset) \
      ( ( entry_offset - ARAD_PP_EG_ENCAP_ACCESS_NOF_ALIGNED_EEDB_ENTRIES(unit) ) % ARAD_PP_EG_ENCAP_ACCESS_NOF_TOP_BANK_ENTRIES(unit))





typedef enum 
{
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_MPLS                   = 0x8, 
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_DATA                   = 0x6, 
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER             = 0x4, 
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_IP                     = 0x3, 
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_ISID                   = 0x2, 
  
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER                  = 0x0,  
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE                   = 0x2,  
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_TRILL                  = 0x1   

} ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE;


typedef enum 
{
  ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_TRILL            = 0x4, 
  ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_OUT_RIF          = 0x1, 
  ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_OUT_AC           = 0x0 
} ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE;


typedef enum
{
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE             = 0,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS             = 1,    
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA             = 2,  
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER       = 3,      
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP               = 4,   
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ISID             = 5,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_TRILL            = 6,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF          = 7,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC           = 8,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL      = 9, 
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP        = 10,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP         = 11,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6        = 17 , 
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_ARP_OVERLAY = 18, 
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ROO_LINK_LAYER   = 19


} ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE;

typedef enum{
  ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_val = 0,
  ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_UPPER_LIMIT_val = 7,
  ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val  = 8,
  ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val = 9,
  ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_JER_B0_UPPER_LIMIT_val = 15
} ARAD_PP_EG_ENCAP_MPLS1_COMMAND;

typedef enum
{
  
  ARAD_PP_EG_ENCAP_ACCESS_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_EG_ENCAP_ACCESS_FIRST,
  ARAD_PP_EG_ENCAP_ACCESS_GET_ERRS_PTR,
  
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_LOCAL_OUTLIF_ADDITIONAL_DATA_LOCATION_GET,
  ARAD_PP_EG_ENACP_ACCESS_ADDITIONAL_DATA_ENTRY_GET,
  ARAD_PP_EG_ENACP_ACCESS_ADDITIONAL_DATA_ENTRY_SET,
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_COMMAND_GET_UNSAFE,
  
  ARAD_PP_EG_ENCAP_ACCESS_PROCEDURE_DESC_LAST
} ARAD_PP_EG_ENCAP_ACCESS_PROCEDURE_DESC;

typedef enum
{
  
  
  ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR = ARAD_PP_ERR_DESC_BASE_EG_ENCAP_ACCESS_FIRST,
  ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR,
  
  
  ARAD_PP_EG_ENCAP_ACCESS_ERR_LAST
} ARAD_PP_EG_ENCAP_ACCESS_ERR;

typedef struct
{  
  uint32 next_outlif;
  uint32 next_vsi_lsb;
  
  uint32 mpls2_label;
  uint32 mpls2_command;
  
  uint32 has_cw;
  uint32 tpid_profile;
  uint32 model_is_pipe;
  uint32 upper_layer_protocol;
  
  uint32 mpls1_label;
  uint32 mpls1_command;  
  uint32 oam_lif_set;
  uint32 drop;  
  uint32 outlif_profile;
  uint32 mpls_swap_remark_profile;
} ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT;

typedef struct
{
  uint32 next_outlif;  
  uint32 next_vsi_lsb;
  uint32 encapsulation_mode;
  uint32 ipv4_tos_index;
  uint32 ipv4_ttl_index;
  uint32 ipv4_src_index;
  uint32 ipv4_dst;
  uint32 oam_lif_set;
  uint32 drop;
  uint32 outlif_profile;
} ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT;

typedef struct
{    
  uint32 next_outlif_lsb;
  uint32 next_outlif_valid;
  uint32 vid;
  uint32 vid_valid;  
  uint32 dest_mac[2];
  uint32 remark_profile;
  uint32 oam_lif_set;
  uint32 drop;
  uint32 outlif_profile;
  uint32 native_ll;
} ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT;


typedef struct
{
  uint32 next_outlif;
  uint32 next_outlif_valid;
  uint32 data[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];
  uint32 oam_lif_set;
  uint32 drop;
  uint32 outlif_profile;
  uint32 rsv_bits;
} ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT;

typedef struct
{
  uint32 next_outlif;
  uint32 next_outlif_valid;
  uint32 next_vsi_lsb;
  uint32 remark_profile;
  uint32 oam_lif_set;
  uint32 drop; 
  uint32 outlif_profile;
  uint32 outrif_profile_index;
} ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT;

typedef struct
{
  uint32 pcp_dei_profile;
  uint32 vid[2];
  uint32 vlan_edit_profile;
  uint32 oam_lif_set;
  uint32 drop;
#ifdef BCM_88660_A0
  uint32 use_as_data_entry;
  uint32 data[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];
#endif
  uint32 outlif_profile;
} ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT;

typedef struct
{
  uint32 next_outlif;
  uint32 next_outlif_valid;
  uint32 m;
  uint32 nick;
  uint32 oam_lif_set;
  uint32 drop; 
  uint32 outlif_profile;
  uint32 my_nickname_index;
  uint32 remark_profile;
} ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT;










uint32
  arad_pp_eg_encap_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
arad_pp_eg_encap_access_entry_read(int unit, int outlif, uint32 *data);

uint32
arad_pp_eg_encap_access_entry_write(int unit, int outlif, uint32 *data);


soc_error_t 
  arad_pp_eg_encap_access_create_none_entry_in_buffer(
     int     unit,
     uint32  table_entry[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE]
     );




soc_error_t
    arad_pp_eg_encap_access_clear_bank(int unit, int bank_id);



uint32
  arad_pp_eg_encap_access_entry_init_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif
  );

uint32
  arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE *key_prefix
  );

uint32
  arad_pp_eg_encap_access_ip_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_ip_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_link_layer_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_link_layer_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_data_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_data_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_trill_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_trill_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
        SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_local_outlif_additional_data_location_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int outlif, 
    SOC_SAND_OUT int *bank_id, 
    SOC_SAND_OUT int *entry_id 
  );

uint32
arad_pp_eg_enacp_access_additional_data_entry_get(    
    SOC_SAND_IN int unit, 
    SOC_SAND_IN  uint32 outlif,
    SOC_SAND_OUT uint32 *data
  );

uint32
arad_pp_eg_enacp_access_additional_data_entry_set(    
    SOC_SAND_IN int unit, 
    SOC_SAND_IN  uint32 outlif,
    SOC_SAND_OUT uint32 *data
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_encap_access_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_encap_access_get_errs_ptr(void);



soc_error_t 
  arad_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_set(
     int     unit, 
     uint32  vlan_edit_profile
     ); 



soc_error_t 
  arad_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_get(
     int      unit, 
     uint32*  vlan_edit_profile
     ); 




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


