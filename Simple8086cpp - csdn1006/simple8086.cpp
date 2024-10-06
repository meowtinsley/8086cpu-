#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "simple8086.h"
#include<iostream>

void hlt(){
	printf("...ִֹͣ��\n");
	exit(0);
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
//���������������ǰ�λ�� 1111 1111 0111 1111=127 �з��ŷ�Χ 1000 0000-0111 1111 -128-127 �޷��ŷ�Χ 0-255 
//todo alu:�����������㵽������ñ�־λ������ �㲻��cf û��af 
/*�����AFλ�Ľ�� 
AFλ��������־λ
flag�ĵ�4λ��AF��������־λ����ӳ�Ӽ�����ʱ��Ͱ��ֽ����޽�λ���߽�λ����Ͱ��ֽ��н�λ���λʱ��AF=1������AF=0��
�����־λ��Ҫ�ɴ������ڲ�ʹ�ã�����ʮ������������ĵ���ָ��û�һ�㲻�ع��ġ�
PFλ: ��ż��־λ Ϊż��ʱ��һ 
SFλ: ���ű�־λ Ϊ����ʱ��һ 
ZFλ�����־λ Ϊ����һ
CFλ���޷��żӷ���λ������λ  �ӷ������в�����λ��Ӱ��CF��־
OFλ���з������� 
����SAL SHL���ߵ�ͬ�����λ����CF  
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
			// ���ab�����ķ���λ��ͬ ���ͽ���ķ���λ��ͬ���Ƿ�������� 
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
			/* �Ѳ���������һ���޷��Ŷ������� ��Ӱ��CF
			0111 1111+1 =1000 0000 ������һ��ɸ����������*/
			if(a==0b01111111) {cpu->status_flags[OF]=1;}
			res = a + 1;
			break;
		case DEC:
			/* �Ѳ���������һ���޷��Ŷ������� ��Ӱ��CF
			1000 0000-1=0111 1111 -128->127 �����޷�������128�����з�������-128*/
			res = a - 1;
			if(a==0b10000000) {cpu->status_flags[OF]=1;}
			break;
		case MUL:
			res = uint8_t(a*b);
			temp = uint8_t((a*b)>>8);
			/*����˻��߰벿�ֲ�Ϊ����CF��OF*/
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
			printf("\n��\n");
			print_8binary(b);
			printf("\n���\n");
			print_8binary(temp);
			printf(" ");
			print_8binary(res);
			printf("\n");
			break;
		case IMUL:
			res = a * b;
			if (a > 0 && b > 0 && a > 255 / b) {cpu->status_flags[OF] = 1;}// �����
			if (a < 0 && b < 0 && a < 255 / b) {cpu->status_flags[OF] = 1;}// �����
			if ((a > 0 && b < 0 && a < 255/ b) ||(a < 0 && b > 0 && b < 255/ a)){cpu->status_flags[OF] = 1;} // �����
			break;
		case DIV:
			res = a / b;
			temp = a % b;
			printf("%d����%d��%d...��%d\n",a,b,res,temp);
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
			printf("\n��\n");
			print_8binary(b);
			printf("\n���\n");
			print_8binary(res);
			printf("\n");
			break;
		case OR:
			res = a | b;
			print_8binary(a);
			printf("\n��\n");
			print_8binary(b);
			printf("\n���\n");
			print_8binary(res);
			printf("\n");
			break;
		case XOR:
			res = a ^ b;			
			print_8binary(a);
			printf("\n���\n");
			print_8binary(b);
			printf("\n���\n");
			print_8binary(res);
			printf("\n");
			break;
		case NOT:
			res = ~a;
			print_8binary(a);
			printf("\nȡ��\n");
			print_8binary(res);
			printf("\n");
			break;
		case SHL:
		case SAL:
			res = a<<1;	
			b = (a>>7)&1;//ȡ���λ 
			printf("����");
			print_8binary(a);
			printf("\n���");
			print_8binary(res);
			cpu->status_flags[CF] = b; 
			break;
		case SHR:
		case SAR:
			res = a>>1;	
			b = a % 2;//ȡ���������λ
			printf("����");
			print_8binary(a);
			printf("\n���");
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
	cout<<"��־�Ĵ�����״̬��"<<endl; 
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
		printf("\n���ڽ���ָ��...");
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
		else {std::cout << "δָ֪��" << std::endl;}
		printf("������%s Ŀ�Ĳ�����%s Դ������%s\n",opcode_str,dest,source);
		excute(cpu,biu);
	}else{biu.queue_index = 0;}
	return;
}

