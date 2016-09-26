// Compiler

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "nodes.h"
#include "viech.h"

/*#define TRUE true
#define FALSE false*/

/*R - E - G - ME -  M/R - EG - DISP

DISP:
(192) 11: Reg als Zweites
(0) 00: kein Disp für Mem
(128) 01: signed 8 Bit
(64) 10: signed 16 Bit

  
Word Operand     Byte operand        Segment
--------------------------------------------
000 AX           000 AL              00 ES
001 BX           001 CL              01 CS
010 CX           010 DL              10 SS
011 DX           011 BL              11 DS
100 SP           100 AH
101 BP           101 CH
110 SI           110 DH
111 DI           111 BH

 r/m   Operand Address
---------------------
000   (BX) + (SI) + DISP
001   (BX) + (DI) + DISP
010   (BP) + (SI) + DISP
011   (BP) + (DI) + DISP
100   (SI) + DISP
101   (DI) + DISP
110   (BP) + DISP*
111   (BX) + DISP*/

short Signed16(unsigned char hi,unsigned char lo)
{
	if((hi&128)==128)
		return (-((((short)hi-128)<<8)+lo));
	else return((hi<<8)+lo);
};
short Signed8(unsigned char lo)
{
	if((lo&128)==128)
		return ((short)128-lo);
	else return(lo);
};
//DISP und LENGTH RAUS!
bool Viech::Compile() //FALSE: Kill Creature, TRUE: ok
{
unsigned char FirstRegister,SecondRegister,length;
bool SecondReg;
unsigned short Disp;
unsigned char tempc,Befehl,par1,par2,par3,par1l,par2l,par3l,mem1,mem2,mem12,mem22;  //Befehl: Tempvariable zum Speichern von ReadC[IP]... net dass da mehrmals mutationen auftreten
unsigned short tempw;
unsigned long MemZiel,MemZiel2;
unsigned short OldReg16[8];
unsigned char OldReg8[8];
unsigned short j;


for(j=0;j<8;j++)
{
	OldReg16[j]=Reg16[j];
	OldReg8[j]=Reg8[j];
}

	if(!((IP>=StartPosition)&&(IP<(StartPosition+Laenge)))) 
	//Programmposhorter außerhalb des Programms => Programm "zerfließt"
		return FALSE;
if(Alter>65535)
return FALSE;
Alter++;
SecondReg=FALSE;
MemZiel=0;
Disp=0;
length=0;

Befehl=ReadC(IP);
par1=ReadC(IP+1);
par2=ReadC(IP+2);
par3=ReadC(IP+3);


FirstRegister=(par1&7);

switch (par1&192)
{
	case 0: break;
	case 64: break;
	case 128: SecondReg=TRUE;break;
	case 192: SecondReg=TRUE;break;
	default:break;
}

/*switch (ReadC(IP+1)&192)
{
	case 0: Disp=0;length=0;break;
	case 64: Disp=Signed16(ReadC(IP+2),ReadC(IP+3)); length=2;break;
	case 128: Disp=Signed8(ReadC(IP+2)); length=1;break;
	case 192: SecondReg=TRUE;length=0;break;
	default:break;
} OLD */

par1l=ReadC(IP+1+length);
par2l=ReadC(IP+2+length);
par3l=ReadC(IP+3+length); //Vorsicht wegen Alter! Jeder Befehl im Moment gleich :[ (außer writec)

//~~~ Falsche lange Befehle am Ende des Programms? mmmh...

SecondRegister=(par1&56)>>3;

	switch(SecondRegister)
	{
		case 0:MemZiel=Reg16[1]+Reg16[6]+Disp;break;
		case 1:MemZiel=Reg16[1]+Reg16[7]+Disp;break;
		case 2:MemZiel=Reg16[5]+Reg16[6]+Disp;break;
		case 3:MemZiel=Reg16[5]+Reg16[7]+Disp;break;
		case 4:MemZiel=Reg16[6]+Disp;break;
		case 5:MemZiel=Reg16[7]+Disp;break;
		case 6:MemZiel=Reg16[5]+Disp;break;
		case 7:MemZiel=Reg16[1]+Disp;break;
		default: MemZiel=0;break;
	}
	//Memory Ziel wählen

	switch(FirstRegister)
	{
		case 0:MemZiel2=Reg16[1]+Reg16[6]+Disp;break;
		case 1:MemZiel2=Reg16[1]+Reg16[7]+Disp;break;
		case 2:MemZiel2=Reg16[5]+Reg16[6]+Disp;break;
		case 3:MemZiel2=Reg16[5]+Reg16[7]+Disp;break;
		case 4:MemZiel2=Reg16[6]+Disp;break;
		case 5:MemZiel2=Reg16[7]+Disp;break;
		case 6:MemZiel2=Reg16[5]+Disp;break;
		case 7:MemZiel2=Reg16[1]+Disp;break;
		default:MemZiel2=0;break;
	}
	//Memory Ziel 2 wählen (für XCHG)
	mem1=ReadC(StartPosition+MemZiel);
	mem2=ReadC(StartPosition+MemZiel+1);
	mem12=ReadC(StartPosition+MemZiel);
	mem22=ReadC(StartPosition+MemZiel2+1);


	if((Befehl<144)||(Befehl>157))
		flags=0;		// Flags resetten vor jedem Befehl... evtl bei NOP Befehlen/ Befehlen die Flags nicht ändern nicht

switch(Befehl)
{
/*track 0: prot
          xwr
nop1    ; 010 110 01   0 beginning marker
nop1    ; 010 110 01   1 beginning marker
nop1    ; 010 110 01   2 beginning marker
nop1    ; 010 110 01   3 beginning marker
zero    ; 010 110 04   4 put zero in cx
not0    ; 010 110 02   5 put 1 in first bit of cx
shl     ; 010 110 03   6 shift left cx (cx = 2)
shl     ; 010 110 03   7 shift left cx (cx = 4)
movcd   ; 010 110 18   8 move cx to dx (dx = 4)
adrb    ; 010 110 1c   9 get (backward) address of beginning marker -> ax
nop0    ; 010 100 00  10 complement to beginning marker
nop0    ; 010 100 00  11 complement to beginning marker
nop0    ; 010 100 00  12 complement to beginning marker
nop0    ; 010 100 00  13 complement to beginning marker
sub_ac  ; 010 110 07  14 subtract cx from ax, result in ax
movab   ; 010 110 19  15 move ax to bx, bx now contains start address of mother
adrf    ; 010 110 1d  16 get (forward) address of end marker -> ax
nop0    ; 010 100 00  17 complement to end marker
nop0    ; 010 100 00  18 complement to end marker
nop0    ; 010 100 00  19 complement to end marker
nop1    ; 010 100 01  20 complement to end marker
inc_a   ; 010 110 08  21 increment ax, to include dummy instruction at end
sub_ab  ; 010 110 06  22 subtract bx from ax to get size, result in cx
nop1    ; 010 110 01  23 reproduction loop marker
nop1    ; 010 110 01  24 reproduction loop marker
nop0    ; 010 110 00  25 reproduction loop marker
nop1    ; 010 110 01  26 reproduction loop marker
mal     ; 010 110 1e  27 allocate space (cx) for daughter, address to ax
call    ; 010 110 16  28 call template below (copy procedure)
nop0    ; 010 100 00  29 copy procedure complement
nop0    ; 010 100 00  30 copy procedure complement
nop1    ; 010 100 01  31 copy procedure complement
nop1    ; 010 100 01  32 copy procedure complement
divide  ; 010 110 1f  33 create independent daughter cell
jmp     ; 010 110 14  34 jump to template below (reproduction loop)
nop0    ; 010 100 00  35 reproduction loop complement
nop0    ; 010 100 00  36 reproduction loop complement
nop1    ; 010 100 01  37 reproduction loop complement
nop0    ; 010 100 00  38 reproduction loop complement
ifz     ; 010 000 05  39 dummy instruction to separate templates
nop1    ; 010 110 01  40 copy procedure template
nop1    ; 010 110 01  41 copy procedure template
nop0    ; 010 110 00  42 copy procedure template
nop0    ; 010 110 00  43 copy procedure template
pushax  ; 010 110 0c  44 push ax onto stack
pushbx  ; 010 110 0d  45 push bx onto stack
pushcx  ; 010 110 0e  46 push cx onto stack
nop1    ; 010 110 01  47 copy loop template
nop0    ; 010 110 00  48 copy loop template
nop1    ; 010 110 01  49 copy loop template
nop0    ; 010 110 00  50 copy loop template
movii   ; 010 110 1a  51 move contents of [bx] to [ax] (copy one instruction)
dec_c   ; 010 110 0a  52 decrement cx (size)
ifz     ; 010 110 05  53 if cx == 0 perform next instruction, otherwise skip it
jmp     ; 010 110 14  54 jump to template below (copy procedure exit)
nop0    ; 010 110 00  55 copy procedure exit complement
nop1    ; 010 110 01  56 copy procedure exit complement
nop0    ; 010 110 00  57 copy procedure exit complement
nop0    ; 010 110 00  58 copy procedure exit complement
inc_a   ; 010 110 08  59 increment ax (address in daughter to copy to)
inc_b   ; 010 110 09  60 increment bx (address in mother to copy from)
jmp     ; 010 110 14  61 bidirectional jump to template below (copy loop)
nop0    ; 010 100 00  62 copy loop complement
nop1    ; 010 100 01  63 copy loop complement
nop0    ; 010 100 00  64 copy loop complement
nop1    ; 010 100 01  65 copy loop complement
ifz     ; 010 000 05  66 this is a dummy instruction to separate templates
nop1    ; 010 110 01  67 copy procedure exit template
nop0    ; 010 110 00  68 copy procedure exit template
nop1    ; 010 110 01  69 copy procedure exit template
nop1    ; 010 110 01  70 copy procedure exit template
popcx   ; 010 110 12  71 pop cx off stack (size)
popbx   ; 010 110 11  72 pop bx off stack (start address of mother)
popax   ; 010 110 10  73 pop ax off stack (start address of daughter)
ret     ; 010 110 17  74 return from copy procedure
nop1    ; 010 100 01  75 end template
nop1    ; 010 100 01  76 end template
nop1    ; 010 100 01  77 end template
nop0    ; 010 100 00  78 end template
ifz     ; 010 000 05  79 dummy instruction to separate creature


*/
	case 1: // MOV	 Reg8/Mem8    Reg8	
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					Reg8[SecondRegister]=Reg8[FirstRegister];
					IP+=2;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel,Reg8[FirstRegister]);
					IP+=2+length;
				};break;
			};
		};break;
	case 2: // MOV Reg8 Mem8 
		{
			Reg8[FirstRegister]=mem1;
			IP+=2+length;
		};break;
	case 3: // MOV Reg16	Reg16/Mem16
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					Reg16[SecondRegister]=Reg16[FirstRegister];
					IP+=2;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel+1,(unsigned char)(Reg16[FirstRegister]&&255));
					WriteC(StartPosition+MemZiel,(unsigned char)(Reg16[FirstRegister]>>8));
					IP+=2+length;
				};break;
			};
		};break;
	case 4: // MOV Reg16 Mem16
		{
			Reg16[FirstRegister]=(((unsigned short)mem1)<<8)+(unsigned short)mem2;
			IP+=2+length;
		};break;
	case 5: // MOV AL, Mem8
		{
			Reg8[0]=mem1;
			IP+=2+length;
		};break;
	case 6: //MOV Mem8, AL
		{
			WriteC(StartPosition+MemZiel,Reg8[0]);
			IP+=2+length;
		};break;
	case 7: //MOV AX, Mem16
		{
			Reg16[0]=(((unsigned short)mem1)<<8)+(unsigned short)mem2;
			IP+=2+length;
		};break;
	case 8: //MOV Mem16, AX
		{
			WriteC(StartPosition+MemZiel+1,(unsigned char)(Reg16[0]&&255));
			WriteC(StartPosition+MemZiel,(unsigned char)(Reg16[0]>>8));
			IP+=2+length;
		};break;
	case 9: //MOV Reg8, Imm8
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		{
			Reg8[Befehl-9]=par1;
			IP+=2;
		};break;
	case 17: //MOV Reg16, Imm16
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
		{
			Reg16[Befehl-17]=(((unsigned short)par1)<<8)+(unsigned short)par2;
			IP+=3;
		};break;
	case 25: // MOV Reg8/Mem8 Imm8
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					Reg8[SecondRegister]=par2;
					IP+=3;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel,par2l);//?
					IP+=3+length;
				};break;
			}
		};break;
	case 26: // Mov Reg16/Mem16 Imm16
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					Reg16[SecondRegister]=(((unsigned short)par2)<<8)+(unsigned short)par3;
					IP+=4;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel,par2l);
					WriteC(StartPosition+MemZiel+1,par3l);
					IP+=4+length;
				};break;
			}
		};break;
	case 27: //XCHG	Reg8/Mem8	Reg8/Mem8	Reg8/Mem8	Nur: Reg8<>Reg8 oder Mem8...	
	{
		switch(SecondReg)
		{
			case TRUE:
			{
				tempc=Reg8[FirstRegister];
				Reg8[FirstRegister]=Reg8[SecondRegister];
				Reg8[SecondRegister]=tempc;
				IP+=2;
			};break;
			case FALSE:
			{
				tempc=mem1;
				WriteC(StartPosition+MemZiel,mem12);
				WriteC(StartPosition+MemZiel2,tempc);
				IP+=2+length;
			};break;
		}
	};break;
	case 28: //XCHG		Reg16/Mem16	Reg16/Mem16	Reg16/Mem16

	{
		switch(SecondReg)
		{
			case TRUE:
			{
				tempw=Reg16[FirstRegister];
				Reg16[FirstRegister]=Reg16[SecondRegister];
				Reg16[SecondRegister]=tempw;
				IP+=2;
			};break;
			case FALSE:
			{
				tempc=mem1;
				WriteC(StartPosition+MemZiel,mem12);
				WriteC(StartPosition+MemZiel2,tempc);
				tempc=mem2;
				
				WriteC(StartPosition+MemZiel+1,mem22);
				WriteC(StartPosition+MemZiel2+1,tempc);
				IP+=2+length;
			};break;
		}
	};break;



