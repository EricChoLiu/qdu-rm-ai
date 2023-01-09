#include "guidinglight_detector.hpp"

#include "spdlog/spdlog.h"

void GuidingLightDetector::InitDefaultParams(const std::string &params_path) {
  cv::FileStorage fs(params_path,
                     cv::FileStorage::WRITE | cv::FileStorage::FORMAT_JSON);
  // params_.write(fs);
  fs << "thresholdStep" << 10;
  fs << "minThreshold" << 0;
  fs << "maxThreshold" << 100;

  fs << "minRepeatability" << 2;
  fs << "minDistBetweenBlobs" << 10;

  fs << "filterByColor" << static_cast<int>(true);
  fs << "blobColor" << 0;

  fs << "filterByArea" << static_cast<int>(true);
  fs << "minArea" << 200;
  fs << "maxArea" << 5000;

  fs << "filterByCircularity" << static_cast<int>(false);
  fs << "minCircularity" << 0.1f;
  fs << "maxCircularity" << std::numeric_limits<float>::max();

  fs << "filterByInertia" << static_cast<int>(true);
  fs << "minInertiaRatio" << 0.2f;
  fs << "maxInertiaRatio" << std::numeric_limits<float>::max();

  fs << "filterByConvexity" << static_cast<int>(true);
  fs << "minConvexity" << 0.65f;
  fs << "maxConvexity" << std::numeric_limits<float>::max();
  SPDLOG_DEBUG("Inited params.");
}

bool GuidingLightDetector::PrepareParams(const std::string &params_path) {
  cv::FileStorage fs(params_path,
                     cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
  if (fs.isOpened()) {
    params_.read(fs.root());
    return true;
  } else {
    SPDLOG_ERROR("Cannot load params.");
    return false;
  }
}

void GuidingLightDetector::FindGuidingLight(const cv::Mat &frame) {
  duration_lights_.Start();
  targets_.clear();

  detector_->detect(frame, key_points_);
  if (!key_points_.empty()) {
    for (auto &kpt : key_points_) {
      GuidingLight light(kpt);
      targets_.emplace_back(light);
    }
    SPDLOG_DEBUG("Found keypoints");
  } else {
    SPDLOG_ERROR("None keypoints");
  }

  duration_lights_.Calc("Find Lights");
}

GuidingLightDetector::GuidingLightDetector() { SPDLOG_TRACE("Constructed."); }

GuidingLightDetector::GuidingLightDetector(const std::string &params_path) {
  LoadParams(params_path);
  detector_ = cv::SimpleBlobDetector::create(params_);
  SPDLOG_TRACE("Constructed.");
}

GuidingLightDetector::~GuidingLightDetector() { SPDLOG_TRACE("Destructed."); }

void GuidingLightDetector::ResetByParam(cv::SimpleBlobDetector::Params param) {
  detector_ = cv::SimpleBlobDetector::create(param);
  SPDLOG_DEBUG("Parameter has been reset.");
}

const tbb::concurrent_vector<GuidingLight> &GuidingLightDetector::Detect(
    const cv::Mat &frame) {
  SPDLOG_DEBUG("Detecting");
  FindGuidingLight(frame);
  SPDLOG_DEBUG("Detected.");
  return targets_;
}

void GuidingLightDetector::VisualizeResult(const cv::Mat &output, int verbose) {
  if (verbose > 1) {
    std::string label = cv::format("%ld lights in %ld ms.", targets_.size(),
                                   duration_lights_.Count());
    draw::VisualizeLabel(output, label);
  }
  cv::drawKeypoints(output, key_points_, output, draw::kGREEN);
}
