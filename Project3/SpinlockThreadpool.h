#pragma once

#include <atomic>
#include <vector>
#include <future>
#include <boost/lockfree/queue.hpp>
#include "Spinlock.h"
#include <iostream>

class SpinlockThreadpool
{
public:
  SpinlockThreadpool(int p_threadCount = 8, int p_taskQueueSize = 200):
  m_taskQueue(p_taskQueueSize)
  {
    setThreadCount(p_threadCount);
  }

  ~SpinlockThreadpool()
  {
    setThreadCount(0);
  }

  void setThreadCount(int p_count)
  {
    auto currentCount = m_threads.size();
    if (currentCount < p_count)
    {
      m_threads.resize(p_count);
      m_stopFlags.resize(p_count);

      for (auto index = currentCount;
      index < p_count;
        index++)
      {
        m_stopFlags[index] = std::make_shared<std::atomic<bool>>(false);
        resetThread(int(index));
      }
    }
    else
    {
      for (int index = (int)(currentCount - 1);
      index >= p_count;
        index--)
      {
        (*m_stopFlags[index]) = true;
        m_threads[index]->detach();
      }
      
      m_mutex.lock();
      m_conditionVar.notify_all();
      m_mutex.unlock();

      m_threads.resize(p_count);
      m_stopFlags.resize(p_count);
    }
  }

  int threadCount()
  {
    return int(m_threads.size());
  }

  void addThread()
  {
    m_stopFlags.push_back(std::make_shared<std::atomic<bool>>(false));
    m_threads.push_back(nullptr);
    resetThread(int(m_threads.size() - 1));
  }

  void removeThread()
  {
    auto index = m_threads.size() - 1;
    (*m_stopFlags[index]) = true;
    m_threads[index]->detach();
    m_stopFlags.pop_back();
    m_threads.pop_back();
  }

  void waitAndDoTasks()
  {
    std::function<void()> * taskFunction;
    while(m_unfinishedTasks.load() != 0)
    {
      while(m_taskQueue.pop(taskFunction))
      {
        std::unique_ptr<std::function<void()>> func(taskFunction);
        (*taskFunction)();
        m_unfinishedTasks.fetch_sub(1);
      }
    }
  }

  void addTask(std::function<void()> * p_task)
  {
    m_unfinishedTasks.fetch_add(1);
    m_taskQueue.push(p_task);
//    std::unique_lock<std::mutex> lock(m_mutex);
//    m_conditionVar.notify_one();
  }

  template<typename F, typename... Rest>
  auto push(F && f, Rest&&... rest) ->std::future<decltype(f(rest...))>
  {
    auto pck = std::make_shared<std::packaged_task<decltype(f(rest...))()>>(
      std::bind(std::forward<F>(f), std::forward<Rest>(rest)...)
      );

    auto taskFunction = new std::function<void()>([pck]()
    {
      (*pck)();
    });

    m_unfinishedTasks.fetch_add(1);
    this->m_taskQueue.push(taskFunction);
    std::unique_lock<std::mutex> lock(m_mutex);
    m_conditionVar.notify_one();

    return pck->get_future();
  }

private:

  void resetThread(int p_threadIndex)
  {
    std::shared_ptr<std::atomic<bool>> threadStopFlag(m_stopFlags[p_threadIndex]);
    auto threadFunction = [this, p_threadIndex, threadStopFlag]()
    {
      std::atomic<bool> & stop = *threadStopFlag;
      std::function<void()> * taskFunction;
      while (!stop)
      {
        if (m_taskQueue.pop(taskFunction))
        {
          std::unique_ptr<std::function<void()>> func(taskFunction);
          (*taskFunction)();
          m_unfinishedTasks.fetch_sub(1);
        }
        else
        {
          std::unique_lock<std::mutex> lock(m_mutex);
          m_conditionVar.wait(lock);
        }
      }
    };
    m_threads[p_threadIndex].reset(new std::thread(threadFunction));
  }

  std::vector<std::shared_ptr<std::atomic<bool>>> m_stopFlags;
  std::vector<std::unique_ptr<std::thread>> m_threads;
  boost::lockfree::queue<std::function<void()> *> m_taskQueue;
  std::atomic<int> m_unfinishedTasks;

  std::mutex m_mutex;
  std::condition_variable m_conditionVar;

};