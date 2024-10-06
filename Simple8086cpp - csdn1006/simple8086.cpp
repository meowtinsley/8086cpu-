#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "simple8086.h"
#include<iostream>

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

void Init_simple8086(simple8086* cpu) {
    cpu->ip = 0;
    memset(cpu->registers, 0, sizeof(cpu->registers));
    memset(cpu->status_flags, 0, sizeof(cpu->status_flags));
    cpu->data_bus = 0;
    cpu->control_bus_read = false;
    cpu->control_bus_write = false;
}

void print_8binary(int num){
	int arr[8];
	for(int i=0;i<8;i++){
		arr[i]=num>>(7-i)&1;
		printf("%d",arr[i]);
		if(i==3){printf(" ");}
	}
} 
//假设参与运算的数是八位的 1111 1111 0111 1111=127 有符号范围 1000 0000-0111 1111 -128-127 无符号范围 0-255 
//todo alu:各种算术运算到底如何置标志位啊啊啊 搞不懂cf 没搞af 
/*不想搞AF位的借口 
AF位：调整标志位
flag的第4位是AF，调整标志位；反映加减运算时最低半字节有无进位或者借位。最低半字节有进位或借位时，AF=1，否则AF=0。
这个标志位主要由处理器内部使用，用于十进制算术运算的调整指令，用户一般不必关心。
PF位: 奇偶标志位 为偶数时置一 
SF位: 符号标志位 为负数时置一 
ZF位：零标志位 为零置一
CF位：无符号加法进位减法借位  加法操作中产生进位不影响CF标志
OF位：有符号运算 
对于SAL SHL二者等同，最高位移入CF  
*/
bool get_sign(int8_t num){
	return (num>>7)&1;
}
int ALU(simple8086* cpu,int opcode,int a,int b){
	int res,temp=0;
	char* dest;
	switch (opcode) {
		case ADD:
			res=a + b;
			//if(res>=0b10000000){cpu->status_flags[OF]=1;}else{cpu->status_flags[CF]=0;} 
			// 如果ab两数的符号位相同 但和结果的符号位不同就是发生了溢出 
			if((a>>7)==(b>>7)&&(res>>7)!=(a>>7)){
				cpu->status_flags[OF]=1;
			}else{
				cpu->status_flags[OF]=0;
			} 
			if(a+b>0b11111111){
				cpu->status_flags[CF]=1;
			}else{
				res=uint8_t(res);
				cpu->status_flags[CF]=0;
			} 
			break;
		case ADC:
			res = a + b + cpu->status_flags[CF];
			if((a>>7)==(b>>7)&&(res>>7)!=(a>>7)){
				cpu->status_flags[OF]=1;
			}else{
				cpu->status_flags[OF]=0;
			} 
			if(a+b>0b11111111){
				cpu->status_flags[CF]=1;
			}else{
				res=uint8_t(res);
				cpu->status_flags[CF]=0;
			} 
			break;
		case SUB:
			res = a - b;
			
			break;
		case SBB:
			res = a - b - cpu->status_flags[CF];
			
			break;
		case INC:
			/* 把操作数看做一个无符号二进制数 不影响CF
			0111 1111+1 =1000 0000 正数加一变成负数发生溢出*/
			if(a==0b01111111) {cpu->status_flags[OF]=1;}
			res = a + 1;
			break;
		case DEC:
			/* 把操作数看做一个无符号二进制数 不影响CF
			1000 0000-1=0111 1111 -128->127 看作无符号数是128看作有符号数是-128*/
			res = a - 1;
			if(a==0b10000000) {cpu->status_flags[OF]=1;}
			break;
		case MUL:
			res = uint8_t(a*b);
			temp = uint8_t((a*b)>>8);
			/*如果乘积高半部分不为零置CF和OF*/
			if(temp!=0){
				cpu->status_flags[OF]=1;
				cpu->status_flags[CF]=1;
			}else{
				cpu->status_flags[OF]=0;
				cpu->status_flags[CF]=0;				
			}
			strcpy(dest,"DX");
			write_value(cpu,dest,temp);
			strcpy(dest,"AX");
			write_value(cpu,dest,res);
			print_8binary(a);
			printf("\n乘\n");
			print_8binary(b);
			printf("\n结果\n");
			print_8binary(temp);
			printf(" ");
			print_8binary(res);
			printf("\n");
			break;
		case IMUL:
			res = a * b;
			if (a > 0 && b > 0 && a > 255 / b) {cpu->status_flags[OF] = 1;}// 正溢出
			if (a < 0 && b < 0 && a < 255 / b) {cpu->status_flags[OF] = 1;}// 负溢出
			if ((a > 0 && b < 0 && a < 255/ b) ||(a < 0 && b > 0 && b < 255/ a)){cpu->status_flags[OF] = 1;} // 负溢出
			break;
		case DIV:
			res = a / b;
			temp = a % b;
			printf("%d除以%d商%d...余%d\n",a,b,res,temp);
			cpu->registers[AX]=res;
			cpu->registers[DX]=temp;
			break;
		case IDIV:
			
			break;	
		case NEG:
			res = 0 - a;
			break; 
		case CMP:

			break;
			
		case TEST:
		case AND:
			res = a & b;
			print_8binary(a);
			printf("\n与\n");
			print_8binary(b);
			printf("\n结果\n");
			print_8binary(res);
			printf("\n");
			break;
		case OR:
			res = a | b;
			print_8binary(a);
			printf("\n或\n");
			print_8binary(b);
			printf("\n结果\n");
			print_8binary(res);
			printf("\n");
			break;
		case XOR:
			res = a ^ b;			
			print_8binary(a);
			printf("\n异或\n");
			print_8binary(b);
			printf("\n结果\n");
			print_8binary(res);
			printf("\n");
			break;
		case NOT:
			res = ~a;
			print_8binary(a);
			printf("\n取反\n");
			print_8binary(res);
			printf("\n");
			break;
		case SHL:
		case SAL:
			res = a<<1;	
			b = (a>>7)&1;//取最高位 
			printf("左移");
			print_8binary(a);
			printf("\n结果");
			print_8binary(res);
			cpu->status_flags[CF] = b; 
			break;
		case SHR:
		case SAR:
			res = a>>1;	
			b = a % 2;//取二进制最低位
			printf("右移");
			print_8binary(a);
			printf("\n结果");
			print_8binary(res); 
			cpu->status_flags[CF] = b;
			break;
		default:
			break;
	}
	if(res==0){cpu->status_flags[ZF] = 1;}else{cpu->status_flags[ZF] = 0;}
	if(res%2==0){cpu->status_flags[PF] = 1;}else{cpu->status_flags[PF] = 0;}
	if(int8_t(res)<0){cpu->status_flags[SF] = 1;}else{cpu->status_flags[SF] = 0;}
	return res;	
}