case 29://POP	Mem16	<- Stack 
	{
//		WriteC(StartPosition+MemZiel,(unsigned char)(Stack[Reg16[4]%STACK_SIZE]>>8));
//		WriteC(StartPosition+MemZiel+1,(unsigned char)(Stack[Reg16[4]%STACK_SIZE]&&255));
		Reg16[4]--;
		IP+=2+length;
	};break;
case 30:
case 31:
case 32:
case 33:
case 34:
case 35:
case 36:
case 37://POP Reg16 <- Stack
	{
//		if(Stack) Reg16[Befehl-30]=Stack[Reg16[4]%STACK_SIZE];
		Reg16[4]--;
		IP++;
	};break;

// Noch Soup[...%Memsize] einfügen!
case 38: //POPF
	{
//		if(Stack) flags=Stack[Reg16[4]%STACK_SIZE];
		Reg16[4]--;
		IP++;
	};break;
case 39: //PUSHF
	{
		Reg16[4]++;
//		if(Stack)  Stack[Reg16[4]%STACK_SIZE]=flags;
		IP++;
	};break;
case 40://PUSH	Stack <- Mem16
	{
		Reg16[4]++;
//		if(Stack) Stack[Reg16[4]%STACK_SIZE]=(mem1<<8)+mem2;
		IP+=2+length;
	};break;
case 41:
case 42:
case 43:
case 44:
case 45:
case 46:
case 47:
case 48://PUSH Stack <- Reg16
	{
		Reg16[4]++;
//		if(Stack) Stack[Reg16[4]%STACK_SIZE]=Reg16[Befehl-41];
		IP++;
	};break;







case 49://ADD	Reg8	Reg8	Reg8 / Mem8
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg8[FirstRegister]+=Reg8[SecondRegister];
					if(Reg8[FirstRegister]<Reg8[SecondRegister])
						flags=(flags|FLAG_CARRY);
					if(Reg8[FirstRegister]==0)
						flags=(flags|FLAG_ZERO);
					IP+=2;
				};break;
			case FALSE:
				{
					Reg8[FirstRegister]+=mem1;
					if(Reg8[FirstRegister]<mem1)
						flags=(flags|FLAG_CARRY);
					if(Reg8[FirstRegister]==0)
						flags=(flags|FLAG_ZERO);
					IP+=2+length;
				};break;
		}
	};break;
case 50://ADD	Mem8	Reg8	Mem8
	{
		WriteC(StartPosition+MemZiel,mem1+Reg8[FirstRegister]);
		if(mem1<Reg8[FirstRegister])
			flags=(flags|FLAG_CARRY);
		if(mem1==0)
			flags=(flags|FLAG_ZERO);
		IP+=2+length;
	};break;
