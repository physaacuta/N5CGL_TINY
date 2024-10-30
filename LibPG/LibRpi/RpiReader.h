// *********************************************************************************
//	RPIファイルを読み、解釈するためのヘッダ

//	[Ver]
//		Ver.01    2014/09/15  
//
//	[メモ]
//		ベースクラスが置かれています
// *********************************************************************************
#pragma once
#include "rpi.h"

namespace RpiLib
{
//RPIファイルを解釈してヘッダと各フレーム位置を管理する基本クラス。
//具体的にファイルから読んだりする連中は、rpi_r.hに。
class RpiReader{
protected:
	CString m_sErrorMsg;
	bool setupFramePreLength( int predicate_length, int first_size_if_cb );
public:
	RpiHeader Header;
	RpiFrameLinkArray LinkArray;

	//RPIファイルから、8192バイトくらい読んだバッファを食わせること。
	//HeaderとLinkArray[0]には値が入る。LinkArray[1]も、位置だけは入る
	bool Parse(const BYTE *header, int length);

	//フッタ(集中インデックス)を与えることで、LinkArrayの位置・サイズ情報を一気に更新する
	int ReadTRPIFooter( const BYTE *footer, int length );

	//フレーム読み込み機能。
	//継承クラスで実装されている。使い終わったらDELETEすること。
	virtual class IRpiFrameEnum *CreateSequencialReader() { return NULL; }

	//ランダムアクセスするリーダ。RPIの形式によっては、SequencialReaderでSkipNextを繰り返すのと変わらない
	virtual class IRpiFrameEnum *CreateRandomReader() { return CreateSequencialReader(); }

	//何か良くないことが起こったら、エラーを返すかもしれない
	virtual	const char *GetError() const { return m_sErrorMsg; }

};

//フレーム情報格納インターフェース。
//ほしい情報は、pGet~で取得する。 szGet~で、サイズを取得。
class IRpiFrame
{
public:
     int szImg;
     BYTE *pFrame;
	 int nCurrentFrame;
	 RPIFrameSizeInfo RpiInfo;

protected:
     IRpiFrame() : szImg(0), pFrame(NULL), nCurrentFrame(0){
     }
public:
	//ReadNextでフレームを読めた場合、下記メソッドでフレーム情報にアクセスする。

	//読み込んだフレーム番号。1オリジン
	int   nFrame() const{ return nCurrentFrame; }

	//フレームリンク情報。たいていは用無し
	const BYTE  *pGetPre() const { return pFrame; }
	BYTE		*pGetPre() { return pFrame; }
	int         szGetPre() const { return RpiInfo.szPreImg; }

	//画像情報
	const BYTE  *pGetImg() const { return &pFrame[RpiInfo.szPreImg]; }
	BYTE		*pGetImg() { return &pFrame[RpiInfo.szPreImg]; }
	int         szGetImg() const { return szImg ;}

	//EPCフレーム情報
	const BYTE  *pGetEpcInf()const { return &pFrame[RpiInfo.szPreImg + szImg ]; }
	BYTE		*pGetEpcInf(){ return &pFrame[RpiInfo.szPreImg + szImg ]; }
	int         szGetEpcInf()const { return RpiInfo.szInfo; }

	//Ｔフレーム情報
	const BYTE  *pGetTFrame()const { return &pFrame[RpiInfo.szPreImg + szImg + RpiInfo.szInfo]; }
	BYTE		*pGetTFrame(){ return &pFrame[RpiInfo.szPreImg + szImg + RpiInfo.szInfo]; }
	int         szGetTFrame()const { return RpiInfo.szTFrameInf; }
  
};


//フレーム読み込みインターフェース。
//使う場合は、rpi_r.hのRpiReaderFileとかからCreateSequencialReader()でもらう。
//クラスはrpi_rf.cppとか、rpi_rm.cppとかで定義。
class IRpiFrameEnum : public IRpiFrame
 {
 protected:
     IRpiFrameEnum(){
     }

 public:
	virtual ~IRpiFrameEnum(){};
     
	//------------------------------------------
	//Readで読むときに、IRpiFrameEnumが内部で確保するのではなく、
	//指定したメモリアドレスに書き込むようにする。
	//　BYTE *pFrameDst	メモリ
	//　int szFrame		サイズ
	// pGetImgなども、これで指定したメモリの中を指すようになる
	//------------------------------------------
	virtual void *SetReadDest( BYTE *pFrameDst, int szFrame ){  return NULL; }


	//------------------------------------------
	//指定したフレームを読み込む。
	//RPIの形式によっては(≒フッタ無しのRPI)、
	//エラーになるので注意
	//------------------------------------------
	virtual bool ReadFrame( int nFrame ){
		if(	ReadToInternalBuffer(nFrame, true ) ){
			nCurrentFrame = nFrame;
			return true;
		}
		return false;

	}

	//------------------------------------------
	// 次のフレームを読みこむ。
	//------------------------------------------
	virtual bool ReadNext()
	{
		if(	ReadToInternalBuffer(nCurrentFrame + 1, true ) ){
			nCurrentFrame++;
			return true;
		}
		return false;
	}

	//------------------------------------------
	// 次のフレームは読まない。スキップする。
	// 発行後は、nFrame以外の情報を使ってはいけない(pGetImgなど)
	//------------------------------------------
	virtual bool SkipNext()	
	{
		if(	ReadToInternalBuffer(nCurrentFrame + 1, false ) ){
			nCurrentFrame++;
			return true;
		}
		return false;

	}
 protected:
	//------------------------------------------
	 //実際の読込担当メソッド。継承先で実装
	//------------------------------------------
	 virtual bool ReadToInternalBuffer( int nFrame, bool bRead = true) { 
		 return false;
	 }
};
};