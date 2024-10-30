#include "StdAfx.h"
#include "ImgCompProc.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// int nId インスタンスNo
//------------------------------------------
ImgCompProc::ImgCompProc(int nId) :
ThreadManager( GetThreadNameArray("CompP", nId) ),
m_bIsExec(false),
m_bIsComp(false),
///mcls_Jpeg(),
mcls_Lac(-1)	// 読込無し 
{
//	m_imgwk = new BYTE[2048*4096];
	m_nCompQuality = 0;
	m_nUionWidth = 0;
	m_nUionHeight = 0;

	mcls_Lac.SetThrId(nId);				// スレッドIDをセット(LAC5ではデバイスIDに使用)
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ImgCompProc::~ImgCompProc(void)
{
//	delete [] m_imgwk;
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ImgCompProc::ThreadFirst()
{
	ParamManager::P_MAIN::INS 	const& prmI	= mcls_pParam->GetMain().ins[0];

	//// DLL読込
	if (EM_COMP_TYPE::COMP_TYPE_LAC == prmI.emCompType) {
		mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_LAC);
	} else if (EM_COMP_TYPE::COMP_TYPE_NUC == prmI.emCompType) {
		mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_NUC);
	} else if (EM_COMP_TYPE::COMP_TYPE_LAC5 == prmI.emCompType) {
		bool bRet = mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_LAC5);
		bool bSts = true;
		if (bRet) {
			int nRetc = mcls_Lac.InitEncoder(prmI.nUionWidth, prmI.nUionHeight, prmI.nCompQualityBase);		// エンコーダ初期化
			LOG(em_INF), "[%s] エンコーダ初期化 W:%d, H:%d Q:%d ret=%d", my_sThreadName, prmI.nUionWidth, prmI.nUionHeight, prmI.nCompQualityBase, nRetc );
			if (0 != nRetc) bSts = false;
		}
		else {
			LOG(em_ERR), "[%s] LAC5 DLLロード失敗", my_sThreadName);
			bSts = false;
		}
		if (!bSts) {
			KizuFunction::SetStatus(mcls_pParam->GetMain().nStateId, false, false);		// 機器状態異常
			syslog(510, "[%s] 幅:%d 高さ:%d 品質:%d", my_sThreadName, prmI.nUionWidth, prmI.nUionHeight, prmI.nCompQualityBase);
		}

		m_nCompQuality = prmI.nCompQualityBase;
		m_nUionWidth = prmI.nUionWidth;
		m_nUionHeight = prmI.nUionHeight;
	}

	//// イベント設定
	HANDLE hArray[] = { this->mque_pIn->GetEvQue() 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int ImgCompProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ImgCompProc::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_ADD_IMG = 0,					// フレーム画像キューイング
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// フレーム取込
		this->ExecQue();
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
void ImgCompProc::Reset()
{

}

//------------------------------------------
// 実処理
// Tin* pIn		入力Queデータ
// 戻り値		NULL:返却しない 出力Queデータ:次のスレッドへ返却する場合  
//------------------------------------------
FrameDataManager* ImgCompProc::OnAddNewItem( FrameDataManager* p )
{
	if( NULL == p ) {	
		LOG(em_ERR), "[%s] スレッドループキュー無し!", my_sThreadName);
		return p;
	}

	//// 処理
	// この処理では、 4096*2048 * 4 で、20msec程度で終わる。  だから並列処理は必要ないかなー
	if(m_bIsExec) {
		m_bIsComp =true;
		p->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgComp_START);
		Exec(p);
		p->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgComp_END);
		m_bIsComp =false;
	} else {
		LOG(em_INF), "[%s]<%d> キャンセル", my_sThreadName, p->nIns );
	}

	return p;
}

//------------------------------------------
// 処理実行
// FrameDataManager* p フレーム情報
//------------------------------------------
void ImgCompProc::Exec(FrameDataManager* p)
{

	// =========================================================
	// よく使うパラメータをエイリアス
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ p->nIns ];
	int nMen = prmI.nMen;

