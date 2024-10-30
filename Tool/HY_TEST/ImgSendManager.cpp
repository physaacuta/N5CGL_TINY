#include "StdAfx.h"
#include "ImgSendManager.h"
#include "time.h"


ImgSendManager::ImgSendManager(void) :
ThreadManager("ImgSend")
{
}

ImgSendManager::~ImgSendManager(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// スレッド
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ImgSendManager::ThreadFirst()
{
	//// 事前準備

	//// イベント設定
	HANDLE hArray[] = { gque_Deli.g_evSem,
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}


//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int ImgSendManager::ThreadLast()
{
		DELIVERY_KEY *pDeli;				// キューデータ

	// 未処理のキューを破棄
	while(true) {
		pDeli = gque_Deli.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// 削除
		LOG(em_WAR), "[%s] 未処理キュー削除 [%s]", my_sThreadName, pDeli->cKizukenNo);
		SAFE_DELETE(pDeli);
	}

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ImgSendManager::ThreadEvent(int nEventNo)
{
	DELIVERY_KEY*	pDeli;			// 受け渡し依頼データ

	////// シグナルの条件
	enum {	EV_QUE = 0,						// キュー依頼
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:
		if(true) {
			//LOG(em_MSG),"[%s] Start ---->", my_sThreadName);
			pDeli = gque_Deli.GetFromHead(); 
			SetDefectInfo( pDeli );
			SAFE_DELETE(pDeli);
			//LOG(em_MSG),"[%s] <----- End", my_sThreadName);
		}
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 1.疵情報(疵画像) セット・送信
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void ImgSendManager::SetDefectInfo(DELIVERY_KEY* pDeli)
{
	CString cfileName[MAX_CAMANGLE];	// ファイルパス
	CFile cfile[MAX_CAMANGLE];			// ファイルオブジェクト
	bool bfile[MAX_CAMANGLE];			// オープン結果
	long nFileLen[MAX_CAMANGLE];		// ファイルサイズ
	static char imageBuff[2000000];		// 画像格納バッファ
	long nImageSize;

	for(int ii=0 ; ii < MAX_CAMANGLE; ii++){
		nFileLen[ii] = 0;
		bfile[ii] = false;
	}

	//// 送信データ生成 (送信先で開放する事) 
		
	nImageSize = 0;
	for (int jj = 0; jj< MAX_CAMANGLE; jj++) {
		if( (pDeli->nTid % 3) + 1 == 1 ){
			switch (jj) {
			case 0:
				cfileName[jj] = ".\\疵01_1.jpg";
				break;
			case 1:
				cfileName[jj] = ".\\疵01_2.jpg";
				break;
			case 2:
				cfileName[jj] = ".\\疵01_3.jpg";
				break;
			case 3:
			default:
				cfileName[jj] = ".\\疵01_4.jpg";
				break;
			}

		} else if ((pDeli->nTid % 3) + 1 == 2) {
			switch (jj) {
			case 0:
				cfileName[jj] = ".\\疵02_1.jpg";
				break;
			case 1:
				cfileName[jj] = ".\\疵02_2.jpg";
				break;
			case 2:
				cfileName[jj] = ".\\疵02_3.jpg";
				break;
			case 3:
			default:
				cfileName[jj] = ".\\疵02_4.jpg";
				break;
			}

		} else {
			switch (jj) {
			case 0:
				cfileName[jj] = ".\\疵1.jpg";
				break;
			case 1:
				cfileName[jj] = ".\\疵2.jpg";
				break;
			case 2:
				cfileName[jj] = ".\\疵3.jpg";
				break;
			case 3:
			default:
				cfileName[jj] = ".\\疵4.jpg";
				break;
			}
		}

		if ("" == cfileName[jj]) {
			nFileLen[jj] = 0;
			nImageSize += nFileLen[jj];
		} else {
			if(cfile[jj].Open(cfileName[jj], 0, 0)) {
				bfile[jj] = true;
				nFileLen[jj] = cfile[jj].GetLength();
				nImageSize += nFileLen[jj];
			}
		}
	}

	// 送信データサイズを求める
	int nSendSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_DETECT) + nImageSize;

	// BYTE配列を作成する
	BYTE* pSend = new BYTE[nSendSize];
	memset(pSend, 0x00, nSendSize);

	// ヘッダ部先頭アドレスをセットする
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];

	// データ部先頭アドレスをセットする
	TO_SO_DATA_DETECT& data = (TO_SO_DATA_DETECT&)pSend[sizeof(TO_SO_HEAD)];

	// ベースヘッダ部
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;
	head.base_head.subinf = pDeli->nSysKbn;
	// 拡張ヘッダ部
	memcpy( head.KizukenNo, pDeli->cKizukenNo , sizeof(head.KizukenNo)); //★ 管理No

	head.men = pDeli->nMen;									//★ 表裏区分 (0:表 1:裏)
	head.kind = TO_SO_DATA_KIND_DETECT;						// 疵情報(疵情報+疵画像)

	// データ部
	for (int jj = 0; jj< MAX_CAMANGLE; jj++) {
		data.image_len[jj] = nFileLen[jj];			//★ 画像データ長 (ひとつ分の画像サイズ) (0:疵無し, -1:探傷していない, 0<疵有り)
	}

	data.datas.TorB = pDeli->nMen;				//★ 表裏区分 (0:表 1:裏)
	data.datas.nTid = pDeli->nTid;				//★ 疵種EdasysID
	data.datas.nGid = pDeli->nGid;				//★ グレードEdasysID

	int dsws = (pDeli->dDS > pDeli->dWS ? 1 : 0 );
	data.datas.dsws = dsws;						// DSWS区分 0:DS 1:WS
	if ( 0 == pDeli->nMen ) {
		data.datas.camset = rand() % 2;			// カメラペアID (0～1)
	} else {
		data.datas.camset = rand() % 2 + 2;		// カメラペアID (2～3)
	}

	data.datas.nKizuNo = pDeli->nKizuSeq;					// 疵No
	data.datas.y = float(pDeli->nInY);						//★ Tからの位置[mm]
	data.datas.y_out = float(pDeli->nInY);					//★ Tからの位置[mm](出側) テストツールではyと同じ値をセットする
	data.datas.ds = pDeli->dDS;							// DS側エッジからの位置[mm]
	data.datas.ws = pDeli->dWS;							// WS側エッジからの位置[mm]
	data.datas.dsc = (pDeli->dDS + pDeli->dWS) / 2 - pDeli->dDS;		// 板センターからの距離[mm]
	data.datas.wsc = (pDeli->dDS + pDeli->dWS) / 2 - pDeli->dWS;		// 板センターからの距離[mm]
	data.datas.speed = pDeli->nSpeed;						// 速度[mpm]
	data.datas.cycle_div = rand()%2;						// 周期性(0:一般疵, 1:簡易周期疵,連続疵 )
	if (1 == pDeli->nSysKbn)	data.datas.cycle_div = 2;	// 周期疵区分(0:一般疵, 1:簡易周期疵 2:周期疵)
	data.datas.pitch = rand()%200;						// ピッチ[mm]
	data.datas.roolkei = 1;								// ロール径[mm]
	data.datas.nFrameNo = 1;								// フレームNo

	// 疵外接をランダムで決定し、疵中心位置、疵幅、疵長さ、面積を計算する
	int xmin = rand()%100 + 1;								// 外接Xmin[pixel]決定
	int	xmax = xmin + rand()%150 +  5;						// 外接Xmax[pixel]決定
	int ymin = rand()%100 + 1;								// 外接Ymin[pixel]決定
	int ymax = ymin + rand()%150 +  5;						// 外接Ymax[pixel]決定
	float h_res = 0.5;										// 縦分解能[mm/pixel]決定
	float w_res = 0.6;										// 横分解能[mm/pixel]決定

	data.datas.h_res = h_res;								// 縦分解能[mm/pixel]
	data.datas.w_res = w_res;								// 横分解能[mm/pixel]

	data.datas.box_x_min = xmin;							// 外接Xmin[pixel]
	data.datas.box_x_max = xmax;							// 外接Xmax[pixel]
	data.datas.box_y_min = ymin;							// 外接Ymin[pixel]
	data.datas.box_y_max = ymax;							// 外接Ymax[pixel]

	data.datas.center_x = int((xmax - xmin)/2) + xmin;		// 疵中心位置X [pixel]
	data.datas.center_y = int((ymax - ymin)/2) + ymin;		// 疵中心位置Y [pixel]
	data.datas.kizu_wid = int( (xmax-xmin) * h_res);		// 疵幅[mm]
	data.datas.kizu_len = int( (ymax-ymin) * w_res);		// 疵長さ[mm]
	if (pDeli->nArea) {
		data.datas.aria = pDeli->nArea;
	} else {
		data.datas.aria = (float)data.datas.kizu_wid*data.datas.kizu_len;	// 面積[mm^2]
	}

	if( (pDeli->nTid % 3) + 1 == 2 ){
		data.datas.h_size = 256;							// 縦画素数[pixel]	※サイズは実際の画像サイズから決定すること
		data.datas.w_size = 256;							// 横画素数[pixel]	※サイズは実際の画像サイズから決定すること
	}else{
		data.datas.h_size = 256;							// 縦画素数[pixel]	※サイズは実際の画像サイズから決定すること
		data.datas.w_size = 256;							// 横画素数[pixel]	※サイズは実際の画像サイズから決定すること
	}

	data.datas.base_box_x_min = xmin;						// 元画像_外接Xmin[pixel]
	data.datas.base_box_x_max = xmax;						// 元画像_外接Xmax[pixel]
	data.datas.base_box_y_min = ymin;						// 元画像_外接Ymin[pixel]
	data.datas.base_box_y_max = ymax;						// 元画像_外接Ymax[pixel]

	data.datas.base_center_x = data.datas.center_x;			// 元画像_疵中心位置X[pixel]
	data.datas.base_center_y = data.datas.center_y;			// 元画像_疵中心位置Y[pixel]

	data.datas.base_h_size = 256;							// 元画像_縦画素数[pixel]
	data.datas.base_w_size = 256;							// 元画像_横画素数[pixel]

	data.datas.base_h_res = h_res;							// 元縦分解能[mm/pixel]
	data.datas.base_w_res = w_res;							// 元横分解能[mm/pixel]
	data.datas.nShrinkRatio = 4;							// 縮小率

	data.datas.nTYuu = mcls_pParam->GetYusenT(pDeli->nTid);	// 疵種優先度 (1最強)
	data.datas.nGYuu = mcls_pParam->GetYusenG(pDeli->nGid);	// グレード優先度 (1最強)
	data.datas.nWidDiv = pDeli->x;							// 幅分割区分 テストツールで決定した幅分割位置を設定する
	data.datas.nObjTansho = pDeli->nObjTansho;				// 探傷画面出力対象疵(1:対象)
	data.datas.nObjDeflist = pDeli->nObjDeflist;			// 重欠陥一覧画面対象疵(1:対象)
	data.datas.nModeDeflist = pDeli->dspmode;				// 重欠陥一覧強調表示対象疵(1:対象)

	SYSTEMTIME time;
	GetLocalTime(&time);
	data.datas.time[0] = time.wYear;						// 発生時刻
	data.datas.time[1] = time.wMonth;						// 発生時刻
	data.datas.time[2] = time.wDay;							// 発生時刻
	data.datas.time[3] = time.wHour;						// 発生時刻
	data.datas.time[4] = time.wMinute;						// 発生時刻
	data.datas.time[5] = time.wSecond;						// 発生時刻

	// 画像データセット
	int nImageSize2 = 0;
	for (int jj = 0; jj < MAX_CAMANGLE; jj++ ) {
		
		// 画像サイズが0の場合はセットしない（できない）
		if (0 == nFileLen[jj]) continue;
		if (!bfile[jj]) continue;

		memset(imageBuff, 0x00, sizeof(imageBuff));
		cfile[jj].Read(&imageBuff, sizeof(imageBuff));
		memcpy(&data.image[nImageSize2], imageBuff, nFileLen[jj]);
		nImageSize2 += nFileLen[jj];
	
	}

	// 表示管理に送信
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);

	// 開いていたファイルをクローズする
	for (int jj = 0; jj < MAX_CAMANGLE; jj++ ) {
		if (!bfile[jj]) continue;
		cfile[jj].Close();
	}
}

