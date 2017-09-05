#include "interface.h"
using namespace std;
using namespace cv;

void imresize(Mat &src, int height)
{
	double ratio = src.rows * 1.0 / height;
	int width = static_cast<int>(src.cols * 1.0 / ratio);
	resize(src, src, Size(width, height));
}

Mat DrawInlier(Mat &src1, Mat &src2, vector<KeyPoint> &kpt1, vector<KeyPoint> &kpt2, vector<DMatch> &inlier, int type) {
	const int height = max(src1.rows, src2.rows);
	const int width = src1.cols + src2.cols;
	Mat output(height, width, CV_8UC3, Scalar(0, 0, 0));
	src1.copyTo(output(Rect(0, 0, src1.cols, src1.rows)));
	src2.copyTo(output(Rect(src1.cols, 0, src2.cols, src2.rows)));

	if (type == 1)
	{
		for (size_t i = 0; i < inlier.size(); i++)
		{
			Point2f left = kpt1[inlier[i].queryIdx].pt;
			Point2f right = (kpt2[inlier[i].trainIdx].pt + Point2f((float)src1.cols, 0.f));
			line(output, left, right, Scalar(0, 255, 255));
		}
	}
	else if (type == 2)
	{
		for (size_t i = 0; i < inlier.size(); i++)
		{
			Point2f left = kpt1[inlier[i].queryIdx].pt;
			Point2f right = (kpt2[inlier[i].trainIdx].pt + Point2f((float)src1.cols, 0.f));
			line(output, left, right, Scalar(255, 0, 0));
		}

		for (size_t i = 0; i < inlier.size(); i++)
		{
			Point2f left = kpt1[inlier[i].queryIdx].pt;
			Point2f right = (kpt2[inlier[i].trainIdx].pt + Point2f((float)src1.cols, 0.f));
			circle(output, left, 1, Scalar(0, 255, 255), 2);
			circle(output, right, 1, Scalar(0, 255, 0), 2);
		}
	}

	return output;
}


void init_match_folder(std::string folder_path, std::string save_path, int large_edge)
{
	vector<string> szFilters;
	szFilters.push_back("jpg");
	szFilters.push_back("bmp");
	szFilters.push_back("png");
	vector<string> filenames;
	LocalCheckDir(folder_path);
	LocalCheckDir(save_path);
	MyFindFile(folder_path, szFilters, filenames);
	for (int i = 0; i < filenames.size(); ++i)
	{
		string image_path = folder_path + filenames[i];
		string feature_path = save_path + absolute_name(filenames[i]) + ".xml";
		init_match(image_path, feature_path, large_edge);
	}
}

void init_match(string image_path, string save_path, int large_edge)
{
	Mat img = imread(image_path);
	imresize(img, large_edge);
	vector<KeyPoint> key_point;
	Ptr<ORB> orb = ORB::create(400);
	Mat des;
	orb->setFastThreshold(0);
	orb->detectAndCompute(img, Mat(), key_point, des);
	FileWrite(img, key_point, des, save_path);
}

int do_match_folder(string image_path, string feature_folder, int large, int threh)
{
	vector<string> szFilters;
	szFilters.push_back("xml");
	vector<string> filenames;
	LocalCheckDir(feature_folder);
	MyFindFile(feature_folder, szFilters, filenames);

	Mat img = imread(image_path);
	imresize(img, large);
	vector<KeyPoint> key_point;
	Ptr<ORB> orb = ORB::create(400);
	Mat des;
	orb->setFastThreshold(0);
	orb->detectAndCompute(img, Mat(), key_point, des);
	
	Mat tar_img, tar_des;
	vector<KeyPoint> tar_key_point;
	BFMatcher matcher(NORM_HAMMING);
	int flag = 0;

	for (int i = 0; i < filenames.size(); ++i)
	{
		string feature_path = feature_folder + filenames[i];
		FileRead(tar_img, tar_key_point, tar_des, feature_path);
		if (tar_key_point.size() == 0 || tar_des.empty())
		{
			continue;
		}
		vector<DMatch> matches_all, matches_gms;
		matcher.match(tar_des, des, matches_all);
		std::vector<bool> vbInliers;
		gms_matcher gms(tar_key_point, tar_img.size(), key_point, img.size(), matches_all);
		int num_inliers = gms.GetInlierMask(vbInliers, false, false);
		if (num_inliers >= threh)
		{
			flag = 1;
			break;
		}
	}
	return flag;
}


int do_match(string image_path, string feature_path, int large_edge, int thresh)
{
	Mat tar_img, tar_des;
	vector<KeyPoint> tar_key_point;
	FileRead(tar_img, tar_key_point, tar_des, feature_path);
	if (tar_key_point.size() == 0 || tar_des.empty())
	{
		cout << "read key point failed" << endl;
		return -1;
	}

	Mat img = imread(image_path);
	imresize(img, large_edge);
	vector<KeyPoint> key_point;
	Ptr<ORB> orb = ORB::create(400);
	Mat des;
	orb->setFastThreshold(0);
	orb->detectAndCompute(img, Mat(), key_point, des);

	BFMatcher matcher(NORM_HAMMING);
	vector<DMatch> matches_all, matches_gms;
	matcher.match(tar_des, des, matches_all);
	std::vector<bool> vbInliers;
	gms_matcher gms(tar_key_point, tar_img.size(), key_point, img.size(), matches_all);
	int num_inliers = gms.GetInlierMask(vbInliers, false, false);
	if (num_inliers >= thresh)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void runImagePair(string first_image, string second_image,int large_size)
{
	Mat img1 = imread(first_image);
	Mat img2 = imread(second_image);

	imresize(img1, large_size);
	imresize(img2, large_size);
	GmsMatch(img1, img2);
}

void GmsMatch(Mat &img1, Mat &img2)
{
	vector<KeyPoint> kp1, kp2;
	Mat d1, d2;
	vector<DMatch> matches_all, matches_gms;

	Ptr<ORB> orb = ORB::create(400);
	orb->setFastThreshold(0);
	orb->detectAndCompute(img1, Mat(), kp1, d1);
	orb->detectAndCompute(img2, Mat(), kp2, d2);

#ifdef USE_GPU
	GpuMat gd1(d1), gd2(d2);
	Ptr<cuda::DescriptorMatcher> matcher = cv::cuda::DescriptorMatcher::createBFMatcher(NORM_HAMMING);
	matcher->match(gd1, gd2, matches_all);
#else
	BFMatcher matcher(NORM_HAMMING);
	matcher.match(d1, d2, matches_all);
#endif

	// GMS filter
	int num_inliers = 0;
	std::vector<bool> vbInliers;
	gms_matcher gms(kp1, img1.size(), kp2, img2.size(), matches_all);
	num_inliers = gms.GetInlierMask(vbInliers, false, false);

	cout << "Get total " << num_inliers << " matches." << endl;

	// draw matches
	for (size_t i = 0; i < vbInliers.size(); ++i)
	{
		if (vbInliers[i] == true)
		{
			matches_gms.push_back(matches_all[i]);
		}
	}

	Mat show = DrawInlier(img1, img2, kp1, kp2, matches_gms, 1);
	imshow("show", show);
	waitKey();
}