/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM core linked list functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_ll_append
@param:

Appends to TDM linked list
 */
void
tdm_ll_append(struct node *llist, unsigned short port_append)
{
	struct node *mbox,
				*tail=(struct node *) llist;
	
	if (llist) {
		while (tail->next!=NULL) {
			tail=tail->next;
		}
	}
	
	if (tail) {
		mbox=( (struct node *) TDM_ALLOC(sizeof(struct node),"node") );
		mbox->port=port_append;
		mbox->next=NULL;
		tail->next=mbox;
		tail=mbox;
		tail->next=NULL;
	}
	else {
		/* llist=mbox; */
	}
}


/**
@name: tdm_ll_insert
@param:

Inserts to TDM linked list
 */
void
tdm_ll_insert(struct node *llist, unsigned short port_insert, int idx)
{
	int i;
	struct node *mbox,
				*right=llist,
				*left=NULL;
	
	if (right==NULL) {
		tdm_ll_append(llist,port_insert);
	}
	else {
		for (i=1; i<=idx; i++) {
			if(right != NULL){
				left=right;
				right=right->next;
			}
		}
		mbox = (struct node *) TDM_ALLOC(sizeof(struct node),"node");
		mbox->port=port_insert;
		left->next=mbox;
		left=mbox;
		left->next=right;
	}
}


/**
@name: tdm_ll_delete
@param:

Delete element from TDM linked list
 */
int
tdm_ll_delete(struct node *llist, int idx)
{
	int i;
	struct node *mbox=llist,
				*last=NULL;
	
	for (i=1; i<=idx; i++) {
		last=mbox;
		mbox=mbox->next;
	}
	if (mbox==llist) {
		llist=mbox->next;
		TDM_FREE(mbox);
		return PASS;
	}
	else {
		last->next=mbox->next;
		TDM_FREE(mbox);
		return PASS;;
	}
	
	return FAIL;
	
}


/**
@name: tdm_ll_print
@param:

Prints elements from TDM linked list
 */
void
tdm_ll_print(struct node *llist)
{
	int i=0;
	struct node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		TDM_PRINT0("\n");
		TDM_PRINT0("\tTDM linked list content: \n\t\t");
		while(list!=NULL) {
			TDM_PRINT1("[%03d]",list->port);
			list=list->next;
			if ((++i)%10==0) {
				TDM_PRINT0("->\n\t\t");
			}
			else {
				TDM_PRINT0("->");
			}
		}
		TDM_PRINT0("[*]\n\n\n");
	}
	else {
		TDM_ERROR0("\nUnable to parse TDM linked list for print\n");
	}
}


/**
@name: tdm_ll_deref
@param:

Dereferences entire TDM linked list into unlinked array
 */
void
tdm_ll_deref(struct node *llist, int *tdm, int lim)
{
	int i=0;
	struct node *list = llist;
	
	if (list!=NULL) {
		list=list->next;
		while ( (list!=NULL) && (i<lim) ) {
			tdm[i++]=list->port;
			list=list->next;
		}
	}
	else {
		TDM_ERROR0("Unable to parse TDM linked list for deref\n");
	}
}


/**
@name: tdm_ll_get
@param:

Returns content of node at index
 */
int
tdm_ll_get(struct node *llist, int idx, int num_ext_ports)
{
	int i;
	struct node *list = llist;
	
	if (list!=NULL) {
		for (i=0; ((i<idx) && (list!=NULL)); i++) {
			list=list->next;
		}
	}
	
	if (list!=NULL) {
		return list->port;
	}
	else {
		return num_ext_ports;
	}
	
}


/**
@name: tdm_ll_dist
@param:

Returns distance within TDM linked list between one node and its nearest identical node
 */
int
tdm_ll_dist(struct node *llist, int idx)
{
	int i, dist=0;
	unsigned short store_port=255;
	struct node *list = llist;
	
	if (list!=NULL) {
		for (i=0; ((i<idx) && (list!=NULL)); i++) {
			list=list->next;
		}
		store_port=(list!=NULL)?(list->port):(255);
	}
	
	while (list!=NULL) {
		if ((list->port)==store_port) {
			break;
		}
		dist++;
		list=list->next;
	}
	
	return dist;
	
}


/**
@name: tdm_ll_tsc_dist
@param:

Returns distance within TDM linked list between one node and its nearest sister node
 */
int
tdm_ll_tsc_dist(struct node *llist, tdm_mod_t *_tdm, int idx)
{
	int i, dist=0, _port=_tdm->_core_data.vars_pkg.port, store_pm;
	unsigned short store_port=255;
	struct node *list = llist;
	
	if (list!=NULL) {
		for (i=0; ((i<idx) && (list!=NULL)); i++) {
			list=list->next;
		}
		store_port=(list!=NULL)?(list->port):(255);
	}
	_tdm->_core_data.vars_pkg.port=( (int) store_port );
	store_pm=_tdm->_chip_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
	while (list!=NULL) {
		list=list->next;
		dist++;
		_tdm->_core_data.vars_pkg.port=( (int) (list->port) );
		if (_tdm->_chip_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)==store_pm) {
			break;
		}
	}
	
	_tdm->_core_data.vars_pkg.port=_port;
	return dist;
}


/**
@name: tdm_ll_strip
@param:

Strips all of one tokenized value from TDM linked list
 */
