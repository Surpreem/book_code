add_executable(ex2_concurrent ex2_concurrent.cpp)

add_executable(ex3_unblocked ex3_unblocked.cpp)
target_link_libraries(ex3_unblocked ws2_32)

add_executable(ex4_blocked ex4_blocked.cpp)
target_link_libraries(ex4_blocked ws2_32)

add_executable(ex5_poll ex5_poll.cpp)
target_link_libraries(ex5_poll ws2_32)

add_executable(ex6_pollwork ex6_pollwork.cpp)
target_link_libraries(ex6_pollwork ws2_32)

add_executable(ex7_removework ex7_removework.cpp)
target_link_libraries(ex7_removework ws2_32)

add_executable(ex8_multithreads ex8_multithreads.cpp)
target_link_libraries(ex8_multithreads ws2_32)

add_executable(ex9_uncalledbind ex9_uncalledbind.cpp)
target_link_libraries(ex9_uncalledbind ws2_32)

add_executable(ex10_calledbind ex10_calledbind.cpp)
target_link_libraries(ex10_calledbind ws2_32)

add_executable(ex11_argumentbind ex11_argumentbind.cpp)
target_link_libraries(ex11_argumentbind ws2_32)

add_executable(ex12_signaturebind ex12_signaturebind.cpp)
target_link_libraries(ex12_signaturebind ws2_32)

add_executable(ex13_classbind ex13_classbind.cpp)
target_link_libraries(ex13_classbind ws2_32)

add_executable(ex14_ioservicebind ex14_ioservicebind.cpp)
target_link_libraries(ex14_ioservicebind ws2_32)

add_executable(ex15_mutexbind ex15_mutexbind.cpp)
target_link_libraries(ex15_mutexbind ws2_32)

add_executable(ex16_post ex16_post.cpp)
target_link_libraries(ex16_post ws2_32)

add_executable(ex17_dispatch ex17_dispatch.cpp)
target_link_libraries(ex17_dispatch ws2_32)
