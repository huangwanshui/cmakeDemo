#ifndef __QOS_ADAPTER_INTERNAL_H__
#define __QOS_ADAPTER_INTERNAL_H__

#include "ezadapter/QosAdapter.h"

class  CQosAdapter:public IQosAdapter
{
public:
	CQosAdapter(int objectID=0,Policy policy=qualityPrio,AdapterConfigCB adapterConfigProc=NULL);
	virtual ~CQosAdapter();
	virtual int TriggerDebug();
	virtual int SetQosThreshold(int qos);
	virtual int UpdateQos(int qos);
	virtual int DoFrameCheck(int frameType,int frameSize);
private:
	//����Ļ�������	
	int				m_objectID;			
	Policy			m_policy;			
	AdapterConfigCB  	m_adapterCallBack;	//�ص�����		
	int				m_qosThreshold;		//�趨������ֵ	

	//����״̬	
	int				m_allowPassBit;		//һ��GOP����ͨ��������
	int 				m_upStepCnt;
	int 				m_downStepCnt;
	int 				m_upEncoder;
	int 				m_downEncoder;		
	int				m_waitIFrame;
	int				m_keepLastStatus;	
	//ͨ������I  ֡���Ч������(��������������)
	int				m_gopStatus; //0 ����̬��1����̬
	int 				m_gopUp;			
	int 				m_gopDown;
	
	//ͳ����Ϣ
	int 				m_totalFramePerGOP;	//ÿһ��I֡����ܵ�֡��	
	int 				m_passFramePerGOP;	//ÿһ��I֡���ͨ����֡��	
	int 				m_avgPassRate;		//����GOP ��������ƽ��

	//��������
	int				m_curTotalCount;		
	int				m_curPassCount;		
	int				m_curPassBit;		//������������

		
	//������Ϣ���
	int				m_debug;
};

#endif //__QOS_ADAPTER_INTERNAL_H__

