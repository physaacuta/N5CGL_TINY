// 疵画像送信スレッド
// スレッド間メッセージで画像送信要求を受信したら、
// 疵情報・疵画像を送信する

#pragma once
#include "..\..\library\kizulibmfc\class\threadmanager.h"
#include "..\..\Library\DefectLib\KizuOpSend.h"	
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB書込み基本クラス
//#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス

#include "ParamManager.h"

class ImgSendManager :
	public ThreadManager
{
public:

	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_KEY {
		int		id;

		char	cKizukenNo[SIZE_KIZUKEN_NO];		// 主キー
		char	cCoilNo[SIZE_COIL_NO];				// コイルNo
		long	x;									// 幅分割位置 0～7
		char	cMeiNo[SIZE_MEI_NO];				// 命令No
		int		nMen;								// 面 0:表、1:裏
		int		nTid;								// 疵種EdasysID
		int		nGid;								// グレードEdasysID
		int		nInY;								// 疵長手位置
		float	dDS;								// DS～距離
		float	dWS;								// WS～距離
		int		nKizuSeq;							// 疵連番
		int		nSpeed;								// 速度
		int		nObjTansho;							// 探傷出力対象
		int		nObjDeflist;						// 重欠陥一覧画面対象疵
		int		nObjDefmap;							// 疵マップ出力対象
		int		dspmode;							// 重欠陥一覧強調表示対象疵
		int		nSysKbn;							// 検出システム
		float	nArea;
	};
	ThreadQueue<DELIVERY_KEY>	gque_Deli;

	ImgSendManager(void);
	virtual ~ImgSendManager(void);


	//=========================================
	// セット
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; };	// パラメータ管理
	void SetOpSend(KizuOpSend *pCls)			{ mcls_pOpSend = pCls; };	// ソケット管理
//	void SetDbQue(ThreadQueue<DBSaveManager::DeliverySQL>*	pQue)			{ mque_AddDB = pQue; };


	//// メンバー関数
protected:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);		

	void SetDefectInfo(DELIVERY_KEY* pDeli);							// 1.疵情報(疵情報+疵画像) 送信


	//// メンバ変数
	//=========================================
	//// 各インスタンス
	ParamManager*					mcls_pParam;	
	KizuOpSend*						mcls_pOpSend;
//	ThreadQueue<DBSaveManager::DeliverySQL>*	mque_AddDB;

};
