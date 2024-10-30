//=====================================================================
// FrameNo�Ńg���b�L���O�\�Ƃ���N���X
//	�y�S���C���z
//		�E���񏈗��ɑΉ��ς�
//		�E��{�I�Ɋ��S���p�\�Ȃ悤�ɍ��
//		�E�]���̑��xUDP�ɕt������ToVal�Ɠ����@�\�������\
//		�E����No�̕����o�^��NG
//=====================================================================
#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\Class\AutoLock.h"
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// ���W���[���̊��N���X

// ���Œ�
//#include "..\..\Include\LineCommon.h"									// �C���N���[�h

// ���[�J���̃N���X
//#include "MainConst.h"

// std
#include <list>


namespace KizuLib
{
	// //////////////////////////////////////////////////////////////////////////////
	// �e���v���[�g�Ȃ̂ŁA�N���X�̒��ɓ���Ă��g���ɂ����̂ŁA�O����

	// �ǂ��g�����̂Ƃ��ăx�[�X�̍\���̂��`�B�p�����Ďg���ĂˁB
	struct TR_FRAME_BASE {
		DWORD				nFrameNo;									// ��t���[��No

		int					nToVal[2];									// �����R��
	};



	// //////////////////////////////////////////////////////////////////////////////
	// ���C��

	template <class TTT>
	class TrackingFrame : public FuncBase
	{

	//// ���J�֐�
	public:
		TrackingFrame();
		virtual ~TrackingFrame();


		// �O���A�N�Z�X
		bool Alloc(int nMaxIns);											// �̈�m��
		void Free();														// �̈���

		// ���
		int  GetQueCnt(int nInsId) { return m_pList[nInsId].size(); }


		// ����
		bool IsFrameNo(int nInsId, DWORD nFrameNo);							// �o�^�ς݂��`�F�b�N
		bool AddItem(int nInsId, TTT* typ);									// �A�C�e���ǉ�
		bool GetItem(int nInsId, DWORD nFrameNo, TTT* typ);					// ���o�� (�����Ȃ��̂Œ���)
		bool GetDelItem(int nInsId, DWORD nFrameNo, TTT* typ);				// ���o�� & �폜
		bool DelItem(int nInsId, DWORD nFrameNo);							// �폜
		int  DelAll(int nInsId);											// �S�v�f�폜

	//// �����o�[�ϐ�
	protected:
		CRITICAL_SECTION		my_cs;										// �N���e�B�J���Z�N�V����

	private:
		int						m_nMaxIns;									// �C���X�^���X��
		std::list<TTT>*			m_pList;									// �g���b�L���O�A�C�e���B m_nIns�����̔z�񂪂���B

	};



	// //////////////////////////////////////////////////////////////////////////////
	// �e���v���[�g�Ȃ̂Ńw�b�_�t�@�C���ɓ���Ă����K�v����

	//------------------------------------------
	// �R���X�g���N�^
	//------------------------------------------
	template <class TTT>
	TrackingFrame<TTT>::TrackingFrame() :
	FuncBase("TrFrame"),
	m_nMaxIns(0),
	m_pList(NULL)
	{
		InitializeCriticalSection(&my_cs);
	}

	//------------------------------------------
	// �f�X�g���N�^
	//------------------------------------------
	template <class TTT>
	TrackingFrame<TTT>::~TrackingFrame()
	{
		Free();

		DeleteCriticalSection(&my_cs);
	}


	//------------------------------------------
	// �̈�m�ہB �S�����ݒ芮����
	// int nMaxIns	��n���X�g�̐��B ���Ƃ��΁A�\���A�P��/�����������4�B
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::Alloc(int nMaxIns)
	{
		// ���Ɏ��s�ς݂ł���Έُ�
		if( NULL != m_pList ) return false;

		// �̈�m��
		m_nMaxIns = nMaxIns;
		m_pList = new std::list<TTT>[m_nMaxIns];
		return true;
	}

	//------------------------------------------
	// �̈���
	//------------------------------------------
	template <class TTT>
	void TrackingFrame<TTT>::Free()
	{
		if( NULL == m_pList ) return;
		delete [] m_pList;
		m_pList = NULL;
	}

