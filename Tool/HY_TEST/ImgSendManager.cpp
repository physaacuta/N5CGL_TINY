#include "StdAfx.h"
#include "ImgSendManager.h"
#include "time.h"


ImgSendManager::ImgSendManager(void) :
ThreadManager("ImgSend")
{
}

ImgSendManager::~ImgSendManager(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// �X���b�h
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ImgSendManager::ThreadFirst()
{
	//// ���O����

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { gque_Deli.g_evSem,
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}


//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int ImgSendManager::ThreadLast()
{
		DELIVERY_KEY *pDeli;				// �L���[�f�[�^

	// �������̃L���[��j��
	while(true) {
		pDeli = gque_Deli.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// �폜
		LOG(em_WAR), "[%s] �������L���[�폜 [%s]", my_sThreadName, pDeli->cKizukenNo);
		SAFE_DELETE(pDeli);
	}

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ImgSendManager::ThreadEvent(int nEventNo)
{
	DELIVERY_KEY*	pDeli;			// �󂯓n���˗��f�[�^

	////// �V�O�i���̏���
	enum {	EV_QUE = 0,						// �L���[�˗�
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:
		if(true) {
			//LOG(em_MSG),"[%s] Start ---->", my_sThreadName);
			pDeli = gque_Deli.GetFromHead(); 
			SetDefectInfo( pDeli );
			SAFE_DELETE(pDeli);
			//LOG(em_MSG),"[%s] <----- End", my_sThreadName);
		}
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// 1.�r���(�r�摜) �Z�b�g�E���M
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void ImgSendManager::SetDefectInfo(DELIVERY_KEY* pDeli)
{
	CString cfileName[MAX_CAMANGLE];	// �t�@�C���p�X
	CFile cfile[MAX_CAMANGLE];			// �t�@�C���I�u�W�F�N�g
	bool bfile[MAX_CAMANGLE];			// �I�[�v������
	long nFileLen[MAX_CAMANGLE];		// �t�@�C���T�C�Y
	static char imageBuff[2000000];		// �摜�i�[�o�b�t�@
	long nImageSize;

	for(int ii=0 ; ii < MAX_CAMANGLE; ii++){
		nFileLen[ii] = 0;
		bfile[ii] = false;
	}

	//// ���M�f�[�^���� (���M��ŊJ�����鎖) 
		
	nImageSize = 0;
	for (int jj = 0; jj< MAX_CAMANGLE; jj++) {
		if( (pDeli->nTid % 3) + 1 == 1 ){
			switch (jj) {
			case 0:
				cfileName[jj] = ".\\�r01_1.jpg";
				break;
			case 1:
				cfileName[jj] = ".\\�r01_2.jpg";
				break;
			case 2:
				cfileName[jj] = ".\\�r01_3.jpg";
				break;
			case 3:
			default:
				cfileName[jj] = ".\\�r01_4.jpg";
				break;
			}

		} else if ((pDeli->nTid % 3) + 1 == 2) {
			switch (jj) {
			case 0:
				cfileName[jj] = ".\\�r02_1.jpg";
				break;
			case 1:
				cfileName[jj] = ".\\�r02_2.jpg";
				break;
			case 2:
				cfileName[jj] = ".\\�r02_3.jpg";
				break;
			case 3:
			default:
				cfileName[jj] = ".\\�r02_4.jpg";
				break;
			}

		} else {
			switch (jj) {
			case 0:
				cfileName[jj] = ".\\�r1.jpg";
				break;
			case 1:
				cfileName[jj] = ".\\�r2.jpg";
				break;
			case 2:
				cfileName[jj] = ".\\�r3.jpg";
				break;
			case 3:
			default:
				cfileName[jj] = ".\\�r4.jpg";
				break;
			}
		}

		if ("" == cfileName[jj]) {
			nFileLen[jj] = 0;
			nImageSize += nFileLen[jj];
		} else {
			if(cfile[jj].Open(cfileName[jj], 0, 0)) {
				bfile[jj] = true;
				nFileLen[jj] = cfile[jj].GetLength();
				nImageSize += nFileLen[jj];
			}
		}
	}

	// ���M�f�[�^�T�C�Y�����߂�
	int nSendSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_DETECT) + nImageSize;

	// BYTE�z����쐬����
	BYTE* pSend = new BYTE[nSendSize];
	memset(pSend, 0x00, nSendSize);

	// �w�b�_���擪�A�h���X���Z�b�g����
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];

	// �f�[�^���擪�A�h���X���Z�b�g����
	TO_SO_DATA_DETECT& data = (TO_SO_DATA_DETECT&)pSend[sizeof(TO_SO_HEAD)];

	// �x�[�X�w�b�_��
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;
	head.base_head.subinf = pDeli->nSysKbn;
	// �g���w�b�_��
	memcpy( head.KizukenNo, pDeli->cKizukenNo , sizeof(head.KizukenNo)); //�� �Ǘ�No

	head.men = pDeli->nMen;									//�� �\���敪 (0:�\ 1:��)
	head.kind = TO_SO_DATA_KIND_DETECT;						// �r���(�r���+�r�摜)

	// �f�[�^��
	for (int jj = 0; jj< MAX_CAMANGLE; jj++) {
		data.image_len[jj] = nFileLen[jj];			//�� �摜�f�[�^�� (�ЂƂ��̉摜�T�C�Y) (0:�r����, -1:�T�����Ă��Ȃ�, 0<�r�L��)
	}

	data.datas.TorB = pDeli->nMen;				//�� �\���敪 (0:�\ 1:��)
	data.datas.nTid = pDeli->nTid;				//�� �r��EdasysID
	data.datas.nGid = pDeli->nGid;				//�� �O���[�hEdasysID

	int dsws = (pDeli->dDS > pDeli->dWS ? 1 : 0 );
	data.datas.dsws = dsws;						// DSWS�敪 0:DS 1:WS
	if ( 0 == pDeli->nMen ) {
		data.datas.camset = rand() % 2;			// �J�����y�AID (0�`1)
	} else {
		data.datas.camset = rand() % 2 + 2;		// �J�����y�AID (2�`3)
	}

	data.datas.nKizuNo = pDeli->nKizuSeq;					// �rNo
	data.datas.y = float(pDeli->nInY);						//�� T����̈ʒu[mm]
	data.datas.y_out = float(pDeli->nInY);					//�� T����̈ʒu[mm](�o��) �e�X�g�c�[���ł�y�Ɠ����l���Z�b�g����
	data.datas.ds = pDeli->dDS;							// DS���G�b�W����̈ʒu[mm]
	data.datas.ws = pDeli->dWS;							// WS���G�b�W����̈ʒu[mm]
	data.datas.dsc = (pDeli->dDS + pDeli->dWS) / 2 - pDeli->dDS;		// �Z���^�[����̋���[mm]
	data.datas.wsc = (pDeli->dDS + pDeli->dWS) / 2 - pDeli->dWS;		// �Z���^�[����̋���[mm]
	data.datas.speed = pDeli->nSpeed;						// ���x[mpm]
	data.datas.cycle_div = rand()%2;						// ������(0:����r, 1:�ȈՎ����r,�A���r )
	if (1 == pDeli->nSysKbn)	data.datas.cycle_div = 2;	// �����r�敪(0:����r, 1:�ȈՎ����r 2:�����r)
	data.datas.pitch = rand()%200;						// �s�b�`[mm]
	data.datas.roolkei = 1;								// ���[���a[mm]
	data.datas.nFrameNo = 1;								// �t���[��No

	// �r�O�ڂ������_���Ō��肵�A�r���S�ʒu�A�r���A�r�����A�ʐς��v�Z����
	int xmin = rand()%100 + 1;								// �O��Xmin[pixel]����
	int	xmax = xmin + rand()%150 +  5;						// �O��Xmax[pixel]����
	int ymin = rand()%100 + 1;								// �O��Ymin[pixel]����
	int ymax = ymin + rand()%150 +  5;						// �O��Ymax[pixel]����
	float h_res = 0.5;										// �c����\[mm/pixel]����
	float w_res = 0.6;										// ������\[mm/pixel]����

	data.datas.h_res = h_res;								// �c����\[mm/pixel]
	data.datas.w_res = w_res;								// ������\[mm/pixel]

	data.datas.box_x_min = xmin;							// �O��Xmin[pixel]
	data.datas.box_x_max = xmax;							// �O��Xmax[pixel]
	data.datas.box_y_min = ymin;							// �O��Ymin[pixel]
	data.datas.box_y_max = ymax;							// �O��Ymax[pixel]

	data.datas.center_x = int((xmax - xmin)/2) + xmin;		// �r���S�ʒuX [pixel]
	data.datas.center_y = int((ymax - ymin)/2) + ymin;		// �r���S�ʒuY [pixel]
	data.datas.kizu_wid = int( (xmax-xmin) * h_res);		// �r��[mm]
	data.datas.kizu_len = int( (ymax-ymin) * w_res);		// �r����[mm]
	if (pDeli->nArea) {
		data.datas.aria = pDeli->nArea;
	} else {
		data.datas.aria = (float)data.datas.kizu_wid*data.datas.kizu_len;	// �ʐ�[mm^2]
	}

	if( (pDeli->nTid % 3) + 1 == 2 ){
		data.datas.h_size = 256;							// �c��f��[pixel]	���T�C�Y�͎��ۂ̉摜�T�C�Y���猈�肷�邱��
		data.datas.w_size = 256;							// ����f��[pixel]	���T�C�Y�͎��ۂ̉摜�T�C�Y���猈�肷�邱��
	}else{
		data.datas.h_size = 256;							// �c��f��[pixel]	���T�C�Y�͎��ۂ̉摜�T�C�Y���猈�肷�邱��
		data.datas.w_size = 256;							// ����f��[pixel]	���T�C�Y�͎��ۂ̉摜�T�C�Y���猈�肷�邱��
	}

	data.datas.base_box_x_min = xmin;						// ���摜_�O��Xmin[pixel]
	data.datas.base_box_x_max = xmax;						// ���摜_�O��Xmax[pixel]
	data.datas.base_box_y_min = ymin;						// ���摜_�O��Ymin[pixel]
	data.datas.base_box_y_max = ymax;						// ���摜_�O��Ymax[pixel]

	data.datas.base_center_x = data.datas.center_x;			// ���摜_�r���S�ʒuX[pixel]
	data.datas.base_center_y = data.datas.center_y;			// ���摜_�r���S�ʒuY[pixel]

	data.datas.base_h_size = 256;							// ���摜_�c��f��[pixel]
	data.datas.base_w_size = 256;							// ���摜_����f��[pixel]

	data.datas.base_h_res = h_res;							// ���c����\[mm/pixel]
	data.datas.base_w_res = w_res;							// ��������\[mm/pixel]
	data.datas.nShrinkRatio = 4;							// �k����

	data.datas.nTYuu = mcls_pParam->GetYusenT(pDeli->nTid);	// �r��D��x (1�ŋ�)
	data.datas.nGYuu = mcls_pParam->GetYusenG(pDeli->nGid);	// �O���[�h�D��x (1�ŋ�)
	data.datas.nWidDiv = pDeli->x;							// �������敪 �e�X�g�c�[���Ō��肵���������ʒu��ݒ肷��
	data.datas.nObjTansho = pDeli->nObjTansho;				// �T����ʏo�͑Ώ��r(1:�Ώ�)
	data.datas.nObjDeflist = pDeli->nObjDeflist;			// �d���׈ꗗ��ʑΏ��r(1:�Ώ�)
	data.datas.nModeDeflist = pDeli->dspmode;				// �d���׈ꗗ�����\���Ώ��r(1:�Ώ�)

	SYSTEMTIME time;
	GetLocalTime(&time);
	data.datas.time[0] = time.wYear;						// ��������
	data.datas.time[1] = time.wMonth;						// ��������
	data.datas.time[2] = time.wDay;							// ��������
	data.datas.time[3] = time.wHour;						// ��������
	data.datas.time[4] = time.wMinute;						// ��������
	data.datas.time[5] = time.wSecond;						// ��������

	// �摜�f�[�^�Z�b�g
	int nImageSize2 = 0;
	for (int jj = 0; jj < MAX_CAMANGLE; jj++ ) {
		
		// �摜�T�C�Y��0�̏ꍇ�̓Z�b�g���Ȃ��i�ł��Ȃ��j
		if (0 == nFileLen[jj]) continue;
		if (!bfile[jj]) continue;

		memset(imageBuff, 0x00, sizeof(imageBuff));
		cfile[jj].Read(&imageBuff, sizeof(imageBuff));
		memcpy(&data.image[nImageSize2], imageBuff, nFileLen[jj]);
		nImageSize2 += nFileLen[jj];
	
	}

	// �\���Ǘ��ɑ��M
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);

	// �J���Ă����t�@�C�����N���[�Y����
	for (int jj = 0; jj < MAX_CAMANGLE; jj++ ) {
		if (!bfile[jj]) continue;
		cfile[jj].Close();
	}
}

