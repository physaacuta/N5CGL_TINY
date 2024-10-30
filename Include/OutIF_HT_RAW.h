#pragma once

// �C���N���[�h
#include <windows.h>

#include "LineCommon.h"
#include "..\Library\DefectLib\SockOneToAny.h"




//***************************************************************
// �N���v��

//=======================================
// �~�ϋ@�\
//=======================================
// TI_RPISAVE
#define FACT_TI_RPISAVE_01			9421					// �����J�n�v�� (��TO_DEFECT) 
#define FACT_TI_RPISAVE_02			9422					// ������~�v�� (��TO_DEFECT)

#define FACT_TI_RPISAVE_04			9424					// ���R�C����񎖑O�ʒm (��TO_DEFECT)
																// ML[0] ��(-1:�S�� 0:�\ 1:��)
																// ML[1,2] �����p�^�[�� (1�`)
																// ML[3,4] �����\�ʏ�� (1�`)
																// ML[5] �ؑփt���[��No  �� �d�v�B ���񂱂�Őؑ�
																// MC[0] �Ǘ�No
																// MC[1] �R�C��No

#define FACT_TI_RPISAVE_05			9425					// �O���R�C���ؑ֏�� (��TO_DEFECT) ���o�b�`���C���̂�
																// FL[0] �\���敪 (0:�\ 1:��)
																// FL[1] ��[���[�敪 (0:��[ 1:���[)
																// FL[2] �Ώۃt���[��No
																// FL[3] �Ώۃt���[����Y�ʒu [pixel] �i�摜�オ0pixel�Ƃ��Ďw��j

//***************************************************************
// ����Ǘ� RAW�摜���M


//=======================================
// �^�X�N����
//=======================================

// �~�ϋ@�\
#define TI_RPISAVE					"TI_RPISAVE"			// �S���摜�ۑ��Ǘ� (=�摜�����G���W��)




//=======================================
// ����Ǘ� RAW�摜���M �Ԉ����ݒ�
#define FRAME_RAW_SEND														// �R�����g�ɂ��邱�Ƃŋ@�\��~
#define FRAME_HT_SEND_PORT				2									// ���葤�̑��M�|�[�g��	
#define MAX_BUF_QUE_RPISOCK				8									// 8MB�~4�摜�~MAX_BUF_QUE_RPISOCK ���̃��������m�ۂ����B8�L���[��256MB



//=======================================
// ���聨�S���摜���� �t���[���f�[�^
struct HT_RPI_FDATA : public SOCK_F_HEAD2
{
	//// ���M���ŃZ�b�g
	int					nCamId;				// �J�����Z�b�g 
	DWORD				nFrameNo;			// ���M�A�� (0�I���W��)
	float				dEdgePos[2];		// �G�b�W�ʒu�i�J�������W�n)mm�P��


	// RAW�f�[�^�A�Œ�T�C�Y�O��
	//		��	addr[0] �̒��ɁA�摜�̈�ő�T�C�Y���Z�b�g����Ă���B
	//			�ő��肽���摜�݂̂�O�l�߂ɃZ�b�g
	int		nImgCnt;						// �摜�Z�b�g��
	int		nWidth;							// �摜��
	int		nHeight;						// �摜����
	DWORD	nImgSize[MAX_IMAGE_CNT];		// �摜�T�C�Y	�� 0 �͉摜�����B
	DWORD	nImgOffset[MAX_IMAGE_CNT];		// �摜�̐擪�ʒu(Addr����̉摜�擪�I�t�Z�b�g)	 �܂�ŏ��̉摜��0�ƂȂ�͂�

	// �󂯎�葤�ŃZ�b�g
	__int64				nTimeFrameRecv;		// ��M��������

	//========================
	// �퐶����
	// int const* p=0 �󂯓n���J�X�^�����(int �z��) 
	// [0] �摜�T�C�Y�ő�T�C�Y
	void	Alloc(int const* p)
	{
		int nSize = p[0];

		// �f�[�^��������
		memset(this, 0x00, sizeof(HT_RPI_FDATA));		// �������g��������
														// �̈�m��
		this->base_head.addr[0] = (long*)_aligned_malloc(nSize, 32);
	};

	//========================
	// ������
	void	Free()
	{
		if (NULL != this->base_head.addr[0]) {
			_aligned_free(this->base_head.addr[0]);
			this->base_head.addr[0] = NULL;
		}
	};

};

//***************************************************************
// ��`
//// �摜���
enum EM_IMG_SAVE_STAT
{				
	// �ۑ��Ώۉ摜																 �A�����C���A	�o�b�`���C��
	IMG_SAVE_STAT_SAVE = 0,											// �ۑ��Ώ�		��				��	
	IMG_SAVE_STAT_CHANGE = 9,										// �R�C���ؑ�	��				�~

	IMG_SAVE_STAT_F = 1,											// ��[�摜		�~				��
	IMG_SAVE_STAT_T = 2,											// ���[�摜		�~				��

};

//// �{�^�X�N�Ŏg�p���鑬�xUDP��� 
//		(�ΏۃV�X�e���S�����ΏۂƂ����Ƃ��́A�{�^�X�N���K�v�ȍ��ڂ̍ő����)
struct UDP_DATA {
	DWORD	framenum;												// �t���[���ԍ�			framenum
	DWORD	speed;													// ���C�����x(mm/sec)		��0
	float	vRes;													// �c����\(mm/pixel)
//	int		toVal[8];												// �����ŏ���Ɏg����̈�i�p�C�v�ɂ��̂܂܋A��j

	struct _pos {
		long	posFromHead;										// �n�ړ_����̋���(mm)		��0
		float	edgePosL;											// �G�b�W�ʒu����(mm)
		float	edgePosR;											// �G�b�W�ʒu����(mm)
	} pos[2];
};
