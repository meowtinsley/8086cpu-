#include "simple8086.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <queue>


#include<iostream>


using namespace std;
int main(){
	simple8086 cpu;
	Init_simple8086(&cpu);
//	cpu.memory[0]="ADD AX 2";
//	cpu.memory[1]="MOV BX 3";
//	cpu.memory[2]= "XCHG BX AX";
//	cpu.memory[3]= "XCHG AX [12]";
//	cpu.memory[4]= "HLT";
//	cpu.memory[10]= "10";
//	cpu.memory[11]= "11";
//	cpu.memory[12]= "12";
//	cpu.memory[0]="";
	cpu.memory[0]="MOV AX 1";
	cpu.memory[1]="MOV BX [20]";
	cpu.memory[2]="MOV BX 21";
	cpu.memory[3]="MOV BX [BX]";
	cpu.memory[4]="MOV AX [BX+1]";
	cpu.memory[5]="MOV BX AX";
	cpu.memory[6]="MOV SI 1";
	cpu.memory[7]="MOV BP [20]";
	cpu.memory[8]="MOV AX [BP+SI]";
	cpu.memory[9]="MOV AX [BP+SI+1]";
	cpu.memory[10]="HLT";
	cpu.memory[20]="20";
	cpu.memory[21]="21";
	cpu.memory[22]="22";

	run(&cpu);
//	char test0[]="[BX+SI+10]";//>>>BX SI 100
//	get_value(test0,&cpu);
////	char test1[]="[BX+SI]";//>>>BX SI]
////	get_value(test1,&cpu);
//	char test2[]="[BX+11]";//>>>BX 100]
//	get_value(test2,&cpu);
//	char test3[]="[12]";//>>>100]
//	get_value(test3,&cpu);
////	char test4[]="[BX]";//>>>BX]
////	get_value(test4,&cpu);
//	char test5[]="13";
//	get_value(test5,&cpu);
//	char test6[]="BX";
//	get_value(test6,&cpu);
	return 0;
	 
} 
