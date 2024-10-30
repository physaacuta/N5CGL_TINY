#include "StdAfx.h"
#include "EdgePosControl.h"


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
EdgePosControl::EdgePosControl(void) :
m_bOldMode(false),
m_bFrameSkipAddDB(false),
FuncBase("EdgePosCont")
{
	Init();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
EdgePosControl::~EdgePosControl(void)
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ʏ��
//------------------------------------------
// �G�b�W���o���[�h ����
//------------------------------------------
CString EdgePosControl::GetDivEdgeMode(EM_DIV_EDGE_MODE em) 
{
	CString wk;
	if		(DIV_EDGE_MODE_NON == em)			{ wk = "�������";}
	else if (DIV_EDGE_MODE_BACK == em)			{ wk = "�ޯ�����";}
	else if (DIV_EDGE_MODE_SKIP == em)			{ wk = "�X�L�b�v";}
	else if (DIV_EDGE_MODE_OK == em)			{ wk = "����";}
	else if (DIV_EDGE_MODE_ONE == em)			{ wk = "�ЃG�b�W�D��";}
	else if (DIV_EDGE_MODE_CENTER == em)		{ wk = "ײݾ����U��";}
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", em);}
	return wk;
}

//------------------------------------------
// �G�b�W���o�ʒu���� (�X�L����������DS��WS)
// int ou �\��
// float* dEdgePos �������J�������̃G�b�W�ʒu�z��
// int& edgeL �G�b�W�z��ʒu���i�摜�����jDS
// int& edgeR �G�b�W�z��ʒu�E�i�摜�E���jWS
//------------------------------------------
void EdgePosControl::SearchEdgeIndex(int ou, float* dEdgePos, int& edgeL, int& edgeR) 
{
	// �E�P��ɗ��G�b�W�͖��Ή�
	// �E��������O���Ɍ���
	int nHalfNum = NUM_CAM_POS / 2;
	//int nMod = NUM_CAM_POS % 2;

	//// �G�b�W���o DS��
	edgeL = -1;
	for(int ii=nHalfNum-1; ii>=0; ii--) {
		if( 0 != dEdgePos[ii] ) {
			edgeL = ii; 
			break;
		}
	}
	//// �G�b�W���o WS�� = �E��
	edgeR = -1;
	for(int ii=nHalfNum; ii<NUM_CAM_POS; ii++) {
		if( 0 != dEdgePos[ii] ) {
			edgeR = ii; 
			break;
		}
	}
}
void EdgePosControl::SearchEdgeIndex(int ou, int& edgeL, int& edgeR) 
{
	// ���߂̃f�[�^���g�p
	SearchEdgeIndex(ou, m_EdgeInf[ou].dEdgePos, edgeL, edgeR);
}
//------------------------------------------
// �G�b�W���o�ʒu���� (�X�L����������DS��WS)
// int ou �\��
// float& edgeL �G�b�W�ʒu���i�摜�����jDS
// float& edgeR �G�b�W�ʒu�E�i�摜�E���jWS
//------------------------------------------
void EdgePosControl::SearchEdgePos(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo) 
{
	// �������`�F�b�N��� �ŏI�G�b�W�ʒu��ԋp
	edgeL = m_EdgeInf[ou].dEdgeAnsPos[0];
	edgeR = m_EdgeInf[ou].dEdgeAnsPos[1];

	if(NULL != pFrameNo) {
		*pFrameNo = m_EdgeInf[ou].nFrameNo;
	}

}

//------------------------------------------
// ���G�b�W���o�ʒu���� (�X�L����������DS��WS)
// int ou �\��
// float& edgeL �G�b�W�ʒu���i�摜�����jDS
// float& edgeR �G�b�W�ʒu�E�i�摜�E���jWS
//------------------------------------------
void EdgePosControl::SearchEdgeRawPos(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo) 
{
	// �������`�F�b�N��� �ŏI�G�b�W�ʒu��ԋp
	edgeL = m_EdgeInf[ou].dEdgeRawPos[0];
	edgeR = m_EdgeInf[ou].dEdgeRawPos[1];

	if(NULL != pFrameNo) {
		*pFrameNo = m_EdgeInf[ou].nFrameNo;
	}

}



//------------------------------------------
// ���G�b�W�ő匟�o�ʒu���� (�X�L����������DS��WS)
// int ou �\��
// float& edgeL �G�b�W�ʒu���i�摜�����jDS
// float& edgeR �G�b�W�ʒu�E�i�摜�E���jWS
//------------------------------------------
void EdgePosControl::SearchEdgeRawPosMax(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo) 
{
	// �������`�F�b�N��� �ŏI�G�b�W�ʒu��ԋp
	edgeL = m_EdgeInf[ou].dEdgeRawPosMax[0];
	edgeR = m_EdgeInf[ou].dEdgeRawPosMax[1];

	if(NULL != pFrameNo) {
		*pFrameNo = m_EdgeInf[ou].nFrameNo;
	}

}

//------------------------------------------
// ���G�b�W�ŏ����o�ʒu���� (�X�L����������DS��WS)
// int ou �\��
// float& edgeL �G�b�W�ʒu���i�摜�����jDS
// float& edgeR �G�b�W�ʒu�E�i�摜�E���jWS
//------------------------------------------
void EdgePosControl::SearchEdgeRawPosMin(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo) 
{
	// �������`�F�b�N��� �ŏI�G�b�W�ʒu��ԋp
	edgeL = m_EdgeInf[ou].dEdgeRawPosMin[0];
	edgeR = m_EdgeInf[ou].dEdgeRawPosMin[1];

	if(NULL != pFrameNo) {
		*pFrameNo = m_EdgeInf[ou].nFrameNo;
	}

}

//------------------------------------------
// �T�[�`�͈� (���o�͈́A���J�o���[���[�h���o�͈͂ŎZ�o�\)
// int nLR 0:���G�b�W�� 1:�E�G�b�W��
// float dCoilWidth �R�C����
// float dDetectRange ���o�͈� [mm]
// float* dPos �T�[�`�͈� (�T�[�`�͈͂̍�������,�T�[�`�͈͂̉E������)
//------------------------------------------
void EdgePosControl::GetSearchPos(int nLR, float dCoilWidth, float dDetectRange, float* dPos)
{
	float dRange	= (float)(dDetectRange * 0.5);	// �͈�

	if(COIL_WID_MAX <= dCoilWidth || 0 >= dCoilWidth) {
		dPos[0]	= 0.0;
		dPos[1]	= 0.0;
	} else {
		if(0==nLR) {
			dPos[0]	= (float)(dCoilWidth*0.5*-1)	- dRange;
			dPos[1]	= (float)(dCoilWidth*0.5*-1)	+ dRange;
		} else {
			dPos[0]	= (float)(dCoilWidth*0.5)		- dRange;
			dPos[1]	= (float)(dCoilWidth*0.5)		+ dRange;
		}
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���C������

//------------------------------------------
// ������
//------------------------------------------
void EdgePosControl::Init()
{
	// �������[�h���������������
	memset( m_EdgeInf,	0x00, sizeof(m_EdgeInf));
	memset( m_Zi, 0x00, sizeof(m_Zi));
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_EdgeInf[ii].emEdgeMode = DIV_EDGE_MODE_NON;
	}

	//// ���̑�����
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_nEdgeSearchSkip[ii]		= 0;
		m_nFrameSaveInterval[ii]	= 0;
	}
}

//------------------------------------------
// �����I�ȃR�C���ؑ� [��MI]
// int ou �\��
//------------------------------------------
void EdgePosControl::CoilChange(int ou)
{
	//// �s���ъJ�n�ݒ� (�����I�ȃR�C���ؑ� ����A �_���I�ȃR�C���ؑ�+�n�ڌ�s���� �܂�)
	// �G���W�����ł́A�R�C���؂�ւ��ŁA����ɃT�[�`�͈͂����Z�b�g����邽��
	m_nEdgeSearchSkip[ou] = -1;			// �_���I�ȃR�C���ؑւōēx�����Ɛݒ肷�邽�߁A�����ł́A�����R�C���ؑց`�_���I�ȃR�C���ؑւ̊ԁA�X�L�b�v�ł���悤�ɂ��Ă���
	LOG(em_MSG), "[%s] <%s> �����I�ȃR�C���ؑ֔F��", my_sThreadName, DivNameManager::GetTorB(ou));
}

