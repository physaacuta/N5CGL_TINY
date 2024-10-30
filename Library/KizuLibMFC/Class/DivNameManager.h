// *********************************************************************************
//	�e�� DIV �萔�̖��̂��擾����N���X
//	[Ver]
//		Ver.01    2007/08/13  vs2005 �Ή�
//
//	[����]
//		�قƂ�� �f�o�b�N�p
// *********************************************************************************
#pragma once

//#include <windows.h>
//#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

#include "..\..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[

namespace KizuLib
{
	class DivNameManager
	{
	public:
		static CString GetDivBool(EM_DIV_BOOL em) ;							// �g��BOOL
		static CString GetDivPlg(EM_DIV_PLG em) ;							// PLG�敪
		static CString GetDivUnten(EM_DIV_UNTEN em) ;						// �^�]���
		static CString GetDivSogyo(EM_DIV_SOGYO em) ;						// ���Ə��
		static CString GetDivKadou(EM_DIV_KADOU em) ;						// �ғ����
		static CString GetDivKensa(EM_DIV_KENSA em) ;						// �������
		static CString GetDivKiki(EM_DIV_KIKI em) ;							// �@����
		static CString GetDivCamSet(EM_DIV_CAMSET em) ;						// �J�����Z�b�g����

		static CString GetDivBool(int em)  {return GetDivBool((EM_DIV_BOOL)em); };			// �g��BOOL
		static CString GetDivPlg(int em)   {return GetDivPlg((EM_DIV_PLG)em); };			// PLG�敪
		static CString GetDivUnten(int em) {return GetDivUnten((EM_DIV_UNTEN)em); };		// �^�]���
		static CString GetDivSogyo(int em) {return GetDivSogyo((EM_DIV_SOGYO)em); };		// ���Ə��
		static CString GetDivKadou(int em) {return GetDivKadou((EM_DIV_KADOU)em); };		// �ғ����
		static CString GetDivKensa(int em) {return GetDivKensa((EM_DIV_KENSA)em); };		// �������
		static CString GetDivKiki(int em)  {return GetDivKiki((EM_DIV_KIKI)em); };			// �@����
		static CString GetDivCamSet(int em)  {return GetDivCamSet((EM_DIV_CAMSET)em); };	// �J�����Z�b�g����

		//static CString GetDivTorB(bool ou) ;								// �\/��
		static CString GetTorB(int ou) {return 0==ou?"�\��":"����";}		// �\/��

	};
};

