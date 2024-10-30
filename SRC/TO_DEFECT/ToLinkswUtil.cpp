#include "stdafx.h"
#include "ToLinkswUtil.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 使用するクラスの定義

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 静的変数の初期化

// ログ用
LogFileManager*					ToLinkswUtil::mcls_pLog = NULL;
CString							ToLinkswUtil::my_sThreadName;

// ステータス制御基底クラス
StatusManager*					ToLinkswUtil::mcls_pStatus = NULL;

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基本

//==========================================
// コンストラクタ
//------------------------------------------
// LogFileManager* pCls1	: ログクラス
// StatusManager* pCls2		: ステータス制御クラス
//==========================================
ToLinkswUtil::ToLinkswUtil(LogFileManager* pCls1, StatusManager* pCls2)
{
	my_sThreadName = "ToLinkswUtil";

	// ログクラスセット
	mcls_pLog = pCls1;

	// ステータス制御クラスセット
	mcls_pStatus = pCls2;
}

//==========================================
// デストラクタ
//==========================================
ToLinkswUtil::~ToLinkswUtil()
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 公開関数

//==========================================
// 指定判定PCのFPGA情報を取得する
//------------------------------------------
// int pcid			: PCID
// int camno		: カメラNo
// int *pFPGABoardNo: FPGAボードNo
// int *pFPGACh		: FPGAチャネルNo
//==========================================
void ToLinkswUtil::GetFPGAInfo(int pcid, int camno, int *pFPGABoardNo, int *pFPGACh)
{
	CString sWk;

	sWk.Format("CAMLINKSW_PCID%d_%d", pcid, camno);
	int nWk = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
	*pFPGABoardNo	= nWk / 100;
	*pFPGACh		= nWk % 100;
}

//==========================================
// カメラリンクスイッチ 出力情報を取得する
//------------------------------------------
// StatusManager *pStatus	: ステータス情報
// TO_LINKSW_OUTPUT *pInfo	: カメラリンクスイッチ情報
//==========================================
void ToLinkswUtil::GetOutputInfo(StatusManager *pStatus, TO_LINKSW_OUTPUT *pInfo)
{
	CString sWk;

	for (int ii = 0; ii<NUM_ALL_HT; ii++) {
		for (int jj = 0; jj<NUM_CAMLINK_BOARD; jj++) {
			int nFPGABoardNo = 0;
			int nFPGACh		 = 0;

			GetFPGAInfo(ii + 1, jj + 1, &nFPGABoardNo, &nFPGACh);
			pInfo->output[ii][jj].nFPGABoardNo	= nFPGABoardNo;
			pInfo->output[ii][jj].nFPGACh		= nFPGACh;
			pInfo->output[ii][jj].nLinkswNo		= pInfo->output[ii][jj].nFPGABoardNo / 2;
		}
	}

	// ----------------------
	// どのボードにどのカメラ画像を出力するか決定
	// ＃FPGAボードNo毎に1から採番
	int nCamset[NUM_CAMLINKSW];		// カメラセットカウンタ（CameraLinkSwitch毎）
	memset(nCamset, 0x00, sizeof(nCamset));
	int nCamNo = 0;
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		for (int jj = 0; jj < NUM_HANTEI; jj++)
		{
			int pcid = pStatus->GetHtID(ii, jj);
			if (0 >= pcid) continue;
			if (NUM_ALL_HT < pcid) continue;

			for (int kk = 0; kk < NUM_CAMLINK_BOARD; kk++)
			{
				int nCamsetIndex = pInfo->output[pcid - 1][kk].nFPGABoardNo;

				// FPGAボードNoの上限チェック
				if ((NUM_CAMLINKSW * NUM_CAMLINK_BOARD) <= nCamsetIndex) continue;

				// CameraLinkSwitch(FPGA:0,1)内で通番(1～8)
				nCamsetIndex = nCamsetIndex / 2;

				// 上限値：８
				// ＃超える場合は強制的に８を設定
				nCamset[nCamsetIndex] = (NUM_CAMLINKSW_CAM_MAX <= nCamset[nCamsetIndex] ? NUM_CAMLINKSW_CAM_MAX : nCamset[nCamsetIndex] + 1);
				pInfo->output[pcid - 1][kk].nCamPort= nCamset[nCamsetIndex];
				pInfo->output[pcid - 1][kk].nCamset	= nCamNo + 1;
				nCamNo++;
			}
		}
	}
}

//==========================================
// 接続済みのカメラリンクスイッチ 出力情報を取得する
//------------------------------------------
// StatusManager *pStatus	: ステータス情報
// TO_LINKSW_OUTPUT *pInfo	: カメラリンクスイッチ情報
//==========================================
void ToLinkswUtil::GetDeterminedHanteiPC(StatusManager *pStatus, TO_LINKSW_OUTPUT *pInfo)
{
	GetOutputInfo(pStatus, pInfo);
		
	// ----------------------
	// 未接続の情報（バックアップPC）をクリアする
	// ＃DB登録対象外
	bool bEnablePC[NUM_ALL_HT];
	for (int ii = 0; ii < NUM_ALL_HT; ii++) bEnablePC[ii] = false;
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		for (int jj = 0; jj < NUM_HANTEI; jj++)
		{
			int pcid = pStatus->GetHtID(ii, jj);
			if (0 >= pcid) continue;
			if (NUM_ALL_HT < pcid) continue;
			bEnablePC[pcid - 1] = true;
		}
	}
	for (int ii = 0; ii < NUM_ALL_HT; ii++)
	{
		if (!bEnablePC[ii])
		{
			for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
			{
				pInfo->output[ii][jj].nFPGABoardNo	= 0;
				pInfo->output[ii][jj].nFPGACh		= 0;
			}
		}
	}
}

//==========================================
// FPGA番号、カメラセットから出力ポートを割り出す
//------------------------------------------
// TO_LINKSW_OUTPUT *pInfo	: カメラリンクスイッチ情報
// long nFPGABoardNo		: FPGA番号
// long nCamno				: カメラセットNo
//==========================================
int ToLinkswUtil::GetTargetCamNo(const TO_LINKSW_OUTPUT *pInfo, long nFPGABoardNo, long nCamno)
{
	for (int ii = 0; ii < NUM_ALL_HT; ii++)
	{
		if (0 == pInfo->output[ii][0].nFPGACh) continue;

		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			if (nFPGABoardNo == pInfo->output[ii][jj].nFPGABoardNo &&
				nCamno == pInfo->output[ii][jj].nCamset)
			{
				return pInfo->output[ii][jj].nCamPort;
			}
		}
	}

	return -1;
}

//==========================================
// 面、判定PCNoからカメラリンクスイッチグループを割り出す
//------------------------------------------
// int nMen	   : 表裏
// int nHantei : 判定No
// int nCam    : カメラNo
//==========================================
int ToLinkswUtil::GetHanteiGroup(int nMen, int nHantei, int nCam)
{
	CString sWk;
	int nPcID = mcls_pStatus->GetHtID(nMen, nHantei);
	int nGroupID = -1;

	if (0 != nPcID)
	{
		sWk.Format("CAMLINKSW_PCID%d_%d", nPcID, nCam);
		int nWk = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
		nGroupID = (nWk / 100) / 2;
	}

	return nGroupID;
}