// *********************************************************************************
//	5CGL�ɔ����Ӄ� �����Ǘ� �P�̃e�X�g�V�i���I
//	[Ver]
//		Ver.01    2007/10/11  DB�ύX����DB�A�N�Z�X�e�X�g������
//
//	[����]
//�@�@�@5CGL�ɔ�������J�����̃V�i���I��`��{�N���X�ɒ�`����
//�@�@�@�P�̃e�X�g�d�l���F02_01 TO_DEFECT.xls
//		�e�X�g�P�[�X�@�@�FUnitTest1�FNo.1, 2
//		�@�@�@�@�@�@�@�@�FUnitTest2�FNo.3 - 16
//		�@�@�@�@�@�@�@�@�FUnitTest3�FNo.17
// *********************************************************************************
#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// �e�X�g�^�[�Q�b�g�p��`
#include <direct.h>
#include "..\..\..\..\Library\KizuLib\KizuLib.h"
#include "..\..\..\..\Library\KizuLib\Class\KizuMacro.h"
#include "..\..\..\..\Library\KizuLibMFC\Class\LogFileManager.h"
#include "..\..\..\..\SRC\TO_DEFECT\ParamManager.h"
#include "..\..\..\..\SRC\TO_DEFECT\StatusManager.h"
#include "..\..\..\..\SRC\TO_DEFECT\DBManager.h"
#include "..\..\..\..\SRC\TO_DEFECT\MaeManager.h"

using namespace KizuLib;

namespace TO_DEFECT_Test
{		
	LogFileManager*	mcls_pLog = NULL;

	TEST_MODULE_INITIALIZE(ModuleInitialize)
	{
		mcls_pLog = new LogFileManager(getmytaskname());
	}

	TEST_MODULE_CLEANUP(ModuleCleanup)
	{
		if (NULL != mcls_pLog)
		{
			delete mcls_pLog;
			mcls_pLog = NULL;
		}
	}

