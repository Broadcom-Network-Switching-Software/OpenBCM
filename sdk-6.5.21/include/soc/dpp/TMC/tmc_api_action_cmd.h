/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_ACTION_CMD_INCLUDED__

#define __SOC_TMC_API_ACTION_CMD_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/TMC/tmc_api_general.h>






#define SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_IN_PORT           (1 << 0)
#define SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_VSQ               (1 << 1)
#define SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_CNM_CANCEL        (1 << 2)
#define SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_TRUNK_HASH_RESULT (1 << 3)
#define SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_ECN_VALUE         (1 << 4)


#define SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_STAMPING_FHEI     (1 << 0)


#define SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER -1









typedef enum
{
  
  SOC_TMC_ACTION_CMD_SIZE_BYTES_64 = 0,
  
  SOC_TMC_ACTION_CMD_SIZE_BYTES_128 = 1,
  
  SOC_TMC_ACTION_CMD_SIZE_BYTES_192 = 2,

  SOC_TMC_ACTION_CMD_SIZE_BYTES_256 = 3,
  
  SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT = 4,
  
  SOC_TMC_ACTION_NOF_CMD_SIZE_BYTESS = 5
}SOC_TMC_ACTION_CMD_SIZE_BYTES;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 value;
  
  uint8 enable;

} SOC_TMC_ACTION_CMD_OVERRIDE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_DEST_INFO dest_id;
  
  SOC_TMC_ACTION_CMD_OVERRIDE tc;
  
  SOC_TMC_ACTION_CMD_OVERRIDE dp;
  
  SOC_TMC_ACTION_CMD_OVERRIDE meter_ptr_low;
  
  SOC_TMC_ACTION_CMD_OVERRIDE meter_ptr_up;
  
  SOC_TMC_ACTION_CMD_OVERRIDE meter_dp;
  
  SOC_TMC_ACTION_CMD_OVERRIDE counter_ptr_1;
  
  SOC_TMC_ACTION_CMD_OVERRIDE counter_ptr_2;
  
  uint8 is_ing_mc;
  
  SOC_TMC_ACTION_CMD_OVERRIDE outlif;

} SOC_TMC_ACTION_CMD;

typedef struct {
    uint32 valid;
    
    uint32 cpu_trap_code;
    
    uint32 cpu_trap_qualifier;
    
    uint32         encap_id;
} SOC_TMC_ACTION_CMD_SNOOP_MIRROR_STAMPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ACTION_CMD cmd;
  
  SOC_TMC_ACTION_CMD_SIZE_BYTES size;
  
  uint32 prob;
  
  uint32 outbound_prob;
  
  uint32         is_trap; 
  uint32         crop_pkt;  
  uint32         add_orig_head; 

  
  uint32         valid; 
  bcm_gport_t    in_port; 
  uint16         vsq; 
  uint8          cnm_cancel; 
  uint32         trunk_hash_result; 
  uint8          ecn_value;   
  uint8          is_out_mirror_disable; 
  uint8          is_eei_invalid;
  uint32         action_type;
  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_STAMPING_INFO stamping;
  uint8 ext_stat_id_update; 
  int ext_stat_id[2]; 
} SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO;









void
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(
    SOC_SAND_OUT SOC_TMC_ACTION_CMD_OVERRIDE *info
  );

void
  SOC_TMC_ACTION_CMD_clear(
    SOC_SAND_OUT SOC_TMC_ACTION_CMD *info
  );

void
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_STAMPING_INFO_clear( 
       SOC_TMC_ACTION_CMD_SNOOP_MIRROR_STAMPING_INFO *info
       );
void
  SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO *info
  );

void
  SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_ACTION_CMD_SIZE_BYTES_to_string(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_SIZE_BYTES enum_val
  );

void
  SOC_TMC_ACTION_CMD_OVERRIDE_print(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_OVERRIDE *info
  );

void
  SOC_TMC_ACTION_CMD_print(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD *info
  );

void
  SOC_TMC_ACTION_CMD_SNOOP_INFO_print(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO *info
  );

void
  SOC_TMC_ACTION_CMD_MIRROR_INFO_print(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
