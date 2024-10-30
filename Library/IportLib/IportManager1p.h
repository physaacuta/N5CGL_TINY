#pragma once

//#ifdef IPORTLIB_IMG_COPY_MMX
//	// MMX��SwdetectMMX::mem_copy_64 ���g�p����ꍇ
//	// EPC�p
//	#include "..\..\EPC\lib\mmx\MemCopy.h"
//#endif
#include "..\KizuMMX\MemCopy.h"												// MMX���C�u����


// ���[�J��
#include "IportBase.h"														// iPort���N���X
#include "IportManager.h"													// iPort���N���X


// �C���|�[�g
using namespace KizuLib;
using namespace ImgLib;


namespace IportLib
{

	//===================================================================
	// �J�����捞�{��
	//===================================================================
	class IportManager1p : public IportManager
	{

	//// ���J�\����
	public:


	//// ���J�֐�
	public:
		IportManager1p(CString sMyManagerName="iPortMgr", CString sMyIport="iPortBase");
		virtual ~IportManager1p(void);
		
		//virtual void		SetQueFrame(ThreadLoopQueue<FRAME_DATA>* pQue) { myque_pFrame = pQue;}


		// ���̂� GraberIF
		virtual bool		Init(DWORD nFrameW, DWORD nFrameH, DWORD camset=1, DWORD angle=1, BYTE nDummyVal=0x80);	// ������
		virtual void		Dispose();										// ���

		virtual bool		Start(); 										// �X���b�h�J�n �� ���̃X���b�h�́A�z���g�ɃX���b�h�����s�����܂Ń��b�N�����
		virtual bool		Stop();											// �X���b�h��~
		
		// �B���n GraberIF
		virtual int			StartGrab();									// �B���J�n
		virtual int			StopGrab();										// �B����~
		virtual bool		Reset();										// ���Z�b�g

		// iPort����n
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, 
										bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, 
										int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);	// iPort�ɐڑ� (1p) �ݒ�:XML, Master:XML
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, 
										const char* mac_addr, const char* ip_addr, char const* mc_addr, 
										bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, 
										int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);	// iPort�ɐڑ� (1p) �ݒ�:XML, Master:IP��

		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, long nAdapterIndex1,
										const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) { _ASSERT(false); return NoErr; }	// iPort�ɐڑ�(2p) �ݒ�:XML, Master:XML, Slave:IP��
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath,
									    const char* mac_addr1, const char* ip_addr1, long nAdapterIndex1, char const* mc_addr1,
										const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) { _ASSERT(false); return NoErr; }	// iPort�ɐڑ�(2p) �ݒ�:XML, Master:IP��, Slave:IP��

		virtual void		DisConnect();									// iPort�Ɛؒf


		// ���̑�����
		virtual bool		SendSerialTemp(DWORD set, DWORD ang, float& temp);	// �J�������x���擾 (�J�����Ɛڑ��������_��)


	//// �v���p�e�B�֌W
	public:
		// �I�u�W�F�N�g�擾

		//(IportBase&)*mycls_pIport[0] ���킩��Ȃ��l����
		//		mycls_pIport[0] �́AIportBase�̃|�C���^�������Ă���
		//		*mycls_pIport[0]�ŁAIportBase�̖{�̂ɃA�N�Z�X
		//		�����(IportBase&)�Ō^�ϊ�

		virtual inline IportBase&		GetClsIpot(DWORD set, DWORD ang, bool isPrimary=true)	    { return GetBase(set, ang); }
		virtual inline int				GetCamIndex(DWORD set, DWORD ang) const						 // �S��0�I���W�� (�J�����P��)
			{ 	_ASSERT(set < my_nCamsetNum);
				_ASSERT(ang < my_nAngleNum);
				return set*my_nAngleNum + ang;
			}		
		virtual inline bool IsPT2000() const		{return false;}


		// �摜���
        virtual long		GetFrameWidth()     const { return mycls_pIport[0]->GetFrameWidth(); }
        virtual long		GetFrameHeight()    const { return mycls_pIport[0]->GetFrameHeight(); }
        virtual long		GetFrameNChannel()  const { return mycls_pIport[0]->GetFrameNChannel(); }
        virtual long		GetFramePixelNBit() const { return mycls_pIport[0]->GetFramePixelNBit(); }

		// �X�e�[�^�X
		virtual DWORD		GetResendCount(DWORD set, DWORD ang) const { DWORD ch=GetCamIndex(set,ang); return mycls_pIport[ch]->GetResendCount();}	// �p�P�b�g�đ���
		virtual DWORD		GetLostCount  (DWORD set, DWORD ang) const { DWORD ch=GetCamIndex(set,ang); return mycls_pIport[ch]->GetLostCount();}	// �p�P�b�g������
		virtual DWORD		GetChDiff	  (DWORD set, DWORD ang) const { DWORD ch=GetCamIndex(set,ang); return  my_nChDiff[ch];}
		virtual DWORD		GetChDiffMax()		const { return my_nChDiffMax;}


	//// �ǉ��֌W
	public:
		inline IportBase&		GetBase(DWORD set, DWORD ang)		{ return (IportBase&)*mycls_pIport[GetCamIndex(set, ang)]; }
		inline IportBase const& GetBase(DWORD set, DWORD ang) const { return (IportBase&)*mycls_pIport[GetCamIndex(set, ang)];}



	//// �����o�[�֐�
	protected:

		// �X���b�h����
		virtual int			ThreadMainLoop();								// �X���b�h���C�����[�v���V���h�E
		virtual int			ThreadFirst();									// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
		virtual int			ThreadLast()				{ return 0; };		// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
		virtual void		ThreadEvent(int nEventNo)	{};					// �X���b�h�C�x���g����


		virtual void		OnFrameGrabbed(void** pImgs, long nX, long nY,  DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime);	// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O


	//// �����o�[�ϐ�
	protected:
		// �N���X
		IportBase*						mycls_pIport[MAX_IPORT_CAMNUM];		// iPort�N���X 

		// ��{���
		short							my_nChDiff[MAX_IPORT_CAMNUM];		// �����Ŏ����Ă���V�[�P���X�ԍ��ɑ΂���eiport�V�[�P���X�ԍ��Ƃ̍�
		short							my_nChDiffMax;
	};
};
