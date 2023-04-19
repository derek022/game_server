#include "sylar/sylar.h"


sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();


void fun1()
{
	SYLAR_LOG_INFO(g_logger) << "name: " << sylar::Thread::GetName()
                             << " this.name: " << sylar::Thread::GetThis()->getName()
                             << " id: " << sylar::GetThreadId()
                             << " this.id: " << sylar::Thread::GetThis()->getId();

}

void fun2()
{
	while(true)
		SYLAR_LOG_INFO(g_logger) << " xxxxxxxxxxxxxxxxxxxxxxxxxxxx ";
	
}

void fun3()
{
	while(true)
	{
		SYLAR_LOG_INFO(g_logger) << " ============================ ";
	}
}


int main(int argc, char** argv)
{
	SYLAR_LOG_INFO(g_logger) <<  "thread test begin" ;
	std::vector<sylar::Thread::ptr> thrs;

	for(int i = 0;i< 5;i++){
		sylar::Thread::ptr thr(new sylar::Thread(&fun1,"name_" + std::to_string(i * 2)));
		sylar::Thread::ptr thr2(new sylar::Thread(&fun2,"name_" + std::to_string(i * 2)));
		thrs.push_back(thr);
		thrs.push_back(thr2);
	}

	for(size_t i = 0;i < thrs.size(); ++i)
	{
		thrs[i]->join();
	}



	return 0;
}

