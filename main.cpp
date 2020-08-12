#include <winsock2.h>
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
#include "plane_detection.h"
#include<ctime>
#include <fstream>
#include"Function_SHUJING.h"
#include"SocketRobot.h"

const double PI = 3.1415926;
cv::Mat depthMat,colorMat, depthcolorMat, irMat, ircolorMat;
cv::Mat depthMatOld,colorMatOld, depthcolorMatOld, irMatOld, ircolorMatOld;
cv::Mat Depth2ColorRotation, Depth2ColorTranslation;
//���ֵ�������У��������ͼ��
cv::Mat depthMatRevise;
Object ObjectRes[10];
cv::Mat depthCameraMatrix, depthDistCoeffs;
cv::Mat colorCameraMatrix, colorDistCoeffs;
cv::Mat color_R_base2cam, color_t_base2cam;
cv::Mat depth_R_base2cam, depth_t_base2cam;
cv::Mat color_R_cam2base, color_t_cam2base;
cv::Mat depth_R_cam2base, depth_t_cam2base;
bool shang = TRUE;
// Checks if a matrix is a valid rotation matrix.
bool isRotationMatrix(cv::Mat &R)
{
    cv::Mat Rt;
    transpose(R, Rt);
    cv::Mat shouldBeIdentity = Rt * R;
    cv::Mat I = cv::Mat::eye(3,3, shouldBeIdentity.type());
    return  norm(I, shouldBeIdentity) < 1e-6;
     
}
 
// Calculates rotation matrix to euler angles
// The result is the same as MATLAB except the order
// of the euler angles ( x and z are swapped ).
cv::Vec3f rotationMatrixToEulerAngles(cv::Mat &R)
{
    //assert(isRotationMatrix(R));

    float sy = sqrt(R.at<float>(0,0) * R.at<float>(0,0) +  R.at<float>(1,0) * R.at<float>(1,0) );

    bool singular = sy < 1e-6; // If

    float x, y, z;
    if (!singular) {
        x = atan2(R.at<float>(2,1) , R.at<float>(2,2));
        y = atan2(-R.at<float>(2,0), sy);
        z = atan2(R.at<float>(1,0), R.at<float>(0,0));
    } else {
        x = atan2(-R.at<float>(1,2), R.at<float>(1,1));
        y = atan2(-R.at<float>(2,0), sy);
        z = 0;
    }
    return cv::Vec3f(z, y, x) / CV_PI * 180;   
}
double getDepthValue(cv::Mat depthMat,int row, int col, int size)
{
	int rows = depthMat.rows, cols = depthMat.cols;
	double sum = 0;
	int cnt = 0;
	for (int r = max(0, row - size); r <= min(rows - 1, row + size); r++)
	{
		for (int c = max(0, col - size); c <= min(cols - 1, col + size); c++)
		if(depthMat.at<UINT16>(r, c) != 0)
		{
			cnt++;
			sum += depthMat.at<UINT16>(r, c);
		}
	}
	assert(cnt);
	return sum / cnt;
}
UINT16 getDepth(cv::Mat point2D)
{
	int x = (int)point2D.at<float>(0, 0);
	int y = (int)point2D.at<float>(1, 0);
	UINT16 Zc = getDepthValue(depthMat, y, x, 6); 
	//TODO:����߽��ж�
	return Zc;
}
float getLength(cv::Mat a)
{
	float dx = a.at<float>(0, 0);
	float dy = a.at<float>(1, 0);
	float d = sqrt(dx * dx + dy * dy);
	return d;
}
float getDistance(cv::Point3f a, cv::Point3f b)
{
	float  d = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)); 
	return d;
}
bool compare(Object a, Object b)
{
	float ay = (a.R[0].y + a.R[1].y + a.R[2].y + a.R[3].y)/4;
	float by = (b.R[0].y + b.R[1].y + b.R[2].y + b.R[3].y)/4;
	if (shang == TRUE)
	{
		//float ay = -1000000, by = -100000;
		//ay = min(a.R[0].y, min(a.R[1].y, min(a.R[2].y, a.R[3].y)));
		//by = min(b.R[0].y, min(b.R[1].y, min(b.R[2].y, b.R[3].y)));

		if (ay > 400)
			return FALSE;
		if (by > 400)
			return TRUE;
		return ay < by;
	}
	else
	{
		float ax = -1000000, bx = -100000;
		ax = max(a.R[0].x, max(a.R[1].x, max(a.R[2].x, a.R[3].x)));
		bx = max(b.R[0].x, max(b.R[1].x, max(b.R[2].x, b.R[3].x)));

		if (ay < 400)
			return FALSE;
		if (by < 400)
			return TRUE;
		return ax < bx;
	}
}
//TODO:û���Թ�
//�Ը߶ȴ��ϵ�������
bool compareByHeight(Object a, Object b)
{
	float ay = (a.R[0].y + a.R[1].y + a.R[2].y + a.R[3].y)/4;
	float ax = (a.R[0].x + a.R[1].x + a.R[2].x + a.R[3].x)/4;
	float by = (b.R[0].y + b.R[1].y + b.R[2].y + b.R[3].y)/4;
	float bx = (b.R[0].x + b.R[1].x + b.R[2].x + b.R[3].x)/4;
	cv::Mat aM =(cv::Mat_<float>(3, 1)<<ax, ay, 1);
	cv::Mat bM =(cv::Mat_<float>(3, 1)<<bx, by, 1);

	UINT16 aDepth = getDepth(aM);
	UINT16 bDepth = getDepth(bM);
	//������μ��㵽��е������ϵ
	cv::Mat a3D = depth_R_base2cam.t() * (depthCameraMatrix.inv() * aDepth * aM - depth_t_base2cam);
	cv::Mat b3D = depth_R_base2cam.t() * (depthCameraMatrix.inv() * bDepth * bM - depth_t_base2cam);
	return a3D.at<float>(2, 0) > b3D.at<float>(2, 0);
}
bool compareByArea(Object a, Object b)
{
	float aArea = abs((a.R[0] - a.R[1]) ^ (a.R[1] - a.R[2]));
	float bArea = abs((b.R[0] - b.R[1]) ^ (b.R[1] - b.R[2]));
	return aArea > bArea;
}
void HomogeneousMtr2RT(cv::Mat& HomoMtr, cv::Mat& R, cv::Mat& T)
{
	//Mat R_HomoMtr = HomoMtr(Rect(0, 0, 3, 3)); //ע��Rectȡֵ
	//Mat T_HomoMtr = HomoMtr(Rect(3, 0, 1, 3));
	//R_HomoMtr.copyTo(R);
	//T_HomoMtr.copyTo(T);
	/*HomoMtr(Rect(0, 0, 3, 3)).copyTo(R);
	HomoMtr(Rect(3, 0, 1, 3)).copyTo(T);*/
	cv::Rect R_rect(0, 0, 3, 3);
	cv::Rect T_rect(3, 0, 1, 3);
	R = HomoMtr(R_rect);
	T = HomoMtr(T_rect);

}

