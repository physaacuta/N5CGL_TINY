#include "StdAfx.h"
#include "DBManager.h"
#include "ToLinkswUtil.h"
#include <float.h>
#include <limits.h>
// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInstance.h"

#define DBMGR_INSERT_S(x)	CreateSQL::sqlColumnInsert(&sql, x, sizeof(x))		// ������p
#define DBMGR_INSERT_V(x)	CreateSQL::sqlColumnInsert(&sql, x)					// ������ȊO�p
#define DBMGR_INSERT_N(x,y) CreateSQL::sqlColumnInsert(&sql, x, y)				// ���̑��p

// �s���Ȓl����BCP�����s����̂ŁA�t�B���^����
//�@NaN -> 0
//  +�� -> +1.79e308
//  -�� -> -1.79e308
//  ����Ȓl -> ���̂܂�.
#ifndef NOT_FILTER_NAN_INF
inline static double filter_nan_inf( double v )
{
	if( _isnan(v) ){	//Not A Number
		return 0;
	}
	if( !_finite(v) ){ //��
		if( v<0 ) return -DBL_MAX;
		if( v>0 ) return +DBL_MAX;
	}
	return v;
}
#else
inline static double filter_nan_inf( double v )
{
	return v;
}
#endif

#ifndef NOT_FILTER_SHRT
inline static short filter_short( int nVal, int nMin, int nMax )
{
	if( nVal < nMin ) return nMin;
	if( nVal > nMax ) return nMax;
	return nVal;
}
#else
inline static short filter_short( int nVal, int nMin, int nMax )
{
	return nVal;
}
#endif

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
DBManager::DBManager(void) :
DbBaseManager("DBMgr")
{

	// �@���Ԗ߂�
	KizuFunction::SetStatus("BCP_ERR_ID",			true, false);	// �摜�����ݎ��s
	KizuFunction::SetStatus("DB_SAVE_MAX_1_ID",		true, false);	// �������ѕۑ����
	KizuFunction::SetStatus("DB_SAVE_MAX_2_ID",		true, false);	// �������ѕۑ����
	KizuFunction::SetStatus("DB_SAVE_SKIP_1_ID",	true, false);	// �������ѕۑ��X�L�b�v
	KizuFunction::SetStatus("DB_SAVE_SKIP_2_ID",	true, false);	// �������ѕۑ��X�L�b�v
	this->SetSaveSkipId(0, GetPrivateProfileInt(INI_STATUS, "DB_SAVE_SKIP_1_ID", 0, TASKINI_NAME) );
	this->SetSaveSkipId(1, GetPrivateProfileInt(INI_STATUS, "DB_SAVE_SKIP_2_ID", 0, TASKINI_NAME) );
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DBManager::~DBManager(void)
{
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p��������R�[���o�b�N

//==========================================
// �֐�������
//------------------------------------------
// bool		bMode	: BCP��ԕύX
//==========================================
void DBManager::OnAlloc(bool bMode)
{
	mcls_Bcps.SetLogMgr( mcls_pLog );
	mcls_Bcps.SetStatusMgr(mcls_pStatus);
	mcls_Bcps.Alloc();
	mcls_Bcps.Start();

	//// �擪�̃C���X�^���X�̂� DB�폜�Ǘ��ɒʒm����Ƃ���
	mcls_pDbs[0]->SetSendDbManager(false);
	this->Send_To_DbManage(true);			// �����ݗD��𑗐M
}

//==========================================
// �֐��J��
//==========================================
void DBManager::OnFree()
{
	mcls_Bcps.Stop();
	mcls_Bcps.Free();
}


//==========================================
// DB�o�^�˗�
//------------------------------------------
// int		emNo	: ENUM_KUBUN�̔ԍ�
// BYTE*	pData	: �f�[�^�� (�p�����ŊJ���������s���̂ł����ł́A�J������K�v�Ȃ�)
//==========================================
void DBManager::OnQueDb(int emNo, BYTE* pData)
{
	//// �敪�ɂ�菈������
	switch(emNo) {
		//-----------------------------------------------------------
		case em_COILINF:												// �R�C�����
			DbBaseManager::ChangeSQLInf(((DBDATA_COIL_INF*)pData)->cKizukenNo);		// �o�b�t�@�ؑ�
			CreateSQL_CoilInf( (const DBDATA_COIL_INF*)pData);
			break;

		//-----------------------------------------------------------
		case em_COILRESULT:												// �R�C������
			CreateSQL_CoilResult( (const DBDATA_COIL_RESULT*)pData );
			break;

		//-----------------------------------------------------------
		case em_COILRESULT_UPDATE:										// �R�C�����уA�b�v�f�[�g
			CreateSQL_CoilResultUpdate( (const DBDATA_COIL_RESULT_UPDATE*)pData );
			break;

		//-----------------------------------------------------------
		case em_COILPOSITION:											// �R�C���ʒu
			CreateSQL_CoilPosition( (const DBDATA_COIL_POSITION*)pData);
			break;

		//-----------------------------------------------------------
		case em_COILTRAND:												// �R�C���g�����h
			CreateSQL_CoilTrand( (const DBDATA_COIL_TRAND*)pData );
			break;

		//-----------------------------------------------------------
		case em_COILCUTPOS:												// �R�C���J�b�g�ʒu
			CreateSQL_CoilCutPos( (const DBDATA_COIL_CUT_POS*)pData );
			break;

		//-----------------------------------------------------------
		case em_DEFECTINF:												// �r���
#ifndef NON_DB_SAVE
			if (!CreateSQL_DefectInf( (const DBDATA_DEFECT_INF*)pData ))
			{
				// �r�ۑ�����ɒB���Ă����ꍇ�͉摜���N���A����
				// ���ȑO�͂��̃N���A���Ȃ����߁A�摜�̈敪���������[�N���Ă���
				for (int ii = 0; ii<MAX_CAMANGLE; ii++) { SAFE_DELETE_ARRAY(((DBDATA_DEFECT_INF*)pData)->pImg[ii]); }
			}
#endif
			break;

		//-----------------------------------------------------------
		case em_EDGE_COIL_INF:											// �G�b�W���o���уe�[�u��
			CreateSQL_EdgeCoilInf( (const DBDATA_EDGE_COIL_INF*)pData );
			break;

		//-----------------------------------------------------------
		case em_EDGE_CHANGE_INF:										// �G�b�W���o���[�h�ω��Ǘ��e�[�u��
			CreateSQL_EdgeChangeInf( (const DBDATA_EDGE_CHANGE_INF*)pData );
			break;
			
		//-----------------------------------------------------------
		case em_CAMLINKSW:												// �J���������N�X�C�b�`�ݒ�
			CreateSQL_CamLinkSW( (const DBDATA_CAMERA_LINKSWITCH*)pData );
			break;

		//-----------------------------------------------------------
		case em_PULSE_RATE_UPDATE:										// �V�[�P���T����p���X���[�g�ύX
			CreateSQL_PulseRateUpdate( (DBDATA_PULSE_RATE_UPDATE*)pData);
			break;

		//-----------------------------------------------------------
		case em_DB_ADD_SYNC:											// �o�^�󋵖₢���킹
			Add_Synclock( (const DBDATA_ADD_SYNC*)pData );
			break;

// ------ MainConst�p ------------>>>
		case ENUM_DB_KUBUN::em_STATUS_LOG :								// �X�e�[�^�X���O
			CreateSQL_StatusLog( (const DBDATA_STATUS_LOG*)pData );
			break;
// <<<------------------------------

	}
}

//==========================================
// ���Z�b�g�˗�
//==========================================
void DBManager::OnReSetMaxCount()
{
	mcls_Bcps.ReSetMaxCount();
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQL�����֘A

//==========================================
// �R�C�����
//------------------------------------------
// const DBDATA_COIL_INF* pWk : �������	
//==========================================
void DBManager::CreateSQL_CoilInf(const DBDATA_COIL_INF* pWk)
{
	CString sql;

	//=================================
	////// �R�C�����
	// �w�b�_�[��
	sql  = "INSERT INTO ";
	sql += DB_COIL_INF;
	sql += " VALUES(";

	// �f�[�^��
	//------------------------------------------------------------------>>>
	DBMGR_INSERT_S(pWk->cKizukenNo);				// �Ǘ�No
	CreateSQL::sqlColumnAddTime(&sql, true);		// �o�^���t
	DBMGR_INSERT_S(pWk->cCoilKey);					// �L�[���
	DBMGR_INSERT_S(pWk->cMeiNo);					// ����No
	DBMGR_INSERT_S(pWk->cCoilNo);					// ���YNo
	DBMGR_INSERT_V(pWk->nSendPoint);				// ���M�ʒu
	DBMGR_INSERT_V(pWk->nLenIn);					// �R�C����
	DBMGR_INSERT_V(pWk->nAtuZai);					// �ޗ���
	DBMGR_INSERT_V(pWk->nWidZai);					// �ޗ���
	DBMGR_INSERT_V(pWk->nAtuSei);					// ������
	DBMGR_INSERT_V(pWk->nWidSei);					// ������
	DBMGR_INSERT_V(pWk->nWidTrim);					// ���H���g������
	DBMGR_INSERT_V(pWk->nNobiSPM);					// SPM�L�ї�
	DBMGR_INSERT_V(pWk->nNobiTLV);					// TLV�L�ї�
	DBMGR_INSERT_V(pWk->nCutF);						// �����ɂ�����t�����g�[�J�b�g�̎��ђ�
	DBMGR_INSERT_V(pWk->nCutT);						// �����ɂ�����e�[���[�J�b�g�̎��ђ�
	DBMGR_INSERT_V(pWk->nSPMRoolKei_Up);			// SPM���[�N���[���a (��)
	DBMGR_INSERT_V(pWk->nSPMRoolKei_Down);			// SPM���[�N���[���a (��)
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_10
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_9
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_8
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_7
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_6
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_5
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_4
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_3
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_2
	DBMGR_INSERT_N(NULL, 0);						// �\��_int_1
	DBMGR_INSERT_S(pWk->cKenKijun_O);				// ����� �\
	DBMGR_INSERT_S(pWk->cKenKijun_U);				// ����� ��
	DBMGR_INSERT_S(pWk->cKurometo_Kind);			// �N�����[�g ���ю��
	DBMGR_INSERT_S(pWk->cKurometo_Umu);				// �N�����[�g ���їL��
	DBMGR_INSERT_S(pWk->cHinCode);					// �i��
	DBMGR_INSERT_S(pWk->cMekkiKind);				// ���b�L���
	DBMGR_INSERT_S(pWk->cSyuZai);					// ��ގ�
	DBMGR_INSERT_S(pWk->cDifMaker);					// �f�B�t�@�����V�����}�[�J�[
	DBMGR_INSERT_S(pWk->cKenban);					// ���敪
	DBMGR_INSERT_S(pWk->cMekkiCycle);				// ���b�L�T�C�N��
	DBMGR_INSERT_S(pWk->cInLineSPM);				// ���x���[ �C�����C��SPM
	DBMGR_INSERT_S(pWk->cKagakuToyu);				// ���w�����h��
	DBMGR_INSERT_S(pWk->cAtukkaSPM);				// �I�t���C��SPM������
	DBMGR_INSERT_S(pWk->cMekkiKubun);				// ���b�L�敪
	DBMGR_INSERT_S(pWk->cOmoteSyori1);				// �\�ʏ���1����
	DBMGR_INSERT_S(pWk->cOmoteSyori2);				// �\�ʏ���2����
	DBMGR_INSERT_S(pWk->cToyu);						// �h��
	DBMGR_INSERT_S(pWk->cKouteiYobi);				// �H���L���\��
	DBMGR_INSERT_S(pWk->cBaund);					// �o�E���_�����킹
	DBMGR_INSERT_S(pWk->cCaplSyuZai);				// CAPL ��ގ�
	DBMGR_INSERT_S(pWk->cMetuke);					// �ڕt�K�i
	DBMGR_INSERT_S(pWk->cNi);						// Ni�t����
	DBMGR_INSERT_S(pWk->cSodoTCM);					// TCM���[���e�x
	DBMGR_INSERT_S(pWk->cSpmUmu);					// SPM�����L��
	DBMGR_INSERT_S(pWk->cNextLine);					// �v�掟�H���R�[�h
	DBMGR_INSERT_S(pWk->cMuke);						// ����
	DBMGR_INSERT_S(pWk->cYouto);					// �p�r
	DBMGR_INSERT_S(pWk->cCaplSiage);				// CAPL�\�ʎd�グ
	DBMGR_INSERT_S(pWk->cCaplSanUmu);				// CAPL�_���L��
	DBMGR_INSERT_S(pWk->cHinMode);					// �i��敪
	DBMGR_INSERT_S(pWk->cGaCrKubun);				// GA/CR�敪
	DBMGR_INSERT_S(pWk->cSpmJisekiUmu);				// SPM�������їL��
	DBMGR_INSERT_S(pWk->cKouteiCode);				// �H���R�[�h
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_10
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_9
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_8
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_7
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_6
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_5
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_4
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_3
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_2
	DBMGR_INSERT_N(NULL, 0);						// �\��_char_1
	//<<<------------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);					// �Ō�̃J���}����菜��

	sql += ")";

	//==================================================
	//// ��������SQL���� DB�o�^�X���b�h�ɓo�^�˗�
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= sql;
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;


	// �˗�
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ){		// ��Γo�^
		// ��Γo�^�������c
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_COIL_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_INF]");
		SAFE_DELETE(pDeli->SendMailItem);
		delete pDeli;
		// DB���ُ�(�ʏ킠�肦�Ȃ�)
		mcls_pStatus->AddQueMyErrWar(true, ON_SAVE_COIL_INF);
	}
}

//==========================================
// �R�C������
//------------------------------------------
// const DBDATA_COIL_RESULT* pWk : �������
//==========================================
void DBManager::CreateSQL_CoilResult(const DBDATA_COIL_RESULT* pWk)
{
	CString sql;

	//======================================
	//// ���O����
	int infRec = GetRecSQLInf(pWk->cKizukenNo);

	//// �o�b�t�@���R�[�h����
	if( -1 == infRec ){
		// ���莸�s
		LOG(em_ERR), "[%s] �ۑ����R�C���o�b�t�@�ɖ��� [%s]", my_sThreadName, pWk->cKizukenNo);
		syslog(280, "[%s:DBDATA_COIL_RESULT]", pWk->cKizukenNo);
		return;
	}

	//// �����R�炵�r�L��H
	if( 0 != m_SqlInf[infRec].nDeleCnt[0] ||
		0 != m_SqlInf[infRec].nDeleCnt[1] ||
		0 != m_SqlInf[infRec].nLowGrDeleCnt[0] ||
		0 != m_SqlInf[infRec].nLowGrDeleCnt[1] ) {

		int nMsgNo = (0<(m_SqlInf[infRec].nDeleCnt[0]+m_SqlInf[infRec].nDeleCnt[1])) ? 282 : 281;
		LOG(em_WAR), "[%s] DB�����R�炵���� [%s][�L�Q=%d:%d / ���Q=%d:%d]", my_sThreadName, pWk->cKizukenNo, 
			m_SqlInf[infRec].nDeleCnt[0], m_SqlInf[infRec].nDeleCnt[1], m_SqlInf[infRec].nLowGrDeleCnt[0], m_SqlInf[infRec].nLowGrDeleCnt[1]);
		syslog(nMsgNo, "[%s; �L�Q=%d:%d / ���Q=%d:%d]", pWk->cKizukenNo, 
			m_SqlInf[infRec].nDeleCnt[0], m_SqlInf[infRec].nDeleCnt[1], m_SqlInf[infRec].nLowGrDeleCnt[0], m_SqlInf[infRec].nLowGrDeleCnt[1]);

	}

	//// ������B����
	DbBaseManager::SaveMaxReset(infRec);

	// �J�������ѓo�^
	CreateSQL_CameraResult(pWk);

	//======================================
	//// BCP (�R�C���P�ʂŎc���Ă���摜��������)
	//OnBcpAutoCommit(false);
	
	BCPSaveManager::DeliveryDB *deliDB = new BCPSaveManager::DeliveryDB;
	deliDB->kubun = BCPSaveManager::em_BCP_BATCH;
	deliDB->data = (BYTE*)new DBDATA_BCP_BATCH_EXEC;
	DBDATA_BCP_BATCH_EXEC* pData = (DBDATA_BCP_BATCH_EXEC*)deliDB->data;
	
	pData->bMode = false;


	//================================================
	//// �o�^�˗�
	if (!mcls_Bcps.gque_AddSQLData.AddToTail(deliDB, 20, 20)) {
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [BCP_BATCH_EXEC]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[BCP_BATCH_EXEC]");

		delete deliDB->data;
		delete deliDB;
	}

	//======================================
	//// �R�C������
	// �w�b�_�[��
	sql  = "INSERT INTO ";
	sql += DB_COIL_RESULT;
	sql += " VALUES(";

	// �f�[�^��
	//------------------------------------------------------------------>>>
	DBMGR_INSERT_S(pWk->cKizukenNo);											// �Ǘ�No
	CreateSQL::sqlColumnAddTime(&sql, true);									// �o�^���t
	DBMGR_INSERT_V(pWk->nEndMode);												// ���ъi�[�敪
	for(int ii=0; ii<NUM_MEN; ii++)	DBMGR_INSERT_V(pWk->nSt_Hantei[ii]);		// ������
	for(int ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V(pWk->nSt_Light[ii]);			// �Ɩ����u���
	DBMGR_INSERT_V(pWk->nSt_CamControl);										// �J�����R���g���[����H���
	DBMGR_INSERT_V(pWk->nSt_CamSwitch);											// �J���������N�X�C�b�`��H���
	DBMGR_INSERT_V(pWk->nSt_Procon);											// �v���R�����
	DBMGR_INSERT_V(pWk->nSt_FF);												// FF���
	DBMGR_INSERT_V(pWk->nSt_Sqence);											// �V�[�P���T���
	for(int ii=0; ii<NUM_MEN; ii++)	DBMGR_INSERT_V(pWk->nSt_Cam_Inline[ii]);	// �J������ԏ��[�\��]
	for(int ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V(pWk->nSt_DbSkip[ii]);		// DB���ۑ�[�\��]
	for(int ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V(pWk->nSt_FrameSkip[ii]);		// ������[�\��]

	// �g�p����
	CString sWkO;	CString sWkU;	CString sWk;
	sWkO = "";		sWkU = "";
	for(int ii=0; ii<NUM_HANTEI; ii++){
		sWk.Format("%02d", pWk->nHtID[0][ii]);	sWkO += sWk;
		sWk.Format("%02d", pWk->nHtID[1][ii]);	sWkU += sWk;
	}
	DBMGR_INSERT_N(sWkO, NUM_HANTEI*2);											// �g�p����_�\
	DBMGR_INSERT_N(sWkU, NUM_HANTEI*2);											// �g�p����_��
	
	// �S���g�p����
	sWkO = "";		sWkU = "";
	for(int ii=0; ii<NUM_RPIEXEC; ii++){
		sWk.Format("%02d", pWk->nRpiID[0][ii]);	sWkO += sWk;
		sWk.Format("%02d", pWk->nRpiID[1][ii]);	sWkU += sWk;
	}
	DBMGR_INSERT_N(sWkO, NUM_RPIEXEC *2);										// �g�p�S��_�\
	DBMGR_INSERT_N(sWkU, NUM_RPIEXEC *2);										// �g�p�S��_��

	DBMGR_INSERT_V( (pWk->bStartWpdOk?0:1) );									// ������_�M�����o��
	DBMGR_INSERT_V(0);															// ���QGr�폜�敪
	DBMGR_INSERT_V(0);															// �����T�[�o�[�擾�敪
	DBMGR_INSERT_V( (pWk->setting.bWalm?1:0) );									// �E�H�[�}�[�ދ敪
	DBMGR_INSERT_V( pWk->nCoilCnt );											// �����{��
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_10
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_9
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_8
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_7
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_6
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_5
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_4
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_3
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_2
	DBMGR_INSERT_N(NULL, 0);													// �@���ԗ\��_1
	DBMGR_INSERT_V( (pWk->setting.bDefScode[0]?1:0) );							// �����\�ʏ��No_Def�敪_�\
	DBMGR_INSERT_V( (pWk->setting.bDefScode[1]?1:0) );							// �����\�ʏ��No_Def�敪_��
	DBMGR_INSERT_V( (pWk->setting.bDefPcode[0]?1:0) );							// �����p�^�[���R�[�h_Def�敪_�\
	DBMGR_INSERT_V( (pWk->setting.bDefPcode[1]?1:0) );							// �����p�^�[���R�[�h_Def�敪_��
	DBMGR_INSERT_V( pWk->setting.scode[0] );									// �����\�ʏ��No_�\
	DBMGR_INSERT_V( pWk->setting.scode[1] );									// �����\�ʏ��No_��
	DBMGR_INSERT_V( pWk->setting.pcode[0] );									// �����p�^�[���R�[�h_�\
	DBMGR_INSERT_V( pWk->setting.pcode[1] );									// �����p�^�[���R�[�h_��

	// �ӃւŊǗ�����f�[�^�����A���Update
	DBMGR_INSERT_V(pWk->nSpmStat);												// SPM�����L��
	DBMGR_INSERT_V(pWk->nLength);												// �������u������
	DBMGR_INSERT_V(pWk->nLengthSub);											// ������r������
	DBMGR_INSERT_V(pWk->nLengthDsp);											// �\��������
	DBMGR_INSERT_V(pWk->nOutCoilPos);											// �o���P�ʐ�[�ʒu

	DBMGR_INSERT_V( m_SqlInf[infRec].nTotalCnt[0] );							// �\�ۑ��Ώ��r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nTotalCnt[1] );							// ���ۑ��Ώ��r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nDeleCnt[0] );								// �\�폜�L�Q�r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nDeleCnt[1] );								// ���폜�L�Q�r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nLowGrDeleCnt[0] );						// �\�폜���Q�r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nLowGrDeleCnt[1] );						// ���폜���Q�r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nImgDeleCnt[0] );							// �\�摜�폜�L�Q�r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nImgDeleCnt[1] );							// ���摜�폜�L�Q�r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nImgLowGrDeleCnt[0] );						// �\�摜�폜���Q�r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nImgLowGrDeleCnt[1] );						// ���摜�폜���Q�r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nSaveCnt[0] );								// �\�ۑ��r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nSaveCnt[1] );								// ���ۑ��r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nSaveImgAllCnt[0] );						// �\�摜�ۑ��r��
	DBMGR_INSERT_V( m_SqlInf[infRec].nSaveImgAllCnt[1] );						// ���摜�ۑ��r��
	DBMGR_INSERT_V( pWk->emUnten );												// �^�]���
	DBMGR_INSERT_V( pWk->emOneKensa[0] );										// �������_�\
	DBMGR_INSERT_V( pWk->emOneKensa[1] );										// �������_��
	DBMGR_INSERT_V( pWk->emOneKiki[0] );										// �@����_�\
	DBMGR_INSERT_V( pWk->emOneKiki[1] );										// �@����_��

	DBMGR_INSERT_V( pWk->emPlg );												// PLG�I���敪
	DBMGR_INSERT_V( pWk->nResultSendOk );										// �O�����ё��M����
	
	// �ʃ^�X�N��Update
	DBMGR_INSERT_V(1.0);														// �␳��
	DBMGR_INSERT_V( 1);															// �v���R�����є��f
	DBMGR_INSERT_V( 0);															// �v���R��������
	DBMGR_INSERT_V( 0);															// ���i��

	DBMGR_INSERT_V( pWk->nToyuSply );											// �h���X�v���[���u���
	//<<<------------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);					// �Ō�̃J���}����菜��

	sql += ")";


	//======================================================	
	//// ��������SQL���� DB�o�^�X���b�h�ɓo�^�˗�����
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= sql;
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;

	//======================================================	
	// �R�C�����ѓo�^�������� �r�g�����h��� �� ��ʍX�V�ʒm�𑗂�
	pDeli->SendMailItem = NULL;
	pDeli->SendMailItem = new MAIL_ITEM;
	memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
	strcpy(pDeli->SendMailItem->cPc, "*");
	strcpy(pDeli->SendMailItem->cTask, SO_TRAND);
	pDeli->SendMailItem->Que.nEventNo = FACT_SO_TRAND_01;

	// �˗�
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_COIL_RESULT]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_RESULT]");
		SAFE_DELETE(pDeli->SendMailItem);
		delete pDeli;
		// DB���ُ� (�ʏ킠�肦�Ȃ�)
		mcls_pStatus->AddQueMyErrWar(true, ON_SAVE_COIL_RESULT);
	} else {
		syslog(283, "[%s] �R�C������", pWk->cKizukenNo);
	}
}

