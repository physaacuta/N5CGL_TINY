#include "StdAfx.h"
#include "CoilManager.h"

#include "MainInstance.h"

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
CoilManager::CoilManager(void) :
CoilBaseManager()
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CoilManager::~CoilManager(void)
{
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����W �s�P�ʊ֌W

//==========================================
// ���R�C�����Ɍ�����Ԃ��Z�b�g [��PlgManager]
//------------------------------------------
// int men			
// int rec			
// EM_DIV_KENSA st	
//==========================================
void CoilManager::SetData_StKensa(int men, int rec, EM_DIV_KENSA st)
{
	if( m_pCoilKenR[men]->emKensa[men][rec] < st )
	{
		m_pCoilKenR[men]->emKensa[men][rec] = st;
	}

	if( m_pCoilKenR[men]->nSetRec_St[men] >= rec )	return;
	m_pCoilKenR[men]->nSetRec_St[men] = rec;
	//LOG(em_MSG), "[%s] ���R�C�����Ɍ�����Ԃ��Z�b�g �r��No=%s rec=%d �����s=%d SetRec[0]=%d SetRec[1]=%d SpmUmu=%d men=%d", my_sThreadName, m_pCoilKenV[men]->cKizukenNo, m_pCoilKenV[men]->rec, rec, m_pCoilKenV[men]->nSetRec_St[SpmUmu][0], m_pCoilKenV[men]->nSetRec_St[SpmUmu][1], SpmUmu, men);
}

//==========================================
// ���R�C�����ɋ@���Ԃ��Z�b�g [��PlgManager]
//------------------------------------------
// int			men	: �\��(0:�\,1:��)
// int			rec	: ���R�[�h
// EM_DIV_KIKI	st	: �@����
//==========================================
void CoilManager::SetData_StKiki(int men, int rec, EM_DIV_KIKI st)
{ 
	if( m_pCoilKenR[men]->emKiki[men][rec] < st ) {
		m_pCoilKenR[men]->emKiki[men][rec] = st;
	}

	if( m_pCoilKenR[men]->nSetRec_St[men] >= rec ) return;
	m_pCoilKenR[men]->nSetRec_St[men] = rec;
	//LOG(em_MSG), "[%s] ���R�C�����ɋ@���Ԃ��Z�b�g �r��No=%s rec=%d �����s=%d SetRec[0]=%d SetRec[1]=%d SpmUmu=%d men=%d", my_sThreadName, m_pCoilKenV[men]->cKizukenNo, m_pCoilKenV[men]->rec, rec, m_pCoilKenV[men]->nSetRec_St[SpmUmu][0], m_pCoilKenV[men]->nSetRec_St[SpmUmu][1], SpmUmu, men);
}

//==========================================
// ���R�C�����ɃG�b�W�ʒu���Z�b�g [��HtRecv]
//------------------------------------------
// int		men		: �\��(0:�\,1:��)
// int		rec		: ���R�[�h
// float*	dEdge	: �G�b�W�ʒu
//==========================================
void CoilManager::SetData_Edge(int men, int rec, float* dEdge)
{
	for (int ii = m_pCoilKenV[men]->nSetRec_Edge[men] + 1; ii <= rec; ii++) {
		m_pCoilKenV[men]->dEdge[men][ii][0] = dEdge[0];
		m_pCoilKenV[men]->dEdge[men][ii][1] = dEdge[1];
	}

	if (m_pCoilKenV[men]->nSetRec_Edge[men] >= rec) return;
	m_pCoilKenV[men]->nSetRec_Edge[men] = rec;
	//LOG(em_MSG), "[%s] ���R�C�����ɃG�b�W�ʒu���Z�b�g �r��No=%s rec=%d �����s=%d SetRec[0]=%d SetRec[1]=%d SpmUmu=%d men=%d Edge=%f/%f", my_sThreadName, m_pCoilKenV[men]->cKizukenNo, m_pCoilKenV[men]->rec, rec, m_pCoilKenV[men]->nSetRec_St[SpmUmu][0], m_pCoilKenV[men]->nSetRec_St[SpmUmu][1], SpmUmu, men, dEdge[0], dEdge[1]);
}

//==========================================
// ���R�C�����ɍs�P�ʂ̏����Z�b�g [��HtRecv]{ 
//------------------------------------------
// int				men		: �\��(0:�\,1:��)
// int				rec		: ���R�[�h
// int				index	: �Z�b�g�ʒu
// MARK_ROW_BUF*	buf		: �}�[�L���O���(�s�P��)
//==========================================
void CoilManager::SetData_Row_V	(int men, int rec, int index, int val)
{
	m_pCoilKenV[men]->nRowVal[men][rec][index] = val;
	//LOG(em_MSG), "[%s] ���R�C�����ɍs�P�ʂ̏����Z�b�g  rec=%d SetRec[0]=%d SetRec[1]=%d men=%d", my_sThreadName, m_pCoilKenV[men]->rec, m_pCoilKenV[men]->nSetRec_St[0], m_pCoilKenV[men]->nSetRec_St[1], men);
}

void CoilManager::SetData_Row_R(int men, int rec, int index, int val)
{ 
	m_pCoilKenR[men]->nRowVal[men][rec][index] = val;
}

//==========================================
// ���R�C�����̍ŏI���R�[�h���X�V
//------------------------------------------
// int	men	: �\��(0:�\,1:��)
// int	rec	: ���R�[�h
//==========================================
void CoilManager::SetData_Row_V_Ls(int men, int rec)
{ 
	if( m_pCoilKenV[men]->nSetRec_Last[men] >= rec ) return;
	m_pCoilKenV[men]->nSetRec_Last[men] = rec;
	//LOG(em_MSG), "[%s] ���R�C�����̍ŏI���R�[�h���X�V rec=%d SetRec[0]=%d SetRec[1]=%d men=%d SPM=%d", my_sThreadName, m_pCoilKenV[men]->rec, m_pCoilKenV[men]->nSetRec_St[SpmUmu][0], m_pCoilKenV[men]->nSetRec_St[SpmUmu][1], men, SpmUmu);
}
void CoilManager::SetData_Row_R_Ls(int men, int rec)
{ 
	if( m_pCoilKenR[men]->nSetRec_Last[men] >= rec ) return;
	m_pCoilKenR[men]->nSetRec_Last[men] = rec;
}


//==========================================
// �R�C���ʒu���𐶐�
//------------------------------------------
// bool mode false �f�[�^���镪�����S���o�^ true:���������f����
// int men �\��
//==========================================
void CoilManager::SetData_CoilPosition(bool mode, int men)
{
	//LOG(em_MSG), "[%s] �R�C���ʒu���𐶐� rec=%d SetRec[0]=%d SetRec[1]=%d men=%d", my_sThreadName, m_pCoilKenV[men]->rec, m_pCoilKenV[men]->nSetRec_St[0], m_pCoilKenV[men]->nSetRec_St[1], men);

	AutoLock autolock(&m_csLock);		//---------------------------->>>

	COIL_INF* pCoil = m_pCoilKenV[men];
	int nMenState = mcls_pStatus->GetMenState();				// �����ʏ��
	int nSetRec = -1;

	//================================================
	// �f�[�^�Z�b�g�ʒu�̌���
	if(! mode) {													// ���镪����
		// �傫����
		if( nSetRec < pCoil->nSetRec_Last[0] ) nSetRec = pCoil->nSetRec_Last[0];
		if( nSetRec < pCoil->nSetRec_Last[1] ) nSetRec = pCoil->nSetRec_Last[1];
		if( nSetRec < pCoil->nSetRec_St[0] )   nSetRec = pCoil->nSetRec_St[0];
		if( nSetRec < pCoil->nSetRec_St[1] )   nSetRec = pCoil->nSetRec_St[1];
		if( nSetRec < pCoil->nSetRec_Edge[0] ) nSetRec = pCoil->nSetRec_Edge[0];
		if( nSetRec < pCoil->nSetRec_Edge[1] ) nSetRec = pCoil->nSetRec_Edge[1];

	} else if( -1!=nMenState || ! mode ) {							// �Жʌ���
		//// �Жʌ����̏ꍇ�A�㗬���݂̂̌����̏ꍇ�A�@���Ԃ����f����Ȃ��ׂ̑΍�
		// �f�[�^�̗���͑傫����
		nSetRec = pCoil->nSetRec_Last[0] > pCoil->nSetRec_Last[1] ? pCoil->nSetRec_Last[0] : pCoil->nSetRec_Last[1];
		// ���R�C���͏������ق�
		if( nSetRec > pCoil->nSetRec_St[0] ) nSetRec = pCoil->nSetRec_St[0];
		if( nSetRec > pCoil->nSetRec_St[1] ) nSetRec = pCoil->nSetRec_St[1];
		if( nSetRec > pCoil->nSetRec_Edge[0] ) nSetRec = pCoil->nSetRec_Edge[0];
		if( nSetRec > pCoil->nSetRec_Edge[1] ) nSetRec = pCoil->nSetRec_Edge[1];
	} else {														// ���ʌ���
		// ���ʌ������� ��������
		nSetRec = pCoil->nSetRec_Last[0] < pCoil->nSetRec_Last[1] ? pCoil->nSetRec_Last[0] : pCoil->nSetRec_Last[1];
		if( nSetRec > pCoil->nSetRec_St[0] ) nSetRec = pCoil->nSetRec_St[0];	// ���Ԃ�K�v�Ȃ��Ǝv�����O�̈�
		if( nSetRec > pCoil->nSetRec_St[1] ) nSetRec = pCoil->nSetRec_St[1];
		if( nSetRec > pCoil->nSetRec_Edge[0] ) nSetRec = pCoil->nSetRec_Edge[0];
		if( nSetRec > pCoil->nSetRec_Edge[1] ) nSetRec = pCoil->nSetRec_Edge[1];
	}
	
	//// �����܂ł̏���
	int nOldRec = pCoil->nLastRec_Postion;
	if( nSetRec > pCoil->nLastRec_Postion) {
		pCoil->nLastRec_Postion = nSetRec;
	}

	autolock.UnLock();		// <<<-------------------------------------

	//================================================
	// ����ʒu�������ݑ΍�
	if( nSetRec > nOldRec ) {
	//LOG(em_MSG), "�r�� %d, %d, %d-%d,  %d,%d,%d,%d", men, pCoil->rec, pCoil->nLastRec_Postion+1, nSetRec, pCoil->nSetRec_Last[0], pCoil->nSetRec_Last[1], pCoil->nSetRec_St[0], pCoil->nSetRec_St[1]);// ��

		for(int ii=nOldRec+1; ii<=nSetRec; ii++ ) {
			//// �R�C���P�ʂň��Ԃł��ُ킪����Έُ�Ƃ���
			for(int jj=0; jj<NUM_MEN; jj++) {
				// ������� (��������������l������ׂ̑΍�)
				if( pCoil->emOneKensa[jj] < pCoil->emKensa[jj][ii] ) pCoil->emOneKensa[jj] = pCoil->emKensa[jj][ii];
				// �@����
				if( pCoil->emOneKiki [jj] < pCoil->emKiki[jj][ii]  ) pCoil->emOneKiki[jj]  = pCoil->emKiki[jj][ii];
			}

			//// �o�^
			SaveDB_CoilPosition(men, ii);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����W �R�C���P�ʊ֌W

//==========================================
// �����_���R�C�����т𐶐� (���ۂ̕��̗���̃V��)
//------------------------------------------
// int endMode ���ђ��߂̏�� (1:�ʏ�ؑ� 2:�o�b�N�A�b�v�ؑ� 3:�蓮��~ 4:������~ 0:�s��܂��͂܂��ʔ�)
// double dLen ������ [mm]
// double dSubLen ������r�p���� [mm]
//==========================================
void CoilManager::SetData_CoilResult_Real(int endMode, double dLen, double dLenSub)
{
	AutoLock autolock(&m_csLock);

	//// ���߂��f�[�^�͒���
	m_CoilInf[m_nKenRecBase].nEndMode	= endMode;
	m_CoilInf[m_nKenRecBase].nLength	= (long)dLen;
	m_CoilInf[m_nKenRecBase].nLengthSub = (long)dLenSub;


	//// �m��ʒm�𑗂�
	// �R�C���̐؂�ւ� = �O��̃R�C���̎������m��
	OpSendFunc::SendOp_CoilResult(&m_CoilInf[m_nKenRecBase]); 
}


//==========================================
// �����_���R�C�����т��Z�b�g (�f�[�^�̗���̃V��)
//	 ����ʂ���Ȃ����̒ʔR�C���ɑ΂��ăZ�b�g
//==========================================
void CoilManager::SetData_CoilResult_Virtual()
{
	AutoLock autolock(&m_csLock);

	//// ���ы����I�ɒ��߂�\���̂���̂ŁA�m�莞�͑S���o�^���Ă���
	SetData_CoilPosition(false, m_nDistNotPos);
	//// �R�C������ �o�^
	SaveDB_CoilResult();
	//// �O�H���������u�nDB���Ǘ�No�t���O�Z�b�g
	mcls_pMae->RecvCoilResultWrite(m_pCoilKenV[m_nDistNotPos]);
}

//==========================================
// �V���[�J�b�g���s	[��MI]
//  �E�t�@�[�X�g�J�b�g�M��ON���́A�o�������R�C�����т̊m��
//  �E���J�b�g�M��ON��t�@�[�X�g�J�b�g�M���̘A��ON���́A
//    ���X�g�J�b�g�ʒu�̍X�V
//------------------------------------------
// double	dLenIn	: �V���[�J�b�g�ʒu�ł̓����_���R�C������[mm]
// int		mode	: 0:�ʏ�J�b�g�A1:���ъm�莞
// �߂�l	int		: �J�b�g��� (0:F�J�b�g 1:�r���J�b�gorL�J�b�g(���̎��_�ł͂킩��Ȃ�))
//==========================================
void CoilManager::SetData_ShaCut(double dLenIn, int mode)
{
	AutoLock autolock(&m_csLock);

	COIL_INF* pCoil = m_pCoilDsp[POS_DSP_SHR_I];		// ���V���[�ʒu�ʉߒ��̃R�C�����|�C���^
	

	// �V���[�J�b�g�ʒu�ʉߒ��̊Y���R�C���̃J�b�g�񐔃`�F�b�N
	if (MAX_SHACUT_COUNT <= pCoil->nCutCount) {
		if (0 == mode)
		{
			LOG(em_WAR), "[%s] �V���[�J�b�g�Ǘ��񐔏�����B�ɂ��A�V���[�J�b�g����j��", my_sThreadName);
			syslog(297, "[�R�C��No=%.12s]", m_pCoilBaseDsp->data.cCoilNoIn);
		}
		return;
	}

	//================================================
	// �J�b�g���i�[

	// �f�[�^�Z�b�g
	memcpy(m_typCut.cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
	m_typCut.dPos = dLenIn;
	
	// �V���[�J�b�g�ʒu�ʉߒ��̊Y���R�C���Ƀf�[�^�Z�b�g
	pCoil->nCutLen[pCoil->nCutCount] = (int)dLenIn;
	pCoil->nCutCount++;

	////// ���ɊY���R�C���̔��[���r���ʒu��ʉ߂��Ă���ꍇ�́A�R�C�����т�DB�Ɋi�[���Ă���ׁA
	////// �R�C�����т�update����B
	//if (pCoil->bCoilResultAddDb) {
	//	// �R�C�����т��r���Ǘ��̃V���[�J�b�g����ύX
	//	SaveDB_UpdateCoilResult(MODE_RECV_SHRCUT, pCoil);
	//}

	//---------------------------------------
	// �o���֘A
	//---------------------------------------
	int nRec=0;
	//// �o���R�C�����L��H
	if (0 != strlen(m_CoilOutInf.cID)) {

		nRec = m_CoilOutInf.nCnt - 1;

		// �ŏI�f�[�^�m��
		m_CoilOutInf.incoil[nRec].nLength = (int)dLenIn - m_CoilOutInf.incoil[nRec].nFront;

		// �o���m�肵���ׁADb�o�^
		SaveDB_CoilCutPos(EM_DIV_CUT::DIV_CUT_FCUT, &m_typCut);
	}

	//���ъm�莞�͂����܂�
	if (1==mode) return;


	int nWk = m_CoilOutInf.incoil[nRec].nInCutNum;
	nRec = 0;
	//// �o���R�C����� �V�K�쐬
	memset(&m_CoilOutInf, 0x00, sizeof(m_CoilOutInf));
	KizuFunction::GetSerialNoFF(m_CoilOutInf.cID);					// �V���A��No�Z�b�g
	// �擪�̓����_���R�C�����擾 (���V���[�ʒu�ʉߒ��̓����_���R�C��)
	memcpy( m_CoilOutInf.incoil[nRec].cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);

	m_CoilOutInf.incoil[nRec].nFront = (int)dLenIn;
	m_CoilOutInf.incoil[nRec].nOutPos = 0;
	m_CoilOutInf.incoil[nRec].nInCutNum = nWk + 1;
	m_CoilOutInf.incoil[nRec].nSt_Spm = pCoil->nSt_Spm;
	m_CoilOutInf.incoil[nRec].dSpmHosei = pCoil->dSpmHosei;
	m_CoilOutInf.nCnt += 1;

	//// �\���֒ʒm
	TO_SO_DATA_CUT  data;
	memset(&data, 0x00, sizeof(data));
	data.nMode = DIV_CUT_FCUT;
	data.nPos = (int)m_typCut.dPos;
	OpSendFunc::SendOp_CutInf(pCoil->cKizukenNo, &data);

	LOG(em_MSG), "[%s] �V���[�J�b�g�M��ON%d��� <%.10s><%.20s> In=%dmm", my_sThreadName,
		pCoil->nCutCount, pCoil->data.cCoilNoIn, pCoil->cKizukenNo, (long)dLenIn );
}

//==========================================
// �R�C���擪�̐ؑ֏��� [��MI]
//------------------------------------------
// int	nMode	: 0:����ؑ� 1:�Q�[�g�M���ɂ��o�b�N�A�b�v	
//==========================================
void CoilManager::SetData_StartWpd(int nMode)
{
	AutoLock autolock(&m_csLock);

	//// ����̌�����_
	m_CoilInf[m_nKenRecBase].nStartWpdOk = nMode;									
}

//==========================================
// �����J�n����N�{�� [��MI]
//==========================================
void CoilManager::SetData_CoilCnt()
{
	AutoLock autolock(&m_csLock);

	//// ��{�O�̃R�C������l���擾
	int nPreCnt = GetCoilIndex(m_nKenRecBase, 1)->nCoilCnt;

	if( INT_MAX == nPreCnt )	m_CoilInf[m_nKenRecBase].nCoilCnt = 1;
	else						m_CoilInf[m_nKenRecBase].nCoilCnt = nPreCnt + 1;
}

//==========================================
// �\��R�C�����Z�b�g [��MI]
// double dLen �\��R�C���� [mm]
//------------------------------------------
void CoilManager::SetData_CoilLenYotei(double dLen)
{
	AutoLock autolock(&m_csLock);
	
	//// ����̌�����_
	m_CoilInf[m_nKenRecBase].nLen_Yotei = (long)dLen;
}

//==========================================
// ���G�b�W�����ϋP�x���R�C�����тɃZ�b�g
//------------------------------------------
// int*		pBr	: �J�������̃G�b�W�����ϋP�x	
//==========================================
void CoilManager::SetData_CoilResult_BrVal(int* pBr)
{
	AutoLock autolock(&m_csLock);

	for(int ii=0; ii<NUM_CAMERA; ii++){
		m_pCoilKenV[NUM_TorB(ii)]->nBrVal[ii] = pBr[ii];
	}
}

//==========================================
// ���I�����Ԃ��R�C�����тɃZ�b�g [��MI]
//------------------------------------------
// int*		pExp	: �J�������̘I������
//==========================================
void CoilManager::SetData_CoilResult_AgcVal(int* pExp)
{
	AutoLock autolock(&m_csLock);

	for(int ii=0; ii<NUM_CAMERA; ii++){
		m_pCoilKenV[NUM_TorB(ii)]->nAgcVal[ii] = pExp[ii];
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����W ���̑�

//==========================================
// ���׌��o�x����� 	[��MI]
// �ʒm���ꂽ�^�C�~���O�ŁA�\�ʌ�����ʒu��ʔ��̃R�C�����ΏۂƂȂ�
//------------------------------------------
// int		pos				: �\���ʒu (0�I���W��)
//==========================================
void CoilManager::SetData_AlarmCancel(int pos)
{
	AutoLock autolock(&m_csLock);

	// �\���ʒu��ʔ��R�C�����Ώ�
	if( 0 > pos || pos >= MAX_DISP_POS ) return;
	COIL_INF const* pCoil = GetCoilAll(pos);
	if( NULL == pCoil ) return;

	// ���׌��o�x������̃t���O�Z�b�g
	int rec = pCoil->rec;
	if( m_CoilInf[rec].bAlarmCancel ) {
		m_CoilInf[rec].bAlarmCancel = false;
		LOGA(em_MSG), "[%s] ���׌��o�x��o�� ��t�����I<%s>", my_sThreadName, m_CoilInf[rec].cKizukenNo );
		//LOG(em_WAR), "[%s] ���׌��o�x��������� ����� �x��������󂯕t���� <%s>", my_sThreadName, m_CoilInf[rec].cKizukenNo );
	} else {
		m_CoilInf[rec].bAlarmCancel = true;
		LOGA(em_MSG), "[%s] ���׌��o�x����� ��t�����I<%s>", my_sThreadName, m_CoilInf[rec].cKizukenNo );
	}
}

//==========================================
// ���R�C�����f���A�����J�n1�{�ڂ��o�b�t�@����N���A[��MI]
//==========================================
void CoilManager::ClearFirstCoilInf()
{
	CoilClear(&m_CoilInf[GetRecIndex(0,0)]);
}

//==========================================
// �r���t�@�C���o�͐�t�@�C�����̂�ԋp
//------------------------------------------
// �y�Ăяo�����z
//  HtRecv
//		OnMainDefect
//  CoilManager
//      SetData_CoilResult_Virtual
//------------------------------------------
// int	men		: �\��(0:�\,1:��)
// int	nRec	: �����_���R�C�����̃o�b�t�@�̃��R�[�hNo
//==========================================
CString CoilManager::GetDefectDumpPath(int nMen, int nRec) {
	CString s;
	char	cBaseFolder[32];

	GetPrivateProfileString("TO_DEFECT", "DEF_DUMP_PATH", "C:\\CSV", cBaseFolder, 32, TASKINI_NAME);
	s.Format("%s\\Def_Men=%d_%d.csv", cBaseFolder, nMen, nRec);		// �r���t�@�C������

	return s;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get

int	CoilManager::GetRecKey(char const* cMeiNo, char const* cCoilNo)
{
	if (0 == strlen(cMeiNo) || 0 == strlen(cCoilNo)) {
		LOG(em_ERR), "[CoilManager] [�R�C�����`��] �A�g�p�R�C�����R�Â����s�i����No,�R�C��No NULL�l�j[����No:%.4s][�R�C��No:%.12s]", cMeiNo, cCoilNo );
		return -1;
	}

	COIL_INF const* pWk;
	if (-1 == m_nKenRecBase) return -1;

	for (int ii = 0; ii<SIZE_COIL_BUF; ii++) {
		pWk = GetCoilIndex(m_nKenRecBase, ii);

		if (0 == memcmp(pWk->data.cCoilNoIn, cCoilNo, SIZE_COIL_NO) && 
			0 == memcmp(pWk->data.cMeiNo, cMeiNo, SIZE_MEI_NO))			return pWk->rec;
	}
	// �����܂ŗ�����Y������
	return -1;
}

void CoilManager::SetAlameInf(DETECT_BASE_DATA const* pD, int men, COIL_INF* pCoil, double const* pAttr)
{
	if (NULL == pCoil) return;
	//================================================
	// ���O����
	//
	bool bAdd = false;												// �O���苭���r��M������t���O
																	//COIL_INF* pCoil = mcls_pCoil->GetCoilKen_DataSet(men);	// ���������u�ʒu�ʉߒ��̃R�C�����|�C���^
	int nWidth = pCoil->data.nWid;								// �R�C����
	int nMapCol = pCoil->nMapCol;								// ���R�C���̃}�b�v�O���b�h��
	int pcode = pCoil->setting.pcode[men] - 1;				// �w�肳�ꂽ�������u�ʒu�̃R�C���̌����p�^�[�� (0�I���W���ɂ���)
	int rank = 0;												// �O���[�h�����N

	int row = (int)(pD->y / OUTPUT_MESYU_SEKKIN_ROW);				// �������b�V���̍s�ʒu (0�I���W��)
	int col = pD->nWidDiv;											// �������b�V���̗�ʒu (0�I���W��)
	if (0 > row || row >= OUTPUT_MESYU_SEKKIN_MAX) return;

	//======================================================
	// �O����
	if (!mcls_pStatus->IsRun()) return;		// ������~���͌x��o�͂Ȃ�
	if (DIV_UNTEN_NOMAL != mcls_pStatus->GetUnten()) return;


	if (pCoil->bAlarmCancel)		return;		// �x��L�����Z����

	ParamManager::P_TYPE_NAME::_TYPE_ID const * prmT = mcls_pParam->GetTypeName(pD->nTid);
	if (NULL == prmT) return;


	//================================================
	// �����Z�b�g
	OUTPUT_ITEM	inf;
	inf.bUmu = true;
	inf.nMen = men;
	inf.nKizuNo = pD->nKizuNo;
	inf.nTid = pD->nTid;
	inf.nGid = pD->nGid;
	inf.nTYuu = pD->nTYuu;
	inf.nGYuu = pD->nGYuu;
	inf.dAria = pD->aria;
	inf.nLen = pD->y;
	inf.nWidDiv = pD->nWidDiv;
	inf.nDs = (int)pD->ds;
	inf.nWs = (int)pD->ws;
	inf.nKind = em_RAMP_NON;
	memset(inf.bPatoFlg, 0x00, sizeof(bool)*MAX_DISP_POS);
	inf.cycle_div = (int)pD->cycle_div;
	//inf.nKind	= prmT->nKizuVoic;

	LOGA(em_MSG), "[%s] �r���� [%s][men=%d row=%d col=%d No=%d, len=%.3f, nWidDiv=%d, t=%d, g=%d, width=%d]",
		my_sThreadName, pCoil->cKizukenNo,
		inf.nMen, row, col, inf.nKizuNo, inf.nLen / 1000.0, inf.nWidDiv, inf.nTid, inf.nGid, nWidth);

	//================================================
	// �x��o�̓Z�b�g
	//================================================
	//// �r�ڋ߃����v �`�F�b�N
	//-----------------------------
	// �d����
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_ALARM_RANP_HIGH, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.ranp[men][row][col].bUmu ||
			 pCoil->alarme.ranp[men][row][col].nKind < em_RAMP_JYU) &&
			inf.nKind < em_RAMP_JYU) {
			inf.nKind = em_RAMP_JYU;
		}
	}
	//-----------------------------
	// �y����
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_ALARM_RANP_LOW, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.ranp[men][row][col].bUmu ||
			pCoil->alarme.ranp[men][row][col].nKind < em_RAMP_KEI) &&
			inf.nKind < em_RAMP_KEI) {
			inf.nKind = em_RAMP_KEI;
		}
	}
	//-----------------------------
	if (em_RAMP_NON != inf.nKind)
	{	// �����r������
		LOGA(em_MSG), "[%s] �r�ڋ߃����v�o�͑Ώ��r����", my_sThreadName);
		memcpy(&pCoil->alarme.ranp[men][row][col], &inf, sizeof(inf));
	}

	// �{�C�X�p�̍s���ɕύX
	row = (int)(pD->y / OUTPUT_MESYU_VOIC_ROW);				// �������b�V���̍s�ʒu (0�I���W��)
	if (0 > row || row >= OUTPUT_MESYU_ROW) return;
	int nVoiceMapCol = KizuFunction::SelectGridCol2(nWidth, OUTPUT_MESYU_VOIC_COL, OUTPUT_MESYU_COL);
	int nVoiceCol = KizuFunction::SelectColPosition2(pD->dsws, pD->ds, pD->ws, nWidth, nVoiceMapCol, OUTPUT_MESYU_COL, OUTPUT_MESYU_VOIC_COL);

	LOGA(em_MSG), "[%s] �{�C�X�p�ʒu [%s][men=%d row=%d col=%d edge_l=%.0f edge_r=%.0f]",
		my_sThreadName, pCoil->cKizukenNo, inf.nMen, row, nVoiceCol, pAttr[CENTER_X_FROM_L_EDGE], pAttr[CENTER_X_FROM_R_EDGE]);

	//================================================
	//// ���׌��o�{�C�X �`�F�b�N

	// �������猇�׌��o�{�C�X�����Ȃ̂ŏ�����
	inf.nKind = em_VOICE_ERR_NON;

	// �D�揇��
	//-----------------------------
	// (�d)�A���r�ڋ�
	//     �d���r�ڋ�
	//     �w�Q�ڋ�
	//     �X���o�[�ڋ�
	//     �\�����r
	//     �����r����
	//     �����r�ڋ�
	// (�y)�r�ڋ�

	//-----------------------------
	// �A���r���f�p ������Ɠ���B
	// ������r�̘A�����𔻒�
	int ii = 0;
	if (!pCoil->alarme.voic_re[men][row][nVoiceCol])
	{
		// �r����萔�A���Ŕ������Ă������߁A�����ʂ�̎菇�Ōx��{�C�X��ރZ�b�g
		if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_SEQ, pcode, inf.nTid, inf.nGid))
		{
			pCoil->alarme.voic_re[men][row][nVoiceCol] = true;

			// �A���r����
			for (ii = 1; ii < OUTPUT_VOICE_RE_ROW; ii++)
			{
				if (0 > row - ii) break;
				if (!pCoil->alarme.voic_re[men][row - ii][nVoiceCol]) break;
			}
			if (OUTPUT_VOICE_RE_ROW == ii)
			{
				LOGA(em_MSG), "[%s] �{�C�X�F�A���r���o", my_sThreadName);
				inf.nKind = em_VOICE_DEF_RE;
			}
		}
	}
	//-----------------------------
	// �d���r
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_HIGH, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			 GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_JU)) &&
			 GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_JU))
		{
			inf.nKind = em_VOICE_DEF_JU;
		}
	}

	//-----------------------------
	// �w�Q
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_HEGE, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_HE)) &&
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_HE))
		{
			inf.nKind = em_VOICE_DEF_HE;
		}
	}

	//-----------------------------
	// �X���o
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_SURI, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_SU)) &&
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_SU))
		{
			inf.nKind = em_VOICE_DEF_SU;
		}
	}

	//-----------------------------
	// �\�����r ������Ɠ���B
	// �r���\���ʂŔ������Ă������߁A�����ʂ�̎菇�Ōx��{�C�X��ރZ�b�g
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_SAME, pcode, inf.nTid, inf.nGid))
	{
		pCoil->alarme.voic_tb[men][row][nVoiceCol] = true;

		// �\���r����
		//// ���̈ʒu�𒆐S�ɋt�ʂ�4�����Ɍ��ׂ����邩�T��
		bool bWk = false;
		int nNotMen = 0 == men ? 1 : 0;
		if (pCoil->alarme.voic_tb[nNotMen][row][nVoiceCol]) bWk = true;												// ���S 
		if (0 <= row - 1)				if (pCoil->alarme.voic_tb[nNotMen][row - 1][nVoiceCol]) bWk = true;			// ��
		if (OUTPUT_MESYU_ROW > row + 1)	if (pCoil->alarme.voic_tb[nNotMen][row + 1][nVoiceCol]) bWk = true;			// ��
		if (0 <= nVoiceCol - 1)		 		if (pCoil->alarme.voic_tb[nNotMen][row][nVoiceCol - 1]) bWk = true;		// ��
		if (OUTPUT_MESYU_COL > nVoiceCol + 1)	if (pCoil->alarme.voic_tb[nNotMen][row][nVoiceCol + 1]) bWk = true;	// �E
		if (bWk)
		{
			if ((!pCoil->alarme.voice[men][row].bUmu ||
				GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_TB)) &&
				GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_TB))
			{
				inf.nKind = em_VOICE_DEF_TB;
				LOGA(em_MSG), "[%s] �{�C�X�F�\�����r���o", my_sThreadName);
			}
		}
	}

	//-----------------------------
	// �����r
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_CYCLE, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_SY)) &&
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_SY))
		{
			inf.nKind = em_VOICE_DEF_SY;
		}
	}

	//-----------------------------
	// ���̑��r
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_OTHER, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_OT)) &&
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_OT))
		{
			inf.nKind = em_VOICE_DEF_OT;
		}
	}

	//-----------------------------
	if (em_VOICE_ERR_NON != inf.nKind)
	{	// �����r������
		if (GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind)) {
			//LOG(em_MSG), "[%s] �{�C�X�o�͑Ώ��r���� [%s][men=%d id=%d row= %d No=%d, len=%.3f, dsws=%d, t=%d, g=%d]",
			//	my_sThreadName, pCoil->cKizukenNo,
			//	inf.nMen, inf.nKind, row, inf.nKizuNo, inf.nLen / 1000.0, inf.nWidDiv, inf.nTid, inf.nGid );
			memcpy(&pCoil->alarme.voice[men][row], &inf, sizeof(inf));
		}

		//LOGG(em_MSG), "[%s] �x��o�͑Ώ�[�r������] [%s][men=%d, No=%d, len=%.3f, dsws=%d, t=%d, g=%d]", my_sThreadName, pCoil->cKizukenNo,
		//	inf.nMen, inf.nSeqNo, inf.nLen / 1000.0, inf.nWidDiv, inf.nTid, inf.nGid );
		LOGA(em_MSG), "[%s] �x��o�͑Ώ�[�r������] [%s][men=%d id=%d No=%d, len=%.3f, dsws=%d, t=%d, g=%d rank=%d]", my_sThreadName, pCoil->cKizukenNo,
			inf.nMen, inf.nKind, inf.nKizuNo, inf.nLen / 1000.0, inf.nWidDiv, inf.nTid, inf.nGid, rank );
	}
}

