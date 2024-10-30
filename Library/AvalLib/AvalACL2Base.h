// *********************************************************************************
//	Aval�{�[�h�ڑ� ��� �N���X
//	[Ver]
//		Ver.01    2014/07/07  vs2005 �Ή�
//
//	[����]
//		�EAval�ւ̒��ڃA�N�Z�X�́A�{�N���X�̂�
//		�EFC �� AvalACL2Batch�ɑ���
//	
//	�G���W����AvalACL2Batch�ɑ����B�R�[���o�b�N�ł͖������ځB
//
//   camera --> driver --> ACapLib2 --> my_buff[nbuff]
//         �Emy_buff��DMA�����O�o�b�t�@�B����ɏ㏑�������B
//				��DMA�́ACPU����̒��ڃA�N�Z�X
//         �EWaitNFrameReady()�ŐV���ɏ����ꂽ�t���[�������擾(�|�[�����O�̏ꍇ�͌Ă΂Ȃ��Ă��ǂ��j
//         �EGetFramePointers()��my_buff�̗v�f���擾�A���񂨂���`�F�b�N
//         �ECheckOverrun()�ŃR�s�[�̑ΏۂƂȂ���my_buff��DMA�̈悪�N�����Ă��Ȃ����Ƃ��`�F�b�N
//   StartGrab() -> WaitNFrameReady() -> GetFramePointers() -> CheckOverrun()
//   AbortFrameWait() -> StopGrab()
//
// *********************************************************************************
#pragma once


#include <Windows.h>

#include "..\KizuLibMFC\CLASS\FuncBase.h"											// 

// Aval �w�b�_�[
	// �K�{�̃w�b�_�[�̂�
	// .c �ŕK�v�ȃw�b�_�[�� *.c�Œ�`



namespace AvalLib
{
	//===================================================================
	// ���N���X
	//===================================================================
	class AvalACL2Base : public FuncBase
	{
	public:

        //  �t���[���҂���Lock
        //   pimage: �o�b�t�@�̐擪�|�C���^�B�T�C�Y��GetImageNBytes()�B
        //   framenum: 0�n�܂�BStartGrab()����x��0�ɖ߂�B
        enum FrameWaitResult { FrameWait_Success = 0,
                               FrameWait_Timeout, FrameWait_Aborted, FrameWait_ACLError,
							   FrameWait_Overrun,
                               FrameWait_Unknown };

	public:
		AvalACL2Base();
		virtual ~AvalACL2Base(void);

		//---------------------------
		// init and dispose
		virtual BOOL Init(int board_id, int board_ch_id, const char* ini_filename, int nbuff);
		virtual void Dispose();

		virtual void SetThreadName(CString sThreadName) { FuncBase::my_sThreadName = sThreadName;}

		//---------------------------
		// image
        inline int GetWidth()     const { return my_nWidth; }
        inline int GetHeight()    const { return my_nHeight; }
        inline int GetPixelNBit() const { return my_nPixelNBit; }
        //
        inline int GetImageNBytes() const { return my_nBufSize; }
		BOOL		reload_size();											// SetInfo�ŃT�C�Y���ς�����ꍇ��my_width�����Đݒ�

		//---------------------------
		// grab
		BOOL				StartGrab();									// �B���J�n
		BOOL				StopGrab();										// �B����~
		FrameWaitResult		WaitNFrameReady(DWORD& nFrame, DWORD nTimeout);	// �V�K�t���[�����̎擾 (�t���[�����͂܂Ńu���b�N)
		FrameWaitResult		GetFramePointers(void** ptrs, DWORD ptrslen, DWORD& nvalid, DWORD& from_framenum);		// �|�C���^�̎擾
		BOOL				CheckOverrun(int nReadFrameCount);									// �ǂݍ��ݒx��`�F�b�N
		void				AbortFrameWait();								// �t���[���҂�(WaitNFrameReady)��Ԃ̉���


		// stat counters
		inline void ClearStatCounters()	{ my_nFrameOverrun = 0; }		// �J�E���g�l�̃N���A
		DWORD	GetNFrameOverrn() const		{ return my_nFrameOverrun; }	


		//---------------------------
		// serial communication
		BOOL OpenSerialPort();										// �I�[�v��
		void CloseSerialPort();										// �ؒf
		BOOL SendToSerialPort(const void* data, int len);			// ���M
		BOOL RecvFromSerialPort(void* buff, int buff_len, int&recv_len, DWORD timeout);		// ��M
		BOOL SetSerialParam(int baud);								// �V���A�� �p�����[�^�ݒ�

		//---------------------------
		// AcapSetInfo/GetInfo
		BOOL	SetInfo(DWORD ulType, int nValue);					// �p�����[�^�ݒ�
		BOOL	GetInfo(DWORD ulType, int& nValue);					// �p�����[�^�ǂݍ���
		DWORD	GetLastError(DWORD* err_board, DWORD* err_ext);		// �G���[���擾




	//// �����o�[�ϐ�
	protected:
		// �d�v���
		HANDLE					my_hDev;									// (0�I���W��)
		DWORD					my_nBoardChId;								// (1�I���W��)

        // error
        DWORD					my_errcode_common;
        DWORD					my_errcode_board;
        DWORD					my_errcode_ext;

		// image
		DWORD					my_nWidth;
		DWORD					my_nHeight;
		DWORD					my_nPixelNBit;

        // grab
        bool					my_bRunning;								// �B���� true

		// internal buffer
		DWORD					my_nBufNum;									// �o�b�t�@��
		DWORD					my_nBufSize;								// �o�b�t�@�T�C�Y	(�摜�T�C�Y)
		std::vector<BYTE*>		my_pBuff;									// �o�b�t�@�̈� (BYTE [�o�b�t�@��][�o�b�t�@�T�C�Y])
	
		//          ���O��GetFramePointers�Ŏg�p���ꂽmy_buff�̗̈�
		//          my_buff[my_last_from]�`my_buff[my_last_from+my_last_count-1]
		DWORD					my_nLastFrom;								// �O��ǂݍ��񂾈ʒu
		AvalACL2Base::FrameWaitResult GetReadyFrameCount(DWORD& nFrameCount);		// ���̓t���[�����̎擾


        // stats
		DWORD					my_nFrameOverrun;	// GetFramePointers/CheckOverrun�Ō��o�����I�[�o�[�����t���[�����ݐ�

	};
};