//==========================================
// �J��������
//------------------------------------------
// const DBDATA_COIL_RESULT* pWk : �������
//==========================================
void DBManager::CreateSQL_CameraResult(const DBDATA_COIL_RESULT* pWk)
{
	CString sql;

	//======================================
	//// �R�C������
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += DB_COIL_CAMERA;
	sql += " VALUES(";

	// �f�[�^��
	//------------------------------------------------------------------>>>
	DBMGR_INSERT_S(pWk->cKizukenNo);									// �Ǘ�No
	// �P�x
	for (int ii = 0; ii < MAX_CAMSET; ii++)
	{
		for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
		{
			DBMGR_INSERT_V(pWk->nBr[ii][jj][0]);						// �P�x(C)
		}
	}
	// �I������
	for (int ii = 0; ii < MAX_CAMSET; ii++)
	{
		for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
		{
			DBMGR_INSERT_V(pWk->nExp[ii][jj]);							// �I������[clk]
		}
	}
	// �Q�C��
	for (int ii = 0; ii < MAX_CAMSET; ii++)
	{
		for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
		{
			DBMGR_INSERT_V(pWk->nGain[ii][jj]);							// �Q�C��[�{]
		}
	}
	//<<<------------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);					// �Ō�̃J���}����菜��

	sql += ")";

	//======================================================	
	//// ��������SQL���� DB�o�^�X���b�h�ɓo�^�˗�����
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql = sql;
	pDeli->addmode = true;
	pDeli->bCalcTime = false;
	pDeli->cnt = 0;

	// �˗�
	if (!mque_AddDB.AddToTail(pDeli, 20, 20)) {		// ��Γo�^
													// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_COIL_CAMERA]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_CAMERA]");
		SAFE_DELETE(pDeli->SendMailItem);
		delete pDeli;
		// DB���ُ� (�ʏ킠�肦�Ȃ�)
		mcls_pStatus->AddQueMyErrWar(true, ON_SAVE_COIL_RESULT);
	}
	else {
		syslog(283, "[%s] �J��������", pWk->cKizukenNo);
	}
}

