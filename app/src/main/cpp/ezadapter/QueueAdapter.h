#ifndef __QUEUE_ADAPTER_INTERNAL_H__
#define __QUEUE_ADAPTER_INTERNAL_H__

#include "ezadapter/QueueAdapter.h"

class  CQueueAdapter:public IQueueAdapter
{
public:
	CQueueAdapter(int objectID=0,Policy policy=qualityPrio,AdapterConfigCB adapterConfigProc=NULL);
	virtual ~CQueueAdapter();
	virtual int TriggerDebug();
	virtual int SetDelayThreshold(int size);
	virtual int UpdateDelaySize(int size);
	virtual int DoFrameCheck(int frameType,int frameSize);	
private:
	//����Ļ�������
	int				m_objectID;	
	Policy			m_policy;	
	AdapterConfigCB  	m_adapterCallBack;	//�ص�����	
	int				m_delayThreshold;	//�趨������ֵ
	int				m_curDelaySize;

	//����״̬
	int				m_allowPassFlag;		//ͨ���Ŀ��ƿ���
	int 				m_upEncoder;
	int 				m_downEncoder;	
	int				m_waitIFrame;
	int				m_keepLastStatus;
	//ͨ������I  ֡���Ч������()
	int				m_gopStatus; //0 ����̬��1����̬
	int 				m_gopUp;			
	int 				m_gopDown;

	//ͳ����Ϣ
	int 				m_totalFramePerGOP;	//ÿһ��I֡����ܵ�֡��	
	int 				m_passFramePerGOP;	//ÿһ��I֡���ͨ����֡��	
	int 				m_avgPassRate;		//����GOP ��������ƽ��
	//ͳ�Ƹ�������
	int				m_curTotalCount;		
	int				m_curPassCount;		

	//������Ϣ���
	int				m_debug;
};

#endif //__QUEUE_ADAPTER_INTERNAL_H__

