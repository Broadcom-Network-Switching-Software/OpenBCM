/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <appl/applref/applrefIface.h>
#include <bcm/error.h>

#ifdef INCLUDE_EXAMPLES
/** cmd usage */
char cmd_example_exec_usage[] = 
    "Usage: \n\t"
    " example list <category>\n\t"
    " example info cmd\n\t"
    " example exec cmd <param ...>\n\t"
    " example selftest cmd\n";

typedef ApplRefFunction_t * (*_myftype)(void);
/** function pointer, interface with the applreflib */
extern _myftype exampleCmdGetCmdList;

/** dumppest linear search for a command */
static ApplRefFunction_t * findFunc(ApplRefFunction_t *pe, char *pn);

/** return 1 if match previous prefix, otherwise return 0 */
static int match_str(char *pstr, char *pc, int n);

/**
 * execute example_cmd.
 * @ param unit chip unit
 * @ a argument list 
 * @return stat.
 */
cmd_result_t cmd_example_exec(int unit, args_t *a) {
  ApplRefFunction_t * pexpls;
  char *subcmd;
  int rc = 0;

  cli_out("Invoking example cmd\n");

  if ( (subcmd = ARG_GET(a)) == NULL )
    return CMD_USAGE;

  if ( exampleCmdGetCmdList == NULL || 
       (pexpls = (*exampleCmdGetCmdList)()) == NULL ||
       pexpls->pf == NULL ) 
    {
      cli_out("No Examples Available\n");
      return CMD_OK;
    }
  
  if ( sal_strcasecmp(subcmd, "list") == 0 ) {
    ApplRefFunction_t *pe;
    char buf[128];
    buf[0] = 0;

    for( pe = pexpls; pe->pf != NULL; pe++ ) {

      if ( match_str( pe->pname, buf, sizeof(buf)-1 ) )
	cli_out("===================================================================================\n");

      cli_out("  %-25s : %s\n", pe->pname, pe->pbriefdescr);
    }
    return CMD_OK;
  }


  if ( sal_strcasecmp(subcmd, "alllist") == 0 ) {
    ApplRefFunction_t *pe;
    char buf[128];
    buf[0] = 0;

    for( pe = pexpls; pe->pf != NULL; pe++ ) {

      if ( match_str( pe->pname, buf, sizeof(buf)-1 ) )
	cli_out("===================================================================================\n");

      cli_out("  %-25s : %s\n", pe->pname, pe->pbriefdescr);

      cli_out("\n**********************************************\n");
      cli_out("%s\n", pe->pdescr);
      cli_out("\n**********************************************\n");      

    }
    return CMD_OK;
  }


  if ( sal_strcasecmp(subcmd, "info") == 0 ) {
    ApplRefFunction_t *pe;
    
    if ( (subcmd = ARG_GET(a)) == NULL )
      return CMD_USAGE;
    
    pe = findFunc( pexpls, subcmd );
    if ( pe == NULL ) {
      cli_out("Example %s is not found\n", subcmd);
      return CMD_FAIL;
    }

    cli_out("\n**********************************************\n");
    cli_out("%s\n", pe->pdescr);
    cli_out("\n**********************************************\n");
    
    return CMD_OK;
  }

  if ( sal_strcasecmp(subcmd, "exec") == 0 ) {
    ApplRefFunction_t *pe;

    if ( (subcmd = ARG_GET(a)) == NULL )
      return CMD_USAGE;
    
    pe = findFunc( pexpls, subcmd );
    if ( pe == NULL ) {
      cli_out("Example %s is not found\n", subcmd);
      return CMD_FAIL;
    }

    /** invoke command */
    {
      char *args[16]; /** limit to 16 args */
      int n = 0;
      
      while ( (n < 16) && ( (args[n] = ARG_GET(a)) != NULL ) )
	n++;

      if ( n == 16 ) {
	cli_out("Excessive amount of arguments\n");
	return CMD_FAIL;
      }

      if ( BCM_FAILURE(rc = (*pe->dispatch)(pe->pf, n, args)) ) {
	cli_out("Error : fail to execute %d %s\n", rc, bcm_errmsg(rc) );
	return rc;
      }
      	
    }
    
    return CMD_OK;
  }

  return CMD_USAGE;

}


/** change it later to better search algorithm */
static ApplRefFunction_t * findFunc(ApplRefFunction_t *pe, char *pn) {

  if ( pe == NULL )
    return NULL;

  while ( pe->pf != NULL ) {
    if ( sal_strcasecmp( pe->pname, pn ) == 0 )
      return pe;
    pe++;
  }

  return NULL;
}

static int match_str(char *pstr, char *pc, int n) {
  int i, stat;
  
  char *c;
  for( c = pstr; *c != 0; c++ ) 
    if ( *c == '_' )
      break;

  if ( c == 0 ) {
    pc[0] = 0;
    return 1;
  }
  
  stat = 0;
  for(i = 0; i < c - pstr && i < n; i++) {
    if ( pc[i] != pstr[i] ) {
      stat = 1;
      pc[i] = pstr[i];
    }
  }

  if ( pc[i] != 0 || i == n )
    stat = 1;

  pc[i] = 0;

  return stat;
}
#else
int __no_complilation_complain________0;
#endif