//==========================================
// �R�C�����уA�b�v�f�[�g �i���ъm����UpDate�p�j
//------------------------------------------
// const DBDATA_COIL_RESULT_UPDATE* pWk	: �������
//==========================================
void DBManager::CreateSQL_CoilResultUpdate(const DBDATA_COIL_RESULT_UPDATE* pWk)
{
	CString sql;	

	// /////////////////////////////////
	// ���R�[�h����
	int infRec = GetRecSQLInf(pWk->cKizukenNo);
	if( -1 == infRec ) {
		LOG(em_ERR), "[%s] �ۑ����R�C���o�b�t�@�ɖ��� [%s]", my_sThreadName, pWk->cKizukenNo );
		syslog(280, "[%s]", pWk->cKizukenNo);
		return;
	}

	//======================================================	
	////// �R�C������
	// �w�b�_�[��
	sql = "UPDATE ";
	sql += DB_COIL_RESULT;
	sql += " SET ";	

	// -------------------------------------------------------------------------------------------->>>
	// �f�[�^��

	// �V���[�J�b�g�M����M
	if(MODE_RECV_SHRCUT == pWk->emMode) {
		CreateSQL::sqlColumnUpdate(&sql, "�o���P�ʐ�[�ʒu",	pWk->nOutCoilPos);					// �o���P�ʐ�[�ʒu [mm]
	}
	// �\����_WPD ON
	else if(MODE_DSP_WPD_ON == pWk->emMode) {
		CreateSQL::sqlColumnUpdate(&sql, "�\��������",			pWk->nDspLength);					// �\�������� [mm]
		CreateSQL::sqlColumnUpdate(&sql, "SPM�����L��",			pWk->nSpmUmu);
	}
	// <<<--------------------------------------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);		// �Ō�̃J���}����菜��

	// Where
	sql += " WHERE ";
	CreateSQL::sqlColumnUpdate(&sql, "�Ǘ�No", pWk->cKizukenNo, SIZE_KIZUKEN_NO);					// �Ǘ�No 
	CreateSQL::sqlColumnTrim(&sql);		// �Ō�̃J���}����菜��


	//======================================================	
	//// ��������SQL���� DB�o�^�X���b�h�ɓo�^�˗�����
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= sql;
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;

	//======================================================	
	// �R�C�����эX�V�������� �r�g�����h��� �� ��ʍX�V�ʒm�𑗂�
	pDeli->SendMailItem = NULL;
	pDeli->SendMailItem = new MAIL_ITEM;
	memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
	strcpy(pDeli->SendMailItem->cPc, "*");
	strcpy(pDeli->SendMailItem->cTask, SO_TRAND);
	pDeli->SendMailItem->Que.nEventNo = FACT_SO_TRAND_01;

	// �˗�
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_RESULT_UPDATE]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_RESULT_UPDATE]");
		delete pDeli;
		// DB���ُ� (�ʏ킠�肦�Ȃ�)
		mcls_pStatus->AddQueMyErrWar(true, ON_SAVE_COIL_RESULT);
	}
}

