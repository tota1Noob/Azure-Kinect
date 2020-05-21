#include <vector>
#include <stdio.h>
#include<string>
#include <stdlib.h>
#include <iostream>
#include <k4a/k4a.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/filters/project_inliers.h>
#include <pcl/visualization/cloud_viewer.h>
#include<pcl/filters/passthrough.h>
#include <pcl/common/common.h>
#include "k4a_grabber.h"
#include<ctime>
#include"Function_SHUJING.h"

cv::Mat depthCameraMatrix, depthDisCoeffs;
Object ObjectRes[10];

//�����ĵ�Ϳ�ڣ������ҵ������ϵģ�0,0����
void DrawCenterPoints(cv::Mat& colorMat);
//����GetXYZAtCameraView����
void TestGetXYZAtCameraView();
k4a::KinectAPI kinect;
int main1()
{
	
	try {
		//pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud < pcl::PointXYZRGB>());
		//opencvͼ��
		int img_cnt = 100;
		int real_cnt = 0;
		for(int i = 0; i < img_cnt; i++)
		{
			printf("��%d��ͼƬ\n", real_cnt);
			cv::Mat depthMat, colorMat, depthcolorMat ,irMat, ircolorMat;
			kinect.GetOpenCVImage(colorMat, depthMat, depthcolorMat, irMat, FALSE);

			//for(int x = 400; x <410; x ++)
			//	for (int y = 600; y <610;y++)
			//		std::cout << (depthMat.at<UINT16>(x, y))<<" ";
			//cout << endl;
			//cout << "finished ori" << endl;

			kinect.ShowOpenCVImage(colorMat, "color");
			kinect.ShowOpenCVImage(depthcolorMat, "depthcolor");
			while (1)
			{
				printf("�Ƿ񱣴�ͼƬ������y����n\n");
				string key;
				getline(cin, key);

				if (key == "y") {}
				else if (key == "n") 
				{ 
					printf("ͼƬδ���棬���������������\n");
					system("pause");
					break; 
				}
				else { continue; }

				std::string name = "imgs/img" + std::to_string(real_cnt);
				cv::imwrite(name + "_depth.png", depthMat);
				cv::imwrite(name + "_depthcolor.png", depthcolorMat);
				cv::imwrite(name + "_color.png", colorMat);
				cv::imwrite(name + "_ir.png", irMat);
				real_cnt++;
				printf("��%d��ͼƬ����ɹ������������������\n", real_cnt);
				system("pause");
				break;
			}

			//cv::Mat img = cv::imread(name+"_depth.png",cv::IMREAD_ANYDEPTH);
			//cout << "after read:" << endl;
			//for(int x = 400; x <410; x ++)
			//	for (int y = 600; y <610;y++)
			//		std::cout << (img.at<UINT16>(x, y)) << " ";
			//cout << endl;
		}

	//	//ת��Ϊ����
	//	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
	//	kinect.GetPointCloud(cloud);
	//	cout << cloud->width << " " << cloud->height << endl;

	//	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud_filter(new pcl::PointCloud<pcl::PointXYZRGBA>());
	//	kinect.GetPointCloud(cloud);

	//	pcl::PointXYZRGBA minpt, maxpt;
	//	pcl::getMinMax3D(*cloud, minpt, maxpt);
	//	cout << minpt.x << " " << minpt.y << " " << minpt.z << endl;
	//	cout << maxpt.x << " " << maxpt.y << " " << maxpt.z << endl;

	//	pcl::PassThrough<pcl::PointXYZRGBA> pass;
	//	pass.setInputCloud(cloud);
	//	pass.setFilterFieldName("z");          //���ù���ʱ����Ҫ�������͵�Z�ֶ�
	//	pass.setFilterLimits(0.2, 10);         //�����ڹ����ֶεķ�Χ
	//	pass.setFilterLimitsNegative(false);   //�������ǹ��˵���Χ�ڵĵ�
	//	pass.filter(*cloud_filter);


	//	pcl::visualization::CloudViewer view("PCL");
	//	view.showCloud(cloud_filter);
	//	cout << cloud_filter->width << " " << cloud_filter->height << endl;
	//	std::system("pause");
		kinect.ReleaseDevice();
	}
	catch(const char * msg)
	{
		printf("%s\n", msg);
		return 1;
	}
    return 0;
}
int main()
{
	for (int i = 0; i < 50; i++)
	{
		cv::Mat depthMat, colorMat, depthcolorMat ,irMat, ircolorMat;
		kinect.GetOpenCVImage(colorMat, depthMat, depthcolorMat, irMat, FALSE);
		DrawCenterPoints(colorMat);

		//�ѻ�е�۲��������Ϊ0�������Ͳ����⵽��е����
		for (int h = 0; h < 0; h++)
			for (int w = 0; w < 0; w++)
			{
				depthMat.at<UINT16>(h, w) = 0;
				depthcolorMat.at<cv::Vec4b>(h, w)[0] = 0;
				depthcolorMat.at<cv::Vec4b>(h, w)[1] = 0;
				depthcolorMat.at<cv::Vec4b>(h, w)[2] = 0;
			}
		
		kinect.ShowOpenCVImage(colorMat, "img_color");
		//
		float robot_x = 336.931, robot_y = 394.312, robot_z = 114.206, robot_len = 215;
		kinect.GetIntrinsicParam(depthCameraMatrix, depthDisCoeffs, "depth");
		int iDistance = 1300;
		int iObj_num = ObjectLocation(depthCameraMatrix, (UINT16*)depthMat.data, iDistance, depthMat.cols, depthMat.rows,10, 710, ObjectRes);
		cout << "iObj_num:"<<iObj_num << endl;
		for (int i = 0; i < iObj_num; i++)
		{
			Draw_Convex(depthcolorMat, depthcolorMat.cols, depthcolorMat.rows, ObjectRes[i].R);
			//float x=0, y=0;
			//for (int j = 0; j < 4; j++)
			//{
			//	x += ObjectRes[i].R[j].x;
			//	y += ObjectRes[i].R[j].y;
			//}
			//x /= 4, y /= 4;
			//cv::Point2i point2D((int)x, (int)y);
			//cv::Point3f point3D;
			////kinect.GetXYZAtCameraView(point2D, img_depth.at<UINT16>(point2D.x, point2D.y), point3D);
			//kinect.GetXYZAtCameraView(point2D, 
			//	(img_depth.at<UINT16>(point2D.y, point2D.x) +img_depth.at<UINT16>(point2D.y+5, point2D.x+5)
			//	+img_depth.at<UINT16>(point2D.y-5, point2D.x-5)
			//	+img_depth.at<UINT16>(point2D.y+5, point2D.x-5)
			//	+img_depth.at<UINT16>(point2D.y-5, point2D.x+5))/5
			//	, point3D);
			////��е�ۻ�������ϵ���������ϵxy���ǶԵ��ġ�
			//swap(point3D.x, point3D.y);
			//point3D.z = 1310 - point3D.z;

			//cout << "pixel:"<<x << " " << y << endl;
			//point3D.x += robot_x;
			//point3D.y += robot_y;
			//point3D.z += -robot_z + robot_len;
			//cout <<"real_robot:"<< "x:" << point3D.x << " " << "y:" << point3D.y << " " << "z:" << point3D.z << endl;

		}
		kinect.ShowOpenCVImage(depthcolorMat, "depthcolor");
	}
	
return 0;
}

