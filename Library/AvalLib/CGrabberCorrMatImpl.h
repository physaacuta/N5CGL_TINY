#pragma once


//���F�����s��̎����B
	class CGrabberCorrMatImpl : public IGrabberCorrMat
	{
	public:
        //[���F�s�� (r,g,b,offset)x3]
		struct CorrMatrix{
		enum { NCOEF  = 12 };
			float coeff[NCOEF];
		};
	protected:
		CorrMatrix*		my_mat_corr[2];					// ���F�s��l [0or1][�J�����Z�b�g]
        LONG			my_mat_fore;					// �_�u���o�b�t�@�[�Bmy_mat_corr�̂ǂ����̔z����g����
		int				my_max_camset;


		~CGrabberCorrMatImpl(){
			for(int ii=0; ii<2; ii++)
			{
				delete []my_mat_corr[ii] ;
			}

		}
	public:
		CGrabberCorrMatImpl(int nMaxCamset = 8)
			:my_max_camset(nMaxCamset)
			,my_mat_fore(0)
		{
			for(int ii=0; ii<2; ii++)
			{
				my_mat_corr[ii] = new CorrMatrix[ my_max_camset ];
			}
			ResetCorrMat();
		}


        //���݃A�N�e�B�u�ȍ��F�����s����擾
		const CorrMatrix *GetCorrMat( int camset ) const 
		{
			return &my_mat_corr[my_mat_fore][camset];
		}

        //���݃A�N�e�B�u�ȍ��F�����s���float�|�C���^�Ƃ��Ă��擾
		const float *GetCorrCoeff( int camset ) const 
		{
			return &my_mat_corr[my_mat_fore][camset].coeff[0] ;
		}

        //���F�����s���ݒ肵�A�A�N�e�B�u�l�X��؂�ւ���B
		void SetCorrMat(int camset, const float* mat)
		{
            LONG mat_fore = my_mat_fore;
			const LONG mat_back = 1 - my_mat_fore;
			memcpy(&my_mat_corr[mat_back][camset].coeff[0], mat, sizeof(CorrMatrix) );
			InterlockedExchange(&my_mat_fore, mat_back);
		}

        //���F�����s�������������B
		void ResetCorrMat()
		{
			const static CorrMatrix CorrMatIdent = { 1,0,0,0,  0,1,0,0,  0,0,1,0 };
			for(int ii=0; ii<2; ii++)
			{
				for(int jj=0; jj<my_max_camset; jj++)
				{
					memcpy( &my_mat_corr[ii][jj], &CorrMatIdent, sizeof( CorrMatrix ) );
				}
			}

		}

	};