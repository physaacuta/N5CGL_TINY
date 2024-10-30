#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"						// ワーカースレッドマネージャー

#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"							// SQL文生成補助クラス

// 共通インクルード
#include "..\..\Include\OutInterface.h"											// 外部インターフェース用ヘッダー

// 各ワーカースレッド
#include "MainConst.h"															// 自クラス定数ヘッダー
#include "MakeProManager.h"														// プロコン検査実績作成管理クラス

class MakeFfManager : public ThreadManager
{
//// 公開構造体
public:
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_KEY
	{
		char				cKey[SIZE_SERIAL_FF_NO];							// 主キー
	};
	
//// メンバー構造体	
private:
	// ＦＦ検査実績伝文対象疵情報
	struct FF_INF {
		//// 共通
		bool bOverStatus;			// オーバーステータス(オーバーした場合True)
		int nFfCnt;					// FF情報にセットした疵個数

		//// 対象予定の疵種
		// (グレード優先度、疵種優先度、長手順に並んでいる)
		int nDefCnt;				// 疵 セット数
		struct DEF {		
			int		nGid;			// グレード EdasysID
			int		nTid;			// 疵種 EdasysID

			int		nRank;			// 欠陥グレード (0:無害 1:軽欠陥 2:中欠陥 3:重欠陥)
			int		nGPriority;		// グレード優先度
			int		nTPriority;		// 疵種優先度

			int		nNo;			// 疵No
			int		nYpos;			// 長手位置[mm]  (補正率考慮後)
			int		nWS;			// WS位置[mm]
			int		nLen;			// 疵長さ [mm]

			int		nGFF;			// FF伝送名称(グレード)
			char	tFF[2+1];		// FF伝送名称(疵種)

			
			// 実際にFFサーバーへセットした時に更新
			bool	bFFSet;			// FFに実際にセットした場合True
			

		} typDef[MAX_FF_DFCT_CNT];		// 疵個数MAX

	};

	// ＦＦ検査実績伝文対象疵情報
	struct FF_STS {
		int nCnt;				// 疵 セット数
		struct STS {
			int						nID;									// ID (1～連番)
			EM_DIV_KENSA 			emKensa;								// 検査状態 (5mピッチ)
			EM_DIV_KENSA 			emKensaOut;								// 出側検査状態 (5mピッチ)
			EM_DIV_KENSA 			emKensaAna;								// 穴検検査状態 (5mピッチ)
		} sts[MAP_ROW_NUM];
	};

	//=======================================
	// パラメータ関係
	//=======================================
	// 共通パラメータ
	struct PARAM_COMMON_MAST {
		int	nEdgeNgCount;							// エッジ整合性_エッジ検出異常FF送信。(0:無し 1～:エッジ未検出区間有りにする)
	};
	// グレード
	struct PARAM_GRADE
	{
		int nNum;									// 総個数
		struct GRADE_ID									// 優先度順
		{
			int	nEdasysID;							// edasysID
			int nPriority;							// 優先度 (1が最強)
			int nRank;								// 欠陥グレード (0:無害 1:軽欠陥 2:中欠陥 3:重欠陥)
			
		} typGrade_id[MAX_GRADE];
	};
	// 出力設定用パラメータ
	struct PARAM_PCODE
	{
		// 出力条件
		struct OUTPUT								// 出力項目
		{
			int		nEdasysID;							// 疵種edasysID (1～:疵種ID  0:未使用  -1:その他項目)

			int		nKind;								// グレードID (0:無し 1～:グレードedasysID) 指定されたグレードID以上の疵を出力対象
		};
		// 実体
		struct INF
		{
			// パターン別出力設定 (PCODE_OUTPUT)
			int		nPutNum;						// outputにセットしている総個数
			OUTPUT	typOutput[MAX_TYPE];				// 出力条件設定
			OUTPUT	typDefput;							// 疵種未設定項目の出力条件

		} rec[MAX_PCODE];							// 最終は、その他項目固定
	};


	//=======================================
	// 未送信実績の強制送信関係
	//=======================================
	// 未送信実績の情報
	struct UNSENT_DATA
	{
		char	kizukenNo[SIZE_KIZUKEN_NO];										// 対象の管理No
		int		len;															// コイル長さ(mm)
		int		pcode[NUM_MEN];													// 検査パターンコード
		char	MeiNo[SIZE_MEI_NO+1];											// キー情報 命令番号
		char	CoilNo[SIZE_COIL_NO+1];											// キー情報 コイルNo
		int		nSpm;															// SPM圧下有無
	};

//// 公開関数
public:
	MakeFfManager(void);
	virtual ~MakeFfManager(void);

	void	SetMakeProMgr(MakeProManager* p) { mcls_pProMgr=p; }

//// 公開変数 
public:
	ThreadQueue<DELIVERY_KEY>	gque_Deli;										// 受け渡しキュー

//// メンバー関数
private:
	int ThreadFirst();															// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();															// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);												// スレッドイベント発生

