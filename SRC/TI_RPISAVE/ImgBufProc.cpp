#include "StdAfx.h"
#include "ImgBufProc.h"


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#define LOG2( em, ... ) do { \
	if(mcls_detail){   mcls_detail->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	if(!mcls_detail || \
		KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_ERR ||\
	    KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_WAR) {\
		if(mcls_pLog){       mcls_pLog->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	}\
}while(0)

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
ImgBufProc::ImgBufProc(void) :
ThreadManager("ImgBufP"),
m_bIsExec(false),
mcls_detail(NULL)
{
	Reset();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ImgBufProc::~ImgBufProc(void)
{
	Free();
}

//------------------------------------------
// �̈�m��
//------------------------------------------
void ImgBufProc::Alloc()
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
}

//------------------------------------------
// �̈�J��
//------------------------------------------
void ImgBufProc::Free()
{
}

//------------------------------------------
// ���Z�b�g
//------------------------------------------
void ImgBufProc::Reset()
{
}

//------------------------------------------
// �������s�L��
//------------------------------------------
void ImgBufProc::SetIsExec(bool bMode)
{
	m_bIsExec = bMode;
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ImgBufProc::ThreadFirst()
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
int ImgBufProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ImgBufProc::ThreadEvent(int nEventNo)
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
		LOG2(em_ERR, "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �摜�擾
//------------------------------------------
void ImgBufProc::AddNewItem()
{
	//// �t���[���摜�L���[���擾
	FRAME_DATA_ARRAY* p = mque_pIn->GetFromHead();
	if( NULL == p ) {
		LOG2(em_ERR, "[%s] �X���b�h���[�v�L���[����!", my_sThreadName);
		return;
	}
	LOG2(em_INF, "[%s] IN (Fno=%d/%d)", my_sThreadName, p->nFno, p->nCFno, p->emImgSaveStat );

	//// ����
	if(m_bIsExec) {
		for(int ii=0; ii<MAX_COLOR_NUM; ii++ ) {
			if(NULL == p->Fdc.pFrameDataMgr[ii]) continue;
			((FrameDataManager*)p->Fdc.pFrameDataMgr[ii])->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgBuf_START);
		}

		Exec(p);

		for(int ii=0; ii<MAX_COLOR_NUM; ii++ ) {
			if(NULL == p->Fdc.pFrameDataMgr[ii]) continue;
			((FrameDataManager*)p->Fdc.pFrameDataMgr[ii])->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgBuf_END);
		}
	} else {
		LOG(em_WAR), "[%s] �L�����Z��", my_sThreadName);
	}


	//// �΂炵�Ď���
	for(int ii=0; ii<MAX_COLOR_NUM; ii++) {
		if( NULL == p->Fdc.pFrameDataMgr[ii] ) continue;

		FrameDataManager* pF = (FrameDataManager*)p->Fdc.pFrameDataMgr[ii];

		if( ! pF->bRpiSave || ! m_bIsExec ) {
//��
	LOG2(em_INF, "[%s] [%d] ���ʕ\���ցB�X�L�b�v (Fno=%d/%d) st=%d (%s/%s)", my_sThreadName, ii, p->nFno, p->nCFno, p->emImgSaveStat, (pF->bRpiSave?"��":"�~"), (m_bIsExec?"��":"�~") );

			mque_pOutSkip->AddToTail( pF );	// ���ʕ\���̂�

		} else {
//��
	LOG2(em_INF, "[%s] [%d] ������ (Fno=%d/%d) st=%d", my_sThreadName, ii, p->nFno, p->nCFno, p->emImgSaveStat );

			mque_pOut->AddToTail( pF);	// ����
		}
	}

	//// iPortQue��ԋp
	// �Z�߂đ���悤�́A�����K�v�Ȃ��̂ŁA�폜
	PoolBack_RecvFreeQue(mque_pRecvFree, p);		// ���g�̉���ƍ��킹�āAiPort�f�[�^��ԋp

	//LOG(em_MSG),"[%s] �J�����P��=%d/%d", my_sThreadName, mque_pRecvFree[p->nIns]->GetCountFree(), mque_pRecvFree[p->nIns]->GetCountPool() );
}

//------------------------------------------
// �������s
// FRAME_DATA_ARRAY* p �t���[�����
//------------------------------------------
void ImgBufProc::Exec(FRAME_DATA_ARRAY* p)
{
	// ���񉽂������ł悢
}