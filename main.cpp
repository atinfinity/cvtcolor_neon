#include "convert.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

bool verify(const cv::Mat& img1, const cv::Mat& img2)
{
	cv::Mat diff;
	cv::absdiff(img1, img2, diff);
	return (cv::countNonZero(diff) == 0);
}

double launch_convert_reference(const cv::Mat& src, cv::Mat& dst, const int pixel_num, const int loop_num)
{
	cv::TickMeter tm;
	double time = 0.0;
	for(int i = 0; i <= loop_num; i++)
	{
		tm.reset();
		tm.start();
		convert_reference((uint8_t *)src.datastart, (uint8_t*)dst.datastart, pixel_num);
		tm.stop();
		time += ((i>0) ? 0.0 : tm.getTimeMilli());
	}
	return time;
}

double launch_convert_neon(const cv::Mat& src, cv::Mat& dst, const int pixel_num, const int loop_num)
{
	cv::TickMeter tm;
	double time = 0.0;
	for(int i = 0; i < loop_num; i++)
	{
		tm.reset();
		tm.start();
		convert_neon((uint8_t *)src.datastart, (uint8_t*)dst.datastart, pixel_num);
		tm.stop();
		time += ((i>0) ? 0.0 : tm.getTimeMilli());
	}
	return time;
}

int main(int argc, char *argv[])
{
	cv::Mat src = cv::imread("lena.jpg", cv::IMREAD_COLOR);
	if (src.empty())
	{
		std::cout << "could not load image." << std::endl;
		return -1;
	}

	cv::resize(src, src, cv::Size(4096, 4096));
	cv::cvtColor(src, src, cv::COLOR_BGR2RGB);
	const int pixel_num = src.cols * src.rows;

	cv::Mat dst(src.size(), CV_8UC1, cv::Scalar(0));
	cv::Mat dst_neon(src.size(), CV_8UC1, cv::Scalar(0));

	const int loop_num = 10;
	double time_ref = launch_convert_reference(src, dst, pixel_num, loop_num);
	double time_neon = launch_convert_neon(src, dst_neon, pixel_num, loop_num);

	std::cout << "time(reference) = " << time_ref << std::endl;
	std::cout << "time(NEON) = " << time_neon << std::endl;
	std::cout << "verify: " << (verify(dst, dst_neon) ? "Passed" : "Failed") << std::endl;

	return 0;
}