case 51://ADD	Reg16	Reg16	Reg16 / Mem16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg16[FirstRegister]+=Reg16[SecondRegister];
					if(Reg16[FirstRegister]<Reg16[SecondRegister])
						flags=(flags|FLAG_CARRY);
					if(Reg16[FirstRegister]==0)
						flags=(flags|FLAG_ZERO);
					IP+=2;
				};break;
			case FALSE:
				{
					Reg16[FirstRegister]+=(((unsigned short)mem1)<<8)+(unsigned short)mem2;
					if(Reg16[FirstRegister]<(((unsigned short)mem1)<<8)+(unsigned short)mem2)
						flags=(flags|FLAG_CARRY);
					if(Reg16[FirstRegister]==0)
						flags=(flags|FLAG_ZERO);
					IP+=2+length;
				};break; //noch Write Protection einsetzen
		}
	};break;
case 52://ADD	Mem16	Reg16	Mem16
	{
		tempc=(mem1);
		
		WriteC(StartPosition+MemZiel+1,mem2+(unsigned char)(Reg16[FirstRegister]&255));
		
		if(mem2<(Reg16[FirstRegister]&255))
			tempc++;
		WriteC(StartPosition+MemZiel,tempc+(unsigned char)(Reg16[FirstRegister]>>8));

		if(mem1<tempc+(Reg16[FirstRegister]>>8))
			flags=(flags|FLAG_CARRY);
		if(mem1+mem2==0)
			flags=(flags|FLAG_ZERO);
		IP+=2+length;
	};break;
case 53://ADD AL	Immed8			
	{
		Reg8[0]+=par1;
		if(Reg8[0]<par1)
			flags=(flags|FLAG_CARRY);
		IP+=2;
	};break;
case 54://ADD AX	Immed16
	{
		Reg16[0]+=(((unsigned short)par1)<<8)+(unsigned short)par2;
		if(Reg16[0]<(((unsigned short)par1)<<8)+(unsigned short)par2)
			flags=(flags|FLAG_CARRY);
		IP+=3;
	};break;
case 55://ADD	Reg8/Mem8	Reg8/Mem8	Immed8		
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg8[SecondRegister]+=par2;
					if(Reg8[SecondRegister]<par2)
						flags=(flags|FLAG_CARRY);
					if(Reg8[SecondRegister]==0)
						flags=(flags|FLAG_ZERO);
					IP+=3;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel,mem1+par2l);
					if(mem1<par2l)
						flags=(flags|FLAG_CARRY);
					if(mem1==0)
						flags=(flags|FLAG_ZERO);
					IP+=3+length;
				};break;
		}
	};break;
case 56://ADD	Reg16/Mem16	Reg16/Mem16	Immed16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg16[SecondRegister]+=(((unsigned short)par2)<<8)+(unsigned short)par3;
					if(Reg16[SecondRegister]<(((unsigned short)par2)<<8)+(unsigned short)par3)
						flags=(flags|FLAG_CARRY);
					if(Reg16[SecondRegister]==0)
						flags=(flags|FLAG_ZERO);
					IP+=4;
				};break;
			case FALSE:
				{
					tempc=(mem1);
					WriteC(StartPosition+MemZiel+1,par2l+mem2);
					if(mem2<par2l)
						tempc++;
					WriteC(StartPosition+MemZiel,tempc+par1l);
					if(mem1<(tempc+par1l))
						flags=(flags|FLAG_CARRY);
					if((mem1<<8)+mem2==0)
						flags=(flags|FLAG_ZERO);
					IP+=4+length;
				};break;
		}
	};break;
case 57://ADD	Reg16/Mem16	Reg16/Mem16	Immed8
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg16[SecondRegister]+=(unsigned short)par2;
					if(Reg16[SecondRegister]<(unsigned short)par2)
						flags=(flags|FLAG_CARRY);
					if(Reg16[SecondRegister]==0)
						flags=(flags|FLAG_ZERO);
					IP+=3;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel+1,par2l+mem2);
					if(mem2<par2l)
						WriteC(StartPosition+MemZiel,mem1+1);
					if(mem1==0)
					{
						flags=(flags|FLAG_CARRY);
						flags=(flags|FLAG_ZERO);
					}
					IP+=3+length;
				};break;
		}
	};break;






	case 58://INC Reg8/Mem8
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					Reg8[SecondRegister]++;
					if(Reg8[SecondRegister]==0)
					{
						flags|=FLAG_ZERO;
						flags|=FLAG_CARRY;
					}
					else 
					{
						flags-=(flags&FLAG_CARRY);
						flags-=(flags&FLAG_ZERO);
					}
					IP+=2;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel,mem1+1);
					if(mem1==0)
					{
						flags|=FLAG_ZERO;
						flags|=FLAG_CARRY;
					}
					else 
					{
						flags-=(flags&FLAG_CARRY);
						flags-=(flags&FLAG_ZERO);
					}
					IP+=2+length;
				};break;
			};
		};break;
	case 59:
	case 60:
	case 61:
	case 62:
	case 63:
	case 64:
	case 65:
	case 66://INC AX, BX, ...
		{
			Reg16[Befehl-59]++; // ROFL GP hats erkannt ;-}
			if(Reg16[Befehl-59]==0)
			{
				flags|=FLAG_CARRY;
				flags|=FLAG_ZERO;
			}
			else 
			{
				flags-=(flags&FLAG_CARRY);
				flags-=(flags&FLAG_ZERO);
			}
			IP++;
		};break;

	case 67:
	case 68:
	case 69:
	case 70:
	case 71:
	case 72:
	case 73:
	case 74://DEC AX, BX, ...
		{
			Reg16[Befehl-67]--;
			if(Reg16[Befehl-67]==65535)
				flags|=FLAG_CARRY;
			else flags-=(flags&FLAG_CARRY);
			if(Reg16[Befehl-67]==0)
				flags|=FLAG_ZERO;
			else flags-=(flags&FLAG_ZERO);
			IP++;
		}
	case 75://DEC Reg8/Mem8
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					Reg8[SecondRegister]--;
					if(Reg8[SecondRegister]==255)
						flags|=FLAG_CARRY;
					else flags-=(flags&FLAG_CARRY);
					if(Reg8[SecondRegister]==0)
						flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);
					IP+=2;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel,mem1-1);
					if(mem1==255)
						flags|=FLAG_CARRY;
					else flags-=(flags&FLAG_CARRY);
					if(mem1==0)
						flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);
					IP+=2+length;
				};break;
			};
		};break;
	case 76://SUB	Reg8	Reg8	Reg8 / Mem8
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					if(Reg8[FirstRegister]<Reg8[SecondRegister])
						flags|=FLAG_CARRY;
					Reg8[FirstRegister]-=Reg8[SecondRegister];
					if(Reg8[FirstRegister]==0)
						flags|=FLAG_ZERO;
					IP+=2+length;
				};break;
			case FALSE:
				{
					if(Reg8[FirstRegister]<mem1)
						flags|=FLAG_CARRY;
					Reg8[FirstRegister]-=mem1;
					if(Reg8[FirstRegister]==0)
						flags|=FLAG_ZERO;
					IP+=2+length;
				};break;
		}
	};break;
case 77://SUB	Mem8	Reg8	Mem8
	{
		if(mem1<Reg8[FirstRegister])
			flags|=FLAG_CARRY;
		WriteC(StartPosition+MemZiel,mem1-Reg8[FirstRegister]);
		if(mem1==0)
			flags|=FLAG_ZERO;
		IP+=2+length;
	};break;
case 78://SUB	Reg16	Reg16	Reg16 / Mem16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					if(Reg16[FirstRegister]<Reg16[SecondRegister])
						flags|=FLAG_CARRY;
					Reg16[FirstRegister]-=Reg16[SecondRegister];
					if(Reg16[FirstRegister]==0)
						flags|=FLAG_ZERO;
					IP+=2+length;
				};break;
			case FALSE:
				{
					if(Reg16[FirstRegister]< (((unsigned short)mem1)<<8) +(unsigned short)mem2)
						flags|=FLAG_CARRY;
					Reg16[FirstRegister]-=(((unsigned short)mem1)<<8)+(unsigned short)mem2;
					if(Reg16[FirstRegister]==0)
						flags|=FLAG_ZERO;
					IP+=2+length;
				};break; //noch Write Protection einsetzen
		}
	};break;
case 79://SUB	Mem16	Reg16	Mem16
	{
		tempc=(mem1);
		if(mem2<(unsigned char)(Reg16[FirstRegister]&255))
			tempc--;
		//~~~~~~ ??
		WriteC(StartPosition+MemZiel+1,mem2-(unsigned char)(Reg16[FirstRegister]&255));
		if(mem1<(unsigned char)(Reg16[FirstRegister]>>8)+tempc-mem1)
			flags|=FLAG_CARRY;
		WriteC(StartPosition+MemZiel,tempc-(unsigned char)(Reg16[FirstRegister]>>8));
		if(mem1+mem2==0)
			flags|=FLAG_ZERO;
		IP+=2+length;
	};break;