//------------------------------------------
// �_���I�ȃR�C���؂�ւ�����ʔ��Z�b�g [��HtRecv]
// int ou �\��
// char const* cKizukenNo �Ǘ�No
// float coilWid ��ʔ� [mm] �����s���̏ꍇ�́A�ő��
// bool bIsInit ��������true
//------------------------------------------
void EdgePosControl::CoilStart(int ou, char const* cKizukenNo, float coilWid, bool bIsInit)
{
	//// ������
	memset( &m_Zi[ou], 0x00, sizeof(ZI_COIL_RESULT));

	//// �f�[�^�Z�b�g
	memcpy( m_Zi[ou].cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
	m_Zi[ou].dCoilWidMin	= COIL_WID_MAX;							// �ŏ��ɂ́A�Ƃ肠�����ő�����Z�b�g���Ă���

	// �L�����`�F�b�N
	if(COIL_WID_MAX <= coilWid || 0 >= coilWid) {
		m_Zi[ou].dCoilWidth = 0.0;
		m_Zi[ou].bCoilWidth = false;
	} else {
		m_Zi[ou].dCoilWidth = coilWid;
		m_Zi[ou].bCoilWidth = true;
	}

//// ��
//m_Zi[0].dCoilWidth = 1300;
//m_Zi[0].bCoilWidth = true;

	//// �R�C���ؑ֎��̂�
	if( ! bIsInit ) {
		// �s���т̐ݒ�
		m_nEdgeSearchSkip[ou] = mcls_pParam->GetCommonMast().Edge.nFromSkip; 
		if(0 != m_nEdgeSearchSkip[ou]) m_nEdgeSearchSkip[ou] += 1;		// �����Ɉ�������邽�߁A��񕪉��Z���Ă���

		LOG(em_MSG), "[%s] <%s> �_���I�ȃR�C���ؑ֔F�� [��=%dmm] [�Ǘ�No: %s]", my_sThreadName, DivNameManager::GetTorB(ou), (int)m_Zi[ou].dCoilWidth, cKizukenNo);
	}
}

