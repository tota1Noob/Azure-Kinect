#include <winsock2.h>
#include <vector>
#include <stdio.h>
#include<string>
#include <stdlib.h>
#include <iostream>
#include <k4a/k4a.h>
#include "k4a_grabber.h"
#include "plane_detection.h"
#include<ctime>
#include <fstream>
#include"SocketRobot.h"
#include "flask.h"
#include "Robot.h"


string caliberation_camera_file = "caliberation_camera.xml";
string Homo_cam2base_file = "Homo_cam2base.xml";
k4a::KinectAPI kinect(caliberation_camera_file, true);
Robot rob(Homo_cam2base_file, &kinect, true);
//ԭʼ�����ͼ�����ͼ��α��ɫͼ��������ͼ��������α��ɫͼ
cv::Mat depthMat, colorMat, depthcolorMat, irMat, ircolorMat;
cv::Mat depthMatOld, colorMatOld, depthcolorMatOld, irMatOld, ircolorMatOld;
//���ֵ�������У��������ͼ��
cv::Mat depthMatRevise;
int main()
{
	bool useRobot = false;

	SocketRobot* sr = NULL;
	if (useRobot)
		sr = new SocketRobot();

	cv::Mat point2D(3, 1, CV_64F, cv::Scalar(0));
	point2D.at<double>(2, 0) = 1;
	cv::Mat point3D;

	int cnt = 0;
	while (1)
	{

		kinect.GetOpenCVImage(colorMatOld, depthMatOld, depthcolorMatOld, irMat, FALSE);
		///*�����ڲκͻ���ϵ��У��ͼ��*/
		kinect.undistort(depthMatOld, depthMat, "depth");
		kinect.undistort(depthcolorMatOld, depthcolorMat, "depth");
		kinect.undistort(colorMatOld, colorMat, "color");

		//��colorͼת�������ͼ�ӽ�
		cv::Mat colorMatRevise(depthMat.rows, depthMat.cols, CV_8UC4, cv::Scalar(0));
		kinect.ConvertColor2Depth(colorMat, depthMat, colorMatRevise);

		double* center;
		double* normal;
		double angle;

		vector<VertexType> highestPlanePoints_3D;
		cv::Point2f vertices[4];
		vector<cv::Mat> masks;
		//��rgbͼ���������������ؽ��
		getMasks(colorMatRevise, masks);

		cout << "��" << masks.size() << "�����" << endl;
		//string name = "imgs/img" + std::to_string(cnt);
		double* x_axis, * y_axis, * z_axis;
		colorMatRevise = processImg(colorMatRevise, depthMat, masks, center, x_axis, y_axis, z_axis, highestPlanePoints_3D, vertices);
		kinect.ShowOpenCVImage(colorMatRevise, "depthcolor", useRobot);
		//cv::imwrite(name + "_depth.png", depthMat);
		//cv::imwrite(name + "_depthcolor.png", colorMatRevise);
		//���û��ݾ�����1��
		if (highestPlanePoints_3D.size() == 0)
		{
			Sleep(1000);
			continue;
		}

		double x = 0, y = 0;
		for (int j = 0; j < 4; j++)
		{
			x += vertices[j].x;
			y += vertices[j].y;
		}

		x /= 4, y /= 4;
		/*�������*/
		point2D.at<double>(0, 0) = x;
		point2D.at<double>(1, 0) = y;
		point2D.at<double>(2, 0) = 1;
		/*ƽ����Χ��ȣ��������*/

		double Zc = rob.getDepthValue(depthMat, y, x, 6);
		angle = rob.calAngle(vertices, depthMat.rows, depthMat.cols, Zc);
		//ƽ�淨����
		cv::Mat rotationMatrix = rob.calRotationMatrix(depthMat, vertices, 0.6);
		cv::Vec3f eulerAngles = rob.rotationMatrixToEulerAngles(rotationMatrix);
		cout << "���Ϸ��������ת����:" << endl;
		cout << rotationMatrix << endl;

		cv::Mat point3D = rob.depth_R_base2cam.t() * (kinect.depthCameraMatrix.inv() * Zc * point2D - rob.depth_t_base2cam);

		cout << "����:" << endl;
		cout << point3D << endl;
		cout << "ŷ����:" << endl;
		cout << eulerAngles << endl;

		float coords[12];
		coords[0] = point3D.at<double>(0, 0);
		coords[1] = point3D.at<double>(1, 0);
		coords[2] = point3D.at<double>(2, 0) - 4;
		coords[3] = eulerAngles[0];
		coords[4] = eulerAngles[1];
		coords[5] = eulerAngles[2];
		coords[6] = 698 + 50;
		//�ų�һ��
		//coords[7] = 342 - 750 + cnt * 220;
		//�ŵ��̶�λ��
		coords[7] = 0;
		//��ݼ���
		cnt++;
		//���õĸ߶�
		if (coords[2] > 160)
			coords[8] = 160 - 50;
		else
			coords[8] = coords[2] - 50;

		//ƽ�ŷ�
		coords[9] = 0;
		coords[10] = 0;
		coords[11] = 0;

		if (useRobot)
			sr->moveRobot(coords);
	}
	return 0;
}
