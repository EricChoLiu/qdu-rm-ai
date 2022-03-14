#include "snipe_detector.hpp"

#include <execution>

#include "opencv2/gapi/render.hpp"

void SnipeDetector::InitDefaultParams(const std::string &params_path) {
  cv::FileStorage fs(params_path,
                     cv::FileStorage::WRITE | cv::FileStorage::FORMAT_JSON);

  fs << "a" << 0;
  fs << "b" << 0;
  SPDLOG_DEBUG("Inited params.");
}

bool SnipeDetector::PrepareParams(const std::string &params_path) {
  cv::FileStorage fs(params_path,
                     cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
  if (fs.isOpened()) {
    params_.a = fs["a"];
    params_.b = fs["b"];
    return true;
  } else {
    SPDLOG_ERROR("Can not load params.");
    return false;
  }
}

void SnipeDetector::FindArmor(const cv::Mat &frame) {
  duration_armors_.Start();
  targets_.clear();

  (void)frame;

  // TODO :Realize
  // Formed an armor then please use method `SetModel(game::Model::kOUTPOST);`
  duration_armors_.Calc("Find Armors");
  targets_.emplace_back(Armor());
}

SnipeDetector::SnipeDetector() { SPDLOG_TRACE("Constructed."); }

SnipeDetector::SnipeDetector(const std::string &params_path,
                             game::Team enemy_team) {
  LoadParams(params_path);
  enemy_team_ = enemy_team;
  SPDLOG_TRACE("Constructed.");
}

SnipeDetector::~SnipeDetector() { SPDLOG_TRACE("Destructed."); }

void SnipeDetector::SetEnemyTeam(game::Team enemy_team) {
  enemy_team_ = enemy_team;
  SPDLOG_DEBUG("{}", game::TeamToString(enemy_team));
}

const tbb::concurrent_vector<Armor> &SnipeDetector::Detect(
    const cv::Mat &frame) {
  FindArmor(frame);
  return targets_;
}

void SnipeDetector::VisualizeResult(const cv::Mat &output, int verbose) {
  auto draw_armor = [&](Armor &armor) {
    auto prims = armor.VisualizeObject(verbose > 2);
    for (auto &prim : prims) prims_.emplace_back(prim);
  };
  if (verbose > 1) {
    std::string label = cv::format("Find %ld Armors in %ld ms", targets_.size(),
                                   duration_armors_.Count());
    prims_.emplace_back(draw::VisualizeLabel(label));
  }

  if (!targets_.empty()) {
    std::for_each(std::execution::par_unseq, targets_.begin(), targets_.end(),
                  draw_armor);
  }
  cv::Mat frame = output.clone();
  cv::gapi::wip::draw::render(frame, prims_);
}
