/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_INGRESS_PACKET_QUEUING_INCLUDED__

#define __SOC_TMC_API_INGRESS_PACKET_QUEUING_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>




#define SOC_TMC_IPQ_INVALID_FLOW_QUARTET    0x3fff




#define SOC_TMC_IPQ_Q_TO_QRTT_ID(que_id) ((que_id)/4)
#define SOC_TMC_IPQ_QRTT_TO_Q_ID(q_que_id) ((q_que_id)*4)
#define SOC_TMC_IPQ_Q_TO_1K_ID(que_id) ((que_id)/1024)
#define SOC_TMC_IPQ_1K_TO_Q_ID(k_que_id) ((k_que_id)*1024)


#define SOC_TMC_IPQ_STACK_LAG_DOMAIN_MIN     0
#define SOC_TMC_IPQ_STACK_LAG_DOMAIN_MAX     64

#define SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MIN     0
#define SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX     (SOC_IS_JERICHO(unit)? 256 : 64)
#define SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_ALL     0xffffffff

#define SOC_TMC_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_MIN     0
#define SOC_TMC_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_MAX     4
#define SOC_TMC_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_ALL     0xffffffff







typedef uint32 SOC_TMC_IPQ_TR_CLS;

typedef enum
{
  
  SOC_TMC_IPQ_TR_CLS_MIN=0,
  
  SOC_TMC_IPQ_TR_CLS_MAX=7,
  
  SOC_TMC_IPQ_TR_CLS_RNG_LAST
}SOC_TMC_IPQ_TR_CLS_RNG;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 base_queue_id;
  
  uint8 queue_id_add_not_decrement;
}SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO;
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 valid;
  
  uint32 base_queue;

} SOC_TMC_IPQ_BASEQ_MAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 flow_quartet_index;
  
  uint8 is_composite;
  
  uint8 is_modport;
  
  uint32 system_physical_port;
  
  uint16 fap_id;
  uint16 fap_port_id;
}SOC_TMC_IPQ_QUARTET_MAP_INFO;











void
  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );

void
  SOC_TMC_IPQ_BASEQ_MAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_IPQ_BASEQ_MAP_INFO *info
  );

void
  SOC_TMC_IPQ_QUARTET_MAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_IPQ_QUARTET_MAP_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_IPQ_TR_CLS_RNG_to_string(
    SOC_SAND_IN SOC_TMC_IPQ_TR_CLS_RNG enum_val
  );

void
  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(
    SOC_SAND_IN SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );

void
  SOC_TMC_IPQ_BASEQ_MAP_INFO_print(
    SOC_SAND_IN  SOC_TMC_IPQ_BASEQ_MAP_INFO *info
  );

void
  SOC_TMC_IPQ_QUARTET_MAP_INFO_print(
    SOC_SAND_IN SOC_TMC_IPQ_QUARTET_MAP_INFO *info
  );

void
  soc_tmcips_non_empty_queues_print(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   print_first_local_flow
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