//------------------------------------------
// �R�C�����ђ��� [��HtRecv]
// int ou �\��
//------------------------------------------
void EdgePosControl::CoilEnd(int ou)
{
	if(0==strlen(m_Zi[ou].cKizukenNo)) return;

#ifdef EDGE_POS_CONTROL_DBSAVE
	//// ���O����
	DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
	pDeli->kubun = DBManager::em_EDGE_COIL_INF;
	pDeli->data = (BYTE*)new DBDATA_EDGE_COIL_INF;
	DBDATA_EDGE_COIL_INF* pWk = (DBDATA_EDGE_COIL_INF*)pDeli->data;
	memset(pWk, 0x00, sizeof(DBDATA_EDGE_COIL_INF));


	//// �f�[�^�Z�b�g
	memcpy( pWk->cKizukenNo, m_Zi[ou].cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->nMen				= ou;										// �\���敪 (0:�\ 1:��)
	pWk->dCoilWidMin		= m_Zi[ou].dCoilWidMin;						// ���ŏ�
	pWk->dCoilWidMax		= m_Zi[ou].dCoilWidMax;						// ���ő�

	pWk->nLimitEdgeErrNum	= m_Zi[ou].nLimitEdgeErrNum;				// �����e�덷�͈͊O��
	pWk->nLostEdgeNum[0]	= m_Zi[ou].nLostEdgeNum[0];					// �G�b�W�����o�� (���G�b�W/�E�G�b�W)
	pWk->nLostEdgeNum[1]	= m_Zi[ou].nLostEdgeNum[1];				


	// �t���[���X�L�b�v��
	if(m_bFrameSkipAddDB) {
		pWk->nFrameNumCoil		= m_Zi[ou].nFrameNumCoil;
		for(int ii=0; ii<NUM_CAM_POS; ii++)	{
			for(int jj=0; jj<5; jj++) {
				pWk->nFrameNumSkip[ii][jj] = m_Zi[ou].nFrameNumSkip[ii][jj]; 
			}
		}
	}

	//// �o�^�˗�
	if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 3) ) {
		// �L���[�t��
		delete pDeli->data;
		delete pDeli;

		// DB���ُ� (�ʏ킠�肦�Ȃ�)
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DB_EDGE_COIL_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[DB_EDGE_COIL_INF]");
	}
#endif

	//// ���d�o�^�h�~���u(�O�̂���)
	memset(&m_Zi[ou], 0x00, sizeof(ZI_COIL_RESULT));
}

