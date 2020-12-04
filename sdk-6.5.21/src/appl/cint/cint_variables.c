/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:        cint_variables.c
 * Purpose:     CINT variable functions
 */

#include "cint_config.h"
#include "cint_variables.h"
#include "cint_porting.h"
#include "cint_debug.h"
#include "cint_interpreter.h"

/*******************************************************************************
 * 
 * Global Static Data Variables
 */

typedef struct lscope_list_s {
    struct lscope_list_s* next;     
    cint_variable_t* vlist; 
    cint_variable_t* autolist; 
} lscope_list_t; 

typedef struct vscope_list_s {
    struct vscope_list_s* next; 
    lscope_list_t* lscopes; 
} vscope_list_t; 

/*
 * Global variable scope. 
 *
 * This scope is available at all times. 
 */
static vscope_list_t _global_scope = { NULL, NULL }; 


/*
 * This is the list of all variable scopes. 
 */
static vscope_list_t* _scopes = NULL; 

/*
 * Mutex to protect all list operations
 */
sal_mutex_t cint_variables_mutex = NULL;

static void
__cint_variables_mutex_create(void)
{
    if (!cint_variables_mutex) {

        SAL_GLOBAL_LOCK;

        if (!cint_variables_mutex) {

            cint_variables_mutex = sal_mutex_create("cint_variables_mutex");
            if (!cint_variables_mutex) {
                /* should probably just assert here */
                cint_internal_error(__FILE__, __LINE__, "Failed to create cint_variables_mutex");
            }
        }

        SAL_GLOBAL_UNLOCK;
    }
}

int 
cint_variable_init(void)
{
    __cint_variables_mutex_create();

    /* Destroy any previous scopes */
    while(cint_variable_scope_pop("") != CINT_E_NOT_FOUND); 

    /* Initialize the global scope */
    _scopes = &_global_scope; 
    cint_variable_lscope_push("global"); 
    return 0; 
}

static int
__destroy_vlist(cint_variable_t* vlist, int force)
{
    cint_variable_t* v; 
    for(v = vlist; v;) {
        cint_variable_t* next = v->next;
        if (force) {
            v->flags &= ~CINT_VARIABLE_F_NODESTROY;
        }
        cint_variable_free(v); 
        v = next; 
    }

    return CINT_E_NONE; 
}


int 
cint_variable_scope_push(const char* msg)
{
    vscope_list_t* vscope = CINT_MALLOC(sizeof(*vscope)); 

    if(vscope == NULL) {
        return CINT_E_MEMORY; 
    }

    CINT_VARIABLES_LOCK;

    vscope->lscopes = NULL; 
    vscope->next = _scopes; 
    _scopes = vscope; 

    CINT_VARIABLES_UNLOCK;
    
    CINT_DTRACE(("scope_push(%s)=%p", msg, (void *)vscope)); 
    return cint_variable_lscope_push(msg); 
}

int
cint_variable_scope_pop(const char* msg)
{
    vscope_list_t* vscope;

    CINT_VARIABLES_LOCK;

    vscope = _scopes; 
    
    if(vscope) {
        while(cint_variable_lscope_pop(msg) != CINT_E_NOT_FOUND); 
        _scopes = vscope->next;     
        CINT_DTRACE(("scope_pop(%s)=%p", msg, (void *)vscope));
        if (vscope != &_global_scope) {
            CINT_FREE(vscope);
        }

        CINT_VARIABLES_UNLOCK;

        return CINT_E_NONE; 
    }   
    else {

        CINT_VARIABLES_UNLOCK;

        return CINT_E_NOT_FOUND; 
    }   
}

void *
cint_variable_scope_global_set(const char *msg)
{
    vscope_list_t *vscope;

    CINT_VARIABLES_LOCK;

    vscope = _scopes;
    _scopes = &_global_scope;

    CINT_VARIABLES_UNLOCK;

    CINT_DTRACE(("scope_global_set(%s)=%p (prev %p)", msg, (void *)_scopes, (void *)vscope)); 

    return vscope;
}

void *
cint_variable_scope_set(const char *msg, void *vscope)
{
    vscope_list_t *c_scope;

    CINT_VARIABLES_LOCK;

    c_scope = _scopes;
    _scopes = vscope;

    CINT_VARIABLES_UNLOCK;

    CINT_DTRACE(("scope_set(%s)=%p (prev %p)", msg, (void *)_scopes, (void *)c_scope)); 

    return c_scope;
}


