# 微机原理与接口第二次作业

简单说就是实现了模拟实现ADD，MOV，HLT三个指令。没有考虑实现寻址方式。

## 目标：

设计并实现一个简化版的8086 CPU模拟器，该模拟器需能够处理并仿真MOV（数据传送）、ADD（加法）、以及HLT（停机）等基本指令的执行过程。通过模拟CPU的主要组成部分（如寄存器、指令寄存器、程序计数器、控制单元以及数据总线），加深对计算机体系结构和工作原理的理解。

## 要求与规范：

### **基础组件定义：**

寄存器：实现至少8个通用寄存器（如AX, BX, CX, DX, SP, BP, SI, DI）和一个指令寄存器（IR）、一个程序计数器（PC/IP，Instruction Pointer）。寄存器大小应设定为16位。

数据总线：定义一条16位的数据总线，用于CPU内部及CPU与外部存储器之间的数据传输。

控制单元：设计逻辑以解析指令，控制指令的执行流程，包括从内存中读取指令、解码指令、执行指令以及更新PC等。

内存：实现一个简单的内存模型，能够存储指令和数据，至少包含足够空间以加载和执行示例程序。

> 根据c语言特点，用结构体实现`simple8086`的类，也需要手动考虑一下用什么数据类型表示各个组件。内存这里实现的也不太理想，c里的字符串不是一个单独的数据类型，是字符数组，所以这里用的二维数组表示的。~~dalao说string不单独作为一个数据类型更好，我还不是dalao只想直接用string~~8个通用寄存器就是一个int型数组，为了方便起见~~我也不知道有没有真的方便到我~~，我想到宏定义各个数组的名称作为索引？其实总线的表示我觉得是很难模拟的，因为实际上并没有数据在传输。。。会在总线更新的函数里完善打印内容的。

```C
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MEMORY_SIZE 256
#define INSTRUCTION_SIZE 25
// 16位 8个通用寄存器 
#define AX 0
#define BX 1
#define CX 2
#define DX 3
#define SP 4 
#define BP 5
#define SI 6
#define DI 7
typedef struct{
	unsigned int ip;//指令指针 
	char memory[MEMORY_SIZE][INSTRUCTION_SIZE];
	int registers[8];
	
	int data_bus;
	bool control_bus_read;
	bool control_bus_write;
}simple8086;
```



### **指令集实现：**

MOV指令：实现数据在寄存器之间的移动。例如，`MOV AX, BX` 会将BX寄存器的值复制到AX寄存器中。

ADD指令：实现两个寄存器之间的加法操作，并将结果存回其中一个寄存器。例如，`ADD AX, BX` 会将AX和BX的值相加，并将结果存回AX。

HLT指令：实现停机指令，当CPU执行到HLT指令时，应停止执行后续指令，模拟系统挂起或等待外部中断的状态。程序加载与执行：设计一种方式（如文本文件或硬编码）来加载和执行包含MOV、ADD、HLT等指令的示例程序。程序应能够连续执行指令，直到遇到HLT指令或程序结束。

> 这里就体现了宏定义寄存器和指令集有点蠢，此乃我觉得写一堆`ifelse`不好看就想写成switch的结果。但容我辩解，一直`strcmp`感觉也挺烦的。退一步讲其实应该都是机器码，不过我不知道怎么表示解释器这一步。

```c
// 指令集 
#define MOV 0
#define ADD 1
#define HLT 2
void mov(simple8086* cpu,int* reg,int val){
	printf("将 %d 移动到寄存器AX",val);
	
	switch(*reg){
		case AX:
			cpu->registers[AX]=val;
			printf("将 %d 移动到寄存器AX\n",val);
			break;
		case BX:
			cpu->registers[BX]=val;
			printf("将 %d 移动到寄存器BX\n",val);
			break;
		case CX:
			cpu->registers[CX]=val;
			printf("将 %d 移动到寄存器CX\n",val);
			break;
		case DX:
			cpu->registers[DX]=val;
			printf("将 %d 移动到寄存器DX\n",val);
			break;
    } 
}

void add(simple8086* cpu,int* reg,int val){
	switch (*reg) {
		case AX:
			cpu->registers[AX]+=val;
			printf("给寄存器AX加上%d\n",val);
			break;
		case BX:
			cpu->registers[BX]+=val;
			printf("给寄存器BX加上%d\n",val);
			break;
		case CX:
			cpu->registers[CX]+=val;
			printf("给寄存器CX加上%d\n",val);
			break;
		case DX:
			cpu->registers[DX]+=val;
			printf("给寄存器DX加上%d\n",val);
			break;
		
	}
}

void hlt(){
	printf("停止执行\n");
	exit(0);
}
```

