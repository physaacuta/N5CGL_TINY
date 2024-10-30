#include "StdAfx.h"
#include "BcpSaveManager.h"
#include <float.h>
#include <limits.h>
// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
BCPSaveManager::BCPSaveManager(void) :
DbBaseManager("BCPMgr")
{
	//// BCP�n ������
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		// �r�摜
		mcls_pBcpDefImg[ii] = NULL;
		m_pBcpDefImgBuf[ii] = NULL;
		m_nBcpDefSendRowCnt[ii] = 0;

		//// �t���[���摜
		//mcls_pBcpFrmImg[ii] = NULL;
		//m_pBcpFrmImgBuf[ii] = NULL;
		//m_nBcpFrmSendRowCnt[ii] = 0;

		// ������
		m_nBcpAttrSendRowCnt[ii] = 0;
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++){
			mcls_pBcpAttr[ii][jj] = NULL;
			m_pBcpAttrBuf[ii][jj] = NULL;
		}
	}
	//////// �R�C���ʒu���f�[�^
	//////for (int ii=0; ii<EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++) {
	//////	mcls_pBcpCoilPos[ii] = NULL;
	//////	m_pBcpCoilPosBuf[ii] = NULL;
	//////}
	//////m_nBcpCoilPosSendRowCnt = 0;


	//// �@���Ԗ߂�
	//KizuFunction::SetStatus("BCP_ERR_ID",			true, false);	// �摜�����ݎ��s
	//KizuFunction::SetStatus("DB_SAVE_MAX_1_ID",		true, false);	// �������ѕۑ����
	//KizuFunction::SetStatus("DB_SAVE_MAX_2_ID",		true, false);	// �������ѕۑ����
	//KizuFunction::SetStatus("DB_SAVE_SKIP_1_ID",	true, false);	// �������ѕۑ��X�L�b�v
	//KizuFunction::SetStatus("DB_SAVE_SKIP_2_ID",	true, false);	// �������ѕۑ��X�L�b�v
	//this->SetSaveSkipId(0, GetPrivateProfileInt(INI_STATUS, "DB_SAVE_SKIP_1_ID", 0, TASKINI_NAME) );
	//this->SetSaveSkipId(1, GetPrivateProfileInt(INI_STATUS, "DB_SAVE_SKIP_2_ID", 0, TASKINI_NAME) );

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
BCPSaveManager::~BCPSaveManager(void)
{
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCP�֘A

//==========================================
// BCP�N���X �e�[�u���R�t�� (�r�摜)
//==========================================
void BCPSaveManager::BCP_Bind_Defect_Img()
{
	//// �\/������\���Ȃ̂ŁA�����̈���g�p����B

	// �������̈�̌Œ� & �ϒ����ڂ̃T�C�Y��`
	for(int ii=0; ii<MAX_BCP_INS; ii++){

		// �̈�m��
		m_pBcpDefImgBuf[ii] = new TYP_BCP_DEFECT_IMG;
		for(int jj=0; jj<MAX_IMAGE_CNT; jj++)					m_pBcpDefImgBuf[ii]->img[jj].Alloc(KIZUBCP_MAX_IMGSIZE);	// �ϒ��Ή��� �摜�̈�m��
		for(int jj=MAX_IMAGE_CNT; jj<MAX_HT1_CAMANGLE; jj++)	m_pBcpDefImgBuf[ii]->img[jj].Alloc(0);						// NULL�̈�


		// DB�̃e�[�u���ɓ�������
		mcls_pBcpDefImg[ii]->BCP_Bind_String	(1, m_pBcpDefImgBuf[ii]->cKizukenNo);
		mcls_pBcpDefImg[ii]->BCP_Bind_Value		(2, (BYTE*)&m_pBcpDefImgBuf[ii]->nKizuNo, BCPBase::em_Int);
		for(int jj=0; jj<MAX_HT1_CAMANGLE; jj++)	mcls_pBcpDefImg[ii]->BCP_Bind_Binary(3+jj, &m_pBcpDefImgBuf[ii]->img[jj], BCPBase::em_VerBinary);

	}
}

//==========================================
// BCP�N���X �e�[�u���R�t�� (������)
//==========================================
void BCPSaveManager::BCP_Bind_Defect_Attr()
{
	//// �\/������\���Ȃ̂ŁA�����̈���g�p����B

	// �������̈�̌Œ� & �ϒ����ڂ̃T�C�Y��`
	for(int ii=0; ii<MAX_BCP_INS; ii++) {
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++ ) {
			// �̈�m��
			m_pBcpAttrBuf[ii][jj] = new TYP_BCP_DEFECT_ATTR;

			// DB�̃e�[�u���ɓ�������
			mcls_pBcpAttr[ii][jj]->BCP_Bind_String	(1, m_pBcpAttrBuf[ii][jj]->cKizukenNo);
			mcls_pBcpAttr[ii][jj]->BCP_Bind_Value 	(2, (BYTE*)&m_pBcpAttrBuf[ii][jj]->nKizuNo,	BCPBase::em_Int);


			if( 0==jj ) {	// ��{
				for(int kk=0; kk<MAX_ATTR_BASE; kk++)  mcls_pBcpAttr[ii][jj]->BCP_Bind_Value(3+kk, (BYTE*)&m_pBcpAttrBuf[ii][jj]->attr[kk], BCPBase::em_Double);
			} else {		// �p�x
				for(int kk=0; kk<MAX_ATTR_ANGLE; kk++) mcls_pBcpAttr[ii][jj]->BCP_Bind_Value(3+kk, (BYTE*)&m_pBcpAttrBuf[ii][jj]->attr[kk], BCPBase::em_Double);
			}
		}
	}
}

