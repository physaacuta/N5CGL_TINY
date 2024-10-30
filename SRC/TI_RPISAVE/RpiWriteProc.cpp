#include "StdAfx.h"
#include "RpiWriteProc.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------




//// ソートの比較関数
static inline bool comp_sd_fd(const double &a, const double &b)
{
	return a < b;
}




//------------------------------------------
// コンストラクタ
// int nIns インスタンスNo
// int nColor 色番号
//------------------------------------------
RpiWriteProc::RpiWriteProc(int nIns, int nColor) :
RpiFileManager( GetThreadNameArray2("RpiWP", nIns, nColor) ),
m_nIns(nIns),
m_nColor(nColor),
m_bIsExec(false)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
RpiWriteProc::~RpiWriteProc(void)
{
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int RpiWriteProc::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { this->mque_pIn->GetEvQue()
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int RpiWriteProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void RpiWriteProc::ThreadEvent(int nEventNo)
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
// 実処理
// Tin* pIn		入力Queデータ
// 戻り値		NULL:返却しない 出力Queデータ:次のスレッドへ返却する場合  
//------------------------------------------
FrameDataManager* RpiWriteProc::OnAddNewItem( DELIVERY_RPIWRITE* p )
{
	if( NULL == p ) {							// 通常ありえないはず
		LOG(em_ERR), "[%s] スレッドループキュー無し!", my_sThreadName);
		return NULL;
	}

	//// 処理
	FrameDataManager*	pFdAns = NULL;			// 返却用
	switch (p->emNo) {
//-------------------------------------------------------------------
	case em_FRAME_INFO:							// フレーム情報書込み
		//// この処理では、 4096*2048 * 4 で、20msec程度で終わる。  だから並列処理は必要ないかなー
		if(m_bIsExec) {
			Exec_WriteFrame( (FrameDataManager*)p->data );
		} else {
			//LOG(em_WAR), "[%s] キャンセル", my_sThreadName);
			// 開いていたら一度閉じる
			if( IsOpen() ) Exec_FileClose(NULL);
		}
		//// 次へ
		pFdAns = (FrameDataManager*)p->data;
		break;

//-------------------------------------------------------------------
	case em_WRITE_END:							// コイル単位の書込み完了
		Exec_FileClose();
		break;

//-------------------------------------------------------------------
	default :
		// ありえない！！
		break;
	}

	//// 解放
	delete p;

	// 次へ
	return pFdAns;		// NULLのときは次へ行かない。=行く必要がないもの
}

//------------------------------------------
// 書込み処理処理
// FrameDataManager* p フレーム情報
//------------------------------------------
void RpiWriteProc::Exec_WriteFrame(FrameDataManager* p)
{
	// そもそも保存対象じゃない。通常無いはずだが、グラバー停止したときの残骸でくるかも
	if( ! p->bRpiSave ) {	// △△△
		LOG(em_WAR), "[%s] キャンセル", my_sThreadName);

		// 開いていたら一度閉じる
		if( IsOpen() ) Exec_FileClose(p);
		return;
	}


	//-----------------------
	// ファイル切替判定
	if( IMG_SAVE_STAT_CHANGE == p->emImgSaveStat || 
		IMG_SAVE_STAT_F == p->emImgSaveStat ) {
		
		// 開いていたら一度閉じる
		if( IsOpen() ) Exec_FileClose(p);
	}


	//-----------------------
	// ファイルオープンチェック
	if( ! IsOpen() ) {
		// コイル単位の情報をセット (コイル先頭)
		SetData_CoilInfo(p);
		// RPIファイルオープン
		RpiOpen();
	}

//LOG(em_MSG), "[%s] RPI書き込み (%d/%d)(%s)", my_sThreadName, p->nNo, p->nCFno, my_sFileName);

	//-----------------------
	// フレーム単位の情報セット
	SetData_FrameInfo(p);


	//-----------------------
	//// 書込み
	p->cls_Time.PutTimestamp(FrameDataManager::TIME_RpiWrite_START);

	p->nSetSize = GET_SIZE_JUST( p->nSetSize, my_Setting.nSecSize);		// セクタ単位にあわせる
	this->WriteFrame(p->nCompSize, p->nSetSize, p->pBuf );

	p->cls_Time.PutTimestamp(FrameDataManager::TIME_RpiWrite_END);

	//-----------------------
	// ファイルクローズ
	if( IMG_SAVE_STAT_T == p->emImgSaveStat ) {
		Exec_FileClose(p);
	}
}

