//=====================================================================
// FrameNoでトラッキング可能とするクラス
//	【全ライン】
//		・並列処理に対応済み
//		・基本的に完全流用可能なように作る
//		・従来の速度UDPに付随するToValと同等機能も実現可能
//		・同一Noの複数登録はNG
//=====================================================================
#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\Class\AutoLock.h"
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// モジュールの基底クラス

// 半固定
//#include "..\..\Include\LineCommon.h"									// インクルード

// ローカルのクラス
//#include "MainConst.h"

// std
#include <list>


namespace KizuLib
{
	// //////////////////////////////////////////////////////////////////////////////
	// テンプレートなので、クラスの中に入れても使いにくいので、外だし

	// 良く使うものとしてベースの構造体を定義。継承して使ってね。
	struct TR_FRAME_BASE {
		DWORD				nFrameNo;									// 基準フレームNo

		int					nToVal[2];									// ご自由に
	};



	// //////////////////////////////////////////////////////////////////////////////
	// メイン

	template <class TTT>
	class TrackingFrame : public FuncBase
	{

	//// 公開関数
	public:
		TrackingFrame();
		virtual ~TrackingFrame();


		// 外部アクセス
		bool Alloc(int nMaxIns);											// 領域確保
		void Free();														// 領域解放

		// 状態
		int  GetQueCnt(int nInsId) { return m_pList[nInsId].size(); }


		// 処理
		bool IsFrameNo(int nInsId, DWORD nFrameNo);							// 登録済みかチェック
		bool AddItem(int nInsId, TTT* typ);									// アイテム追加
		bool GetItem(int nInsId, DWORD nFrameNo, TTT* typ);					// 取り出し (消えないので注意)
		bool GetDelItem(int nInsId, DWORD nFrameNo, TTT* typ);				// 取り出し & 削除
		bool DelItem(int nInsId, DWORD nFrameNo);							// 削除
		int  DelAll(int nInsId);											// 全要素削除

	//// メンバー変数
	protected:
		CRITICAL_SECTION		my_cs;										// クリティカルセクション

	private:
		int						m_nMaxIns;									// インスタンス数
		std::list<TTT>*			m_pList;									// トラッキングアイテム。 m_nIns数分の配列がある。

	};



	// //////////////////////////////////////////////////////////////////////////////
	// テンプレートなのでヘッダファイルに入れておく必要あり

	//------------------------------------------
	// コンストラクタ
	//------------------------------------------
	template <class TTT>
	TrackingFrame<TTT>::TrackingFrame() :
	FuncBase("TrFrame"),
	m_nMaxIns(0),
	m_pList(NULL)
	{
		InitializeCriticalSection(&my_cs);
	}

	//------------------------------------------
	// デストラクタ
	//------------------------------------------
	template <class TTT>
	TrackingFrame<TTT>::~TrackingFrame()
	{
		Free();

		DeleteCriticalSection(&my_cs);
	}


	//------------------------------------------
	// 領域確保。 全初期設定完了後
	// int nMaxIns	受渡リストの数。 たとえば、表裏、単発/周期があれば4。
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::Alloc(int nMaxIns)
	{
		// 既に実行済みであれば異常
		if( NULL != m_pList ) return false;

		// 領域確保
		m_nMaxIns = nMaxIns;
		m_pList = new std::list<TTT>[m_nMaxIns];
		return true;
	}

	//------------------------------------------
	// 領域解放
	//------------------------------------------
	template <class TTT>
	void TrackingFrame<TTT>::Free()
	{
		if( NULL == m_pList ) return;
		delete [] m_pList;
		m_pList = NULL;
	}

