// *********************************************************************************
//	CRITICAL_SECTION のロック・アンロック クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//		Ver.02	  2013/12/26  強制キャンセル機能追加
//
//	[メモ]
//
// *********************************************************************************

#ifndef KIZULIB_AUTOLOCK_H
#define KIZULIB_AUTOLOCK_H

#include <Windows.h>

namespace KizuLib
{
	////////////////////////////////////////////////////////////////////////
	// 任意排他制御
	//------------
	// 使い方
	//  AutoLock autolock(&m_csLock);	// ←これだけでOk
	class AutoLock  
	{
	// Ver.01
	//public:
	//	AutoLock(CRITICAL_SECTION * pCS)									// コンストラクタ
	//	{
	//		my_pCS = pCS;
	//		EnterCriticalSection(my_pCS);
	//	}
	//		
	//	virtual ~AutoLock() {												// デストラクタ
	//		LeaveCriticalSection(my_pCS);
	//	}

	//protected:
	//	CRITICAL_SECTION*		my_pCS;										// クリティカルセクション

	// Ver.02
	public:
		AutoLock(CRITICAL_SECTION * pCS)									// コンストラクタ
		{
			my_pCS = pCS;
			EnterCriticalSection(my_pCS);
		}
			
		void UnLock() {														// 強制キャンセル
			if( NULL == my_pCS ) return;
			LeaveCriticalSection(my_pCS);
			my_pCS = NULL;
		}

		virtual ~AutoLock() {												// デストラクタ
			//if( NULL == my_pCS ) return;
			//LeaveCriticalSection(my_pCS);
			//my_pCS = NULL;
			UnLock();
		}

	protected:
		CRITICAL_SECTION*		my_pCS;										// クリティカルセクション

	};


	////////////////////////////////////////////////////////////////////////
	// プロセス内 排他制御
/**
	//--------------------------------------------
	// プロセス内唯一の排他
	// singleton
	class LockOnlyCs
	{
	//// 重要。exeで唯一のインスタンスやクラス類
	private:
		static LockOnlyCs				m_inst;											// exeで唯一のインスタンス。 初期化時にnewされて、終了時にdeleteされる.
		mutable CRITICAL_SECTION		m_csLock;									// 自クラスの他インスタンス間の排他制御

	//// 公開はすべてstatic
	public:
		static LockOnlyCs* GetIns() { 
			static LockOnlyCs ins; 
			return &ins;
		}
		static inline CRITICAL_SECTION* GetCS() { 
			return &GetIns()->m_csLock;
		}

	//// 実体
	private:

		//// コンストラクタ(singleton)
		LockOnlyCs(void)																	// privateコンストラクタ
		{
			InitializeCriticalSection(&m_csLock);
		}

		// コピーコンストラクタ 禁止
		LockOnlyCs(const LockOnlyCs &ob) {}
		// 代入禁止
		LockOnlyCs& operator=(const LockOnlyCs& ob) {}

	public:
		virtual ~LockOnlyCs(void)
		{ 
			DeleteCriticalSection(&m_csLock); 
		}
	};

**/

	//--------------------------------------------
	// プロセス内唯一
	//------------
	// 使い方
	//  ・ライブラリ内で使うの絶対に禁止
	//  ・AutoLockOnly	autolock;	// ←これだけでOk
	class AutoLockOnly
	{

		//--------------------------------------------
		// プロセス内唯一の排他 (入れ子クラス)
		// singleton
		class LockOnlyCs
		{
		//// 重要。exeで唯一のインスタンスやクラス類
		private:
			static LockOnlyCs				m_inst;											// exeで唯一のインスタンス。 初期化時にnewされて、終了時にdeleteされる.
			mutable CRITICAL_SECTION		m_csLock;									// 自クラスの他インスタンス間の排他制御

		//// 公開はすべてstatic
		public:
			static LockOnlyCs* GetIns() { 
				static LockOnlyCs ins; 
				return &ins;
			}
			static inline CRITICAL_SECTION* GetCS() { 
				return &GetIns()->m_csLock;
			}

		//// 実体
		private:

			//// コンストラクタ(singleton)
			LockOnlyCs(void)																	// privateコンストラクタ
			{
				InitializeCriticalSection(&m_csLock);
			}

			// コピーコンストラクタ 禁止
			LockOnlyCs(const LockOnlyCs &ob) {}
			// 代入禁止
			LockOnlyCs& operator=(const LockOnlyCs& ob) {}

		public:
			virtual ~LockOnlyCs(void)
			{ 
				DeleteCriticalSection(&m_csLock); 
			}
		};


		
	//--------------------------------------------
	// 実体
	public:
		AutoLockOnly()										// コンストラクタ
		{
			//CRITICAL_SECTION* cs = OnlyCs::GetCS();
			EnterCriticalSection(LockOnlyCs::GetCS());
		}
			
		void UnLock() {										// 強制キャンセル
			LeaveCriticalSection(LockOnlyCs::GetCS());
		}

		virtual ~AutoLockOnly() {							// デストラクタ
			UnLock();
		}
	};

};

#endif