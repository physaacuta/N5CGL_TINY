#include "StdAfx.h"
#include "CoilControlProc.h"


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// int nIns �C���X�^���XNo
//------------------------------------------
CoilControlProc::CoilControlProc(int nIns) :
ThreadManager( GetThreadNameArray("CoilP", nIns) ),
m_nIns(nIns),
m_nRecvFno(0),
m_bDebugNextWpd(false),
m_nCFno(0),
m_bIsExec(false)
{
	memset( &m_NextCoil, 0x00, sizeof(m_NextCoil));
	memset( &m_NowCoil, 0x00, sizeof(m_NowCoil));
}

////------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CoilControlProc::~CoilControlProc(void)
{

}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int CoilControlProc::ThreadFirst()
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];


	//// �C�x���g�ݒ�
	HANDLE hArray[] = { this->mque_pIn->GetEvQue()
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int CoilControlProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void CoilControlProc::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_ADD_IMG = 0,					// �t���[���摜�L���[�C���O
			EV_MAIL_QUE						// �O������L���[
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
// ���Z�b�g
//------------------------------------------
void CoilControlProc::Reset()
{
	m_nRecvFno = 0;

	//// �R�C����񏉊���
	m_bDebugNextWpd = false;
	m_nCFno = 0;

	memset( &m_NowCoil, 0x00, sizeof(m_NowCoil));
}

//------------------------------------------
// �摜�擾
//------------------------------------------
void CoilControlProc::AddNewItem()
{
	//// �t���[���摜�L���[���擾
	FRAME_DATA_ARRAY* p = mque_pIn->GetFromHead();
	if( NULL == p ) {							// �ʏ킠�肦�Ȃ��͂�
		LOG(em_ERR), "[%s] �X���b�h���[�v�L���[����!", my_sThreadName);
		return;
	}

	//// ����
	bool bNextThread = true;
	if(m_bIsExec) {
		Exec_Len(p);

	} else {
		LOG(em_INF), "[%s] �L�����Z��", my_sThreadName);
	}

	//// ���X���b�h��
	if(bNextThread) {
		if( ! mque_pOut->AddToTail(p) ) {
			PoolBack_RecvFreeQue(mque_pRecvFree, p);		// ���g�̉���ƍ��킹�āAiPort�f�[�^��ԋp
		}
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �R�C�����֌W

//------------------------------------------
// ���R�C�����Z�b�g
// COMMON_QUE const* p ���R�C�����
//------------------------------------------
void CoilControlProc::SetNextCoil(COMMON_QUE const* p)
{
	//// ���Ɏ��R�C����񂪂���H
	if( 0 != strlen( m_NextCoil.cKanriNo ) ) {
		LOG(em_WAR), "[%s] ���R�C�����L�� [%s][%s]", my_sThreadName, m_NextCoil.cKanriNo, m_NextCoil.cCoilNo);
	}


	//// ���R�C�����ɃZ�b�g
	memset( &m_NextCoil, 0x00, sizeof(m_NextCoil));
	strcpy(m_NextCoil.cKanriNo, p->mix.cdata[0]);
	strcpy(m_NextCoil.cCoilNo, p->mix.cdata[1]);

	m_NextCoil.pcode[0] = p->mix.idata[1];
	m_NextCoil.pcode[1] = p->mix.idata[2];
	m_NextCoil.scode[0] = p->mix.idata[3];
	m_NextCoil.scode[1] = p->mix.idata[4];
	m_NextCoil.nChangeFno = p->mix.idata[5];
	m_NextCoil.mmFromHead = p->mix.idata[6];



	//// �R�C���P�ʂ̏��Z�b�g
	// �Ǎ�
	mcls_pParam->Read_IniCoilChange(m_nIns); 
	mcls_pParam->Read_IniKidoChange(m_nIns);

	// �Z�b�g
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ m_nIns ];
	int pcode = m_NextCoil.pcode[prmI.nMen] - 1;

	m_NextCoil.emCompType = prmI.emCompType;
	m_NextCoil.nCompQuality = prmI.nCompQualityBase;
	m_NextCoil.bDummyCoil = false;

	if (0 > pcode || MAX_PCODE <= pcode) {
		m_NextCoil.nCompQuality = prmI.nCompQualityBase;
	}
	else {
		m_NextCoil.nCompQuality = prmI.nCompQuality[pcode];
		if (0 == m_NextCoil.nCompQuality) m_NextCoil.nCompQuality = prmI.nCompQualityBase;
	}

	if (0 == strlen(m_NextCoil.cKanriNo)) {
		KizuFunction::GetKizukenNo(m_NextCoil.cKanriNo);
		strcpy(m_NextCoil.cCoilNo, "Dummey");
	}

	LOG(em_MSG), "[%s] ���ރZ�b�g[%s][%s]", my_sThreadName, m_NextCoil.cKanriNo, m_NextCoil.cCoilNo );
}

//------------------------------------------
// ���R�C�����G���A�Ƀ_�~�[�R�C�����Z�b�g
// COIL_INFO_BUF& pCoil ���R�C���G���A
//------------------------------------------
void CoilControlProc::SetDummeyCoil(COIL_INFO_BUF& typCoil)
{
	memset( &typCoil, 0x00, sizeof(typCoil));
	KizuFunction::GetKizukenNo( typCoil.cKanriNo ); 
	strcpy( typCoil.cCoilNo, "Dummey");

	// ���k���Z�b�g
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ m_nIns ];
	typCoil.emCompType = prmI.emCompType;
	typCoil.nCompQuality = prmI.nCompQualityBase;
	typCoil.bDummyCoil = true;
}

