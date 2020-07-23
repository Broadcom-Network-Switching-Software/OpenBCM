/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip operations
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
	#include <tdmv.h>
	#include <tdm_mn_vec.h>
#else
	#include <soc/tdm/core/tdm_top.h>
	#include <soc/tdm/monterey/tdm_mn_vec.h>
#endif

extern void tdm_mn_tdm_print(int *tdm_tbl, int length);
extern int chk_tdm_roundup(int m, int n);
extern int chk_tdm_rounddown(int m, int n);
extern int chk_tdm_round(int m, int n);

int tdm_core_vec_load(unsigned short **map, int bw, int port_speed, int yy, int len, int num_ext_ports);

/**
@name: tdm_mn_ovs_spacer
@param:

Reconfigures oversub TDM calendar into Flexport compatibility mode
 */
void
tdm_mn_ovs_spacer(int *ovs_tdm_tbl, int *ovs_spacing)
{
	int i, j, w=0, k=0, port=0;
  int idx=0, size=MN_OS_LLS_GRP_LEN;
  unsigned short mirror_tdm_tbl[2*MN_OS_LLS_GRP_LEN];

 for(j=0; j<(2*MN_OS_LLS_GRP_LEN); j++) { mirror_tdm_tbl[j] = MN_NUM_EXT_PORTS;}
  
   do {
     idx++;
   } while(ovs_tdm_tbl[idx] != MN_NUM_EXT_PORTS);
   size =idx;

	for (i=0; i<size; i++) {
		mirror_tdm_tbl[i]        = ovs_tdm_tbl[i]; 
		mirror_tdm_tbl[i+size]   = ovs_tdm_tbl[i];
	}
	/* Populate PGW spacing table by calculating min spacing for each non-empty slots */
	for (j=0; j<size; j++) {
		w=size+1; 
	 	if(ovs_tdm_tbl[j]!=MN_NUM_EXT_PORTS){ 
      port = ovs_tdm_tbl[j]; 
			for (i=j+1; i<2*size; i++){
				if (mirror_tdm_tbl[i]== port){ 
					w=i-j;
					break;
				}
      }
		ovs_spacing[j] = w;
	}
}
	/* Regulate spacing values with MN_OS_LLS_GRP_LEN */
	for (k=0; k<MN_OS_LLS_GRP_LEN; k++) {
		if (ovs_spacing[k] == (size+1) || ovs_tdm_tbl[k] == MN_OVSB_TOKEN){
			ovs_spacing[k] = size;
		}
	}

}

/**
OVSB TBL RECONFIG WHEN CLPORTS in 100G 
 */
void tdm_mn_reconfig_ovs_tbl(int *ovs_tdm_tbl)
{

int i,j,k,idx,size=MN_OS_LLS_GRP_LEN;
int elmnt_repeating,elmnt_repeating_cnt=0, num_elements_repeating=0, rpt_slot0, rpt_slot1, rpt_slot2;
int uniq_elements_cnt=0, sum_of_spacing=0, current_spacing=0;
int temp, spacing_idx=0, timeout=16, temp_port=130;

int spacing_tbl[MN_OS_LLS_GRP_LEN],elmnt_repeating_tbl[MN_OS_LLS_GRP_LEN],uniq_elements_tbl[MN_OS_LLS_GRP_LEN];


  for(j=0; j<MN_OS_LLS_GRP_LEN; j++) { spacing_tbl[j] = 0;}
  
  for(i =0; i<MN_OS_LLS_GRP_LEN; i++) {
    if(ovs_tdm_tbl[i] == MN_NUM_EXT_PORTS) break;
   }
  size = i;
  
  for(i=0; i<size; i++) {
    MN_TOKEN_CHECK(ovs_tdm_tbl[i]) {
  	 if(ovs_tdm_tbl[i]==ovs_tdm_tbl[i+1]) {
  		 elmnt_repeating_tbl[num_elements_repeating++] = ovs_tdm_tbl[i];
  		 elmnt_repeating = ovs_tdm_tbl[i];
  		 for(j=0; j<size; j++) {
  			 if(ovs_tdm_tbl[j] == elmnt_repeating_tbl[num_elements_repeating-1]) {
  				 for(k=j-1; k>=0; k--){
  					 ovs_tdm_tbl[k+1] = ovs_tdm_tbl[k];
  				 }
  				 elmnt_repeating_cnt++;
  			   ovs_tdm_tbl[0] = elmnt_repeating;
  			 }
  		 }
  		 i = elmnt_repeating_cnt;
  	 }
   }
 }
   
  /*  move all ovsb tokens to end */
	while((--timeout)>0) {
		for (i=0; i<(size-1); i++) {
			if (ovs_tdm_tbl[i]==MN_OVSB_TOKEN && ovs_tdm_tbl[i+1]!=MN_OVSB_TOKEN) {
        temp_port=ovs_tdm_tbl[i+1];
        ovs_tdm_tbl[i+1]=ovs_tdm_tbl[i];
        ovs_tdm_tbl[i]=temp_port;
			}
		}
	}
  for(i=elmnt_repeating_cnt; i<size; i++) {
    MN_OVS_TOKEN_CHECK(ovs_tdm_tbl[i]) {
  	uniq_elements_tbl[uniq_elements_cnt++] = ovs_tdm_tbl[i];
    }
  }
  /*if no unique elements*/
   if(uniq_elements_cnt ==0 || uniq_elements_tbl[0] ==MN_OVSB_TOKEN)  {
     if(num_elements_repeating == 3) {
       rpt_slot0=ovs_tdm_tbl[0]; rpt_slot1=ovs_tdm_tbl[10]; rpt_slot2=ovs_tdm_tbl[20];
       for(i=0; i<(size-4); i=i+3) {
          ovs_tdm_tbl[i]   = rpt_slot0;
          ovs_tdm_tbl[i+1] = rpt_slot1;
          ovs_tdm_tbl[i+2] = rpt_slot2;
       }
     }
     else if (num_elements_repeating ==2) {
       rpt_slot0=ovs_tdm_tbl[0]; rpt_slot1=ovs_tdm_tbl[10];
       for(i=0; i<(size-4); i++) {
         if(i%2==0) ovs_tdm_tbl[i] = rpt_slot0;
         else       ovs_tdm_tbl[i] = rpt_slot1;
       }
     }
   } 
   else if (elmnt_repeating_cnt!=0) {   
     do	{
    	  for(i=0; i<elmnt_repeating_cnt/num_elements_repeating; i++) {
    		spacing_tbl[i]++;
    		sum_of_spacing++;
    		   if(sum_of_spacing == uniq_elements_cnt) break;
      	   }
     } while (sum_of_spacing < uniq_elements_cnt);
   /* make the spacing table symmetric*/
    do {
     spacing_idx++;
    } while(spacing_tbl[spacing_idx] != 0);

   for(i=0; i<spacing_idx; i++) {
      temp= spacing_tbl[spacing_idx-1];   
      for(k=spacing_idx-1; k>i; k--) { spacing_tbl[k] = spacing_tbl[k-1];}
      spacing_tbl[i] = temp;
      i=i+1;
   }
     
   /* put the repeating elements at start of tbl*/ 
     for(k=0, j=0; k<num_elements_repeating; k++) {
    		ovs_tdm_tbl[j++] = elmnt_repeating_tbl[k];
     }
    
     for(i=0, current_spacing=0, idx=0 ; i<uniq_elements_cnt; ) {
    	  if(current_spacing==spacing_tbl[idx]) {
    	 	 current_spacing = 0;
    		 idx++;
    		  for(k=0; k<num_elements_repeating; k++) {
    			ovs_tdm_tbl[j++] = elmnt_repeating_tbl[k];
    	 	  }
    	  }
    	  else { ovs_tdm_tbl[j++]=uniq_elements_tbl[i]; current_spacing++; i++; }
     }
   }


}

/**
 */
void tdm_mn_reconfig_ovs_8x25(int *ovs_tdm_tbl, int port1, int port2)
{

int i, j, idx=0, size=MN_OS_LLS_GRP_LEN;

  do {
     idx++;
   } while(ovs_tdm_tbl[idx] != MN_NUM_EXT_PORTS);
  
    ovs_tdm_tbl[idx] = port1;
    ovs_tdm_tbl[idx+1] = port1;
    ovs_tdm_tbl[idx+2] = port2;
    ovs_tdm_tbl[idx+3] = port2;

  for(i=0; i<MN_OS_LLS_GRP_LEN; i++) {
    if(ovs_tdm_tbl[i] == MN_NUM_EXT_PORTS) break;
   }
  size = i;

   for(i=0;i<size;i++) {
     for(j=0;j<4;j++) {
  	   if(ovs_tdm_tbl[i]==(port1+j)) {
         ovs_tdm_tbl[i] = port1;
       }
  	   if(ovs_tdm_tbl[i]==(port2+j)) {
         ovs_tdm_tbl[i] = port2;
       }
     }
   }
 }

/**
 */
void tdm_mn_append_ovs_8x25(int *ovs_tdm_tbl, int port1, int port2, int cxx_port, enum port_speed_e cxx_speed )
{

int i, cnt=0, cnt1=0, cnt2=0;
int token1=port1, token2=port2;

   if(cxx_speed == SPEED_100G) {
     for (i=0; i<MN_OS_LLS_GRP_LEN; i++) {
       if( ovs_tdm_tbl[i]>=cxx_port && ovs_tdm_tbl[i] <(cxx_port+10) ) {
         ovs_tdm_tbl[i] = cxx_port;
         if((cnt2++)==9) break;
       }
     }
     for (i=0; i<MN_OS_LLS_GRP_LEN; i++) {
       if( ovs_tdm_tbl[i]>cxx_port && ovs_tdm_tbl[i] <(cxx_port+10) ) {
         ovs_tdm_tbl[i] = (ovs_tdm_tbl[i] % 2)? port2 : port1;
       }
     }
   }
  for (i=0; i<MN_OS_LLS_GRP_LEN; i++) {
     if(ovs_tdm_tbl[i]== port1) {
       ovs_tdm_tbl[i] = token1++;
       if((cnt++) ==3) { 
         cnt =0;
         token1=port1;
       }
     }
     if(ovs_tdm_tbl[i]== port2) {
       ovs_tdm_tbl[i] = token2++;
       if((cnt1++) ==3) { 
         cnt1 =0;
         token2=port2;
       }
     }
   }

 }

