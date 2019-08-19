#pragma once
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <vector>
#include <queue>
#include <pthread.h>
namespace Core
{
	class ThreadPool;
	class MutexLock;
	class ThreadCondition;
	class ThreadTask;
	class ThreadNoncopyable
	{
	protected:
		ThreadNoncopyable() {}
		~ThreadNoncopyable() {}
	private:
		ThreadNoncopyable(const ThreadNoncopyable & rhs);
		ThreadNoncopyable & operator=(const ThreadNoncopyable &rhs);
	};
	

	class Thread :private ThreadNoncopyable
	{
	public:
		Thread(ThreadPool& pool);
		~Thread();
		void Start();
		void Join();
		void Deatch();
		void Run();
	private:
		static void* RunInThread(void *arg);
	private:
		ThreadPool &m_Pool;
		pthread_t m_ThreadID;
		bool m_IsRunning;
	};
	class MutexLock :private ThreadNoncopyable
	{
	public:
		MutexLock();
		~MutexLock();
		void Lock();
		void Unlock();
		bool IsLock() const { return m_locked; }
		pthread_mutex_t * GetPtr();
	private:
		pthread_mutex_t mutex;
		bool m_locked;
	};
	//class ThreadObject
	//{
	//public:
	//	ThreadObject();
	//private:
	//	MutexLock m_Lock;
	//	bool m_Locked;
	//public:
	//	void Lock();
	//	void UnLock();
	//	bool IsLocked();
	//};
	class EasyMutexLock
	{
	public:
		EasyMutexLock(MutexLock &lock)
			:mutex(lock)
		{
			mutex.Lock();
		}

		~EasyMutexLock()
		{
			mutex.Unlock();
		}
	private:
		MutexLock & mutex;
	};
	class ThreadCondition :private ThreadNoncopyable
	{
	public:
		ThreadCondition();
		~ThreadCondition();
		void Init(MutexLock *mutex);
		void Wait();
		void Notify();
		void NotifyAll();
	private:
		pthread_cond_t cond;
		MutexLock *mutex;
	};
	class ThreadBuffer
	{
	public:
		ThreadBuffer();
		~ThreadBuffer();
		void Push(ThreadTask* task);
		ThreadTask* Pop();
		bool Init(int size);
		bool Empty();
		bool Full();
	private:
		MutexLock mutex;
		ThreadCondition notfull;
		ThreadCondition notempty;
		int m_Size;
		std::queue<ThreadTask*> m_TaskQueue;
	};


	class ThreadTask
	{
	public:
		ThreadTask();
		~ThreadTask();
		virtual void Process() = 0;

	private:

	};


	class ThreadPool
	{
	public:
		ThreadPool();
		~ThreadPool();
		bool Start(int buff_size, int thread_count);
		void Stop();
		void AddTask(ThreadTask* task);
		ThreadTask* GetTask();
		void Process();
	private:
		ThreadBuffer m_Buffer;
		int m_ThreadCount;
		bool m_Running;
		std::vector<Thread*> m_ThreadList;
	};
}

#endif // !__THREADPOOL_H__
