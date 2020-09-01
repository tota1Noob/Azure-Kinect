#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include"k4a_grabber.h"
class Robot
{
public:
	k4a::KinectAPI *kinect;
	cv::Mat color_R_base2cam, color_t_base2cam;
	cv::Mat depth_R_base2cam, depth_t_base2cam;
	cv::Mat color_R_cam2base, color_t_cam2base;
	cv::Mat depth_R_cam2base, depth_t_cam2base;
	cv::Mat color_Homo_cam2base, depth_Homo_cam2base;
	Robot(const std::string& Homo_cam2base_file, k4a::KinectAPI *kinect, bool verbose);
	const double PI = 3.1415926;
	// Checks if a matrix is a valid rotation matrix.
	bool isRotationMatrix(cv::Mat& R);

	// Calculates rotation matrix to euler angles
	// The result is the same as MATLAB except the order
	// of the euler angles ( x and z are swapped ).
	cv::Vec3f rotationMatrixToEulerAngles(cv::Mat& R);
	//�����(row,col������ȣ�Ϊ�˽�������Ӱ�죬����Χ2*size��С������������Ϊ�����㡣

	double getDepthValue(cv::Mat depthMat, int row, int col, int size);
	//�����point2D����ȣ�Ϊ�˽�������Ӱ�죬����Χ2*size��С������������Ϊ�����㡣
	cv::uint16_t getDepth(const cv::Mat& depthMat, cv::Mat& point2D);
	double getLength(cv::Mat a);
	double getDistance(cv::Point3f a, cv::Point3f b);
	//��Ӧ����ת��Ϊ��ת�����ƽ������
	void HomogeneousMtr2RT(cv::Mat& HomoMtr, cv::Mat& R, cv::Mat& T);
	
	//��ת�����ƽ������ת��Ϊ��Ӧ����
	cv::Mat RT2HomogeneousMatrix(const cv::Mat& R, const cv::Mat& T);
	//������������ϵ�ĵ㣬���ػ�е������ϵ�µ�����
	void calPoint3D(cv::Mat point2D, cv::Point3f& real, cv::uint16_t Zc);
	cv::Mat calPoint3D(cv::Mat& depthMat, cv::Mat point2D);
	//������Ӿ��μ����ݵ���ת����
	cv::Mat calRotationMatrix(cv::Mat& depthMat, cv::Point2f* R, double scale);
	//����
	/*������z��*/
	double calAngle(cv::Point2f* R, int h, int w, cv::uint16_t depth);
};
