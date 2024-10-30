'// *********************************************************************************
'//	�O���C���^�[�t�F�[�X�p�w�b�_�[
'//	[Ver]
'//		Ver.01    2012/07/19  �V�K�쐬
'//
'//	[����]
'//		��{�I�ɁA����������
'// *********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices
Public Module OutInterface

	'//***************************************************************
	'// �ݒ�ϒ萔 ��`
	'//***************************************************************
	'//=======================================
	'// �ݒ�萔 ��`
	'//=======================================
	'//=======================================
	'// ���W�J�p���C���敪
	'//=======================================
	'// LINE�X�C�b�` (�K��1�R�̂݃R���g�A�E�g���鎖) VB�̏ꍇ���W���[�����ɕK�v'
#Const cLINE = "LINE_5CGL_TINY"

	'//***************************************************************
	'// �v���R���ʐM�@��`
	'//***************************************************************
	'//=======================================
	'// �v���R���萔
	'//=======================================
	Public Const PRO_SIZE_TC As Integer = 4						' �g�����U�N�V�����R�[�h MID
    Public Const PRO_MAX_CUT_COUNT As Integer = 16              ' �ő�J�b�g��
	Public Const PRO_SIZE_COIL_NO As Integer = 12				' �R�C��No (�����A�o������)
	Public Const PRO_SIZE_ORDER_NO As Integer = 4				' ����No

	Public Const PRO_SIZE_NEXTINF_CNT As Integer = 4			' ���H���R�C����񌏐�

	'//=======================================
	'// �g�����U�N�V�����R�[�h
	'//=======================================
	Public Const PRO_TC_COIL As Integer   = 1000				' �R�C�����`��
	Public Const PRO_TC_SAISUN As Integer = 1001				' �̐����ѓ`��
	Public Const PRO_TC_CUT As Integer    = 1002				' �J�b�g���ѓ`��
	Public Const PRO_TC_INIT As Integer   = 1003				' �C�j�V�����`��
	Public Const PRO_TC_ZISEKI As Integer = 2000                ' �������ѓ`��
    Public Const PRO_TC_HELS As Integer = -1                    ' �w���V�[�`��


    ' �v���R���`���T�u�w�b�_�[ [0]
    Public Const PRO_HEAD_H_GYOUMU As Byte = &H0				' �Ɩ��`�� ���� �����`��(CGL)
	' �v���R���`���T�u�w�b�_�[ [1]
	Public Const PRO_HEAD_L_OK As Byte = &H0					' ����


	'//=======================================
	'// �v���R�� �\����
	'//=======================================

	''�e�T�C�Y
	' �S��
    Public Const SIZE_PRO_TYPE_RESP As Integer = 2              ' ���X�|���X�`���T�C�Y
    Public Const SIZE_PRO_TYPE_COILINF As Integer = 512         ' �R�C�����`���T�C�Y
    Public Const SIZE_PRO_TYPE_SAISUN As Integer = 512          ' �̐����ѓ`���T�C�Y
    Public Const SIZE_PRO_TYPE_CUT As Integer = 128             ' �J�b�g���`���T�C�Y
    Public Const SIZE_PRO_TYPE_INIT As Integer = 128            ' �C�j�V�����`���T�C�Y
    Public Const SIZE_PRO_TYPE_ZISEKI As Integer = 512          ' �������ѓ`���T�C�Y
    Public Const SIZE_PRO_TYPE_HELS As Integer = 8              ' �w���V�[�`���T�C�Y

    ' �w�b�_�[��
    Public Const SIZE_PRO_TYPE_HEAD As Integer = 8            ' �w�b�_�[���T�C�Y
    Public Const SIZE_PRO_TYPE_COMMON As Integer = 56         ' ���ʏ��T�C�Y

    ' �f�[�^��
    Public Const SIZE_PRO_TYPE_DATA_COILINF As Integer = SIZE_PRO_TYPE_COILINF - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_SAISUN As Integer = SIZE_PRO_TYPE_SAISUN - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_CUT As Integer = SIZE_PRO_TYPE_CUT - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_INIT As Integer = SIZE_PRO_TYPE_INIT - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_ZISEKI As Integer = SIZE_PRO_TYPE_ZISEKI - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_HELS As Integer = SIZE_PRO_TYPE_HELS

    ' ���̑�
    Public Const SIZE_PRO_TYPE_KEY As Integer = PRO_SIZE_ORDER_NO + PRO_SIZE_COIL_NO
    Public Const SIZE_PRO_TYPE_NEXTINF As Integer = 24
    Public Const SIZE_PRO_NEXTINF_CNT As Integer    = 4			' ���H���R�C����񌏐� 
	Public Const SIZE_PRO_SAISUN_INF_CNT As Integer = 16
	Public Const SIZE_PRO_GRADE_INF As Integer      = 32
	
	' ���X�|���X�`�� (2byte)
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_RESP, pack:=1)> _
	Public Structure PRO_TYPE_RESP
		<VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
		Dim bHead() As Byte								' �T�u�w�b�_�[

		'�C�j�V�����C�Y
		Public Sub initialize()
			ReDim bHead(2 - 1)
		End Sub
	End Structure

    ' �w�b�_�[�� (8byte)
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_HEAD, Pack:=1)>
    Public Structure PRO_TYPE_HEAD

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim bHead() As Byte                             ' �T�u�w�b�_�[ (�z���g�͂����ɓ��ꂽ���Ȃ��������� long�������Ă���̂Ńo�E���_�����Y����)

        Dim nLen As Short                               ' �f�[�^�� (byte�P��)
        Dim nTC As Integer                              ' ���b�Z�[�WNo

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim bHead(2 - 1)
        End Sub

    End Structure

    ' ���ʏ�� (56byte)
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_COMMON, Pack:=1)>
    Public Structure PRO_TYPE_COMMON

        Dim nKubun As Short                             ' �`���敪 (0:�Ɩ��`�� 1:�����`��)	

        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)>
        Dim nDate() As Short                            ' ���M���� (�N���������b)
        Dim nGrandFlg As Short                          ' �O�����g�t���O (0:�`���̒��g ���� 1:�`���̒��g �ُ�)

        <VBFixedArray(40 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=40)>
        Dim cYobi() As Byte                             ' �\��

 
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim nDate(6 - 1)
            ReDim cYobi(40 - 1)
        End Sub

    End Structure

    ' �L�[���(�����_���R�C���P�ʂ̃L�[���)
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_KEY, pack:=1)> _
	Public Structure PRO_TYPE_KEY
		<VBFixedArray(PRO_SIZE_ORDER_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=PRO_SIZE_ORDER_NO)> _
		Dim cMeiNo() As Byte							' ����No
        <VBFixedArray(PRO_SIZE_COIL_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=PRO_SIZE_COIL_NO)> _
        Dim cCoilNo() As Byte                           ' �R�C��No

		'�C�j�V�����C�Y
		Public Sub initialize()
			ReDim cMeiNo(PRO_SIZE_ORDER_NO - 1)
            ReDim cCoilNo(PRO_SIZE_COIL_NO - 1)
		End Sub
	End Structure


	' �f�[�^��
	' �C�j�V�����`��
	'''' <summary>
	'''' �C�j�V�����`��
	'''' </summary>
	'''' <remarks></remarks>
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_INIT, pack:=1)> _
	Public Structure PRO_TYPE_DATA_INIT

        Dim Common As PRO_TYPE_COMMON                   ' ���ʏ��

        <VBFixedArray(SIZE_PRO_TYPE_DATA_INIT - SIZE_PRO_TYPE_COMMON - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_PRO_TYPE_DATA_INIT - SIZE_PRO_TYPE_COMMON)>
        Dim cYobi() As Byte                             ' �R�C��No

        '�C�j�V�����C�Y
        Public Sub initialize()
            Common.initialize()
            ReDim cYobi(SIZE_PRO_TYPE_DATA_INIT - SIZE_PRO_TYPE_COMMON - 1)
        End Sub
    End Structure

    ' �R�C�����`��
	' �R�C�����
	'///////////////////
	' �����_���R�C����񋤒ʕ� 24 byte
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_NEXTINF, pack:=1)> _
	Public Structure NEXT_COIL

        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cMeiNo() As Byte                            ' ����No
        <VBFixedArray(12 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=12)> _
        Dim cCoilNo() As Byte                           ' �R�C��No
		Dim nFrontLen As Integer						' �����t�����g�[���� [mm]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
         Dim cKouteiCode() As Byte                      ' ���H���R�[�h
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cYobi() As Byte                             ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim cMeiNo(4 - 1)
            ReDim cCoilNo(12 - 1)
            ReDim cKouteiCode(2 - 1)
            ReDim cYobi(2 - 1)
        End Sub
    End Structure


    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_COILINF - SIZE_PRO_TYPE_COMMON - SIZE_PRO_TYPE_NEXTINF * SIZE_PRO_NEXTINF_CNT - 16, Pack:=1)>
    Public Structure PRO_TYPE_COIL
        Dim KeyInf As PRO_TYPE_KEY                      ' �L�[No
        Dim nSendPoint As Integer                       ' ���M�ʒu
        Dim nLenIn As Integer                           ' �����_���R�C�� ���� [m]
        Dim nAtuZai As Integer                          ' �ޗ���[��m] (�o��)
        Dim nWidZai As Integer                          ' �ޗ��� [mm]
        Dim nAtuSei As Integer                          ' ������ [��m] (�o��)
        Dim nWidSei As Integer                          ' ������ [mm]
        Dim nWidTrim As Integer                         ' ���H���g������ [mm] (�m�[�g�������́A0)
        Dim nNobiSPM As Integer                         ' SPM�L�ї� [10-2%]
        Dim nNobiTLV As Integer                         ' TLV�L�ї� [10-2%]
        Dim nCutF As Integer                            ' �����t�����g�[����
        Dim nCutT As Integer                            ' �����e�[���[����
        Dim nSPMRoolKei_Up As Integer                   ' SPM���[�N���[���a (��) [10-1mm]
        Dim nSPMRoolKei_Down As Integer                 ' SPM���[�N���[���a (��) [10-1mm]

        <VBFixedArray(31 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=31)> _
        Dim nYobi() As Integer                          ' �\��
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cKenKijun_O() As Byte                       ' ����� �\
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cKenKijun_U() As Byte                       ' ����� ��
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cKurometo_Kind() As Byte                    ' �N�����[�g ���ю��
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cKurometo_Umu() As Byte                     ' �N�����[�g ���їL��
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cHinCode() As Byte                          ' �i��
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cMekkiKind() As Byte                        ' ���b�L���
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cSyuZai() As Byte                           ' ��ގ�
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cDifMaker() As Byte                         ' �f�B�t�@�����V�����}�[�J�[
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cKenban() As Byte                           ' ���敪
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cMekkiCycle() As Byte                       ' ���b�L�T�C�N��
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cInLineSPM() As Byte                        ' ���x���[ �C�����C��SPM
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cKagakuToyu() As Byte                       ' ���w�����h��
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cAtukkaSPM() As Byte                        ' �I�t���C��SPM������
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cMekkiKubun() As Byte                       ' ���b�L�敪
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cOmoteSyori1() As Byte                      ' �\�ʏ���1����
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cOmoteSyori2() As Byte                      ' �\�ʏ���2����
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cToyu() As Byte                             ' �h��
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cKouteiYobi() As Byte                       ' �H���L���\��
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cBaund() As Byte                            ' �o�E���_�����킹
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim cCaplSyuZai() As Byte                       ' CAPL ��ގ�
        <VBFixedArray(8 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
        Dim cMetuke() As Byte                           ' �ڕt�K�i
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cNi() As Byte                               ' Ni�t����
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cSodoTCM() As Byte                          ' TCM���[���e�x
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cSpmUmu() As Byte                           ' SPM�����L�� (�L��:O  ����:X)
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cNextLine() As Byte                         ' �v�掟�H���R�[�h
        <VBFixedArray(32 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)> _
        Dim cMuke() As Byte                             ' ����
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cYouto() As Byte                            ' �p�r
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cCaplSiage() As Byte                        ' CAPL�\�ʎd�グ
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cCaplSanUmu() As Byte                       ' CAPL�_���L��
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cHinMode() As Byte                          ' �i��敪
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cGaCrKubun() As Byte                        ' GA/CR�敪
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cSpmJisekiUmu() As Byte                     ' SPM�������їL���i�L��F0 ����:X�j
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cKouteiCode() As Byte                       ' �H���R�[�h
        <VBFixedArray(42 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=42)> _
        Dim cYobi1() As Byte                            '

        '�C�j�V�����C�Y
        Public Sub initialize()
            KeyInf.initialize()
            ReDim nYobi(31 - 1)
            ReDim cKenKijun_O(4 - 1)
            ReDim cKenKijun_U(4 - 1)
            ReDim cKurometo_Kind(2 - 1)
            ReDim cKurometo_Umu(2 - 1)
            ReDim cHinCode(1 - 1)
            ReDim cMekkiKind(1 - 1)
            ReDim cSyuZai(2 - 1)
            ReDim cDifMaker(1 - 1)
            ReDim cKenban(1 - 1)
            ReDim cMekkiCycle(1 - 1)
            ReDim cOmoteSyori1(1 - 1)
            ReDim cOmoteSyori2(1 - 1)
            ReDim cToyu(1 - 1)
            ReDim cKouteiYobi(1 - 1)
            ReDim cBaund(1 - 1)
            ReDim cCaplSyuZai(10 - 1)
            ReDim cMetuke(8 - 1)
            ReDim cNi(2 - 1)
            ReDim cSodoTCM(2 - 1)
            ReDim cSpmUmu(2 - 1)
            ReDim cNextLine(2 - 1)
            ReDim cMuke(32 - 1)
            ReDim cYouto(4 - 1)
            ReDim cCaplSiage(4 - 1)
            ReDim cCaplSanUmu(2 - 1)
            ReDim cHinMode(1 - 1)
            ReDim cGaCrKubun(1 - 1)
            ReDim cSpmJisekiUmu(2 - 1)
            ReDim cKouteiCode(2 - 1)
            ReDim cYobi1(42 - 1)
        End Sub
    End Structure

    '''' <summary>
    '''' �����p�R�C�����`��
    '''' </summary>
    '''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_COILINF, Pack:=1)>
    Public Structure PRO_TYPE_DATA_COILINF
        Dim Common As PRO_TYPE_COMMON                   ' ���ʏ��

        Dim CoilInf As PRO_TYPE_COIL                    ' �R�C�����

        <VBFixedArray(PRO_SIZE_NEXTINF_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=PRO_SIZE_NEXTINF_CNT)>
        Dim typInCoil() As NEXT_COIL                    ' �����_���R�C�����

        <VBFixedArray(16 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=16)>
        Dim cYobi() As Byte                             ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            Common.initialize()
            CoilInf.initialize()
            ReDim typInCoil(PRO_SIZE_NEXTINF_CNT - 1)
            For ii As Integer = 0 To PRO_SIZE_NEXTINF_CNT - 1
                typInCoil(ii).initialize()
            Next
            ReDim cYobi(16 - 1)
        End Sub
    End Structure

    ' �̐����ѓ`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_SAISUN_INF_CNT, pack:=1)> _
    Public Structure PRO_TYPE_SAISUN_INF
        Dim nStart As Integer                           ' �̐��J�n�ʒu [m]
        Dim nEnd As Integer                             ' �̐��I���ʒu [m]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim nYobi() As Integer

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim nYobi(2 - 1)
        End Sub
    End Structure

    '''' <summary>
    '''' �̐����ѓ`��
    '''' </summary>
    '''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_SAISUN, Pack:=1)>
    Public Structure PRO_TYPE_DATA_SAISUN
        Dim Common As PRO_TYPE_COMMON                   ' ���ʏ��

        Dim KeyInf As PRO_TYPE_KEY                      ' �L�[No

        Dim nLen As Integer                             ' ���уR�C���� [m]
        Dim nMakiLenF As Integer                        ' �t�����g�������܂꒷�� [m]
        Dim nSetCnt As Integer                          ' �Z�b�g�����̐���
        <VBFixedArray(9 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=9)>
        Dim nYobi() As Integer                          ' �\��

        <VBFixedArray(PRO_MAX_CUT_COUNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=PRO_MAX_CUT_COUNT)>
        Dim inf() As PRO_TYPE_SAISUN_INF                ' �̐����
        <VBFixedArray(128 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=128)>
        Dim cYobi() As Byte                             ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            Common.initialize()
            KeyInf.initialize()
            ReDim nYobi(9 - 1)
            ReDim inf(PRO_MAX_CUT_COUNT - 1)
            For ii As Integer = 0 To PRO_MAX_CUT_COUNT - 1
                inf.Initialize()
            Next
            ReDim cYobi(128 - 1)
        End Sub
    End Structure

    ' �J�b�g����
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_CUT, Pack:=1)>
    Public Structure PRO_TYPE_DATA_CUT

        Dim Common As PRO_TYPE_COMMON                   ' ���ʏ��

        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)>
        Dim cMeiNo() As Byte                            ' �����_���R�C�� ����No
        <VBFixedArray(12 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=12)>
        Dim cCoilNo() As Byte                           ' �����_���R�C�� ���YNo
        Dim nMode As Integer                            ' ���ы敪 (0:�n�ړ_�ʉ� 1:F�J�b�g)
        Dim nStart As Integer                           ' �̐��J�n�ʒu [m]
        Dim nEnd As Integer                             ' �̐��I���ʒu [m]
        Dim nKoban As Integer                           ' �q��(1CAPL�̂�)
        <VBFixedArray(32 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)>
        Dim cYobi() As Byte

        '�C�j�V�����C�Y
        Public Sub initialize()
            Common.initialize()
            ReDim cMeiNo(4 - 1)
            ReDim cCoilNo(12 - 1)
            ReDim cYobi(32 - 1)
        End Sub
    End Structure


    ' �O���[�h 1�`6�����r���������
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_GRADE_INF, pack:=1)> _
    Public Structure GRADE_INF
        Dim nMixedRate As Short                         ' ������ [10-2%]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim nYobi1() As Short
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim nTypeCnt() As Short                         ' �r�� (�r��1�`10)
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim nYobi2() As Short

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim nYobi1(2 - 1)
            ReDim nTypeCnt(10 - 1)
            ReDim nYobi2(3 - 1)
        End Sub

    End Structure


    ' ��������
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_ZISEKI, Pack:=1)>
    Public Structure PRO_TYPE_DATA_ZISEKI

        Dim Common As PRO_TYPE_COMMON                   ' ���ʏ��

        Dim nZiMode As Integer                          ' ���ы敪 (0:���� 1:�ُ�(�J�b�g���тŎ�M�����Y���R�C������) )
        Dim nKikiMode As Integer                        ' ���W�敪 (0:���� 1:�ُ�)
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)>
        Dim cMeiNo() As Byte                            ' �����_���R�C�� ����No
        <VBFixedArray(12 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=12)>
        Dim cCoilNo() As Byte                           ' �����_���R�C�� ���YNo
        Dim nKoban As Integer                           ' �q��(1CAPL�̂�)
        <VBFixedArray(36 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=36)>
        Dim cYobi1() As Byte
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)>
        Dim grade_inf() As GRADE_INF                    ' �O���[�h 1�`6�����r���������
        <VBFixedArray(192 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=192)>
        Dim cYobi() As Byte

        '�C�j�V�����C�Y
        Public Sub initialize()
            Common.initialize()
            ReDim cMeiNo(4 - 1)
            ReDim cCoilNo(12 - 1)
            ReDim cYobi1(40 - 1)
            ReDim grade_inf(6 - 1)
            ReDim cYobi(192 - 1)
        End Sub

    End Structure

    ' �w���V�[�`��
    '<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_HELS, Pack:=1)>
    'Public Structure PRO_TYPE_DATA_HELS
    '    <VBFixedArray(SIZE_PRO_TYPE_DATA_HELS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_PRO_TYPE_DATA_HELS)>
    '    Dim cYobi() As Byte                             ' �R�C��No

    '    '�C�j�V�����C�Y
    '    Public Sub initialize()
    '        ReDim cYobi(SIZE_PRO_TYPE_DATA_HELS - 1)
    '    End Sub
    'End Structure


    ' //=======================================
    ' // �v���R�� �`��������
    ' //=======================================

    ' �C�j�V�����`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_INIT, pack:=1)> _
    Public Structure PRO_TYPE_INIT
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_INIT

        '�C�j�V�����C�Y
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' �R�C�����`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_COILINF, pack:=1)> _
    Public Structure PRO_TYPE_COILINF
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_COILINF

        '�C�j�V�����C�Y
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' �̐����ѓ`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_SAISUN, pack:=1)> _
    Public Structure PRO_TYPE_SAISUN
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_SAISUN

        '�C�j�V�����C�Y
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' �J�b�g���ѓ`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_CUT, pack:=1)> _
    Public Structure PRO_TYPE_CUT
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_CUT

        '�C�j�V�����C�Y
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' �������ѓ`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_ZISEKI, pack:=1)> _
    Public Structure PRO_TYPE_ZISEKI
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_ZISEKI

        '�C�j�V�����C�Y
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' // �w���V�[�`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_HELS, pack:=1)> _
    Public Structure PRO_TYPE_HEALTHY
        Dim head As PRO_TYPE_HEAD

        '�C�j�V�����C�Y
        Public Sub initialize()
            head.initialize()
        End Sub
    End Structure


    '//***************************************************************
    '// FF�ʐM�@��`
    '//***************************************************************
    '//=======================================
    '// FF�ʐM�n�@�Œ�T�C�Y
    '//=======================================
    Public Const MAX_FF_DFCT_CNT As Integer = 1800          ' �e�e�������ѓ`���r���

    Public Const MAX_FF_MAE_DINF_CNT As Integer = 7         ' �e�e�O�H���r���ѓ`���r���o���

    Public Const MAX_FF_MAE_DEF_CNT As Integer = 2000       ' �e�e�O�H���r���ѓ`�� (�S��)
    'Public Const MAX_FF_MAE_CC_CNT As Integer = 10			' �e�e�O�H���r���ѓ`���b�b�r���
    'Public Const MAX_FF_MAE_HOT_CNT As Integer = 50		' �e�e�O�H���r���ѓ`���g�n�s�r���
    'Public Const MAX_FF_MAE_PL_CNT As Integer = 80			' �e�e�O�H���r���ѓ`���o�k�r���
    'Public Const MAX_FF_MAE_TCM_ANA_CNT As Integer = 30	' �e�e�O�H���r���ѓ`���s�b�l�����
    'Public Const MAX_FF_MAE_TCM_KIZU_CNT As Integer = 30	' �e�e�O�H���r���ѓ`���s�b�l�r���
    'Public Const MAX_FF_MAE_ATU_CNT As Integer = 1800		' �e�e�O�H���r���ѓ`�����O�H�����
    ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
    Public Const MAX_FF_MAE_CC_CNT As Integer = 10          ' �e�e�O�H���r���ѓ`���b�b�r���
    Public Const MAX_FF_MAE_HOT_CNT As Integer = 50         ' �e�e�O�H���r���ѓ`���g�n�s�r���
    Public Const MAX_FF_MAE_PL_CNT As Integer = 80          ' �e�e�O�H���r���ѓ`���o�k�r���
    Public Const MAX_FF_MAE_TCM_ANA_CNT As Integer = 30     ' �e�e�O�H���r���ѓ`���s�b�l�����
    Public Const MAX_FF_MAE_YOBI_CNT As Integer = 30        ' �e�e�O�H���r���ѓ`�����̑����
    Public Const MAX_FF_MAE_ATU_CNT As Integer = 900        ' �e�e�O�H���r���ѓ`�����O�H�����
    Public Const MAX_FF_MAE_TCM_KIZU_CNT As Integer = 900   ' �e�e�O�H���r���ѓ`���s�b�l�r���
    ' 20221021 2TCM���@���ɔ������H������ ���� <<<---

    Public Const MAX_FF_STS_CNT As Integer = 30000          ' �e�e������ԓ`���������

    Public Const MAX_FF_MAE_THAN_CNT As Integer = 10        ' �e�e�O�H���r���ѓ`���ʔ��я��
    Public Const MAX_FF_MAE_BNKT_CNT As Integer = 20        ' �e�e�O�H���r���ѓ`���������

    '//=======================================
    '// �\���́i��FF�T�[�o�[�`���j
    '//=======================================

    ' �������ѓ`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=34, pack:=1)> _
    Public Structure FF_TYPE_HEAD

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' �`���R�[�h �\=M1 ��=M2
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' �A��(FF) 
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim bytDate() As Byte                   ' �`���������� �`�� YYYYMMDHHNNSS0
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' �H���R�[�h [CTL=A1]
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Meino() As Byte                     ' ���ߔԍ�
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim Coilno() As Byte                    ' �≄�R�C���ԍ�

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim bytDate(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Meino(4 - 1)
            ReDim Coilno(10 - 1)
        End Sub

    End Structure

    ' �f�[�^��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=30, pack:=1)> _
    Public Structure FF_TYPE_DATA
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim Kizukbn() As Byte                   ' �r���o�敪 0:�Ӄ�
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Kind() As Byte                      ' �r��
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Grade() As Byte                     ' �r�O���[�h
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim WidPos() As Byte                    ' �r�Е����ʒu
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Len() As Byte                       ' �r����
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim DefectPos() As Byte                 ' �r�����������ʒu
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim kbn() As Byte                       ' �r�敪
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim No() As Byte                        ' �rNO
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim OpeDsp() As Byte                    ' �I�y�K�C�\���t���O
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim SysKbn() As Byte                    ' �V�X�e���敪
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim Filler() As Byte                    ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Kizukbn(1 - 1)
            ReDim Kind(2 - 1)
            ReDim Grade(2 - 1)
            ReDim WidPos(4 - 1)
            ReDim Len(4 - 1)
            ReDim DefectPos(6 - 1)
            ReDim kbn(1 - 1)
            ReDim No(4 - 1)
            ReDim OpeDsp(1 - 1)
            ReDim SysKbn(1 - 1)
            ReDim Filler(7 - 1)
        End Sub

    End Structure


    ''FF�{��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_JISSEKI
        Dim Hd As FF_TYPE_HEAD                  ' �`���w�b�_�[

        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim CoilLen() As Byte                   ' �R�C������
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Mode() As Byte                      ' �r���������[�h
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim OverStatus() As Byte                ' �r���I�[�o�[�X�e�[�^�X
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim Id() As Byte                        ' �r���Ǘ�No
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim DataDefault() As Byte               ' �f�[�^�ݒ萔
        <VBFixedArray(32 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)> _
        Dim cYobi1() As Byte                    ' �\��
        <VBFixedArray(MAX_FF_DFCT_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_DFCT_CNT)> _
        Dim Dt() As FF_TYPE_DATA                ' �f�[�^
        <VBFixedArray(2000 - MAX_FF_DFCT_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2000 - MAX_FF_DFCT_CNT)> _
        Dim Dtyobi() As FF_TYPE_DATA            ' �f�[�^
        <VBFixedArray(900 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=900)> _
        Dim cYobi() As Byte                     ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim CoilLen(6 - 1)
            ReDim Mode(2 - 1)
            ReDim OverStatus(2 - 1)
            ReDim Id(20 - 1)
            ReDim DataDefault(4 - 1)
            ReDim cYobi1(32 - 1)
            ReDim Dt(MAX_FF_DFCT_CNT - 1)
            ReDim Dtyobi(2000 - MAX_FF_DFCT_CNT - 1)
            ReDim cYobi(900 - 1)
        End Sub

    End Structure

    Public Const FF_MAE_MEINO_LEN As Integer = 4        ' �e�e�O�H���r���ѓ`���������

    ''  //// �����`��
    ''FF�{��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=34, pack:=1)> _
    Public Structure FF_ANS_HD
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' �`���R�[�h [M7/M8]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' �A��(00)					  
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim DateD() As Byte                     ' �`���������� �`�� YYYYMMDHHNNSS
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' �H���R�[�h[CTL=A1]
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Meino() As Byte                     ' ���ߔԍ�
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim Coilno() As Byte                    ' �����R�C���ԍ�

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim DateD(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Meino(4 - 1)
            ReDim Coilno(10 - 1)
        End Sub

    End Structure


    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=2, pack:=1)> _
    Public Structure FF_ANS_DATA
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Status() As Byte                    ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Status(2 - 1)
        End Sub

    End Structure


    ''  FF_ANS
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_ANS
        Dim Hd As FF_ANS_HD                     ' �`���w�b�_�[
        Dim Data As FF_ANS_DATA                 ' �f�[�^
        <VBFixedArray(64964 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=64964)> _
        Dim cYobi() As Byte                     ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim cYobi(64964 - 1)
        End Sub

    End Structure

    '-------------------------------------------------------
    '�O�H���֘A �����Ή��p
    '-------------------------------------------------------
    ' �O�H���r���їv���`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_MAE_REQUEST

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim HeaderCode() As Byte                ' �`���R�[�h
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' �A�� ("00"�Œ�)
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim DateTime() As Byte                  ' �`���������� �`�� YYYYMMDDHHNNSS
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' �H���R�[�h  (CTL=A1)
        <VBFixedArray(FF_MAE_MEINO_LEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=FF_MAE_MEINO_LEN)> _
        Dim MeireiNo() As Byte                  ' ���ߔԍ��S��(	'�O	'�{���ߔԍ��R��)���E�l�ݒ�

        <VBFixedArray(64976 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=64976)> _
        Dim Filler() As Byte                    ' �\�� ��(0x20)

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim HeaderCode(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim DateTime(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim MeireiNo(FF_MAE_MEINO_LEN - 1)
            ReDim Filler(64976 - 1)
        End Sub

    End Structure

    ' �O�H���r���ѓ`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=113, Pack:=1)>
    Public Structure FF_MAE_TYPE_HEAD

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' �`���R�[�h (XX:�\, XX:��)
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' �A�� ("00"�Œ�)
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim DateTime() As Byte                  ' �`���������� �`�� YYYYMMDDHHNNSS
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim Line_Code() As Byte                 ' �H���R�[�h  (5CGL=85)
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Meino() As Byte                     ' ���ߔԍ�
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim O_UKbn() As Byte                    ' �\���敪(1:�\, 2:��)
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim ReiCoilNo() As Byte                 ' �≄�R�C���ԍ�
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim ReiCoilG() As Byte                  ' �≄�R�C���d�� [10-3t]
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim ReiCoilL() As Byte                  ' �≄�R�C������ [10-1m]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim ReiCoilAtu() As Byte                ' �≄�� [10-3mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim ItaatuMin() As Byte                 ' ���@�����@MIN [10-3mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim ItaatuMax() As Byte                 ' ���@�����@MAX [10-3mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1i() As Byte                ' ������P�@�� [10-1mm]
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Kensa() As Byte                     ' �����
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim Maki() As Byte                      ' ��������
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Zairyou() As Byte                   ' �ޗ��� [10-1mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1kMin() As Byte             ' ������P�@�Ќ����@MIN [10-1mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1kMax() As Byte             ' ������P�@�Ќ����@MAX [10-1mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1L() As Byte                ' ������P�@����(���b�h����) [m]
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim Mukesaki1N() As Byte                ' ������P�@���a [10-1���]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Itaatu() As Byte                    ' �� [10-3mm]
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim Irigawa() As Byte                   ' �������a [10-1���]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1KL() As Byte               ' ������P�@���f�� [10-1mm]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Makoutei() As Byte                  ' �O�H���R�[�h
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim RealMakoutei() As Byte              ' ���O�H���R�[�h
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)>
        Dim TCMFlag() As Byte                   ' ����/TCM�����ރt���O

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim DateTime(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Meino(4 - 1)
            ReDim O_UKbn(2 - 1)
            ReDim ReiCoilNo(10 - 1)
            ReDim ReiCoilG(5 - 1)
            ReDim ReiCoilL(6 - 1)
            ReDim ReiCoilAtu(5 - 1)
            ReDim ItaatuMin(5 - 1)
            ReDim ItaatuMax(5 - 1)
            ReDim Mukesaki1i(5 - 1)
            ReDim Kensa(4 - 1)
            ReDim Maki(1 - 1)
            ReDim Zairyou(5 - 1)
            ReDim Mukesaki1kMin(5 - 1)
            ReDim Mukesaki1kMax(5 - 1)
            ReDim Mukesaki1L(5 - 1)
            ReDim Mukesaki1N(3 - 1)
            ReDim Itaatu(5 - 1)
            ReDim Irigawa(3 - 1)
            ReDim Mukesaki1KL(5 - 1)
            ReDim Makoutei(2 - 1)
            ReDim RealMakoutei(2 - 1)
            ReDim TCMFlag(1 - 1)
        End Sub

    End Structure

    ' �O�H�����
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=30, pack:=1)> _
    Public Structure FF_MAE_TYPE_INF
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Kizukbn() As Byte                   ' �r���o�H��
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim Id() As Byte                        ' �Ǘ�No
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Mode() As Byte                      ' �����������[�h
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim DataDefault() As Byte               ' �r�ݒ��
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim OverStatus() As Byte                ' �r���I�[�o�[�X�e�[�^�X

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Kizukbn(2 - 1)
            ReDim Id(20 - 1)
            ReDim Mode(2 - 1)
            ReDim DataDefault(4 - 1)
            ReDim OverStatus(2 - 1)
        End Sub

    End Structure

    ' 
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=20, pack:=1)> _
    Public Structure FF_MAE_TYPE_VRCLINF
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim KizukenNo() As Byte                 ' VRCL�Ǘ�No

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim KizukenNo(20 - 1)
        End Sub

    End Structure

    ' From-To
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=24, pack:=1)> _
    Public Structure FF_MAE_TYPE_FROMTO
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim CoilNo() As Byte                    ' �≄�R�C��No
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim From() As Byte                      ' �̎�J�n�ʒu�����iFrom�j [0.1m]
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim To2() As Byte                       ' �̎�I���ʒu�����iTo�j	[0.1m]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Koban() As Byte                     ' �̎�q��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim CoilNo(10 - 1)
            ReDim From(6 - 1)
            ReDim To2(6 - 1)
            ReDim Koban(2 - 1)
        End Sub
    End Structure

    ' �r���
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=30, pack:=1)> _
    Public Structure FF_MAE_TYPE_DATA
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Kind() As Byte                      ' �r��
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Grade() As Byte                     ' �r�O���[�h
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Kizukbn() As Byte                   ' �r���o�H��
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim kbn() As Byte                       ' �r�敪
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim DefectPos() As Byte                 ' �r���������ʒu
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim WidPos() As Byte                    ' �r�Е����ʒu
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Len() As Byte                       ' �r����
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim No() As Byte                        ' �rNO
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)>
        Dim SysKbn() As Byte                    ' �V�X�e���敪
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)>
        Dim Filler() As Byte                    ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Kind(2 - 1)
            ReDim Grade(2 - 1)
            ReDim Kizukbn(2 - 1)
            ReDim kbn(1 - 1)
            ReDim DefectPos(6 - 1)
            ReDim WidPos(4 - 1)
            ReDim Len(4 - 1)
            ReDim No(4 - 1)
            ReDim SysKbn(1 - 1)
            ReDim Filler(4 - 1)
        End Sub
    End Structure

    ' �������я��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=20, pack:=1)> _
    Public Structure FF_MAE_TYPE_BUNKATSU
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim MakiHoukou() As Byte                ' ���ъ�����(1:��o�㌞, 2:��o����, 3:���o�㌞, 4:���o����)
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim CoilLen() As Byte                   ' �R�C����
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim Cutkbn() As Byte                    ' �J�b�g�ꏊ�敪(I:�����J�b�g, O:�o���J�b�g)
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim GenpinNo() As Byte                  ' �������i�Ǘ�NO
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Filler() As Byte                    ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim MakiHoukou(1 - 1)
            ReDim CoilLen(6 - 1)
            ReDim Cutkbn(1 - 1)
            ReDim GenpinNo(10 - 1)
            ReDim Filler(2 - 1)
        End Sub
    End Structure

    ' �ʔ���
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=425, pack:=1)> _
    Public Structure FF_MAE_TYPE_THUHAN
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim JissekiCode() As Byte               ' ���эH���R�[�h
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim MeireiNo() As Byte                  ' ���ߔԍ�
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim ThuhanCnt() As Byte                 ' �ʔ�
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim GenpinNo() As Byte                  ' �������i�Ǘ�NO
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim CoilLen() As Byte                   ' �����R�C������
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Filler() As Byte                    ' �\��
        <VBFixedArray(MAX_FF_MAE_BNKT_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_MAE_BNKT_CNT)> _
        Dim Bu() As FF_MAE_TYPE_BUNKATSU        ' �������

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim JissekiCode(2 - 1)
            ReDim MeireiNo(4 - 1)
            ReDim ThuhanCnt(1 - 1)
            ReDim GenpinNo(10 - 1)
            ReDim CoilLen(6 - 1)
            ReDim Filler(2 - 1)
            ReDim Bu(MAX_FF_MAE_BNKT_CNT - 1)
        End Sub

    End Structure

    ' �O�H���r���`���{��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_MAE_JISSEKI

        Dim Hd As FF_MAE_TYPE_HEAD              ' �w�b�_�[��

        <VBFixedArray(27 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=27)> _
        Dim Yobi1() As Byte                     ' �\��

        <VBFixedArray(MAX_FF_MAE_DINF_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_MAE_DINF_CNT)> _
        Dim Inf() As FF_MAE_TYPE_INF            ' �r���o���

        <VBFixedArray(90 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=90)> _
        Dim Yobi2() As Byte                     ' �\��
        Dim VRcl As FF_MAE_TYPE_VRCLINF         ' VRCL���
        Dim FromTo As FF_MAE_TYPE_FROMTO        ' From-To
        <VBFixedArray(116 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=116)> _
        Dim Yobi3() As Byte                     ' �\��

        <VBFixedArray(MAX_FF_MAE_DEF_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_MAE_DEF_CNT)> _
        Dim def() As FF_MAE_TYPE_DATA           ' �r��� (�w�b�_�[���r���o�H�� �� �X���r���r���o�H�� �� �S������ׂĕR�t����)

        <VBFixedArray(MAX_FF_MAE_THAN_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_MAE_THAN_CNT)> _
        Dim Th() As FF_MAE_TYPE_THUHAN          ' �ʔ��я��
        <VBFixedArray(150 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=150)> _
        Dim Yobi() As Byte                      ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            Hd.initialize()
            ReDim Yobi1(27 - 1)
            ReDim Inf(MAX_FF_MAE_DINF_CNT - 1)
            Inf.Initialize()
            ReDim Yobi2(90 - 1)
            ReDim Yobi3(116 - 1)
            ReDim def(MAX_FF_MAE_DEF_CNT - 1)
            def.Initialize()
            ReDim Th(MAX_FF_MAE_THAN_CNT - 1)
            Th.Initialize()
            ReDim Yobi(150 - 1)
        End Sub

    End Structure

    ' �������ѓ`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=34, pack:=1)> _
    Public Structure FF_STSTYPE_HEAD

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' �`���R�[�h �\:95 ��:96
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' �A��(00) 
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim bytDate() As Byte                   ' �`���������� �`�� YYYYMMDHHNNSS0
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' �r�����C���̍H���R�[�h[	'81	',	'85	',	'54	',	'82(53)	']�ݒ�
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Meino() As Byte                     ' ���ߔԍ�
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim Coilno() As Byte                    ' �≄�R�C���ԍ�

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim bytDate(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Meino(4 - 1)
            ReDim Coilno(10 - 1)
        End Sub


    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=1, pack:=1)> _
    Public Structure FF_KENSA_STATUS

        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim status() As Byte                    ' �������(1m�Ԃ̌�����Ԃ�1byte�Ɋi�[(���30000m))

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim status(1 - 1)
        End Sub


    End Structure

    ' �f�[�^��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=66 + MAX_FF_STS_CNT, Pack:=1)>
    Public Structure FF_STSTYPE_DATA
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim Id() As Byte                        ' �r���Ǘ�No
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)>
        Dim dataSize() As Byte                  ' ������ԃZ�b�g���F������Ԃ��i�[����byte�������E�l�ݒ�(�O����)
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim yobi1() As Byte                      ' �\��1
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)>
        Dim yobi2() As Byte                      ' �\��2
        <VBFixedArray(38 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=38)>
        Dim yobi() As Byte                      ' �\��
        <VBFixedArray(MAX_FF_STS_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_STS_CNT)> _
        Dim data() As FF_KENSA_STATUS           ' �������(1m�Ԃ̌�����Ԃ�1byte�Ɋi�[(���30000m))

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Id(20 - 1)
            ReDim dataSize(5 - 1)
            ReDim yobi1(2 - 1)
            ReDim yobi2(1 - 1)
            ReDim yobi(38 - 1)
            For ii As Integer = 0 To data.Length - 1
                data(ii).initialize()
            Next
        End Sub

    End Structure


    ''FF�{��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_STS_JISSEKI
        Dim Hd As FF_STSTYPE_HEAD               ' �`���w�b�_�[
        Dim Dt As FF_STSTYPE_DATA               ' �f�[�^
        <VBFixedArray(34900 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=34900)>
        Dim Filler2() As Byte                   ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Filler2(34900 - 1)
        End Sub

    End Structure

    ' �w���V�[�`�����w���V�[�����`��
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_HELCY

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' �`���R�[�h (ZZ)
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' �A�� (FF)
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim DateTime() As Byte                  ' �`���������� �`�� YYYYMMDDHHNNSS
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' �H���R�[�h [CTL=A1]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Denkan_No() As Byte                 ' �`�ǃR�}���hNo
        <VBFixedArray(64978 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=64978)> _
        Dim Yobi() As Byte                      ' �\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim DateTime(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Denkan_No(2 - 1)
            ReDim Yobi(64978 - 1)
        End Sub

    End Structure

End Module
