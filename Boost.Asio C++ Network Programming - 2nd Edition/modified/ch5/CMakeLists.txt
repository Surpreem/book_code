add_executable(ex1_nonstrand ex1_nonstrand.cpp)
target_link_libraries(ex1_nonstrand ws2_32)

add_executable(ex2_strand ex2_strand.cpp)
target_link_libraries(ex2_strand ws2_32)

add_executable(ex3_strandwrap ex3_strandwrap.cpp)
target_link_libraries(ex3_strandwrap ws2_32)

add_executable(ex4_exception ex4_exception.cpp)
target_link_libraries(ex4_exception ws2_32)

add_executable(ex5_exception2 ex5_exception2.cpp)
target_link_libraries(ex5_exception2 ws2_32)

add_executable(ex6_errorcode ex6_errorcode.cpp)
target_link_libraries(ex6_errorcode ws2_32)

add_executable(ex7_errorcode2 ex7_errorcode2.cpp)
target_link_libraries(ex7_errorcode2 ws2_32)

add_executable(ex8_timer ex8_timer.cpp)
target_link_libraries(ex8_timer ws2_32)

add_executable(ex9_timer2 ex9_timer2.cpp)
target_link_libraries(ex9_timer2 ws2_32)

add_executable(ex10_timer3 ex10_timer3.cpp)
target_link_libraries(ex10_timer3 ws2_32)
