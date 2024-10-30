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
	HANDLE m_hFile;
	CString m_sFilename;
	bool m_bDirect;
	bool bIndexAccess;

public:
	RpiReaderFile() : m_hFile(0)
	{};

	~RpiReaderFile(void)
	{	Close();	}

	//ファイルを閉じる
	void Close();


	//ファイルオープン。
	//sFilename：開くRPIファイル。
	//bDirect：FILE_FLAG_NO_BUFFERINGを指定したい場合、True。
	//戻り：成功時true。エラーメッセージは、GetErrorで取れる。
	bool Open( const char *sFilename, bool bDirect )
	{
		m_bDirect = bDirect;
		m_sFilename = sFilename;
		if( !doOpen( bDirect ) ) {
			return false;
		}
		return ParseRead();
	}
	HANDLE handle() { return m_hFile; }

protected:
	bool doOpen( bool bDirect );
	bool ParseRead();
	bool ReadIndexPos(bool bForce = false);

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