cv::Mat RT2HomogeneousMatrix(const cv::Mat& R,const cv::Mat& T)
{
	cv::Mat HomoMtr;
	cv::Mat_<float> R1 = (cv::Mat_<float>(4, 3) << 
										R.at<float>(0, 0), R.at<float>(0, 1), R.at<float>(0, 2),
										R.at<float>(1, 0), R.at<float>(1, 1), R.at<float>(1, 2),
										R.at<float>(2, 0), R.at<float>(2, 1), R.at<float>(2, 2),
										0, 0, 0);
	cv::Mat_<float> T1 = (cv::Mat_<float>(4, 1) <<
										T.at<float>(0,0),
										T.at<float>(1,0),
										T.at<float>(2,0),
										1);
	cv::hconcat(R1, T1, HomoMtr);		//����ƴ��
	return HomoMtr;
}
//������Χ5�����ƽ��ֵ
void calPoint3D(cv::Mat point2D, cv::Point3f & real, UINT16 Zc)
{
	assert(Zc != 0);
	cv::Mat point3D = depth_R_base2cam.t() * (depthCameraMatrix.inv() * Zc * point2D - depth_t_base2cam);
	real.x = point3D.at<float>(0, 0);
	real.y = point3D.at<float>(1, 0);
	real.z = point3D.at<float>(2, 0);
}

