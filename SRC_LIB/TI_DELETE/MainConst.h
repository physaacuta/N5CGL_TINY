// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/01  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// ��{�I�ɂǂ��ł��g�p����\��������C���N���[�h
#include "..\..\Include\KizuCommon.h"								// �r�����ʃw�b�_�[



//=======================================
// �ȒP�ȋ��ʊ֐�
//=======================================
static CString GetThreadNameArray(char* c, int nNo) {
	CString s;
	s.Format("%s_%d", c, nNo);
	return s;
}
//------------------------------------------
// �P�s�̃f�[�^���� , �P�ʂ̕�������擾
// CString& sVal 1�s�P�ʂ̏��
// int& curPos ��؂�ʒu
// CString& sToken �擾������̕�����
// �߂�l ���A���
//------------------------------------------
static bool GetColumnVal(CString& sVal, int& curPos, CString& sToken)
{
	CString sWk;
	sWk = sVal.Tokenize(",", curPos);
	sWk.Trim(_T(" 	"));					// �O��̃X�y�[�X�A�^�u����菜��
	if( 0 == sWk.GetLength() ) return false;

	sToken = sWk;
	return true;
}


//=======================================
// �萔
//=======================================
static const int CONTROL_MAXCNT				= 10;					// �R���g���[���X���b�h�ő吔


//------------------------------------------
// ���Q�O���[�h�폜��`
//------------------------------------------
static const int MAX_TBL_MAIN	= 4;
static const int MAX_TBL_SUB	= 16;
struct SETTING_TBL {
	CString		cTblResult;											// �R�C�����уe�[�u��
	CString		cKeyResultTime;										// �R�C�����уe�[�u���̓��t��
	CString		cKeyMain;											// ��L�[��
	CString		cKeySub;											// �T�u�L�[��

	int			nMainCnt;											// ���C���Z�b�g��
	struct MAIN {
		CString		cTblMain;										// ���C���e�[�u����
		CString		cSqlWhere;										// ���oSQL

		int			nSubCnt;										// Sub�Z�b�g��
		CString		cTblSub[MAX_TBL_SUB];							// Sub�e�[�u����
	} main[MAX_TBL_MAIN];
};
