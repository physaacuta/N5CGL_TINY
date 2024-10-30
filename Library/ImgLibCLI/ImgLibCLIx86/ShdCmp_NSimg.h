#ifndef NSimg_h
#define NSimg_h 1

#ifdef __cplusplus /* C++ */

#include <cstddef>
#ifdef _MSC_VER
namespace std { using ::size_t; }
#endif

extern "C" {

typedef unsigned char	   ubyte_t;
typedef signed char		   sbyte_t;
typedef unsigned short	   uword_t;
typedef short			   sword_t;
typedef unsigned long	   ulong_t;
typedef long			   slong_t;
#if !defined _MSC_VER && !defined __TURBOC__
typedef unsigned long long uquad_t;
typedef long long		   squad_t;
#else
typedef unsigned __int64   uquad_t;
typedef __int64			   squad_t;
#endif

// �G�b�W���o�֐�
long __stdcall get_edge(
	const ubyte_t* begin,				/* �o�b�t�@ */
	std::size_t	   size_x,				/* �� */
	std::size_t	   size_y,				/* ���� */
	std::size_t	   dec_rate,			/* �Ԉ����� */
	std::size_t	   threshold,			/* �X���b�V�����h */
	std::size_t	   cont_pels,			/* �A���� */
	int			   direction) throw();	/* �������� */

// �k�����Ċg�傷�邱�Ƃɂ���ă��[�p�X�t�B���^��������֐�
void __stdcall LPF(
	const ubyte_t* source,
	ubyte_t*	   destination,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   block_size_x,
	std::size_t	   block_size_y);

// look_up_table[num*256+den]��������quo�����߂�֐�
void __stdcall lut2byte(
	ubyte_t*	   quotient,
	const ubyte_t* numerator,
	const ubyte_t* denominator,
	std::size_t	   size_x,
	std::size_t	   size_y,
	const ubyte_t* look_up_table) throw();

// look_up_table[num*256+den]��������quo�����߂�֐�(non-temporal�o�[�W����)
void __stdcall lut2bytent(
	ubyte_t*	   quotient,
	const ubyte_t* numerator,
	const ubyte_t* denominator,
	std::size_t	   size_x,
	std::size_t	   size_y,
	const ubyte_t* look_up_table) throw();
    
void __stdcall patch_binarize(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

// �p�b�`��l���֐��i�ȉ�_1.._8�̓��x����)
void __stdcall patch_binarize_1(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_2(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_3(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_4(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_5(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_6(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_7(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_8(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

} /* end of extern "C" */

#ifdef _MSC_VER
#define for if (false) ; else for
#endif

#else /* OLD GOOD C */

#include <stddef.h>

typedef unsigned char	   ubyte_t;
typedef signed char		   sbyte_t;
typedef unsigned short	   uword_t;
typedef short			   sword_t;
typedef unsigned long	   ulong_t;
typedef long			   slong_t;
#if !defined _MSC_VER && !defined __TURBOC__
typedef unsigned long long uquad_t;
typedef long long		   squad_t;
#else
typedef unsigned __int64   uquad_t;
typedef __int64			   squad_t;
#endif

long __stdcall get_edge(
	const ubyte_t* begin,				/* �o�b�t�@ */
	size_t		   size_x,				/* �� */
	size_t		   size_y,				/* ���� */
	size_t		   dec_rate,			/* �Ԉ����� */
	size_t		   threshold,			/* �X���b�V�����h */
	size_t		   cont_pels,			/* �A���� */
	int			   direction);			/* �������� */

void __stdcall LPF(
	const ubyte_t* source,
	ubyte_t*	   destination,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   block_size_x,
	size_t		   block_size_y);

void __stdcall lut2byte(
	ubyte_t*	   quotient,
	const ubyte_t* numerator,
	const ubyte_t* denominator,
	size_t		   size_x,
	size_t		   size_y,
	const ubyte_t* look_up_table);

void __stdcall patch_binarize(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* �^�C���T�C�Y */
	size_t		   py,
	size_t		   lx,					/* �I�[�o���b�v */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_1(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* �^�C���T�C�Y */
	size_t		   py,
	size_t		   lx,					/* �I�[�o���b�v */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_2(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* �^�C���T�C�Y */
	size_t		   py,
	size_t		   lx,					/* �I�[�o���b�v */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_3(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* �^�C���T�C�Y */
	size_t		   py,
	size_t		   lx,					/* �I�[�o���b�v */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_4(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* �^�C���T�C�Y */
	size_t		   py,
	size_t		   lx,					/* �I�[�o���b�v */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_5(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* �^�C���T�C�Y */
	size_t		   py,
	size_t		   lx,					/* �I�[�o���b�v */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_6(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* �^�C���T�C�Y */
	size_t		   py,
	size_t		   lx,					/* �I�[�o���b�v */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_7(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* �^�C���T�C�Y */
	size_t		   py,
	size_t		   lx,					/* �I�[�o���b�v */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_8(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* �^�C���T�C�Y */
	size_t		   py,
	size_t		   lx,					/* �I�[�o���b�v */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);
#endif /* C or C++ */

/*
 * local variables:
 * tab-width: 4
 * end:
 * char-encoding: �r���������|�i�h�r.
 */
#endif /* !NSimg_h */
