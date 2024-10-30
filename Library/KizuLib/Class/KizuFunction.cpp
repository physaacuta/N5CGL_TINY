#include "StdAfx.h"
#include "KizuFunction.h"

#include <stdio.h>
#include <fstream>	
#include <direct.h>
#include <math.h>

#include "..\..\..\Include\KizuCommon.h"							// 疵検共通インクルード
#include "..\KizuLib.h"												// 疵検ライブラリインクルード
#include "..\..\KizuODBC\OdbcBase.h"								// ODBC操作クラス
#include "KizuMacro.h"												// 疵検マクロ

#include <tchar.h>
using namespace KizuODBC;
using namespace KizuLib;


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 機器状態関係

//------------------------------------------
// DB_SYSTEM_STATEに機器状態をセットする
// int id 機器ID
// int state 機器状態 (0:正常  1～:異常)
// bool bLogDsp シスログ出力の有無
// char const * cMsg 追加メッセージ
//------------------------------------------

bool KizuFunction::SetStatus(char const* cSec, char const * cKeyFormat, int id, int state, bool bLogDsp, char const * cMsg)
{
	char cKey[256];
	memset(cKey, 0x00, sizeof(cKey));
	sprintf_s(cKey, sizeof(cKey), cKeyFormat, id);

	return SetStatus(cSec, cKey, state, bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(char const* cSec, char const * cKey, int state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(cSec, cKey, 0, TASKINI_NAME);
	return SetStatus(id, state, bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(char const* cSec, char const * cKey, bool state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(cSec, cKey, 0, TASKINI_NAME);
	return SetStatus(id, (state ? 0 : 1), bLogDsp, cMsg );
}

bool KizuFunction::SetStatus(char const * cKeyFormat, int id, int state, bool bLogDsp, char const * cMsg)
{
	char cKey[256];
	memset(cKey, 0x00, sizeof(cKey));
	sprintf_s(cKey, sizeof(cKey), cKeyFormat, id);

	return SetStatus(cKey, state, bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(char const * cKey, int state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(INI_STATUS, cKey, 0, TASKINI_NAME);
	return SetStatus(id, state, bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(char const * cKey, bool state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(INI_STATUS, cKey, 0, TASKINI_NAME);
	return SetStatus(id, (state ? 0 : 1), bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(int id, bool state, bool bLogDsp, char const * cMsg)
{
	return SetStatus(id, (state ? 0 : 1), bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(int id, int state, bool bLogDsp, char const * cMsg)
{
	//if(0==id) {
	//	MessageBox(NULL, "SetStatus", "", 0);
	//}
	_ASSERT(0!=id);

	//// DB接続
	OdbcBase cls_DB;						// 実体有りのDB接続クラス
	if( ! cls_DB.Connect(TASKINI_NAME) ){
		syslog(SYSNO_DB_CONNECT_ERR,"[%s書込み失敗 ID=%d State=%d]", DB_STATUS_INF, id, state);
		return false;
	}
	
	//// SQL文作成
	char cSql[256];
	int len=0;
	memset(cSql, 0x00, sizeof(cSql));
	sprintf( cSql, "UPDATE %s SET 機器状態=%d WHERE 機器ID=%d", DB_STATUS_INF, state, id );

	//// SQL文実行
	//if( ! cls_DB.ExecuteDirect(cSql) ) {
	//	syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[%s書込み失敗 ID=%d State=%d]", DB_STATUS_INF, id, state);
	//	return false;
	//}
	bool bRetc = cls_DB.ExecuteDirect(cSql);
	if( !bRetc ) {
		cls_DB.DisConnect();
		if( ! cls_DB.Connect(TASKINI_NAME) ){
			syslog(SYSNO_DB_CONNECT_ERR,"[%s書込み失敗(2回目) ID=%d State=%d]", DB_STATUS_INF, id, state);
			return false;
		}
		bRetc = cls_DB.ExecuteDirect(cSql);
	}
	if( ! bRetc ) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[%s書込み失敗 ID=%d State=%d]", DB_STATUS_INF, id, state);
		return false;
	}


	//// コミット
	cls_DB.TranCommit(); 
	//// 切断
	cls_DB.DisConnect();

	//// シスログ出力
	if(bLogDsp) {
		if(0==state) statelog(SYSNO_STATUS_OK, id, state, cMsg );
		else		 statelog(SYSNO_STATUS_NG, id, state, cMsg );
	}
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 外部I/F関係

//------------------------------------------
// ダンプ出力関数
//   データ行でない部分は 先頭に # をつける事。(疵検ダンプファイル専用フォーマット)
// LPCSTR filename ダンプファイル名
// long len 出力サイズ
// const BYTE* data データ
// bool addTime ダンプファイル名に日付を付与するしない
//------------------------------------------
void KizuFunction::DumpData(LPCSTR filename, long len, const BYTE* data, bool addTime)
{
	//// VS2003 → VS2005 では、
	// ofstream::open()関数の実装が変わり、VS2003ではマルチバイト文字列のまま扱っていたのが、
	// VS2005ではワイド文字に変換してから扱うようになったため、変換がうまく動くようにロケールを設定してあげないといけない
	setlocale(LC_ALL, "Japanese"); 

	/////// データ行でない部分は 先頭に # をつける事。(疵検ダンプファイル専用フォーマット)
	// ログフォルダ生成
	_mkdir("Dump");
	// ファイルオープン
	std::ofstream	os;
	char			cFileName[MAX_PATH];
	sprintf(cFileName, ".\\Dump\\%s", filename );
	if(addTime) {									// 通信テスト時に使用 (受信毎にファイル生成)
		SYSTEMTIME time;
		GetLocalTime(&time);
		//cFileName.Format("Dump\\%04d%02d%02d_%02d%02d%02d_%03d_%s.log", 
		sprintf(cFileName, ".\\Dump\\%04d%02d%02d_%02d%02d%02d_%03d_%s", 
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
			filename);
	}
	os.open(cFileName, std::ios::out | std::ios::trunc);

	// ダンプ出力
	char buf[64];
	char wk[64];
	int ii;
	int rec = 0;

	sprintf(buf, "# %d[byte]送受信", len);
	os << buf << std::endl;
	os << "# (Hex)ADDR(Dec)  :  0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F | [ASCII]" << std::endl;

	while(true) {
		if(rec == len) break;		// 最後まで到達

		sprintf(buf, "%07X (%07d) :", rec, rec);
		os << buf;

		memset(wk, 0x00, sizeof(wk));
		//strcpy(wk, " | ");		// _sがついてから、NULLの後ろが不定値になってしまう。
		memcpy(wk, &" | ", sizeof(" | ") );

		for(ii=0; ii<16; ii++) {
			if(ii == 8 ) os << " -";
			sprintf(buf, " %02X", data[rec]);
			os << buf;

			if( data[rec] < 0x20 || data[rec] > 0x7F )	wk[ii+3] = '･'; //0x20;		// " | "の3文字分
			else										wk[ii+3] = data[rec];	// " | "の3文字分

			// 次
			rec ++;
			if( rec == len ) {							// 最終データ
				for(int jj=ii+1; jj<16; jj++) {					// 余白分設定
					if(ii == 8 ) os << " -";
					os << "   ";
				}
				os << wk << std::endl;
				break;
			}
			if( ii == 15 ) {							// 1行終わり 
				os << wk << std::endl;
				break;
			}
			/*
			if( ii == 15 || rec == len) {	// 1行終わり 又は バッファ最後 
				os << wk << std::endl;
				break;
			}
			*/
		}
	}
	os << std::endl;
	os.close();
}

//------------------------------------------
// 4バイト反転 (元データ書き換えの為 注意)
// long len 反転データサイズ (4バイト単位)
// BYTE* data 変換データの先頭ポインタ
//------------------------------------------
BOOL KizuFunction::ByteHanten4(long len, BYTE* data) 
{
	BYTE wk;

	//// 4バイト単位でなければならない
	if( 0 != (len%4) ) return FALSE;
	
	//// 反転 (ABCD → DCBA)
	for( int ii=0; ii<len; ii+=4 ){
		// AとD入れ替え
		wk = data[ii];
		data[ii] = data[ii+3];
		data[ii+3] = wk;
		// BとC入れ替え
		wk = data[ii+1];
		data[ii+1] = data[ii+2];
		data[ii+2] = wk;
	}
	return TRUE;
}

//------------------------------------------
// 2バイト反転 (元データ書き換えの為 注意)
// long len 反転データサイズ (2バイト単位)
// BYTE* data 変換データの先頭ポインタ
//------------------------------------------
BOOL KizuFunction::ByteHanten2(long len, BYTE* data) 
{
	BYTE wk;

	//// 2バイト単位でなければならない
	if( 0 != (len%2) ) return FALSE;
	
	//// 反転 (AB → BA)
	for( int ii=0; ii<len; ii+=2 ){
		// AとB入れ替え
		wk = data[ii];
		data[ii] = data[ii+1];
		data[ii+1] = wk;
	}
	return TRUE;
}
//------------------------------------------
// 4バイト反転後のデータ取得 (元データは変わらない)
// BYTE* data 変換データの先頭ポインタ
// 戻り値 反転した結果
//------------------------------------------
long KizuFunction::ByteHanten4(BYTE* data) 
{
	long wk;
	// ABCD → DCBAに
	((BYTE*)&wk)[0] = data[3];
	((BYTE*)&wk)[1] = data[2];
	((BYTE*)&wk)[2] = data[1];
	((BYTE*)&wk)[3] = data[0];
	return wk;
}
//------------------------------------------
// 2バイト反転後のデータ取得 (元データは変わらない)
// BYTE* data 変換データの先頭ポインタ
// 戻り値 反転した結果
//------------------------------------------
short KizuFunction::ByteHanten2(BYTE* data) 
{
	short wk;
	// ABCD → DCBAに
	((BYTE*)&wk)[0] = data[1];
	((BYTE*)&wk)[1] = data[0];
	return wk;
}

//------------------------------------------
// スペースをNULLに変更する
// char* cVal 変換文字
// int size チェック最大文字数
//------------------------------------------
void KizuFunction::SpaceToNull(char* cVal, int size) 
{
	for(int ii=0; ii<size; ii++) {
		if( cVal[ii] == ' ' ) {
			cVal[ii] = 0x00;
			return;
		} else if( cVal[ii] == 0x00 ) {
			return;
		}
	}
}

//------------------------------------------
// スペースをトリムする
// char* cVal 変換文字 (終端はNULLであること)
// int nMode トリム種類 (0両方 1:先頭のみ 2:エンドのみ)
//------------------------------------------
void KizuFunction::Trim(char* cVal, int nMode) 
{
	if( NULL == cVal) return;
	int ii = (int)strlen(cVal);
	if( 0 == ii ) return;

	//// 末から
	if(0 == nMode || 2 == nMode ) {
		while(--ii >= 0 && cVal[ii] == ' ' ) ;
		cVal[ii+1] = 0x00;
	} 

	if(0 == nMode || 1 == nMode ) {
		ii = 0;
		while( cVal[ii] != 0x00 && cVal[ii] == ' ') ii++;
		strcpy( cVal, &cVal[ii]);
	}
}
//------------------------------------------
// 固定長文字からStringに変換。
// char* cT			変換後文字
// int nTSize		変換後領域サイズ (基本的に、元領域よりも+1大きいこと)※同じはOKだが、元よりも小さいのは絶対NG
// char const* cF	元文字
// int nFSize		元領域サイズ
// int nMode トリム種類 (0両方 1:先頭のみ 2:エンドのみ)
//------------------------------------------
void KizuFunction::TrimSpaceNull(char* cT, int nTSize, char const* cF, int nFSize, int nMode)
{
	if( NULL == cF) return;
	if( NULL == cT) return;
	memset(cT, 0x00, nTSize);
	memcpy(cT, cF, nFSize);
	int nEnd = __min(nFSize, nTSize-1);
	cT[nEnd] = 0x00;
	KizuFunction::Trim(cT, nMode);
}

//------------------------------------------
// 指定数分の文字を使用してIntに変換
// char const* c 対象文字
// int size 指定文字数
//------------------------------------------
int KizuFunction::AtoIex(char const* c, int size)
{
	char wk[32];
	memcpy(wk, c, size);
	wk[size] = 0x00;

	return atoi(wk);
}
//------------------------------------------
// 指定数分の文字を使用してFloatに変換
// char const* c 対象文字
// int size 指定文字数
//------------------------------------------
float KizuFunction::AtoFex(char const* c, int size)
{
	char wk[32];
	memcpy(wk, c, size);
	wk[size] = 0x00;

	return (float)atof(wk);
}
//------------------------------------------
// 指定数分の文字を使用してDoubleに変換
// char const* c 対象文字
// int size 指定文字数
//------------------------------------------
double KizuFunction::AtoDex(char const* c, int size)
{
	char wk[32];
	memcpy(wk, c, size);
	wk[size] = 0x00;

	return atof(wk);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 主キー生成

//------------------------------------------
// 基本となる疵検管理Noを生成 (ID+yyyymmdd hhnnss\0)
// char* cNo 生成される疵検管理No
//------------------------------------------
void KizuFunction::GetKizukenNo(char* cNo) 
{
    SYSTEMTIME time;
    GetLocalTime(&time);

	char cWk[SIZE_KIZUKEN_NO];
	int size;
	if( 0 != strcmp("", KIZUKEN_CODE) ) {
		size = sprintf(cWk, "%02s%04d%02d%02d %02d%02d%02d", KIZUKEN_CODE,
								time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	} else {
		size = sprintf(cWk, "%04d%02d%02d %02d%02d%02d", 
								time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	}
	memset(cNo, 0x00, SIZE_KIZUKEN_NO);
	memcpy(cNo, cWk, size);
}
//------------------------------------------
// 基本となるシリアルNoを生成 (yyyymmdd hhnnss\0)
// char* cNo 生成される疵検管理No
//------------------------------------------
void KizuFunction::GetSerialNo(char* cNo) 
{
    SYSTEMTIME time;
    GetLocalTime(&time);

	char cWk[SIZE_SERIAL_NO];
	int size = sprintf(cWk, "%04d%02d%02d %02d%02d%02d", 
								time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	memset(cNo, 0x00, SIZE_SERIAL_NO);
	memcpy(cNo, cWk, size);
}
//------------------------------------------
// 基本となるIDNoを生成 (yyyymmdd hhnnssff\0)
// char* cNo 生成されるID (20桁)
//------------------------------------------
void KizuFunction::GetSerialNoFF(char* cNo) 
{
    SYSTEMTIME time;
    GetLocalTime(&time);

	char cWk[SIZE_SERIAL_FF_NO];
	int size = sprintf(cWk, "%04d%02d%02d %02d%02d%02d%03d", 
								time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	memset(cNo, 0x00, SIZE_SERIAL_FF_NO);
	memcpy(cNo, cWk, size);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 内部処理関係

//------------------------------------------
// NetBIOSのコンピューター名前
// char const* cNewNetBiosComputerName 新しいコンピューター名
//------------------------------------------
bool KizuFunction::SetNetBiosComputerName(char const* cNewNetBiosComputerName)
{
	//// コンピューター名を変更する
	// ※ ただし、NetBIOSの名前ね。 (システムのプロパティ / コンピュータ名 / 変更 / 詳細 に出て来る名前)
	
	int retc = SetComputerName(cNewNetBiosComputerName);
	return (1==retc ? true : false); 
}

//------------------------------------------
// ファイルサイズを取得する
// char const* cFPath 取得したいファイルのフルパス
// 戻り値: -1:異常  1～:ファイルサイズ
//------------------------------------------
long KizuFunction::GetFileSize(char const* cFPath)
{
	//// ファイル関係 のデータ事前準備
	FILE*	fp = NULL;			// ファイルポインタ
	fpos_t	size;				// ファイルサイズ

	// ファイルオープン
	fp = fopen(cFPath, "rb");
	if(fp == NULL){        // オープン失敗
		return -1;
	}
	// ファイルサイズ取得
	fseek(fp, 0, SEEK_END);					// 尾端に移動
	fgetpos(fp, &size);						// ファイルポインタ位置取得 = 最終位置なのでファイルサイズが分かる
	fclose(fp);
	
	return (long)size;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 疵検独自で良く使用する関数

//------------------------------------------
// 欠陥優先度比較
// double nowLen (新欠陥) 長手方向位置 [mm] 
// int nowYusenT (新欠陥) 疵種優先度     (1が最強)
// int nowYusenG (新欠陥) グレード優先度 (1が最強)
// double preLen (旧欠陥) 長手方向位置 [mm]
// int preYusenT (旧欠陥) 疵種優先度
// int preYusenG (旧欠陥) グレード優先度
// 戻り値 true:新欠陥の方が強い  false:旧欠陥の方が強い
//------------------------------------------
bool KizuFunction::SelectPriorityDetect(double nowLen, int nowYusenT, int nowYusenG,
										double preLen, int preYusenT, int preYusenG )
{
	//// チェック条件。上が一番強い
	// ・Gr
	// ・疵種
	// ・長手方向
	// ・上記3つも一緒なら、旧欠陥を優先
	if( nowYusenG < preYusenG ) {				// 今回のグレードの方が強い (数値が小さい)
		return true;
	} else if( nowYusenG == preYusenG ) {		// グレードの優先度が同じ
		if( nowYusenT < preYusenT ) {				// 今回の疵種の方が強い
			return true;
		} else if( nowYusenT == preYusenT ) {
			if ( nowLen < preLen ) {					// 今回の長手方向位置が小さい（強い）
				return true;
			}
		}
	}
	return false;				// ここまできたら、旧欠陥を優先
}

//------------------------------------------
// 欠陥優先度比較 Ex
//	 各変数配列に情報をセット
// int nCnt				比較数
// double const* pNow	比較する新情報を順番にセット(配列の先頭が最優先) 
// double const* pPre	比較する旧情報を順番にセット(配列の先頭が最優先) 
// bool const* bBig		比較条件 (true:新旧比較したときに大きい方が強い。 false:新旧比較したときに小さい方が強い)
// 戻り値 true:新欠陥の方が強い  false:旧欠陥の方が強い
//------------------------------------------
bool KizuFunction::SelectPriorityDetect(int nCnt, double const* pNow, double const* pPre, bool const* bMode)
{
	for(int ii=0; ii<nCnt; ii++) {
		if(pNow[ii] == pPre[ii]) continue;

		if( bMode[ii] ) {
			// 大きい方が強い
			if(pNow[ii] > pPre[ii]) return true;
			else					return false;
		} else {
			// 小さい方が強い
			if(pNow[ii] < pPre[ii]) return true;
			else					return false;
		}
	}
	return false;				// 全部一緒なら 旧欠陥を優先
}

//------------------------------------------
// グリッドの列数を算出する
// int width 算出したい幅
// int interval 列の間隔
// int maxCol 最大の列数 
// 戻り値 グリッド数 
//------------------------------------------
int KizuFunction::SelectGridCol( int width, int interval, int maxCol )
{
	// グリッド位置 (0～199:0、200～299:1)
	int ii;

	// 板幅不正により、最大にする
	if( 0 >= width ) return maxCol;
	// 板幅による算出
	for(ii=1; ii<(maxCol/2); ii++) {
		if( width < (interval*2)*ii ) break;
	}
	if( ii < (maxCol/2) ) return ii*2;
	else				  return maxCol;
}

//------------------------------------------
// グリッドの列数を算出する (奇数マップに対応)
// int width 算出したい幅
// int interval 列の間隔
// int maxCol 最大の列数 
// 戻り値 グリッド数 
//------------------------------------------
int KizuFunction::SelectGridCol2( int width, int interval, int maxCol )
{
	//// グリッド位置 (0～200:0、200～400:1)

	// 板幅不正により、最大にする
	if( 0 >= width ) return maxCol;

	// 板幅による算出
	int nCol  = 0;
	int env	  = maxCol % 2;		// 探傷画面の列数 (0:偶数列  1:奇数列)

	// 初回の境界位置決定
	if(0 == env) {		// 偶数	
		nCol = ( GET_JUST_INDEX(width, (interval*2) ) + 1 ) * 2;

	} else {			// 奇数
		width -= interval;
		if(0 >= width ) return 1;		// 1マス以内の場合の特殊処理

		nCol = ( GET_JUST_INDEX(width, (interval*2) ) + 1 ) * 2 + 1;
	}

	// 整合性チェック
	if(maxCol < nCol)	return maxCol;
	if(0 > nCol)		return maxCol;
	return nCol;
}

//------------------------------------------
// グリッド位置判定関数 (エッジからの距離基準)
// int DorW 0:DS基準の疵 1:WS基準の疵
// double edge_D 左エッジからの距離 [mm]
// double edge_W 右エッジからの距離 [mm]
// long width コイル板幅 [mm] (整合性チェック後の値である事)
// long col_width 板幅によるグリッド数  (論理的な最大グリッド数)
// long col_max 最大グリッド			(物理的な最大グリッド数)
// long col_interval グリッドの幅 [mm]
// 戻り値 算出グリッド位置 (0:DS～7:WS)
//------------------------------------------
int KizuFunction::SelectColPosition(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval)
{
	int centerWid = width / 2;			// 板幅の中心
	int widthCol = col_width / 2;		// 整合性チェック用
	int centerCol = col_max / 2;		// DS基準のグリッド位置算出用 (戻り値)
	int col;							// 算出グリッド

	//-----------------------------------------
	// エッジからの距離基準
	//-----------------------------------------
	if (false) {
		//// グリッド位置
		if (0 == DorW)	col = (int)(edge_D / col_interval);				// エッジからどれだけ離れているか
		else			col = (int)(edge_W / col_interval);

		//// 整合性チェック
		// コイル幅とグリッド個数が一致しない為、グリッド数よりコイル幅が広い場合は、一番端のグリッドとする
		if (0 > col)			col = 0;
		if (widthCol <= col)	col = widthCol - 1;

		//// 基準位置からの絶対位置に変換
		if (true) {							// DS=0基準
			if (0 == DorW)	return (centerCol - widthCol) + col;		// 基準側
			else			return (centerCol + widthCol) - col - 1;		// 反対側

		} else {							// WS=0基準
			if (0 == DorW)	return (centerCol + widthCol) - col - 1;		// 反対側
			else			return (centerCol - widthCol) + col;		// 基準側

		}


	//-----------------------------------------
	// コイルセンター基準
	//-----------------------------------------
	} else {
		//// グリッド位置 (0～200:0、201～400:1)
		if (0 == DorW)	col = (int)((centerWid - edge_D) / col_interval);				// コイル中心からどれだけ離れているか
		else			col = (int)((centerWid - edge_W) / col_interval);

		//// 整合性チェック
		if (0 > col)			col = 0;
		if (widthCol <= col)	col = widthCol - 1;

		//// 基準位置からの絶対位置に変換
		if (true) {							// DS=0基準
			if (0 == DorW)	return centerCol - col - 1;		// 基準側
			else			return centerCol + col;			// 反対側

		} else {							// WS=0基準
			if (0 == DorW)	return centerCol + col;			// 反対側
			else			return centerCol - col - 1;		// 基準側

		}
	}
}

//------------------------------------------
// グリッド位置判定関数 (センター基準)
// int DorW 0:DS基準の疵 1:WS基準の疵

// double edge_D 左エッジからの距離 [mm]
// double edge_W 右エッジからの距離 [mm]
// long width コイル板幅 [mm] (整合性チェック後の値である事)
// long col_width 板幅によるグリッド数  (論理的な最大グリッド数)
// long col_max 最大グリッド			(物理的な最大グリッド数)
// long col_interval グリッドの幅 [mm]
// 戻り値 算出グリッド位置 (0:DS～)
//------------------------------------------
// --->>> 20180220 不具合修正
//int KizuFunction::SelectColPosition2(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval)
//{
//	//// グリッド位置 (0～200:0、201～400:1)
//
//	int env = col_max % 2;		// 探傷画面の列数 (0:偶数列  1:奇数列)
//	int centerWid = width / 2;		// 板幅の中心
//	int widthCol = col_width / 2;	// 整合性チェック用
//	int centerCol = col_max / 2;		// DS基準のグリッド位置算出用 (戻り値)
//	int centerX;						// センターからの疵位置
//
//
//	//-----------------------------------------
//	// センターからの疵位置
//	if (0 == DorW)	centerX = (int)(centerWid - edge_D);
//	else			centerX = (int)(centerWid - edge_W);
//	if (0 > centerX) {
//		DorW = (0 == DorW ? 1 : 0);
//		centerX = abs(centerX);
//	}
//
//
//	//-----------------------------------------
//	// コイルセンター基準
//	if (0 == env) {		// 偶数	
//		int col = GET_JUST_INDEX(centerX, (col_interval));
//
//		if (0 == DorW)	return centerCol - col - 1;		// 基準側
//		else			return centerCol + col;			// 反対側
//
//
//	}
//	else {			// 奇数
//		centerX -= col_interval / 2;
//		if (0 >= centerX) return centerCol;		// 中央
//
//		int col = GET_JUST_INDEX(centerX, (col_interval));
//
//		if (0 == DorW)	return centerCol - col - 1;		// 基準側
//		else			return centerCol + col + 1;		// 反対側
//	}
//}
int KizuFunction::SelectColPosition2(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval)
{
	//// グリッド位置 (0～200:0、201～400:1)

	int env = col_max % 2;				// 探傷画面の列数 (0:偶数列  1:奇数列)
	double centerWid = width / 2;		// 板幅の中心
	int widthCol = col_width / 2;		// 整合性チェック用
	int centerCol = col_max / 2;		// DS基準のグリッド位置算出用
	int centerX;						// センターからの疵位置
	int col;							// 戻り値


	//-----------------------------------------
	// センターからの疵位置
	if (0 == DorW)	centerX = (int)ceil(centerWid - edge_D);
	else			centerX = (int)ceil(centerWid - edge_W);
	if (0 > centerX) {
		DorW = (0 == DorW ? 1 : 0);
		centerX = abs(centerX);
	}


	//-----------------------------------------
	// コイルセンター基準
	if (0 == env) {		// 偶数	
		col = GET_JUST_INDEX(centerX, (col_interval));

		if (0 == DorW)	col = centerCol - col - 1;		// 基準側
		else			col = centerCol + col;			// 反対側


	} else {			// 奇数
		centerX -= col_interval / 2;
		if (0 >= centerX) return centerCol;		// 中央

		col = GET_JUST_INDEX(centerX, (col_interval));

		if (0 == DorW)	col = centerCol - col - 1;		// 基準側
		else			col = centerCol + col + 1;		// 反対側
	}

	// 整合性チェック
	if (col < (centerCol - widthCol))				col = (centerCol - widthCol);
	if (col >= col_max - (centerCol - widthCol))	col = col_max - (centerCol - widthCol);

	return col;
}
// <<<--- 20180220 不具合修正

//------------------------------------------
// グリッド位置判定関数 (列数固定バージョン)
// int DorW 0:DS基準の疵 1:WS基準の疵

// double edge_D 左エッジからの距離 [mm]
// double edge_W 右エッジからの距離 [mm]
// long width コイル板幅 [mm] (整合性チェック後の値である事)
// long col_max 最大グリッド			(物理的な最大グリッド数)
// 戻り値 算出グリッド位置 (0:DS～)
//------------------------------------------
int KizuFunction::SelectColPositionFix(int DorW, double edge_D, double edge_W, long width, long col_max)
{
	int cMaxCol		= col_max*2;		// 算出上で、奇数列の場合考慮し、偶数列にして計算させる
	int nWidIntval	= width / cMaxCol;	// 列幅を都度算出
	
	// 2倍の列数でとりあえず算出
	int col = SelectColPosition2(DorW, edge_D, edge_W, width, cMaxCol, cMaxCol, nWidIntval);

	// 整合性チェック
	if( 0 > col || col >= cMaxCol ) {
		//if( col > 0 )	col = 0;
		if( col < 0 )	col = 0;		// 20150305 不具合対応
		else			col = cMaxCol - 1;
	}

	// 本来の列数に戻し
	return (col/2);
}