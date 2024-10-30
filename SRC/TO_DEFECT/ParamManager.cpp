#include "StdAfx.h"
#include "ParamManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInstance.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
ParamManager::ParamManager(void) :
ThreadManager("ParamMgr")
{
	m_evReadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ParamManager::~ParamManager(void)
{
	CloseHandle(m_evReadEnd);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ParamManager::ThreadFirst()
{

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { gque_Deli.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int ParamManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ParamManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_QUE = 0,						// �p�����[�^�ύX�ʒm
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 							// �p�����[�^�ύX�ʒm
		COMMON_QUE*	pDeli;
		pDeli = gque_Deli.GetFromHead();
		ExecParam(pDeli);
		delete pDeli;

		// �p�����[�^�Ǎ������ʒm
		SetEvent(m_evReadEnd);
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ǂݍ��ݐ���
void ParamManager::ExecParam(COMMON_QUE* pQue)
{
	int dly	  = pQue->fl.data[0];
	int id    = pQue->fl.data[1];			// �e�[�u��ID
	int scode = pQue->fl.data[2];			// ���
	int scode_men[NUM_MEN];					// �ʌŒ�
	for(int ii=0; ii<NUM_MEN; ii++) scode_men[ii] = pQue->fl.data[3+ii];

	//================================================
	//// ���^�X�N�փp�����[�^�ύX�ʒm
	// ����@�\ �֒ʒm
	switch( id ) {
		case CH_PARAM_ALL:					// �S��
		case CH_PARAM_TYPE_GRADE:			// �r��EGr
		case CH_PARAM_COMMON:				// �Œ�p�����[�^
		case CH_PARAM_CAMERA:				// �J�����ݒ�
		case CH_PARAM_EDGE:					// �G�b�W���o
		case CH_PARAM_SHD:					// �V�F�[�f�B���O�␳
		case CH_PARAM_THRESHOLD:			// 臒l�ݒ�
		case CH_PARAM_DETECT:				// �r���o
		case CH_PARAM_CYCLE:				// �����r����
		//case CH_PARAM_MERGE:				// �J�������E���� (�r����)
		//case CH_PARAM_LIGHT:				// �Ɩ�
		case CH_PARAM_TINY:					// ��������
		case CH_PARAM_SYNCADD:				// �摜���Z
		//case CH_PARAM_YOUSET:				// �n�ړ_���o
		case CH_PARAM_COLOR_DIV:			// ���F����
		case CH_PARAM_PCINFODESC:			// ��ʐݒ���
		case CH_PARAM_TAJUUTHRESHOLD:		// ���d����\臒l�ݒ�
		//case CH_PARAM_CYCLETHRESHOLD:		// �����p����\臒l�ݒ�
		case CH_PARAM_REDUCTION:			// �k�����ݒ�
		case CH_PARAM_FILTER:				// �t�B���^�ݒ�
		case CH_PARAM_PARA:					// ���W�b�N�p�������ݒ�
		case CH_PARAM_CROP_SHRINK:			// �Ԉ����ݒ�

			for(int ii=0; ii<NUM_MEN; ii++) {
				MainInstance::Send_HtDefect_PreLoadParam(ii, DIV_HANTEI_NON, scode_men[ii]);	// �p�����[�^���O�Ǎ��ʒm
				MainInstance::Send_HtDefect_Param(dly, ii, scode_men[ii] );
			}
			break;
	}

	// �J�����R���g���[���[�֑��M
	switch( id ) {
		case CH_PARAM_ALL:					// �S��
		case CH_PARAM_COMMON:				// �Œ�p�����[�^
		case CH_PARAM_CAMERA:				// �J�����ݒ�

			ToOutMailSender::Send_ToCamera_Param(scode);
			break;
	}

	// �J���������N�X�C�b�`�ʐM
	switch( id )
	{
		case CH_PARAM_ALL:						// �S��
		case CH_PARAM_COMMON:					// �Œ�p�����[�^

			for(int ii=0; ii<NUM_MEN; ii++)	ToOutMailSender::Send_ToLinksw_Param( scode_men[ii] );
			break;
	}

	// �Ɩ����u���� �֑��M
	switch( id ) {
		case CH_PARAM_ALL:					// �S��
		case CH_PARAM_LIGHT:				// �Ɩ�
#ifndef KT_LEDMGR
			ToOutMailSender::Send_ToLights_Param(scode);
#else
			ToOutMailSender::Send_KtLedMgr_Param(scode);
#endif
		break;
	}

	// �\���Ǘ� �֑��M
	switch( id ) {
		case CH_PARAM_ALL:					// �S��	
		case CH_PARAM_TYPE_GRADE:			// �r��EGr
		case CH_PARAM_TINY:					// �������אݒ�
		case CH_PARAM_SYNCADD:				// �摜���Z (�����p)
		case CH_PARAM_SETTIG_PCODE:			// �����p�^�[�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
		case CH_PARAM_SETTIG_SCODE:			// �����\�ʏ�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�  �T����ʂ�scode�̖��̂��o���Ă��邽��
		case CH_PARAM_SETTIG_COMMON:		// �萔�ݒ�
//		case CH_PARAM_SETTIG_PCOMMON:		// �����p�^�[�� �ŗL �o�͐ݒ� 5CGL�ɔ����ł͎g�p���Ȃ�
		case CH_PARAM_SETTIG_PALL:			// �����p�^�[�� ���� �o�͐ݒ�
		case CH_PARAM_MAE_MAST:				// �O�H���ݒ�
		case CH_PARAM_DEFLIST:				// �r�ꗗ
		case CH_PARAM_SETTIG_ALARM:			// �r���x��o�͐ݒ�

			MainInstance::Send_HyDefect_Param(id);
			break;
	}

	// �r�_�}�b�v��� �֑��M
	switch (id) {
	case CH_PARAM_ALL:						// �S��	
	case CH_PARAM_TYPE_GRADE:				// �r��EGr
	case CH_PARAM_SETTIG_PCODE:				// �����p�^�[�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
		MainInstance::Send_SoDefMap_Param(id);
		break;
	}

	//// ���̓f�[�^�ҏW�Ǘ� �֑��M
	//switch( id ){
	//	case CH_PARAM_ALL:					// �S��
	//	case CH_PARAM_SETTIG_SCODE:			// �����\�ʏ�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
	//	case CH_PARAM_SETTIG_PCODE:			// �����p�^�[�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
	//	case CH_PARAM_SETTIG_COMMON:		// �Œ�p�����[�^

	//		MainInstance::Send_ToGetdat_Param(id);

	//}

	//// ���ѕҏW�Ǘ� �֑��M
	//switch( id ){
	//	case CH_PARAM_ALL:					// �S��
	//	case CH_PARAM_TYPE_GRADE:			// �r��EGr
	//	case CH_PARAM_SETTIG_PCODE:			// �����p�^�[�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
	//	case CH_PARAM_SETTIG_COMMON:		// �Œ�p�����[�^

	//		MainInstance::Send_ToPutdat_Param(id);

	//}

	//// �O�H�����ѕҏW�Ǘ� �֑��M
	//switch( id ){
	//	case CH_PARAM_ALL:					// �S��
	//	case CH_PARAM_MAE_MAST:				// �O�H���ݒ�

	//		MainInstance::Send_ToMaedat_Param(id);

	//}


	//================================================
	//// �����̃p�����[�^�ύX
	switch( id ) {
		case CH_PARAM_ALL:					// �S��
			Read_CommonMast();
			Read_TypeName();
			Read_GradeName();
			Read_ScodeOutput();							// �r��Gr�̐ݒ�ɂ��A�ύX�����\���L��
			Read_PcodeOutput();
			Read_PallOutput(); 
			//Read_PCommonOutput();			//5CGL�ɔ����ł͎g�p���Ȃ�

			Read_Param_Common();
			Read_Param_CamSet();
			Read_Param_Scode();
			Read_Param_Camset_Scode();
			Read_Param_Camset_Angle();
			Read_Param_Camset_Angle_Scode();
			break;

		case CH_PARAM_TYPE_GRADE:			// �r��EGr
			Read_TypeName();
			Read_GradeName();
			Read_ScodeOutput();							// �r��Gr�̐ݒ�ɂ��A�ύX�����\���L��
			Read_PcodeOutput();
			Read_PallOutput(); 
			//Read_PCommonOutput();			//5CGL�ɔ����ł͎g�p���Ȃ�
			break;

	//CH_PARAM_ATTR_NAME
	//CH_PARAM_PCINFODESC

		case CH_PARAM_COMMON:				// �Œ�p�����[�^
			Read_Param_Common();
			Read_Param_CamSet();
			Read_Param_Scode();
			break;

		case CH_PARAM_CAMERA:				// �J�����ݒ�
			Read_Param_Scode();
			Read_Param_Camset_Scode();
			Read_Param_Camset_Angle();
			Read_Param_Camset_Angle_Scode();
			break;	

		case CH_PARAM_EDGE:					// �G�b�W���o
			Read_Param_Scode();
			Read_Param_Camset_Scode();
			Read_Param_Camset_Angle_Scode();
			break;	

	//CH_PARAM_SHD
	//CH_PARAM_THRESHOLD
	//CH_PARAM_DETECT
	//CH_PARAM_CYCLE

		case CH_PARAM_MERGE:				// �r����
			Read_Param_Common();
			break;

	//CH_PARAM_LIGHT

 		case CH_PARAM_TINY:					// �������אݒ�
			Read_CommonMast();
			Read_Param_Scode();
			break;

 		case CH_PARAM_SYNCADD:		        // �摜���Z
			break;

	//CH_PARAM_YOUSET
	//CH_PARAM_COLOR_DIV


		case CH_PARAM_SETTIG_SCODE:			// �����\�ʏ�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
			Read_ScodeOutput();
			break;

		case CH_PARAM_SETTIG_PCODE:			// �����p�^�[�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
			Read_PcodeOutput();
			break;

		case CH_PARAM_SETTIG_COMMON:		// �萔�ݒ�
			Read_CommonMast();
			//// ����PC����JPEG���k������
			MainInstance::Send_HtDefect_JpegQuality(GetCommonMast().nJpegQuality);
			break;

		//5CGL�ɔ����ł͎g�p���Ȃ�
		//case CH_PARAM_SETTIG_PCOMMON:		// �����p�^�[�� �ŗL �o�͐ݒ�
		//	Read_PCommonOutput();
		//	break;

		case CH_PARAM_SETTIG_PALL:			// �����p�^�[�� ���� �o�͐ݒ�
			Read_PallOutput(); 
			break;

	//CH_PARAM_SETTIG_LOG
	//CH_PARAM_SETTIG_STATUS

		// ���C���Ǝ� ----------------------------- 
		//case CH_PARAM_SETTIG_GAP:			// ����Y���␳�����ݒ�
		//	Read_CommonMast();
		//	Read_Param_CamSet();
		//	break;

		// �O�H���ݒ�
		case CH_PARAM_MAE_MAST:
			break;

		// ���d����\臒l�ݒ�
		case CH_PARAM_TAJUUTHRESHOLD:
			break;

		//// �����p����\臒l�ݒ�
		//case CH_PARAM_CYCLETHRESHOLD:
		//	break;

		// �k�����ݒ�
		case CH_PARAM_REDUCTION:
			break;

		// �t�B���^�ݒ�
		case CH_PARAM_FILTER:
			break;

		// �r�ꗗ�ݒ�
		case CH_PARAM_DEFLIST:
			Read_CommonMast();
			Read_PallOutput(); 
			break;

		case CH_PARAM_SETTIG_ALARM:
			Read_CommonMast();
			break;
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �o�͏����`�F�b�N

//==========================================
// �r��EdasysID �̗L��
//------------------------------------------
// long		tid		: �r��EdasysID	
// �߂�l			: ���A���
//==========================================
bool ParamManager::CheckEdasysID_T(long tid) const 
{
	if(0==tid) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.Type_Id[ii].nEdasysID == tid ) return true;
	}
	return false;
}

//==========================================
// �O���[�hEdasysID �̗L��
//------------------------------------------
// long		tid		: �O���[�hEdasysID
// �߂�l			: ���A���
//==========================================
bool ParamManager::CheckEdasysID_G(long gid) const 
{
	if(0==gid) return false;
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.Grade_Id[ii].nEdasysID == gid ) return true;
	}
	return false;
}

//==========================================
// �r��D��x�擾
//------------------------------------------
// long		tid		: �r��EdasysID
// �߂�l			: �D��x (�����o-1)
//==========================================
int ParamManager::GetYusenT(long tid) const 
{
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.Type_Id[ii].nEdasysID == tid ) return m_TypeName.Type_Id[ii].nPriority;
	}
	return -1;
}

//==========================================
// �O���[�h�D��x�擾
//------------------------------------------
// long		gid		: �O���[�hEdasysID
// �߂�l			: �D��x (�����o-1)
//==========================================
int ParamManager::GetYusenG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.Grade_Id[ii].nEdasysID == gid ) return m_GradeName.Grade_Id[ii].nPriority;
	}
	return -1;
}

//==========================================
// ���׃O���[�h�擾
//------------------------------------------
// long		gid		: �O���[�hEdasysID
// �߂�l			: ���׃O���[�h (�����o-1)
//==========================================
int ParamManager::GetRankG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.Grade_Id[ii].nEdasysID == gid ) return m_GradeName.Grade_Id[ii].rank;
	}
	return -1;
}

//==========================================
// �o�͑Ώۂ̌��ׂ�����
//------------------------------------------
// EM_OUTPUTFLG	output	: �ǂ̏o�͏������`�F�b�N����́H
// long			pcode	: �����p�^�[�� (0�I���W��)
// long			tid		: �r��EdasysID
// long			gid		: �O���[�hEdasysID
// �߂�l				: true:�Ώ�
//==========================================
bool ParamManager::Check_Output(EM_OUTPUTFLG output, long pcode, long tid, long gid) const
{
	// �����`�F�b�N
	if( 0 > pcode || MAX_PCODE <= pcode ) {
		LOG(em_ERR), "[%s] output=%d pcodeNG=%d", my_sThreadName, output, pcode);	
		return false;
	}

	//// �r��EdasysID �w��L��H
	for(int ii=0; ii<m_PcodeOutput.rec[pcode].nPutNum; ii++) {
		if( tid == m_PcodeOutput.rec[pcode].output[ii].edasysID ) {
			// Gr�ݒ薳�����r��̈� �S���ΏۊO
			if( 0 == m_PcodeOutput.rec[pcode].output[ii].kind[output] ){
				return false;
			}

			// Gr�ݒ�L����r��̈� �D��x��r
			if( GetYusenG(gid) <= GetYusenG(m_PcodeOutput.rec[pcode].output[ii].kind[output]) )	return true;
			else																				return false;
		}
	}

	//// �����Ƃ��� �f�t�H���g�w�� (�R�R�܂ŗ����ꍇ�́A�r��ݒ薳���̎�)
	// Gr�ݒ薳�����r��̈� �S���ΏۊO
	if( 0 == m_PcodeOutput.rec[pcode].defput.kind[output] ) return false;

	// Gr�ݒ�L����r��̈� �D��x��r
	if( GetYusenG(gid) <= GetYusenG(m_PcodeOutput.rec[pcode].defput.kind[output]) )	return true;
	else																			return false;
}

//==========================================
// �o�͑Ώۂ̌��ׂ�����
//------------------------------------------
// EM_OUTPUTFLG	output	: �ǂ̏o�͏������`�F�b�N����́H
// long			pcode	: �����p�^�[�� (0�I���W��)
// long			tid		: �r��EdasysID
// long			gid		: �O���[�hEdasysID
// �߂�l				: -9:�ΏۊO�w��  -1:��菬����  0:����  1:���傫��
//==========================================
int ParamManager::Check_OutputEx(EM_OUTPUTFLG output, long pcode, long tid, long gid) const
{
	int nVal, nPrm;

	// �����`�F�b�N
	if( 0 > pcode || MAX_PCODE <= pcode ) {
		LOG(em_ERR), "[%s] output=%d pcodeNG=%d", my_sThreadName, output, pcode);	
		return -9;
	}

	//// �r��EdasysID �w��L��H
	for(int ii=0; ii<m_PcodeOutput.rec[pcode].nPutNum; ii++) {
		if( tid == m_PcodeOutput.rec[pcode].output[ii].edasysID ) {
			// Gr�ݒ薳�����r��̈� �S���ΏۊO
			if( 0 == m_PcodeOutput.rec[pcode].output[ii].kind[output] ) return -9;

			// Gr�ݒ�L����r��̈� �D��x��r
			nVal = GetYusenG(gid);
			nPrm = GetYusenG(m_PcodeOutput.rec[pcode].output[ii].kind[output]);
			if( nVal == nPrm )		{ return 0; }
			else if( nVal < nPrm )	{ return 1; }
			else					{ return -1;}
		}
	}

	//// �����Ƃ��� �f�t�H���g�w�� (�R�R�܂ŗ����ꍇ�́A�r��ݒ薳���̎�)
	// Gr�ݒ薳�����r��̈� �S���ΏۊO
	if( 0 == m_PcodeOutput.rec[pcode].defput.kind[output] ) return -9;

	// Gr�ݒ�L����r��̈� �D��x��r
	nVal = GetYusenG(gid);
	nPrm = GetYusenG(m_PcodeOutput.rec[pcode].defput.kind[output]);
	if( nVal == nPrm )		{ return 0; }
	else if( nVal < nPrm )	{ return 1; }
	else					{ return -1;}
}

//==========================================
// �����p�^�[�����ʏo�͑Ώۂ̌��ׂ�����
//------------------------------------------
// EM_OUTPUT_PALLFLG	output		: �ǂ̏o�͏������`�F�b�N����́H
// long					tid			: �r��EdasysID
// long					gid			: �O���[�hEdasysID
// bool					bUp = true	: (true:�w������ȏ�őΏ�  false:�w������ȉ��őΏ�)
// �߂�l							: true:�Ώ�
//==========================================
bool ParamManager::Check_Pall_Output(EM_OUTPUT_PALLFLG output, long tid, long gid, bool bUp) const
{
	//// �r��EdasysID �w��L��H
	for(int ii=0; ii<m_PallOutput.nPutNum; ii++) {
		if( tid == m_PallOutput.output[ii].edasysID ) {
			// Gr�ݒ薳�����r��̈� �S���ΏۊO
			if( 0 == m_PallOutput.output[ii].kind[output] ) return false;

			// Gr�ݒ�L����r��̈� �D��x��r
			if(bUp) {
				if( GetYusenG(gid) <= GetYusenG(m_PallOutput.output[ii].kind[output]) )	return true;
				else																	return false;
			} else {
				if( GetYusenG(gid) >= GetYusenG(m_PallOutput.output[ii].kind[output]) )	return true;
				else																	return false;
			}
		}
	}

	//// �����Ƃ��� �f�t�H���g�w�� (�R�R�܂ŗ����ꍇ�́A�r��ݒ薳���̎�)
	// Gr�ݒ薳�����r��̈� �S���ΏۊO
	if( 0 == m_PallOutput.defput.kind[output] ) return false;

	// Gr�ݒ�L����r��̈� �D��x��r
	if(bUp) {
		if( GetYusenG(gid) <= GetYusenG(m_PallOutput.defput.kind[output]) )	return true;
		else																return false;
	} else {
		if( GetYusenG(gid) >= GetYusenG(m_PallOutput.defput.kind[output]) )	return true;
		else																return false;
	}
}

//------------------------------------------
// �r��ɑ΂����r�摜�̕\���Ώۂ��`�F�b�N
// long tid		�r��EdasysID
// int index	�摜��� (0�I���W��)
// �߂�l true:�Ώ�
//------------------------------------------
bool ParamManager::Check_ImgDsp(long tid, int index) const
{

	// ����
	//return true;

	// �F��
	if(0==tid) return false;
	if(index>=MAX_IMAGE_CNT) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.Type_Id[ii].nEdasysID == tid ) {
			return m_TypeName.Type_Id[ii].bEnable[ index ];
		}
	}

	return false;
}

//------------------------------------------
// �v�Z�l�c����\ (���S�Œ蕪��\�Ƃ���������)
// int idx	�`�����l�� (1:ײ�PLG  2:�\�Z���p�p���X 3:���Z���p�p���X 4:�e�X�g�p���X)  
// �߂�l				: �c����\[mm/pixel]
//------------------------------------------
double ParamManager::CalcResV(int idx)
{
	double dResV = 1.0;		// �c����\
	dResV = m_Param_Common.dPls * m_Param_Common.typCamCon[idx-1].nDiv / m_Param_Common.typCamCon[idx-1].nMul;
	return dResV;
}

//------------------------------------------
// �v�Z�l�P�t���[���J�E���^�l (���S�Œ蕪��\�Ƃ���������)
// int idx	�`�����l�� (1:ײ�PLG  2:�\�Z���p�p���X 3:���Z���p�p���X 4:�e�X�g�p���X)  
// �߂�l				: �J�E���^�l(1�t���[��) [pls]
//------------------------------------------
double ParamManager::CalcSampBuff(int idx)
{
	double dSampBuff = 0.0;		// �J�E���^�l(1�t���[��)
	dSampBuff = (int)(SIZE_FRAME_IMG_Y * m_Param_Common.typCamCon[idx-1].nDiv / m_Param_Common.typCamCon[idx-1].nMul);
	return dSampBuff;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ����ŗL�̌��J�֐�
//------------------------------------------
// �{�C�X�o�͏����̗D��x����DB���ڂ��擾
// ENUM_VOICE_DEF_TYPE emViceDefType	:  
// �߂�l				: �J�E���^�l(1�t���[��) [pls]
//------------------------------------------
ParamManager::EM_OUTPUTFLG ParamManager::GetOutputFlg(ENUM_VOICE_PRI_TYPE emViceDefType)
{
	switch (emViceDefType) {
	case em_VOICE_PRI_OT:	return OUTPUTFLG_VOICE_OTHER;		// �r�ڋ�
	case em_VOICE_PRI_BI:	return OUTPUTFLG_VOICE_CYCLE;		// �����r �P��
	case em_VOICE_PRI_SY:	return OUTPUTFLG_VOICE_CYCLE;		// �����r
	case em_VOICE_PRI_TB:	return OUTPUTFLG_VOICE_SAME;		// �\�������r
	case em_VOICE_PRI_SU:	return OUTPUTFLG_VOICE_SURI;		// �X���o
	case em_VOICE_PRI_HE:	return OUTPUTFLG_VOICE_HEGE;		// �w�Q
	case em_VOICE_PRI_JU:	return OUTPUTFLG_VOICE_HIGH;		// �d���r
	case em_VOICE_PRI_RE:	return OUTPUTFLG_VOICE_SEQ;			// �A���r
	default:				return OUTPUTFLG_END;	 			// ����
	}

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p�����[�^�Ǎ�

//------------------------------------------
// �S�p�����[�^�ǂݍ���
//------------------------------------------
bool ParamManager::ReadParam_All()
{
	if( ! Read_CommonMast() )	return false;
	if( ! Read_TypeName() )		return false;
	if( ! Read_GradeName() )	return false;
	if( ! Read_ScodeOutput() )	return false;
	if( ! Read_PcodeOutput() )	return false;
	if( ! Read_PallOutput() )	return false;
//	if( ! Read_PCommonOutput())	return false;	//5CGL�ɔ����ł͎g�p���Ȃ�

	if( ! Read_Param_Common() )	return false;
	if( ! Read_Param_CamSet() )	return false;
	if( ! Read_Param_Scode() )	return false;
	if( ! Read_Param_Camset_Scode() )		return false;
	if( ! Read_Param_Camset_Angle() )		return false;
	if( ! Read_Param_Camset_Angle_Scode() )	return false;
	return true;
}
//------------------------------------------
// �Ӄ֒萔�p�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_CommonMast()
{
	CString sql;					// SQL��

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_COMMON_MASTER;
	P_COMMON_MAST&	p = m_CommonMast;
	P_COMMON_MAST	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[[%s]", my_sThreadName, clsDB.GetLastErrMsg());
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql  = "SELECT ";
	sql += " [RAS����F] ";
	sql += ",[RAS�ُ�F] ";
	sql += ",[RAS���ӐF] ";
	sql += ",[LOG�\������] ";
	sql += ",[LOG�C�x���g�F] ";
	sql += ",[LOG�d�̏�F] ";
	sql += ",[LOG�y�̏�F] ";

	sql += ",[�T����ʕ\����ԏ�] ";
	sql += ",[�T����ʕ\����ԑ�] ";
	sql += ",[�E�H�[�}�ގw��ʒu] ";
	sql += ",[Jpeg�؂�o���摜�i��] ";
	sql += ",[�J�������x臒l] ";

	sql += ",[�G�b�W������_���{���] ";
	sql += ",[�G�b�W������_���o�͈�] ";
	sql += ",[�G�b�W������_���J�o���[���o�͈�] ";
	sql += ",[�G�b�W������_�����e�덷] ";
	sql += ",[�G�b�W������_�O�t���[���Ƃ̋��e�덷] ";
	sql += ",[�G�b�W������_�n�ڌ�s����] ";
	sql += ",[�G�b�W������_�G�b�W���o�ُ�FF���M] ";
	sql += ",[�G�b�W���o_���{���] ";
	sql += ",[�G�b�W���o_���o�͈�] ";

	sql += ",[�d���r�}�b�v�w�i�F_�y] ";
	sql += ",[�d���r�}�b�v�w�i�F_��] ";
	sql += ",[�d���r�}�b�v�w�i�F_�d] ";
	sql += ",[�d���r�}�b�v�r��臒l_�y] ";
	sql += ",[�d���r�}�b�v�r��臒l_��] ";
	sql += ",[�d���r�}�b�v�r��臒l_�d] ";

	sql += ",[�ݔ���_������__�㗬�J����] ";
	sql += ",[�ݔ���_������__SPM] ";
	sql += ",[�ݔ���_�\����__��������] ";
	sql += ",[�ݔ���_�\����__�\������] ";
	sql += ",[�ݔ���_�\����__�V���[] ";
	sql += ",[�ݔ���_������_�Q�[�g�M��ON_������_] ";
	sql += ",[�ݔ���_�\����_�Q�[�g�M��ON_�\����_] ";

	sql += ",[�r�ڋ߃����v_�s����_��[] ";
	sql += ",[�r�ڋ߃����v_�s����_���[] ";
	sql += ",[�r�ڋ߃����v_�o�͋��_�O] ";
	sql += ",[�r�ڋ߃����v_�o�͋��_��] ";
	sql += ",[�r�ڋ߃A���[��_�s����_��[] ";
	sql += ",[�r�ڋ߃A���[��_�s����_���[] ";
	sql += ",[�r�ڋ߃A���[��_�o�͋��_�O] ";
	sql += ",[�r�ڋ߃A���[��_�o�͋��_��] ";
	sql += ",[�{�C�X_�o�͈ʒu_�O] ";

	sql += ",[�S���摜_�ۑ�_R] ";
	sql += ",[�S���摜_�ۑ�_G] ";
	sql += ",[�S���摜_�ۑ�_B] ";
	sql += ",[�S���摜_�ۑ�_��] ";

	sql += ",[PLG������r�ُ����l] ";
	sql += ",[PLG�����\���ُ����l] ";
	sql += ",[�v���R���R�C������r�ُ����l] ";

	sql += ",[���W�b�N�p�������@�\01_���̓J����01] ";
	sql += ",[���W�b�N�p�������@�\01_���̓J����02] ";
	sql += ",[���W�b�N�p�������@�\02_���̓J����01] ";
	sql += ",[���W�b�N�p�������@�\02_���̓J����02] ";

	sql += "FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	int nRecord = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
	
			// �f�[�^��
			clsDB.GetData(1, &buf.Ras.nColorNorm);				// RAS����F
			clsDB.GetData(2, &buf.Ras.nColorErr);				// RAS�ُ�F
			clsDB.GetData(3, &buf.Ras.nColorWarn);				// RAS���ӐF
			clsDB.GetData(4, &buf.Log.nDispNum);				// LOG�\������
			clsDB.GetData(5, &buf.Log.nEventColor);				// LOG�C�x���g�F
			clsDB.GetData(6, &buf.Log.nFailColor_Ju);			// LOG�d�̏�F
			clsDB.GetData(7, &buf.Log.nFailColor_Kei);			// LOG�y�̏�F

			clsDB.GetData(8, &buf.Tansho.nDspSectionMin);		// �T����ʕ\����ԏ�
			clsDB.GetData(9, &buf.Tansho.nDspSectionMax);		// �T����ʕ\����ԑ�
			clsDB.GetData(10, &buf.walm);						// �E�H�[�}�ގw��ʒu
			clsDB.GetData(11, &buf.nJpegQuality);				// Jpeg�؂�o���摜�i��
			clsDB.GetData(12, &buf.nCamTempThr);				// �J�������x臒l

			clsDB.GetData(13, &buf.Edge.nType);					// �G�b�W������_���{���
			clsDB.GetData(14, &buf.Edge.dDetectRange);			// �G�b�W������_���o�͈�
			clsDB.GetData(15, &buf.Edge.dDetectRangeR);			// �G�b�W������_���J�o���[���o�͈�
			clsDB.GetData(16, &buf.Edge.dWidthDiff);			// �G�b�W������_�����e�덷
			clsDB.GetData(17, &buf.Edge.dPerEdgeDiff);			// �G�b�W������_�O�t���[���Ƃ̋��e�덷
			clsDB.GetData(18, &buf.Edge.nFromSkip);				// �G�b�W������_�n�ڌ�s����
			clsDB.GetData(19, &buf.Edge.nFFErr);				// �G�b�W������_�G�b�W���o�ُ�FF���M
			clsDB.GetData(20, &buf.Edge_Restrict.nType);		// �G�b�W���o_���{���
			clsDB.GetData(21, &buf.Edge_Restrict.dDetectRange);	// �G�b�W���o_���o�͈�
			
			clsDB.GetData(22, &buf.DefMap.nBGColor_Kei);		// �d���r�}�b�v�w�i�F_�y
			clsDB.GetData(23, &buf.DefMap.nBGColor_Tyu);		// �d���r�}�b�v�w�i�F_��
			clsDB.GetData(24, &buf.DefMap.nBGColor_Ju);			// �d���r�}�b�v�w�i�F_�d
			clsDB.GetData(25, &buf.DefMap.nDefCntThr_Kei);		// �d���r�}�b�v�r��臒l_�y
			clsDB.GetData(26, &buf.DefMap.nDefCntThr_Tyu);		// �d���r�}�b�v�r��臒l_��
			clsDB.GetData(27, &buf.DefMap.nDefCntThr_Ju);		// �d���r�}�b�v�r��臒l_�d

			clsDB.GetData(28, &buf.Dist.nKenBase);				// �ݔ���_������__�㗬�J����
			clsDB.GetData(29, &buf.Dist.nKenBaseSpm);			// �ݔ���_������__SPM
			clsDB.GetData(30, &buf.Dist.nDspBaseKensaBot);		// �ݔ���_�\����__��������
			clsDB.GetData(31, &buf.Dist.nDspBaseKensaTop);		// �ݔ���_�\����__�\������
			clsDB.GetData(32, &buf.Dist.nDspSharCut);			// �ݔ���_�\����__�V���[
			clsDB.GetData(33, &buf.Dist.nKenBaseGate);			// �ݔ���_������_�Q�[�g�M��ON_������_
			clsDB.GetData(34, &buf.Dist.nDspBaseGate);			// �ݔ���_�\����_�Q�[�g�M��ON_�\����_

			clsDB.GetData(35, &buf.nNoLamp[0]);					// �r�ڋ߃����v_�s����_��[
			buf.nNoLamp[0] *= 1000;
			clsDB.GetData(36, &buf.nNoLamp[1]);					// �r�ڋ߃����v_�s����_���[
			buf.nNoLamp[1] *= 1000;
			clsDB.GetData(37, &buf.nKizuLamp[0]);				// �r�ڋ߃����v_�o�͋��_�O
			buf.nKizuLamp[0] *= 1000;
			clsDB.GetData(38, &buf.nKizuLamp[1]);				// �r�ڋ߃����v_�o�͋��_��
			buf.nKizuLamp[1] *= 1000;
			clsDB.GetData(39, &buf.nNoAlarm[0]);				// �r�ڋ߃A���[���s����_��[
			buf.nNoAlarm[0] *= 1000;
			clsDB.GetData(40, &buf.nNoAlarm[1]);				// �r�ڋ߃A���[��_�s����_���[
			buf.nNoAlarm[1] *= 1000;
			clsDB.GetData(41, &buf.nKizuAlarm[0]);				// �r�ڋ߃A���[��_�o�͋��_�O
			buf.nKizuAlarm[0] *= 1000;
			clsDB.GetData(42, &buf.nKizuAlarm[1]);				// �r�ڋ߃A���[��_�o�͋��_��
			buf.nKizuAlarm[1] *= 1000;
			clsDB.GetData(43, &buf.nVoicePos);					// �{�C�X_�o�͈ʒu_�O
			buf.nVoicePos *= 1000;

			clsDB.GetData(44, &buf.nRPISaveColor_R);			// �S���摜_�ۑ�_R
			clsDB.GetData(45, &buf.nRPISaveColor_G);			// �S���摜_�ۑ�_G
			clsDB.GetData(46, &buf.nRPISaveColor_B);			// �S���摜_�ۑ�_B
			clsDB.GetData(47, &buf.nRPISaveColor_Sa);			// �S���摜_�ۑ�_��

			clsDB.GetData(48, &buf.dPlgDiffLimit[0]);			// PLG������r�ُ����l
			clsDB.GetData(49, &buf.dPlgDiffLimit[1]);			// PLG�����\���ُ����l
			clsDB.GetData(50, &buf.dProLenDiffLimit);			// �v���R���R�C������r�ُ����l

			clsDB.GetData(51, &buf.nLogiParaCam[0][0]);			// ���W�b�N�p�������@�\01_���̓J����01
			clsDB.GetData(52, &buf.nLogiParaCam[0][1]);			// ���W�b�N�p�������@�\01_���̓J����02
			clsDB.GetData(53, &buf.nLogiParaCam[1][0]);			// ���W�b�N�p�������@�\02_���̓J����01
			clsDB.GetData(54, &buf.nLogiParaCam[1][1]);			// ���W�b�N�p�������@�\02_���̓J����02

			nRecord++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// ����Ă������R�[�h���͐���H�H
	if( 1 != nRecord ){
		LOG(em_ERR), "[%s] %s ���R�[�h���ُ�[%d]", my_sThreadName, cDbName, nRecord);
		return false;
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}


//------------------------------------------
// �r��p�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_TypeName()
{
	CString sql;					// SQL��
	int id;
	int imgidx;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_TYPE_NAME;
	P_TYPE_NAME&	p = m_TypeName;
	P_TYPE_NAME		buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[[%s]", my_sThreadName, clsDB.GetLastErrMsg());
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql  = "SELECT ";
	sql += "ID, EdasysID, �D��x ";
	sql += ", ISNULL(�؏o�摜���1,1), ISNULL(�؏o�摜���2,2)";
	sql += "FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
	
			// ��L�[
			clsDB.GetData(1, &id);
			if( MAX_TYPE < id || 0 > id ){
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);
				continue;
			}

			// �f�[�^��
			clsDB.GetData(2, &buf.Type_Id[buf.num].nEdasysID );
			clsDB.GetData(3, &buf.Type_Id[buf.num].nPriority );

			clsDB.GetData(4, &imgidx);
			if (0<imgidx || imgidx <= MAX_IMAGE_CNT) buf.Type_Id[buf.num].bEnable[imgidx - 1] = true;

			clsDB.GetData(5, &imgidx);
			if (0<imgidx || imgidx <= MAX_IMAGE_CNT) buf.Type_Id[buf.num].bEnable[imgidx - 1] = true;

			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}
//------------------------------------------
// �O���[�h�p�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_GradeName()
{
	CString sql;					// SQL��
	int id;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_GRADE_NAME;
	P_GRADE_NAME&	p = m_GradeName;
	P_GRADE_NAME	buf;
	memset(&buf, 0x00, sizeof(buf));

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " ID, EdasysID, �D��x, ���׃O���[�h";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY �D��x ASC ";

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			if( MAX_GRADE <= id || 0 > id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			
			// -----------------
			// �f�[�^��
			clsDB.GetData(2, &buf.Grade_Id[buf.num].nEdasysID );
			clsDB.GetData(3, &buf.Grade_Id[buf.num].nPriority );
			clsDB.GetData(4, &buf.Grade_Id[buf.num].rank );

			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}
//------------------------------------------
// �����\�ʏ�ԃp�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_ScodeOutput()
{
	CString sql;					// SQL��
	int id;


	//// �p�����[�^������
	CString cDbName;
	P_SCODE_OUTPUT&	p = m_ScodeOutput;
	P_SCODE_OUTPUT buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////////////////////////////////////
	// �}�X�^�[
	////////////////////////////////////
	cDbName = DB_SCODE_MASTER;
	//// SQL���쐬
	sql =  "SELECT";
	sql += " scode, �L������";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			if( MAX_SCODE < id || 0 >= id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// �g���₷���悤��0�I���W���ɕύX


			// �f�[�^��
			clsDB.GetData(2, &buf.rec[id].umu );

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);

	return true;
}
//------------------------------------------
// �����p�^�[���ʃp�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_PcodeOutput()
{
	CString sql;					// SQL��
	int id;
	int no;

	//// �p�����[�^������
	CString cDbName;
	P_PCODE_OUTPUT&	p = m_PcodeOutput;
	//P_PCODE_OUTPUT		buf;
	// MAX_TYPE=64��256�ɂ������߁AP_PCODE_OUTPUT�̃T�C�Y���傫���Ȃ��ăX�^�b�N�I�[�o�[�t���[��������������static��
	static P_PCODE_OUTPUT		buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////////////////////////////////////
	// �}�X�^�[
	////////////////////////////////////
	cDbName = DB_PCODE_MASTER;
	//// SQL���쐬
	sql =  "SELECT";
	sql += " pcode, �L������";

	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			if( MAX_PCODE < id || 0 >= id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// �g���₷���悤��0�I���W���ɕύX


			// -----------------
			// �f�[�^��
			clsDB.GetData(2, &buf.rec[id].umu );

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);

	////////////////////////////////////
	// �o�͏���
	////////////////////////////////////
	int typid;	
	no = 0;
	cDbName = DB_PCODE_OUTPUT;
	//// SQL���쐬
	sql  = "SELECT ";
	sql += " [pcode] ";
	sql += ",[�r��EdasysID] ";
	sql += ",[�T����ʕ\��_�\] ";
	sql += ",[�T����ʕ\��_��] ";
	sql += ",[�����P���r�_�}�b�v��ʕ\��] ";
	sql += ",[FF���M�L��] ";
	sql += ",[�r�ڋߌy] ";
	sql += ",[�r�ڋߏd] ";
	sql += ",[�p�g���C�g�o��] ";
	sql += ",[�{�C�X�A���r] ";
	sql += ",[�{�C�X�d���r] ";
	sql += ",[�{�C�X�w�Q] ";
	sql += ",[�{�C�X�X���o] ";
	sql += ",[�{�C�X�\�������r] ";
	sql += ",[�{�C�X�����r] ";
	sql += ",[�{�C�X���̑��r] ";
	sql += ",[DB�ۑ�_���_�\] ";
	sql += ",[DB�ۑ�_���_��] ";
	sql += ",[DB�ۑ�_�D��_�\] ";
	sql += ",[DB�ۑ�_�D��_��] ";

	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			if( MAX_PCODE < id || 0 >= id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// �g���₷���悤��0�I���W���ɕύX


			// -----------------
			clsDB.GetData(2, &typid);
			if(-1 == typid) {								// �f�t�H���g
				buf.rec[id].defput.edasysID = typid;
				clsDB.GetData(3, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_U]);
				clsDB.GetData(5, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_DEFMAP]);
				clsDB.GetData(6, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_FF_SEND]);
				clsDB.GetData(7, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_ALARM_RANP_LOW]);
				clsDB.GetData(8, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_ALARM_RANP_HIGH]);
				clsDB.GetData(9, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_PATOLOGHT]);
				clsDB.GetData(10, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_SEQ]);
				clsDB.GetData(11, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_HIGH]);
				clsDB.GetData(12, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_HEGE]);
				clsDB.GetData(13, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_SURI]);
				clsDB.GetData(14, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_SAME]);
				clsDB.GetData(15, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_CYCLE]);
				clsDB.GetData(16, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_OTHER]);
				clsDB.GetData(17, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_LOWGR_DB_O]);
				clsDB.GetData(18, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_LOWGR_DB_U]);
				clsDB.GetData(19, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DB_O]);
				clsDB.GetData(20, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DB_U]);

			} else {										// �ʏ�
				no = buf.rec[id].nPutNum;
				buf.rec[id].output[no].edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_U]);
				clsDB.GetData(5, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_DEFMAP]);
				clsDB.GetData(6, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_FF_SEND]);
				clsDB.GetData(7, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_ALARM_RANP_LOW]);
				clsDB.GetData(8, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_ALARM_RANP_HIGH]);
				clsDB.GetData(9, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_PATOLOGHT]);
				clsDB.GetData(10, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_SEQ]);
				clsDB.GetData(11, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_HIGH]);
				clsDB.GetData(12, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_HEGE]);
				clsDB.GetData(13, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_SURI]);
				clsDB.GetData(14, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_SAME]);
				clsDB.GetData(15, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_CYCLE]);
				clsDB.GetData(16, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_OTHER]);
				clsDB.GetData(17, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_LOWGR_DB_O]);
				clsDB.GetData(18, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_LOWGR_DB_U]);
				clsDB.GetData(19, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DB_O]);
				clsDB.GetData(20, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DB_U]);
				
				buf.rec[id].nPutNum = no + 1;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);

	return true;
}
//------------------------------------------
// �����p�^�[�����ʏo�͐ݒ�p�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_PallOutput()
{
//#ifdef DISABLE_GAP_CALC
//	return true;
//#endif

	CString sql;					// SQL��

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_PALL_OUTPUT;
	P_PALL_OUTPUT&	p = m_PallOutput;
	P_PALL_OUTPUT	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	////////////////////////////////////
	// �o�͏���
	////////////////////////////////////
	int no;
	int typid;	
	no = 0;

	//// SQL���쐬
	sql =  "SELECT";
	sql += " �r��EdasysID"; 
	sql += ", �d���׈ꗗ��ʕ\��_�ڍ׏���, �d���׈ꗗ��ʕ\��_�ȈՏ���, �d���׈ꗗ��ʕ\��_�ȈՍ��� ";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &typid);
			if(-1 == typid) {								// �f�t�H���g
				buf.defput.edasysID = typid;

				clsDB.GetData(2, (int*)&buf.defput.kind[OUTPUTPALLFLG_DEFLIST_DETAIL]);
				clsDB.GetData(3, (int*)&buf.defput.kind[OUTPUTPALLFLG_DEFLIST_SIMPLE]);
				clsDB.GetData(4, (int*)&buf.defput.kind[OUTPUTPALLFLG_DEFLIST_SIMPLE_LIST]);

			} else {										// �ʏ�
				no = buf.nPutNum;
				buf.output[no].edasysID = typid;

				clsDB.GetData(2, (int*)&buf.output[no].kind[OUTPUTPALLFLG_DEFLIST_DETAIL]);
				clsDB.GetData(3, (int*)&buf.output[no].kind[OUTPUTPALLFLG_DEFLIST_SIMPLE]);
				clsDB.GetData(4, (int*)&buf.output[no].kind[OUTPUTPALLFLG_DEFLIST_SIMPLE_LIST]);


				buf.nPutNum = no + 1;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);

	return true;
}
//5CGL�ɔ����ł͎g�p���Ȃ�
//------------------------------------------
// �����p�^�[���ŗL�p�����[�^�Ǎ�
//------------------------------------------
//bool ParamManager::Read_PCommonOutput()
//{
//	CString sql;					// SQL��
//	int id;
//
//
//	//// �p�����[�^������
//	CString cDbName;
//	cDbName = DB_PCOMMON_OUTPUT;
//	P_PCOMMON_OUTPUT&	p = m_PcommonOutput;
//	P_PCOMMON_OUTPUT	buf;
//	memset(&buf, 0x00, sizeof(buf));
//
//
//	//// ODBC�ڑ��N���X �C���X�^���X����
//	OdbcBase clsDB;
//	if( ! clsDB.Connect(TASKINI_NAME)) {
//		// �f�[�^�x�[�X�ڑ��G���[
//		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
//		syslog(SYSNO_DB_CONNECT_ERR, "");
//		return false;
//	}
//
//	////////////////////////////////////
//	// �}�X�^�[
//	////////////////////////////////////
//	cDbName = DB_PCOMMON_OUTPUT;
//	//// SQL���쐬
//	sql =  "SELECT";
//	sql += " pcode ";
//	sql += ", �X�g�b�v�}�[�NFF���M�L�� ";
//	sql += " FROM ";
//	sql += cDbName;
//	sql += " ORDER BY pcode ASC";
//
//	//// ���s
//	if( !clsDB.ExecuteDirect(sql)) {
//		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
//		return false;
//	}
//
//	//// �擾
//	while (TRUE) {
//		int sqlrc = clsDB.Fetch();
//		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
//
//			// ��L�[
//			clsDB.GetData(1, &id);
//			if( MAX_PCODE < id || 0 >= id ) {
//				// ��L�[�s��
//				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
//				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
//				continue;
//			}
//			id = id -1;		// �g���₷���悤��0�I���W���ɕύX
//
//
//
//			//-----------------
//			//�f�[�^��
//			clsDB.GetData(2, &buf.rec[id].nStopMarkFFSend );
//
//		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
//			break;	
//		} else {								// �G���[
//			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
//			return false;
//		}
//	}
//
//	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
//	memcpy( &m_PcommonOutput, &buf, sizeof(buf));
//	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
//	return true;
//}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPC�p�����[�^�Ǎ�
//------------------------------------------
// ���ʃp�����[�^ �Ǎ�
//------------------------------------------
bool ParamManager::Read_Param_Common()
{
	CString sql;					// SQL��
	double  wk;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_PARAM_COMMON;
	P_PARAM_COMMON&	p = m_Param_Common;
	P_PARAM_COMMON	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT ";
	sql += " [PLG�p���X���[�g] ";				// �p���X���[�g�͈ꉞDB����擾����i���Ƃ�DM����擾���ADB�㏑���j
	sql += ",[PLG�p���X���[�g��r] ";
	sql += ",[PLG�p���X���[�g�\��] ";
	sql += ",0 ";
	sql += ",0 ";
	sql += ",0 ";
	//sql += ",[�J�E���^�{�[�hPLG���{��] ";
	//sql += ",[�J�E���^�{�[�hPLG�M������] ";
	//sql += ",[�J�E���^�{�[�hPLG�ʑ�] ";
	//sql += ",[�J�E���^�{�[�hPLG�ʑ���] ";
	//sql += ",[�J�E���^�{�[�hPLG�f�W�^���t�B���^] ";
	sql += ",[�\���Ԋ��] ";
	sql += ",[�\���ԋ���] ";
	sql += ",[ExSyncIntval] ";
	sql += ",[ExSyncMinH] ";
	sql += ",[ExSyncMinL] ";
	//sql += ",[AGCbyCamcont] ";
	sql += ",0 ";		// AGCbyCamcont���Ȃ��H
	sql += ",[�O�ڒ����`�̏d�Ȃ�] ";
#ifndef DISABLE_GAP_CALC
	sql += ", UseWarpProj";
#else
	sql += ",0 ";		// �A�Ԃ��ς�肽���Ȃ�����
#endif

	// �Œ蕪��\��
	sql += ",[�J����PLG���{��] ";
	sql += ",[�J����PLG������] ";
	sql += ",[�J����PLG���{��_O] ";
	sql += ",[�J����PLG������_O] ";
	sql += ",[�J����PLG���{��_U] ";
	sql += ",[�J����PLG������_U] ";
	sql += ",[�J����PLG���{��_T] ";
	sql += ",[�J����PLG������_T] ";
	
	// �G�b�W���o����
	sql += ",[EdgeRestrictPort] ";
	
	// �t���[���摜�Ԉ���
	sql += ",[ShFrameShrinkX] ";
	sql += ",[ShFrameShrinkY] ";
	
	sql += " FROM ";
	sql += cDbName;
		
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// �f�[�^��
			clsDB.GetData(1,  &buf.dPls );
			clsDB.GetData(2,  &buf.dPlsSub);
			clsDB.GetData(3,  &buf.dPlsDsp);
			//clsDB.GetData(2,  &buf.typCntSet[0].nPlgMul );
			//clsDB.GetData(3,  &buf.typCntSet[0].nPlgPulse );
			//clsDB.GetData(4,  &buf.typCntSet[0].nPlgPhase );
			//clsDB.GetData(5,  &buf.typCntSet[0].nPlgPhaseDiff );
			//clsDB.GetData(6,  &buf.typCntSet[0].nPlgFilter );

#ifndef PLG_DSP_KEN_EQAL
			// �\���pPLG�͍��񖳂�
#else
			// �����pPLG �� �\���pPLG������
#endif
			clsDB.GetData(7,  &buf.nDistPosition );
			clsDB.GetData(8,  &buf.dDistLen );

			clsDB.GetData(9,  &buf.nExSyncIntval );
			clsDB.GetData(10, &buf.nExSyncMinH );
			clsDB.GetData(11, &buf.nExSyncMinL );
			//clsDB.GetData(12, &buf.nAGCbyCamcont );
			clsDB.GetData(13, &wk );				buf.dLapRate = wk / 100.0f;

#ifndef DISABLE_GAP_CALC
			clsDB.GetData(14, &buf.nUseWarpProj );		
#endif

			clsDB.GetData(15, &buf.typCamCon[0].nMul );
			clsDB.GetData(16, &buf.typCamCon[0].nDiv );
			clsDB.GetData(17, &buf.typCamCon[1].nMul );
			clsDB.GetData(18, &buf.typCamCon[1].nDiv );
			clsDB.GetData(19, &buf.typCamCon[2].nMul );
			clsDB.GetData(20, &buf.typCamCon[2].nDiv );
			clsDB.GetData(21, &buf.typCamCon[3].nMul );
			clsDB.GetData(22, &buf.typCamCon[3].nDiv );

			clsDB.GetData(23, &buf.nEdgeRestrictPort);

			clsDB.GetData(24, &buf.nShFrameShrinkX);
			clsDB.GetData(25, &buf.nShFrameShrinkY);
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	// �L���������_�S�ʂƂ��ĂT�ʂ��l�̌ܓ�
	buf.dPls	= ((buf.dPls * 10000.0) + 0.5) / 10000.0;
	buf.dPlsSub = ((buf.dPlsSub * 10000.0) + 0.5) / 10000.0;
	buf.dPlsDsp = ((buf.dPlsDsp * 10000.0) + 0.5) / 10000.0;

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// �J�����Z�b�g�� �p�����[�^ �Ǎ�
//------------------------------------------
bool ParamManager::Read_Param_CamSet()
{
	CString sql;					// SQL��
	int camset;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_PARAM_CAMSET;
	P_PARAM_CAMSET&	p = m_Param_CamSet;
	P_PARAM_CAMSET	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT ";
	sql += " [camset] ";
	sql += ",[Hres] ";
	sql += ",[Xoffset] ";

#ifndef DISABLE_GAP_CALC		// ��ԍŌ�
	// ����Y��
	sql += ", ";
	if(true){
		CString		cWk;
		for(int ii=1; ii<=MAX_CAMERA_GAP_WIDDIV; ii++){
			cWk.Format("[����Y���␳_����������%d], ", ii);
			sql += cWk;
		}
		for(int ii=1; ii<=MAX_CAMERA_GAP_WIDDIV; ii++){
			cWk.Format("[����Y���␳_�W�v���{����%d], ", ii);
			sql += cWk;
		}		
	}
#endif

	sql += " FROM ";
	sql += cDbName;
		
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
		
			// ��L�[
			clsDB.GetData(1, &camset);
			if( MAX_CAMSET < camset || 0 >= camset ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� camset=%d]", my_sThreadName, cDbName, camset);
				syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d]", cDbName, camset);					
				continue;
			}
			camset = camset -1;		// �g���₷���悤��0�I���W���ɕύX

			// �f�[�^��
			clsDB.GetData(2, &buf.camset[camset].dHres );
			clsDB.GetData(3, &buf.camset[camset].dXoffset );

