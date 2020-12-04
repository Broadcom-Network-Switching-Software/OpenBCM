 /*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	variable.c
 * Purpose:	Variable routines with scope support functions.
 */

#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#define	VAR_IS_SYSTEM(_s) (*(_s) == PARSE_VARIABLE_SYSTEM)

/*
 * Typedef:	var_t
 * Purpose:	Maps out a circular double linked list entry 
 *		representing a single variable.
 * Notes:	Each linked list contains a dummy entry at the
 *		head of the list for easy insert/delete.
 */
typedef struct var_s {
    struct var_s	*v_next;	/* Next in linked list */
    struct var_s	*v_prev;	/* Next in linked list */
    char		*v_name;	/* Variable name */
    char		*v_value;	/* Variable value */
} var_t;

/*
 * Typedef:	var_scope_t
 * Purpose:	Defines a scope level for local variables. 
 */
typedef struct var_scope_s {
    struct var_scope_s 	*vs_next;	/* Next in Queue */
    var_t		vs_local;	/* Local variable list */
} var_scope_t;

/*
 * Variable:	var_scope_list
 * Purpose:	Queue of pushed local variable lists, each queue
 *		entry contains a variable list. 
 */
    
static	var_scope_t 	*var_scope_list = NULL;

/*
 * Variable:	var_global_list
 * Purpose:	Global variable list.
 * Notes:	List is created with the dummy queue head.
 */
static	var_t		var_global_list = 
   {&var_global_list, &var_global_list, NULL, NULL};

#define	VAR_SYSTEM_STRING	\
    "\tSystem variables are those that begin with an \"_\". They may be\n\t"\
    "referenced but not set.  Use printenv to show the system variables.\n"

void	*
var_push_scope(void)
/*
 * Function: 	var_push_scope 
 * Purpose:	Push a new variable scope onto the top of the stack.
 * Parameters:	None.
 * Returns:	Pointer to current scope as a cookie. 
 * Notes:	The cookie is useful to allow calling routine to know if the
 *		push was done, and also is passed to POP, and the pop is only
 *		done if the cookie matches the top of the queue. This can help 
 *		in control-c handing.
 */
{
    var_scope_t	*vs;
    int		il;			/* Don't corrupt list */

    vs = (var_scope_t *)sal_alloc(sizeof(var_scope_t), "diag_scope");
    assert(vs);

    il = sal_splhi();
    vs->vs_next  = var_scope_list;
    vs->vs_local.v_next = vs->vs_local.v_prev = &vs->vs_local;
    vs->vs_local.v_name = vs->vs_local.v_value = NULL;
    var_scope_list = vs;		/* Bang - new scope */
    sal_spl(il);
    return((void *)vs);
}

void
var_pop_scope(void *vs_cookie)
/*
 * Function: 	var_pop_scope
 * Purpose:	Pop the current variable scope, and restore previous.
 * Parameters:	Cookie from PUSH, if does not match TOP of list, POP
 *		not done.
 * Returns:	Nothing.
 */
{
    var_scope_t	*vs;
    var_t	*v;
    int		il = sal_splhi();

    vs = var_scope_list;
    if (vs_cookie == vs) {
	var_scope_list = vs->vs_next;	/* Popped */
	sal_spl(il);
	for (v = vs->vs_local.v_next; v != &vs->vs_local; ) {
	    sal_free(v->v_name);		/* Free name */
	    sal_free(v->v_value);		/* Free value */
	    v = v->v_next;		/* Next please */
	    sal_free(v->v_prev);		/* Free last goober */
	}
	sal_free(vs);			/* Finally free scope */
    } else {
	sal_spl(il);
    }
}

STATIC var_t 	*
var_find_list(var_t *vl, const char *name)
/*
 * Function: 	var_find_list
 * Purpose:	Find a variable in the given list.
 * Parameters:	vl - pointer to dummy entry at head of list.
 *		name - name of variable to find.
 * Returns:	NULL if failed, pointer to var_t if found.
 */
{
    var_t	*v;

    for (v = vl->v_next; v != vl; v = v->v_next) {
        if (v->v_name != NULL && name != NULL) {
            if (sal_strcmp(v->v_name, name) == 0) {
                return(v);
            }
        }
    }
    return(NULL);
}

