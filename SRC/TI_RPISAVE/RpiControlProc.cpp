#include "StdAfx.h"
#include "RpiControlProc.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
RpiControlProc::RpiControlProc(void) :
ThreadManager("RpiCP")
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
RpiControlProc::~RpiControlProc(void)
{
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int RpiControlProc::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { this->mque_pIn->GetEvQue() 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int RpiControlProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void RpiControlProc::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_ADD_IMG = 0,					// �t���[���摜�L���[�C���O
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// �t���[���捞
		AddNewItem();
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �摜�擾
//------------------------------------------
void RpiControlProc::AddNewItem()
{
	//// �t���[���摜�L���[���擾
	FrameDataManager* p = mque_pIn->GetFromHead();
	if( NULL == p ) {															// �ʏ킠�肦�Ȃ��͂�
		LOG(em_ERR), "[%s] �X���b�h���[�v�L���[����!", my_sThreadName);
		return;
	}

	//// �����U�蕪��
	int nIns = p->nIns;
	int nColor = p->nColor;
	if( 0 > nIns || nIns >= mcls_pParam->GetMain().nIns ) {						// �ʏ킠�肦�Ȃ��͂�
		LOG(em_ERR), "[%s] �C���X�^���X�ԍ��ُ�=%d", my_sThreadName, nIns);
		return;
	}
	if( 0 > nColor || nColor >= MAX_COLOR_NUM  ) {		// �ʏ킠�肦�Ȃ��͂�
		LOG(em_ERR), "[%s] �摜�F�ԍ��ُ�=%d", my_sThreadName, nColor);
		return;
	}

	//// �w���RPI�������݃N���X��
	DELIVERY_RPIWRITE* pDeli = new DELIVERY_RPIWRITE;
	pDeli->emNo = ENUM_KUBUN_RPIWRITE::em_FRAME_INFO;
	pDeli->data = (BYTE*) p;
	mque_pOut[nIns][nColor]->AddToTail(pDeli);
}

//------------------------------------------
// RPI�t�@�C���N���[�Y�v��
//------------------------------------------
void RpiControlProc::ClosingRpi()
{
	//// �����ݒ��� RPI�t�@�C�����N���[�Y����
	for(int ii=0; ii<MAX_INS; ii++) {
		ClosingRpi(ii);
	}
}

//------------------------------------------
// RPI�t�@�C���N���[�Y�v��
// int nIns �w��Ins�̂� (0�I���W��)
//------------------------------------------
void RpiControlProc::ClosingRpi(int nIns)
{
	for(int jj=0; jj<MAX_COLOR_NUM; jj++) {
		if( NULL == mque_pOut[nIns][jj]) continue;

		//// �w���RPI�������݃N���X��
		DELIVERY_RPIWRITE* pDeli = new DELIVERY_RPIWRITE;
		pDeli->emNo = ENUM_KUBUN_RPIWRITE::em_WRITE_END;
		pDeli->data = NULL;
		mque_pOut[nIns][jj]->AddToTail(pDeli);
	}
}
//------------------------------------------
// �����J�n�� ����
//------------------------------------------
bool RpiControlProc::Init()
{
	for(int ii=0; ii<MAX_INS; ii++) {
		for(int jj=0; jj<MAX_COLOR_NUM; jj++) {
			if( NULL == mcls_pRpiWProc[ii][jj]) continue;

			mcls_pRpiWProc[ii][jj]->SetData_Setting();
		}
	}
	return true;
}