void tdm_mn_clport_ovs_scheduler_32x25(int *ovs_tdm_tbl, int port[5], enum port_speed_e speed[5], int cxx_port, enum port_speed_e cxx_speed, int clk )
{

int i, j, idx=0, size=MN_OS_LLS_GRP_LEN, k=0,index=0;
int elmnt_repeating,elmnt_repeating_cnt=0, num_elements_repeating=0, num_slots_for_25gport=3;
int elmnt_repeating_tbl[MN_OS_LLS_GRP_LEN];

  do {
     idx++;
  } while(ovs_tdm_tbl[idx] != MN_NUM_EXT_PORTS);
  
  for(i=0; i<size; i++) {
    MN_TOKEN_CHECK(ovs_tdm_tbl[i]) {
  	 if(ovs_tdm_tbl[i]==ovs_tdm_tbl[i+1]) {
  		 elmnt_repeating_tbl[num_elements_repeating++] = ovs_tdm_tbl[i];
  		 elmnt_repeating = ovs_tdm_tbl[i];
  		 for(j=0; j<size; j++) {
  			 if(ovs_tdm_tbl[j] == elmnt_repeating_tbl[num_elements_repeating-1]) {
  				 for(k=j-1; k>=0; k--){
  					 ovs_tdm_tbl[k+1] = ovs_tdm_tbl[k];
  				 }
  				 elmnt_repeating_cnt++;
  			   ovs_tdm_tbl[0] = elmnt_repeating;
  			 }
  		 }
  		 i = elmnt_repeating_cnt;
  	 }
   }
 }
  for(i=0; i < num_slots_for_25gport; i++) {
    for(j=0; j <num_elements_repeating/4; j++) {
      for(k=0; k<4; k++){
        ovs_tdm_tbl[index] = elmnt_repeating_tbl[k]+j;
        index +=1;
      }
    }
  }
}

   
/*
void tdm_mn_scheduler_ovs_4x25(int *ovs_tdm_tbl, int port)
{

int i, j, k,cnt=0,div=0, cnt1=0, size=MN_OS_LLS_GRP_LEN;
int uniq_elements_tbl[MN_OS_LLS_GRP_LEN];
int token=port;

 for(j=0; j<MN_OS_LLS_GRP_LEN; j++) { uniq_elements_tbl[j] = MN_NUM_EXT_PORTS;}

   for(i=0; i<MN_OS_LLS_GRP_LEN; i++) {
    if(ovs_tdm_tbl[i] == MN_NUM_EXT_PORTS) break;
   }
   size = i;

    for(i=0,j=0; i<size; i++) {
  	   if(ovs_tdm_tbl[i]!=port) { uniq_elements_tbl[j++] = ovs_tdm_tbl[i]; }
       else cnt++;
    }
    div=cnt;

    for(i= ((size/div)-1); i<size; i+= (size/div) ) {
      for(k=size-1; k>=i; k--){
       uniq_elements_tbl[k] = uniq_elements_tbl[k-1];
  		}
      uniq_elements_tbl[i]=port;
			if ((--cnt)<=0) {
					break;
			}
    }

   for (i=0; i<size; i++) {
     if(uniq_elements_tbl[i]== port) {
       uniq_elements_tbl[i] = token++;
       if((cnt1++) ==3) { 
         cnt1 =0;
         token=port;
       }
     }
   }
 TDM_COPY(ovs_tdm_tbl,uniq_elements_tbl,sizeof(int)*MN_OS_LLS_GRP_LEN); 
 }
*/


/**
@name: tdm_postalloc
@param:

Unless special exceptions are needed for certain port speeds, this is the standard procedure to load time vectors into TDM vector map
 */
int
mn_tdm_postalloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, short *y, int lr_idx_limit, unsigned short lr_stack[TDM_AUX_SIZE], int token, const char* speed, int num_ext_ports)
{
	int v, load_status;
	
	TDM_BIG_BAR
	TDM_PRINT1("%sG scheduling pass\n",speed);
	TDM_SML_BAR
	TDM_PRINT0("Stack contains: [ ");
	for (v=*y; v>0; v--) {
		if (lr_stack[v]!=num_ext_ports) {
			TDM_PRINT1(" %0d ",lr_stack[v]);
		}
	}
	TDM_PRINT0(" ]\n");
	TDM_SML_BAR
	while (*y > 0) {
		load_status = tdm_mn_vec_load(vector_map, freq, spd, *yy, lr_idx_limit, num_ext_ports);
		if (load_status==FAIL) {
			TDM_ERROR2("Failed to load %sG vector for port %0d\n",speed,lr_stack[*y]);
			return FAIL;
		}
		else {
			for (v=0; v<VEC_MAP_LEN; v++) {
				if (vector_map[*yy][v]==token) {
					vector_map[*yy][v]=lr_stack[*y];
				}
			}
			TDM_PRINT4("Loaded %sG port %d vector from stack pointer %d into map at index %0d\n",speed, lr_stack[*y], *y, *yy);
		}
		(*y)--; (*yy)++;
		if ((*y)>0) {
			TDM_SML_BAR
			TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", *yy, *y);
			TDM_SML_BAR
		}
		else {
			TDM_SML_BAR
			TDM_PRINT2("Done loading %sG vectors, map pointer floating at: %0d\n",speed, *yy);
			TDM_SML_BAR
		}
	}
	TDM_BIG_BAR
	
	return PASS;
	
}

int
mn_tdm_acc_alloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, int lr_idx_limit, int num_ext_ports)
{
	int load_status;
	
	TDM_BIG_BAR
	TDM_PRINT0("ACC BW scheduling pass\n");
	TDM_SML_BAR

		load_status = tdm_mn_vec_load(vector_map, freq, spd, *yy, lr_idx_limit, num_ext_ports);
		if (load_status==FAIL) {
			TDM_ERROR1("Failed to load acc_bw vectors for acc ports %0d\n",MN_ANCL_TOKEN);
			return FAIL;
		}
    (*yy)++;
			
      TDM_PRINT1("Done loading acc_bw vectors for acc ports, map pointer floating at: %0d\n",*yy);
	TDM_BIG_BAR
	
	return PASS;
	
}


int 
tdm_mn_actual_pmap( tdm_mod_t *_tdm )
{
	int i; 
	
	if (_tdm==NULL) {return FAIL;}

    for (i=1; i<=MN_NUM_PHY_PORTS; i+=MN_NUM_PM_LNS) {
      /* CLPORTS PM4x25 in dual port mode */
      if (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_50G && _tdm->_chip_data.soc_pkg.speed[i+2]==SPEED_50G) {
          _tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][2] = i+2;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][3] = i+2;
      }
      /* CLPORTS PM4x25 in tri-port mode x-xx or xx-x */
      if(i==MN_CL1_PORT || i==MN_CL2_PORT || i==MN_CL4_PORT || i==MN_CL5_PORT ){
				if ( _tdm->_chip_data.soc_pkg.speed[i]>_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i+2]==_tdm->_chip_data.soc_pkg.speed[i+3] && _tdm->_chip_data.soc_pkg.speed[i+2]!=SPEED_0 ) {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][2];
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][0];
				}
				else if ( _tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+1] && _tdm->_chip_data.soc_pkg.speed[i]<_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i]!=SPEED_0 )  {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][1];
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/MN_NUM_PM_LNS][3];
				}
      }
    }

return PASS;
}


/**
@name: tdm_mn_vmap_alloc
@param:

Populate original vmap containing all linerate ports with matched speed rates accordingly
 */
