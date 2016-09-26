#include "nodes.h"
#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <objbase.h>
#include <iostream> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include "viech.h"


Viech *head,*tail,*tempn;
unsigned short node_num; //Anzahl aktiver Nodes
unsigned long node_counter; //aktueller Zaehler / maximale ID

Viech* Insert_Node(unsigned long id) 
{
// this function inserts a node at the end of the list
Viech* new_node = NULL;

// step 1: create the new node
new_node = (Viech*)malloc(sizeof(Viech)); // in C++ use new operator

// fill in fields
new_node->ID=id;

new_node->next = NULL; // good practice

// step 2: what is the current state of the linked list?

if (head==NULL) // case 1
   {
   // empty list, simplest case
   head = new_node;
   tail = new_node;
 
   // return new node
   return(new_node);
   } // end if
else
if ((head != NULL) && (head==tail)) // case 2
   {
   // there is exactly one element, just a little 
   // finesse...
   head->next = new_node;
   tail = new_node;

   // return new node
   return(new_node);
   } // end if
else // case 3
   { 
   // there are 2 or more elements in list
   // simply move to end of the list and add
   // the new node
   tail->next = new_node;
   tail = new_node;

   // return the new node
   return(new_node);
   } // end else

} // end Insert_Node


////////////////////////////////////////////////////////////////////////

Viech *Delete_Node(unsigned long id) // node to delete
{
// this function deletes a node from 
// the linked list given its id
Viech *curr_ptr = head, // used to search the list
         *prev_ptr = head; // previous record

// test if there is a linked list to delete from
if (!head) 
    return(NULL); 

// traverse the list and find node to delete
while(curr_ptr->ID != id && curr_ptr)
     {
     // save this position
     prev_ptr = curr_ptr;
     curr_ptr = curr_ptr->next;
     } // end while

// at this point we have found the node
// or the end of the list
if (curr_ptr == NULL)
    return(NULL); // couldn't find record

// record was found, so delete it, but be careful, 
// need to test cases
// case 1: one element 
if (head==tail) 
   {
   // delete node
   free(head);
   
   // fix up pointers
   head=tail=NULL;

   // return id of deleted node   
   return(NULL);
   } // end if
else // case 2: front of list
if (curr_ptr == head)
   {
   // move head to next node
   head=head->next;

   // delete the node
   free(curr_ptr);

   // return id of deleted node
   return(head);

   } // end if
else // case 3: end of list
if (curr_ptr == tail)
   {
   // fix up previous pointer to point to null
   prev_ptr->next = NULL;
  
   // delete the last node
   free(curr_ptr);

   // point tail to previous node
   tail = prev_ptr; 

   // return id of deleted node
   return(head);

   } // end if
else  // case 4: node is in middle of list
   {     
   // connect the previous node to the next node
   prev_ptr->next = curr_ptr->next;

   // now delete the current node
   free(curr_ptr);

   // return id of deleted node
   //return(id);
   return(prev_ptr->next);

   } // end else

} // end Delete_Node
