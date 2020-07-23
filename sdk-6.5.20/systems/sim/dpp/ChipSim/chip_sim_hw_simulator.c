/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* includes */
#include <stdio.h>
#ifdef WIN32
  #include <winsock2.h>
  #include <windows.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <malloc.h>
  #include <wsipx.h>
  #include <fcntl.h>
#elif DUNE_CSR_CYG
  #include <sys/socket.h>
  #include <sys/select.h>
  #include <sys/time.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <time.h>
  #include <string.h>
  #include <arpa/inet.h>
#endif

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/ARAD/arad_general.h>
/* 
#include <errno.h>
*/
#define DEFAULT_BUFLEN   1024
#define DEFAULT_SEND_LEN 128
#define DEFAULT_PORT 9999

typedef enum
{
  CHIP_SIM_MSG_TYPE_SET_REG,
  CHIP_SIM_MSG_TYPE_GET_REG,
  CHIP_SIM_MSG_TYPE_SET_MEM,
  CHIP_SIM_MSG_TYPE_GET_MEM,
  CHIP_SIM_MSG_TYPE_SEND_PACKET,
  /*
   *  Number of types in CHIP_SIM_MSG_TYPE
   */
  CHIP_SIM_MSG_NOF_TYPES
}CHIP_SIM_MSG_TYPE;

#define CHIP_SIM_ERR_AND_EXIT(msg) \
  if (res != SOC_SAND_OK)\
  {\
    soc_sand_os_printf(msg);\
    goto exit;\
  }

#define CHIP_SIM_SPRINTF_TO_BUFFER1(args) \
  Chip_sim_buffer_level += sal_sprintf(&(Chip_sim_buffer[Chip_sim_buffer_level]), args);
#define CHIP_SIM_SPRINTF_TO_BUFFER2(args, args2) \
  Chip_sim_buffer_level += sal_sprintf(&(Chip_sim_buffer[Chip_sim_buffer_level]), args, args2);
#define CHIP_SIM_SPRINTF_TO_BUFFER3(args, args2, args3) \
  Chip_sim_buffer_level += sal_sprintf(&(Chip_sim_buffer[Chip_sim_buffer_level]), args, args2, args3);
#define CHIP_SIM_SPRINTF_TO_BUFFER4(args, args2, args3, args4) \
  Chip_sim_buffer_level += sal_sprintf(&(Chip_sim_buffer[Chip_sim_buffer_level]), args, args2, args3, args4);


/*
 *	Global socket definition
 */
#if NEGEV_NATIVE
SOCKET 
#else
int
#endif
  Chip_sim_connect_socket;

static
  uint32
    Chip_sim_buffer_level = 0;

static
  char
    *Chip_sim_buffer;

#define CHIP_SIM_BUFFER_MAX_SIZE               (10000000)

/*
 *	Allocation of place
 */
uint32
  chip_sim_buffer_alloc(void)
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_ALLOC_ANY_SIZE(Chip_sim_buffer, char, CHIP_SIM_BUFFER_MAX_SIZE, "Chip_sim_buffer");

  goto exit;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in chip_sim_buffer_alloc()",0,0);
}
void
  chip_sim_buffer_free(void)
{
  ARAD_FREE(Chip_sim_buffer);
  Chip_sim_buffer_level = 0;
}

/*
 *	Add the CLI name to the file
 */
uint32
  chip_sim_cli_name_add(
    SOC_SAND_IN  uint32       driver_api_result
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    error_word,
    proc_id ;
  char
    *soc_sand_proc_name,
    *soc_sand_module_name ;

  if (Chip_sim_buffer != NULL)
  {
  /*
   * Initialize error word
   */
    soc_sand_initialize_error_word(0,0,&error_word) ;
    soc_sand_set_proc_id_into_error_word(driver_api_result, &error_word) ;
    proc_id = soc_sand_get_proc_id_from_error_word(error_word) ;
    soc_sand_proc_id_to_string((uint32)proc_id,&soc_sand_module_name,&soc_sand_proc_name)  ;
    if ((soc_sand_proc_name[0] == 'P') && (soc_sand_proc_name[1] == 'B'))
    {
      CHIP_SIM_SPRINTF_TO_BUFFER2("CLI: %s", soc_sand_proc_name);
    }
  }
  
  return res;
}

      
/*
 *	Print the file in an output
 */
uint32
  chip_sim_print_output(void)
{
  uint32 
    res = SOC_SAND_OK;
  FILE
    *ofp;
  char
    filename[200] = "";
  uint32
    nof_bytes = 0;
  uint32
    seconds, 
    nano_seconds;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_sand_os_get_time(&seconds, &nano_seconds);
  filename[0] = 0;
  nof_bytes += sal_sprintf(&(filename[nof_bytes]),"C:\\Boot\\simulator\\");
  nof_bytes += sal_sprintf(&(filename[nof_bytes]),"reg_cmd_%d", ((unsigned int) (nano_seconds ^ seconds)) % 100000);
  nof_bytes += sal_sprintf(&(filename[nof_bytes]),".txt");
  ofp = fopen(filename, "w");
  if (ofp == NULL) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
  fprintf(ofp, "%s", Chip_sim_buffer);
  fclose(ofp);
  chip_sim_buffer_free();
  res = chip_sim_buffer_alloc();
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in chip_sim_buffer_alloc()",0,0);
}

uint32
  hw_client_close(void)
{
#if NEGEV_NATIVE
  closesocket(Chip_sim_connect_socket);
#else
#if 1
  soc_sand_os_printf("hw_client_close\n\r");
#endif
#endif

  return SOC_SAND_OK;
}
