/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_OFP_RATES_INCLUDED__

#define __SOC_TMC_API_OFP_RATES_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>




#define SOC_TMC_OFP_RATES_BURST_LIMIT_MAX               (0xFFFF)
#define SOC_TMC_OFP_RATES_BURST_DEFAULT                 (0x4000)
#define SOC_TMC_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX       (0x3FFF)
#define SOC_TMC_OFP_RATES_BURST_FC_Q_LIMIT_MAX          (0x3FFF)
#define SOC_TMC_OFP_RATES_SCH_BURST_LIMIT_MAX           (0x7FFF)
#define SOC_TMC_OFP_RATES_ILLEGAL_PORT_ID     (SOC_TMC_NOF_FAP_PORTS)











typedef enum
{
  
  SOC_TMC_OFP_RATES_CAL_SET_A=0,
  
  SOC_TMC_OFP_RATES_CAL_SET_B=1,
  
  SOC_TMC_OFP_NOF_RATES_CAL_SETS=2
}SOC_TMC_OFP_RATES_CAL_SET;

typedef enum
{
  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE = 0,

  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST = 1,

  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN = 2,

  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_NOF_FIELD_TYPE = 3
} SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE;

typedef enum
{
  
  SOC_TMC_OFP_RATES_EGQ_CAL_CHAN_ARB = 0,

  
  SOC_TMC_OFP_RATES_EGQ_CAL_TCG = 1,

  
  SOC_TMC_OFP_RATES_EGQ_CAL_PORT_PRIORITY = 2

} SOC_TMC_OFP_RATES_EGQ_CAL_TYPE;

typedef struct
{
  
  SOC_TMC_OFP_RATES_EGQ_CAL_TYPE cal_type;
  
  uint32 chan_arb_id;
} SOC_TMC_OFP_RATES_CAL_INFO;

typedef enum
{
  
  SOC_TMC_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS=0,
  
  SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE=1,
  
  SOC_TMC_OFP_SHPR_UPDATE_MODE_DONT_TUCH=2,
  
  SOC_TMC_OFP_NOF_SHPR_UPDATE_MODES=3
}SOC_TMC_OFP_SHPR_UPDATE_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode;
  
  uint32 rate;
}SOC_TMC_OFP_RATES_MAL_SHPR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode;
  
  uint32 rate;
}SOC_TMC_OFP_RATES_INTERFACE_SHPR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_OFP_RATES_MAL_SHPR sch_shaper;
  
  SOC_TMC_OFP_RATES_MAL_SHPR egq_shaper;
}SOC_TMC_OFP_RATES_MAL_SHPR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_OFP_RATES_INTERFACE_SHPR sch_shaper;
  
  SOC_TMC_OFP_RATES_INTERFACE_SHPR egq_shaper;
}SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  uint32 flags;
  
  uint32 port_id;
  
  uint32 sch_rate;
  
  uint32 egq_rate;
  
  uint32 max_burst;
  
  uint32 sch_max_burst;
  
  uint32 port_priority;
  
  SOC_TMC_TCG_NDX tcg_ndx;
}SOC_TMC_OFP_RATE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 nof_valid_entries;
  
  SOC_TMC_OFP_RATE_INFO rates[SOC_TMC_NOF_FAP_PORTS_MAX];
}SOC_TMC_OFP_RATES_TBL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 sch_rate;
  
  uint32 egq_rate;
  
  uint32 max_burst;
}SOC_TMC_OFP_FAT_PIPE_RATE_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 rate;
  
  uint32 max_burst;
}SOC_TMC_OFP_RATE_SHPR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_OFP_RATE_SHPR_INFO sch_shaper;
  
  SOC_TMC_OFP_RATE_SHPR_INFO egq_shaper;
}SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_OFP_RATE_SHPR_INFO sch_shaper;
  
  SOC_TMC_OFP_RATE_SHPR_INFO egq_shaper;
}SOC_TMC_OFP_RATES_TCG_SHPR_INFO;











void
  SOC_TMC_OFP_RATES_MAL_SHPR_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_MAL_SHPR *info
  );

void
  SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_MAL_SHPR_INFO *info
  );

void
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_INTERFACE_SHPR *info
  );

void
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO *info 
  );

void
  SOC_TMC_OFP_RATE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATE_INFO *info
  );

void
  SOC_TMC_OFP_RATES_TBL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_TBL_INFO *info
  );

void
  SOC_TMC_OFP_FAT_PIPE_RATE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_FAT_PIPE_RATE_INFO *info
  );

void
  SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO *info
  );

void
  SOC_TMC_OFP_RATE_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATE_SHPR_INFO *info
  );

void
  SOC_TMC_OFP_RATES_TCG_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_TCG_SHPR_INFO *info
  );


#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_OFP_RATES_CAL_SET_to_string(
    SOC_SAND_IN SOC_TMC_OFP_RATES_CAL_SET enum_val
  );

const char*
  SOC_TMC_OFP_SHPR_UPDATE_MODE_to_string(
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE enum_val
  );

void
  SOC_TMC_OFP_RATES_MAL_SHPR_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_MAL_SHPR *info
  );

void
  SOC_TMC_OFP_RATES_MAL_SHPR_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_MAL_SHPR_INFO *info
  );

void
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_INTERFACE_SHPR *info
  );

void
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO *info
  );

void
  SOC_TMC_OFP_RATE_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_RATE_INFO *info
  );

void
  SOC_TMC_OFP_RATES_TBL_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_TBL_INFO *info
  );

void
  SOC_TMC_OFP_FAT_PIPE_RATE_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_FAT_PIPE_RATE_INFO *info
  );

void
  SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO_print(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO *info
  );

void
  SOC_TMC_OFP_RATE_SHPR_INFO_print(
    SOC_SAND_OUT SOC_TMC_OFP_RATE_SHPR_INFO *info
  );

void
  SOC_TMC_OFP_RATES_TCG_SHPR_INFO_print(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_TCG_SHPR_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
