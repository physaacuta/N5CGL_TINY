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
bool RpiReaderFile::doOpen()
{
	DWORD flag = FILE_FLAG_SEQUENTIAL_SCAN ;

	m_hFile = CreateFile( m_sFilename, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, flag, NULL);

	if( INVALID_HANDLE_VALUE == m_hFile ){
		m_hFile = 0;
		m_sErrorMsg.Format("�t�@�C�����J���Ȃ��BErr=%d", GetLastError() );
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
	MemoryDataVMAlloc ovd( nFirstReadSize );

	int nRead = ReadFileAt( 0, ovd.pBuffer, nFirstReadSize );
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

	//�t�b�^�i�S�ʒu���j������ΓǂށB
	if( Header.theader.nFooterStartPos > 0 ){

		//�ǂݍ���
		MemoryDataVMAlloc ovfooter( LinkArray.roundup( Header.theader.szFooterRecord * Header.nframe) );
		
		int nFooterSize = ReadFileAt(Header.theader.nFooterStartPos, ovfooter.pBuffer, ovfooter.szCapacity );

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
		MemoryDataVMAlloc ovd(nSize);

		//�w�b�_��ǂ񂾎��_�ő�P�t���[���̃����N���͓ǂݍ��ݍς݁B
		for( int nFrame = 1; nFrame < Header.nframe; nFrame++ ){
			int nRead = ReadFileAt( LinkArray[nFrame].nPosInFile, ovd.pBuffer, nSize );
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
//RPI�t�@�C������t���[����ǂ݁A�Ԃ��N���X�B
//------------------------------------------
class RpiFrameEnum : public IRpiFrameEnum
{
	RpiReaderFile *that;				//RPI�{�̏��

	std::auto_ptr<MemoryData> m_ovFrame;	//�ǂݍ��݃o�b�t�@
public:

	RpiFrameEnum(RpiReaderFile *athat)
		:that(athat)
	{
		IRpiFrameEnum::RpiInfo = that->LinkArray;
	}

	//------------------------------------------
	//Read�œǂނƂ��ɁAIRpiFrameEnum�������Ŋm�ۂ���̂ł͂Ȃ��A
	//�w�肵���������A�h���X�ɏ������ނ悤�ɂ���B
	//�@BYTE *pFrameDst	������
	//�@int szFrame		�T�C�Y
	// pGetImg�Ȃǂ��A����Ŏw�肵���������̒����w���悤�ɂȂ�
	//------------------------------------------
	 virtual void * SetReadDest( BYTE *pFrameDst, int szFrame ){  
		 m_ovFrame = std::auto_ptr<MemoryData>( new MemoryData( pFrameDst, szFrame  ) );
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
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d �͔͈͊O[1�`%d]", nFrame, that->LinkArray.size()-1);
		return false;
	}
	

	//���t���[���̃����N���F���B���̎��_�ł́AnPosInFile�������܂��ĂȂ������B
	const RPIFrameLink &link = that->LinkArray[nFrame-1];

    //�ǂ߂Ȃ�
	if(link.nPosInFile <= 32 ){
		that->m_sErrorMsg.Format("RpiFrameEnum::Read : nFrame=%d �͈ʒu������Ȃ��B", nFrame);
		return false;
	}

	//�t���[���w�b�_��ǂݍ���ŁA���̃t���[���̈ʒu�ƍ��̃t���[���T�C�Y���擾����B
	if( link.szFrame <= 0 ) {
		unsigned char byTmp[16]="";
		int nRead = that->ReadFileAt( link.nPosInFile, byTmp, 16);
		if( nRead >= 12 ) {
			that->LinkArray.UpdateNextPosOnRead( nFrame-1, byTmp );
		}
		else{
			that->m_sErrorMsg.Format("RpiFrameEnum::Read : nFrame=%d�͓ǂ߂Ȃ��Bpos=%lld, e=%d", nFrame, link.nPosInFile, GetLastError() );

			return false;
		}
	}

	if( link.szFrame > 512*1024*1024 || link.szFrame < 0 ){
		that->m_sErrorMsg.Format("RpiFrameEnum::Read : nFrame=%d�̃T�C�Y�����������B sz=%d", nFrame, link.szFrame);
		return false;		
	}

    //�܂��͂O�N���A�B
    szImg = 0;
    pFrame = 0;
	if( bRead ){
		//1f�i�[�p���������m��
		///szFrame��AlignOf(szFrame)�ɂ���K�v�����邩��,,,
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