#include "StdAfx.h"
#include "PioControlManager.h"

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
PioControlManager::PioControlManager(void) :
ThreadManager("PioMgr"),
m_nStPioReset(-1)
{
	//// ������
	m_nMaxBood = 0;
	for(int ii=0; ii<SMYU_PIO_MAX_BOOD; ii++) {
		m_hTblSmyu[ii] = NULL;
		mtbl_pSmyu[ii] = NULL;
	}
	m_bDegub[0] = false;
	m_bDegub[1] = false;

	memset(m_cBitName, 0x00, sizeof(m_cBitName));


	//// INI�t�@�C���ǂݍ���
	GetIniParam();

	//// ���L����������
	// ����
	for(int ii=0; ii<m_nMaxBood; ii++) {
		m_hTblSmyu[ii] = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SMYU_TBL_PIO), m_sSmyuName[ii]);
		mtbl_pSmyu[ii] =(SMYU_TBL_PIO*) MapViewOfFile(m_hTblSmyu[ii], FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if(m_hTblSmyu[ii] == NULL) {
			_ASSERT(FALSE);
		}
		memset(mtbl_pSmyu[ii], 0x00, sizeof(SMYU_TBL_PIO));
	}

	m_hTblSeq = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_SEQMEM_TBL), m_sSeqName);
	mtbl_pSeq = (TO_SEQMEM_TBL*)MapViewOfFile(m_hTblSeq, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (m_hTblSeq == NULL) {
		_ASSERT(FALSE);
	}

	//// �n���h������
	m_evTimerCycle = CreateWaitableTimer(NULL, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
PioControlManager::~PioControlManager(void)
{
	//// ���L�������J��
	for(int ii=0; ii<SMYU_PIO_MAX_BOOD; ii++) {
		UnmapViewOfFile(mtbl_pSmyu[ii]);
		CloseHandle(m_hTblSmyu[ii]);
	}
	CloseHandle(m_evTimerCycle);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int PioControlManager::ThreadFirst()
{
#ifdef SEQ_PLG
	__int64 ts = TIMER_CYCLE * -10000;
	SetWaitableTimer(m_evTimerCycle, (LARGE_INTEGER *)&ts, TIMER_CYCLE, NULL, NULL, FALSE);
#endif
	AddEventNum(1, &m_evTimerCycle);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int PioControlManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void PioControlManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {
		EV_TIMER_CYCLE = 0,				// �t���[���o�͒�������s�C�x���g
	};

	////// �V�O�i����������
	//-----------------------------------------------------------------------------------------------
	if (EV_TIMER_CYCLE == nEventNo) {
		ResetCheck();
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// Ini�t�@�C���擾
//------------------------------------------
void PioControlManager::GetIniParam()
{
	CString	sWk;
	char	cWk[1024];
	int		nWk;

	// ��
	nWk = GetPrivateProfileInt(GOT_CONTROL, "MAX_NUM_PIO_BOOD", 0, TASKINI_NAME);
	if(nWk < 0) nWk = 0;
	if(nWk > SMYU_PIO_MAX_BOOD) nWk = SMYU_PIO_MAX_BOOD;


	// ���̎擾
	for(int ii=0; ii<nWk; ii++) {
		sWk.Format("TBL_NAME_PIO_%d", ii); 
		GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);

		if(0 != strlen(cWk)) {
			m_sSmyuName[m_nMaxBood] = cWk;
			m_nMaxBood ++;
		}
	}
	GetPrivateProfileString(GOT_CONTROL, "TBL_NAME_SEQ", TBL_TO_SEQMEM_NAME, cWk, sizeof(cWk), TASKINI_NAME);	m_sSeqName = cWk;

	// �X�̃{�^�����̎擾
	for(int ii=0; ii<nWk; ii++) {
		for(int jj=0; jj<SMYU_PIO_MAX_PORT; jj++) {
			for(int kk=0; kk<8; kk++) {
				for(int ll=0; ll<2; ll++) {
					sWk.Format("BITNAME_%s_%d_%d_%d", 0==ll?"PI":"PO", ii, jj, kk);

					GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);
					if(0 != strlen(cWk)) memcpy( m_cBitName[ii][jj][kk][ll], cWk, 32 );

#ifdef SEQ_PIO
					sWk.Format("BITADDR_%s_%d_%d_%d", 0 == ll ? "PI" : "PO", ii, jj, kk);

					m_cBitAddr[ii][jj][kk][ll] = GetPrivateProfileInt(GOT_CONTROL, sWk, -1, TASKINI_NAME);
#endif
				}
			}
		}
	}
#ifdef SEQ_PLG
	m_nIdPioKen = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PIO_KEN", -1, TASKINI_NAME);
	m_nIdPioSub = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PIO_SUB", -1, TASKINI_NAME);
	m_nIdPioDsp = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PIO_DSP", -1, TASKINI_NAME);
	m_nIdPioReset = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PIO_RESET", -1, TASKINI_NAME);
#endif
}

//------------------------------------------
// �w��ʒu�r�b�g �� No �ɕϊ�
// BYTE addr		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// �߂�l			0�`7
//------------------------------------------
int PioControlManager::GetBitToIndex(BYTE addr)
{
	if		(0x01 == addr)	{ return 0; }
	else if (0x02 == addr)	{ return 1; }
	else if (0x04 == addr)	{ return 2; }
	else if (0x08 == addr)	{ return 3; }
	else if (0x10 == addr)	{ return 4; }
	else if (0x20 == addr)	{ return 5; }
	else if (0x40 == addr)	{ return 6; }
	else if (0x80 == addr)	{ return 7; }
	else					{ return 0; }	// ����		
}
//------------------------------------------
// �w��ʒuNo �� �r�b�g �ɕϊ�
// int index			0�`7
// �߂�l		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)			
//------------------------------------------
BYTE PioControlManager::GetIndexToBit(int index)
{
	if		(0 == index)	{ return 0x01; }
	else if (1 == index)	{ return 0x02; }
	else if (2 == index)	{ return 0x04; }
	else if (3 == index)	{ return 0x08; }
	else if (4 == index)	{ return 0x10; }
	else if (5 == index)	{ return 0x20; }
	else if (6 == index)	{ return 0x40; }
	else if (7 == index)	{ return 0x80; }
	else					{ return 0x00; }	// ����	
}

#ifdef SEQ_PIO
void PioControlManager::SetValPi(int bood, int port, BYTE onoff) {
	mtbl_pSmyu[bood]->nDi[port] = onoff;
	SetSeqToSmyu(bood, port, onoff, 0); 
}
void PioControlManager::SetValPi(int bood, int port, int bit, bool onoff) {
	if (onoff) { mtbl_pSmyu[bood]->nDi[port] |= GetIndexToBit(bit); }
	else { mtbl_pSmyu[bood]->nDi[port] &= ~GetIndexToBit(bit); }
	SetSeqToSmyu(bood, port, bit, onoff, 0);
}
void PioControlManager::SetValPo(int bood, int port, int bit, bool onoff) {
	if (onoff) { mtbl_pSmyu[bood]->nDo[port] |= GetIndexToBit(bit); }
	else { mtbl_pSmyu[bood]->nDo[port] &= ~GetIndexToBit(bit); } 
	SetSeqToSmyu(bood, port, bit, onoff, 1);
}
bool PioControlManager::GetValPi(int bood, int port, int bit) const {
	int nWk = m_cBitAddr[bood][port][bit][0];
	if (-1 == nWk)	return (mtbl_pSmyu[bood]->nDi[port] & GetIndexToBit(bit)) ? true : false;
	else			return mtbl_pSeq->fb_dm[0].Data[nWk] == 1 ? true : false;
}
bool PioControlManager::GetValPo(int bood, int port, int bit) const {
	int nWk = m_cBitAddr[bood][port][bit][1];
	if (-1 == nWk)	return (mtbl_pSmyu[bood]->nDo[port] & GetIndexToBit(bit)) ? true : false;
	else			return mtbl_pSeq->fb_dm[1].Data[nWk] == 1 ? true : false;
}
//BYTE GetValPo(int bood, int port) const			 { return mtbl_pSmyu[bood]->nDo[port]; }
#endif