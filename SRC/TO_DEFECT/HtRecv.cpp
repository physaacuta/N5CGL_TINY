#include "StdAfx.h"
#include "HtRecv.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInstance.h"




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �悭�g���}�N��
#ifdef LOG_DEF_OUTPUT
	#define LOGDEF(t)	if(mcls_pLogDef!=NULL)	mcls_pLogDef->Write(KizuLib::LogFileManager::t		// ���O�o��
#endif

#define KIZUFILE(t)  if(mcls_pKizuFile!=NULL)		mcls_pKizuFile->Write(t									// �r���t�@�C���o��

// offset�́A HT_TO_DATA �̃I�t�Z�b�g
#define GET_PTR_HEADER( cam, header_offset)			my_cRecvData[cam][ header_offset ]
#define GET_PTR_RESULT( cam, result_offset, num )	my_cRecvData[cam][ result_offset + (num*sizeof(NCL_DEFECT_RESULT)) ]
#define GET_PTR_ATTR( cam, attr_offset, num)		my_cRecvData[cam][ attr_offset + (num*sizeof(double)*(MAX_ATTR_BASE+(MAX_ATTR_ANGLE*MAX_IMAGE_CNT))) ]


#ifdef DEBUG_LOG_EDGE_UDP
#define LOGUDP(t)	if(mcls_pLogUdp!=NULL)	mcls_pLogUdp->Write(KizuLib::LogFileManager::t		// ���O�o��
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���C��

//==========================================
// �R���X�g���N�^
//------------------------------------------
// const char *	cSession		: �Z�b�V����������
// int			TorB			: 0:TOP 1:BOT	
// bool			bDistPosition	: ��ʂ̏ꍇtrue	
//==========================================
HtRecv::HtRecv(const char *cSession, int TorB, bool bDistPosition) : 
HtRecvBaseManager(cSession, TorB, bDistPosition, NUM_HANTEI, NUM_CAM_POS, DIV_HANTEI01),
mcls_pCoil(NULL),
#ifndef DISABLE_GAP_CALC
mcls_pGap(NULL),
#endif
#ifdef LOG_DEF_OUTPUT
  mcls_pLogDef(NULL),
#endif
#ifdef DEBUG_LOG_EDGE_UDP
  mcls_pLogUdp(NULL),
#endif
mcls_pDefDump(NULL),
mque_pAddSQLData(NULL)
{
	//===========================================================
	//// �����o�ϐ���������
	//////memset( m_nAGCSetFrameNo, 0x00, sizeof(m_nAGCSetFrameNo));
	//////memset( m_nAGCLastCommand, 0x00, sizeof(m_nAGCLastCommand));
	//////memset( m_nAGCBrCenter, 0x00, sizeof(m_nAGCBrCenter));
	//////m_bEnableAGC = true;
	memset( m_nBr, 0x00, sizeof(m_nBr));

	m_nNowGridTrandRow = 0;


	//===========================================================
	//// �@��ID�Z�b�g
	CString sWk;
	sWk.Format("FRAME_SKIP_%d_ID", TorB+1);
	this->SetFrameSkipId( GetPrivateProfileInt(INI_STATUS, sWk, 0, TASKINI_NAME) );
	KizuFunction::SetStatus(sWk, true, false);		// ����ɖ߂�

	sWk.Format("EDGE_NG_%d_ID", TorB+1);
	this->SetEdgeNgId( GetPrivateProfileInt(INI_STATUS, sWk, 0, TASKINI_NAME) );
	KizuFunction::SetStatus(sWk, true, false);		// ����ɖ߂�
	
	sWk.Format("FRAME_UNMATCH_%d_ID", TorB + 1);
	this->SetnFrameUnmatchId(GetPrivateProfileInt(INI_STATUS, sWk, 0, TASKINI_NAME));
	KizuFunction::SetStatus(sWk, true, false);		// ����ɖ߂�

	// �n�ړ_
	m_nPantiTypeId = GetPrivateProfileInt(TO_DEFECT, "PANCH_TYPE_ID", 0, TASKINI_NAME);

#ifdef LOG_DEF_OUTPUT
	//===========================================================
	// �f�o�b�N���O
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	CString sName;
	sName.Format("Def_%s", DivNameManager::GetTorB(TorB) );
	//mcls_pLogDef = new LogFileManager(sName, NULL, 20, 2);	// �������R�����g�����邾���Ń��O�͏o�Ȃ��B
	mcls_pLogDef = new LogFileManager(sName, NULL, 100, 2);	// �������R�����g�����邾���Ń��O�͏o�Ȃ��B
	//LOGDEF(em_NON), "���t,�Ǘ�No,�r�A��,tid,gid,�ŏI�ۑ�,�ۑ��Ώ�,���Q,�ő�,�}�[�W,����ʒu,SQLQUE,DBQUE,BCPQUE,���Q�폜,�L�Q�폜,�rؽ�FLG,���ײ�FLG");
	//LOGDEF(em_NON), "���t,�Ǘ�No,�r�A��,tid,gid,�ŏI�ۑ�,�ۑ��Ώ�,���Q,�ő�,�}�[�W,����ʒu,DS,WS,FRM,CAM,LABEL,SQLQUE,DBQUE,BCPQUE,���Q�폜,�L�Q�폜,�rؽ�FLG,���ײ�FLG");
	// 20210312
	LOGDEF(em_NON), "���t,�Ǘ�No,�r�A��,tid,gid,�ŏI�ۑ�,�ۑ��Ώ�,���Q,�ő�,�}�[�W,����ʒu,DS,WS,FRM,CAM,LABEL,SQLQUE,DBQUE,BCPQUE,���Q�폜,�L�Q�폜,�rؽ�FLG,���ײ�FLG,�r�ۑ������");

#endif

#ifdef DEBUG_LOG_EDGE_UDP
	//===========================================================
	// �f�o�b�N���O
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	CString sName2;
	sName2.Format("EdgeUdp_%s", DivNameManager::GetTorB(TorB));
	mcls_pLogUdp = new LogFileManager(sName2, NULL, 20, 2);	// �������R�����g�����邾���Ń��O�͏o�Ȃ��B
	LOGUDP(em_NON), ", �\��, �Ǘ�No, fno, ���苗��, ���G�b�W��, ���G�b�W�E, �����G�b�W��, �����G�b�W�E, �s�M���t���O, �`���T�C�Y ");
#endif

	// �\���Ǘ����M�p�r�o�b�t�@�N���A
	memset(&m_OpSendDefInfo, 0x00, sizeof(m_OpSendDefInfo));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
HtRecv::~HtRecv(void)
{
#ifdef LOG_DEF_OUTPUT
	SAFE_DELETE(mcls_pLogDef);
#endif
#ifdef DEBUG_LOG_EDGE_UDP
	SAFE_DELETE(mcls_pLogUdp);
#endif

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���z�֐�

//==========================================
// �����J�n�҂��J�n����
//==========================================
void HtRecv::OnExecDetectWaitStart()
{
	////// ���̃^�C�~���O�ł́A����ʒu���O��l�̂��߁A�O���b�h�ʒu�������ʒu�ɂȂ�Ȃ�
	//m_nNowGridRow = (long)(mcls_pPlg->GetLen_Ken( my_nTorB ) / (MAP_REPRES_MIN*1000));

	//// ��\�r�o�b�t�@ (���̎��_�Ŏc���Ă��鎖�͖����͂�)
	ReSetOp_DetectData(0);

	//// �f�[�^������
	//////memset( m_nAGCSetFrameNo, 0x00, sizeof(m_nAGCSetFrameNo));
	//////memset( m_nAGCLastCommand, 0x00, sizeof(m_nAGCLastCommand));
	//////memset( m_nAGCBrCenter, 0x00, sizeof(m_nAGCBrCenter));
	memset( m_nBr, 0x00, sizeof(m_nBr));
	m_nNowGridTrandRow = 0;

	mcls_pDefDump->FileNoClear();	// �r���t�@�C��������

#ifndef DISABLE_TINY_CALC
	//// �������חp�N���X������
	mcls_pTiny->Init(true);
#endif
}

//==========================================
// �����J�n����
//==========================================
void HtRecv::OnExecDetectRun()
{

	//// �����J�n���ɁA��O�̕������ُ�Ƃ��Ȃ�΍�
	////// ����ʒu�������J�n���ɃZ�b�g���Ă��邽�߁A���̃^�C�~���O�ŃO���b�h�ʒu���Z�o
	m_nPerFramePos = (int)mcls_pPlg->GetLen_Ken( my_nTorB );
	m_nNowGridRow = (long)(mcls_pPlg->GetLen_Ken( my_nTorB ) / (MAP_REPRES_MIN*1000));

	m_nNowOutGridRow = m_nNowGridRow;
	m_dNowOutLength  = m_nNowGridRow;

	//// �J�n���Ɉꔭ�s���𑗂�B (�����ƕ\���̏���rec���X�V����Ȃ�����)
	OnOpNewConnect();


	// ���ʌ����̏ꍇ�́A�����̖ʂ̂�
	// �Жʌ����̏ꍇ�́A����
	int nMenState   = mcls_pStatus->GetMenState();				// �����ʏ�� (-1:���ʌ��� 0:�\�݂̂Ŏ��{ 1:���݂̂Ŏ��{)
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( (-1==nMenState && ii==my_nTorB) ||
			(-1!=nMenState) ) {

			//// �G�b�W���o����������
			mcls_pEdgePosCont->CoilStart(ii, mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo, (float)mcls_pCoil->GetCoilKenV(my_nTorB)->data.nWid, true);
		}
	}

	// �r���t�@�C���ؑց@�w�b�_�[���t�^
	int nCoilRec = mcls_pCoil->GetRecKizukenNo(mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo);		// ���݃��R�[�h�ʒu
	mcls_pDefDump->Write(nCoilRec, DEF_DUMP_HEAD);												// �r���t�@�C���w�b�_�[��


}

//==========================================
// ������~����
//==========================================
void HtRecv::OnExecDetectStop()
{
	//// �G�b�W���o��������~
	mcls_pEdgePosCont->CoilEnd(my_nTorB);

#ifndef DISABLE_TINY_CALC
	//// �������׊֌W���ђ��� (�O�񂪓����Ă����ꍇ�̂�)
	if( mcls_pStatus->IsRun() )	{
		mcls_pTiny->CoilEnd(mcls_pCoil->GetCoilKenV(my_nTorB), NULL);
	}
#endif

	mcls_pDefDump->FileNoClear();		// �r���t�@�C��������

	// ��\�r�o�b�t�@�N���A
	ReSetOp_DetectData(0);
}

//==========================================
// �V�K�\���@�\�V�K�ڑ�
//==========================================
void HtRecv::OnOpNewConnect()
{
	if( ! this->my_bRun ) return;

	// �����܂��Ă��� �s����A�s���𑗐M
	OpSendFunc::SendOp_ColRowData(
					my_nTorB,
					mcls_pCoil->GetCoilKenV(my_nTorB),
					m_nNowGridRow,
					m_OpSendDefInfo.ColnowBuf_Now.BufDefCol,
					m_OpSendDefInfo.ColnowBuf_Now.BufDefAry,
					mcls_pCoil->GetCoilKenV(my_nTorB)->nDefCnt[my_nTorB][m_nNowGridRow]);
	// ��n��
	ClearDefectBuffer();
}

