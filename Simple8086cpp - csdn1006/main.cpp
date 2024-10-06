#include "simple8086.h"
using namespace std;
/* 除了加减法、有符号乘除法其他指令基本完工*/
/*
	7种寻址方式可识别的写法 全部应大写 
	立即数寻址 mov ax 2000
	寄存器寻址 mov ax bx
	寄存器间接寻址 mov ax [bx] 注意：只可以是bx bp sp si di 
	直接寻址 mov ax [2000]
	相对基址寻址 mov ax [bx/bp/si/di+2000]
	基址变址寻址 mov ax [bx/bp+si/di]
	相对基址变址寻址 mov ax [bx/bp+si/di+2000]
	一定要记得写停机指令 
*/
int main(){
	simple8086 cpu;
	Init_simple8086(&cpu);
//	cpu.memory[0]="MOV AX 1";
//	cpu.memory[1]="MOV BX [20]";
//	cpu.memory[2]="MOV BX 21";
//	cpu.memory[3]="MOV BX [BX]";
//	cpu.memory[4]="MOV AX [BX+1]";
//	cpu.memory[5]="MOV BX AX";
//	cpu.memory[6]="MOV SI 1";
//	cpu.memory[7]="MOV BP [20]";
//	cpu.memory[8]="MOV AX [BP+SI]";
//	cpu.memory[9]="MOV AX [BP+SI+1]";
//	cpu.memory[10]="HLT";
//	cpu.memory[20]="20";
//	cpu.memory[21]="21";
//	cpu.memory[22]="22";

//	cpu.memory[0]="MOV AX 50";
//	cpu.memory[1]="PUSH AX";
//	cpu.memory[2]="PUSH AX";
//	cpu.memory[3]="PUSH AX";
//	cpu.memory[4]="POP BX";
//	cpu.memory[5]="POP CX";
//	cpu.memory[6]="POP DX";
//	cpu.memory[7]="MOV BX 55";
//	cpu.memory[8]="XCHG AX BX";
//	cpu.memory[9]="HLT";
	
//	print_8binary(8);
//
//	cpu.memory[0]="MOV BX 255";
//	cpu.memory[1]="MOV AX 1";
//	cpu.memory[2]="AND BX 4";	
//	cpu.memory[3]="TEST BX 255";
//	cpu.memory[4]="OR BX 255";
//	cpu.memory[5]="XOR BX 170";
//	cpu.memory[6]="NOT BX";
//	cpu.memory[7]="MOV BX 255";
//	cpu.memory[8]="SHR BX";
//	cpu.memory[9]="SAR BX";
//	cpu.memory[10]="SHL BX";
//	cpu.memory[11]="SAL BX";
//	cpu.memory[12]="HLT";
//
//	cpu.memory[0]="";
//	cpu.memory[1]="";
//	cpu.memory[2]="";	
//	cpu.memory[3]="";
//	cpu.memory[4]="";
//	cpu.memory[5]="";
//	cpu.memory[6]="";
//	cpu.memory[7]="";
//	cpu.memory[8]="";
//	cpu.memory[9]="";
//	cpu.memory[10]="";
//	cpu.memory[11]="";
//	cpu.memory[12]="";	
//	cpu.memory[13]="";
//	cpu.memory[14]="";
//	cpu.memory[15]="";
//	cpu.memory[16]="";
//	cpu.memory[17]="";
//	cpu.memory[18]="";
//	cpu.memory[19]="";
//	cpu.memory[20]="";
//	cpu.memory[21]="";
//	cpu.memory[22]="";
//	cpu.memory[23]="";
//	cpu.memory[24]="";
//	cpu.memory[25]="";
//	cpu.memory[26]="";
//	cpu.memory[27]="";
//	cpu.memory[28]="";
//	cpu.memory[29]="";
//	cpu.memory[30]="";
//	cpu.memory[31]="";

	cpu.memory[0]="MOV AX 255";
	cpu.memory[1]="MOV DX 255";
	cpu.memory[2]="DIV 255";
	cpu.memory[3]="MOV AX 127";
	cpu.memory[4]="INC AX";
	cpu.memory[5]="DEC AX";
	cpu.memory[6]="ADD AX 127";
	cpu.memory[7]="MUL 255";
	cpu.memory[8]="HLT";
	
	run(&cpu);

	return 0;
} 
