include_directories(./include)

add_library(SocketMatTransmissionClient src/SocketMatTransmissionClient.cpp)
add_executable(SocketClientMat src/SocketClientMat.cpp)
target_link_libraries(SocketClientMat SocketMatTransmissionClient ${OpenCV_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/rec_as_client)       #指定可执行文件储存的路径