void TestGetXYZAtCameraView()
{	
	for (int i = 1; i <= 6; i++)
	{
		std::string name = "imgs/img" + to_string(i);
		cout << name << endl;
		cv::Mat img_depth = cv::imread(name+"_depth.png",cv::IMREAD_ANYDEPTH);
		cv::Mat img_color = cv::imread(name+"_color.png");
		cv::Mat img_depthcolor = cv::imread(name+"_depthcolor.png");
		int iDistance = 1380;

		//����GetXYZAtCameraView����
		//cv::Point2i point2D(360, 640);
		for(int x = 0; x<720;x++)
			for (int y = 0; y < 1280; y++)
			{
				cv::Point2i point2D(x, y);
				cv::Point3f point3D;
				//kinect.GetXYZAtCameraView(point2D, img_depth.at<UINT16>(point2D.x, point2D.y), point3D);
				kinect.GetXYZAtCameraView(point2D, 1000, point3D);
				if (abs(point3D.x) < 5 && abs(point3D.y) < 5)
				{
					cout << x << " " << y << endl;
					cout << "x:" << point3D.x << " " << "y:" << point3D.y << " " << "z:" << point3D.z << endl;
				}
			}
		
		int iObj_num = ObjectLocation(depthCameraMatrix, (UINT16*)img_depth.data, iDistance, img_depth.cols, img_depth.rows,10, 710, ObjectRes);
		cout << "iObj_num:"<<iObj_num << endl;
		for (int i = 0; i < iObj_num; i++)
		{
			Draw_Convex(img_depthcolor, img_depthcolor.cols, img_depthcolor.rows, ObjectRes[i].R);
		}
		kinect.ShowOpenCVImage(img_depthcolor, "depthcolor");
	}
}
void DrawCenterPoints(cv::Mat& colorMat)
{
	colorMat.at<cv::Vec4b>(360, 640)[0] = 0;
	colorMat.at<cv::Vec4b>(360, 640)[1] = 0;
	colorMat.at<cv::Vec4b>(360, 640)[2] = 0;
	colorMat.at<cv::Vec4b>(360, 641)[0] = 0;
	colorMat.at<cv::Vec4b>(360, 641)[1] = 0;
	colorMat.at<cv::Vec4b>(360, 641)[2] = 0;
	colorMat.at<cv::Vec4b>(360, 639)[0] = 0;
	colorMat.at<cv::Vec4b>(360, 639)[1] = 0;
	colorMat.at<cv::Vec4b>(360, 639)[2] = 0;
	colorMat.at<cv::Vec4b>(361, 640)[3]= 0;
	colorMat.at<cv::Vec4b>(359, 640)[0] = 0;
	colorMat.at<cv::Vec4b>(359, 640)[1] = 0;
	colorMat.at<cv::Vec4b>(359, 640)[2] = 0;
}

