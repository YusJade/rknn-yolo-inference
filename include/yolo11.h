
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

#include "iyolo.h"
#include "third_party/utils/common.h"
#include "third_party/utils/image_utils.h"
#include "third_party/yolo11/cpp/yolo11.h"

namespace rknn_yolo_inference {

// Yolo11 version implement, using rknn.
class Yolo11 : public IYolo {
 public:
  Yolo11(std::string model);

  vector<DetectResult> Detect(cv::Mat img0) override;

  string ToString() override { return "I`m Yolo11."; }

  bool Save() { return true; }

 private:
  rknn_app_context_t rknn_app_ctx_;
  cv::VideoWriter* video_writer_ = nullptr;
  std::vector<cv::Mat> processed_frames;
};
}  // namespace rknn_yolo_inference
