//=====================================================================
// �}�[�W�N���X
//	�y�S���C���z
//		�E���E�J�����̃I�[�o�[���b�v���r�̏d�Ȃ蔻����s��
//=====================================================================

#pragma once


// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X


// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCattr.h"
//#include "..\..\Include\EPC\NCsmemData.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`
#include "..\..\Include\EPC\NCL2attr.h"									// �����ʒ�`

#include "..\..\Include\TohtCommon.h"


using namespace KizuLib;

class MergeManager
{
public :

	// �p�����[�^
	struct  PARAM_MERGE {
		double			dOffset_x[NUM_CAM_POS];							// �Жʔ��萔��
		double			dLapRitu;										// �O�ڒ����`�d�Ȃ�
	};

//private:

	//=======================================
	// �\����
	//=======================================

	// �d�Ȃ胊�X�g�̃f�[�^�\�� 
	struct  nodeLap {
		int				vertex;											// �d�Ȃ�ԍ�
		nodeLap*		pNext;											// �|�C���^
	};


	// ���בΏ� �\����
	struct MergeInfo {
		double*			pAttr;											// �����ʃ|�C���^

		int				nCamset;										// �J�����Z�b�g

		// ��\�r�I�ʂƓ����d�Ȃ蔻��p
		int				yusenG;											// �O���[�h�D��x
		int				yusenT;											// �r��D��x
		double			area;											// �r�ʐ�
		double			len;											// ����ʒu


		// �悭�g�����ڂ��g���₷���悤�Ɏ��o���Ă���
		double			xmin;											// �S�J�������ʂ̐�΍��W
		double			xmax;
		double			ymin;
		double			ymax;


		// �d�Ȃ胊�X�g (�O���t�̌���������p)
		nodeLap*		lstLap;											// �d�Ȃ胊�X�g (�d�Ȃ��Ă����r������ꍇ�́A�����ɂǂ�ǂ�ǉ����Ă���)
		bool			isChecked;										// �A��������p
	};
	


public :
	MergeManager(void);
	virtual ~MergeManager(void);

	void SetLogMgr(LogFileManager* pLog)   { mcls_pLog = pLog; };		// ���O�t�@�C���Z�b�g
	void SetParam(PARAM_MERGE* prm)	{ memcpy(&m_prm, prm, sizeof(m_prm)); }	// �p�����[�^�Z�b�g


	void	DoMerge(int* nDetectCnt, MergeInfo* lst);	// �}�[�W�������s

private:
	double	CheckLap(const MergeInfo& a, const MergeInfo& b);			// �d�Ȃ藦�`�F�b�N
	int		Visit(int myrec, int maxrec, MergeInfo* lst);				// �[���D��T��

	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X


	PARAM_MERGE				m_prm;										// �p�����[�^
};
