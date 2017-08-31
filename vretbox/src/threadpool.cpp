#include "threadpool.hpp"
#include <boost/thread/thread.hpp>
#include <cpluslogger.hpp>

vretbox::ThreadPool::ThreadPool(int _numThreads)
	:mNumThreads(_numThreads)
{
}

void vretbox::ThreadPool::run(int _id)
{
	boost::unique_lock<boost::mutex> guard(mQueueMutex);
	guard.unlock();

	while(!mTasks.empty())
	{
		guard.lock();
		boost::function<void()> task = mTasks.front();
		mTasks.pop();
		//LOG_INFO("Thread: " << _id << " " << mTasks.size());
		guard.unlock();
		task();
	}
}

void vretbox::ThreadPool::createAndJoinThreads()
{
	for (int i = 0; i < mNumThreads; i++)
	{
		mWorker.emplace_back(new boost::thread(boost::bind(&ThreadPool::run, this, i)));
	}

	for(int i = 0; i < mWorker.size(); i++)
	{
		mWorker.at(i)->join();
	}
}

vretbox::ThreadPool::~ThreadPool(){ }

void vretbox::ThreadPool::addTask(boost::function<void()> _task)
{
	mTasks.push(_task);
}