////////==========================================
//////// BCP�N���X �e�[�u���R�t�� (�R�C���ʒu���)
////////==========================================
//////void BCPSaveManager::BCP_Bind_Coil_Position()
//////{
//////	for (int ii=0; ii<EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++) {
//////		// �������̈�̌Œ� & �ϒ����ڂ̃T�C�Y��`
//////		m_pBcpCoilPosBuf[ii] = new TYP_BCP_COIL_POSITION;
//////
//////		// DB�̃e�[�u���ɓ�������
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_String(1, m_pBcpCoilPosBuf[ii]->CoilPos.cKizukenNo);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(2, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.nSpmUmu, BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(3, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.nID, BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(4, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.emKensa[0], BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(5, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.emKensa[1], BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(6, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.emKiki[0], BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(7, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.emKiki[1], BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(8, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.dEdge[0], BCPBase::em_Float);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(9, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.dEdge[1], BCPBase::em_Float);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(10, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.dWalk, BCPBase::em_Float);
//////	}
//////}

//==========================================
// BCP��Ԃ�ύX
//------------------------------------------
// bool		bMode		: BCP��ԕύX
//==========================================
void BCPSaveManager::CheckBcpStatus(bool bMode)
{
	if( ! bMode ) {
		m_bBcpState = false;

		LOG(em_MSG), "[%s] BCP ��ԂɈُ픭��", my_sThreadName);
		mcls_pStatus->SetStatusQue("BCP_ERR_ID", false);


		//// BCP�Đڑ�
		OnFree();			// ���
		OnAlloc(false);		// �ڑ�


		// ������Ƃ������̃L���[�����������̂�҂�
		Sleep(50);


	////////////////////////////////
	}else if( !m_bBcpState && bMode ) {
		m_bBcpState = true;
		LOG(em_INF), "[%s] BCP ��Ԃ����한��", my_sThreadName);
		mcls_pStatus->SetStatusQue("BCP_ERR_ID", true);
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p��������R�[���o�b�N

//==========================================
// �֐�������
//------------------------------------------
// bool		bMode	: BCP��ԕύX
//==========================================
void BCPSaveManager::OnAlloc(bool bMode)
{
	//// BCP�N���X����
	if(bMode) m_bBcpState = true;

	//------------------------------------
	//// �r�摜
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		mcls_pBcpDefImg[ii] = new BCPBase();
		CString strTable = (0==ii ? DB_DEFECT_IMG_T : DB_DEFECT_IMG_B);
		BCP_Connect(mcls_pBcpDefImg[ii], strTable);
	}
	BCP_Bind_Defect_Img();	// �o�b�t�@��R��

	
	//// ������
	char Tbl[MAX_BCP_INS][MAX_BCP_INS_ATTR][32] = 
#ifdef HT_IS_COLOR
		{	{DB_DEFECT_ATTR0_T, DB_DEFECT_ATTR1_T, DB_DEFECT_ATTR2_T, DB_DEFECT_ATTR3_T, DB_DEFECT_ATTR4_T},
			{DB_DEFECT_ATTR0_B, DB_DEFECT_ATTR1_B, DB_DEFECT_ATTR2_B, DB_DEFECT_ATTR3_B, DB_DEFECT_ATTR4_B} };
#else
		{	{DB_DEFECT_ATTR0_T, DB_DEFECT_ATTR1_T, DB_DEFECT_ATTR2_T},
			{DB_DEFECT_ATTR0_B, DB_DEFECT_ATTR1_B, DB_DEFECT_ATTR2_B} };
#endif
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++){
			mcls_pBcpAttr[ii][jj] = new BCPBase();
			CString strTable = Tbl[ii][jj];
			BCP_Connect(mcls_pBcpAttr[ii][jj], strTable);
		}
	}
	BCP_Bind_Defect_Attr();	// �o�b�t�@�R��

	////////// �R�C���ʒu���
	//////CString strTable[EM_DIV_SYSTEM::DIV_SYSTEM_END] = { DB_COIL_POSITION, DB_COIL_POSITION_OUT, DB_COIL_POSITION_ANA };
	//////for (int ii = 0; ii<EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++) {
	//////	mcls_pBcpCoilPos[ii] = new BCPBase();
	//////	BCP_Connect(mcls_pBcpCoilPos[ii], strTable[ii]);
	//////}
	//////BCP_Bind_Coil_Position();

}

//==========================================
// �֐��J��
//==========================================
void BCPSaveManager::OnFree()
{
	//// BCP�N���X�J��
	// �r�摜
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		if(NULL == mcls_pBcpDefImg[ii]) continue;
		BCP_Dispose( mcls_pBcpDefImg[ii] );
		SAFE_DELETE( mcls_pBcpDefImg[ii] );
		SAFE_DELETE( m_pBcpDefImgBuf[ii] );
		m_nBcpDefSendRowCnt[ii] = 0;
	}

	// ������
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++){
			if(NULL == mcls_pBcpAttr[ii][jj]) continue;
			BCP_Dispose( mcls_pBcpAttr[ii][jj] );
			SAFE_DELETE( mcls_pBcpAttr[ii][jj] );
			SAFE_DELETE( m_pBcpAttrBuf[ii][jj] );
		}
		m_nBcpAttrSendRowCnt[ii] = 0;
	}

	//////// �R�C���ʒu���
	//////for (int ii = 0; ii < EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++) {
	//////	if (NULL != mcls_pBcpCoilPos[ii]) {
	//////		BCP_Dispose(mcls_pBcpCoilPos[ii]);
	//////		SAFE_DELETE(mcls_pBcpCoilPos[ii]);
	//////		SAFE_DELETE(m_pBcpCoilPosBuf[ii]);
	//////		m_nBcpCoilPosSendRowCnt = 0;
	//////	}
	//////}

}


//==========================================
// BCP�����R�~�b�g�p
//------------------------------------------
// bool		bAuto		: �����R�~�b�g��true	
//==========================================
void BCPSaveManager::OnBcpAutoCommit(bool bAuto)
{
	// �����ƃR�~�b�g���ꂸ�ɂ���BCP_SendRow�����f�[�^���m�肳���� (��C���Ɍ��ʓI)

	for(int ii=0; ii<MAX_BCP_INS; ii++){

		// �r�摜
		if( 0 != m_nBcpDefSendRowCnt[ii] ){
			if(bAuto) { LOG(em_MSG), "[BcpSaveManager] BCP �����R�~�b�g <%s>",  (0 ==ii?DB_DEFECT_IMG_T:DB_DEFECT_IMG_B)); }

			BCP_Batch(mcls_pBcpDefImg[ii]);

			m_nBcpDefSendRowCnt[ii] = 0;

		}

		// ������
		if( 0 != m_nBcpAttrSendRowCnt[ii] ){
			for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++){
				BCP_Batch(mcls_pBcpAttr[ii][jj]);
			}

			m_nBcpAttrSendRowCnt[ii] = 0;
		}
	}

	//////// �R�C���ʒu���
	//////if (0 != m_nBcpCoilPosSendRowCnt) {
	//////	if (bAuto) { LOG(em_MSG), "[%s] BCP �����R�~�b�g <%s>", my_sThreadName, DB_COIL_POSITION); }

	//////	for (int ii = 0; ii < EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++)
	//////		BCP_Batch(mcls_pBcpCoilPos[ii]);

	//////	m_nBcpCoilPosSendRowCnt = 0;
	//////}

}

