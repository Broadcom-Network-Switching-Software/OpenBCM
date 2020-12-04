 $Id: README.txt,v 1.55 2013/03/10 13:29:08 omery Exp $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                        
	 
 ~ Name:	 
+-------------------------+                                                        
|    Snake Application    |                                                         
+-------------------------+                                                       
 ~ Description:
	The test is a stress test on the BCM88750 (FE1600) links. The test validates the links performance under a burst of cells.                                                                                                   
                                                                              
 ~ File Reference:                                                              
    cint_snake_test.c                                                              
	
 ~ Name:   
+-----------------+
|    Warm Boot    |
+-----------------+
 ~ Description:
	Test for BCM88750 (FE1600) warm reboot.

 ~ File Reference: 
	cint_warmboot.c
 
 
 ~ Name:   
+--------------------+
|    Flow Control    |
+--------------------+
 ~ Description:
	This example demonstrates and tests the FIFO threshold sequence.

 ~ File Reference:
	cint_fifos.c

	  
 ~ Name:   
+---------------------------+
|    Flow Control FE3200    |
+---------------------------+
 ~ Description:
	This example demonstrates and tests the FIFO threshold pipe_set/get API for BCM88770 (FE3600, previously known as FE3200).

 ~ File Reference:
	cint_flow_control_fe3200.c
    
	  
 ~ Name:   	  
+----------------------+
|    FE3200 Interop    |
+----------------------+
 ~ Description:
	This example demonstrate the configuration required when connecting BCM88770 (FE3600, previously known as FE3200) to legacy devices.

 ~ File Reference:
	cint_fe3200_interop.c
