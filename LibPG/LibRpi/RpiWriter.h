// *********************************************************************************
//	拡張形式のRPIファイルを書き込むクラス。
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[メモ・使い方]
//　　　1.コンストラクト。
//　　　2.ヘッダを用意して、OpenFileで出力ファイルを作成
//　　　3.WriteFrameOpt,WriteFrameCopyなどでフレームを出力
//　　　4.ヘッダを更新し、CloseFileでファイルを閉じる
//
// *********************************************************************************
#pragma once

namespace RpiLib
{

//
class TRpiWriter
{
protected:
	CString m_sErrorMsg;				//最後のエラーを格納している。

	struct  MemoryDataVMAlloc *m_ovHeader;	//書き込み用ヘッダ（セクタアライン）。
	CString m_sFilename;


	int prepare_header();				//構造体ヘッダ->書き込み用バイトヘッダへ。

	void initLinkArray();				//書き込み用リンク情報の初期化
	void modify_last_nextpos(int64_t lastpos);
	int64_t write_index_footer();

	RPIFrameLink &PrepareFrameOpt( BYTE *frame_image, int image_size, const void *t_frameinf );

public:
	RpiHeader Header;					//ヘッダ。構造体として取り扱う用
	T_RPI_HEADER_BASE &theader;			//拡張ヘッダ。 Header.theaderの別名
	RpiFrameLinkArray LinkArray;		//リンク情報。あとで集中インデックスフッタを書くためなど

	TRpiWriter();
	virtual ~TRpiWriter();

	//ファイルオープン。
	// bDirect:FILE_FLAG_NO_BUFFERINGをつけるかどうか。
	bool OpenFile ( const CString &filename, const RpiHeader &ah, bool bDirect );
    
	bool IsOpen() const { return 0 != m_hFile; }

	const char *GetFileName() const { return m_sFilename ; } 

	HANDLE GetHandle() { return m_hFile; }
	
	//同期。フレーム書き込み。frame_imgaeは、12バイト開けておくこと。512バイトアラインされていること。
	int WriteFrameOpt( BYTE *frame_image, int image_size, const void  *t_frameinf=NULL);

	//同期。フレーム書き込み。imageがアラインされていない場合や、12バイトの先頭空白が無い場合はこちら。
	int WriteFrameCopy( const BYTE *image, int image_size, const void *t_frameinf=NULL);
    

	//現在書き込み済みのフレーム数を返す
	int GetFrameNum() const {  return static_cast<int>( LinkArray.size() ) - 1; } 
    

	//RPIファイルのヘッダを強制的に更新する
	bool FlushHeader();


	//ファイルを閉じる
	bool CloseFile();

	//なんかメソッドがエラーになった場合(falseが戻ってきた際)
	//ここにエラーの詳細が詰まっているかもしれない。
	const char *GetError() const {  return m_sErrorMsg; } 


protected:
	HANDLE m_hFile;						//書き込み用ファイルハンドル
	CRITICAL_SECTION m_csFile;			//ファイルハンドルの保護用
	int WriteFileAt( int64_t nPos, const void *pMem, int nSize );	//書き込み
	int ReadFileAt ( int64_t nPos, void *pMem, int nSize );			//読み込み。

};
};