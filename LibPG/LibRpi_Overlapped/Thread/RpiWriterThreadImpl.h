// *********************************************************************************
//	RPI�̃X���b�h�ʏ������݃N���X	[�Q�l����]
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[����]
//		���ۂ̎�����A�p���������Ȃ����Ƃ��͂�����
//
// *********************************************************************************
#pragma once
#include "RpiWriterThread.h"

namespace RpiLib
{

//------------------------------------------
//�t�@�C���I�[�v���w�ߗp�\����
//------------------------------------------
struct RpiWriterOpen  : public RpiWriterRequest
{
	CString filename;	//�t�@�C����
	int optvalue;		//�J�����ȂǁAint�I�v�V����
	RpiHeader header;//RpiHeader�̏����l

	RpiWriterOpen() : RpiWriterRequest( opOpen ) 	{}
};

//------------------------------------------
//�t�@�C���N���[�Y�w�ߗp�\����
//------------------------------------------
struct RpiWriterClose : public RpiWriterRequest
{
	RpiWriterClose() : RpiWriterRequest( opClose )	{}
};

//------------------------------------------
//�t���[���������ݗp�\����
//------------------------------------------
struct RpiWriterFrame : public RpiWriterRequest
{
	//�������ݗp�̃�������ێ����Ă����B
	COVData *pFrame;
	int szFrame;
	int szImg;

	RpiWriterFrame() : RpiWriterRequest( opWrite) 
	{
		pFrame = 0;
		szFrame = 0;
		szImg = 0;
	}
	~RpiWriterFrame()
	{
		delete pFrame;
	}
};


//------------------------------------------
//RPI�t�@�C���̏������݃N���X�B
//�R�}���h�i���\�b�h�j�͑S���A�����̃L���[���o�R���āA
//�ʃX���b�h(�������݃X���b�h)�Ŏ��s�����B
//------------------------------------------
class RpiWriterThread : public ThreadManager
	                  , public IRpiWriterThread
{
protected:


public:

	RpiWriterThread(const char* pThreadName="ProcRPIWriterThread", int nMaxWaitNum = 50 ) 
		: ThreadManager( pThreadName, nMaxWaitNum )
		, m_pWriter(NULL)
	{
	}

	~RpiWriterThread()
	{
		//���Ƃ��΁A�t�@�C���r���ŏI����Ă������񂾂낤���B�B�B�H
		StopSignal();
		WaitForSingleObject(my_hThread, 10000 );
	}


	virtual int addWriteQueue_Or_Delete ( RpiWriterRequest *pCmd );
	virtual int addResultQueue_Or_Delete( RpiWriterResult  *pCmd );

	//�L���[�ɐς݂��ޘA��
	virtual int PushOpenFile(  const RpiHeader *srcHeader, const CString &filename, int nOption);
	virtual int PushCloseFile(  );
	virtual int PushWriteFrame( const void *pImage, int szImage, const void *pFrameInf, int szFrameInf );
	virtual int PushWriteFrame( const void *pFrameAll, int szFrame, int  szImage);


	virtual void StartThread(){
		Start();
	}

protected://�ȉ��́A�X���b�h���s���B

	virtual int ThreadFirst();	
	virtual int ThreadLast();
	virtual void ThreadEvent(int nEventNo);

	//�L���[�ɐς܂ꂽ�̂ŕ��򂳂���
	virtual void Execute(RpiWriterRequest *p);

	//�t�@�C����V���ɊJ���B
	virtual bool OpenFile( RpiWriterOpen *p);

	//���J���Ă���t�@�C�������B
	virtual bool CloseFile(RpiWriterRequest *p = NULL);

	//�t���[�����������ށB
	virtual bool WriteFrame(RpiWriterFrame *p);


	//���������Ȃ��݂���
	volatile LONG m_bStopWriting;

	//RPI���C�^�B
	class TRpiWriter *m_pWriter;

	//�t�@�C����
	CString m_filename;
};

};