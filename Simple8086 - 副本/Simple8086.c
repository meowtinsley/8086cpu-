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

// 指令集 
#define HLT 0

#define ADD 1

#define MOV 100

typedef struct{
	unsigned int ip;//指令指针 
	char memory[MEMORY_SIZE][INSTRUCTION_SIZE];//字符串结尾有多余的“/0” 
	int registers[8];
	
	int data_bus;
	bool control_bus_read;
	bool control_bus_write;

}simple8086;



void mov(simple8086* cpu,int* reg,int* val){
    printf("将 %d 移动到寄存器%s\n", *val, *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" :"DX");
    cpu->registers[*reg] = *val;
}


void add(simple8086* cpu,int* reg,int* val){
    printf("给寄存器%s加上 %d\n", *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" : "DX", *val);
    cpu->registers[*reg] += *val;
}

void hlt(){
	printf("...停止执行\n");
	exit(0);
}
static char instruction_buffer[INSTRUCTION_SIZE];
char* fetch(simple8086* cpu){
	strcpy(instruction_buffer,cpu->memory[cpu->ip]);
	printf("正在从内存地址 %u 获取指令：%s\n", cpu->ip, instruction_buffer);
	cpu->ip++;
	return instruction_buffer;
}
void update_buses(simple8086* cpu,int* op,int* reg,int* val);
// 译码 
void decode(simple8086* cpu,char* instruction,int* op,int* reg,int* val){
	printf("正在解码指令..."); 
	char parts[3][16];
	int part_count = sscanf(instruction,"%s %s %d",
							parts[0],parts[1],val);
	printf("操作码%s 寄存器%s 值%d\n",parts[0],parts[1],*val);
    //反向译码hhhh 我把字符串翻译成数字怎么不算译码呢 
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
// 显示 
void print_reg(simple8086* cpu){
	printf("寄存器状态：\n");
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

// 更新总线状态 
void update_buses(simple8086* cpu,int* op,int* reg,int* val){
	printf("总线状态:\n");
	printf("地址总线：\n");
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
			printf("HLT指令：停止执行\n");
			break;			
	}

	printf("\t控制总线:\n");
	printf("\t\tREAD：%s WRITE：%s\n",cpu->control_bus_read? "TRUE" : "FALSE",cpu->control_bus_write? "TRUE" : "FALSE");
	printf("\t数据总线:\n");
	printf("\t\tDATA:%d\n",cpu->data_bus);
} 


// 执行 
void execute(simple8086* cpu,int* op,int* reg,int* val) {
	printf("正在执行指令: %s...\n", 
	       *op == MOV ? "MOV" : 
	       (*op == ADD ? "ADD" : 
	       (*op == HLT ? "HLT" : "未知指令")));
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
			printf("未知指令\n");
			break; 
			
	}
	update_buses(cpu,op,reg,val);
	print_reg(cpu);
}



void run(simple8086* cpu){
	printf("开始执行...\n");
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