	void ClearMember();															// 処理中変数 初期化
	bool Exec(char const* cID);													// ＦＦサーバー検査実績 伝文生成

//------------>>> Execからの一連の処理 (この中の関数だけ privateの処理中変数が使用可能)
	bool GetBufferSai(char const* cID);											// プロコン採寸実績受信バッファテーブル取得
	bool GetCoilInf_KizukenNo();												// コイル情報から疵検管理Noを取得
	bool CheckCoilResultInsertOk(int *nIsWarm, int* nIsSendOk, int* pcode);		// コイル実績が登録されるまで待ち。＆補正率確定
	bool UpdateProconBuf(const char *cID);										// 採寸実績テーブル を 更新
	bool UpdateCoilResult();													// コイル実績テーブル を 更新
	bool InsertCutResult(const char *cID);										// カット実績テーブルに追加
	bool InsertSaisunResult(const char *cID);									// 採寸実績テーブルに追加

	bool MakeSendData(int ou, int pcode, int nIsResult, FF_INF* pInf, FF_JISSEKI* pTypZi);	// FF送信用 実績伝文生成
	void MakeCutData(int ou, FF_INF * pInf, FF_JISSEKI* pTypZi);				// カット情報を FF実績にセットする
	void MakeDefectData(int ou, FF_INF * pInf, FF_JISSEKI* pTypZi);				// 疵情報エリアにセットする
	bool MakeSendStsData(int ou, int pcode, int nIsResult, FF_STS* pPos, FF_STS_JISSEKI* pTypSts);	// FF送信用 検査状態伝文生成
	bool GetData_CoilPosition(int ou, FF_STS* pPos);							// 対象コイルのコイル位置情報を取得する
	void MakeFfManager::MakeStatusData(int ou, FF_STS * pPos, FF_STS_JISSEKI* pTypSts);				// 検査状態エリアにセットする
	
	int GetStatus_Kensa();														// 対象コイルの検査状態(疵検の稼動状況)
	int GetStatus_EdgeDetect(int ou, int nState);								// 対象コイルのエッジ検出状況を取得する

	bool GetData_DefectInf(int ou, int pcode, FF_INF* pInf);					// 対象コイルの疵情報を取得する

	bool InsertZiBuf(char const* cID, FF_JISSEKI const* pTypZi, FF_STS_JISSEKI const* pTypSts);		// FF用検査実績伝文 を バッファテーブルに登録

	bool InsertAccessNextInf( FF_INF pInf[], FF_JISSEKI pTypZi[]);		// 次工程情報テーブルに疵情報等をセット

	// パラメータ関係
	bool ReadParam();															// パラメータ読込
	bool Check_Output(long pcode, long tid, long gid) const;					// 検査パターン出力対象の欠陥か判定
	int GetYusenG(long gid) const;

	// 未送信実績の強制送信関連
	bool ExecUnsentResult(char const* cKizukenNo);						// 未送信の実績の検出とFF送信処理
	bool GetUnsentCoilResult(char const* cKizukenNo);					// 未送信データのコイル実績を抽出
	bool MakeUnsentFFSendData(int nRec);								// 未送信実績データの生成
	bool UpdateCoilResultForUnsentData(char* cKizukenNo);				// 強制送信レコードのコイル実績テーブル を 更新
																		// グレード優先度取得
// <<<-----------


	void Send_To_FF(char const* cKey);											// 対FF通信 に送信依頼

//// メンバー変数
private:
	//// 各インスタンス
	MakeProManager				*mcls_pProMgr;									// プロコン用実績伝文生成クラス

	// 処理中 (処理中のみデータが有効)
	OdbcBase					*mcls_pDB;										// DB接続クラス
	char						m_cKizukenNo[SIZE_KIZUKEN_NO];					// 対象の管理No
	PRO_TYPE_SAISUN				m_typBuf;										// 受信データ (採寸実績)

	PARAM_COMMON_MAST			m_Param_CommonMast;								// 疵検定数
	PARAM_GRADE					m_Param_Grade;									// グレード 
	PARAM_PCODE					m_Param_Pcode;									// 検査パターン別
	UNSENT_DATA					m_unsent_data[UNSENT_MAX];						// 強制送信対象レコード情報 

	// その他もろもろ
	bool						m_bCreateOk;									// 処理結果
	double						m_dHosei;										// 補正率 [%]
	int							m_nRecCnt;										// バックアップ送信レコード件数
	int							m_nSentCnt;										// バックアップ送信済み件数(登録に成功した件数)
	int							m_nLen;											// 検査実測長[mm]
	
	// iniファイル値
	int							m_nChkCoilDate;									// コイル検索の対象期間
	int							m_nRetryCount;									// コイル実績確認リトライ回数
	int							m_nRetryWaitTime;								// コイル実績確認リトライ時間(ms)
	
	int							m_nChkUnSentDate;								// 強制送信対象データ検索の対象期間

};