//------------------------------------------
// 終了処理
// FrameDataManager* p フレーム情報 (NULLでもOK)
//------------------------------------------
void RpiWriteProc::Exec_FileClose(FrameDataManager* p)
{
	if( IsOpen() ) {
		if( NULL != p ) SetData_CoilResult(p);
		this->RpiClose();
		LOG(em_MSG), "[%s]<%d> RPIファイルクローズ完了(%s)", my_sThreadName, m_nIns, my_sFileName);
	}
}

//------------------------------------------
// リセット
//------------------------------------------
void RpiWriteProc::Reset()
{

}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 仮想関数

//------------------------------------------
// ファイル名決定
// RpiHeader const& h 今回のヘッダー情報
// CString* strName 決定したファイル名
//------------------------------------------
void RpiWriteProc::OnSetFileName(RpiHeader const& h, RPI_SETTING const& s, RPI_COIL_INFO const& c, CString* strName)
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();

	const T_RPI_HEADER_BASE &tH =h.theader;

	// TEX製RPIファイル命名規則
	// [疵検管理No]@men=[表裏]@Angle=%d@No=[コイル単位のファイル分割No]@以降なんでもOK

	if( COLOR_TYPE_GRAY == prm.emColorType ) {
		// グレー
		// [疵検管理No]@men=[表裏]@Angle=%d@No=[コイル単位のファイル分割No]@コイルNo
		strName->Format("%s@men=%d@Angle=%d@No=%d@%s.%s", tH.cKizukenNo, tH.nMen, tH.nCamAngle, tH.nFileNum, c.cCoilNo, RPI_TYPE_WK );
	} else if( COLOR_TYPE_COLOR == prm.emColorType || COLOR_TYPE_SA == prm.emColorType ) {
		// 色差
		// [疵検管理No]@men=[表裏]@Color=%d@No=[コイル単位のファイル分割No]@コイルNo
		strName->Format("%s@men=%d@Color=%d@No=%d@%s.%s", tH.cKizukenNo, tH.nMen, tH.nCamAngle, tH.nFileNum, c.cCoilNo, RPI_TYPE_WK );
	}
}
//------------------------------------------
// RPIファイル生成完了
// char const* strPath 対象のフォルダパス (最後の\はなし)
// CString* strName 対象のファイル名
//------------------------------------------
void RpiWriteProc::OnRpiFileSave(char const* strPath, CString* strName)
{
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ m_nIns ];

	//// 拡張性変更
	// .rwk → .rpi
	CString sWk = *strName;
	sWk.Replace(RPI_TYPE_WK, RPI_TYPE);

	CString sOldPath, sNewPath;
	sOldPath.Format("%s\\%s", strPath, *strName);
	sNewPath.Format("%s\\%s", strPath, sWk);
	rename(sOldPath, sNewPath);

	if(!m_bIsExec) return;	// 停止時は、やらない (コピー中にタスクが落ちたら、次回コピー失敗となるため)


	//// 移動用 データ作りこみ
	RPI_MOVE* pDeli = new RPI_MOVE;
	
	pDeli->sName	= sWk;
	pDeli->sPath_F	= strPath;
	pDeli->nIns		= m_nIns;

	//// キューに追加
	if( ! mque_pRpiMove->AddToTailFreeCheck(pDeli, 1) ) {
		delete pDeli;

		LOG(em_ERR), "[%s] キューフル！！(RPIファイル移動)", my_sThreadName);
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// その他

