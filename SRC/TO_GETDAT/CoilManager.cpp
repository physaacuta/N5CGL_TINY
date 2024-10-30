#include "StdAfx.h"
#include "CoilManager.h"

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��r�p�}�N��
#define JUDGE_CHECK_STR(coil, prm)   if(0!=strlen(prm)) { if(!clsAtlReg.IsRegularA(coil, sizeof(coil), prm) ) continue;  }		// ������`�F�b�N (���K�\��)
#define JUDGE_CHECK_INTVAL_L(coil, prm)  if(0!=prm[0]) { if(coil<prm[0]) continue; }											// �㉺���`�F�b�N (����)
#define JUDGE_CHECK_INTVAL_H(coil, prm)  if(0!=prm[1]) { if(coil>prm[1]) continue; }											// �㉺���`�F�b�N (���)


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �w�b�_�[��

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
CoilManager::CoilManager(LogFileManager* pLog) :
ThreadManager("CoilMgr")
{
	// ���O�t�@�C�����Z�b�g
	SetLogMgr(pLog);
	mcls_Param.SetLogMgr(pLog);

//*//----> ��
#ifdef LOCAL
	// LOCAL���ł� ���L�������������Ő�������
	m_hCoil = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_COIL_TBL), TBL_TO_COIL_NAME);
	mtbl_pCoil =(TO_COIL_TBL*) MapViewOfFile(m_hCoil, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
//*///<<---

	//// ���L�������A�N�Z�X
	// �R�C�����e�[�u��
	mem_connect(&m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME);
	if( NULL == m_hCoil ) {
		syslog(SYSNO_MEM_OPEN, "�R�C�����e�[�u�� �A�N�Z�X���s");
		_ASSERT(FALSE);
	}

	// ���̑��������
	memset(&m_typNewCoil, 0x00, sizeof(m_typNewCoil));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CoilManager::~CoilManager(void)
{
	//// ���L�������J��
	mem_close(&m_hCoil, (LPVOID *)&mtbl_pCoil);							// �R�C�����
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �r�����ʏ�����

//------------------------------------------
// �R�C���������
// int  nRec        ���L���������R�[�h�ʒu (0�I���W��)
// bool bRevFlg     �\�����]�t���O(�f�t�H���gFalse)
// bool bHandSetFlg �_�C�A���O���̓t���O(�f�t�H���gFalse)
//------------------------------------------
void CoilManager::Check_CoilInf(int nRec, bool bRevFlg, bool bHandSetFlg)
{
	//================================================
	// ��������

	COIL_SETTING			setting;
	COIL_BASEDATA			basedata;
	PRO_TYPE_COILINF		coil;

	int						nWk = 0;

	if (nRec < 0 || nRec >= 2) {
		LOG(em_ERR), "[%s] �R�C����� ���R�[�h�ʒu�ُ� [%d]", my_sThreadName, nRec);
		return;
	}

	////// ���L�������̏����擾����
	memset( &m_typNewCoil, 0x00, sizeof(m_typNewCoil));
	memset( &setting,   0x00, sizeof(setting));
	memset( &basedata,  0x00, sizeof(basedata));
	memcpy( &coil, mtbl_pCoil->rec[nRec].val, sizeof(coil));

	////// �����l�Z�b�g
	for(int ii=0; ii<NUM_MEN; ii++) {
		setting.bDefScode[ii] = true;
		setting.bDefPcode[ii] = true;
		setting.scode[ii] = MAX_SCODE;
		setting.pcode[ii] = MAX_PCODE;
		setting.scode_spmoff[ii] = MAX_SCODE;
		setting.pcode_spmoff[ii] = MAX_PCODE;
	}
	setting.bWalm = false;


	////// �ŐV�̃p�����[�^���擾����
	//// �p�����[�^�擾
	mcls_Param.SetLogMgr(mcls_pLog); 
	if( ! mcls_Param.ReadParam_All() ) {
		LOG(em_ERR), "[%s] �ŐV�p�����[�^�擾���s", my_sThreadName);
	}

	//================================================
	// data���𐶐�

	// ����No �� �v���R���`�� ����No
	memcpy(basedata.cMeiNo,	coil.data.CoilInf.KeyInf.cMeiNo, sizeof(coil.data.CoilInf.KeyInf.cMeiNo));
	// �����R�C��No+NULL  (�R�C��No) �K�C�_���X�p �� �v���R���`�� �R�C��No
	memcpy(basedata.cCoilNoIn,	coil.data.CoilInf.KeyInf.cCoilNo, sizeof(coil.data.CoilInf.KeyInf.cCoilNo));					// �R�C��No
	// �R�C������ӂɓ��肷�邽�߂̃L�[ �� �R�C��No+����No
	memcpy(basedata.cCoilKey, &coil.data.CoilInf.KeyInf, (SIZE_MEI_NO+PRO_SIZE_COIL_NO));

	// �����R�C����[m] (��ʏ��̒���) �� �v���R���`�� �R�C����[m] - �����e�[���[����[mm] - �����t�����g�[����[mm]
	// �����e�[���[�����Ɠ����t�����g�[������[mm] �� [m]�ɕϊ�
	basedata.nLenIn = (int)((float)coil.data.CoilInf.nLenIn - (float)coil.data.CoilInf.nCutF / 1000.0f - (float) coil.data.CoilInf.nCutT / 1000.0f);
	
	// �� [��m] �� �v���R���`�� �ޗ��� [��m]
	basedata.nAtu = coil.data.CoilInf.nAtuZai;

	// �� [mm] �� �v���R���`�� �ޗ���[mm]
	basedata.nWid = coil.data.CoilInf.nWidZai;

	// �����t�����g�[����[mm] �� �v���R���`�� �����t�����g�[���� [mm]
	basedata.nCutLenF = coil.data.CoilInf.nCutF;

	// �����e�[���[����[mm] �� �v���R���`�� �����e�[���[���� [mm]
	basedata.nCutLenT = coil.data.CoilInf.nCutT;

	// SPM�L�ї�[10^2%] �� �v���R���`�� SPM�L�ї� [10-2%]
	basedata.dNobiSPM = (double)(coil.data.CoilInf.nNobiSPM / 10000.0);

	// �o���R�C����[m]
	basedata.nLenOut = basedata.nLenIn;

	memcpy( &m_typNewCoil.basedata, &basedata, sizeof(COIL_BASEDATA) );

	//================================================
	// setting �������W

	//// �e�����p�^�[��������肷��
	for(int ii=0; ii<NUM_MEN; ii++) {
		setting.bDefScode[ii] = false;
		setting.bDefPcode[ii] = false;

		SelectScode(&coil.data, ii, true,  &setting.scode[ii] );			// �f�t�H���g SPM����
		SelectScode(&coil.data, ii, false, &setting.scode_spmoff[ii] );		// �R�C����[�ʉߎ��APIO��Ԃɂ��؂�ւ��p

		SelectPcode(&coil.data, ii, true,  &setting.pcode[ii] );			// �f�t�H���g SPM����
		SelectPcode(&coil.data, ii, false, &setting.pcode_spmoff[ii] );		// �R�C����[�ʉߎ��APIO��Ԃɂ��؂�ւ��p
	}

	//// �E�H�[�}�ޔ��� (������ƌ����Č������u�ł͕ʂɉ������Ȃ�)
	if( mcls_Param.GetCommonMast().nWalm == setting.pcode[0] ) {			// �E�H�[�}��?
		LOG(em_WAR), "[%s] �E�H�[�}�� �F���I�I [�R�C��No=%s]", my_sThreadName, basedata.cCoilNoIn);
		setting.bWalm = true;
	} else {
		setting.bWalm = false;
	}
	
	memcpy( &m_typNewCoil.setting, &setting, sizeof(COIL_SETTING));

	//================================================
	// ���㏈��
	// ����̌��ʂ����L�������ɃZ�b�g
	memcpy(&mtbl_pCoil->rec[nRec].setting,  &setting,  sizeof(setting));
	memcpy(&mtbl_pCoil->rec[nRec].basedata, &basedata, sizeof(basedata));
	
	CString sMsg;
	if( 0 == nRec ){
		sMsg.Format( "%s" , "�����p" );
	} else if( 1 == nRec ) {
		sMsg.Format( "%s" , "�\���p" );
	} else {
		// ���̏����͖{�����肦�Ȃ�
		sMsg.Format( "%s rec = (%d)" , "�s��" , nRec);
	}

	LOG(em_MSG), "[%s] [%s] �R�C��No=[%s] (%d/%d)(%d/%d) SPM_OFF(%d/%d)(%d/%d) %s ", my_sThreadName, sMsg,
		m_typNewCoil.basedata.cCoilNoIn, setting.scode[0], setting.scode[1],	setting.pcode[0], setting.pcode[1],
		setting.scode_spmoff[0], setting.scode_spmoff[1], setting.pcode_spmoff[0], setting.pcode_spmoff[1],
		setting.bWalm ? "�E�H�[�}��" : "");

	
	syslog(395, "[%s �R�C��No=[%s] (%d/%d)(%d/%d) SPM_OFF(%d/%d)(%d/%d) %s]", sMsg,
		m_typNewCoil.basedata.cCoilNoIn, setting.scode[0], setting.scode[1],	setting.pcode[0], setting.pcode[1], 
		setting.scode_spmoff[0], setting.scode_spmoff[1], setting.pcode_spmoff[0], setting.pcode_spmoff[1],
		setting.bWalm ? "�E�H�[�}��" : "");

	//// �����ɒʒm
	COMMON_QUE		que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo	= FACT_TO_DEFECT_11;	// �R�C������M�ʒm (��TO_GETDAT)
	que.nLineNo		= getlineid();
	que.fl.data[0]	= nRec;
	send_mail(TO_DEFECT, ".", &que);
}

