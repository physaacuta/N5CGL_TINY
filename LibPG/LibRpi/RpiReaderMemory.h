// *********************************************************************************
//	メモリ上にあるRPIファイルを読み込むクラス。（参考実装）
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[メモ]
//
// *********************************************************************************
#pragma once
#include "RpiReader.h"
namespace RpiLib
{

// メモリにあるRPIを読み込むクラス。
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