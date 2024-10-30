// *********************************************************************************
//	Grabber����n�N���X
//	 (iPort�ڑ� ���� �N���X)  �� �̂́AiPort��p���������AGrabber�p�Ƃ��Ďg�p����
//
//	[Ver]
//		Ver.01    2010/01/29  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************
#pragma once

// �r�����C�u����
#include "..\KizuLib\Class\KizuPerformanceCounter.h"						// �^�C�}�[
#include "..\KizuLibMFC\CLASS\ThreadLoopQueue.h"							// ��]���i�`���̃X���b�h�L���[


// �C���|�[�g
using namespace KizuLib;

////
namespace IportLib
{
#ifndef HAS_IPORT
	enum {
		NoErr = 0,
		ApErr = -1,
	};
#endif

	struct  IAlloc {					// �������Ǘ��֐�
		virtual void *Alloc(int nSize) = 0;
		virtual void Free(void *pMem) = 0;
	};

	//===================================================================
	// �󂯓n���t���[���摜 (ThreadLoopQueue���ɒ�`����Ă���A�C���^�[�t�F�[�X���p��)
	//===================================================================
	class FRAME_DATA : public IThreadLoopItem {
	//// ���J�ϐ�
	public:
		DWORD			nCamset;										// �J�����Z�b�g		(0�I���W��) (1�R����)
		BYTE*			pImgs;											// �摜				(�J�����p�x��)
		DWORD			nLen;											// �摜�T�C�Y		(1�R����) (�X�ŃT�C�Y���قȂ�摜�`���͑Ή����Ȃ�)

		DWORD			nNo;											// iPort�t���[��No	(0�I���W��) (1�R����)
		//WORD			nSeqNum[32];									// iPort�V�[�P���X�ԍ�
		DWORD			nSeqNum[32];
		
		HANDLE			hInfo[4];										// SetAddInfo �ŕt�^�������
		KizuPerformanceCounter	cls_Time;								// �擾��������(�X�g�b�v�E�H�b�`�@�\���g�p)
																			// ���C�u�������ŃZ�b�g����^�C���X�^���v
																			// ���O�F�ŏ��̃J�����p�x���擾��������
																			// ���P�F�S���̃J�����p�x�������āA�摜���R�s�[������������


		static const int ALLOC_ARGSPTR_SIZE = 2;						// Alloc���̈����T�C�Y

	// �����o�[�ϐ�
	protected:
		IAlloc* m_alloc;

	// �����o�[�֐�
	protected:
		//========================
		// �퐶����
		// int const* p=0 �󂯓n���J�X�^�����(int �z��) 
			// [0] �摜�T�C�Y (�{���A1*2�ł������A�悭�g������n���Ă���) �� �J���[�̏ꍇ�́A1�摜�~3�J���[���̒l���Z�b�g
			// [1] �J�����p�x�����Z�b�g���Ă������� (�݊��ׂ̈ɁA0�̏ꍇ�́A�����I��1�ɂ���)
			// [4] �������Ǘ��֐��Q�̃|�C���^
			// [5] ����
		virtual void	Alloc(int const* p=0)
		{ 
			int fsize	= p[0];
			int angle	= p[1];
			if(angle<=0) angle = 1;

			m_alloc = *(IAlloc**)&p[4];

			// �摜�̈�m��
			//pImgs		= new BYTE[ fsize*angle ];
			if (m_alloc){
				pImgs = (BYTE*)m_alloc->Alloc(fsize*angle);
			}
			else {
				pImgs = (BYTE*)_aligned_malloc(fsize*angle, 32);
			}
			if (NULL == pImgs) return;

			// ������
			memset(pImgs, 0x00, fsize*angle);
			nCamset		= 0;
			nLen		= 0;
			nNo			= 0;
			for(int ii=0; ii<4; ii++) hInfo[ii]	= INVALID_HANDLE_VALUE;
			memset(nSeqNum, 0x00, sizeof(nSeqNum));
		};

		//========================
		// ������
		virtual void	Free()
		{ 
			//delete [] pImgs;
			//pImgs = NULL;
			if( NULL != pImgs ) {
				if (m_alloc) {
					m_alloc->Free(pImgs);
				}
				else {
					_aligned_free(pImgs);
				}
				pImgs = NULL;
			}
		};
	
	public:
		//========================
		// �R���X�g���N�^
		FRAME_DATA(void) {
			pImgs = NULL;
		};

		//========================
		// �퐶����
		virtual ~FRAME_DATA(void) {
			if( NULL != pImgs ) {
				if (m_alloc) {
					m_alloc->Free(pImgs);
				}
				else {
					_aligned_free(pImgs);
				}
				
				pImgs = NULL;
			}
		};

	};


	//===================================================================
	// ���[�j���O��ُ펞�̎󂯓n�����
	//===================================================================
	// �󂯓n���\���̂̎��
	enum EM_DELIVERY_ID {
														//	�ڰ�No,			��׾��	��׊p�x	�ð��	�T�v
		DIV_ID_ERROR_NO_IMG_QUEFULL			= 400,		//	my_nFrameNum	��		0�Œ�,	0		�E�X���b�h���[�v�L���[�ɋ󂫖���
		//DIV_ID_ERROR_NO_IMG_QUELOCK			= 401,	//	my_nFrameNum	��		0�Œ�,	0		�E�X���b�h���[�v�L���[�����b�N��
		DIV_ID_ERROR_GRABBER_FRAME_OVERRUN	= 10000,	//	my_nFrameNum	��		��		status	�ELockForRead�Ŏ擾����status�Ŕ���
        DIV_ID_ERROR_GRABBER_FIFO_OVERRUN,				//	my_nFrameNum	��		��		status	�ELockForRead�Ŏ擾����status�Ŕ���
        DIV_ID_ERROR_GRABBER_IMAGE_DROPPED,				//	my_nFrameNum	��		��		status	�ELockForRead�Ŏ擾����status�Ŕ���
        DIV_ID_ERROR_GRABBER_PARTIAL_LINE_MISSING,		//	my_nFrameNum	��		��		status	�ELockForRead�Ŏ擾����status�Ŕ���
        DIV_ID_ERROR_GRABBER_FULL_LINE_MISSING,			//	my_nFrameNum	��		��		status	�ELockForRead�Ŏ擾����status�Ŕ���
        DIV_ID_ERROR_GRABBER_FRAME_LOCK_TIMEOUT,		//	my_nFrameNum	��		��		0		�ELockForRead�^�C���A�E�g��
		DIV_ID_ERROR_GRABBER_FRAME_LOCK_FAILED,			//	my_nFrameNum	��		��		CyRetc	�ELockForRead���s��
		DIV_ID_ERROR_GRABBER_IMAGEID_MISMATCH			//	my_nFrameNum	��		��		iPort�ԍ�	�E�����ŊǗ����Ă���A�Ԃ�iport����擾�����ԍ�������Ă���Ƃ�
	};


	// �󂯓n���\����
	struct DELIVERY_INFO {
		EM_DELIVERY_ID	em;												// ���
		
		DWORD			nFrameNum;										// �t���[��No   (0�I���W��)
		DWORD			nCamset;										// �J�����Z�b�g (0�I���W��)
		DWORD			nAngle;											// �J�����p�x   (0�I���W��)
		DWORD			status;											// ���ۂ̃X�e�[�^�X���
	};



	//===================================================================
	// �ŗL�萔
	//===================================================================
	//// �J���[�z��
	enum IPORT_COLOR_IDX {
		IPORT_COLOR_R = 0,
		IPORT_COLOR_G,
		IPORT_COLOR_B
	};
};