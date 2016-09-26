#include "nodes.h"
#include "viech.h"

#include <objbase.h>
#include <iostream.h> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>


unsigned char *Soup, *FirstProgram;
bool *Availible;

extern Viech *head,*tail,*tempn;
extern unsigned short node_num;
extern unsigned long node_counter;
unsigned long node_temp;

short main(short argc, char* argv[])
{
	Soup =(UCHAR*)malloc(SIZE_OF_SOUP); // in C++ use new operator
	FirstProgram =(UCHAR*)malloc(FIRST_PROGRAM_LENGTH); // in C++ use new operator
	Availible =(bool*)malloc(SIZE_OF_SOUP);

	FirstProgram[0]=200;
	FirstProgram[1]=5;
	FirstProgram[2]=6;
	FirstProgram[3]=159;
	FirstProgram[4]=6;
	FirstProgram[5]=5;
	FirstProgram[6]=59;
	FirstProgram[7]=59;
	FirstProgram[8]=160;
	FirstProgram[9]=3;
	FirstProgram[10]=218;
	FirstProgram[11]=51;
	FirstProgram[12]=195;
	FirstProgram[13]=3;
	FirstProgram[14]=242;
	FirstProgram[15]=210;
	FirstProgram[16]=200;
	FirstProgram[17]=4;
	FirstProgram[18]=3;
	FirstProgram[19]=5;
	FirstProgram[20]=56;
	FirstProgram[21]=6;
	FirstProgram[22]=240;
	FirstProgram[23]=60;
	FirstProgram[24]=65;
	FirstProgram[25]=137;
	FirstProgram[26]=243;
	FirstProgram[27]=146;
	FirstProgram[28]=251;
	FirstProgram[29]=252;
	FirstProgram[30]=161;
	FirstProgram[31]=158;
	FirstProgram[32]=250;
	FirstProgram[33]=249;
	FirstProgram[34]=249;
	FirstProgram[35]=250;

	
	/* // START
  200 (Überspringe Sprungmarke)
    5 //StartSprungmarke
	6
 
  159 //addr, look for ende template => AX
	1
	2
  
  59   //inc ax (template ist 2 bytes lang...)
  	
  160 //Allocate Memory  // Parameter: AX: Laenge, 
				jetzt: AX Länge Mutter, CX Relative StartPosition des Kinds

  3 // MOV Reg16	Reg16/Mem16
  11 011 010 // DX(011) <= CX(010) (218)
  // jetzt: AX Länge Mutter, CX,DX Relative StartPosition des Kinds

  51 //ADD	Reg16	Reg16	Reg16 / Mem16
  11 000 011 // DX(011) = DX(011)+AX(000)    (195)
  // jetzt: AX Länge Mutter, CX Relative StartPosition des Kinds, DX Relative EndPosition des Kindes

  3 // MOV Reg16/Mem16
  11 110 010 //SI(110) <= CX(010)  (242)

  210 //zero bx
  
  200 (Überspringe Sprungmarke)
// KOPIEREN
    4
	3 //Kopiersprungmarke

	5  // MOV AL, Mem8
	00 111 000 //mov al, soup[bx(111)]  (56)
		//Eigene ErbInformationen lesen
    
	6 //MOV Mem8, AL
	11 110 000 //mov soup[SI(110)],al   (240)
  60  // INC BX  Parentposhorter weiter
  
  65  //INC SI  Childposhorter weiter

  137  //CMP	Flags	Reg16	Reg16/Mem16
  11 110 011  //CMP   SI(110),DX(011)   DX: Ende, Schon am ende?    (243)

  146://jump if below (DX < SI)
    251
	252  // => Kopiersprungmarke

  161 //Give Birth

  158 // jump 
     250 //=> STARTSprungmarke
	 249

    254  // Ende Marker
    253
*/
	unsigned char a;
	unsigned long i,k,l,x;
//	unsigned short temp2;
	node_counter=0;
	for(i=0;i<SIZE_OF_SOUP;i++)
	{
		Soup[i]=0;
		Availible[i]=true;
	};

	//Inject soup with 16 Programs
	for(i=0;i<16;i++)
	{
		node_num++;
		node_counter++;
		Insert_Node(node_counter);  //Warum Nodecounter und nicht node_num? evtl. Mehrfachbelegung ein und der selben Nummer nach Löschung!
	}
	tempn=head;
	while(tempn!=NULL)
	{

		tempn->StartPosition=tempn->LookForMemoryBlock(((rand()<<16)+rand())%SIZE_OF_SOUP,FIRST_PROGRAM_LENGTH,FIRST_PROGRAM_LENGTH);
		tempn->Laenge=FIRST_PROGRAM_LENGTH;
		for(i=(tempn->StartPosition);i<(tempn->StartPosition+tempn->Laenge);i++)
		{
			Availible[i]=false;
			Soup[i]=FirstProgram[i-tempn->StartPosition]; //Reinkopieren
		}
		tempn->active=true;
		tempn->Alter=0;
		tempn->Child=NULL;
		tempn->flags=0;
		tempn->IP=tempn->StartPosition;
//		tempn->Stack=(unsigned short*)malloc(STACK_SIZE);
		for(i=0;i<8;i++) 
		{
			tempn->Reg16[i]=0;
			tempn->Reg8[i]=0;
			}
		tempn=tempn->next;
	}



	tempn=head;
	while(head!=NULL)
	{
		if(tempn==NULL)
		{
			tempn=head;
			continue;
		}
		else
		{
			if(tempn->active)
			{
					node_temp=tempn->ID;


					if(1==0)						
					{
						l=0;
						for(i=tempn->StartPosition;i<tempn->StartPosition+tempn->Laenge;i++)
							if((i>0)&&(i<SIZE_OF_SOUP))
								l+=Soup[i];
						//		printf("%c",Soup[i]);
						//cout<<"\n";
						cout<<"Parent: "<<tempn->ID<<"  Laenge:"<<tempn->Laenge<<" CRC: "<<l;
						if(tempn->Child)
						{
							l=0;
							for(i=tempn->Child->StartPosition;i<tempn->Child->StartPosition+tempn->Child->Laenge;i++)
								if((i>0)&&(i<SIZE_OF_SOUP))
									l+=Soup[i];
							//cout<<"  Child: "<<tempn->Child->ID<<"  Laenge:"<<tempn->Child->Laenge<<" CRC: "<<l;
						}
						cout<<"\n";
						//cout << "used: " <<l<<"\n";
					}

					/*}*/

					
					//cout<< "Nodes " <<node_counter<<"\n";


			//if(node_temp==1)
			//{
				//cout << "A" <<tempn->Alter<<" I "<<tempn->IP-tempn->StartPosition<<" . "<<tempn->ReadC(tempn->IP)<<" SI "<<tempn->Reg16[6]<<" DX "<<tempn->Reg16[3]<<" AX "<< tempn->Reg16[0]<<" BX "<<tempn->Reg16[1]<<" CX "<<tempn->Reg16[2]<<" F "<<(tempn->flags&FLAG_ERROR)<<"\n";
		//		cin >> a;

			//}

		

		if(!tempn->Compile()) //Creature have to be killed
		{
			for(i=tempn->StartPosition;i<=(tempn->StartPosition+tempn->Laenge);i++)
			{
				Availible[i%SIZE_OF_SOUP]=true;
				if(rand()%8==0)
					Soup[i]=(unsigned char)(rand()%255);
			}
			//if(tempn->Stack) free(tempn->Stack);
			//tempn=tempn->next; ?

			tempn=Delete_Node(node_temp);
			if(tempn!=NULL)
				node_num--; else return -1;
		}
		else
			tempn=tempn->next;
		}else tempn=tempn->next;
		} 
	}
	
	//Fehlt: Ausgabeprozedur, die 3 besten Programme anzeigen lassen (Länge, Inhalt, Anzahl), belegten Speicher, Anzahl der Kreaturen insgesamt, durchschnittliche Länge
	free(Soup);
	free(Availible);
	return 0;
}


//ok... 1. Fortpflanzung funzt, 2. net
//READC Anwendungen überprüfen (Mutationen!)