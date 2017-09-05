#ifndef FIND_FILES_H
#define FIND_FILES_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <vector>

void LocalCheckDir(std::string &sDir);
std::string absolute_name(std::string input);
void FileWrite(const cv::Mat &img, std::vector<cv::KeyPoint> &keyPoint, const cv::Mat &src, const std::string &filename);
void FileRead(cv::Mat &img, std::vector<cv::KeyPoint> &keyPoint, cv::Mat &dst, std::string &filename);
int MyFindFile(std::string szPath, std::vector<std::string> szFilters, std::vector<std::string> &filenames, std::vector<std::string> *pdirnames = 0);
#endif