	//------------------------------------------
	// 登録済みかチェック
	// int nInsId 対象のインスタンス (0オリジン) -1:全部に登録
	// DWORD nFrameNo	チェックするフレームNo
	// 戻り値	１つでもあればtrue
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::IsFrameNo(int nInsId, DWORD nFrameNo)
	{
		if( NULL == m_pList ) return false;
		int is = -1 == nInsId ? 0 : nInsId;
		int ie = -1 == nInsId ? m_nMaxIns-1 : nInsId;

		AutoLock lock(&my_cs);
		for(int ii=is; ii<=ie; ii++) {

			std::list<TTT>::iterator	itr;					// イテレータ
			for( itr = m_pList[ii].begin(); itr != m_pList[ii].end(); itr++ ) {
			
				// コピーはコストの無駄なので、参照にしておく
				//TR_FRAME_BASE val = (TR_FRAME_BASE)(*itr);
				TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);
				
				if(val.nFrameNo == nFrameNo) {
					return true;
				}
			}
		}
		return false;
	}


	//------------------------------------------
	// 登録
	// int nInsId 対象のインスタンス (0オリジン) -1:全部に登録
	// 復帰情報
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::AddItem(int nInsId, TTT* typ)
	{
		if( NULL == m_pList ) return false;
		int is = -1 == nInsId ? 0 : nInsId;
		int ie = -1 == nInsId ? m_nMaxIns-1 : nInsId;

		AutoLock lock(&my_cs);

		// 対象に登録
		for(int ii=is; ii<=ie; ii++) {
			m_pList[ii].push_back(*typ);
		}
		return true;
	}

	//------------------------------------------
	// 取り出し
	// int nInsId 対象のインスタンス (0オリジン)
	// DWORD nFrameNo	取得するフレームNo
	// TTT* typ	取得情報
	// 復帰情報  false:取得データ無し。   true:取得データ有り
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::GetItem(int nInsId, DWORD nFrameNo, TTT* typ)
	{
		if( NULL == m_pList ) return false;

		AutoLock lock(&my_cs);

		//// リスト格納分 全件チェックを行う
		std::list<TTT>::iterator	itr = m_pList[nInsId].begin();	// イテレータ
		while( itr != m_pList[nInsId].end() ) {

			TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);
			if(val.nFrameNo == nFrameNo) {

				memcpy(typ, &val, sizeof(TTT));
				return true;
			}
			itr++;
		}
		return false;
	}


	//------------------------------------------
	// 取り出し & 削除
	// int nInsId 対象のインスタンス (0オリジン)
	// DWORD nFrameNo	取得するフレームNo
	// TTT* typ	取得情報
	// 復帰情報  false:取得データ無し。   true:取得データ有り
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::GetDelItem(int nInsId, DWORD nFrameNo, TTT* typ)
	{
		if( NULL == m_pList ) return false;

		AutoLock lock(&my_cs);

		//// リスト格納分 全件チェックを行う
		std::list<TTT>::iterator	itr = m_pList[nInsId].begin();	// イテレータ
		while( itr != m_pList[nInsId].end() ) {

			TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);
			if(val.nFrameNo == nFrameNo) {

				memcpy(typ, &val, sizeof(TTT));

				// リストから削除
				itr = m_pList[nInsId].erase(itr);	// 削除した後、次のイテレータを戻す
				return true;
			}
			itr++;
		}
		return false;
	}

	//------------------------------------------
	// 削除
	// int nInsId 対象のインスタンス (0オリジン)
	// DWORD nFrameNo	削除するフレームNo
	// 復帰情報  false:削除データ無し。   true:削除データ有り
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::DelItem(int nInsId, DWORD nFrameNo)
	{
		if( NULL == m_pList ) return false;

		AutoLock lock(&my_cs);

		//// リスト格納分 全件チェックを行う
		std::list<TTT>::iterator	itr = m_pList[nInsId].begin();	// イテレータ
		while( itr != m_pList[nInsId].end() ) {

			TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);
			if(val.nFrameNo == nFrameNo) {

				// リストから削除
				itr = m_pList[nInsId].erase(itr);	// 削除した後、次のイテレータを戻す
				return true;
			}
			itr++;
		}
		return false;
	}

	//------------------------------------------
	// 全要素解放
	// int nInsId 対象のインスタンス (0オリジン)
	// 戻り値		削除件数
	//------------------------------------------
	template <class TTT>
	int TrackingFrame<TTT>::DelAll(int nInsId)
	{
		int nCnt = 0;
		if( NULL == m_pList ) return nCnt;

		AutoLock lock(&my_cs);

		//// リスト格納分 全件チェックを行う
		std::list<TTT>::iterator	itr = m_pList[nInsId].begin();	// イテレータ
		while( itr != m_pList[nInsId].end() ) {

			TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);

			// リストから削除
			itr = m_pList[nInsId].erase(itr);	// 削除した後、次のイテレータを戻す
			nCnt++;
		}
		return nCnt;
	}
};