//------------------------------------------
// �����󋵃Z�b�g
// int ou �\��
// int* nFrameSkip  �ЖʃJ�����Z�b�g���̏����X�L�b�v
// int* nEdgeNg		�ЖʃJ�����Z�b�g���̃G�b�W���o���s
// int* nFrameSts	�ЖʃJ�����Z�b�g���̒T����Ԓ�`
//------------------------------------------
void EdgePosControl::SetFrameSkip(int ou, int* nFrameSkip, int* nEdgeNg, int* nFrameSt)
{
	m_Zi[ou].nFrameNumCoil ++;
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		if(0 != nEdgeNg[ii])						m_Zi[ou].nFrameNumSkip[ii][0] ++;	// �G�b�W�����́A�摜������NG���ʌn�� 
		if(STATUS_PROCSKIP == nFrameSt[ii])			m_Zi[ou].nFrameNumSkip[ii][1] ++; 
		if(NCL_SMEMPIPE_SMEMFULL == nFrameSkip[ii]) m_Zi[ou].nFrameNumSkip[ii][2] ++; 
		if(STATUS_TOOMANY == nFrameSt[ii])			m_Zi[ou].nFrameNumSkip[ii][3] ++; 
		if(STATUS_QFULL == nFrameSt[ii])			m_Zi[ou].nFrameNumSkip[ii][4] ++; 
	}
}

