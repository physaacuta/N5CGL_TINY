// *********************************************************************************
//	非同期読み書きヘルパ
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[メモ]
//		WIN32の OVERLAPPED構造体の使いやすいラッパを提供する
//
// *********************************************************************************
#pragma once

//------------------------------------------
//基本クラス
//　読み書き用のメモリ自体は、このクラスでは管理(確保・解放)しない
//  単にコンストラクタで与えられるだけ。
//------------------------------------------
class COVData : private OVERLAPPED
{
public:
	BYTE* pBuffer;
	int   szCapacity;

	//OffsetとOffsetHighに泣き別れているのに対処
	int64_t &offset() { return *reinterpret_cast<int64_t *>(&this->Offset); }
	HANDLE GetEvent() { return hEvent; }



	//------------------------------------------
	//コンストラクタ。
	// abuffer, acapacity : バッファメモリとそのサイズ
	// bCreateEvent       : 待機イベント作成するか(IOCPならイベントは要らない)
	//------------------------------------------
	COVData(BYTE* abuffer = NULL, int acapacity = 0,  bool bCreateEvent = true)
		:pBuffer(abuffer), szCapacity(acapacity)
	{
		if ( bCreateEvent)
			hEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
		else
			hEvent = NULL;
	}


	//------------------------------------------
	//デストラクタ。
	//------------------------------------------
	virtual ~COVData()
	{
		if( 0 != hEvent ){
			CloseHandle( hEvent );
		}
	}

	//------------------------------------------
	//バッファをチェックする
	//	const BYTE *ptr, DWORD size操作対象にするメモリの範囲
	//戻り値　両方成立した時、true
	//・セクタアラインメント要求を満たしているか
	//・バッファ範囲内か。
	//------------------------------------------
	bool check_alignment(const BYTE *ptr, DWORD size) 
	{
		ASSERT( 0 == (int64_t(ptr) & 0x1ff) );
		ASSERT( int(&ptr[size] - pBuffer) <= szCapacity);
		return true;
	}

	//------------------------------------------
	//読み込み開始。（非同期）
	//　ファイルhFileのaoffset位置からからsizeバイトデータを読み込みはじめる。
	//  bufstartpos 読み込む先のバッファ位置
	//------------------------------------------
	bool IssureRead( HANDLE hFile, int64_t aoffset,  DWORD size, int bufstartpos = 0 ){
		check_alignment(  &pBuffer[bufstartpos], size ) ;
		this->offset() = aoffset;
		int r = ReadFile( hFile, &pBuffer[bufstartpos], size, NULL, this );
		if( r ) 
			return true;
		if( 0 == r && GetLastError() == ERROR_IO_PENDING )
			return true;
		return false;

	}
	
	//------------------------------------------
	//書き込み開始。（非同期）
	//　ファイルhFileのaoffset位置からからsizeバイトデータを空き込みはじめる。
	//  bufstartpos　書き込みに使用するバッファの位置
	//------------------------------------------
	bool IssureWrite( HANDLE hFile, int64_t aoffset, DWORD size, int bufstartpos = 0 ){
		check_alignment(  &pBuffer[bufstartpos], size) ;
		this->offset() = aoffset;
		int r = WriteFile( hFile, &pBuffer[bufstartpos], size, NULL, this );
		if( r ) 
			return true;
		if( 0 == r && GetLastError() == ERROR_IO_PENDING )
			return true;
		return false;
	}

	//------------------------------------------
	//非同期結果を取得
	//戻り値　書き込みor読み込みバイト数。０は異常。
	//------------------------------------------
	int WaitResult( HANDLE hFile )
	{
		DWORD read = 0;
		if (GetOverlappedResult( hFile, this, &read, TRUE ))
			return read;
		return 0;
	}
};

//------------------------------------------
//バッファをVirtualAllocで確保し、所有するクラス。
//＊FILE_FLAG_NO_BUFFERINGでオープンすると、バッファポインタにセクタアラインメントの要請がつく＊
//そういう用途に。オブジェクト破棄でバッファは解放
//------------------------------------------
class COVDataVMAlloc : public COVData
{
public:
	static int PageAlignOf( int sz){ return (sz+4095) & 0x7ffff000; }

	COVDataVMAlloc(int buszFrame, bool createEvent = true)
		:COVData( NULL,  buszFrame, createEvent )
	{
		pBuffer  = (BYTE*)VirtualAlloc( NULL, buszFrame, MEM_COMMIT, PAGE_READWRITE);
	}
	~COVDataVMAlloc()
	{
		if( pBuffer ){
			VirtualFree( pBuffer, 0, MEM_RELEASE);
			pBuffer = 0;
		}
	}
};
