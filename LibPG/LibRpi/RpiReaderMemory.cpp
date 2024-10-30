// RPI 読込クラスの実装
//  メモリにあるRPIを読み込むクラス。
#include "stdafx.h"
#include "rpi.h"
#include "RpiReaderMemory.h"

namespace RpiLib{

//------------------------------------------
//与えられたメモリがRPIかどうか、ヘッダを読んでみる
//------------------------------------------
bool RpiReaderMemory::ParseRead()
{
    const int nFirstReadSize = 8192;
	if( m_nSize < nFirstReadSize ){
		m_sErrorMsg.Format("RPIファイルなら、長さが%dバイトくらいは欲しい. nLen = %d", nFirstReadSize, nFirstReadSize);
		return false;
	}

	//RPIヘッダ読み込み。
	bool bRet = Parse( m_pRpiMemory, nFirstReadSize );
	if( !bRet ){
		m_sErrorMsg.Format("RPIのヘッダがおかしい. type=%x, len=%d", Header.type, Header.header_size );
		return false;
	}

    return ReadIndexPos();
}

//------------------------------------------
//メモリを手繰って、フレーム情報を作成する
//------------------------------------------
bool RpiReaderMemory::ReadIndexPos()
{
    for( int nFrame = 1; nFrame < Header.nframe; nFrame++ )
    {
        int64_t nFramePos = LinkArray[nFrame].nPosInFile;
        if( 32 < nFramePos  && nFramePos < m_nSize ){
            const BYTE *pBuffer =  &m_pRpiMemory [ nFramePos  ];
            LinkArray.UpdateNextPosOnRead( nFrame, pBuffer );
        }
        else{
            m_sErrorMsg.Format("RPIをたぐっていったが、途中で切れた。frame=%d, %lld", nFrame,  nFramePos);
            return false;
        }
	}
    return true;
}







//------------------------------------------
//メモリからフレームを読み、返すクラス。
//------------------------------------------
class RpiMemoryFrameEnum : public IRpiFrameEnum
{
	RpiReaderMemory *that;

public:

	RpiMemoryFrameEnum(RpiReaderMemory *athat)
		:that(athat)

	{

	}


protected:
	//イマイチ.
    bool ReadToInternalBuffer( int nFrame, bool bRead );
};

//------------------------------------------
// 開いているRpiファイルの、nFrame目を読みだし、メモリに格納する。
// int nFrame	対象フレーム
// bool bRead	True：メモリに本体を読む。False:リンクだけ読んで次へすすむ。
//------------------------------------------
bool RpiMemoryFrameEnum::ReadToInternalBuffer( int nFrame, bool bRead)
{
	if( 1 <= nFrame && nFrame < that->LinkArray.size() );
	else	{
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d はおかしい。 1~%dの範囲", nFrame, that->LinkArray.size());
		return false;
	}
	

	const RPIFrameLink &link = that->LinkArray[nFrame-1];

	if(link.nPosInFile <= 32 ){
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d は位置が判らない。", nFrame);
		return false;
	}


    if( link.nPosInFile < that->m_nSize )
    {
        pFrame = &that->m_pRpiMemory[ link.nPosInFile ];
        szImg = link.szImg;
        return true;
    }
    else{
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d はメモリ範囲外。%lld", nFrame, link.nPosInFile);
    }
	return false;
}

//------------------------------------------
//フレーム読み込みクラスを作って返す。
//------------------------------------------
class IRpiFrameEnum *RpiReaderMemory::CreateSequencialReader()
{
	return new RpiMemoryFrameEnum(this);
}
};