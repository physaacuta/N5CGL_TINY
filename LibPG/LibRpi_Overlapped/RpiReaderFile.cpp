// RPI �Ǎ��N���X�̎���
#include "stdafx.h"
#include "rpi.h"
#include "rpi_impl.h"
#include "RpiReaderFile.h"

namespace RpiLib{

const int PAGESIZE = 4096;

//------------------------------------------
//�t�@�C����{���ɊJ���B
// bDirect �Ftrue���AFILE_FLAG_NO_BUFFERING������CreateFile
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
		m_sErrorMsg.Format("�t�@�C�����J���܂���BErr=%d", GetLastError() );
		return false;
	}
	return true;
}


//------------------------------------------
//�w�b�_��ǂݍ��ށB
//------------------------------------------
bool RpiReaderFile::ParseRead()
{
	const int nFirstReadSize = 8192;
	COVDataVMAlloc ovd( nFirstReadSize );
	ovd.IssureRead( m_hFile, 0, nFirstReadSize );
	int nRead = ovd.WaitResult( m_hFile);
	if( nRead != nFirstReadSize ){
		m_sErrorMsg.Format("RPI�t�@�C���Ȃ�A������%d�o�C�g���炢�͗~����. nLen = %d", nFirstReadSize, nRead);
		Close();
		return false;
	}

	//RPI�w�b�_�ǂݍ��݁B
	bool bRet = Parse( ovd.pBuffer, nFirstReadSize );
	if( !bRet ){
		m_sErrorMsg.Format("RPI�̃w�b�_����������. type=%x, len=%d", Header.type, Header.header_size );
		Close();
		return false;
	}

	//Direct IO �ۃ`�F�b�N�B
	if( m_bDirect && (LinkArray[0].nPosInFile%LinkArray.nSectorAlign) != 0 ){
		//���߂����B
		Close();
		m_bDirect = false;
		doOpen( false );
	}

	//�t�b�^�i�S�ʒu���j������ΓǂށB
	if( Header.theader.nFooterStartPos > 0 ){

		//�ǂݍ���
		COVDataVMAlloc ovfooter( LinkArray.roundup( Header.theader.szFooterRecord * Header.nframe) );
		ovfooter.IssureRead( m_hFile, Header.theader.nFooterStartPos, ovfooter.szCapacity );
		int nFooterSize = ovfooter.WaitResult( m_hFile );

		//�����N��񂪑S�t���[���������B
		ReadTRPIFooter( ovfooter.pBuffer, nFooterSize );
		bIndexAccess = true;
	}
	else{
		//�t�b�^�������Ă��A���摜RPI�Ȃ烊���N���͂�����Ă�B
		bIndexAccess = LinkArray[ LinkArray.size() - 1 ].nPosInFile != 0;
	}
	return true;
}

//------------------------------------------
//�����N�����A�t�@�C������J�邱�Ƃō쐬����
// bool bForce �t�b�^�����Ă������ȂǁA���邯�ǁA��J��Ȃ��������Ƃ���True�B
//------------------------------------------
bool RpiReaderFile::ReadIndexPos(bool bForce )
{
	if( !bIndexAccess || bForce){
		int nSize = max(512, LinkArray.nSectorAlign );
		COVDataVMAlloc ovd(nSize);

		//�w�b�_��ǂ񂾎��_�ő�P�t���[���̃����N���͓ǂݍ��ݍς݁B
		for( int nFrame = 1; nFrame < Header.nframe; nFrame++ ){
			ovd.IssureRead( m_hFile, LinkArray[nFrame].nPosInFile, nSize );
			int nRead = ovd.WaitResult( m_hFile);
			if( nRead != nSize ){
				m_sErrorMsg.Format("RPI���������Ă��������A�r���Ő؂ꂽ�Bframe=%d, %lld", nFrame,  LinkArray[nFrame].nPosInFile);
				return false;
			}
			LinkArray.UpdateNextPosOnRead( nFrame, ovd.pBuffer );
		}
		bIndexAccess  = true;
	}
	return true;
}


//------------------------------------------
//�t�@�C�������
//------------------------------------------
void RpiReaderFile::Close()
{
	if( m_hFile ){
		CloseHandle( m_hFile );
		m_hFile = 0;
	}
}




//------------------------------------------
//RPI�t�@�C������t���[����ǂ݁A�Ԃ��N���X�B
//------------------------------------------
class RpiFrameEnum : public IRpiFrameEnum
{
	RpiReaderFile *that;				//RPI�{�̏��

	std::auto_ptr<COVData> m_ovFrame;	//�ǂݍ��݃o�b�t�@
	BYTE m_NextBlock[PAGESIZE];				//���t���[���擪�G���A�̕ێ��̈�
	int m_NextBlockFrame;				//���ł����u���t���[���v�̔ԍ�
public:

	RpiFrameEnum(RpiReaderFile *athat)
		:that(athat)
	{
		IRpiFrameEnum::RpiInfo = that->LinkArray;
		m_NextBlockFrame = 0;
	}

	//------------------------------------------
	//Read�œǂނƂ��ɁAIRpiFrameEnum�������Ŋm�ۂ���̂ł͂Ȃ��A
	//�w�肵���������A�h���X�ɏ������ނ悤�ɂ���B
	//�@BYTE *pFrameDst	������
	//�@int szFrame		�T�C�Y
	// pGetImg�Ȃǂ��A����Ŏw�肵���������̒����w���悤�ɂȂ�
	//------------------------------------------
	 virtual void * SetReadDest( BYTE *pFrameDst, int szFrame ){  
		 m_ovFrame = std::auto_ptr<COVData>( new COVData( pFrameDst, szFrame  ) );
		 return m_ovFrame.get(); 
	 }

protected:
	bool ReadToInternalBuffer( int nFrame, bool bRead );

};

//------------------------------------------
// �J���Ă���Rpi�t�@�C���́AnFrame�ڂ�ǂ݂����A�������Ɋi�[����B
// int nFrame	�Ώۃt���[��. �P�I���W��
// bool bRead	True�F�������ɖ{�̂�ǂށBFalse:�����N�����ǂ�Ŏ��ւ����ށB
//------------------------------------------
bool RpiFrameEnum::ReadToInternalBuffer( int nFrame, bool bRead )
{
	if( 1 <= nFrame && nFrame < (int)that->LinkArray.size() );
	else	{
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d �͂��������B 1~%d�͈̔�", nFrame, that->LinkArray.size());
		return false;
	}
	

	const RPIFrameLink &link = that->LinkArray[nFrame-1];

    //�ǂޕK�v�͂Ȃ��B(�ʒu�͊��ɕ������Ă���)
    if( !bRead && that->LinkArray[nFrame].szFrame > 0 ){
        return true;
    }

    //�ǂ߂Ȃ�
	if(link.nPosInFile <= 32 ){
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d �͈ʒu������Ȃ��B", nFrame);
		return false;
	}

	//1f�i�[�p���������m��
	///szFrame��AlignOf(szFrame)�ɂ���K�v�����邩��,,,
	if( m_ovFrame.get() == NULL || m_ovFrame->szCapacity < link.szFrame + PAGESIZE ){
		m_ovFrame = std::auto_ptr<COVData>( new COVDataVMAlloc( COVDataVMAlloc::PageAlignOf(link.szFrame) + PAGESIZE ) );
	}

	//�T�^�I��RPI�t�@�C���B�擪F = 512
	// 512 : Size1, Pos2
	// Pos2: Size2, Pos3

	bool bIssureReadResult;
	if( !bRead ){
		//�t���[���̓X�L�b�v����̂ŁA���̐擪�t���[���̂ݓǂށB
		bIssureReadResult=m_ovFrame->IssureRead( that->m_hFile, link.nPosInFile + link.szFrame, PAGESIZE, link.szFrame );
	}
	else{
		if( nFrame != m_NextBlockFrame ){
			//�ŏ��̃t���[���͂�����Ɨ]���ɓǂށB
			bIssureReadResult=m_ovFrame->IssureRead( that->m_hFile, link.nPosInFile, link.szFrame + PAGESIZE);
		}
		else{
			//�O��ǂ񂾕����R�s�[����B
			memcpy( &m_ovFrame->pBuffer[0], m_NextBlock, PAGESIZE );
			//�J�n�ʒu�͂��̕������B
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
		//���t���[���̐擪�����o���B
		memcpy( m_NextBlock, &m_ovFrame->pBuffer[ link.szFrame ], PAGESIZE );
		m_NextBlockFrame = nFrame + 1;

		//���̃T�C�Y�ƁA���X�̈ʒu���킩��B
		that->LinkArray.UpdateNextPosOnRead( nFrame, m_NextBlock );
	}


	szImg = link.szImg;
	pFrame = m_ovFrame->pBuffer;
	return true;
}


//------------------------------------------
// �A���p�̃t���[���ǂݍ��݃N���X������ĕԂ�
//------------------------------------------
class IRpiFrameEnum *RpiReaderFile::CreateSequencialReader()
{
	return new RpiFrameEnum (this);
}

//------------------------------------------
// �����_���A�N�Z�X�p�̃t���[���ǂݍ��݃N���X��Ԃ�
//------------------------------------------
class IRpiFrameEnum *RpiReaderFile::CreateRandomReader()
{
	//�t���[�������m�����邾���B
	ReadIndexPos();
	return new RpiFrameEnum (this);
}

};