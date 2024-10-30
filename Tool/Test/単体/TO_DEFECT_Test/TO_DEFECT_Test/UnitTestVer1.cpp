// *********************************************************************************
//	5CGL極微小φω 統括管理 単体テストシナリオ
//	[Ver]
//		Ver.01    2007/10/11  DB変更分のDBアクセステストを実装
//
//	[メモ]
//　　　5CGL極微小初回開発分のシナリオ定義を本クラスに定義する
//　　　単体テスト仕様書：02_01 TO_DEFECT.xls
//		テストケース　　：UnitTest1：No.1, 2
//		　　　　　　　　：UnitTest2：No.3 - 16
//		　　　　　　　　：UnitTest3：No.17
// *********************************************************************************
#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// テストターゲット用定義
#include <direct.h>
#include "..\..\..\..\Library\KizuLib\KizuLib.h"
#include "..\..\..\..\Library\KizuLib\Class\KizuMacro.h"
#include "..\..\..\..\Library\KizuLibMFC\Class\LogFileManager.h"
#include "..\..\..\..\SRC\TO_DEFECT\ParamManager.h"
#include "..\..\..\..\SRC\TO_DEFECT\StatusManager.h"
#include "..\..\..\..\SRC\TO_DEFECT\DBManager.h"
#include "..\..\..\..\SRC\TO_DEFECT\MaeManager.h"

using namespace KizuLib;

namespace TO_DEFECT_Test
{		
	LogFileManager*	mcls_pLog = NULL;

	TEST_MODULE_INITIALIZE(ModuleInitialize)
	{
		mcls_pLog = new LogFileManager(getmytaskname());
	}

	TEST_MODULE_CLEANUP(ModuleCleanup)
	{
		if (NULL != mcls_pLog)
		{
			delete mcls_pLog;
			mcls_pLog = NULL;
		}
	}

