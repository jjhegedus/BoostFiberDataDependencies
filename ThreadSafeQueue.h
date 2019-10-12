#pragma once

#include <queue>
#include <mutex>

namespace ndtech {

  template <typename ItemType>
  struct ThreadSafeQueue {

    std::queue<ItemType> m_q;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    void Push(ItemType item) {
      std::lock_guard lock(m_mutex);
      m_q.push(item);
      m_cv.notify_one();
    }

    ItemType Pop() {
      std::unique_lock lock(m_mutex);
      while (m_q.empty())
      {
        m_cv.wait(lock);
      }
      ItemType val = m_q.front();
      m_q.pop();
      return val;
    }

  };

}