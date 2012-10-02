/*-------------------------------------*\
|
|  File Name: Processor.hpp
|
|  Creation Date: 25-09-2012
|
|  Last Modified: Mon, Oct  1, 2012  9:22:12 PM
|
|  Created By: Robert Nelson
|
\*-------------------------------------*/

#pragma once

#include <vector>

#include "Instruction.hpp"
#include "Register.hpp"

enum eRegisters {
	REG_AX,
	REG_BX,
	REG_CX,
	REG_DX,
	REG_SI,
	REG_DI,
	REG_BP,
	REG_SP,
	REG_CS,
	REG_DS,
	REG_SS,
	REG_ES,
	REG_IP,
	REG_FLAGS,
	NumRegisters,
	REG_AL,
	REG_BL,
	REG_CL,
	REG_DL,
	LowRegisters,
	REG_AH,
	REG_BH,
	REG_CH,
	REG_DH,
	HighRegisters,
	AllRegisters=HighRegisters,
};

enum eFlags {
	FLAGS_CF = 0,
	FLAGS_PF = 2,
	FLAGS_AF = 4,
	FLAGS_ZF = 6,
	FLAGS_SF = 7,
	FLAGS_TF = 8,
	FLAGS_IF = 9,
	FLAGS_DF = 10,
	FLAGS_OF = 11,
};
	

class Processor {

	public:
		int Initialize(unsigned int startAddr = 0x0000);
		Processor(unsigned char* mem);
		int Step();

		static const int PROC_SUCCESS		= 0;
		static const int PROC_ERR_INV_ADDR 	= 1;
		static const int PROC_ERR_INV_INST 	= 2;
		static const int PROC_ERR_INST		= 3;

		bool GetFlag(eFlags flag);
		void SetFlag(eFlags flag, bool val);

		unsigned int GetRegister(eRegisters reg);
		void SetRegister(eRegisters reg, unsigned int val);

		unsigned int GetRegisterLow(eRegisters reg);
		void SetRegisterLow(eRegisters reg, unsigned int val);

		unsigned int GetRegisterHigh(eRegisters reg);
		void SetRegisterHigh(eRegisters reg, unsigned int val);

		void ProcDump();


		
	private:
		
		int Execute(Instruction* inst);
		
		Register	mRegisters[NumRegisters];
		unsigned char*	mMem;

};