int
tdm_mn_vmap_alloc( tdm_mod_t *_tdm )
{
	int j, v, w, clk_freq, vmap_port_spd, cal_length;
	unsigned short lr1[TDM_AUX_SIZE], lr10[TDM_AUX_SIZE], lr20[TDM_AUX_SIZE], lr25[TDM_AUX_SIZE], lr40[TDM_AUX_SIZE], lr50[TDM_AUX_SIZE], lr100[TDM_AUX_SIZE];
	int speed_type_num,
        num_120g, num_100g, num_50g, num_40g, 
        num_25g, num_20g, num_10g, num_1g;
	int param_vmap_wid, param_vmap_len;
    
    param_vmap_wid = _tdm->_core_data.vmap_max_wid;
    param_vmap_len = _tdm->_core_data.vmap_max_len;
    
	/* Initialize speed group arrays for both linerate and oversub */
	for (j=0; j<(TDM_AUX_SIZE); j++) {
		lr1[j]  = MN_NUM_EXT_PORTS;
		lr10[j] = MN_NUM_EXT_PORTS;
		lr20[j] = MN_NUM_EXT_PORTS;  
		lr25[j] = MN_NUM_EXT_PORTS; 
		lr40[j] = MN_NUM_EXT_PORTS;
		lr50[j] = MN_NUM_EXT_PORTS; 
		lr100[j]= MN_NUM_EXT_PORTS;
		
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1[j]  = MN_NUM_EXT_PORTS; 
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10[j] = MN_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20[j] = MN_NUM_EXT_PORTS; 
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25[j] = MN_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40[j] = MN_NUM_EXT_PORTS; 
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50[j] = MN_NUM_EXT_PORTS; 
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100[j]= MN_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120[j]= MN_NUM_EXT_PORTS;
	}	
	clk_freq  = _tdm->_chip_data.soc_pkg.clk_freq;
	cal_length= _tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size;
	
	for (j=0; j<TDM_AUX_SIZE; j++) {
		/* Pre-allocate linerate ports with the same speed into the same linerate speed group */
		if (_tdm->_core_data.vars_pkg.lr_buffer[j] != MN_NUM_EXT_PORTS){
			if (_tdm->_core_data.vars_pkg.lr_buffer[j] <= MN_NUM_PHY_PORTS){
				if (_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1] == PORT_STATE__LINERATE   || 
					_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1] == PORT_STATE__LINERATE_HG){
					switch (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.lr_buffer[j]]) {
						case SPEED_1G:
						case SPEED_2p5G:
							tdm_core_prealloc(lr1, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8), &(_tdm->_core_data.vars_pkg.lr_1), j);
							break;
						case SPEED_10G:
						case SPEED_11G:
						case SPEED_10G_DUAL:
						case SPEED_10G_XAUI:
							tdm_core_prealloc(lr10, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), &(_tdm->_core_data.vars_pkg.lr_10), j);
							break;
						case SPEED_20G:
						case SPEED_21G:
						case SPEED_21G_DUAL:
							tdm_core_prealloc(lr20, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), &(_tdm->_core_data.vars_pkg.lr_20), j);
							break;
						case SPEED_25G:
						case SPEED_27G:
							tdm_core_prealloc(lr25, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), &(_tdm->_core_data.vars_pkg.lr_25), j);
							break;
						case SPEED_40G:
						case SPEED_42G:
						case SPEED_42G_HG2:
							tdm_core_prealloc(lr40, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), &(_tdm->_core_data.vars_pkg.lr_40), j);
							break;
						case SPEED_50G:
						case SPEED_53G:
							tdm_core_prealloc(lr50, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), &(_tdm->_core_data.vars_pkg.lr_50), j);
							break;
						case SPEED_100G:
						case SPEED_106G:
							tdm_core_prealloc(lr100, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), &(_tdm->_core_data.vars_pkg.lr_100), j);
							break;				
						default:
							TDM_ERROR2("Speed %0d port %0d was skipped in linerate preallocation\n",_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.lr_buffer[j]],_tdm->_core_data.vars_pkg.lr_buffer[j]);
							break;
					}
				}
				else{
					TDM_ERROR2("Failed to presort linerate port %0d with state mapping %0d\n", _tdm->_core_data.vars_pkg.lr_buffer[j], _tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1]);
				}
			}
			else{
				TDM_ERROR1("Unrecognized linerate port %0d in linerate preallocation\n", _tdm->_core_data.vars_pkg.lr_buffer[j]);
			}
		}
		/* Pre-allocate oversub ports with the same speed into the same ovesub speed group */
		if (_tdm->_core_data.vars_pkg.os_buffer[j] != MN_NUM_EXT_PORTS){
			if(_tdm->_core_data.vars_pkg.os_buffer[j] <= MN_NUM_PHY_PORTS){
				if (_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1] == PORT_STATE__OVERSUB   || 
					_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1] == PORT_STATE__OVERSUB_HG){
					switch (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.os_buffer[j]]) {
						case SPEED_1G:
						case SPEED_2p5G:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8), &(_tdm->_core_data.vars_pkg.os_1), j);
							break;
						case SPEED_10G:
						case SPEED_11G:
						case SPEED_10G_DUAL:
						case SPEED_10G_XAUI:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1), &(_tdm->_core_data.vars_pkg.os_10), j);
							break;
						case SPEED_20G:
						case SPEED_21G:
						case SPEED_21G_DUAL:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2), &(_tdm->_core_data.vars_pkg.os_20), j);
							break;
						case SPEED_25G:
						case SPEED_27G:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6), &(_tdm->_core_data.vars_pkg.os_25), j);
							break;	
						case SPEED_40G:
						case SPEED_42G:
						case SPEED_42G_HG2:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3), &(_tdm->_core_data.vars_pkg.os_40), j);
							break;
						case SPEED_50G:
						case SPEED_53G:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5), &(_tdm->_core_data.vars_pkg.os_50), j);
							break;
						case SPEED_100G:
						case SPEED_106G:
							tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4), &(_tdm->_core_data.vars_pkg.os_100), j);
							break;
						default:
							TDM_ERROR2("Speed %0d port %0d was skipped in oversub preallocation\n",_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.os_buffer[j]],_tdm->_core_data.vars_pkg.os_buffer[j]);
					}
				}
				else{
					TDM_ERROR2("Failed to presort oversub port %0d with state mapping %0d\n", _tdm->_core_data.vars_pkg.os_buffer[j], _tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1]);
				}
			}
			else{
				TDM_ERROR1("Unrecognized oversub port %0d in oversub preallocation\n", _tdm->_core_data.vars_pkg.lr_buffer[j]);
			}
		}
	}
	/* Set part of parameters used by core execution for vmap population */
	_tdm->_core_data.vars_pkg.os_enable = (_tdm->_core_data.vars_pkg.os_1||_tdm->_core_data.vars_pkg.os_10||_tdm->_core_data.vars_pkg.os_20||_tdm->_core_data.vars_pkg.os_25||_tdm->_core_data.vars_pkg.os_40||_tdm->_core_data.vars_pkg.os_50||_tdm->_core_data.vars_pkg.os_100||_tdm->_core_data.vars_pkg.os_120);
	_tdm->_core_data.vars_pkg.lr_enable = (_tdm->_core_data.vars_pkg.lr_1||_tdm->_core_data.vars_pkg.lr_10||_tdm->_core_data.vars_pkg.lr_20||_tdm->_core_data.vars_pkg.lr_25||_tdm->_core_data.vars_pkg.lr_40||_tdm->_core_data.vars_pkg.lr_50||_tdm->_core_data.vars_pkg.lr_100||_tdm->_core_data.vars_pkg.lr_120);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_os = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4;
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3;
    {
        num_120g= _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7;
        num_100g= _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4;
        num_50g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5;
        num_40g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3;
        num_25g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6;
        num_20g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2;
        num_10g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1;
        num_1g  = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8;
        speed_type_num = _tdm->_core_data.vars_pkg.lr_1+
                         _tdm->_core_data.vars_pkg.lr_10+
                         _tdm->_core_data.vars_pkg.lr_20+
                         _tdm->_core_data.vars_pkg.lr_25+
                         _tdm->_core_data.vars_pkg.lr_40+
                         _tdm->_core_data.vars_pkg.lr_50+
                         _tdm->_core_data.vars_pkg.lr_100+
                         _tdm->_core_data.vars_pkg.lr_120;
        TDM_PRINT2("%8s%d\n", "120G - ", num_120g);
        TDM_PRINT2("%8s%d\n", "100G - ", num_100g);
        TDM_PRINT2("%8s%d\n", " 50G - ", num_50g);
        TDM_PRINT2("%8s%d\n", " 40G - ", num_40g);
        TDM_PRINT2("%8s%d\n", " 25G - ", num_25g);
        TDM_PRINT2("%8s%d\n", " 20G - ", num_20g);
        TDM_PRINT2("%8s%d\n", " 10G - ", num_10g);
        TDM_PRINT2("%8s%d\n", "  1G - ", num_1g);
        TDM_PRINT1("\nNumber of Linerate Speed Types: %d\n\n", speed_type_num);
        TDM_BIG_BAR
    }
	/* Populate original vmap with pre-allocated linerate speed groups */
	/* 100G */
	if(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4 > 0){
		vmap_port_spd = 100;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), cal_length, lr100, TOKEN_100G, "100", MN_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 100G port vector preallocation\n");
		}
	}
	/* 50G */
	if(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5 > 0){
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			if (tdm_mn_legacy_which_tsc(lr50[v+1],_tdm->_chip_data.soc_pkg.pmap)==tdm_mn_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap) ||
				tdm_mn_legacy_which_tsc(lr50[v-1],_tdm->_chip_data.soc_pkg.pmap)==tdm_mn_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap)) {
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap=lr50[v];
				for (w=v; w>0; w--) {
					lr50[w]=lr50[w-1];
				}
				lr50[1]=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap;
			}
		}
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_mn_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap);
			for (w=0; w<4; w++) {
				if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]]!=SPEED_50G) {
					_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_50=BOOL_TRUE;
					TDM_PRINT0("50G triport detected\n");
					break;
				}
			}
		}
		vmap_port_spd = 50;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), cal_length, lr50, TOKEN_50G, "50", MN_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 50G port vector preallocation\n");
		}
	}
	/* 40G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3 > 0) {
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			if (tdm_mn_legacy_which_tsc(lr40[v+1],_tdm->_chip_data.soc_pkg.pmap)==tdm_mn_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap) ||
				tdm_mn_legacy_which_tsc(lr40[v-1],_tdm->_chip_data.soc_pkg.pmap)==tdm_mn_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap)) {
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap=lr40[v];
				for (w=v; w>0; w--) {
					lr40[w]=lr40[w-1];
				}
				lr40[1]=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap;
			}
		}
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_mn_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap);
			for (w=0; w<4; w++) {
				if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]]!=SPEED_40G && _tdm->_chip_data.soc_pkg.state[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]-1]!=PORT_STATE__DISABLED) {
					_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40=BOOL_TRUE;
					TDM_PRINT0("40G triport detected\n");
					break;
				}
			}
		}
		vmap_port_spd = 40;
		if(num_100g>0) {
			vmap_port_spd=41;
		}
		else if (num_100g==0 && num_50g>0){
			vmap_port_spd = 44;
		}
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), cal_length, lr40, TOKEN_40G, "40", MN_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 40G port vector vmap allocation\n");
		}
	}
	/* 25G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6 > 0) {
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_mn_legacy_which_tsc(lr20[v],_tdm->_chip_data.soc_pkg.pmap);
			for (w=0; w<4; w++) {
				if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]]!=SPEED_20G) {
					_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_20=BOOL_TRUE;
					TDM_PRINT0("20G triport detected\n");
					break;
				}
			}
		}
		vmap_port_spd = 25;
        if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4==0 &&
            _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5==0 &&
            _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3>0  ){
            vmap_port_spd = 26;
        }
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), cal_length, lr25, TOKEN_25G, "25", MN_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 25G port vector preallocation\n");
		}
	}
	/* 20G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2 > 0) {
		vmap_port_spd = 20;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), cal_length, lr20, TOKEN_20G, "20", MN_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 20G port vector preallocation\n");
		}
	}
	/* 10G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1 > 0) {
		vmap_port_spd = 10;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), cal_length, lr10, TOKEN_10G, "10", MN_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 10G port vector preallocation\n");
		}
	}
	/* 1G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8 > 0) {
		vmap_port_spd = 1;
		if (tdm_core_postalloc_vmap(_tdm->_core_data.vmap, clk_freq, vmap_port_spd, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8), cal_length, lr1, TOKEN_1G, "1", MN_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 1G port vector preallocation\n");
		}
	}
	
	/* 
	return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER]( _tdm ) ); */
	return (tdm_mn_vbs_scheduler(_tdm));
}


/**
@name: tdm_mn_acc_alloc_new
@param:
 */
