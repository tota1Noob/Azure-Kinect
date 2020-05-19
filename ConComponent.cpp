#include <math.h>
#include "ConComponent.h"
//���ܣ��ͷ���ͬԪ�궨ʱ����������
//20170221-���Ӧ�þؼ�����б�ǶȺ���
#define STACKSIZE 20000



void ReleaseList(runnode * *HeadRun, int Length)
{
	runnode* current = NULL;
	runnode* precurrent = NULL;
	for (int i = 0; i < Length; i++)//�ͷ��γ̱�
	{
		if (HeadRun[i] != NULL)
		{
			current = HeadRun[i];
			while (current != NULL)
			{
				precurrent = current;
				current = current->linenext;
				delete precurrent;
			}
		}
	}

}
//��ͨԪ�궨

int ConCompLabelling8(BYTE* lpDIB, LONG lWidth, LONG lHeight, CodeComponent* rescomponent, BOOL imageflag, int max_num)
{
	// ָ��Դͼ���ָ��
	BYTE* pdibData;
	//��ͼ���ָ��
	int* pdata;
	pdata = (int*)malloc((lWidth + 2) * (lHeight + 2) * sizeof(int));
	if (pdata == NULL)
		return -1;
	// ͼ��ÿ�е��ֽ���
	LONG	lLineBytes;

	pdibData = lpDIB;
	lLineBytes = lWidth;

	int count;
	int i, j;
	long temp, nexttemp;
	runnode* current = NULL;
	runnode* nextcurrent = NULL;
	runnode* precurrent = NULL;
	runnode* preceding = NULL;

	component stack[STACKSIZE];//��ͨԪ��ǩ��ջ
	runnode** headrun;//ÿ�е��γ�ָ��
	headrun = (runnode * *)malloc((lHeight + 2) * sizeof(runnode*));
	if (headrun == NULL)
	{
		free(pdata);
		return -1;
	}
	for (i = 0; i < lHeight + 2; i++)
		headrun[i] = NULL;
	for (i = 0; i < STACKSIZE; i++)//��ʼ����ջ
	{
		stack[i].firstrunnode = NULL;
		stack[i].lastrunnode = NULL;
		stack[i].value = 255;
		stack[i].pixelnum = 0;
	}

	// ��ʼ������pdata��ʼ************
	for (i = 0; i < lHeight + 1; i++)
	{
		for (j = 0; j < lWidth + 2; j++)
		{
			if (i == 0)
				pdata[j] = 255;
			else
			{
				if (j != 0 && j != (lWidth + 1)) //////////////////////////////////////
				{
					temp = lLineBytes * (i - 1) + j - 1;
					pdata[i * (lWidth + 2) + j] = (int)pdibData[temp];
				}
				else
				{
					pdata[i * (lWidth + 2) + j] = 255;
				}
			}
		}
	}
	for (i = lHeight + 1, j = 0; j < lWidth + 2; j++)
		pdata[i * (lWidth + 2) + j] = 255;
	//��ʼ������pdata����********************

	//��ʼ����ͨԪ��ǩ***********************
	count = 1;
	for (i = 1; i < lHeight + 1; i++)
	{
		for (j = 1; j < lWidth + 1; j++)
		{
			temp = (lWidth + 2) * i + j;
			if (pdata[temp] != 255)//��ǰ����Ϊǰ������
			{
				if ((pdata[temp - 1] + pdata[temp - lWidth - 1] + pdata[temp - lWidth - 2] + pdata[temp - lWidth - 3]) != 255 * 4)
				{
					if (pdata[temp - 1] != 255)//�������Ϊǰ������
					{
						current->endx++;
						pdata[temp] = stack[pdata[temp - 1]].value;
					}
					else if (pdata[temp - lWidth - 3] != 255)//����
					{
						preceding = current;
						current = new runnode;
						if (current == NULL)
						{
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						if (headrun[i] == NULL)
						{
							headrun[i] = current;
							preceding = current;
						}
						else
							preceding->linenext = current;
						current->y = i;
						current->beginx = j;
						current->endx = j;
						current->linenext = NULL;
						current->next = NULL;
						if (stack[pdata[temp - lWidth - 3]].lastrunnode == NULL)
						{
							printf("stack[pdata[temp-lWidth-3]].lastrunnode==NULL");
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;

						}
						stack[pdata[temp - lWidth - 3]].lastrunnode->next = current;
						stack[pdata[temp - lWidth - 3]].lastrunnode = current;
						pdata[temp] = stack[pdata[temp - lWidth - 3]].value;
					}//���Ͻ���

					else if (pdata[temp - lWidth - 2] != 255)//��������
					{
						preceding = current;
						current = new runnode;
						if (current == NULL)
						{
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						if (headrun[i] == NULL)
						{
							headrun[i] = current;
							preceding = current;
						}
						else
							preceding->linenext = current;
						current->y = i;
						current->beginx = j;
						current->endx = j;
						current->linenext = NULL;
						current->next = NULL;
						if (stack[pdata[temp - lWidth - 2]].lastrunnode == NULL)
						{
							printf("stack[pdata[temp-lWidth-2]].lastrunnode==NULL");

							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						if (stack[pdata[temp - lWidth - 2]].lastrunnode->next == NULL)
							stack[pdata[temp - lWidth - 2]].lastrunnode->next = current;
						stack[pdata[temp - lWidth - 2]].lastrunnode = current;
						pdata[temp] = stack[pdata[temp - lWidth - 2]].value;
					}//���Ͻ���

					else if (pdata[temp - lWidth - 1] != 255)//����
					{
						preceding = current;
						current = new runnode;
						if (current == NULL)
						{
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						if (headrun[i] == NULL)
						{
							headrun[i] = current;
							preceding = current;
						}
						else
							preceding->linenext = current;
						current->y = i;
						current->beginx = j;
						current->endx = j;
						current->linenext = NULL;
						current->next = NULL;
						if (stack[pdata[temp - lWidth - 1]].lastrunnode == NULL)
						{
							printf("stack[pdata[temp-lWidth-1]].lastrunnode==NULL");
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						stack[pdata[temp - lWidth - 1]].lastrunnode->next = current;
						stack[pdata[temp - lWidth - 1]].lastrunnode = current;
						pdata[temp] = stack[pdata[temp - lWidth - 1]].value;
					}//���Ͻ���
				}//end for (***if((pdata[temp-1]+pdata[temp-lWidth-1]+pdata[temp-lWidth-2]+pdata[temp-lWidth-3])!=255*4)***)
				else//��ʼһ���µ���ͨԪ
				{
					preceding = current;
					current = new runnode;
					if (current == NULL)
					{
						ReleaseList(headrun, lHeight + 2);///zhoulijun add
						free(pdata);
						free(headrun);
						return 0;
					}
					if (headrun[i] == NULL)
					{
						headrun[i] = current;
						preceding = current;
					}
					else
						preceding->linenext = current;
					current->beginx = j;
					current->endx = j;
					current->y = i;
					current->linenext = NULL;
					current->next = NULL;
					pdata[temp] = count;
					count++;
					if (count > STACKSIZE - 1)
					{
						i = lHeight + 2;
						j = lWidth + 2;
					}
					if (count % 255 == 0)
						count++;
					stack[pdata[temp]].compshape.minx = j;
					stack[pdata[temp]].firstrunnode = current;
					stack[pdata[temp]].value = pdata[temp];
					stack[pdata[temp]].lastrunnode = current;
				}
			}// end for (**if(pdata[temp]!=255)//��ǰ����Ϊǰ������**)
		}//end for (**for(j=1;j<lWidth+1;j++)**)
	}//end for (**for(i=1;i<lHeight+1;i++)**)
	//----------------------------------------------------
	LONG minx, maxx, miny, maxy;
	for (i = 1; i < count; i++)
	{
		minx = lWidth;
		maxx = 0;
		miny = lHeight;
		maxy = 0;
		current = stack[i].firstrunnode;
		while (current != NULL)
		{
			if (current->beginx < minx) minx = current->beginx;
			if (current->endx > maxx) maxx = current->endx;
			if (current->y < miny) miny = current->y;
			if (current->y > maxy) maxy = current->y;
			stack[i].pixelnum = stack[i].pixelnum + current->endx - current->beginx + 1;
			current = current->next;
		}
		stack[i].compshape.minx = minx;
		stack[i].compshape.maxx = maxx;
		stack[i].compshape.miny = miny;
		stack[i].compshape.maxy = maxy;
		stack[i].sign = FALSE;
	}
	//---��ͨԪ��ǩ��ǽ���-----------

	//---�ϲ���ǩΪ��ͨԪ-------------

	int k;
	long index, nextindex;
	for (i = lHeight; i > 1; i--)
	{
		if (headrun[i] != NULL && headrun[i - 1] != NULL)
		{
			current = headrun[i];
			nextcurrent = headrun[i - 1];
			while (current != NULL && nextcurrent != NULL)
			{

				if ((current->beginx) >= (nextcurrent->endx + 2) || (current->endx) <= (nextcurrent->beginx - 2))
				{
					if (current->beginx >= nextcurrent->endx + 2)
						nextcurrent = nextcurrent->linenext;
					else current = current->linenext;
				}

				else if (current->beginx + 1 <= nextcurrent->beginx)
				{
					temp = current->y * (lWidth + 2) + current->beginx;
					nexttemp = nextcurrent->y * (lWidth + 2) + nextcurrent->beginx;
					if (stack[pdata[temp]].sign == FALSE && stack[pdata[nexttemp]].sign == FALSE)
					{
						if (stack[pdata[temp]].value != stack[pdata[nexttemp]].value)
						{
							index = pdata[temp];//index��ʾ��ǰ�γ��е�����������ͨԪ�ĸ�������
							nextindex = pdata[nexttemp];//nextindex��ʾ��һ�е�ǰ�γ��е�����������ͨԪ�ĸ�������
							stack[nextindex].value = index;//��ʼ������һ���е��γ����ڵ�component������ǰ�γ����ڵ�component
							stack[nextindex].sign = TRUE;
							stack[index].pixelnum = stack[index].pixelnum + stack[nextindex].pixelnum;
							if (stack[index].compshape.minx > stack[nextindex].compshape.minx)
								stack[index].compshape.minx = stack[nextindex].compshape.minx;
							if (stack[index].compshape.maxx < stack[nextindex].compshape.maxx)
								stack[index].compshape.maxx = stack[nextindex].compshape.maxx;
							if (stack[index].compshape.miny > stack[nextindex].compshape.miny)
								stack[index].compshape.miny = stack[nextindex].compshape.miny;
							if (stack[index].compshape.maxy < stack[nextindex].compshape.maxy)
								stack[index].compshape.maxy = stack[nextindex].compshape.maxy;//����
						}
					}
					else if (stack[pdata[temp]].sign == FALSE && stack[pdata[nexttemp]].sign == TRUE)
					{
						index = pdata[temp];
						nextindex = pdata[nexttemp];
						while (stack[nextindex].sign == TRUE)
						{
							nextindex = stack[nextindex].value;
						}
						if (stack[nextindex].value != index)
						{
							stack[nextindex].value = index;//��ʼ����һ���е��γ����ڵ�component������ǰ�γ����ڵ�component
							stack[nextindex].sign = TRUE;
							stack[index].pixelnum = stack[index].pixelnum + stack[nextindex].pixelnum;
							if (stack[index].compshape.minx > stack[nextindex].compshape.minx)
								stack[index].compshape.minx = stack[nextindex].compshape.minx;
							if (stack[index].compshape.maxx < stack[nextindex].compshape.maxx)
								stack[index].compshape.maxx = stack[nextindex].compshape.maxx;
							if (stack[index].compshape.miny > stack[nextindex].compshape.miny)
								stack[index].compshape.miny = stack[nextindex].compshape.miny;
							if (stack[index].compshape.maxy < stack[nextindex].compshape.maxy)
								stack[index].compshape.maxy = stack[nextindex].compshape.maxy;
						}//����
					}
					else if (stack[pdata[temp]].sign == TRUE && stack[pdata[nexttemp]].sign == FALSE)
					{
						index = pdata[temp];
						while (stack[index].sign == TRUE)
						{
							index = stack[index].value;
						}
						nextindex = pdata[nexttemp];
						if (stack[nextindex].value != index)
						{
							stack[nextindex].value = index;//��ʼ������һ���е��γ����ڵ�component������ǰ�γ����ڵ�component
							stack[nextindex].sign = TRUE;
							stack[index].pixelnum = stack[index].pixelnum + stack[nextindex].pixelnum;
							if (stack[index].compshape.minx > stack[nextindex].compshape.minx)
								stack[index].compshape.minx = stack[nextindex].compshape.minx;
							if (stack[index].compshape.maxx < stack[nextindex].compshape.maxx)
								stack[index].compshape.maxx = stack[nextindex].compshape.maxx;
							if (stack[index].compshape.miny > stack[nextindex].compshape.miny)
								stack[index].compshape.miny = stack[nextindex].compshape.miny;
							if (stack[index].compshape.maxy < stack[nextindex].compshape.maxy)
								stack[index].compshape.maxy = stack[nextindex].compshape.maxy;//����
						}
					}
					else
					{
						if (stack[pdata[temp]].value != stack[pdata[nexttemp]].value)
						{
							index = pdata[temp];
							while (stack[index].sign == TRUE)
							{
								index = stack[index].value;
							}
							nextindex = pdata[nexttemp];
							while (stack[nextindex].sign == TRUE)
							{
								nextindex = stack[nextindex].value;
							}
							if (stack[nextindex].value != index)
							{
								stack[nextindex].value = index;//��ʼ����һ���е��γ����ڵ�component������ǰ�γ����ڵ�component
								stack[nextindex].sign = TRUE;
								stack[index].pixelnum = stack[index].pixelnum + stack[nextindex].pixelnum;
								if (stack[index].compshape.minx > stack[nextindex].compshape.minx)
									stack[index].compshape.minx = stack[nextindex].compshape.minx;
								if (stack[index].compshape.maxx < stack[nextindex].compshape.maxx)
									stack[index].compshape.maxx = stack[nextindex].compshape.maxx;
								if (stack[index].compshape.miny > stack[nextindex].compshape.miny)
									stack[index].compshape.miny = stack[nextindex].compshape.miny;
								if (stack[index].compshape.maxy < stack[nextindex].compshape.maxy)
									stack[index].compshape.maxy = stack[nextindex].compshape.maxy;
							}
						}//����
					}


					if (current->endx <= nextcurrent->endx)
					{
						current = current->linenext;
						//	nextcurrent=nextcurrent->linenext;
					}
					else nextcurrent = nextcurrent->linenext;
				}//end for (**else if(current->beginx+1<=nextcurrent->beginx)**)

				else if (current->beginx >= nextcurrent->beginx - 1)//��ǩ���ʱ�Ѿ����ǵ�
				{
					if (current->endx <= nextcurrent->endx)
					{
						current = current->linenext;
						//	nextcurrent=nextcurrent->linenext;
					}
					else
					{
						nextcurrent = nextcurrent->linenext;
					}

				}
			}//end for (**while(current!=NULL&&nextcurrent!=NULL)**)

		}//end for (**if(headrun[i]!=NULL&&headrun[i-1]!=NULL)**)
	}// ��ͨԪ�궨����



	//�˳������Լ�������̫�ٵ���ͨԪ------------------------------------------------------
	LONG SmallThresh;
	SmallThresh = 15;

	long precdeletenum;//�˳�����ǰ����ͨԪ����
	for (i = 1, k = 0; i < count; i++)//�Ѹ���ͨԪ�ĸ���������ȡ���������pstack�����У��Լ��ٺ�����ʣ�������ʱ��
	{
		if (i % 255 == 0)
			stack[i].sign = TRUE;
		if (stack[i].sign == FALSE)
		{
			if (stack[i].compshape.maxy - stack[i].compshape.miny <= 1 || stack[i].compshape.maxx - stack[i].compshape.minx < 2)
			{
				stack[i].sign = TRUE;
				stack[i].value = 255;
			}
			else
				k++;
		}
	}
	for (i = 1, precdeletenum = 0; i < count; i++)//�Ѹ���ͨԪ�ĸ���������ȡ���������pstack�����У��Լ��ٺ�����ʣ�������ʱ��
	{
		if (stack[i].sign == FALSE && stack[i].pixelnum > SmallThresh)

		{
			rescomponent[precdeletenum].sign = FALSE;
			rescomponent[precdeletenum].pixelnum = stack[i].pixelnum;
			rescomponent[precdeletenum].compshape.maxx = stack[i].compshape.maxx - 1;
			rescomponent[precdeletenum].compshape.minx = stack[i].compshape.minx - 1;
			rescomponent[precdeletenum].compshape.maxy = stack[i].compshape.maxy - 1;
			rescomponent[precdeletenum].compshape.miny = stack[i].compshape.miny - 1;
			rescomponent[precdeletenum].value = stack[i].value;
			precdeletenum++;
		}
		if (precdeletenum > max_num - 1)
			break;
	}

	//��ͼ���е�ǰ�����ظ�ֵΪ��������ͨԪ�ı�־ֵ������������Ӿ����е���������
	runnode* p;
	for (i = 1; i < count; i++)
	{
		p = stack[i].firstrunnode;//
		while (p != NULL)
		{
			for (j = p->beginx; j <= p->endx; j++)
			{
				temp = p->y * (lWidth + 2) + j;
				index = pdata[temp];
				while (stack[index].sign == TRUE && index != 255)
				{
					index = stack[index].value;
				}
				if (index == 255) pdata[temp] = 255;
				else
					pdata[temp] = index;


			}
			p = p->next;
		}
	}
	LONG anothertemp;
	for (i = 1; i < lHeight + 1; i++)
	{
		for (j = 1; j < lWidth + 1; j++)
		{
			temp = lLineBytes * (i - 1) + j - 1;
			anothertemp = i * (lWidth + 2) + j;
			if (pdata[anothertemp] % 255 != 0 && pdata[anothertemp] != 0)
				pdibData[temp] = (BYTE)pdata[anothertemp] % 255;
			else
				pdibData[temp] = (BYTE)pdata[anothertemp];
		}

	}
	ReleaseList(headrun, lHeight + 2);///�ͷ��γ̱�
	free(pdata);
	free(headrun);
	return precdeletenum;
}


void Calculate_centroidc(CodeComponent* component, int comp_num)
{
	int i;
	int x = 0, y = 0;
	for (i = 0; i < comp_num; i++)
	{
		y = (component[i].compshape.miny + component[i].compshape.maxy) / 2;
		x = (component[i].compshape.minx + component[i].compshape.maxx) / 2;
		component[i].Barycenter.x = x;
		component[i].Barycenter.y = y;
	}
}
//��ͨԪ�궨
int ConCompLabelling8_label(BYTE* lpDIB, LONG lWidth, LONG lHeight, CodeComponent* rescomponent, BOOL imageflag, int max_num)
{
	// ָ��Դͼ���ָ��
	int* pdibData = (int*) malloc(lWidth*lHeight*sizeof(int));
	for (int i = 0; i < lHeight * lWidth; i++)
		pdibData[i] = (int)lpDIB[i];
	//��ͼ���ָ��
	int* pdata;
	pdata = (int*)malloc((lWidth + 2) * (lHeight + 2) * sizeof(int));
	if (pdata == NULL)
		return -1;
	// ͼ��ÿ�е��ֽ���
	LONG	lLineBytes;

	lLineBytes = lWidth;

	int count;
	int i, j;
	long temp, nexttemp;
	runnode* current = NULL;
	runnode* nextcurrent = NULL;
	runnode* precurrent = NULL;
	runnode* preceding = NULL;

	component stack[STACKSIZE];//��ͨԪ��ǩ��ջ
	runnode** headrun;//ÿ�е��γ�ָ��
	headrun = (runnode * *)malloc((lHeight + 2) * sizeof(runnode*));
	if (headrun == NULL)
	{
		free(pdata);
		return -1;
	}
	for (i = 0; i < lHeight + 2; i++)
		headrun[i] = NULL;
	for (i = 0; i < STACKSIZE; i++)//��ʼ����ջ
	{
		stack[i].firstrunnode = NULL;
		stack[i].lastrunnode = NULL;
		stack[i].value = 255;
		stack[i].pixelnum = 0;
	}

	// ��ʼ������pdata��ʼ************��ԭͼ�񿽱���pdata�У��Ա����һȦ���õ�255������������ͨ��
	for (i = 0; i < lHeight + 1; i++)
	{
		for (j = 0; j < lWidth + 2; j++)
		{
			if (i == 0)
				pdata[j] = 255;
			else
			{
				if (j != 0 && j != (lWidth + 1)) //////////////////////////////////////
				{
					temp = lLineBytes * (i - 1) + j - 1;
					pdata[i * (lWidth + 2) + j] = (int)pdibData[temp];
				}
				else
				{
					pdata[i * (lWidth + 2) + j] = 255;
				}
			}
		}
	}
	for (i = lHeight + 1, j = 0; j < lWidth + 2; j++)
		pdata[i * (lWidth + 2) + j] = 255;
	//��ʼ������pdata����********************

	//��ʼ����ͨԪ��ǩ***********************8��ͨ
	count = 1;//��ͨԪ��ż���
	for (i = 1; i < lHeight + 1; i++)
	{
		for (j = 1; j < lWidth + 1; j++)
		{
			temp = (lWidth + 2) * i + j;
			if (pdata[temp] != 255)//��ǰ����Ϊǰ������
			{
				if ((pdata[temp - 1] + pdata[temp - lWidth - 1] + pdata[temp - lWidth - 2] + pdata[temp - lWidth - 3]) != 255 * 4)//�����ǰǰ�����ص��ѱ����������ص�����ǰ������
				{
					if (pdata[temp - 1] != 255)//�������Ϊǰ������
					{
						current->endx++;
						pdata[temp] = stack[pdata[temp - 1]].value;//�ѵ�ǰǰ�����ر��Ϊ��ͨԪ���
					}
					else if (pdata[temp - lWidth - 3] != 255)//����
					{
						preceding = current;
						current = new runnode;
						if (current == NULL)
						{
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						if (headrun[i] == NULL)
						{
							headrun[i] = current;
							preceding = current;
						}
						else
							preceding->linenext = current;
						current->y = i;
						current->beginx = j;
						current->endx = j;
						current->linenext = NULL;
						current->next = NULL;
						if (stack[pdata[temp - lWidth - 3]].lastrunnode == NULL)
						{
							printf("stack[pdata[temp-lWidth-3]].lastrunnode==NULL");
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;

						}
						stack[pdata[temp - lWidth - 3]].lastrunnode->next = current;
						stack[pdata[temp - lWidth - 3]].lastrunnode = current;
						pdata[temp] = stack[pdata[temp - lWidth - 3]].value;
					}//���Ͻ���

					else if (pdata[temp - lWidth - 2] != 255)//��������
					{
						preceding = current;
						current = new runnode;
						if (current == NULL)
						{
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						if (headrun[i] == NULL)
						{
							headrun[i] = current;
							preceding = current;
						}
						else
							preceding->linenext = current;
						current->y = i;
						current->beginx = j;
						current->endx = j;
						current->linenext = NULL;
						current->next = NULL;
						if (stack[pdata[temp - lWidth - 2]].lastrunnode == NULL)
						{
							printf("stack[pdata[temp-lWidth-2]].lastrunnode==NULL");

							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						if (stack[pdata[temp - lWidth - 2]].lastrunnode->next == NULL)
							stack[pdata[temp - lWidth - 2]].lastrunnode->next = current;
						stack[pdata[temp - lWidth - 2]].lastrunnode = current;
						pdata[temp] = stack[pdata[temp - lWidth - 2]].value;
					}//���Ͻ���

					else if (pdata[temp - lWidth - 1] != 255)//����
					{
						preceding = current;
						current = new runnode;
						if (current == NULL)
						{
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						if (headrun[i] == NULL)
						{
							headrun[i] = current;
							preceding = current;
						}
						else
							preceding->linenext = current;
						current->y = i;
						current->beginx = j;
						current->endx = j;
						current->linenext = NULL;
						current->next = NULL;
						if (stack[pdata[temp - lWidth - 1]].lastrunnode == NULL)
						{
							printf("stack[pdata[temp-lWidth-1]].lastrunnode==NULL");
							ReleaseList(headrun, lHeight + 2);///zhoulijun add
							free(pdata);
							free(headrun);
							return 0;
						}
						stack[pdata[temp - lWidth - 1]].lastrunnode->next = current;
						stack[pdata[temp - lWidth - 1]].lastrunnode = current;
						pdata[temp] = stack[pdata[temp - lWidth - 1]].value;
					}//���Ͻ���
				}//end for (***if((pdata[temp-1]+pdata[temp-lWidth-1]+pdata[temp-lWidth-2]+pdata[temp-lWidth-3])!=255*4)***)
				else//��ʼһ���µ���ͨԪ
				{
					preceding = current;
					current = new runnode;
					if (current == NULL)
					{
						ReleaseList(headrun, lHeight + 2);///zhoulijun add
						free(pdata);
						free(headrun);
						return 0;
					}
					if (headrun[i] == NULL)
					{
						headrun[i] = current;
						preceding = current;
					}
					else
						preceding->linenext = current;
					current->beginx = j;
					current->endx = j;
					current->y = i;
					current->linenext = NULL;
					current->next = NULL;
					pdata[temp] = count;
					count++;
					if (count > STACKSIZE - 1)
					{
						i = lHeight + 2;
						j = lWidth + 2;
					}
					if (count % 255 == 0)
						count++;
					stack[pdata[temp]].compshape.minx = j;
					stack[pdata[temp]].firstrunnode = current;
					stack[pdata[temp]].value = pdata[temp];
					stack[pdata[temp]].lastrunnode = current;
				}
			}// end for (**if(pdata[temp]!=255)//��ǰ����Ϊǰ������**)
		}//end for (**for(j=1;j<lWidth+1;j++)**)
	}//end for (**for(i=1;i<lHeight+1;i++)**)
	//��ʼ����ͨԪ��ǩ����***********************

	//�ҳ�ÿ����ͨԪ���������ұ߽�
	LONG minx, maxx, miny, maxy;
	for (i = 1; i < count; i++)
	{
		minx = lWidth;
		maxx = 0;
		miny = lHeight;
		maxy = 0;
		current = stack[i].firstrunnode;
		while (current != NULL)
		{
			if (current->beginx < minx) minx = current->beginx;
			if (current->endx > maxx) maxx = current->endx;
			if (current->y < miny) miny = current->y;
			if (current->y > maxy) maxy = current->y;
			stack[i].pixelnum = stack[i].pixelnum + current->endx - current->beginx + 1;
			current = current->next;
		}
		stack[i].compshape.minx = minx;
		stack[i].compshape.maxx = maxx;
		stack[i].compshape.miny = miny;
		stack[i].compshape.maxy = maxy;
		stack[i].sign = FALSE;
	}
	//---��ͨԪ��ǩ��ǽ���-----------

	//---�ϲ���ǩΪ��ͨԪ-------------

	int k;
	long index, nextindex;
	for (i = lHeight; i > 1; i--)
	{
		if (headrun[i] != NULL && headrun[i - 1] != NULL)
		{
			current = headrun[i];
			nextcurrent = headrun[i - 1];
			while (current != NULL && nextcurrent != NULL)
			{

				if ((current->beginx) >= (nextcurrent->endx + 2) || (current->endx) <= (nextcurrent->beginx - 2))
				{
					if (current->beginx >= nextcurrent->endx + 2)
						nextcurrent = nextcurrent->linenext;
					else current = current->linenext;
				}

				else if (current->beginx + 1 <= nextcurrent->beginx)
				{
					temp = current->y * (lWidth + 2) + current->beginx;
					nexttemp = nextcurrent->y * (lWidth + 2) + nextcurrent->beginx;
					if (stack[pdata[temp]].sign == FALSE && stack[pdata[nexttemp]].sign == FALSE)
					{
						if (stack[pdata[temp]].value != stack[pdata[nexttemp]].value)
						{
							index = pdata[temp];//index��ʾ��ǰ�γ��е�����������ͨԪ�ĸ�������
							nextindex = pdata[nexttemp];//nextindex��ʾ��һ�е�ǰ�γ��е�����������ͨԪ�ĸ�������
							stack[nextindex].value = index;//��ʼ������һ���е��γ����ڵ�component������ǰ�γ����ڵ�component
							stack[nextindex].sign = TRUE;
							stack[index].pixelnum = stack[index].pixelnum + stack[nextindex].pixelnum;
							if (stack[index].compshape.minx > stack[nextindex].compshape.minx)
								stack[index].compshape.minx = stack[nextindex].compshape.minx;
							if (stack[index].compshape.maxx < stack[nextindex].compshape.maxx)
								stack[index].compshape.maxx = stack[nextindex].compshape.maxx;
							if (stack[index].compshape.miny > stack[nextindex].compshape.miny)
								stack[index].compshape.miny = stack[nextindex].compshape.miny;
							if (stack[index].compshape.maxy < stack[nextindex].compshape.maxy)
								stack[index].compshape.maxy = stack[nextindex].compshape.maxy;//����
						}
					}
					else if (stack[pdata[temp]].sign == FALSE && stack[pdata[nexttemp]].sign == TRUE)
					{
						index = pdata[temp];
						nextindex = pdata[nexttemp];
						while (stack[nextindex].sign == TRUE)
						{
							nextindex = stack[nextindex].value;
						}
						if (stack[nextindex].value != index)
						{
							stack[nextindex].value = index;//��ʼ����һ���е��γ����ڵ�component������ǰ�γ����ڵ�component
							stack[nextindex].sign = TRUE;
							stack[index].pixelnum = stack[index].pixelnum + stack[nextindex].pixelnum;
							if (stack[index].compshape.minx > stack[nextindex].compshape.minx)
								stack[index].compshape.minx = stack[nextindex].compshape.minx;
							if (stack[index].compshape.maxx < stack[nextindex].compshape.maxx)
								stack[index].compshape.maxx = stack[nextindex].compshape.maxx;
							if (stack[index].compshape.miny > stack[nextindex].compshape.miny)
								stack[index].compshape.miny = stack[nextindex].compshape.miny;
							if (stack[index].compshape.maxy < stack[nextindex].compshape.maxy)
								stack[index].compshape.maxy = stack[nextindex].compshape.maxy;
						}//����
					}
					else if (stack[pdata[temp]].sign == TRUE && stack[pdata[nexttemp]].sign == FALSE)
					{
						index = pdata[temp];
						while (stack[index].sign == TRUE)
						{
							index = stack[index].value;
						}
						nextindex = pdata[nexttemp];
						if (stack[nextindex].value != index)
						{
							stack[nextindex].value = index;//��ʼ������һ���е��γ����ڵ�component������ǰ�γ����ڵ�component
							stack[nextindex].sign = TRUE;
							stack[index].pixelnum = stack[index].pixelnum + stack[nextindex].pixelnum;
							if (stack[index].compshape.minx > stack[nextindex].compshape.minx)
								stack[index].compshape.minx = stack[nextindex].compshape.minx;
							if (stack[index].compshape.maxx < stack[nextindex].compshape.maxx)
								stack[index].compshape.maxx = stack[nextindex].compshape.maxx;
							if (stack[index].compshape.miny > stack[nextindex].compshape.miny)
								stack[index].compshape.miny = stack[nextindex].compshape.miny;
							if (stack[index].compshape.maxy < stack[nextindex].compshape.maxy)
								stack[index].compshape.maxy = stack[nextindex].compshape.maxy;//����
						}
					}
					else
					{
						if (stack[pdata[temp]].value != stack[pdata[nexttemp]].value)
						{
							index = pdata[temp];
							while (stack[index].sign == TRUE)
							{
								index = stack[index].value;
							}
							nextindex = pdata[nexttemp];
							while (stack[nextindex].sign == TRUE)
							{
								nextindex = stack[nextindex].value;
							}
							if (stack[nextindex].value != index)
							{
								stack[nextindex].value = index;//��ʼ����һ���е��γ����ڵ�component������ǰ�γ����ڵ�component
								stack[nextindex].sign = TRUE;
								stack[index].pixelnum = stack[index].pixelnum + stack[nextindex].pixelnum;
								if (stack[index].compshape.minx > stack[nextindex].compshape.minx)
									stack[index].compshape.minx = stack[nextindex].compshape.minx;
								if (stack[index].compshape.maxx < stack[nextindex].compshape.maxx)
									stack[index].compshape.maxx = stack[nextindex].compshape.maxx;
								if (stack[index].compshape.miny > stack[nextindex].compshape.miny)
									stack[index].compshape.miny = stack[nextindex].compshape.miny;
								if (stack[index].compshape.maxy < stack[nextindex].compshape.maxy)
									stack[index].compshape.maxy = stack[nextindex].compshape.maxy;
							}
						}//����
					}


					if (current->endx <= nextcurrent->endx)
					{
						current = current->linenext;
						//	nextcurrent=nextcurrent->linenext;
					}
					else nextcurrent = nextcurrent->linenext;
				}//end for (**else if(current->beginx+1<=nextcurrent->beginx)**)

				else if (current->beginx >= nextcurrent->beginx - 1)//��ǩ���ʱ�Ѿ����ǵ�
				{
					if (current->endx <= nextcurrent->endx)
					{
						current = current->linenext;
						//	nextcurrent=nextcurrent->linenext;
					}
					else
					{
						nextcurrent = nextcurrent->linenext;
					}

				}
			}//end for (**while(current!=NULL&&nextcurrent!=NULL)**)

		}//end for (**if(headrun[i]!=NULL&&headrun[i-1]!=NULL)**)
	}// ��ͨԪ�궨����



	//�˳������Լ�������̫�ٵ���ͨԪ------------------------------------------------------
	LONG SmallThresh;
	SmallThresh = 100;

	long precdeletenum;//�˳�����ǰ����ͨԪ����
	for (i = 1, k = 0; i < count; i++)//�Ѹ���ͨԪ�ĸ���������ȡ���������pstack�����У��Լ��ٺ�����ʣ�������ʱ��
	{
		if (i % 255 == 0)
			stack[i].sign = TRUE;
		if (stack[i].sign == FALSE)
		{
			if (stack[i].compshape.maxy - stack[i].compshape.miny <= 1 || stack[i].compshape.maxx - stack[i].compshape.minx < 2)
			{
				stack[i].sign = TRUE;
				stack[i].value = 255;
			}
			else
				k++;
		}
	}
	int Map[1000];//value��precdeletenum��ӳ��
	memset(Map, -1, sizeof(Map));
	for (i = 1, precdeletenum = 0; i < count; i++)//�Ѹ���ͨԪ�ĸ���������ȡ���������pstack�����У��Լ��ٺ�����ʣ�������ʱ��
	{
		if (stack[i].sign == FALSE && stack[i].pixelnum > SmallThresh)
		{
			Map[stack[i].value] = precdeletenum;
			rescomponent[precdeletenum].sign = FALSE;
			rescomponent[precdeletenum].pixelnum = stack[i].pixelnum;
			rescomponent[precdeletenum].compshape.maxx = stack[i].compshape.maxx - 1;
			rescomponent[precdeletenum].compshape.minx = stack[i].compshape.minx - 1;
			rescomponent[precdeletenum].compshape.maxy = stack[i].compshape.maxy - 1;
			rescomponent[precdeletenum].compshape.miny = stack[i].compshape.miny - 1;
			rescomponent[precdeletenum].value = stack[i].value;
		//	printf("i%d, value:%d\n", precdeletenum, stack[i].value);
		//	if(stack[i].value==283)
		//	printf("rescompont\n");
			precdeletenum++;
		}
		if (precdeletenum > max_num - 1)
			break;
	}
	if (lHeight != 400)
	{
		//��ͼ���е�ǰ�����ظ�ֵΪ��������ͨԪ�ı�־ֵ������������Ӿ����е���������
		runnode* p;
		for (i = 1; i < count; i++)
		{
			p = stack[i].firstrunnode;
			while (p != NULL)
			{
				for (j = p->beginx; j <= p->endx; j++)
				{
					temp = p->y * (lWidth + 2) + j;
					index = pdata[temp];
					while (stack[index].sign == TRUE && index != 255)
					{
						index = stack[index].value;
					}
					if (index == 255) pdata[temp] = 255;
					else
						pdata[temp] = index;
		//			if (pdata[temp] == 283)printf("w");


				}
				p = p->next;
			}
		}
		LONG anothertemp;
		for (i = 1; i < lHeight + 1; i++)
		{
			for (j = 1; j < lWidth + 1; j++)
			{
				temp = lLineBytes * (i - 1) + j - 1;
				anothertemp = i * (lWidth + 2) + j;
				if (pdata[anothertemp] % 255 != 0 && pdata[anothertemp] != 0)
					pdibData[temp] = pdata[anothertemp];
				else
					pdibData[temp] = pdata[anothertemp];
			}
		}
	}
	//ZZQ��
	//��ÿ����ͨԪ�ı߽��ҳ���������͹��
	for (int y = 0; y < lHeight; y++)
		for (int x = 0; x < lWidth; x++)
		{
			int tmp = y * lWidth + x;//�õ��������������е�λ��

			if (pdibData[tmp] == 0 || pdibData[tmp] == 255)continue;
			int id = Map[pdibData[tmp]];
			if (id != -1)rescomponent[id].P.push_back(Point(x, y));
#ifdef _ZZQDebug__
			printf("%d",pdibData[tmp]);
			if (x == lWidth - 1)printf("\n");
			else printf(" ");
#endif // _ZZQDebug__

		}

	for (int i = 0; i < precdeletenum; i++)
	{
		//cout << "pixelnum:"<<rescomponent[i].pixelnum << endl;
		//cout << "value:"<<rescomponent[i].value << endl;
		//cout << "sign"<<rescomponent[i].sign << endl;
		//cout << "num" << rescomponent[i].P.size() << endl;
			
		//rescomponent[i].R = rescomponent[i].P;
		Convex(rescomponent[i].P, rescomponent[i].R);
		std::vector<Point> t;
		RC(rescomponent[i].R, t);//�����С��Ӿ���
		rescomponent[i].R = t;
	}

	//ZZQ�ӽ���
	ReleaseList(headrun, lHeight + 2);///�ͷ��γ̱�
	free(pdata);
	free(headrun);
	return precdeletenum;
}

int Cal_Ang(BYTE* pImage, int iWidth, int iHeight, int iTop, int iBottom, int iLeft, int iRight, int iValue)
{
	int i, j, k;
	int xx, yy;//��������ƽ��ֵ
	int M20, M02, M11;
	int iAng;
	double dAng;
	xx = 0;
	yy = 0;
	k = 0;
	for (i = iTop; i < iBottom + 1; i++)
	{
		for (j = iLeft; j < iRight + 1; j++)
		{
			if (pImage[i * iWidth + j] == iValue)
			{
				xx = xx + j;
				yy = yy + i;
				k++;
			}
		}
	}
	if (k > 0)
	{
		xx = xx / k;
		yy = yy / k;
	}
	else
		return -400;
	M20 = 0;
	M02 = 0;
	M11 = 0;
	for (i = iTop; i < iBottom + 1; i++)
	{
		for (j = iLeft; j < iRight + 1; j++)
		{
			if (pImage[i * iWidth + j] == iValue)
			{
				M20 = M20 + (j - xx) * (j - xx);
				M02 = M02 + (i - yy) * (i - yy);
				M11 = M11 + (j - xx) * (i - yy);
			}
		}
	}
	if (M20 - M02 == 0)
	{
		if (M11 > 0)
			iAng = 90;
		else
			iAng = 270;
	}
	else
	{
		dAng = (atan((double)2 * M11 / (M20 - M02))) / 2;
		iAng = dAng / 3.1415 * 180;
	}

	if (iBottom - iTop >= iRight - iLeft)
	{
		if (iAng >= 0 && iAng < 45)
			iAng = -(90 - iAng);
		else if (iAng<0 && iAng>-45)
			iAng = 90 + iAng;
	}

	if (iAng < 0)
		iAng = 180 + iAng;

	return iAng;


}
//��ͨԪ����������
void Comp_Order(CodeComponent* rescomponent, int Comp_num)
{
	int j;
	double* comp_minx;
	int* sn;
	CodeComponent rescomponent_temp[200];
	comp_minx = (double*)malloc(Comp_num * sizeof(double));
	if (comp_minx == NULL)
		return;
	sn = (int*)malloc(Comp_num * sizeof(int));
	if (sn == NULL)
	{
		free(comp_minx);
		return;
	}
	for (j = 0; j < Comp_num; j++)
	{
		comp_minx[j] = (double)rescomponent[j].compshape.minx;
		sn[j] = j;
	}
	Quick_Order(comp_minx, 0, Comp_num - 1, sn);
	for (j = 0; j < Comp_num; j++)
	{
		rescomponent_temp[j].sign = rescomponent[sn[j]].sign;
		rescomponent_temp[j].pixelnum = rescomponent[sn[j]].pixelnum;
		rescomponent_temp[j].value = rescomponent[sn[j]].value;
		rescomponent_temp[j].compshape.minx = rescomponent[sn[j]].compshape.minx;
		rescomponent_temp[j].compshape.maxx = rescomponent[sn[j]].compshape.maxx;
		rescomponent_temp[j].compshape.miny = rescomponent[sn[j]].compshape.miny;
		rescomponent_temp[j].compshape.maxy = rescomponent[sn[j]].compshape.maxy;
	}
	for (j = 0; j < Comp_num; j++)
	{
		rescomponent[j].sign = rescomponent_temp[j].sign;
		rescomponent[j].pixelnum = rescomponent_temp[j].pixelnum;
		rescomponent[j].value = rescomponent_temp[j].value;
		rescomponent[j].compshape.minx = rescomponent_temp[j].compshape.minx;
		rescomponent[j].compshape.maxx = rescomponent_temp[j].compshape.maxx;
		rescomponent[j].compshape.miny = rescomponent_temp[j].compshape.miny;
		rescomponent[j].compshape.maxy = rescomponent_temp[j].compshape.maxy;
	}
	free(comp_minx);
	free(sn);
}
//��������
void Quick_Order(double* aaa, int left, int right, int* sn)
{
	int pivotpos = Partition(aaa, left, right, sn);
	if (left < pivotpos - 1)
		Quick_Order(aaa, left, pivotpos - 1, sn);
	if (pivotpos + 1 < right)
		Quick_Order(aaa, pivotpos + 1, right, sn);
}
int Partition(double* aaa, int low, int high, int* sn)
{
	int i, k;
	double temp;
	int pivotpos = low;
	double pivot = aaa[low];
	for (i = low + 1; i <= high; i++)
	{
		if (aaa[i] < pivot)
		{
			pivotpos++;
			if (pivotpos != i)
			{
				temp = aaa[pivotpos];
				aaa[pivotpos] = aaa[i];
				aaa[i] = temp;
				k = sn[pivotpos];
				sn[pivotpos] = sn[i];
				sn[i] = k;
			}
		}
	}
	temp = aaa[pivotpos];
	aaa[pivotpos] = aaa[low];
	aaa[low] = temp;
	k = sn[pivotpos];
	sn[pivotpos] = sn[low];
	sn[low] = k;
	return pivotpos;
}
//int Cal_Round(BYTE* pImage, int iWidth, int iHeight, int iTop, int iBottom, int iLeft, int iRight, CPoint Round[8], int iValue)
//{
//	int i, j;
//	for (i = iTop; i < iBottom + 1; i++)//����
//	{
//		if (pImage[i * iWidth + iLeft] == iValue)
//		{
//			Round[0].x = iLeft;
//			Round[0].y = i;
//			break;
//		}
//	}
//	if (i > iBottom)
//		return 0;
//	for (j = iLeft; j < iRight + 1; j++)//����
//	{
//		if (pImage[iTop * iWidth + j] == iValue)
//		{
//			Round[1].x = j;
//			Round[1].y = iTop;
//			break;
//		}
//	}
//	if (j > iRight)
//		return -1;
//	for (j = iRight; j > iLeft - 1; j--)//����
//	{
//		if (pImage[iTop * iWidth + j] == iValue)
//		{
//			Round[2].x = j;
//			Round[2].y = iTop;
//			break;
//		}
//	}
//	if (j < iLeft)
//		return -2;
//	for (i = iTop; i < iBottom + 1; i++)//����
//	{
//		if (pImage[i * iWidth + iRight] == iValue)
//		{
//			Round[3].x = iRight;
//			Round[3].y = i;
//			break;
//		}
//	}
//	if (i > iBottom)
//		return -3;
//	for (i = iBottom; i > iTop - 1; i--)//����
//	{
//		if (pImage[i * iWidth + iRight] == iValue)
//		{
//			Round[4].x = iRight;
//			Round[4].y = i;
//			break;
//		}
//	}
//	if (i < iTop)
//		return -4;
//	for (j = iRight; j > iLeft - 1; j--)//BottomRight
//	{
//		if (pImage[iBottom * iWidth + j] == iValue)
//		{
//			Round[5].x = j;
//			Round[5].y = iBottom;
//			break;
//		}
//	}
//	if (j < iLeft)
//		return -5;
//	for (j = iLeft; j < iRight + 1; j++)//BottomLeft
//	{
//		if (pImage[iBottom * iWidth + j] == iValue)
//		{
//			Round[6].x = j;
//			Round[6].y = iBottom;
//			break;
//		}
//	}
//	if (j > iRight)
//		return -6;
//	for (i = iBottom; i > iTop - 1; i--)//LeftBottom
//	{
//		if (pImage[i * iWidth + iLeft] == iValue)
//		{
//			Round[7].x = iLeft;
//			Round[7].y = i;
//			break;
//		}
//	}
//	if (i < iTop)
//		return -7;
//	return 1;
//
//
//}
//����ͼ��ĻҶȷ�Χ
void Gray_Area(BYTE* proImage, int lWidth, int lHeight, int& gray1, int& gray2)
{
	int i, j;
	int gray[256];
	double probability[256];
	for (i = 0; i < 256; i++)
		gray[i] = 0;
	//ֱ��ͼ
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			gray[proImage[i * lWidth + j]]++;
		}
	}
	for (i = 0; i < 256; i++)
	{
		probability[i] = (double)gray[i] / (lWidth * lHeight);
	}
	double m;
	i = 0; m = 0;
	while (m < 0.0001)
	{
		m = m + probability[i];
		i++;
	}
	gray1 = i - 1;
	i = 255; m = 0;
	while (m < 0.0001)
	{
		m = m + probability[i];
		i--;
	}
	gray2 = i + 1;

}
//ȫ����ֵ��ֲ���ֵ��ӺϵĶ�ֵ������
void Otsu_Bernsen(BYTE* proImage, int lWidth, int lHeight, int iValue)
{
	int i, j, ii, jj;
	BYTE Temp;
	BYTE* image;
	image = (BYTE*)malloc(lWidth * lHeight * sizeof(BYTE));
	if (image == NULL)
		return;
	memset(image, 255, lWidth * lHeight);
	//ȫ����ֵ
	int T_Otsu = iValue;
	//ͼ��ĻҶȷ�Χ
	int gray1, gray2;
	gray1 = 0; gray2 = 255;
	Gray_Area(proImage, lWidth, lHeight, gray1, gray2);
	//��ֵ��
	for (i = 0; i < lHeight; i++)
	{
		if (i<2 || i>lHeight - 3)
		{
			for (j = 0; j < lWidth; j++)
			{
				if (proImage[i * lWidth + j] < iValue)
					image[i * lWidth + j] = 0;
			}
		}
	}
	for (j = 0; j < lWidth; j++)
	{
		if (j<2 || j>lWidth - 3)
		{
			for (i = 0; i < lHeight; i++)
			{
				if (proImage[i * lWidth + j] < iValue)
					image[i * lWidth + j] = 0;

			}
		}
	}

	int max;
	int min;
	int T_Bernsen = 1;
	double belte = 0;
	belte = 0.15 * (gray2 - gray1 + 1) / 128;
	for (i = 2; i < lHeight - 2; i++)
	{
		for (j = 2; j < lWidth - 2; j++)
		{
			max = 0;
			min = 255;
			for (ii = i - 2; ii < i + 3; ii++)
			{
				for (jj = j - 2; jj < j + 3; jj++)
				{
					if (proImage[ii * lWidth + jj] > max)
						max = proImage[ii * lWidth + jj];
					if (proImage[ii * lWidth + jj] < min)
						min = proImage[ii * lWidth + jj];
				}
			}
			T_Bernsen = (min + max) / 2;
			Temp = proImage[i * lWidth + j];
			if (Temp < (1.0 - belte) * T_Otsu)
				image[i * lWidth + j] = 0;
			else if (Temp < T_Bernsen && Temp >= (1 - belte) * T_Otsu && Temp <= (1 + belte) * T_Otsu)
				image[i * lWidth + j] = 0;

		}
	}


	memcpy(proImage, image, lWidth * lHeight);
	free(image);

}