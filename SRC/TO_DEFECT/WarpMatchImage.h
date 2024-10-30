//
// WarpMatchImage.h
// ���Ⴘ��v�Z�p���C�u����
// ���ׂē��o�͉͂�f�P��

#pragma once

namespace SwdetectImm {

	struct ManualAdjustCoef {
		int Xdelay;			// X�x��
		int Xinsert;		// X�}��
		int YdelayLeft;		// Y�x����	
		int YdelayRight;	// Y�x���E
	};

	struct ProjCoef {
		//	x' = ((a1+1)*x + c1) / (a0*x + 1)
		//	y' = (a2*x + y + c2) 
		double a0;
		double a1;
		double a2;
		double c1;
		double c2;
	};

	struct Dpoint {	// �S���P�ʂ͉�f�Amm�ŗ^���Ă͂����Ȃ�
		int	x;
		double dx;
		double dy;
	};

	struct RANSAC_param {
		int nsamp;			// �S���̃f�[�^�_����菬�����Ȃ��Ƃ����Ȃ�
		int ntrial;			// ���s��
		double error_limit;	// ���e�덷 ��f�P�ʂȂ̂ŁA0.3���炢���K��
	};


	// X�x��,X�}��,Y�x�����E����ˉe�ϊ��p�����[�^�����߂�
	// neyes�͊ᐔ�Acamwidth�̓J�����̉��r�b�g���Am[neyes],p[neyes]�ł���B
	// ����̏ꍇneyes=2, m[2]�ł���Am[0]�͐��Am[1]�͗��̒l�����ČĂԂ�
	// ���摜���W(x,y)�����摜���W(x',y')�ɑ΂��Ă̎ˉe�ϊ��p�����[�^��p[1]�ɓ���
	BOOL manual2proj(int neyes, int camwidth, const ManualAdjustCoef* m, ProjCoef* p);

	// �J�����������ʒux�ɂ���
	// �ˉe�ϊ��p�����[�^��p�ł������Ƃ��́A
	// ���J�����ɑ΂���Y����dx,dy�����߂�
	BOOL calcDxDy(const ProjCoef& p, Dpoint& d); 

	// �����ʂ̃Y���ʃf�[�^�z��d[n]����
	// �ˉe�ϊ��W��p�����߂�
	BOOL calcProj(int n, const Dpoint* d, ProjCoef& p);
	
	BOOL calcProj_ransac(int n, const Dpoint* d, ProjCoef& p, const RANSAC_param* rp=NULL);

};