//==========================================
// BCP��ԃ`�F�b�N(������^�C�}�[�C�x���g)
// �� DB�ُ��BCP���؂ꂽ�ꍇ�A�]�����Ǝ��r�����܂ŕ������Ȃ������Ή�
//==========================================
void BCPSaveManager::OnBcpStatusCheck()
{
	//// �ĕ��������݂�
	if( ! m_bBcpState ) {
		bool bMode = OdbcBase::OpenCheck(TASKINI_NAME, INI_DB);
		// ���한�A�����ꍇ�ɍĐڑ�������
		CheckBcpStatus(bMode);
	}
}

//==========================================
// DB�o�^�˗�
//------------------------------------------
// int		emNo	: ENUM_KUBUN�̔ԍ�
// BYTE*	pData	: �f�[�^�� (�p�����ŊJ���������s���̂ł����ł́A�J������K�v�Ȃ�)
//==========================================
void BCPSaveManager::OnQueDb(int emNo, BYTE* pData)
{
	//// �敪�ɂ�菈������
	switch(emNo) {
		//-----------------------------------------------------------
		case em_DEFECTINF:												// �r���
#ifndef NON_DB_SAVE
			CreateSQL_DefectInf( (const DBDATA_DEFECT_INF*)pData );
#endif
			// ���ׂ̈����ɓn���ꂽ�摜�̈���J��
			for(int ii=0; ii<MAX_HT1_CAMANGLE; ii++) { SAFE_DELETE_ARRAY( ((DBDATA_DEFECT_INF*)pData)->pImg[ii] ); }
			break;

		//-----------------------------------------------------------
		case em_BCP_BATCH:												// BCP batch�������s
			BcpBatchExec((const DBDATA_BCP_BATCH_EXEC*)pData);
			break;

		// <<<------------------------------

	}
}