//==========================================
// �Ɩ��`�� ��M��̃��C������
//==========================================
void HtRecv::OnMainDefect()
{
	//==================================================================
	// �������擾
	HT_TO_HEAD*	pHth;
	HT_TO_HEAD& hth = (HT_TO_HEAD&)this->my_cRecvHead[0];			// �Ƃ肠���� �擪�̃w�b�_�[���擾
	HT_TO_DATA& htd = (HT_TO_DATA&)this->my_cRecvData[0][0];		// �Ƃ肠���� �擪�̃f�[�^���擾
	int nMenMode	= mcls_pStatus->GetMenState();					// �����ʏ��(-1:���ʌ��� 0:�\�݂̂Ō��� 1:���݂̂Ō���)


	//==================================================================
	// �摜�捞�`���聨�����`�������܂ł̕��ρA�ő及�v����[ms]���Z�o
	__int64 pc_pre, pc_now;
	//memcpy(&pc_pre, &hth.toVal[TOVAL_REC_PROCTIME], sizeof(__int64));
	memcpy(&pc_pre, &m_trFrm.nRecProcTime, sizeof(__int64));
	QueryPerformanceCounter((LARGE_INTEGER*)&pc_now);
	m_dHtProcNowTime = (double)(pc_now - pc_pre) / m_pf * 1000.0;		// [s] �� [ms]
	m_dHtProcAveTime = m_dHtProcAveTime*0.8 + m_dHtProcNowTime*0.2;
	if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
		if( m_dHtProcMaxTime < m_dHtProcNowTime ) {
			m_dHtProcMaxTime = m_dHtProcNowTime;
		}
	}


	//==================================================================
	// �e����PC�����r����M���A�ŏ��̎�M�`1�t���[�������܂ł̕��ρA�ő及�v����[ms]���Z�o
	//
	m_dRecvProcTotalNowTime = (double)(pc_now - m_pc_RecvFirst) / m_pf * 1000.0;		// [s] �� [ms]
	m_dRecvProcTotalAveTime = m_dRecvProcTotalAveTime*0.8 + m_dRecvProcTotalNowTime*0.2;
	if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
		if( m_dRecvProcTotalMaxTime < m_dRecvProcTotalNowTime ) {
			m_dRecvProcTotalMaxTime = m_dRecvProcTotalNowTime;
		}
	}



	//==================================================================
	// �ؑ֔c�� (�ЖʑS���ɑ����Ă���ׁA�ŏ���1��̂݃`�F�b�N�����OK)
	// �����ł́A�c�����邾���B��������͉̂��̂ق�
	bool bGridChange = false;										// �O���b�h�ؑ֔c��
	bool bCoilChange[NUM_MEN] = {{false}};							// �R�C���ؑ֔c��
	bool bCoilOutChange = false;									// �o�������P�ʂ̃R�C���ؑ֔c��[0:F�J�b�g 1:L�J�b�g]

	//// �R�C���؂�ւ��(�����_���P��)
	//if( 1 == hth.toVal[TOVAL_TOP_COILCHANGE] )	bCoilChange[0] = true;
	//if( 1 == hth.toVal[TOVAL_BOT_COILCHANGE] )	bCoilChange[1] = true;
	if(1 == m_trFrm.nCoilChg[my_nTorB]) bCoilChange[my_nTorB] = true;


	//// �R�C���؂�ւ��(�o�������P��)
	//if( 1 == hth.toVal[TOVAL_FCUT_COILOUTCHANGE] )	bCoilOutChange[0] = true;
	//if( 1 == hth.toVal[TOVAL_LCUT_COILOUTCHANGE] )	bCoilOutChange[1] = true;
	if(1 == m_trFrm.nFCut) bCoilOutChange = true;

	long nNowGridRow = m_nNowGridRow;
	long nFramePos = hth.frame_pos;

	//// �O���b�h�؂�ւ��
	if (nNowGridRow + 1 <= (nFramePos / (MAP_REPRES_MIN * 1000)))
	{
		bGridChange = true;
	}


	//==================================================================
	// ���̃G�b�W�ʒu���Z�o
	float	dEdge[2];												// ���G�b�W�ʒu[0:DS 1:WS]
	for(int ii=0; ii<2; ii++){
		pHth			= (HT_TO_HEAD*)&my_cRecvHead[0];
		dEdge[ii]		= pHth->edge_pos[ii];
	}


	//==================================================================
	// �e������ �v�Z�����G�b�W�����ϋP�x���Z�b�g
	for(int ii=0; ii<NUM_CAM_POS; ii++){
		for(int jj=0; jj<(NUM_CAM_ANGLE-1); jj++){
			m_nBr[ii][jj] = ((HT_TO_HEAD*)&my_cRecvHead[ii])->br[jj];
		}
	}

	//==================================================================
	// �E�R�C�����؂�ւ������
	// �E�ŏ���Ԃ��ς������ = ��\�r����
	if (bCoilChange[my_nTorB] || bGridChange)
	{
//LOG(em_INF), "[%s] <%s> �ŏ���Ԑؑ� GridRow:%d nFramePos:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), m_nNowGridRow, nFramePos);

		if(MAP_ROW_NUM <= nNowGridRow) nNowGridRow = MAP_ROW_NUM-1;		// �ő�R�C�����𒴂����ꍇ�́A�ő�R�C�����̊i�[�ʒu���X�V
		if (0 <= nNowGridRow && MAP_ROW_NUM > nNowGridRow)
		{
			// �G�b�W�ʒu�Z�b�g
			mcls_pCoil->SetData_Edge(my_nTorB, nNowGridRow, dEdge);
		}

		//================================================
		// ��\�r�{�r���X�g���M
		OpSendFunc::SendOp_ColRowData(
						my_nTorB,
						mcls_pCoil->GetCoilKenV(my_nTorB),
						nNowGridRow,
						m_OpSendDefInfo.ColnowBuf_Now.BufDefCol,
						m_OpSendDefInfo.ColnowBuf_Now.BufDefAry,
						mcls_pCoil->GetCoilKenV(my_nTorB)->nDefCnt[my_nTorB][nNowGridRow]);
		// ��\�r�{�r���X�g�Ǘ��̈�N���A
		ReSetOp_DetectData(nFramePos);

#ifndef SENDOP_LENGTH_PLG
		// �ʔ������ �𑗐M
		OpSendFunc::SendOp_Length();
#endif

		//// �s�P�ʐ�p
		if (0 <= nNowGridRow && MAP_ROW_NUM > nNowGridRow)
		{
			// �s�P�ʂ̕t����� (5m�s�b�`) ���ۂ�[MAP_ROW_NUM]���̃f�[�^������
			mcls_pCoil->SetData_Row_V_Ls(my_nTorB, nNowGridRow);
		}

		//// �R�C���P��
		////// �ŏ���Ԗ��̍s�����Z�b�g (�f�[�^���W�������ʂ��Z�b�g)
		////// ���т�5001mm�ȂǂƂȂ�ƁA�O���b�h�́{�P����邪 �s��񂪑���Ȃ��̂ŁA�Ō�̃}�b�v�P�s����������Ԃ݂����ɂȂ��h���Ή�
		if( bCoilChange[my_nTorB] ) {
			long nLen = mcls_pCoil->GetCoilKenV(my_nTorB)->nLength;
			int  nNewGridRow = nLen / (MAP_REPRES_MIN*1000);
			if(MAP_ROW_NUM <= nNewGridRow) nNewGridRow = MAP_ROW_NUM-1;		// �ő�R�C�����𒴂����ꍇ�́A�ő�R�C�����̊i�[�ʒu���X�V

			if(true) {
				// �G�b�W�ʒu�Z�b�g
				mcls_pCoil->SetData_Edge(my_nTorB, nNewGridRow, dEdge);
			}

			// �������ŃO���b�h�ʒu���v�Z���Ă������ �s���𑗂�B
			LOG(em_INF), "[%s] <%s> �R�C���ؑ� [%3.3fm] GridRow(%d��%d) fno:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), nLen/1000.0, nNowGridRow, nNewGridRow, hth.nFrameNo);

#ifndef SENDOP_LENGTH_PLG
			// �ʔ������ �𑗐M
			OpSendFunc::SendOp_Length( my_nTorB, nNewGridRow );
#endif
			// 20210604 Add --->
			// �R�C���؂�ւ����ɖ����M�r�̗L�����`�F�b�N���A�����M������Α��M
			bool bLeftovers = false;
			for (int ii = 0; ii < MAP_COL_NUM; ii++) {
				if (m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].umu) {
					LOG(em_INF), "[%s] <%s> ������ ����� �����M��\�r���M [fno:%d][No:%d][Pos:%.1f(m)]",
						my_sThreadName, DivNameManager::GetTorB(my_nTorB),
						m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].Data.nFrameNo,
						m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].Data.nKizuNo,
						m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].Data.y / 1000.0);
						bLeftovers = true;
				}
			}

			if (bLeftovers) {
				//================================================
				// ��\�r�{�r���X�g���M �����ŏ���Ԃ��r�̗L��
				OpSendFunc::SendOp_ColRowData(
					my_nTorB,
					mcls_pCoil->GetCoilKenV(my_nTorB),
					nNowGridRow,
					m_OpSendDefInfo.ColnowBuf_Now.BufDefCol,
					m_OpSendDefInfo.ColnowBuf_Now.BufDefAry,
					mcls_pCoil->GetCoilKenV(my_nTorB)->nDefCnt[my_nTorB][nNowGridRow]);
				// ��\�r�{�r���X�g�Ǘ��̈�N���A
				ReSetOp_DetectData(nFramePos);
			}
			// <---
		}
	}

	//// �o�b�t�@�ɕێ����Ă���ʒu���� DB�ɓo�^
	mcls_pCoil->SetData_CoilPosition(true, my_nTorB);


	//==================================================================
	// �R�C���g�����h
	// �Ƃ肠���� 5m�Ԋu
	if(true){
		bool bCoilTrand = false;
		int  nNewTrandRow = (hth.frame_pos / (5*1000));
		if( m_nNowGridTrandRow + 1 <= nNewTrandRow ) {
			SetCoilTrand();		// �������œo�^
		}
		m_nNowGridTrandRow = nNewTrandRow;
	}


#ifndef DISABLE_TINY_CALC
	//==================================================================
	// �������׊֘A
	// �E�R�C���ؑւ莞
	if( bCoilChange[my_nTorB] ) {
		mcls_pTiny->CoilEnd( mcls_pCoil->GetCoilKenV(my_nTorB), NULL );
	}
#endif


	//==================================================================
	// �c�������R�C���ؑւ����s
	//		�Жʂł����������Ă��Ȃ��ꍇ�ɂ��F��������K�v������ׁA�ǂ����̖ʂł�����������������

	//// ���ђ���
	////// ���ʌ����̏ꍇ�́A��ʂŖ������̃C���X�^���X�ŁA��ʂŖ������̃R�C���؂�ւ�肪����
	////// �Жʌ����̏ꍇ�́A��ʂŖ������̃R�C���؂�ւ�肪����
	for(int ii=0; ii<NUM_MEN; ii++){
		if( bCoilChange[ii] ){
			if( (-1==nMenMode && ! m_bDistPostion && bCoilChange[my_nTorB] ) ||
				(-1!=nMenMode && bCoilChange[m_nDistNotPos]) ) {
				
				mcls_pCoil->SetData_CoilResult_Virtual();

			}

	//// �Q�ƃ|�C���^�ؑ�
	// ���ʌ����̏ꍇ�́A�����̖ʂ̂�
	// �Жʌ����̏ꍇ�́A����
			if( (-1==nMenMode && ii==my_nTorB) ||
				(-1!=nMenMode) ) {

				LOG(em_MSG), "[%s] <%s> %s�R�C���؂�ւ��c�� [%.3fm]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), DivNameManager::GetTorB(ii), (m_nPerFramePos/1000.0));
				mcls_pCoil->ChangeCoilKenPointer(ii);
			
				// �G�b�W��� ���� & ���̏���
				mcls_pEdgePosCont->CoilEnd(ii); 
				mcls_pEdgePosCont->CoilStart(ii, mcls_pCoil->GetCoilKenV(ii)->cKizukenNo, (float)mcls_pCoil->GetCoilKenV(ii)->data.nWid, false);	// �J�n
			
				// �r�A�ԏ�����
				//m_nNowKizuSeq = 0;

			}
		}
	}

	//// �R�C���ؑ֊�����
	const COIL_INF*  pCoilInf = mcls_pCoil->GetCoilKenV(my_nTorB);	// ���������u�ʒu�ʉߒ��̃R�C�����|�C���^

	//// �����̖ʂ̃R�C���؂�ւ��̏ꍇ�A�擪���Ƃ��ăg�����h�o�^
	if( bCoilChange[my_nTorB] ) {
		SetCoilTrand();	
		
		// �d�v�B�L���[�������āA�܂�CoilInf���o�^�o���Ă��Ȃ��ŁA���̃R�C�����r��񂪗����Ƃ��̃g���u���Ή�
		mcls_pDbMgr->ChangeSQLInf( pCoilInf->cKizukenNo );

	}

	//==================================================================
	// �o�������P�ʂ̃R�C���ؑ֔c��
	for(int ii=0; ii<NUM_MEN; ii++){
		if(bCoilOutChange){				// ���X�g�J�b�g�^�C�~���O�Ŏ��{

			//// �Q�ƃ|�C���^�ؑ�
			// ���ʌ����̏ꍇ�́A�����̖ʂ̂�
			// �Жʌ����̏ꍇ�́A����
			if( (-1==nMenMode && ii==my_nTorB) ||
				(-1!=nMenMode) ){
				LOG(em_MSG), "[%s] �o�������R�C���ؑ֔c�� [%.3fm]", my_sThreadName, (m_nPerFramePos/1000.0));
				mcls_pCoil->ChangeCoilOutKenPointer(ii);

			}
		}
	}


	//==================================================================
	// �R�C�����їp �G�b�W�����ϋP�x���Z�b�g
	for(int ii=0; ii<NUM_CAM_POS; ii++){
		HT_TO_HEAD& pWk = (HT_TO_HEAD&)this->my_cRecvHead[ii];
		int camset = pWk.camset - (my_nTorB * NUM_CAM_POS);		// ���ʍ��킹��1�`20 ���ЖʒP�ʂ�1�`10
		StatusManager* cls_pS = (StatusManager*) mcls_pStatus;
		for(int jj=0; jj<(MAX_CAMANGLE-1); jj++){
			// �f�[�^�Z�b�g
			mcls_pCoil->SetBrCenter(my_nTorB, camset-1, jj, pWk.br[jj]);
			cls_pS->SetBrValue(pWk.camset-1, jj, pWk.br[jj]);
		}
	}


	//==================================================================
	// AGC�֘A
	//	(�R�C���؂�ւ���Ɏ��{���鎖)
	// ��CTL�Ӄւł́A�T���G���W����AGC�����s���邽�߁A�����ł͖����{
	//////if(DIV_UNTEN_NOMAL == mcls_pStatus->GetUnten() || DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) { 
	//////	if( mcls_pParam->GetParamCommon().nAGCbyCamcont ) {
	//////		// ������AGC���䂷��ꍇ�̂ݎ��s	
	//////		if(m_bEnableAGC){
	//////			Check_AGC(hth.nFrameNo);
	//////		}
	//////	}
	//////}


	//==================================================================
	// �t���[���X�L�b�v
	//	�E�R�C���؂�ւ���ł��邱��
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		pHth = (HT_TO_HEAD*)&my_cRecvHead[ii];	
		if( 0 != pHth->nFrameSkip || 
			STATUS_PROCSKIP == pHth->nFrameSt ||
			STATUS_TOOMANY  == pHth->nFrameSt ||
			STATUS_QFULL    == pHth->nFrameSt) {

			if( ! this->IsFrameSkipState() ) {
				LOG(em_INF), "[%s] �t���[���X�L�b�v (fno=%d, camset=%d, ans=%d)", my_sThreadName, pHth->nFrameNo, pHth->camset, pHth->nFrameSkip );
			}
			this->SetEvFrameSkip();
			break;
		}
	}


	//==================================================================
	// �G�b�W�ʒu�̔c��
	//	�E�R�C���؂�ւ���ł��邱��
	//////float	dEdgePos[NUM_CAM_POS];				// �e�J�����Z�b�g�̌��o�G�b�W�ʒu
	//////int		nFrameSkip[NUM_CAM_POS];			// �e�J�����Z�b�g�̃t���[���X�L�b�v
	//////int		nEdgeNg[NUM_CAM_POS];				// �e�J�����Z�b�g�̃G�b�W���o���s
	//////int		nFrameSt[NUM_CAM_POS];				// �e�J�����Z�b�g�̒T����Ԓ�`
	//////for(int ii=0; ii<NUM_CAM_POS; ii++) {
	//////	pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];	
	//////	dEdgePos[ii]	= pHth->edge_pos[0]; 
	//////	nFrameSkip[ii]	= pHth->nFrameSkip;
	//////	nEdgeNg[ii]		= pHth->nEdgeNg;
	//////	nFrameSt[ii]	= pHth->nFrameSt;
	//////

	//////	//if(0 != nFrameSkip[ii] || 0 != nEdgeNg[ii]) {
	//////	//	LOG(em_INF), "[%s] %s (%d)<%s> �t���[����Ԉُ� Frame=%d, Edge=%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB),
	//////	//		ii, pCoilInf->cKizukenNo, nFrameSkip[ii], nEdgeNg[ii]);
	//////	//}

	//////}

	int		nFrameSkip[NUM_CAM_POS];			// �e�J�����Z�b�g�̃t���[���X�L�b�v
	int		nEdgeNg[NUM_CAM_POS];				// �e�J�����Z�b�g�̃G�b�W���o���s
	int		nFrameSt[NUM_CAM_POS];				// �e�J�����Z�b�g�̒T����Ԓ�`
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];	
		nFrameSkip[ii]	= pHth->nFrameSkip;
		nEdgeNg[ii]		= pHth->nEdgeNg;
		nFrameSt[ii]	= pHth->nFrameSt;
	}
	// �t���[�����A�����������Z�b�g
	mcls_pEdgePosCont->SetFrameSkip(my_nTorB, nFrameSkip, nEdgeNg, nFrameSt);

	float	dEdgePos[2];						// ���o�G�b�W�ʒu[0:DS 1:WS]
	float	dEdgePosMax[2];						// ���o�ő�G�b�W�ʒu[0:DS 1:WS]
	float	dEdgePosMin[2];						// ���o�ŏ��G�b�W�ʒu[0:DS 1:WS]
	pHth	= (HT_TO_HEAD*)&my_cRecvHead[0];	
	for(int ii=0; ii<2; ii++) {
		dEdgePos[ii]	= pHth->edge_pos[ii]; 
		dEdgePosMax[ii]	= pHth->edge_pos_max[ii];
		dEdgePosMin[ii]	= pHth->edge_pos_min[ii];
	}
	// �G�b�W�ʒu�Z�b�g & �G�b�W����������
	if( ! mcls_pEdgePosCont->SetEdgePos(my_nTorB, hth.nFrameNo, hth.frame_pos, mcls_pPlg->GetResV(), dEdgePos, dEdgePosMax, dEdgePosMin, pCoilInf->dSpmHosei) ) {
		// ��[3m�̓G�b�W�s���̃`�F�b�N�͖����B�m�b�`���[�A�R�C�����ύX�łǂ����Ă��ُ킪�o�邽�߁B
		// 2024.02.22 mizoguchi --->>>
		//if (3000 < hth.frame_pos) {
		int nMaskFront = (long)(SIZE_GRAB_IMG_Y * mcls_pParam->GetCommonMast().Edge.nFromSkip * mcls_pPlg->GetResV());
		if (nMaskFront < hth.frame_pos) {
			// 2024.02.22 mizoguchi <<<---
			if( ! this->IsEdgeNgState() ) {
				LOG(em_INF), "[%s] <%s> �G�b�W�s���� [%s] (���o:%.3fm, ��:%.3fm, fno=%d)", my_sThreadName, DivNameManager::GetTorB(my_nTorB),
					pCoilInf->cKizukenNo, hth.frame_pos/1000.0, mcls_pPlg->GetLen_Ken(my_nTorB)/1000.0, hth.nFrameNo);
			}
			this->SetEvEdgeNg();
		}
	}


	//==================================================================
	// �}�[�W
	//	�E�����ʂ̍Ō�̂ق��ɁA�}�[�W���ʂ�ǉ�
	ExecMerge();


	//==================================================================
	// �摜�捞�`�������������܂ł̕��ρA�ő及�v����[ms]���Z�o
	QueryPerformanceCounter((LARGE_INTEGER*)&pc_now);
	m_dToProcNowTime = (double)(pc_now - pc_pre) / m_pf * 1000.0;		// [s] �� [ms]
	m_dToProcAveTime = m_dToProcAveTime*0.8 + m_dToProcNowTime*0.2;
	if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
		if( m_dToProcMaxTime < m_dToProcNowTime ) {
			m_dToProcMaxTime = m_dToProcNowTime;
		}
	}



	//==================================================================
	// DS(WS)���r����M�J�n�`WS(DS)���r����M�J�n�܂ł̕��ρA�ő及�v����[ms]���Z�o
	// �e����PC�����r����M���A�ŏ��̎�M�`���̎�M�܂ł̕��ρA�ő及�v����[ms]���Z�o

	m_dRecvProcNowTime = (double)(m_pc_RecvSecond - m_pc_RecvFirst) / m_pf * 1000.0;		// [s] �� [ms]
	m_dRecvProcAveTime = m_dRecvProcAveTime*0.8 + m_dRecvProcNowTime*0.2;
	if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
		if( m_dRecvProcMaxTime < m_dRecvProcNowTime ) {
			m_dRecvProcMaxTime = m_dRecvProcNowTime;
		}
	}


	//==================================================================
	// �摜�����G���W���̕��ρA�ő及�v����[ms]���擾
	__int64 ht_start, ht_end;
	__int64 nc_freq;
	DWORD nCamSet;
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		nCamSet = NUM_CamPos( ((HT_TO_HEAD*)&my_cRecvHead[ii])->camset );
		HT_TO_HEAD& hth  = (HT_TO_HEAD&)this->my_cRecvHead[ii];
		memcpy(&ht_start, &hth.ht_timestamp_start, sizeof(__int64));
		memcpy(&ht_end, &hth.ht_timestamp_end, sizeof(__int64));
		memcpy(&nc_freq, &hth.ht_freq, sizeof(__int64));

		// �摜�����G���W�����������`�����`���J�n�܂ł̏��p���� �Z�o
		m_dNcHtProcNowTime[ii] = (double)( ht_end - ht_start ) / nc_freq * 1000.0;	// [s] �� [ms]

		// ���ρ��ő厞�ԃZ�b�g
		m_dNcHtProcAveTime[ii] = m_dNcHtProcAveTime[ii]*0.8 + m_dNcHtProcNowTime[ii]*0.2;
		if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
			if( m_dNcHtProcMaxTime[ii] < m_dNcHtProcNowTime[ii] ) {
				m_dNcHtProcMaxTime[ii] = m_dNcHtProcNowTime[ii];
			}
		}
	}

	//==================================================================
	// ���ה���������
	ExecDefect((double)nFramePos);


