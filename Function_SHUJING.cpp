/*
Author: SHUJING LYU
TIME:2019-05-17


*/
//#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "Function_SHUJING.h"
#include "ConComponent.h"


void Image_Binary(unsigned char* image, int wid, int hei)
{
	int i, j;
	for (i = 0; i < hei; i++)
	{
		for (j = 0; j < wid; j++)
		{
			if (image[i*wid + j] < 100)
				image[i*wid + j] = 0;
			else
				image[i*wid + j] = 255;
		}
	}

}

int ObjectLocation(cv::Mat cameraParameters, UINT16* DepthFrameData, int iDistance, int iWid, int iHei, int iTop, int iBottom, Object* ObjectRes)
{
	BYTE* DepthImage = (BYTE*)malloc(iWid * iHei * sizeof(BYTE));
	int cnt = 0;
	if (DepthFrameData != NULL)
	{
		//Read the depthFrameData in uint_16
		int wid_depth = iWid;
		int hei_depth = iHei;
		memset(DepthImage, 0, hei_depth * wid_depth); //�����Ϣת�Ҷ�ԭͼ
		for (int i = 0; i < hei_depth; i++)//������Ƥ����Χ�ڵ�ͼ����д���
		{
			for (int j = 0; j < wid_depth; j++)
			{
				//k = 5.3;
				if (DepthFrameData[i * iWid + j] == 0 || abs(iDistance - DepthFrameData[i * iWid + j]) < 20)//������ֵ��ʧ���ߺ�Ƥ���߶Ȳ��Ĳ��֣���Ϊ1300��Ƥ���ĸ߶ȣ�
				{
					//DepthImage[i*iWid + j] = DepthFrameData[130*iWid + 320] / k;
					DepthImage[i * iWid + j] = 255;// iDistance / 5.3;// iDistance��Ƥ���߶ȣ�����Ƥ������ͳһ����
				}
				else if (iDistance - DepthFrameData[i * iWid + j] < 400 && iDistance - DepthFrameData[i * iWid + j]>0)
				{	//��Ƥ���ڵİ�������ź�ת��Ϊ�Ҷ�ֵ
					cnt++;
					DepthImage[i * iWid + j] = 255 - (float)(iDistance - DepthFrameData[i * iWid + j]) / 400 * 255;// / 5.3;//((float)(DepthFrameData[i*iWid + j] - 700)/600)*255;
				}
				else
					DepthImage[i * iWid + j] = 255;
			}
		}
	}

	const int DEPTH = 1280;//�����������ĸ߶�
	int i, j, k;
	int iCen = 0;
	int iHis[256];
	float fx = cameraParameters.at<float>(0, 0);
	float fy = cameraParameters.at<float>(1, 1);
	float cx = cameraParameters.at<float>(0, 2);
	float cy = cameraParameters.at<float>(1, 2);
	//�����ֵ��ͼ
	BYTE* image_t1; //�������
	image_t1 = (BYTE*)malloc(1000 * 1000 * sizeof(BYTE));
	memset(image_t1, 255, iWid*iHei);
	//ͳ��ֱ��ͼ
	memset(iHis, 0, 256 * sizeof(int));
	for (i = iTop; i < iBottom; i++)
	{
		for (j = 20; j < iWid; j++)
		{
			iHis[DepthImage[i* iWid + j]]++;
		}
	}
	//�Բ�ͬ�߶����尴��Ƚ��зֲ㴦��
	int inum = 5;
	int iBegin[7];
	int iEnd[7];
	iCen = iHis_part(iHis, iBegin, iEnd,inum);//���طֲ�����iBegin,iEnd��ÿһ����ֱ��ͼ��ֱ��ͼ��Χ
	//�������ж�
	int num_com;
	int kk;
	int minx, maxx, miny, maxy;
	int vsum = 0;
	int vi = 0;
	int vlength[10];
	int vwideth[10];
	int vheight[10];
	int volume[10];
	std::vector<Point> rects[10];//ZZQ�ӣ���С��Ӿ���
	int minx1[10];
	int maxx1[10];
	int miny1[10];
	int maxy1[10];
	int minx_p[10];
	int miny_p[10];
	memset(vlength, 0, sizeof(int)* 10);
	memset(vwideth, 0, sizeof(int)* 10);
	memset(vheight, 0, sizeof(int)* 10);
	memset(volume, 0, sizeof(int)* 10);
	memset(minx_p, 0, sizeof(int)* 10);
	memset(miny_p, 0, sizeof(int)* 10);
	memset(minx1, 0, sizeof(int)* 10);
	memset(maxx1, 0, sizeof(int)* 10);
	memset(miny1, 0, sizeof(int)* 10);
	memset(maxy1, 0, sizeof(int)* 10);
	int gao1 = 0;
	int inum1[DEPTH];
	int inum_all = 0;
	int iobj_num=0;//��⵽�Ŀ������
	static CodeComponent rescomponent[40]; //��ͨԪ
	for (kk = 0; kk < iCen; kk++)
	{
		for (int i = 0; i < 20; i++)
			rescomponent[i].P.clear(), rescomponent[i].R.clear();//ZZQ
		memset(image_t1, 255, iWid*iHei*sizeof(BYTE));
		for (i = iTop; i < iBottom; i++)
		{
			for (j = 20; j < iWid; j++)
			{
				if (DepthImage[i* iWid + j] >iBegin[kk] - 1 && DepthImage[i* iWid + j] < iEnd[kk]+1)//ѡ����Ϊ0����ֵ
				{
					image_t1[i*iWid + j] = 0;
				}
			}
		}
		

		//��image_t1����ɫ����
		num_com = 0;
		num_com = ConCompLabelling8_label(image_t1, iWid, iHei, rescomponent, FALSE, 20);//���ظò�Ŀ������
		if (num_com < 1)
			break;
		int pixelnum = 500;
		
		for (k = 0; k < num_com; k++)
		{
			if (pixelnum > rescomponent[k].pixelnum)
			{
				rescomponent[k].sign = true;
				continue;
			}
			if (rescomponent[k].compshape.maxx - rescomponent[k].compshape.minx < 30
				|| rescomponent[k].compshape.maxy - rescomponent[k].compshape.miny < 30)
			{
				rescomponent[k].sign = true;
				continue;
			}

			/*minx = rescomponent[k].compshape.minx;
			miny = rescomponent[k].compshape.miny;
			maxx = rescomponent[k].compshape.maxx;
			maxy = rescomponent[k].compshape.maxy;
			*/
			//�ҵ������ε��������ұ߽�
			minx = min(rescomponent[k].R[0].x, min(rescomponent[k].R[1].x, min(rescomponent[k].R[2].x, rescomponent[k].R[3].x)));
			miny = min(rescomponent[k].R[0].y, min(rescomponent[k].R[1].y, min(rescomponent[k].R[2].y, rescomponent[k].R[3].y)));
			maxx = max(rescomponent[k].R[0].x, max(rescomponent[k].R[1].x, max(rescomponent[k].R[2].x, rescomponent[k].R[3].x)));
			maxy = max(rescomponent[k].R[0].y, max(rescomponent[k].R[1].y, max(rescomponent[k].R[2].y, rescomponent[k].R[3].y)));
			//���������������
			vsum = 0;
			vi = 0;
	/*		for (i = miny; i < maxy; i++)
			{
				for (j = minx; j < maxx; j++)
				{
					// ����㣨�飬�꣩�����������ھ�����
					if (pointIsInRect(Point(j,i), rescomponent[k].R) == 0)continue;//ZZQ
					vi = DepthFrameData[i*iWid + j] * DepthFrameData[i*iWid + j] * (iDistance - DepthFrameData[i*iWid + j]) / fx / fy;//���������ʽ
					vsum = vsum + vi;
				}
			}*/
			volume[iobj_num] = vsum;// / 1000;
			minx_p[iobj_num] = minx;
			miny_p[iobj_num] = miny;
			minx1[iobj_num] = minx;
			maxx1[iobj_num] = maxx;
			miny1[iobj_num] = miny;
			maxy1[iobj_num] = maxy;
			rects[iobj_num] = rescomponent[k].R;//ZZQ��
			//DepthImage[maxy*depthFrame.width + maxx] = 0;

			//������Ʒ�߶�
			gao1 = 0;			
			inum_all = 0;
			memset(inum1, 0, DEPTH * sizeof(int));//ͳ�Ƹ߶�
			for (i = max(miny, 0); i < min(maxy, iHei); i++)
			{
				for (j = max(minx, 0); j < min(maxx, iWid); j++)
				{
					if (((UINT16*)DepthFrameData)[iWid*i + j]<DEPTH)
						inum1[((UINT16*)DepthFrameData)[iWid*i + j]]++;
				}
			}
			for (i = 20; i<DEPTH; i++)
			{
				inum_all = inum_all + inum1[i];
				if (inum_all>(maxx - minx)*(maxy - miny) / 5)
				{
					gao1 = iDistance - i;
					break;
				}

			}
			//	gao1 = iDistance - DepthFrameData[depthFrame.width*(maxy + miny) / 2 + (maxx + minx) / 2];
			vheight[iobj_num] = gao1;

			if ((maxx - minx) > (maxy - miny))
			{

				vlength[iobj_num] = abs((maxx - cx) / fx*(iDistance - gao1) - (minx - cx) / fx*(iDistance - gao1));
				vwideth[iobj_num] = abs((maxy - cy) / fy*(iDistance - gao1) - (miny - cy) / fy*(iDistance - gao1));
			}
			else
			{
				
				vwideth[iobj_num] = abs((maxx - cx) / fx*(iDistance - gao1) - (minx - cx) / fx*(iDistance - gao1));
				vlength[iobj_num] = abs((maxy - cy) / fy*(iDistance - gao1) - (miny - cy) / fy*(iDistance - gao1));

			}
			minx1[iobj_num] = abs((minx - cx) / fx*(iDistance -0) - (0 - cx) / fx*(iDistance - 0));
			maxx1[iobj_num] = abs((maxx - cx) / fx*(iDistance - 0) - (0 - cx) / fx*(iDistance - 0));
			miny1[iobj_num] = abs((miny - cy) / fy*(iDistance - 0) - (0 - cy) / fy*(iDistance - 0));
			maxy1[iobj_num] = abs((maxy - cy) / fy*(iDistance - 0) - (0 - cy) / fy*(iDistance - 0));
			iobj_num++;
			if (iobj_num > 9)
				break;
		}

		

	}
	//Ŀ�����������
	int sn[10];
	
	double minx1_d[10];
	for (i = 0; i < 10; i++)
	{
		minx1_d[i] = minx1[i];
		sn[i] = i;
	}
	Quick_Order(minx1_d, 0, iobj_num - 1, sn);
	for (k = 0; k < iobj_num; k++)
	{
		ObjectRes[k].hei = vheight[sn[k]];
		ObjectRes[k].len = vlength[sn[k]];
		ObjectRes[k].wid = vwideth[sn[k]];

		//ObjectRes[k].len = (rects[sn[k]][0] - rects[sn[k]][1]).len();
		//ObjectRes[k].wid = (rects[sn[k]][1] - rects[sn[k]][2]).len();
		ObjectRes[k].vol = volume[sn[k]];
		ObjectRes[k].minx = minx1[sn[k]];
		ObjectRes[k].maxx = maxx1[sn[k]];
		ObjectRes[k].miny = miny1[sn[k]];
		ObjectRes[k].maxy = maxy1[sn[k]];
		ObjectRes[k].minx_p = minx_p[sn[k]];
		ObjectRes[k].miny_p = miny_p[sn[k]];
		ObjectRes[k].R = rects[sn[k]];
		
	}
	free(image_t1);
	free(DepthImage);
	return iobj_num;

}
int iHis_part(int* iHis, int* iBegin, int* iEnd, int inum)
{
	int i,j,k;
	int iBeg_t[100];
	int iEnd_t[100];
	int iPixel_num;
	int threshold_num = 100;
	bool bflg = false;
	k = 0;
	for (i = 1; i <255;i++)
	{
		if (bflg==false&&iHis[i] > threshold_num)
		{
			iBeg_t[k] = i;
			bflg = true;
		}
		else if (bflg == true && iHis[i] < threshold_num)
		{
			iEnd_t[k] = i - 1;
			bflg = false;
			k++;
		}
	}
	if (k < 1)
		return 0;
	int kk = 0;
	for (i = 0; i < k-1; i++)
	{
		iPixel_num = 0;
		for (j = iBeg_t[i]; j < iEnd_t[i] + 1; j++)
		{
			iPixel_num = iPixel_num + iHis[j];
		}
		if (iPixel_num>1000 )//&& iBeg_t[i + 1] - iEnd_t[i]>5)
		{
			iBegin[kk] = iBeg_t[i];
			iEnd[kk] = iEnd_t[i];
			kk++;
			if (kk > inum-1)
				break;
		}
	}
	//���һ���ж�
	iPixel_num = 0;
	for (j = iBeg_t[k-1]; j < iEnd_t[k-1] + 1; j++)
	{
		iPixel_num = iPixel_num + iHis[j];
	}
	if (iPixel_num>threshold_num)
	{
		iBegin[kk] = iBeg_t[k-1];
		iEnd[kk] = iEnd_t[k-1];
		kk++;
	}
	return kk;
}