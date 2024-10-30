#include "StdAfx.h"
#include "AtlRegManager.h"

#ifdef ATLREGEXPLIB_IS_ATL
	// VS2005 �ȑO�p
	#include <atlrx.h>
#else
	// VS2008�@�ȍ~�p
	// atlrx.h ���p�~����Astd�ɐ��K�\�����ǉ����ꂽ�ׂ̑Ή�
	#include <regex>

	using namespace std;
#endif

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
AtlRegManager::AtlRegManager(void)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
AtlRegManager::~AtlRegManager(void)
{
}


//------------------------------------------
// ���K�\�� ���`�F�b�N [1byte ascii] (���{�ꖢ�Ή�) �� �ꉞ�Ή��͏o������
// char const* val �`�F�b�N������������
// char const* match ���K�\�� ��(�݂�Ȃ��ԈႤ���C���h�J�[�h�̎w����@) ".*" ��
// �߂�l true:��v  false:�_��
//------------------------------------------
bool AtlRegManager::IsRegularA(char const* val, char const* match)
{
	//char val[] = "100-12345";
	//char match[] = "{-}{.*}{2}{.*}5$";		// OK�ƂȂ�

#ifdef ATLREGEXPLIB_IS_ATL
	CAtlRegExp<> re; 
	CAtlREMatchContext<> mc; 


	// ���K�\���̖߂�l�`�F�b�N�B "aaa(" �Ƃ����Ɩ������[�v�ɂȂ�΍�
	//re.Parse(match);					// ���K�\���Z�b�g
	if( REPARSE_ERROR_OK != re.Parse(match) ) {
		return false;
	}


	if(re.Match(val, &mc)) {				// ����
		return true;
	} else {
		return false;
	}
#else
	/***
	tr1::cmatch		mr;
	tr1::regex		rx(match); 
	
	const char * last = val + strlen(val);

	if( tr1::regex_search( val, last, mr, rx) ) {
		return true;
	} else {
		return false;
	}
	***/

	return IsRegularA(val, (int)strlen(val), match);
#endif
}
//------------------------------------------
// ���K�\�� ���`�F�b�N [1byte ascii] 
// char const* val �`�F�b�N������������
// int valSize �`�F�b�N������������̃T�C�Y (�悭�v���R������NULL�����������񂪂���ׂ̑΍�)
// char const* match ���K�\�� ��(�݂�Ȃ��ԈႤ���C���h�J�[�h�̎w����@) ".*" ��
// �߂�l true:��v  false:�_��
//------------------------------------------
bool AtlRegManager::IsRegularA(char const* val, int valSize, char const* match)
{
#ifdef ATLREGEXPLIB_IS_ATL	

	char* pWk = new char[valSize+1];
	memset(pWk, 0x00, valSize+1);
	memcpy(pWk, val, valSize);

	// ��r
	bool retc = IsRegularA(pWk, match);

	// ��n��
	delete [] pWk;
	return retc;

#else
	try {
		tr1::cmatch		mr;
		tr1::regex		rx(match); 
	
		const char * last = val + valSize;

		if( tr1::regex_search( val, last, mr, rx) ) {
			return true;
		} else {
			return false;
		}

	} catch (...) {
		 return false;
	}
	
#endif
}

//------------------------------------------
// ���K�\�� ���`�F�b�N [�}���`�o�C�g] (���{��Ή�) (�ނ���ނ���x�����璍��)
// unsigned char const* val �`�F�b�N������������ (BYTE�^�ł�OK)
// unsigned char const* match ���K�\�� ��(�݂�Ȃ��ԈႤ���C���h�J�[�h�̎w����@) ".*" ��
// �߂�l true:��v  false:�_��
//------------------------------------------
bool AtlRegManager::IsRegularMB(unsigned char const* val, unsigned char const* match)
{
	//char val[] = "100-12345";
	//char match[] = "{-}{.*}{2}{.*}5$";		// OK�ƂȂ�

#ifdef ATLREGEXPLIB_IS_ATL
	CAtlRegExp<CAtlRECharTraitsMB> re; 
	CAtlREMatchContext<CAtlRECharTraitsMB> mc; 

	re.Parse(match);					// ���K�\���Z�b�g

	if(re.Match(val, &mc)) {				// ����
		return true;
	} else {
		return false;
	}
#else

	// �ꉞ�ȒP�ȃe�X�g�����Ă݂����A
	//   �Ђ炪�ȁA�����A�J�^�J�i�A���ŁA�p��
	// ���������Ă��Ă����ʂɎg����

	return IsRegularA((char const*)val, (char const*)match);
#endif
}
