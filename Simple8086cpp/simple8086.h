#define MEMORY_SIZE 64
#define INSTRUCTION_SIZE 25
// 16λ 8��ͨ�üĴ��� 
#define AX 0
#define BX 1
#define CX 2
#define DX 3
#define SP 4 
#define BP 5
#define SI 6
#define DI 7
// ָ� 
#define HLT 0

#define ADD 1
#define ADC 2
#define SUB 3
#define SBB 4
#define INC 5
#define DEC 6
#define MUL 7
#define IMUL 8
#define DIV 9
#define IDIV 10
#define NEG 11
#define CMP 12

#define AND 13
#define OR 14
#define XOR 15
#define NOT 16
#define TEST 17
#define SHL 18
#define SHR 19
#define SAL 20
#define SAR 21
#define ROL 22
#define ROR 23
#define RCL 24
#define RCR 25

#define MOV 26
#define PUSH 27
#define POP 28
#define XCHG 29

// flags 
#define CF 0
#define PF 1
#define AF 2
#define ZF 3
#define SF 4
#define OF 5

/*
16λ�μĴ���
����μĴ���cs
���ݶμĴ���ds
���ӶμĴ���es
��ջ�μĴ���ss
*/
#define InstructionQueueSize 3
#define CS 0 
#define DS 1
#define ES 2
#define SS 3

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <queue>


#include<iostream>

#include <sstream>
#include <string>
#include <vector>

using namespace std;

typedef struct{
	unsigned int ip;//ָ��ָ�� 
	string memory[MEMORY_SIZE];
	int registers[8];
	bool status_flags[6];
	int data_bus;
	bool control_bus_read;
	bool control_bus_write;
	
}simple8086;




//���߽ӿڵ�Ԫ ������ڴ���ȡ��ָ������ָ������У��ȴ�EUȥִ�� 
class BIU{
	public:
		int registers[4];
		string registers_name[4]={"CS","DS","ES","SS"}; 
		queue<string> instructions;
		//int IP;
		int queue_index = 0;
		void fetch(simple8086* cpu){
			while(queue_index<InstructionQueueSize&&cpu->ip<MEMORY_SIZE){
				queue_index++;
				printf("���ڻ�ȡָ�����(%d/%d)",queue_index,InstructionQueueSize);
				string instruction_temp = cpu->memory[cpu->ip];
				cout<<instruction_temp<<endl;
				instructions.push(instruction_temp);
				cpu->ip++;
			}
		}
		string get_instruction(void){
			if(!instructions.empty()){
				string instruction = instructions.front();
				//cout<<instruction<<endl;
				cout << "ָ�����ͷ��Ԫ��: " << instructions.front()<<endl;
				instructions.pop();
				cout << "ȡ�������ͷ��Ԫ��: " << instructions.front()<<endl;
				return instruction;
			}
		}
		
		bool instruction_queue_isempty(void){
			return instructions.empty();
		}
		
		int address_adder(int base,int offset){
			base<<=4;
			int physical_address=base+offset;
			return physical_address;
		} 
		
};
/*
ִ�е�Ԫ
��biuָ������л�ȡָ��
����
*/
class EU{
	public:
		int opcode;
		char source[10];
		char dest[10];
		string instruction; 
		void decode(BIU& biu,simple8086* cpu);
		void excute(simple8086* cpu,BIU biu);
};

void hlt();
void print_reg(simple8086* cpu);
void print_flags(simple8086* cpu);
void update_buses(simple8086* cpu,int* op,int* reg,int* val);
void Init_simple8086(simple8086* cpu);
int ALU(simple8086* cpu,int opcode,int a,int b);
void run(simple8086* cpu);
int get_value(char* operand,simple8086* cpu);
void write_value(simple8086* cpu,char* operand,int value); 
