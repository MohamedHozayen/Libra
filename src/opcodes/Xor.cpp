/*-------------------------------------*\
|
|  File Name: Xor.cpp
|
|  Creation Date: 10-10-2012
|
|  Last Modified: Wed, Oct 10, 2012 11:08:49 AM
|
|  Created By: Robert Nelson
|
\*-------------------------------------*/

#include "Xor.hpp"
#include "../Prefix.hpp"
#include "../Processor.hpp"
#include "../ImmediateOperand.hpp"
#include "../ModrmOperand.hpp"
#include "../RegisterOperand.hpp"

#include <cstdlib>
#include <cstdio>

Xor::Xor(Prefix* pre, std::string text, std::string inst, int op) : Instruction(pre,text,inst,op) {}

Instruction* Xor::CreateInstruction(unsigned char* memLoc, Processor* proc) {
	unsigned char* opLoc = memLoc;
	unsigned int preSize = 0;
	char buf[65];
	std::string inst;
	Instruction* newXor = 0;

	Prefix* pre = Prefix::GetPrefix(memLoc);

	if(pre) {
		opLoc += preSize = pre->GetLength();
	}

	switch(*opLoc) {
		case XOR_AL_IMM8:
		case XOR_AX_IMM16:
		{
			unsigned int size = (*opLoc) == XOR_AL_IMM8 ? 1 : 2;
			unsigned int val = (int)*(opLoc + 1);
			if(size == 2) {
				val += (int)*(opLoc + 2) << 8;
			}

			GETINST(preSize + 1 + size);
			snprintf(buf, 65, "XOR %s, %s", "", "");

			Operand* dst = new RegisterOperand(size == 1 ? REG_AL : REG_AX, proc);
			Operand* src = new ImmediateOperand(val, size);
			newXor = new Xor(pre, buf, inst, (int)*opLoc);
			newXor->SetOperand(Operand::SRC, src);
			newXor->SetOperand(Operand::DST, dst);
			break;
		}
		case XOR_MOD8_IMM8:
		case XOR_MOD16_IMM16:
		case XOR_MOD16_IMM8:
		{
			if(((*(opLoc + 1) & 0x38) >> 3) != XOR_REG_CONST)
				break;

			unsigned int size = *opLoc == XOR_MOD8_IMM8 ? 1 : 2;
			Operand* dst = ModrmOperand::GetModrmOperand(proc, opLoc, ModrmOperand::MOD, size);

			unsigned int val = (int)*(opLoc + 2 +dst->GetBytecodeLen());
			if(size == 2) {
				if(*opLoc == XOR_MOD16_IMM16)
					val += (int)*(opLoc + 3 + dst->GetBytecodeLen()) << 8;
				else
					val += val >= 0x80 ? 0xFF00 : 0x0000;
			}

			Operand* src = new ImmediateOperand(val, size);
			
			GETINST(preSize + 2 + (*opLoc == XOR_MOD16_IMM8 ? 1 : size) + dst->GetBytecodeLen());
			snprintf(buf, 65, "XOR %s, %s", "", "");

			newXor = new Xor(pre, buf, inst, (int)*opLoc);
			newXor->SetOperand(Operand::SRC, src);
			newXor->SetOperand(Operand::DST, dst);
			break;
		}
		case XOR_MOD8_REG8:
		case XOR_MOD16_REG16:
		case XOR_REG8_MOD8:
		case XOR_REG16_MOD16:
		{
			unsigned int size = (*opLoc == XOR_MOD8_REG8 || *opLoc == XOR_REG8_MOD8) ? 1 : 2;
			
			Operand* dst = ModrmOperand::GetModrmOperand(
					proc,
					opLoc,
					(*opLoc == XOR_MOD8_REG8 || *opLoc == XOR_MOD16_REG16) ? 
						ModrmOperand::MOD : ModrmOperand::REG,
					size);
			Operand* src = ModrmOperand::GetModrmOperand(
					proc,
					opLoc,
					(*opLoc == XOR_REG8_MOD8 || *opLoc == XOR_REG16_MOD16) ? 
						ModrmOperand::MOD : ModrmOperand::REG,
					size);

			GETINST(preSize + 2 + src->GetBytecodeLen() + dst->GetBytecodeLen());
			snprintf(buf, 65, "XOR %s, %s", "", "");
			newXor = new Xor(pre, buf, inst, (int)*opLoc);
			newXor->SetOperand(Operand::SRC, src);
			newXor->SetOperand(Operand::DST, dst);
			break;
		}
	}

	return newXor;

}

int Xor::Execute(Processor* proc) {
	Operand* dst = mOperands[Operand::DST];
	Operand* src = mOperands[Operand::SRC];

	if(dst == 0 || src == 0)
		return -1;

	unsigned int val = dst->GetValue() ^ src->GetValue();

	proc->SetFlag(FLAGS_OF, 0);
	proc->SetFlag(FLAGS_CF, 0);

	proc->SetFlag(FLAGS_ZF, val == 0);
	proc->SetFlag(FLAGS_PF, Parity(val));
	proc->SetFlag(FLAGS_SF, val >= (dst->GetBitmask() == 0xFF ? 0x80 : 0x8000));

	dst->SetValue(val);

	return 0;
}