int
tdm_mn_acc_alloc_new( tdm_mod_t *_tdm )
{
	int i,j, idx, idx_opt, idx_up, idx_dn, dist_opt=0,
	    empty_slot_cnt=0, empty_slot_token, acc_slot_cnt=0, acc_slot_token, 
	    acc_slot_num, result=PASS;
	    
	int *tdm_pipe_main, tdm_pipe_main_len;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_pipe_main_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	empty_slot_token  = _tdm->_chip_data.soc_pkg.num_ext_ports;
	acc_slot_num      = _tdm->_chip_data.soc_pkg.tvec_size;
	acc_slot_token    = MN_ANCL_TOKEN;
	
	/* check available slots */
	for (i=0; i<tdm_pipe_main_len; i++){
		if (tdm_pipe_main[i] == empty_slot_token){
			empty_slot_cnt++;
		}
	}
	if (empty_slot_cnt<acc_slot_num){
		TDM_ERROR2("FAILED in ACC bandwidth allocation, Not enough available slots for ACC, Slot_Required %d, Slot_Available %d\n",acc_slot_cnt, empty_slot_cnt);
		result = FAIL;
	}
	/* allocate acc slots */
	if (acc_slot_num>0 && result==PASS){
		dist_opt = tdm_pipe_main_len/acc_slot_num;
		for (i=0; i<acc_slot_num; i++){
			idx_opt = i*dist_opt;
			if(idx_opt<tdm_pipe_main_len){
				idx    = tdm_pipe_main_len;
				idx_up = tdm_pipe_main_len;
				idx_dn = tdm_pipe_main_len;
				for (j=idx_opt; j>0; j--){
					if(tdm_pipe_main[j]==empty_slot_token){
						idx_up = j;
						break;
					}
				}
				for (j=idx_opt; j<tdm_pipe_main_len; j++){
					if(tdm_pipe_main[j]==empty_slot_token){
						idx_dn = j;
						break;
					}
				}
				if (idx_up<tdm_pipe_main_len || idx_dn<tdm_pipe_main_len){
					if (idx_up==tdm_pipe_main_len){
						idx = idx_dn;
					}
					else if (idx_dn==tdm_pipe_main_len){
						idx = idx_up;
					}
					else{
						idx = ((idx_opt-idx_up) < (idx_dn-idx_opt))? (idx_up): (idx_dn);
					}
				}
				
				if (idx < tdm_pipe_main_len){
					tdm_pipe_main[idx] = acc_slot_token;
				}
			}
		}
	}
	/* check acc slots */
	if (acc_slot_num>0 && result==PASS){
		for (i=0; i<tdm_pipe_main_len; i++){
			if (tdm_pipe_main[i] == acc_slot_token){
				acc_slot_cnt++;
			}
		}
		if(acc_slot_cnt != acc_slot_num){
			result = FAIL;
			TDM_ERROR2("FAILED in ACC bandwidth allocation, Slot_Required %d, Slot_Allocated %d\n",acc_slot_num, acc_slot_cnt);
		}
	}
	
	return result;
}

/**
@name: tdm_mn_vbs_scheduler
@param:
 */
int
tdm_mn_vbs_scheduler( tdm_mod_t *_tdm )
{
	int i, *tdm_pipe_main, tdm_cal_length, cnt=0; 
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_cal_length = _tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size;
	
	/* Allocate slots for all linerate ports among entire TDM calendar */
	if (_tdm->_core_data.vars_pkg.lr_enable) {
		if(FAIL==tdm_core_vbs_scheduler_lr(_tdm)){
			TDM_ERROR0("FAILED in slot allocation for LINERATE ports\n");
		}
	}
	/* Allocate ACC slots */
	if (_tdm->_core_data.vars_pkg.os_enable || _tdm->_core_data.vars_pkg.lr_enable) {
		if(FAIL==tdm_mn_acc_alloc_new(_tdm)){
			TDM_ERROR0("FAILED in slot allocation for ANCILLARY bandwidth\n");
		}
	}
	/* Allocate OVSB and/or IDLE slots */
	if (_tdm->_core_data.vars_pkg.os_enable || _tdm->_core_data.vars_pkg.lr_enable) {
		/* OVSB: Place the OVS token for all unallocated slots */
		if (_tdm->_core_data.vars_pkg.os_enable && (!_tdm->_core_data.vars_pkg.lr_enable)) {
			for (i=0; i<tdm_cal_length; i++) {
				if (tdm_pipe_main[i]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
					tdm_pipe_main[i] = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
				}
			}
		}
		/* IDLE: Place IDLE tokens for all unallocated slots */
		else {
		  for (i=0; i<tdm_cal_length; i++) {
				if (tdm_pipe_main[i]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
 				  if (_tdm->_core_data.vars_pkg.os_enable) {
					  tdm_pipe_main[i] = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
				  }
				  else {
					  if ((cnt++)%2==0) {
						  tdm_pipe_main[i] = _tdm->_chip_data.soc_pkg.soc_vars.idl1_token;
					  }
					  else {
						  tdm_pipe_main[i] = _tdm->_chip_data.soc_pkg.soc_vars.idl2_token;
					  }
				  }
			  }
		  }
    }
	}
	
	/* OVS grouping */
	if (_tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER_OVS](_tdm)==FAIL) {
		return (TDM_EXEC_CORE_SIZE+1);
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__FILTER](_tdm) );
}

/**     
 * @name: tdm_mn_cal_pgw_bw
 * @param:
 *                     
 * Calculate PGW IO BW
 */
void
tdm_mn_cal_pgw_bw( tdm_mod_t *_tdm, int *pgw_io_bw, int start_offset, int *high_speed_port_slots )
{
int p, high_speed_100G_port_found=0, high_speed_50G_port_found =0;
  for (p = (start_offset*(MN_NUM_PHY_PORTS/MN_NUM_QUAD)); p < ( (start_offset+1) *(MN_NUM_PHY_PORTS/MN_NUM_QUAD)); p++) {
   switch (_tdm->_chip_data.soc_pkg.speed[p+1]) {
     case SPEED_1G:
     	*pgw_io_bw = *pgw_io_bw + 1;
     	break;
     case SPEED_2G:
     	*pgw_io_bw = *pgw_io_bw + 3;
     	break;
     case SPEED_2p5G:
     	*pgw_io_bw = *pgw_io_bw + 3;
     	break;
     case SPEED_5G:
     	*pgw_io_bw = *pgw_io_bw + 5;
     	break;
     case SPEED_10G:
     case SPEED_10G_DUAL:
     case SPEED_10G_XAUI:
     case SPEED_11G:
     	*pgw_io_bw = *pgw_io_bw + 10;
     	break;
     case SPEED_12p5G:
     	*pgw_io_bw = *pgw_io_bw + 13;
     	break;
     case SPEED_20G:
     case SPEED_21G:
     case SPEED_21G_DUAL:
     	*pgw_io_bw = *pgw_io_bw + 20;
     	break;
     case SPEED_25G:
     case SPEED_27G:
     	*pgw_io_bw = *pgw_io_bw + 25;
     	break;
     case SPEED_40G:
     case SPEED_42G:
     case SPEED_42G_HG2:
     	*pgw_io_bw = *pgw_io_bw + 40;
     	break;
     case SPEED_50G:
     case SPEED_53G:
     	*pgw_io_bw = *pgw_io_bw + 50;
        high_speed_50G_port_found = 1;
     	break;
     case SPEED_100G:
     case SPEED_106G:
     	*pgw_io_bw = *pgw_io_bw +  100;
        high_speed_100G_port_found = 1;
     	break;
     default:
     	break;
    }
    _tdm->_chip_data.soc_pkg.soc_vars.mn.pipe_start= (start_offset*(MN_NUM_PHY_PORTS/MN_NUM_QUAD))+1;
    _tdm->_chip_data.soc_pkg.soc_vars.mn.pipe_end  = ( (start_offset+1) *(MN_NUM_PHY_PORTS/MN_NUM_QUAD) );
  }
  if(high_speed_100G_port_found == 1) {
    *high_speed_port_slots = ( (100/(TDM_SLOT_UNIT_5G/1000)) * 2);
  } else if(high_speed_50G_port_found == 1) {
    *high_speed_port_slots = ( (50/(TDM_SLOT_UNIT_5G/1000)) * 2);
  } else {
    *high_speed_port_slots = 0;
  }
  high_speed_50G_port_found = 0;
  high_speed_100G_port_found = 0;
}

void
tdm_mn_chip2core_init(tdm_mod_t *_tdm)
{
    int index, token_empty;

    token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
        TDM_MSET(_tdm->_core_data.vmap[index], token_empty,
                 _tdm->_core_data.vmap_max_len);
    }

    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z11=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z22=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z33=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z44=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z55=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z66=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z77=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z88=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z99=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.zaa=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.zbb=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.zcc=0;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt=0;

    _tdm->_core_data.vars_pkg.lr_1=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.lr_10=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.lr_20=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.lr_25=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.lr_40=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.lr_50=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.lr_100=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.lr_120=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.lr_enable=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.os_1=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.os_10=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.os_20=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.os_25=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.os_40=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.os_50=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.os_100=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.os_120=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.os_enable=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.HG4X106G_3X40G=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.HG1X106G_xX40G_OVERSUB=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.HGXx120G_Xx100G=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr=0;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_os=0;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g=0;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g=0;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_50=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_20=BOOL_FALSE;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap=token_empty;

    TDM_MSET(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1,  token_empty, TDM_AUX_SIZE);
    TDM_MSET(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10, token_empty, TDM_AUX_SIZE);
    TDM_MSET(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20, token_empty, TDM_AUX_SIZE);
    TDM_MSET(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25, token_empty, TDM_AUX_SIZE);
    TDM_MSET(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40, token_empty, TDM_AUX_SIZE);
    TDM_MSET(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50, token_empty, TDM_AUX_SIZE);
    TDM_MSET(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100,token_empty, TDM_AUX_SIZE);
    TDM_MSET(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120,token_empty, TDM_AUX_SIZE);

    _tdm->_core_data.vars_pkg.m_tdm_pick_vec.triport_priority=BOOL_FALSE;

    /* NGTDM */
    _tdm->_core_data.vars_pkg.pipe_info.lr_en = 0;
    _tdm->_core_data.vars_pkg.pipe_info.os_en = 0;
    _tdm->_core_data.vars_pkg.pipe_info.num_lr_prt = 0;
    _tdm->_core_data.vars_pkg.pipe_info.num_os_prt = 0;
    TDM_MSET(_tdm->_core_data.vars_pkg.pipe_info.lr_spd_en, 0, MAX_SPEED_TYPES);
    TDM_MSET(_tdm->_core_data.vars_pkg.pipe_info.os_spd_en, 0, MAX_SPEED_TYPES);
    TDM_MSET(_tdm->_core_data.vars_pkg.pipe_info.lr_prt_cnt, 0, MAX_SPEED_TYPES);
    TDM_MSET(_tdm->_core_data.vars_pkg.pipe_info.os_prt_cnt, 0, MAX_SPEED_TYPES);
    for (index = 0; index < MAX_SPEED_TYPES; index++) {
        TDM_MSET(_tdm->_core_data.vars_pkg.pipe_info.lr_prt[index], token_empty, TDM_AUX_SIZE);
        TDM_MSET(_tdm->_core_data.vars_pkg.pipe_info.os_prt[index], token_empty, TDM_AUX_SIZE);
    }
}


/**     
 * @name: tdm_mn_proc_cal_lr
 * @param:
 *                     
 *                     Allocate linerate slots with matched speed rates accordingly
 */
