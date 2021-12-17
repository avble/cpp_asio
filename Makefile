IO_OBJS = src/event_base.o
IO_OBJS += src/evt_tmr_utils.o
IO_OBJS += src/thread_pool.o
# thread-pool library
IO_OBJS += deps/thrpool.o
IO_LDFLAG = -lpthread
# Compile flag
IO_CPPFLAGS  = -I ./deps -I ./src -fcoroutines -g -fPIC

# TEST_EV_OBJS = test/event_test.o
# TEST_EV_CPPFLAG = -I ./src

# TEST_TIMER_OBJS = test/event_timer_test.o
# TEST_TIMER_CPPFLAG = -I ./src

# TEST_ASYNC_SLEEP_OBJS = test/test_async_sleep.o
# TEST_ASYNC_SLEEP_CPPFLAGS = -I ./src -g -O0
# TEST_ASYNC_SLEEP_LDFLAGS = -g

# TEST_TASK_OBJS = test/test_create_task.o
# TEST_TASK_CPPFLAGS = -I ./src -g -O0
# TEST_TASK_LDFLAGS = -g

# TEST_THPOOL_OBJS = deps/thrpool_ex1.o

# TEST_THPOOL_1_OBJS = test/test_thread_pool.o

TEST_IO_CORO_OBJS = test/test_io_coro.o

all:  lib_io
	echo "compile lib-io"

# test: test_event test_timer test_thrpool test_async_sleep test_task test_thread_pool test_io_coro
# 	echo "compile test"

# test_event: lib_io $(TEST_EV_OBJS) $(IO_OBJS)
# 	g++ -O0 -std=c++20 $(TEST_EV_OBJS) $(IO_OBJS) $(IO_LDFLAG) -o test_event

# test_timer: lib_io $(TEST_TIMER_OBJS) $(IO_OBJS)
# 	g++ -O0 -std=c++20 $(TEST_TIMER_OBJS) $(IO_OBJS) $(IO_LDFLAG) -o test/test_timer

# test_async_sleep: lib_io $(TEST_ASYNC_SLEEP_OBJS) $(IO_OBJS)
# 	g++ -O0 -std=c++20 $(TEST_ASYNC_SLEEP_OBJS) $(IO_OBJS) $(IO_LDFLAG) $(TEST_ASYNC_SLEEP_LDFLAGS) -o test/test_async_sleep

# test_thrpool: $(TEST_THPOOL_OBJS) $(IO_OBJS)
# 	g++ -O0 -std=c++20 $(TEST_THPOOL_OBJS) $(IO_OBJS) $(IO_LDFLAG) -o test_thrpool

# test_thread_pool: $(TEST_THPOOL_1_OBJS) $(IO_OBJS)
# 	g++ -O0 -std=c++20 $(TEST_THPOOL_1_OBJS) $(IO_OBJS) $(IO_LDFLAG) -o test/test_thread_pool

test_io_coro: $(TEST_IO_CORO_OBJS) $(IO_OBJS)
	g++ -O0 -std=c++20 $(TEST_IO_CORO_OBJS) $(IO_OBJS) $(IO_LDFLAG) -o test/test_io_coro

# test_task: $(TEST_TASK_OBJS) $(IO_OBJS)
# 	g++ -O0 -std=c++20 $(TEST_TASK_OBJS) $(IO_OBJS) $(TEST_TASK_LDFLAGS) -lpthread -o test/test_task

lib_io: $(IO_OBJS)
	g++ -shared -o lib_io.so $(IO_OBJS)

$(IO_OBJS): %.o: %.cpp
	g++ -c $(IO_CPPFLAGS)  $^ -o $@

# $(TEST_EV_OBJS): %.o: %.cpp
# 	g++ -c $(IO_CPPFLAGS) $(TEST_EV_CPPFLAG)  $^ -o $@

# $(TEST_TIMER_OBJS): %.o: %.cpp
# 	g++ -c -fcoroutines $(IO_CPPFLAGS) $(TEST_TIMER_CPPFLAG)  $^ -o $@

# $(TEST_THPOOL_OBJS): %.o: %.cpp
# 	g++ -c  $(IO_CPPFLAGS) $^ -o $@

# $(TEST_THPOOL_1_OBJS): %.o: %.cpp
# 	g++ -c $(IO_CPPFLAGS) $^ -o $@

$(TEST_IO_CORO_OBJS): %.o: %.cpp
	g++ -c $(IO_CPPFLAGS) $^ -o $@

# $(TEST_ASYNC_SLEEP_OBJS): %.o: %.cpp
# 	g++ -O0 -c -fcoroutines $(IO_CPPFLAGS) $(TEST_ASYNC_SLEEP_CPPFLAGS) $^ -o $@

# $(TEST_TASK_OBJS): %.o: %.cpp
# 	g++ -O0 -c -fcoroutines $^ $(TEST_TASK_CPPFLAGS) $(IO_CPPFLAGS) -o $@

clean:
	rm -rf $(IO_OBJS) $(TEST_EV_OBJS) $(TEST_TIMER_OBJS) $(TEST_ASYNC_SLEEP_OBJS)
