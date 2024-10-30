// *********************************************************************************
//	複数トラッキング制御クラス
//	[Ver]
//		Ver.01    2007/03/14  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************
#pragma once

#pragma warning( disable : 4786 ) 

#include <list>

// 疵検クラス インクルード
#include "..\KizuLib\Class\WorkThread.h"								// ワーカースレッドクラス
#include "..\KizuLib\Class\AutoLock.h"									// クリティカルセクション管理クラスヘッダー
#include "..\KizuLib\Class\ThreadQueue.h"								// スレッドキュークラスヘッダー

using namespace KizuLib;

class TrackingList : public WorkThread  
{
public:
	//// トラッキングの終了区分
	enum EM_TR_END_STATE {
		TR_END = 0,														// 指定距離経過した場合
		TR_CANCEL,														// 途中中断した場合
		TR_STOP															// スレットが停止時
	};

	//// トラッキングチェック用構造体 (スレッドキュー取り出し後に解放必須)
	struct TR_INF {
		// トラッキングで必要な情報
		long					nDist;									// 何mm後に通知? [mm] (固定)
		double					dDistNow;								// インスタンス生成時の距離 [mm]
		double					dDistEnd;								// イベント発生時の距離 [mm]
		double					dChangeLengthMax;						// コイル切替等でチェック距離が0クリアされた場合のゲタはかせ分

		// スレッドキュー渡しで取得した後で必要になる項目
		EM_TR_END_STATE			emEndState;								// スレッドキューに登録した区分
		int						nKey;									// キー情報 (0:キーチェック無しの意味となる)
		long					nAddr1;									// ユーザー用ワークエリア
		long					nAddr2;									// ユーザー用ワークエリア
		long*					pAddr;									// ユーザー用ワークエリア (通常クラスのポインタ等セット)(未使用時はNULL)
	};


public:
	TrackingList(const double* length);
	virtual ~TrackingList();

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(); };								// スレッド終了
	
	// 受け渡しキュー
	ThreadQueue<TR_INF>			gque_Deli;								// 受け渡しキュー (スレッドキュー取り出し後に解放必須)


	// 外部アクセス
	void		TrackingAdd_Len(DWORD len, int nKey=0, long nAddr1=0, long nAddr2=0, long const* pAddr=NULL);	// トラッキングアイテム をチェックして、指定距離を通板していたら TR_END でキューイング
	void		TrackingAdd_Dist(DWORD dist, int nKey=0, long nAddr1=0, long nAddr2=0, long const* pAddr=NULL);	// トラッキングアイテム をチェックして、指定距離を通板していたら TR_END でキューイング
	void		TrackingCancel(double dLength, int nKey=0);				// 強制停止 (これをコールすると TR_CANCEL でキューイングする)
	void		TrackingCancelKey(int nKey);							// 強制停止 指定キーのみをキャンセル
	void		TrackingStop();											// トラッキングアイテム を全て解放＆キューイングする
	void		TrackingLengthChange(double dOldLen);					// コイル切替等が発生時

	bool		GetCloneKeyItem(int nKey, TR_INF* inf);					// 指定キーNoのアイテムのクローンを取得する (現状を知りたい場合等)
	double		CheckExecDist(TR_INF const* inf) const					// 後何ｍで実行？
		{
			return (*m_dLength - (inf->dDistNow - inf->dChangeLengthMax));
		}


	void		QueAdd(DWORD dist, int nKey=0, long nAddr1=0, long nAddr2=0, long const* pAddr=NULL);			// 新規キュー アイテムを追加する (トラッキングせずに、即時にキューイング)
	void		QueAllFree();											// すべてのキューを開放



private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	void		TrackingCheck();										// トラッキングアイテム をチェックして、指定距離を通板していたらシグナル発行する


	// トラッキングに必要な情報
	const double*				m_dLength;								// チェックする値 [mm]
	std::list<TR_INF*>			m_List;									// トラッキングアイテム


	// その他もろもろ
	CRITICAL_SECTION			m_csLock;								// クリティカルセクション
	
};