int
tdm_mn_proc_cal_lr( tdm_mod_t *_tdm )
{
    int result=PASS, i=0, idx1=0, j=0, acc=0, higig_mgmt=BOOL_FALSE, lr_idx_limit=0, loop=0, k=0;
    int pgw_tdm_tbl0[160], pgw_tdm_tbl1[160];
    int pgw_io_bw=0, actual_pgw_slots_required = 0;
    int *high_speed_port_slots;

    for(idx1=0; idx1<160; idx1++){
      pgw_tdm_tbl0[idx1] = MN_NUM_EXT_PORTS;
      pgw_tdm_tbl1[idx1] = MN_NUM_EXT_PORTS;
    }
    _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = 0;
    _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = 0;
    for(i =0; i<MN_LR_LLS_LEN; i++) {
      if(_tdm->_chip_data.cal_1.cal_main[i] == MN_NUM_EXT_PORTS) break;
    }
    high_speed_port_slots = NULL;	
	high_speed_port_slots =(int *) TDM_ALLOC(sizeof(int), "high_speed_port_slots");
	if (high_speed_port_slots==NULL) {return FAIL;}
    for(i=0; i < 3; i++){
      /*tdm_mn_chip2core_init(_tdm);*/
      pgw_io_bw = 0;
      switch(i) {
        case 0:
          if(_tdm->_chip_data.soc_pkg.clk_freq != 861 && _tdm->_chip_data.soc_pkg.clk_freq != 862
             && _tdm->_chip_data.soc_pkg.soc_vars.mn.os_config == 0
            ){
            
            pgw_io_bw = 0;
            tdm_mn_cal_pgw_bw(_tdm, &pgw_io_bw, i, high_speed_port_slots);
            switch (_tdm->_chip_data.soc_pkg.clk_freq) {
            	case 815: case 817:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                     lr_idx_limit=140;
                     actual_pgw_slots_required = lr_idx_limit;
                     if(actual_pgw_slots_required == *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                       lr_idx_limit = *high_speed_port_slots;
                     }
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                     break;
                    case 800: case 720: case 460:
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000))/2;
                     actual_pgw_slots_required = lr_idx_limit;
                     if(actual_pgw_slots_required == *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                       lr_idx_limit = *high_speed_port_slots;
                     }
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                     break;
                   }
                    break;
            	case 450:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 460: case 360: case 200: case 400:
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000))/2;
                     actual_pgw_slots_required = lr_idx_limit;
                     if(actual_pgw_slots_required == *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                       lr_idx_limit = *high_speed_port_slots;
                     }
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                     break;
                   }
                    break;
            	case 666:
                   lr_idx_limit=80;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
	 	           _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                break;
                case 667:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      lr_idx_limit=56;
                      actual_pgw_slots_required = lr_idx_limit;
                      if(actual_pgw_slots_required == *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                        lr_idx_limit = *high_speed_port_slots;
                      }
	 	              _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                    break;
                    default:
                      lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000))/2;
                      actual_pgw_slots_required = lr_idx_limit;
                      if(actual_pgw_slots_required == *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                        lr_idx_limit = *high_speed_port_slots;
                      }
                      _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                    break;
                  }
                break;
            	case 816:
                   lr_idx_limit=132;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                break;
            	case 818: 
                   lr_idx_limit=160;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                    break;
            	case 819: 
                   lr_idx_limit=128;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                break;
            	case 820:
                   lr_idx_limit=152;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                break;
            	case 550:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      lr_idx_limit=84;
                      actual_pgw_slots_required = lr_idx_limit;
                      if(actual_pgw_slots_required == *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                        lr_idx_limit = *high_speed_port_slots;
                      }
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                     break;
                    case 580: case 460: case 280: case 400:
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000))/2;
                     actual_pgw_slots_required = lr_idx_limit;
                     if(actual_pgw_slots_required == *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                       lr_idx_limit = *high_speed_port_slots;
                     }
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = lr_idx_limit;
                     break;
                   }
                break;
            	default : break;
            }
          if(_tdm->_chip_data.soc_pkg.clk_freq != 861 && _tdm->_chip_data.soc_pkg.clk_freq != 862){
            for (idx1=0; idx1<TDM_AUX_SIZE; idx1++) {
            	_tdm->_core_data.vars_pkg.lr_buffer[idx1]=MN_NUM_EXT_PORTS;
            	_tdm->_core_data.vars_pkg.os_buffer[idx1]=MN_NUM_EXT_PORTS;
            }
            /* higig_mgmt */
            if ( (_tdm->_chip_data.soc_pkg.soc_vars.mn.pm_encap_type[MN_NUM_PHY_PM]==PM_ENCAP__HIGIG2 /* && (pipe==1||pipe==2)*/) && 
                 ((_tdm->_core_data.vars_pkg.lr_buffer[0]!=MN_NUM_EXT_PORTS && _tdm->_chip_data.soc_pkg.clk_freq>=MIN_HG_FREQ) ||
                 (_tdm->_core_data.vars_pkg.lr_buffer[0]==MN_NUM_EXT_PORTS)) ) {
            	higig_mgmt=BOOL_TRUE;
            }
            pgw_io_bw = 0.0;
            tdm_mn_cal_pgw_bw(_tdm, &pgw_io_bw, i, high_speed_port_slots);
            _tdm->_chip_data.soc_pkg.soc_vars.mn.mntry_pgw0_io_bw = pgw_io_bw;
            _tdm->_chip_data.soc_pkg.tvec_size   = 0;
            /*_tdm->_chip_data.soc_pkg.lr_idx_limit= (int) (pgw_io_bw/MN_PGW_PER_SLOT_IO_BW);*/
            _tdm->_chip_data.soc_pkg.lr_idx_limit= lr_idx_limit;
            _tdm->_core_data.vars_pkg.cal_id=0;
            for (idx1=0; idx1<TDM_AUX_SIZE; idx1++) {
            	_tdm->_core_data.vars_pkg.lr_buffer[idx1]=MN_NUM_EXT_PORTS;
              _tdm->_core_data.vars_pkg.os_buffer[idx1]=MN_NUM_EXT_PORTS;
            }
            for (idx1=1; idx1<=32; idx1++) {
            	if((_tdm->_chip_data.soc_pkg.state[idx1-1] ==  PORT_STATE__LINERATE   ||
                 _tdm->_chip_data.soc_pkg.state[idx1-1] ==  PORT_STATE__LINERATE_HG) &&
                 (_tdm->_chip_data.soc_pkg.speed[idx1] > SPEED_0)
                 ){
            		_tdm->_core_data.vars_pkg.lr_buffer[j++]= (idx1);
              }
            }
            /* allocate Linerate slots: vmap pre-allocation */
            if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_PREALLOC](_tdm)!=PASS){
                result = FAIL;
            }
            /* allocate Linerate slots: vmap allocation */
            else if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_ALLOC](_tdm)!=PASS){
                result = FAIL;
            }
            for(loop =0; loop<MN_LR_LLS_LEN; loop++) {
                pgw_tdm_tbl0[loop] = _tdm->_chip_data.cal_0.cal_main[loop];
            }
            /*_tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len = loop;*/
            TDM_PRINT0("\n==================== Monterey PGW-0 TDM Calendar  ==================\n");
            tdm_mn_tdm_print(_tdm->_chip_data.cal_0.cal_main, MN_LR_LLS_LEN);
            TDM_PRINT0("\n==================== Monterey PGW-0 TDM Calendar  ==================\n");
          }
          break;
        }
        case 1:
          if(_tdm->_chip_data.soc_pkg.clk_freq != 861 && _tdm->_chip_data.soc_pkg.clk_freq != 862
              && _tdm->_chip_data.soc_pkg.soc_vars.mn.os_config == 0
            ){
            pgw_io_bw = 0;
            tdm_mn_cal_pgw_bw(_tdm, &pgw_io_bw, i, high_speed_port_slots);
            switch (_tdm->_chip_data.soc_pkg.clk_freq) {
            	case 815: 
                   lr_idx_limit=152;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                    break;
            	case 666:
                   lr_idx_limit=44;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
	 	           _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                break;
            	case 667:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
            	    case 0: 
                      lr_idx_limit=80;
	 	              _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                    break;
                    default:
                      lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000))/2;
                      actual_pgw_slots_required = lr_idx_limit;
                      if(actual_pgw_slots_required == *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                        lr_idx_limit = *high_speed_port_slots;
                      }
	 	              _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                    break;
                  }
                break;
            	case 816:
                   lr_idx_limit=136;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                break;
            	case 450:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 460: case 360: case 200: case 400:
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000))/2;
                     actual_pgw_slots_required = lr_idx_limit;
                     if(actual_pgw_slots_required == *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                       lr_idx_limit = *high_speed_port_slots;
                     }
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                     break;
                   }
                break;
            	case 817:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      lr_idx_limit=149;
                      actual_pgw_slots_required = lr_idx_limit;
                      if(actual_pgw_slots_required == *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                        lr_idx_limit = actual_pgw_slots_required;
                      } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                        lr_idx_limit = *high_speed_port_slots;
                      }
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                     break;
                    case 800: case 720: case 460:
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000))/2;
                     actual_pgw_slots_required = lr_idx_limit;
                     if(actual_pgw_slots_required == *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                       lr_idx_limit = *high_speed_port_slots;
                     }
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                     break;
                   }
                break;
            	case 818: 
                   lr_idx_limit=160;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                    break;
            	case 819: 
                   lr_idx_limit=128;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                break;
            	case 820:
                   lr_idx_limit=152;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                break;
            	case 861: case 862:
                   lr_idx_limit=48;
                   actual_pgw_slots_required = lr_idx_limit;
                   if(actual_pgw_slots_required == *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                     lr_idx_limit = actual_pgw_slots_required;
                   } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                     lr_idx_limit = *high_speed_port_slots;
                   }
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                break;
            	case 550:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      lr_idx_limit=80;
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                     break;
                    case 580: case 460: case 280: case 400:
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000))/2;
                     actual_pgw_slots_required = lr_idx_limit;
                     if(actual_pgw_slots_required == *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (actual_pgw_slots_required > *high_speed_port_slots) {
                       lr_idx_limit = actual_pgw_slots_required;
                     } else if (*high_speed_port_slots > actual_pgw_slots_required) {
                       lr_idx_limit = *high_speed_port_slots;
                     }
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len = lr_idx_limit;
                     break;
                   }
                break;
            	default : break;
            }
            if(_tdm->_chip_data.soc_pkg.clk_freq != 861 && _tdm->_chip_data.soc_pkg.clk_freq != 862){
              for (idx1=0; idx1<TDM_AUX_SIZE; idx1++) {
              	_tdm->_core_data.vars_pkg.lr_buffer[idx1]=MN_NUM_EXT_PORTS;
              	_tdm->_core_data.vars_pkg.os_buffer[idx1]=MN_NUM_EXT_PORTS;
              }
              /* higig_mgmt */
              if ( (_tdm->_chip_data.soc_pkg.soc_vars.mn.pm_encap_type[MN_NUM_PHY_PM]==PM_ENCAP__HIGIG2 /* && (pipe==1||pipe==2)*/) && 
                   ((_tdm->_core_data.vars_pkg.lr_buffer[0]!=MN_NUM_EXT_PORTS && _tdm->_chip_data.soc_pkg.clk_freq>=MIN_HG_FREQ) || (_tdm->_core_data.vars_pkg.lr_buffer[0]==MN_NUM_EXT_PORTS)) ) {
              	higig_mgmt=BOOL_TRUE;
              }
              pgw_io_bw = 0.0;
              tdm_mn_cal_pgw_bw(_tdm, &pgw_io_bw, i, high_speed_port_slots);
              _tdm->_chip_data.soc_pkg.soc_vars.mn.mntry_pgw1_io_bw = pgw_io_bw;
              _tdm->_core_data.vars_pkg.cal_id=1;
              _tdm->_chip_data.soc_pkg.tvec_size   = 0;
              /*_tdm->_chip_data.soc_pkg.lr_idx_limit= (int) (pgw_io_bw/MN_PGW_PER_SLOT_IO_BW);*/
              _tdm->_chip_data.soc_pkg.lr_idx_limit= lr_idx_limit;
              for (idx1=0; idx1<TDM_AUX_SIZE; idx1++) {
                _tdm->_core_data.vars_pkg.os_buffer[idx1]=MN_NUM_EXT_PORTS;
              }
              j=0;
              for (idx1=(i*(MN_NUM_PHY_PORTS/MN_NUM_QUAD))+1; idx1<=( (i+1) *(MN_NUM_PHY_PORTS/MN_NUM_QUAD) ); idx1++) {
              	if((_tdm->_chip_data.soc_pkg.state[(idx1-1)] ==  PORT_STATE__LINERATE   ||
                   _tdm->_chip_data.soc_pkg.state[(idx1-1)] ==  PORT_STATE__LINERATE_HG) &&
                   (_tdm->_chip_data.soc_pkg.speed[(idx1)] > SPEED_0)
                   ){
              		_tdm->_core_data.vars_pkg.lr_buffer[j++]= (idx1);
                }
              }
              /* allocate Linerate slots: vmap pre-allocation */
              if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_PREALLOC](_tdm)!=PASS){
                  /*result = FAIL;*/
              }
              /* allocate Linerate slots: vmap allocation */
              else if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_ALLOC](_tdm)!=PASS){
                  /*result = FAIL;*/
              }
              for(loop =0; loop<MN_LR_LLS_LEN; loop++) {
                  pgw_tdm_tbl1[loop] = _tdm->_chip_data.cal_1.cal_main[loop];
              }
              TDM_PRINT0("\n==================== Monterey PGW-1 TDM Calendar  ==================\n");
              tdm_mn_tdm_print(_tdm->_chip_data.cal_1.cal_main, MN_LR_LLS_LEN);
              TDM_PRINT0("\n==================== Monterey PGW-1 TDM Calendar  ==================\n");
            }
            break;
        }
        case 2:
          _tdm->_core_data.rule__prox_port_min = MN_MIN_SPACING_SISTER_PORT;
          if ( (tdm_mn_check_ethernet(_tdm)) && (higig_mgmt==BOOL_FALSE)){
          	switch (_tdm->_chip_data.soc_pkg.clk_freq) {
          		case 815: 
                   acc=(MN_ACC_PORT_NUM+15);
                   lr_idx_limit=(MN_LEN_815MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_815MHZ_EN;
                break;
          		case 666: 
                   acc=(16);
                   lr_idx_limit=(MN_LEN_666MHZ_EN-acc);
	 	           _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_666MHZ_EN;
                break;
          		case 667: 
                   switch(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                     case 0:
                       acc=(16);
                       lr_idx_limit=(MN_LEN_667MHZ_EN-acc);
	 	               _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                       _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_667MHZ_EN;
                     break;
                     case 720: case 580:
                       acc = 15;
                       lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
	 	               _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                       _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit + acc;
                     break;
                     case 360: case 400:
                       acc = 14;
                       lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
	 	               _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                       _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit + acc;
                     break;
                     case 380: case 420:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 12;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 4;
                     }
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                   }
                break;
          		case 816:
                   acc=(MN_ACC_PORT_NUM);
                   lr_idx_limit=(MN_LEN_816MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_816MHZ_EN;
                break;
                case 450:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 460: case 360: case 400:
                     acc = 15;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                    case 200:
                     acc = 14;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                     case 235: case 275:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 12;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 4;
                     }
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                   }
                break;
          		case 817:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      acc=(MN_ACC_PORT_NUM-1);
                      lr_idx_limit=(MN_LEN_817MHZ_EN-acc);
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_817MHZ_EN;
                     break;
                    case 800: case 720:
                     acc = 15;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                    case 460:
                     acc = 14;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                     case 475: case 515:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 12;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 4;
                     }
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                   }
                break;
          		case 818: 
                   acc=(MN_ACC_PORT_NUM);
                   lr_idx_limit=(MN_LEN_818MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_818MHZ_EN;
                    break;
          		case 819: 
                   acc=(MN_ACC_PORT_NUM+13);
                   lr_idx_limit=(MN_LEN_819MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_819MHZ_EN;
                break;
          		case 820:
                   acc=(MN_ACC_PORT_NUM+17);
                lr_idx_limit=(MN_LEN_820MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_820MHZ_EN;
                break;
          		case 861: case 862:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      acc=(MN_ACC_PORT_NUM-9);
                      lr_idx_limit=(MN_LEN_861MHZ_EN-acc);
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_861MHZ_EN;
                     break;
                    case 800:
                      acc=(MN_ACC_PORT_NUM-6);
                      _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_2P5G; 
                      lr_idx_limit=(MN_LEN_862MHZ_EN-acc);
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_862MHZ_EN;
                     break;
                    case 548: case 508:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 24;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 8;
                     }
                     lr_idx_limit= (((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw*1000)/TDM_SLOT_UNIT_2P5G));
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                  
                   }
                break;
          		case 550:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      acc=(MN_ACC_PORT_NUM+15);
                      lr_idx_limit=(MN_LEN_550MHZ_EN-acc);
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_550MHZ_EN;
                     break;
                    case 580: case 460: case 400:
                     acc = 15;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                    case 280:
                     acc = 14;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                    /*case 400:
                     acc = 8;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;*/
                    case 300: case 340:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 12;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 4;
                     }
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                   }
                break;
          		default : break;
          	}
          } else {
          	switch (_tdm->_chip_data.soc_pkg.clk_freq) {
          		case 815: 
                   acc=(MN_ACC_PORT_NUM+15);
                   lr_idx_limit=(MN_LEN_815MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_815MHZ_EN;
                break;
          		case 666: 
                   acc=(16);
                   lr_idx_limit=(MN_LEN_666MHZ_EN-acc);
	 	           _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_666MHZ_EN;
                break;
          		case 667: 
                   switch(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                     case 0:
                       acc=(16);
                       lr_idx_limit=(MN_LEN_667MHZ_EN-acc);
	 	               _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                       _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_667MHZ_EN;
                     break;
                     case 720: case 580:
                       acc = 15;
                       lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
	 	               _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                       _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit + acc;
                     break;
                     case 360: case 400:
                       acc = 14;
                       lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
	 	               _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                       _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit + acc;
                     break;
                     case 380: case 420:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 12;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 4;
                     }
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                   }
                break;
          		case 816:
                   acc=(MN_ACC_PORT_NUM);
                   lr_idx_limit=(MN_LEN_816MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_816MHZ_EN;
                break;
                case 450:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 460: case 360:
                     acc = 15;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                    case 200:
                     acc = 14;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                     case 235: case 275:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 12;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 4;
                     }
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                   }
                break;
          		case 817:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      acc=(MN_ACC_PORT_NUM-1);
                      lr_idx_limit=(MN_LEN_817MHZ_EN-acc);
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_817MHZ_EN;
                     break;
                    case 800: case 720:
                     acc = 15;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                    case 460:
                     acc = 14;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                     case 475: case 515:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 12;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 4;
                     }
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                   }
                break;
          		case 818: 
                   acc=(MN_ACC_PORT_NUM);
                   lr_idx_limit=(MN_LEN_818MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_818MHZ_EN;
                    break;
          		case 819: 
                   acc=(MN_ACC_PORT_NUM+13);
                   lr_idx_limit=(MN_LEN_819MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_819MHZ_EN;
                break;
          		case 820:
                   acc=(MN_ACC_PORT_NUM+17);
                lr_idx_limit=(MN_LEN_820MHZ_EN-acc);
                   _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_820MHZ_EN;
                break;
          		case 861: case 862:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      acc=(MN_ACC_PORT_NUM-9);
                      lr_idx_limit=(MN_LEN_861MHZ_EN-acc);
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_861MHZ_EN;
                     break;
                    case 800:
                      acc=(MN_ACC_PORT_NUM-6);
                      _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_2P5G; 
                      lr_idx_limit=(MN_LEN_862MHZ_EN-acc);
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_862MHZ_EN;
                     break;
                    case 548: case 508:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 24;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 8;
                     }
                     lr_idx_limit= (((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw*1000)/TDM_SLOT_UNIT_2P5G));
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                    break;
                   }
                break;
          		case 550:
                  switch (_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
                    case 0:
                      acc=(MN_ACC_PORT_NUM+15);
                      lr_idx_limit=(MN_LEN_550MHZ_EN-acc);
                      _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = MN_LEN_550MHZ_EN;
                     break;
                    case 580: case 460: case 400:
                     acc = 15;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                    case 280:
                     acc = 14;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                    /*case 400:
                     acc = 8;
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;*/
                    case 300: case 340:
                     if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
                       acc = 12;
                     } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
                       acc = 4;
                     }
                     lr_idx_limit= ((_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw)/(TDM_SLOT_UNIT_5G/1000));
                     _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
                     _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len = lr_idx_limit+acc;
                     break;
                   }
                break;
          		default : break;
          	}
          }
          _tdm->_core_data.vars_pkg.cal_id=2;
          _tdm->_chip_data.soc_pkg.tvec_size   = acc;
          _tdm->_chip_data.soc_pkg.lr_idx_limit= lr_idx_limit;
          _tdm->_chip_data.soc_pkg.soc_vars.mn.pipe_start= 1;
          _tdm->_chip_data.soc_pkg.soc_vars.mn.pipe_end  = MN_NUM_PHY_PORTS;
          for(k =0; k<TDM_AUX_SIZE; k++) {
            _tdm->_core_data.vars_pkg.lr_buffer[k]= MN_NUM_EXT_PORTS;
          }
          for (idx1=0; idx1<TDM_AUX_SIZE; idx1++) {
            _tdm->_core_data.vars_pkg.os_buffer[idx1]=MN_NUM_EXT_PORTS;
          }
          j=0;
          for (idx1=1; idx1<=MN_NUM_PHY_PORTS; idx1++) {
          	if(_tdm->_chip_data.soc_pkg.state[idx1-1] ==  PORT_STATE__LINERATE   ||
               _tdm->_chip_data.soc_pkg.state[idx1-1] ==  PORT_STATE__LINERATE_HG
               ){
          		_tdm->_core_data.vars_pkg.lr_buffer[j++]= (idx1);
            }
          }
          j=0;
          for (idx1=0; idx1<(MN_NUM_PHY_PORTS+1); idx1++) {
          	if(_tdm->_chip_data.soc_pkg.state[idx1-1] ==  PORT_STATE__OVERSUB   ||
               _tdm->_chip_data.soc_pkg.state[idx1-1] ==  PORT_STATE__OVERSUB_HG
               ){
          		_tdm->_core_data.vars_pkg.os_buffer[j++]= (idx1);
            }
          }
        /* allocate Linerate slots: vmap pre-allocation */
        if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_PREALLOC](_tdm)!=PASS){
            result = FAIL;
        }
        /* allocate Linerate slots: vmap allocation */
        else if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_ALLOC](_tdm)!=PASS){
            result = FAIL;
        }
        TDM_PRINT0("==================== Monterey MMU TDM Calendar  ==================");
        tdm_mn_tdm_print(_tdm->_chip_data.cal_2.cal_main, (lr_idx_limit+acc));
        break;
      }
    }
    if(_tdm->_chip_data.soc_pkg.clk_freq != 861 && _tdm->_chip_data.soc_pkg.clk_freq != 862){
      for(i =0; i<MN_LR_LLS_LEN; i++) {
        if(pgw_tdm_tbl0[i] == MN_NUM_EXT_PORTS) break;
        else {
          _tdm->_chip_data.cal_0.cal_main[i]  = pgw_tdm_tbl0[i];
        }
      }
      for(i =0; i<MN_LR_LLS_LEN; i++) {
        if(pgw_tdm_tbl1[i] == MN_NUM_EXT_PORTS) break;
        else {
          _tdm->_chip_data.cal_1.cal_main[i]  = pgw_tdm_tbl1[i];
        }
      }
    }
    /*if(_tdm->_chip_data.soc_pkg.clk_freq == 817){
      tdm_mn_custom_post_process_pgw(_tdm);
      tdm_mn_custom_post_process_mmu(_tdm);
      tdm_mn_custom_restore_vars(_tdm);
    }*/
    if(_tdm->_chip_data.soc_pkg.clk_freq == 816 ||
       _tdm->_chip_data.soc_pkg.clk_freq == 817 ||
       _tdm->_chip_data.soc_pkg.clk_freq == 818
      ){
      tdm_mn_tdm_smooth(_tdm);
    }
    TDM_FREE(high_speed_port_slots);     
    
    return result;
}
/*
 * tdm_mn_tdm_smooth(tdm_mod_t *_tdm)
 *   This API smooths 40G and 10G ports in Flex scenario where Core layer is 
 *   inserting psuedo PM and introducing Jitter errors for 40G and 10G ports.
 *   This API is used only for Customer-25G, CPRI-25G and other SKU's.
 *   Check for min and max distance and if violated swap with idle token 
 *   already present in TDM from core layer.
 */