#ifndef DISABLE_GAP_CALC
			for(int ii=0; ii<MAX_CAMERA_GAP_WIDDIV; ii++) clsDB.GetData(4+ii,  &buf.camset[camset].nGapGroup[ii] );
			for(int ii=0; ii<MAX_CAMERA_GAP_WIDDIV; ii++) clsDB.GetData(4+MAX_CAMERA_GAP_WIDDIV+ii, &buf.camset[camset].nGapCount[ii] );
#endif

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}


//------------------------------------------
// �\�ʏ�ԕ� �p�����[�^ �Ǎ�
//------------------------------------------
bool ParamManager::Read_Param_Scode()
{
	CString sql;					// SQL��
	int scode;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_PARAM_SCODE;
	P_PARAM_SCODE&	p = m_Param_Scode;
	P_PARAM_SCODE	buf;
	memset(&buf, 0x00, sizeof(buf));

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT ";
	sql += " [scode] ";

#ifndef DISABLE_TINY_CALC
	// ��������
	sql += ",[�������׏d��_����_����] ";
	sql += ",[�������׏d��_����_����] ";
	sql += ",[�������׏d��_����_����] ";
	sql += ",[�������׏d��_����_����] ";
	sql += ",[�������׏d��_�����̂�] ";
	sql += ",[�������׏d��_�����̂�] ";
	sql += ",[�������׏d��_�����̂�] ";
	sql += ",[�������׏d��_�����̂�] ";

	sql += ",[����������臒l���x��1_�c��16] ";
	sql += ",[����������臒l���x��2_�c��16] ";
	sql += ",[����������臒l���x��3_�c��16] ";
	sql += ",[����������臒l���x��1_��4] ";
	sql += ",[����������臒l���x��2_��4] ";
	sql += ",[����������臒l���x��3_��4] ";
	sql += ",[����������臒l���x��1_�c4] ";
	sql += ",[����������臒l���x��2_�c4] ";
	sql += ",[����������臒l���x��3_�c4] ";
	sql += ",[����������臒l���x��1_1] ";
	sql += ",[����������臒l���x��2_1] ";
	sql += ",[����������臒l���x��3_1] ";

	sql += ",[����������臒l���x��1_�c��16] ";
	sql += ",[����������臒l���x��2_�c��16] ";
	sql += ",[����������臒l���x��3_�c��16] ";
	sql += ",[����������臒l���x��1_��4] ";
	sql += ",[����������臒l���x��2_��4] ";
	sql += ",[����������臒l���x��3_��4] ";
	sql += ",[����������臒l���x��1_�c4] ";
	sql += ",[����������臒l���x��2_�c4] ";
	sql += ",[����������臒l���x��3_�c4] ";
	sql += ",[����������臒l���x��1_1] ";
	sql += ",[����������臒l���x��2_1] ";
	sql += ",[����������臒l���x��3_1] ";
#endif

	sql += " FROM ";
	sql += cDbName;
		
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
		
			// ��L�[
			clsDB.GetData(1, &scode);
			if( MAX_SCODE < scode || 0 >= scode ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� scode=%d]", my_sThreadName, cDbName, scode);
				syslog(SYSNO_DB_DATA_ERR, "[%s scode=%d]", cDbName, scode);					
				continue;
			}
			scode = scode -1;		// �g���₷���悤��0�I���W���ɕύX

			// �f�[�^��

#ifndef DISABLE_TINY_CALC
			clsDB.GetData(5,  &buf.scode[scode].dTinyWeight.white_b_white_d );
			clsDB.GetData(6,  &buf.scode[scode].dTinyWeight.white_b_black_d );
			clsDB.GetData(7,  &buf.scode[scode].dTinyWeight.black_b_white_d );
			clsDB.GetData(8,  &buf.scode[scode].dTinyWeight.black_b_black_d );
			clsDB.GetData(9,  &buf.scode[scode].dTinyWeight.white_b );
			clsDB.GetData(10,  &buf.scode[scode].dTinyWeight.white_d );
			clsDB.GetData(11, &buf.scode[scode].dTinyWeight.black_b );
			clsDB.GetData(12, &buf.scode[scode].dTinyWeight.black_d );

			clsDB.GetData(13, &buf.scode[scode].dTinyLimit[0][0][0] );
			clsDB.GetData(14, &buf.scode[scode].dTinyLimit[0][0][1] );
			clsDB.GetData(15, &buf.scode[scode].dTinyLimit[0][0][2] );
			clsDB.GetData(16, &buf.scode[scode].dTinyLimit[0][1][0] );
			clsDB.GetData(17, &buf.scode[scode].dTinyLimit[0][1][1] );
			clsDB.GetData(18, &buf.scode[scode].dTinyLimit[0][1][2] );
			clsDB.GetData(19, &buf.scode[scode].dTinyLimit[0][2][0] );
			clsDB.GetData(20, &buf.scode[scode].dTinyLimit[0][2][1] );
			clsDB.GetData(21, &buf.scode[scode].dTinyLimit[0][2][2] );
			clsDB.GetData(22, &buf.scode[scode].dTinyLimit[0][3][0] );
			clsDB.GetData(23, &buf.scode[scode].dTinyLimit[0][3][1] );
			clsDB.GetData(24, &buf.scode[scode].dTinyLimit[0][3][2] );

			clsDB.GetData(25, &buf.scode[scode].dTinyLimit[1][0][0] );
			clsDB.GetData(26, &buf.scode[scode].dTinyLimit[1][0][1] );
			clsDB.GetData(27, &buf.scode[scode].dTinyLimit[1][0][2] );
			clsDB.GetData(28, &buf.scode[scode].dTinyLimit[1][1][0] );
			clsDB.GetData(29, &buf.scode[scode].dTinyLimit[1][1][1] );
			clsDB.GetData(30, &buf.scode[scode].dTinyLimit[1][1][2] );
			clsDB.GetData(31, &buf.scode[scode].dTinyLimit[1][2][0] );
			clsDB.GetData(32, &buf.scode[scode].dTinyLimit[1][2][1] );
			clsDB.GetData(33, &buf.scode[scode].dTinyLimit[1][2][2] );
			clsDB.GetData(34, &buf.scode[scode].dTinyLimit[1][3][0] );
			clsDB.GetData(35, &buf.scode[scode].dTinyLimit[1][3][1] );
			clsDB.GetData(36, &buf.scode[scode].dTinyLimit[1][3][2] );