cv::Mat calRotationMatrix(const vector<Point> &R, float scale)
{
	cv::Mat a = (cv::Mat_<float>(3, 1) << R[0].x, R[0].y, 1);
	cv::Mat b = (cv::Mat_<float>(3, 1) << R[1].x, R[1].y, 1);
	cv::Mat c = (cv::Mat_<float>(3, 1) << R[2].x, R[2].y, 1);
	cv::Mat d = (cv::Mat_<float>(3, 1) << R[3].x, R[3].y, 1);
	cout << "abcd" << a << b << c << d << endl;
	cv::Mat center = (a + b + c + d) / 4;
	cout << "center" << center << endl;

	cv::Mat aa = center + (a - center)  * scale;
	cv::Mat bb = center + (b - center)  * scale;
	cv::Mat cc = center + (c - center)  * scale;
	cv::Mat dd = center + (d - center)  * scale;
	cout << aa << bb << cc << dd << endl;
	
	cv::Point3f raa, rbb, rcc, rdd, rcenter;
	calPoint3D(center, rcenter, getDepth(center));
	calPoint3D(aa, raa, getDepth(aa));
	calPoint3D(bb, rbb, getDepth(bb));
	calPoint3D(cc, rcc, getDepth(cc));
	calPoint3D(dd, rdd, getDepth(dd));
	//����4���ߵ�����
	cv::Point3f ab = raa - rbb;
	cv::Point3f dc = rdd - rcc;
	cv::Point3f ad = raa - rdd;
	cv::Point3f bc = rbb - rcc;
	//���ߵķ��򵱳�x���y��ƽ��һ��
	cv::Point3f v1 = (ab + dc) / 2;
	cv::Point3f v2 = (ad + bc) / 2;
	//�ѽϳ��ıߵ���x�ᣬ��Ϊ2��������x�᷽���
	cv::Point3f X, Y;
	if (v1.dot(v1) > v2.dot(v2))
	{
		X = v1 / sqrt(v1.dot(v1));//��λ��
		Y = v2 / sqrt(v2.dot(v2));
	}
	else 
	{
		X = v2 / sqrt(v2.dot(v2));//��λ��
		Y = v1 / sqrt(v1.dot(v1));
	}

	if (X.x < 0)
		X *= -1;

	//��������������Ȼ��ƽ��һ�£�����
	cv::Point3f normal1 = (raa - rbb).cross(raa - rdd);
	cv::Point3f normal2 = (rcc - rbb).cross(rcc - rdd);
	cv::Point3f normal3 = (rbb - raa).cross(rbb - rcc);
	cv::Point3f normal4 = (rdd - raa).cross(rdd - rcc);
	normal1 = normal1 / sqrt(normal1.dot(normal1));
	normal2 = normal2 / sqrt(normal2.dot(normal2));
	normal3 = normal3 / sqrt(normal3.dot(normal3));
	normal4 = normal4 / sqrt(normal4.dot(normal4));

	if (normal1.z < 0)
		normal1 *= -1;
	if (normal2.z < 0)
		normal2 *= -1;
	if (normal3.z < 0)
		normal3 *= -1;
	if (normal4.z < 0)
		normal4 *= -1;

	cv::Point3f normal = (normal1 + normal2 + normal3 + normal4) / 4;
	normal = normal / sqrt(normal.dot(normal));

	//��֪x���z�ᣬ���ݲ�����y��,��֮ǰ�õ�y��ͳһ����
	if (normal.cross(X).x * Y.x < 0)
		Y *= -1;
	cout << "����" << endl;
	cout << X.dot(X) << " " << Y.dot(Y) << " " << normal.dot(normal) << endl;;

	cv::Mat rotation = cv::Mat_<float>(3, 3);
	rotation.at<float>(0, 0) = X.x;
	rotation.at<float>(1, 0) = X.y;
	rotation.at<float>(2, 0) = X.z;
	rotation.at<float>(0, 1) = Y.x;
	rotation.at<float>(1, 1) = Y.y;
	rotation.at<float>(2, 1) = Y.z;
	rotation.at<float>(0, 2) = normal.x;
	rotation.at<float>(1, 2) = normal.y;
	rotation.at<float>(2, 2) = normal.z;
	
	//����֮ǰ�����x,y,z��3���᲻һ��������������������������ֵ�ֽ�ķ������һ�����������ľ���
	cv::Mat w, u, vt;
	cv::SVDecomp(rotation, w, u, vt);
    cv::Mat I = cv::Mat::eye(3,3, rotation.type());
	cv::Mat new_rotation = u * I * vt;

	return new_rotation;
}
/*������z��*/
float calAngle(const vector<Point>& R, int h, int w, UINT16 depth)
{
	cv::Mat point2D(3, 1, CV_32F, cv::Scalar(0));
	point2D.at<float>(2, 0) = 1;
	cv::Mat point3D(3, 1, CV_32F, cv::Scalar(0));
	cv::Point3f a, b, c;//�����������������ʵ����
	/*���ĳһ���㳬���߽磬��˳�Ƶ㡣ë����Ӧ�����ֻ��һ���㳬���߽�*/
	int k;
	for (int i = 0; i < R.size(); i++)
	{
		k = i;
		if (R[k].x < 0 || R[k].x >= w || R[k].y < 0 || R[k].y >= h)
			continue;
		point2D.at<float>(0, 0) = R[k].x;
		point2D.at<float>(1, 0) = R[k].y;
		calPoint3D(point2D, a, depth);

		k = (i + 1) % R.size();
		if (R[k].x < 0 || R[k].x >= w || R[k].y < 0 || R[k].y >= h)
			continue;
		point2D.at<float>(0, 0) = R[k].x;
		point2D.at<float>(1, 0) = R[k].y;
		calPoint3D(point2D, b, depth);

		k = (i + 2) % R.size();
		if (R[k].x < 0 || R[k].x >= w || R[k].y < 0 || R[k].y >= h)
			continue;
		point2D.at<float>(0, 0) = R[k].x;
		point2D.at<float>(1, 0) = R[k].y;
		calPoint3D(point2D, c, depth);
		break;
	}
	float angle;
	cout << "�߳�1" << getDistance(a, b) << ":�߳�2" << getDistance(b, c) << endl;
	cout << endl;
	if(getDistance(a, b) > getDistance(b, c))
	{
		if (abs(a.x - b.x) < 30)
		{
			angle = 90;
		}
		else 
		{
			float k = (a.y - b.y) / (a.x - b.x);
			angle = (tanh(k) / PI * 180);
		}
	}
	else
	{
		if (abs(b.x - c.x) < 30)
		{
			angle = 90;
		}
		else 
		{
			float k = (b.y - c.y) / (b.x - c.x);
			angle = (tanh(k) / PI * 180);
		}
	}
	cout << "oldangle" << endl;
	cout << angle << endl;
	if (angle > 90)
		angle -= 180;
	if (angle < -90)
		angle += 180;
	return angle;
}