//==========================================
// �R�C���ʒu
//------------------------------------------
// const DBDATA_COIL_POSITION* pWk	: �������
//==========================================
void DBManager::CreateSQL_CoilPosition(const DBDATA_COIL_POSITION* pWk)
{
	CString sql;

	//========================================================
	////// �R�C���ʒu���
	// �w�b�_�[��
	sql  = "INSERT INTO ";
	sql += DB_COIL_POSITION;
	sql += " VALUES(";

	//------------------------------------------------------->>>
	// �f�[�^��
	DBMGR_INSERT_S(pWk->cKizukenNo);											// �Ǘ�No
	DBMGR_INSERT_V(pWk->nID);													// ID
	for (int ii = 0; ii < NUM_MEN; ii++)DBMGR_INSERT_V(pWk->emKensa[ii]);		// �������
	for (int ii = 0; ii < NUM_MEN; ii++)DBMGR_INSERT_V(pWk->emKiki[ii]);		// �@����
	for (int ii = 0; ii < 2; ii++)		DBMGR_INSERT_V(pWk->dEdge[ii]);			// �G�b�W�ʒu��[0:��/1:�E]
	DBMGR_INSERT_V(pWk->dWalk);													// �E�H�[�N��
	//<<<-------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);												// �Ō�̃J���}����菜��

	sql += ")";

	//======================================================	
	//// ��������SQL���� DB�o�^�X���b�h�ɓo�^�˗�����
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= sql;
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;


	// �˗�
	if( ! mque_AddDB.AddToTailFreeCheck(pDeli, 10) ) {
		// �L���[�t��
		delete pDeli;
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_COIL_POSITION]", my_sThreadName);
	}

}