#ifndef DISABLE_TINY_CALC
	//==================================================================
	// �������׊֘A
	// �E�ʏ펞
	//
	if(true) {
		TinyManager::ANSER_INF	Tiny;			// �������׌��o����
		bool bWk = mcls_pTiny->CheckTiny( (HT_TO_HEAD const*)my_cRecvHead, &Tiny );
		if( bWk ) {
			//// �������חL��B�Ȃ̂ŁA�s���ɃZ�b�g
			// �\���Ǘ���		
			OpSendFunc::SendOp_Tiny(mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo, my_nTorB, Tiny.emGr, Tiny.nPos);
		}
	}
#endif
}

//==========================================
// �Ɩ��`�� (��)���C������
// �����̏����Ԃ̓u���b�N����Ă���̂Œ���
//------------------------------------------
// SOCK_F_HEAD* p	��M�f�[�^
//==========================================
void HtRecv::OnMainHls(SOCK_F_HEAD* p)
{
	HT_TO_HEAD_HLS* pH = (HT_TO_HEAD_HLS*) p;
	ENG_STAT* pD = (ENG_STAT*) &pH->EngStat;
	//if( ! pH->nEngStatSet ) return;		// ���񖳎��f�[�^�H

	// �f�[�^�Z�b�g
	StatusManager* cls_pS = (StatusManager*) mcls_pStatus;

#ifdef DEBUG_LOG_HT_HEALTHY
	LOG(em_FIL), "[%s] <%s> �J�����䐔�F%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), pD->camnum);
#endif

	int camnum = pD->camnum;

	if (0 > camnum || NCL_ncam_max <= camnum) return;

	for(int ii=0; ii<camnum; ii++) {
		//==================================================================
		// �J�������x�Z�b�g
		for(int jj=0; jj<MAX_PHYSICAL_CAMANGLE; jj++){
			int camset = pD->camid[ii] - 1;
			if (0 > camset || MAX_CAMSET <= camset) continue;
			cls_pS->SetCamTemp(camset, jj, pD->temp[ii]); 
		}

		//==================================================================
		// �I�����ԃZ�b�g
		for(int jj=0; jj<(MAX_CAMANGLE-1); jj++){
			int camset = pD->camid[ii] - 1;
			int nWk = (int)(pD->exptime[jj][ii]);
			if (0 > camset || NUM_CAMERA <= camset) continue;
			cls_pS->SetExpTime(camset, jj, nWk);
		}

		//==================================================================
		// �J�����Q�C���Z�b�g
		for(int jj=0; jj<(MAX_CAMANGLE-1); jj++){
			int camset = pD->camid[ii] - 1;
			int nWk = (int)(pD->gain[jj][ii] * 100);
			if (0 > camset || MAX_CAMSET <= camset) continue;
			cls_pS->SetCamGain(camset, jj, nWk);
		}

		//==================================================================
		// �J�����d���Z�b�g
		for(int jj=0; jj<MAX_PHYSICAL_CAMANGLE; jj++){
			int camset = pD->camid[ii] - 1;
			if (0 > camset || NUM_CAMERA <= camset) continue;
			cls_pS->SetCamVolt(camset, (int)(pD->volt[ii] * 10.0));
		}

#ifdef DEBUG_LOG_HT_HEALTHY
	LOG(em_FIL), "[%s] <%s> �J������ԃZ�b�g[ID:%d][���x:%.3f][�I��:%.3f %.3f %.3f][�Q�C��:%.3f %.3f %.3f][�d��:%.3f]"
																				, my_sThreadName
																				, DivNameManager::GetTorB(my_nTorB)
																				, pD->camid[ii]
																				, pD->temp[ii]
																				, pD->exptime[0][ii]
																				, pD->exptime[1][ii]
																				, pD->exptime[2][ii]
																				, pD->gain[0][ii]
																				, pD->gain[1][ii]
																				, pD->gain[2][ii]
																				, pD->volt[ii]
																				);
#endif
	}

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ʏ���

//==========================================
// ���ה���������
//==========================================
void HtRecv::ExecDefect(double dFramePos)
{
	//================================================
	// ���׏��擾
	HT_TO_HEAD*				pHth = (HT_TO_HEAD*)&my_cRecvHead[0];		
	HT_TO_DATA*				pHtd = (HT_TO_DATA*)&my_cRecvData[0][0];		
	int						nJpegCmp;			// JPEG���k���[�h (-1:JPEG���k���� 0:�r���� 1�`:���k�����T�C�Y�B=Img�ȍ~�̃I�t�Z�b�g�l���Q�Ƃ���ꍇ�́A���̒l�����Z���邱��)

	NCL_SMEM_FRAME_HEADER*	pHead;
	NCL_DEFECT_RESULT*		pResu;
	double*					pAttr;
	NCL_IMAGE_HEADER*		pImgBmp;
	DEFECT_HEAD_IMAGE*		pImgJpg;
	DEFECT_DATA_IMAGE		typImg;				// ���� �Ǝ��̉摜�\����
	int						nDetectCnt;			// ���׌�
	int						tid;
	int						gid;


	const COIL_INF*  pCoilInf = mcls_pCoil->GetCoilKenV(my_nTorB);		// ���������u�ʒu�ʉߒ��̃R�C�����|�C���^
	int pcode	= pCoilInf->setting.pcode[my_nTorB]-1;					// �w�肳�ꂽ�������u�ʒu�̃R�C���̌����p�^�[�� (0�I���W���ɂ���)


	//================================================
	// 1�t���[���S�̂̌��ʎ擾
	//		�E�rNo�t�^
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		//// ���O�`�F�b�N
		pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];							// �w�b�_�[���擾
		pHtd	= (HT_TO_DATA*)&this->my_cRecvData[ii][0];					// �Ƃ肠���� �擪�̃f�[�^���擾

		pHead	= (NCL_SMEM_FRAME_HEADER*) &GET_PTR_HEADER(ii, pHtd->header_offset);	// �G���W���̃w�b�_�[

		if (FRONT == (pHead->executed_position_mask & FRONT))
		{
			//LOG(em_INF), "[%s] �摜�G���W������̃R�C���ؑ֔F�� Cam:%d frame_num:%d", my_sThreadName, ii + 1, pHead->frame_num );
		}

		if( sizeof(HT_TO_HEAD) == pHth->base_head.len ) continue;			// �r�������́A������


		//================================================
		// ��{�����ʌ�[ �� �Ǝ�����t�^
		nDetectCnt	= pHead->defect_num;
		for(int jj=0; jj<nDetectCnt; jj++) {
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd->result_offset, jj);
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, jj);	

			// ------------------------
			// �rNo�t�^
			pAttr[EPC_ATTR_BASE_KIZUNO] = mcls_pCoil->GetKizuNo((COIL_INF*)pCoilInf);
		}
	}

	//================================================
	// 1�t���[���S�̂̌��ʎ擾
	//		�E�r�A�ԕt�^
	//		�E��{�����ʌ�[ �� �Ǝ�����t�^
	//		�E1�t���[�����ł����Ƃ��D��x�̍����r����肷��
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		//// ���O�`�F�b�N
		pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];							// �w�b�_�[���擾
		pHtd	= (HT_TO_DATA*)&this->my_cRecvData[ii][0];					// �Ƃ肠���� �擪�̃f�[�^���擾

		pHead	= (NCL_SMEM_FRAME_HEADER*) &GET_PTR_HEADER(ii, pHtd->header_offset);	// �G���W���̃w�b�_�[
		if( sizeof(HT_TO_HEAD) == pHth->base_head.len ) continue;			// �r�������́A������


		//================================================
		// ��{�����ʌ�[ �� �Ǝ�����t�^
		nDetectCnt	= pHead->defect_num;
		for(int jj=0; jj<nDetectCnt; jj++) {
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd->result_offset, jj);
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, jj);	
			tid	  = pResu->type;											// �r��EdasysID
			gid	  = pResu->grade;											// �O���[�hEdasysID

			// ------------------------
			// ���׃O���[�h �t�^
			int nRank = mcls_pParam->GetRankG(gid);
			if(0 > nRank) {		// �o�^�O�̏ꍇ�́A�����I�ɁA�擪�̏��ɏ�������
				int nGidWk = mcls_pParam->GetGradeName().Grade_Id[0].nEdasysID;
				nRank = mcls_pParam->GetRankG(nGidWk);
			}
			pAttr[EPC_ATTR_BASE_RANK_G] = nRank;

		}

		//================================================
		// 1�t���[�����ł����Ƃ��D��x�̍����r����肷��
		//   �{ ��{�����ʌ�[ �� �Ǝ�����t�^
		static const int CHECKNUM = 4;									// ��r�p��
		bool   bBig[CHECKNUM] = {false, false, true, false};		// ��r�p��� (Gr[��]�A�r��[��]�A�ʐ�[��]�A����ʒu[��])
		double dNow[CHECKNUM];
		double dPre[CHECKNUM];
		int nDefMaxIndex = -1;								// �P�t���[�����ł����Ƃ��D��x�̍����r�C���f�b�N�X (-1:�Ώۖ���)

		for(int jj=0; jj<nDetectCnt; jj++) {
			pHead = (NCL_SMEM_FRAME_HEADER*) &GET_PTR_HEADER(ii, pHtd->header_offset);	// �G���W���̃w�b�_�[
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd->result_offset, jj);
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, jj);	