//------------------------------------------
// �����\�ʏ�Ԃ�����
// SEQ_TYPE_COIL const* c �R�C�����
// int ou �\��
// bool bSpm SPM��� (true:��  false:�J)
// int* scode �Z�o���������\�ʏ�� (1�I���W��)
//------------------------------------------
void CoilManager::SelectScode(PRO_TYPE_DATA_COILINF const* c, int ou, bool bSpm, int* scode)
{
	AtlRegManager	clsAtlReg;				// ���K�\���N���X
	const ParamManager::P_SCODE_OUTPUT& p = mcls_Param.GetScodeOutput();

	int		id;
	int		nWk = 0;
	float	dWk = 0.0;

	// -1 �́A��ԍŌ�̃f�t�H���g�����s�����Ȃ����� 
	for(int ii=0; ii<p.nMasterNum; ii++) {
		id = p.nPriority[ii]; 

		// ���̃}�X�^�[�͗L���H
		if( ! p.rec[id].bUmu ) continue;

		// ���K�\���Ɉ�v����������`�F�b�N
		for(int jj=0; jj<MAX_CODE_JUDGE; jj++) {
			// ���̏����͗L���H
			if (!p.rec[id].typJudge[jj].bUmu) continue;

			//// ������r
			// SPM�����L�� (-1:���� 0:�~(����:�J��) 1:��(�L��:����))
			if (-1 != p.rec[id].typJudge[jj].nSpm) {
				if ((0 == p.rec[id].typJudge[jj].nSpm && bSpm) ||
					(1 == p.rec[id].typJudge[jj].nSpm && !bSpm)) {
					// SPM�����^�J���̕s��v
					continue;
				}
			}

			// ����No (���K�\��) �v���R��:����No ����:����No
			JUDGE_CHECK_STR(c->CoilInf.KeyInf.cMeiNo, p.rec[id].typJudge[jj].cMeiNo);

			// �R�C��No (���K�\��) �v���R��:�R�C��No ����:���YNo
			JUDGE_CHECK_STR(c->CoilInf.KeyInf.cCoilNo, p.rec[id].typJudge[jj].cCoilNo);

			// �� (����E����) �v���R��:������[��m] ����:������[��m]
			JUDGE_CHECK_INTVAL_L(c->CoilInf.nAtuSei, p.rec[id].typJudge[jj].nAtuSei);
			JUDGE_CHECK_INTVAL_H(c->CoilInf.nAtuSei, p.rec[id].typJudge[jj].nAtuSei);

			// ����� (���K�\��) �v���R��:����� ����:�����
			// �{�� ������͕\�����ɂ���̂����ACGL�́A���̃f�[�^�������ׁA���̏ꍇ���A�\�̃f�[�^���g�p
			if (0 == ou)
			{
				JUDGE_CHECK_STR(c->CoilInf.cKenKijun_O, p.rec[id].typJudge[jj].cKenKijun);
			}
			else
			{
				JUDGE_CHECK_STR(c->CoilInf.cKenKijun_O , p.rec[id].typJudge[jj].cKenKijun);
			}

			// ���� (���K�\��) �v���R��:���� ����:����
			JUDGE_CHECK_STR(c->CoilInf.cMuke, p.rec[id].typJudge[jj].cMuke);

			// �p�r (���K�\��) �v���R��:�p�r ����:�p�r
			JUDGE_CHECK_STR(c->CoilInf.cYouto ,	p.rec[id].typJudge[jj].cYouto);

			// ��ގ� (���K�\��) �v���R��:��ގ� ����:��ގ�
			JUDGE_CHECK_STR(c->CoilInf.cSyuZai, p.rec[id].typJudge[jj].cSyuZai);

			// �\�ʏ���1���� (���K�\��) �v���R��:�\�ʏ���1���� ����:�\�ʏ���1����
			JUDGE_CHECK_STR(c->CoilInf.cOmoteSyori1,	p.rec[id].typJudge[jj].cOmoteSyori1);

			// �i�� (���K�\��) �v���R��:�i�� ����:�i��
			JUDGE_CHECK_STR(c->CoilInf.cHinCode, p.rec[id].typJudge[jj].cHinCode);


			//// �����܂ł����炻�̌����\�ʏ�Ԃ̏���No�Ɉ�v
			LOG(em_MSG), "[%s] <%s> [%s] �����\�ʏ��=%d, ��v����No=%d ���g�p", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmModeName(bSpm), id+1, jj+1);
			*scode = id+1;
			return;
		}
	}

	// �����܂ł������v�����ׁ̈A�f�t�H���g���g�p
	LOG(em_MSG), "[%s] <%s> [%s] �f�t�H���g�����\�ʏ��=%d ���g�p", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmModeName(bSpm), MAX_SCODE);
	*scode = MAX_SCODE;
}

