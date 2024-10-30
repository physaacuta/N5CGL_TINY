// *********************************************************************************
//	AVI画像クラス
//	[Ver]
//		Ver.01    2010/02/16  vs2005 対応
//
//	[メモ]
//		・vfw.libを使用しているため、KizuLib等には入れない
//		・デザインパターンのシングルトンに対応 ←なぜかvfwがダメなのでやめた
// *********************************************************************************

#pragma once

#include <windows.h>
#include <vfw.h>

namespace ImgLib
{
	//// 本体
	class KizuAviManager
	{
	public:
		KizuAviManager(void);
		virtual ~KizuAviManager(void);


		//// スタティック
		static bool OpenAVIFile(const char* filename, PAVIFILE& avi_file, PAVISTREAM& avi_stream, AVISTREAMINFO* avi_info);	// AVIファイルのオープン
		static void CloseAVIFile(PAVIFILE& avi_file, PAVISTREAM& avi_stream);												// AVIファイルのクロース
		static bool GetAVIInfo(const char* filename, DWORD* width, DWORD* height, DWORD* bit_count, DWORD* nframes);		// AVIファイルの情報取得
		static BITMAPINFO* GetAVIFormat(PAVISTREAM avi_stream);													// AVIファイルのフォーマット情報取得


		//// メンバー
		bool OpenAVIFile(const char* filename);				// AVIファイルのオープン
		void CloseAVIFile();								// AVIファイルのクロース
		bool LoadFromAVI(long count, BYTE* img, bool nTB=false) const;	// AVIファイルの１フレーム読み込み


		//// プロパティ
		DWORD		GetWidth()	  const { return m_nW; }
		DWORD		GetHeight()   const { return m_nH; }
		DWORD		GetBitCnt()   const { return m_nBitCnt; }
		DWORD		GetFrameCnt() const { return m_nFrameCnt; }

		
		//// その他支援系
		static void HantenTB( DWORD nW, DWORD nH, BYTE *p );	// 上下を反転しながらコピーする


	private:
		PAVIFILE			m_AviFile;						// AVIファイルのハンドル
		PAVISTREAM			m_AviStream;					// AVIファイルのストリーム
		AVISTREAMINFO		m_AviInfo;						// AVIファイルのヘッダー情報

		// AIVファイルの情報
		DWORD				m_nW;							// 画像幅
		DWORD				m_nH;							// 画像高さ
		DWORD				m_nBitCnt;						// １ピクセル当たりのビット数
		DWORD				m_nFrameCnt;					// AVIファイルに含まれるフレーム数

	};


	//// 環境ハンドル (singleton)
	class KizuAviEnv
	{
	private :
		static KizuAviEnv			m_inst;					// exeで唯一のインスタンス。 初期化時にnewされて、終了時にdeleteされる.

		KizuAviEnv();										// private コンストラクタ
	public :
		virtual ~KizuAviEnv();								// デストラクタ

	};
};