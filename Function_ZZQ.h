/*
Author: Zhequan Zhou 
TIME:2019-07-29
*/
#ifndef Function_ZZQ
#define Function_ZZQ

#include<iostream>
#include<cstdio>
#include<math.h>
#include<algorithm>
#include<vector>
#include <windows.h>
#include<minwindef.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#define INF 0x3f3f3f3f
#define INFLL 0x3f3f3f3f3f3f3f3f
#define mem(x,y) memset(x,y,sizeof(x))
typedef unsigned long long ULL;
typedef long long LL;
using namespace std;

struct Point {
	double x, y;
	Point() {}
	Point(double x, double y) :x(x), y(y) {}
	bool operator<(const Point& b) const {
		if (x != b.x) return x < b.x;
		return y < b.y;
	}
	Point operator-(const Point& P) const {
		return Point(x - P.x, y - P.y);
	}
	Point operator+(const Point& P) const {
		return Point(x + P.x, y + P.y);
	}
	double operator^(const Point& P) const { //���
		return x * P.y - y * P.x;//����ĳ����μǵü�LL
	}
	double operator*(const Point& P) const { //���
		return x * P.x + y * P.y;//����ĳ����μǵü�LL
	}
	Point operator*(const double b)const {
		return Point(x * b, y * b);
	}
	Point operator/(const double b)const {
		return Point(x / b, y / b);
	}
	double len()
	{
		return sqrt(x*x+y*y);
	}

};
typedef Point Vector;
//��㼯��͹����P������㼯��R��͹�����㼯��
void Convex(std::vector<Point>& P, std::vector<Point>& R);
//channel������ͼ���ͨ����3����4��R������ͼ�ϻ����Ķ���Ρ� cb��cg��cr��BGR��ɫ,
void Draw_Polygon(BYTE* image, LONG lwidth, LONG lheight, int channels, const std::vector<Point>& R, int cb, int cg, int cr);

//����㼯p����Ӿ��󲢻�����
//image����Ҫ��ͼ��ͼ��lwidth��lheight��ͼ���͸ߡ�channels��ͼ��ͨ������3��4����p���㼯��cb��cg��cr��BGR��ɫֵ
void Draw_MBROfPoints(BYTE* image, long lwidth, long lheight, int channels, std::vector<Point>& p, int cb, int cg, int cr);
//�������㼯p����Ӿ��󲢻�����
//image����Ҫ��ͼ��ͼ��lwidth��lheight��ͼ���͸ߡ�channels��ͼ��ͨ������3��4����p���㼯��cb��cg��cr��BGR��ɫֵ
void Draw_MBRsOfPoints(BYTE* image, long lwidth, long lheight, int channels, std::vector<vector<Point>>& p, int cb, int cg, int cr);
//��㼯������ı��Ρ�P������㼯��t������ı��ζ��㡣
void RC(std::vector<Point>& P, std::vector<Point> & t);
//�ж�һ�����Ƿ���͹������ڲ�
bool pointIsInRect(Point p, const std::vector<Point> & R);
#endif
