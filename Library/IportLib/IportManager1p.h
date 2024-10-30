#pragma once

//#ifdef IPORTLIB_IMG_COPY_MMX
//	// MMXのSwdetectMMX::mem_copy_64 を使用する場合
//	// EPC用
//	#include "..\..\EPC\lib\mmx\MemCopy.h"
//#endif
#include "..\KizuMMX\MemCopy.h"												// MMXライブラリ


// ローカル
#include "IportBase.h"														// iPort基底クラス
#include "IportManager.h"													// iPort基底クラス


// インポート
using namespace KizuLib;
using namespace ImgLib;


namespace IportLib
{

	//===================================================================
	// カメラ取込本体
	//===================================================================
	class IportManager1p : public IportManager
	{

	//// 公開構造体
	public:


	//// 公開関数
	public:
		IportManager1p(CString sMyManagerName="iPortMgr", CString sMyIport="iPortBase");
		virtual ~IportManager1p(void);
		
		//virtual void		SetQueFrame(ThreadLoopQueue<FRAME_DATA>* pQue) { myque_pFrame = pQue;}


		// 一回のみ GraberIF
		virtual bool		Init(DWORD nFrameW, DWORD nFrameH, DWORD camset=1, DWORD angle=1, BYTE nDummyVal=0x80);	// 初期化
		virtual void		Dispose();										// 解放

		virtual bool		Start(); 										// スレッド開始 ※ このスレッドは、ホントにスレッドが実行されるまでロックされる
		virtual bool		Stop();											// スレッド停止
		
		// 撮像系 GraberIF
		virtual int			StartGrab();									// 撮像開始
		virtual int			StopGrab();										// 撮像停止
		virtual bool		Reset();										// リセット

		// iPort操作系
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, 
										bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, 
										int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);	// iPortに接続 (1p) 設定:XML, Master:XML
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, 
										const char* mac_addr, const char* ip_addr, char const* mc_addr, 
										bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, 
										int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);	// iPortに接続 (1p) 設定:XML, Master:IP直

		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, long nAdapterIndex1,
										const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) { _ASSERT(false); return NoErr; }	// iPortに接続(2p) 設定:XML, Master:XML, Slave:IP直
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath,
									    const char* mac_addr1, const char* ip_addr1, long nAdapterIndex1, char const* mc_addr1,
										const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) { _ASSERT(false); return NoErr; }	// iPortに接続(2p) 設定:XML, Master:IP直, Slave:IP直

		virtual void		DisConnect();									// iPortと切断


		// その他小物
		virtual bool		SendSerialTemp(DWORD set, DWORD ang, float& temp);	// カメラ温度を取得 (カメラと接続時しかダメ)


	//// プロパティ関係
	public:
		// オブジェクト取得

		//(IportBase&)*mycls_pIport[0] がわからない人向け
		//		mycls_pIport[0] は、IportBaseのポインタが入っている
		//		*mycls_pIport[0]で、IportBaseの本体にアクセス
		//		それを(IportBase&)で型変換

		virtual inline IportBase&		GetClsIpot(DWORD set, DWORD ang, bool isPrimary=true)	    { return GetBase(set, ang); }
		virtual inline int				GetCamIndex(DWORD set, DWORD ang) const						 // 全部0オリジン (カメラ単位)
			{ 	_ASSERT(set < my_nCamsetNum);
				_ASSERT(ang < my_nAngleNum);
				return set*my_nAngleNum + ang;
			}		
		virtual inline bool IsPT2000() const		{return false;}


		// 画像情報
        virtual long		GetFrameWidth()     const { return mycls_pIport[0]->GetFrameWidth(); }
        virtual long		GetFrameHeight()    const { return mycls_pIport[0]->GetFrameHeight(); }
        virtual long		GetFrameNChannel()  const { return mycls_pIport[0]->GetFrameNChannel(); }
        virtual long		GetFramePixelNBit() const { return mycls_pIport[0]->GetFramePixelNBit(); }

		// ステータス
		virtual DWORD		GetResendCount(DWORD set, DWORD ang) const { DWORD ch=GetCamIndex(set,ang); return mycls_pIport[ch]->GetResendCount();}	// パケット再送回数
		virtual DWORD		GetLostCount  (DWORD set, DWORD ang) const { DWORD ch=GetCamIndex(set,ang); return mycls_pIport[ch]->GetLostCount();}	// パケット損失個数
		virtual DWORD		GetChDiff	  (DWORD set, DWORD ang) const { DWORD ch=GetCamIndex(set,ang); return  my_nChDiff[ch];}
		virtual DWORD		GetChDiffMax()		const { return my_nChDiffMax;}


	//// 追加関係
	public:
		inline IportBase&		GetBase(DWORD set, DWORD ang)		{ return (IportBase&)*mycls_pIport[GetCamIndex(set, ang)]; }
		inline IportBase const& GetBase(DWORD set, DWORD ang) const { return (IportBase&)*mycls_pIport[GetCamIndex(set, ang)];}



	//// メンバー関数
	protected:

		// スレッド操作
		virtual int			ThreadMainLoop();								// スレッドメインループをシャドウ
		virtual int			ThreadFirst();									// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
		virtual int			ThreadLast()				{ return 0; };		// スレッド終了前処理 (終了シグナル後にコールバック)
		virtual void		ThreadEvent(int nEventNo)	{};					// スレッドイベント発生


		virtual void		OnFrameGrabbed(void** pImgs, long nX, long nY,  DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime);	// 1カメラセット分の画像を取得して、スレッドループキューにキューイング


	//// メンバー変数
	protected:
		// クラス
		IportBase*						mycls_pIport[MAX_IPORT_CAMNUM];		// iPortクラス 

		// 基本情報
		short							my_nChDiff[MAX_IPORT_CAMNUM];		// 自分で持っているシーケンス番号に対する各iportシーケンス番号との差
		short							my_nChDiffMax;
	};
};
