# 微机原理与接口第三次作业

[TOC]

## 前言

因为发现作为最终目标的第四次作业要求有变，于是将整个工程改为C++工程。~~虽然时候看完全没必要。。。~~为了便于维护和小组合作方便，进行了文件分割，希望有同学愿意帮我完善各部分功能。其实我感觉`Simple8086`这个结构体可以改成一个类。~~面向对象的语言真好。。。~~但是这版结构改起来就加入了`BIU`和`EU`两个类。此次作业完全是一个过程作业，在截止日期前我已经来不及调试好全部功能。一开始想直接完成作业四，但是发现有点庞大，停止搭框架的时候发现已然过于庞大。有现成的`queue`真好。

## 目的：

通过编写程序模拟8086 CPU的基本运行过程，深入理解CPU的指令执行流程，包括指令的取指、解码和执行，以及实现CPU内部的总线接口单元（BIU）和执行单元（EU）的并行工作机制。本模拟将涵盖三条基础指令：MOV AX, 1、ADD AX, 2 和 HLT，并展示CPU内部寄存器（如AX、BX、CX、DX、IP、FLAGS等）、指令队列、地址总线、数据总线和状态总线的使用。

## 要求：

### 设计CPU结构：

总线接口单元（BIU）：负责从模拟的内存中读取指令到指令队列。模拟内存地址由指令指针寄存器（IP）指示。

执行单元（EU）：负责从指令队列中取出指令进行解码和执行。

> 舍不得把结构体改了。

```c++
typedef struct{
	unsigned int ip;//指令指针 
	string memory[MEMORY_SIZE];
	string registers_name[10]={"AX","BX","CX","DX","SP","BP","SI","DI","AH","AL"};
	int registers[8];
	bool status_flags[6];
	int data_bus;
	bool control_bus_read;
	bool control_bus_write;
}simple8086;
```

> 这点队列没写明白，整个工程就这块还没想明白到底怎么取队列里的指令，没调明白。
>
> 这个工程都会被我宏定义寄存器名统治的。还有宏定义指令代码。

```c++
#define InstructionQueueSize 1
#define CS 0 
#define DS 1
#define ES 2
#define SS 3
//总线接口单元 负责从内存中取出指令送入指令队列中，等待EU去执行 
class BIU{
	public:
		int registers[4];
		string registers_name[4]={"CS","DS","ES","SS"}; 
		queue<string> instructions;
		int IP;
		int queue_index = 1;
		void fetch(simple8086* cpu){
			printf("正在获取指令队列，(%d/%d)",queue_index,InstructionQueueSize);
			if(queue_index<=InstructionQueueSize){
				string instruction_temp = cpu->memory[cpu->ip];
				instructions.push(instruction_temp);
				queue_index++;
				cpu->ip++;
				printf("\n%d\n",cpu->ip);
				std::cout << "队列大小: " << instructions.size() << std::endl;
			}			
		}
		string get_instruction(void){
			if(!instructions.empty()){
				string instruction = instructions.front();
				instructions.pop();
				std::cout << "队列头部元素: " << instructions.front() << std::endl; 
				return instruction;
			}
		}
		
		int address_adder(int base,int offset){
			base<<=4;
			int physical_address=base+offset;
			return physical_address;
		} 
		
};
/*
执行单元
从biu指令队列中获取指令
译码
*/
class EU{
	public:
		int opcode;
		char source[10];
		char dest[10];
		void decode(BIU biu);
		void excute(simple8086* cpu,BIU biu);
};
```



### 指令队列：

用于暂存从内存中读取的指令，以便EU在BIU读取新指令时能够继续执行当前指令。寄存器组：实现AX、BX、CX、DX作为通用寄存器，IP作为指令指针寄存器，FLAGS作为状态标志寄存器，以及必要的段寄存器（虽然本模拟不直接涉及段操作，但应提及其存在）。