STATIC var_t	*
var_find(const char *name, int local, int global)
/*
 * Function: 	var_find
 * Purpose:	Locate a variable.
 * Parameters:	name - name of variable to locate.
 *		local - TRUE if local scope should be checked.
 *		global- TRUE if global scope should be checked.
 * Returns:	Pointer to variable structure or NULL if failed.
 * Notes:	If local is TRUE, the local scope is always 
 *		checked before the global scope.
 */
{
    var_t	*v = NULL;

    if (local && var_scope_list) {
	v = var_find_list(&var_scope_list->vs_local, name);
    }
    if (global && !v) {
	v = var_find_list(&var_global_list, name);
    }	
    return(v);
}

STATIC INLINE void
var_delete(var_t *v)
/*
 * Function: 	var_delete
 * Purpose:	Delete a variable from a list
 * Parameters:	v - pointer to variable to delete.
 * Returns:	Nothing.
 */
{
    int	il = sal_splhi();			/* Don't corrupt list */

    v->v_prev->v_next = v->v_next;
    v->v_next->v_prev = v->v_prev;
    sal_spl(il);
    sal_free(v->v_name);
    sal_free(v->v_value);
    sal_free(v);
}

STATIC  void
var_set_list(var_t *vl, const char *name, char *val)
/*
 * Function:    var_set
 * Purpose: Set a variable to the specified value.
 * Parameters:  vl - pointer to dummy entry on head of var list.
 *              name - name of variable to set.
 *              val - value to set the variable to.
 * Returns: Nothing
 */
{
    var_t   *v;
    char    *old_val_addr;
    char    *v_value = sal_strdup(val);
    int     il;

    assert(v_value);

    if (NULL != (v = var_find_list(vl, name))) {
        /* If already exists on list, replace value */
        il = sal_splhi();
        old_val_addr = v->v_value;
        v->v_value = v_value;
        sal_spl(il);
        sal_free(old_val_addr);
    } else {
        /* Otherwise, create a new entry and link it on the list */
        v = (var_t *)sal_alloc(sizeof(*v), "diag_var");
        assert(v);
        v->v_name = sal_strdup(name);
        assert(v->v_name);
        il = sal_splhi();
        v->v_prev  = vl->v_prev;
        v->v_next  = vl;
        v->v_prev->v_next = v;
        v->v_next->v_prev = v;
        v->v_value = v_value;
        sal_spl(il);
    }
}

cmd_result_t
var_set_system(char *name, char *value)
/*
 * Function: 	var_set_system
 * Purpose:	Add a system variable (ie, beginning with '_').
 * Parameters:	name  - name of variable to add
 *		value - value of variable.
 * Returns:	CMD_OK - Variable set.
 *		CMD_FAIL - Invalid variable name.
 */
{
    if (*name != PARSE_VARIABLE_SYSTEM) {
	return(CMD_FAIL);
    } else {
	var_set_list(&var_global_list, name, value);
	return(CMD_OK);
    }
}

int
var_set(const char *name, char *value, int local, int system)
/*
 * Function: 	var_set
 * Purpose:	Set a variable to the specified variable.
 * Parameters:	name - name of variable to set.
 *		value - value of variable to set.
 *		local - if TRUE, scope of variable is local.
 *		system - if TRUE, first character "_", otherwise, 
 *			first character can NOT be "_".
 * Returns:	0 - success, -1 - invalid name for <system> setting.
 */
{
    if ((system && !VAR_IS_SYSTEM(name))||(!system && VAR_IS_SYSTEM(name))){
	return(-1);
    }
    if (local && var_scope_list) {
	var_set_list(&var_scope_list->vs_local, name, value);
    } else {
	var_set_list(&var_global_list, name, value);
    }
    return(0);
}

int
var_set_integer(const char *name, int value, int local, int system)
/*
 * Function: 	var_set_integer
 * Purpose:	Set a variable to an integer value.
 * Parameters:	name - name of variable.
 *		value - integer value to set to.
 *		local - TRUE if local variable.
 *		system - TRUE if system variable.
 * Returns:	0 - success, -1 - invalid name for <system> setting.
 */
{
    char	v_str[12];
    
    sal_sprintf(v_str, "%d", value);
    return(var_set(name, v_str, local, system));
}

