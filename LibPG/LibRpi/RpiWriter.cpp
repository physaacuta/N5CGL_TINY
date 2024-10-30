#include <stdafx.h>
#include <stddef.h>
#include "rpi.h"
#include "rpi_impl.h"
#include "rpiwriter.h"

namespace RpiLib{

const bool B_WRITE_FOOTER_INDEX =  true; //RPIフッタを追加するか

//---------------------------	---------------
// コンストラクタ
//------------------------------------------
TRpiWriter::TRpiWriter()
	:m_hFile(0)
	,theader(Header.theader)
{
	m_ovHeader = new MemoryDataVMAlloc(4096);
	InitializeCriticalSection( &m_csFile ) ;
}

//------------------------------------------
// リンク情報を初期化
//------------------------------------------
void TRpiWriter::initLinkArray()
{
	LinkArray.szPreImg = RPIFrameSizeInfo::LINK_SIZE_Cf;
	LinkArray.szTFrameInf = theader.frame_info_size;
	LinkArray.szInfo = 0;

	if(  theader.nTHeaderSize <= 416 )
		Header.frame_start_pos = 512;
	else
		Header.frame_start_pos = 4096;
	
	///**EPC フレーム情報の書き込みはサポートしていない。**
	//if( 1 == Header.frame_info_type ){
	//	LinkArray.szInfo  = 20;
	//}
	//if( 2 == Header.frame_info_type ){
	//	LinkArray.szInfo  = 8;
	//}


	LinkArray.clear();
	LinkArray.push_back( RPIFrameLink());
	LinkArray[0].nPosInFile = Header.frame_start_pos;
	LinkArray[0].szImg = 0;
	LinkArray[0].szFrame = 0;
}


//------------------------------------------
// ファイルを書き込み用に開く
// const CString &filename	ファイル名
// const RpiHeader &ah		RPIヘッダ
// bool bDirect				FILE_FLAG_NO_BUFFERINGをつけるかどうか
// 戻り値　正常か異常か
//------------------------------------------
bool TRpiWriter::OpenFile( const CString &filename, const RpiHeader &ah, bool bDirect)
{
    //既に開いてるし..
    if( 0 != m_hFile ){
		this->m_sErrorMsg.Format("すでに開いているぽい. '%s'", filename);
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return false;
    }
    
	DWORD flag = 0;
	if( bDirect ){
		flag |= FILE_FLAG_NO_BUFFERING;
	}
	else{
		flag |= 0 ;
	}

	m_hFile = CreateFile( filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
					CREATE_ALWAYS, flag, NULL);
	if( INVALID_HANDLE_VALUE == m_hFile ){
		m_hFile = 0;
		this->m_sErrorMsg.Format("CreateFile('%s')=%d", filename, GetLastError());
		return false;
	}
	//ヘッダをコピー
	//サポートしてない形式を渡されても困るので、ここで念のため修正
	m_sFilename = filename ;
	Header = ah;
	Header.frame_info_type = 0;

	//フレームリンク情報を初期化
	initLinkArray();	

	//とりあえず、ヘッダを仮出力
	return FlushHeader();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
TRpiWriter::~TRpiWriter()
{
	if( m_hFile ){
		CloseFile();
	}
	delete m_ovHeader;
	DeleteCriticalSection( &m_csFile );
}


//------------------------------------------
// ヘッダ構造体をファイルに書き込めるよう、バイトの塊に変換する
// 入力 Header, 出力 m_byHeader
//------------------------------------------
int TRpiWriter::prepare_header() {
	//拡張形式PRIは、圧縮・フレーム無・解像度ありなので、そのように設定する
	Header.type        = TYPE_RWFL ;
	Header.header_size = SIZE_COMP_RESO;

	//Ｔヘッダの位置を決める
	if( 0 < theader.nTHeaderSize ){
		Header.tHeader_start_pos = 96;
	}

	//元の解像度がない場合は０なので、とりあえず１にする
	if( 0 == Header.reso_x ) {
		Header.reso_x = 1;
	}
	if( 0 == Header.reso_y ) {
		Header.reso_y = 1;
	}


	//バイトの塊としてのヘッダ
	memset( m_ovHeader->pBuffer, 0, 4096 );

	//拡張形式PRIは、[Cf] 圧縮・フレーム無なので、そのように作る。
	for( const CopyInt32 *p = RpiHeaderCopier::Parse_Cf; p->nFile >=0; p++)
	{
		p->FileFromStruct( m_ovHeader->pBuffer, &Header);
	}

	//Ｔヘッダをコピーする。
	if( Header.tHeader_start_pos > 0 ){
		memcpy(&m_ovHeader->pBuffer[ Header.tHeader_start_pos ], &Header.theader, Header.theader.nTHeaderSize);
		return  Header.tHeader_start_pos  + Header.theader.nTHeaderSize;
	}
	else{
		return Header.header_size;
	}
}

//------------------------------------------
//フレームメモリに、前置する位置情報と、後置するＴフレーム情報を書き込む。
//　　BYTE *frame_image		書き込むフレーム。画像が12バイト目から埋まっている
//　　int   image_size		画像サイズ。
//　　BYTE *frame_image		フッタ。無い場合や、既にframe_imageに埋まっているなら、NULL。
// メンバ変数のLinkArrayにリンク情報を追加する
//------------------------------------------
RPIFrameLink & TRpiWriter::PrepareFrameOpt( BYTE *frame_image, int image_size, const void *t_frameinf )
{
	if(!m_hFile ) 
		return LinkArray.front();
	
	//現フレーム情報のszImgを確定させ、次フレーム情報を追加する。
	RPIFrameLink &thislink = LinkArray.back();
	RPIFrameLink nextlink = thislink.CalcFrameSizeAndReturnNextPos( image_size, LinkArray );
	LinkArray.push_back( nextlink );
	Header.nframe = static_cast<int>( LinkArray.size() ) - 1;

	//メモリにフレーム情報を書き込み
	put_int32( &frame_image[0], image_size );
	put_int64( &frame_image[4], nextlink.nPosInFile );

	//メモリにＴフレーム情報をコピー。
	if( t_frameinf ) {
		memcpy( &frame_image[image_size + LinkArray.szPreImg], t_frameinf, LinkArray.szTFrameInf);
	}

	//フレームサイズの最後のほうの隙間を０でクリアする。
	int tinfo_end = image_size + LinkArray.szPreImg + LinkArray.szTFrameInf;
	memset( &frame_image[tinfo_end], 0, thislink.szFrame - tinfo_end );
	
	return thislink;
}

//------------------------------------------
//フレームをディスクに書き込む。同期。
//  BYTE *frame_image	書き込む画像。 セクタアライン制約は無し。12バイト開けろとかもなし。
//  int   image_size	画像サイズ。
//  BYTE *frame_image	フッタ。無い場合や、既にframe_imageに埋まっているなら、NULL。
// 戻り値　ディスクに書き込んだサイズ
// frame_imageへの制約が緩い代わりに、メモリ確保・コピーのオーバーヘッド有。
//------------------------------------------
int TRpiWriter::WriteFrameCopy( const BYTE *frame_image, int image_size, const void *t_frameinf )
{
    //アラインされていない+リンク情報分開いていないので、書き込み用メモリを確保する
    MemoryDataVMAlloc ovd( LinkArray.CalcFrameSize(image_size) );

    //画像(+EPCフレーム情報)コピー
    memcpy( &ovd.pBuffer[ LinkArray.szPreImg ], frame_image, image_size );

    //リンク情報追加。Ｔフレーム情報書き込み。
	RPIFrameLink &thislink = PrepareFrameOpt( ovd.pBuffer, image_size, t_frameinf );
    //書き込みの発行。

	return WriteFileAt( thislink.nPosInFile, ovd.pBuffer, thislink.szFrame );
}

//------------------------------------------
//フレームをディスクに書き込む。同期。
//　　BYTE *frame_image		書き込む画像。 セクタアライン制約。12バイト開けてから画像が書かれていること。
//　　int   image_size		画像サイズ。
//　　BYTE *frame_image		フッタ。無い場合や、既にframe_imageに埋まっているなら、NULL。
//  直接引数で渡されたメモリ領域に、リンク情報・フレーム情報を埋め込み、ディスクに書き込む。
//  メモリ確保・コピーのオーバーヘッドは無し。
// 戻り値　ディスクに書き込んだサイズ
//------------------------------------------
int TRpiWriter::WriteFrameOpt( BYTE *frame_image, int image_size, const void *t_frameinf )
{
	RPIFrameLink &thislink = PrepareFrameOpt( frame_image, image_size, t_frameinf );

	return WriteFileAt( thislink.nPosInFile, frame_image, thislink.szFrame );
}



const int szFooteUnit = sizeof(T_RPI_FOOTER_RECORD);


//------------------------------------------
//リンク情報の配列を、フッタ用に変換する。
//　　RpiFrameLinkArray &LinkArray	リンク情報。
// 戻り値　非同期書き込み用のクラス。
//------------------------------------------
static MemoryDataVMAlloc * SerializeLinkArray( const RpiFrameLinkArray &LinkArray)
{
	// LinkArrayは、中身空である最終フレームを持っているため、-1する。
	int linksize = int( LinkArray.size()  ) - 1;

	//書き込むフッタエリアの準備。最後に番兵をおくため、＋１する
	MemoryDataVMAlloc *ovdf = new MemoryDataVMAlloc( LinkArray.roundup(szFooteUnit*(linksize+1)) );

	//フッタの各フレームの情報。位置・サイズ・画像サイズを設定。
	for( int ii=0; ii<linksize; ii++){
		T_RPI_FOOTER_RECORD *rec = reinterpret_cast<T_RPI_FOOTER_RECORD*>( &ovdf->pBuffer[ii*szFooteUnit] );
		rec->nFilePos = LinkArray[ii].nPosInFile ;
		rec->szFrame  = LinkArray[ii].szFrame;
		rec->szImage  = LinkArray[ii].szImg ;
	}
	{//最終に置く番兵。
		T_RPI_FOOTER_RECORD *erec = reinterpret_cast<T_RPI_FOOTER_RECORD*>( &ovdf->pBuffer[linksize*szFooteUnit] );
		erec->nFilePos = -1;
		erec->szFrame  = linksize;
		memcpy( &erec->szImage, "*END", 4 );
	}
	return ovdf;
}

//------------------------------------------
//リンク情報をまとめて、フッタ用に書き込む。
// 戻り値　フッタを置いた後のファイルの最終位置。
//------------------------------------------
int64_t TRpiWriter::write_index_footer()
{
	//えー。
	if( 0 == Header.theader.nTHeaderSize ){
		this->m_sErrorMsg.Format("拡張ヘッダが無いのでフッタはつけられません。");
		return 0;
	}
	if( LinkArray.size() < 10 )
		return 0;

	//フッタ書き込み。
	MemoryDataVMAlloc *ovdf =	SerializeLinkArray( LinkArray );
	int64_t nPosFooter = LinkArray.back().nPosInFile;

	if( WriteFileAt( nPosFooter, ovdf->pBuffer, ovdf->szCapacity  ) ){
		//Ｔヘッダにフッタ開始位置など追記
		if( Header.theader.nTHeaderSize < sizeof(T_RPI_HEADER_BASE)){
			Header.theader.nTHeaderSize = sizeof(T_RPI_HEADER_BASE);
		}
		Header.theader.nFooterStartPos = nPosFooter;
		Header.theader.szFooterRecord  = szFooteUnit;

		//最終フレームの「次位置」を変更
		modify_last_nextpos(nPosFooter + ovdf->szCapacity);
		delete ovdf;
		return nPosFooter + ovdf->szCapacity;
	}
	delete ovdf;
	return 0;
}

//------------------------------------------
// RPIの最後にフレーム情報をおいた際、
// 最終有効フレームの指している「次フレーム位置」を
// ファイルの最後に持ってくるよう修正する
//  int64_t lastpos 最後のフレームの先頭位置
//------------------------------------------
void TRpiWriter::modify_last_nextpos(int64_t lastpos)
{
	//最終フレームの位置
	int64_t nPosLastFrame =  LinkArray[ LinkArray.size() - 2].nPosInFile;

	//最終フレームを読み込む
	int nSize = max(512, LinkArray.nSectorAlign );
	MemoryDataVMAlloc lastf( nSize );
	if( ReadFileAt( nPosLastFrame, lastf.pBuffer, nSize) > 0 )
	{
		//「次フレーム位置」を修正する
		put_int64( &lastf.pBuffer[4], lastpos );

		//書き込む。
		WriteFileAt( nPosLastFrame, lastf.pBuffer, nSize ) ;
	}
}

//------------------------------------------
//ヘッダを保存する。
//　注。基本的に、ヘッダの更新はRPIのクローズ時の自動処理で十分のはず
//　　　書き込み中のRPIファイルを触っているとか、特殊な場合のみ呼び出す
//戻り値　途中の書き込みが失敗などすれば、False。
//------------------------------------------
bool TRpiWriter::FlushHeader()
{
	//ヘッダのフレーム数は、実際に書き込んだ数に修正する。
	Header.nframe = static_cast<int>( LinkArray.size() ) - 1;
	int nHeaderSize = prepare_header();

	//ヘッダを書き込み
	return WriteFileAt( 0, m_ovHeader->pBuffer, LinkArray.roundup(nHeaderSize )) > 0 ;
}


//------------------------------------------
//ファイルを閉じる。、
// ・ヘッダは、フレーム数を実際の数に変更後、更新される
// ・フッタとして集中リンク情報が書かれる。
//戻り値　途中の書き込みが失敗などすれば、False。
//------------------------------------------
bool TRpiWriter::CloseFile()
{
	if(!m_hFile){ 
		this->m_sErrorMsg.Format("ファイルはもう閉じてる。");
		return false;
	}

	/*
	bool ret = FlushHeader();
	if( !ret ) {
		this->m_sErrorMsg.Format("ヘッダの最終更新1に失敗. ER=%d", GetLastError());
		return false;
	}
	*/

	//フッタ（集中インデックス）の書き込み
	if( B_WRITE_FOOTER_INDEX ) 	{
		write_index_footer();
	}

	bool ret = FlushHeader();
	if( !ret ) {
		this->m_sErrorMsg.Format("ヘッダの最終更新2に失敗. ER=%d", GetLastError());
		return false;
	}
	
	CloseHandle( m_hFile ) ;
	m_hFile = 0;

	//メモリ系統の後始末
	LinkArray.clear();

	return ret;
}


int TRpiWriter::WriteFileAt( int64_t nPos, const void *pMem, int nSize ){
	if( !m_hFile ) return 0;


    DWORD nWritten = 0;
	LARGE_INTEGER nFilePos; 
	nFilePos.QuadPart = nPos;
    
	EnterCriticalSection( &m_csFile );
		BOOL r = ::SetFilePointerEx(m_hFile, nFilePos, NULL, FILE_BEGIN );
		if( r ){
            r  = ::WriteFile( m_hFile, pMem, nSize, &nWritten, NULL );
        }
	LeaveCriticalSection( &m_csFile );

	if( r ) 
		return nWritten;
	else
		return 0 ;
}

int TRpiWriter::ReadFileAt ( int64_t nPos, void *pMem, int nSize ){
	if( !m_hFile ) return 0;

    DWORD nRead = 0;
	LARGE_INTEGER nFilePos; 
	nFilePos.QuadPart = nPos;

	EnterCriticalSection( &m_csFile );
		BOOL r = ::SetFilePointerEx(m_hFile, nFilePos, NULL, FILE_BEGIN );
		if( r ){
            r  = ::ReadFile( m_hFile, pMem, nSize, &nRead, NULL );
        }
	LeaveCriticalSection( &m_csFile );

	if( r ) 
		return nRead;
	else
		return  0;
}

};