//------------------------------------------
// 設定情報 セット (検査開始時)
//------------------------------------------
bool RpiWriteProc::SetData_Setting()
{
	ParamManager::P_MAIN		 const& prm	 = mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ m_nIns ];


	//// セクタサイズ等算出
	int		nSecSize;
	int		nWriteMode;										// 書込みモード (0:同期書込みモード 1:セクタ書込みモード)
	if(0==strlen(prm.cWriteSecPath))	{
		nSecSize	= 512;									// 512バイト単位固定
		nWriteMode	= 0;
	} else {
		nSecSize	= GetSecSize(prm.cWriteSecPath);		// セクタ単位固定
		nWriteMode	= 1;
	}

	//////////////////////////////////////
	// ヘッダー情報生成
	//////////////////////////////////////
	 
	// そもそもファイルを作りこむため用 --------
	my_Setting.nFastFrame		= 0;//5;				// 最初のコイル単位のフレーム格納数 (0は、1コイル1ファイル。1～は、その枚数になったら次ファイルに)
	my_Setting.nCycleFrame		= 0;//10;				// ２回目以降のフレーム格納数 
	
	for(int ii=0; ii<prmI.nRpiFolderBufNum; ii++) {
		if( RpiFileManager::MAX_SAVE_FOLDER <= ii ) break;
		strcpy(my_Setting.cFolderPath[ii], prmI.cRpiFolderBuf[ii]);		// フォルダパス (最後の\は無し)
		my_Setting.nFolderNum = ii+1;
	}

	my_Setting.nWriteMode		= nWriteMode;			// 書込みモード (0:同期書込みモード 1:セクタ書込みモード)
	my_Setting.nSecSize			= nSecSize;				// セクタサイズ (分からない人は1で良い)


	// 基本部ヘッダ用 --------
	my_Setting.width			= prmI.nUionWidth;		// 画像幅 (8の倍数)
	my_Setting.height			= prmI.nUionHeight;		// 画像高さ (8の倍数)
	my_Setting.reso_x			= prmI.reso_x;			// 横分解能
	my_Setting.reso_y			= prmI.reso_y; //(float)mcls_pParam->CalcResV();		// 縦分解能 


	// TEXヘッダ用 --------
	strcpy(my_Setting.cSysName, prm.cSystemName );		// システム略称
	my_Setting.nLineCenter		= prmI.nLineCenter;		// ラインセンター画素(連結画像左端からのオフセット値) [pixel]
	my_Setting.nStartCamset		= prmI.nStartCamset;	// 左端カメラセット番号 (1オリジン)
	my_Setting.nCamsetNum		= prmI.nCamsetNum;		// 幅方向フレーム結合数 (1オリジン)
	if( COLOR_TYPE_GRAY == prm.emColorType ) {
		my_Setting.nCamAngle	= prmI.nCamAngle;		// カメラ角度 (1オリジン)
	} else if( COLOR_TYPE_COLOR == prm.emColorType || COLOR_TYPE_SA == prm.emColorType ) {
		my_Setting.nCamAngle	= m_nColor + 1;			// 色画像 (1オリジン)
	}
	my_Setting.nMen				= prmI.nMen;			// 表裏区分 (0:表 1:裏)
	my_Setting.nColorType		= prm.emColorType;		// 画像タイプ (0:グレー画像  1:カラー画像  2:色差画像)
	my_Setting.nSysType			= prm.emSysType;		// 疵検タイプ (0:連続ライン  1:バッチライン)


	// フレーム付加情報
	my_Setting.frame_info_type	= prm.nAddFrameInfoType;			// フレーム情報部の格納形式 
	if(0 == prm.nAddFrameInfoType) {
		my_Setting.frame_info_size	= 0;
	} else if(1 == prm.nAddFrameInfoType) {
		my_Setting.frame_info_size	= sizeof(T_RPI_FRAME_INFO_T1);	// フレーム情報部の格納サイズ 
	}


	//// その他


	return true;
}