#endif

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// �J�����Z�b�g�E�����\�ʏ�ԕ� �p�����[�^ �Ǎ�
//------------------------------------------
bool ParamManager::Read_Param_Camset_Scode()
{
	CString sql;					// SQL��
	int scode;
	int camset;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_PARAM_CAMSET_SCODE;
	P_PARAM_CAMSET_SCODE&	p = m_Param_CamSetScode;
	P_PARAM_CAMSET_SCODE	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT ";
	sql += " [scode] ";
	sql += ",[camset] ";
	sql += ",[EdgeDetectCam] ";

	sql += " FROM ";
	sql += cDbName;
		
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
		
			// ��L�[
			clsDB.GetData(1, &scode);
			clsDB.GetData(2, &camset);
			if( scode > MAX_SCODE || scode <= 0 || camset > MAX_CAMSET || camset <= 0 ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� scode=%d camset=%d]", my_sThreadName, cDbName, scode, camset);
				syslog(SYSNO_DB_DATA_ERR, "[%s scode=%d camset=%d]", cDbName, scode, camset);					
				continue;
			}
			scode  = scode -1;		// �g���₷���悤��0�I���W���ɕύX
			camset = camset -1;

			clsDB.GetData(3, &buf.scode[scode].camset[camset].nEdgeDetectCam );

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}


