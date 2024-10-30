//=====================================================================
// CSVに Iniファイルのようなセクション単位での設定ファイルの読込既定クラス
//	【全ライン】
//		・基本的に完全流用可能なように作る
//=====================================================================

#pragma once

// STL
#include <vector>														// リスト用

namespace KizuLib
{
	class CsvIniFunc
	{
	private :
		static const int	SEC_HEAD_S = '[';								// セクション区切り開始キー
		static const int	SEC_HEAD_E = ']';								// セクション区切り開始キー

		
	//// 公開定数
	public:
		CsvIniFunc(void);
		virtual ~CsvIniFunc(void);


		//=========================================
		// プロパティ
		void	SetSecHeader(int rec, char const* cKey) { strcpy( m_cSecHead[rec], cKey); }


		//=========================================
		// 外部操作関数
		bool	ReadCsv(LPCSTR sSec, LPCSTR sPath, std::vector<CString>* pRow);		// CSVファイル読込
		int		CntColumn(const CString& sVal);										// 要素が何個あるか

		bool	GetColumn(const CString& sVal, int& curPos, CString& sToken);			// １行のデータから , 単位の文字列を取得
		CString GetColumnStr(const CString& sVal, int& curPos);
		void	GetColumnChar(const CString& sVal, int& curPos, char* cAns, int nSize);
		int		GetColumnInt(const CString& sVal, int& curPos);
		double	GetColumnDouble(const CString& sVal, int& curPos);


	//// メンバー関数
	private:
		// セクション区切り
		char						m_cSecHead[2][16];					// セクション開始区切り
	};
};