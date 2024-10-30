// RPI 読込クラスの実装
#include "stdafx.h"
#include "rpi.h"
#include "rpi_impl.h"
#include "RpiReaderFile.h"

namespace RpiLib{

const int PAGESIZE = 4096;

//------------------------------------------
//ファイルを本当に開く。
// bDirect ：true時、FILE_FLAG_NO_BUFFERINGをつけてCreateFile
//------------------------------------------
bool RpiReaderFile::doOpen()
{
	DWORD flag = FILE_FLAG_SEQUENTIAL_SCAN ;

	m_hFile = CreateFile( m_sFilename, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, flag, NULL);

	if( INVALID_HANDLE_VALUE == m_hFile ){
		m_hFile = 0;
		m_sErrorMsg.Format("ファイルが開けない。Err=%d", GetLastError() );
		return false;
	}
	return true;
}


//------------------------------------------
//ヘッダを読み込む。
//------------------------------------------
bool RpiReaderFile::ParseRead()
{
	const int nFirstReadSize = 8192;
	MemoryDataVMAlloc ovd( nFirstReadSize );

	int nRead = ReadFileAt( 0, ovd.pBuffer, nFirstReadSize );
	if( nRead != nFirstReadSize ){
		m_sErrorMsg.Format("RPIファイルなら、長さが%dバイトくらいは欲しい. nLen = %d", nFirstReadSize, nRead);
		Close();
		return false;
	}

	//RPIヘッダ読み込み。
	bool bRet = Parse( ovd.pBuffer, nFirstReadSize );
	if( !bRet ){
		m_sErrorMsg.Format("RPIのヘッダがおかしい. type=%x, len=%d", Header.type, Header.header_size );
		Close();
		return false;
	}

	//フッタ（全位置情報）があれば読む。
	if( Header.theader.nFooterStartPos > 0 ){

		//読み込み
		MemoryDataVMAlloc ovfooter( LinkArray.roundup( Header.theader.szFooterRecord * Header.nframe) );
		
		int nFooterSize = ReadFileAt(Header.theader.nFooterStartPos, ovfooter.pBuffer, ovfooter.szCapacity );

		//リンク情報が全フレーム分揃う。
		ReadTRPIFooter( ovfooter.pBuffer, nFooterSize );
		bIndexAccess = true;
	}
	else{
		//フッタが無くても、生画像RPIならリンク情報はそろってる。
		bIndexAccess = LinkArray[ LinkArray.size() - 1 ].nPosInFile != 0;
	}
	return true;
}

//------------------------------------------
//リンク情報を、ファイルを手繰ることで作成する
// bool bForce フッタが壊れていた時など、あるけど、手繰りなおしたいときにTrue。
//------------------------------------------
bool RpiReaderFile::ReadIndexPos(bool bForce )
{
	if( !bIndexAccess || bForce){
		int nSize = max(512, LinkArray.nSectorAlign );
		MemoryDataVMAlloc ovd(nSize);

		//ヘッダを読んだ時点で第１フレームのリンク情報は読み込み済み。
		for( int nFrame = 1; nFrame < Header.nframe; nFrame++ ){
			int nRead = ReadFileAt( LinkArray[nFrame].nPosInFile, ovd.pBuffer, nSize );
			if( nRead != nSize ){
				m_sErrorMsg.Format("RPIをたぐっていったが、途中で切れた。frame=%d, %lld", nFrame,  LinkArray[nFrame].nPosInFile);
				return false;
			}
			LinkArray.UpdateNextPosOnRead( nFrame, ovd.pBuffer );
		}
		bIndexAccess  = true;
	}
	return true;
}


//------------------------------------------
//ファイルを閉じる
//------------------------------------------
void RpiReaderFile::Close()
{
	if( m_hFile ){
		CloseHandle( m_hFile );
		m_hFile = 0;
	}
}

int RpiReaderFile::ReadFileAt ( int64_t nPos, void *pMem, int nSize ){
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

//------------------------------------------
//RPIファイルからフレームを読み、返すクラス。
//------------------------------------------
class RpiFrameEnum : public IRpiFrameEnum
{
	RpiReaderFile *that;				//RPI本体情報

	std::auto_ptr<MemoryData> m_ovFrame;	//読み込みバッファ
public:

	RpiFrameEnum(RpiReaderFile *athat)
		:that(athat)
	{
		IRpiFrameEnum::RpiInfo = that->LinkArray;
	}

	//------------------------------------------
	//Readで読むときに、IRpiFrameEnumが内部で確保するのではなく、
	//指定したメモリアドレスに書き込むようにする。
	//　BYTE *pFrameDst	メモリ
	//　int szFrame		サイズ
	// pGetImgなども、これで指定したメモリの中を指すようになる
	//------------------------------------------
	 virtual void * SetReadDest( BYTE *pFrameDst, int szFrame ){  
		 m_ovFrame = std::auto_ptr<MemoryData>( new MemoryData( pFrameDst, szFrame  ) );
		 return m_ovFrame.get(); 
	 }

protected:
	bool ReadToInternalBuffer( int nFrame, bool bRead );

};

//------------------------------------------
// 開いているRpiファイルの、nFrame目を読みだし、メモリに格納する。
// int nFrame	対象フレーム. １オリジン
// bool bRead	True：メモリに本体を読む。False:リンクだけ読んで次へすすむ。
//------------------------------------------
bool RpiFrameEnum::ReadToInternalBuffer( int nFrame, bool bRead )
{
	if( 1 <= nFrame && nFrame < (int)that->LinkArray.size() );
	else	{
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d は範囲外[1～%d]", nFrame, that->LinkArray.size()-1);
		return false;
	}
	

	//今フレームのリンク情報：注。この時点では、nPosInFileしか埋まってないかも。
	const RPIFrameLink &link = that->LinkArray[nFrame-1];

    //読めない
	if(link.nPosInFile <= 32 ){
		that->m_sErrorMsg.Format("RpiFrameEnum::Read : nFrame=%d は位置が判らない。", nFrame);
		return false;
	}

	//フレームヘッダを読み込んで、次のフレームの位置と今のフレームサイズを取得する。
	if( link.szFrame <= 0 ) {
		unsigned char byTmp[16]="";
		int nRead = that->ReadFileAt( link.nPosInFile, byTmp, 16);
		if( nRead >= 12 ) {
			that->LinkArray.UpdateNextPosOnRead( nFrame-1, byTmp );
		}
		else{
			that->m_sErrorMsg.Format("RpiFrameEnum::Read : nFrame=%dは読めない。pos=%lld, e=%d", nFrame, link.nPosInFile, GetLastError() );

			return false;
		}
	}

	if( link.szFrame > 512*1024*1024 || link.szFrame < 0 ){
		that->m_sErrorMsg.Format("RpiFrameEnum::Read : nFrame=%dのサイズがおかしい。 sz=%d", nFrame, link.szFrame);
		return false;		
	}

    //まずは０クリア。
    szImg = 0;
    pFrame = 0;
	if( bRead ){
		//1f格納用メモリを確保
		///szFrameをAlignOf(szFrame)にする必要があるかも,,,
		if( m_ovFrame.get() == NULL || m_ovFrame->szCapacity < link.szFrame ){
			m_ovFrame = std::auto_ptr<MemoryData>( new MemoryDataVMAlloc( MemoryDataVMAlloc::PageAlignOf(link.szFrame)  ) );
		}
		int nRead = that->ReadFileAt( link.nPosInFile, &m_ovFrame->pBuffer[0], link.szFrame );

		if( nRead != link.szFrame){
			that->m_sErrorMsg.Format("RpiFrameEnum::Read : ReadFile(%d)=>(%d), Err=%d", link.szFrame, nRead, GetLastError() );
			return false;
		}
        szImg = link.szImg;
        pFrame = m_ovFrame->pBuffer;
	}

	return true;
}





//------------------------------------------
// 連続用のフレーム読み込みクラスを作って返す
//------------------------------------------
class IRpiFrameEnum *RpiReaderFile::CreateSequencialReader()
{
	return new RpiFrameEnum (this);
}

//------------------------------------------
// ランダムアクセス用のフレーム読み込みクラスを返す
//------------------------------------------
class IRpiFrameEnum *RpiReaderFile::CreateRandomReader()
{
	//フレーム情報を確立するだけ。
	ReadIndexPos();
	return new RpiFrameEnum (this);
}

};