void EU::excute(simple8086* cpu,BIU biu){
	printf("����ִ��ָ��...");
	if(opcode<MOV){
		int a,b;
		// �����߼�����ָ�� 
		switch (opcode) {
				case HLT:
					hlt();
					break;
				case ADD:
					a = get_value(source,cpu);
					b = get_value(dest,cpu);
					write_value(cpu,dest,ALU(cpu,ADD,a,b));
					printf("���Ĵ���%s����%d\n",dest,a);
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
					printf("�Ĵ���AX�е�ֵ%d��%d\n",b,a);	
					ALU(cpu,MUL,a,b);
					//ALU(cpu,MUL,get_value(dest,cpu),cpu->registers[AX]);
					break;
				case IMUL:
					break;
				case DIV:
					a = (cpu->registers[DX]<<8) + cpu->registers[AX]; 
					b = get_value(dest,cpu);
					printf("�Ĵ���DX:AX�е�ֵ%d����%d\n",a,b);
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
				printf("��ջ�ε�ַ%d ��ַ%d ѹ��%d\n",cpu->registers[SP],cpu->registers[SP]+biu.registers[SS],get_value(dest,cpu));
				cpu->registers[SP]-=1;
				break;
			case POP:
				cpu->registers[SP]+=1;
				value = stoi(cpu->memory[cpu->registers[SP]+biu.registers[SS]]);
				printf("��ջ�ε�ַ%d ��ַ%d ����%d\n",cpu->registers[SP],cpu->registers[SP]+biu.registers[SS],value);
				write_value(cpu,dest,value);
				break;
			case XCHG:
				printf("���ڽ���%s %s",source,dest);
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
			// ��ַ��ַѰַ
		    if(strcmp(base_str,"BX")==0){a = cpu->registers[BX];}
		    else if(strcmp(base_str,"BP")==0){a = cpu->registers[BP];}
		    else if(strcmp(base_str,"SI")==0){a = cpu->registers[SI];}
		    else if(strcmp(base_str,"DI")==0){a = cpu->registers[DI];}
		
		    if(strcmp(offset_str,"SI]")==0){b = cpu->registers[SI];}
		    else if(strcmp(offset_str,"DI]")==0){b = cpu->registers[DI];}
		    // ��Ի�ַѰַ
		    else{b = atoi(offset_str);}
		    address = a + b;
		}else if(part_count==1){
		 // �Ĵ������Ѱַ
		    if(!strcmp(base_str,"BX]")){address = cpu->registers[BX];}
		    else if(!strcmp(base_str,"SI]")){address = cpu->registers[SI];}
		    else if(!strcmp(base_str,"DI]")){address = cpu->registers[DI];}
		    else if(!strcmp(base_str,"SP]")){address = cpu->registers[SP];}
		    else if(!strcmp(base_str,"BP]")){address = cpu->registers[BP];}
		    // ֱ��Ѱַ
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
			// ��ַ��ַѰַ
		    if(strcmp(base_str,"BX")==0){a = cpu->registers[BX];}
		    else if(strcmp(base_str,"BP")==0){a = cpu->registers[BP];}
		    else if(strcmp(base_str,"SI")==0){a = cpu->registers[SI];}
		    else if(strcmp(base_str,"DI")==0){a = cpu->registers[DI];}
		
		    if(strcmp(offset_str,"SI]")==0){a = cpu->registers[SI];}
		    else if(strcmp(offset_str,"DI]")==0){a = cpu->registers[DI];}
		    // ��Ի�ַѰַ
		    else{b = atoi(offset_str);}
		    address = a + b;
		}else if(part_count==1){
		 // �Ĵ������Ѱַ
		    if(!strcmp(base_str,"BX]")){address = cpu->registers[BX];}
		    else if(!strcmp(base_str,"SI]")){address = cpu->registers[SI];}
		    else if(!strcmp(base_str,"DI]")){address = cpu->registers[DI];}
		    else if(!strcmp(base_str,"SP]")){address = cpu->registers[SP];}
		    else if(!strcmp(base_str,"BP]")){address = cpu->registers[BP];}
		    // ֱ��Ѱַ
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
	printf("��ʼִ��...\n");
	EU eu;
	BIU biu;
	biu.registers[SS]=63;
	while(1){
		biu.fetch(cpu);
		eu.decode(biu,cpu);
	}
}
