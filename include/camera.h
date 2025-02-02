/**
 * @file camera.h
 * @brief 摄像头类，对摄像头的功能做一些基本的封装。
 * @date 2024-10-19
 */
#pragma once

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

#include "treasure_chest/observer/core.h"
#include "treasure_chest/producer_consumer/core.h"

namespace arm_face_id {

using treasure_chest::pattern::Observer;
using treasure_chest::pattern::Producer;
using treasure_chest::pattern::Subject;

/**
 * @brief
 * Camera 承担摄像头的基础操作，如配置本地和网络摄像头，打开并读取摄像头。
 * 同时 Camera 将视频帧的更新通知给需要的模块，这些模块需要调用 Camera 的
 * AddObserver<cv::Mat> 方法向 Camera 告知其需要收到通知。
 */
class Camera : public Producer<cv::Mat>,
               public Observer<cv::Mat>,
               public Subject {
 public:
  // 摄像头相关配置
  struct Settings {
    // 网络摄像头 url
    std::string cam_url = "";
    // 本地摄像头设备 index
    int cam_index = 0;
    // 是否启用网络摄像头
    bool enable_net_cam = true;
  };

  Camera() = delete;
  Camera(const Settings &, treasure_chest::pattern::SyncQueue<cv::Mat> &);

  bool Open();
  void Close();
  /**
   * @brief
   * 打开并开始读取摄像头，这会阻塞当前线程。
   */
  void Start();
  bool IsRunning() const { return is_running_; }

  void SetReadInterval(int interval) {
    interval_msec_ = interval > 0 ? interval : interval_msec_;
  }

  void OnNotify(const cv::Mat &message) override;

 private:
  cv::VideoWriter *writer_ = nullptr;
  int interval_msec_ = 30;
  Settings settings_;
  bool is_running_ = false;
  cv::VideoCapture cap_;
};
}  // namespace arm_face_id
