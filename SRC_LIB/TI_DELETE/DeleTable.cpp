#include "StdAfx.h"
#include "DeleTable.h"


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// このクラスのみが、システム毎に違う

//------------------------------------------
// コンストラクタ
// int id 自分のID (1オリジン)
// int* waittime 待ち時間 [ms]
// HANDLE evEndCycle 終了シグナル
// HANDLE evEndSizeMin 終了シグナル
// CString cSession 自分の設定が定義されているiniファイルのセッション
// CString cOdbcSession ODBC設定セッション
//------------------------------------------
DeleTable::DeleTable(int id, int* nWaitTime, HANDLE evEndCycle, HANDLE evEndSizeMin, CString cSession, CString cOdbcSession) :
KizuDeleteManager(id, nWaitTime, evEndCycle, evEndSizeMin, cSession, cOdbcSession),
m_nDelTblCoil(0),
m_nDelTblTime(0),
m_nDelTblCnt(0),
m_pDelTblCoil(NULL),
m_pDelTblTime(NULL),
m_pDelTblCnt(NULL)
{

	int		nTblCnt;				// 件数
	char    cWk[1024];
	CString strVal;

	//----------------------------------------
	////// コイルに紐付く削除テーブル生成
	//// 件数を取得
	nTblCnt = 0;
	while(true) {
		strVal.Format("ZI_%d", nTblCnt+1);
		GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if(0 >= strlen(cWk) ) break;
		nTblCnt++;
	}
	m_nDelTblCoil = nTblCnt;
	//// 0件以外の場合 テーブル生成
	if(0 != m_nDelTblCoil) {
		nTblCnt = 0;
		m_pDelTblCoil = new DEL_TBL[m_nDelTblCoil];
		for(int ii=0; ii<m_nDelTblCoil; ii++) {
			strVal.Format("ZI_%d", ii+1);
			GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		
			if( 0 < strlen(cWk) ) {
				strVal = cWk;
				
				// トークン分割
				int		rec = 0;
				int		curPos = 0;
				CString strToken = strVal.Tokenize(",", curPos);
				while ( strToken != "" ) {
					strToken.Trim(_T(" 	"));		// iniファイル設定文字の前後のスペース、タブを取り除く

					switch( rec ) {
						case 0:						// テーブル名
							m_pDelTblCoil[ii].cDelTable = strToken; 
							break;
						case 1:						// 主キーキー
							m_pDelTblCoil[ii].cKeyName	= strToken;
							break;
						case 2:						// サブキー
							m_pDelTblCoil[ii].cSubName	= strToken; 
							break;
						case 3:						// 一回の削除件数
							m_pDelTblCoil[ii].nDelRow	= atoi(strToken); 
							break;
						default:
							break;
					}

					// 次へ
					strToken = strVal.Tokenize(",", curPos);
					rec ++;
				}
				// 分割したトークンが有効？
				if( 4 != rec) {
					_ASSERT(false);
				}
			}
		}
	}

	//----------------------------------------
	////// 日付に紐付く削除テーブル生成
	//// 件数を取得
	nTblCnt = 0;
	while(true) {
		strVal.Format("TI_%d", nTblCnt+1);
		GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if(0 >= strlen(cWk) ) break;
		nTblCnt++;
	}
	m_nDelTblTime = nTblCnt;
	//// 0件以外の場合 テーブル生成
	if(0 != m_nDelTblTime) {
		nTblCnt = 0;
		m_pDelTblTime = new DEL_TBL[m_nDelTblTime];
		for(int ii=0; ii<m_nDelTblTime; ii++) {
			strVal.Format("TI_%d", ii+1);
			GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		
			if( 0 < strlen(cWk) ) {
				strVal = cWk;
				
				// トークン分割
				int		rec = 0;
				int		curPos = 0;
				CString strToken = strVal.Tokenize(",", curPos);
				while ( strToken != "" ) {
					strToken.Trim(_T(" 	"));		// iniファイル設定文字の前後のスペース、タブを取り除く

					switch( rec ) {
						case 0:						// テーブル名
							m_pDelTblTime[ii].cDelTable = strToken; 
							break;
						case 1:						// 日付列
							m_pDelTblTime[ii].cKeyName	= strToken; 
							break;
						case 2:						// 保存期間
							m_pDelTblTime[ii].nDelRow	= atoi(strToken); 
							break;
						default:
							break;
					}

					// 次へ
					strToken = strVal.Tokenize(",", curPos);
					rec ++;
				}
				// 分割したトークンが有効？
				if( 3 != rec) {
					_ASSERT(false);
				}
			}
		}
	}

	//----------------------------------------
	////// 保存上限数に紐付く削除テーブル生成
	//// 件数を取得
	nTblCnt = 0;
	while(true) {
		strVal.Format("CI_%d", nTblCnt+1);
		GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if(0 >= strlen(cWk) ) break;
		nTblCnt++;
	}
	m_nDelTblCnt = nTblCnt;
	//// 0件以外の場合 テーブル生成
	if(0 != m_nDelTblCnt) {
		nTblCnt = 0;
		m_pDelTblCnt = new DEL_TBL_CNT[m_nDelTblCnt];
		for(int ii=0; ii<m_nDelTblCnt; ii++) {
			strVal.Format("CI_%d", ii+1);
			GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		
			if( 0 < strlen(cWk) ) {
				strVal = cWk;
				
				// トークン分割
				int		rec = 0;
				int		curPos = 0;
				CString strToken = strVal.Tokenize(",", curPos);
				while ( strToken != "" ) {
					strToken.Trim(_T(" 	"));		// iniファイル設定文字の前後のスペース、タブを取り除く

					switch( rec ) {
						case 0:						// テーブル名
							m_pDelTblCnt[ii].cDelTable = strToken; 
							break;
						case 1:						// 主キーキー
							m_pDelTblCnt[ii].cKeyName	= strToken;
							break;
						case 2:						// サブキー
							m_pDelTblCnt[ii].cSubName	= strToken; 
							break;
						case 3:						// 一回の削除件数
							m_pDelTblCnt[ii].nDelRow	= atoi(strToken); 
							break;
						case 4:						// 保存上限数
							m_pDelTblCnt[ii].nMaxCnt	= atoi(strToken); 
							break;
						default:
							break;
					}

					// 次へ
					strToken = strVal.Tokenize(",", curPos);
					rec ++;
				}
				// 分割したトークンが有効？
				if( 5 != rec) {
					_ASSERT(false);
				}
			}
		}
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DeleTable::~DeleTable(void)
{
	// 開放
	SAFE_DELETE_ARRAY(m_pDelTblCoil); 
	SAFE_DELETE_ARRAY(m_pDelTblTime); 
	SAFE_DELETE_ARRAY(m_pDelTblCnt); 
}

//------------------------------------------
// 定周期実行メソッド
//------------------------------------------
void DeleTable::ExecCycle()
{
	//// 送受信伝文バッファ
	for(int ii=0; ii<m_nDelTblTime; ii++) {
		Delete_Day2(m_pDelTblTime[ii].cDelTable,	m_pDelTblTime[ii].cKeyName, m_pDelTblTime[ii].nDelRow);
		Exec_Shrink();														// トランザクション圧縮処理
	}
	for(int ii=0; ii<m_nDelTblCnt; ii++) {
		Delete_Cnt(m_pDelTblCnt[ii].cDelTable,	m_pDelTblCnt[ii].cKeyName,	m_pDelTblCnt[ii].cSubName, m_pDelTblCnt[ii].nDelRow, m_pDelTblCnt[ii].nMaxCnt);
		Exec_Shrink();														// トランザクション圧縮処理
	}
	return;
}

//------------------------------------------
// DB容量低下実行メソッド
//------------------------------------------
void DeleTable::ExecSizeMin()
{
	char cLastKizukenNo[SIZE_KIZUKEN_NO];		// 削除した最新の疵検管理No
	char cKizukenNo[SIZE_KIZUKEN_NO];			
	bool retc;

	memset(cLastKizukenNo, 0x00, sizeof(cLastKizukenNo));
	memset(cKizukenNo, 0x00, sizeof(cKizukenNo));

	////// 指定件数分のコイル実績を削除
	for(int ii=0; ii<my_Param.nDelCoilCnt; ii++) {
		if(my_bStopFlg) return; // 終了？

		//---------------------------------------
		// クリティカルセクション区間 (コイル特定)
		//---------------------------------------		
		//// 最古のコイルの取得
		// 主キーの為 クリティカルセクション とする (重要)
		mcls_pLock->LockTh();	//// -------------->>>>>
		//retc = Select_OldKey(DB_COIL_RESULT, "疵検管理No", SIZE_KIZUKEN_NO, cKizukenNo);
		//retc = Select_OldKey( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, SIZE_KIZUKEN_NO, cKizukenNo);
		retc = Select_OldKey( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, my_Param.sCoilResultSort, SIZE_KIZUKEN_NO, cKizukenNo);
		if(!retc || 0==strlen(cKizukenNo)) {		// 取得データの整合性確認
			mcls_pLock->UnlockTh();//// <<<<<--------------
			break;		// 取得失敗 又は データ無し
		}

		//// コイル実績削除
		// 実績画面とかで検索が掛からないように
		//if( ! Delete_Inf( DB_COIL_RESULT, "疵検管理No", cKizukenNo) ) {
		if( ! Delete_Inf( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, cKizukenNo) ) {
			mcls_pLock->UnlockTh();//// <<<<<--------------
			continue;		// 削除失敗
		}
		mcls_pLock->UnlockTh();	//// <<<<<--------------
		if(my_bStopFlg) return; // 終了？

		//// 処理した疵検管理Noをセット
		memcpy(cLastKizukenNo, cKizukenNo, sizeof(cKizukenNo));


		//---------------------------------------
		// 取得したコイルに紐付く情報を削除
		//---------------------------------------		
		for(int jj=0; jj<m_nDelTblCoil; jj++) {
			Exec_Shrink();														// トランザクション圧縮処理

			// 一括/少量削除 判定
			if( ! Delete_Inf( &m_pDelTblCoil[jj], cKizukenNo ) ) {
				continue;		// 削除失敗
			}
			if(my_bStopFlg) return; // 終了？
		}

		if(my_bStopFlg) return; // 終了？
	}


	//// バックアップ削除実行
	// この疵検管理Noより、小さい疵検管理Noは絶対のこっていないはず。（残っていたらそれはゴミ）
	if( 1 == my_nID && 0 != strlen(cLastKizukenNo)) {
		Delete_BackUp(cLastKizukenNo);
	}
	
	return;
}

//------------------------------------------
// 初回実行
//	・容量チェックのバックアップ実行をまず行う
//------------------------------------------
void DeleTable::ExecFast()
{
	bool retc;
	if(0 == m_nDelTblCnt) return;

	//// 最古の疵検管理Noを取得
	char cLastKizukenNo[SIZE_KIZUKEN_NO];		
	memset(cLastKizukenNo, 0x00, sizeof(cLastKizukenNo));
	retc = Select_OldKey( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, SIZE_KIZUKEN_NO, cLastKizukenNo);
	if(!retc || 0==strlen(cLastKizukenNo)) {		// 取得データの整合性確認
		return;		// 取得失敗 又は データ無し
	}

	//// バックアップ削除実行
	// この疵検管理Noより、小さい疵検管理Noは絶対のこっていないはず。（残っていたらそれはゴミ）
	Delete_BackUp(cLastKizukenNo);
}

//------------------------------------------
// バックアップ削除
// この疵検管理Noより、小さい疵検管理Noは絶対のこっていないはず。（残っていたらそれはゴミ）
// const char* cKizukenNo 疵検管理No
//------------------------------------------
void DeleTable::Delete_BackUp(const char* cKizukenNo)
{
	//// ソートキー指定の場合、ソートで大きい疵検管理Noが消えた場合、バックアップ削除でそれよりも小さい疵検管理Noが全部消えてしまうため
	if( 0 != strcmp( my_Param.sCoilResultKey, my_Param.sCoilResultSort) ) {
		LOG(em_MSG), "[Dele] [%s] ソートキー指定のため、バックアップ削除機能無効化 (key=%s, sort=%s) [%s]", my_cSession, my_Param.sCoilResultKey, my_Param.sCoilResultSort, cKizukenNo);
		return;
	}
	LOG(em_MSG), "[Dele] [%s] バックアップ削除実行 [%s]", my_cSession, cKizukenNo);


	//// 指定キーよりも古い実績は全て削除
	//if( ! Delete_OldInf( DB_COIL_RESULT, "疵検管理No", cKizukenNo) ) {
	if( ! Delete_OldInf( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, cKizukenNo) ) {
		return;		// 削除失敗
	}
	if(my_bStopFlg) return; // 終了？


	//// 取得したコイルに紐付く情報を削除
	for(int jj=0; jj<m_nDelTblCoil; jj++) {
		// 一括/少量削除 判定
		if( ! Delete_OldInf( &m_pDelTblCoil[jj], cKizukenNo ) ) {
			return;		// 削除失敗
		}			
		if(my_bStopFlg) return; // 終了？
	}
	
}