#pragma once

#include <vector>

#include "armor.hpp"
#include "common.hpp"
#include "opencv2/opencv.hpp"

class ArmorClassifier {
 private:
  float scale_;
  double conf_;
  bool swap_rb_;
  int in_width_, in_height_, class_id_;
  std::vector<game::Model> classes_;
  cv::Scalar mean_;
  cv::dnn::Net net_;
  cv::Size net_input_size_;
  cv::Mat blob_;
  game::Model model_;

 public:
  ArmorClassifier();
  ArmorClassifier(const std::string model_path);
  ~ArmorClassifier();

  void LoadModel(const std::string &path);

  void Train();
  void ClassifyModel(Armor &armor);
  void VisualizeResult(const cv::Mat &output, int verbose = 1);
};
