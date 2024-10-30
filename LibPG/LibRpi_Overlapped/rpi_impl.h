// *********************************************************************************
//	[内部] RPIファイルをつくったり、読んだりするクラスに必要なヘッダ。

//	[Ver]
//		Ver.01    2014/09/15  
//
//	[メモ]
//		内部用のヘッダです。使う分には不要
// *********************************************************************************

#pragma once

namespace RpiLib
{
const int SIZE_RAW_nreso  = 44;    //非圧縮・無分解能 ヘッダサイズ
const int SIZE_RAW_RESO   = 52;    //非圧縮・有分解能 ヘッダサイズ
const int SIZE_COMP_nreso = 40;    //　圧縮・無分解能 ヘッダサイズ
const int SIZE_COMP_RESO  = 48;    //　圧縮・有分解能 ヘッダサイズ
    
enum RpiHeaderType{
    TYPE_RWFL = RPIFOURCC('R', 'W', 'F', 'L'),    //フレーム情報無し
    TYPE_RWFT = RPIFOURCC('R', 'W', 'F', 'T'),    //フレーム情報あり
};


//メモリ<=>整数のアクセス用。reinterpret_castを使うよりは書きやすいはず。
//Intel系はバスエラーがないからいいといえばいいけど

//------------------------------------------
//メモリから64bit値を読み込む
// const void *buf メモリ
// int nPosInFile  メモリ上の位置
//戻り値　bufから 読んだ64bit値
//------------------------------------------
static int64_t read_int64( const void *buf, int nPosInFile ){
	int64_t m;
	memcpy( &m, &static_cast<const char*>( buf )[nPosInFile], 8);
	return m;
}

//------------------------------------------
//メモリから32bit値を読み込む
// const void *buf メモリ
// int nPosInFile  メモリ上の位置
//戻り値　bufから 読んだ32bit値
//------------------------------------------
static int read_int32( const void *buf, int nPosInFile ){
	int m;
	memcpy( &m, &static_cast<const char*>( buf )[nPosInFile], 4);
	return m;
}

//------------------------------------------
//メモリへ64bit値を書き込む
// void *buf	メモリ
// int nValue	書き込む値
//------------------------------------------
static void put_int64(  void *buf, int64_t nValue){
	memcpy( buf, &nValue, 8);
}

//------------------------------------------
//メモリへ32bit値を書き込む
// void *buf	 メモリ
// int nValue	書き込む値
//------------------------------------------
static void put_int32(  void *buf, int nValue ){
	memcpy( buf, &nValue, 4);
}


//32bit単位で、構造体とファイル(から読み込んだメモリ)の転送をする。
struct CopyInt32 {
	int nStructure; //構造体側メモリオフセット
	int nFile;		//ファイル側オフセット
	
	//------------------------------------------
	//ファイル側のメモリから、構造体側のメモリへデータをコピーする
	//         void *st	構造体側メモリ
	// const void *file	ファイル側メモリ
	//------------------------------------------
	void StructFromFile( void *st, const void *file )const{
		if( nStructure < 0 || nFile < 0 )
			return;

		static_cast<int*>(st)[ nStructure/4 ] = static_cast<const int*>(file)[ nFile/4 ]; 
	}

	//------------------------------------------
	//構造体側のメモリから、ファイル側のメモリへデータをコピーする
	//       void *file	ファイル側メモリ
	//   const void *st	構造体側メモリ
	//------------------------------------------
	void FileFromStruct(void *file, const  void *st )const{
		if( nStructure < 0 || nFile < 0 )
			return;
		static_cast< int*>(file)[ nFile/4 ] = 		static_cast<const int*>(st)[ nStructure/4 ] ;

	}
};

struct RpiHeaderCopier
{
	static const CopyInt32 Parse_Rf[];	//生・無フレーム用コピー構造体
	static const CopyInt32 Parse_Cf[];	//圧縮・無フレーム用コピー構造体
	static const CopyInt32 Parse_RF[];  //生・有フレーム用コピー構造体
	static const CopyInt32 Parse_CF[];	//圧縮・有フレーム用コピー構造体
};
};