/*********************************/
/*Lv Shujing**********************/ 
/*���ܣ� ��ͨԪ���ٱ궨
//20170221-���Ӧ�þؼ�����б�ǶȺ���
/*********************************/
#ifndef _INC_ConComponentAPI
#define _INC_ConComponentAPI
#include "Function_ZZQ.h"//ZZQ�ӣ�2019.7.29
//---begin---��ͨԪ�㷨�ṹ��---begin----//
typedef struct a{
	long y;//�γ̵�Y����ֵ
	long beginx;//�γ̿�ʼ����X����ֵ
	long endx;//�γ�ĩ�˵�X����ֵ
	struct a *next;//ָ����ͨԪ�е���һ���γ�
	struct a *linenext;//ָ������е���һ���γ�
}runnode;//�γ̽ڵ�
typedef struct b{
	long minx;//��ͨԪ����������ص�����
	long maxx;//��ͨԪ�����ұ����ص�����
	long miny;//��ͨԪ�����ϱ����ص�����
	long maxy;//��ͨԪ�����±����ص�����
}shape;
typedef struct d{
	runnode *firstrunnode;//��ͨԪ�еĵ�һ���γ�
	runnode *lastrunnode;//ָ����ͨԪ�е����һ���γ�
	int value;//��ͨԪ�ı�ǩֵ
	shape compshape;//��ͨԪ����״��Ϣ
	LONG pixelnum;//��ͨԪ�е�ǰ��������
	BOOL sign;//��signΪTRUE�����ʾvalueֵ��ʾ��component������ͨԪ�ĸ�������ֵ����signΪFALSE,���ʾ��component������ͨԪ�ĸ�����valueֵ��ʾ����ɫֵ
	POINT Barycenter;//��ͨԪ������
}component;

typedef struct e{
	int value;//component�ı�ǩֵ
	shape compshape;//component����������Ϣ
	LONG pixelnum;//component��ǰ��������
	BOOL sign;//��signΪTRUE�����ʾvalueֵ��ʾ��component������ͨԪ�ĸ�������ֵ����signΪFALSE,���ʾ��component������ͨԪ�ĸ�����valueֵ��ʾ����ɫֵ
	unsigned int 
		distance;//��ˮ�صķ�����Ϊ1����ˮ�ط������ϣ���Ϊ-1����ˮ�ط������£���Ϊ0����һ���ն�����Ϊ����ֵ����˵������һ����ˮ��
	POINT Barycenter;//component������
	std::vector<Point> R;//��С��Ӿ��Σ�ZZQ�ӣ�2019.7.29
	std::vector<Point>P;//�߽磬ZZQ
}CodeComponent;
//---end---��ͨԪ�㷨�ṹ��----end------//

//�����ͨԪ
int ConCompLabelling8(BYTE *lpDIB,LONG	lWidth,LONG	lHeight,CodeComponent *rescomponent,BOOL imageflag,int max_num);
void ReleaseList(runnode **HeadRun,int Length);//�ͷ���ͨԪ�б��ڴ�
void Calculate_centroidc(CodeComponent* component, int comp_num);
int ConCompLabelling8_label(BYTE *lpDIB, LONG lWidth, LONG lHeight, CodeComponent *rescomponent, BOOL imageflag,int max_num);
int Cal_Ang(BYTE *pImage, int iWidth, int iHeight, int iTop, int iBottom, int iLeft, int iRight, int iValue);
//int Cal_Round(BYTE *pImage, int iWidth, int iHeight, int iTop, int iBottom, int iLeft, int iRight,CPoint Round[8], int iValue);//�������Χ��
//��ͨԪ����������
void Comp_Order(CodeComponent *rescomponent, int Comp_num);
//��������
int Partition(double * aaa, int low, int high, int * sn);
void Quick_Order(double * aaa, int left, int right, int * sn);
//����ͼ��ĻҶȷ�Χ
void Gray_Area(BYTE *proImage, int lWidth, int lHeight,int &gray1,int &gray2);
//ȫ����ֵ��ֲ���ֵ��ӺϵĶ�ֵ������
void Otsu_Bernsen(BYTE* proImage, int lWidth, int lHeight,int iValue);
#endif