其实后面调试的时候发现一种~~我觉得~~更好的写法，非常滴简洁，我非常喜欢。

```c
void mov(simple8086* cpu,int* reg,int* val){
 	printf("将 %d 移动到寄存器%s\n", 
           *val, *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" : "DX");
    cpu->registers[*reg] = *val;
}
void add(simple8086* cpu,int* reg,int* val){
    printf("给寄存器%s加上 %d\n", 
           *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" : "DX", *val);
    cpu->registers[*reg] += *val;
}
void hlt(){
	printf("...停止执行\n");
	exit(0);
}
```

### **调试与输出**：

实现一种机制（如控制台输出）来显示每一步执行后的寄存器状态、内存内容以及当前执行的指令，以便于调试和观察CPU的工作过程。

> 这里就是简单的打印寄存器状态和总线状态的代码

```c
// 显示寄存器状态
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

// 更新并显示总线状态 
void update_buses(simple8086* cpu,int* op,int* reg,int* val){
	printf("总线状态:\n");
	printf("地址总线：\n");
	switch(*op){
		case MOV:
			cpu->data_bus = *val;
			cpu->control_bus_read=true;
			cpu->control_bus_write=false;
			printf("\tSOURCE: %s DESTINATION: memory[%d]\n",
                   *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" : "DX",cpu->ip);
			break;
		case ADD:
			cpu->data_bus = *val;
			cpu->control_bus_read=false;
			cpu->control_bus_write=true;
			printf("\tSOURCE: %s DESTINATION: %s\n",
                   *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" :"DX",
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
```

> 关于指令的执行的打印输出感觉逻辑并不是很清楚，但姑且完成了。

```c
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

void decode(simple8086* cpu,char* instruction,int* op,int* reg,int* val){
	printf("正在解码指令..."); 
	/* 执行部分代码 */
	printf("操作码%s 寄存器%s 值%d\n",parts[0],parts[1],*val);
    /* 执行部分代码 */						
}
// 显示寄存器状态
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

// 更新并显示总线状态 
void update_buses(simple8086* cpu,int* op,int* reg,int* val){
	printf("总线状态:\n");
	printf("地址总线：\n");
	switch(*op){
		case MOV:
			/* 执行部分代码 */
			printf("\tSOURCE: %s DESTINATION: memory[%d]\n",
                   *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" : "DX",cpu->ip);
			break;
		case ADD:
			/* 执行部分代码 */
			printf("\tSOURCE: %s DESTINATION: %s\n",
                   *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" :"DX",
                   *reg == AX ? "AX" : *reg == BX ? "BX" : *reg == CX ? "CX" :"DX");
			break;
		case HLT:
			/* 执行部分代码 */
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
	       *op == MOV ? "MOV" : *op == ADD ? "ADD" : *op == HLT ? "HLT" : "未知指令");
	/* 执行部分代码 */
	update_buses(cpu,op,reg,val);
	print_reg(cpu);
}
```



本次作业大量参考老师给出的python源码。

此乃参考代码的运行结果。

<img src="C:\Users\Adminstrator\Desktop\临时图片存放处\微机原理2.参考代码运行结果.png" alt="微机原理2.参考代码运行结果" style="zoom: 50%;" />

此乃本次完成的代码结果。

<img src="C:\Users\Adminstrator\Desktop\临时图片存放处\微机原理2运行结果.png" alt="微机原理2运行结果" style="zoom: 67%;" />

从结果上来看已经基本完成需求了。不过操作码，寄存器和值是三个指针感觉好像不是很好，不过暂时没想到更好的办法，就先这样吧。

## 执行流程图

<img src="C:\Users\Adminstrator\Desktop\临时图片存放处\执行流程图.jpg" alt="执行流程图" style="zoom: 5%;" />
