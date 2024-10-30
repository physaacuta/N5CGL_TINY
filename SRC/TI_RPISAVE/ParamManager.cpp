#include "StdAfx.h"
#include "ParamManager.h"

#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"		// ���ɂ̓����N�������Ȃ�(=����قǂ���Ȃ�)����Acpp�ɓ���Ă����B

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
ParamManager::ParamManager(void) :
ThreadManager("ParamMgr")
{
	m_evReadEnd	= CreateEvent(NULL, FALSE, FALSE, NULL);	
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ParamManager::~ParamManager(void)
{
	CloseHandle(m_evReadEnd);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ParamManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { gque_Deli.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ParamManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���		
	enum {	EV_QUE = 0,								// �p�����[�^�ύX�ʒm
			EV_END	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:									// �p�����[�^�ύX�ʒm
		COMMON_QUE* pDeli;				// �X���b�h�L���[�󂯓n��
		pDeli = gque_Deli.GetFromHead();
		ExecParam(pDeli);
		delete pDeli;

		// �p�����[�^�ǂݍ��݊����ʒm
		SetEvent(m_evReadEnd);
		break;


//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		break;
	}
}

//------------------------------------------
// �p�����[�^�Ǎ�����
// COMMON_QUE* pQue �L���[�󂯓n��
//------------------------------------------
void ParamManager::ExecParam(COMMON_QUE* pQue)
{	
	// ����́A�S�p�����[�^��ǂݍ���ŏI���H
	ReadParam_All(false);
}

//------------------------------------------
// �v�Z�l�c����\ (���S�Œ蕪��\�Ƃ���������)
// int idx	�`�����l�� (0:ײ�PLG)  EM_DIV_PLG
//------------------------------------------
double ParamManager::CalcResV()
{
	double dResV = 1.0;		// �c����\
	dResV = m_Param_Common.dPls * m_Param_Common.nDiv / m_Param_Common.nMul;
	return dResV;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p�����[�^�Ǎ�

//------------------------------------------
// �S�p�����[�^�ǂݍ���
// bool bStatic static�p�����[�^���擾����ꍇ true
//------------------------------------------
bool ParamManager::ReadParam_All(bool bStatic)
{

	// =========================================================
	// �p�����[�^�擾
	P_MAIN	buf;
	memset(&buf, 0x00, sizeof(buf));

	if( ! Read_Ini(buf, false) )	return false;
	if( ! Read_Param_CamSet(buf) )	return false;
	if( ! Read_Param_CamEnable(buf))return false;
	if( ! Read_Param_Common() )		return false;

	// �v�Z
	CalcParam(buf);
	memcpy( &m_Main, &buf, sizeof(buf));

	// �R�C���P�ʂł̓ǂݍ���
	for(int ii=0; ii<buf.nIns; ii++) {
		Read_IniCoilChange(ii);
		Read_IniKidoChange(ii);
	}
	return true;
}

//------------------------------------------
// ini�t�@�C���ǂݍ���
// P_MAIN& buf �Z�b�g�p�G���A
// bool bStatic static�p�����[�^���擾����ꍇ true
//------------------------------------------
bool ParamManager::Read_Ini(P_MAIN& buf, bool bStatic)
{
	CString	sWk, sWk2;
//	char	cWk[256];

	//// ini�t�@�C���擾
	// ���ʕ�
	sWk.Format("%s", TI_RPISAVE);
	buf.nIns		= GetPrivateProfileInt(sWk, "INS_NUM", 0, TASKINI_NAME );
	buf.nFrameSizeX = GetPrivateProfileInt(sWk, "FRAME_SIZE_X", 0, TASKINI_NAME );
	buf.nFrameSizeY	= GetPrivateProfileInt(sWk, "FRAME_SIZE_Y", 0, TASKINI_NAME );
	buf.nGrabSizeX  = GetPrivateProfileInt(sWk, "GRAB_SIZE_X", 0, TASKINI_NAME );
	buf.nGrabSizeY	= GetPrivateProfileInt(sWk, "GRAB_SIZE_Y", 0, TASKINI_NAME );

	buf.nExecCoilStart	= GetPrivateProfileInt(sWk, "EXEC_COIL_START", 0, TASKINI_NAME );

	GetPrivateProfileString( sWk, "WRITE_SEC_PATH", "", buf.cWriteSecPath, sizeof(buf.cWriteSecPath), TASKINI_NAME );
	GetPrivateProfileString( sWk, "SYSTEM_NAME", "", buf.cSystemName, sizeof(buf.cSystemName), TASKINI_NAME );
	buf.nHddFreeSizeSave  = GetPrivateProfileInt(sWk, "HDD_FREE_SIZE", 10, TASKINI_NAME);
	buf.nHddFreeSizeLocal = GetPrivateProfileInt(sWk, "HDD_FREE_SIZE_LOCAL", buf.nHddFreeSizeSave, TASKINI_NAME);
	
	buf.nAddFrameInfoType = GetPrivateProfileInt(sWk, "ADD_FRAMEINFO_TYPE", 0, TASKINI_NAME);		// �t���[���t�����ؑ� (0:���� 1:T_RPI_FRAME_INFO_T1)


	// ����
	buf.nLoopQueRecvSize	= GetPrivateProfileInt(sWk, "LOOP_QUE_RECV_SIZE", 10, TASKINI_NAME);
	buf.nLoopQueFrameSize	= GetPrivateProfileInt(sWk, "LOOP_QUE_FRAME_SIZE", 20, TASKINI_NAME);
	buf.nThreadUnionNum		= GetPrivateProfileInt(sWk, "THREAD_UNION_NUM", 2, TASKINI_NAME);
	buf.nThreadCompNum		= GetPrivateProfileInt(sWk, "THREAD_COMP_NUM", 8, TASKINI_NAME);

	// ���
	buf.nDispWindScaleX		= GetPrivateProfileInt(sWk, "DEBUG_DISP_SCALE_X", 8, TASKINI_NAME);
	buf.nDispWindScaleY		= GetPrivateProfileInt(sWk, "DEBUG_DISP_SCALE_Y", 8, TASKINI_NAME);
	buf.nDispWindPosX		= GetPrivateProfileInt(sWk, "DEBUG_DISP_POS_X", 0, TASKINI_NAME);
	buf.nDispWindPosY		= GetPrivateProfileInt(sWk, "DEBUG_DISP_POS_Y", 0, TASKINI_NAME);
	buf.nDispPrint			= GetPrivateProfileInt(sWk, "DEBUG_PRINT", 0, TASKINI_NAME);

	// �@��ID
	buf.nStateId			= GetPrivateProfileInt(sWk, "ERR_ID", 0, TASKINI_NAME);

	// �Œ�
	buf.emColorType = COLOR_TYPE_COLOR;
	buf.emSysType	= SYS_TYPE_BAT;
	buf.nImgNumTotal= MAX_COLOR_NUM;

	// �C���X�^���X��
	for(int ii=0; ii<buf.nIns; ii++) {
		sWk.Format("%s_INS_%d", TI_RPISAVE, ii+1);
	
		// ��{
		buf.ins[ii].nStartCamset	= GetPrivateProfileInt(sWk, "START_CAMSET", 0, TASKINI_NAME );
		buf.ins[ii].nCamsetNum		= GetPrivateProfileInt(sWk, "CAMSET_NUM", 0, TASKINI_NAME );
		buf.ins[ii].nMen			= GetPrivateProfileInt(sWk, "MEN", 0, TASKINI_NAME );
		buf.ins[ii].bWidthSync		= __BOOL GetPrivateProfileInt(sWk, "WIDTH_SYNC", 0, TASKINI_NAME );
		buf.ins[ii].bImgTorB		= __BOOL GetPrivateProfileInt(sWk, "IMG_TorB", 0, TASKINI_NAME );
		GetPrivateProfileString(sWk, "RAW_SAVE_PATH", "", buf.ins[ii].cRawSavePath, sizeof(buf.ins[ii].cRawSavePath), TASKINI_NAME );

		buf.ins[ii].nCamAngle = 1;

		// �ꎞ�o�b�t�@
		for(int jj=0; jj<MAX_FOLDER_SAVE_NUM; jj++) {
			sWk2.Format("RPI_FOLDER_BUF_%d", jj+1);
			GetPrivateProfileString(sWk, sWk2, "", buf.ins[ii].cRpiFolderBuf[jj], sizeof(buf.ins[ii].cRpiFolderBuf[jj]), TASKINI_NAME );
			if( 0 == strlen(buf.ins[ii].cRpiFolderBuf[jj])) break;
			buf.ins[ii].nRpiFolderBufNum ++;
		}
		_ASSERT( 0 != buf.ins[ii].nRpiFolderBufNum );

		// �����ԕۑ��p�X
		for(int jj=0; jj<MAX_FOLDER_SAVE_NUM; jj++) {
			sWk2.Format("RPI_FOLDER_SAVE_%d", jj+1);
			GetPrivateProfileString(sWk, sWk2, "", buf.ins[ii].cRpiFolderSave[jj], sizeof(buf.ins[ii].cRpiFolderSave[jj]), TASKINI_NAME );
			if( 0 == strlen(buf.ins[ii].cRpiFolderSave[jj])) break;
			buf.ins[ii].nRpiFolderSaveNum ++;
		}			
		_ASSERT( 0 != buf.ins[ii].nRpiFolderSaveNum );

		//// �A���֌W
		buf.ins[ii].nOverLapMode	= GetPrivateProfileInt(sWk, "OVERLAP_MODE", 0, TASKINI_NAME );
		buf.ins[ii].nImgDelete[0]	= GetPrivateProfileInt(sWk, "IMG_DELETE_L", 0, TASKINI_NAME );
		buf.ins[ii].nImgDelete[1]	= GetPrivateProfileInt(sWk, "IMG_DELETE_R", 0, TASKINI_NAME );
		buf.ins[ii].bEdgeDeleteEnabel= __BOOL GetPrivateProfileInt(sWk, "EDGE_DELETE_ENABLE", 0, TASKINI_NAME );
		buf.ins[ii].nEdgeDelete[0]	= GetPrivateProfileInt(sWk, "EDGE_DELETE_L", 0, TASKINI_NAME );
		buf.ins[ii].nEdgeDelete[1]	= GetPrivateProfileInt(sWk, "EDGE_DELETE_R", 0, TASKINI_NAME );
	}


	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	LOG(em_MSG), "[%s] Ini�t�@�C�� �擾����", my_sThreadName);
	return true;
}

//------------------------------------------
// �J�����Z�b�g�� �p�����[�^ �Ǎ�
// P_MAIN& buf �Z�b�g�p�G���A
//------------------------------------------
bool ParamManager::Read_Param_CamSet(P_MAIN& pm)
{
	CString sql;					// SQL��
	CString	sWk;
	int camset;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_PARAM_CAMSET;
	P_PARAM_CAMSET&	p = m_Param_CamSet;
	P_PARAM_CAMSET	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	for(int ii=0; ii<pm.nIns; ii++) {
		//// SQL���쐬
		sql =  "SELECT";
		sql += " camset";
		sql += ", Hres, Xoffset";

		sql += " FROM ";
		sql += cDbName;
		sWk.Format(" WHERE camset>=%d and camset<%d", pm.ins[ii].nStartCamset, pm.ins[ii].nStartCamset+pm.ins[ii].nCamsetNum );
		sql += sWk;


		//// ���s
		if( !clsDB.ExecuteDirect(sql)) {
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
			return false;
		}

		//// �擾
		while (TRUE) {
			int sqlrc = clsDB.Fetch();
			if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			
				// ��L�[
				clsDB.GetData(1, &camset);
				if( MAX_CAMSET < camset || 0 >= camset ) {
					// ��L�[�s��
					LOG(em_ERR), "[%s] [%s ��L�[�s�� camset=%d]", my_sThreadName, cDbName, camset);
					syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d]", cDbName, camset);					
					continue;
				}
				camset = camset - pm.ins[ii].nStartCamset;		// �g���₷���悤��0�I���W���ɕύX

				// �f�[�^��
				// ���E����
				clsDB.GetData(2, &buf.ins[ii].camset[camset].dResWidth );
				clsDB.GetData(3, &buf.ins[ii].camset[camset].dXoffset );

				buf.ins[ii].camset[camset].bIsEnable = true;

			} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
				break;	
			} else {								// �G���[
				syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
				return false;
			}
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// �L���J�����擾
//------------------------------------------
bool ParamManager::Read_Param_CamEnable(P_MAIN& pm)
{
/***
	CString sql;					// SQL��
	CString	sWk;
	int camset;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_HT_ENABLE;
	P_PARAM_CAMSET&	p = m_Param_CamSet;


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//-------------------------
	// �J�����s�g�p�ݒ�
	for(int ii=0; ii<pm.nIns; ii++) {
		//// SQL���쐬
		sql =  "SELECT";
		sql += " camset";
		sql += ", �����";

		sql += " FROM ";
		sql += cDbName;
		sWk.Format(" WHERE camset>=%d and camset<%d", pm.ins[ii].nStartCamset, pm.ins[ii].nStartCamset+pm.ins[ii].nCamsetNum );
		sql += sWk;


		//// ���s
		if( !clsDB.ExecuteDirect(sql)) {
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
			return false;
		}

		//// �擾
		while (TRUE) {
			int sqlrc = clsDB.Fetch();
			if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			
				// ��L�[
				clsDB.GetData(1, &camset);
				if( MAX_CAMSET < camset || 0 >= camset ) {
					// ��L�[�s��
					LOG(em_ERR), "[%s] [%s ��L�[�s�� camset=%d]", my_sThreadName, cDbName, camset);
					syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d]", cDbName, camset);					
					continue;
				}
				camset = camset - pm.ins[ii].nStartCamset;		// �g���₷���悤��0�I���W���ɕύX

				// �f�[�^��
				int wk;
				clsDB.GetData(2, &wk);
				p.ins[ii].camset[camset].bIsEnable = 1 == wk ? FALSE : TRUE;


			} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
				break;	
			} else {								// �G���[
				syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
				return false;
			}
		}
	}
	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
***/
	return true;
}

//------------------------------------------
// ���ʃp�����[�^ �Ǎ�
//------------------------------------------
bool ParamManager::Read_Param_Common()
{
	CString sql;					// SQL��
	double wk;

	//// �p�����[�^������
	CString cDbName;
	cDbName.Format("%s CROSS JOIN %s",  DB_PARAM_COMMON, DB_COMMON_MASTER) ;


	P_PARAM_COMMON	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	//sql =  "SELECT TOP 1 [PLG�p���X���[�g], [�J����PLG���{��], [�J����PLG������], [�S���摜_�Ԉ�������], [�S���摜_�Ԉ������c]";
	sql =  "SELECT TOP 1 [PLG�p���X���[�g], [�J����PLG���{��], [�J����PLG������], [ShFrameShrinkX], [ShFrameShrinkY]";
	sql += " FROM ";
	sql += cDbName;
		
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// �f�[�^��
			clsDB.GetData(1, &buf.dPls );
			clsDB.GetData(2, &buf.nMul );
			clsDB.GetData(3, &buf.nDiv);

			clsDB.GetData(4, &buf.nImgCullX);
			clsDB.GetData(5, &buf.nImgCullY);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &m_Param_Common, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// �擾�����p�����[�^���� �K�v�ȏ����Z�o����
// P_MAIN& buf �Z�b�g�p�G���A
//------------------------------------------
void ParamManager::CalcParam(P_MAIN& buf)
{
	int ii, jj;

	// �������`�F�b�N
	if( buf.nIns < 1 )			_ASSERT(false);
	if( buf.nIns > MAX_INS )	_ASSERT(false);


	////// ����
	//if( COLOR_TYPE_GRAY == buf.emColorType ) { buf.nImgNumTotal = 1; buf.nImgNumRaw = 1; }
	//if( COLOR_TYPE_COLOR== buf.emColorType ) { buf.nImgNumTotal = 3; buf.nImgNumRaw = 3; }	// RGB��3�摜�ARGB��3�摜
	//if( COLOR_TYPE_SA== buf.emColorType )	 { buf.nImgNumTotal = 4; buf.nImgNumRaw = 3; }	// RGB,R-B��4�摜�ARGB��3�摜


	//// �O�i�ŒP���Ɍv�Z�ł���Ԉ������W�֌W
	buf.nFrameSizeX = CalcCullX(buf.nFrameSizeX);
	buf.nFrameSizeY = CalcCullY(buf.nFrameSizeY);
	buf.nGrabSizeX = CalcCullX(buf.nGrabSizeX);
	buf.nGrabSizeY = CalcCullY(buf.nGrabSizeY);

	for(ii=0; ii<buf.nIns; ii++) {
		for(jj=0; jj<buf.ins[ii].nCamsetNum; jj++) {

			buf.ins[ii].camset[jj].dResWidth = m_Param_CamSet.ins[ii].camset[jj].dResWidth * m_Param_Common.nImgCullX;
		}
	}





	//// �P�i��
	int			nMaxWidth = 0;									// �ő�̉摜�� [pixel]
	int			nSyncWidth = 0;									// ��������������摜�� [pixel]
	int			nRawWidth[MAX_INS];								// �����ȉ摜�� [pixel]
	for(ii=0; ii<buf.nIns; ii++) {
		double	dResXSum = 0.0;									// ���ω�����\�v�Z�p���[�N
		int		nLetfStart = 0;									// 1�O�̃J�����̍��J�n�ʒu
		int		nOldLap = 0;									// 1�O�̃J�����̉E�I�[�o�[���b�v��
		double	dWk = 0.0;

		for(jj=0; jj<buf.ins[ii].nCamsetNum; jj++) {
			//ParamManager::P_PARAM_CAMSET::INS::CAMSET& prm = m_Param_CamSet.ins[ii].camset[jj];
			P_MAIN::INS::CAMSET& set = buf.ins[ii].camset[jj];

			// ---------------------------------------
			// �A������
			if( -1 == buf.ins[ii].nOverLapMode ) {	
				set.nOverLap		= 0;
				set.nOverLapL	= 0;
				set.nBaseLeft	= buf.nFrameSizeX * jj;
				set.nLapStartBase= buf.nFrameSizeX;

			// ---------------------------------------
			// �A����
			} else {

				if( jj != buf.ins[ii].nCamsetNum-1 ) {
					dWk = m_Param_CamSet.ins[ii].camset[jj+1].dXoffset - m_Param_CamSet.ins[ii].camset[jj].dXoffset;					// ���J�������Ǝ��J�������d�Ȃ��Ă���͈� (mm�n)
					//set.nOverLap = buf.nFrameSizeX - (int)(dWk / set.dResWidth);		// �d�Ȃ��Ă���͈� (pixel�n)
					set.nOverLap = buf.nGrabSizeX - (int)(dWk / set.dResWidth);		// �d�Ȃ��Ă���͈� (pixel�n)
					int nOverLapResult = (set.nOverLap < MAX_OVERLAP ? set.nOverLap : MAX_OVERLAP);
		
					if(0 < set.nOverLap) {
						//set.nLapStartBase = buf.nFrameSizeX - set.nOverLap;
						//set.nLapStartBaseResult = buf.nFrameSizeX - nOverLapResult;
						set.nLapStartBase = buf.nGrabSizeX - set.nOverLap;
						set.nLapStartBaseResult = buf.nGrabSizeX - nOverLapResult;
					} else {
						// �E�I�[�o�[���b�v��������
						set.nOverLap = 0;
						//set.nLapStartBase = buf.nFrameSizeX;
						//set.nLapStartBaseResult = buf.nFrameSizeX;
						set.nLapStartBase = buf.nGrabSizeX;
						set.nLapStartBaseResult = buf.nGrabSizeX;
					}

				} else {
					// ��ԍŌ�̃J�����͖������ŉE�I�[�o�[���b�v�͖���
					set.nOverLap = 0;
					//set.nLapStartBase = buf.nFrameSizeX;
					//set.nLapStartBaseResult = buf.nFrameSizeX;
					set.nLapStartBase = buf.nGrabSizeX;
					set.nLapStartBaseResult = buf.nGrabSizeX;
				}
				set.nOverLapL = nOldLap;

				// ���[����̃I�t�Z�b�g�l�Z�o
				if(0 != jj) {
					set.nBaseLeft = nLetfStart + buf.ins[ii].camset[jj-1].nLapStartBase;
				} else {
					set.nBaseLeft = 0;					// ���[�J�����͏��0
				}
			}

			// ���̂���
			dResXSum   += set.dResWidth;
			nLetfStart	= set.nBaseLeft;
			nOldLap		= set.nOverLap;
		}


		buf.ins[ii].reso_x = (float)(dResXSum / buf.ins[ii].nCamsetNum); 	// ������\
		buf.ins[ii].reso_y = (float) this->CalcResV() * m_Param_Common.nImgCullY;
		buf.ins[ii].nLineCenter = (int)(abs( m_Param_CamSet.ins[ii].camset[0].dXoffset ) / buf.ins[ii].reso_x);		// ���C���Z���^�[ [pixel]


		// �摜�T�C�Y (���́A������)
		//nRawWidth[ii]	= nLetfStart + buf.nFrameSizeX;	
		nRawWidth[ii]	= nLetfStart + buf.nGrabSizeX;	
		nMaxWidth		= max( nMaxWidth, nRawWidth[ii]);
		if(buf.ins[ii].bWidthSync ) nSyncWidth = max( nSyncWidth, nRawWidth[ii]);
	}


	//// �Q�i��
	// �摜��������
	buf.nMaxHeight = buf.nFrameSizeY;
	buf.nMaxWidth  = GET_SIZE_JUST(nMaxWidth, 16); // (((nMaxWidth-1) / 8) + 1)*8;						// 8�̔{���ɕύX

	for(int ii=0; ii<buf.nIns; ii++) {
		P_MAIN::INS& set = buf.ins[ii];

		set.nUionHeight = buf.nFrameSizeY;

		// �摜��������
		if(set.bWidthSync ) {
			set.nUionWidth   = GET_SIZE_JUST(nSyncWidth, 16); //(((nSyncWidth-1) / 8) + 1)*8;						// 8�̔{���ɕύX
			set.nInsertRight = set.nUionWidth - nRawWidth[ii];			// �E�[���}���� [pixel]
		} else {
			set.nUionWidth   = GET_SIZE_JUST(nRawWidth[ii], 16); //(((nRawWidth[ii]-1) / 8) + 1)*8;					// 8�̔{���ɕύX
			set.nInsertRight = set.nUionWidth - nRawWidth[ii];			// �E�[���}���� [pixel]
		}
	}


	// ------------------------
	// �I�[�o�[���b�v��
	for(int ii=0; ii<buf.nIns; ii++) {
		for(jj=0; jj<buf.ins[ii].nCamsetNum; jj++) {

			P_MAIN::INS::CAMSET& set = buf.ins[ii].camset[jj];

			//// ���E�I�[�o�[���b�v (���������B�A���̂�)
			if( -1 == buf.ins[ii].nOverLapMode ) {
				
				set.nCopyImg[0] = 0;
				//set.nCopyImg[1] = buf.nFrameSizeX;
				set.nCopyImg[1] = buf.nGrabSizeX;

				set.nHokanImg[0] = 0;
				set.nHokanImg[1] = 0;
			}

			//// ���E�I�[�o�[���b�v (�X���[�W���O)
			if( 0 == buf.ins[ii].nOverLapMode ) {
				set.nCopyImg[0] = set.nOverLapL;
				set.nCopyImg[1] = set.nLapStartBaseResult - set.nCopyImg[0];
				if( 0 > set.nCopyImg[1]) set.nCopyImg[1] = 0;

				set.nHokanImg[0] = set.nLapStartBaseResult;
				//set.nHokanImg[1] = buf.nFrameSizeX - set.nLapStartBaseResult;
				set.nHokanImg[1] = buf.nGrabSizeX - set.nLapStartBaseResult;
			}

			//// ���E�I�[�o�[���b�v (���t���[���D��)
			if( 1 == buf.ins[ii].nOverLapMode ) {
				set.nCopyImg[0] = set.nOverLapL;
				set.nCopyImg[1] = set.nLapStartBase + set.nOverLap - set.nOverLapL;

				set.nHokanImg[0] = 0;
				set.nHokanImg[1] = 0;
			}

			//// ���E�I�[�o�[���b�v (�E�t���[���D��)
			if( 2 == buf.ins[ii].nOverLapMode ) {
				set.nCopyImg[0] = 0;
				set.nCopyImg[1] = set.nLapStartBase;

				set.nHokanImg[0] = 0;
				set.nHokanImg[1] = 0;
			}
		}
	}

	
	//// �r�b�g�}�b�v�w�b�_����
	KizuBitmapManager	kb;
	buf.nBmpHeadSize = kb.GetBitmapHeaderSize_Gray(); 
	for(int ii=0; ii<buf.nIns; ii++) {
		//kb.CreateBitmapHeader_Gray( buf.ins[ii].nUionWidth, buf.ins[ii].nUionHeight, buf.ins[ii].BmpHead, false); // LAC���Ɏg�p���邽�߁A�摜�𔽓]�����Ă����B(=�p�͕��ʂ��r����BMP�摜�̒ʂ�)
		kb.CreateBitmapHeader_Gray( buf.ins[ii].nUionWidth, buf.ins[ii].nUionHeight, buf.ins[ii].BmpHead, buf.ins[ii].bImgTorB); // LAC���Ɏg�p���邽�߁A�摜�𔽓]�����Ă����B(=�p�͕��ʂ��r����BMP�摜�̒ʂ�)
	}



	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	LOG(em_MSG), "[%s] �v�Z���� [�摜�T�C�Y x=%d/Y=%d]", my_sThreadName, buf.nMaxWidth, buf.nMaxHeight );
}



//------------------------------------------
// �R�C���P�ʂŃp�����[�^�Ǎ�����	�� �����Ɛ擪�t���[�����甽�f
// int nIns InsNo (0�I���W��)
//------------------------------------------
void ParamManager::Read_IniCoilChange(int nIns)
{
	CString	sWk, sWk2, sWk3;
	sWk.Format("%s", TI_RPISAVE);
	sWk2.Format("%s_INS_%d", TI_RPISAVE, nIns+1);

	// �܂��̓��[�J���ɓǍ�
	EM_COMP_TYPE emCompType = (EM_COMP_TYPE) GetPrivateProfileInt(sWk, "COMP_TYPE", 0, TASKINI_NAME );
	int nCompQualityBase = GetPrivateProfileInt(sWk2, "COMP_QUALITY", 1, TASKINI_NAME );

	int nComp[MAX_PCODE];
	for (int ii = 0; ii<MAX_PCODE; ii++) {
		sWk3.Format("COMP_%d", ii + 1);
		nComp[ii] = GetPrivateProfileInt(sWk, sWk3, 0, TASKINI_NAME);
	}


	// ���f
	m_Main.ins[nIns].emCompType = emCompType;
	m_Main.ins[nIns].nCompQualityBase = nCompQualityBase;
	memcpy(m_Main.ins[nIns].nCompQuality, nComp, sizeof(nComp));
}

//------------------------------------------
// �P�x�l�X�V			�� �Ǎ����������f�Ȃ̂ŁA����
// int nIns InsNo (0�I���W��)
//------------------------------------------
void ParamManager::Read_IniKidoChange(int nIns)
{
	char	cKidoPath[256];					// �P�x�␳�p�X
	CString	sWk, sWk2;
	sWk.Format("%s_INS_%d", TI_RPISAVE, nIns + 1);


	ParamManager::P_MAIN::INS& prmI = m_Main.ins[nIns];


	for (int jj = 0; jj<m_Main.ins[nIns].nCamsetNum; jj++) {
		// �P�x�␳
		sWk2.Format("KIDO_PATH_%d", jj + 1);
		GetPrivateProfileString(sWk, sWk2, "", cKidoPath, sizeof(cKidoPath), TASKINI_NAME);
		if (0 == strlen(cKidoPath)) {
			prmI.camset[jj].bKidoUmu = false;
			m_vKido[nIns][jj].clear();
			continue;
		}

		// �Ǎ�	
		CsvIniFunc	cls;
		std::vector<CString>	v;
		std::vector<short>		vB;
		if (!cls.ReadCsv(KIDO_SEC, cKidoPath, &v)) {
			LOG(em_ERR), "[%s] �P�x�␳�t�@�C���I�[�v���G���[[%s]", my_sThreadName, cKidoPath);
			prmI.camset[jj].bKidoUmu = false;
			m_vKido[nIns][jj].clear();
			continue;
		}
		for (DWORD ii = 0; ii<v.size(); ii++) {
			if (ii > m_Main.nFrameSizeX) break;

			vB.push_back((short)atoi(v[ii]));
		}

		// ���f
		m_vKido[nIns][jj] = vB;
		prmI.camset[jj].bKidoUmu = true;

		LOG(em_INF), "[%s] �P�x�␳�t�@�C���擾[%s][%s] Path[%s]", my_sThreadName, sWk, sWk2, cKidoPath);
	}
}
