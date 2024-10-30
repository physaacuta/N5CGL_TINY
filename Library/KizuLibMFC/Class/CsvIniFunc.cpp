#include "StdAfx.h"
#include "CsvIniFunc.h"


using namespace KizuLib;

//------------------------------------------
// コンストラクタ
//------------------------------------------
CsvIniFunc::CsvIniFunc(void)
{
	// セクション開始区切り
	memset(m_cSecHead, 0x00, sizeof(m_cSecHead));
	m_cSecHead[0][0] = SEC_HEAD_S;
	m_cSecHead[1][0] = SEC_HEAD_E;

}
//------------------------------------------
// デストラクタ
//------------------------------------------
CsvIniFunc::~CsvIniFunc(void)
{
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共通情報

//------------------------------------------
// CSVファイル読込
// LPCSTR sSec	CSVの[]までのキー
// LPCSTR sPath	Iniファイルパス
// std::vector<CString>* pRow	1行単位の文字リスト		
//------------------------------------------
bool CsvIniFunc::ReadCsv(LPCSTR sSec, LPCSTR sPath, std::vector<CString>* pRow)
{
	char cWk[2048];
	int	 nWk;
	bool bSecIni = false;			// 指定セクションを検出したら、true


	//================================================
	// オープン
	FILE* fp = fopen(sPath, "rt"); 
	if(NULL == fp) {
		return false;
	}

	//================================================
	// 読み込み
	while (!feof(fp)) {
		char* ret = fgets(cWk, sizeof(cWk), fp);	// 1行取得
		if( 0 == ret ) break;

		//// 空白行, コメント行は 対象外
		bool bAddOk = false;				// 有効行
		bool bSectionFind = false;			// セクション行
		nWk = -1;
		int nCnt = (int)strlen(cWk);
		// 先端のみチェックパフォーマンス対策
		if( ! bSecIni ) {
			if( 10 < nCnt ) nCnt = 10;			
		}
		for(int ii=0; ii<nCnt; ii++) {
			nWk ++;

			if( cWk[nWk] == ' ' || cWk[nWk] == '\t' ) continue;
			if( cWk[nWk] == 0x0d || cWk[nWk] == 0x0a) {
				// ここで文字列が終わりの認識にする
				cWk[nWk] = 0x00;
				break;
			}
			//コメント行チェック。
			if(!bAddOk && cWk[nWk] == '#' )
			{
				cWk[nWk] = 0x00;
				break;
			}
			//// セクション開始区切りをサーチ。先頭文字限定
			if(!bAddOk && strchr( m_cSecHead[0], cWk[nWk] ) != NULL ){ 
				bSectionFind=true;
				break;
			}

			bAddOk = true;
		}

		if( bSectionFind && ! bSecIni ) {
			// 同一セクションか？
			if(NULL != strstr(cWk, sSec)) {					
				bSecIni = true;
				continue;				// セクション部分は無視
			}
		}

		//// あたらしいセクションがでてきたのでこのセクションは終わり。
		if( bSectionFind && bSecIni ) {
			break;
		}


		//// ここまで来たら、指定セクション内での追加対象データ
		if(bAddOk && bSecIni) {
			CString s = cWk;
			//s.Remove('"');
			//s.Remove(' ');			// SQLとかの意味のあるものまでNGとなる
			//s.Remove('	'); 
			//s.Remove(0x0a); 
			//s.Remove(0x0d); 
		
			s.Trim(" \t");

			// [,,] だと2個として取得できないので、 [, , ] というようにスペースを必ず挿入 重要
			//s.Replace(_T(",,"), _T(", , "));


			if( !s.IsEmpty() ) {
				pRow->push_back(s);
			}
		}
	}

	//================================================
	// クローズ
	fclose(fp);
	return true;
}

//------------------------------------------
// 要素が何個あるか (単純に , が何個あるかだけで判断)
// CString& sVal 1行単位の情報
// 戻り値 要素数
//------------------------------------------
int CsvIniFunc::CntColumn(const CString& sVal)
{
	CString sWk = sVal;
	sWk.Trim(_T(" 	"));

	// 基本を取得
	int nTotal = sWk.GetLength();
	if( 0 == nTotal ) return 0;
	
	// , 削除
	sWk.Remove(',');
	return nTotal - sWk.GetLength() + 1;
}

//------------------------------------------
// １行のデータから , 単位の文字列を取得
// CString& sVal 1行単位の情報
// int& curPos 区切り位置
// CString& sToken 取得した列の文字列
// 戻り値 復帰情報
//------------------------------------------
bool CsvIniFunc::GetColumn(const CString& sVal, int& curPos, CString& sToken)
{
	static const char CH_DQUOTE = '"';
	static const char CH_COMMA  = ',';

	// ダブルクォーテーション中にカンマがある場合は文字として扱う
	// ダブルクォーテーション中に、文字としてのダブルクォーテーションは、
	// その直前にひとつダブルクォーテーションを付加して、エスケープされる。
	CString sWk = "";
	int	nLength = sVal.GetLength();
	bool bWQuote = false;				// ダブルクォーテーション中かどうか。

	int ii;
	for(ii=curPos; ii<nLength; ii++)
	{
		const char ch = sVal[ii];
		// 対象の文字がダブルクォーテーションの場合
		if(CH_DQUOTE == ch) {

			if(ii+1 >= nLength) break; // 行末がダブルクォーテーションの場合は無視してbreak
			const char ch2 = sVal[ii+1];
			// エスケープ対象（文字扱いのダブルクォーテーション）かをチェック。
			if(bWQuote && CH_DQUOTE==ch2) {
				// ２文字が１文字になるので、iiはここで１すすめる。
				// したのcontinueとあわせて結局は２すすむ。
				ii++;

				sWk += ch;
			}
			else {
				// 裸のダブルクォテーションが現れたので、
				// ダブルクォテーション中状態は、外→中、中→外と反転する。
				bWQuote = !bWQuote;
			}
			continue;
		}

		//ダブルクォーテーション中の場合は、区切り文字としては扱わない。
		if(!bWQuote){
			if(CH_COMMA == ch) {
				// 区切りとしてのカンマなのでフィールド終了。
				 break; 
			}
			if( '#' == ch ){
				//裸で出てきたシャープは以降をコメント扱いする。
				ii = nLength;
				break;
			}
		}


		//フィールド文字列の要素なので、追記する。
		sWk += ch;
	}
	sWk.Trim(_T(" 	"));					// 前後のスペース、タブを取り除く
	sToken = sWk;							// 今回のフィールド文字列
	curPos = ii+1;							// 区切り位置を、次のフィールドまで進める

	// 読み込んだ文字列がない
	if( 0 == sWk.GetLength() ) return false;
	return true;
}

CString CsvIniFunc::GetColumnStr(const CString& sVal, int& curPos)
{
	CString		sWk;
	if( ! GetColumn(sVal, curPos, sWk) ) return "";
	if( 0 == sWk.CompareNoCase("NULL") ) return "";	// NULLチェック
	return sWk;
}
void CsvIniFunc::GetColumnChar(const CString& sVal, int& curPos, char* cAns, int nSize)
{
	CString		sWk;
	cAns[0] = 0x00;
	if( ! GetColumn(sVal, curPos, sWk) ) return;
	if( 0 == sWk.CompareNoCase("NULL") ) return;	// NULLチェック

	int nCopyLength = sWk.GetLength();
	if(nCopyLength > nSize - 1) nCopyLength = nSize - 1;
	memset( cAns, 0x00, nSize);
	memcpy( cAns, sWk, nCopyLength);

}
int CsvIniFunc::GetColumnInt(const CString& sVal, int& curPos)
{
	CString		sWk;
	if( ! GetColumn(sVal, curPos, sWk) ) return 0;
	if( 0 == sWk.CompareNoCase("NULL") ) return 0;	// NULLチェック
	return atoi(sWk);
}
double CsvIniFunc::GetColumnDouble(const CString& sVal, int& curPos)
{
	CString		sWk;
	if( ! GetColumn(sVal, curPos, sWk) ) return 0;
	if( 0 == sWk.CompareNoCase("NULL") ) return 0;	// NULLチェック
	return atof(sWk);
}