void print_flags(simple8086* cpu){
	cout<<"标志寄存器的状态："<<endl; 
	printf("\tCF:%d PF:%d ZF:%d SF:%d OF:%d\n",
			cpu->status_flags[CF],
			cpu->status_flags[PF],
			cpu->status_flags[ZF],
			cpu->status_flags[SF],
			cpu->status_flags[OF]);
}


void EU::decode(BIU& biu,simple8086* cpu){
	memset(source, 0, sizeof(source));
	memset(dest, 0, sizeof(dest));	
	if(!biu.instruction_queue_isempty()){
		char opcode_str[16];
		instruction = biu.get_instruction();
		printf("\n正在解码指令...");
		cout<<instruction<<endl;
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
		printf("操作码%s 目的操作数%s 源操作数%s\n",opcode_str,dest,source);
		excute(cpu,biu);
	}else{biu.queue_index = 0;}
	return;
}

void EU::excute(simple8086* cpu,BIU biu){
	printf("正在执行指令...");
	if(opcode<MOV){
		int a,b;
		// 算数逻辑运算指令 
		switch (opcode) {
				case HLT:
					hlt();
					break;
				case ADD:
					a = get_value(source,cpu);
					b = get_value(dest,cpu);
					write_value(cpu,dest,ALU(cpu,ADD,a,b));
					printf("给寄存器%s加上%d\n",dest,a);
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
					a = get_value(dest,cpu);
					b = cpu->registers[AX];
					printf("寄存器AX中的值%d乘%d\n",b,a);	
					ALU(cpu,MUL,a,b);
					//ALU(cpu,MUL,get_value(dest,cpu),cpu->registers[AX]);
					break;
				case IMUL:
					break;
				case DIV:
					a = (cpu->registers[DX]<<8) + cpu->registers[AX]; 
					b = get_value(dest,cpu);
					printf("寄存器DX:AX中的值%d除以%d\n",a,b);
					ALU(cpu,DIV,a,b);
					break;
				case IDIV:
					
					break;	
				case NEG:
					
					break; 
				case CMP:
		
					break;
				case TEST:
					a = get_value(source,cpu);
					b = get_value(dest,cpu);
					ALU(cpu,TEST,a,b);
					break;					
				case AND:
					a = get_value(source,cpu);
					b = get_value(dest,cpu);
					write_value(cpu,dest,ALU(cpu,AND,a,b)); 
					break;
				case OR:
					a = get_value(source,cpu);
					b = get_value(dest,cpu);
					write_value(cpu,dest,ALU(cpu,OR,a,b)); 					
					break;
				case XOR:
					a = get_value(source,cpu);
					b = get_value(dest,cpu);
					write_value(cpu,dest,ALU(cpu,XOR,a,b)); 							
					break;
				case NOT:
					a = get_value(dest,cpu);
					b = 0;
					write_value(cpu,dest,ALU(cpu,NOT,a,b));
					break;

				case SHL:
				case SAL:
					a = get_value(dest,cpu);
					b = 0;
					write_value(cpu,dest,ALU(cpu,SAL,a,b));
					break;	
				case SHR:
				case SAR:
					a = get_value(dest,cpu);
					b = 0;
					write_value(cpu,dest,ALU(cpu,SAR,a,b));		
					break;											
			}
		print_flags(cpu);
	
	}
	else if(opcode>=MOV){
		int temp,value;
		switch (opcode) {
			case MOV:
				write_value(cpu,dest,get_value(source,cpu));
				break;
			case PUSH:
				cpu->memory[cpu->registers[SP]+biu.registers[SS]]=to_string(get_value(dest,cpu));
				printf("堆栈段地址%d 地址%d 压入%d\n",cpu->registers[SP],cpu->registers[SP]+biu.registers[SS],get_value(dest,cpu));
				cpu->registers[SP]-=1;
				break;
			case POP:
				cpu->registers[SP]+=1;
				value = stoi(cpu->memory[cpu->registers[SP]+biu.registers[SS]]);
				printf("堆栈段地址%d 地址%d 弹出%d\n",cpu->registers[SP],cpu->registers[SP]+biu.registers[SS],value);
				write_value(cpu,dest,value);
				break;
			case XCHG:
				printf("正在交换%s %s",source,dest);
				temp = get_value(dest,cpu);
				write_value(cpu,dest,get_value(source,cpu));
				write_value(cpu,source,temp);
				break; 
			default:
				break;
		}
	}

	print_reg(cpu);

	
	return;
}

int get_value(char* operand,simple8086* cpu){
	int address,value;
	int a,b,c;	
	char base_str[8],offset_str[8], val_str[8];
	if(operand[0] == '['){
		int part_count = sscanf(operand,"[%[^+]+%[^+]+%[^]]]",base_str,offset_str,val_str);
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
	biu.registers[SS]=63;
	while(1){
		biu.fetch(cpu);
		eu.decode(biu,cpu);
	}
}
