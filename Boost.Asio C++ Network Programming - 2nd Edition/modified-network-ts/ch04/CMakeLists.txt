add_executable(ex01_nonconcurrent ex01_nonconcurrent.cpp)
add_executable(ex02_concurrent ex02_concurrent.cpp)

add_executable(ex03_unblocked ex03_unblocked.cpp)
target_link_libraries(ex03_unblocked ws2_32)

add_executable(ex04_blocked ex04_blocked.cpp)
target_link_libraries(ex04_blocked ws2_32)

add_executable(ex05_poll ex05_poll.cpp)
target_link_libraries(ex05_poll ws2_32)

add_executable(ex06_pollwork ex06_pollwork.cpp)
target_link_libraries(ex06_pollwork ws2_32)

add_executable(ex07_removework ex07_removework.cpp)
target_link_libraries(ex07_removework ws2_32)

add_executable(ex08_multithreads ex08_multithreads.cpp)
target_link_libraries(ex08_multithreads ws2_32)
