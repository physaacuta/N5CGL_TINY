#pragma once
#include "..\..\library\kizulibmfc\class\threadmanager.h"
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X

#include "MainConst.h"



class TestMaeManager :
	public ThreadManager
{
public:
////�@���J�֐�
public:
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_KEY {
		char	cKizukenNo[SIZE_KIZUKEN_NO + 1];		// �r���Ǘ�No
		char	cKeyInf[SIZE_COIL_KEY + 1];				// �L�[���
		char	cMeiNo[SIZE_MEI_NO + 1];				// ����No
		char	cCoilNo[SIZE_COIL_NO + 1];				// HOT�R�C��No
		int		nCoilLen;
		double	dNobiSPM;								// SPM�L�ї�[10^2%]

	};
	ThreadQueue<DELIVERY_KEY>	gque_Deli;

	// �O�H���r���\����
	struct MAE_DEFECT_IMAGE_INF
	{
		char cKizuKenNo[SIZE_KIZUKEN_NO];		// �r���Ǘ�No
		char cPreKizuKenNo[SIZE_KIZUKEN_NO];	// �O�H���r���Ǘ�No
		int  nKouteiSyubetu;					// �H����ʁu�r���o�H�����v�����̉��Ԗڂ�
		int  nKizuNo;							// �rNo
		int  nMaeKizuNo;						// �O�H���rNo
		int  nOuDiv;							// �\���敪
		int  nCnt;								// �摜�f�[�^��
		DETECT_BASE_DATA typDefectData;			// �r���
		int  nGazouNo;							// �摜No
		DWORD nSize[MAX_MAE_IMAGE];				// �摜�T�C�Y
		BYTE* pImg[MAX_MAE_IMAGE];				// �摜�f�[�^
	};


	TestMaeManager(void);
	virtual ~TestMaeManager(void);

//// �����o�֐�
private:



	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);				

	void Exec( DELIVERY_KEY* pDeli);
	bool SetMaeCoilInf(char const* cKizukenNo, char const* cKeyInf, char const* cMeiNo, char const* cCoilNo);
	bool SetMaeFFInf(char const* cKizukenNo , int nKizuNo, int ou, double	dNobiSPM, int nKouteiID);
	bool SetMaeCoilLine(char const* cKizukenNo,int ou,int nKouteiID);
	bool SetMaeDefectInf(MAE_DEFECT_IMAGE_INF* typData);
	bool CreateDefectInf(char const* cKizukenNo , int nKizuNo, int ou, int nKouteiID);

	bool InsertDefectImg(MAE_DEFECT_IMAGE_INF* typData);

	void Send_Hy_MaeRead(char const* cKizukenNo);



};