	//------------------------------------------
	// �o�^�ς݂��`�F�b�N
	// int nInsId �Ώۂ̃C���X�^���X (0�I���W��) -1:�S���ɓo�^
	// DWORD nFrameNo	�`�F�b�N����t���[��No
	// �߂�l	�P�ł������true
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::IsFrameNo(int nInsId, DWORD nFrameNo)
	{
		if( NULL == m_pList ) return false;
		int is = -1 == nInsId ? 0 : nInsId;
		int ie = -1 == nInsId ? m_nMaxIns-1 : nInsId;

		AutoLock lock(&my_cs);
		for(int ii=is; ii<=ie; ii++) {

			std::list<TTT>::iterator	itr;					// �C�e���[�^
			for( itr = m_pList[ii].begin(); itr != m_pList[ii].end(); itr++ ) {
			
				// �R�s�[�̓R�X�g�̖��ʂȂ̂ŁA�Q�Ƃɂ��Ă���
				//TR_FRAME_BASE val = (TR_FRAME_BASE)(*itr);
				TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);
				
				if(val.nFrameNo == nFrameNo) {
					return true;
				}
			}
		}
		return false;
	}


	//------------------------------------------
	// �o�^
	// int nInsId �Ώۂ̃C���X�^���X (0�I���W��) -1:�S���ɓo�^
	// ���A���
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::AddItem(int nInsId, TTT* typ)
	{
		if( NULL == m_pList ) return false;
		int is = -1 == nInsId ? 0 : nInsId;
		int ie = -1 == nInsId ? m_nMaxIns-1 : nInsId;

		AutoLock lock(&my_cs);

		// �Ώۂɓo�^
		for(int ii=is; ii<=ie; ii++) {
			m_pList[ii].push_back(*typ);
		}
		return true;
	}

	//------------------------------------------
	// ���o��
	// int nInsId �Ώۂ̃C���X�^���X (0�I���W��)
	// DWORD nFrameNo	�擾����t���[��No
	// TTT* typ	�擾���
	// ���A���  false:�擾�f�[�^�����B   true:�擾�f�[�^�L��
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::GetItem(int nInsId, DWORD nFrameNo, TTT* typ)
	{
		if( NULL == m_pList ) return false;

		AutoLock lock(&my_cs);

		//// ���X�g�i�[�� �S���`�F�b�N���s��
		std::list<TTT>::iterator	itr = m_pList[nInsId].begin();	// �C�e���[�^
		while( itr != m_pList[nInsId].end() ) {

			TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);
			if(val.nFrameNo == nFrameNo) {

				memcpy(typ, &val, sizeof(TTT));
				return true;
			}
			itr++;
		}
		return false;
	}


	//------------------------------------------
	// ���o�� & �폜
	// int nInsId �Ώۂ̃C���X�^���X (0�I���W��)
	// DWORD nFrameNo	�擾����t���[��No
	// TTT* typ	�擾���
	// ���A���  false:�擾�f�[�^�����B   true:�擾�f�[�^�L��
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::GetDelItem(int nInsId, DWORD nFrameNo, TTT* typ)
	{
		if( NULL == m_pList ) return false;

		AutoLock lock(&my_cs);

		//// ���X�g�i�[�� �S���`�F�b�N���s��
		std::list<TTT>::iterator	itr = m_pList[nInsId].begin();	// �C�e���[�^
		while( itr != m_pList[nInsId].end() ) {

			TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);
			if(val.nFrameNo == nFrameNo) {

				memcpy(typ, &val, sizeof(TTT));

				// ���X�g����폜
				itr = m_pList[nInsId].erase(itr);	// �폜������A���̃C�e���[�^��߂�
				return true;
			}
			itr++;
		}
		return false;
	}

	//------------------------------------------
	// �폜
	// int nInsId �Ώۂ̃C���X�^���X (0�I���W��)
	// DWORD nFrameNo	�폜����t���[��No
	// ���A���  false:�폜�f�[�^�����B   true:�폜�f�[�^�L��
	//------------------------------------------
	template <class TTT>
	bool TrackingFrame<TTT>::DelItem(int nInsId, DWORD nFrameNo)
	{
		if( NULL == m_pList ) return false;

		AutoLock lock(&my_cs);

		//// ���X�g�i�[�� �S���`�F�b�N���s��
		std::list<TTT>::iterator	itr = m_pList[nInsId].begin();	// �C�e���[�^
		while( itr != m_pList[nInsId].end() ) {

			TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);
			if(val.nFrameNo == nFrameNo) {

				// ���X�g����폜
				itr = m_pList[nInsId].erase(itr);	// �폜������A���̃C�e���[�^��߂�
				return true;
			}
			itr++;
		}
		return false;
	}

	//------------------------------------------
	// �S�v�f���
	// int nInsId �Ώۂ̃C���X�^���X (0�I���W��)
	// �߂�l		�폜����
	//------------------------------------------
	template <class TTT>
	int TrackingFrame<TTT>::DelAll(int nInsId)
	{
		int nCnt = 0;
		if( NULL == m_pList ) return nCnt;

		AutoLock lock(&my_cs);

		//// ���X�g�i�[�� �S���`�F�b�N���s��
		std::list<TTT>::iterator	itr = m_pList[nInsId].begin();	// �C�e���[�^
		while( itr != m_pList[nInsId].end() ) {

			TR_FRAME_BASE& val = (TR_FRAME_BASE&)(*itr);

			// ���X�g����폜
			itr = m_pList[nInsId].erase(itr);	// �폜������A���̃C�e���[�^��߂�
			nCnt++;
		}
		return nCnt;
	}
};