//==========================================
// �R�C���g�����h
//------------------------------------------
// const DBDATA_COIL_TRAND* pWk	: �������
//==========================================
void DBManager::CreateSQL_CoilTrand(const DBDATA_COIL_TRAND* pWk)
{
	CString sql;

	//===========================================
	//// �R�C���g�����h���
	// �w�b�_�[��
	sql  = "INSERT INTO ";
	sql += DB_COIL_TRAND;
	sql += " VALUES(";

	//----------------------------------------------------------------------->>>
	// �f�[�^��
	DBMGR_INSERT_S(pWk->cKizukenNo);							// �Ǘ�No
	DBMGR_INSERT_V(pWk->nMen);									// �\���敪
	double dLen = (double)pWk->nLen * pWk->dSpmHosei;
	DBMGR_INSERT_V((int)dLen);										// �擾�ʒu
	CreateSQL::sqlColumnAddTime(&sql, true);					// �o�^���t
	DBMGR_INSERT_V(pWk->nSpeed);								// ���C�����x
	DBMGR_INSERT_V(pWk->dCoilWidMax);							// ��Min
	DBMGR_INSERT_V(pWk->dCoilWidMin);							// ��Max
	DBMGR_INSERT_V(pWk->dWalk);									// �E�H�[�N��
	// �Ɩ����u_�����l[0:R 1:G1 2:G2 3:B1 4:B2]
	for (int ii = 0; ii < DIV_LED_COLOR_END; ii++)		 DBMGR_INSERT_V(pWk->nLightValue[ii]);
	// �P�x[0:DS�`9:WS][0:R 1:G 2:B 3:G-B][0:C]
	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) DBMGR_INSERT_V(pWk->nBr[ii][jj][0]);
	}
	// �I������[0:DS�`9:WS][0:R 1:G 2:B 3:G-B]
	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) DBMGR_INSERT_V(filter_short(pWk->nExp[ii][jj], SHRT_MIN, SHRT_MAX));
	}
	// �J�����Q�C���l[0:DS�`9:WS][0:R 1:G 2:B 3:G-B]
	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) DBMGR_INSERT_V(pWk->dGain[ii][jj]);
	}
	//<<<-----------------------------------------------------------------------
	CreateSQL::sqlColumnTrim(&sql);								// �Ō�̃J���}����菜��
	sql += ")";

	//// �˗�
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTailFreeCheck(pDeli, 30) ) {
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DB_COIL_TRAND]", my_sThreadName);
		delete pDeli;
	}
}

//==========================================
// �R�C���J�b�g�ʒu
//------------------------------------------
// const DBDATA_COIL_CUT_POS* pWk : �������
//==========================================
void DBManager::CreateSQL_CoilCutPos(const DBDATA_COIL_CUT_POS* pWk)
{
	CString sql;

	////// �R�C���J�b�g�ʒu���
	for (int ii = 0; ii<pWk->data.nCnt; ii++)
	{
		// �w�b�_�[��
		sql  = "INSERT INTO ";
		sql += DB_COIL_CUT_POS;
		sql += " VALUES(";

		//---------------------------------------------------------------->>>
		// �f�[�^��
		DBMGR_INSERT_S(pWk->data.incoil[ii].cKizukenNo);	// �Ǘ�No
		DBMGR_INSERT_V(pWk->data.incoil[ii].nInCutNum);		// �����q��
		CreateSQL::sqlColumnAddTime(&sql, true);			// �o�^���t
		DBMGR_INSERT_S(pWk->data.cID);						// ID
		DBMGR_INSERT_V(ii+1);								// No(�o������No)
		DBMGR_INSERT_V(pWk->nMode);							// �敪
		DBMGR_INSERT_V(pWk->data.incoil[ii].nFront);		// �擪�ʒu
		DBMGR_INSERT_V(pWk->data.incoil[ii].nLength);		// �ۗL����
		DBMGR_INSERT_V(pWk->data.incoil[ii].nOutPos);		// �o�������ʒu
		//<<<----------------------------------------------------------------

		CreateSQL::sqlColumnTrim(&sql);						// �Ō�̃J���}����菜��

		sql += ")";

		//// �˗�
		DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
		pDeli->addmode	= true;
		pDeli->cnt		= 0;
		pDeli->strSql = sql;
		if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
			// ��Γo�^�������E�E�E
			LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DB_COIL_CUT_POS]", my_sThreadName);
			syslog(SYSNO_QUEFULL_ERR, "[DB_COIL_CUT_POS]");
			delete pDeli;
		}
	}
}