	TEST_CLASS(UnitTest1)
	{
	private:
		ParamManager *mcls_pParam = NULL;
		StatusManager	mcls_Status;
		DBManager mcls_DB;
		MaeManager	mcls_Mae;

		// ���Ǘ�No����
		void GetCurrentTimeString(LPCSTR lpPrefix, char *buf)
		{
			CString str;
			SYSTEMTIME time;

			GetLocalTime(&time);
			str.Format("%s%02d:%02d:%02d.%03d",
				lpPrefix, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
			strcpy(buf, str.operator LPCSTR());
		}

		// SQL�˗�
		void ExecSQL(DBManager::DeliveryDB *deliDB)
		{
			if (!mcls_DB.gque_AddSQLData.AddToTail(deliDB, 20, 20))
			{
				LOG(em_ERR), "DB�o�^�˗��L���[�t�� [DBDATA_COIL_INF]");
				delete deliDB->data;
				delete deliDB;
			}

		}

		// �e�X�g�f�[�^�Z�b�g�i�R�C�����j
		void SetInfo(DBDATA_COIL_INF* pWk)
		{
			strcpy(pWk->cCoilKey, "1");			// �L�[���
			strcpy(pWk->cMeiNo, "2");			// ����No
			strcpy(pWk->cCoilNo, "3");			// ���YNo
			pWk->nSendPoint = 4;				// ���M�ʒu
			pWk->nLenIn = 5;					// �R�C����
			pWk->nAtuZai = 6;					// �ޗ���
			pWk->nWidZai = 7;					// �ޗ���
			pWk->nAtuSei = 8;					// ������
			pWk->nWidSei = 9;					// ������
			pWk->nWidTrim = 10;					// ���H���g������
			pWk->nNobiSPM = 11;					// SPM�L�ї�
			pWk->nNobiTLV = 12;					// TLV�L�ї�
			pWk->nCutF = 13;					// �����ɂ�����t�����g�[�J�b�g�̎��ђ�
			pWk->nCutT = 14;					// �����ɂ�����e�[���[�J�b�g�̎��ђ�
			pWk->nSPMRoolKei_Up = 15;			// SPM���[�N���[���a (��)
			pWk->nSPMRoolKei_Down = 16;			// SPM���[�N���[���a (��)
			strcpy(pWk->cKenKijun_O, "1");		// ����� �\
			strcpy(pWk->cKenKijun_U, "2");		// ����� ��
			strcpy(pWk->cKurometo_Kind, "3");	// �N�����[�g ���ю��
			strcpy(pWk->cKurometo_Umu, "4");	// �N�����[�g ���їL��
			strcpy(pWk->cHinCode, "5");			// �i��
			strcpy(pWk->cMekkiKind, "6");		// ���b�L���
			strcpy(pWk->cSyuZai, "7");			// ��ގ�
			strcpy(pWk->cDifMaker, "8");		// �f�B�t�@�����V�����}�[�J�[
			strcpy(pWk->cKenban, "9");			// ���敪
			strcpy(pWk->cMekkiCycle, "0");		// ���b�L�T�C�N��
			strcpy(pWk->cInLineSPM, "1");		// ���x���[ �C�����C��SPM
			strcpy(pWk->cKagakuToyu, "2");		// ���w�����h��
			strcpy(pWk->cAtukkaSPM, "3");		// �I�t���C��SPM������
			strcpy(pWk->cMekkiKubun, "4");		// ���b�L�敪
			strcpy(pWk->cOmoteSyori1, "5");		// �\�ʏ���1����
			strcpy(pWk->cOmoteSyori2, "6");		// �\�ʏ���2����
			strcpy(pWk->cToyu, "7");			// �h��
			strcpy(pWk->cKouteiYobi, "8");		// �H���L���\��
			strcpy(pWk->cBaund, "9");			// �o�E���_�����킹
			strcpy(pWk->cCaplSyuZai, "0");		// CAPL ��ގ�
			strcpy(pWk->cMetuke, "1");			// �ڕt�K�i
			strcpy(pWk->cNi, "2");				// Ni�t����
			strcpy(pWk->cSodoTCM, "3");			// TCM���[���e�x
			strcpy(pWk->cSpmUmu, "4");			// SPM�����L��
			strcpy(pWk->cNextLine, "5");		// �v�掟�H���R�[�h
			strcpy(pWk->cMuke, "6");			// ����
			strcpy(pWk->cYouto, "7");			// �p�r
			strcpy(pWk->cCaplSiage, "8");		// CAPL�\�ʎd�グ
			strcpy(pWk->cCaplSanUmu, "9");		// CAPL�_���L��
			strcpy(pWk->cHinMode, "0");			// �i��敪
			strcpy(pWk->cGaCrKubun, "1");		// GA/CR�敪
			strcpy(pWk->cSpmJisekiUmu, "2");	// SPM�������їL��
			strcpy(pWk->cKouteiCode, "3");		// �H���R�[�h
		}

		// �e�X�g�f�[�^�Z�b�g�i�R�C�����сj
		void SetInfo(DBDATA_COIL_RESULT* pWk)
		{
			pWk->nEndMode = 1;									// ���ъi�[�敪
			pWk->nSt_Hantei[0] = 2;								// ������
			pWk->nSt_Hantei[1] = 3;								// ������
			pWk->nSt_Light[0] = 4;								// �Ɩ����u���
			pWk->nSt_Light[1] = 5;								// �Ɩ����u���
			pWk->nSt_CamControl = 6;							// �J�����R���g���[����H���
			pWk->nSt_CamSwitch = 7;								// �J���������N�X�C�b�`��H���
			pWk->nSt_Procon = 8;								// �v���R�����
			pWk->nSt_FF = 9;									// FF���
			pWk->nSt_Sqence = 10;								// �V�[�P���T���
			pWk->nSt_Cam_Inline[0] = 11;						// �J������ԏ��[�\��]
			pWk->nSt_Cam_Inline[1] = 12;						// �J������ԏ��[�\��]
			pWk->nSt_DbSkip[0] = 13;							// DB���ۑ�[�\��]
			pWk->nSt_DbSkip[1] = 14;							// DB���ۑ�[�\��]
			pWk->nSt_FrameSkip[0] = 15;							// ������[�\��]
			pWk->nSt_FrameSkip[1] = 16;							// ������[�\��]
			pWk->bStartWpdOk = 0;								// ������_�M�����o��
			pWk->setting.bWalm = 1;								// �E�H�[�}�[�ދ敪
			pWk->nCoilCnt = 17;									// �����{��
			pWk->setting.bDefScode[0] = 0;						// �����\�ʏ��No_Def�敪_�\
			pWk->setting.bDefScode[1] = 1;						// �����\�ʏ��No_Def�敪_��
			pWk->setting.bDefPcode[0] = 0;						// �����p�^�[���R�[�h_Def�敪_�\
			pWk->setting.bDefPcode[1] = 1;						// �����p�^�[���R�[�h_Def�敪_��
			pWk->setting.scode[0] = 18;							// �����\�ʏ��No_�\
			pWk->setting.scode[1] = 19;							// �����\�ʏ��No_��
			pWk->setting.pcode[0] = 20;							// �����p�^�[���R�[�h_�\
			pWk->setting.pcode[1] = 21;							// �����p�^�[���R�[�h_��
			pWk->nSpmStat = 22;									// SPM�����L��
			pWk->nLength = 23;									// �������u������
			pWk->nLengthSub = 24;								// ������r������
			pWk->nLengthDsp = 25;								// �\��������
			pWk->nOutCoilPos = 26;								// �o���P�ʐ�[�ʒu
			pWk->emUnten = EM_DIV_UNTEN::DIV_UNTEN_CAMERA;		// �^�]���
			pWk->emOneKensa[0] = EM_DIV_KENSA::DIV_KENSA_DBNON;	// �������_�\
			pWk->emOneKensa[1] = EM_DIV_KENSA::DIV_KENSA_EDGENG;// �������_��
			pWk->emOneKiki[0] = EM_DIV_KIKI::DIV_KIKI_JYU;		// �@����_�\
			pWk->emOneKiki[1] = EM_DIV_KIKI::DIV_KIKI_OK;		// �@����_��
			pWk->emPlg = EM_DIV_PLG::DIV_PLG_LINE;				// PLG�I���敪
			pWk->nResultSendOk = 27;							// �O�����ё��M����
			for (int ii = 0; ii < MAX_CAMSET; ii++)
			{
				for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
				{
					pWk->nBr[ii][jj][0] = ii * 10 + jj;			// �P�x(C)
				}
			}
			for (int ii = 0; ii < MAX_CAMSET; ii++)
			{
				for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
				{
					pWk->nExp[ii][jj] = ii * 10 + jj;			// �I������[clk]
				}
			}
			for (int ii = 0; ii < MAX_CAMSET; ii++)
			{
				for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
				{
					pWk->nGain[ii][jj] = ii * 10 + jj;			// �Q�C��[�{]
				}
			}
		}

	public:

		// �e�X�g���ځ@�P�`�Q
		TEST_METHOD(TestMethod1)
		{
			LOG(em_MSG), "\r\n�sTestMethod1�t--------------------------------------------------------------------------------------------");

			// �g�p�I�u�W�F�N�g�̐���
			mcls_pParam = new ParamManager();
			mcls_pParam->SetLogMgr(mcls_pLog);
			mcls_pParam->Start();

			LOG(em_MSG), "�c�a�Ǎ��e�X�g[%s]", DB_COMMON_MASTER);
			bool bResp = mcls_pParam->Read_CommonMast();
			Assert::AreEqual(true, bResp);
			if (!bResp)
			{
				LOG(em_ERR), "�Ǎ��G���[ [%s]", DB_COMMON_MASTER);
			}
			else
			{
				LOG(em_MSG), "�@RAS����F                            : %d", mcls_pParam->GetCommonMast().Ras.nColorNorm);
				LOG(em_MSG), "�@RAS�ُ�F                            : %d", mcls_pParam->GetCommonMast().Ras.nColorErr);
				LOG(em_MSG), "�@RAS���ӐF                            : %d", mcls_pParam->GetCommonMast().Ras.nColorWarn);
				LOG(em_MSG), "�@LOG�\������                          : %d", mcls_pParam->GetCommonMast().Log.nDispNum);
				LOG(em_MSG), "�@LOG�C�x���g�F                        : %d", mcls_pParam->GetCommonMast().Log.nEventColor);
				LOG(em_MSG), "�@LOG�d�̏�F                          : %d", mcls_pParam->GetCommonMast().Log.nFailColor_Ju);
				LOG(em_MSG), "�@LOG�y�̏�F                          : %d", mcls_pParam->GetCommonMast().Log.nFailColor_Kei);
				LOG(em_MSG), "�@�T����ʕ\����ԏ�                   : %d", mcls_pParam->GetCommonMast().Tansho.nDspSectionMin);
				LOG(em_MSG), "�@�T����ʕ\����ԑ�                   : %d", mcls_pParam->GetCommonMast().Tansho.nDspSectionMax);
				LOG(em_MSG), "�@�E�H�[�}�ގw��ʒu                   : %d", mcls_pParam->GetCommonMast().walm);
				LOG(em_MSG), "�@Jpeg�؂�o���摜�i��                 : %d", mcls_pParam->GetCommonMast().nJpegQuality);
				LOG(em_MSG), "�@�J�������x臒l                       : %d", mcls_pParam->GetCommonMast().nCamTempThr);
				LOG(em_MSG), "�@�G�b�W������_���{���                : %d", mcls_pParam->GetCommonMast().Edge.nType);
				LOG(em_MSG), "�@�G�b�W������_���o�͈�                : %d", mcls_pParam->GetCommonMast().Edge.dDetectRange);
				LOG(em_MSG), "�@�G�b�W������_���J�o���[���o�͈�      : %d", mcls_pParam->GetCommonMast().Edge.dDetectRangeR);
				LOG(em_MSG), "�@�G�b�W������_�����e�덷            : %d", mcls_pParam->GetCommonMast().Edge.dWidthDiff);
				LOG(em_MSG), "�@�G�b�W������_�O�t���[���Ƃ̋��e�덷  : %d", mcls_pParam->GetCommonMast().Edge.dPerEdgeDiff);
				LOG(em_MSG), "�@�G�b�W������_�n�ڌ�s����            : %d", mcls_pParam->GetCommonMast().Edge.nFromSkip);
				LOG(em_MSG), "�@�G�b�W������_�G�b�W���o�ُ�FF���M    : %d", mcls_pParam->GetCommonMast().Edge.nFFErr);
				LOG(em_MSG), "�@�G�b�W���o_���{���                  : %d", mcls_pParam->GetCommonMast().Edge_Restrict.nType);
				LOG(em_MSG), "�@�G�b�W���o_���o�͈�                  : %d", mcls_pParam->GetCommonMast().Edge_Restrict.dDetectRange);
				LOG(em_MSG), "�@�d���r�}�b�v�w�i�F_�y                : %d", mcls_pParam->GetCommonMast().DefMap.nBGColor_Kei);
				LOG(em_MSG), "�@�d���r�}�b�v�w�i�F_��                : %d", mcls_pParam->GetCommonMast().DefMap.nBGColor_Tyu);
				LOG(em_MSG), "�@�d���r�}�b�v�w�i�F_�d                : %d", mcls_pParam->GetCommonMast().DefMap.nBGColor_Ju);
				LOG(em_MSG), "�@�d���r�}�b�v�r��臒l_�y            : %d", mcls_pParam->GetCommonMast().DefMap.nDefCntThr_Kei);
				LOG(em_MSG), "�@�d���r�}�b�v�r��臒l_��            : %d", mcls_pParam->GetCommonMast().DefMap.nDefCntThr_Tyu);
				LOG(em_MSG), "�@�d���r�}�b�v�r��臒l_�d            : %d", mcls_pParam->GetCommonMast().DefMap.nDefCntThr_Ju);
				LOG(em_MSG), "�@�ݔ���_������__�㗬�J����           : %d", mcls_pParam->GetCommonMast().Dist.nKenBase);
				LOG(em_MSG), "�@�ݔ���_������__SPM                  : %d", mcls_pParam->GetCommonMast().Dist.nKenBaseSpm);
				LOG(em_MSG), "�@�ݔ���_�\����__��������             : %d", mcls_pParam->GetCommonMast().Dist.nDspBaseKensaBot);
				LOG(em_MSG), "�@�ݔ���_�\����__�\������             : %d", mcls_pParam->GetCommonMast().Dist.nDspBaseKensaTop);
				LOG(em_MSG), "�@�ݔ���_�\����__�V���[               : %d", mcls_pParam->GetCommonMast().Dist.nDspSharCut);
				LOG(em_MSG), "�@�ݔ���_������_�Q�[�g�M��ON_������_ : %d", mcls_pParam->GetCommonMast().Dist.nKenBaseGate);
				LOG(em_MSG), "�@�ݔ���_�\����_�Q�[�g�M��ON_�\����_ : %d", mcls_pParam->GetCommonMast().Dist.nDspBaseGate);
				LOG(em_MSG), "�@�r�ڋ߃����v_�s����_��[             : %d", mcls_pParam->GetCommonMast().nNoLamp[0]);
				LOG(em_MSG), "�@�r�ڋ߃����v_�s����_���[             : %d", mcls_pParam->GetCommonMast().nNoLamp[1]);
				LOG(em_MSG), "�@�r�ڋ߃����v_�o�͋��_�O             : %d", mcls_pParam->GetCommonMast().nKizuLamp[0]);
				LOG(em_MSG), "�@�r�ڋ߃����v_�o�͋��_��             : %d", mcls_pParam->GetCommonMast().nKizuLamp[1]);
				LOG(em_MSG), "�@�r�ڋ߃A���[��_�s����_��[           : %d", mcls_pParam->GetCommonMast().nNoAlarm[0]);
				LOG(em_MSG), "�@�r�ڋ߃A���[��_�s����_���[           : %d", mcls_pParam->GetCommonMast().nNoAlarm[1]);
				LOG(em_MSG), "�@�r�ڋ߃A���[��_�o�͋��_�O           : %d", mcls_pParam->GetCommonMast().nKizuAlarm[0]);
				LOG(em_MSG), "�@�r�ڋ߃A���[��_�o�͋��_��           : %d", mcls_pParam->GetCommonMast().nKizuAlarm[1]);
				LOG(em_MSG), "�@�{�C�X_�o�͈ʒu_�O                   : %d", mcls_pParam->GetCommonMast().nVoicePos);
				LOG(em_MSG), "�@�S���摜_�ۑ�_R                      : %d", mcls_pParam->GetCommonMast().nRPISaveColor_R);
				LOG(em_MSG), "�@�S���摜_�ۑ�_G                      : %d", mcls_pParam->GetCommonMast().nRPISaveColor_G);
				LOG(em_MSG), "�@�S���摜_�ۑ�_B                      : %d", mcls_pParam->GetCommonMast().nRPISaveColor_B);
				LOG(em_MSG), "�@�S���摜_�ۑ�_��                     : %d", mcls_pParam->GetCommonMast().nRPISaveColor_Sa);
				LOG(em_MSG), "�@PLG������r�ُ����l                : %d", mcls_pParam->GetCommonMast().dPlgDiffLimit[0]);
				LOG(em_MSG), "�@PLG�����\���ُ����l                : %d", mcls_pParam->GetCommonMast().dPlgDiffLimit[1]);
				LOG(em_MSG), "�@�v���R���R�C������r�ُ����l       : %d", mcls_pParam->GetCommonMast().dProLenDiffLimit);
				LOG(em_MSG), "�@���W�b�N�p�������@�\01_���̓J����01  : %d", mcls_pParam->GetCommonMast().nLogiParaCam[0][0]);
				LOG(em_MSG), "�@���W�b�N�p�������@�\01_���̓J����02  : %d", mcls_pParam->GetCommonMast().nLogiParaCam[0][1]);
				LOG(em_MSG), "�@���W�b�N�p�������@�\02_���̓J����01  : %d", mcls_pParam->GetCommonMast().nLogiParaCam[1][0]);
				LOG(em_MSG), "�@���W�b�N�p�������@�\02_���̓J����02  : %d", mcls_pParam->GetCommonMast().nLogiParaCam[1][1]);
			}

			LOG(em_MSG), "�c�a�Ǎ��e�X�g[%s]", DB_PCODE_MASTER);
			bResp = mcls_pParam->Read_PcodeOutput();
			Assert::AreEqual(true, bResp);
			if (!bResp)
			{
				LOG(em_ERR), "�Ǎ��G���[ [%s]", DB_PCODE_MASTER);
			}
			else
			{
				for(int ii=0; ii<MAX_PCODE; ii++)
				{
					LOG(em_MSG), "�@pcode: %02d --------------------------------", ii + 1);
					LOG(em_MSG), "�@�@�L������ : %d", mcls_pParam->GetPcodeOutput().rec[ii].umu);
					LOG(em_MSG), "�@�@�@�r��EdasysID             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.edasysID);
					LOG(em_MSG), "�@�@�@�T����ʕ\��_�\          : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DSP_O]);
					LOG(em_MSG), "�@�@�@�T����ʕ\��_��          : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DSP_U]);
					LOG(em_MSG), "�@�@�@�����P���r�}�b�v��ʕ\�� : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DSP_DEFMAP]);
					LOG(em_MSG), "�@�@�@FF���M�L��               : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_FF_SEND]);
					LOG(em_MSG), "�@�@�@�r�ڋߌy                 : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_ALARM_RANP_LOW]);
					LOG(em_MSG), "�@�@�@�r�ڋߏd                 : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_ALARM_RANP_HIGH]);
					LOG(em_MSG), "�@�@�@�p�g���C�g�o��           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_PATOLOGHT]);
					LOG(em_MSG), "�@�@�@�{�C�X�A���r             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_SEQ]);
					LOG(em_MSG), "�@�@�@�{�C�X�d���r             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_HIGH]);
					LOG(em_MSG), "�@�@�@�{�C�X�w�Q               : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_HEGE]);
					LOG(em_MSG), "�@�@�@�{�C�X�X���o             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_SURI]);
					LOG(em_MSG), "�@�@�@�{�C�X�\�������r         : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_SAME]);
					LOG(em_MSG), "�@�@�@�{�C�X�����r             : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_CYCLE]);
					LOG(em_MSG), "�@�@�@�{�C�X���̑��r           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_VOICE_OTHER]);
					LOG(em_MSG), "�@�@�@DB�ۑ�_���_�\           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_LOWGR_DB_O]);
					LOG(em_MSG), "�@�@�@DB�ۑ�_���_��           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_LOWGR_DB_U]);
					LOG(em_MSG), "�@�@�@DB�ۑ�_�D��_�\           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DB_O]);
					LOG(em_MSG), "�@�@�@DB�ۑ�_�D��_��           : %d", mcls_pParam->GetPcodeOutput().rec[ii].defput.kind[ParamManager::OUTPUTFLG_DB_U]);
					Sleep(1);
				}
			}

			// �g�p�I�u�W�F�N�g�̔j��
			mcls_pParam->Stop();
			delete mcls_pParam;
		}

		// �e�X�g���ځ@�R�`�P�U
		TEST_METHOD(TestMethod2)
		{
			LOG(em_MSG), "\r\n�sTestMethod2�t--------------------------------------------------------------------------------------------");

			// �g�p�I�u�W�F�N�g�̐���
			mcls_pParam = new ParamManager();
			mcls_pParam->SetLogMgr(mcls_pLog);
			mcls_pParam->Start();
			mcls_Status.SetLogMgr(mcls_pLog);
			mcls_Status.SetParamMgr(mcls_pParam);
			mcls_Status.SetKadou(DIV_KADOU_INIT);
			mcls_Status.SetDspSyslog(true);
			mcls_Status.SetAddStlog(true);
			mcls_Status.Start();
			mcls_Status.SetStatusRead();
			mcls_DB.SetLogMgr(mcls_pLog);
			mcls_DB.SetStatusMgr(&mcls_Status);
			mcls_DB.Alloc();
			mcls_DB.Start();

			char cKizukenNo[SIZE_KIZUKEN_NO];
			memset(cKizukenNo, 0x00, SIZE_KIZUKEN_NO);
			GetCurrentTimeString("UT-", cKizukenNo);// �Ǘ�No

			LOG(em_MSG), "�c�a�����e�X�g[%s]", DB_COIL_INF);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILINF;
				deliDB->data = (BYTE*)new DBDATA_COIL_INF;
				DBDATA_COIL_INF* pWk = (DBDATA_COIL_INF*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_INF));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// �Ǘ�No
				SetInfo(pWk);

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "�c�a�����e�X�g[%s %s]", DB_COIL_RESULT, DB_COIL_CAMERA);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILRESULT;
				deliDB->data = (BYTE*)new DBDATA_COIL_RESULT;
				DBDATA_COIL_RESULT* pWk = (DBDATA_COIL_RESULT*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_RESULT));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// �Ǘ�No
				SetInfo(pWk);

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "�c�a�X�V�e�X�g[%s]", DB_COIL_RESULT);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILRESULT_UPDATE;
				deliDB->data = (BYTE*)new DBDATA_COIL_RESULT_UPDATE;
				DBDATA_COIL_RESULT_UPDATE* pWk = (DBDATA_COIL_RESULT_UPDATE*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_RESULT_UPDATE));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// �Ǘ�No
				pWk->emMode = MODE_RECV_SHRCUT;
				pWk->nOutCoilPos = 1234;

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "�c�a�X�V�e�X�g[%s]", DB_COIL_RESULT);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILRESULT_UPDATE;
				deliDB->data = (BYTE*)new DBDATA_COIL_RESULT_UPDATE;
				DBDATA_COIL_RESULT_UPDATE* pWk = (DBDATA_COIL_RESULT_UPDATE*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_RESULT_UPDATE));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// �Ǘ�No
				pWk->emMode = MODE_DSP_WPD_ON;
				pWk->nDspLength = 5678;
				pWk->nSpmUmu = 1;

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "�c�a�����e�X�g[%s]", DB_COIL_POSITION);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILPOSITION;
				deliDB->data = (BYTE*)new DBDATA_COIL_POSITION;
				DBDATA_COIL_POSITION* pWk = (DBDATA_COIL_POSITION*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_POSITION));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->nID = 1234;
				for (int ii = 0; ii < NUM_MEN; ii++)pWk->emKensa[ii] = (EM_DIV_KENSA)(EM_DIV_KENSA::DIV_KENSA_INIT + ii);
				for (int ii = 0; ii < NUM_MEN; ii++)pWk->emKiki[ii] = (EM_DIV_KIKI)(EM_DIV_KIKI::DIV_KIKI_OK + ii);
				for (int ii = 0; ii < 2; ii++)		pWk->dEdge[ii] = (float)(ii + 1);
				pWk->dWalk = 5678;

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "�c�a�����e�X�g[%s]", DB_COIL_TRAND);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILTRAND;
				deliDB->data = (BYTE*)new DBDATA_COIL_TRAND;
				DBDATA_COIL_TRAND* pWk = (DBDATA_COIL_TRAND*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_TRAND));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->nMen = 1;
				pWk->nLen = 9;
				pWk->dSpmHosei = 1000;
				pWk->nSpeed = 1111;
				pWk->dCoilWidMax = 2222;
				pWk->dCoilWidMin = 3333;
				pWk->dWalk = 4444;
				for (int ii = 0; ii < DIV_LED_COLOR_END; ii++)		 pWk->nLightValue[ii] = ii + 1;
				for (int ii = 0; ii < NUM_CAM_POS; ii++)
				{
					for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) pWk->nBr[ii][jj][0] = ii * NUM_CAM_ANGLE + jj;
				}
				for (int ii = 0; ii < NUM_CAM_POS; ii++)
				{
					for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) pWk->nExp[ii][jj] = ii * NUM_CAM_ANGLE + jj;
				}
				for (int ii = 0; ii < NUM_CAM_POS; ii++)
				{
					for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) pWk->dGain[ii][jj] = (float)(ii * NUM_CAM_ANGLE + jj);
				}

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "�c�a�����e�X�g[%s]", DB_COIL_CUT_POS);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_COILCUTPOS;
				deliDB->data = (BYTE*)new DBDATA_COIL_CUT_POS;
				DBDATA_COIL_CUT_POS* pWk = (DBDATA_COIL_CUT_POS*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_COIL_CUT_POS));

				pWk->data.nCnt = 1;
				memcpy(pWk->data.incoil[0].cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->data.incoil[0].nInCutNum = 1111;
				strcpy(pWk->data.cID , "2222");
				pWk->nMode = 3333;
				pWk->data.incoil[0].nFront = 4444;
				pWk->data.incoil[0].nLength = 5555;
				pWk->data.incoil[0].nOutPos = 6666;

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			//LOG(em_MSG), "�c�a�����e�X�g[%s]", DB_DEFECT_INF_T);
			//{
			//	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
			//	deliDB->kubun = DBManager::em_DEFECTINF;
			//	deliDB->data = (BYTE*)new DBDATA_DEFECT_INF;
			//	DBDATA_DEFECT_INF* pWk = (DBDATA_DEFECT_INF*)deliDB->data;
			//	memset(pWk, 0x00, sizeof(DBDATA_DEFECT_INF));

			//	memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
			//	pWk->nKizuNo = 1;
			//	pWk->data.TorB = 0;
			//	pWk->data.nTid = 111;
			//	pWk->data.nGid = 1111;
			//	pWk->data.cycle_div = 2;
			//	pWk->data.dsws = 22;
			//	pWk->data.camset = 222;
			//	pWk->dLen = 2222;
			//	pWk->data.ds = 3;
			//	pWk->data.ws = 33;
			//	pWk->data.speed = 333;
			//	pWk->data.pitch = 3333;
			//	pWk->data.roolkei = 4;
			//	pWk->data.aria = 44;
			//	pWk->data.kizu_wid = 444;
			//	pWk->data.kizu_len = 4444;
			//	pWk->nLowGrFlg = 5;
			//	pWk->data.nWidDiv = 55;
			//	pWk->data.nWidAbsPos = 555;
			//	pWk->nIsDspFlg = 5555;
			//	pWk->data.nKizuNo = 6;
			//	pWk->data.h_res = 66;
			//	pWk->data.w_res = 666;
			//	pWk->data.nFrameNo = 6666;
			//	pWk->data.frm_x_min = 7;
			//	pWk->data.frm_x_max = 77;
			//	pWk->data.frm_y_min = 777;
			//	pWk->data.frm_y_max = 7777;
			//	pWk->data.cull_frm_x_min = 8;
			//	pWk->data.cull_frm_x_max = 88;
			//	pWk->data.cull_frm_y_min = 8888;
			//	pWk->data.cull_frm_y_max = 9;
			//	pWk->data.box_x_min = 99;
			//	pWk->data.box_x_max = 999;
			//	pWk->data.box_y_min = 9999;
			//	pWk->data.box_y_max = 12;
			//	pWk->data.center_x = 34;
			//	pWk->data.center_y = 56;
			//	pWk->data.h_size = 78;
			//	pWk->data.w_size = 90;
			//	for (int ii = 0; ii<MAX_HT1_CAMANGLE; ii++)
			//	{
			//		pWk->nImgsize[ii] = 1024;
			//		pWk->pImg[ii] = new BYTE[pWk->nImgsize[ii]];
			//		memset(pWk->pImg[ii], 0x00, sizeof(pWk->nImgsize[ii]));
			//	}
			//	pWk->data.base_box_x_min = 0;
			//	pWk->data.base_box_x_max = 1;
			//	pWk->data.base_box_y_min = 2;
			//	pWk->data.base_box_y_max = 3;
			//	pWk->data.base_center_x = 4;
			//	pWk->data.base_center_y = 5;
			//	pWk->data.base_h_size = 6;
			//	pWk->data.base_w_size = 7;

			//	//================================================
			//	//// �o�^�˗�
			//	ExecSQL(deliDB);
			//	Sleep(10000);

			//	{
			//		BCPSaveManager::DeliveryDB *deliDB = new BCPSaveManager::DeliveryDB;
			//		deliDB->kubun = BCPSaveManager::em_BCP_BATCH;
			//		deliDB->data = (BYTE*)new DBDATA_BCP_BATCH_EXEC;
			//		DBDATA_BCP_BATCH_EXEC* pData = (DBDATA_BCP_BATCH_EXEC*)deliDB->data;

			//		pData->bMode = false;

			//		//================================================
			//		//// �o�^�˗�
			//		if (!mcls_DB.GetBcpSaveMgr().gque_AddSQLData.AddToTail(deliDB, 20, 20)) {
			//			LOG(em_ERR), "DB�o�^�˗��L���[�t�� [BCP_BATCH_EXEC]");
			//			syslog(SYSNO_QUEFULL_ERR, "[BCP_BATCH_EXEC]");

			//			delete deliDB;
			//		}
			//		Sleep(10000);
			//	}
			//}

			//LOG(em_MSG), "�c�a�����e�X�g[%s]", DB_DEFECT_INF_B);
			//{
			//	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
			//	deliDB->kubun = DBManager::em_DEFECTINF;
			//	deliDB->data = (BYTE*)new DBDATA_DEFECT_INF;
			//	DBDATA_DEFECT_INF* pWk = (DBDATA_DEFECT_INF*)deliDB->data;
			//	memset(pWk, 0x00, sizeof(DBDATA_DEFECT_INF));

			//	memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
			//	pWk->nKizuNo = 1;
			//	pWk->data.TorB = 1;
			//	pWk->data.nTid = 111;
			//	pWk->data.nGid = 1111;
			//	pWk->data.cycle_div = 2;
			//	pWk->data.dsws = 22;
			//	pWk->data.camset = 222;
			//	pWk->dLen = 2222;
			//	pWk->data.ds = 3;
			//	pWk->data.ws = 33;
			//	pWk->data.speed = 333;
			//	pWk->data.pitch = 3333;
			//	pWk->data.roolkei = 4;
			//	pWk->data.aria = 44;
			//	pWk->data.kizu_wid = 444;
			//	pWk->data.kizu_len = 4444;
			//	pWk->nLowGrFlg = 5;
			//	pWk->data.nWidDiv = 55;
			//	pWk->data.nWidAbsPos = 555;
			//	pWk->nIsDspFlg = 5555;
			//	pWk->data.nKizuNo = 6;
			//	pWk->data.h_res = 66;
			//	pWk->data.w_res = 666;
			//	pWk->data.nFrameNo = 6666;
			//	pWk->data.frm_x_min = 7;
			//	pWk->data.frm_x_max = 77;
			//	pWk->data.frm_y_min = 777;
			//	pWk->data.frm_y_max = 7777;
			//	pWk->data.cull_frm_x_min = 8;
			//	pWk->data.cull_frm_x_max = 88;
			//	pWk->data.cull_frm_y_min = 8888;
			//	pWk->data.cull_frm_y_max = 9;
			//	pWk->data.box_x_min = 99;
			//	pWk->data.box_x_max = 999;
			//	pWk->data.box_y_min = 9999;
			//	pWk->data.box_y_max = 12;
			//	pWk->data.center_x = 34;
			//	pWk->data.center_y = 56;
			//	pWk->data.h_size = 78;
			//	pWk->data.w_size = 90;
			//	for (int ii = 0; ii<MAX_HT1_CAMANGLE; ii++)
			//	{
			//		pWk->nImgsize[ii] = (-1 * MAX_HT1_CAMANGLE) + ii;
			//	}
			//	pWk->data.base_box_x_min = 0;
			//	pWk->data.base_box_x_max = 1;
			//	pWk->data.base_box_y_min = 2;
			//	pWk->data.base_box_y_max = 3;
			//	pWk->data.base_center_x = 4;
			//	pWk->data.base_center_y = 5;
			//	pWk->data.base_h_size = 6;
			//	pWk->data.base_w_size = 7;

			//	//================================================
			//	//// �o�^�˗�
			//	ExecSQL(deliDB);
			//	Sleep(1000);

			//	{
			//		BCPSaveManager::DeliveryDB *deliDB = new BCPSaveManager::DeliveryDB;
			//		deliDB->kubun = BCPSaveManager::em_BCP_BATCH;
			//		deliDB->data = (BYTE*)new DBDATA_BCP_BATCH_EXEC;
			//		DBDATA_BCP_BATCH_EXEC* pData = (DBDATA_BCP_BATCH_EXEC*)deliDB->data;

			//		pData->bMode = false;

			//		//================================================
			//		//// �o�^�˗�
			//		if (!mcls_DB.GetBcpSaveMgr().gque_AddSQLData.AddToTail(deliDB, 20, 20)) {
			//			LOG(em_ERR), "DB�o�^�˗��L���[�t�� [BCP_BATCH_EXEC]");
			//			syslog(SYSNO_QUEFULL_ERR, "[BCP_BATCH_EXEC]");

			//			delete deliDB;
			//		}
			//		Sleep(1000);
			//	}
			//}

			LOG(em_MSG), "�c�a�����e�X�g[%s]", DB_EDGE_COIL_INF);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_EDGE_COIL_INF;
				deliDB->data = (BYTE*)new DBDATA_EDGE_COIL_INF;
				DBDATA_EDGE_COIL_INF* pWk = (DBDATA_EDGE_COIL_INF*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_EDGE_COIL_INF));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->nMen = 1111;
				pWk->dCoilWidMin = 2222;
				pWk->dCoilWidMax = 3333;
				pWk->nLimitEdgeErrNum = 4444;
				pWk->nLostEdgeNum[0] = 5555;
				pWk->nLostEdgeNum[1] = 6666;
				pWk->nFrameNumCoil = NUM_CAM_POS * 5;
				for (int ii = 0; ii < NUM_CAM_POS; ii++)
				{
					for (int jj = 0; jj < 5; jj++)
					{
						pWk->nFrameNumSkip[ii][jj] = NUM_CAM_POS * ii + jj;
					}
				}

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "�c�a�����e�X�g[%s]", DB_EDGE_CHANGE_INF);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_EDGE_CHANGE_INF;
				deliDB->data = (BYTE*)new DBDATA_EDGE_CHANGE_INF;
				DBDATA_EDGE_CHANGE_INF* pWk = (DBDATA_EDGE_CHANGE_INF*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_EDGE_CHANGE_INF));

				memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
				pWk->nPos = 1111;
				pWk->nMen = 2222;
				pWk->nFrameNo = 3333;
				pWk->nEdgeMode = 4444;
				pWk->dEdgeRawPos[0] = 5555;
				pWk->dEdgeRawPos[1] = 6666;
				pWk->dEdgeAnsPos[0] = 7777;
				pWk->dEdgeAnsPos[1] = 8888;
				pWk->nEdgeErrReason = 9999;

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			LOG(em_MSG), "�c�a�X�V�e�X�g[%s]", DB_CAMERA_LINKSWITCH);
			{
				DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
				deliDB->kubun = DBManager::em_CAMLINKSW;
				deliDB->data = (BYTE*)new DBDATA_CAMERA_LINKSWITCH;
				DBDATA_CAMERA_LINKSWITCH* pWk = (DBDATA_CAMERA_LINKSWITCH*)deliDB->data;
				memset(pWk, 0x00, sizeof(DBDATA_CAMERA_LINKSWITCH));

				mcls_Status.SetHtID(0, 0, 1);
				mcls_Status.SetHtID(0, 1, 2);
				mcls_Status.SetHtID(0, 2, 3);
				mcls_Status.SetHtID(0, 3, 4);
				mcls_Status.SetHtID(0, 4, 5);
				mcls_Status.SetHtID(1, 0, 6);
				mcls_Status.SetHtID(1, 1, 7);
				mcls_Status.SetHtID(1, 2, 8);
				mcls_Status.SetHtID(1, 3, 9);
				mcls_Status.SetHtID(1, 4, 10);

				pWk->nPCID[0][0] = 1;
				pWk->nPCID[0][1] = 2;
				pWk->nPCID[0][2] = 3;
				pWk->nPCID[0][3] = 5;
				pWk->nPCID[0][4] = 6;
				pWk->nPCID[1][0] = 8;
				pWk->nPCID[1][1] = 9;
				pWk->nPCID[1][2] = 10;
				pWk->nPCID[1][3] = 12;
				pWk->nPCID[1][4] = 13;

				pWk->nParaPCID[0] = 15;
				pWk->nParaPCID[1] = 16;

				pWk->nLogiParaCam[0][0] = 1;
				pWk->nLogiParaCam[0][1] = 2;
				pWk->nLogiParaCam[1][0] = 3;
				pWk->nLogiParaCam[1][1] = 4;

				//================================================
				//// �o�^�˗�
				ExecSQL(deliDB);
				Sleep(1000);
			}

			// �g�p�I�u�W�F�N�g�̔j��
			mcls_DB.Stop();
			mcls_DB.Free();
			mcls_Status.Stop();
			mcls_pParam->Stop();
			delete mcls_pParam;
		}

		// �e�X�g���ځ@�P�V
		TEST_METHOD(TestMethod3)
		{
			LOG(em_MSG), "\r\n�sTestMethod3�t--------------------------------------------------------------------------------------------");

			// �g�p�I�u�W�F�N�g�̐���
			mcls_Mae.SetLogMgr(mcls_pLog);
			mcls_Mae.Start();
			Sleep(5000);

			char cKizukenNo[SIZE_KIZUKEN_NO];
			memset(cKizukenNo, 0x00, SIZE_KIZUKEN_NO);
			GetCurrentTimeString("UT-", cKizukenNo);// �Ǘ�No

			LOG(em_MSG), "�c�a�����e�X�g[%s %s]", DB_MAE_COIL_INF, DB_MAE_FF_INF);
			{
				COIL_INF *pInf = new COIL_INF();
				memset(pInf, 0x00, sizeof(COIL_INF));

				// DB_MAE_COIL_INF�̈ȉ��̏����̃f�[�^��\�ߗp�ӂ��Ă���
				// �E�Ǘ�No         = 12345
				// �E���Ǘ�No�t���O = -1
				// �E����No         = "test"
				// �E�R�C��o        = "     coil-ok"
				// �ESPM�����L��    = 0
				// DB_MAE_FF_INF�̈ȉ��̏����̃f�[�^��\�ߗp�ӂ��Ă���
				// �E�Ǘ�No         = 12345
				// �ESPM�����L��    = 0
				memcpy(pInf->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);// �Ǘ�No
				strcpy(pInf->data.cMeiNo, "test");						
				strcpy(pInf->data.cCoilNoIn, "     coil-ok");
				pInf->nSt_Spm = 0;

				mcls_Mae.RecvCoilInf(pInf);
				Sleep(5000);
				delete pInf;
			}

			//// �g�p�I�u�W�F�N�g�̔j��
			mcls_Mae.Stop();
		}

		// �e�X�g���ځ@�P�W�`�Q�V
		TEST_METHOD(TestMethod4)
		{
			LOG(em_MSG), "\r\n�sTestMethod4�t--------------------------------------------------------------------------------------------");
		}
	};
}