//------------------------------------------
// �����p�^�[��������
// SEQ_TYPE_COIL const* c �R�C�����
// int ou �\��
// bool bSpm SPM��� (true:��  false:�J)
// int* pcode �Z�o���������p�^�[�� (1�I���W��)
//------------------------------------------
void CoilManager::SelectPcode(PRO_TYPE_DATA_COILINF const* c, int ou, bool bSpm, int* pcode)
{
	AtlRegManager	clsAtlReg;				// ���K�\���N���X
	const ParamManager::P_PCODE_OUTPUT& p = mcls_Param.GetPcodeOutput();

	int		id;
	int		nWk = 0;
	float	dWk = 0.0;

	// -1 �́A��ԍŌ�̃f�t�H���g�����s�����Ȃ����� 
	for(int ii=0; ii<p.nMasterNum; ii++) {
		id = p.nPriority[ii]; 

		// ���̃}�X�^�[�͗L���H
		if( ! p.rec[id].bUmu ) continue;

		// ���K�\���Ɉ�v����������`�F�b�N
		for(int jj=0; jj<MAX_CODE_JUDGE; jj++) {
			// ���̏����͗L���H
			if (!p.rec[id].typJudge[jj].bUmu) continue;

			//// ������r
			// ����No (���K�\��) �v���R��:����No ����:����No
			JUDGE_CHECK_STR(c->CoilInf.KeyInf.cMeiNo, p.rec[id].typJudge[jj].cMeiNo);

			// �R�C��No (���K�\��) �v���R��:�R�C��No ����:���YNo
			JUDGE_CHECK_STR(c->CoilInf.KeyInf.cCoilNo, p.rec[id].typJudge[jj].cCoilNo);

			// �� (����E����) �v���R��:������[��m] ����:������[��m]
			JUDGE_CHECK_INTVAL_L(c->CoilInf.nAtuSei, p.rec[id].typJudge[jj].nAtuSei);
			JUDGE_CHECK_INTVAL_H(c->CoilInf.nAtuSei, p.rec[id].typJudge[jj].nAtuSei);

			// ����� (���K�\��) �v���R��:����� ����:�����
			// �{�� ������͕\�����ɂ���̂����ACGL�́A���̃f�[�^�������ׁA���̏ꍇ���A�\�̃f�[�^���g�p
			if (0 == ou)
			{
				JUDGE_CHECK_STR(c->CoilInf.cKenKijun_O, p.rec[id].typJudge[jj].cKenKijun);
			}
			else
			{
				JUDGE_CHECK_STR(c->CoilInf.cKenKijun_O, p.rec[id].typJudge[jj].cKenKijun);
			}

			// ���� (���K�\��) �v���R��:���� ����:����
			JUDGE_CHECK_STR(c->CoilInf.cMuke, p.rec[id].typJudge[jj].cMuke);

			// �p�r (���K�\��) �v���R��:�p�r ����:�p�r
			JUDGE_CHECK_STR(c->CoilInf.cYouto, p.rec[id].typJudge[jj].cYouto);

			// ��ގ� (���K�\��) �v���R��:��ގ� ����:��ގ�
			JUDGE_CHECK_STR(c->CoilInf.cSyuZai, p.rec[id].typJudge[jj].cSyuZai);

			// �\�ʏ���1���� (���K�\��) �v���R��:�\�ʏ���1���� ����:�\�ʏ���1����
			JUDGE_CHECK_STR(c->CoilInf.cOmoteSyori1, p.rec[id].typJudge[jj].cOmoteSyori1);

			// �i�� (���K�\��) �v���R��:�i�� ����:�i��
			JUDGE_CHECK_STR(c->CoilInf.cHinCode, p.rec[id].typJudge[jj].cHinCode);


			//// �����܂ł����炻�̌����p�^�[���̏���No�Ɉ�v
			LOG(em_MSG), "[%s] <%s> [%s] �����p�^�[��=%d, ��v����No=%d ���g�p", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmModeName(bSpm), id+1, jj+1);
			*pcode = id+1;
			return;
		}
	}

	// �����܂ł������v�����ׁ̈A�f�t�H���g���g�p
	LOG(em_MSG), "[%s] <%s> [%s] �f�t�H���g�����p�^�[��=%d ���g�p", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmModeName(bSpm), MAX_PCODE);
	*pcode = MAX_PCODE;
}
