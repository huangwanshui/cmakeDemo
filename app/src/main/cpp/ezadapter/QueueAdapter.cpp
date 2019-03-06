#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "QueueAdapter.h"


CQueueAdapter::CQueueAdapter(int objectID,Policy policy,AdapterConfigCB adapterConfigProc)
{
	m_objectID = objectID;
	m_policy = policy;
	m_adapterCallBack = adapterConfigProc;

	m_delayThreshold = 10*1024*1024; 
	m_allowPassFlag = 1;
	m_upEncoder = 0;	
	m_downEncoder = 0;
	m_waitIFrame = 1;
	m_keepLastStatus = -1;
	m_gopStatus = 0;
	m_gopUp = 0;
	m_gopDown = 0;

	m_totalFramePerGOP = 0;
	m_passFramePerGOP = 0;
	m_avgPassRate = 100;	
	m_curTotalCount = 0;
	m_curPassCount =  0;
	m_debug = 0;
}

CQueueAdapter::~CQueueAdapter()
{
}

int CQueueAdapter::TriggerDebug()
{
	m_debug = !m_debug;	
	printf("TriggerDebug == [%d] \n",m_debug);	
	return 0;	
}

int CQueueAdapter::SetDelayThreshold(int size)
{
	m_delayThreshold = size;
	return 0;
}

int CQueueAdapter::UpdateDelaySize(int size)
{
	if(size>=m_delayThreshold) 
	{
		m_allowPassFlag = 0;
	}
	else
	{
		m_allowPassFlag = 1;
	} 
	m_curDelaySize = size;
	return 0;
}

int CQueueAdapter::DoFrameCheck(int frameType,int frameSize)
{
	if((frameType != 'P')&&(frameType != 'I'))	 
	{
		return 0;		//其他帧全部放行 
	}
	if(frameSize == 0)	//非视频起始包，特殊处理
	{
		//跟随之前的状态,不做统计
		return m_keepLastStatus;
	}
	
	//统计
	if(frameType == 'I')
	{
		m_waitIFrame = 0;
		
		m_totalFramePerGOP = m_curTotalCount;
		m_passFramePerGOP = m_curPassCount;
		m_curTotalCount = 0;
		m_curPassCount = 0;

		int curPassRate = 0;
		if(m_totalFramePerGOP)
		{
			curPassRate = (m_passFramePerGOP*100/m_totalFramePerGOP);
		}	
		m_avgPassRate = (m_avgPassRate*75+curPassRate *25)/100;
		if(m_debug)
		{	
			printf("===================delaySize[%d]=================== \n",m_curDelaySize);
			printf("=========QueueAdapter pass percent curRate[%d] avgRate[%d]===passCnt[%d]totalCnt[%d]======>\n",curPassRate,m_avgPassRate,m_passFramePerGOP,m_totalFramePerGOP);
			printf("policy:%d,upEncoder:%d,downEncoder:%d ,gopStatus=%d,gopUp=%d,gopDown=%d\n",m_policy,m_upEncoder,m_downEncoder,m_gopStatus,m_gopUp,m_gopDown);
		}
		if(m_policy == qualityPrio)
		{
			if(m_avgPassRate>=90)
			{
				m_gopUp++;
				m_gopDown = 0;
			}
			else
			{
				m_gopUp = 0;
				m_gopDown++;
			}
			//调节GOP
			if(m_gopUp>15)
			{
				m_gopUp = 0;
				m_gopDown = 0; 
				if(m_gopStatus == 1)
				{	
					//上调为配置态
					m_gopStatus = 0;					
					if(!m_adapterCallBack.empty())
					{
						AdapterCmdSt cmd;
						cmd.op = qopEncUp;
						m_adapterCallBack(m_objectID,&cmd);
					}
				}
			}
			else if(m_gopDown>5)
			{	
				m_gopUp = 0;
				m_gopDown = 0; 
				if(m_gopStatus == 0)
				{	
					//下调为调节态
					m_gopStatus = 1;
					if(!m_adapterCallBack.empty())
					{						
						AdapterCmdSt cmd;
						cmd.op = gopEncDown;	
						m_adapterCallBack(m_objectID,&cmd);
					}
				}
			}
		}
		if((m_policy == fluencyPrio)||(m_policy == newqualityPrio))
		{
			if(m_avgPassRate>=85)
			{
				m_upEncoder++;
				m_downEncoder = 0;
			}
			else if(m_avgPassRate<=45)
			{
				m_upEncoder = 0;
				m_downEncoder++;
			}
			else
			{
				m_upEncoder = 0;
				m_downEncoder = 0;
			}
			//调节
			if(m_upEncoder>5)
			{
				m_upEncoder = 0;
				m_downEncoder = 0; 
				if(!m_adapterCallBack.empty())
				{
					AdapterCmdSt cmd;
					cmd.op = (m_policy == fluencyPrio)?fluencyEncUp:qualityEncUp;	
					m_adapterCallBack(m_objectID,&cmd);	
				}	
			}
			else if(m_downEncoder>5)
			{
				m_upEncoder = 0;
				m_downEncoder = 0;
				if(!m_adapterCallBack.empty())
				{
					AdapterCmdSt cmd;
					cmd.op = (m_policy == fluencyPrio)?fluencyEncDown:qualityEncDown;	
					m_adapterCallBack(m_objectID,&cmd);
				}
			}
		}
	}	

	m_curTotalCount++;
	if(m_allowPassFlag == 1)
	{
		if((0==m_waitIFrame)
			||(frameType == 'I'))
		{
			m_curPassCount++;
			m_keepLastStatus = 0;
			return 0;	
		}
		else
		{
			m_keepLastStatus = -1;
			m_waitIFrame = 1;
			return -1;
		}
	}
	else
	{
		m_keepLastStatus = -1;
		m_waitIFrame = 1;
		return -1;
	}
}

/////////////////////////////////////////////////////////////
IQueueAdapter* IQueueAdapter::create(int objectID,Policy policy,AdapterConfigCB adapterConfigProc)
{
	return new CQueueAdapter(objectID, policy, adapterConfigProc);
}

