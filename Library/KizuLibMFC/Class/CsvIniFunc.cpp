#include "StdAfx.h"
#include "CsvIniFunc.h"


using namespace KizuLib;

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
CsvIniFunc::CsvIniFunc(void)
{
	// �Z�N�V�����J�n��؂�
	memset(m_cSecHead, 0x00, sizeof(m_cSecHead));
	m_cSecHead[0][0] = SEC_HEAD_S;
	m_cSecHead[1][0] = SEC_HEAD_E;

}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CsvIniFunc::~CsvIniFunc(void)
{
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ʏ��

//------------------------------------------
// CSV�t�@�C���Ǎ�
// LPCSTR sSec	CSV��[]�܂ł̃L�[
// LPCSTR sPath	Ini�t�@�C���p�X
// std::vector<CString>* pRow	1�s�P�ʂ̕������X�g		
//------------------------------------------
bool CsvIniFunc::ReadCsv(LPCSTR sSec, LPCSTR sPath, std::vector<CString>* pRow)
{
	char cWk[2048];
	int	 nWk;
	bool bSecIni = false;			// �w��Z�N�V���������o������Atrue


	//================================================
	// �I�[�v��
	FILE* fp = fopen(sPath, "rt"); 
	if(NULL == fp) {
		return false;
	}

	//================================================
	// �ǂݍ���
	while (!feof(fp)) {
		char* ret = fgets(cWk, sizeof(cWk), fp);	// 1�s�擾
		if( 0 == ret ) break;

		//// �󔒍s, �R�����g�s�� �ΏۊO
		bool bAddOk = false;				// �L���s
		bool bSectionFind = false;			// �Z�N�V�����s
		nWk = -1;
		int nCnt = (int)strlen(cWk);
		// ��[�̂݃`�F�b�N�p�t�H�[�}���X�΍�
		if( ! bSecIni ) {
			if( 10 < nCnt ) nCnt = 10;			
		}
		for(int ii=0; ii<nCnt; ii++) {
			nWk ++;

			if( cWk[nWk] == ' ' || cWk[nWk] == '\t' ) continue;
			if( cWk[nWk] == 0x0d || cWk[nWk] == 0x0a) {
				// �����ŕ����񂪏I���̔F���ɂ���
				cWk[nWk] = 0x00;
				break;
			}
			//�R�����g�s�`�F�b�N�B
			if(!bAddOk && cWk[nWk] == '#' )
			{
				cWk[nWk] = 0x00;
				break;
			}
			//// �Z�N�V�����J�n��؂���T�[�`�B�擪��������
			if(!bAddOk && strchr( m_cSecHead[0], cWk[nWk] ) != NULL ){ 
				bSectionFind=true;
				break;
			}

			bAddOk = true;
		}

		if( bSectionFind && ! bSecIni ) {
			// ����Z�N�V�������H
			if(NULL != strstr(cWk, sSec)) {					
				bSecIni = true;
				continue;				// �Z�N�V���������͖���
			}
		}

		//// �����炵���Z�N�V�������łĂ����̂ł��̃Z�N�V�����͏I���B
		if( bSectionFind && bSecIni ) {
			break;
		}


		//// �����܂ŗ�����A�w��Z�N�V�������ł̒ǉ��Ώۃf�[�^
		if(bAddOk && bSecIni) {
			CString s = cWk;
			//s.Remove('"');
			//s.Remove(' ');			// SQL�Ƃ��̈Ӗ��̂�����̂܂�NG�ƂȂ�
			//s.Remove('	'); 
			//s.Remove(0x0a); 
			//s.Remove(0x0d); 
		
			s.Trim(" \t");

			// [,,] ����2�Ƃ��Ď擾�ł��Ȃ��̂ŁA [, , ] �Ƃ����悤�ɃX�y�[�X��K���}�� �d�v
			//s.Replace(_T(",,"), _T(", , "));


			if( !s.IsEmpty() ) {
				pRow->push_back(s);
			}
		}
	}

	//================================================
	// �N���[�Y
	fclose(fp);
	return true;
}

//------------------------------------------
// �v�f�������邩 (�P���� , �������邩�����Ŕ��f)
// CString& sVal 1�s�P�ʂ̏��
// �߂�l �v�f��
//------------------------------------------
int CsvIniFunc::CntColumn(const CString& sVal)
{
	CString sWk = sVal;
	sWk.Trim(_T(" 	"));

	// ��{���擾
	int nTotal = sWk.GetLength();
	if( 0 == nTotal ) return 0;
	
	// , �폜
	sWk.Remove(',');
	return nTotal - sWk.GetLength() + 1;
}

//------------------------------------------
// �P�s�̃f�[�^���� , �P�ʂ̕�������擾
// CString& sVal 1�s�P�ʂ̏��
// int& curPos ��؂�ʒu
// CString& sToken �擾������̕�����
// �߂�l ���A���
//------------------------------------------
bool CsvIniFunc::GetColumn(const CString& sVal, int& curPos, CString& sToken)
{
	static const char CH_DQUOTE = '"';
	static const char CH_COMMA  = ',';

	// �_�u���N�H�[�e�[�V�������ɃJ���}������ꍇ�͕����Ƃ��Ĉ���
	// �_�u���N�H�[�e�[�V�������ɁA�����Ƃ��Ẵ_�u���N�H�[�e�[�V�����́A
	// ���̒��O�ɂЂƂ_�u���N�H�[�e�[�V������t�����āA�G�X�P�[�v�����B
	CString sWk = "";
	int	nLength = sVal.GetLength();
	bool bWQuote = false;				// �_�u���N�H�[�e�[�V���������ǂ����B

	int ii;
	for(ii=curPos; ii<nLength; ii++)
	{
		const char ch = sVal[ii];
		// �Ώۂ̕������_�u���N�H�[�e�[�V�����̏ꍇ
		if(CH_DQUOTE == ch) {

			if(ii+1 >= nLength) break; // �s�����_�u���N�H�[�e�[�V�����̏ꍇ�͖�������break
			const char ch2 = sVal[ii+1];
			// �G�X�P�[�v�Ώہi���������̃_�u���N�H�[�e�[�V�����j�����`�F�b�N�B
			if(bWQuote && CH_DQUOTE==ch2) {
				// �Q�������P�����ɂȂ�̂ŁAii�͂����łP�����߂�B
				// ������continue�Ƃ��킹�Č��ǂ͂Q�����ށB
				ii++;

				sWk += ch;
			}
			else {
				// ���̃_�u���N�H�e�[�V���������ꂽ�̂ŁA
				// �_�u���N�H�e�[�V��������Ԃ́A�O�����A�����O�Ɣ��]����B
				bWQuote = !bWQuote;
			}
			continue;
		}

		//�_�u���N�H�[�e�[�V�������̏ꍇ�́A��؂蕶���Ƃ��Ă͈���Ȃ��B
		if(!bWQuote){
			if(CH_COMMA == ch) {
				// ��؂�Ƃ��ẴJ���}�Ȃ̂Ńt�B�[���h�I���B
				 break; 
			}
			if( '#' == ch ){
				//���ŏo�Ă����V���[�v�͈ȍ~���R�����g��������B
				ii = nLength;
				break;
			}
		}


		//�t�B�[���h������̗v�f�Ȃ̂ŁA�ǋL����B
		sWk += ch;
	}
	sWk.Trim(_T(" 	"));					// �O��̃X�y�[�X�A�^�u����菜��
	sToken = sWk;							// ����̃t�B�[���h������
	curPos = ii+1;							// ��؂�ʒu���A���̃t�B�[���h�܂Ői�߂�

	// �ǂݍ��񂾕����񂪂Ȃ�
	if( 0 == sWk.GetLength() ) return false;
	return true;
}

CString CsvIniFunc::GetColumnStr(const CString& sVal, int& curPos)
{
	CString		sWk;
	if( ! GetColumn(sVal, curPos, sWk) ) return "";
	if( 0 == sWk.CompareNoCase("NULL") ) return "";	// NULL�`�F�b�N
	return sWk;
}
void CsvIniFunc::GetColumnChar(const CString& sVal, int& curPos, char* cAns, int nSize)
{
	CString		sWk;
	cAns[0] = 0x00;
	if( ! GetColumn(sVal, curPos, sWk) ) return;
	if( 0 == sWk.CompareNoCase("NULL") ) return;	// NULL�`�F�b�N

	int nCopyLength = sWk.GetLength();
	if(nCopyLength > nSize - 1) nCopyLength = nSize - 1;
	memset( cAns, 0x00, nSize);
	memcpy( cAns, sWk, nCopyLength);

}
int CsvIniFunc::GetColumnInt(const CString& sVal, int& curPos)
{
	CString		sWk;
	if( ! GetColumn(sVal, curPos, sWk) ) return 0;
	if( 0 == sWk.CompareNoCase("NULL") ) return 0;	// NULL�`�F�b�N
	return atoi(sWk);
}
double CsvIniFunc::GetColumnDouble(const CString& sVal, int& curPos)
{
	CString		sWk;
	if( ! GetColumn(sVal, curPos, sWk) ) return 0;
	if( 0 == sWk.CompareNoCase("NULL") ) return 0;	// NULL�`�F�b�N
	return atof(sWk);
}