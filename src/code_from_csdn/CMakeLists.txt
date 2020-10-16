include_directories(./include)

add_library(SocketMatTransmissionServer src/SocketMatTransmissionServer.cpp)
add_library(SocketMatTransmissionClient src/SocketMatTransmissionClient.cpp)
add_executable(SocketClientMat src/SocketClientMat.cpp)
add_executable(SocketServerMat src/SocketServerMat.cpp)

target_link_libraries(SocketClientMat SocketMatTransmissionClient ${OpenCV_LIBS})
target_link_libraries(SocketServerMat SocketMatTransmissionServer ${OpenCV_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/code_from_csdn)       #指定可执行文件储存的路径
