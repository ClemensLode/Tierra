#ifndef _NODES_H_
#define _NODES_H_

#include "viech.h"

extern Viech *head,*tail,*tempn;
extern unsigned short node_num;
extern unsigned long node_counter;


Viech *Insert_Node(unsigned long id);
Viech *Delete_Node(unsigned long id); // node to delete

#endif
