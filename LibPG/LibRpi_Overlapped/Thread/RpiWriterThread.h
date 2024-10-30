// *********************************************************************************
//	RPI�̃X���b�h�ʏ������݃N���X�ւ̃C���^�[�t�F�[�X	[�Q�l����]
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[����]
//		�g�������Ȃ�A���̃w�b�_��������ɂ��Ă�OK
//
// *********************************************************************************
#pragma once


namespace RpiLib
{
//�L���[�ɂ��܂�A�w�ߗp�\���́i�x�[�X�\���́j
struct RpiWriterRequest{
	enum op_cmd
	{
		opOpen =1,
		opClose=2,
		opWrite=3,
	};
	int cmd;

	RpiWriterRequest(int c)  : cmd(c) {}
	virtual ~RpiWriterRequest()       {}
};


//���ʍ\����
struct RpiWriterResult{
	enum op_cmd
	{
		opOpened =1,
		opClosed=2,
		opFrameWritten=3,
		opError=4,
	};

	op_cmd cmd;
	CString filename;

	int errorcode;
	CString ErrorDetail;
	
	RpiWriterResult(op_cmd c, const CString file, int nError = 0)
		: cmd(c)
		, filename(file)
		, errorcode(nError )
	{

	}
};


struct IRpiWriterThread
{
	//���͗p����L���[�B��{�I�ɂ́A���Lpush���\�b�h�A�����g������
	ThreadQueue<RpiWriterRequest> mque_WriteQueue ;

	//���ʃL���[�B�K���������邱��
	ThreadQueue<RpiWriterResult> mque_ResultQueue;



	//�w�߂��L���[�ɐςށB
	//�t�@�C���I�[�v��
	virtual int PushOpenFile(  const RpiHeader *srcHeader,  const CString &filename, int nOption) = 0;
	//�t�@�C���N���[�Y
	virtual int PushCloseFile(  )= 0;
	//�t���[���̏������݁B�摜�ƁA�t���[������񂪕ʁX�̃G���A�ɂ���ꍇ
	virtual int PushWriteFrame( const void *pImage, int szImage, const void *pFrameInf, int szFrameInf )= 0;
	//�t���[���̏������݁B�摜�{�t���[�������Ō����ς�
	virtual int PushWriteFrame( const void *pFrameAll, int szFrame, int  szImage)= 0;

	//�����݁A�t�@�C�����J���Ă��邩
	virtual bool IsOpen() const { return m_bOpen; }


	//close���钼�O�̃R�[���o�b�N�B�I���ԍۂɊm�肷��w�b�_���ڂ�ݒ肵�����Ƃ��ɁB
	//�ق�Ƃ́AEVENT OnBeforeClose�Ƃ��������B
	//���B OnAfterClose�͖����̂ŁAResultQueue����opClsoed���E���ď������ĂˁB
	void *pObjCallback;
	int (*OnBeforeClose)( void *pObjCallback,  RpiHeader &Header);

	//�f�X�g���N�^
	virtual ~IRpiWriterThread(){};

	//�������݃X���b�h�̊J�n
	virtual void StartThread() = 0;

	//�C���^�[�t�F�[�X�̎��̂��쐬����
	static IRpiWriterThread * craete();

protected:
	bool m_bOpen;
	IRpiWriterThread() : pObjCallback(NULL), OnBeforeClose(NULL), m_bOpen(false){	}
};
};