//==========================================
// �r���
//------------------------------------------
// const DBDATA_DEFECT_INF* pWk	: �������
//==========================================
BOOL DBManager::CreateSQL_DefectInf(const DBDATA_DEFECT_INF* pWk)
{
	int  ou;							// �\���敪[0:�\ / 1:��]
	int  infRec;						// �o�b�t�@���R�[�h
	int  nKizuNo;						// ������rNo
	bool bImgSkip = false;				// true���摜�̂݃X�L�b�v
	CString sql;
	DBSaveManager::DeliverySQL* pDeli;

	//==================================================
	// ���O����
	ou		= pWk->data.TorB;
	infRec	= GetRecSQLInf(pWk->cKizukenNo);

	//// �o�b�t�@���R�[�h����
	if( -1 == infRec ){
		// ���莸�s
		LOG(em_ERR), "[%s] �ۑ����R�C���o�b�t�@�ɖ��� [%s]", my_sThreadName, pWk->cKizukenNo);
		syslog(280, "[%s:DBDATA_DEFECT_INF]", pWk->cKizukenNo);
		return false;
	}

	//==================================================
	// �t���[����
	int CheckResult = this->SaveMaxCheck(infRec, ou, pWk->nLowGrFlg);

	//// �ő�ۑ������ɓ��B
	if( -1 == CheckResult ) return false;

	//// �摜�̂݃X�L�b�v������
	if( pWk->bImgSkip || 9 == CheckResult ) bImgSkip = true;

	//// �폜�D��H
	if( true){
		int nQueCntSQL = gque_AddSQLData.GetCount();		// SQL�o�^�L���[
		int nQueCntDB  = mque_AddDB.GetCount();				// DB�o�^�L���[
		if( 200<=nQueCntSQL || 200<=nQueCntDB ) this->Send_To_DbManage(true);		// �����D��𑗐M
		else									this->Send_To_DbManage(false);		// �폜�D��𑗐M
	}

	this->mcls_Time.CounterStart();		// �������Ԍv���J�n

	//==================================================
	// �r�����Z
	m_SqlInf[infRec].nSaveCnt[ou]++;
	m_SqlInf[infRec].nTotalCnt[ou]++;
	nKizuNo = m_SqlInf[infRec].nSaveCnt[0] + m_SqlInf[infRec].nSaveCnt[1];
	// 20200520 �r�ۑ�������g�� --->>>
	int nOffsetVrcl = VRCL_DEFECT_NO_END - VRCL_DEFECT_NO_START + 1;
	if (VRCL_DEFECT_NO_START <= nKizuNo) {		// VRCL�V�K�r���͔͈͂����O����
		nKizuNo = nKizuNo + nOffsetVrcl;
	}
	// 20200520 �r�ۑ�������g�� ---<<<

	if( !bImgSkip ) {
		m_SqlInf[infRec].nSaveImgAllCnt[ou]++;
		m_SqlInf[infRec].nSaveImgCnt[ou]++;
	}

	//======================================================
	// �r���
	pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->bCalcTime= true;
	pDeli->cnt		= 0;
 
	////// �r���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	if(0 == ou) sql += DB_DEFECT_INF_T;
	else		sql += DB_DEFECT_INF_B;
	sql += " VALUES(";

	//---------------------------------------------------------------->>>
	// �f�[�^��
	DBMGR_INSERT_S(pWk->cKizukenNo);								// �Ǘ�No 
	DBMGR_INSERT_V(nKizuNo);										// �rNo

	DBMGR_INSERT_V( pWk->data.TorB);								// �\��
	DBMGR_INSERT_V( (int)pWk->attr_base[EPC_ATTR_BASE_MERGE]);		// �d������ (1:�d��) 
	DBMGR_INSERT_V( pWk->data.nTid);								// �r��EdasysID
	DBMGR_INSERT_V( pWk->data.nGid);								// �O���[�hEdasysID
	DBMGR_INSERT_V( pWk->data.cycle_div);							// �����r�敪(0:����r, 1:�ȈՎ����r,�A���r 2:�����r)
	DBMGR_INSERT_V( pWk->data.dsws);								// DS,WS�敪 (0,1)
	DBMGR_INSERT_V( pWk->data.camset);								// �J�����Z�b�gID (1�`20)

	DBMGR_INSERT_V( pWk->dLen);										// F����̈ʒu[mm]
	DBMGR_INSERT_V( pWk->data.ds);									// DS���G�b�W����̈ʒu[mm]
	DBMGR_INSERT_V( pWk->data.ws);									// WS���G�b�W����̈ʒu[mm]

	DBMGR_INSERT_V( pWk->data.speed);								// ���x[mpm]
	DBMGR_INSERT_V( pWk->data.pitch);								// �s�b�`[mm]
	DBMGR_INSERT_V( pWk->data.roolkei);								// ���[���a[mm]
	
	DBMGR_INSERT_V( pWk->data.aria);								// �ʐ� [mm^2]
	DBMGR_INSERT_V( (int)pWk->data.kizu_wid);						// �r�̕� [mm]
	DBMGR_INSERT_V( (int)pWk->data.kizu_len);						// �r�̒��� [mm]
	DBMGR_INSERT_V( pWk->nLowGrFlg);								// ���QGr / �L�QGr

	DBMGR_INSERT_V( pWk->data.nWidDiv);								// �������敪
	DBMGR_INSERT_V( pWk->data.nWidAbsPos);							// ��������Έʒu
	DBMGR_INSERT_V( pWk->nIsDspFlg);								// �\���Ώۋ敪_�T�����

	DBMGR_INSERT_N(NULL, 0);										// 10
	DBMGR_INSERT_N(NULL, 0);										// 9
	DBMGR_INSERT_N(NULL, 0);										// 8
	DBMGR_INSERT_N(NULL, 0);										// 7
	DBMGR_INSERT_N(NULL, 0);										// 6
	DBMGR_INSERT_N(NULL, 0);										// 5
	DBMGR_INSERT_N(NULL, 0);										// 4
	DBMGR_INSERT_N(NULL, 0);										// 3
	DBMGR_INSERT_N(NULL, 0);										// 2
	DBMGR_INSERT_N(NULL, 0);										// 1

	DBMGR_INSERT_V(pWk->data.nKizuNo);								// �r�A��
	DBMGR_INSERT_V( pWk->data.h_res);								// �c����\[mm/pixel]
	DBMGR_INSERT_V( pWk->data.w_res);								// ������\[mm/pixel]
	DBMGR_INSERT_V( (int)pWk->data.nFrameNo);						// �t���[��No

	DBMGR_INSERT_V( pWk->data.frm_x_min);							// �t���[���O��Xmin[pixel] (�����ʂ̊O�ڒ����`���̂���)
	DBMGR_INSERT_V( pWk->data.frm_x_max);							// �t���[���O��Xmax[pixel] 
	DBMGR_INSERT_V( pWk->data.frm_y_min);							// �t���[���O��Ymin[pixel]
	DBMGR_INSERT_V( pWk->data.frm_y_max);							// �t���[���O��Ymax[pixel]

	DBMGR_INSERT_V( pWk->data.cull_frm_x_min);						// �Ԉ����t���[���O��Xmin[pixel] (�����ʂ̊O�ڒ����`���̂���)
	DBMGR_INSERT_V( pWk->data.cull_frm_x_max);						// �Ԉ����t���[���O��Xmax[pixel] 
	DBMGR_INSERT_V( pWk->data.cull_frm_y_min);						// �Ԉ����t���[���O��Ymin[pixel]
	DBMGR_INSERT_V( pWk->data.cull_frm_y_max);						// �Ԉ����t���[���O��Ymax[pixel]

	DBMGR_INSERT_V( pWk->data.box_x_min);							// �O��Xmin[pixel]		(�}�C�i�X�l���肦��)
	DBMGR_INSERT_V( pWk->data.box_x_max);							// �O��Xmax[pixel]	
	DBMGR_INSERT_V( pWk->data.box_y_min);							// �O��Ymin[pixel]
	DBMGR_INSERT_V( pWk->data.box_y_max);							// �O��Ymax[pixel]
	DBMGR_INSERT_V( pWk->data.center_x);							// �r���S�ʒuX [pixel] 
	DBMGR_INSERT_V( pWk->data.center_y);							// �r���S�ʒuY [pixel]
	DBMGR_INSERT_V( pWk->data.h_size);								// �c��f��[pixel]
	DBMGR_INSERT_V( pWk->data.w_size);								// ����f��[pixel]
	
	for(int ii=0; ii<MAX_HT1_CAMANGLE; ii++) {
		DBMGR_INSERT_V( pWk->nImgsize[ii]);							// �摜�T�C�Y�i���^���j
	}
	
	DBMGR_INSERT_V(pWk->data.base_box_x_min);						// ���摜�O��Xmin[pixel]	(�}�C�i�X�l���肦��)
	DBMGR_INSERT_V(pWk->data.base_box_x_max);						// ���摜�O��Xmax[pixel]	
	DBMGR_INSERT_V(pWk->data.base_box_y_min);						// ���摜�O��Ymin[pixel]
	DBMGR_INSERT_V(pWk->data.base_box_y_max);						// ���摜�O��Ymax[pixel]
	DBMGR_INSERT_V(pWk->data.base_center_x);						// ���摜�r���S�ʒuX [pixel] 
	DBMGR_INSERT_V(pWk->data.base_center_y);						// ���摜�r���S�ʒuY [pixel]
	DBMGR_INSERT_V(pWk->data.base_h_size);							// ���摜�c��f��[pixel]
	DBMGR_INSERT_V(pWk->data.base_w_size);							// ���摜����f��[pixel]

	DBMGR_INSERT_V(pWk->data.base_h_res);							// ���c����\[mm/pixel]
	DBMGR_INSERT_V(pWk->data.base_w_res);							// ��������\[mm/pixel]
	DBMGR_INSERT_V(pWk->data.nShrinkRatio);							// �k����
	//<<<----------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);		// �Ō�̃J���}����菜��

	sql += ")";

	//// �˗�
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DB_DEFECT_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DB_DEFECT_INF]");
		delete pDeli;
		// DB���ُ� (��ōŒጸ�`�F�b�N���Ă���ׁA�ʏ킠�肦�Ȃ�)
		mcls_pStatus->AddQueMyErrWar(false, ON_SAVE_DEFECT_INF);
	}

	// BCP�����݃N���X�ɏ����݈˗�
	BCPSaveManager::DeliveryDB *deliDB = new BCPSaveManager::DeliveryDB;
	deliDB->kubun = BCPSaveManager::em_DEFECTINF;
	deliDB->data = (BYTE*)new DBDATA_DEFECT_INF;
	DBDATA_DEFECT_INF* pData = (DBDATA_DEFECT_INF*)deliDB->data;
	memcpy(pData, pWk, sizeof(DBDATA_DEFECT_INF));
	
	
	// �摜�f�[�^�Z�b�g
	for(int ii=0; ii<MAX_CAMANGLE; ii++) {
		pData->pImg[ii]	  = pWk->pImg[ii];
	}

	// BCP�����݃N���X��n���p�ϐ�
	pData->nKizuNo = nKizuNo;
	pData->bImgSkip = bImgSkip;

	//================================================
	//// �o�^�˗�
	if (!mcls_Bcps.gque_AddSQLData.AddToTail(deliDB, 20, 20)) {
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_DEFECT_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[DBDATA_DEFECT_INF]");

		// �L���[�t��
		for(int ii=0; ii<MAX_CAMANGLE; ii++) { SAFE_DELETE_ARRAY(pData->pImg[ii]); }
		delete deliDB->data;
		delete deliDB;
	}

	// �������Ԍv���I��
	mcls_Time.CounterEnd();

	return true;
}

