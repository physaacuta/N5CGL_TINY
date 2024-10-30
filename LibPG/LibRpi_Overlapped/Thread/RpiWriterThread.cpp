#include <stdafx.h>
#include <stddef.h>
#include "../rpi.h"
#include "../rpiwriter.h"

#include "..\..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"							// ��]���i�`���̃X���b�h�L���[
#include "..\..\..\Library\KizuLibMFC\CLASS\ThreadSyncManager.h"							// ��]���i�`���̃X���b�h�L���[

#include "RpiWriterThread.h"
#include "RpiWriterThreadImpl.h"

namespace RpiLib{

//------------------------------------------
//�@�������݃L���[�ɗv���\���̂�ςݍ��ށB
//�@RpiWriterRequest *pCmd�@�ςݍ��ޗv���\����
//�@�߂�l�@�L���[�t���̂Ƃ��Afalse
//------------------------------------------
int RpiWriterThread::addWriteQueue_Or_Delete( RpiWriterRequest *pCmd )
{
	int nRet = mque_WriteQueue.AddToTail( pCmd  );
	if( nRet ){
		return true;
	}	
	else{
		delete pCmd;
		return false;
	}
}

//------------------------------------------
//�@���ʃL���[�Ɍ��ʍ\���̂�ςݍ��ށB
//�@RpiWriterResult *pCmd�@�ςݍ��ތ��ʍ\����
//�@�߂�l�@�L���[�t���̂Ƃ��Afalse
//------------------------------------------
int RpiWriterThread::addResultQueue_Or_Delete( RpiWriterResult *pCmd )
{
	int nRet = mque_ResultQueue.AddToTail( pCmd );
	if( nRet ){
		return true;
	}	
	else{
		delete pCmd;
		return false;
	}
}

//�p�����\�b�h�Q�B

//------------------------------------------
//�@�������݃L���[�Ƀt�@�C���I�[�v���v����ςݍ��ށB
//�@ const RpiHeader *srcHeader RPI�w�b�_
//�@ const CString &filename	�t�@�C����
//�@ int nOption				1:DIRECT
//�@�߂�l�@�L���[�t���̂Ƃ��Afalse
//------------------------------------------
int RpiWriterThread::PushOpenFile(  const RpiHeader *srcHeader, const CString &filename, int nOption)
{
	RpiWriterOpen *pCmd = new RpiWriterOpen();
	pCmd->filename = filename;
	pCmd->header = *srcHeader;
	pCmd->optvalue = nOption;
	if( addWriteQueue_Or_Delete( pCmd ) ) {
		m_bOpen = true;
		return true;
	}
	return false;

}

//------------------------------------------
//�@�������݃L���[�Ƀt�@�C���N���[�Y�v����ςݍ��ށB
//�@�߂�l�@�L���[�t���̂Ƃ��Afalse
//------------------------------------------
int RpiWriterThread::PushCloseFile( )
{
	RpiWriterRequest *pCmd = new RpiWriterRequest( RpiWriterRequest::opClose );
	if( addWriteQueue_Or_Delete( pCmd ) ){
		m_bOpen = false;
		return true;
	}
	return false;

}

//------------------------------------------
//�@�������݃L���[�Ƀt���[���������ݗv����ςݍ��ށB
//�@ const void *pImage		�摜�o�b�t�@
//�@ int szImage			�摜�T�C�Y
//�@ const void *pFrameInf�@�t���[�����
//�@ int szFrameInf			�t���[�����T�C�Y
//�@�߂�l�@�L���[�t���̂Ƃ��Afalse
//�@�t���[���������͎��O�Ŋm�ۂ��܂�
//------------------------------------------
int RpiWriterThread::PushWriteFrame( const void *pImage, int szImage, const void *pFrameInf, int szFrameInf )
{
	RpiWriterFrame *pCmd = new RpiWriterFrame();
	pCmd->pFrame = new COVDataVMAlloc( COVDataVMAlloc::PageAlignOf(szImage + szFrameInf ) );
	if( !pCmd->pFrame->pBuffer){
		delete pCmd;
		return false;
	}
	pCmd->szFrame = 12 + szImage + szFrameInf ;
	pCmd->szImg   = szImage;
	memcpy( &pCmd->pFrame->pBuffer[12]         , pImage, szImage );
	memcpy( &pCmd->pFrame->pBuffer[12+szImage] , pFrameInf, szFrameInf );
	return addWriteQueue_Or_Delete( pCmd );
}


//------------------------------------------
//�@�������݃L���[�Ƀt���[���������ݗv����ςݍ��ށB
//�@ const void *pFrameAll	�t���[���o�b�t�@
//�@ int szFrame			�t���[���̃T�C�Y
//�@ int szImage			�摜�T�C�Y
//�@�߂�l�@�L���[�t���̂Ƃ��Afalse
//�@�t���[���������͎��O�Ŋm�ۂ��܂�
//------------------------------------------
int RpiWriterThread::PushWriteFrame( const void *pFrameAll, int szFrame, int  szImage)
{
	RpiWriterFrame *pCmd = new RpiWriterFrame();
	pCmd->pFrame = new COVDataVMAlloc( COVDataVMAlloc::PageAlignOf(szFrame) );
	if( !pCmd->pFrame->pBuffer  ){
		delete pCmd;
		return false;
	}
	pCmd->szFrame = 12 + szFrame ;
	pCmd->szImg   = szImage;
	memcpy( &pCmd->pFrame->pBuffer[12]         , pFrameAll, szFrame );
	return addWriteQueue_Or_Delete( pCmd );
}

////// �V�O�i���̏���
enum {	EV_ADD_QUEUE = 0,				// �L���[�C���O
		};


int RpiWriterThread::ThreadFirst()	
{
		//// �C�x���g�ݒ�
	HANDLE hArray[] = { mque_WriteQueue.GetQueueEvent()
						};

	// IMustDie�̓I�v�V���i���Ȃ̂ŁANULL�̂Ƃ��͑҂��Ȃ��B
	SetEventNum(sizeof(hArray)/sizeof(HANDLE) , hArray);
	return 0;
}


int RpiWriterThread::ThreadLast()
{
	return 0;
}


void RpiWriterThread::ThreadEvent(int nEventNo)
{

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_QUEUE: 						// �L���[�C���O�F�����B
		{
			RpiWriterRequest *p = mque_WriteQueue.GetFromHead();
			Execute(p);	
			delete p;
		}
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//�������݂̎��{�B
void RpiWriterThread::Execute(RpiWriterRequest *p)
{

	switch( p->cmd ){
	case RpiWriterRequest::opOpen :
		OpenFile(static_cast<RpiWriterOpen*>(p) );
		break;

	case RpiWriterRequest::opClose:
		CloseFile();
		break;

	case RpiWriterRequest::opWrite:
		WriteFrame(static_cast<RpiWriterFrame*>(p));
		break;

	default:
		;//

	}
	return ;
}

//------------------------------------------
//�@�t�@�C���I�[�v���v���ɏ]���A�t�@�C����V���ɊJ���B
//------------------------------------------
bool RpiWriterThread::OpenFile(RpiWriterOpen *p)
{
	// �ŏI�����N���[�Y
	if( m_pWriter ){
		CloseFile();
	}

	//�I�[�v���n�̏���
	m_pWriter = new TRpiWriter();
	bool r = m_pWriter->OpenFile( p->filename, p->header, (p->optvalue & 1) ? 1 : 0);
	m_bStopWriting = false;
	m_filename =  p->filename;

	//������
	RpiWriterResult *pRet = new RpiWriterResult( RpiWriterResult::opOpened, m_filename, r);
//			pRet->ErrorDetail = ;
	addResultQueue_Or_Delete( pRet );
	return r;
}

//------------------------------------------
//�@�t�@�C���N���[�Y�v���ɏ]���A�t�@�C�������B
//------------------------------------------
bool RpiWriterThread::CloseFile(RpiWriterRequest *p)
{
	if( m_pWriter )
	{
	//�����O�R�[���o�b�N
		if( OnBeforeClose ){
			//���CloseFile�����ł��Z�b�g���邪�A�R�[���o�b�N���g���₷���悤�ɂ����ŃZ�b�g���Ƃ��B
			m_pWriter->Header.nframe = m_pWriter->GetFrameNum();
			OnBeforeClose( pObjCallback, m_pWriter->Header );
		}
	//��n��
		m_pWriter->CloseFile();
		delete m_pWriter;
		m_pWriter = NULL;

	//������
		RpiWriterResult*pRet = new RpiWriterResult( RpiWriterResult::opClosed, m_filename );
		addResultQueue_Or_Delete( pRet );

		m_filename= "";
		return true;
	}
	else{
		//�������Ă�
		RpiWriterResult*pRet = new RpiWriterResult( RpiWriterResult::opClosed, m_filename );
		addResultQueue_Or_Delete( pRet );
	}
	return false;
}

//------------------------------------------
//�@�t���[���������ݗv���ɏ]���A�t���[�����o�͂���B
//------------------------------------------
bool RpiWriterThread::WriteFrame(RpiWriterFrame *p)
{
	if( m_pWriter ) 
	{
		if(  m_pWriter->WriteFrameOpt( p->pFrame->pBuffer, p->szImg, NULL) ){
			RpiWriterResult *pRet = new RpiWriterResult( RpiWriterResult::opFrameWritten, m_filename, m_pWriter->GetFrameNum());
			addResultQueue_Or_Delete( pRet );
			return true;
		}
		else{
			m_bStopWriting = true;
			DWORD dwErr  = GetLastError();
			RpiWriterResult *pRet = new RpiWriterResult( RpiWriterResult::opError, m_filename, dwErr );
			pRet->ErrorDetail = "WriteFrame";
			addResultQueue_Or_Delete( pRet );
		}
	}
	return false;
}


//------------------------------------------
//�@IRpiWriterThread �C���X�^���X�̍쐬
//------------------------------------------
IRpiWriterThread * IRpiWriterThread::craete()
{
	return new RpiWriterThread();
}

};