//------------------------------------------
// �J�����Z�b�g�E�J�����p�x�� �p�����[�^ �Ǎ�
//------------------------------------------
bool ParamManager::Read_Param_Camset_Angle()
{
	CString sql;					// SQL��
	int camset;
	int camangle;
//	int	wk;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_PARAM_CAMSET_ANGLE;
	P_PARAM_CAMSET_ANGLE&	p = m_Param_CamsetAngle;
	P_PARAM_CAMSET_ANGLE	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " [camangle] ";
	sql += ",[camset] ";

#ifndef DISABLE_GAP_CALC
	// �ˉe�ϊ�
	sql += ",[Xdelay] ";
	sql += ",[Xinsert] ";
	sql += ",[YdelayLeft] ";
	sql += ",[YdelayRight] ";
	sql += ",[Proj_a0] ";
	sql += ",[Proj_a1] ";
	sql += ",[Proj_a2] ";
	sql += ",[Proj_c1] ";
	sql += ",[Proj_c2] ";
#endif

	sql += " FROM ";
	sql += cDbName;
		
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
		
			// ��L�[
			clsDB.GetData(1, &camangle);
			clsDB.GetData(2, &camset);
			if( camset > MAX_CAMSET || camset <= 0  || camangle > MAX_CAMANGLE || camangle <= 0 ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� camset=%d camangle=%d]", my_sThreadName, cDbName, camset, camangle);
				syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d camangle=%d]", cDbName, camset, camangle);					
				continue;
			}
			camset = camset -1;		// �g���₷���悤��0�I���W���ɕύX
			camangle = camangle -1;

