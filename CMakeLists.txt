cmake_minimum_required(VERSION 3.19)
project(githou)

set(CMAKE_CXX_STANDARD 14)

add_executable(githou ThreadCache.h SizeClass.h CentralCache.h Span.h SpanList.h PageCache.h "Benchmark.cpp .cpp" Common.h ConcurrentAlloc.h CentralCache.h.cpp ThreadCache.cpp PageCache.cpp UnitTest.cpp)