void
tdm_ll_strip(struct node *llist, tdm_mod_t *_tdm, int cadence_start_idx, int *pool, int *s_idx, int token)
{
	int i, store_dist=0, safe_count=0, safe=BOOL_TRUE;
	unsigned short start_port=( (int) _tdm->_chip_data.soc_pkg.num_ext_ports );
	struct node *list = llist;
	
	if ( list!=NULL ) {
		for (i=0; ((i<cadence_start_idx) && (list!=NULL)); i++) {
			list=list->next;
		}
		start_port=(list!=NULL)?(list->port):(255);
	}
	
	while (list!=NULL) {
		if ( ((*s_idx)>cadence_start_idx) && ((list->port)==start_port) ) {
			break;
		}
		else if ( ((list->port)==token) && safe ) {
			tdm_ll_delete(llist,(*s_idx));
			(*s_idx)--;
			(*pool)++;
		}
		if ( ((list->port)!=token) ) {
			if ( safe && (tdm_ll_tsc_dist(llist,_tdm,(*s_idx))<=_tdm->_core_data.rule__prox_port_min) ) {
				/* Following region is not safe to strip */
				safe = BOOL_FALSE;
				store_dist=tdm_ll_tsc_dist(llist,_tdm,(*s_idx));
				safe_count=0;
			}
		}
		if ( (!safe) && ((++safe_count)>=store_dist) ) {
			safe = BOOL_TRUE;
		}
		list=list->next;
		(*s_idx)++;
	}

}


/**
@name: tdm_ll_retrace
@param:

Retraces all null slots in TDM linked list inside specified cadence subject to spacing rules
 */
int
tdm_ll_retrace(struct node *llist, tdm_mod_t *_tdm, int cadence_start_idx)
{
	int i, timeout, store_idx=cadence_start_idx, oversub_pool=0, starting_dist, ending_dist, relinked, num_ins=0, cadence_step;
	
	starting_dist=tdm_ll_dist(llist,cadence_start_idx);
	tdm_ll_strip(llist,_tdm,cadence_start_idx,&oversub_pool,&store_idx,_tdm->_chip_data.soc_pkg.num_ext_ports);
	if ( (tdm_ll_dist(llist,cadence_start_idx)>oversub_pool) && (oversub_pool>0) ) {
		cadence_step=tdm_ll_dist(llist,cadence_start_idx)/oversub_pool;
		for (i=(store_idx-1+num_ins); i>cadence_start_idx; i-=cadence_step) {
			if (tdm_ll_get(llist,i,_tdm->_chip_data.soc_pkg.num_ext_ports)!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				tdm_ll_insert(llist,_tdm->_chip_data.soc_pkg.num_ext_ports,(i+1));
				oversub_pool--;
			}
		}
		timeout=32000;
		while ( (oversub_pool>0) && ((--timeout)>0) ) {
			for (i=(store_idx-1+num_ins); i>cadence_start_idx; i-=cadence_step) {
				if (oversub_pool>0) {
					tdm_ll_insert(llist,_tdm->_chip_data.soc_pkg.num_ext_ports,(i+1));
					oversub_pool--;
				}
			}
		}		
	}
	else {
		for (i=(store_idx-1+num_ins); i>cadence_start_idx; i--) {
			if (tdm_ll_get(llist,i,_tdm->_chip_data.soc_pkg.num_ext_ports)!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				if ( (tdm_ll_dist(llist,i)<_tdm->_core_data.rule__same_port_min) ||
					 (tdm_ll_tsc_dist(llist,_tdm,i)<_tdm->_core_data.rule__prox_port_min) ) {
					break;
				}
			}
		}
		while (oversub_pool>0) {
			relinked=BOOL_FALSE;
			for (i=(store_idx-1+num_ins); i>cadence_start_idx; i--) {
				if (tdm_ll_get(llist,i,_tdm->_chip_data.soc_pkg.num_ext_ports)!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
					if ( (tdm_ll_dist(llist,i)<_tdm->_core_data.rule__same_port_min) ||
						 (tdm_ll_tsc_dist(llist,_tdm,i)<_tdm->_core_data.rule__prox_port_min) ) {
						tdm_ll_insert(llist,_tdm->_chip_data.soc_pkg.num_ext_ports,(i+1));
						relinked=BOOL_TRUE; oversub_pool--; num_ins++;
						break;
					}
				}
			}
			if (!relinked) {
				for (i=(cadence_start_idx+1); i<tdm_ll_dist(llist,cadence_start_idx); i++) {
					if (tdm_ll_get(llist,i,_tdm->_chip_data.soc_pkg.num_ext_ports)!=_tdm->_chip_data.soc_pkg.num_ext_ports && tdm_ll_get(llist,(i-1),_tdm->_chip_data.soc_pkg.num_ext_ports)!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
						tdm_ll_insert(llist,_tdm->_chip_data.soc_pkg.num_ext_ports,i);
						relinked=BOOL_TRUE; oversub_pool--;
						break;
					}
				}
				if (!relinked) {
					tdm_ll_insert(llist,_tdm->_chip_data.soc_pkg.num_ext_ports,(cadence_start_idx+1));
					oversub_pool--;
					continue;
				}
			}
		}		
	}

	ending_dist=tdm_ll_dist(llist,cadence_start_idx);
	
	return tdm_abs(ending_dist-starting_dist);	
}

/**
@name: tdm_ll_free
@param:

Free memory space allocated to linked-list
 */
int
tdm_ll_free(struct node *llist)
{
	struct node *temp, *list = llist;
	if(list!=NULL){
		list = list->next;
		while(list!=NULL ){
			temp = list;
			list = list->next;
			TDM_FREE(temp);
		}
	}
	llist->next = NULL;
	
	return PASS;
}