//------------------------------------------
// コイル単位の設定情報 セット (先端)
// FrameDataManager* p フレーム情報
//------------------------------------------
bool RpiWriteProc::SetData_CoilInfo(FrameDataManager* p) 
{
	ParamManager::P_MAIN		 const& prm	 = mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ m_nIns ];
	ParamManager::P_PARAM_CAMSET const&	pDb  = mcls_pParam->GetParamCamSet();


	switch(p->emCompType){
	case COMP_TYPE_JPEG:	my_Setting.codec= RpiHeader::CODEC_JPEG; break;
	case COMP_TYPE_LAC:		my_Setting.codec= RpiHeader::CODEC_LAC; break;
	case COMP_TYPE_RAW:		my_Setting.codec= RpiHeader::CODEC_RAW; break;
	case COMP_TYPE_BMP:		my_Setting.codec= RpiHeader::CODEC_BMP; break;
	case COMP_TYPE_NUC:		my_Setting.codec= RpiHeader::CODEC_NUC; break;
	case COMP_TYPE_LAC5:	my_Setting.codec= RpiHeader::CODEC_LAC5; break;
	default:		my_Setting.codec= RpiHeader::CODEC_UNKNOWN; break;
	}


	//// コイル単位で変更される情報をセット
	memset(&my_CoilInfo, 0x00, sizeof(my_CoilInfo));
	my_CoilInfo.reso_y	  = my_Setting.reso_y;			// 実際に分解能を書き込みのはCoilInfoの方！
	my_CoilInfo.nCFNo	  = p->UdpData.framenum;
	if( p->bDummyCoil ) {
		my_CoilInfo.nStartLen = false;						// ダミーコイル
	} else {
		my_CoilInfo.nStartLen = p->UdpData.pos[ prmI.nMen ].posFromHead;						// コイル切替時のフレーム先頭位置 [mm] (下流面は0じゃない)
	}
	memcpy(my_CoilInfo.cKizukenNo, p->cKizukenNo, SIZE_KIZUKEN_NO );
	strcpy(my_CoilInfo.cCoilNo, p->cCoilNo);

	for(int jj=0; jj<prmI.nCamsetNum; jj++) {
		my_Setting.caminfo[jj].dBaseXoffset	= (float)pDb.ins[m_nIns].camset[jj].dXoffset;	// 基準位置。ラインセンターからの各カメラ左端の位置(左方向は、マイナス値) [mm] (PARAM_CAMSETのXoffset値そのもの)
		my_Setting.caminfo[jj].nBaseLeft	= prmI.camset[jj].nBaseLeft;					// 基準画素。結合後のフレーム左端から自カメラ画像左端までの距離
		my_Setting.caminfo[jj].nOverLap		= prmI.camset[jj].nOverLap;						// 次カメラとの右側のオーバーラップ量
	}
	
	//if( 1 == p->nFTset ) {
	//	my_CoilInfo.nFTposCfNo[0]  = p->nFTposCfNo;
	//	my_CoilInfo.nFTposPixel[0] = p->nFTposPixel;
	//}

	//if( SYS_TYPE_BAT == prm.emSysType ) {
	//	my_CoilInfo.nFTposCfNo[0]  = p->nFTposCfNo;
	//	my_CoilInfo.nFTposPixel[0] = p->nFTposPixel;
	//}
	return true;
}

//------------------------------------------
// コイル単位の実績情報 セット (尾端)
// FrameDataManager* p フレーム情報
//------------------------------------------
bool RpiWriteProc::SetData_CoilResult(FrameDataManager* p) 
{
	ParamManager::P_MAIN		 const& prm	 = mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ m_nIns ];
	ParamManager::P_PARAM_CAMSET const&	pDb  = mcls_pParam->GetParamCamSet();

	return true;
}

//------------------------------------------
// フレーム単位の設定情報 セット
// FrameDataManager* p フレーム情報
//------------------------------------------
bool RpiWriteProc::SetData_FrameInfo(FrameDataManager* p) 
{
	// 毎フレームセットされる情報をセット
	my_FrameInfo.nFNo = p->UdpData.framenum;


	//if( 1 == p->nFTset ) {
	//	my_CoilInfo.nFTposCfNo[0]  = p->nFTposCfNo;
	//	my_CoilInfo.nFTposPixel[0] = p->nFTposPixel;
	//}
	//if( 2 == p->nFTset ) {
	//	my_CoilInfo.nFTposCfNo[1]  = p->nFTposCfNo;
	//	my_CoilInfo.nFTposPixel[1] = p->nFTposPixel;
	//}

//	p->nCFno = p->UdpData.framenum - my_CoilInfo.nCFNo; 


	return true;
}


	
