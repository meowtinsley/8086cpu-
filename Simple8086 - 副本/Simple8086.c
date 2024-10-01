#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MEMORY_SIZE 256
#define INSTRUCTION_SIZE 25

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

#define MOV 100

typedef struct{
	unsigned int ip;//ָ��ָ�� 
	char memory[MEMORY_SIZE][INSTRUCTION_SIZE];//�ַ�����β�ж���ġ�/0�� 
	int registers[8];
	
	int data_bus;
	bool control_bus_read;
	bool control_bus_write;

}simple8086;



void mov(simple8086* cpu,int* reg,int* val){
    printf("�� %d �ƶ����Ĵ���%s\n", *val, *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" :"DX");
    cpu->registers[*reg] = *val;
}


void add(simple8086* cpu,int* reg,int* val){
    printf("���Ĵ���%s���� %d\n", *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" : "DX", *val);
    cpu->registers[*reg] += *val;
}

void hlt(){
	printf("...ִֹͣ��\n");
	exit(0);
}
static char instruction_buffer[INSTRUCTION_SIZE];
char* fetch(simple8086* cpu){
	strcpy(instruction_buffer,cpu->memory[cpu->ip]);
	printf("���ڴ��ڴ��ַ %u ��ȡָ�%s\n", cpu->ip, instruction_buffer);
	cpu->ip++;
	return instruction_buffer;
}
void update_buses(simple8086* cpu,int* op,int* reg,int* val);
// ���� 
void decode(simple8086* cpu,char* instruction,int* op,int* reg,int* val){
	printf("���ڽ���ָ��..."); 
	char parts[3][16];
	int part_count = sscanf(instruction,"%s %s %d",
							parts[0],parts[1],val);
	printf("������%s �Ĵ���%s ֵ%d\n",parts[0],parts[1],*val);
    //��������hhhh �Ұ��ַ��������������ô���������� 
	if(strcmp(parts[0],"MOV")==0) {*op = MOV;}
	else if(strcmp(parts[0],"ADD")==0){*op = ADD;}
	else if(strcmp(parts[0],"HLT")==0){*op = HLT;}
	if(part_count>=2){
		if(strcmp(parts[1],"AX")==0) {*reg = AX;}
		else if(strcmp(parts[1],"BX")==0){*reg = BX;}
		else if(strcmp(parts[1],"CX")==0){*reg = CX;}
		else if(strcmp(parts[1],"DX")==0){*reg = DX;}
	}
	
	
							
}
// ��ʾ 
void print_reg(simple8086* cpu){
	printf("�Ĵ���״̬��\n");
	printf("\tAX: %d BX: %d CX: %d DX: %d SP: %d BP: %d SI: %d DI: %d\n\n",
			cpu->registers[AX],
			cpu->registers[BX],
			cpu->registers[CX],
			cpu->registers[DX],
			cpu->registers[SP],
			cpu->registers[BP],
			cpu->registers[SI],
			cpu->registers[DI]);
}

// ��������״̬ 
void update_buses(simple8086* cpu,int* op,int* reg,int* val){
	printf("����״̬:\n");
	printf("��ַ���ߣ�\n");
	switch(*op){
		case MOV:
			cpu->data_bus = *val;
			cpu->control_bus_read=true;
			cpu->control_bus_write=false;
			printf("\tSOURCE: %s DESTINATION: memory[%d]\n",*reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" : "DX",
														 cpu->ip);
			break;
		case ADD:
			cpu->data_bus = *val;
			cpu->control_bus_read=false;
			cpu->control_bus_write=true;
			printf("\tSOURCE: %s DESTINATION: %s\n",*reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" :"DX",
												  *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" :"DX");
			break;
		case HLT:
			cpu->data_bus = 0;
			cpu->control_bus_read=false;
			cpu->control_bus_write=false;
			printf("HLTָ�ִֹͣ��\n");
			break;			
	}

	printf("\t��������:\n");
	printf("\t\tREAD��%s WRITE��%s\n",cpu->control_bus_read? "TRUE" : "FALSE",cpu->control_bus_write? "TRUE" : "FALSE");
	printf("\t��������:\n");
	printf("\t\tDATA:%d\n",cpu->data_bus);
} 


// ִ�� 
void execute(simple8086* cpu,int* op,int* reg,int* val) {
	printf("����ִ��ָ��: %s...\n", 
	       *op == MOV ? "MOV" : 
	       (*op == ADD ? "ADD" : 
	       (*op == HLT ? "HLT" : "δָ֪��")));
	switch(*op){
		case MOV:
			mov(cpu,reg,val);
			break;
		case ADD:
			add(cpu,reg,val);
			break;
		case HLT:
			hlt();
			break;
		default:
			printf("δָ֪��\n");
			break; 
			
	}
	update_buses(cpu,op,reg,val);
	print_reg(cpu);
}



void run(simple8086* cpu){
	printf("��ʼִ��...\n");
	while(1){
		char* instruction = fetch(cpu);
		int* op = (int*) malloc(sizeof(int));
		int* reg = (int*) malloc(sizeof(int));
		int* val = (int*) malloc(sizeof(int));
		*op = 0;
		*reg = 0;
		*val = 0;
		decode(cpu,instruction,op,reg,val);
		execute(cpu,op,reg,val); 
		free(op);
		free(reg);
		free(val);
	} 
}

void Init_simple8086(simple8086 *cpu) {
    cpu->ip = 0;
    memset(cpu->memory, 0, sizeof(cpu->memory));
    memset(cpu->registers, 0, sizeof(cpu->registers));
    cpu->data_bus = 0;
    cpu->control_bus_read = false;
    cpu->control_bus_write = false;
}

int main(){
	simple8086 cpu;
	Init_simple8086(&cpu);
	strcpy(cpu.memory[0], "MOV AX 1");
	strcpy(cpu.memory[1], "ADD AX 2");
	strcpy(cpu.memory[2], "HLT");
	run(&cpu);
	
	return 0;
	 
} 
