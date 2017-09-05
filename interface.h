#ifndef INTER_FACE_H
#define INTER_FACE_H

#include "Header.h"
#include "gms_matcher.h"
#include "FindFiles.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void imresize(Mat &src, int height);
cv::Mat DrawInlier(cv::Mat &src1, cv::Mat &src2, std::vector<cv::KeyPoint> &kpt1, std::vector<cv::KeyPoint> &kpt2, std::vector<cv::DMatch> &inlier, int type);
void init_match(std::string image_path, std::string save_path, int large_edge);
int do_match(std::string image_path, std::string feature_path, int large_edge, int thresh);
void runImagePair(std::string first_image, std::string second_image, int large_size);
void GmsMatch(cv::Mat &img1, cv::Mat &img2);

void init_match_folder(std::string folder_path, std::string save_path, int large_edge);
int do_match_folder(std::string image_path, std::string feature_folder, int large, int threh);

#endif