//------------------------------------------
// �G�b�W���o�ʒu�Z�b�g
// int ou �\��
// long nFrameNo �t���[��No
// long nPos ����ʒu [mm]
// double dResY �c����\
// float* edge �\�����̃G�b�W�ʒu�z��|�C���^ (���A�E�̏��ԂɊi�[)
// float* edgemax �\�����̍ő�G�b�W�ʒu�z��|�C���^ (���A�E�̏��ԂɊi�[)
// float* edgemin �\�����̍ŏ��G�b�W�ʒu�z��|�C���^ (���A�E�̏��ԂɊi�[)
// double dSpmNobiHosei SPM�L�ѕ␳��
// �߂�l�F true:�G�b�W����   false:�G�b�W�����o�A�ЃG�b�W�A�Z���^�[�U�� �� �s���т��܂�
//------------------------------------------
bool EdgePosControl::SetEdgePos(int ou, long nFrameNo, long nPos, double dResY, float* edge, float* edgemax, float* edgemin, double dSpmNobiHosei)
{
	bool		bAns = true;				// �߂�l
	EDGE_INF	NewEdge;					// ����̐V�������
	float		dNewSearchPos[2][2];		// �ЃG�b�W�D�掞�̃T�[�`�͈� (��/�E)(��������͈�/�E������͈�) (0�Ő�������)
	float		dCoilWid[2];				// ��ʔ� [mm] (��, �E)
	float		dWk[2];
	bool		bWk;
	ParamManager::P_COMMON_MAST const& prm = mcls_pParam->GetCommonMast(); 
	int			nEdgeErrReason = EDGE_NORMAL;	// �G�b�W�s�����R�̘_���a


	//================================================
	// �O����
	// �O��l�������l�Ƃ��ăZ�b�g
	memcpy(&NewEdge, &m_EdgeInf[ou], sizeof(NewEdge));
	
	// ����̃G�b�W�ʒu���Z�b�g
	NewEdge.nFrameNo = nFrameNo;
	//memcpy(NewEdge.dEdgePos, edge, sizeof(NewEdge.dEdgePos));
	memcpy(NewEdge.dEdgeRawPos, edge, sizeof(NewEdge.dEdgeRawPos));
	memcpy(NewEdge.dEdgeRawPosMax, edgemax, sizeof(NewEdge.dEdgeRawPosMax));
	memcpy(NewEdge.dEdgeRawPosMin, edgemin, sizeof(NewEdge.dEdgeRawPosMin));
	for(int ii=0; ii<2; ii++) NewEdge.bEdgeEnable[ii] = false;
	if(0.0 > NewEdge.dEdgeRawPos[0]) NewEdge.bEdgeEnable[0] = true;
	if(0.0 < NewEdge.dEdgeRawPos[1]) NewEdge.bEdgeEnable[1] = true;

	// ���̃G�b�W�ʒu���Z�o
	for(int ii=0; ii<2; ii++) {
		if(!NewEdge.bEdgeEnable[ii]) {
			NewEdge.dEdgeRawPos[ii] = 0;
			bAns = false;
		}
		NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];	
	}

	//SearchEdgeIndex(ou, NewEdge.dEdgePos, nWk[0], nWk[1]);
	//for(int ii=0; ii<2; ii++) {
	//	if(-1 == nWk[ii]) {
	//		NewEdge.bEdgeEnable[ii] = false;
	//		NewEdge.dEdgeRawPos[ii] = 0;
	//		bAns = false;

	//	} else {
	//		NewEdge.bEdgeEnable[ii] = true;
	//		NewEdge.dEdgeRawPos[ii] = NewEdge.dEdgePos[ nWk[ii] ];
	//		m_nEdgeDetectCamSet[ou][ii]	= ou*NUM_CAM_POS + nWk[ii] + 1;	// ���o�J�����Z�b�g
	//	}
	//	NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];	
	//}

	//// ���낢�댸�Z
	if( 0 < m_nEdgeSearchSkip[ou] )		m_nEdgeSearchSkip[ou]	 -= 1;	// �X�L�b�v��
	if( 0 < m_nFrameSaveInterval[ou] )	m_nFrameSaveInterval[ou] -= 1;	// �摜�v����


	//// �����܂ł��A�ȑO�̂܂�
	if(0 == prm.Edge.nType) {
		NewEdge.bEdgeAns = false;		// �ŏI�G�b�W�ʒu �g�p�s��
		// �����H�ŁA����G�b�W�����̂܂܍ŏI�G�b�W�Ƃ��Ďg�p
		memcpy(&m_EdgeInf[ou], &NewEdge, sizeof(EDGE_INF));
		
		bAns = true;		// ����������Ȃ��̂��߁A����
		return bAns;
	}



	// ��
	//NewEdge.dEdgeRawPos[0] = -900;
	//NewEdge.dEdgeRawPos[1] = 900;

	//================================================
	// �G�b�W�ʒu����������
	//   �E�G�b�W���o���[�h�̌���
	//	 �E�ŏI�G�b�W�ʒu�̌���

	// �悭�g����������
	if( m_Zi[ou].bCoilWidth ) {
		dCoilWid[0] = (float)(m_Zi[ou].dCoilWidth*0.5) * -1;
		dCoilWid[1] = (float)(m_Zi[ou].dCoilWidth*0.5);
	}

	for(int ww=0; ww<1; ww++) {	
		//--------------------------
		// 0 �J�����������́A����
		if(m_bOldMode) {
			NewEdge.emEdgeMode				= DIV_EDGE_MODE_NON;		// �������ɉ������Ȃ�
			for(int ii=0; ii<2; ii++) {
				if( NewEdge.bEdgeEnable[ii] ) {
					NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];			// ����l���g�p
				} else {
					NewEdge.dEdgeAnsPos[ii]	= m_EdgeInf[ou].dEdgeAnsPos[ii];	// �O��l���g�p
				}
			}
			break;
		}


		//--------------------------
		// �@ ����H �܂��́A��ʃR�C�����̔�����
		//		�E�ŏ�����R�C����񂪂��郉�C���ł��Ή��\�ȗl�ɂ��Ă���
		if( ! m_Zi[ou].bCoilWidth ) {

			//// �ŏ�
			if( DIV_EDGE_MODE_NON == m_EdgeInf[ou].emEdgeMode ) {
				//// ����� �G�b�W���o������
				if(NewEdge.bEdgeEnable[0] && NewEdge.bEdgeEnable[1]) {
					NewEdge.emEdgeMode			= DIV_EDGE_MODE_BACK;		// �o�b�N�A�b�v�ɑJ��	
					for(int ii=0; ii<2; ii++) {
						NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];
					}

				//// ����ŁA�G�b�W�������o�B �������Ȃ��B
				} else {
					NewEdge.emEdgeMode			= DIV_EDGE_MODE_NON;
					for(int ii=0; ii<2; ii++) {
						if(NewEdge.bEdgeEnable[ii])	NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];
						else						NewEdge.dEdgeAnsPos[ii]	= 0.0;
					}
				}

			//// ���ł��G�b�W������������
			} else {
				NewEdge.emEdgeMode				= DIV_EDGE_MODE_BACK;		// �������Ƀo�b�N�A�b�v�ɑJ��
				for(int ii=0; ii<2; ii++) {
					if( NewEdge.bEdgeEnable[ii] ) {
						NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];			// ����l���g�p
					} else {
						NewEdge.dEdgeAnsPos[ii]	= m_EdgeInf[ou].dEdgeAnsPos[ii];	// �O��l���g�p
					}
				}
			}
			break;
		}
		
		//--------------------------
		// �A �X�L�b�v���H
		if( 0 != m_nEdgeSearchSkip[ou] ) {
			NewEdge.emEdgeMode					= DIV_EDGE_MODE_SKIP;

			for(int ii=0; ii<2; ii++) {
				if( NewEdge.bEdgeEnable[ii] ) {
					NewEdge.dEdgeAnsPos[ii]		= NewEdge.dEdgeRawPos[ii];			// ����l���g�p
				} else {
					NewEdge.dEdgeAnsPos[ii]		= m_EdgeInf[ou].dEdgeAnsPos[ii];	// �O��l���g�p
				}
			}

#ifdef DEBUG_LOG_EDGE
			LOG(em_WAR), "[%s] <%s> %d �G�b�W���o�X�L�b�v �Ǘ�No:%s �����ʒu:%d (%.3f/%.3f) OldEdgeMode:%d", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo,
				m_Zi[ou].cKizukenNo, nPos, NewEdge.dEdgeRawPos[0], NewEdge.dEdgeRawPos[1], m_EdgeInf[ou].emEdgeMode);