case 80://SUB AL	Immed8			
	{
		if(Reg8[0]<par1)
			flags|=FLAG_CARRY;
		Reg8[0]-=par1;
		IP+=2;
	};break;
case 81://SUB AX	Immed16
	{
		if(Reg16[0]<(((unsigned short)par1)<<8)+(unsigned short)par2)
			flags|=FLAG_CARRY;
		Reg16[0]-=(((unsigned short)par1)<<8)+((unsigned short)par2);
		IP+=3;
	};break;
case 82://SUB	Reg8/Mem8	Reg8/Mem8	Immed8		
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					if(Reg8[SecondRegister]<par2)
						flags|=FLAG_CARRY;
					Reg8[SecondRegister]-=par2;
					if(Reg8[SecondRegister]==0)
						flags|=FLAG_ZERO;
					IP+=3;
				};break;
			case FALSE:
				{
					if(mem1<par2l)
						flags|=FLAG_CARRY;
					WriteC(StartPosition+MemZiel,mem1-par2l);
					if(mem1==0)
						flags|=FLAG_ZERO;
					IP+=3+length;
				};break;
		}
	};break;
case 83://SUB	Reg16/Mem16	Reg16/Mem16	Immed16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					if(Reg16[SecondRegister]<(((unsigned short)par2)<<8)+par3)
						flags|=FLAG_CARRY;
					Reg16[SecondRegister]-=(par2<<8)+par3;
					if(Reg16[SecondRegister]==0)
						flags|=FLAG_ZERO;
					IP+=4;
				};break;
			case FALSE:
				{
					tempc=mem1;
					if(mem2<par2l)
						tempc--;
					WriteC(StartPosition+MemZiel+1,mem2-par2l);
					if(mem1<(par1l+tempc-mem1))
						flags|=FLAG_CARRY;
					WriteC(StartPosition+MemZiel,tempc-par1l);
					if((((unsigned short)mem1)<<8) +mem2==0)
						flags|=FLAG_ZERO;
					IP+=4+length;
				};break;
		}
	};break;
case 84://SUB	Reg16/Mem16	Reg16/Mem16	Immed8
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					if(Reg16[SecondRegister]<par2)
						flags|=FLAG_CARRY;
					Reg16[SecondRegister]-=par2;
					if(Reg16[SecondRegister]==0)
						flags|=FLAG_ZERO;
					IP+=3;
				};break;
			case FALSE:
				{
					if(mem2<par2l)
						WriteC(StartPosition+MemZiel,mem1-1);
					WriteC(StartPosition+MemZiel+1,mem2-par2l);
					if(mem1==0)
					{
						flags|=FLAG_CARRY;
						flags|=FLAG_ZERO;
					}
					IP+=3+length;
				};break;
		}
	};break;
	case 85: // DIV Reg8/Mem8		AX / Reg8/Mem8	
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					if(Reg8[SecondRegister]!=0)
					{
						flags-=(flags&FLAG_ERROR);
						Reg16[0]/=Reg8[SecondRegister];
					}
					else flags=(flags|FLAG_ERROR);
				};break;
			case FALSE:
				{
					if(mem1!=0)
					{
						flags-=(flags&FLAG_ERROR);
						Reg16[0]/=mem1;
					}
					else flags|=FLAG_ERROR;
				};break;
			}
		};break;


	case 86:  //DIV Reg16/Mem16		DX:AX / Reg16/Mem16	
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					if(Reg16[SecondRegister]!=0)
					{
					    unsigned long temp2;
						flags-=(flags&FLAG_ERROR);
						temp2=((Reg16[0]+(((unsigned long)Reg16[3])<<16))/Reg16[SecondRegister]);
						Reg16[0]=(unsigned short)(temp2&65535);
						Reg16[3]=(unsigned short)((temp2&4294901760)>>16);
					}
					else flags|=FLAG_ERROR;
				};break;
			case FALSE:
				{
					if(mem2+mem1!=0)
					{
					    unsigned long temp2;
						flags-=(flags&FLAG_ERROR);
						temp2= ( (Reg16[0]+(((unsigned long)Reg16[3])<<16)) /((((unsigned long)mem1)<<16)+mem2));
						Reg16[0]=(unsigned short)(temp2&65535);
						Reg16[3]=(unsigned short)((temp2&4294901760)>>16);
					}
					else flags|=FLAG_ERROR;
				};break;
			}
		};break;
case 87: // SHR Reg8/Mem8 1
	{
		switch(SecondReg)
		{
		case TRUE:
			{
				if((Reg8[SecondRegister]&1)==1)
					flags|=FLAG_CARRY;
				else flags-=(flags&FLAG_CARRY);
				Reg8[SecondRegister]>>=1;
				IP+=2;
			};break;
		case FALSE:
			{
				if((mem1&1)==1)
					flags|=FLAG_CARRY;
				else flags-=(flags&FLAG_CARRY);
				WriteC(StartPosition+MemZiel,mem1>>1);
				IP+=2;
			};break;
		};
	};break;
case 88: //SHR Reg16/Mem16 1
	{
		switch(SecondReg)
		{
		case TRUE:
			{
				if((Reg16[SecondRegister]&1)==1)
					flags|=FLAG_CARRY;
				else flags-=(flags&FLAG_CARRY);
				Reg16[SecondRegister]>>=1;
				IP+=2;
			};break;
		case FALSE:
			{
				if((mem2&1)==1)
					flags|=FLAG_CARRY;
				else flags-=(flags&FLAG_CARRY);
				WriteC(StartPosition+MemZiel+1,mem1>>1);
				if((mem1&1)==1)
					WriteC(StartPosition+MemZiel+1,mem2|128);
				WriteC(StartPosition+MemZiel,mem1>>1);
				IP+=2+length;
			};break;
		};
	};break;
case 89: // SHR Reg8/Mem8 CL
	{
		switch(SecondReg)
		{
		case TRUE:
			{
				if((Reg8[SecondRegister]&1)==1)
				{
					if(Reg8[1]==1)
					{
						flags|=FLAG_CARRY;
						flags-=(flags&FLAG_OVERFLOW);
					}
					else 
					{ 
						flags-=(flags&FLAG_CARRY);
						if(Reg8[1]>1)
							flags|=FLAG_OVERFLOW;
					};
				};
				Reg8[SecondRegister]>>=Reg8[1];
				IP+=2;
			};break;
		case FALSE:
			{
				if((mem1&1)==1)
				{
					if(Reg8[1]==1)
					{
						flags|=FLAG_CARRY;
						flags-=(flags&FLAG_OVERFLOW);
					}
					else 
					{
						flags-=(flags&FLAG_CARRY);
						if(Reg8[1]>1)
							flags|=FLAG_OVERFLOW;
					};
				};
				WriteC(StartPosition+MemZiel,mem1>>Reg8[1]);
				IP+=2+length;
			};break;
		};
	};break;
case 90: //SHR Reg16/Mem16 CL
	{
		switch(SecondReg)
		{
		case TRUE:
			{
				if((Reg16[SecondRegister]&1)==1)
				{
					if(Reg8[1]==1)
					{
						flags|=FLAG_CARRY;
						flags-=(flags&FLAG_OVERFLOW);
					}
					else 
					{
						flags-=(flags&FLAG_CARRY);
						if(Reg8[1]>1)
							flags|=FLAG_OVERFLOW;
					};
				};
				Reg16[SecondRegister]>>=Reg8[1];
				IP+=2;
			};break;
		case FALSE:
			{
				if((mem2&1)==1)
				{
					if(Reg8[1]==1)
					{
						flags|=FLAG_CARRY;
						flags-=(flags&FLAG_OVERFLOW);
					}
					else 
					{
						flags-=(flags&FLAG_CARRY);
						if(Reg8[1]>1)
							flags|=FLAG_OVERFLOW;
					};
				};
				WriteC(StartPosition+MemZiel+1,mem2>>Reg8[1]);
				unsigned short temp3;
				if(Reg8[1]>8) temp3=8;
					else temp3=Reg8[1];
				WriteC(StartPosition+MemZiel+1,mem2|(mem1<<(8-temp3)));
				//Carry... :P
				WriteC(StartPosition+MemZiel,mem1>>Reg8[1]);
				IP+=2+length;
			};break;
		};
	};break;	
