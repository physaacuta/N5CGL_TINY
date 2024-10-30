// *********************************************************************************
//	Grabber情報受渡クラス
//	 (iPort接続 共通 クラス)  ※ 昔は、iPort専用だったが、Grabber用として使用する
//
//	[Ver]
//		Ver.01    2010/01/29  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************
#pragma once

// 疵検ライブラリ
#include "..\KizuLib\Class\KizuPerformanceCounter.h"						// タイマー
#include "..\KizuLibMFC\CLASS\ThreadLoopQueue.h"							// 回転寿司形式のスレッドキュー


// インポート
using namespace KizuLib;

////
namespace IportLib
{
#ifndef HAS_IPORT
	enum {
		NoErr = 0,
		ApErr = -1,
	};
#endif

	struct  IAlloc {					// メモリ管理関数
		virtual void *Alloc(int nSize) = 0;
		virtual void Free(void *pMem) = 0;
	};

	//===================================================================
	// 受け渡しフレーム画像 (ThreadLoopQueue内に定義されている、インターフェースを継承)
	//===================================================================
	class FRAME_DATA : public IThreadLoopItem {
	//// 公開変数
	public:
		DWORD			nCamset;										// カメラセット		(0オリジン) (1コだけ)
		BYTE*			pImgs;											// 画像				(カメラ角度分)
		DWORD			nLen;											// 画像サイズ		(1コだけ) (個々でサイズが異なる画像形式は対応しない)

		DWORD			nNo;											// iPortフレームNo	(0オリジン) (1コだけ)
		//WORD			nSeqNum[32];									// iPortシーケンス番号
		DWORD			nSeqNum[32];
		
		HANDLE			hInfo[4];										// SetAddInfo で付与した情報
		KizuPerformanceCounter	cls_Time;								// 取得した時間(ストップウォッチ機能を使用)
																			// ライブラリ内でセットするタイムスタンプ
																			// ＃０：最初のカメラ角度を取得した時間
																			// ＃１：全部のカメラ角度が揃って、画像をコピー完了した時間


		static const int ALLOC_ARGSPTR_SIZE = 2;						// Alloc時の引数サイズ

	// メンバー変数
	protected:
		IAlloc* m_alloc;

	// メンバー関数
	protected:
		//========================
		// 器生成時
		// int const* p=0 受け渡しカスタム情報(int 配列) 
			// [0] 画像サイズ (本来、1*2でいいが、よく使うから渡しておく) ※ カラーの場合は、1画像×3カラー分の値をセット
			// [1] カメラ角度数をセットしておくこと (互換の為に、0の場合は、強制的に1にする)
			// [4] メモリ管理関数群のポインタ
			// [5] 同上
		virtual void	Alloc(int const* p=0)
		{ 
			int fsize	= p[0];
			int angle	= p[1];
			if(angle<=0) angle = 1;

			m_alloc = *(IAlloc**)&p[4];

			// 画像領域確保
			//pImgs		= new BYTE[ fsize*angle ];
			if (m_alloc){
				pImgs = (BYTE*)m_alloc->Alloc(fsize*angle);
			}
			else {
				pImgs = (BYTE*)_aligned_malloc(fsize*angle, 32);
			}
			if (NULL == pImgs) return;

			// 初期化
			memset(pImgs, 0x00, fsize*angle);
			nCamset		= 0;
			nLen		= 0;
			nNo			= 0;
			for(int ii=0; ii<4; ii++) hInfo[ii]	= INVALID_HANDLE_VALUE;
			memset(nSeqNum, 0x00, sizeof(nSeqNum));
		};

		//========================
		// 器解放時
		virtual void	Free()
		{ 
			//delete [] pImgs;
			//pImgs = NULL;
			if( NULL != pImgs ) {
				if (m_alloc) {
					m_alloc->Free(pImgs);
				}
				else {
					_aligned_free(pImgs);
				}
				pImgs = NULL;
			}
		};
	
	public:
		//========================
		// コンストラクタ
		FRAME_DATA(void) {
			pImgs = NULL;
		};

		//========================
		// 器生成時
		virtual ~FRAME_DATA(void) {
			if( NULL != pImgs ) {
				if (m_alloc) {
					m_alloc->Free(pImgs);
				}
				else {
					_aligned_free(pImgs);
				}
				
				pImgs = NULL;
			}
		};

	};


	//===================================================================
	// ワーニングや異常時の受け渡し情報
	//===================================================================
	// 受け渡し構造体の種別
	enum EM_DELIVERY_ID {
														//	ﾌﾚｰﾑNo,			ｶﾒﾗｾｯﾄ	ｶﾒﾗ角度	ｽﾃｰﾀｽ	概要
		DIV_ID_ERROR_NO_IMG_QUEFULL			= 400,		//	my_nFrameNum	○		0固定,	0		・スレッドループキューに空き無し
		//DIV_ID_ERROR_NO_IMG_QUELOCK			= 401,	//	my_nFrameNum	○		0固定,	0		・スレッドループキューがロック中
		DIV_ID_ERROR_GRABBER_FRAME_OVERRUN	= 10000,	//	my_nFrameNum	○		○		status	・LockForReadで取得したstatusで判定
        DIV_ID_ERROR_GRABBER_FIFO_OVERRUN,				//	my_nFrameNum	○		○		status	・LockForReadで取得したstatusで判定
        DIV_ID_ERROR_GRABBER_IMAGE_DROPPED,				//	my_nFrameNum	○		○		status	・LockForReadで取得したstatusで判定
        DIV_ID_ERROR_GRABBER_PARTIAL_LINE_MISSING,		//	my_nFrameNum	○		○		status	・LockForReadで取得したstatusで判定
        DIV_ID_ERROR_GRABBER_FULL_LINE_MISSING,			//	my_nFrameNum	○		○		status	・LockForReadで取得したstatusで判定
        DIV_ID_ERROR_GRABBER_FRAME_LOCK_TIMEOUT,		//	my_nFrameNum	○		○		0		・LockForReadタイムアウト時
		DIV_ID_ERROR_GRABBER_FRAME_LOCK_FAILED,			//	my_nFrameNum	○		○		CyRetc	・LockForRead失敗時
		DIV_ID_ERROR_GRABBER_IMAGEID_MISMATCH			//	my_nFrameNum	○		○		iPort番号	・自分で管理している連番とiportから取得した番号が違っているとき
	};


	// 受け渡し構造体
	struct DELIVERY_INFO {
		EM_DELIVERY_ID	em;												// 種別
		
		DWORD			nFrameNum;										// フレームNo   (0オリジン)
		DWORD			nCamset;										// カメラセット (0オリジン)
		DWORD			nAngle;											// カメラ角度   (0オリジン)
		DWORD			status;											// 実際のステータス情報
	};



	//===================================================================
	// 固有定数
	//===================================================================
	//// カラー配列
	enum IPORT_COLOR_IDX {
		IPORT_COLOR_R = 0,
		IPORT_COLOR_G,
		IPORT_COLOR_B
	};
};