//------------------------------------------
// �R�C���ؑ�
// FRAME_DATA_ARRAY* p �t���[�����
// ���A���  true:�R�C���ؑ֗L��
//------------------------------------------
bool CoilControlProc::ChangeCoil(FRAME_DATA_ARRAY* p)
{
	int nCoilSet = 0;												// �ؑ֔F�� (0�F�Ȃ�  1:�ʏ� -1:�͂��߂̈��)

	//// �R�C���ؑ� ���O����
	if( 0 == strlen(m_NowCoil.cKanriNo) )	nCoilSet = -1;			// �n�߂̈��̂݁H

	if( 0 != strlen(m_NextCoil.cKanriNo) ) {
		long nSa = m_NextCoil.nChangeFno - p->nFno;
		if( 0 == nSa ) nCoilSet = 1;
		if( -100 < nSa && 0 > nSa ) {
			nCoilSet = 1;					// ���ɒʂ肷��
			LOG(em_WAR), "[%s] �ʂ�߂��ɂ�苭���ؑ� (��%d �ؑ֗\��%d)", my_sThreadName, p->nFno, m_NextCoil.nChangeFno);
		}
	}

	if(m_bDebugNextWpd) {
		// �����ؑ�
		if(0 == strlen(m_NextCoil.cKanriNo))  nCoilSet = -1;
		else									nCoilSet = 1;
		m_bDebugNextWpd = false;
	}


	//// �R�C���؂�ւ�
	if(0 != nCoilSet) {
		if( -1 == nCoilSet || 
			( 1 == nCoilSet && 0 == strlen(m_NextCoil.cKanriNo)) ) {
			LOG(em_WAR), "[%s] ���R�C����񖳂��B�_�~�[��񐶐�", my_sThreadName);
			
			// �_�~�[�R�C���Z�b�g
			COIL_INFO_BUF  typWk;
			SetDummeyCoil(typWk);
			memcpy(&m_NowCoil, &typWk, sizeof(COIL_INFO_BUF));

		} else {
			// �{�����Z�b�g
			memcpy(&m_NowCoil, &m_NextCoil, sizeof(COIL_INFO_BUF));
			memset(&m_NextCoil, 0x00, sizeof(COIL_INFO_BUF));
		}
		LOG(em_INF), "[%s] �R�C���ؑ֔F�� FNo=%d [%s][%s] COMP_Q=%d", my_sThreadName, p->nFno, m_NowCoil.cKanriNo, m_NowCoil.cCoilNo, m_NowCoil.nCompQuality);

		m_nCFno = p->nFno;
	}
	return (0==nCoilSet ? false : true);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �A�����C���֌W

//------------------------------------------
// �A�����C�����̏������s
// FRAME_DATA_ARRAY* p �t���[�����
//------------------------------------------
void CoilControlProc::Exec_Len(FRAME_DATA_ARRAY* p)
{
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ m_nIns ];

	bool bCoilSet;						// �R�C���ؑ֔F��
	
	//// �R�C���ؑ֔���
	bCoilSet = ChangeCoil(p);

	//// �R�C�����ɃZ�b�g
	memcpy(p->cKanriNo, m_NowCoil.cKanriNo, SIZE_KIZUKEN_NO );
	strcpy(p->cCoilNo, m_NowCoil.cCoilNo );
	p->mmFromHeadAtCoilStart = m_NowCoil.mmFromHead;
	p->nCFno = p->nFno - m_nCFno;

	p->emCompType   = m_NowCoil.emCompType;
	p->nCompQuality = m_NowCoil.nCompQuality;
	p->bDummyCoil	= m_NowCoil.bDummyCoil;



	// �ʏ��
	p->emImgSaveStat = bCoilSet ? IMG_SAVE_STAT_CHANGE : IMG_SAVE_STAT_SAVE;

 
	

	//// �A����摜���[����̃G�b�W�ʒu���Z�o
	HT_RPI_FDATA* pHt = p->pData[ p->nIdx ];
	p->nEdgePos[0] = pHt->dEdgePos[0];
	p->nEdgePos[1] = pHt->dEdgePos[1];


	int nRawEdge;						// ���摜����̃G�b�W�ʒu [pixel]
	// ���G�b�W
	if( 0.0 > pHt->dEdgePos[0] ) {
		int nWk = (int)(pHt->dEdgePos[0] / prmI.reso_x);				// ���C���Z���^�[����̃I�t�Z�b�g�ʒu [pixel]
		nRawEdge = prmI.nLineCenter + nWk;								// ���G�b�W�ʒu
		if( nRawEdge < 0 ) nRawEdge = 0;
	} else {
		nRawEdge = 0;
	}
	p->nEdgeIdx[0] = nRawEdge; 
	// �E�G�b�W
	if( 0.0 < pHt->dEdgePos[1] ) {
		int nWk = (int)(pHt->dEdgePos[1] / prmI.reso_x);				// ���C���Z���^�[����̃I�t�Z�b�g�ʒu [pixel]
		nRawEdge = prmI.nLineCenter + nWk;								// �E�G�b�W�ʒu
		if( nRawEdge >= prmI.nUionWidth ) nRawEdge = prmI.nUionWidth-1;
	} else {
		nRawEdge = prmI.nUionWidth-1;
	}
	p->nEdgeIdx[1] = nRawEdge; 
}
