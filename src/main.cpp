#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <absl/flags/flag.h>
#include <absl/flags/internal/flag.h>
#include <absl/flags/internal/parse.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

#include "camera.h"
#include "sync_queue/core.h"
#include "yolov7.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

ABSL_FLAG(std::string, serial_port, "ttyS3", "use which serial port");
ABSL_FLAG(std::string, classes, "classes.txt", "the classes.txt file");
ABSL_FLAG(std::string, model, "yolov5.onnx", "onnx model path");
ABSL_FLAG(bool, gui, false, "open with gui");
ABSL_FLAG(int, camera_index, 0, "native camera device index");
ABSL_FLAG(std::string, camera_url, "http://localhost:4747/video",
          "network camera url");

using arm_face_id::Camera;
using treasure_chest::pattern::SyncQueue;
using yolo_cvdnn_inference::Yolov7;

std::vector<std::string> load_classes(std::string file) {
  std::ifstream file_stream(file);
  std::string class_name;
  std::vector<std::string> classes;
  while (std::getline(file_stream, class_name)) {
    classes.push_back(class_name);
  }
  return std::move(classes);
}

int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);

  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] [%s:%#] %v");
  spdlog::set_level(spdlog::level::debug);
  SPDLOG_DEBUG("initialized logger");
  SPDLOG_DEBUG("this is a script to inference with yolo.");

  std::vector<std::string> classes = load_classes(absl::GetFlag(FLAGS_classes));
  Yolov7 yolo;
  yolo.Load({1, 3, 640, 640}, classes, absl::GetFlag(FLAGS_model));

  SyncQueue<cv::Mat> detect_task_queue;
  Camera::Settings camera_settings;
  camera_settings.cam_index = absl::GetFlag(FLAGS_camera_index);
  camera_settings.cam_url = absl::GetFlag(FLAGS_camera_url);
  camera_settings.enable_net_cam = true;
  Camera cam(camera_settings, detect_task_queue);
  cam.Open();
  std::thread cam_thread([&] { cam.Start(); });
  std::thread detect_thread([&] {
    while (true) {
      cv::Mat mat = detect_task_queue.Dequeue();
      auto start = std::chrono::high_resolution_clock::now();
      yolo.Detect(mat);
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> duration = end - start;
      SPDLOG_INFO("detected frame, FPS: {}", 1 / duration.count());
    }
  });

  cam_thread.join();

  return 0;
}
