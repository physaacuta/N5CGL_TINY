#include "StdAfx.h"
#include "CoilControlProc.h"


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// int nIns インスタンスNo
//------------------------------------------
CoilControlProc::CoilControlProc(int nIns) :
ThreadManager( GetThreadNameArray("CoilP", nIns) ),
m_nIns(nIns),
m_nRecvFno(0),
m_bDebugNextWpd(false),
m_nCFno(0),
m_bIsExec(false)
{
	memset( &m_NextCoil, 0x00, sizeof(m_NextCoil));
	memset( &m_NowCoil, 0x00, sizeof(m_NowCoil));
}

////------------------------------------------
// デストラクタ
//------------------------------------------
CoilControlProc::~CoilControlProc(void)
{

}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int CoilControlProc::ThreadFirst()
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];


	//// イベント設定
	HANDLE hArray[] = { this->mque_pIn->GetEvQue()
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int CoilControlProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void CoilControlProc::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_ADD_IMG = 0,					// フレーム画像キューイング
			EV_MAIL_QUE						// 外部操作キュー
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// フレーム取込
		AddNewItem();
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理
//------------------------------------------
// リセット
//------------------------------------------
void CoilControlProc::Reset()
{
	m_nRecvFno = 0;

	//// コイル情報初期化
	m_bDebugNextWpd = false;
	m_nCFno = 0;

	memset( &m_NowCoil, 0x00, sizeof(m_NowCoil));
}

//------------------------------------------
// 画像取得
//------------------------------------------
void CoilControlProc::AddNewItem()
{
	//// フレーム画像キューを取得
	FRAME_DATA_ARRAY* p = mque_pIn->GetFromHead();
	if( NULL == p ) {							// 通常ありえないはず
		LOG(em_ERR), "[%s] スレッドループキュー無し!", my_sThreadName);
		return;
	}

	//// 処理
	bool bNextThread = true;
	if(m_bIsExec) {
		Exec_Len(p);

	} else {
		LOG(em_INF), "[%s] キャンセル", my_sThreadName);
	}

	//// 次スレッドへ
	if(bNextThread) {
		if( ! mque_pOut->AddToTail(p) ) {
			PoolBack_RecvFreeQue(mque_pRecvFree, p);		// 自身の解放と合わせて、iPortデータを返却
		}
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// コイル情報関係

//------------------------------------------
// 次コイル情報セット
// COMMON_QUE const* p 次コイル情報
//------------------------------------------
void CoilControlProc::SetNextCoil(COMMON_QUE const* p)
{
	//// 既に次コイル情報がある？
	if( 0 != strlen( m_NextCoil.cKanriNo ) ) {
		LOG(em_WAR), "[%s] 次コイル情報有り [%s][%s]", my_sThreadName, m_NextCoil.cKanriNo, m_NextCoil.cCoilNo);
	}


	//// 次コイル情報にセット
	memset( &m_NextCoil, 0x00, sizeof(m_NextCoil));
	strcpy(m_NextCoil.cKanriNo, p->mix.cdata[0]);
	strcpy(m_NextCoil.cCoilNo, p->mix.cdata[1]);

	m_NextCoil.pcode[0] = p->mix.idata[1];
	m_NextCoil.pcode[1] = p->mix.idata[2];
	m_NextCoil.scode[0] = p->mix.idata[3];
	m_NextCoil.scode[1] = p->mix.idata[4];
	m_NextCoil.nChangeFno = p->mix.idata[5];
	m_NextCoil.mmFromHead = p->mix.idata[6];



	//// コイル単位の情報セット
	// 読込
	mcls_pParam->Read_IniCoilChange(m_nIns); 
	mcls_pParam->Read_IniKidoChange(m_nIns);

	// セット
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ m_nIns ];
	int pcode = m_NextCoil.pcode[prmI.nMen] - 1;

	m_NextCoil.emCompType = prmI.emCompType;
	m_NextCoil.nCompQuality = prmI.nCompQualityBase;
	m_NextCoil.bDummyCoil = false;

	if (0 > pcode || MAX_PCODE <= pcode) {
		m_NextCoil.nCompQuality = prmI.nCompQualityBase;
	}
	else {
		m_NextCoil.nCompQuality = prmI.nCompQuality[pcode];
		if (0 == m_NextCoil.nCompQuality) m_NextCoil.nCompQuality = prmI.nCompQualityBase;
	}

	if (0 == strlen(m_NextCoil.cKanriNo)) {
		KizuFunction::GetKizukenNo(m_NextCoil.cKanriNo);
		strcpy(m_NextCoil.cCoilNo, "Dummey");
	}

	LOG(em_MSG), "[%s] 次材セット[%s][%s]", my_sThreadName, m_NextCoil.cKanriNo, m_NextCoil.cCoilNo );
}

//------------------------------------------
// 次コイル情報エリアにダミーコイルをセット
// COIL_INFO_BUF& pCoil 次コイルエリア
//------------------------------------------
void CoilControlProc::SetDummeyCoil(COIL_INFO_BUF& typCoil)
{
	memset( &typCoil, 0x00, sizeof(typCoil));
	KizuFunction::GetKizukenNo( typCoil.cKanriNo ); 
	strcpy( typCoil.cCoilNo, "Dummey");

	// 圧縮率セット
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ m_nIns ];
	typCoil.emCompType = prmI.emCompType;
	typCoil.nCompQuality = prmI.nCompQualityBase;
	typCoil.bDummyCoil = true;
}