void tdm_mn_tdm_smooth(tdm_mod_t *_tdm)
{
    int tdm_len = 0, loop = 0, idx=0, min_jitter_int=0, max_jitter_int=0, j=0, k=0;
    int min_jitter=0, max_jitter=0;
    int  *tdm_cal=NULL, lr_spacing=0, wrap_arnd=0, idle_token_location=0;
    enum port_speed_e *port_speeds;
    int apply_correction=0, idle_cnt=0, i, idle_loc_array[MN_NUM_PHY_PORTS];
	int ideal_spacing_roundup=0,ideal_spacing_rounddown=0, rem=0, jitter_range=0, min_spacing=0, max_spacing=0, port_slots=0;

  port_speeds = _tdm->_chip_data.soc_pkg.speed;

  for(i=0; i<MN_NUM_PHY_PORTS; i++) { 
    idle_loc_array[i]     = MN_NUM_EXT_PORTS;
  } 
  for(k=0; k<3; k++) {
    for(j=0; j<3; j++) {
    switch(j){
     case 0:
     tdm_cal    = _tdm->_chip_data.cal_0.cal_main;
     tdm_len    = _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len;
     break;
     case 1:
     tdm_cal    = _tdm->_chip_data.cal_1.cal_main;
     tdm_len    = _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len;
     break;
     case 2:
       tdm_cal  = _tdm->_chip_data.cal_2.cal_main;
       tdm_len  = _tdm->_chip_data.soc_pkg.soc_vars.mn.mmu_len;
     break;
    }
    lr_spacing=0; apply_correction=0;wrap_arnd=0;idle_token_location=0;idx=0;loop=0;
      for (loop=0; loop<tdm_len; loop++ ) {
        lr_spacing =0;
        MN_TOKEN_CHECK(tdm_cal[loop]) {
          if(port_speeds[tdm_cal[loop]] == SPEED_40G || 
             (port_speeds[tdm_cal[loop]] == SPEED_10G ) ||
             (port_speeds[tdm_cal[loop]] == SPEED_50G )
            ){
              switch(port_speeds[tdm_cal[loop]]){
                case SPEED_10G: case SPEED_40G: case SPEED_50G:
                  port_slots = tdm_cmn_get_speed_slots(_tdm, port_speeds[tdm_cal[loop]]);
                  if (!port_slots) { 
                      break; 
                  }
                  ideal_spacing_roundup = chk_tdm_roundup(tdm_len, port_slots);
                  ideal_spacing_rounddown = chk_tdm_rounddown(tdm_len, port_slots);
                  rem = tdm_len % port_slots;
                  jitter_range = chk_tdm_round(2*tdm_len*30,port_slots*100); 	/* 30% from ideal_spacing 2*tabl_len*30/port_slots/100 */
                  
                  if(rem < ((port_slots+1)/2)) {
                  	min_spacing = ideal_spacing_rounddown - chk_tdm_rounddown(jitter_range, 2);         /*ROUNDDOWN(ideal_spacing[port_speed],0) - ROUNDDOWN(range[port_speed]/2, 0);*/
                  	max_spacing = ideal_spacing_rounddown + chk_tdm_roundup(jitter_range, 2);     /*ROUNDDOWN(ideal_spacing[port_speed],0) + ROUNDUP(range[port_speed]/2, 0);*/
                  } else {
                  	min_spacing = ideal_spacing_roundup - chk_tdm_roundup(jitter_range,2);   /*ROUNDUP(ideal_spacing[port_speed],0) - ROUNDUP(range[port_speed]/2, 0);*/
                  	max_spacing = ideal_spacing_roundup + chk_tdm_rounddown(jitter_range,2);       /*ROUNDUP(ideal_spacing[port_speed],0) + ROUNDDOWN(range[port_speed]/2, 0);*/
                  }
                  min_jitter = min_spacing;
                  max_jitter = max_spacing;
                  min_jitter_int = (int) (  ((min_jitter*2)+1)/2  );
                  max_jitter_int = (int) (  ((max_jitter*2)+1)/2  );
                break;
                default: break;
              }
            idle_cnt = 0;
            for (idx=loop+1, wrap_arnd=0; idx<=tdm_len && wrap_arnd<2; idx++ ) {
               apply_correction = 0;
               if (idx == tdm_len) {
                 idx = -1; wrap_arnd++; continue;
               }
               lr_spacing++;
               if(lr_spacing >= min_jitter_int && lr_spacing <= max_jitter_int){
                 if(tdm_cal[idx] == MN_NUM_EXT_PORTS){
                   idle_loc_array[idle_cnt] = loop +(idx-loop);
                   idle_cnt++;
                 }
                   idle_token_location = idle_loc_array[(idle_cnt-1)];
               }
               if (tdm_cal[loop] == tdm_cal[idx]) {
                 if(lr_spacing >max_jitter_int ){
                   apply_correction = 1;
                 } else if (lr_spacing<min_jitter_int) {
                  apply_correction = 1;
      					  idx = loop + min_jitter_int;  
      					  while (tdm_cal[idx]!=MN_NUM_EXT_PORTS) {
      						  idx++;
      					  };
             			idle_token_location = idx;
      					 } 
      					break;
               }
            }
            idle_cnt = 0;
          }
          if(apply_correction == 1  && wrap_arnd == 0){
            tdm_cal[idle_token_location] = tdm_cal[(lr_spacing+loop)];
            tdm_cal[(lr_spacing + loop)] = MN_NUM_EXT_PORTS;
            apply_correction = 0;
          } else if(apply_correction == 1  && wrap_arnd == 1) {
            tdm_cal[idle_token_location] = tdm_cal[lr_spacing - (tdm_len-loop)];
            tdm_cal[lr_spacing - (tdm_len - loop)] = MN_NUM_EXT_PORTS;
            apply_correction = 0;
          }
          idle_token_location=0;
        }
      }
    }
  }
  tdm_mn_tdm_print(_tdm->_chip_data.cal_2.cal_main, (tdm_len));
  return;
}
/*
void tdm_mn_custom_post_process_pgw(tdm_mod_t *_tdm)
{
  int i=0, j=0, idx=0, numof_12p5_in_single_pm=0, pgw_len=0,
      total_pm_with_12p5=0, port_to_replace=MN_NUM_EXT_PORTS, total_12p5_ports=0, curr_port_to_replace=MN_NUM_EXT_PORTS;
  int *pgw_cal,*speed_orig, **pmap_orig;
  int ports_12p5[MN_NUM_PHY_PM], pm_with_12p5[MN_NUM_PHY_PM];
  int replaced_port_cnt=0, pm_12p5_cnt=0, running_pm_cnt=0;
  speed_orig = _tdm->_chip_data.soc_pkg.soc_vars.mn.speed;
  pmap_orig  = _tdm->_chip_data.soc_pkg.soc_vars.mn.pmap;
  for(i=0; i<MN_NUM_PHY_PM; i++){
    pm_with_12p5[i] = MN_NUM_EXT_PORTS;
  }
  for(i=0; i<(MN_NUM_PHY_PM);i=i++){
    for(j=0;j<MN_NUM_PM_LNS;j++){
    if(speed_orig[pmap_orig[i][j]]  == SPEED_12p5G){
      ports_12p5[idx++] = ( ( (i*MN_NUM_PM_LNS) + j) +1);
      numof_12p5_in_single_pm++;
      total_12p5_ports++;
    }
    }
    if(numof_12p5_in_single_pm>0){
      pm_with_12p5[i] = numof_12p5_in_single_pm;
      total_pm_with_12p5++;
    }
    numof_12p5_in_single_pm = 0;
  }
    idx=0;
  for(j=0; j < MN_NUM_QUAD; j++){
    switch(j){
      case 0:
        pgw_cal =  _tdm->_chip_data.cal_0.cal_main;
        pgw_len = _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len;
        idx=0;
      break;
      case 1:
        pgw_cal =  _tdm->_chip_data.cal_1.cal_main;
        pgw_len = _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len;
        idx=0;
      break;
      default: break;
    }
    for(j=0; j<total_pm_with_12p5; j++){
      numof_12p5_in_single_pm = pm_with_12p5[j];
      port_to_replace = ports_12p5[j*numof_12p5_in_single_pm];
      curr_port_to_replace = port_to_replace;
      for(i=0; i<pgw_len; i++){
        if(_tdm->_chip_data.soc_pkg.soc_vars.mn.speed[pgw_cal[i]] == SPEED_12p5G && pgw_cal[i] == port_to_replace){
          pgw_cal[i] = curr_port_to_replace;
          curr_port_to_replace++;
          if((curr_port_to_replace == ((j*numof_12p5_in_single_pm)+numof_12p5_in_single_pm)+1)) {
            curr_port_to_replace = port_to_replace;
          }
        }
      }
    }
    tdm_mn_tdm_print(pgw_cal, pgw_len);
  }
}
void tdm_mn_custom_post_process_mmu(tdm_mod_t *_tdm)

{
  int i=0, j=0, idx=0, numof_12p5_in_single_pm=0, mmu_len=0,
      total_pm_with_12p5=0, port_to_replace=MN_NUM_EXT_PORTS, total_12p5_ports=0, curr_port_to_replace=MN_NUM_EXT_PORTS;
  int *mmu_cal,*speed_orig, **pmap_orig;
  int ports_12p5[MN_NUM_PHY_PM], pm_with_12p5[MN_NUM_PHY_PM];
  int replaced_port_cnt=0, pm_12p5_cnt=0, running_pm_cnt=0;
  speed_orig = _tdm->_chip_data.soc_pkg.soc_vars.mn.speed;
  pmap_orig  = _tdm->_chip_data.soc_pkg.soc_vars.mn.pmap;
  mmu_len    = _tdm->_chip_data.cal_2.cal_len;
  mmu_cal    = _tdm->_chip_data.cal_2.cal_main;
  for(i=0; i<MN_NUM_PHY_PM; i++){
    pm_with_12p5[i] = MN_NUM_EXT_PORTS;
  }
  for(i=0; i<(MN_NUM_PHY_PM);i=i++){
    for(j=0;j<MN_NUM_PM_LNS;j++){
    if(speed_orig[pmap_orig[i][j]]  == SPEED_12p5G){
      ports_12p5[idx++] = ( ( (i*MN_NUM_PM_LNS) + j) +1);
      numof_12p5_in_single_pm++;
      total_12p5_ports++;
    }
    }
    if(numof_12p5_in_single_pm>0){
      pm_with_12p5[i] = numof_12p5_in_single_pm;
      total_pm_with_12p5++;
    }
    numof_12p5_in_single_pm = 0;
  }
  idx=0;
  for(j=0; j<total_pm_with_12p5; j++){
    numof_12p5_in_single_pm = pm_with_12p5[j];
    port_to_replace = ports_12p5[j*numof_12p5_in_single_pm];
    curr_port_to_replace = port_to_replace;
    for(i=0; i<mmu_len; i++){
      if(_tdm->_chip_data.soc_pkg.soc_vars.mn.speed[mmu_cal[i]] == SPEED_12p5G && mmu_cal[i] == port_to_replace){
        mmu_cal[i] = curr_port_to_replace;
        curr_port_to_replace++;
        if((curr_port_to_replace == ((j*numof_12p5_in_single_pm)+numof_12p5_in_single_pm)+1)) {
          curr_port_to_replace = port_to_replace;
        }
      }
    }
  }
  tdm_mn_tdm_print(mmu_cal, mmu_len);
}

void tdm_mn_custom_restore_vars(tdm_mod_t *_tdm)
{
  int i=0, j=0;
	for (i=1; i<(_tdm->_chip_data.soc_pkg.num_ext_ports-MN_EXT_PLUS_CPU_PORT); i++) {
		_tdm->_chip_data.soc_pkg.state[i] = _tdm->_chip_data.soc_pkg.soc_vars.mn.state[i];
  }
	for (i=0; i<(_tdm->_chip_data.soc_pkg.num_ext_ports-MN_NUM_PHY_PORTS); i++) {
		_tdm->_chip_data.soc_pkg.speed[i] = _tdm->_chip_data.soc_pkg.soc_vars.mn.speed[i];
	}
  for(i=0; i<MN_NUM_PHY_PM;i++){
    for(j=0; j< MN_NUM_PM_LNS; j++){
      _tdm->_chip_data.soc_pkg.pmap[i][j] = _tdm->_chip_data.soc_pkg.soc_vars.mn.pmap[i][j];
    }
  }
  _tdm->_chip_data.soc_pkg.state[0] = PORT_STATE__LINERATE; 
  _tdm->_chip_data.soc_pkg.state[_tdm->_chip_data.soc_pkg.num_ext_ports-MN_EXT_PLUS_CPU_PORT] = PORT_STATE__LINERATE; 
 
   _tdm->_chip_data.soc_pkg.speed[MN_SBUS_TOKEN] = SPEED_5G;
}
*/
/**
@name: tdm_mn_proc_cal_ancl
@param:

Allocate ancillary slots with matched speed rates accordingly
 */
