// *********************************************************************************
//	ファイルからRPIファイルを読み込むクラス。
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
class RpiReaderFile : public RpiReader
{
public:
	RpiReaderFile() : m_hFile(0)
	{ InitializeCriticalSection( &m_csFile ); };

	~RpiReaderFile(void)
	{
		Close();
		DeleteCriticalSection( &m_csFile ); 
	}

	//ファイルを閉じる
	void Close();


	//ファイルオープン。
	//sFilename：開くRPIファイル。
	//bDirect：FILE_FLAG_NO_BUFFERINGを指定したい場合、True。
	//戻り：成功時true。エラーメッセージは、GetErrorで取れる。
	bool Open( const char *sFilename)
	{
		m_sFilename = sFilename;
		if( !doOpen() ) {
			return false;
		}
		return ParseRead();
	}
	HANDLE handle() { return m_hFile; }

protected:
	bool doOpen();
	bool ParseRead();
	bool ReadIndexPos(bool bForce = false);
	int  ReadFileAt ( int64_t nPos, void *pMem, int nSize );

	CRITICAL_SECTION m_csFile;			//ファイルハンドルの保護用
	HANDLE m_hFile;

	CString m_sFilename;
	bool bIndexAccess;

public:
	//順次読み込み専用リーダを作って返す。
	//使い終わったらDELETEすること。
	class IRpiFrameEnum *CreateSequencialReader();

	//ランダム読み込み専用リーダを作って返す。
	//使い終わったらDELETEすること。
	class IRpiFrameEnum *CreateRandomReader();

	friend class RpiFrameEnum;
};

};