case 91:// MUL	AX	Reg8/Mem8		AX * Reg8/Mem8	
		{
			switch(SecondReg)
			{
			case TRUE:
				{
						Reg16[0]=Reg16[0]*Reg8[SecondRegister];
						if(Reg16[0]==0) 
							flags|=FLAG_ZERO;
						else flags-=(flags&FLAG_ZERO);
						flags-=(flags&FLAG_ERROR);
						IP+=2;
				};break;
			case FALSE:
				{
						Reg16[0]=Reg16[0]*mem1;
						if(Reg16[0]==0) 
							flags|=FLAG_ZERO;
						else flags-=(flags&FLAG_ZERO);
						flags-=(flags&FLAG_ERROR);
						//Overflow flag noch rein
						IP+=2+length;
				};break;
			}
		};break;
case 92: // SHL Reg8/Mem8 1
	{
		switch(SecondReg)
		{
		case TRUE:
			{
				if((Reg8[SecondRegister]&128)==128)
					flags|=FLAG_CARRY;
				else flags-=(flags&FLAG_CARRY);
				Reg8[SecondRegister]<<=1;
				IP+=2;
			};break;
		case FALSE:
			{
				if((mem1&128)==128)
					flags|=FLAG_CARRY;
				else flags-=(flags&FLAG_CARRY);
				WriteC(StartPosition+MemZiel,mem1<<1);
				IP+=2;
			};break;
		};
	};break;
case 93: //SHL Reg16/Mem16 1
	{
		switch(SecondReg)
		{
		case TRUE:
			{
				if((Reg16[SecondRegister]&32768)==32768)
					flags|=FLAG_CARRY;
				else flags-=(flags&FLAG_CARRY); // Overflow???
				Reg16[SecondRegister]<<=1;
				IP+=2;
			};break;
		case FALSE:
			{
				
				if((mem1&128)==128)
					flags|=FLAG_CARRY;
				else flags-=(flags&FLAG_CARRY);
				WriteC(StartPosition+MemZiel,mem1<<1);
				
				if((mem2&128)==128)
					WriteC(StartPosition+MemZiel,mem1|1);
				WriteC(StartPosition+MemZiel+1,mem2<<1);
				IP+=2+length;
			};break;
		};
	};break;
case 94: // SHL Reg8/Mem8 CL
	{
		switch(SecondReg)
		{
		case TRUE:
			{
				if((Reg8[SecondRegister]&128)==128)
				{
					if(Reg8[1]==1)
					{
						flags|=FLAG_CARRY;
						flags-=(flags&FLAG_OVERFLOW);
					}
					else 
					{ 
						flags-=(flags&FLAG_CARRY);
						if(Reg8[1]>1)
							flags|=FLAG_OVERFLOW;
					};
				};
				Reg8[SecondRegister]<<=Reg8[1];
				IP+=2;
			};break;
		case FALSE:
			{
				if((mem1&128)==128)
				{
					if(Reg8[1]==1)
					{
						flags|=FLAG_CARRY;
						flags-=(flags&FLAG_OVERFLOW);
					}
					else 
					{
						flags-=(flags&FLAG_CARRY);
						if(Reg8[1]>1)
							flags|=FLAG_OVERFLOW;
					};
				};
				WriteC(StartPosition+MemZiel,mem1<<Reg8[1]);
				IP+=2+length;
			};break;
		};
	};break;
case 95: //SHL Reg16/Mem16 CL
	{
		switch(SecondReg)
		{
		case TRUE:
			{
				if((Reg16[SecondRegister]&32768)==32768)
				{
					if(Reg8[1]==1)
					{
						flags|=FLAG_CARRY;
						flags-=(flags&FLAG_OVERFLOW);
					}
					else 
					{
						flags-=(flags&FLAG_CARRY);
						if(Reg8[1]>1)
							flags|=FLAG_OVERFLOW;
					};
				};
				Reg16[SecondRegister]<<=Reg8[1];
				IP+=2;
			};break;
		case FALSE:
			{
				if((mem1&128)==128)
				{
					if(Reg8[1]==1)
					{
						flags|=FLAG_CARRY;
						flags-=(flags&FLAG_OVERFLOW);
					}
					else 
					{
						flags-=(flags&FLAG_CARRY);
						if(Reg8[1]>1)
							flags|=FLAG_OVERFLOW;
					};
				};
				WriteC(StartPosition+MemZiel,mem1<<Reg8[1]);
				short temp3;
				if(Reg8[1]>8) temp3=8;
					else temp3=Reg8[1];
				
				WriteC(StartPosition+MemZiel,mem2>>(8-temp3));
				//Carry... :P
				WriteC(StartPosition+MemZiel+1,mem2<<Reg8[1]);
				IP+=2+length;
			};break;
		};
	};break;



	case 96:  //MUL 	DX:AX	Reg16/Mem16		DX:AX * Reg16/Mem16	
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					    unsigned long temp2;
						temp2=((Reg16[0]+(((unsigned long)Reg16[3])<<16))*Reg16[SecondRegister]);
						Reg16[0]=(unsigned short)(temp2&65535);
						Reg16[3]=(unsigned short)((temp2&4294901760)>>16);
						flags-=(flags&FLAG_ERROR);
						IP+=2;
				};break;
			case FALSE:
				{
					    unsigned long temp2;
						temp2=((Reg16[0]+(((unsigned long)Reg16[3])<<16))*((((unsigned long)mem1)<<16)+mem2));
						Reg16[0]=(unsigned short)(temp2&65535);
						Reg16[3]=(unsigned short)((temp2&4294901760)>>16);
						if(temp2==0) 
							flags|=FLAG_ZERO;
						else flags-=(flags&FLAG_ZERO);
						if((((unsigned long)mem1)<<16)+mem2!=0)
						{
							if((temp2/(((((unsigned long)mem1)<<16))+mem2))!=((Reg16[0]+(((unsigned long)Reg16[3])<<16))))
								flags|=FLAG_OVERFLOW;
						else flags-=(flags&FLAG_OVERFLOW);
						}
						flags-=(flags&FLAG_ERROR);
						IP+=2+length;
				};break;
			}
		};break;

















case 97://AND	Reg8	Reg8	Reg8 / Mem8
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg8[FirstRegister]&=Reg8[SecondRegister];
					IP+=2+length;
				};break;
			case FALSE:
				{
					Reg8[FirstRegister]&=mem1;
					IP+=2+length;
				};break;
		}
	};break;
case 98: //AND	Mem8	Reg8	Mem8
	{
		WriteC(StartPosition+MemZiel,mem1&Reg8[FirstRegister]);
		IP+=2+length;
	};break;
case 99://AND	Reg16	Reg16	Reg16 / Mem16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg16[FirstRegister]&=Reg16[SecondRegister];
					IP+=2+length;
				};break;
			case FALSE:
				{
					Reg16[FirstRegister]&=(mem1<<8)+mem2;
					IP+=2+length;
				};break; //noch Write Protection einsetzen
		}
	};break;
case 100://AND	Mem16	Reg16	Mem16
	{
		WriteC(StartPosition+MemZiel+1,mem2&((unsigned char)(Reg16[FirstRegister]&255)));
		WriteC(StartPosition+MemZiel,mem1&((unsigned char)(Reg16[FirstRegister]>>8)));
		IP+=2+length;
	};break;
case 101://AND AL	Immed8			
	{
		Reg8[0]&=par1;
		IP+=2;
	};break;
case 102://AND AX	Immed16
	{
		Reg16[0]&=(par1<<8)+255;
		Reg16[0]&=par2+65280;
		IP+=3;
	};break;
case 103://AND	Reg8/Mem8	Reg8/Mem8	Immed8		
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg8[SecondRegister]&=par2;
					IP+=3;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel,par2l&mem1);
					IP+=3+length;
				};break;
		}
	};break;
case 104://AND	Reg16/Mem16	Reg16/Mem16	Immed16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg16[SecondRegister]=(((unsigned short)par2)<<8)+par3;
					IP+=4;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel+1,mem2&par2l);
					WriteC(StartPosition+MemZiel,mem1&par1l);
					IP+=4+length;
				};break;
		}
	};break;

case 105://OR	Reg8	Reg8	Reg8 / Mem8
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg8[FirstRegister]|=Reg8[SecondRegister];
					IP+=2;
				};break;
			case FALSE:
				{
					Reg8[FirstRegister]|=mem1;
					IP+=2+length;
				};break;
		}
	};break;
case 106: //OR	Mem8	Reg8	Mem8
	{
		WriteC(StartPosition+MemZiel,mem1|Reg8[FirstRegister]);
		IP+=2+length;
	};break;
case 107://OR	Reg16	Reg16	Reg16 / Mem16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg16[FirstRegister]|=Reg16[SecondRegister];
					IP+=2;
				};break;
			case FALSE:
				{
					Reg16[FirstRegister]|=(mem1<<8)+mem2;
					IP+=2+length;
				};break; //noch Write Protection einsetzen
		}
	};break;
