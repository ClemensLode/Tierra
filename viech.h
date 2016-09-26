#ifndef _VIECH_H_
#define _VIECH_H_
#include <malloc.h>
#include <memory.h>
#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <objbase.h>
#include <iostream> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>


#define WRITE_PROTECTION TRUE
#define STACK_SIZE 16
#define SIZE_OF_SOUP 512000
#define MEMORY_RANGE 1024 //weitest entfernter Block der von einem Programm alloziiert werden kann (Geschwindigkeitsgründe...)
#define FIRST_PROGRAM_LENGTH 36

const unsigned char FLAG_ZERO=1;
const unsigned char FLAG_POSITIVE=2;
const unsigned char FLAG_EQUAL=4;
const unsigned char FLAG_GREATER=8;
const unsigned char FLAG_CARRY=16;
const unsigned char FLAG_ERROR=32; //Error Flag, Division durch 0, Sprungmarke nicht gefunden,...
const unsigned char FLAG_OVERFLOW=64;
const unsigned char FLAG_DIRECTION=128;


extern unsigned char *Soup;
extern bool *Availible;


class Viech
{
public:
	bool active;
	unsigned char Reg8[8];
	unsigned short Reg16[4];
	unsigned short flags;
	unsigned long StartPosition;
	unsigned short Laenge;
	Viech *Child;
	unsigned long IP;
	unsigned short Alter;
//	unsigned short *Stack;
	unsigned short Viech::LookForFollowingBlock(unsigned char par1,unsigned char par2);
	void WriteC(unsigned long position,unsigned char wert);
	unsigned char ReadC(unsigned long position);
	bool Compile();
	Viech();
	~Viech();
   unsigned long ID;
   Viech *next; // this is the link to the next node
   unsigned long LookForMemoryBlock(unsigned long SearchStart,unsigned short Laenge,unsigned short LaengeChild);

};

#endif