#ifdef LOCAL
/** ��2�񖈂ɃO���[�hID��ύX(ID+1)
pResu->grade = (my_nFrameNo[0] % 2) +1;
//**/
#endif

			tid	  = pResu->type;											// �r��EdasysID
			gid	  = pResu->grade;											// �O���[�hEdasysID


			// ------------------------
			// �������}�[�W�Ώ��r�Ȃ�֌W����
			if( 0 != pAttr[EPC_ATTR_BASE_MERGE] ) continue;

			// ------------------------
			// �ŏ����r������
			if( -1 == nDefMaxIndex) {
				nDefMaxIndex = jj;
				dPre[0] = pAttr[EPC_ATTR_BASE_YU_G];
				dPre[1] = pAttr[EPC_ATTR_BASE_YU_T];
				dPre[2] = pAttr[AREA];
				dPre[3] = pAttr[MERGED_CENTER_Y];
			}

			// ��r�p���
			dNow[0] = pAttr[EPC_ATTR_BASE_YU_G];
			dNow[1] = pAttr[EPC_ATTR_BASE_YU_T];
			dNow[2] = pAttr[AREA];
			dNow[3] = pAttr[MERGED_CENTER_Y];

			// ------------------------
			// ��r
			if( KizuFunction::SelectPriorityDetect(CHECKNUM, dNow, dPre, bBig) ) { 
				// �V������
				nDefMaxIndex = jj;
				memcpy(dPre, dNow, sizeof(dPre));
			}
		}

		// ------------------------
		// �ő�D��x
		if(-1 != nDefMaxIndex) {
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, nDefMaxIndex);	
			pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX] = 1;
		}

	}

	int nShrinkRatio = 1;

	//================================================
	// �X���r�̌��ʎ擾
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		//// ���O�`�F�b�N
		pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];	
		pHtd	= (HT_TO_DATA*)&this->my_cRecvData[ii][0];					// �Ƃ肠���� �擪�̃f�[�^���擾

		pHead	= (NCL_SMEM_FRAME_HEADER*) &GET_PTR_HEADER(ii, pHtd->header_offset);	// �G���W���̃w�b�_�[

		nJpegCmp	= pHth->nJpegCmp;
		if( sizeof(HT_TO_HEAD) == pHth->base_head.len ) continue;			// �r�������́A������

		//// ���׌���
		nDetectCnt	= pHead->defect_num;
//LOG(em_INF), "[%s] �r�摜 Cam:%d num:%d", my_sThreadName, ii + 1, nDetectCnt );
		for(int jj=0; jj<nDetectCnt; jj++) {


			//================================================
			//// ���O���� �Ƃ��� �e�A�h���X���Z�b�g
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd->result_offset, jj);
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, jj);	
			tid	  = pResu->type;											// �r��EdasysID
			gid	  = pResu->grade;											// �O���[�hEdasysID


			//================================================
			// �摜�͂�����Ɠ��� (�����r�œ����摜�̏ꍇ������)
			// �摜�z�񕪃I�t�Z�b�g�ʒu�����炵�Ă����A�擾�������摜���̃I�t�Z�b�g�ʒu���擾����

			//// BMP�摜
			if( -1 == nJpegCmp) {		
				// ����̉摜�z��ʒu�����
				int imgRecOff =	pHtd->image_offset;									// �摜���̐擪�ʒu
				for(int kk=0; kk<pHtd->image_num; kk++) {
					if(kk == pResu->im_idx) break;
					pImgBmp = (NCL_IMAGE_HEADER*) &my_cRecvData[ii][imgRecOff];		// �{�z��̉摜���擾
					imgRecOff += sizeof(DWORD)*2 + (pImgBmp->height * pImgBmp->width * MAX_CAMANGLE);			// �{�z��̉摜���T�C�Y�����Z = ���摜���̈ʒu
				}
				pImgBmp  = (NCL_IMAGE_HEADER*) &my_cRecvData[ii][imgRecOff];
				
				// �����p�摜�\���̂փf�[�^�Z�b�g
				typImg.bIsBmp	= true;
				typImg.nWidth	= pImgBmp->width;
				typImg.nHeight	= pImgBmp->height;
				for(int kk=0; kk<MAX_CAMANGLE; kk++) {
					typImg.nSize[kk] = pImgBmp->height * pImgBmp->width;
					typImg.pImg[kk]  = &pImgBmp->image[pImgBmp->height * pImgBmp->width * kk];
				}
//LOG(em_INF), "[%s] �r�摜 [BMP kizuno=%d tid=%d, gid=%d][v:%d h:%d size:%d]", my_sThreadName, (long)pAttr[EPC_ATTR_BASE_KIZUNO], tid, gid, typImg.nHeight, typImg.nWidth, typImg.nSize );

				// �؂�o���摜�̏k����
				nShrinkRatio = (int)pImgBmp->shrinkratio;

			//// Jepg�摜
			} else {
				// ����̉摜�z��ʒu�����
				int imgRecOff =	pHtd->image_offset;									// �摜���̐擪�ʒu
				int kk;
				for(kk=0; kk<pHtd->image_num; kk++) {
					if(kk == pResu->im_idx) break;
					pImgJpg = (DEFECT_HEAD_IMAGE*) &my_cRecvData[ii][imgRecOff];		// �{�z��̉摜���擾
					imgRecOff += pImgJpg->nNextImgOffset;								// ���摜���̈ʒu�����Z
//LOG(em_INF), "[%s] �r�摜 [JPG kizuno=%d no=%d tid=%d, gid=%d][imgnum:%d v:%d h:%d size:%d]", my_sThreadName, (long)pAttr[EPC_ATTR_BASE_KIZUNO], kk + 1, tid, gid, pHtd->image_num, pImgJpg->nHeight, pImgJpg->nWidth, pImgJpg->nSize );
				}
				pImgJpg  = (DEFECT_HEAD_IMAGE*) &my_cRecvData[ii][imgRecOff];
//LOG(em_INF), "[%s] �r�摜 [JPG kizuno=%d no=%d tid=%d, gid=%d][imgnum:%d v:%d h:%d size:%d]", my_sThreadName, (long)pAttr[EPC_ATTR_BASE_KIZUNO], kk + 1, tid, gid, pHtd->image_num, pImgJpg->nHeight, pImgJpg->nWidth, pImgJpg->nSize );

				// �����p�摜�\���̂փf�[�^�Z�b�g
				typImg.bIsBmp	= false;
				typImg.nWidth	= pImgJpg->nWidth;
				typImg.nHeight	= pImgJpg->nHeight;
				for(int kk=0; kk<MAX_CAMANGLE; kk++) {
					typImg.nSize[kk] = pImgJpg->nSize[kk];
//					typImg.pImg[kk]  = (BYTE*)((int)pImgJpg + pImgJpg->nImgOffset[kk]);
					typImg.pImg[kk] = &reinterpret_cast<BYTE*>(pImgJpg)[ pImgJpg->nImgOffset[kk] ];

/*/��
CString s;
s.Format("to_men%d_cam%d_%d_%d.jpg", my_nTorB, ii, jj, kk);
std::ofstream   os;
os.open(s, std::ios::binary);
os.write((const char*)typImg.pImg[kk], typImg.nSize[kk] );
os.close();
//*/
				}
//LOG(em_INF), "[%s] �r�摜 [JPG kizuno=%d tid=%d, gid=%d][imgnum:%d v:%d h:%d size:%d]", my_sThreadName, (long)pAttr[EPC_ATTR_BASE_KIZUNO], tid, gid, pHtd->image_num, typImg.nHeight, typImg.nWidth, typImg.nSize );
/*/��				
//				if (pHtd->defect_num != pHtd->image_num) {
//					LOG(em_ERR), "[%s]  [y=%dmm kizuNo=%d defect_num=%d image_num=%d, im_idx=%d, size=%d/%d/%d/%d]"
//						, my_sThreadName, (long)pAttr[MERGED_CENTER_Y], (int)pAttr[EPC_ATTR_BASE_KIZUNO]
//						, pHtd->defect_num, pHtd->image_num, pResu->im_idx 
//						, typImg.nSize[0], typImg.nSize[1], typImg.nSize[2], typImg.nSize[3]);
//				}
//*/
				// �؂�o���摜�̏k����
				nShrinkRatio = (int)pImgJpg->fShrinkRatio;
			}

			//================================================
			////// �S���摜�Ԉ�����������ʂɃZ�b�g
			////pAttr[EPC_ATTR_BASE_IMG_CULL_X]			= mcls_pParam->GetCommonMast().nImgCullX;	// �S���摜_�Ԉ�������
			////pAttr[EPC_ATTR_BASE_IMG_CULL_Y]			= mcls_pParam->GetCommonMast().nImgCullY;	// �S���摜_�Ԉ������c
			pAttr[EPC_ATTR_BASE_IMG_CULL_X] = mcls_pParam->GetParamCommon().nShFrameShrinkX;	// ���L��������t���[���摜�k��X
			pAttr[EPC_ATTR_BASE_IMG_CULL_Y] = mcls_pParam->GetParamCommon().nShFrameShrinkY;	// ���L��������t���[���摜�k��Y
			if(1 > pAttr[EPC_ATTR_BASE_IMG_CULL_X]) pAttr[EPC_ATTR_BASE_IMG_CULL_X] = 1.0;
			if(1 > pAttr[EPC_ATTR_BASE_IMG_CULL_Y]) pAttr[EPC_ATTR_BASE_IMG_CULL_Y] = 1.0;

#ifdef DEBUG_LOG_KIZU
			// ��
			LOG(em_INF), "[%s] �r���o [FNo=%d, kizuNo=%d, cam=%d, tid=%d, gid=%d, max=%d, y=%dmm]", my_sThreadName, pHth->nFrameNo, 
				(int)pAttr[EPC_ATTR_BASE_KIZUNO], pHth->camset, tid, gid, (int)pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX], (long)pAttr[MERGED_CENTER_Y] );
#endif


			//================================================
			// �}�[�L���O���v���Ԃ�����ʂɃZ�b�g
			pAttr[EPC_ATTR_BASE_HTTO_PROC_TIME]		= m_dHtProcNowTime;						// �摜�捞�`���聨�����`�������܂ł̏��v���� [msec]
			pAttr[EPC_ATTR_BASE_TOTAL_PROC_TIME]	= m_dToProcNowTime;						// �摜�捞�`�������������܂ł̏��v���� [msec]
			pAttr[EPC_ATTR_BASE_HT_PROC_TIME]		= m_dNcHtProcNowTime[pHth->camset];		// �摜�����G���W�����������`���聨�����`���J�n�܂ł̏��v���� [msec]
			pAttr[EPC_ATTR_BASE_TR1_PROC_TIME]		= m_dRecvProcNowTime;					// DS(WS)���r����M�J�n�`DS(WS)���r����M�J�n�܂ł̏��v���� [msec]
			pAttr[EPC_ATTR_BASE_TR2_PROC_TIME]		= m_dRecvProcTotalNowTime;				// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̏��v���� [msec]
			pAttr[EPC_ATTR_BASE_NC_PROC_TIME]		= 0;									// �摜�捞�`�摜�����G���W�����������܂ł̏��v���� [msec]

			//================================================
			// �X���r���̏��� (������Ƃł��������A�ȗ�������ׂɕʃ��\�b�h�Ƃ���)
			SetDetectInf(pResu, pAttr, dFramePos, nShrinkRatio, &typImg);
		}
	}
}