#ifndef DISABLE_GAP_CALC
			// �ˉe�ϊ�
			clsDB.GetData(3, &buf.camset[camset].camangle[camangle].nXdelay );
			clsDB.GetData(4, &buf.camset[camset].camangle[camangle].nXinsert );
			clsDB.GetData(5, &buf.camset[camset].camangle[camangle].nYdelayLeft );
			clsDB.GetData(6, &buf.camset[camset].camangle[camangle].nYdelayRight );

			clsDB.GetData(7, &buf.camset[camset].camangle[camangle].dProjA0 );
			clsDB.GetData(8, &buf.camset[camset].camangle[camangle].dProjA1 );
			clsDB.GetData(9, &buf.camset[camset].camangle[camangle].dProjA2 );
			clsDB.GetData(10, &buf.camset[camset].camangle[camangle].dProjC1 );
			clsDB.GetData(11, &buf.camset[camset].camangle[camangle].dProjC2 );
#endif

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// �\�ʏ�ԁE�J�����Z�b�g�E�J�����p�x�� �p�����[�^ �Ǎ�
//------------------------------------------
bool ParamManager::Read_Param_Camset_Angle_Scode()
{
	CString sql;					// SQL��
	int scode;
	int camset;
	int camangle;
//	int	wk;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_PARAM_CAMSET_ANGLE_SCODE;
	P_PARAM_CAMSET_ANGLE_SCODE&	p = m_Param_CamsetAngleScode;
	P_PARAM_CAMSET_ANGLE_SCODE	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " [scode] ";
	sql += ",[camangle] ";
	sql += ",[camset] ";
	sql += ",[CameraGain] ";
	sql += " FROM ";
	sql += cDbName;
		
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
		
			// ��L�[
			clsDB.GetData(1, &scode);
			clsDB.GetData(2, &camangle);
			clsDB.GetData(3, &camset);
			if( scode > MAX_SCODE || scode <= 0 || camset > MAX_CAMSET || camset <= 0  || camangle > MAX_CAMANGLE || camangle <= 0 ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� scode=%d camset=%d camangle=%d]", my_sThreadName, cDbName, scode, camset, camangle);
				syslog(SYSNO_DB_DATA_ERR, "[%s scode=%d camset=%d camangle=%d]", cDbName, scode, camset, camangle);					
				continue;
			}
			scode = scode -1;		// �g���₷���悤��0�I���W���ɕύX
			camset = camset -1;
			camangle = camangle -1;


			// �f�[�^��
			clsDB.GetData(4, &buf.scode[scode].camset[camset].camangle[camangle].dCameraGain);


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}
//------------------------------------------
// �O���[�v���̕K�v����PC�����擾
// int groupmax �擾����GroupNo�̏��
// �߂�l ���A���
//------------------------------------------
bool ParamManager::Read_HtGroupNum(int groupmax)
{
	char sqlSelect[256];
	char sqlSelectNum[256];
	int groupno = 1;

	memset(m_nHtGroupNum, 0x00, sizeof(m_nHtGroupNum));
	memset(m_nHtGroupMax, 0x00, sizeof(m_nHtGroupMax));
	
	// DB�ڑ��`�F�b�N
	OdbcBase cls_Db_Select;
	if (!cls_Db_Select.Connect(TASKINI_NAME))
	{
		return false;
	}
	// DB�ڑ��`�F�b�N
	OdbcBase cls_Db_SelectNum;
	if (!cls_Db_SelectNum.Connect(TASKINI_NAME))
	{
		return false;
	}

	memset(sqlSelect, 0x00, sizeof(sqlSelect));
	sprintf(sqlSelect, "SELECT distinct m.GroupNo, (SELECT COUNT(*) FROM M_PC_INF WHERE GroupNo = m.GroupNo) as MAX FROM %s m WHERE m.GroupNo > 0 ORDER BY m.GroupNo ASC", DB_PC_INF);

	//// ���s
	if (!cls_Db_Select.ExecuteDirect(sqlSelect))
	{
		return false;
	}

	//// �擾
	while (TRUE)
	{
		int sqlrc = cls_Db_Select.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK)
		{
			// �f�[�^�L��

			// �O���[�vID�擾
			cls_Db_Select.GetData(1, &groupno);
			if (0 >= groupno) {
				LOG(em_ERR), "[%s] %s GroupNo=%d �ُ�", my_sThreadName, DB_PC_INF, groupno);
				return false;
			}
			// �O���[�v��PC�����擾
			m_nHtGroupMax[groupno - 1] = 0;
			cls_Db_Select.GetData(2, &m_nHtGroupMax[groupno - 1]);

			m_nHtGroupNum[groupno - 1] = 0;
			memset(sqlSelectNum, 0x00, sizeof(sqlSelectNum));
			sprintf(sqlSelectNum, "SELECT COUNT(*) FROM %s WHERE GroupNo = %d AND PC�敪 = 0", DB_PC_INF, groupno);
			if (!cls_Db_SelectNum.ExecuteDirect(sqlSelectNum))
			{
				return false;
			}
			else
			{
				sqlrc = cls_Db_SelectNum.Fetch();
				if (sqlrc == KIZUODBC_FETCH_OK)
				{
					// �f�[�^�L��

					// �O���[�v���K�vPC���擾
					cls_Db_SelectNum.GetData(1, &m_nHtGroupNum[groupno - 1]);
				}
				else if (sqlrc == KIZUODBC_FETCH_NODATE)
				{
					// �f�[�^����
				}
				else
				{
					// �G���[
					return false;
				}
			}
		}
		else if (sqlrc == KIZUODBC_FETCH_NODATE)
		{
			// �f�[�^����
			break;
		}
		else
		{
			// �G���[
			return false;
		}

		groupno++;
		if (groupmax < groupno)
		{
			break;
		}
	}

	return true;
}