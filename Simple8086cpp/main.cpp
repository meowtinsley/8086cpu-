#include "simple8086.h"
using namespace std;
/*
	7��Ѱַ��ʽ��ʶ���д�� ȫ��Ӧ��д 
	û��ƫ������ 
	������Ѱַ mov ax 2000
	�Ĵ���Ѱַ mov ax bx
	�Ĵ������Ѱַ mov ax [bx] ע�⣺ֻ������bx bp sp si di 
	ֱ��Ѱַ mov ax [2000]
	��Ի�ַѰַ mov ax [bx/bp/si/di+2000]
	��ַ��ַѰַ mov ax [bx/bp+si/di]
	��Ի�ַ��ַѰַ mov ax [bx/bp+si/di+2000]
	һ��Ҫ�ǵ�дͣ��ָ�� 
*/
int main(){
	simple8086 cpu;
	Init_simple8086(&cpu);
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
	return 0;
} 