//==========================================
// �G�b�W���o����
//------------------------------------------
// const DBDATA_EDGE_COIL_INF* pWk : �������
//==========================================
void DBManager::CreateSQL_EdgeCoilInf(const DBDATA_EDGE_COIL_INF* pWk)
{
	CString sql;	

	////// ���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += DB_EDGE_COIL_INF;
	sql += " VALUES(";

	// �f�[�^��
	DBMGR_INSERT_S( pWk->cKizukenNo);								// �Ǘ�No 
	DBMGR_INSERT_V( pWk->nMen);										// �\��
	DBMGR_INSERT_V( pWk->dCoilWidMin);								// ���ŏ�
	DBMGR_INSERT_V( pWk->dCoilWidMax);								// ���ő�

	DBMGR_INSERT_V( pWk->nLimitEdgeErrNum);							// �����e�덷�͈͊O��
	DBMGR_INSERT_V( pWk->nLostEdgeNum[0]);							// �G�b�W�����o�� (���G�b�W)
	DBMGR_INSERT_V( pWk->nLostEdgeNum[1]);							// �G�b�W�����o�� (�E�G�b�W)
	DBMGR_INSERT_V( pWk->nFrameNumCoil);							// �R�C���P�ʃt���[����
	for(int ii=0; ii<NUM_CAM_POS; ii++)	{
		for(int jj=0; jj<5; jj++) {
			DBMGR_INSERT_V( pWk->nFrameNumSkip[ii][jj]); 
		}
	}

	CreateSQL::sqlColumnTrim(&sql);		// �Ō�̃J���}����菜��
	sql += ")";
	
	//// �˗�
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DB_EDGE_COIL_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DB_EDGE_COIL_INF]");
		delete pDeli;
	}
}

//==========================================
// �G�b�W���o���[�h�ω��Ǘ�
//------------------------------------------
// const DBDATA_EDGE_CHANGE_INF* pWk	
//==========================================
void DBManager::CreateSQL_EdgeChangeInf(const DBDATA_EDGE_CHANGE_INF* pWk)
{
	CString sql;	

	//======================================
	// EDGE_CHANGE_INF
	////// ���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += DB_EDGE_CHANGE_INF;
	sql += " VALUES(";

	//---------------------------------------------------------------->>>
	// �f�[�^��
	DBMGR_INSERT_S( pWk->cKizukenNo);			// �Ǘ�No 
	DBMGR_INSERT_V((int)(pWk->nPos));			// ����ʒu [mm]
	DBMGR_INSERT_V( pWk->nMen);					// �\���敪 (0:�\ 1:��)
	DBMGR_INSERT_V( pWk->nFrameNo);				// �t���[��No
	DBMGR_INSERT_V( pWk->nEdgeMode);			// ���o�敪 (0:����  1:�д��ޗD��  2:����ײݐU�蕪��  -1:�X�L�b�v   -9:�ΏۊO   -99:������ԁj
	DBMGR_INSERT_V( pWk->dEdgeRawPos[0]);		// ���G�b�W�ʒu [mm] (���G�b�W)
	DBMGR_INSERT_V( pWk->dEdgeRawPos[1]);		// ���G�b�W�ʒu [mm] (�E�G�b�W)
	DBMGR_INSERT_V( pWk->dEdgeAnsPos[0]);		// �ŏI�G�b�W�ʒu [mm] (���G�b�W)
	DBMGR_INSERT_V( pWk->dEdgeAnsPos[1]);		// �ŏI�G�b�W�ʒu [mm] (�E�G�b�W)
	DBMGR_INSERT_V( pWk->nEdgeErrReason);		// �G�b�W�s�����R�̘_���a
	//<<<----------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);				// �Ō�̃J���}����菜��

	sql += ")";
	
	//// �˗�
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DB_EDGE_CHANGE_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DB_EDGE_CHANGE_INF]");
		delete pDeli;
	}
}