int
tdm_mn_proc_cal_ancl(tdm_mod_t *_tdm)
{
    int i, j, k, k_prev, k_idx, idx_up, idx_dn, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_empty, param_token_ancl,
        param_lr_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    /* allocate ANCL slots */
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_ancl_num>0){
        TDM_PRINT1("TDM: Allocate %d ANCL slots\n\n", param_ancl_num);
        if (param_lr_en==BOOL_TRUE){
            k = 0;
            for (j=0; j<param_ancl_num; j++){
                idx_up = 0;
                idx_dn = 0;
                k_idx  = 0;
                k_prev = k;
                if (j==(param_ancl_num-1)){
                    k = param_cal_len - 1;
                }
                else {
                    k = k_prev + ((param_cal_len-k_prev)/(param_ancl_num-j)) -1;
                }
                for (i=k; i>0 && i<param_cal_len; i--){
                    if (param_cal_main[i]==param_token_empty){
                        idx_up = i;
                        break;
                    }
                }
                for (i=k; i<param_cal_len; i++){
                    if (param_cal_main[i]==param_token_empty){
                        idx_dn = i;
                        break;
                    }
                }
                if (idx_up>0 || idx_dn>0){
                    if (idx_up>0 && idx_dn>0){
                        k_idx = ((k-idx_up)<(idx_dn-k))?(idx_up):(idx_dn);
                    }
                    else if (idx_up>0){
                        k_idx = idx_up;
                    }
                    else if (idx_dn>0){
                        k_idx = idx_dn;
                    }
                }
                if (k_idx==0){
                    for (i=(param_cal_len-1); i>0; i--){
                        if (param_cal_main[i]==param_token_empty){
                            k_idx = i;
                            break;
                        }
                    }
                }
                if (k_idx>0 && k_idx<param_cal_len){
                    param_cal_main[k_idx] = param_token_ancl;
                    TDM_PRINT2("TDM: Allocate %d-th ANCL slot at index #%03d\n",
                                j+1, k_idx);
                }
                else {
                    TDM_WARN1("TDM: Failed in %d-th ANCL slot allocation\n",j);
                    result = FAIL;
                }
            }
        }
        else {
           /* Commented for OS config k_prev = 0;
            for (j=0; j<param_ancl_num; j++){
                k_idx = k_prev + ((param_cal_len-k_prev)/(param_ancl_num-j)) - 1;
                param_cal_main[k_idx] = param_token_ancl;
                k_prev= k_idx+1;
                TDM_PRINT1("TDM: Allocate ANCL slot at index #%03d\n",
                            k_idx);
            }*/
        }
        TDM_SML_BAR
    }
    
    return result;
}
/**                 
 * @name: tdm_mn_proc_cal
 * @param:             
 *          
 Allocate slots for linerate/oversub/ancl ports with matched speed rates accordingly
 */                 
