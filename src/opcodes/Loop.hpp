/*-------------------------------------*\
|
|  File Name: Loop.hpp
|
|  Creation Date: 04-03-2012
|
|  Last Modified: Mon, Mar 04, 2013  2:41:34 PM
|
|  Created By: Robert Nelson
|
\*-------------------------------------*/

#pragma once

#include "../Instruction8086.hpp"

class Loop : public Instruction8086 {
	public:
		static Instruction* CreateInstruction(Memory::MemoryOffset& memLoc, Processor* proc);
		int Execute();

		enum eValidOpcodes {
			LOOP		= 0xE2,
			LOOPE		= 0xE1,
			LOOPNE		= 0xE0
		};

	protected:
		Loop(Prefix* pre, std::string text, std::string inst, int op);
};