//==========================================
// �X���r���̏���
//------------------------------------------
// nt_defect_result*	pResu		: �������ʏ��
// double*				pAttr		: �����ʃ��X�g (��{������+�p�x1������+�p�x2�����ʥ��)
// double				nFramePos	: �t���[��[mm]
// int					nShrinkRatio: �؂�o���摜�̏k����
// DEFECT_DATA_IMAGE*	pImg		: �����p�摜���|�C���^
//==========================================
void HtRecv::SetDetectInf(NCL_DEFECT_RESULT const* pResu, double const* pAttr, double dFramePos, int nShrinkRatio, DEFECT_DATA_IMAGE const* pImg)
{
	//================================================
	// ���O����
	const COIL_INF*  pCoilInf = mcls_pCoil->GetCoilKenV(my_nTorB);			// ���������u�ʒu�ʉߒ��̃R�C�����|�C���^
	int nWidth  = pCoilInf->data.nWid;										// �R�C����[mm]

	int pcode	= pCoilInf->setting.pcode[my_nTorB]-1;					// �w�肳�ꂽ�������u�ʒu�̃R�C���̌����p�^�[�� (0�I���W���ɂ���)
	int tid		= pResu->type;											// �r��EdasysID
	int gid		= pResu->grade;											// �O���[�hEdasysID
	long nPos	= (long)pAttr[MERGED_CENTER_Y];							// �t���[���P�ʂ̃R�C����[����̋��� [mm]
	int nKizuNo = (long)pAttr[EPC_ATTR_BASE_KIZUNO];					// �rNo
	char cKanriNo[SIZE_KIZUKEN_NO];
	float dLen;															// ������� 
	memcpy(cKanriNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);

	if (0 == pImg->nHeight || 0 == pImg->nWidth || 0 == pImg->nSize || NULL == pImg->pImg)
	{
		LOG(em_ERR), "[%s] �r�摜�ُ� [%s kizuno=%d tid=%d, gid=%d][v:%d h:%d size:%d]", my_sThreadName, cKanriNo, nKizuNo, tid, gid, pImg->nHeight, pImg->nWidth, pImg->nSize );
	}

	//================================================
	// EdasysID���o�^����Ă���z���ǂ����`�F�b�N����K�v������H
	if( ! mcls_pParam->CheckEdasysID_T(tid) || ! mcls_pParam->CheckEdasysID_G(gid) ) {
		LOG(em_ERR), "[%s] �o�^�O��EdasysID�����o�B�擪EdasysID�ɋ����U�� [%s kizuno=%d tid=%d, gid=%d]", my_sThreadName, cKanriNo, nKizuNo, tid, gid );
		syslog(285, "[%s kizuno=%d tid=%d gid=%d]", cKanriNo, nKizuNo, tid, gid);

		// �����I�ɁA�擪�̏��ɏ�������
		if( ! mcls_pParam->CheckEdasysID_T(tid)) {
			tid = mcls_pParam->GetTypeName().Type_Id[0].nEdasysID; 
			((NCL_DEFECT_RESULT*)pResu)->type = tid; 
		}
		if( ! mcls_pParam->CheckEdasysID_G(gid)) {
			gid = mcls_pParam->GetGradeName().Grade_Id[0].nEdasysID;
			((NCL_DEFECT_RESULT*)pResu)->grade = gid; 
		}
	}

	// �f�o�b�N�p ��
	if( 0 != m_nPantiTypeId && m_nPantiTypeId == tid ) {
		LOG(em_INF), "[%s] <%s> �p���`�����o [%0.3fm]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), nPos/1000.0);
	}


	//LOG(em_INF), "[%s] <%s> �r���o [%0.3fm]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), nPos/1000.0);


	//================================================
	// ��{��� ����
	DETECT_BASE_DATA data;
	Create_DetectBaseData( my_nTorB, nWidth, pCoilInf->nMapCol, pResu, pAttr, nShrinkRatio, &data );
	//LOG(em_INF), "[%s] <%s> �O��Xmin[pixel]:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.frm_x_min);
	//LOG(em_INF), "[%s] <%s> �O��Xmax[pixel]:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.frm_x_max);
	//LOG(em_INF), "[%s] <%s> �O��Ymin[pixel]:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.frm_y_min);
	//LOG(em_INF), "[%s] <%s> �O��Ymax[pixel]:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.frm_y_max);
	//LOG(em_INF), "--------------------------------");
	data.w_size		 = (WORD)pImg->nWidth;
	data.h_size		 = (WORD)pImg->nHeight;
	data.base_w_size = (WORD)(pImg->nWidth * nShrinkRatio);
	data.base_h_size = (WORD)(pImg->nHeight * nShrinkRatio);
	//if (1 != nShrinkRatio)
	//{
	//	LOG(em_INF), "[%s] <%s> �؂�o���摜�̏k�����F%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), nShrinkRatio);
	//	LOG(em_INF), "[%s] <%s> ���̏c����\�F%.4f�@������\�F%.4f", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.base_h_res, data.base_w_res);
	//	LOG(em_INF), "[%s] <%s> �@�@�c����\�F%.4f�@������\�F%.4f", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.h_res, data.w_res);
	//	LOG(em_INF), "[%s] <%s> ���̐؂�o���摜 left:%d top:%d right:%d bottom:%d center:(%d, %d)", my_sThreadName, DivNameManager::GetTorB(my_nTorB),
	//		data.base_box_x_min,
	//		data.base_box_y_min,
	//		data.base_box_x_max,
	//		data.base_box_y_max,
	//		data.base_center_x,
	//		data.base_center_y);
	//	LOG(em_INF), "[%s] <%s> �@�@�؂�o���摜 left:%d top:%d right:%d bottom:%d center:(%d, %d)", my_sThreadName, DivNameManager::GetTorB(my_nTorB),
	//		data.box_x_min,
	//		data.box_y_min,
	//		data.box_x_max,
	//		data.box_y_max,
	//		data.center_x,
	//		data.center_y);
	//	LOG(em_INF), "--------------------------------");
	//}

	// 20210312 --->>>
	char cKizukenNo[SIZE_KIZUKEN_NO];
	memcpy(cKizukenNo, mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo, SIZE_KIZUKEN_NO);

	// �rDB�ۑ��E�\���𐧌�i��Ԃ��I�����C���ʒu�łȂ��A���A�@��}�X�^�[�̌����ُ�x������ȊO�𔻒�j
	if (1 == mcls_pStatus->GetStatusRead().DaisyaEx_St[my_nTorB].stat && DIV_KENSA_OK != mcls_pStatus->GetStatusRead().DaisyaEx_St[my_nTorB].kensa) {

		LOGDEF(em_NON), ",%s,%d,%d,%d,%s(%d),%s(%d),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,�Z",
			cKizukenNo, (int)pAttr[EPC_ATTR_BASE_KIZUNO], tid, gid,
			"�~", -1,
			(mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid) ? "��" : "�~"), mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid),
			0, (int)pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX], (int)pAttr[EPC_ATTR_BASE_MERGE], nPos,
			(int)pAttr[CENTER_X_FROM_L_EDGE], (int)pAttr[CENTER_X_FROM_R_EDGE], (int)pAttr[FRAME_NUM], (int)pAttr[CAMSET], (int)pAttr[LABEL_VALUE],

			mcls_pDbMgr->GetQueData_Count(), mcls_pDbMgr->GetQueDB_Count(), mcls_pDbMgr->GetBcpSaveMgr().GetQueData_Count(),
			mcls_pDbMgr->GetInf_LowGrDeleCnt(0, my_nTorB),
			mcls_pDbMgr->GetInf_DeleCnt(0, my_nTorB), false, false
			);

		return;
	}
	// 20210312 <<<---

	//================================================
	// �r�����ʒu���Z�b�g
	dLen = data.y;

	//================================================
	// �\���o�͑Ώۂ̏ꍇ�A��\�r����p�o�b�t�@�ɃZ�b�g
	//// �\���o�͑Ώ� �I��̂݁B �T����ʕ\�����ʂ�DB�Ɏc����������
	bool bAddOp = false;
	bool bObjTansho = mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_DSP_O + my_nTorB), pcode, tid, gid);		// �T����ʕ\���Ώ�
	bool bObjDeflist = mcls_pParam->Check_Pall_Output((ParamManager::EM_OUTPUT_PALLFLG)(ParamManager::OUTPUTPALLFLG_DEFLIST_DETAIL), tid, gid); // �ڍ׃t�B���^
	bool bObjPatoOutput = mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_PATOLOGHT), pcode, tid, gid);				// �p�g���C�g�o��
	bool bDspMode = false;
	if (!pCoilInf->bAlarmCancel && bObjPatoOutput & !pCoilInf->bDummeyCoil)
	{
		// �E���׌x��֎~�w�薳��
		// �E�p�g���C�g�o��
		// �E�_�~�[�R�C���ȊO
		if( 0 == pAttr[EPC_ATTR_BASE_MERGE] ) bDspMode = mcls_pArm->Add_Pato(pCoilInf->cKizukenNo, my_nTorB, nKizuNo, nPos);
	}

	// �\���@�\���M�p�Ƀf�[�^�Z�b�g
	data.nObjTansho = (bObjTansho ? 1 : 0);
	data.nObjDeflist = (bObjDeflist ? 1 : 0);
	data.nModeDeflist = (bDspMode ? 1 : 0);

	SYSTEMTIME time;
	GetLocalTime(&time);

	data.time[0] = time.wYear;
	data.time[1] = time.wMonth;
	data.time[2] = time.wDay;
	data.time[3] = time.wHour;
	data.time[4] = time.wMinute;
	data.time[5] = time.wSecond;

	bAddOp |= bObjTansho;												// �T����ʏo�͑ΏۂȂ瑗��			OR
	bAddOp |= bObjDeflist;												// �d���׈ꗗ��ʏo�͑ΏۂȂ瑗��	OR
	bAddOp &= (0 == pAttr[EPC_ATTR_BASE_MERGE] ? true : false);			// �}�[�W�Ώ��r�Ȃ瑗��Ȃ�			AND

	//// �T����ʂ��r���݂̂��J�E���g�ΏۂƂ���(�T����ʈȊO���r�����M���邽��)
	bool bRowKizuCnt = false;
	bRowKizuCnt |= mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_DSP_O + my_nTorB), pcode, tid, gid);	// �ʏ�̕\���Ώ�

	if (bAddOp)
	{
		// �\���Ǘ����M�p�r���ҏW
		SetBuf_DetectData(mcls_pCoil->GetCoilKenV(my_nTorB), &data, pImg, dLen, dFramePos);
	}


	//================================================
	// DB�ۑ��ΏہH
	int	nAddDb = -1;						// -1:�ۑ�NG   0:�ۑ�OK   9:�摜�ۑ������o�[�W����

	// ���QGr �̓���
	bool bUpGrFlg  = mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_DB_O + my_nTorB), pcode, tid, gid);	
	bool bLowGrFlg = mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid);	
	int  nLowGrFlg = ( bLowGrFlg && ! bUpGrFlg ) ? 1 : 0;	// ���Q�r�ꎞ�ۑ��t���O (��Gr <=x< ��Gr �͈͓̔��̏ꍇ�A1)	
	if( bUpGrFlg || bLowGrFlg ) {
		int  nLowGrFlgWk = nLowGrFlg;
		if( 0 != pAttr[EPC_ATTR_BASE_MERGE] ) nLowGrFlgWk = 1;	// �}�[�W�Ώ��r�Ȃ��Gr�F�� (����d�v)		
			
		// �t���[����́A�o�^�O�ɍs���悤�ɕύX (BCP���x���Ȃ�ƁA�ǂ����悤�������Ȃ��)
		nAddDb = mcls_pDbMgr->SaveDefectCheck( cKizukenNo, my_nTorB, nLowGrFlgWk, (int)pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX]);
	}


#ifdef LOG_DEF_OUTPUT
	// �����p���O	20210312 --->>>
	LOGDEF(em_NON), ",%s,%d,%d,%d,%s(%d),%s(%d),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,�~",
		cKizukenNo, (int)pAttr[EPC_ATTR_BASE_KIZUNO],tid,gid,
		(-1==nAddDb?"�~": (0==nAddDb?"��":"��")), nAddDb,
		(mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid)?"��":"�~"),mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid),
		nLowGrFlg, (int)pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX], (int)pAttr[EPC_ATTR_BASE_MERGE], nPos,
		(int)pAttr[CENTER_X_FROM_L_EDGE], (int)pAttr[CENTER_X_FROM_R_EDGE], (int)pAttr[FRAME_NUM], (int)pAttr[CAMSET], (int)pAttr[LABEL_VALUE],
	
		mcls_pDbMgr->GetQueData_Count(), mcls_pDbMgr->GetQueDB_Count(), mcls_pDbMgr->GetBcpSaveMgr().GetQueData_Count(),
		mcls_pDbMgr->GetInf_LowGrDeleCnt(0, my_nTorB),
		mcls_pDbMgr->GetInf_DeleCnt(0, my_nTorB), bObjDeflist, bDspMode
	);
	// <<<---
#endif

	if(-1 != nAddDb) {
		//// ���O����
		DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
		pDeli->kubun = DBManager::em_DEFECTINF;
		pDeli->data  = (BYTE*)new DBDATA_DEFECT_INF;
		DBDATA_DEFECT_INF* pWk = (DBDATA_DEFECT_INF*)pDeli->data;
		memset(pWk, 0x00, sizeof(DBDATA_DEFECT_INF));

		//// �f�[�^�Z�b�g
		memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
		// ��{�����ʃZ�b�g
		memcpy(pWk->attr_base, pAttr, sizeof(pWk->attr_base));
		// �p�x�ʓ����ʃZ�b�g
		memcpy(pWk->attr_angle, &pAttr[MAX_ATTR_BASE], sizeof(pWk->attr_angle));	// �p�x�����ʂ́A��{�̌��ɂ����

		// �r��{���Z�b�g
		memcpy(&pWk->data, &data, sizeof(DETECT_BASE_DATA) );

		// �r�����ʒu
		pWk->dLen = dLen;

		// �g�������r���Z�b�g
		pWk->nLowGrFlg			= nLowGrFlg;												// ���Q�r�ꎞ�ۑ��t���O (��Gr <=x< ��Gr �͈͓̔��̏ꍇ�A1)	
		pWk->nIsDspFlg			= (bObjTansho ? 1 : 0);								// �T����ʕ\���t���O
		pWk->bImgSkip	  = 9 == nAddDb ? true : false;								// �r�摜�X�L�b�v

		// �\���@�\�p ���Z�b�g(��ŃZ�b�g���Ă���Ɣ���Ă镨������ �v����)
		pWk->data.nObjTansho = pWk->nIsDspFlg;

		// �r��{���Z�b�g
		pWk->dSpmHosei = pCoilInf->dSpmHosei;										// SPM�␳��

		// ��������Έʒu�Z�b�g
		pWk->data.nWidAbsPos = (int)pAttr[ABS_X_POS];

		// �摜�f�[�^�Z�b�g
		pWk->isBmp = pImg->bIsBmp;
		for(int ii=0; ii<MAX_CAMANGLE; ii++) {
			pWk->nImgsize[ii] = pImg->nSize[ii];
			pWk->pImg[ii]	  = new BYTE[ pImg->nSize[ii] ];
			memcpy(pWk->pImg[ii], pImg->pImg[ii], pImg->nSize[ii] );
/*/��
CString s;
s.Format("to%d.jpg", ii);
std::ofstream   os;
os.open(s, std::ios::binary);
os.write((const char*)pWk->pImg[ii], pImg->nSize[ii] );
os.close();
//*/
		}

		//// �o�^�˗�
		// �� ��Ńt���[�`�F�b�N������悤�ɂ����̂ŁA�K�v�����͂������A�Ƃ肠�����c���Ă����B
		if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 50) ) {
			// DB�Ǘ��N���X�ɒʒm
			if(0 == pWk->nLowGrFlg ) mcls_pDbMgr->SetEvSaveSkip(my_nTorB);					// �L�Q�rGr�̏ꍇ DB���ۑ��ʒm
			mcls_pDbMgr->SetSaveSkipAdd(pWk->cKizukenNo, my_nTorB, pWk->nLowGrFlg );		// �폜���������Z

			// DB�����ɒx�� (�ʏ�A���܂薳���͂�)
			if( 0 == pWk->nLowGrFlg && ! mcls_pDbMgr->IsSaveSkipState(my_nTorB) ) {
				LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_DEFECT_INF]", my_sThreadName);
				syslog(SYSNO_QUEFULL_WAR, "[DBDATA_DEFECT_INF]");
			}

			// �L���[�t��
			for(int ii=0; ii<MAX_CAMANGLE; ii++) { SAFE_DELETE_ARRAY(pWk->pImg[ii]); }
			delete pDeli->data;
			delete pDeli;
		}
	}