//	LOG(em_MSG), "[%s]<%d> start", my_sThreadName, p->nIns);

/*/
	CString s;
	s.Format("c:\\comp%d-%d.bmp", p->nColor, p->UdpData.framenum);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)p->pImgs, p->nLen );
	os.close();
	//CString sWk;
	//sWk.Format("c:\\testt_%d.bmp", p->nIns); 
	//KizuBitmapManager cls;
	//cls.DataToBMPfile_Gray(4096, 2048, p->pImgs, sWk);
//*/


	// =========================================================
	// フロー制御
	//		本決め
	bool bSkip		 = false;									// 画像連結スキップ有無
	EM_COMP_TYPE emCompType = p->emCompType ;					// 圧縮タイプ
	int nCompQuality = p->nCompQuality;							// 圧縮率
	int nExecQueCnt  = mque_pIn->GetCount();

	if (EM_COMP_TYPE::COMP_TYPE_LAC5 == prmI.emCompType) {
		// 圧縮率の変更がある場合、GPU版LACではエンコーダの初期化が必要
		if (m_nCompQuality != p->nCompQuality ||
			m_nUionWidth != p->nUnionWidth ||
			m_nUionHeight != p->nUnionHeight) {

			m_nCompQuality = p->nCompQuality;
			m_nUionWidth = p->nUnionWidth;
			m_nUionHeight = p->nUnionHeight;

			// パラメータ変更時に対応
			int nRetc = mcls_Lac.InitEncoder(m_nUionWidth, m_nUionHeight, m_nCompQuality);		// エンコーダ初期化
			LOG(em_INF), "[%s] エンコーダ初期化(再) W:%d, H:%d Q:%d ret=%d", my_sThreadName, m_nUionWidth, m_nUionHeight, m_nCompQuality, nRetc );
			if (!mcls_Lac.IsGpuEncoder()) {
				if (0 != nRetc) {
					KizuFunction::SetStatus(mcls_pParam->GetMain().nStateId, false, false);		// 機器状態異常
					syslog(510, "幅:%d 高さ:%d 品質:%d", m_nUionWidth, m_nUionHeight, m_nCompQuality);
				}
			}
			LOG(em_INF), "[%s] エンコーダ初期化(再) W:%d, H:%d Q:%d [再確保は未サポート]", my_sThreadName, m_nUionWidth, m_nUionHeight, m_nCompQuality );
		}
	}

	//// DLL リロード
	int nDll = mcls_Lac.IsLoadType();
	if( -1 != nDll) {
		// LAC → NUC
		if( 0 == nDll && COMP_TYPE_NUC == emCompType ) {
			mcls_Lac.UnLoad();
			mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_NUC);
		}
		// NUC → LAC
		if( 1 == nDll && COMP_TYPE_LAC == emCompType ) {
			mcls_Lac.UnLoad();
			mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_LAC);
		}
	}




	// 間に合わないので黒埋めして処理を軽くして長さだけはあわせる
	if( nExecQueCnt > (prm.nLoopQueFrameSize * 0.8) ) { 
		if (COMP_TYPE_LAC == emCompType) {
			bSkip = true;
			nCompQuality = 200;
		} else if (COMP_TYPE_NUC == emCompType) {
			bSkip = true;
			nCompQuality = -100;

		} else if (COMP_TYPE_LAC5 == emCompType) {
			bSkip = true;
			nCompQuality = 100;

		} else if (COMP_TYPE_JPEG == emCompType) {
			bSkip = true;
			nCompQuality = 1;
		}
	}
	if(bSkip) {
		LOG(em_ERR), "[%s] 圧縮スキップ", my_sThreadName);
		memset(&p->pImgs[prm.nBmpHeadSize], 0x00, p->nUnionWidth * p->nUnionHeight);
	}


	// =========================================================
	// 圧縮
	BYTE* pWk = &p->pBuf[RPIFrameSizeInfo::LINK_SIZE_Cf];		// 使いやすいように画像先頭位置を取得しておく
	int	nCompSize = 0;