case 108://OR	Mem16	Reg16	Mem16
	{
		WriteC(StartPosition+MemZiel+1,mem2|(unsigned char)(Reg16[FirstRegister]&255));
		WriteC(StartPosition+MemZiel,mem1|(unsigned char)(Reg16[FirstRegister]>>8));
		IP+=2+length;
	};break;
case 109://OR AL	Immed8			
	{
		Reg8[0]|=par1;
		IP+=2;
	};break;
case 110://OR AX	Immed16
	{
		Reg16[0]|=(par1<<8)+255;
		Reg16[0]|=par2+65280;
		IP+=3;
	};break;
case 111://OR	Reg8/Mem8	Reg8/Mem8	Immed8		
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg8[SecondRegister]|=par2;
					IP+=3;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel,mem1|par2l);
					IP+=3+length;
				};break;
		}
	};break;
case 112://OR	Reg16/Mem16	Reg16/Mem16	Immed16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg16[SecondRegister]|=(((unsigned short)par2)<<8)+par3;
					IP+=4;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel+1,mem2|par2l);
					WriteC(StartPosition+MemZiel,mem1|par1l);
					IP+=4+length;
				};break;
		};
	};break;





case 113://XOR	Reg8	Reg8	Reg8 / Mem8
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg8[FirstRegister]^=Reg8[SecondRegister];
					IP+=2;
				};break;
			case FALSE:
				{
					Reg8[FirstRegister]^=mem1;
					IP+=2+length;
				};break;
		}
	};break;
case 114: //XOR	Mem8	Reg8	Mem8
	{
		WriteC(StartPosition+MemZiel,mem1^Reg8[FirstRegister]);
		IP+=2+length;
	};break;
case 115://XOR	Reg16	Reg16	Reg16 / Mem16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg16[FirstRegister]^=Reg16[SecondRegister];
					IP+=2;
				};break;
			case FALSE:
				{
					Reg16[FirstRegister]^=(unsigned short)(mem1<<8)+mem2;
					IP+=2+length;
				};break; //noch Write Protection einsetzen
		}
	};break;
case 116://XOR	Mem16	Reg16	Mem16
	{
		WriteC(StartPosition+MemZiel+1,mem2^(unsigned char)(Reg16[FirstRegister]&255));
		WriteC(StartPosition+MemZiel,mem1^(unsigned char)(Reg16[FirstRegister]>>8));
		IP+=2+length;
	};break;
case 117://XOR AL	Immed8			
	{
		Reg8[0]^=par1;
		IP+=2;
	};break;
case 118://XOR AX	Immed16
	{
		Reg16[0]^=((unsigned short)par1<<8)+255;
		Reg16[0]^=(unsigned short)par2+65280;
		IP+=3;
	};break;
case 119://XOR	Reg8/Mem8	Reg8/Mem8	Immed8		
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg8[SecondRegister]^=par2;
					IP+=3;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel,mem1^par2l);
					IP+=3+length;
				};break;
		}
	};break;
case 120://XOR	Reg16/Mem16	Reg16/Mem16	Immed16
	{
		switch(SecondReg)
		{
			case TRUE:
				{
					Reg16[SecondRegister]^=(((unsigned short)par2)<<8)+par3;
					IP+=4;
				};break;
			case FALSE:
				{
					WriteC(StartPosition+MemZiel+1,mem2^par2l);
					WriteC(StartPosition+MemZiel,mem1^par1l);
					IP+=4+length;
				};break;
		};
	};break;
case 121://NOT	Reg8/Mem8	Reg8/Mem8			

		{
			switch(SecondReg)
			{
			case TRUE:
				{
					flags-=(flags&FLAG_ERROR);
					Reg8[SecondRegister]=(!Reg8[SecondRegister]);
					if(Reg8[SecondRegister]==0)
						flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);
					IP+=2;
				};break;
			case FALSE:
				{
					flags-=(flags&FLAG_ERROR);
					WriteC(StartPosition+MemZiel,!mem1);
					if(mem1==0)
						flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);
					IP+=2+length;
				};break;
			}
		};break;

	case 122: //NOT Reg16/Mem16	Reg16/Mem16			
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					flags-=(flags&FLAG_ERROR);
					Reg16[SecondRegister]=(!Reg16[SecondRegister]);
					if(Reg8[SecondRegister]==0)
						flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);
					IP+=2;
				};break;
			case FALSE:
				{
					flags-=(flags&FLAG_ERROR);
					WriteC(StartPosition+MemZiel,!mem1);
					WriteC(StartPosition+MemZiel+1,!mem2);
					if(mem1+mem2==0)
						flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);
					IP+=2+length;
				};break;
			}
		};break;



	case 123: // LAXF
		{
			Reg16[0]=flags;
			IP++;
		};break;
	case 124: // SAXF
	{
		flags=Reg16[0];
		IP++;
	};break;
	
	case 125: //SCF
	{
		flags|=FLAG_CARRY;
		IP++;
	};break;
	
	case 126: //SDF
	{
		flags|=FLAG_DIRECTION;
		IP++;
	};break;





	case 127: //Clear Flags...
		{
			flags-=(flags&1);
			IP++;
		};break;
	case 128: //Clear Flags...
		{
			flags-=(flags&2);
			IP++;
		};break;

	case 129: //Clear Flags...
		{
			flags-=(flags&4);
			IP++;
		};break;

	case 130: //Clear Flags...
		{
			flags-=(flags&8);
			IP++;
		};break;

	case 131: //Clear Flags...
		{
			flags-=(flags&16);
			IP++;
		};break;

	case 132: //Clear Flags...
		{
			flags-=(flags&32);
			IP++;
		};break;

	case 133: //Clear Flags...
		{
			flags-=(flags&64);
			IP++;
		};break;

	case 134: //Clear Flags...
		{
			flags-=(flags&128);
			IP++;
		};break;
	case 135://CMP	Flags	Reg8	Reg8/Mem8		
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					if(Reg8[FirstRegister]==0)
						flags|=FLAG_ZERO;
						else flags-=(flags&FLAG_ZERO);

					if((Reg8[FirstRegister]&128)==128)
						flags-=(flags&FLAG_POSITIVE);
						else flags|=FLAG_POSITIVE;
					
					if(Reg8[FirstRegister]==Reg8[SecondRegister])
						flags|=FLAG_EQUAL;
						else flags-=(flags&FLAG_EQUAL);

					if(Reg8[FirstRegister]<Reg8[SecondRegister])
						flags-=(flags&FLAG_GREATER);
						else flags|=FLAG_GREATER;
					IP+=2;
				};break;
			case FALSE:
				{
					if(Reg8[FirstRegister]==0)
						flags|=FLAG_ZERO;
						else flags-=(flags&FLAG_ZERO);

					if((Reg8[FirstRegister]&128)==128)
						flags-=(flags&FLAG_POSITIVE);
						else flags|=FLAG_POSITIVE;
					
					if(Reg8[FirstRegister]==mem1)
						flags|=FLAG_EQUAL;
						else flags-=(flags&FLAG_EQUAL);

					if(Reg8[FirstRegister]<mem1)
						flags-=(flags&FLAG_GREATER);
						else flags|=FLAG_GREATER;
					IP+=2+length;
				};break;
			}
		};break;

case 136: //CMP	Flags	Mem8	Reg8		
				{
					if(mem1==0)
						flags|=FLAG_ZERO;
						else flags-=(flags&FLAG_ZERO);

					if((mem1&128)==128)
						flags-=(flags&FLAG_POSITIVE);
						else flags|=FLAG_POSITIVE;
					
					if(Reg8[SecondRegister]==mem1)
						flags|=FLAG_EQUAL;
						else flags-=(flags&FLAG_EQUAL);

					if(Reg8[SecondRegister]>mem1)
						flags-=(flags&FLAG_GREATER);
						else flags|=FLAG_GREATER;
					IP+=2+length;
				};break;
	case 137://CMP	Flags	Reg16	Reg16/Mem16
		{
			switch(SecondReg)
			{
			case TRUE:
				{
					if(Reg16[FirstRegister]==0)
						flags|=FLAG_ZERO;
						else flags-=(flags&FLAG_ZERO);

					if((Reg16[FirstRegister]&32768)==32768)
						flags-=(flags&FLAG_POSITIVE);
						else flags|=FLAG_POSITIVE;
					
					if(Reg16[FirstRegister]==Reg16[SecondRegister])
						flags|=FLAG_EQUAL;
						else flags-=(flags&FLAG_EQUAL);

					if(Reg16[FirstRegister]<Reg16[SecondRegister])
						flags-=(flags&FLAG_GREATER);
						else flags|=FLAG_GREATER;
					IP+=2;
				};break;
			case FALSE:
				{
					if(Reg16[FirstRegister]==0)
						flags|=FLAG_ZERO;
						else flags-=(flags&FLAG_ZERO);

					if((Reg16[FirstRegister]&32768)==32768)
						flags-=(flags&FLAG_POSITIVE);
						else flags|=FLAG_POSITIVE;
					
					if(Reg16[FirstRegister]==((((unsigned short)(mem1))<<8)+mem2))
						flags|=FLAG_EQUAL;
						else flags-=(flags&FLAG_EQUAL);

					if(Reg16[FirstRegister]<((((unsigned short)(mem1))<<8)+mem2))
						flags-=(flags&FLAG_GREATER);
						else flags|=FLAG_GREATER;
					IP+=2+length;
				};break;
			}
		};break;
