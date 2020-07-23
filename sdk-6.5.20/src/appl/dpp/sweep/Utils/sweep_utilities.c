/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
    #error  "Add your system support for packed attribute."
#endif

/*************
 * INCLUDES  *
 */
/* { */
#include <appl/dpp/sweep/Utils/sweep_utilities.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#ifdef FAP20M_STANDALONE_PACKAGE
  #include <appl/dpp/sweep/Utils/Fap20m/sweep_fap20m_gfa.h>
#endif
/*
 * Remote-Calls
 */
#if !defined(FAP20M_STANDALONE_PACKAGE) && !(DUNE_STANDALONE_PACKAGE)
  #include <RefDesign/dune_rpc_auto_network_learning.h>
  #include <RefDesign/dune_remote_access.h>
  #include <Pub/utils_defx.h>
#endif

#include <stdio.h>

/*
 * Public
 */

/* } */

/*************
 * DEFINES   *
 */
/* { */
/* } */

/*************
 *  MACROS   *
 */
/* { */

/* } */

/*************
 * TYPE DEFS *
 */
/* { */

/* } */

/*************
 * GLOBALS   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

/* } */



#if !defined(FAP20M_STANDALONE_PACKAGE) && !(DUNE_STANDALONE_PACKAGE)
/*
 * Sweep Remote Calls Utilities.
 * {
 */
/*
 * Not ZERO indicates error.
 */
unsigned int
  sweep_init_remote_access_system(
    void
  )
{
  enum
  {
    NO_ERR = 0,
    ERR_001,
    NOF_ERR
  }err;
  unsigned int
    num_units,
    unit_i;
  char
    *proc_name = "sweep_init_remote_access_system";

  err = NO_ERR;

  num_units =
      dune_rpc_get_number_of_fabric_cards() +
      dune_rpc_get_number_of_line_cards();

  if(0 != dune_rpc_set_number_of_units(num_units))
  {
    gen_err(FALSE,FALSE,(int)err,0,
            "sweep_init_remote_access_system learning - Error allocating memory",
            proc_name,SVR_WRN,SOC_SAND_GEN_ERR,TRUE,0,-1,FALSE) ;
    err = ERR_001 ;
    goto exit;
  }


  for (unit_i = 0 ; unit_i < num_units ; unit_i++)
  {
    if(unit_i < dune_rpc_get_number_of_line_cards())
    {
      dune_rpc_set_remote_device_and_unit(
        unit_i, unit_i | SOC_SAND_BIT(30),
        dune_rpc_get_ip_of_line_card_at(unit_i)
      );
    }
    else /* fabric cards */
    {
      dune_rpc_set_remote_device_and_unit(
        unit_i,
        (2*unit_i)   | SOC_SAND_BIT(31),
        dune_rpc_get_ip_of_fabric_card_at(unit_i - dune_rpc_get_number_of_line_cards()));
      dune_rpc_set_remote_device_and_unit(
        unit_i,
        (2*unit_i+1) | SOC_SAND_BIT(31),
        dune_rpc_get_ip_of_fabric_card_at(unit_i - dune_rpc_get_number_of_line_cards()));
    }
  }

exit:
  return err;
}
#endif
/*
 * }
 */
int
  swp_read_line(
    SOC_SAND_INOUT FILE*                file_p,
    SOC_SAND_INOUT char                 line[SWEEP_MAX_LINE_SIZE]
)
{
  unsigned int
    char_i,
    nof_read;
  char
    character;
  const char* proc_name ="swp_read_line";
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002,
    NUM_ERRORS
  } err ;
  err = NO_ERR;

  char_i = 0;
  if(line == NULL)
  {
    soc_sand_os_printf("%s received NULL\n\r", proc_name);
    err = ERR_001;

    goto exit;
  }
  soc_sand_os_memset(line, 0x0, SWEEP_MAX_LINE_SIZE);

  while(TRUE)
  {
    nof_read = fread(&character, 1, 1, file_p);
    if(nof_read == 0)
    {
      goto exit;
    }
    if( character == '\n' )
    {
      /*
      * Encounter new line.
      */
      goto exit;
    }
    else if (character == '\r')
    {
      /*
      * Do nothing.
      */
    }
    else
    {
      line[char_i] = character;
      char_i++;
      if(char_i >= SWEEP_MAX_LINE_SIZE)
      {
        /*
        * Exceeding maximum line size.
        */
        soc_sand_os_printf(
          " line is bigger than %u characters\n\r",
          SWEEP_MAX_LINE_SIZE
          );
        err = ERR_002;
        goto exit;
      }
    }
  }

exit:
  return err;
}


int
swp_remove_comment(
           SOC_SAND_INOUT char                 line[SWEEP_MAX_LINE_SIZE]
)
{
  unsigned int
    char_i,
    str_size;
  const char* proc_name ="swp_remove_comment";
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002,
    NUM_ERRORS
  } err ;
  err = NO_ERR;

  char_i = 0;
  if(line == NULL)
  {
    soc_sand_os_printf("%s received NULL\n\r", proc_name);
    err = ERR_001;
    goto exit;
  }

  str_size = soc_sand_os_strlen(line);
  if(str_size>SWEEP_MAX_LINE_SIZE)
  {
    str_size = SWEEP_MAX_LINE_SIZE;
  }

  for(char_i=1; char_i < str_size; ++char_i)
  {
    if(
      (line[char_i-1] == '/') &&
      (line[char_i] == '/')
      )
    {
      line[char_i-1] = '\0';
      goto exit;
    }
  }

exit:
  return err;
}

