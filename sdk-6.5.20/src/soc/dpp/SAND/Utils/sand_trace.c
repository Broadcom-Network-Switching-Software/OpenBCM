/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/




#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_trace.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>


typedef struct
{
  
  uint32  time_signature;

  
  uint32      identifier;
  const char*       str;

} SOC_SAND_TRACE;


#define SOC_SAND_TRACE_DEFAULT_TABLE_SIZE 50


static SOC_SAND_TRACE
  Soc_sand_trace_table[SOC_SAND_TRACE_DEFAULT_TABLE_SIZE];


static uint32
  Soc_sand_trace_table_index = 0;


static uint32
  Soc_sand_trace_table_turn_over = FALSE;





SOC_SAND_RET
  soc_sand_trace_init(
    void
  )
{
  SOC_SAND_RET
    ex;

  ex = SOC_SAND_OK;

  Soc_sand_trace_table_index = 0;
  Soc_sand_trace_table_turn_over = FALSE;
  soc_sand_os_memset(Soc_sand_trace_table,
                 0,
                 sizeof(SOC_SAND_TRACE)*SOC_SAND_TRACE_DEFAULT_TABLE_SIZE);

  return ex;
}


SOC_SAND_RET
  soc_sand_trace_end(
    void
  )
{
  return SOC_SAND_OK;
}


SOC_SAND_RET
  soc_sand_trace_clear(
    void
  )
{
  SOC_SAND_RET
    ex;
  ex = soc_sand_trace_init();
  return ex;
}



SOC_SAND_RET
  soc_sand_trace_add_entry(
    SOC_SAND_IN uint32    identifier,
    SOC_SAND_IN char* const     str 
  )
{
  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INTERRUPTS_STOP;

  Soc_sand_trace_table[Soc_sand_trace_table_index].identifier = identifier;
  Soc_sand_trace_table[Soc_sand_trace_table_index].str        = str;
  Soc_sand_trace_table[Soc_sand_trace_table_index].time_signature =
    soc_sand_os_get_time_micro();
  Soc_sand_trace_table_index ++;
  if( Soc_sand_trace_table_index >= SOC_SAND_TRACE_DEFAULT_TABLE_SIZE )
  {
    Soc_sand_trace_table_index = 0;
    Soc_sand_trace_table_turn_over = TRUE;
  }

  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
  return SOC_SAND_OK;
}




SOC_SAND_RET
  soc_sand_trace_print(
    void
  )
{
  int
    index;
  uint32
    micro_time_diff;

  LOG_CLI((BSL_META("soc_sand_trace_print():\n\r")));
  LOG_CLI((BSL_META("Soc_sand_trace_table_index:%u  Soc_sand_trace_table_turn_over:%u\n\r"),
Soc_sand_trace_table_index,
           Soc_sand_trace_table_turn_over));
  for(index=0; index<SOC_SAND_TRACE_DEFAULT_TABLE_SIZE; ++index)
  {
    if(index>0)
    {
      micro_time_diff =
        Soc_sand_trace_table[index].time_signature -
          Soc_sand_trace_table[index-1].time_signature;

    }
    else
    {
      micro_time_diff =
        Soc_sand_trace_table[0].time_signature -
          Soc_sand_trace_table[SOC_SAND_TRACE_DEFAULT_TABLE_SIZE-1].time_signature;
    }
    LOG_CLI((BSL_META("%3d:  micro_time_diff(%5u), identifier(%5u), str(%s)\n\r"),
index,
             micro_time_diff,
             Soc_sand_trace_table[index].identifier,
             Soc_sand_trace_table[index].str
             ));
  }


  return SOC_SAND_OK;
}