case 138: //CMP	Flags	Mem16	Reg16		
		{
					if((((unsigned short)mem1)<<8)+mem2==0)
						flags|=FLAG_ZERO;
						else flags-=(flags&FLAG_ZERO);

					if((((((unsigned short)mem1)<<8)+mem2)&128)==128)
						flags-=(flags&FLAG_POSITIVE);
						else flags|=FLAG_POSITIVE;
					
					if(Reg16[SecondRegister]==(((unsigned short)mem1)<<8)+mem2)
						flags|=FLAG_EQUAL;
						else flags-=(flags&FLAG_EQUAL);

					if(Reg16[SecondRegister]>(((unsigned short)mem1)<<8)+mem2)
						flags-=(flags&FLAG_GREATER);
						else flags|=FLAG_GREATER;
					IP+=2+length;
		};break;

case 139://CMP AL	Immed8			
	{
		if(Reg8[0]==0)
			flags|=FLAG_ZERO;
			else flags-=(flags&FLAG_ZERO);

		if((Reg8[0]&128)==128)
			flags-=(flags&FLAG_POSITIVE);
			else flags|=FLAG_POSITIVE;
				
		if(Reg8[0]==par1)
			flags|=FLAG_EQUAL;
			else flags-=(flags&FLAG_EQUAL);

		if(Reg8[0]<par1)
			flags-=(flags&FLAG_GREATER);
			else flags|=FLAG_GREATER;
		IP+=2;
	};break;
case 140://CMP AX	Immed16
	{
		if(Reg16[0]==0)
			flags|=FLAG_ZERO;
			else flags-=(flags&FLAG_ZERO);

		if((Reg16[0]&32768)==32768)
			flags-=(flags&FLAG_POSITIVE);
			else flags|=FLAG_POSITIVE;
				
		if(Reg16[0]==(((unsigned short)(par1))<<8)+par2)
			flags|=FLAG_EQUAL;
			else flags-=(flags&FLAG_EQUAL);

		if(Reg16[0]<(((unsigned short)(par1))<<8)+par2)
			flags-=(flags&FLAG_GREATER);
			else flags|=FLAG_GREATER;
		IP+=3;
	};break;

case 141://CMP	Flags	Reg8/Mem8	Immed8		
	{
	switch(SecondReg)
		{
		case TRUE:
			{
				if(Reg8[SecondRegister]==0)
					flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);

				if((Reg8[SecondRegister]&128)==128)
					flags-=(flags&FLAG_POSITIVE);
					else flags|=FLAG_POSITIVE;
				
				if(Reg8[SecondRegister]==par2)
					flags|=FLAG_EQUAL;
					else flags-=(flags&FLAG_EQUAL);

				if(Reg8[SecondRegister]<par2)
					flags-=(flags&FLAG_GREATER);
					else flags|=FLAG_GREATER;
				IP+=3;
			};break;
		case FALSE:
			{
				if(mem1==0)
					flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);

				if((mem1&128)==128)
					flags-=(flags&FLAG_POSITIVE);
					else flags|=FLAG_POSITIVE;
				
				if(mem1==par3l)
					flags|=FLAG_EQUAL;
					else flags-=(flags&FLAG_EQUAL);

				if(mem1<par3l)
					flags-=(flags&FLAG_GREATER);
					else flags|=FLAG_GREATER;
				IP+=3+length;
			};break;
	}

	};break;
case 142://CMP	Flags	Reg16/Mem16	Immed16		
	{
	switch(SecondReg)
		{
		case TRUE:
			{
				if(Reg16[SecondRegister]==0)
					flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);

				if((Reg16[SecondRegister]&32768)==32768)
					flags-=(flags&FLAG_POSITIVE);
					else flags|=FLAG_POSITIVE;
				
				if(Reg16[SecondRegister]==(((unsigned short)par2)<<8)+par3)
					flags|=FLAG_EQUAL;
					else flags-=(flags&FLAG_EQUAL);

				if(Reg16[SecondRegister]<(((unsigned short)par2)<<8)+par3)
					flags-=(flags&FLAG_GREATER);
					else flags|=FLAG_GREATER;
				IP+=4;
			};break;
		case FALSE:
			{
				if((((unsigned short)mem1)<<8)+mem2==0)
					flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);

				if( (((((unsigned short)mem1)<<8)+mem2)&128)==128)
					flags-=(flags&FLAG_POSITIVE);
					else flags|=FLAG_POSITIVE;
				
				if((((unsigned short)mem1)<<8)+mem2==(((unsigned short)par2l)<<8)+par3l)
					flags|=FLAG_EQUAL;
					else flags-=(flags&FLAG_EQUAL);

				if((((unsigned short)mem1)<<8)+mem2<(((unsigned short)par2l)<<8)+par3l)
					flags-=(flags&FLAG_GREATER);
					else flags|=FLAG_GREATER;
				IP+=4+length;
			};break;
	}

	};break;