#ifndef DISABLE_GAP_CALC
	//================================================
	// ����Y���W�v�ΏہH
	//	�E�W�v�Ώۂ̉����`����̊Ԃɓ����Ă���ꍇ�A�W�v�Ώ�
	//
	if( mcls_pParam->Check_Pall_Output(ParamManager::OUTPUTPALLFLG_DEFLIST_L, tid, gid, true) &&
		mcls_pParam->Check_Pall_Output(ParamManager::OUTPUTPALLFLG_DEFLIST_H, tid, gid, false) ) {
		
		mcls_pGap->AddDetect(pAttr);
	}
#endif


	//================================================
	//// �x��o�͗p���Z�b�g
	bool bAdd = true;
	bAdd &= (0 != pAttr[EPC_ATTR_BASE_MERGE]) ? false : true;
	if (bAdd) {
		SetAlameInf(&data, pAttr);
	}


}


//==========================================
// �}�[�W�̎��s
//==========================================
void HtRecv::ExecMerge()
{
	//================================================
	// �X�̃f�[�^�����擾
	HT_TO_DATA*		pHtd[NUM_CAM_POS];				// �S�J�����̃t���[���w�b�_�[�|�C���^ (�r�����̏ꍇ�́ANULL���Z�b�g����Ă���)
	int				nDetectCnt[NUM_CAM_POS];		// ���׌� (PCID)=�J�����Z�b�g��
	int				nDetectTotalCnt = 0;

	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		// �f�[�^������H
		HT_TO_DATA& htd  = (HT_TO_DATA&)this->my_cRecvData[ii][0];

		if( sizeof(HT_TO_HEAD) == ((HT_TO_HEAD*)&my_cRecvHead[ii])->base_head.len ) {
			pHtd[ii]		= NULL;
			nDetectCnt[ii]	= 0;							// �f�[�^���� (�w�b�_�[���̂�)
		} else {
			pHtd[ii]		= &htd;
			nDetectCnt[ii]	= htd.defect_num;
			nDetectTotalCnt+= htd.defect_num;
		}
	}

	//----------------------------
	// �D��x�Z�b�g (�}�[�W�@�\�����Ή�)
	NCL_DEFECT_RESULT*		pResu;
	double*					pAttr;							// ������
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		for( int jj=0; jj<nDetectCnt[ii]; jj++) {
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd[ii]->result_offset, jj);
			pAttr = (double*)			 &GET_PTR_ATTR(ii, pHtd[ii]->attr_offset, jj);


			// �����I���W�i���G���A�ɃZ�b�g
			pAttr[EPC_ATTR_BASE_YU_G] = mcls_pParam->GetYusenG(pResu->grade);
			pAttr[EPC_ATTR_BASE_YU_T] = mcls_pParam->GetYusenT(pResu->type);			
		}
	}


	//================================================
	// �}�[�W�̎��s
	if( 0 != nDetectTotalCnt &&
		(DIV_UNTEN_NOMAL == mcls_pStatus->GetUnten() || DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) ) { 
		// �p�����[�^�Z�b�g
		MergeManager::PARAM_MERGE prmMerge;				// �}�[�W�ɕK�v�ȃp�����[�^�Z�b�g
		prmMerge.dLapRitu = mcls_pParam->GetParamCommon().dLapRate;
		for(int ii=0; ii<NUM_CAM_POS; ii++) {
			int camset = NUM_CAM_POS*my_nTorB + ii;
			prmMerge.dOffset_x[ii] = mcls_pParam->GetParamCamSet().camset[camset].dXoffset; 
			//LOG(em_INF), "[%s] [%02d] offset=%.4f", my_sThreadName, camset, prmMerge.dOffset_x[ii]);
		}

		// ������
		MergeManager  cls_Merge;
		cls_Merge.SetLogMgr(mcls_pLog);
		cls_Merge.SetParam(&prmMerge);

		//----------------------------
		// ����Ȃ̂ŁAAP���ŃZ�b�g
		int cnt = 0;
		MergeManager::MergeInfo* lst = new MergeManager::MergeInfo[nDetectTotalCnt];			// �r���X�g

		for(int ii=0; ii<NUM_CAM_POS; ii++) {
			for( int jj=0; jj<nDetectCnt[ii]; jj++) {
				lst[cnt].pAttr		= (double*) &GET_PTR_ATTR(ii, pHtd[ii]->attr_offset, jj);
				//
				lst[cnt].nCamset	= (int)lst[cnt].pAttr[CAMSET];
				//
				lst[cnt].yusenG		= (int)lst[cnt].pAttr[EPC_ATTR_BASE_YU_G];
				lst[cnt].yusenT		= (int)lst[cnt].pAttr[EPC_ATTR_BASE_YU_T];
				lst[cnt].area		= lst[cnt].pAttr[AREA];
				lst[cnt].len		= lst[cnt].pAttr[CENTER_Y];
				//
				lst[cnt].xmin		= lst[cnt].pAttr[BOX_X_MIN] * lst[cnt].pAttr[HORZ_RES] + prmMerge.dOffset_x[ii];
				lst[cnt].xmax		= lst[cnt].pAttr[BOX_X_MAX] * lst[cnt].pAttr[HORZ_RES] + prmMerge.dOffset_x[ii];
				lst[cnt].ymin		= lst[cnt].pAttr[BOX_Y_MIN] * lst[cnt].pAttr[VERT_RES];
				lst[cnt].ymax		= lst[cnt].pAttr[BOX_Y_MAX] * lst[cnt].pAttr[VERT_RES];
				//
				lst[cnt].lstLap		= NULL; 
				lst[cnt].isChecked	= false;
				cnt++;
			}
		}

		//// ���s
		cls_Merge.DoMerge( nDetectCnt, lst);

		//// ���
		SAFE_DELETE_ARRAY(lst);
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AGC 
//#define DEBUG_PRINT_AGC			// ��`�� ���O�o��

//==========================================
// AGC���s
//------------------------------------------
// DWORD	framenum	: ���݂̃t���[��No	
//==========================================
void HtRecv::Check_AGC(DWORD framenum)
{
	// �T���G���W����AGC�����s���邽�߁A�����ł͖����{
}

//==========================================
// �R�C���g�����h����o�^
//==========================================
void HtRecv::SetCoilTrand()
{
	float dWk1, dWk2;

	//// ���O����
	DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
	pDeli->kubun = DBManager::em_COILTRAND;
	pDeli->data  = (BYTE*)new DBDATA_COIL_TRAND;
	DBDATA_COIL_TRAND* pWk = (DBDATA_COIL_TRAND*)pDeli->data;
	memset(pWk, 0x00, sizeof(DBDATA_COIL_TRAND));

	//================================================
	//// �f�[�^�Z�b�g
	memcpy(pWk->cKizukenNo, mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->nMen		= my_nTorB; 
	pWk->nLen		= ((HT_TO_HEAD*)&my_cRecvHead[0])->frame_pos;
	pWk->nSpeed		= mcls_pPlg->GetSpeed();		// ���C�����x
	mcls_pEdgePosCont->SearchEdgeRawPos(my_nTorB, dWk1, dWk2);
	//dWk1			= mcls_pCoil->GetCoilKenV(my_nTorB)->dEdge[mcls_pParam->GetCommonMast().nWalkMen][m_nNowGridRow][0];	// ���G�b�W�ʒu
	//dWk2			= mcls_pCoil->GetCoilKenV(my_nTorB)->dEdge[mcls_pParam->GetCommonMast().nWalkMen][m_nNowGridRow][1];	// ���G�b�W�ʒu
	pWk->dCoilWid	= abs(dWk1) + abs(dWk2);		// ��(���G�b�W�ʒu����擾)
	pWk->dWalk      = (dWk1 + dWk2) / 2.0f;			// �E�H�[�N��(���C���Z���^�[����̋���)

	mcls_pEdgePosCont->SearchEdgeRawPosMax(my_nTorB, dWk1, dWk2);
	pWk->dCoilWidMax	= abs(dWk1) + abs(dWk2);	// ���ő�(���G�b�W�ʒu����擾)
	mcls_pEdgePosCont->SearchEdgeRawPosMin(my_nTorB, dWk1, dWk2);
	pWk->dCoilWidMin	= abs(dWk1) + abs(dWk2);	// ���ŏ�(���G�b�W�ʒu����擾)

	pWk->dSpmHosei	= mcls_pCoil->GetCoilKenV(my_nTorB)->dSpmHosei;		// SPM�␳��
//LOG(em_INF), "[%s] [DBDATA_COIL_TRAND] '%s',%d,%d (%d %.4f)", my_sThreadName, pWk->cKizukenNo, my_nTorB, (int)((double)pWk->nLen * pWk->dSpmHosei), pWk->nLen, pWk->dSpmHosei);

	for(int ii=0; ii<NUM_CAM_POS; ii++){
		HT_TO_HEAD* pHth = (HT_TO_HEAD*)&my_cRecvHead[ii];	
		HT_TO_DATA* pDth = (HT_TO_DATA*)&my_cRecvData[ii][0];	

#ifndef HT_IS_COLOR				// �P�x
		for(int jj=0; jj<NUM_CAM_ANGLE; jj++) {
			pWk->nBr[ii][jj][0]	= pHth->br[jj].br_center;
			pWk->nBr[ii][jj][1]	= pHth->br[jj].br_l_ovl;
			pWk->nBr[ii][jj][2]	= pHth->br[jj].br_r_ovl;

			pWk->nExp[ii][jj]	= mcls_pCoil->GetCamExpTime(my_nTorB, ii, jj);
		}
#else							// �F��
		for(int jj=0; jj<(NUM_CAM_ANGLE -1); jj++) {
			pWk->nBr[ii][jj][0]	= pHth->br[jj];						// �P�x

		}
		for(int jj=0; jj<(NUM_CAM_ANGLE -1); jj++) {
			pWk->nExp[ii][jj] = mcls_pCoil->GetCamExpTime(my_nTorB, ii, jj);	// �I������
		}
		for (int jj = 0; jj<(NUM_CAM_ANGLE -1); jj++) {
			pWk->dGain[ii][jj] = mcls_pCoil->GetCamGain(my_nTorB, ii, jj);		// �J�����Q�C���l
		}
#endif
	}
	for(int ii=0; ii<DIV_LED_COLOR_END; ii++) {
		pWk->nLightValue[ii] = mcls_pCoil->GetLedValue(my_nTorB,ii);
	}

	//================================================
	//// �o�^�˗�
	if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 100) ) {	// �o�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_COIL_TRAND]", my_sThreadName);
		//syslog(SYSNO_QUEFULL_WAR, "[DBDATA_COIL_TRAND]");
		delete pDeli->data;
		delete pDeli;
	}
}

