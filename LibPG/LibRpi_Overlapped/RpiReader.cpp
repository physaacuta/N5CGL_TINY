#include <stdafx.h>
#include <stddef.h>
#include "rpi.h"
#include "rpi_impl.h"
#include "RpiReader.h"

namespace RpiLib{


//------------------------------------------
//RPIヘッダの種類に応じたコピー構造体を取得
//　RpiHeader::FormatType type　ヘッダ種類
//戻り値　コピー用の構造体配列
//------------------------------------------
static const CopyInt32 * getHeaderCopier( RpiHeader::FormatType type)
{
	switch( type ){
		case RpiHeader::FORMATTYPE_Rf: return RpiHeaderCopier::Parse_Rf;
		case RpiHeader::FORMATTYPE_RF: return RpiHeaderCopier::Parse_RF;	
		case RpiHeader::FORMATTYPE_Cf: return RpiHeaderCopier::Parse_Cf;
		case RpiHeader::FORMATTYPE_CF: return RpiHeaderCopier::Parse_CF;
		default:		   return NULL;
	}
}

//------------------------------------------
//RPIファイルのリンク情報を初期化する。
// predicate_frame_pitch  フレームサイズ予測値：２フレーム目の位置 - １フレーム目の位置
//
//固定長などの場合は、一気にリンク情報（フレームがどのファイル位置にあるか）が決定できる。
//------------------------------------------
bool RpiReader::setupFramePreLength( int predicate_frame_pitch, int first_size_if_cb )
{
	LinkArray.clear();
	LinkArray.szInfo   = 0;
	LinkArray.szTFrameInf = 0;

	//EPCフレーム情報ありの場合、それは各フレームの画像後何バイト？
	if( Header.HasFrameInfo() ){
		switch(Header.frame_info_type)
		{
		case  0: LinkArray.szInfo = 0;              break;
		case  1: LinkArray.szInfo = 8;              break;
		case  2: LinkArray.szInfo = 8*Header.height;break;
		default: LinkArray.szInfo = 0;              break;
		}
	}

	LinkArray.resize( Header.nframe + 1 );

	//決められる場合、一気に各フレームの開始位置・サイズを決定する。
	switch( Header.GetFormatType() )
	{

	case  RpiHeader::FORMATTYPE_Rf:
		//生無は、固定長フォーマット。 frame_pitch * Nでフレーム開始位置が求まる
		LinkArray.szPreImg = 0;
		for( int ii=0; ii<Header.nframe; ii++){
			LinkArray[ii].nPosInFile   = Header.frame_start_pos + int64_t(Header.frame_pitch_nbyte) * ii;
			LinkArray[ii].szImg = Header.frame_nbyte;
			LinkArray[ii].szFrame = Header.frame_pitch_nbyte; 
		}
		break;

	case RpiHeader::FORMATTYPE_RF:
		LinkArray.szPreImg = 8;

		//生有は、固定長フォーマットではないが、フレーム情報サイズは変わらないだろうという推測
		for( int ii=0; ii<Header.nframe; ii++){
			LinkArray[ii].nPosInFile   = Header.frame_start_pos + int64_t(predicate_frame_pitch) * ii;
			LinkArray[ii].szImg = Header.frame_nbyte;
			LinkArray[ii].szFrame = predicate_frame_pitch; 
		}
		break;

	case RpiHeader::FORMATTYPE_Cf:
	case RpiHeader::FORMATTYPE_CF: 
		LinkArray.szPreImg = RPIFrameSizeInfo::LINK_SIZE_Cf;
		LinkArray[0].nPosInFile = Header.frame_start_pos;

		//フォーマットは圧縮形式(というかCODEC有)だが、CODECが
		//非圧縮の場合は、各フレームの開始位置・サイズを決定できる。
		if(RpiHeader::CODEC_BMP == Header.codec || RpiHeader::CODEC_RAW == Header.codec )
		{
			for( int ii=0; ii<Header.nframe; ii++){
				LinkArray[ii].nPosInFile   = Header.frame_start_pos + int64_t(predicate_frame_pitch) * ii;
				LinkArray[ii].szImg   = first_size_if_cb;
				LinkArray[ii].szFrame = predicate_frame_pitch; 
			}
		}
		
		////have Index
		//if(sizeof(T_RPI_FOOTER_RECORD) == Header.theader.szFooterRecord &&
		//	0 != Header.theader.nFooterStartPos )

		break;
	}
	return true;
}

//------------------------------------------
//RPIフッタを読み込む。
//LinkArrayが一気に全部埋まる。
//　const BYTE *footer	読み込んだフッタの先頭
//　int length			読み込んだフッタのサイズ
//戻り値　決定したフレームの個数
//------------------------------------------
int RpiReader::ReadTRPIFooter( const BYTE *footer, int length )
{
	//考慮：lengthはセクタサイズで切りあがる。
	// ほんとの個数は、番兵が出てくるまで　Or ヘッダのフレーム数。

	// LinkArrayはsetupFramePreLengthでリサイズされているはず。
	const int szFooteUnit = Header.theader.szFooterRecord; //sizeof(T_RPI_FOOTER_RECORD) ;
	int  nUnit = min( int(LinkArray.size()), length/szFooteUnit);

	for( int ii=0; ii<nUnit; ii++){
		const T_RPI_FOOTER_RECORD *rec =
			 reinterpret_cast<const T_RPI_FOOTER_RECORD*>( &footer[ii*szFooteUnit] );

		if(  -1 == rec->nFilePos )
			return ii;
		LinkArray[ii].nPosInFile = rec->nFilePos;
		LinkArray[ii].szImg = rec->szImage;
		LinkArray[ii].szFrame = rec->szFrame;
	}

	return nUnit;
}


//------------------------------------------
// フレームの開始位置を更新する。
// int64_t &nPosFromLink　リンク構造体中の、ファイル位置リファレンス
// int64_t nPosFromFile　　新たに判明した、ファイルの位置
//------------------------------------------
static bool set_with_check_pos( int64_t &nPosFromLink, int64_t nPosFromFile )
{
	// 生画像特例とかで位置がわかっているときは、比較してから更新する。
	if( 0 != nPosFromLink && nPosFromLink != nPosFromFile ) 
	{
		nPosFromLink = nPosFromFile;
		return false;
	}
	nPosFromLink = nPosFromFile;
	return true;
}


//------------------------------------------
//次フレームの先頭部分データから、
//次フレームの長さと、次次フレームの開始位置を決定する。
//	int nFrame　　　　　　　　　　　 次フレームの番号(0オリジン)
//  const unsigned char *pNextFrame　次フレームの先頭領域
//------------------------------------------
bool RpiFrameLinkArray::UpdateNextPosOnRead(int nFrame, const unsigned char *pNextFrame)
{
	bool bRet = true;
	if( nFrame+1 >= (int)this->size() ) return false;

	RPIFrameLink &ThisLink = this->at(nFrame    );
	RPIFrameLink &NextLink = this->at(nFrame + 1);

	switch( szPreImg ){
		//生画像・無フレーム
		case 0:
			//フレーム情報はないのでなにもできない
			break;

		//生画像・有フレーム形式
		case 8:
			bRet = set_with_check_pos(NextLink.nPosInFile, read_int64( pNextFrame, 0 ) );
			break;

		//圧縮形式
		case 12:
			ThisLink.szImg = read_int32( pNextFrame , 0);
			bRet = set_with_check_pos(NextLink.nPosInFile , read_int64( pNextFrame ,  4 ) );
			ThisLink.szFrame = static_cast<int>(NextLink.nPosInFile - ThisLink.nPosInFile);
			break;

		default:
			bRet = false;
			//!?
			break;
	}
	return bRet;
}

//------------------------------------------
//読み込んだRPIファイルのヘッダ部を解釈してRpiHeaderを埋める。
//　const BYTE *buf	RPIファイルの先頭部分。
//　int length		読み込んだ長さ。最低8192バイト
//戻り値 RPIとして解釈できたのなら、true
//------------------------------------------
bool RpiReader::Parse(const BYTE *buf, int length)
{
	memset( &Header, 0, sizeof(Header) );
	
	//ヘッダの形式を決める部分を読む。
	Header.type = read_int32( buf, 0);
	Header.header_size = read_int32( buf, 4);

	//形式に従って、ファイルの中身を構造体へコピー。
	RpiHeader::FormatType ft = Header.GetFormatType();
	const CopyInt32 * copier =  getHeaderCopier( ft );
	if( !copier ) {
		char cType[5]=""; memcpy(&cType, &Header.type, 4);
		m_sErrorMsg.Format("不明な型式です。ヘッダサイズ=%d, タイプ=%s", Header.header_size, cType );
		//ヘッダが既知の４種類のどれでもなかった。
		return false;
	}
	for( const CopyInt32 *pCp = copier; pCp->nFile >=0; pCp++)
	{
		pCp->StructFromFile( &Header, buf );
	}

	//生形式RPIヘッダにはCODECフィールドはないが、利便のためODEC_RAWを埋めておく。
	if( RpiHeader::FORMATTYPE_RF == ft ||
		RpiHeader::FORMATTYPE_Rf == ft )
	{
		Header.codec = RpiHeader::CODEC_RAW;
	}

	//分解能がないときのデフォルト値設定。
	if( !Header.HasResolution() )
	{
		Header.reso_x = 0;
		Header.reso_y = 0;
	}

	//Ｔヘッダが存在する場合、構造体へコピー。
	if (   0 < Header.tHeader_start_pos  
		&& Header.tHeader_start_pos + int(sizeof(T_RPI_HEADER_BASE)) < length )
	{
		int szTHeader = read_int32(  buf, Header.tHeader_start_pos );
		memcpy( &Header.theader, &buf[Header.tHeader_start_pos],   szTHeader);

        //Ｔヘッダがある場合、Ｔフレーム情報が、各フレームに何バイトあるか
		LinkArray.szTFrameInf = Header.theader.frame_info_size;
	}


	//ヘッダ＋αから、フレームの決定。
	setupFramePreLength( 
		static_cast<int>( read_int64( buf, Header.frame_start_pos + 4 ) - Header.frame_start_pos), 
		read_int32(buf, Header.frame_start_pos)
		);

	//8192も読めば、最初のフレームの頭部分もついでに読んでると考えてよい。
	//サイズと次フレーム位置をLinkArrayに登録する
	return LinkArray.UpdateNextPosOnRead( 0, &buf[ Header.frame_start_pos] );
}


};