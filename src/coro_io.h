#ifndef __CORO_IO_H__
#define __CORO_IO_H__

#include <coroutine>
#include <iostream>


struct Task {
  struct promise_type {
    // std::vector<int> v{};
    Task get_return_object() {
      // std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << std::endl;
      return {};
    }
    std::suspend_never initial_suspend() { 
      // std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << std::endl;
      return {}; 
      }
    std::suspend_never final_suspend() noexcept { 
      // std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << std::endl;
      return {}; 
      }
    void unhandled_exception() {}
  };
};


struct AwaitData{
    std::coroutine_handle<Task::promise_type> h;
};

struct AwaitObject {

    struct AwaitData *pAwaitData;

    AwaitObject(struct AwaitData *pAwaitData)
    {
      this->pAwaitData = pAwaitData;
    }
    // always await
    bool await_ready() {
      // std::cout << "AwaitObject: " << __FILE__ << ":" << __LINE__ << std::endl;
      return false; 
      }
    void await_suspend(std::coroutine_handle<Task::promise_type> _h) {
        // std::cout << "AwaitableTask: " << __FILE__ << ":" << __LINE__ << std::endl;
        // std::cout << "AwaitObject: " << std::hex << pAwaitData << std::endl;
        pAwaitData->h = _h;
    }
    void await_resume() {}
};

#endif