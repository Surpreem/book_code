add_executable(ex1_connectsync ex1_connectsync.cpp)
target_link_libraries(ex1_connectsync ws2_32)

add_executable(ex2_connectasync ex2_connectasync.cpp)
target_link_libraries(ex2_connectasync ws2_32)

add_executable(ex3_serverasync ex3_serverasync.cpp)
target_link_libraries(ex3_serverasync ws2_32 mswsock)

add_executable(ex4_readwritesocket ex4_readwritesocket.cpp)
target_link_libraries(ex4_readwritesocket ws2_32 mswsock)

add_executable(ex5_echoserver ex5_echoserver.cpp wrapper.cpp)
target_link_libraries(ex5_echoserver ws2_32 mswsock)

add_executable(ex6_clienthttpget ex6_clienthttpget.cpp wrapper.cpp)
target_link_libraries(ex6_clienthttpget ws2_32 mswsock)