int
var_set_hex(const char *name, int value, int local, int system)
/*
 * Function: 	var_set_integer
 * Purpose:	Set a variable to an integer value.
 * Parameters:	name - name of variable.
 *		value - integer value to set to.
 *		local - TRUE if local variable.
 *		system - TRUE if system variable.
 * Returns:	0 - success, -1 - invalid name for <system> setting.
 */
{
    char	v_str[12];
    
    sal_sprintf(v_str, "0x%04x", value);
    return(var_set(name, v_str, local, system));
}

cmd_result_t
var_unset_system(const char *name)
/*
 * Function: 	var_unset_system
 * Purpose:	Delete a system defined variable.
 * Parameters:	name - name of variable to delete.
 * Returns:	CMD_OK - success
 *		CMD_FAIL - variable not found or invalid name.
 */
{
    var_t	*v;

    if (*name != PARSE_VARIABLE_SYSTEM) {
	return(CMD_FAIL);
    } 
    v = var_find(name, FALSE, TRUE);
    if (!v) {
	return(CMD_FAIL);
    } else {
	var_delete(v);
	return(CMD_OK);
    }
}

int
var_unset(const char *name, int local, int global, int system)
/*
 * Function: 	var_unset
 * Purpose:	Unset a variable
 * Parameters:	name - name of variable to unset.
 *		local - if true, variable is looked for on local list
 *		global - if TRUE, variable is looked for on global
 *			list.
 *		system - if TRUE, system variables may be operated on.
 * Returns:	0 - OK
 *		-1 - failed due to system setting.
 */
{
    var_t	*v;

    if ((system && !VAR_IS_SYSTEM(name)) || (!system && VAR_IS_SYSTEM(name))) {
	return(-1);
    }
    v = var_find(name, local, global);
    if (!v) {
	return(-1);
    } else {
	var_delete(v);
	return(0);
    }
}

STATIC	INLINE void
var_display_entry(var_t *v)
/*
 * Function: 	var_display_entry
 * Purpose:	Print the name and value of a specific variable.
 * Parameters:	v - pointer to variable to display.
 * Returns:	Nothing.
 */
{
    cli_out("%20s = %-20s\n", v->v_name, v->v_value);
}


STATIC void
var_display_list(char *s, var_t *vl)
/*
 * Function: 	var_display_list
 * Purpose:	Display an entire list of variables.
 * Parameters:	s - String to ID list.
 *		vl - pointer to dummy entry at head of list.
 * Returns:	Nothing.
 */
{
    var_t	*v;

    cli_out("\n%20s | %-20s\n"
            "---------------------+---------------------\n",
            s, "Value");
    for (v = vl->v_next; v != vl; v = v->v_next) {
	var_display_entry(v);
    }
    cli_out("-------------------------------------------\n");
}

char var_display_usage[] = 
    "Parameters: [local|global|all]\n\t"
    "Display variables and values. If no options are supplied, all local\n\t"
    "variables in the current scope and global variables are displayed.\n\t"
    "Options have the following meanings:\n\t"
    "\tlocal\t- display only local variables in the current scope.\n\t"
    "\tglobal\t- display all global (exported) variables\n\t"
    "\tall\t- display all variables in all scopes\n"
    VAR_SYSTEM_STRING;

