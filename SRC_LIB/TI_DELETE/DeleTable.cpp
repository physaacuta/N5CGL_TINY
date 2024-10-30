#include "StdAfx.h"
#include "DeleTable.h"


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���̃N���X�݂̂��A�V�X�e�����ɈႤ

//------------------------------------------
// �R���X�g���N�^
// int id ������ID (1�I���W��)
// int* waittime �҂����� [ms]
// HANDLE evEndCycle �I���V�O�i��
// HANDLE evEndSizeMin �I���V�O�i��
// CString cSession �����̐ݒ肪��`����Ă���ini�t�@�C���̃Z�b�V����
// CString cOdbcSession ODBC�ݒ�Z�b�V����
//------------------------------------------
DeleTable::DeleTable(int id, int* nWaitTime, HANDLE evEndCycle, HANDLE evEndSizeMin, CString cSession, CString cOdbcSession) :
KizuDeleteManager(id, nWaitTime, evEndCycle, evEndSizeMin, cSession, cOdbcSession),
m_nDelTblCoil(0),
m_nDelTblTime(0),
m_nDelTblCnt(0),
m_pDelTblCoil(NULL),
m_pDelTblTime(NULL),
m_pDelTblCnt(NULL)
{

	int		nTblCnt;				// ����
	char    cWk[1024];
	CString strVal;

	//----------------------------------------
	////// �R�C���ɕR�t���폜�e�[�u������
	//// �������擾
	nTblCnt = 0;
	while(true) {
		strVal.Format("ZI_%d", nTblCnt+1);
		GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if(0 >= strlen(cWk) ) break;
		nTblCnt++;
	}
	m_nDelTblCoil = nTblCnt;
	//// 0���ȊO�̏ꍇ �e�[�u������
	if(0 != m_nDelTblCoil) {
		nTblCnt = 0;
		m_pDelTblCoil = new DEL_TBL[m_nDelTblCoil];
		for(int ii=0; ii<m_nDelTblCoil; ii++) {
			strVal.Format("ZI_%d", ii+1);
			GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		
			if( 0 < strlen(cWk) ) {
				strVal = cWk;
				
				// �g�[�N������
				int		rec = 0;
				int		curPos = 0;
				CString strToken = strVal.Tokenize(",", curPos);
				while ( strToken != "" ) {
					strToken.Trim(_T(" 	"));		// ini�t�@�C���ݒ蕶���̑O��̃X�y�[�X�A�^�u����菜��

					switch( rec ) {
						case 0:						// �e�[�u����
							m_pDelTblCoil[ii].cDelTable = strToken; 
							break;
						case 1:						// ��L�[�L�[
							m_pDelTblCoil[ii].cKeyName	= strToken;
							break;
						case 2:						// �T�u�L�[
							m_pDelTblCoil[ii].cSubName	= strToken; 
							break;
						case 3:						// ���̍폜����
							m_pDelTblCoil[ii].nDelRow	= atoi(strToken); 
							break;
						default:
							break;
					}

					// ����
					strToken = strVal.Tokenize(",", curPos);
					rec ++;
				}
				// ���������g�[�N�����L���H
				if( 4 != rec) {
					_ASSERT(false);
				}
			}
		}
	}

	//----------------------------------------
	////// ���t�ɕR�t���폜�e�[�u������
	//// �������擾
	nTblCnt = 0;
	while(true) {
		strVal.Format("TI_%d", nTblCnt+1);
		GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if(0 >= strlen(cWk) ) break;
		nTblCnt++;
	}
	m_nDelTblTime = nTblCnt;
	//// 0���ȊO�̏ꍇ �e�[�u������
	if(0 != m_nDelTblTime) {
		nTblCnt = 0;
		m_pDelTblTime = new DEL_TBL[m_nDelTblTime];
		for(int ii=0; ii<m_nDelTblTime; ii++) {
			strVal.Format("TI_%d", ii+1);
			GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		
			if( 0 < strlen(cWk) ) {
				strVal = cWk;
				
				// �g�[�N������
				int		rec = 0;
				int		curPos = 0;
				CString strToken = strVal.Tokenize(",", curPos);
				while ( strToken != "" ) {
					strToken.Trim(_T(" 	"));		// ini�t�@�C���ݒ蕶���̑O��̃X�y�[�X�A�^�u����菜��

					switch( rec ) {
						case 0:						// �e�[�u����
							m_pDelTblTime[ii].cDelTable = strToken; 
							break;
						case 1:						// ���t��
							m_pDelTblTime[ii].cKeyName	= strToken; 
							break;
						case 2:						// �ۑ�����
							m_pDelTblTime[ii].nDelRow	= atoi(strToken); 
							break;
						default:
							break;
					}

					// ����
					strToken = strVal.Tokenize(",", curPos);
					rec ++;
				}
				// ���������g�[�N�����L���H
				if( 3 != rec) {
					_ASSERT(false);
				}
			}
		}
	}

	//----------------------------------------
	////// �ۑ�������ɕR�t���폜�e�[�u������
	//// �������擾
	nTblCnt = 0;
	while(true) {
		strVal.Format("CI_%d", nTblCnt+1);
		GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if(0 >= strlen(cWk) ) break;
		nTblCnt++;
	}
	m_nDelTblCnt = nTblCnt;
	//// 0���ȊO�̏ꍇ �e�[�u������
	if(0 != m_nDelTblCnt) {
		nTblCnt = 0;
		m_pDelTblCnt = new DEL_TBL_CNT[m_nDelTblCnt];
		for(int ii=0; ii<m_nDelTblCnt; ii++) {
			strVal.Format("CI_%d", ii+1);
			GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		
			if( 0 < strlen(cWk) ) {
				strVal = cWk;
				
				// �g�[�N������
				int		rec = 0;
				int		curPos = 0;
				CString strToken = strVal.Tokenize(",", curPos);
				while ( strToken != "" ) {
					strToken.Trim(_T(" 	"));		// ini�t�@�C���ݒ蕶���̑O��̃X�y�[�X�A�^�u����菜��

					switch( rec ) {
						case 0:						// �e�[�u����
							m_pDelTblCnt[ii].cDelTable = strToken; 
							break;
						case 1:						// ��L�[�L�[
							m_pDelTblCnt[ii].cKeyName	= strToken;
							break;
						case 2:						// �T�u�L�[
							m_pDelTblCnt[ii].cSubName	= strToken; 
							break;
						case 3:						// ���̍폜����
							m_pDelTblCnt[ii].nDelRow	= atoi(strToken); 
							break;
						case 4:						// �ۑ������
							m_pDelTblCnt[ii].nMaxCnt	= atoi(strToken); 
							break;
						default:
							break;
					}

					// ����
					strToken = strVal.Tokenize(",", curPos);
					rec ++;
				}
				// ���������g�[�N�����L���H
				if( 5 != rec) {
					_ASSERT(false);
				}
			}
		}
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DeleTable::~DeleTable(void)
{
	// �J��
	SAFE_DELETE_ARRAY(m_pDelTblCoil); 
	SAFE_DELETE_ARRAY(m_pDelTblTime); 
	SAFE_DELETE_ARRAY(m_pDelTblCnt); 
}

//------------------------------------------
// ��������s���\�b�h
//------------------------------------------
void DeleTable::ExecCycle()
{
	//// ����M�`���o�b�t�@
	for(int ii=0; ii<m_nDelTblTime; ii++) {
		Delete_Day2(m_pDelTblTime[ii].cDelTable,	m_pDelTblTime[ii].cKeyName, m_pDelTblTime[ii].nDelRow);
		Exec_Shrink();														// �g�����U�N�V�������k����
	}
	for(int ii=0; ii<m_nDelTblCnt; ii++) {
		Delete_Cnt(m_pDelTblCnt[ii].cDelTable,	m_pDelTblCnt[ii].cKeyName,	m_pDelTblCnt[ii].cSubName, m_pDelTblCnt[ii].nDelRow, m_pDelTblCnt[ii].nMaxCnt);
		Exec_Shrink();														// �g�����U�N�V�������k����
	}
	return;
}

//------------------------------------------
// DB�e�ʒቺ���s���\�b�h
//------------------------------------------
void DeleTable::ExecSizeMin()
{
	char cLastKizukenNo[SIZE_KIZUKEN_NO];		// �폜�����ŐV���r���Ǘ�No
	char cKizukenNo[SIZE_KIZUKEN_NO];			
	bool retc;

	memset(cLastKizukenNo, 0x00, sizeof(cLastKizukenNo));
	memset(cKizukenNo, 0x00, sizeof(cKizukenNo));

	////// �w�茏�����̃R�C�����т��폜
	for(int ii=0; ii<my_Param.nDelCoilCnt; ii++) {
		if(my_bStopFlg) return; // �I���H

		//---------------------------------------
		// �N���e�B�J���Z�N�V������� (�R�C������)
		//---------------------------------------		
		//// �ŌẪR�C���̎擾
		// ��L�[�̈� �N���e�B�J���Z�N�V���� �Ƃ��� (�d�v)
		mcls_pLock->LockTh();	//// -------------->>>>>
		//retc = Select_OldKey(DB_COIL_RESULT, "�r���Ǘ�No", SIZE_KIZUKEN_NO, cKizukenNo);
		//retc = Select_OldKey( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, SIZE_KIZUKEN_NO, cKizukenNo);
		retc = Select_OldKey( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, my_Param.sCoilResultSort, SIZE_KIZUKEN_NO, cKizukenNo);
		if(!retc || 0==strlen(cKizukenNo)) {		// �擾�f�[�^�̐������m�F
			mcls_pLock->UnlockTh();//// <<<<<--------------
			break;		// �擾���s ���� �f�[�^����
		}

		//// �R�C�����э폜
		// ���щ�ʂƂ��Ō������|����Ȃ��悤��
		//if( ! Delete_Inf( DB_COIL_RESULT, "�r���Ǘ�No", cKizukenNo) ) {
		if( ! Delete_Inf( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, cKizukenNo) ) {
			mcls_pLock->UnlockTh();//// <<<<<--------------
			continue;		// �폜���s
		}
		mcls_pLock->UnlockTh();	//// <<<<<--------------
		if(my_bStopFlg) return; // �I���H

		//// ���������r���Ǘ�No���Z�b�g
		memcpy(cLastKizukenNo, cKizukenNo, sizeof(cKizukenNo));


		//---------------------------------------
		// �擾�����R�C���ɕR�t�������폜
		//---------------------------------------		
		for(int jj=0; jj<m_nDelTblCoil; jj++) {
			Exec_Shrink();														// �g�����U�N�V�������k����

			// �ꊇ/���ʍ폜 ����
			if( ! Delete_Inf( &m_pDelTblCoil[jj], cKizukenNo ) ) {
				continue;		// �폜���s
			}
			if(my_bStopFlg) return; // �I���H
		}

		if(my_bStopFlg) return; // �I���H
	}


	//// �o�b�N�A�b�v�폜���s
	// �����r���Ǘ�No���A�������r���Ǘ�No�͐�΂̂����Ă��Ȃ��͂��B�i�c���Ă����炻��̓S�~�j
	if( 1 == my_nID && 0 != strlen(cLastKizukenNo)) {
		Delete_BackUp(cLastKizukenNo);
	}
	
	return;
}

//------------------------------------------
// ������s
//	�E�e�ʃ`�F�b�N�̃o�b�N�A�b�v���s���܂��s��
//------------------------------------------
void DeleTable::ExecFast()
{
	bool retc;
	if(0 == m_nDelTblCnt) return;

	//// �ŌÂ��r���Ǘ�No���擾
	char cLastKizukenNo[SIZE_KIZUKEN_NO];		
	memset(cLastKizukenNo, 0x00, sizeof(cLastKizukenNo));
	retc = Select_OldKey( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, SIZE_KIZUKEN_NO, cLastKizukenNo);
	if(!retc || 0==strlen(cLastKizukenNo)) {		// �擾�f�[�^�̐������m�F
		return;		// �擾���s ���� �f�[�^����
	}

	//// �o�b�N�A�b�v�폜���s
	// �����r���Ǘ�No���A�������r���Ǘ�No�͐�΂̂����Ă��Ȃ��͂��B�i�c���Ă����炻��̓S�~�j
	Delete_BackUp(cLastKizukenNo);
}

//------------------------------------------
// �o�b�N�A�b�v�폜
// �����r���Ǘ�No���A�������r���Ǘ�No�͐�΂̂����Ă��Ȃ��͂��B�i�c���Ă����炻��̓S�~�j
// const char* cKizukenNo �r���Ǘ�No
//------------------------------------------
void DeleTable::Delete_BackUp(const char* cKizukenNo)
{
	//// �\�[�g�L�[�w��̏ꍇ�A�\�[�g�ő傫���r���Ǘ�No���������ꍇ�A�o�b�N�A�b�v�폜�ł�������������r���Ǘ�No���S�������Ă��܂�����
	if( 0 != strcmp( my_Param.sCoilResultKey, my_Param.sCoilResultSort) ) {
		LOG(em_MSG), "[Dele] [%s] �\�[�g�L�[�w��̂��߁A�o�b�N�A�b�v�폜�@�\������ (key=%s, sort=%s) [%s]", my_cSession, my_Param.sCoilResultKey, my_Param.sCoilResultSort, cKizukenNo);
		return;
	}
	LOG(em_MSG), "[Dele] [%s] �o�b�N�A�b�v�폜���s [%s]", my_cSession, cKizukenNo);


	//// �w��L�[�����Â����т͑S�č폜
	//if( ! Delete_OldInf( DB_COIL_RESULT, "�r���Ǘ�No", cKizukenNo) ) {
	if( ! Delete_OldInf( my_Param.sCoilResultTbl, my_Param.sCoilResultKey, cKizukenNo) ) {
		return;		// �폜���s
	}
	if(my_bStopFlg) return; // �I���H


	//// �擾�����R�C���ɕR�t�������폜
	for(int jj=0; jj<m_nDelTblCoil; jj++) {
		// �ꊇ/���ʍ폜 ����
		if( ! Delete_OldInf( &m_pDelTblCoil[jj], cKizukenNo ) ) {
			return;		// �폜���s
		}			
		if(my_bStopFlg) return; // �I���H
	}
	
}