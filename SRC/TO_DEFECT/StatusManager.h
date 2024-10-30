#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �ӃփC���N���[�h
#include "..\..\Include\ToTbl.h"										// �����p���L�������e�[�u���w�b�_

// ���[�J��
#include "MainConst.h"
#include "StatusBaseManager.h"
#include "ParamManager.h"

#include "OpSendFunc.h"												// �\���Ǘ����M�N���X
#include "ToOutMailSender.h"											// �O�����[���X���b�g���M���W���[��

// �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

using namespace KizuLib;

class StatusManager :	public StatusBaseManager
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �^�C�}�[�p�萔
private:
#ifndef LOCAL
	static const int PARAM_ST_WRITE_INTERVAL_TIME	= 30000;			// �X�e�[�^�X�����ݎ��� [ms]
	static const int PARAM_ST_SOGYO_INTERVAL_TIME   = 2000;				// ���Ə�ԏ������ݎ��� [ms]
#else
	static const int PARAM_ST_WRITE_INTERVAL_TIME	= 30000;			// �X�e�[�^�X�����ݎ��� [ms]
	static const int PARAM_ST_SOGYO_INTERVAL_TIME   = 2000;				// ���Ə�ԏ������ݎ��� [ms]
#endif

	//enum EM_DIV_PROCON_ST {											// �v���R���֑��M����X�e�[�^�X�l	
	//	DIV_PROCON_ST_NON	= -1,										// �����I�ȏ����l
	//	DIV_PROCON_ST_OK	= 0,										// ����
	//	DIV_PROCON_ST_NG,												// �ُ� (�d�̏�or�k��)
	//	DIV_PROCON_ST_STOP												// ��~
	//};

//// ���J�֐�
public:
	StatusManager(void);
	virtual ~StatusManager(void);


/////////////////////////////////////////////////////////////////////////////////////////
//// �v���p�e�B
public:
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; }		// �p�����[�^�Ǘ�

/////////////////////////////////////////////////////////////////////////////////////////
//// �@����(�O���A�N�Z�X)
public:

	//=====================================
	// SPM���
	void	SetSpmStat(EM_DIV_SPM_STAT st)	{ m_emSpmStat = st; };
	int		GetSpmStat(void)	{ return (int)m_emSpmStat; };
	bool	IsSpmStat(void)		{ return DIV_SPM_TRUE == m_emSpmStat;};

		
	//=====================================
	// �T�[�o�� �J���
	void	SetDoorOpen(bool val)					{ m_bIsDoorOpen = val; }	// �T�[�o�� �� �J/���
	void	SetKeyOpen(bool val)					{ m_bIsKeyOpen  = val; }	// �T�[�o�� �� �J/���
	bool	GetDoorOpen() { return m_bIsDoorOpen; }	// True:�T�[�o�� �� �J���
	bool	GetKeyOpen() { return m_bIsKeyOpen; }	// True:�T�[�o�� �� �J���

	//=====================================
	// �R�C�����
	void SetCoilInfStatus(COIL_INF* pCoil)								// �ʔ��̃R�C�����ɋ@���Ԃ��Z�b�g
			{ 
				AutoLock lock(&m_csLock); 
				m_pCoil = pCoil;
				if(NULL == pCoil){
					this->m_pCoilBase = NULL;
					memset(this->m_cKizukenNo, 0x00, SIZE_KIZUKEN_NO);
				}else{
					this->m_pCoilBase = &pCoil->data;
					memcpy(this->m_cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
				}

				CheckCoilInfStatus();	// ������Ԃ��Z�b�g
				
				//// �@���� �ύX
				// �����\�ʏ�� // �����p�^�[��
				if (NULL != mcls_pKiki) {
					mcls_pKiki->SetStatusQue("SCODE_TOP_ID", pCoil->setting.scode[0], false);
					mcls_pKiki->SetStatusQue("SCODE_BOT_ID", pCoil->setting.scode[1], false);
					mcls_pKiki->SetStatusQue("PCODE_TOP_ID", pCoil->setting.pcode[0], false);
					mcls_pKiki->SetStatusQue("PCODE_BOT_ID", pCoil->setting.pcode[1], false);
				}
				else {
					KizuFunction::SetStatus("SCODE_TOP_ID", pCoil->setting.scode[0], false);
					KizuFunction::SetStatus("SCODE_BOT_ID", pCoil->setting.scode[1], false);
					KizuFunction::SetStatus("PCODE_TOP_ID", pCoil->setting.pcode[0], false);
					KizuFunction::SetStatus("PCODE_BOT_ID", pCoil->setting.pcode[1], false);
				}
			}

	
	//=====================================
	// LED�Ɩ����u �����_���������
	void SetLedAutoCheckOff(bool bAutoOff) { m_bLedAutoCheckOff = bAutoOff; }

	//=========================================
	//// �����Ԏ擾
	// PCID
	int				GetHtID(int men, int no)	  const { return m_nHt_ID[men][no]; }					// �����Ώۂ̔���PC��pcid
	EM_DIV_BOOL		GetHtState(int men, int no)	  { return m_bHt_State[men][no]; }
	bool			IsHtState(int men, int no)	  { return DIV_BOOL_TRUE == m_bHt_State[men][no] ? true : false; }	// ���茟�����
	bool			IsHtState_AllOnOff(int men, EM_DIV_BOOL flg)   { for(int ii=0; ii<NUM_HANTEI; ii++) {if(flg!=m_bHt_State[men][ii]) return false;} return true; }	// �S��flg�Ɠ����l�ł���� true
	bool			IsHtState_OneOnOff(int men, EM_DIV_BOOL flg)
	{
		for(int ii=0; ii<NUM_HANTEI; ii++)
		{
			if(flg==m_bHt_State[men][ii])
				return true;
		}
		return false;
	}	// 1�ł�flg�Ɠ����l������� true

	//=========================================
	// �����Ԑݒ�
	void			SetHtState_AllOff(int men)		{ for(int ii=0; ii<NUM_HANTEI; ii++) m_bHt_State[men][ii] = DIV_BOOL_NON; }	// ��ԃ��Z�b�g
	void			SetHtState(int men, int no, EM_DIV_BOOL flg)	{ m_bHt_State[men][no] = flg; }			// �����Ԃ��Z�b�g

	//=========================================
	//// ���W�b�N�p��������Ԏ擾
	// �����Ώۂ̃��W�b�N�p������PC��pcid
	int				GetHtParaID(int pcno)	const {	return m_nHt_Para_ID[pcno]; }

	//=========================================
	// ���W�b�N�p�������������	
	EM_DIV_BOOL		GetHtParaState(int pcno)	const { return m_bHt_Para_State[pcno];	}
	void			SetHtParaID(int pcno, int value) { m_nHt_Para_ID[pcno] = value; }
	bool			IsHtParaState(int pcno)	{ return DIV_BOOL_TRUE == m_bHt_Para_State[pcno]? true : false;	}
	bool			IsHtParaState_AllOnOff(EM_DIV_BOOL flg)	{ for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) { if (flg != m_bHt_Para_State[ii]) return false; } return true; }	// �S��flg�Ɠ����l�ł���� true
	bool			IsHtParaState_OneOnOff(EM_DIV_BOOL flg) { for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) { if (flg == m_bHt_Para_State[ii]) return true; }	return false; }	// 1�ł�flg�Ɠ����l������� true
	int				GetGroupTopCamNo(int nFPGABoardNo);	// �w��X�C�b�`FPGA�{�[�hID�̃O���[�v�̐擪�J����No��Ԃ�

	//=========================================
	// ���W�b�N�p��������Ԑݒ�
	// ��ԃ��Z�b�g
	void			SetHtParaState_AllOff()
	{
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) m_bHt_Para_State[ii] = DIV_BOOL_NON;
	}
	// ���W�b�N�p��������Ԃ��Z�b�g
	void			SetHtParaState(int pcno, EM_DIV_BOOL flg)
	{
		m_bHt_Para_State[pcno] = flg;
	}

	//=========================================
	// ����PC�U��
	bool			SelectHanteiPC_SingleGroup(bool* bOk);									// �����\�Ȕ���PC�𔻕ʂ��� [��MainInstance]�i���񖢎g�p�j
	bool			SelectHanteiPC_MultiGroup(bool* bOk);									// �����\�Ȕ���PC�𔻕ʂ��� [��MainInstance]
	bool			SelectHanteiPC_ResumeGroup();											// �����\�ȃ��W�b�N�p������PC�𔻕ʂ��� [��MainInstance]

	//=========================================
	// �@���ԏ�������
	void			SetCamTemp(int camset, BYTE *temp)
						{
							AutoLock lock(&m_csLock); 
							for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++) {
								if( 0==temp[ii] ) continue;
								m_nCamTemp[camset][ii] = (int)temp[ii];
							}
						}
	void			SetCamTemp(int camset, int angle, float temp)
						{
							AutoLock lock(&m_csLock);
							m_nCamTemp[camset][angle] = (int)temp;
						}
	void			SetExpTime(int camset, int color, int exptime)
						{
							AutoLock lock(&m_csLock);
							m_nExpTime[camset][color] = exptime;
						}
	void			SetCamGain(int camset, int color, int gain)
						{
							AutoLock lock(&m_csLock);
							m_nCamGain[camset][color] = gain;
						}
	void			SetCamVolt(int camset, int volt)
						{
							AutoLock lock(&m_csLock);
							m_nCamVolt[camset] = volt;
						}
	void			SetBrValue(int camset, int color, int br)
						{
							AutoLock lock(&m_csLock);
							m_nBrValue[camset][color] = br;
						}
	//void			SetCamGain(int camset, float *gain)
	//					{
	//						AutoLock lock(&m_csLock);
	//						for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++){
	//							if( 0==gain[ii] ) continue;
	//							m_nCamGain[camset-1][ii] = (int)(gain[ii] * 100);
	//						}
	//					}
	//void			SetBrValue(int camset, BYTE *br)
	//					{
	//						AutoLock lock(&m_csLock);
	//						for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++){
	//							if( 0== br[ii] ) continue;
	//							m_nBrValue[camset-1][ii] = (int)(br[ii]);
	//						}
	//					}
	void			SetLineSpeed(int val) { m_nLineSpeed = val; }		// �����p���C�����x	[��SogyoMgr]