cmd_result_t
var_display(int u, args_t *a)
/*
 * Function: 	var_display
 * Purpose:	Display variable information.
 * Parameters:	u - unit number (ignored)
 *		a - pointer to argument list.
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE.
 */
{
    int	do_local  = FALSE,
        do_global = FALSE,
        do_all    = FALSE;

    COMPILER_REFERENCE(u);

    if (ARG_CNT(a) == 0) {		/* everything */
	do_local  = TRUE;
	do_global = TRUE;
    } else if (ARG_CNT(a) != 1) {
	return(CMD_FAIL);
    } else if (!sal_strcasecmp("local", _ARG_CUR(a))) { /* Local only */
	do_local = TRUE;
    } else if (!sal_strcasecmp("global", _ARG_CUR(a))) { /* Global only  */
	do_global = TRUE;
    } else if (!sal_strcasecmp("all", _ARG_CUR(a))) { /* All scopes */
	do_all = TRUE;
	do_global = TRUE;
    }
    if (do_all) {
	var_scope_t *vs;		/* Variable Scope*/
	int	l;			/* Level */
	char	l_str[64];
	for (l = 1, vs = var_scope_list; vs; vs = vs->vs_next, l++) {
	    sal_sprintf(l_str, "Scope Level %d", l);
	    var_display_list(l_str, &vs->vs_local);
	}
    } else if (do_local) {
        if (var_scope_list) {
            var_display_list("Local Variables", &var_scope_list->vs_local);
        }
    }
    if (do_global) {
	var_display_list("Global Variables", &var_global_list);
    }
    return(CMD_OK);
}

char var_export_usage[] = 
    "Parameters: <variable> [<value>]\n\t"
    "Set variable <variable> to a specific value <value>, and export it\n\t"
    "making it visible in all scopes. If <value> is not present, the\n\t"
    "variable is removed from the global scope.\n"
    VAR_SYSTEM_STRING;

/*ARGSUSED*/
cmd_result_t
var_export(int u, args_t *a)
/*
 * Function: 	var_export
 * Purpose:	Create a global variable visible from anywhere.
 * Parameters:	u - unit (IGNORED)
 *		a - arguments
 * Returns: CMD_OK/CMD_FAIL
 */
{
    char	*name, *value;
    int		rv = 0;

    COMPILER_REFERENCE(u);

    switch(ARG_CNT(a)) {
    case 0:
	var_display_list("Global Variables", &var_global_list);
	break;
    case 1:				/* Remove variable */
	rv = var_unset(ARG_GET(a), FALSE, TRUE, FALSE);
	break;
    case 2:				/* Add variable */
	name = ARG_GET(a);
	value = ARG_GET(a);
	if (NULL != var_find(name, TRUE, FALSE)) {
	    cli_out("%s: Warning: variable %s shadowed by local variable\n", 
                    ARG_CMD(a), name);
	}
	rv = var_set(name, value, FALSE, FALSE);
	break;
    default:				/* Error */
	return(CMD_USAGE);
    }
    return(rv ? CMD_FAIL : CMD_OK);
}

char var_local_usage[] = 
    "Parameters: <variable> [<value>]\n\t"
    "Set variable <variable> to a specific value <value>, in the local\n\t"
    "scope only. Variable in the local scope are not visible in\n\t"
    "recursive diag shells or in rc load files.\n\t"
    VAR_SYSTEM_STRING;

cmd_result_t
var_local(int u, args_t *a)
/*
 * Function: 	var_local
 * Purpose:	Set/Remove a local variable.
 * Parameters:	u - unit number (ignored)
 *		a -arguments, either a variable name only to delete, 
 *		   or a variable name and a value.
 * Returns:	CMD_OK or CMD_FAIL if variable not found for removal
 *		or attempt to set/remove a system variable.
 *		CMD_USAGE for invalid number of arguments.
 */
{
    char	*name, *value;
    int		rv = 0;

    COMPILER_REFERENCE(u);

    switch(ARG_CNT(a)) {
    case 0:
	var_display_list("Local Variables", &var_scope_list->vs_local);
	break;
    case 1:				/* Remove variable */
	rv = var_unset(ARG_GET(a), TRUE, FALSE, FALSE);
	break;
    case 2:				/* Add variable */
	name = ARG_GET(a);
	value= ARG_GET(a);
	rv = var_set(name, value, TRUE, FALSE);
	break;
    default:				/* Error */
	return(CMD_USAGE);
    }
    return(rv ? CMD_FAIL : CMD_OK);
}

char	*
var_get(const char *name)
/*
 * Function: 	var_get
 * Purpose:	Perform normal lookup for a variable in the current scope and
 *		then on the global list.
 * Parameters:	name - name of variable to lookup.
 * Returns:	NULL - not found
 *		!NULL - pointer to value of variable.
 */
{
    var_t	*v;
    
    v = var_find(name, TRUE, TRUE);
    return(v ? v->v_value : NULL);
}