//------------------------------------------
// コイル切替
// FRAME_DATA_ARRAY* p フレーム情報
// 復帰情報  true:コイル切替有り
//------------------------------------------
bool CoilControlProc::ChangeCoil(FRAME_DATA_ARRAY* p)
{
	int nCoilSet = 0;												// 切替認識 (0：なし  1:通常 -1:はじめの一回)

	//// コイル切替 事前判定
	if( 0 == strlen(m_NowCoil.cKanriNo) )	nCoilSet = -1;			// 始めの一回のみ？

	if( 0 != strlen(m_NextCoil.cKanriNo) ) {
		long nSa = m_NextCoil.nChangeFno - p->nFno;
		if( 0 == nSa ) nCoilSet = 1;
		if( -100 < nSa && 0 > nSa ) {
			nCoilSet = 1;					// 既に通りすぎ
			LOG(em_WAR), "[%s] 通り過ぎにより強制切替 (今%d 切替予定%d)", my_sThreadName, p->nFno, m_NextCoil.nChangeFno);
		}
	}

	if(m_bDebugNextWpd) {
		// 強制切替
		if(0 == strlen(m_NextCoil.cKanriNo))  nCoilSet = -1;
		else									nCoilSet = 1;
		m_bDebugNextWpd = false;
	}


	//// コイル切り替え
	if(0 != nCoilSet) {
		if( -1 == nCoilSet || 
			( 1 == nCoilSet && 0 == strlen(m_NextCoil.cKanriNo)) ) {
			LOG(em_WAR), "[%s] 次コイル情報無し。ダミー情報生成", my_sThreadName);
			
			// ダミーコイルセット
			COIL_INFO_BUF  typWk;
			SetDummeyCoil(typWk);
			memcpy(&m_NowCoil, &typWk, sizeof(COIL_INFO_BUF));

		} else {
			// 本物情報セット
			memcpy(&m_NowCoil, &m_NextCoil, sizeof(COIL_INFO_BUF));
			memset(&m_NextCoil, 0x00, sizeof(COIL_INFO_BUF));
		}
		LOG(em_INF), "[%s] コイル切替認識 FNo=%d [%s][%s] COMP_Q=%d", my_sThreadName, p->nFno, m_NowCoil.cKanriNo, m_NowCoil.cCoilNo, m_NowCoil.nCompQuality);

		m_nCFno = p->nFno;
	}
	return (0==nCoilSet ? false : true);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 連続ライン関係

//------------------------------------------
// 連続ライン時の処理実行
// FRAME_DATA_ARRAY* p フレーム情報
//------------------------------------------
void CoilControlProc::Exec_Len(FRAME_DATA_ARRAY* p)
{
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ m_nIns ];

	bool bCoilSet;						// コイル切替認識
	
	//// コイル切替判定
	bCoilSet = ChangeCoil(p);

	//// コイル情報にセット
	memcpy(p->cKanriNo, m_NowCoil.cKanriNo, SIZE_KIZUKEN_NO );
	strcpy(p->cCoilNo, m_NowCoil.cCoilNo );
	p->mmFromHeadAtCoilStart = m_NowCoil.mmFromHead;
	p->nCFno = p->nFno - m_nCFno;

	p->emCompType   = m_NowCoil.emCompType;
	p->nCompQuality = m_NowCoil.nCompQuality;
	p->bDummyCoil	= m_NowCoil.bDummyCoil;



	// 個別情報
	p->emImgSaveStat = bCoilSet ? IMG_SAVE_STAT_CHANGE : IMG_SAVE_STAT_SAVE;

 
	

	//// 連結後画像左端からのエッジ位置を算出
	HT_RPI_FDATA* pHt = p->pData[ p->nIdx ];
	p->nEdgePos[0] = pHt->dEdgePos[0];
	p->nEdgePos[1] = pHt->dEdgePos[1];


	int nRawEdge;						// 左画像からのエッジ位置 [pixel]
	// 左エッジ
	if( 0.0 > pHt->dEdgePos[0] ) {
		int nWk = (int)(pHt->dEdgePos[0] / prmI.reso_x);				// ラインセンターからのオフセット位置 [pixel]
		nRawEdge = prmI.nLineCenter + nWk;								// 左エッジ位置
		if( nRawEdge < 0 ) nRawEdge = 0;
	} else {
		nRawEdge = 0;
	}
	p->nEdgeIdx[0] = nRawEdge; 
	// 右エッジ
	if( 0.0 < pHt->dEdgePos[1] ) {
		int nWk = (int)(pHt->dEdgePos[1] / prmI.reso_x);				// ラインセンターからのオフセット位置 [pixel]
		nRawEdge = prmI.nLineCenter + nWk;								// 右エッジ位置
		if( nRawEdge >= prmI.nUionWidth ) nRawEdge = prmI.nUionWidth-1;
	} else {
		nRawEdge = prmI.nUionWidth-1;
	}
	p->nEdgeIdx[1] = nRawEdge; 
}
