/*-------------------------------------*\
|
|  File Name: Add.cpp
|
|  Creation Date: 26-09-2012
|
|  Last Modified: Sat, Oct  6, 2012 12:00:52 PM
|
|  Created By: Robert Nelson
|
\*-------------------------------------*/

#include "Add.hpp"
#include "../Prefix.hpp"
#include "../Processor.hpp"
#include "../ImmediateOperand.hpp"
#include "../RegisterOperand.hpp"
#include "../ModrmOperand.hpp"

#include <cstdlib>
#include <cstdio>

Add::Add(Prefix* pre, std::string text, std::string inst, int op) 
{
	mPrefix = pre;
       	mText = text;
       	mInst = inst;
       	mOpcode = op;
       	mValid = true;
}

Instruction* Add::CreateInstruction(unsigned char* memLoc, Processor* proc) {

	unsigned char* opLoc = memLoc;
	int prefixLen = 0;
	char buf[65];
	int tInt1 = 0;
	unsigned char modrm = 0;

	Prefix* prefix = 0;

	Instruction* newAdd = 0;

	//Build a prefix if possible
	prefix = Prefix::GetPrefix(memLoc);

	//Start looking after the prefix
	if(prefix) {
		opLoc += prefix->GetLength();
		prefixLen += prefix->GetLength();
	}


	std::string inst;
	//Switch for the different valid opcodes
	switch(*opLoc) {
		case ADD_AL_BYTE:
			sprintf(buf, "ADD AL, 0x%02X", (int)*(opLoc + 1));

			inst.insert(0, (char*)memLoc, prefixLen + 2);	

			newAdd = new Add(prefix, buf, inst, (unsigned char)*opLoc);
			newAdd->SetOperand(Operand::SRC, new ImmediateOperand(*(opLoc + 1), 1));
			newAdd->SetOperand(Operand::DST, new RegisterOperand(REG_AL, proc));

			break;
		case ADD_AX_WORD:
			tInt1 = (unsigned char)*(opLoc + 1);
			tInt1 |= (((unsigned char)*(opLoc + 2)) << 8);

			sprintf(buf, "ADD AX, 0x%04X", tInt1);

			inst.insert(0, (char*)memLoc, prefixLen + 3);

			newAdd = new Add(prefix, buf, inst, (unsigned char)*opLoc);
			newAdd->SetOperand(Operand::SRC, new ImmediateOperand(tInt1, 2));
			newAdd->SetOperand(Operand::DST, new RegisterOperand(REG_AX, proc));

			break;

		case GRP1_ADD_MOD_IMM8:
		case GRP1_ADD_MOD_IMM16:
		case GRP1_ADD_MOD_SIMM8:

			modrm = *(opLoc + 1);

			if(((modrm & 0x38) >> 3) == 0) {
				unsigned int immSize = (*opLoc == GRP1_ADD_MOD_IMM8) ? 1 : 2;

				Operand* dst = ModrmOperand::GetModrmOperand(
							proc, opLoc, ModrmOperand::MOD, immSize);

				tInt1 = (int)*(opLoc+2+dst->GetBytecodeLen());
				if(immSize == 2) {
					if(*opLoc == GRP1_ADD_MOD_IMM16) {
						tInt1 += ((int)*(opLoc+3+dst->GetBytecodeLen())) << 8;
					}else {
						tInt1 = (tInt1 >= 0x80) ? 0xFF00 + tInt1 : tInt1;
					}
				}

				if(immSize == 1)
					sprintf(buf, "ADD %s, 0x%02X", "", tInt1);
				else
					sprintf(buf, "ADD %s, 0x%04X", "", tInt1);

				inst.insert(0, (char*)memLoc, prefixLen + 2 + immSize + dst->GetBytecodeLen() - (*opLoc == GRP1_ADD_MOD_SIMM8 ? 1 : 0));
				newAdd = new Add(prefix, buf, inst, (unsigned char)*opLoc);
				newAdd->SetOperand(Operand::SRC, new ImmediateOperand(tInt1, immSize));
				newAdd->SetOperand(Operand::DST, dst);
			}
			break;

		case ADD_MOD_REG8:
		case ADD_MOD_REG16:
			{
				unsigned int size = *opLoc == ADD_MOD_REG8 ? 1 : 2;
				modrm = *(opLoc + 1);
				Operand* dst = ModrmOperand::GetModrmOperand(
						proc, opLoc, ModrmOperand::MOD, size);
				Operand* src = ModrmOperand::GetModrmOperand(
						proc, opLoc, ModrmOperand::REG, size);
				sprintf(buf, "ADD %s, %s", "", "");
				inst.insert(0, (char*)memLoc, prefixLen + 2 + dst->GetBytecodeLen() + src->GetBytecodeLen());
				newAdd = new Add(prefix, buf, inst, (unsigned char)*opLoc);
				newAdd->SetOperand(Operand::SRC, src);
				newAdd->SetOperand(Operand::DST, dst);
				
				break;
			}

		case ADD_REG8_MOD:
		case ADD_REG16_MOD:
			{
				unsigned int size = *opLoc == ADD_REG8_MOD ? 1 : 2;

				modrm = *(opLoc + 1);
				Operand* dst = ModrmOperand::GetModrmOperand(
						proc, opLoc, ModrmOperand::REG, size);
				Operand* src = ModrmOperand::GetModrmOperand(
						proc, opLoc, ModrmOperand::MOD, size);
				sprintf(buf, "ADD %s, %s", "", "");
				inst.insert(0, (char*)memLoc, prefixLen + 2 + dst->GetBytecodeLen() + src->GetBytecodeLen());
				newAdd = new Add(prefix, buf, inst, (unsigned char)*opLoc);
				newAdd->SetOperand(Operand::SRC, src);
				newAdd->SetOperand(Operand::DST, dst);
				
				break;


			}

		default:
			break;
	}

	return newAdd;

}

int Add::Execute(Processor* proc) {

	unsigned int parity = 0;

	Operand* dst = mOperands[Operand::DST];
	Operand* src = mOperands[Operand::SRC];

	unsigned int newVal = dst->GetValue();
	unsigned int srcVal = src->GetValue();
	newVal += srcVal;
	proc->SetFlag(FLAGS_CF, newVal > dst->GetBitmask());
	newVal &= dst->GetBitmask();

	proc->SetFlag(FLAGS_OF, newVal >= 0x80 && dst->GetValue() < 0x80);
	proc->SetFlag(FLAGS_SF, newVal >= 0x80);
	proc->SetFlag(FLAGS_ZF, newVal == 0x00);
	proc->SetFlag(FLAGS_AF, (newVal & ~0x0F) != 0);

	parity = newVal;
	parity ^= parity >> 16;
	parity ^= parity >> 8;
	parity ^= parity >> 4;
	parity &= 0x0f;
	proc->SetFlag(FLAGS_PF, (0x6996 >> parity) & 1);

	dst->SetValue(newVal);

	return 0;
}


