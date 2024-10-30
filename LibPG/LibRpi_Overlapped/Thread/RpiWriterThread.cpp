#include <stdafx.h>
#include <stddef.h>
#include "../rpi.h"
#include "../rpiwriter.h"

#include "..\..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"							// 回転寿司形式のスレッドキュー
#include "..\..\..\Library\KizuLibMFC\CLASS\ThreadSyncManager.h"							// 回転寿司形式のスレッドキュー

#include "RpiWriterThread.h"
#include "RpiWriterThreadImpl.h"

namespace RpiLib{

//------------------------------------------
//　書き込みキューに要求構造体を積み込む。
//　RpiWriterRequest *pCmd　積み込む要求構造体
//　戻り値　キューフルのとき、false
//------------------------------------------
int RpiWriterThread::addWriteQueue_Or_Delete( RpiWriterRequest *pCmd )
{
	int nRet = mque_WriteQueue.AddToTail( pCmd  );
	if( nRet ){
		return true;
	}	
	else{
		delete pCmd;
		return false;
	}
}

//------------------------------------------
//　結果キューに結果構造体を積み込む。
//　RpiWriterResult *pCmd　積み込む結果構造体
//　戻り値　キューフルのとき、false
//------------------------------------------
int RpiWriterThread::addResultQueue_Or_Delete( RpiWriterResult *pCmd )
{
	int nRet = mque_ResultQueue.AddToTail( pCmd );
	if( nRet ){
		return true;
	}	
	else{
		delete pCmd;
		return false;
	}
}

//継承メソッド群。

//------------------------------------------
//　書き込みキューにファイルオープン要求を積み込む。
//　 const RpiHeader *srcHeader RPIヘッダ
//　 const CString &filename	ファイル名
//　 int nOption				1:DIRECT
//　戻り値　キューフルのとき、false
//------------------------------------------
int RpiWriterThread::PushOpenFile(  const RpiHeader *srcHeader, const CString &filename, int nOption)
{
	RpiWriterOpen *pCmd = new RpiWriterOpen();
	pCmd->filename = filename;
	pCmd->header = *srcHeader;
	pCmd->optvalue = nOption;
	if( addWriteQueue_Or_Delete( pCmd ) ) {
		m_bOpen = true;
		return true;
	}
	return false;

}

//------------------------------------------
//　書き込みキューにファイルクローズ要求を積み込む。
//　戻り値　キューフルのとき、false
//------------------------------------------
int RpiWriterThread::PushCloseFile( )
{
	RpiWriterRequest *pCmd = new RpiWriterRequest( RpiWriterRequest::opClose );
	if( addWriteQueue_Or_Delete( pCmd ) ){
		m_bOpen = false;
		return true;
	}
	return false;

}

//------------------------------------------
//　書き込みキューにフレーム書き込み要求を積み込む。
//　 const void *pImage		画像バッファ
//　 int szImage			画像サイズ
//　 const void *pFrameInf　フレーム情報
//　 int szFrameInf			フレーム情報サイズ
//　戻り値　キューフルのとき、false
//　フレームメモリは自前で確保します
//------------------------------------------
int RpiWriterThread::PushWriteFrame( const void *pImage, int szImage, const void *pFrameInf, int szFrameInf )
{
	RpiWriterFrame *pCmd = new RpiWriterFrame();
	pCmd->pFrame = new COVDataVMAlloc( COVDataVMAlloc::PageAlignOf(szImage + szFrameInf ) );
	if( !pCmd->pFrame->pBuffer){
		delete pCmd;
		return false;
	}
	pCmd->szFrame = 12 + szImage + szFrameInf ;
	pCmd->szImg   = szImage;
	memcpy( &pCmd->pFrame->pBuffer[12]         , pImage, szImage );
	memcpy( &pCmd->pFrame->pBuffer[12+szImage] , pFrameInf, szFrameInf );
	return addWriteQueue_Or_Delete( pCmd );
}


//------------------------------------------
//　書き込みキューにフレーム書き込み要求を積み込む。
//　 const void *pFrameAll	フレームバッファ
//　 int szFrame			フレームのサイズ
//　 int szImage			画像サイズ
//　戻り値　キューフルのとき、false
//　フレームメモリは自前で確保します
//------------------------------------------
int RpiWriterThread::PushWriteFrame( const void *pFrameAll, int szFrame, int  szImage)
{
	RpiWriterFrame *pCmd = new RpiWriterFrame();
	pCmd->pFrame = new COVDataVMAlloc( COVDataVMAlloc::PageAlignOf(szFrame) );
	if( !pCmd->pFrame->pBuffer  ){
		delete pCmd;
		return false;
	}
	pCmd->szFrame = 12 + szFrame ;
	pCmd->szImg   = szImage;
	memcpy( &pCmd->pFrame->pBuffer[12]         , pFrameAll, szFrame );
	return addWriteQueue_Or_Delete( pCmd );
}

////// シグナルの条件
enum {	EV_ADD_QUEUE = 0,				// キューイング
		};


int RpiWriterThread::ThreadFirst()	
{
		//// イベント設定
	HANDLE hArray[] = { mque_WriteQueue.GetQueueEvent()
						};

	// IMustDieはオプショナルなので、NULLのときは待たない。
	SetEventNum(sizeof(hArray)/sizeof(HANDLE) , hArray);
	return 0;
}


int RpiWriterThread::ThreadLast()
{
	return 0;
}


void RpiWriterThread::ThreadEvent(int nEventNo)
{

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_QUEUE: 						// キューイング：処理。
		{
			RpiWriterRequest *p = mque_WriteQueue.GetFromHead();
			Execute(p);	
			delete p;
		}
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//書き込みの実施。
void RpiWriterThread::Execute(RpiWriterRequest *p)
{

	switch( p->cmd ){
	case RpiWriterRequest::opOpen :
		OpenFile(static_cast<RpiWriterOpen*>(p) );
		break;

	case RpiWriterRequest::opClose:
		CloseFile();
		break;

	case RpiWriterRequest::opWrite:
		WriteFrame(static_cast<RpiWriterFrame*>(p));
		break;

	default:
		;//

	}
	return ;
}

//------------------------------------------
//　ファイルオープン要求に従い、ファイルを新たに開く。
//------------------------------------------
bool RpiWriterThread::OpenFile(RpiWriterOpen *p)
{
	// 最終自動クローズ
	if( m_pWriter ){
		CloseFile();
	}

	//オープン系の処理
	m_pWriter = new TRpiWriter();
	bool r = m_pWriter->OpenFile( p->filename, p->header, (p->optvalue & 1) ? 1 : 0);
	m_bStopWriting = false;
	m_filename =  p->filename;

	//完了報告
	RpiWriterResult *pRet = new RpiWriterResult( RpiWriterResult::opOpened, m_filename, r);
//			pRet->ErrorDetail = ;
	addResultQueue_Or_Delete( pRet );
	return r;
}

//------------------------------------------
//　ファイルクローズ要求に従い、ファイルを閉じる。
//------------------------------------------
bool RpiWriterThread::CloseFile(RpiWriterRequest *p)
{
	if( m_pWriter )
	{
	//完了前コールバック
		if( OnBeforeClose ){
			//後のCloseFile内部でもセットするが、コールバックが使いやすいようにここでセットしとく。
			m_pWriter->Header.nframe = m_pWriter->GetFrameNum();
			OnBeforeClose( pObjCallback, m_pWriter->Header );
		}
	//後始末
		m_pWriter->CloseFile();
		delete m_pWriter;
		m_pWriter = NULL;

	//完了報告
		RpiWriterResult*pRet = new RpiWriterResult( RpiWriterResult::opClosed, m_filename );
		addResultQueue_Or_Delete( pRet );

		m_filename= "";
		return true;
	}
	else{
		//もう閉じてる
		RpiWriterResult*pRet = new RpiWriterResult( RpiWriterResult::opClosed, m_filename );
		addResultQueue_Or_Delete( pRet );
	}
	return false;
}

//------------------------------------------
//　フレーム書き込み要求に従い、フレームを出力する。
//------------------------------------------
bool RpiWriterThread::WriteFrame(RpiWriterFrame *p)
{
	if( m_pWriter ) 
	{
		if(  m_pWriter->WriteFrameOpt( p->pFrame->pBuffer, p->szImg, NULL) ){
			RpiWriterResult *pRet = new RpiWriterResult( RpiWriterResult::opFrameWritten, m_filename, m_pWriter->GetFrameNum());
			addResultQueue_Or_Delete( pRet );
			return true;
		}
		else{
			m_bStopWriting = true;
			DWORD dwErr  = GetLastError();
			RpiWriterResult *pRet = new RpiWriterResult( RpiWriterResult::opError, m_filename, dwErr );
			pRet->ErrorDetail = "WriteFrame";
			addResultQueue_Or_Delete( pRet );
		}
	}
	return false;
}


//------------------------------------------
//　IRpiWriterThread インスタンスの作成
//------------------------------------------
IRpiWriterThread * IRpiWriterThread::craete()
{
	return new RpiWriterThread();
}

};