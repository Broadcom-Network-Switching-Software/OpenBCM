/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __QAX_INGRESS_SCHEDULER_INCLUDED__




#define QAX_INGRESS_SCHEDULER_HP_MC_SHAPER_INDEX    (5)
#define QAX_INGRESS_SCHEDULER_LP_MC_SHAPER_INDEX    (6)


#define QAX_INGRESS_SCHEDULER_HP_MC_SHAPER_SLOW_START_CONFIG_INDEX    (0)
#define QAX_INGRESS_SCHEDULER_LP_MC_SHAPER_SLOW_START_CONFIG_INDEX    (1)



soc_error_t
qax_ingress_scheduler_clos_slow_start_get (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    );


soc_error_t
qax_ingress_scheduler_clos_slow_start_set (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    );


soc_error_t
qax_ingress_scheduler_mesh_slow_start_get (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER   *shaper_info
    );


soc_error_t
qax_ingress_scheduler_mesh_slow_start_set (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN SOC_TMC_ING_SCH_SHAPER   *shaper_info
    );



#endif
