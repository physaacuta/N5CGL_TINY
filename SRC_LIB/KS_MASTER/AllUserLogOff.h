#pragma once

#include <string>
#include <windows.h>
#include <WtsApi32.h>

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"			// ���[�J�[�X���b�h�}�l�[�W���[

#pragma comment(lib, "Wtsapi32.lib")

class AllUserLogOff : public FuncBase
{

// ���J�֐�
public:
	AllUserLogOff(void);
	~AllUserLogOff(void);

	bool Exec();				// �����[�g�ڑ����Ă��郆�[�U�[�Ƀ��O�I�t�����s
	bool Change();				// �R���\�[������ �\�łȂ����ɂ́A�R���\�[����\�ɕύX����

};

