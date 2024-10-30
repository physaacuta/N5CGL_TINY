// *********************************************************************************
//	��������ɂ���RPI�t�@�C����ǂݍ��ރN���X�B�i�Q�l�����j
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[����]
//
// *********************************************************************************
#pragma once
#include "RpiReader.h"
namespace RpiLib
{

// �������ɂ���RPI��ǂݍ��ރN���X�B
class RpiReaderMemory : public RpiReader
{
    BYTE *m_pRpiMemory;
    int m_nSize;

public:
	RpiReaderMemory() : m_pRpiMemory(0), m_nSize(0)
	{}

	RpiReaderMemory::~RpiReaderMemory(void)
	{}

	bool Open( BYTE *pMemory, int nSize )
	{
        m_pRpiMemory = pMemory;
        m_nSize = nSize;
		return ParseRead();
	}

protected:
    bool ParseRead();
	bool ReadIndexPos();

public:
	class IRpiFrameEnum *CreateSequencialReader();

	friend class RpiMemoryFrameEnum;
};

};