#include "ObjectPool.h"
#include "ConcurrentAlloc.h"

void Alloc1()
{
	for (size_t i = 0; i < 5; ++i)
	{
		void* ptr = ConcurrentAlloc(6);
	}
}

void Alloc2()
{
	for (size_t i = 0; i < 5; ++i)
	{
		void* ptr = ConcurrentAlloc(7);
	}
}

void TLSTest()
{
	std::thread t1(Alloc1);
	t1.join();

	std::thread t2(Alloc2);
	t2.join();
}

// 测试申请释放7次个8byte内不等的几个对象的申请和释放
// 通过调试观察演示讲解申请的过程和释放过程，
// 申请过程体现也分割和切分,释放过程体现出页的合并。
void TestConcurrentAlloc1()
{
	void* p1 = ConcurrentAlloc(6);
	void* p2 = ConcurrentAlloc(8);
	void* p3 = ConcurrentAlloc(1);
	void* p4 = ConcurrentAlloc(7);
	void* p5 = ConcurrentAlloc(8);
	void* p6 = ConcurrentAlloc(8);
	void* p7 = ConcurrentAlloc(8);


	cout << p1 << endl;
	cout << p2 << endl;
	cout << p3 << endl;
	cout << p4 << endl;
	cout << p5 << endl;
	cout << p6 << endl;
	cout << p7 << endl;


	ConcurrentFree(p1);
	ConcurrentFree(p2);
	ConcurrentFree(p3);
	ConcurrentFree(p4);
	ConcurrentFree(p5);
	ConcurrentFree(p6);
	ConcurrentFree(p7);
}

// 测试申请释放1024个8byte内不等的几个对象的申请，这1024次申请把第一个页分完了。
// 打断点，通过调试观察演示讲解第1025次申请，在page cache去切一个新span，验证前1024次申请和切分逻辑
void TestConcurrentAlloc2()
{
	for (size_t i = 0; i < 1024; ++i)
	{
		void* p1 = ConcurrentAlloc(6);
		cout << p1 << endl;
	}

	void* p2 = ConcurrentAlloc(8);
	cout << p2 << endl;
}

void TestAddressShift()
{
	PAGE_ID id1 = 2000;
	PAGE_ID id2 = 2001;
	char* p1 = (char*)(id1 << PAGE_SHIFT);
	char* p2 = (char*)(id2 << PAGE_SHIFT);
	while(p1 < p2)
	{
		cout << (void*)p1 <<":"<< ((PAGE_ID)p1 >> PAGE_SHIFT) << endl;
		p1 += 8;
	}
}

// 创建两个线程分别走一下申请释放10次，验证基本的多线程场景下申请释放是否有问题
void MultiThreadAlloc1()
{
	std::vector<void*> v;
	for (size_t i = 0; i < 10; ++i)
	{
		void* ptr = ConcurrentAlloc(6);
		cout <<std::this_thread::get_id() <<ptr << endl;
		v.push_back(ptr);
	}

	for (auto e : v)
	{
		ConcurrentFree(e);
	}
}

void MultiThreadAlloc2()
{
	std::vector<void*> v;
	for (size_t i = 0; i < 10; ++i)
	{
		void* ptr = ConcurrentAlloc(16);
		cout << std::this_thread::get_id() << ptr << endl;
		v.push_back(ptr);
	}

	for (auto e : v)
	{
		ConcurrentFree(e);
	}
}

void TestMultiThread()
{
	std::thread t1(MultiThreadAlloc1);
	std::thread t2(MultiThreadAlloc2);

	t1.join();
	t2.join();
}

// 申请释放一次257KB内存，验证大块内存申请释放，调试观察向page cache切分和合并大块页的逻辑
// 申请释放一次129*8KB内存，验证超大块内存申请释放，调试观察向堆申请释放内存逻辑
void BigAlloc()
{
	void* p1 = ConcurrentAlloc(257 * 1024);
	ConcurrentFree(p1);

	void* p2 = ConcurrentAlloc(129 * 8 * 1024);
	ConcurrentFree(p2);
}