//==========================================
// ��{�r��� ����
//------------------------------------------
// int					men		: 0:�\ 1:��
// int					coilwid	: �R�C���� [mm]
// int					nMapCol	: �R�C��������
// NCL_DEFECT_RESULT*	pResu	: ����
// double*				attr	: ��{������
// int				nShrinkRatio: �k����
// DETECT_BASE_DATA*	data	: ��{�r���
//==========================================
void HtRecv::Create_DetectBaseData(int men, int coilwid, int nMapCol, NCL_DEFECT_RESULT const* pResu, double const* attr, int nShrinkRatio, DETECT_BASE_DATA* data)
{
	// ���O����
	double dkiriX		= attr[IMAGE_X] / attr[HORZ_RES];						// �؂�o���摜�����Pixel�ʒu
	double dkiriY		= attr[IMAGE_Y] / attr[VERT_RES];						// �؂�o���摜�����Pixel�ʒu

	// �f�[�^�Z�b�g
	data->TorB			=			men;
	//data->nTid			= (WORD)	attr[DEFECT_TYPE];
	//data->nGid			= (WORD)	attr[DEFECT_GRADE];
	data->nTid			= (WORD)	pResu->type;			// �U��ւ���ɂ���
	data->nGid			= (WORD)	pResu->grade;			// �U��ւ���ɂ���
		

	// �t���O (�ȈՎ����̏ꍇ�́A1�Œ�)
	if(      ((DWORD)attr[DEFECT_FLAG] & NCL_DEFECT_FLAGS::PERIODIC)  == NCL_DEFECT_FLAGS::PERIODIC ) {	// �ȈՎ����ł̎����r
		data->cycle_div	= 1;
	} else if( ((DWORD)attr[DEFECT_FLAG] & NCL_DEFECT_FLAGS::SERIES) == NCL_DEFECT_FLAGS::SERIES ) {	// �ȈՎ����ł̘A���r
		data->cycle_div	= 1;
	} else {
		data->cycle_div	= 0;
	}

	data->camset		= (WORD)	attr[CAMSET];

	data->nKizuNo		= (long)	attr[EPC_ATTR_BASE_KIZUNO];				// �rNo (�\�����ʂ�1�`�̘A��)

	data->y				= (float)	attr[MERGED_CENTER_Y];
	data->y_out			= data->y;												// �����ł͓��l���Z�b�g
	if( 0 == CAMLEFT_IS_DS(men) ) {												// �J����������WS�Ή�
		data->ds		= (float)	attr[CENTER_X_FROM_L_EDGE];
		data->ws		= (float)	attr[CENTER_X_FROM_R_EDGE];
	} else {
		data->ds		= (float)	attr[CENTER_X_FROM_R_EDGE];
		data->ws		= (float)	attr[CENTER_X_FROM_L_EDGE];
	}
	// ��G�b�W�́A�P���ɋ߂��ق����̗p����
	data->dsws	= ( data->ds <= data->ws ) ? 0 : 1;
	

	data->dsc			= (float)	((coilwid/2.0) - data->ds);
	data->wsc			= (float)	((coilwid/2.0) - data->ws);
	data->speed			= (int)		( attr[LINE_SPEED] * 60.0f / 1000.0f + 0.5f );
	data->pitch			= (int)		attr[CYCLE_LENGTH];
	data->roolkei		= (float)	( attr[CYCLE_LENGTH] / M_PI );

	data->aria			= (float)	attr[AREA];
	data->kizu_wid		= (int)		attr[FERET_X];
	data->kizu_len		= (int)		attr[FERET_Y];

	data->h_res			= (float)	(attr[VERT_RES] * (double)nShrinkRatio);	// �c����\[mm/pixel] ���k�������l��
	data->w_res			= (float)	(attr[HORZ_RES] * (double)nShrinkRatio);	// ������\[mm/pixel] ���k�������l��
	data->nFrameNo		= (long)	attr[FRAME_NUM];	

	data->frm_x_min		= (short)	attr[BOX_X_MIN];
	data->frm_x_max		= (short)	attr[BOX_X_MAX];
	data->frm_y_min		= (short)	attr[BOX_Y_MIN];
	data->frm_y_max		= (short)	attr[BOX_Y_MAX];

	data->cull_frm_x_min		= (short)	(attr[BOX_X_MIN] / attr[EPC_ATTR_BASE_IMG_CULL_X]);
	data->cull_frm_x_max		= (short)	(attr[BOX_X_MAX] / attr[EPC_ATTR_BASE_IMG_CULL_X]);
	data->cull_frm_y_min		= (short)	(attr[BOX_Y_MIN] / attr[EPC_ATTR_BASE_IMG_CULL_Y]);
	data->cull_frm_y_max		= (short)	(attr[BOX_Y_MAX] / attr[EPC_ATTR_BASE_IMG_CULL_Y]);

	// �؂�o���摜�Z�b�g (���_�́A�؂�o���摜�̍��ォ��̈ʒu)
	// ���k�������l��
	data->box_x_min = (short)((attr[BOX_X_MIN] - dkiriX) / (double)nShrinkRatio);
	data->box_x_max = (short)((attr[BOX_X_MAX] - dkiriX) / (double)nShrinkRatio);
	data->box_y_min = (short)((attr[BOX_Y_MIN] - dkiriY) / (double)nShrinkRatio);
	data->box_y_max = (short)((attr[BOX_Y_MAX] - dkiriY) / (double)nShrinkRatio);
	data->center_x = (short)(((attr[CENTER_X] / attr[HORZ_RES]) - dkiriX) / (double)nShrinkRatio);
	data->center_y = (short)(((attr[CENTER_Y] / attr[VERT_RES]) - dkiriY) / (double)nShrinkRatio);
//	data->h_size		= // ���̎��_�ł͕s��
//	data->w_size		= 

// ���摜�̐؂�o���摜�Z�b�g (���_�́A�؂�o���摜�̍��ォ��̈ʒu)
// ���k���O�̍��W�n�ő����Ă���̂ŁA���̂܂܃Z�b�g
	data->base_box_x_min = (short)(attr[BOX_X_MIN] - dkiriX);
	data->base_box_x_max = (short)(attr[BOX_X_MAX] - dkiriX);
	data->base_box_y_min = (short)(attr[BOX_Y_MIN] - dkiriY);
	data->base_box_y_max = (short)(attr[BOX_Y_MAX] - dkiriY);
	data->base_center_x = (short)((attr[CENTER_X] / attr[HORZ_RES]) - dkiriX);
	data->base_center_y = (short)((attr[CENTER_Y] / attr[VERT_RES]) - dkiriY);
//	data->base_h_size	 = // ���̎��_�ł͕s��
//	data->base_w_size	 =
	data->base_h_res	 = (float)attr[VERT_RES];	// ���̏c����\[mm/pixel]
	data->base_w_res	 = (float)attr[HORZ_RES];	// ���̉�����\[mm/pixel]
	data->nShrinkRatio	 = nShrinkRatio;			// �k����

	//data->nFrameDefMax	= (WORD)	attr[EPC_ATTR_BASE_FRAME_DEFECT_MAX];
	data->nTYuu			= (WORD)	attr[EPC_ATTR_BASE_YU_T];
	data->nGYuu			= (WORD)	attr[EPC_ATTR_BASE_YU_G];

// 2023.09.14 �摜�������u H�ES�\�t�g���� -------------------->>>>>
	data->nIFMergeFlg	= (short)		attr[IFMERGE_FLAG];
// 2023.09.14 �摜�������u H�ES�\�t�g���� <<<<<--------------------

	//================================================
	// �}�b�v���O���b�h�ʒu�Z�o
	int col;													// �������b�V���̗�ʒu (0�I���W��)
#ifndef PLG_DSP_MAP_FIX
	col		= KizuFunction::SelectColPosition2(data->dsws, data->ds, data->ws, coilwid, nMapCol, MAP_COL_NUM, MAP_COL_INTERVAL );
#else
	col		= KizuFunction::SelectColPositionFix(data->dsws, data->ds, data->ws, coilwid, MAP_COL_NUM);
#endif

	// �������m�F
	if( 0 > col || col >= MAP_COL_NUM ) {
		if( col < 0 )	col = 0;
		else			col = MAP_COL_NUM - 1;
	}
	data->nWidDiv		= col;


	//// �t�я�� ------------------------------------------
	//data->nCycleNo		= (int) attr[CYCLE_SERIAL];					// �����r�A��


}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �x��
//------------------------------------------
// �x��o�͑Ώی��ׂ̔�����s��
//------------------------------------------
// DETECT_BASE_DATA const* pD ��{���
//------------------------------------------
void HtRecv::SetAlameInf(DETECT_BASE_DATA const* pD, double const* pAttr)
{
	// ���x�񔻒菈���̓R�C�����Ǘ��Ɉړ�
	mcls_pCoil->SetAlameInf(pD, my_nTorB, mcls_pCoil->GetCoilKen_DataSet(my_nTorB), pAttr);
}

//------------------------------------------
// ��\�r�A�r���X�g�N���A
//------------------------------------------
void HtRecv::ClearDefectBuffer()
{
	for (int ii = 0; ii<MAP_COL_NUM; ii++)
	{
		for (int jj = 0; jj<MAX_CAMANGLE; jj++)
		{
			if (NULL != m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].pImg[jj])
			{
//LOG(em_INF), "[%s] ��\�r�摜��� kizuNo= %d men=%d col:%d addr:0x%x", my_sThreadName,
//	m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].Data.nKizuNo, my_nTorB, ii, m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].pImg[jj]);
				SAFE_DELETE_ARRAY(m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].pImg[jj]);
			}
		}
	}
	for (int ii = 0; ii<MAP_COL_NUM; ii++)
	{
		for (int jj = 0; jj < MAP_NEST_NUM; jj++)
		{
			for (int kk = 0; kk<MAX_CAMANGLE; kk++)
			{
				if (NULL != m_OpSendDefInfo.ColnowBuf_Now.BufDefAry[ii][jj].pImg[kk])
				{
//LOG(em_INF), "[%s] �r���X�g�摜��� kizuNo= %d men=%d col:%d no:%d addr:0x%x", my_sThreadName,
//	m_OpSendDefInfo.ColnowBuf_Now.BufDefAry[ii][jj].Data.nKizuNo, my_nTorB, ii, jj, m_OpSendDefInfo.ColnowBuf_Now.BufDefAry[ii][jj].pImg[kk]);
					SAFE_DELETE_ARRAY(m_OpSendDefInfo.ColnowBuf_Now.BufDefAry[ii][jj].pImg[kk]);
				}
			}
		}
	}
}

//------------------------------------------
// ��\�r�I��o�b�t�@������
// int nFramePos	�t���[���ʒu[mm]
//------------------------------------------
void HtRecv::ReSetOp_DetectData(int nFramePos)
{
	//// ��\�r�o�b�t�@ (���̎��_�Ŏc���Ă��鎖�͖����͂�)
	// �C���X�^���X�J��
	ClearDefectBuffer();

	// ���̍ŏ���ԃo�b�t�@���J�����g�ɃR�s�[
	memcpy(&m_OpSendDefInfo.ColnowBuf_Now, &m_OpSendDefInfo.ColnowBuf_Next, sizeof(m_OpSendDefInfo.ColnowBuf_Next));
	memset(&m_OpSendDefInfo.ColnowBuf_Next, 0x00, sizeof(m_OpSendDefInfo.ColnowBuf_Next));
}

//------------------------------------------
// �r����\���Ǘ����M�p�\���̂ɕϊ�
// TYP_COLROW_BUF *	pBuf �r���܂܂��R�C�����
// double dLen �r�����ʒu
// DETECT_BASE_DATA const* pData �������ʃ}�g��
// DEFECT_DATA_IMAGE* pImg �����p�摜���|�C���^
//------------------------------------------
void HtRecv::DetectToColRow(TYP_COLROW_BUF *pBuf, double dLen, DETECT_BASE_DATA const *pData, DEFECT_DATA_IMAGE const *pImg)
{
	pBuf->umu = true;
	pBuf->bIsBmp = pImg->bIsBmp;
	pBuf->dLen = dLen;

	// �r���R�s�[
	memcpy(&pBuf->Data, pData, sizeof(DETECT_BASE_DATA));

	// �摜�R�s�[
	int nSize;
	for (int ii = 0; ii < MAX_CAMANGLE; ii++)
	{
		nSize = pImg->nSize[ii];
		pBuf->nImgsize[ii] = nSize;
		if (0 < nSize)
		{
			pBuf->pImg[ii] = new BYTE[nSize];
			memcpy(pBuf->pImg[ii], pImg->pImg[ii], nSize);
		}
	}
}

//------------------------------------------
// �\���Ǘ����M�p�r���ҏW
// COIL_INF const*	pCoilInf �r���܂܂��R�C�����
// DETECT_BASE_DATA const* pData �������ʃ}�g��
// DEFECT_DATA_IMAGE const* pImg �����p�摜���|�C���^
// double dLen �r�����ʒu�i������[����̈ʒu[mm]�j
// double dFramePos ���݂̃t���[���ʒu[mm]
//------------------------------------------
void HtRecv::SetBuf_DetectData(COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, double dLen, double dFramePos)
{
	int col			= pData->nWidDiv;		// �������b�V���̗�ʒu (0�I���W��)
	int nRange		= (MAP_REPRES_MIN * 1000);
	int nStartPos	= (int)(dFramePos / (double)nRange) * nRange;
	int nEndPos		= nStartPos + nRange;

#ifdef DEF_DETECT_LOG
	LOG(em_INF), "[%s] ���݂̍ŏ���ԁF[st:%d ed:%d]", my_sThreadName, nStartPos, nEndPos);
#endif

	int nGridRow = (int)(0 <= dLen ? dLen / (double)nRange : 0);	// �����r�����ʒu���O�����̏ꍇ�͂O�s�ڂƂ���
	int nDefCnt	 = 1;												// �s�ʒu���͈͊O�i�ő�R�C���������j�ł�������r���P��ۏ�

	// �}�b�v�ő�s���i�ő�R�C�����j�𒴂��Ă�����A�ŏI�s�̌��ɉ��Z
	if (MAP_ROW_NUM <= nGridRow) nGridRow = MAP_ROW_NUM - 1;
	// ����ʒu�P�ʂ��r�������Z���A���Z��̒l��Ԃ�
	if (0 != pData->nObjTansho)
	{
		// �T����ʏo�͑Ώ�
		// �����݂��r���Ɂ{�P���ĕԂ�
		nDefCnt = mcls_pCoil->SetDefectCount(my_nTorB, pCoilInf->cKizukenNo, nGridRow);
	}
	else
	{
		// �T����ʏo�͑ΏۊO
		// �����݂��r����Ԃ�
		nDefCnt = mcls_pCoil->GetDefectCount(my_nTorB, pCoilInf->cKizukenNo, nGridRow);
	}
	// ���Z�悪������Ȃ������ꍇ�A������r���P��ۏ�
	if (-1 == nDefCnt) nDefCnt = 1;

	// �O�̍ŏ���Ԃ��r
	if (dLen < (double)nStartPos)
	{
#ifdef DEF_DETECT_LOG
		LOG(em_WAR), "[%s] �r���m(�ʔς̍ŏ���Ԃ��r) kizuNo=%d �r��:%d �O���[�h:%d col:%d len=%.2f", my_sThreadName, pData->nKizuNo, pData->nTid, pData->nGid, col, dLen);
#endif

		// �����ɑ��M����

		// �r���ϊ�
		TYP_COLROW_BUF	wkBuf;
		memset(&wkBuf, 0x00, sizeof(wkBuf));
		DetectToColRow(&wkBuf, dLen, pData, pImg);

		// �P�r��񑗐M
		OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &wkBuf);

		// �r�����M
#ifdef DEF_DETECT_LOG
		LOG(em_INF), "[%s] �����˕\���� �r�����M [����ʒu=%d �r��=%d]", my_sThreadName,
			nGridRow * nRange, nDefCnt);