//==========================================
// �w��ŏ���Ԃ��r�����{�P���� [��HtRecv]
//------------------------------------------
// int	men				  : �\��(0:�\,1:��)
// const char* pKizukenNo : �Ǘ�No
// int	row				  : �w��ŏ���ԍs��
//==========================================
int CoilManager::SetDefectCount(int men, const char* pKizukenNo, int row)
{
	for (int ii = 0; ii < SIZE_COIL_BUF; ii++)
	{
		if (0 == memcmp(pKizukenNo, m_CoilInf[ii].cKizukenNo, sizeof(m_CoilInf[ii].cKizukenNo)))
		{
			m_CoilInf[ii].nDefCnt[men][row]++;
#ifdef DEF_DETECT_LOG
			LOG(em_MSG), "[%s] �r���X�V �Ǘ�No=%s ��=%d �s=%d �r��=%d->%d", my_sThreadName, m_CoilInf[ii].cKizukenNo, men, row, m_CoilInf[ii].nDefCnt[men][row] - 1, m_CoilInf[ii].nDefCnt[men][row]);
#endif
			return m_CoilInf[ii].nDefCnt[men][row];
		}
	}
	LOG(em_WAR), "[%s] �r���X�V�i�ΏۃR�C�������j�Ǘ�No=%s ��=%d �s=%d", my_sThreadName, pKizukenNo, men, row);
	return -1;
}