//TODO:ɾ����Щ�غϵ�����
void DeleteBadObejct(Object *ObjectRes, int &iObj_num) 
{
	//������Ӵ�С����
	sort(ObjectRes, ObjectRes + iObj_num, compareByArea);
	for (int i = 0; i < iObj_num; i++)
	{
		for (int j = i + 1; j < iObj_num; j++)
		{
			//�ҵ�j�����ĵ�
			float y = (ObjectRes[j].R[0].y + ObjectRes[j].R[1].y + ObjectRes[j].R[2].y + ObjectRes[j].R[3].y)/4;
			float x = (ObjectRes[j].R[0].x + ObjectRes[j].R[1].x + ObjectRes[j].R[2].x + ObjectRes[j].R[3].x)/4;

			vector<cv::Point> contours(4);
			contours[0].x = ObjectRes[i].R[0].x;
			contours[0].y = ObjectRes[i].R[0].y;
			contours[1].x = ObjectRes[i].R[1].x;
			contours[1].y = ObjectRes[i].R[1].y;
			contours[2].x = ObjectRes[i].R[2].x;
			contours[2].y = ObjectRes[i].R[2].y;
			contours[3].x = ObjectRes[i].R[3].x;
			contours[3].y = ObjectRes[i].R[3].y;

			float isIn = cv::pointPolygonTest(contours, cv::Point2f(x, y), true);

			//
			if (isIn > 0)
			{
				for (int k = j; k < iObj_num - 1; k++)
					swap(ObjectRes[k], ObjectRes[k+1]);
				iObj_num--;
				cout << "ɾ��һ�����壬xyΪ��" << x << " " << y << endl;
			}
		}
	}
}
//�����ĵ�Ϳ�ڣ������ҵ������ϵģ�0,0����
void DrawCenterPoints(cv::Mat& colorMat);
//����GetXYZAtCameraView����
void TestGetXYZAtCameraView();
k4a::KinectAPI kinect;
int main()
{
	bool useRobot = false;
	
	string caliberation_camera_file = "caliberation_camera.xml";
	string Homo_cam2base_file = "Homo_cam2base.xml";
	cv::FileStorage fs(caliberation_camera_file, cv::FileStorage::READ); //��ȡ�궨XML�ļ�  
	//��ȡ���ͼ���ڲξ���
	fs["depth_cameraMatrix"] >> depthCameraMatrix;
	depthCameraMatrix.convertTo(depthCameraMatrix, CV_32F);
	cout << depthCameraMatrix.type() << endl;
	fs["depth_distCoeffs"] >> depthDistCoeffs;
	cout << "depthCameraMatrix" << depthCameraMatrix << endl;
	cout << "depthdisCoeffs" << depthDistCoeffs << endl;
	//��ȡcolorͼ���ڲξ���
	fs["color_cameraMatrix"] >> colorCameraMatrix;
	colorCameraMatrix.convertTo(colorCameraMatrix, CV_32F);
	cout << colorCameraMatrix.type() << endl;
	fs["color_distCoeffs"] >> colorDistCoeffs;
	cout << "colorCameraMatrix" << colorCameraMatrix << endl;
	cout << "colordisCoeffs" << colorDistCoeffs << endl;
	fs.release();

	cv::FileStorage fs2(Homo_cam2base_file, cv::FileStorage::READ); //��ȡ����������ת����ϵXML�ļ�  
	cv::Mat color_Homo_cam2base, depth_Homo_cam2base;
	fs2["color_Homo_cam2base"] >> color_Homo_cam2base;
	cout << "color_Homo_cam2base" << color_Homo_cam2base << endl;
	fs2["depth_Homo_cam2base"] >> depth_Homo_cam2base;
	cout << "depth_Homo_cam2base" << depth_Homo_cam2base << endl;
	/*ΪʲôҪ���棿*/
	//color_Homo_cam2base = color_Homo_cam2base.inv();
	cv::Mat depth_Homo_base2cam = depth_Homo_cam2base.inv();
	fs2.release();
	/*����Ӧ����ת��Ϊ��ת�����ƽ�������������������*/
	HomogeneousMtr2RT(depth_Homo_base2cam, depth_R_base2cam, depth_t_base2cam);

	SocketRobot* sr = NULL;
	if (useRobot)
		sr = new SocketRobot();
	
	kinect.GetRotationAndTranslationFromDepth2Color(Depth2ColorRotation, Depth2ColorTranslation);

	cv::Mat Homo_depth2color = RT2HomogeneousMatrix(Depth2ColorRotation, Depth2ColorTranslation);


	cv::Mat point2D(3, 1, CV_32F, cv::Scalar(0)); 
	point2D.at<float>(2, 0) = 1;
	cv::Mat point3D;
	//cv::Mat point3D = color_R_cam2base.t() * (depthCameraMatrix.inv() * Zc * point2D - t_cam2base);

	int cnt = 0;
	while(1)
	{ 

			kinect.GetOpenCVImage(colorMatOld, depthMatOld, depthcolorMatOld, irMat, FALSE);
			///*�����ڲκͻ���ϵ��У��ͼ��*/
			undistort(depthMatOld,depthMat,depthCameraMatrix,depthDistCoeffs);  
			undistort(depthcolorMatOld,depthcolorMat,depthCameraMatrix,depthDistCoeffs);  
			undistort(colorMatOld,colorMat,colorCameraMatrix,colorDistCoeffs);  
			//kinect.ShowOpenCVImage(colorMat, "color");
			//kinect.ShowOpenCVImage(colorMatOld, "color");
			//kinect.ShowOpenCVImage(depthcolorMat, "depthcolor");
			//kinect.ShowOpenCVImage(depthcolorMatOld, "depthcolor");
			//��depthת��Ϊcolor�ӽ�
			cv::Mat colorMatRevise(depthMat.rows, depthMat.cols, CV_8UC4, cv::Scalar(0));

			for(int i = 0; i < depthMat.rows;i++)
				for (int j = 0; j < depthMat.cols; j++)
				if(depthMat.at<UINT16>(i,j)!=0)
				{
					point2D.at<float>(0, 0) = j;
					point2D.at<float>(1, 0) = i;
					//��depth��ͼ������ϵת��Ϊdepth�������ϵ
					point3D = depthCameraMatrix.inv() * point2D * depthMat.at<UINT16>(i, j);
					point3D = Depth2ColorRotation * point3D + Depth2ColorTranslation;
					//cout << j << " " << i << " " << depthMat.at<UINT16>(i, j) << endl;
					point2D = colorCameraMatrix * point3D / point3D.at<float>(2, 0);
					int new_i = point2D.at<float>(0, 1), new_j = point2D.at<float>(0, 0);
					if (new_i >= 0 && new_j >= 0 && new_i < colorMat.rows && new_j < colorMat.cols)
					{
						colorMatRevise.at<UINT32>(i, j) = colorMat.at<UINT32>(new_i, new_j);
						//cout << depthcolorMat.at<UINT32>(new_i, new_j) << endl;
					}
				}

			//kinect.ShowOpenCVImage(colorMat, "img_color");
			//kinect.ShowOpenCVImage(colorMatRevise, "img_color");
			//kinect.ShowOpenCVImage(depthcolorMat, "img_color");

			double* center;
			double* normal;
			float angle;

			vector<VertexType> highestPlanePoints_3D;
			vector<Point> R;
			cv::Point2f vertices[4];
			string name = "imgs/img" + std::to_string(cnt);
			cv::imwrite(name + "_color.png", colorMatRevise);
			colorMatRevise = processImg(colorMatRevise, depthMat, center, normal, angle, highestPlanePoints_3D, vertices);
			kinect.ShowOpenCVImage(colorMatRevise, "depthcolor", 0);

			cv::imwrite(name + "_depth.png", depthMat);
			cv::imwrite(name + "_depthcolor.png", colorMatRevise);
			for (int i = 0; i < 4; i++)
			{
				R.push_back(Point(vertices[i].x, vertices[i].y));
			}

			//cv::Mat Points_3D = cv::Mat_<float>(highestPlanePoints_3D.size(), 3);
			//for (int i = 0; i < highestPlanePoints_3D.size(); i++)
			//{
			//	Points_3D.at<float>(i, 0) = highestPlanePoints_3D[i].x();
			//	Points_3D.at<float>(i, 1) = highestPlanePoints_3D[i].y();
			//	Points_3D.at<float>(i, 2) = highestPlanePoints_3D[i].z();
			//}
			//cv::PCA pca(Points_3D, cv::Mat(), CV_PCA_DATA_AS_ROW, 3);
			//pca.eigenvectors.convertTo(pca.eigenvectors, CV_32F);
			//cv::Mat eigenvectors  = pca.eigenvectors;
			//cout << eigenvectors << endl;
			//cv::Mat vector_z = eigenvectors.rowRange(0, 1).cross(eigenvectors.rowRange(1, 2)).reshape(0, 3);
			//vector_z.copyTo(eigenvectors.col(2));
			//cout << eigenvectors.rowRange(0, 1).cross(eigenvectors.rowRange(1, 2)) << endl;
			//cout << eigenvectors << endl;
			//cv::Mat centerMat = (cv::Mat_<float>(3, 1) << center[0], center[1], center[2]);
			//cout << centerMat << endl;
			//cv::Mat express2depthHomo = RT2HomogeneousMatrix(eigenvectors, centerMat);
			//cout << depth_Homo_cam2base << " " << express2depthHomo << endl;
			//cv::Mat express2roboticHomo = depth_Homo_cam2base * express2depthHomo;
			//cout << express2roboticHomo << endl;
			//cv::Mat express2roboticRotation, express2roboticTranslation;
			//HomogeneousMtr2RT(express2roboticHomo, express2roboticRotation, express2roboticTranslation);
			//if (express2roboticRotation.at<float>(2, 2) < 0)
			//{
			//	express2roboticRotation.at<float>(2, 2) *= -1;
			//	express2roboticRotation.at<float>(1, 2) *= -1;
			//	express2roboticRotation.at<float>(0, 2) *= -1;
			//}
			//cv::Mat vector_y = express2roboticRotation.rowRange(0, 1).cross(express2roboticRotation.rowRange(2, 3)).reshape(0, 3);
			//vector_y.copyTo(express2roboticRotation.col(1));
			//cv::Vec3f eulerAngles = rotationMatrixToEulerAngles(express2roboticRotation);
			//cout << "����Ϊ" << endl;
			//cout << express2roboticTranslation << endl;
			//cout << "ŷ����Ϊ" << endl;
			//cout << eulerAngles << endl;
			//cv::waitKey(0);
			//int iObj_num = ObjectLocation(colorCameraMatrix, (UINT16*)colorMat.data, iDistance, depthMatRevise.cols, depthMatRevise.rows,0, depthMatRevise.rows, ObjectRes);
			//DeleteBadObejct(ObjectRes, iObj_num);
			//���߶ȴӸߵ�������
			//sort(ObjectRes, ObjectRes + iObj_num, compareByHeight);
			//TODO:��iou��ȥ���ص����
			//std::sort(ObjectRes, ObjectRes + iObj_num, compare);
			//cout << "��⵽" << iObj_num << "������" << endl;
			//if (iObj_num == 0)
			//{
			//	Sleep(1000);
			//	//if(useRobot)
			//	//	sr->close();
			//	//cout << "�˳�" << endl;
			//	//break;
			//}

			//////��ʾͼƬ
			////cv::namedWindow("name", CV_WINDOW_NORMAL);  
			////cv::setWindowProperty("name", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
			////cv::imshow("name",depthcolorMat);
			////cv::waitKey(0);
			////cv::destroyAllWindows();

			//kinect.ShowOpenCVImage(depthcolorMat, "depthcolor", 0);
			//cout << "iObj_num:"<<iObj_num << endl;
//				if(i == 0)
//					Draw_Convex(depthcolorMat, depthcolorMat.cols, depthcolorMat.rows, ObjectRes[i].R);
				//kinect.ShowOpenCVImage(depthcolorMat, "depthcolor");
				float x=0, y=0;
				for (int j = 0; j < 4; j++)
				{
					x += R[j].x;
					y += R[j].y;
				}
				x /= 4, y /= 4;
				/*�������*/
				point2D.at<float>(0, 0) = x;
				point2D.at<float>(1, 0) = y;
				point2D.at<float>(2, 0) = 1;
				/*ƽ����Χ��ȣ��������*/
				
				double Zc = getDepthValue(depthMat, y, x, 6); 
				angle = calAngle(R, depthMat.rows, depthMat.cols, Zc);
				//ƽ�淨����
				cv::Mat rotationMatrix = calRotationMatrix(R, 0.6);
				cv::Vec3f eulerAngles = rotationMatrixToEulerAngles(rotationMatrix);
				cout << "�������ת����Ϊ" << endl;
				cout << rotationMatrix << endl;


				cv::Mat point3D = depth_R_base2cam.t() * (depthCameraMatrix.inv() * Zc * point2D - depth_t_base2cam);
				
				cout << "����Ϊ:" << endl;
				cout << point3D << endl;
				cout << "�����ŷ����Ϊ" << endl;
				cout << eulerAngles << endl;
				
				float coords[12];
				coords[0] = point3D.at<float>(0, 0);
				coords[1] = point3D.at<float>(1, 0);
				coords[2] = point3D.at<float>(2, 0) - 2;
				coords[3] = eulerAngles[0];
				coords[4] = eulerAngles[1];
				coords[5] = eulerAngles[2];
				coords[6] = 698 + 50;
				//coords[7] = 342 - 750 + cnt * 220;
				coords[7] = 0;
				cnt++;
			
				if(coords[2] > 160)
					coords[8] = 160 - 50;
				else
					coords[8] = coords[2] - 50;
				coords[9] = 0;
				coords[10] = 0;
				coords[11] = 0;

				if(useRobot)
					sr->moveRobot(coords);

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
			Draw_Polygon(img_depthcolor.data, img_depthcolor.cols, img_depthcolor.rows, 4, ObjectRes[i].R, 0, 0, 255);
		}
		kinect.ShowOpenCVImage(img_depthcolor, "depthcolor", 0);
	}
}
void DrawCenterPoints(cv::Mat& colorMat)
{
	colorMat.at<cv::Vec4b>(288, 320)[0] = 0;
	colorMat.at<cv::Vec4b>(288, 320)[1] = 0;
	colorMat.at<cv::Vec4b>(288, 320)[2] = 0;
	colorMat.at<cv::Vec4b>(288, 321)[0] = 0;
	colorMat.at<cv::Vec4b>(288, 321)[1] = 0;
	colorMat.at<cv::Vec4b>(288, 321)[2] = 0;
	colorMat.at<cv::Vec4b>(288, 319)[0] = 0;
	colorMat.at<cv::Vec4b>(288, 319)[1] = 0;
	colorMat.at<cv::Vec4b>(288, 319)[2] = 0;
	colorMat.at<cv::Vec4b>(289, 320)[3]= 0;
	colorMat.at<cv::Vec4b>(289, 320)[0] = 0;
	colorMat.at<cv::Vec4b>(289, 320)[1] = 0;
	colorMat.at<cv::Vec4b>(289, 320)[2] = 0;
}

