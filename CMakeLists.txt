cmake_minimum_required(VERSION 3.10.2)

project(rknn-yolo-inference)

add_subdirectory(third_party)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_AUTOMOC ON)

find_package(OpenCV CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
# find_package(matplotlib_cpp CONFIG REQUIRED)
# find_package(Qt6 COMPONENTS Core Widgets SerialPort CONFIG)

set(SRCS 
  src/main.cpp 
  # src/yolov7.cpp 
  src/yolov7_dnn.cpp 
  src/camera.cpp 
  src/yolov5-lite.cpp
  src/yolov5.cpp
)

if(${Qt6_FOUND})
  set(Qt_LIBS Qt6::Core Qt6::Widgets Qt6::SerialPort)
  qt_wrap_cpp(SRCS)
else()
  find_package(
    Qt5
    COMPONENTS Core Widgets SerialPort
    REQUIRED CONFIG)
  set(Qt_LIBS Qt5::Core Qt5::Widgets Qt5::SerialPort)
  qt5_wrap_cpp(SRCS)
endif()

add_executable(run_inference ${SRCS})

set(LIBRKNNRT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rknpu2/Linux/aarch64/librknnrt.so)
message(STATUS ${LIBRKNNRT})

target_link_libraries(
  run_inference
  ${Qt_LIBS}
  # Qt6::Core Qt6::Widgets
  spdlog::spdlog
  absl::flags
  absl::flags_parse
  absl::flags_usage
  ${OpenCV_LIBS}
  imageutils
  fileutils
  imagedrawing    
  ${LIBRKNNRT}
  dl
  Threads::Threads
  stdc++fs
  # yolov7
  )

target_include_directories(
  run_inference
  PUBLIC include
         ${OpenCV_INCLUDE_DIRS}
         ${CMAKE_CURRENT_SOURCE_DIR}
         ${TREASURE_CHEST_INCLUDE_DIRS}
         ${LIBRKNNRT_INCLUDES}
         third_party
         third_party/treasure_chest
         third_party/yolov7/cpp
         third_party/yolov5
         third_party/rknpu2/include)