#ifdef DSP_SOGYO_CHECK
	void			SetLineSpeedDsp(int val) { m_nLineSpeedDsp = val; }	// �\���p���C�����x	[��SogyoMgr]
#endif
	void			SetLength(int val)		{ m_nLength = val; }		// �ʔ��� [m]	[��PlgMgr]
	void			SetLedValue(int men, int color, int value)			// �Ɩ����u�����l
						{m_nLedValue[men][color] = value;}

	//=========================================
	// �@���ԓǂݍ���
	int				GetExpTime(int camset, int color) { return m_nExpTime[camset][color]; }			// �J�����I�����Ԏ擾
	int				GetCamGain(int camset, int color) { return m_nCamGain[camset][color]; }			// �J�����Q�C���l�擾
	int				GetLedValue(int men, int color)   { return m_nLedValue[men][color]; }			// �Ɩ����u�����l�擾
	int				GetBrValue(int men, int color)	  { return m_nBrValue[men][color]; }			// �J�����P�x�擾

	//=========================================
	// �������ُ�c��
	bool			OnMyErrWar(TYP_MY_ERRWAR_INF const* pInf);			// �������̏�Ԕc�� (���^�X�N���ُ̈�c��)

	bool	GetEdgeUdpHealthySt(int men) {
		AutoLock lock(&m_csLock);
		return m_bEdgeUdpHealthySt[men]; 
	}

	bool	GetEdgeUdpTrackingSt(int men) {
		AutoLock lock(&m_csLock);
		return m_bEdgeUdpTrackingSt[men]; 
	}

//// �����o�[�֐�
private:
	void			CheckCoilInfStatus();								// ���݂̋@���Ԃ��R�C�����ɃZ�b�g
	void			WriteStatus();										// �X�e�[�^�X�������ݏ���
	void			SogyoStatus_LedOnOff();								// ���Ə�ԃ`�F�b�N����_LED�Ɩ��_��/��������

	//=========================================
	// ���z�֐�
	virtual void	OnReadKikiStatusEnd(bool bChange);					// �@���Ԏ擾 & ��ԃ`�F�b�N ������
	virtual void	ThreadFirst_AddHandle();							// �V�O�i���҂��s��̒ǉ�
	virtual void	OnAddSignal(int nEventNo);							// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)

	