int
cint_variable_lscope_push(const char* msg)
{
    lscope_list_t* lscope;

    if (!_scopes) {
        CINT_DTRACE(("lscope_push(%s) no scope", msg));
        return CINT_E_NONE;
    }

    lscope = CINT_MALLOC(sizeof(*lscope));

    if(lscope == NULL) {
        return CINT_E_MEMORY; 
    }

    lscope->vlist = NULL; 
    lscope->autolist = NULL; 

    CINT_VARIABLES_LOCK;

    lscope->next = _scopes->lscopes; 
    _scopes->lscopes = lscope;

    CINT_VARIABLES_UNLOCK;
    
    CINT_DTRACE(("lscope_push(%s)", msg)); 
    return CINT_E_NONE; 
}
    
int
cint_variable_lscope_pop(const char* msg)
{
    lscope_list_t* lscope;

    CINT_VARIABLES_LOCK;

    lscope = _scopes->lscopes; 
    if (!lscope) {
        CINT_VARIABLES_UNLOCK;
        return CINT_E_NOT_FOUND;
    }

    _scopes->lscopes = lscope->next; 
    __destroy_vlist(lscope->vlist, 0); 
    __destroy_vlist(lscope->autolist, 0); 

    CINT_VARIABLES_UNLOCK;

    CINT_FREE(lscope); 
    CINT_DTRACE(("lscope_pop(%s)", msg)); 
    return CINT_E_NONE;     
}        

    
cint_variable_t*
cint_variable_alloc(void)
{
    cint_variable_t* v = (cint_variable_t*)CINT_MALLOC(sizeof(cint_variable_t));
    if (v) {
        CINT_MEMSET(v, 0, sizeof(*v));
    }
    return v; 
}

int
cint_variable_free(cint_variable_t* v)
{
    if(v) {
        
        if(v->flags & CINT_VARIABLE_F_NODESTROY){
            return 0; 
        }
        /*CINT_PRINTF("free variable '%s' flags=0x%x name=%p data=%p\n", 
          v->name, v->flags, v->name, v->data);*/
        if(v->name && !(v->flags & CINT_VARIABLE_F_SNAME)) {
            CINT_FREE(v->name); 
        }
        if(v->data && !(v->flags & CINT_VARIABLE_F_SDATA)) {
            if(v->flags & CINT_VARIABLE_F_CSTRING) {
                char **sptr = (char **)v->data;
                if (sptr) {
                    CINT_FREE(*sptr);
                }
            }
            CINT_FREE(v->data); 
        }
        CINT_FREE(v); 
    }   
    return 0; 
}

/* Find a variable on a variable list */
static cint_variable_t*
_variable_find_list(cint_variable_t* vlist, const char* name)
{
    cint_variable_t* v; 
    for(v = vlist; v; v = v->next) {
        if(!CINT_STRCMP(v->name, name)) {
            return v; 
        }
    }
    return NULL; 
}

/* Find a variable within a scope on manual and auto lists */
static cint_variable_t*
_variable_find_scope(lscope_list_t* lscope, const char* name)
{
    cint_variable_t* rv = NULL;

    rv = _variable_find_list(lscope->vlist, name); 

    if (rv == NULL) {
        rv = _variable_find_list(lscope->autolist, name);
    }

    return rv;
}

/* Find a variable within a set of scopes on manual and auto lists */
static cint_variable_t*
_variable_find_scopes(lscope_list_t* lscope, const char* name)
{
    lscope_list_t* s; 
    cint_variable_t* rv = NULL;

    for (s = lscope; s; s = s->next) {
        if ((rv = _variable_find_scope(s, name)) != NULL) {
            break; 
        }
    }       

    return rv;
}

cint_variable_t* 
cint_variable_find(const char* name, int current_scope_only)
{
    cint_variable_t* rv = NULL; 

    CINT_VARIABLES_LOCK;
    
    if(_scopes && _scopes->lscopes) {
        if(current_scope_only) {
            rv = _variable_find_scope(_scopes->lscopes, name); 
            CINT_VARIABLES_UNLOCK;
            return rv; 
        } else {
            rv = _variable_find_scopes(_scopes->lscopes, name);
        }   
    }

    if(rv == NULL) {
        rv = _variable_find_scopes(_global_scope.lscopes, name); 
    }

    CINT_VARIABLES_UNLOCK;

    return rv; 
}

