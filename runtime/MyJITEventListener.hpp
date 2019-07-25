#ifndef MYJITEVENTLISTENER_HPP
#define MYJITEVENTLISTENER_HPP

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Object/SymbolSize.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <map>


class MyJITEventListener : public llvm::JITEventListener {

private:

public:
	MyJITEventListener(){}

	~MyJITEventListener(){}

	virtual void NotifyObjectEmitted(const llvm::object::ObjectFile &obj, const llvm::RuntimeDyld::LoadedObjectInfo &L){}
		llvm::object::OwningBinary<llvm::object::ObjectFile>OWOF = L.getObjectForDebug(obj);
		llvm::object::ObjectFile &OF = *OWOF.getBinary();

		/* Enabling perf mappings */
        pid_t pid = getpid();
        std::string fname = "/tmp/perf-" + std::to_string(pid) + ".map";
        FILE* fperf = fopen(fname.c_str(), "w");
        FILE* fsection = fopen("content.bin", "w");
      
		for (const std::pair<llvm::object::SymbolRef, uint64_t> &P : llvm::object::computeSymbolSizes(OF)) {
			llvm::object::SymbolRef Sym = P.first;
         
			llvm::Expected<llvm::StringRef> eName = Sym.getName();
			if(!eName)
				continue;
			llvm::StringRef Name = *eName;

			llvm::Expected<uint64_t> eAddr = Sym.getAddress();
			if(!eAddr)
				continue;
			uint64_t Addr = *eAddr;

			uint64_t Size = P.second;

			std::string NameS = Name.str();

			fprintf(fperf,"%llx %x %s\n", Addr, P.second, NameS.c_str());

			//get jited machine code
            if(Size != 0){
            	fname = NameS + ".jit";
            	FILE* fsection = fopen(fname.c_str(), "w");
            	uint8_t *AddrP =  reinterpret_cast<uint8_t *>(Addr);
            	for (int i = 0; i < Size; i++){
            		fprintf(fsection,"0x%hx ", *AddrP++);
            	}
            	fclose(fsection);
            }
              
	}
		fclose(fperf);

	}
};



#endif