	TEST_CLASS(UnitTest1)
	{
	private:
		ParamManager *mcls_pParam = NULL;
		StatusManager	mcls_Status;
		DBManager mcls_DB;
		MaeManager	mcls_Mae;

		// 仮管理No生成
		void GetCurrentTimeString(LPCSTR lpPrefix, char *buf)
		{
			CString str;
			SYSTEMTIME time;

			GetLocalTime(&time);
			str.Format("%s%02d:%02d:%02d.%03d",
				lpPrefix, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
			strcpy(buf, str.operator LPCSTR());
		}

		// SQL依頼
		void ExecSQL(DBManager::DeliveryDB *deliDB)
		{
			if (!mcls_DB.gque_AddSQLData.AddToTail(deliDB, 20, 20))
			{
				LOG(em_ERR), "DB登録依頼キューフル [DBDATA_COIL_INF]");
				delete deliDB->data;
				delete deliDB;
			}

		}

		// テストデータセット（コイル情報）
		void SetInfo(DBDATA_COIL_INF* pWk)
		{
			strcpy(pWk->cCoilKey, "1");			// キー情報
			strcpy(pWk->cMeiNo, "2");			// 命令No
			strcpy(pWk->cCoilNo, "3");			// 生産No
			pWk->nSendPoint = 4;				// 送信位置
			pWk->nLenIn = 5;					// コイル長
			pWk->nAtuZai = 6;					// 材料板厚
			pWk->nWidZai = 7;					// 材料板幅
			pWk->nAtuSei = 8;					// 注文板厚
			pWk->nWidSei = 9;					// 注文板幅
			pWk->nWidTrim = 10;					// 現工程トリム幅
			pWk->nNobiSPM = 11;					// SPM伸び率
			pWk->nNobiTLV = 12;					// TLV伸び率
			pWk->nCutF = 13;					// 入側におけるフロント端板カットの実績長
			pWk->nCutT = 14;					// 入側におけるテール端板カットの実績長
			pWk->nSPMRoolKei_Up = 15;			// SPMワークロール径 (上)
			pWk->nSPMRoolKei_Down = 16;			// SPMワークロール径 (下)
			strcpy(pWk->cKenKijun_O, "1");		// 検査基準 表
			strcpy(pWk->cKenKijun_U, "2");		// 検査基準 裏
			strcpy(pWk->cKurometo_Kind, "3");	// クロメート 実績種類
			strcpy(pWk->cKurometo_Umu, "4");	// クロメート 実績有無
			strcpy(pWk->cHinCode, "5");			// 品種
			strcpy(pWk->cMekkiKind, "6");		// メッキ種類
			strcpy(pWk->cSyuZai, "7");			// 主材質
			strcpy(pWk->cDifMaker, "8");		// ディファレンシャルマーカー
			strcpy(pWk->cKenban, "9");			// 原板区分
			strcpy(pWk->cMekkiCycle, "0");		// メッキサイクル
			strcpy(pWk->cInLineSPM, "1");		// レベラー インラインSPM
			strcpy(pWk->cKagakuToyu, "2");		// 化学処理塗油
			strcpy(pWk->cAtukkaSPM, "3");		// オフラインSPM圧下率
			strcpy(pWk->cMekkiKubun, "4");		// メッキ区分
			strcpy(pWk->cOmoteSyori1, "5");		// 表面処理1桁目
			strcpy(pWk->cOmoteSyori2, "6");		// 表面処理2桁目
			strcpy(pWk->cToyu, "7");			// 塗油
			strcpy(pWk->cKouteiYobi, "8");		// 工程記号予備
			strcpy(pWk->cBaund, "9");			// バウンダリ合わせ
			strcpy(pWk->cCaplSyuZai, "0");		// CAPL 主材質
			strcpy(pWk->cMetuke, "1");			// 目付規格
			strcpy(pWk->cNi, "2");				// Ni付着量
			strcpy(pWk->cSodoTCM, "3");			// TCMロール粗度
			strcpy(pWk->cSpmUmu, "4");			// SPM圧下有無
			strcpy(pWk->cNextLine, "5");		// 計画次工程コード
			strcpy(pWk->cMuke, "6");			// 向先
			strcpy(pWk->cYouto, "7");			// 用途
			strcpy(pWk->cCaplSiage, "8");		// CAPL表面仕上げ
			strcpy(pWk->cCaplSanUmu, "9");		// CAPL酸洗有無
			strcpy(pWk->cHinMode, "0");			// 品種区分
			strcpy(pWk->cGaCrKubun, "1");		// GA/CR区分
			strcpy(pWk->cSpmJisekiUmu, "2");	// SPM圧下実績有無
			strcpy(pWk->cKouteiCode, "3");		// 工程コード
		}

		// テストデータセット（コイル実績）
		void SetInfo(DBDATA_COIL_RESULT* pWk)
		{
			pWk->nEndMode = 1;									// 実績格納区分
			pWk->nSt_Hantei[0] = 2;								// 判定状態
			pWk->nSt_Hantei[1] = 3;								// 判定状態
			pWk->nSt_Light[0] = 4;								// 照明装置状態
			pWk->nSt_Light[1] = 5;								// 照明装置状態
			pWk->nSt_CamControl = 6;							// カメラコントロール回路状態
			pWk->nSt_CamSwitch = 7;								// カメラリンクスイッチ回路状態
			pWk->nSt_Procon = 8;								// プロコン状態
			pWk->nSt_FF = 9;									// FF状態
			pWk->nSt_Sqence = 10;								// シーケンサ状態
			pWk->nSt_Cam_Inline[0] = 11;						// カメラ台車状態[表裏]
			pWk->nSt_Cam_Inline[1] = 12;						// カメラ台車状態[表裏]
			pWk->nSt_DbSkip[0] = 13;							// DB未保存[表裏]
			pWk->nSt_DbSkip[1] = 14;							// DB未保存[表裏]
			pWk->nSt_FrameSkip[0] = 15;							// 未検査[表裏]
			pWk->nSt_FrameSkip[1] = 16;							// 未検査[表裏]
			pWk->bStartWpdOk = 0;								// 検査基点信号検出可否
			pWk->setting.bWalm = 1;								// ウォーマー材区分
			pWk->nCoilCnt = 17;									// 検査本数
			pWk->setting.bDefScode[0] = 0;						// 検査表面状態No_Def区分_表
			pWk->setting.bDefScode[1] = 1;						// 検査表面状態No_Def区分_裏
			pWk->setting.bDefPcode[0] = 0;						// 検査パターンコード_Def区分_表
			pWk->setting.bDefPcode[1] = 1;						// 検査パターンコード_Def区分_裏
			pWk->setting.scode[0] = 18;							// 検査表面状態No_表
			pWk->setting.scode[1] = 19;							// 検査表面状態No_裏
			pWk->setting.pcode[0] = 20;							// 検査パターンコード_表
			pWk->setting.pcode[1] = 21;							// 検査パターンコード_裏
			pWk->nSpmStat = 22;									// SPM圧下有無
			pWk->nLength = 23;									// 検査装置実測長
			pWk->nLengthSub = 24;								// 検査比較実測長
			pWk->nLengthDsp = 25;								// 表示実測長
			pWk->nOutCoilPos = 26;								// 出側単位先端位置
			pWk->emUnten = EM_DIV_UNTEN::DIV_UNTEN_CAMERA;		// 運転状態
			pWk->emOneKensa[0] = EM_DIV_KENSA::DIV_KENSA_DBNON;	// 検査状態_表
			pWk->emOneKensa[1] = EM_DIV_KENSA::DIV_KENSA_EDGENG;// 検査状態_裏
			pWk->emOneKiki[0] = EM_DIV_KIKI::DIV_KIKI_JYU;		// 機器状態_表
			pWk->emOneKiki[1] = EM_DIV_KIKI::DIV_KIKI_OK;		// 機器状態_裏
			pWk->emPlg = EM_DIV_PLG::DIV_PLG_LINE;				// PLG選択区分
			pWk->nResultSendOk = 27;							// 外部実績送信許可
			for (int ii = 0; ii < MAX_CAMSET; ii++)
			{
				for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
				{
					pWk->nBr[ii][jj][0] = ii * 10 + jj;			// 輝度(C)
				}
			}
			for (int ii = 0; ii < MAX_CAMSET; ii++)
			{
				for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
				{
					pWk->nExp[ii][jj] = ii * 10 + jj;			// 露光時間[clk]
				}
			}
			for (int ii = 0; ii < MAX_CAMSET; ii++)
			{
				for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
				{
					pWk->nGain[ii][jj] = ii * 10 + jj;			// ゲイン[倍]
				}
			}
		}

	public:

		// テスト項目　１～２
		TEST_METHOD(TestMethod1)
		{
			LOG(em_MSG), "\r\n《TestMethod1》--------------------------------------------------------------------------------------------");

			// 使用オブジェクトの生成
			mcls_pParam = new ParamManager();
			mcls_pParam->SetLogMgr(mcls_pLog);
			mcls_pParam->Start();

			LOG(em_MSG), "ＤＢ読込テスト[%s]", DB_COMMON_MASTER);
			bool bResp = mcls_pParam->Read_CommonMast();
			Assert::AreEqual(true, bResp);
			if (!bResp)
			{
				LOG(em_ERR), "読込エラー [%s]", DB_COMMON_MASTER);
			}
			else
			{
				LOG(em_MSG), "　RAS正常色                            : %d", mcls_pParam->GetCommonMast().Ras.nColorNorm);
				LOG(em_MSG), "　RAS異常色                            : %d", mcls_pParam->GetCommonMast().Ras.nColorErr);
				LOG(em_MSG), "　RAS注意色                            : %d", mcls_pParam->GetCommonMast().Ras.nColorWarn);
				LOG(em_MSG), "　LOG表示件数                          : %d", mcls_pParam->GetCommonMast().Log.nDispNum);
				LOG(em_MSG), "　LOGイベント色                        : %d", mcls_pParam->GetCommonMast().Log.nEventColor);
				LOG(em_MSG), "　LOG重故障色                          : %d", mcls_pParam->GetCommonMast().Log.nFailColor_Ju);
				LOG(em_MSG), "　LOG軽故障色                          : %d", mcls_pParam->GetCommonMast().Log.nFailColor_Kei);
				LOG(em_MSG), "　探傷画面表示区間小                   : %d", mcls_pParam->GetCommonMast().Tansho.nDspSectionMin);
				LOG(em_MSG), "　探傷画面表示区間大                   : %d", mcls_pParam->GetCommonMast().Tansho.nDspSectionMax);
				LOG(em_MSG), "　ウォーマ材指定位置                   : %d", mcls_pParam->GetCommonMast().walm);
				LOG(em_MSG), "　Jpeg切り出し画像品質                 : %d", mcls_pParam->GetCommonMast().nJpegQuality);
				LOG(em_MSG), "　カメラ温度閾値                       : %d", mcls_pParam->GetCommonMast().nCamTempThr);
				LOG(em_MSG), "　エッジ整合性_実施種別                : %d", mcls_pParam->GetCommonMast().Edge.nType);
				LOG(em_MSG), "　エッジ整合性_検出範囲                : %d", mcls_pParam->GetCommonMast().Edge.dDetectRange);
				LOG(em_MSG), "　エッジ整合性_リカバリー検出範囲      : %d", mcls_pParam->GetCommonMast().Edge.dDetectRangeR);
				LOG(em_MSG), "　エッジ整合性_板幅許容誤差            : %d", mcls_pParam->GetCommonMast().Edge.dWidthDiff);
				LOG(em_MSG), "　エッジ整合性_前フレームとの許容誤差  : %d", mcls_pParam->GetCommonMast().Edge.dPerEdgeDiff);
				LOG(em_MSG), "　エッジ整合性_溶接後不感帯            : %d", mcls_pParam->GetCommonMast().Edge.nFromSkip);
				LOG(em_MSG), "　エッジ整合性_エッジ検出異常FF送信    : %d", mcls_pParam->GetCommonMast().Edge.nFFErr);
				LOG(em_MSG), "　エッジ検出_実施種別                  : %d", mcls_pParam->GetCommonMast().Edge_Restrict.nType);
				LOG(em_MSG), "　エッジ検出_検出範囲                  : %d", mcls_pParam->GetCommonMast().Edge_Restrict.dDetectRange);
				LOG(em_MSG), "　重複疵マップ背景色_軽                : %d", mcls_pParam->GetCommonMast().DefMap.nBGColor_Kei);
				LOG(em_MSG), "　重複疵マップ背景色_中                : %d", mcls_pParam->GetCommonMast().DefMap.nBGColor_Tyu);
				LOG(em_MSG), "　重複疵マップ背景色_重                : %d", mcls_pParam->GetCommonMast().DefMap.nBGColor_Ju);
				LOG(em_MSG), "　重複疵マップ疵個数閾値_軽            : %d", mcls_pParam->GetCommonMast().DefMap.nDefCntThr_Kei);
				LOG(em_MSG), "　重複疵マップ疵個数閾値_中            : %d", mcls_pParam->GetCommonMast().DefMap.nDefCntThr_Tyu);
				LOG(em_MSG), "　重複疵マップ疵個数閾値_重            : %d", mcls_pParam->GetCommonMast().DefMap.nDefCntThr_Ju);
				LOG(em_MSG), "　設備長_検査基点_上流カメラ           : %d", mcls_pParam->GetCommonMast().Dist.nKenBase);
				LOG(em_MSG), "　設備長_検査基点_SPM                  : %d", mcls_pParam->GetCommonMast().Dist.nKenBaseSpm);
				LOG(em_MSG), "　設備長_表示基点_裏検査台             : %d", mcls_pParam->GetCommonMast().Dist.nDspBaseKensaBot);
				LOG(em_MSG), "　設備長_表示基点_表検査台             : %d", mcls_pParam->GetCommonMast().Dist.nDspBaseKensaTop);
				LOG(em_MSG), "　設備長_表示基点_シャー               : %d", mcls_pParam->GetCommonMast().Dist.nDspSharCut);
				LOG(em_MSG), "　設備長_検査基点ゲート信号ON_検査基点 : %d", mcls_pParam->GetCommonMast().Dist.nKenBaseGate);
				LOG(em_MSG), "　設備長_表示基点ゲート信号ON_表示基点 : %d", mcls_pParam->GetCommonMast().Dist.nDspBaseGate);
				LOG(em_MSG), "　疵接近ランプ_不感帯_先端             : %d", mcls_pParam->GetCommonMast().nNoLamp[0]);
				LOG(em_MSG), "　疵接近ランプ_不感帯_尾端             : %d", mcls_pParam->GetCommonMast().nNoLamp[1]);
				LOG(em_MSG), "　疵接近ランプ_出力区間_前             : %d", mcls_pParam->GetCommonMast().nKizuLamp[0]);
				LOG(em_MSG), "　疵接近ランプ_出力区間_後             : %d", mcls_pParam->GetCommonMast().nKizuLamp[1]);
				LOG(em_MSG), "　疵接近アラーム_不感帯_先端           : %d", mcls_pParam->GetCommonMast().nNoAlarm[0]);
				LOG(em_MSG), "　疵接近アラーム_不感帯_尾端           : %d", mcls_pParam->GetCommonMast().nNoAlarm[1]);
				LOG(em_MSG), "　疵接近アラーム_出力区間_前           : %d", mcls_pParam->GetCommonMast().nKizuAlarm[0]);
				LOG(em_MSG), "　疵接近アラーム_出力区間_後           : %d", mcls_pParam->GetCommonMast().nKizuAlarm[1]);
				LOG(em_MSG), "　ボイス_出力位置_前                   : %d", mcls_pParam->GetCommonMast().nVoicePos);
				LOG(em_MSG), "　全長画像_保存_R                      : %d", mcls_pParam->GetCommonMast().nRPISaveColor_R);
				LOG(em_MSG), "　全長画像_保存_G                      : %d", mcls_pParam->GetCommonMast().nRPISaveColor_G);
				LOG(em_MSG), "　全長画像_保存_B                      : %d", mcls_pParam->GetCommonMast().nRPISaveColor_B);
				LOG(em_MSG), "　全長画像_保存_差                     : %d", mcls_pParam->GetCommonMast().nRPISaveColor_Sa);
				LOG(em_MSG), "　PLG距離比較異常上限値                : %d", mcls_pParam->GetCommonMast().dPlgDiffLimit[0]);
				LOG(em_MSG), "　PLG距離表示異常上限値                : %d", mcls_pParam->GetCommonMast().dPlgDiffLimit[1]);
				LOG(em_MSG), "　プロコンコイル長比較異常上限値       : %d", mcls_pParam->GetCommonMast().dProLenDiffLimit);
				LOG(em_MSG), "　ロジックパララン機能01_入力カメラ01  : %d", mcls_pParam->GetCommonMast().nLogiParaCam[0][0]);
				LOG(em_MSG), "　ロジックパララン機能01_入力カメラ02  : %d", mcls_pParam->GetCommonMast().nLogiParaCam[0][1]);
				LOG(em_MSG), "　ロジックパララン機能02_入力カメラ01  : %d", mcls_pParam->GetCommonMast().nLogiParaCam[1][0]);
				LOG(em_MSG), "　ロジックパララン機能02_入力カメラ02  : %d", mcls_pParam->GetCommonMast().nLogiParaCam[1][1]);
			}

			LOG(em_MSG), "ＤＢ読込テスト[%s]", DB_PCODE_MASTER);
			bResp = mcls_pParam->Read_PcodeOutput();
			Assert::AreEqual(true, bResp);
			if (!bResp)
			{
				LOG(em_ERR), "読込エラー [%s]", DB_PCODE_MASTER);
			}
			else
			{
				for(int ii=0; ii<MAX_PCODE; ii++)
				{
					LOG(em_MSG), "　pcode: %02d --------------------------------", ii + 1);
					LOG(em_MSG), "　　有効判定 : %d", mcls_pParam->GetPcodeOutput().rec[ii].umu);
					LOG(em_MSG), "　　　疵種EdasysID             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.edasysID);
					LOG(em_MSG), "　　　探傷画面表示_表          : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DSP_O]);
					LOG(em_MSG), "　　　探傷画面表示_裏          : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DSP_U]);
					LOG(em_MSG), "　　　入側単位疵マップ画面表示 : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DSP_DEFMAP]);
					LOG(em_MSG), "　　　FF送信有無               : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_FF_SEND]);
					LOG(em_MSG), "　　　疵接近軽                 : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_ALARM_RANP_LOW]);
					LOG(em_MSG), "　　　疵接近重                 : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_ALARM_RANP_HIGH]);
					LOG(em_MSG), "　　　パトライト出力           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_PATOLOGHT]);
					LOG(em_MSG), "　　　ボイス連続疵             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_SEQ]);
					LOG(em_MSG), "　　　ボイス重大疵             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_HIGH]);
					LOG(em_MSG), "　　　ボイスヘゲ               : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_HEGE]);
					LOG(em_MSG), "　　　ボイススリバ             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_SURI]);
					LOG(em_MSG), "　　　ボイス表裏同一疵         : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_SAME]);
					LOG(em_MSG), "　　　ボイス周期疵             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_CYCLE]);
					LOG(em_MSG), "　　　ボイスその他疵           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_OTHER]);
					LOG(em_MSG), "　　　DB保存_一般_表           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_LOWGR_DB_O]);
					LOG(em_MSG), "　　　DB保存_一般_裏           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_LOWGR_DB_U]);
					LOG(em_MSG), "　　　DB保存_優先_表           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DB_O]);
					LOG(em_MSG), "　　　DB保存_優先_裏           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DB_U]);
					Sleep(1);
				}
			}

			// 使用オブジェクトの破棄
			mcls_pParam->Stop();
			delete mcls_pParam;
		}

		// テスト項目　３～１６
		TEST_METHOD(TestMethod2)
		{
			LOG(em_MSG), "\r\n《TestMethod2》--------------------------------------------------------------------------------------------");

			// 使用オブジェクトの生成
			mcls_pParam = new ParamManager();
			mcls_pParam->SetLogMgr(mcls_pLog);
			mcls_pParam->Start();
			mcls_Status.SetLogMgr(mcls_pLog);
			mcls_Status.SetParamMgr(mcls_pParam);
			mcls_Status.SetKadou(DIV_KADOU_INIT);
			mcls_Status.SetDspSyslog(true);
			mcls_Status.SetAddStlog(true);
			mcls_Status.Start();
			mcls_Status.SetStatusRead();
			mcls_DB.SetLogMgr(mcls_pLog);
			mcls_DB.SetStatusMgr(&mcls_Status);
			mcls_DB.Alloc();
			mcls_DB.Start();

			char cKizukenNo[SIZE_KIZUKEN_NO];
			memset(cKizukenNo, 0x00, SIZE_KIZUKEN_NO);
			GetCurrentTimeString("UT-", cKizukenNo);// 管理No

			LOG(em_MSG), "ＤＢ書込テスト[%s]", DB_COIL_INF);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILINF;
				deliDB->data = (BYTE*)new DBDATA_COIL_INF;
				DBDATA_COIL_INF* pWk = (DBDATA_COIL_INF*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_INF));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// 管理No
				SetInfo(pWk);

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "ＤＢ書込テスト[%s %s]", DB_COIL_RESULT, DB_COIL_CAMERA);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILRESULT;
				deliDB->data = (BYTE*)new DBDATA_COIL_RESULT;
				DBDATA_COIL_RESULT* pWk = (DBDATA_COIL_RESULT*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_RESULT));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// 管理No
				SetInfo(pWk);

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "ＤＢ更新テスト[%s]", DB_COIL_RESULT);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILRESULT_UPDATE;
				deliDB->data = (BYTE*)new DBDATA_COIL_RESULT_UPDATE;
				DBDATA_COIL_RESULT_UPDATE* pWk = (DBDATA_COIL_RESULT_UPDATE*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_RESULT_UPDATE));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// 管理No
				pWk->emMode = MODE_RECV_SHRCUT;
				pWk->nOutCoilPos = 1234;

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "ＤＢ更新テスト[%s]", DB_COIL_RESULT);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILRESULT_UPDATE;
				deliDB->data = (BYTE*)new DBDATA_COIL_RESULT_UPDATE;
				DBDATA_COIL_RESULT_UPDATE* pWk = (DBDATA_COIL_RESULT_UPDATE*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_RESULT_UPDATE));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// 管理No
				pWk->emMode = MODE_DSP_WPD_ON;
				pWk->nDspLength = 5678;
				pWk->nSpmUmu = 1;

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "ＤＢ書込テスト[%s]", DB_COIL_POSITION);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILPOSITION;
				deliDB->data = (BYTE*)new DBDATA_COIL_POSITION;
				DBDATA_COIL_POSITION* pWk = (DBDATA_COIL_POSITION*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_POSITION));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->nID = 1234;
				for (int ii = 0; ii < NUM_MEN; ii++)pWk->emKensa[ii] = (EM_DIV_KENSA)(EM_DIV_KENSA::DIV_KENSA_INIT + ii);
				for (int ii = 0; ii < NUM_MEN; ii++)pWk->emKiki[ii] = (EM_DIV_KIKI)(EM_DIV_KIKI::DIV_KIKI_OK + ii);
				for (int ii = 0; ii < 2; ii++)		pWk->dEdge[ii] = (float)(ii + 1);
				pWk->dWalk = 5678;

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "ＤＢ書込テスト[%s]", DB_COIL_TRAND);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILTRAND;
				deliDB->data = (BYTE*)new DBDATA_COIL_TRAND;
				DBDATA_COIL_TRAND* pWk = (DBDATA_COIL_TRAND*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_TRAND));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->nMen = 1;
				pWk->nLen = 9;
				pWk->dSpmHosei = 1000;
				pWk->nSpeed = 1111;
				pWk->dCoilWidMax = 2222;
				pWk->dCoilWidMin = 3333;
				pWk->dWalk = 4444;
				for (int ii = 0; ii < DIV_LED_COLOR_END; ii++)		 pWk->nLightValue[ii] = ii + 1;
				for (int ii = 0; ii < NUM_CAM_POS; ii++)
				{
					for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) pWk->nBr[ii][jj][0] = ii * NUM_CAM_ANGLE + jj;
				}
				for (int ii = 0; ii < NUM_CAM_POS; ii++)
				{
					for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) pWk->nExp[ii][jj] = ii * NUM_CAM_ANGLE + jj;
				}
				for (int ii = 0; ii < NUM_CAM_POS; ii++)
				{
					for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) pWk->dGain[ii][jj] = (float)(ii * NUM_CAM_ANGLE + jj);
				}

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "ＤＢ書込テスト[%s]", DB_COIL_CUT_POS);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILCUTPOS;
				deliDB->data = (BYTE*)new DBDATA_COIL_CUT_POS;
				DBDATA_COIL_CUT_POS* pWk = (DBDATA_COIL_CUT_POS*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_CUT_POS));

				pWk->data.nCnt = 1;
				memcpy(pWk->data.incoil[0].cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->data.incoil[0].nInCutNum = 1111;
				strcpy(pWk->data.cID , "2222");
				pWk->nMode = 3333;
				pWk->data.incoil[0].nFront = 4444;
				pWk->data.incoil[0].nLength = 5555;
				pWk->data.incoil[0].nOutPos = 6666;

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			//LOG(em_MSG), "ＤＢ書込テスト[%s]", DB_DEFECT_INF_T);
			//{
			//	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
			//	deliDB->kubun = DBManager::em_DEFECTINF;
			//	deliDB->data = (BYTE*)new DBDATA_DEFECT_INF;
			//	DBDATA_DEFECT_INF* pWk = (DBDATA_DEFECT_INF*)deliDB->data;
			//	memset(pWk, 0x00, sizeof(DBDATA_DEFECT_INF));

			//	memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
			//	pWk->nKizuNo = 1;
			//	pWk->data.TorB = 0;
			//	pWk->data.nTid = 111;
			//	pWk->data.nGid = 1111;
			//	pWk->data.cycle_div = 2;
			//	pWk->data.dsws = 22;
			//	pWk->data.camset = 222;
			//	pWk->dLen = 2222;
			//	pWk->data.ds = 3;
			//	pWk->data.ws = 33;
			//	pWk->data.speed = 333;
			//	pWk->data.pitch = 3333;
			//	pWk->data.roolkei = 4;
			//	pWk->data.aria = 44;
			//	pWk->data.kizu_wid = 444;
			//	pWk->data.kizu_len = 4444;
			//	pWk->nLowGrFlg = 5;
			//	pWk->data.nWidDiv = 55;
			//	pWk->data.nWidAbsPos = 555;
			//	pWk->nIsDspFlg = 5555;
			//	pWk->data.nKizuNo = 6;
			//	pWk->data.h_res = 66;
			//	pWk->data.w_res = 666;
			//	pWk->data.nFrameNo = 6666;
			//	pWk->data.frm_x_min = 7;
			//	pWk->data.frm_x_max = 77;
			//	pWk->data.frm_y_min = 777;
			//	pWk->data.frm_y_max = 7777;
			//	pWk->data.cull_frm_x_min = 8;
			//	pWk->data.cull_frm_x_max = 88;
			//	pWk->data.cull_frm_y_min = 8888;
			//	pWk->data.cull_frm_y_max = 9;
			//	pWk->data.box_x_min = 99;
			//	pWk->data.box_x_max = 999;
			//	pWk->data.box_y_min = 9999;
			//	pWk->data.box_y_max = 12;
			//	pWk->data.center_x = 34;
			//	pWk->data.center_y = 56;
			//	pWk->data.h_size = 78;
			//	pWk->data.w_size = 90;
			//	for (int ii = 0; ii<MAX_HT1_CAMANGLE; ii++)
			//	{
			//		pWk->nImgsize[ii] = 1024;
			//		pWk->pImg[ii] = new BYTE[pWk->nImgsize[ii]];
			//		memset(pWk->pImg[ii], 0x00, sizeof(pWk->nImgsize[ii]));
			//	}
			//	pWk->data.base_box_x_min = 0;
			//	pWk->data.base_box_x_max = 1;
			//	pWk->data.base_box_y_min = 2;
			//	pWk->data.base_box_y_max = 3;
			//	pWk->data.base_center_x = 4;
			//	pWk->data.base_center_y = 5;
			//	pWk->data.base_h_size = 6;
			//	pWk->data.base_w_size = 7;

			//	//================================================
			//	//// 登録依頼
			//	ExecSQL(deliDB);
			//	Sleep(10000);

			//	{
			//		BCPSaveManager::DeliveryDB *deliDB = new BCPSaveManager::DeliveryDB;
			//		deliDB->kubun = BCPSaveManager::em_BCP_BATCH;
			//		deliDB->data = (BYTE*)new DBDATA_BCP_BATCH_EXEC;
			//		DBDATA_BCP_BATCH_EXEC* pData = (DBDATA_BCP_BATCH_EXEC*)deliDB->data;

			//		pData->bMode = false;

			//		//================================================
			//		//// 登録依頼
			//		if (!mcls_DB.GetBcpSaveMgr().gque_AddSQLData.AddToTail(deliDB, 20, 20)) {
			//			LOG(em_ERR), "DB登録依頼キューフル [BCP_BATCH_EXEC]");
			//			syslog(SYSNO_QUEFULL_ERR, "[BCP_BATCH_EXEC]");

			//			delete deliDB;
			//		}
			//		Sleep(10000);
			//	}
			//}

			//LOG(em_MSG), "ＤＢ書込テスト[%s]", DB_DEFECT_INF_B);
			//{
			//	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
			//	deliDB->kubun = DBManager::em_DEFECTINF;
			//	deliDB->data = (BYTE*)new DBDATA_DEFECT_INF;
			//	DBDATA_DEFECT_INF* pWk = (DBDATA_DEFECT_INF*)deliDB->data;
			//	memset(pWk, 0x00, sizeof(DBDATA_DEFECT_INF));

			//	memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
			//	pWk->nKizuNo = 1;
			//	pWk->data.TorB = 1;
			//	pWk->data.nTid = 111;
			//	pWk->data.nGid = 1111;
			//	pWk->data.cycle_div = 2;
			//	pWk->data.dsws = 22;
			//	pWk->data.camset = 222;
			//	pWk->dLen = 2222;
			//	pWk->data.ds = 3;
			//	pWk->data.ws = 33;
			//	pWk->data.speed = 333;
			//	pWk->data.pitch = 3333;
			//	pWk->data.roolkei = 4;
			//	pWk->data.aria = 44;
			//	pWk->data.kizu_wid = 444;
			//	pWk->data.kizu_len = 4444;
			//	pWk->nLowGrFlg = 5;
			//	pWk->data.nWidDiv = 55;
			//	pWk->data.nWidAbsPos = 555;
			//	pWk->nIsDspFlg = 5555;
			//	pWk->data.nKizuNo = 6;
			//	pWk->data.h_res = 66;
			//	pWk->data.w_res = 666;
			//	pWk->data.nFrameNo = 6666;
			//	pWk->data.frm_x_min = 7;
			//	pWk->data.frm_x_max = 77;
			//	pWk->data.frm_y_min = 777;
			//	pWk->data.frm_y_max = 7777;
			//	pWk->data.cull_frm_x_min = 8;
			//	pWk->data.cull_frm_x_max = 88;
			//	pWk->data.cull_frm_y_min = 8888;
			//	pWk->data.cull_frm_y_max = 9;
			//	pWk->data.box_x_min = 99;
			//	pWk->data.box_x_max = 999;
			//	pWk->data.box_y_min = 9999;
			//	pWk->data.box_y_max = 12;
			//	pWk->data.center_x = 34;
			//	pWk->data.center_y = 56;
			//	pWk->data.h_size = 78;
			//	pWk->data.w_size = 90;
			//	for (int ii = 0; ii<MAX_HT1_CAMANGLE; ii++)
			//	{
			//		pWk->nImgsize[ii] = (-1 * MAX_HT1_CAMANGLE) + ii;
			//	}
			//	pWk->data.base_box_x_min = 0;
			//	pWk->data.base_box_x_max = 1;
			//	pWk->data.base_box_y_min = 2;
			//	pWk->data.base_box_y_max = 3;
			//	pWk->data.base_center_x = 4;
			//	pWk->data.base_center_y = 5;
			//	pWk->data.base_h_size = 6;
			//	pWk->data.base_w_size = 7;

			//	//================================================
			//	//// 登録依頼
			//	ExecSQL(deliDB);
			//	Sleep(1000);

			//	{
			//		BCPSaveManager::DeliveryDB *deliDB = new BCPSaveManager::DeliveryDB;
			//		deliDB->kubun = BCPSaveManager::em_BCP_BATCH;
			//		deliDB->data = (BYTE*)new DBDATA_BCP_BATCH_EXEC;
			//		DBDATA_BCP_BATCH_EXEC* pData = (DBDATA_BCP_BATCH_EXEC*)deliDB->data;

			//		pData->bMode = false;

			//		//================================================
			//		//// 登録依頼
			//		if (!mcls_DB.GetBcpSaveMgr().gque_AddSQLData.AddToTail(deliDB, 20, 20)) {
			//			LOG(em_ERR), "DB登録依頼キューフル [BCP_BATCH_EXEC]");
			//			syslog(SYSNO_QUEFULL_ERR, "[BCP_BATCH_EXEC]");

			//			delete deliDB;
			//		}
			//		Sleep(1000);
			//	}
			//}

			LOG(em_MSG), "ＤＢ書込テスト[%s]", DB_EDGE_COIL_INF);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_EDGE_COIL_INF;
				deliDB->data = (BYTE*)new DBDATA_EDGE_COIL_INF;
				DBDATA_EDGE_COIL_INF* pWk = (DBDATA_EDGE_COIL_INF*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_EDGE_COIL_INF));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->nMen = 1111;
				pWk->dCoilWidMin = 2222;
				pWk->dCoilWidMax = 3333;
				pWk->nLimitEdgeErrNum = 4444;
				pWk->nLostEdgeNum[0] = 5555;
				pWk->nLostEdgeNum[1] = 6666;
				pWk->nFrameNumCoil = NUM_CAM_POS * 5;
				for (int ii = 0; ii < NUM_CAM_POS; ii++)
				{
					for (int jj = 0; jj < 5; jj++)
					{
						pWk->nFrameNumSkip[ii][jj] = NUM_CAM_POS * ii + jj;
					}
				}

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "ＤＢ書込テスト[%s]", DB_EDGE_CHANGE_INF);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_EDGE_CHANGE_INF;
				deliDB->data = (BYTE*)new DBDATA_EDGE_CHANGE_INF;
				DBDATA_EDGE_CHANGE_INF* pWk = (DBDATA_EDGE_CHANGE_INF*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_EDGE_CHANGE_INF));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->nPos = 1111;
				pWk->nMen = 2222;
				pWk->nFrameNo = 3333;
				pWk->nEdgeMode = 4444;
				pWk->dEdgeRawPos[0] = 5555;
				pWk->dEdgeRawPos[1] = 6666;
				pWk->dEdgeAnsPos[0] = 7777;
				pWk->dEdgeAnsPos[1] = 8888;
				pWk->nEdgeErrReason = 9999;

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "ＤＢ更新テスト[%s]", DB_CAMERA_LINKSWITCH);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_CAMLINKSW;
				deliDB->data = (BYTE*)new DBDATA_CAMERA_LINKSWITCH;
				DBDATA_CAMERA_LINKSWITCH* pWk = (DBDATA_CAMERA_LINKSWITCH*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_CAMERA_LINKSWITCH));

				mcls_Status.SetHtID(0, 0, 1);
				mcls_Status.SetHtID(0, 1, 2);
				mcls_Status.SetHtID(0, 2, 3);
				mcls_Status.SetHtID(0, 3, 4);
				mcls_Status.SetHtID(0, 4, 5);
				mcls_Status.SetHtID(1, 0, 6);
				mcls_Status.SetHtID(1, 1, 7);
				mcls_Status.SetHtID(1, 2, 8);
				mcls_Status.SetHtID(1, 3, 9);
				mcls_Status.SetHtID(1, 4, 10);

				pWk->nPCID[0][0] = 1;
				pWk->nPCID[0][1] = 2;
				pWk->nPCID[0][2] = 3;
				pWk->nPCID[0][3] = 5;
				pWk->nPCID[0][4] = 6;
				pWk->nPCID[1][0] = 8;
				pWk->nPCID[1][1] = 9;
				pWk->nPCID[1][2] = 10;
				pWk->nPCID[1][3] = 12;
				pWk->nPCID[1][4] = 13;

				pWk->nParaPCID[0] = 15;
				pWk->nParaPCID[1] = 16;

				pWk->nLogiParaCam[0][0] = 1;
				pWk->nLogiParaCam[0][1] = 2;
				pWk->nLogiParaCam[1][0] = 3;
				pWk->nLogiParaCam[1][1] = 4;

				//================================================
				//// 登録依頼
				ExecSQL(deliDB);
				Sleep(1000);
			}

			// 使用オブジェクトの破棄
			mcls_DB.Stop();
			mcls_DB.Free();
			mcls_Status.Stop();
			mcls_pParam->Stop();
			delete mcls_pParam;
		}

		// テスト項目　１７
		TEST_METHOD(TestMethod3)
		{
			LOG(em_MSG), "\r\n《TestMethod3》--------------------------------------------------------------------------------------------");

			// 使用オブジェクトの生成
			mcls_Mae.SetLogMgr(mcls_pLog);
			mcls_Mae.Start();
			Sleep(5000);

			char cKizukenNo[SIZE_KIZUKEN_NO];
			memset(cKizukenNo, 0x00, SIZE_KIZUKEN_NO);
			GetCurrentTimeString("UT-", cKizukenNo);// 管理No

			LOG(em_MSG), "ＤＢ書込テスト[%s %s]", DB_MAE_COIL_INF, DB_MAE_FF_INF);
			{
				COIL_INF *pInf = new COIL_INF();
				memset(pInf, 0x00, sizeof(COIL_INF));

				// DB_MAE_COIL_INFの以下の条件のデータを予め用意しておく
				// ・管理No         = 12345
				// ・仮管理Noフラグ = -1
				// ・命令No         = "test"
				// ・コイルo        = "     coil-ok"
				// ・SPM圧下有無    = 0
				// DB_MAE_FF_INFの以下の条件のデータを予め用意しておく
				// ・管理No         = 12345
				// ・SPM圧下有無    = 0
				memcpy(pInf->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// 管理No
				strcpy(pInf->data.cMeiNo, "test");						
				strcpy(pInf->data.cCoilNoIn, "     coil-ok");
				pInf->nSt_Spm = 0;

				mcls_Mae.RecvCoilInf(pInf);
				Sleep(5000);
				delete pInf;
			}

			//// 使用オブジェクトの破棄
			mcls_Mae.Stop();
		}

		// テスト項目　１８～２７
		TEST_METHOD(TestMethod4)
		{
			LOG(em_MSG), "\r\n《TestMethod4》--------------------------------------------------------------------------------------------");
		}
	};
}