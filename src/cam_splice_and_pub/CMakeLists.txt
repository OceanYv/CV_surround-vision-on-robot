include_directories(./include)

add_library(SocketMatTransmissionServer src/SocketMatTransmissionServer.cpp)
add_library(SocketMatTransmissionClient src/SocketMatTransmissionClient.cpp)
add_library(using_a_cam src/using_a_cam.cpp)

add_executable(SocketClientMat src/SocketClientMat.cpp)
add_executable(SocketServerMat src/SocketServerMat.cpp)
add_executable(ope_cam_by_class_client src/ope_cam_by_class_client.cpp)

target_link_libraries(SocketClientMat SocketMatTransmissionClient ${OpenCV_LIBS})
target_link_libraries(SocketServerMat SocketMatTransmissionServer ${OpenCV_LIBS})
target_link_libraries(ope_cam_by_class_client SocketMatTransmissionClient using_a_cam ${OpenCV_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/cam_splice_and_pub)       #指定可执行文件储存的路径
