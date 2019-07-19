#ifndef MYJITEVENTLISTENER_HPP
#define MYJITEVENTLISTENER_HPP

#include "llvm/ExecutionEngine/JITEventListener.h"

#include <iostream>
#include <map>


class MyJITEventListener : public llvm::JITEventListener {

private:

public:
	MyJITEventListener(){}

	~MyJITEventListener(){}

	virtual void NotifyObjectEmitted(const llvm::object::ObjectFile &obj, const llvm::RuntimeDyld::LoadedObjectInfo &L){
		std::cout << "JIT OBJECT EMITED" << std::endl;
		//get binary into a file
		llvm::object::OwningBinary<llvm::object::ObjectFile>OWOF = L.getObjectForDebug(obj);
		llvm::object::ObjectFile &OF = *OWOF.getBinary();

	}
};



#endif