// *********************************************************************************
//	RPIのスレッド別書き込みクラス	[参考実装]
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[メモ]
//		実際の実装や、継承したくなったときはこちら
//
// *********************************************************************************
#pragma once
#include "RpiWriterThread.h"

namespace RpiLib
{

//------------------------------------------
//ファイルオープン指令用構造体
//------------------------------------------
struct RpiWriterOpen  : public RpiWriterRequest
{
	CString filename;	//ファイル名
	int optvalue;		//開き方など、intオプション
	RpiHeader header;//RpiHeaderの初期値

	RpiWriterOpen() : RpiWriterRequest( opOpen ) 	{}
};

//------------------------------------------
//ファイルクローズ指令用構造体
//------------------------------------------
struct RpiWriterClose : public RpiWriterRequest
{
	RpiWriterClose() : RpiWriterRequest( opClose )	{}
};

//------------------------------------------
//フレーム書き込み用構造体
//------------------------------------------
struct RpiWriterFrame : public RpiWriterRequest
{
	//書き込み用のメモリを保持しておく。
	COVData *pFrame;
	int szFrame;
	int szImg;

	RpiWriterFrame() : RpiWriterRequest( opWrite) 
	{
		pFrame = 0;
		szFrame = 0;
		szImg = 0;
	}
	~RpiWriterFrame()
	{
		delete pFrame;
	}
};


//------------------------------------------
//RPIファイルの書き込みクラス。
//コマンド（メソッド）は全部、内蔵のキューを経由して、
//別スレッド(書き込みスレッド)で実行される。
//------------------------------------------
class RpiWriterThread : public ThreadManager
	                  , public IRpiWriterThread
{
protected:


public:

	RpiWriterThread(const char* pThreadName="ProcRPIWriterThread", int nMaxWaitNum = 50 ) 
		: ThreadManager( pThreadName, nMaxWaitNum )
		, m_pWriter(NULL)
	{
	}

	~RpiWriterThread()
	{
		//たとえば、ファイル途中で終わってもいいんだろうか。。。？
		StopSignal();
		WaitForSingleObject(my_hThread, 10000 );
	}


	virtual int addWriteQueue_Or_Delete ( RpiWriterRequest *pCmd );
	virtual int addResultQueue_Or_Delete( RpiWriterResult  *pCmd );

	//キューに積みこむ連中
	virtual int PushOpenFile(  const RpiHeader *srcHeader, const CString &filename, int nOption);
	virtual int PushCloseFile(  );
	virtual int PushWriteFrame( const void *pImage, int szImage, const void *pFrameInf, int szFrameInf );
	virtual int PushWriteFrame( const void *pFrameAll, int szFrame, int  szImage);


	virtual void StartThread(){
		Start();
	}

protected://以下は、スレッド実行分。

	virtual int ThreadFirst();	
	virtual int ThreadLast();
	virtual void ThreadEvent(int nEventNo);

	//キューに積まれたので分岐させる
	virtual void Execute(RpiWriterRequest *p);

	//ファイルを新たに開く。
	virtual bool OpenFile( RpiWriterOpen *p);

	//今開いているファイルを閉じる。
	virtual bool CloseFile(RpiWriterRequest *p = NULL);

	//フレームを書き込む。
	virtual bool WriteFrame(RpiWriterFrame *p);


	//もう書けないみたい
	volatile LONG m_bStopWriting;

	//RPIライタ。
	class TRpiWriter *m_pWriter;

	//ファイル名
	CString m_filename;
};

};