case 143://CMP	Flags	Reg16/Mem16	Immed8		
	{
	switch(SecondReg)
		{
		case TRUE:
			{
				if(Reg16[SecondRegister]==0)
					flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);

				if((Reg16[SecondRegister]&32768)==32768)
					flags-=(flags&FLAG_POSITIVE);
					else flags|=FLAG_POSITIVE;
				
				if(Reg16[SecondRegister]==par2)
					flags|=FLAG_EQUAL;
					else flags-=(flags&FLAG_EQUAL);

				if(Reg16[SecondRegister]<par2)
					flags-=(flags&FLAG_GREATER);
					else flags|=FLAG_GREATER;
				IP+=3;
			};break;
		case FALSE:
			{
				if((((unsigned short)mem1)<<8)+mem2==0)
					flags|=FLAG_ZERO;
					else flags-=(flags&FLAG_ZERO);

				if((((((unsigned short)mem1)<<8)+mem2)&128)==128)
					flags-=(flags&FLAG_POSITIVE);
					else flags|=FLAG_POSITIVE;
				
				if((((unsigned short)mem1)<<8)+mem2==par2l)
					flags|=FLAG_EQUAL;
					else flags-=(flags&FLAG_EQUAL);

				if((((unsigned short)mem1)<<8)+mem2<par2l)
					flags-=(flags&FLAG_GREATER);
					else flags|=FLAG_GREATER;
				IP+=3+length;
			};break;
	}

	};break;


	//IP=tempw+2; // +2 damit man net in die Sprungmarke reinspringt

	case 144: //jump if above
		{
			if( ((flags&FLAG_GREATER)==FLAG_GREATER)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
				else IP+=3;
		};break;
	case 145: //jump if above or equal
		{
			if( ( ((flags&FLAG_GREATER)==FLAG_GREATER)||((flags&FLAG_EQUAL)==FLAG_EQUAL)) &&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 146://jump if below                       			
		{
			if(((flags&FLAG_GREATER)==0)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 147://jump below or equal
		{
			if((((flags&FLAG_GREATER)==0)||((flags&FLAG_EQUAL)==FLAG_EQUAL))&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 148://jump if equal                       			
		{
			if(((flags&FLAG_EQUAL)==FLAG_EQUAL)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 149://	jump if not equal                   			
		{
			if(((flags&FLAG_EQUAL)==0)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 150://jump if carry set                   			
		{
			if(((flags&FLAG_CARRY)==FLAG_CARRY)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 151://jump if no carry                    			
		{
			if(((flags&FLAG_CARRY)==0)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 152://jump if positive
		{
			if(((flags&FLAG_POSITIVE)==FLAG_POSITIVE)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 153://jump if not positive
		{
			if(((flags&FLAG_POSITIVE)==0)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 154://jump if not zero                    			
		{
			if(((flags&FLAG_ZERO)==0)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 155://	jump if zero
		{
			if(((flags&FLAG_ZERO)==FLAG_ZERO)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 156://jump if CX zero 			
		{
			if((Reg16[2]==0)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;

	case 157://jump if error
		{
			if(((flags&FLAG_ERROR)==FLAG_ERROR)&&((tempw=LookForFollowingBlock(par1,par2))!=65535))
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;
	case 158: //just Jump
		{
			if((tempw=LookForFollowingBlock(par1,par2))!=65535)
				IP=tempw+2+StartPosition;
			else IP+=3;
		};break;	
	case 159: // addr, Return relative Adress of following block
		{
			if((tempw=LookForFollowingBlock(par1,par2))!=65535)
				Reg16[0]=tempw;//tempw; // => ax
			else
			{
				Reg16[0]=65500;
				flags|=FLAG_ERROR;
			}
			IP+=3;			
		};break;


	case 160: // Allocate Memory
		{
			unsigned long Start,i;
			if(!Child)
			{
				Start=LookForMemoryBlock(StartPosition,Laenge,Reg16[0]);
				Alter+=200;
				if(Start>0) 
				{
					for(i=Start;i<Start+Reg16[0];i++)
						Availible[i]=false;
					node_num++;
					node_counter++;
					Child=Insert_Node(node_counter);
					Child->Alter=0;
					Child->Child=NULL;
					Child->flags=0;
					Child->StartPosition=Start;
					Child->IP=Start;
					Child->Laenge=Reg16[0];
//					Child->Stack=NULL; ^^ Wenn Stack nochmal rein, hier vielleicht mal initialisieren :P
					Child->active=false;
					if(Start>IP)
						Reg16[2]=(unsigned short)(Start-IP);
					else 
					{
						Reg16[2]=(((unsigned short)(Start-IP)))|(32768); //Negativ!
						flags=flags|FLAG_DIRECTION;
					}

					for(i=0;i<8;i++) 
					{	
						Child->Reg16[i]=0;
						Child->Reg8[i]=0;
					}
				} else // if(start>0) 
					flags|=FLAG_ERROR;
			} //if(!Child)
			else
				flags|=FLAG_ERROR;
			IP++;
		};break;
	case 161: // Give Birth
		{
			if(Child)
			{
				Child->active=TRUE;
				Child->Alter=0;
				Child=NULL; // Nabelschnur trennen :P
			}
			else flags|=FLAG_ERROR;
			IP++;
		};break;
	case 200: // NOOOP Überspringe nächsten 2 Befehle => für Sprungmarken wichtig
		{
			IP+=3;
		};break;


	case 209: //Zero AX,BX,...
	case 210:
	case 211:
	case 212:
	case 213:
	case 214:
	case 215:
	case 216:
		{
			Reg16[Befehl-209]=0;
			IP++;
		};break;
		
		
		/*	case 161: //Start Marker
		{
			IP++;
		};break;
	case 162: //Ende Marker
		{
			return FALSE;
		};break; WOZU???*/
	default: //NOP
		{
			IP++;
		};break;
	};

	for(j=0;j<4;j++)
	{
		if(OldReg16[j]!=Reg16[j])
		{
			Reg8[j]=(unsigned char)(Reg16[j]&&255);
			Reg8[j+4]=(unsigned char)(Reg16[j]>>8);
		}
		else if((OldReg8[j]!=Reg8[j])||(OldReg8[j+4]!=Reg8[j+4]))
		{
			Reg16[j]=(((unsigned short)Reg8[j+4])<<8)+(unsigned short)Reg8[j];
		}
	} //Regs abgleichen

	return TRUE;
};


	// Alle 255 Stellen benutzen? (Mehr Möglichkeiten pro Befehl, je nach Wichtigkeit)
	// evtl Kommunikation zwischen Programmen


unsigned short Viech::LookForFollowingBlock(unsigned char par1,unsigned char par2) // Suche Negativ zur Sprungmarke bei (IP+1, IP+2) und springe dann dorthin
{
unsigned short h;

for(h=0;h<Laenge;h++)
	 if((ReadC((unsigned long)h+StartPosition)==(255-par1))&&(ReadC((unsigned long)h+StartPosition+1)==(255-par2)))// "255-": Negativ nehmen (sonst würde sich die Sprungmarke immer selbst finden)
		return (h);

	flags|=FLAG_ERROR; // Sprungmarke nicht gefunden ^^
	return (65535);
};

Viech::Viech()
{

};

Viech::~Viech()
{
//	if(Stack) free(Stack);
};

void Viech::WriteC(unsigned long position,unsigned char wert)
{
	unsigned char mutwert;
	mutwert=wert;

	if(rand()% 8192==8192)
		mutwert=rand()%256;
	if((position>=SIZE_OF_SOUP)||(position<0))
		flags|=FLAG_ERROR;
	else
	{
		Alter++;
		// Mutationen hier rein
		if(Child)
		{
			if(((position>=StartPosition)&&(position<=StartPosition+Laenge)) || ((position>=Child->StartPosition)&&(position<=Child->StartPosition+Child->Laenge)))
			{
				Soup[position%SIZE_OF_SOUP]=mutwert;
				cout<<(unsigned short)mutwert;
			}
			else
				flags|=FLAG_ERROR;
		}
		else
		{
			if((position>=StartPosition)&&(position<=StartPosition+Laenge))
			{
				Soup[position%SIZE_OF_SOUP]=mutwert;
				cout<<(unsigned short)mutwert;
			}
			else
				flags|=FLAG_ERROR;
		}
	}
}
//Start & Endmarker dürfen nicht geschrieben werden? (autoamtisch gesetzt?)

unsigned char Viech::ReadC(unsigned long position)
{
	//Zusätzlichen Parameter einfügen! (Befehl, Parameterbyte oder Parameter des Befehls!)
	
	unsigned char temp3;
	if((position>=SIZE_OF_SOUP)||(position<0))
	{
		flags|=FLAG_ERROR;
		// ? vielleicht net... wegen den ganzen parametern... mal gugn
		return ((unsigned char)3);
	}
	else
	{
		temp3=Soup[position%SIZE_OF_SOUP];
//		if((temp3>165)&&(rand()%1024==0)) // NOP => Zufallsbefehl...
//			return (rand()%165);
//		else 
//		{

/*			Arten der Mutation:
		Änderung innerhalb der Befehlsgruppe
		Nur Änderung der Befehlsgruppe (Parameter bleiben gleich / werden geändert)
		Bei Jump: ähnliche Blöcke werden genommen*/

			if(rand()%65535==0)
				return (rand()%255);
			else if(rand()%32765==0)
				return (temp3+1*(rand()%2)*(-1));
			else if(rand()%32765==0)
				return (temp3+2*(rand()%2)*(-1));
			else if(rand()%32765==0)
				return (temp3+4*(rand()%2)*(-1));
			else if(rand()%32765==0)
				return (temp3+8*(rand()%2)*(-1));
			else if(rand()%32765==0)
				return (temp3+16*(rand()%2)*(-1));
			else if(rand()%32765==0)
				return (temp3+32*(rand()%2)*(-1));
			else if(rand()%32765==0)
				return (temp3+64*(rand()%2)*(-1));
			else if(rand()%32765==0)
				return (temp3+128*(rand()%2)*(-1));

			

		else
			return temp3;
			/*Jeder Speicherzugriff: +Alter
				(schreiben: 2, lesen 1)
			evtl unterschiedlicher Energieverbrauch bei unterschiedlichen Befehlen*/

//		}


	}
	return (0);
};
// Read, Write in Inline wandeln?


unsigned long Viech::LookForMemoryBlock(unsigned long SearchStart,unsigned short Laenge,unsigned short LaengeChild)
{
	// Übergreifende Blöcke sind NICHT erlaubt
	unsigned short i,j;
	bool temp3;
	bool temp2;
	for(j=0;j<MEMORY_RANGE;j++)
	{
		temp3=false;
		temp2=false;
		for(i=0;i<LaengeChild;i++)
		{
			if((0<=SearchStart+j+i+Laenge+1)&&(SearchStart+j+i+Laenge+1<=SIZE_OF_SOUP))
				temp3|=(!Availible[SearchStart+j+i+Laenge+1]);
			else temp3=true;
			
			if((0<=SearchStart-j-i-1)&&(SearchStart-j-i-1<=SIZE_OF_SOUP))
				temp2|=(!Availible[SearchStart-j-i-1]);
			else temp2=true;

		}
		
		if((!temp3)&&((SearchStart+j+Laenge+1)>0)&&((SearchStart+j+Laenge+LaengeChild)<SIZE_OF_SOUP))
			return (SearchStart+j+Laenge+1);
	}
	return -1;
}