#endif
			break;
		}

		//--------------------------
		// �B ���o�͈͓��ɃG�b�W�ʒu�������Ƃ���H
		bWk = false;
		for(int ii=0; ii<2; ii++) {
			dWk[0] = abs( dCoilWid[ii] - NewEdge.dEdgeRawPos[ii] );
			if( ! NewEdge.bEdgeEnable[ii] ||
				dWk[0] > prm.Edge.dDetectRange*0.5 ) {
				
				NewEdge.emEdgeMode				= DIV_EDGE_MODE_ONE;		// ���Ȃ��Ƃ��ЃG�b�W�D��
				bWk = true;
				nEdgeErrReason |= ii == 0 ? EDGE_ERR_LEFT : EDGE_ERR_RIGHT;	// �G�b�W�s�����R
			}
		}
		if(bWk) {
			m_Zi[ou].nLimitEdgeErrNum += 1;
			if(DIV_EDGE_MODE_OK == m_EdgeInf[ou].emEdgeMode) {
#ifdef DEBUG_LOG_EDGE
				//��
				LOG(em_WAR), "[%s] <%s> %d �G�b�W���o�͈͊O (%.3f/%.3f)", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo, NewEdge.dEdgeRawPos[0], NewEdge.dEdgeRawPos[1]);
#endif
			}
			break;
		}

		//--------------------------
		// �C �����e�덷�͈͓��H
		dWk[0] = (abs(NewEdge.dEdgeRawPos[0])+abs(NewEdge.dEdgeRawPos[1])) ;	// ��
		if( dWk[0] < (m_Zi[ou].dCoilWidth-prm.Edge.dWidthDiff) || 
			dWk[0] > (m_Zi[ou].dCoilWidth+prm.Edge.dWidthDiff) ) {
			NewEdge.emEdgeMode					= DIV_EDGE_MODE_ONE;		// ���Ȃ��Ƃ��ЃG�b�W�D��
			m_Zi[ou].nLimitEdgeErrNum += 1;
			nEdgeErrReason |= EDGE_ERR_WIDTH;								// �G�b�W�s�����R

			if(DIV_EDGE_MODE_OK == m_EdgeInf[ou].emEdgeMode) {
#ifdef DEBUG_LOG_EDGE
				//��
				LOG(em_WAR), "[%s] <%s> %d �����e�͈͊O=%.3fmm (��ʔ�=%.3f)", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo, dWk[0], m_Zi[ou].dCoilWidth);
#endif
			}
			break;
		}

		//--------------------------
		// �D �����܂ŗ�����A����
		NewEdge.emEdgeMode						= DIV_EDGE_MODE_OK;
		for(int ii=0; ii<2; ii++) {
			NewEdge.dEdgeAnsPos[ii]				= NewEdge.dEdgeRawPos[ii];		// ���񌟏o�̃G�b�W�ʒu�����̂܂܎g�p
			
			dWk[0] = (abs(NewEdge.dEdgeRawPos[0])+abs(NewEdge.dEdgeRawPos[1]));		// ��
			if(m_Zi[ou].dCoilWidMin > dWk[0]) m_Zi[ou].dCoilWidMin = dWk[0];
			if(m_Zi[ou].dCoilWidMax < dWk[0]) m_Zi[ou].dCoilWidMax = dWk[0];
		}
		break;
	}


	//================================================
	// �o�b�N�A�b�v���[�h�̌���
	if(DIV_EDGE_MODE_ONE == NewEdge.emEdgeMode) {
		// ���̎��_�ŁA�G�b�W�s���� �m��
		bAns = false;

		// �O�t���[���Ƃ̌덷
		dWk[0] = abs( m_EdgeInf[ou].dEdgeAnsPos[0] - NewEdge.dEdgeRawPos[0] );		// ��
		dWk[1] = abs( m_EdgeInf[ou].dEdgeAnsPos[1] - NewEdge.dEdgeRawPos[1] );		// �E

#ifdef DEBUG_LOG_EDGE
		LOG(em_WAR), "[%s] <%s> %d, �O�ڰтƂ̌덷 ��(%.3f<%.3f) �E(%.3f<%.3f)", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo, 
			dWk[0], prm.Edge.dPerEdgeDiff*0.5, dWk[1], prm.Edge.dPerEdgeDiff*0.5 ); // ��
#endif


		//--------------------------
		// �E�ЃG�b�W�D�惂�[�h	
		if( DIV_EDGE_MODE_SKIP != m_EdgeInf[ou].emEdgeMode &&
			( ( NewEdge.bEdgeEnable[0] && dWk[0] <= prm.Edge.dPerEdgeDiff*0.5 ) || 
			  ( NewEdge.bEdgeEnable[1] && dWk[1] <= prm.Edge.dPerEdgeDiff*0.5 ) ) ) {

			//// ��G�b�W��������
			int nBase;						// ����̃G�b�WIndex
			int	nOther;						// �������̃G�b�WIndex
			if( dWk[0] <= dWk[1] )	{ nBase = 0; nOther = 1; } 
			else					{ nBase	= 1; nOther	= 0; }
			

			//// �T�[�`�͈�
			// ���
			dNewSearchPos[nBase][0]		= NewEdge.dEdgeRawPos[nBase] - (float)(prm.Edge.dDetectRange*0.5);	// �������̃T�[�`�͈�
			dNewSearchPos[nBase][1]		= NewEdge.dEdgeRawPos[nBase] + (float)(prm.Edge.dDetectRange*0.5);	// �E�����̃T�[�`�͈�
			// ������
			float	dOtherEdgePos		= NewEdge.dEdgeRawPos[nBase] + (0==nBase ? m_Zi[ou].dCoilWidth : -m_Zi[ou].dCoilWidth);		// �����̃G�b�W�ʒu
			dNewSearchPos[nOther][0]	= dOtherEdgePos - (float)(prm.Edge.dDetectRangeR*0.5);				// �������̃T�[�`�͈�
			dNewSearchPos[nOther][1]	= dOtherEdgePos + (float)(prm.Edge.dDetectRangeR*0.5);				// �E�����̃T�[�`�͈�


			//// ����̃G�b�W�ʒu������
			NewEdge.emEdgeMode				= DIV_EDGE_MODE_ONE;
			NewEdge.dEdgeAnsPos[nBase]		= NewEdge.dEdgeRawPos[nBase];
			if( 0 == nOther	)	NewEdge.dEdgeAnsPos[nOther]	= dOtherEdgePos + (float)(prm.Edge.dDetectRange*0.2);
			else 				NewEdge.dEdgeAnsPos[nOther]	= dOtherEdgePos - (float)(prm.Edge.dDetectRange*0.2);

		//--------------------------
		// �E���C���Z���^�[�U�蕪�����[�h
		} else {
			//// ����̃G�b�W�ʒu������
			NewEdge.emEdgeMode				= DIV_EDGE_MODE_CENTER;
			GetSearchPos(0, m_Zi[ou].dCoilWidth, (float)(prm.Edge.dDetectRangeR*0.8), dWk);
			NewEdge.dEdgeAnsPos[0]			= dWk[1];		// ���́A��
			GetSearchPos(1, m_Zi[ou].dCoilWidth, (float)(prm.Edge.dDetectRangeR*0.8), dWk);
			NewEdge.dEdgeAnsPos[1]			= dWk[0];		// ���́A�E
			nEdgeErrReason |= EDGE_ERR_PREFRAME;			// �G�b�W�s�����R
		}

	}

#ifdef DEBUG_LOG_EDGE
//	
	LOG(em_MSG), "[%s] <%s> %d, ���G�b�W(%d/%d)", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo, (int)NewEdge.dEdgeRawPos[0], (int)NewEdge.dEdgeRawPos[1] ); // ��
