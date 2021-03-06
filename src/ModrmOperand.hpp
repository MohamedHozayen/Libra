/*-------------------------------------*\
|
|  File Name: ModrmOperand.hpp
|
|  Creation Date: 28-09-2012
|
|  Last Modified: Thu, Oct 18, 2012  9:37:36 PM
|
|  Created By: Robert Nelson
|
\*-------------------------------------*/

#pragma once

#include "Memory.hpp"
#include "Operand.hpp"

class Processor;

class ModrmOperand : public Operand {

	public:
		enum eModRm {
			MOD,
			REG,
			SEGREG,
		};

		void SetValue(unsigned int addr);
		unsigned int GetValue() { return GetValue(0); }
		unsigned int GetValue(unsigned int size);
		int GetUnresolvedValue() const;

		static Operand* GetModrmOperand(Processor* proc, Memory::MemoryOffset& inst, eModRm modrm, unsigned int size);

		unsigned int GetBitmask();
		unsigned int GetBytecodeLen();

		const std::string GetDisasm();

		Memory::MemoryOffset& GetAddress() { return mAddr; }


	protected:
		//Pass it the location of the opcode
		ModrmOperand(Processor* proc, const Memory::MemoryOffset& addr, unsigned int size, unsigned int bytelen);

		static Operand* GetRegister(Processor* proc, unsigned int val, unsigned int size);
		static Operand* GetSegRegister(Processor* proc, unsigned int val);
		Memory::MemoryOffset mAddr;
		unsigned int mSize;
		Processor* mProc;
		unsigned int mByteLen;
};