//if(0 == pWk ) {
//	_ASSERT(false);
//}



	//// JPEG
	if( EM_COMP_TYPE::COMP_TYPE_JPEG == emCompType) {
/***
		int retc = mcls_Jpeg.BmpToJpeg( p->nUnionWidth, p->nUnionHeight, &p->pImgs[prm.nBmpHeadSize], nCompQuality, &nCompSize, pWk);
		if( 0 != retc) {
			LOG(em_ERR), "[%s]<%d> JPEG圧縮失敗 [%d]", my_sThreadName, p->nIns, retc);
		}
***/

	//// RAW
	} else if (EM_COMP_TYPE::COMP_TYPE_RAW == emCompType) {
		nCompSize = p->nUnionWidth * p->nUnionHeight;
		memcpy( pWk, &p->pImgs[prm.nBmpHeadSize], nCompSize);


	//// BMP
	} else if (EM_COMP_TYPE::COMP_TYPE_BMP == emCompType) {
		nCompSize = p->nLen;
		memcpy( pWk, &p->pImgs[0], nCompSize);

	//// LAC
	} else if (EM_COMP_TYPE::COMP_TYPE_LAC == emCompType) {

		LacManager::LacStatus retc = mcls_Lac.LacEncode(nCompQuality, p->nLen, &p->pImgs[0], p->nLen, pWk, &nCompSize);
LOG(em_MSG), "[%s]<%d> %d LAC圧縮 [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, retc, pWk);

//LacManager::LacStatus retc = mcls_Lac.LacEncode(nCompQuality, p->nLen, &p->pImgs[0], p->nLen, m_imgwk, &nCompSize);
//LOG(em_MSG), "[%s]<%d> %d LAC圧縮 [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, retc, m_imgwk);
//memcpy(pWk, m_imgwk, nCompSize);

		if(LacManager::LacStatus::LacStatus_Ok != retc) {
			LOG(em_ERR), "[%s]<%d>(%d) LAC圧縮失敗 [%d]", my_sThreadName, p->nIns, p->nCFno, retc);
		}

	//// NUC
	} else if (EM_COMP_TYPE::COMP_TYPE_NUC == emCompType) {
		LacManager::LacStatus retc = mcls_Lac.LacEncode(nCompQuality, p->nLen, &p->pImgs[0], p->nLen, pWk, &nCompSize);
		if(LacManager::LacStatus::LacStatus_Ok != retc) {
			LOG(em_ERR), "[%s]<%d> NUC圧縮失敗 [%d]", my_sThreadName, p->nIns, retc);
		}
		//// LAC
	}
	else if (EM_COMP_TYPE::COMP_TYPE_LAC5 == emCompType) {
		if (mcls_Lac.IsGpuEncoder()) {				// GPU版LACエンコーダー生成済

			//// 入力データのメモリ領域確保処理は、スレッドループキューを確保持に実施
			//////if (p->nLen > m_GpuImgSize) {
			//////	if (m_GpuImgs) mcls_Lac.GpuFreeHost(m_GpuImgs);
			//////	m_GpuImgs = (BYTE*)mcls_Lac.GpuMallocHost(p->nLen);
			//////}
			// 入力データをLAC5用のメモリ領域にコピー(ビットマップヘッダーは不要)
			int nRetc = mcls_Lac.LacEncodeGpu(&p->pImgs[prm.nBmpHeadSize], (p->nLen-prm.nBmpHeadSize), pWk, &nCompSize);
			//LOG(em_MSG), "[%s]<%d> %d LAC5圧縮 [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, nRetc, pWk);

			//LacManager::LacStatus retc = mcls_Lac.LacEncode(nCompQuality, p->nLen, &p->pImgs[0], p->nLen, m_imgwk, &nCompSize);
			//LOG(em_MSG), "[%s]<%d> %d LAC5圧縮 [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, retc, m_imgwk);
			//memcpy(pWk, m_imgwk, nCompSize);

			if (LacManager::LacStatus::LacStatus_Ok != nRetc) {
				LOG(em_ERR), "[%s]<%d>(%d) LAC5圧縮失敗 [%d]", my_sThreadName, p->nIns, p->nCFno, nRetc);
			}

#if 0		// LAC5の解凍試験(ライブラリの試験用のため、試験完了後削除予定)
			{
				KizuBitmapManager clsBmpMgr;
				int nBmpHeadSz = clsBmpMgr.GetBitmapHeaderSize_Gray();
				BYTE* m_bOutImg = new BYTE[p->nLen + nBmpHeadSz];
				int nRetc = mcls_Lac.LacDecodeGpu(pWk, nCompSize, &m_bOutImg[nBmpHeadSz]);
				LOG(em_MSG), "[%s]<%d> %d LAC5解凍 [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, nRetc, m_bOutImg);

				clsBmpMgr.CreateBitmapHeader_Gray(m_nUionWidth, m_nUionHeight, &m_bOutImg[0], false);	// BMPヘッダー付与
				CString s;
				s.Format("c:\\decode%d.bmp", p->nIns);
				std::ofstream   os;
				os.open(s, std::ios::binary);
				os.write((const char*)m_bOutImg, p->nLen);		// BMPファイル出力
				os.close();
			}
#endif
		}
		else {
			LOG(em_ERR), "[%s]<%d>(%d) LAC5圧縮失敗 (エンコーダー生成失敗)", my_sThreadName, p->nIns, p->nCFno);
		}
	}
	
	// この時点ではセクタまでは考慮していない (=しなくて良い)
	p->nCompSize = nCompSize;
	p->nSetSize  = RPIFrameSizeInfo::LINK_SIZE_Cf + nCompSize;	


