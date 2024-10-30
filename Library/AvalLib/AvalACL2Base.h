// *********************************************************************************
//	Avalボード接続 基底 クラス
//	[Ver]
//		Ver.01    2014/07/07  vs2005 対応
//
//	[メモ]
//		・Avalへの直接アクセスは、本クラスのみ
//		・FC の AvalACL2Batchに相当
//	
//	エンジンのAvalACL2Batchに相当。コールバックでは無く直接。
//
//   camera --> driver --> ACapLib2 --> my_buff[nbuff]
//         ・my_buffはDMAリングバッファ。勝手に上書きされる。
//				※DMAは、CPUからの直接アクセス
//         ・WaitNFrameReady()で新たに書かれたフレーム数を取得(ポーリングの場合は呼ばなくても良い）
//         ・GetFramePointers()でmy_buffの要素を取得、周回おくれチェック
//         ・CheckOverrun()でコピーの対象となったmy_buffにDMA領域が侵入していないことをチェック
//   StartGrab() -> WaitNFrameReady() -> GetFramePointers() -> CheckOverrun()
//   AbortFrameWait() -> StopGrab()
//
// *********************************************************************************
#pragma once


#include <Windows.h>

#include "..\KizuLibMFC\CLASS\FuncBase.h"											// 

// Aval ヘッダー
	// 必須のヘッダーのみ
	// .c で必要なヘッダーは *.cで定義



namespace AvalLib
{
	//===================================================================
	// 基底クラス
	//===================================================================
	class AvalACL2Base : public FuncBase
	{
	public:

        //  フレーム待ちとLock
        //   pimage: バッファの先頭ポインタ。サイズはGetImageNBytes()。
        //   framenum: 0始まり。StartGrab()する度に0に戻る。
        enum FrameWaitResult { FrameWait_Success = 0,
                               FrameWait_Timeout, FrameWait_Aborted, FrameWait_ACLError,
							   FrameWait_Overrun,
                               FrameWait_Unknown };

	public:
		AvalACL2Base();
		virtual ~AvalACL2Base(void);

		//---------------------------
		// init and dispose
		virtual BOOL Init(int board_id, int board_ch_id, const char* ini_filename, int nbuff);
		virtual void Dispose();

		virtual void SetThreadName(CString sThreadName) { FuncBase::my_sThreadName = sThreadName;}

		//---------------------------
		// image
        inline int GetWidth()     const { return my_nWidth; }
        inline int GetHeight()    const { return my_nHeight; }
        inline int GetPixelNBit() const { return my_nPixelNBit; }
        //
        inline int GetImageNBytes() const { return my_nBufSize; }
		BOOL		reload_size();											// SetInfoでサイズが変わった場合にmy_width等を再設定

		//---------------------------
		// grab
		BOOL				StartGrab();									// 撮像開始
		BOOL				StopGrab();										// 撮像停止
		FrameWaitResult		WaitNFrameReady(DWORD& nFrame, DWORD nTimeout);	// 新規フレーム数の取得 (フレーム入力までブロック)
		FrameWaitResult		GetFramePointers(void** ptrs, DWORD ptrslen, DWORD& nvalid, DWORD& from_framenum);		// ポインタの取得
		BOOL				CheckOverrun(int nReadFrameCount);									// 読み込み遅れチェック
		void				AbortFrameWait();								// フレーム待ち(WaitNFrameReady)状態の解除


		// stat counters
		inline void ClearStatCounters()	{ my_nFrameOverrun = 0; }		// カウント値のクリア
		DWORD	GetNFrameOverrn() const		{ return my_nFrameOverrun; }	


		//---------------------------
		// serial communication
		BOOL OpenSerialPort();										// オープン
		void CloseSerialPort();										// 切断
		BOOL SendToSerialPort(const void* data, int len);			// 送信
		BOOL RecvFromSerialPort(void* buff, int buff_len, int&recv_len, DWORD timeout);		// 受信
		BOOL SetSerialParam(int baud);								// シリアル パラメータ設定

		//---------------------------
		// AcapSetInfo/GetInfo
		BOOL	SetInfo(DWORD ulType, int nValue);					// パラメータ設定
		BOOL	GetInfo(DWORD ulType, int& nValue);					// パラメータ読み込み
		DWORD	GetLastError(DWORD* err_board, DWORD* err_ext);		// エラー情報取得




	//// メンバー変数
	protected:
		// 重要情報
		HANDLE					my_hDev;									// (0オリジン)
		DWORD					my_nBoardChId;								// (1オリジン)

        // error
        DWORD					my_errcode_common;
        DWORD					my_errcode_board;
        DWORD					my_errcode_ext;

		// image
		DWORD					my_nWidth;
		DWORD					my_nHeight;
		DWORD					my_nPixelNBit;

        // grab
        bool					my_bRunning;								// 撮像中 true

		// internal buffer
		DWORD					my_nBufNum;									// バッファ数
		DWORD					my_nBufSize;								// バッファサイズ	(画像サイズ)
		std::vector<BYTE*>		my_pBuff;									// バッファ領域 (BYTE [バッファ数][バッファサイズ])
	
		//          直前のGetFramePointersで使用されたmy_buffの領域
		//          my_buff[my_last_from]～my_buff[my_last_from+my_last_count-1]
		DWORD					my_nLastFrom;								// 前回読み込んだ位置
		AvalACL2Base::FrameWaitResult GetReadyFrameCount(DWORD& nFrameCount);		// 入力フレーム情報の取得


        // stats
		DWORD					my_nFrameOverrun;	// GetFramePointers/CheckOverrunで検出したオーバーランフレーム数累積

	};
};