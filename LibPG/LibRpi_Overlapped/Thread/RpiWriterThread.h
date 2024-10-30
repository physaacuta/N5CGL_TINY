// *********************************************************************************
//	RPIのスレッド別書き込みクラスへのインターフェース	[参考実装]
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[メモ]
//		使うだけなら、このヘッダだけ相手にしてもOK
//
// *********************************************************************************
#pragma once


namespace RpiLib
{
//キューにたまる、指令用構造体（ベース構造体）
struct RpiWriterRequest{
	enum op_cmd
	{
		opOpen =1,
		opClose=2,
		opWrite=3,
	};
	int cmd;

	RpiWriterRequest(int c)  : cmd(c) {}
	virtual ~RpiWriterRequest()       {}
};


//結果構造体
struct RpiWriterResult{
	enum op_cmd
	{
		opOpened =1,
		opClosed=2,
		opFrameWritten=3,
		opError=4,
	};

	op_cmd cmd;
	CString filename;

	int errorcode;
	CString ErrorDetail;
	
	RpiWriterResult(op_cmd c, const CString file, int nError = 0)
		: cmd(c)
		, filename(file)
		, errorcode(nError )
	{

	}
};


struct IRpiWriterThread
{
	//入力用動作キュー。基本的には、下記pushメソッド連中を使うこと
	ThreadQueue<RpiWriterRequest> mque_WriteQueue ;

	//結果キュー。必ず処理すること
	ThreadQueue<RpiWriterResult> mque_ResultQueue;



	//指令をキューに積む。
	//ファイルオープン
	virtual int PushOpenFile(  const RpiHeader *srcHeader,  const CString &filename, int nOption) = 0;
	//ファイルクローズ
	virtual int PushCloseFile(  )= 0;
	//フレームの書き込み。画像と、フレーム毎情報が別々のエリアにある場合
	virtual int PushWriteFrame( const void *pImage, int szImage, const void *pFrameInf, int szFrameInf )= 0;
	//フレームの書き込み。画像＋フレーム毎情報で結合済み
	virtual int PushWriteFrame( const void *pFrameAll, int szFrame, int  szImage)= 0;

	//今現在、ファイルを開いているか
	virtual bool IsOpen() const { return m_bOpen; }


	//closeする直前のコールバック。終了間際に確定するヘッダ項目を設定したいときに。
	//ほんとは、EVENT OnBeforeCloseとかしたい。
	//注。 OnAfterCloseは無いので、ResultQueueからopClsoedを拾って処理してね。
	void *pObjCallback;
	int (*OnBeforeClose)( void *pObjCallback,  RpiHeader &Header);

	//デストラクタ
	virtual ~IRpiWriterThread(){};

	//書き込みスレッドの開始
	virtual void StartThread() = 0;

	//インターフェースの実体を作成する
	static IRpiWriterThread * craete();

protected:
	bool m_bOpen;
	IRpiWriterThread() : pObjCallback(NULL), OnBeforeClose(NULL), m_bOpen(false){	}
};
};