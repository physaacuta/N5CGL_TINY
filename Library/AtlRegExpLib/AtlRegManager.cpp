#include "StdAfx.h"
#include "AtlRegManager.h"

#ifdef ATLREGEXPLIB_IS_ATL
	// VS2005 以前用
	#include <atlrx.h>
#else
	// VS2008　以降用
	// atlrx.h が廃止され、stdに正規表現が追加された為の対応
	#include <regex>

	using namespace std;
#endif

//------------------------------------------
// コンストラクタ
//------------------------------------------
AtlRegManager::AtlRegManager(void)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
AtlRegManager::~AtlRegManager(void)
{
}


//------------------------------------------
// 正規表現 をチェック [1byte ascii] (日本語未対応) ← 一応対応は出来そう
// char const* val チェックしたい文字列
// char const* match 正規表現 例(みんなが間違うワイルドカードの指定方法) ".*" ね
// 戻り値 true:一致  false:ダメ
//------------------------------------------
bool AtlRegManager::IsRegularA(char const* val, char const* match)
{
	//char val[] = "100-12345";
	//char match[] = "{-}{.*}{2}{.*}5$";		// OKとなる

#ifdef ATLREGEXPLIB_IS_ATL
	CAtlRegExp<> re; 
	CAtlREMatchContext<> mc; 


	// 正規表現の戻り値チェック。 "aaa(" とかだと無限ループになる対策
	//re.Parse(match);					// 正規表現セット
	if( REPARSE_ERROR_OK != re.Parse(match) ) {
		return false;
	}


	if(re.Match(val, &mc)) {				// 検索
		return true;
	} else {
		return false;
	}
#else
	/***
	tr1::cmatch		mr;
	tr1::regex		rx(match); 
	
	const char * last = val + strlen(val);

	if( tr1::regex_search( val, last, mr, rx) ) {
		return true;
	} else {
		return false;
	}
	***/

	return IsRegularA(val, (int)strlen(val), match);
#endif
}
//------------------------------------------
// 正規表現 をチェック [1byte ascii] 
// char const* val チェックしたい文字列
// int valSize チェックしたい文字列のサイズ (よくプロコン等でNULLが無い文字列がある為の対策)
// char const* match 正規表現 例(みんなが間違うワイルドカードの指定方法) ".*" ね
// 戻り値 true:一致  false:ダメ
//------------------------------------------
bool AtlRegManager::IsRegularA(char const* val, int valSize, char const* match)
{
#ifdef ATLREGEXPLIB_IS_ATL	

	char* pWk = new char[valSize+1];
	memset(pWk, 0x00, valSize+1);
	memcpy(pWk, val, valSize);

	// 比較
	bool retc = IsRegularA(pWk, match);

	// 後始末
	delete [] pWk;
	return retc;

#else
	try {
		tr1::cmatch		mr;
		tr1::regex		rx(match); 
	
		const char * last = val + valSize;

		if( tr1::regex_search( val, last, mr, rx) ) {
			return true;
		} else {
			return false;
		}

	} catch (...) {
		 return false;
	}
	
#endif
}

//------------------------------------------
// 正規表現 をチェック [マルチバイト] (日本語対応) (むちゃむちゃ遅いから注意)
// unsigned char const* val チェックしたい文字列 (BYTE型でもOK)
// unsigned char const* match 正規表現 例(みんなが間違うワイルドカードの指定方法) ".*" ね
// 戻り値 true:一致  false:ダメ
//------------------------------------------
bool AtlRegManager::IsRegularMB(unsigned char const* val, unsigned char const* match)
{
	//char val[] = "100-12345";
	//char match[] = "{-}{.*}{2}{.*}5$";		// OKとなる

#ifdef ATLREGEXPLIB_IS_ATL
	CAtlRegExp<CAtlRECharTraitsMB> re; 
	CAtlREMatchContext<CAtlRECharTraitsMB> mc; 

	re.Parse(match);					// 正規表現セット

	if(re.Match(val, &mc)) {				// 検索
		return true;
	} else {
		return false;
	}
#else

	// 一応簡単なテストをしてみたが、
	//   ひらがな、漢字、カタカナ、ｶﾀｶﾅ、英語
	// が混じっていても普通に使えた

	return IsRegularA((char const*)val, (char const*)match);
#endif
}
