#ifndef _THREADPOOL_HPP_
#define _THREADPOOL_HPP_

#include <queue>
#include <boost/thread/thread.hpp>
#include <boost/function.hpp>

namespace vretbox
{
	
	class ThreadPool
	{
		std::vector<boost::thread*> mWorker;

		int mNumThreads;

		std::queue<boost::function<void()>> mTasks;

		boost::mutex mQueueMutex;

		boost::unique_lock<boost::mutex> mLock;

	public:

		ThreadPool(int _numThreads);

		void run(int _id);

		void createAndJoinThreads();

		~ThreadPool();

		void addTask(boost::function<void()> _task);
	};

}


#endif //_THREADPOOL_HPP_