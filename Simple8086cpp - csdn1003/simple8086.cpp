#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include<stdbool.h>
#include "simple8086.h"
#include<iostream>


/*
ver2 更新内容 
更多的指令 todo 算数指令1-12 停机指令0 13-25逻辑运算指令 
alu todo 
各种算术运算到底如何置标志位啊啊啊 搞不懂cf 没搞af 

我们需要把总线转换成一个buffer数组之类的 todo 
decode需要重写 ok
各个指令的实现也应该重写 
线程？ 



寻址方式 ok

*/ 

void hlt(){
	printf("...停止执行\n");
	exit(0);
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


void Init_simple8086(simple8086* cpu) {
    cpu->ip = 0;
//    for (int i = 0; i < MEMORY_SIZE; i++) {
//	        cpu->memory[i] = ""; // 清空内存
//	}
    //memset(cpu->memory, 0, sizeof(cpu->memory));
    memset(cpu->registers, 0, sizeof(cpu->registers));
    memset(cpu->status_flags, 0, sizeof(cpu->status_flags));
    cpu->data_bus = 0;
    cpu->control_bus_read = false;
    cpu->control_bus_write = false;
}


//假设参与运算的数是八位的 1111 1111 0111 1111=127 有符号范围 1111 1111-0111 1111 -127-127 无符号范围 0-255 
//todo alu:各种算术运算到底如何置标志位啊啊啊 搞不懂cf 没搞af 
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

void print_flags(simple8086* cpu){
	cout<<"标志寄存器的状态："<<endl; 
	printf("\tCF:%d PF:%d AF:%d ZF:%d SF:%d OF:%d\n",
			cpu->status_flags[CF],
			cpu->status_flags[PF],
			cpu->status_flags[AF],
			cpu->status_flags[ZF],
			cpu->status_flags[SF],
			cpu->status_flags[OF]);
}
void EU::decode(BIU& biu,simple8086* cpu){	
	char opcode_str[16];
	//char buffer[20];
	if(!biu.instruction_queue_isempty()){
		instruction = biu.get_instruction();
		printf("\n正在解码指令...");
		cout<<instruction<<endl;
		//int part_count = sscanf(instruction.c_str(),"%s %s %s",opcode_str,dest,source);
		sscanf(instruction.c_str(),"%s %s %s",opcode_str,dest,source);
		
		if (strcmp(opcode_str, "HLT") == 0) {opcode = HLT;} 
		else if (strcmp(opcode_str, "ADD") == 0){opcode = ADD;} 
		else if (strcmp(opcode_str, "ADC") == 0){opcode = ADC;} 
		else if (strcmp(opcode_str, "SUB") == 0){opcode = SUB;} 
		else if (strcmp(opcode_str, "SBB") == 0){opcode = SBB;} 
		else if (strcmp(opcode_str, "INC") == 0){opcode = INC;} 
		else if (strcmp(opcode_str, "DEC") == 0){opcode = DEC;} 
		else if (strcmp(opcode_str, "MUL") == 0){opcode = MUL;} 
		else if (strcmp(opcode_str, "IMUL") == 0){opcode = IMUL;} 
		else if (strcmp(opcode_str, "DIV") == 0){opcode = DIV;} 
		else if (strcmp(opcode_str, "IDIV") == 0){opcode = IDIV;} 
		else if (strcmp(opcode_str, "NEG") == 0){opcode = NEG;} 
		else if (strcmp(opcode_str, "CMP") == 0){opcode = CMP;} 
		else if (strcmp(opcode_str, "AND") == 0){opcode = AND;} 
		else if (strcmp(opcode_str, "OR") == 0){opcode = OR;} 
		else if (strcmp(opcode_str, "XOR") == 0){opcode = XOR;} 
		else if (strcmp(opcode_str, "MOV") == 0){opcode = MOV;} 
		else if (strcmp(opcode_str, "NOT") == 0){opcode = NOT;} 
		else if (strcmp(opcode_str, "TEST") == 0){opcode = TEST;} 
		else if (strcmp(opcode_str, "SHL") == 0){opcode = SHL;} 
		else if (strcmp(opcode_str, "SHR") == 0){opcode = SHR;} 
		else if (strcmp(opcode_str, "SAL") == 0){opcode = SAL;} 
		else if (strcmp(opcode_str, "SAR") == 0){opcode = SAR;} 
		else if (strcmp(opcode_str, "ROL") == 0){opcode = ROL;} 
		else if (strcmp(opcode_str, "ROR") == 0){opcode = ROR;} 
		else if (strcmp(opcode_str, "RCL") == 0){opcode = RCL;} 
		else if (strcmp(opcode_str, "RCR") == 0){opcode = RCR;}
		else if (strcmp(opcode_str, "MOV") == 0){opcode = MOV;} 
		else if (strcmp(opcode_str, "PUSH") == 0){opcode = PUSH;} 
		else if (strcmp(opcode_str, "POP") == 0){opcode = POP;} 
		else if (strcmp(opcode_str, "XCHG") == 0){opcode = XCHG;}
		else {std::cout << "未知指令" << std::endl;}
		//printf("%d",opcode);
		printf("操作码%s 目的操作数%s 源操作数%s\n",opcode_str,dest,source);
		excute(cpu,biu);
	}else{biu.queue_index = 0;}
	//instruction = biu.get_instruction();
	//biu.instructions.pop();
	//string parts[3][16];
	//	//sscanf从字符串中读取格式化输入的函数
	//	//sscanf返回成功接收到几个参数
//	printf("正在解码指令...");
	return;
}

void EU::excute(simple8086* cpu,BIU biu){
//	while(!biu.instructions.empty()){
//		
//	} 
	if(opcode<=12){
		int a,b;
		// 算数逻辑运算指令 
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

					//printf("%s %s",source,dest);
					if(strcmp(dest,"AX")==0){b = cpu->registers[AX];cpu->registers[AX]= ALU(cpu,ADD,a,b);}
					else if(strcmp(dest,"BX")==0){b = cpu->registers[BX];cpu->registers[BX]= ALU(cpu,ADD,a,b);}
					else if(strcmp(dest,"CX")==0){b = cpu->registers[CX];cpu->registers[CX]= ALU(cpu,ADD,a,b);}
					else if(strcmp(dest,"DX")==0){b = cpu->registers[DX];cpu->registers[DX]= ALU(cpu,ADD,a,b);}
					//printf("%d",a);
					printf("给寄存器%s加上%d\n",dest,a);
					
					
					//exit(0);
										
					//dest = ALU(cpu,ADD,a,b);
					//ALU()
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
		print_flags(cpu);
		//print_reg(cpu);	
	}
	
	else if(opcode>=MOV&&opcode<=XCHG){
		switch (opcode) {
			case MOV:
				write_value(cpu,dest,get_value(source,cpu));
				break;
			case PUSH:
				//TODO
				break;
			case POP:
				//TODO
				break;
			case XCHG:
				int temp = get_value(dest,cpu);
				write_value(cpu,dest,get_value(source,cpu));
				write_value(cpu,source,temp);
				break; 
//			default:
//				break;
		}
	}

	print_reg(cpu);
	memset(source, 0, sizeof(source));
	memset(dest, 0, sizeof(dest));
	
	return;
}
/*
		7种寻址方式可识别的写法
		没有偏移量的 
		立即数寻址 mov ax 2000
		寄存器寻址 mov ax bx
		寄存器间接寻址 mov ax [bx]
		直接寻址 mov ax [2000]
		相对基址寻址 mov ax [bx/bp/si/di+2000]
		基址变址寻址 mov ax [bx/bp+si/di]
		相对基址变址寻址 mov ax [bx/bp+si/di+2000] 
		*/

int get_value(char* operand,simple8086* cpu){
	int address,value;
	int a,b,c;	
	char base_str[8],offset_str[8], val_str[8];
	if(operand[0] == '['){
		int part_count = sscanf(operand,"[%[^+]+%[^+]+%[^]]]",base_str,offset_str,val_str);
		//printf("%s %s %s\n",base_str,offset_str,val_str);
		
		if(part_count==3){
			if(strcmp(base_str,"BX")==0){a = cpu->registers[BX];}
		    else if(strcmp(base_str,"BP")==0){a = cpu->registers[BP];}
		    
		    if(strcmp(offset_str,"SI")==0){b = cpu->registers[SI];}
		    else if(strcmp(offset_str,"DI")==0){b = cpu->registers[DI];}
		    c = atoi(val_str);
		    address = a + b + c;
		}else if(part_count==2){
			// 基址变址寻址
		    if(strcmp(base_str,"BX")==0){a = cpu->registers[BX];}
		    else if(strcmp(base_str,"BP")==0){a = cpu->registers[BP];}
		    else if(strcmp(base_str,"SI")==0){a = cpu->registers[SI];}
		    else if(strcmp(base_str,"DI")==0){a = cpu->registers[DI];}
		
		    if(strcmp(offset_str,"SI]")==0){b = cpu->registers[SI];}
		    else if(strcmp(offset_str,"DI]")==0){b = cpu->registers[DI];}
		    // 相对基址寻址
		    else{b = atoi(offset_str);}
		    //printf("hhhhhhhhhhhhhhhhhhhhhhhhhh%d",a+b);
		    address = a + b;
		}else if(part_count==1){
		 // 寄存器间接寻址
		    if(!strcmp(base_str,"BX]")){address = cpu->registers[BX];}
		    else if(!strcmp(base_str,"SI]")){address = cpu->registers[SI];}
		    else if(!strcmp(base_str,"DI]")){address = cpu->registers[DI];}
		    else if(!strcmp(base_str,"SP]")){address = cpu->registers[SP];}
		    else if(!strcmp(base_str,"BP]")){address = cpu->registers[BP];}
		    // 直接寻址
		    else{address = atoi(base_str);}
		}
		value = stoi(cpu->memory[address]);
	}else{
		if(!strcmp(operand,"AX")){value = cpu->registers[AX];}
		else if(!strcmp(operand,"BX")){value = cpu->registers[BX];}
		else if(!strcmp(operand,"CX")){value = cpu->registers[CX];}
		else if(!strcmp(operand,"DX")){value = cpu->registers[DX];}
		else if(!strcmp(operand,"SI")){value = cpu->registers[SI];}
		else if(!strcmp(operand,"DI")){value = cpu->registers[DI];}
		else if(!strcmp(operand,"SP")){value = cpu->registers[SP];}
		else if(!strcmp(operand,"BP")){value = cpu->registers[BP];}
		else{value = atoi(operand);}
	}
//	cout<<operand<<endl;
//	printf("%d\n",value); 
	return value;
}

void write_value(simple8086* cpu,char* operand,int value){
	int address;
	int a,b,c;	
	char base_str[8],offset_str[8], val_str[8];
	if(operand[0] == '['){
		int part_count = sscanf(operand,"[%[^+]+%[^+]+%[^]]]",base_str,offset_str,val_str);
		printf("%s %s %s\n",base_str,offset_str,val_str);
		
		if(part_count==3){
			if(strcmp(base_str,"BX")==0){a = cpu->registers[BX];}
		    else if(strcmp(base_str,"BP")==0){a = cpu->registers[BP];}
		    
		    if(strcmp(offset_str,"SI")==0){b = cpu->registers[SI];}
		    else if(strcmp(offset_str,"DI")==0){b = cpu->registers[DI];}
		    c = atoi(val_str);
		    address = a + b + c;
		}else if(part_count==2){
			// 基址变址寻址
		    if(strcmp(base_str,"BX")==0){a = cpu->registers[BX];}
		    else if(strcmp(base_str,"BP")==0){a = cpu->registers[BP];}
		    else if(strcmp(base_str,"SI")==0){a = cpu->registers[SI];}
		    else if(strcmp(base_str,"DI")==0){a = cpu->registers[DI];}
		
		    if(strcmp(offset_str,"SI]")==0){a = cpu->registers[SI];}
		    else if(strcmp(offset_str,"DI]")==0){a = cpu->registers[DI];}
		    // 相对基址寻址
		    else{b = atoi(offset_str);}
		    address = a + b;
		}else if(part_count==1){
		 // 寄存器间接寻址
		    if(!strcmp(base_str,"BX]")){address = cpu->registers[BX];}
		    else if(!strcmp(base_str,"SI]")){address = cpu->registers[SI];}
		    else if(!strcmp(base_str,"DI]")){address = cpu->registers[DI];}
		    else if(!strcmp(base_str,"SP]")){address = cpu->registers[SP];}
		    else if(!strcmp(base_str,"BP]")){address = cpu->registers[BP];}
		    // 直接寻址
		    else{address = atoi(base_str);}
		}
		cpu->memory[address]=value;
	}else{
		if(!strcmp(operand,"AX")){cpu->registers[AX]=value;}
		else if(!strcmp(operand,"BX")){cpu->registers[BX]=value;}
		else if(!strcmp(operand,"CX")){cpu->registers[CX]=value;}
		else if(!strcmp(operand,"DX")){cpu->registers[DX]=value;}
		else if(!strcmp(operand,"SI")){cpu->registers[SI]=value;}
		else if(!strcmp(operand,"DI")){cpu->registers[DI]=value;}
		else if(!strcmp(operand,"SP")){cpu->registers[SP]=value;}
		else if(!strcmp(operand,"BP")){cpu->registers[BP]=value;}
		else{value = atoi(operand);}
	}
}

void run(simple8086* cpu){
	printf("开始执行...\n");
	EU eu;
	BIU biu;
	while(1){
		biu.fetch(cpu);
		eu.decode(biu,cpu);
		//eu.excute(cpu,biu);
	}
}