#endif
		OpSendFunc::SendOp_DetectNum(pCoilInf->cKizukenNo, my_nTorB, nGridRow * nRange, nDefCnt);

		// �摜�̈���
		for (int ii = 0; ii < MAX_CAMANGLE; ii++)
		{
			if (0 < wkBuf.nImgsize[ii])
			{
				SAFE_DELETE_ARRAY(wkBuf.pImg[ii]);
				wkBuf.pImg[ii] = NULL;
			}
		}
	}
	else if (dLen >= (double)nStartPos && dLen < (double)nEndPos)
	{
		// �J�����g�o�b�t�@�ɓo�^���āA�ŏ���ԐؑցA�R�C���ؑ֎��ɂ܂Ƃ߂đ��M
		SetOp_DetectData(mcls_pCoil->GetCoilKenV(my_nTorB), pData, pImg, dLen, &m_OpSendDefInfo.ColnowBuf_Now);
	}
	else
	{
		if (dLen < (double)(nEndPos + nRange))
		{
			// ���ŏ���ԃo�b�t�@�ɓo�^����
#ifdef DEF_DETECT_LOG
			LOG(em_WAR), "[%s] �r���m(���̍ŏ���Ԃ��r) kizuNo=%d �r��:%d �O���[�h:%d col:%d len=%.2f", my_sThreadName, pData->nKizuNo, pData->nTid, pData->nGid, col, dLen);
#endif
			// ���ŏ���ԃo�b�t�@�ɓo�^���āA�ŏ���ԐؑցA�R�C���ؑ֎��ɂ܂Ƃ߂đ��M
			SetOp_DetectData(mcls_pCoil->GetCoilKenV(my_nTorB), pData, pImg, dLen, &m_OpSendDefInfo.ColnowBuf_Next);
		}
		else
		{
			// ���ŏ���Ԃ�����r
#ifdef DEF_DETECT_LOG
			LOG(em_WAR), "[%s] �r���m(���̍ŏ���Ԃ�����r) kizuNo=%d �r��:%d �O���[�h:%d col:%d len=%.2f", my_sThreadName, pData->nKizuNo, pData->nTid, pData->nGid, col, dLen);
#endif
			// �����ɑ��M����

			// �r���ϊ�
			TYP_COLROW_BUF	wkBuf;
			memset(&wkBuf, 0x00, sizeof(wkBuf));
			DetectToColRow(&wkBuf, dLen, pData, pImg);

			// �P�r��񑗐M
			OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &wkBuf);

			// �r�����M
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] �����˕\���� �r�����M [����ʒu=%d �r��=%d]", my_sThreadName,
				nGridRow * nRange, nDefCnt);
#endif
			OpSendFunc::SendOp_DetectNum(pCoilInf->cKizukenNo, my_nTorB, nGridRow * nRange, nDefCnt);

			// �摜�̈���
			for (int ii = 0; ii < MAX_CAMANGLE; ii++)
			{
				if (0 < wkBuf.nImgsize[ii])
				{
					SAFE_DELETE_ARRAY(wkBuf.pImg[ii]);
					wkBuf.pImg[ii] = NULL;
				}
			}
		}
	}
}

//------------------------------------------
// ��\�r�I�聕�r���X�g�܂Ƃ�
// COIL_INF const*	pCoilInf �r���܂܂��R�C�����
// DETECT_BASE_DATA const* pData �������ʃ}�g��
// DEFECT_DATA_IMAGE* pImg �����p�摜���|�C���^
// double dLen �r�����ʒu
// �߂�l�F0:����, -1:�}�b�v�ʒu�s��
//------------------------------------------
int HtRecv::SetOp_DetectData(COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, double dLen, TYP_COLROW_INFO *pColRow_Info)
{
	int nDataSet	= 0;					// �o�b�t�@�Ƀf�[�^�Z�b�g����H���Ȃ��H (0:����  1:�V�K  2:�V������ 3:�V���ア)
	int col			= pData->nWidDiv;		// �������b�V���̗�ʒu (0�I���W��)
	TYP_COLROW_BUF	wkBuf;					// ���[�N

	if (MAP_COL_NUM <= col)
	{
		LOG(em_ERR), "[%s] �������敪 �ُ� kizuNo= %d col:%d", my_sThreadName, pData->nKizuNo, col);
		return -1;
	}

#ifdef DEF_DETECT_LOG
	LOG(em_INF), "[%s] �r���m kizuNo=%d �r��:%d �O���[�h:%d col:%d len=%.2f", my_sThreadName, pData->nKizuNo, pData->nTid, pData->nGid, col, dLen);
#endif

	memset(&wkBuf, 0x00, sizeof(wkBuf));

	////////////////////////////////////////////////////////////////////////////
	// �P�i�ځB��P�ʂő�\�r���Z�o

	// ������ɑ�\�r�L��
	if (!pColRow_Info->BufDefCol[col].umu)
	{
		// �V�K
		nDataSet = 1;
	}
	else
	{
		// �����f�[�^�L��

		// �o�b�t�@���ƗD��x��r
		if (SelectPriorityDetect(pData, &pColRow_Info->BufDefCol[col].Data))
		{
			// �V������
			nDataSet = 2;

			// �Â��z�̓��[�N�Ɉړ�
			memcpy(&wkBuf, &pColRow_Info->BufDefCol[col], sizeof(wkBuf));
			// �摜�̈�͍ė��p���邩���ŉ��
			memset(&pColRow_Info->BufDefCol[col], 0x00, sizeof(pColRow_Info->BufDefCol[col]));
		}
		else
		{
			// �V���ォ����
			nDataSet = 3;
		}
	}

	// ������r�����o�b�t�@�ɃZ�b�g
	// ���V�K or �V������
	if (1 == nDataSet || 2 == nDataSet)
	{
		// ����̃f�[�^���Z�b�g
		DetectToColRow(&pColRow_Info->BufDefCol[col], dLen, pData, pImg);

#ifdef DEF_DETECT_LOG
		LOG(em_INF), "[%s] ��\�r�o�^ kizuNo= %d �r��:%d �O���[�h:%d col:%d", my_sThreadName,
			pColRow_Info->BufDefCol[col].Data.nKizuNo, pData->nTid, pData->nGid, col);
#endif

		if (1 == nDataSet)
		{
			// �V�K�o�^�̏ꍇ�͂����܂�
			return 0;
		}
	}

	////////////////////////////////////////////////////////////////////////////
	// �Q�i�ځB��P�ʂŃ��X�g�r���Z�o

	// ����̃Z�b�g�ʒu
	// ���f�t�H���g�F�Ώۗ�̍Ō�̈ʒu
	int nIndex = pColRow_Info->nSetCnt[col];

	DETECT_BASE_DATA const* pNewWk;
	if (2 == nDataSet)
	{
		// �V������

		// ��\����ǂ��o���ꂽ�r�Ɣ�r
		pNewWk = &wkBuf.Data;
	}
	else if (3 == nDataSet)
	{
		// �V���ア

		// �����r�Ɣ�r
		// ����\�r�ɂ͓���Ȃ�����
		pNewWk = pData;
	}

	for (int ii = 0; ii < pColRow_Info->nSetCnt[col]; ii++)
	{
		// �o�b�t�@���ƗD��x��r
		if (SelectPriorityDetect(pNewWk, &pColRow_Info->BufDefAry[col][ii].Data))
		{
			// ������r���D��x��
			nIndex = ii;
			break;
		}
	}

	// �Z�b�g�ΏۊO
	// �����X�gMAX�ł��A�����r���ł��ア
	if (MAP_NEST_NUM == nIndex)
	{
		nIndex = -1;

		if (2 == nDataSet)
		{
			// �V������

			// �����o���ꂽ��\�r�̋������M
			// �d���׈ꗗ�����\���Ώ��r�̏ꍇ�͑������M
			// ���d���בΏ��r���A���������ɁA�܂Ƃߑ��M�@�\�ɂ��
			// �@�d���׈ꗗ�����\���Ώ��r���j������Ă��܂����΍�
			if (1 == wkBuf.Data.nModeDeflist)
			{
				// �����������d���׈ꗗ�����\���Ώ��r���𑗐M
				OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &wkBuf);

//#ifdef DEF_DETECT_LOG
				LOG(em_INF), "[%s] �r���M(�����o���d���׈ꗗ�����\���Ώ��r�F��\�r) kizuNo=%d �r��:%d �O���[�h:%d col:%d", my_sThreadName,
					wkBuf.Data.nKizuNo, wkBuf.Data.nTid, wkBuf.Data.nGid, col);
//#endif
			}

			// ��\����ǂ��o���ꂽ�r���폜
			for (int ii = 0; ii < MAX_CAMANGLE; ii++)
			{
				if (0 < wkBuf.nImgsize[ii])
				{
					SAFE_DELETE_ARRAY(wkBuf.pImg[ii]);
					wkBuf.pImg[ii] = NULL;
				}
			}
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] ��\�r�폜 kizuNo=%d �r��:%d �O���[�h:%d col:%d", my_sThreadName,
				wkBuf.Data.nKizuNo, wkBuf.Data.nTid, wkBuf.Data.nGid, col);
#endif
			memset(&wkBuf, 0x00, sizeof(wkBuf));
		}
		else if (3 == nDataSet)
		{
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] �o�^�ΏۊO kizuNo=%d �r��:%d �O���[�h:%d col:%d", my_sThreadName,
				pData->nKizuNo, pData->nTid, pData->nGid, col);
#endif
			// �ꎞ�o�b�t�@�Ɋi�[
			DetectToColRow(&wkBuf, dLen, pData, pImg);

			// ��\�r�ɂ����X�g�r�ɂ�����Ȃ��r�̋������M
			// �d���׈ꗗ�����\���Ώ��r�̏ꍇ�͑������M
			// ���d���בΏ��r���A���������ɁA�܂Ƃߑ��M�@�\�ɂ��
			// �@�d���׈ꗗ�����\���Ώ��r���j������Ă��܂����΍�
			if (1 == wkBuf.Data.nModeDeflist)
			{
				// �����������d���׈ꗗ�����\���Ώ��r���𑗐M
				OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &wkBuf);

//#ifdef DEF_DETECT_LOG
				LOG(em_INF), "[%s] �r���M(�����o���d���׈ꗗ�����\���Ώ��r�F�����r) kizuNo=%d �r��:%d �O���[�h:%d col:%d", my_sThreadName,
					wkBuf.Data.nKizuNo, wkBuf.Data.nTid, wkBuf.Data.nGid, col);
//#endif
			}

			// �ꎞ�̈���
			for (int ii = 0; ii < MAX_CAMANGLE; ii++)
			{
				if (0 < wkBuf.nImgsize[ii])
				{
					SAFE_DELETE_ARRAY(wkBuf.pImg[ii]);
					wkBuf.pImg[ii] = NULL;
				}
			}
			memset(&wkBuf, 0x00, sizeof(wkBuf));
		}
	}

	//================================================
	// �}��
	if (-1 != nIndex)
	{
		if (MAP_NEST_NUM == pColRow_Info->nSetCnt[col])
		{
			// ���X�gMAX�̏ꍇ

			// ���X�g���略���o�����Ŏ��r�̋������M
			// �d���׈ꗗ�����\���Ώ��r�̏ꍇ�͑������M
			// ���d���בΏ��r���A���������ɁA�܂Ƃߑ��M�@�\�ɂ��
			// �@�d���׈ꗗ�����\���Ώ��r���j������Ă��܂����΍�
			if (1 == pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nModeDeflist)
			{
				// �����������d���׈ꗗ�����\���Ώ��r���𑗐M
				OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1]);
//#ifdef DEF_DETECT_LOG
				LOG(em_INF), "[%s] �r���M(�����o���d���׈ꗗ�����\���Ώ��r�F���X�g�r) kizuNo=%d �r��:%d �O���[�h:%d col:%d", my_sThreadName,
					pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nKizuNo,
					pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nTid,
					pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nGid, col);
//#endif
			}

			// �Ŏ��r���J��
			pColRow_Info->nSetCnt[col]--;
			for (int ii = 0; ii < MAX_CAMANGLE; ii++)
			{
				if (0 < pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].nImgsize[ii])
				{
					SAFE_DELETE_ARRAY(pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].pImg[ii]);
					pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].pImg[ii] = NULL;
				}
			}
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] ���X�g�r�폜 kizuNo=%d �r��:%d �O���[�h:%d col:%d", my_sThreadName,
				pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nKizuNo, pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nTid, pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nGid, col);
#endif
			memset(&pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1], 0x00, sizeof(TYP_COLROW_BUF));
		}

		// �V�f�[�^���������ވʒu��艺�ʃf�[�^���ړ�
		// �����̈���Ȃ̂ŁA�����珇��
		for (int ii = MAP_NEST_NUM - 1; ii >= nIndex; ii--)
		{
			if (MAP_NEST_NUM - 1 == ii) continue;
			memcpy(&pColRow_Info->BufDefAry[col][ii + 1], &pColRow_Info->BufDefAry[col][ii], sizeof(TYP_COLROW_BUF));
		}

		if (2 == nDataSet)
		{
			// �V������

			// ��\����ǂ��o���ꂽ�r��o�^
			memcpy(&pColRow_Info->BufDefAry[col][nIndex], &wkBuf, sizeof(TYP_COLROW_BUF));
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] ���X�g�r�o�^(��\�r) kizuNo=%d �r��:%d �O���[�h:%d col:%d", my_sThreadName,
				pColRow_Info->BufDefAry[col][nIndex].Data.nKizuNo, pData->nTid, pData->nGid, col);
#endif
		}
		else if (3 == nDataSet)
		{
			// �V���ア

			// �����r��o�^
			DetectToColRow(&pColRow_Info->BufDefAry[col][nIndex], dLen, pData, pImg);

#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] ���X�g�r�o�^(�����r) kizuNo=%d �r��:%d �O���[�h:%d col:%d", my_sThreadName,
				pColRow_Info->BufDefAry[col][nIndex].Data.nKizuNo, pData->nTid, pData->nGid, col);
#endif
		}
	}

	//================================================
	//// �\���Ώۂ̌��׌�₪�L�����̂ŁA�������Z
	if (-1 != nIndex) pColRow_Info->nSetCnt[col]++;
	if (MAP_NEST_NUM < pColRow_Info->nSetCnt[col]) pColRow_Info->nSetCnt[col] = MAP_NEST_NUM;

	return 0;
}

//------------------------------------------
// ��\�r�I��
// DETECT_BASE_DATA* pNow	�����r
// DETECT_BASE_DATA* pPre	��r�Ώ��r
//------------------------------------------
bool HtRecv::SelectPriorityDetect(DETECT_BASE_DATA const* pNow, DETECT_BASE_DATA const* pPre)
{
	const int cnCnt = 4;
	bool   bBig[cnCnt] = { false,		false,			true,		false };	// ��r�p��� (Gr[��]�A�r��[��]�A�ʐ�[��]�A����ʒu[��])
	double dNow[cnCnt] = { (double)pNow->nGYuu, (double)pNow->nTYuu, pNow->aria, pNow->y };
	double dPre[cnCnt] = { (double)pPre->nGYuu,	(double)pPre->nTYuu, pPre->aria, pPre->y };

	return KizuFunction::SelectPriorityDetect(cnCnt, dNow, dPre, bBig);
}