> 指令队列的实现就是`BIU`里的`queue`。~~其实没实现好。~~还需要标志寄存器嗯嗯于是我直接就想实现一个`ALU`。因为`ALU`在运算出结果时同时置标志位。在`ALU`中其实是`ACC`和一个缓存器存放两个操作数的，我直接简化成`a`和`b`，这样的化把复杂度交给上层调用也就是`excute()`就好了。有野心完成全部的`ALU`处理的算数和逻辑运算指令只是我自己的做不到啊啊啊啊。
>
> 我理解执行过程是`decode()`解码操作码，体现在程序里就是把存在“内存数组”中的指令（str）转换成我宏定义的指令（数）。解码操作码之后再根据操作数的寻址方式取后面的数，这部分有一个大坑就是实现寻址方式。对这里理解的并不是很清楚于是暂且搁置，想清楚之前我不会完成`MOV`指令。
>
> 真的要实现`AF`吗。。。没想到这个标志位有什么用。。。实现的并不完善，有的标志位我并不清楚何时置`1`，未来会有同学帮我完善的。（确信）
>
> ~~这样又少了十来条指令，ohyeah。~~

```c++
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
				printf("正在获取指令队列(%d/%d)",queue_index,InstructionQueueSize);
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
				cout << "指令队列头部元素: " << instructions.front()<<endl;
				instructions.pop();
				cout << "取出后队列头部元素: " << instructions.front()<<endl;
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
```



```c++
#define HLT 0
// 算数指令
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

#define MOV 100
// flags 
#define CF 0
#define PF 1
#define AF 2
#define ZF 3
#define SF 4
#define OF 5
int ALU(simple8086* cpu,int opcode,int a,int b){
	int res = 0;
	switch (opcode) {
		case ADD:
			res = a + b;
			if(res>255){cpu->status_flags[CF] = 1;}
			break;
		case ADC:
			res = a + b + cpu->status_flags[CF];
			if(res>255){cpu->status_flags[CF] = 1;}
			break;
		case SUB:
			res = a - b;
			if(res < 0){cpu->status_flags[CF] = 1;}
			break;
		case SBB:
			res = a - b - cpu->status_flags[CF];
			if(res < 0){cpu->status_flags[CF] = 1;}
			break;
		case INC:
			res = a + 1;
			break;
		case DEC:
			res = a - 1;
			break;
		case MUL:
			
			break;
		case IMUL:
			res = a * b;
			if (a > 0 && b > 0 && a > 255 / b) {cpu->status_flags[OF] = 1;}// 正溢出
			if (a < 0 && b < 0 && a < 255 / b) {cpu->status_flags[OF] = 1;}// 负溢出
			if ((a > 0 && b < 0 && a < 255/ b) ||(a < 0 && b > 0 && b < 255/ a)) 
			{cpu->status_flags[OF] = 1;} // 负溢出 
		case DIV:
			res = a - 1;
			break;
		case IDIV:
			
			break;	
		case NEG:
			res = 0 - a;
			break; 
		case CMP:

			break;
			
		case AND:
			res = a & b;
			cpu->status_flags[OF] = 0;
			cpu->status_flags[CF] = 0;
			break;
		case OR:
			res = a | b;
			cpu->status_flags[OF] = 0;
			cpu->status_flags[CF] = 0;
			break;
		case XOR:
			res = a ^ b;
			cpu->status_flags[OF] = 0;
			cpu->status_flags[CF] = 0;			
			break;
		case NOT:
			res = ~a;
			cpu->status_flags[OF] = 0;
			cpu->status_flags[CF] = 0;
			break;	
		default:
			break;
	}
	if(res==0){cpu->status_flags[ZF] = 1;}
	if(res%2==0){cpu->status_flags[PF] = 1;}
	if(res<0){cpu->status_flags[SF] = 1;}
	return res;	
}

```

### 实现指令集：

MOV AX, 1：将立即数1移动到AX寄存器中。ADD AX, 2：将AX寄存器的值增加2。HLT：使CPU进入暂停状态，模拟程序的结束。模拟过程：取指令：BIU根据IP寄存器的值从模拟的内存中取出指令，放入指令队列。IP随后更新以指向下一条指令的地址。

### 解码指令：

