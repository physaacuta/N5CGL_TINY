#pragma once

// 排他有りのCString 皮被せクラス (singleton)
// 書込みと参照が別のスレッドの場合の受渡用
//
//--------------
//想定ケース１
//  CoilManager 
//		CString  m_KanriNo;
//		CString  GetKanriNo() { return m_KanriNo;}
//
//  CoilMgr で
//		m_KanriNo = sVal;
//
//  Dlg で
//		CoilMgr->GetKanriNo()
//  の場合、 
//  CoilMgr ) 古いm_KanriNo を削除
//  Dlg ) GetKanriNo() 経由で m_KanriNo の中身をコピー
//  CoilMgr ) m_KanriNo の中身を新しく作る。
// となって、Dlgで死ぬ
//
//  そのため、
//		CString  m_KanriNo;  →  CStringCS m_KanriNoに変更
//
//
//--------------
//想定ケース２
//  上記の異常のときに、CStringCSに置き換えが困難な場合。
//		CString* でいろんな関数に渡している
//		ホントに触りたくないタスクなど
//	その場合、以下で実施すると、プロセス間で排他制御できるため、
//  読み、書き 部分のみ皮被せすることで、落ちるのを防止する
//
//  CStringCS::LockCpy(&cstringto, &cstringfrom);
//  sWk = CStringCS::LockCpy(&cstringfrom);
//	
namespace KizuLib
{
	// 本体
	class CStringCS
	{

		//----------
		// singleton
		//    LockCpy用
		class CStringEnv
		{
		//// 重要。exeで唯一のインスタンスやクラス類
		private:
			static CStringEnv				m_inst;											// exeで唯一のインスタンス。 初期化時にnewされて、終了時にdeleteされる.
			mutable CRITICAL_SECTION		m_csLock;										// 自クラスの他インスタンス間の排他制御
			//CRITICAL_SECTION		m_csLock;	

		//// 公開はすべてstatic
		public:
			static CStringEnv* GetIns() { 
				static CStringEnv ins; 
				return &ins;
			}
			//static CStringEnv* GetIns() { return &m_inst;}
			static inline CRITICAL_SECTION* GetCS() { 
				return &GetIns()->m_csLock;
			}

		//// 実体
		private:

			//// コンストラクタ(singleton)
			CStringEnv(void)																	// privateコンストラクタ
			{
				InitializeCriticalSection(&m_csLock);
			}

			//// コピーコンストラクタ
			CStringEnv(const CStringEnv &ob) {}
			// 代入
			CStringEnv& operator=(const CStringEnv& ob) {}

		public:
			virtual ~CStringEnv(void)
			{ 
				DeleteCriticalSection(&m_csLock); 
			}
		};




	//// メンバー定数
	protected:
		CString						m_sVal;								// 実体
		mutable CRITICAL_SECTION	m_csLock;							// 自クラス内の排他制御

	public:
		CStringCS() 
		{
			InitializeCriticalSection(&m_csLock);
		}
		virtual ~CStringCS(void)
		{ 
			DeleteCriticalSection(&m_csLock); 
		}

		CStringCS(const char* c) 
		{
			AutoLock lock(&m_csLock);
			m_sVal = c;
		}

		//// コピーコンストラクタ
		CStringCS(CStringCS const& ob)
		{
			AutoLock lock(&m_csLock);
			this->m_sVal = ob.m_sVal;
		}


		//// セット (よく使うもののみ)
		//		※これ以外のFormatやAppend などは、CSringでやってから本クラスにセット
		CStringCS& operator = (const CStringCS& ob)
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob.m_sVal;
			return *this;
		}
		CStringCS& operator = ( const CString& ob )
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob;
			return *this;
		}
		CStringCS& operator = (char* ob)
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob;
			return *this;
		}


		//// ポインタのオーバーロードは絶対にダメ。意味を成さないため
		//CString* operator & ()
		//{
		//	_ASSERT(false);
		//	//AutoLock lock(&m_csLock);
		//	return NULL;
		//}

		//// 暗黙的型変換
		operator CString () const {
			AutoLock lock(&m_csLock);
			return m_sVal;		// CString の コピーコンストラクタが走るから、ＯＫ。これ重要
		}


	//// 外部操作用
	// 既設改造が少なくなるようにするための処置
	// 基本は上を使ってほしい。
	// プロセス間で、ロック可能
	public:
		static void LockCpy(CString* to, CString* from) {
			AutoLock lock(CStringEnv::GetCS());
			*to = *from;
		}

		static void LockCpy(CString* to, char const* from) {
			AutoLock lock(CStringEnv::GetCS());
			//Sleep(20000);  // テスト
			*to = from;
		}

		static CString LockCpy(CString* from)
		{
			AutoLock lock(CStringEnv::GetCS());
			return *from;	// CString のコピーコンストラクタが走るはず
		}
		static CString LockCpy(CString& from)
		{
			AutoLock lock(CStringEnv::GetCS());
			return from;	// CString のコピーコンストラクタが走るはず
		}

	};


/*
	class CStringCS_back
	{
		
	//// メンバー定数
	protected:
			mutable	CRITICAL_SECTION	m_csLock;							// クリティカルセクション
			CString						m_sVal;								// 実体

	//// 実態
	public:
		CStringCS(void) 
		{ 
			InitializeCriticalSection(&m_csLock);
		}
		CStringCS(const char* c) 
		{ 
			InitializeCriticalSection(&m_csLock);

			AutoLock lock(&m_csLock);
			m_sVal = c;
		}
		virtual ~CStringCS(void)
		{ 
			DeleteCriticalSection(&m_csLock); 
		}

		//// コピーコンストラクタ
		CStringCS(CStringCS const& ob)
		{
			InitializeCriticalSection(&this->m_csLock);
			this->m_sVal = ob.m_sVal;
		}


		//// セット (よく使うもののみ)
		//		※これ以外のFormatやAppend などは、CSringでやってから本クラスにセット
		CStringCS& operator = (const CStringCS& ob)
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob.m_sVal;
			return *this;
		}
		CStringCS& operator = ( const CString& ob )
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob;
			return *this;
		}

		CStringCS& operator = (char* ob)
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob;
			return *this;
		}



		//CStringCS& operator * ()
		//{
		//	AutoLock lock(&m_csLock);
		//	m_sVal = ob;
		//	return *this;
		//}

		// ポインタのオーバーロードは絶対にダメ。意味を成さないため
		//CString* operator & ()
		//{
		//	AutoLock lock(&m_csLock);
		//	return &m_sVal;
		//}

		//// 暗黙的型変換
		operator CString () const {
			AutoLock lock(&m_csLock);
			return m_sVal;		// CString の コピーコンストラクタが走る
		}
	};
*/
};