int
swp_remove_spaces_from_end(
               SOC_SAND_INOUT char                 line[SWEEP_MAX_LINE_SIZE]
)
{
  unsigned int
    char_i,
    str_size;
  const char* proc_name ="swp_remove_spaces_from_end";
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002,
    NUM_ERRORS
  } err ;
  err = NO_ERR;


  char_i = 0;
  if(line == NULL)
  {
    soc_sand_os_printf("%s recived NULL\n\r", proc_name);
    err = ERR_001;
    goto exit;
  }

  str_size = soc_sand_os_strlen(line);
  if(str_size>SWEEP_MAX_LINE_SIZE)
  {
    str_size = SWEEP_MAX_LINE_SIZE;
  }
  if(str_size == 0)
  {
    /*
    * Nothing to do here.
    */
    goto exit;
  }

  for(char_i=str_size-1; char_i > 1 ; --char_i)
  {
    if( (line[char_i] == ' ') ||
      (line[char_i] == '\t')
      )
    {
      /*
      * space/tab at the end of line. Continue
      */
    }
    else
    {
      /*
      * no space/tap here.
      */
      break;
    }
  }

  line[char_i+1] = '\0';


exit:
  return err;
}


int
swp_remove_spaces_from_start(
               SOC_SAND_INOUT char                 line[SWEEP_MAX_LINE_SIZE]
)
{
  unsigned int
    char_i,
    str_size,
    start_of_data;
  const char* proc_name ="swp_remove_spaces_from_start";
  char tmp_line[SWEEP_MAX_LINE_SIZE];
  enum
  {
    NO_ERR = 0,
    ERR_001,
    NUM_ERRORS
  } err ;
  err = NO_ERR;


  char_i = 0;
  if(line == NULL)
  {
    soc_sand_os_printf("%s recived NULL\n\r", proc_name);
    err = ERR_001;
    goto exit;
  }

  str_size = soc_sand_os_strlen(line);
  if(str_size>SWEEP_MAX_LINE_SIZE)
  {
    str_size = SWEEP_MAX_LINE_SIZE;
  }

  for(char_i=0; char_i < str_size; ++char_i)
  {
    if( (line[char_i] == ' ') ||
      (line[char_i] == '\t')
      )
    {
      /*
      * space/tab at the start of line. Continue
      */
    }
    else
    {
      /*
      * no space/tap here.
      */
      break;
    }
  }
  start_of_data = char_i;

  if(start_of_data != 0)
  {
    soc_sand_os_memcpy(
      tmp_line,
      &line[start_of_data],
      str_size - start_of_data+1
      );
    soc_sand_os_memcpy(
      line,
      tmp_line,
      str_size - start_of_data+1
      );
  }


exit:
  return err;
}


int
swp_convert_tab_to_space(
             SOC_SAND_INOUT char                 line[SWEEP_MAX_LINE_SIZE]
)
{
  unsigned int
    char_i,
    str_size;
  const char* proc_name ="swp_convert_tab_to_space";
  enum
  {
    NO_ERR = 0,
    ERR_001,
    NUM_ERRORS
  } err ;
  err = NO_ERR;


  char_i = 0;
  if(line == NULL)
  {
    soc_sand_os_printf("%s received NULL\n\r", proc_name);
    err = ERR_001;
    goto exit;
  }

  str_size = soc_sand_os_strlen(line);
  if(str_size>SWEEP_MAX_LINE_SIZE)
  {
    str_size = SWEEP_MAX_LINE_SIZE;
  }

  for(char_i=0; char_i < str_size; ++char_i)
  {
    if(line[char_i] == '\t')
    {
      line[char_i] = ' ';
    }
  }

exit:
  return err;
}


int
swp_remove_spaces(
          SOC_SAND_INOUT char                 line[SWEEP_MAX_LINE_SIZE]
)
{
  unsigned int
    char_i;
  const char* proc_name ="swp_remove_spaces";
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004,
    NUM_ERRORS
  } err ;
  err = NO_ERR;


  char_i = 0;
  if(line == NULL)
  {
    soc_sand_os_printf("%s received NULL\n\r", proc_name);
    err = ERR_001;
    goto exit;
  }

  if(swp_remove_spaces_from_end(line) )
  {
    err = ERR_002;
    goto exit;
  }

  if(swp_remove_spaces_from_start(line) )
  {
    err = ERR_003;
    goto exit;
  }

  if(swp_convert_tab_to_space(line) )
  {
    err = ERR_004;
    goto exit;
  }

exit:
  return err;
}


int
swp_read_and_clear_line(
            SOC_SAND_INOUT FILE* file_p,
            SOC_SAND_INOUT char  line[SWEEP_MAX_LINE_SIZE]
)
{
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003,
    ERR_006,
    NUM_ERRORS
  } err ;

  err = NO_ERR;



  if (swp_read_line(file_p, line))
  {
    err = ERR_001;
    goto exit;
  }

  if (swp_remove_comment(line))
  {
    err = ERR_002;
    goto exit;
  }

  if (swp_remove_spaces(line))
  {
    err = ERR_003;
    goto exit;
  }



exit:
  return err;
}


/*
*/
/*****************************************************
 * The seed for random numbers
 */
unsigned long Sweep_config_seed=SWEEP_USE_RANDOM_SEED;

/*****************************************************
 * See details in sweep_utilities.h
 */
unsigned int
  sweep_set_random_seed(
    unsigned int given_seed
  )
{
  unsigned long seconds, nano_seconds;

  if(given_seed == SWEEP_USE_RANDOM_SEED)
  {
    soc_sand_os_get_time(&seconds, &nano_seconds);
    Sweep_config_seed = (unsigned int) (nano_seconds ^ seconds);
  }
  else
    Sweep_config_seed = given_seed;

  soc_sand_os_srand(Sweep_config_seed);

  return TRUE;
}
unsigned long
  sweep_get_random_seed(
    void
  )
{
  return Sweep_config_seed;
}



/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif
