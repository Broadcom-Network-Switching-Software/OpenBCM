/* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $Copyright
 * $
*/


#ifndef __ARAD_CHIP_DEFINES_INCLUDED__

#define __ARAD_CHIP_DEFINES_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/TMC/tmc_api_reg_access.h>
#include <soc/dpp/dpp_config_defs.h>






#define ARAD_EXPECTED_CHIP_TYPE     0x0
#define ARAD_EXPECTED_CHIP_VER_01   0x01
#define ARAD_EXPECTED_CHIP_VER      0x01

#define ARAD_TOTAL_SIZE_OF_REGS     0x1000 


#define ARAD_NOF_LOCAL_PORTS(unit) SOC_DPP_DEFS_GET(unit, nof_logical_ports)
#define ARAD_NOF_LOCAL_PORTS_MAX 512



#define ARAD_REV_MATCH(val_actual, val_expected) \
  (SOC_SAND_NUM2BOOL((val_actual) == (val_expected)))

typedef enum
{
  
  ARAD_REV_A0=0,
  
  ARAD_REV_A1=1,
  
  ARAD_NOF_REVS
}ARAD_REV;





#define ARAD_DFLT_TICKS_PER_SEC               (600000000)













typedef struct
{
  uint32
    ticks_per_sec;

} __ATTRIBUTE_PACKED__ ARAD_CHIP_DEFINITIONS;


typedef struct
{
  uint16      msb;
  uint16      lsb;
} __ATTRIBUTE_PACKED__ ARAD_TBL_FIELD;

typedef SOC_TMC_REG_ADDR                                       ARAD_REG_ADDR;
typedef SOC_TMC_REG_FIELD                                      ARAD_REG_FIELD;
typedef SOC_TMC_REG_INFO                                       ARAD_REG_INFO;













uint32
  arad_revision_fld_get(
    SOC_SAND_IN  int unit
  );

uint32
  arad_chip_defines_init(int unit);




uint32
  arad_chip_time_to_ticks(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        time_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_IN  uint8       is_round_up,
    SOC_SAND_OUT uint32        *result
  );


uint32
  arad_ticks_to_time(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        ticks_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_OUT uint32        *result
  );

uint32
  arad_chip_ticks_per_sec_get(
    SOC_SAND_IN int unit
  );

void
  arad_chip_kilo_ticks_per_sec_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  clck_freq_khz
  );

uint32
  arad_chip_kilo_ticks_per_sec_get(
    SOC_SAND_IN int unit
  );

uint32
  arad_chip_mega_ticks_per_sec_get(
    SOC_SAND_IN int unit
  );




ARAD_REV
  arad_revision_get(
    SOC_SAND_IN  int  unit
  );

const char*
  arad_ARAD_REV_to_string(
    SOC_SAND_IN ARAD_REV enum_val
  );

void
  arad_ARAD_REG_INFO_clear(
    SOC_SAND_OUT ARAD_REG_INFO *info
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