/*/
	p->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgComp_END);
	double dWk =p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgComp_END, FrameDataManager::TIME_ImgComp_START );

	LOG(em_MSG), "[%s]<%d> 圧縮完了(%dms) [%d→%d=%0.3f%%]", my_sThreadName, 
		p->nIns, (int)dWk, p->nLen, nCompSize, (double)nCompSize/(double)p->nLen*100);
//*/

	// =========================================================
	// その他データ付与
	//// 付加情報部
	if(0 == prm.nAddFrameInfoType) {
		p->pFdata		= NULL;
		p->nFdataSize	= 0;
		p->nSetSize		+= p->nFdataSize;
	} else if(1 == prm.nAddFrameInfoType) {
		p->pFdata		= &p->pBuf[RPIFrameSizeInfo::LINK_SIZE_Cf + nCompSize];	
		p->nFdataSize	= sizeof(RpiLib::T_RPI_FRAME_INFO_T1);
		p->nSetSize		+= p->nFdataSize;

		// データセット
		RpiLib::T_RPI_FRAME_INFO_T1* pF = (RpiLib::T_RPI_FRAME_INFO_T1*)p->pFdata;
		pF->nFNo		= p->UdpData.framenum;
		pF->nSpeed		= p->UdpData.speed;
		pF->vRes		= p->UdpData.vRes;
		pF->nFromLen	= p->UdpData.pos[nMen].posFromHead;
		pF->dEdgePos[0]	= p->UdpData.pos[nMen].edgePosL;
		pF->dEdgePos[1]	= p->UdpData.pos[nMen].edgePosR;
		pF->nEdgeIdx[0]	= p->nEdgeIdx[0];
		pF->nEdgeIdx[1]	= p->nEdgeIdx[1];

	}


	//// 圧縮が終わったタイミングで既にグラバーOFFになっている？
	if( ! m_bIsExec ) {
		p->bRpiSave = false;
	}


/*/
	CString s;
	s.Format("c:\\comp%d.jpg", p->nIns);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)p->pImgComp, p->nCompSize );
	os.close();
//*/
}