//==========================================
// �J���������N�X�C�b�`�ݒ�
//------------------------------------------
// const DBDATA_CAMERA_LINKSWITCH* pWk : �������	
//==========================================
void DBManager::CreateSQL_CamLinkSW(const DBDATA_CAMERA_LINKSWITCH* pWk)
{
	CString sql;

	// �J���������N�X�C�b�`�ڑ��ݒ�擾
	ToLinkswUtil::TO_LINKSW_OUTPUT info;
	memset(&info, 0x00, sizeof(info));
	ToLinkswUtil::GetDeterminedHanteiPC(mcls_pStatus, &info);

	// ----------------------
	// DB�ɓo�^
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	// ������`�N���A
	sql.Format("UPDATE %s SET ", DB_CAMERA_LINKSWITCH);
	for (int ii = 0; ii < NUM_CAMLINKSW_CH_MAX; ii++)
	{
		if (0 != ii) sql.AppendFormat(", ");
		sql.AppendFormat("[�{�[�h%d_�J�����I��]=0", ii + 1);
	}
	if (!clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] %s<%s>", my_sThreadName, clsDB.GetLastErrMsg(), sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sql);
		return;
	}

	Sleep(100);

	// ����䐔���J�����Z�b�g�o�^
	sql = "";
	for(int ii=0; ii<NUM_ALL_HT; ii++ ) {
		for(int jj=0; jj<NUM_CAMLINK_BOARD; jj++) {
			if (0 == info.output[ii][jj].nFPGACh) continue;				// �{�[�h������
			sql.Format("UPDATE %s SET [�{�[�h%d_�J�����I��]=%d WHERE �{�[�hID=%d", DB_CAMERA_LINKSWITCH,
				info.output[ii][jj].nFPGACh,
				info.output[ii][jj].nCamPort,
				info.output[ii][jj].nFPGABoardNo );

			if( !clsDB.ExecuteDirect(sql)) {
				LOG(em_ERR), "[%s] %s<%s>", my_sThreadName, clsDB.GetLastErrMsg(), sql);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sql);
				return;
			}
		}
	}

	LOG(em_MSG), "[%s] CamLinkSw DB��`�ύX", my_sThreadName);
	CString sTarget;
	int nPCID = 0;
	for (int ii = 0; ii < NUM_ALL_HT; ii++)
	{
		sTarget.Empty();
		if (0 == info.output[ii][0].nFPGACh)
		{
			sTarget = "-----------------";
		}
		else
		{
			sTarget.AppendFormat("PCID:%02d(%s)", nPCID + 1, HANTEI(nPCID));
			nPCID++;
		}

		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			LOG(em_MSG), "[%s]   %s LinkswNo:%02d FPGA:%d Ch:%02d camNo:%02d camset:%02d",
				my_sThreadName,
				sTarget,
				info.output[ii][jj].nLinkswNo,
				info.output[ii][jj].nFPGABoardNo,
				info.output[ii][jj].nFPGACh,
				info.output[ii][jj].nCamPort,
				info.output[ii][jj].nCamset);
		}
	}

	LOG(em_MSG), "[%s] �����J�n�ݒ�R�}���h���M(FACT_TO_LINKSW_02)", my_sThreadName);
	ToOutMailSender::Send_ToLinksw_Start();
	return;
}

//==========================================
// �V�[�P���T����p���X���[�g�ύX
//------------------------------------------
// const DBDATA_PULSE_RATE_UPDATE* pWk : �������	
//==========================================
void DBManager::CreateSQL_PulseRateUpdate(const DBDATA_PULSE_RATE_UPDATE* pWk)
{
	CString sql;

	////// ���
	sql =  "UPDATE ";
	sql += DB_PARAM_COMMON;
	sql += " SET ";
	CreateSQL::sqlColumnUpdate(&sql, "PLG�p���X���[�g", pWk->dPls[0]);
	CreateSQL::sqlColumnUpdate(&sql, "PLG�p���X���[�g��r", pWk->dPls[1]);
	CreateSQL::sqlColumnUpdate(&sql, "PLG�p���X���[�g�\��", pWk->dPls[2]);
	CreateSQL::sqlColumnUpdate(&sql, "Vres", pWk->dVres[0]);

	CreateSQL::sqlColumnTrim(&sql);		// �Ō�̃J���}����菜��

	// Where
	//sql += " WHERE ";
	//CreateSQL::sqlColumnUpdate(&sql, "Key", 0);		// Key(1���R�[�h�Ȃ̂ŁA�{��Where�喳���Ă��ǂ��͂�����
	//CreateSQL::sqlColumnTrim(&sql);		// �Ō�̃J���}����菜��

	//// �˗�
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql   = sql;

	//// �o�^������A���^�X�N�Ƀp�����[�^�ύX�ʒm�𑗐M
	//// �z���g�͂���Ȏ�i�ł�肽���͂Ȃ����c
	pDeli->SendMailItem = NULL;
	pDeli->SendMailItem = new MAIL_ITEM;
	memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
	strcpy(pDeli->SendMailItem->cTask, TO_DEFECT);
	strcpy(pDeli->SendMailItem->cPc, ".");
	pDeli->SendMailItem->Que.nEventNo = FACT_TO_DEFECT_01;

	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DB_PARAM_COMMON]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DB_PARAM_COMMON]");
		delete pDeli->SendMailItem;
		delete pDeli;
	}
}

//==========================================
// �o�^�󋵖₢���킹
//------------------------------------------
// const DBDATA_ADD_SYNC* pWk : �������
//==========================================
void DBManager::Add_Synclock(const DBDATA_ADD_SYNC* pWk)
{
	//// �₢���킹�p�̓���f�[�^
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= "";
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;

	// ���[���X���b�g��M�^�C�~���O�ŁADB�L���[���ꏄ������A�ԋp����
	pDeli->SendMailItem = NULL;
	pDeli->SendMailItem = new MAIL_ITEM;
	memcpy( pDeli->SendMailItem, &pWk->sendItem, sizeof(MAIL_ITEM));	 

	// �˗�
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] �o�^�󋵖₢���킹�B�L���[�t���ׁ̈A�₢���킹�����", my_sThreadName);
		delete pDeli->SendMailItem;
		delete pDeli;
	}
	LOG(em_MSG), "[%s] �o�^�󋵖₢���킹�J�n �v����(%s)", my_sThreadName, pWk->sendItem.cTask);
}

//==========================================
// �r�摜�o�^�����Z�b�g
//------------------------------------------
// const DBDATA_COUNT_RESET* pWk : �������
//==========================================
void DBManager::Reset_ImgCount(const DBDATA_COUNT_RESET* pWk)
{
	ResetInf_SaveImgCnt(pWk->cKizukenNo, pWk->nTorB);

	int nRec = GetRecSQLInf(pWk->cKizukenNo);
	if(0 > nRec) nRec = 0;

	LOG(em_MSG), "[%s] �r�摜�o�^�����Z�b�g(%s) <�ۑ�> �S��(%d/%d) �o��(%d/%d) <�폜> ���Q(%d/%d) �L�Q(%d/%d)", 
		my_sThreadName, pWk->cKizukenNo,
		GetInf_SaveImgAllCnt(nRec, 0), GetInf_SaveImgAllCnt(nRec, 1),
		GetInf_SaveImgCnt(nRec, 0), GetInf_SaveImgCnt(nRec, 1),
		GetInf_ImgLowGrDeleCnt(nRec, 0), GetInf_ImgLowGrDeleCnt(nRec, 1),
		GetInf_ImgDeleCnt(nRec, 0), GetInf_ImgDeleCnt(nRec, 1));
}

//==========================================
// �X�e�[�^�X���O
//------------------------------------------
// const DBDATA_STATUS_LOG* pWk : �������
//==========================================
void DBManager::CreateSQL_StatusLog(const DBDATA_STATUS_LOG* pWk)
{
	CString sql;	
	int ii;

	////// ���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += DB_STATUS_LOG;
	sql += " VALUES(";

	// �f�[�^��
	CreateSQL::sqlColumnAddTime(&sql, pWk->time, true);		// �o�^���t 
	DBMGR_INSERT_V(pWk->nAutoNo);			// �A��

	DBMGR_INSERT_V( (int)pWk->emKadou);
	for(ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V( (int)pWk->emKensa[ii]);
	for(ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V( (int)pWk->emKiki[ii]);
	
	DBMGR_INSERT_S( pWk->cMsg );
	DBMGR_INSERT_S( pWk->cKizukenNo);
	DBMGR_INSERT_S( pWk->cCoilNo);


	CreateSQL::sqlColumnTrim(&sql);		// �Ō�̃J���}����菜��
	sql += ")";
	
	//// �˗�
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[DBManager] DB�o�^�˗��L���[�t�� [DB_STATUS_LOG]");
		syslog(SYSNO_QUEFULL_ERR, "[DB_STATUS_LOG]");
		delete pDeli;
	}
}