int                 
tdm_mn_proc_cal( tdm_mod_t *_tdm )
{                     
    /* allocate LINERATE slots */
    if(tdm_mn_proc_cal_lr(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }                   
    
    /* allocate ANCILLARY slots */
    if( _tdm->_chip_exec[TDM_CHIP_EXEC__PROC_ANCL](_tdm)!=PASS){ 
        return (TDM_EXEC_CORE_SIZE+1);
    }
           
    /* allocate IDLE/OVSB slots */
    if(tdm_mn_proc_cal_idle(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }
                    
    return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER](_tdm) );
}                   
/**
 * @name: tdm_mn_proc_cal_idle
 * @param:
 *
 * Allocate idle slots
 */
int
tdm_mn_proc_cal_idle( tdm_mod_t *_tdm )
{
    int i, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_token_empty, param_token_ovsb,
        param_token_idl1, param_token_idl2,
        param_os_en;
    int *param_cal_main;

    /* higig_mgmt */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_token_idl1  = _tdm->_chip_data.soc_pkg.soc_vars.idl1_token;
    param_token_idl2  = _tdm->_chip_data.soc_pkg.soc_vars.idl2_token;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;

    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);

    if (param_os_en==BOOL_TRUE){
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_empty){
                param_cal_main[i] = param_token_ovsb;
            }
        }
    }
    else {
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_empty){
                if ((i%2)==0){
                    param_cal_main[i] = param_token_idl1;
                }
                else {
                    param_cal_main[i] = param_token_idl2;
                }
            }
        }
    }

    return result;
}