int
cint_variable_add(cint_variable_t* v)
{
    int is_global = 0;
    v->prev = NULL; 

    CINT_VARIABLES_LOCK;

    if(v->flags & CINT_VARIABLE_F_AUTO) {
        v->next = _scopes->lscopes->autolist; 
        _scopes->lscopes->autolist = v; 
    }   
    else {
        v->next = _scopes->lscopes->vlist; 
        _scopes->lscopes->vlist = v;
        if (_scopes == &_global_scope) {
            is_global = 1;
        }
    }   
    
    if(v->next) {
        v->next->prev = v; 
    }

    CINT_VARIABLES_UNLOCK;

    if (is_global) {
        cint_interpreter_event(cintEventGlobalVariableAdded); 
    }

    return CINT_E_NONE; 
}

int
cint_variable_rename(cint_variable_t* v, const char *name)
{
    if (name) {
        if (v->name) {
            CINT_FREE(v->name);
        }
        v->name = CINT_STRDUP(name);
    }

    return CINT_E_NONE; 
}

int
cint_variable_list_remove(cint_variable_t* v)
{
    if(v == NULL) {
        return 0; 
    }   

    CINT_VARIABLES_LOCK;
        
    if(v->prev) {
        v->prev->next = v->next; 
    }

    if (v->next) {
        v->next->prev = v->prev;
    }

    if(v == _scopes->lscopes->autolist) {
        _scopes->lscopes->autolist = v->next; 
    }       
    else if(v == _scopes->lscopes->vlist) {
        _scopes->lscopes->vlist = v->next; 
    }       

    CINT_VARIABLES_UNLOCK;
        
    return 0; 
}


int
cint_variable_auto_clear(void)
{
    CINT_VARIABLES_LOCK;

    /* Clear all autovars from the current scope */
    __destroy_vlist(_scopes->lscopes->autolist, 0); 
    _scopes->lscopes->autolist = NULL; 

    CINT_VARIABLES_UNLOCK;
    return 0;
}

int
cint_variable_auto_save(cint_variable_t* v)
{
    /*
     * Move this autovar from the auto list to variable list
     */
    if(v && (v->flags & CINT_VARIABLE_F_AUTO)) {
        
        /* Remove it from it's auto list */
        cint_variable_list_remove(v); 

        v->flags &= ~CINT_VARIABLE_F_AUTO; 
        cint_variable_add(v); 
    }   
    return 0; 
}       
        
 


int 
cint_variable_delete(const char* name) 
{
    cint_variable_t* v = cint_variable_find(name, 1); 
    if(v) {
        cint_variable_list_remove(v);
        cint_variable_free(v); 
    }
    return 0; 
}


int
cint_variable_show(const char* var)
{
    return CINT_E_NONE; 
}       


int
cint_variable_dump(void)
{
    int l, v; 
    cint_variable_t* var; 

    CINT_VARIABLES_LOCK;

    if(_scopes) {
        vscope_list_t* vscopes; 
        for(v = 0, vscopes = _scopes; vscopes; vscopes = vscopes->next, v++) {
            lscope_list_t* lscopes; 
            for(l = 0, lscopes = vscopes->lscopes; lscopes; lscopes = lscopes->next, l++) {
                for(var = lscopes->vlist; var; var = var->next) {
                    if(!CINT_STRCMP(var->name, "NULL")) continue; 
                    CINT_PRINTF("V %d L %d: %s\n", v, l, var->name); 
                }       
                for(var = lscopes->autolist; var; var = var->next) {
                    CINT_PRINTF("A %d L %d: %s\n", v, l, var->name); 
                }       
            }
        }
    }
    
    CINT_VARIABLES_UNLOCK;

    return CINT_E_NONE; 
}       



int
cint_variable_clear(void)
{
    vscope_list_t* vscopes; 
   
    __cint_variables_mutex_create();

    CINT_VARIABLES_LOCK;

    for (vscopes = _scopes; vscopes; vscopes = vscopes->next) {
        lscope_list_t* lscopes; 
        for (lscopes = vscopes->lscopes; lscopes; lscopes = lscopes->next) {
            __destroy_vlist(lscopes->vlist, 1); 
            __destroy_vlist(lscopes->autolist, 1);
            lscopes->vlist = NULL;
            lscopes->autolist = NULL;
        }
    }
    _scopes = NULL;
    
    CINT_VARIABLES_UNLOCK;

    return CINT_E_NONE; 
}