//==========================================
// �w��ŏ���Ԃ��r�����擾���� [��HtRecv]
//------------------------------------------
// int	men				  : �\��(0:�\,1:��)
// const char* pKizukenNo : �Ǘ�No
// int	row				  : �w��ŏ���ԍs��
//==========================================
int CoilManager::GetDefectCount(int men, const char* pKizukenNo, int row)
{
	for (int ii = 0; ii < SIZE_COIL_BUF; ii++)
	{
		if (0 == memcmp(pKizukenNo, m_CoilInf[ii].cKizukenNo, sizeof(m_CoilInf[ii].cKizukenNo)))
		{
#ifdef DEF_DETECT_LOG
			LOG(em_MSG), "[%s] �r���擾 �Ǘ�No=%s ��=%d �s=%d �r��=%d->%d", my_sThreadName, m_CoilInf[ii].cKizukenNo, men, row, m_CoilInf[ii].nDefCnt[men][row] - 1, m_CoilInf[ii].nDefCnt[men][row]);
#endif
			return m_CoilInf[ii].nDefCnt[men][row];
		}
	}
	LOG(em_WAR), "[%s] �r���擾�i�ΏۃR�C�������j�Ǘ�No=%s ��=%d �s=%d", my_sThreadName, pKizukenNo, men, row);
	return 0;
}
