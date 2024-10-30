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
bool RpiReaderFile::doOpen( bool bDirect )
{
	DWORD flag = FILE_FLAG_OVERLAPPED ;
	if( bDirect ){
		flag |= FILE_FLAG_NO_BUFFERING;
	}
	else{
		flag |= FILE_FLAG_SEQUENTIAL_SCAN ;
	}

	m_hFile = CreateFile( m_sFilename, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, flag, NULL);

	if( INVALID_HANDLE_VALUE == m_hFile ){
		m_hFile = 0;
		m_sErrorMsg.Format("ファイルが開けません。Err=%d", GetLastError() );
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
	COVDataVMAlloc ovd( nFirstReadSize );
	ovd.IssureRead( m_hFile, 0, nFirstReadSize );
	int nRead = ovd.WaitResult( m_hFile);
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

	//Direct IO 可否チェック。
	if( m_bDirect && (LinkArray[0].nPosInFile%LinkArray.nSectorAlign) != 0 ){
		//だめそう。
		Close();
		m_bDirect = false;
		doOpen( false );
	}

	//フッタ（全位置情報）があれば読む。
	if( Header.theader.nFooterStartPos > 0 ){

		//読み込み
		COVDataVMAlloc ovfooter( LinkArray.roundup( Header.theader.szFooterRecord * Header.nframe) );
		ovfooter.IssureRead( m_hFile, Header.theader.nFooterStartPos, ovfooter.szCapacity );
		int nFooterSize = ovfooter.WaitResult( m_hFile );

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
		COVDataVMAlloc ovd(nSize);

		//ヘッダを読んだ時点で第１フレームのリンク情報は読み込み済み。
		for( int nFrame = 1; nFrame < Header.nframe; nFrame++ ){
			ovd.IssureRead( m_hFile, LinkArray[nFrame].nPosInFile, nSize );
			int nRead = ovd.WaitResult( m_hFile);
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




//------------------------------------------
//RPIファイルからフレームを読み、返すクラス。
//------------------------------------------
class RpiFrameEnum : public IRpiFrameEnum
{
	RpiReaderFile *that;				//RPI本体情報

	std::auto_ptr<COVData> m_ovFrame;	//読み込みバッファ
	BYTE m_NextBlock[PAGESIZE];				//次フレーム先頭エリアの保持領域
	int m_NextBlockFrame;				//↑でいう「次フレーム」の番号
public:

	RpiFrameEnum(RpiReaderFile *athat)
		:that(athat)
	{
		IRpiFrameEnum::RpiInfo = that->LinkArray;
		m_NextBlockFrame = 0;
	}

	//------------------------------------------
	//Readで読むときに、IRpiFrameEnumが内部で確保するのではなく、
	//指定したメモリアドレスに書き込むようにする。
	//　BYTE *pFrameDst	メモリ
	//　int szFrame		サイズ
	// pGetImgなども、これで指定したメモリの中を指すようになる
	//------------------------------------------
	 virtual void * SetReadDest( BYTE *pFrameDst, int szFrame ){  
		 m_ovFrame = std::auto_ptr<COVData>( new COVData( pFrameDst, szFrame  ) );
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
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d はおかしい。 1~%dの範囲", nFrame, that->LinkArray.size());
		return false;
	}
	

	const RPIFrameLink &link = that->LinkArray[nFrame-1];

    //読む必要はない。(位置は既に分かっている)
    if( !bRead && that->LinkArray[nFrame].szFrame > 0 ){
        return true;
    }

    //読めない
	if(link.nPosInFile <= 32 ){
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d は位置が判らない。", nFrame);
		return false;
	}

	//1f格納用メモリを確保
	///szFrameをAlignOf(szFrame)にする必要があるかも,,,
	if( m_ovFrame.get() == NULL || m_ovFrame->szCapacity < link.szFrame + PAGESIZE ){
		m_ovFrame = std::auto_ptr<COVData>( new COVDataVMAlloc( COVDataVMAlloc::PageAlignOf(link.szFrame) + PAGESIZE ) );
	}

	//典型的なRPIファイル。先頭F = 512
	// 512 : Size1, Pos2
	// Pos2: Size2, Pos3

	bool bIssureReadResult;
	if( !bRead ){
		//フレームはスキップするので、次の先頭フレームのみ読む。
		bIssureReadResult=m_ovFrame->IssureRead( that->m_hFile, link.nPosInFile + link.szFrame, PAGESIZE, link.szFrame );
	}
	else{
		if( nFrame != m_NextBlockFrame ){
			//最初のフレームはちょっと余分に読む。
			bIssureReadResult=m_ovFrame->IssureRead( that->m_hFile, link.nPosInFile, link.szFrame + PAGESIZE);
		}
		else{
			//前回読んだ分をコピーする。
			memcpy( &m_ovFrame->pBuffer[0], m_NextBlock, PAGESIZE );
			//開始位置はその分ずれる。
			bIssureReadResult=m_ovFrame->IssureRead( that->m_hFile, link.nPosInFile + PAGESIZE, link.szFrame, PAGESIZE );
		}
	}

	if( !bIssureReadResult ){
		that->m_sErrorMsg.Format("IssureRead=>False, nFrame=%d, Err=%d", nFrame, GetLastError() );
	}


	int nRead =  m_ovFrame->WaitResult(that->m_hFile);
	if( nRead < PAGESIZE){
		that->m_sErrorMsg.Format("GetOverlappedReadt=>(%d), Err=%d", nRead, GetLastError() );
		return false;
	}

	{
		//次フレームの先頭を取り出し。
		memcpy( m_NextBlock, &m_ovFrame->pBuffer[ link.szFrame ], PAGESIZE );
		m_NextBlockFrame = nFrame + 1;

		//次のサイズと、次々の位置がわかる。
		that->LinkArray.UpdateNextPosOnRead( nFrame, m_NextBlock );
	}


	szImg = link.szImg;
	pFrame = m_ovFrame->pBuffer;
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