#endif

	//================================================
	// ���[�h�ʏ���
	bool bDbAdd = false;							// ����̏�Ԃ�DB�ɓo�^
	//////bool bSearchChange = false;						// �T�[�`�͈͕ύX(�������ς�邶��Ȃ��āA����ɕύX�ʒm�𑗂�t���O)


	//// ���[�h�ω��ɂ�鏈������H
	if( m_EdgeInf[ou].emEdgeMode != NewEdge.emEdgeMode ) {
		if( DIV_EDGE_MODE_NON != NewEdge.emEdgeMode ) bDbAdd = true;
	}

	// �ŏI�G�b�W�ʒu �g�p��
	if( DIV_EDGE_MODE_OK == NewEdge.emEdgeMode ||
	    DIV_EDGE_MODE_ONE == NewEdge.emEdgeMode ||
		DIV_EDGE_MODE_CENTER == NewEdge.emEdgeMode) {
		NewEdge.bEdgeAns = true;		// �ŏI�G�b�W�ʒu �g�p��
	} else {
		NewEdge.bEdgeAns = false;		// �ŏI�G�b�W�ʒu �g�p�s��
	}

	//// �G�b�W�����o���Z�b�g
	if( DIV_EDGE_MODE_SKIP != NewEdge.emEdgeMode ) {
		for(int ii=0; ii<2; ii++) {
			if( ! NewEdge.bEdgeEnable[ii])	m_Zi[ou].nLostEdgeNum[ii] += 1;					// �G�b�W�����o��		
		}
	}

	//// ����̌��ʂ𔽉f
	memcpy(&m_EdgeInf[ou], &NewEdge, sizeof(EDGE_INF));



	//================================================
	// �O���A���N���X �����
#ifdef EDGE_POS_CONTROL_DBSAVE
	//------------------------
	// DB�� �G�b�W���o���[�h�ω��Ǘ��e�[�u�� ��o�^
	if(bDbAdd) {
#ifdef DEBUG_LOG_EDGE
			LOG(em_MSG), "[%s] <%s> %d, �G�b�W���o���[�h�ω��o�^ �Ǘ�No:%s �����ʒu:%d ���o�敪:%d", my_sThreadName, DivNameManager::GetTorB(ou),
				nFrameNo, m_Zi[ou].cKizukenNo, nPos, NewEdge.emEdgeMode);
#endif
			//// ���O����
			DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
			pDeli->kubun = DBManager::em_EDGE_CHANGE_INF;
			pDeli->data = (BYTE*)new DBDATA_EDGE_CHANGE_INF;
			DBDATA_EDGE_CHANGE_INF* pWk = (DBDATA_EDGE_CHANGE_INF*)pDeli->data;
			memset(pWk, 0x00, sizeof(DBDATA_EDGE_CHANGE_INF));


			//// �f�[�^�Z�b�g
			memcpy( pWk->cKizukenNo, m_Zi[ou].cKizukenNo, SIZE_KIZUKEN_NO);
			pWk->nPos = nPos;								// �����ʒu [mm]
			//GetSpmLen(KBN_KEN, DIV_SPM_TRUE, nPos, dSpmNobiHosei, pWk->nPos);// �����ʒu [mm]
			pWk->nMen			= ou;						// �\���敪 (0:�\ 1:��)
			pWk->nFrameNo		= nFrameNo;					// �t���[��No
			pWk->nEdgeMode		= NewEdge.emEdgeMode;		// ���o�敪
			pWk->dEdgeRawPos[0]	= NewEdge.dEdgeRawPos[0];	// ���G�b�W�ʒu [mm]
			pWk->dEdgeRawPos[1]	= NewEdge.dEdgeRawPos[1]; 		
			pWk->dEdgeAnsPos[0]	= NewEdge.dEdgeAnsPos[0];	// �ŏI�G�b�W�ʒu [mm]
			pWk->dEdgeAnsPos[1]	= NewEdge.dEdgeAnsPos[1]; 
			pWk->nEdgeErrReason = nEdgeErrReason;			// �G�b�W�s�����R�̘_���a


			//// �o�^�˗�
			if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 10) ) {
				// �L���[�t��
				delete pDeli->data;
				delete pDeli;

				// DB���ُ� (�ʏ킠�肦�Ȃ�)
				LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_DEFECT_INF]", my_sThreadName);
				syslog(SYSNO_QUEFULL_WAR, "[DBDATA_DEFECT_INF]");
				// �����ł̃L���[�t���ł́A��~�����Ȃ��B
		}
	}

#endif

	return bAns;
}