//// �����o�[�ϐ�
private:

	//=========================================
	// �e�C���X�^���X
	ParamManager*			mcls_pParam;								// �p�����[�^�Ǘ�
	COIL_INF*				m_pCoil;									// ���݂̊�ʒʔ��̃R�C�����	

	// �V�O�i��
	HANDLE					m_evTimerWriteSt;							// �X�e�[�^�X�������݃^�C�}�[�n���h��
	HANDLE					m_evTimerSogyoSt;							// ���Ə�ԏ������݃^�C�}�[�n���h��

	//=========================================
	// ������
	int						m_nHt_ID   [NUM_MEN][NUM_HANTEI];			// �����Ώۂ̔���PC��pcid���Z�b�g  (PCKind���B�����ɓ����Ă���̂�����Ȕ���PCID) PCID1�I���W��
	EM_DIV_BOOL				m_bHt_State[NUM_MEN][NUM_HANTEI];			// �����Ώۂ̔���PC�̏�� (PCKind��) (����������������true�B��~������������false)

	//// ����̓v���r�W����
	//int						m_nCy_ID   [NUM_MEN][NUM_HANTEI_CYCLE];		// �����Ώۂ̔���PC��pcid���Z�b�g  (PCKind���B�����ɓ����Ă���̂�����Ȕ���PCID) PCID1�I���W��
	//EM_DIV_BOOL				m_bCy_State[NUM_MEN][NUM_HANTEI_CYCLE];		// �����Ώۂ̔���PC�̏�� (PCKind��) (����������������true�B��~������������false)

	//=========================================
	// ���W�b�N�p���������
	int						m_nHt_Para_ID[NUM_ALL_HTPARA];				// �����Ώۂ̃��W�b�N�p������PC��pcid���Z�b�g  (PCKind���B�����ɓ����Ă���̂�����Ȕ���PCID) PCID1�I���W��
	EM_DIV_BOOL				m_bHt_Para_State[NUM_ALL_HTPARA];			// �����Ώۂ̃��W�b�N�p������PC�̏�� (PCKind��) (����������������true�B��~������������false)
	int						m_nHt_TopCamno[DIV_GROUP_MAX - 1];			// �e�J���������N�X�C�b�`�O���[�v�̐擪�J����No

	//=========================================
	// �@����
	int						m_nCamTemp[MAX_CAMSET][MAX_CAMANGLE];		// �J�������x
	int						m_nCamGain[MAX_CAMSET][MAX_CAMANGLE];		// �J�����Q�C��(10^2�\�L 1.00��100)
	int						m_nExpTime[NUM_CAMERA][MAX_CAMANGLE];		// �J�����I������[ns]
	int						m_nBrValue[NUM_CAMERA][MAX_CAMANGLE];		// �J�����P�x(0-255)
	int						m_nCamVolt[NUM_CAMERA];						// �J�����d��
	int						m_nLineSpeed;								// �����p���C�����x (���Ə�ԊǗ��̌����p���C�����x���g�p)
#ifdef DSP_SOGYO_CHECK
	int						m_nLineSpeedDsp;							// �\���p���C�����x (���Ə�ԊǗ��̕\���p���C�����x���g�p)
#endif
	int						m_nLength;									// �ʔ��� [m]
	EM_DIV_SPM_STAT			m_emSpmStat;								// SPM���(0:SPM��������, 1:SPM�����L��)
	

	// LED�Ɩ��֌W
	bool					m_bLedAutoCheckOff;							// LED�Ɩ� �����_��������ԃ`�F�b�N�������� true 
	int						m_nLedLineStopOffTime;						// LED�Ɩ� ���C���x�~OFF���� [��]
	int						m_nLedDaisyaOffLineTime;					// LED�Ɩ� ��ԃI�t���C��OFF����[�b] ���P�ʂ��b�ł��邱�Ƃɒ��ӁI�I
																		// �}�C�i�X�l�w��ŏ�������
	int						m_nLedLineStopCnt[NUM_MEN];					// ���C����~���̌o�߉�
	int						m_nLedLineRunOffCnt[NUM_MEN];				// ���C���ғ����ɏ������Ă���o�߉�
	int						m_nLedDaisyaOffLineCnt[NUM_MEN];			// ��ԃI�t���C�����̌o�߉�
	int						m_nLedValue[NUM_MEN][DIV_LED_COLOR_END];	// �Ɩ����u�̒����l[��][�F](�P��:%)


	// �T�[�o�� �J���
	bool					m_bIsDoorOpen;								// True:�J��� False:���
	bool					m_bIsKeyOpen;								// True:�J��� False:���

	// �G�b�WUDP���
	bool					m_bEdgeUdpHealthySt[NUM_MEN];				// UDP�w���V�[��ԁi��莞�ԓ��ɉ�������̓`������M��True�j
	bool					m_bEdgeUdpTrackingSt[NUM_MEN];				// UDP�g���b�L���O��ԁi��苗�����ɉ�������̓`������M��True�j


#ifdef IS_TO_CAMERA_TBL
	HANDLE					m_hCam;										// �J�����R���g���[�����e�[�u���@�n���h��
	TO_CAMERA_TBL*			mtbl_pCam;									// �J�����R���g���[�����e�[�u��	
#endif

public:

	int  GetCamSetPerPc() { return CAMSET_PER_PC; }						// 1PC������̃J�����Z�b�g��[��CamLinkSpdMgr]

private:

	static const int CAMSET_PER_PC = (MAX_CAMSET / NUM_MEN) / (NUM_HANTEI);		// 1PC������̃J�����Z�b�g��
};