EU从指令队列中取出指令，根据指令的操作码调用相应的执行函数。执行指令：执行函数根据指令内容更新CPU内部状态，包括寄存器的值。对于MOV和ADD指令，需要操作数据总线来读取或写入寄存器的值。并行执行：模拟BIU和EU的并行工作，即在EU执行当前指令的同时，BIU可以预取下一条指令到指令队列中，以提高CPU的执行效率。输出：执行过程中输出所有寄存器、总线、标志位、指令队列，内存的状态，以验证指令执行的结果。附加要求（可选）：引入时钟周期的概念，模拟CPU操作的时序，包括取指周期、执行周期等。

> 因为是觉得`C++`功能更强大，比如有我喜欢的单独的`string`数据类型，就直接转成`c++`，结果因为不熟悉其实际实现方法被狠狠坑了。运行结果一片乱码哈哈。比如美滋滋使用字符串相等后面还是得改回去`strcmp(str1,str2)`，而且`strcmp`字符串相等时返回的是`0`。被这样的问题折磨一上午啊啊。
>
> 哦还有，C++的字符串分割有点太反人类了。于是在c++中用C的实现方法，才发现`sscanf()`的实现好像是返回`char*`型的，指针啊指针，我真是对你又爱又恨。这次把译码实现逻辑中的指针都改掉了。

```c++
void EU::decode(BIU biu){
	char opcode_str[16];
	string instruction = biu.get_instruction();
	printf("正在解码指令...");
	cout<<instruction<<endl;
	int part_count = sscanf(instruction.c_str(),"%s %s %s",
							opcode_str,dest,source);
	if (strcmp(opcode_str,"HLT")==0) {opcode = HLT;} 
	else if (strcmp(opcode_str,"ADD")==0) {opcode = ADD;} 
	else if (strcmp(opcode_str,"MOV")==0) {opcode = MOV;} 
	else {cout << "未知指令" << endl;}
	printf("操作码%s 目的操作数%s 源操作数%s\n",opcode_str,dest,source);
}

```

```c++
void EU::excute(simple8086* cpu,BIU biu){
	if(opcode<=12){
		int a,b;
		switch (opcode) {
				case HLT:
					hlt();
					break;
				case ADD:
					if(!strcmp(source,"AX")){a = cpu->registers[AX];}
					else if(!strcmp(source,"BX")){a = cpu->registers[BX];}
					else if(!strcmp(source,"CX")){a = cpu->registers[CX];}
					else if(!strcmp(source,"DX")){a = cpu->registers[DX];}
					else{a=atoi(source);}
					if(strcmp(dest,"AX")==0){b = cpu->registers[AX];cpu->registers[AX]= ALU(cpu,ADD,a,b);}
					else if(strcmp(dest,"BX")==0){b = cpu->registers[BX];cpu->registers[BX]= ALU(cpu,ADD,a,b);}
					else if(strcmp(dest,"CX")==0){b = cpu->registers[CX];cpu->registers[CX]= ALU(cpu,ADD,a,b);}
					else if(strcmp(dest,"DX")==0){b = cpu->registers[DX];cpu->registers[DX]= ALU(cpu,ADD,a,b);}
					printf("给寄存器%s加上%d",dest,a);
					print_reg(cpu);
					break;
				case ADC:
				
					break;
				case SUB:
					
					break;
				case SBB:
					
					break;
				case INC:
					break;
				case DEC:
					break;
				case MUL:
					
					break;
				case IMUL:
					break;
				case DIV:
				
					break;
				case IDIV:
					
					break;	
				case NEG:
					
					break; 
				case CMP:
		
					break;
					
				case AND:
					
					break;
				case OR:
					
					break;
				case XOR:
							
					break;
				case NOT:
					
					break;	
				default:
					break;
			}
	}
	
	return;
}
```



注意：本模拟是高度简化的，旨在帮助学生理解CPU的基本工作原理，而不是实现一个完整的8086 CPU模拟器。在实际编程时，需要根据所选的编程语言（如Python、C/C++等）来具体实现上述功能。考虑到并行执行的复杂性，模拟中的“并行”可能是通过轮询或简单的多线程/协程来实现的，以模拟BIU和EU的同时工作。

## 已经通过测试的其他内容

寻址方式的实现，具体请看[模拟8086cpu-寻址方式的实现（全）-CSDN博客](https://blog.csdn.net/m0_75163580/article/details/142685329?spm=1001.2014.3001.5502)