//==========================================
// ���Z�b�g�˗�
//==========================================
void BCPSaveManager::OnReSetMaxCount()
{
	//// ���Ԍv������
	mcls_AttrSendRowTime.InitAve();
	mcls_AttrBcpBatchTime.InitAve();
	mcls_ImgSendRowTime.InitAve();
	mcls_ImgBcpBatchTime.InitAve();
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQL�����֘A

//==========================================
// �r���
//------------------------------------------
// const DBDATA_DEFECT_INF* pWk	: �������
//==========================================
void BCPSaveManager::CreateSQL_DefectInf(const DBDATA_DEFECT_INF* pWk)
{
	int  ou;							// �\���敪[0:�\ / 1:��]
	int	 nJpegSize[MAX_HT1_CAMANGLE];	// Bitmap��Jpeg�ɕϊ������摜�T�C�Y

	//==================================================
	// ���O����
	ou		= pWk->data.TorB;


	this->mcls_Time.CounterStart();		// �������Ԍv���J�n
	
	//==================================================
	// ��{ ������ (BCP������)
	{
		// �w�b�_�[
		for(int ii=0; ii<MAX_BCP_INS_ATTR; ii++){
			memcpy(m_pBcpAttrBuf[ou][ii]->cKizukenNo, pWk->cKizukenNo, sizeof(pWk->cKizukenNo));	// �Ǘ�No
			m_pBcpAttrBuf[ou][ii]->nKizuNo = pWk->nKizuNo;												// �rNo
		}
		// ��{
		for(int ii=0; ii<MAX_ATTR_BASE; ii++){
			m_pBcpAttrBuf[ou][0]->attr[ii] = filter_nan_inf(pWk->attr_base[ii]);
		}
		// �p�x
		for(int jj=0; jj<MAX_BCP_INS_ATTR-1; jj++) {
			for(int ii=0; ii<MAX_ATTR_ANGLE; ii++) { 
				m_pBcpAttrBuf[ou][jj+1]->attr[ii] = filter_nan_inf(pWk->attr_angle[jj][ii]);
			}
		}
		// �]��
		mcls_AttrSendRowTime.CounterStart();		// �������Ԍv���J�n
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++) {
			bool bWk = BCP_SendRow( mcls_pBcpAttr[ou][jj] );
			CheckBcpStatus(bWk);										// BCP�̌��ʂ�����������
			if(!bWk) OnBcpStatusCheck();								// �ĕ��������݂�
#ifdef DEBUG_HYPER_V_TEST
			if (0 == jj) break;					// ���J�����ł�DB���b�N����邽��
#endif
		}
		mcls_AttrSendRowTime.CounterEnd();		// �������Ԍv���I��

		//// ������x�܂Ƃ܂��ď������� (���ꂪ�z���g�̏�����)
		m_nBcpAttrSendRowCnt[ou] += 1;
		mcls_AttrBcpBatchTime.CounterStart();		// �������Ԍv���J�n
		if( m_nBcpAttrSendRowCnt[ou] >= MAX_BCP_BATCH_COUNT_ATTR ) {	
			for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++) {
				BCP_Batch(mcls_pBcpAttr[ou][jj]);
			}
			m_nBcpAttrSendRowCnt[ou] = 0;
		}
		mcls_AttrBcpBatchTime.CounterEnd();		// �������Ԍv���I��
	}


	//======================================================
	// �r�摜 (BCP������)

	//// ���Z�b�g
	m_pBcpDefImgBuf[ou]->cKizukenNo[0] = 0x00;
	m_pBcpDefImgBuf[ou]->nKizuNo = 0;
	for(int ii=0; ii<MAX_HT1_CAMANGLE; ii++) m_pBcpDefImgBuf[ou]->img[ii].SetNullData();		// ���Z�b�g��NULL�摜��o�^����悤��
	memset(nJpegSize, 0x00, sizeof(nJpegSize));

	if( ! pWk->bImgSkip) {
		if( pWk->isBmp ) {
			// Jpeg�ɕϊ��Ƌ��ɁABCP�G���A�ɐ��E���摜�f�[�^�Z�b�g
			//    ����Z�b�g�����z�ȊO�́A��`�ł��ׂ�NULL�ƂȂ�̂ŋC�����鎖�i�z���g�͂�����NULL��o�^�������������E�E�E�������s���j
			for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
				// �摜�Ȃ��r�Ή�
				if (0 == pWk->nImgsize[ii]) continue;	// �摜�T�C�Y��0�̏ꍇ�̓o�C�i���Z�b�g���Ȃ�
#ifndef JPEGLIB_LJT
				KizuJpegManager::BmpToJpeg( pWk->data.w_size, pWk->data.h_size, pWk->pImg[ii], m_nJpgQuality, &nJpegSize[ii], m_pBcpDefImgBuf[ou]->img[ii].pData);
#else
				mcls_KizuJpegManager.BmpToJpeg(pWk->data.w_size, pWk->data.h_size, pWk->pImg[ii], m_nJpgQuality, &nJpegSize[ii], m_pBcpDefImgBuf[ou]->img[ii].pData);
#endif
				m_pBcpDefImgBuf[ou]->img[ii].SetSize( nJpegSize[ii] );
			}
		} else {
			// ���ł�Jpeg�摜�̈� BCP�G���A�ɉ摜���R�s�[���邾��
			for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
				// �摜�Ȃ��r�Ή�
				if (0 == pWk->nImgsize[ii]) continue;	// �摜�T�C�Y��0�̏ꍇ�̓o�C�i���Z�b�g���Ȃ�
				memcpy(m_pBcpDefImgBuf[ou]->img[ii].pData, pWk->pImg[ii], pWk->nImgsize[ii]);
				nJpegSize[ii] = pWk->nImgsize[ii];
				m_pBcpDefImgBuf[ou]->img[ii].SetSize( nJpegSize[ii] );
				BCP_ColLen( mcls_pBcpDefImg[ou], 3+ii, nJpegSize[ii] );	// varbinary(MAX) �ɒ�`�ύX���Ă���A0xFFFF�܂ł����o�^�ł��Ȃ��b��΍�
			}
		}


		// ���̑� �摜��� ���ڃZ�b�g
		memcpy(m_pBcpDefImgBuf[ou]->cKizukenNo, pWk->cKizukenNo, sizeof(pWk->cKizukenNo));		// �Ǘ�No
		//m_pBcpDefImgBuf[ou]->nKizuNo = pWk->data.nKizuNo;										// �rNo
		m_pBcpDefImgBuf[ou]->nKizuNo = pWk->nKizuNo;											// �rNo
		//KizuODBC::BCPBase::Get_BPC_Time( m_pBcpDefImgBuf[nBcpInc]->cAddTime );						// �o�^���t�̕t�^

		
		//// BCP �o�^
		// �f�[�^�]�� (���ꂾ���ł́A�������܂�Ȃ��̂Œ���)
		mcls_ImgSendRowTime.CounterStart();		// �������Ԍv���J�n
		bool bWk = BCP_SendRow( mcls_pBcpDefImg[ou] );
		mcls_ImgSendRowTime.CounterEnd();		// �������Ԍv���I��
		CheckBcpStatus(bWk);										// BCP�̌��ʂ�����������
		if(!bWk) OnBcpStatusCheck();								// �ĕ��������݂�

				
		//// ������x�܂Ƃ܂��ď������� (���ꂪ�z���g�̏�����)
		m_nBcpDefSendRowCnt[ou] += 1;
		if( m_nBcpDefSendRowCnt[ou] >= MAX_BCP_BATCH_COUNT_DEF ) {
			mcls_ImgBcpBatchTime.CounterStart();		// �������Ԍv���J�n
			BCP_Batch(mcls_pBcpDefImg[ou]);
			mcls_ImgSendRowTime.CounterEnd();		// �������Ԍv���I��
			m_nBcpDefSendRowCnt[ou] = 0;
		}
	}

	//// �����R�~�b�g�p
	this->BCP_AutoCommitTimerOn();

	// �������Ԍv���I��
	mcls_Time.CounterEnd();
}

//==========================================
// BCP batch �������s
//==========================================
void BCPSaveManager::BcpBatchExec(const DBDATA_BCP_BATCH_EXEC* pWk)
{
	OnBcpAutoCommit(pWk->bMode);
}

