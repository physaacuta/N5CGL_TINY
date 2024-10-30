// *********************************************************************************
//	スレッド間同期制御クラス
//	[Ver]
//		Ver.01    2007/10/11  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************

#ifndef KIZULIB_LOCKMANAGER_H
#define KIZULIB_LOCKMANAGER_H

#include <Windows.h>

namespace KizuLib
{
	class LockManager
	{
	public:
		// //////////////////////////////////////////////////////////////////////////////
		// ちっちゃいからヘッダーに全部入れておく
		// //////////////////////////////////////////////////////////////////////////////
		// コンストラクタ
		LockManager()														
		{
			// 初期化
			InitializeCriticalSection(&my_CsLock);								// クリティカルセクションオブジェクトを初期化
			my_EvLock  = CreateEvent(NULL, TRUE, TRUE, NULL);					// マニュアルリセット、通常ずっとシグナル状態
		}

		// デストラクタ
		virtual ~LockManager() {											
			// 開放
			DeleteCriticalSection(&my_CsLock);									// クリティカルセクションオブジェクトを開放
			CloseHandle(my_EvLock);
		}

		// ロック
		void LockTh() {
			EnterCriticalSection(&my_CsLock);
			ResetEvent(my_EvLock); 
		}
		// アンロック
		void UnlockTh() {
			LeaveCriticalSection(&my_CsLock);
			SetEvent(my_EvLock); 
		}
		// ブロック
		void BlockTh() {
			WaitForSingleObject(my_EvLock, INFINITE);
		}
		
		//// 外部アクセス
		HANDLE	GetEvLock()   const {return my_EvLock; };	

	protected:
		CRITICAL_SECTION		my_CsLock;									// クリティカルセクション
		HANDLE					my_EvLock;									// ロックシグナル (ロックしている時のみ、非シグナルにする)
	};
};

#endif