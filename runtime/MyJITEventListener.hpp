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

	virtual void NotifyObjectEmitted(const llvm::object::ObjectFile &obj, const llvm::RuntimeDyld::LoadedObjectInfo &L){
		std::cout << "JIT OBJECT EMITED" << std::endl;
		//TODO get binary into a file
		llvm::object::OwningBinary<llvm::object::ObjectFile>OWOF = L.getObjectForDebug(obj);
		llvm::object::ObjectFile &OF = *OWOF.getBinary();

		/* Enabling perf mappings */
        pid_t pid = getpid();
        std::string fname = "/tmp/perf-" + std::to_string(pid) + ".map";
        FILE* fperf = fopen(fname.c_str(), "w");
        FILE* fsection = fopen("content.s", "w");
      
		for (const std::pair<llvm::object::SymbolRef, uint64_t> &P : llvm::object::computeSymbolSizes(OF)) {
			std::cout << "BEGIN FOR" << std::endl;
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
                
            /*if (Sym.getType() != llvm::object::SymbolRef::ST_Function)
				continue;*/
			llvm::Expected<llvm::object::section_iterator> eSecIt = Sym.getSection();
			if(!eSecIt)
				continue;
			llvm::object::section_iterator SecIt = *eSecIt;
			llvm::object::SectionRef SecRef = *SecIt;
			if(SecRef.isText()){
			/*llvm::Expected<llvm::StringRef> eContent = SecRef.getContents();
			if(!eContent)
				continue;
			llvm::StringRef Content = *eContent;*/
			llvm::StringRef Content;
			llvm::StringRef& RefContent = Content;
			std::error_code EC = SecRef.getContents(RefContent);
			
			std::string ContentS = Content.str();
			//std::cout << ContentS << std::endl;
			fprintf(fsection, "%s", ContentS.c_str());
			}



            fprintf(fperf,"%llx %x %s\n", Addr, P.second, NameS.c_str());
              
	}
        fclose(fperf);
        fclose(fsection);

	}
};



#endif