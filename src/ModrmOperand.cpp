/*-------------------------------------*\
|
|  File Name: ModrmOperand.cpp
|
|  Creation Date: 28-09-2012
|
|  Last Modified: Thu, Oct  4, 2012  7:21:26 PM
|
|  Created By: Robert Nelson
|
\*-------------------------------------*/

#include "ModrmOperand.hpp"
#include "Processor.hpp"
#include "RegisterOperand.hpp"

#include <cstring>

ModrmOperand::ModrmOperand(Processor* proc, unsigned int addr, unsigned int size, unsigned int bytelen )
	: mAddr(addr), mSize(size), mProc(proc), mByteLen(bytelen) {}

unsigned int ModrmOperand::GetValue() {

	return mProc->GetMemory(mAddr, mSize);
}

void ModrmOperand::SetValue(unsigned int val) {

	mProc->SetMemory(mAddr, mSize, val);
}

Operand* ModrmOperand::GetRegister(Processor* proc, unsigned int val, unsigned int size) {
	switch(val) {
		case 0x00:
			if(size == 1)
				return new RegisterOperand(REG_AL, proc);
			else if(size == 2)
				return new RegisterOperand(REG_AX, proc);
			return 0;
			break;
		case 0x01:
			if(size == 1)
				return new RegisterOperand(REG_CL, proc);
			else if(size == 2)
				return new RegisterOperand(REG_CX, proc);
			return 0;
			break;
		case 0x02:
			if(size == 1)
				return new RegisterOperand(REG_DL, proc);
			else if(size == 2)
				return new RegisterOperand(REG_DX, proc);
			return 0;
			break;
		case 0x03:
			if(size == 1)
				return new RegisterOperand(REG_BL, proc);
			else if(size == 2)
				return new RegisterOperand(REG_BX, proc);
			return 0;
			break;
		case 0x04:
			if(size == 1)
				return new RegisterOperand(REG_AH, proc);
			else if(size == 2)
				return new RegisterOperand(REG_SP, proc);
			return 0;
			break;
		case 0x05:
			if(size == 1)
				return new RegisterOperand(REG_CH, proc);
			else if(size == 2)
				return new RegisterOperand(REG_BP, proc);
			return 0;
			break;
		case 0x06:
			if(size == 1)
				return new RegisterOperand(REG_DH, proc);
			else if(size == 2)
				return new RegisterOperand(REG_SI, proc);
			return 0;
			break;
		case 0x07:
			if(size == 1)
				return new RegisterOperand(REG_BH, proc);
			else if(size == 2)
				return new RegisterOperand(REG_DI, proc);
			return 0;
			break;
		default:
			return 0;
		}

}

Operand* ModrmOperand::GetSegRegister(Processor* proc, unsigned int sreg) {

	switch(sreg) {
		case 0x00:
			return new RegisterOperand(REG_ES, proc);
			break;
		case 0x01:
			return new RegisterOperand(REG_CS, proc);
			break;
		case 0x02:
			return new RegisterOperand(REG_SS, proc);
			break;
		case 0x03:
			return new RegisterOperand(REG_DS, proc);
			break;
		default:
			return 0;
	}

}

Operand* ModrmOperand::GetModrmOperand(Processor* proc, unsigned char* inst, eModRm position, unsigned int size) {

	unsigned int addr = 0;
	unsigned int disp = 0;

	unsigned char* modrm = inst + 1;
	unsigned int byteCodeLen = 0;

	if(position == ModrmOperand::REG) {
		return GetRegister(proc, (*modrm & 0x38) >> 3, size);
	} else if(position == ModrmOperand::SEGREG) {
		return GetSegRegister(proc, (*modrm & 0x38) >> 3);
	}


	//switch on mod field
	switch((*modrm & 0xC0) >> 6) {
		case 0x00:
			//No displacement or
			//Special case for direct mem access
			if((*modrm & 0x07) == 6) {
				disp = *(modrm + 1) + ((*(modrm + 2)) << 8);
				byteCodeLen = 2;
			}
			break;
		case 0x01:
			//8 bit displacement
			disp = *(modrm + 1);
			byteCodeLen = 1;
			break;
		case 0x02:
			//16 bit displacement
			disp = *(modrm + 1) + ((*(modrm + 2)) << 8);
			byteCodeLen = 2;
			break;
		case 0x03:
			//return a register Operand
			return GetRegister(proc, (*modrm & 0x07), size);
			break;
	}

	switch(*modrm & 0x07) {
		case 0x00:
			addr = proc->GetRegister(REG_BX) + proc->GetRegister(REG_SI);
			break;
		case 0x01:
			addr = proc->GetRegister(REG_BX) + proc->GetRegister(REG_DI);
			break;
		case 0x02:
			addr = proc->GetRegister(REG_BP) + proc->GetRegister(REG_SI);
			break;
		case 0x03:
			addr = proc->GetRegister(REG_BP) + proc->GetRegister(REG_DI);
			break;
		case 0x04:
			addr = proc->GetRegister(REG_SI);
			break;
		case 0x05:
			addr = proc->GetRegister(REG_DI);
			break;
		case 0x06:
			addr = proc->GetRegister(REG_BP);
			break;
		case 0x07:
			addr = proc->GetRegister(REG_BX);
			break;
	}

	return new ModrmOperand(proc, (addr + disp) % 0x10000, size, byteCodeLen);
}

unsigned int ModrmOperand::GetBitmask() {
	unsigned int retval = 0;
	for(unsigned int i = 0; i < mSize; i++)
		retval = (retval << 8) + 0xFF;
	return retval;
}

unsigned int ModrmOperand::GetBytecodeLen() {
	return mByteLen;

}
