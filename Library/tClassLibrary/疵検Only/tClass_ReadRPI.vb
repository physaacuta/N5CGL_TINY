'*********************************************************************************
'RPI�t�@�C����ǂݍ��ރN���X�B
'	[Ver]
'		Ver.01    2006/03/04  vs2005 �Ή�
'		Ver.02    2014/09/02  ���ۃN���X��
'
'	[����]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.IO						' �t�@�C���A�N�Z�X

Namespace tClass
    Public MustInherit Class tClass_ReadRPI
        Implements IDisposable													' �f�X�g���N�^��`


        '*****************************************************************************************************
        ' ���J�萔�E�\���̒�`


        ''' <summary>
        ''' �T�|�[�g���k�`��
        ''' </summary>
        ''' <remark> �ǋL����ꍇ�́A����GetCompType�ŏo�Ă����l����ŃR�s�[</remark>
        Public Enum EM_COMP_TYPE
            COMP_TYPE_UNKNOWN = 0												'�s��
            COMP_TYPE_RAW = &H52415720											' BMP(BMPͯ�ް��)
            COMP_TYPE_BMP = &H424D5020											' BMP(BMPͯ�ް�L)
            COMP_TYPE_JPEG = &H4A504547											' JPEG
            COMP_TYPE_LAC = &H4C414320                                          ' LAC 
            COMP_TYPE_NUC = &H4E554320                                          ' NUC 
            COMP_TYPE_LAC3 = &H4C414333                                          ' LAC 3
            COMP_TYPE_LAC4 = &H4C414334                                          ' LAC 4
            COMP_TYPE_LAC5 = &H4C414335                                          ' LAC 5
        End Enum

        ''' <summary>
        ''' �����񂩂�A���k�`��ENUM�֕ϊ�����
        ''' </summary>
        ''' <param name="sComp">"JPEG"���̕�����</param>
        ''' <returns>���k�`��</returns>
        Public Shared Function GetCompType(ByVal sComp As String) As EM_COMP_TYPE
            Dim sComp4 As String = Left(sComp + "    ", 4)
            Dim byComp() As Byte = System.Text.Encoding.ASCII.GetBytes(sComp4)
            Array.Reverse(byComp)
            Dim nVal As Integer = BitConverter.ToInt32(byComp, 0)
            Return CType(nVal, EM_COMP_TYPE)
        End Function

        ''' <summary>
        ''' ���k�`��ENUM���A������֕ϊ�����
        ''' </summary>
        ''' <remarks>  �����ɃX�y�[�X�����ꍇ������܂��B </remarks>
        Public Shared Function GetString(ByVal emComp As EM_COMP_TYPE) As String
            Dim byComp() As Byte = BitConverter.GetBytes(CType(emComp, Integer))
            Array.Reverse(byComp)
            Dim sComp As String = System.Text.Encoding.ASCII.GetString(byComp)
            Return sComp
        End Function

        '''' �t���[������`
        Public Enum EM_FRM_TYPE
            EDGE_PER_NON = 0											' ��񖳂�
            EDGE_PER_FRAME = 1											' �t���[���P�ʂ̃G�b�W�ʒu
            EDGE_PER_LINE = 2											' ���胉�C���P�ʂ̃G�b�W�ʒu
        End Enum

        ''' <summary>
        ''' RPI�t�@�C����1�t���[�����
        ''' </summary>
        ''' <remarks></remarks>
        Structure RPI_FRAME_INF
            ''' <summary>�P���ȘA�� (1�`)</summary>
            Dim no As Integer

            ''' <summary>�t�@�C���擪�A�h���X����̊Y���t���[���摜�擪�ʒu�܂ł̑��Έʒu (0�I���W��)</summary>
            Dim img_pos As Long
            ''' <summary>�t���[���摜�̃T�C�Y</summary>
            Dim img_size As Integer

            ''' <summary>�t�@�C���擪�A�h���X����̊Y���t�����擪�ʒu�܂ł̑��Έʒu (0�I���W��)</summary>
            Dim inf_pos As Long
        End Structure


        ''' <summary>
        ''' �t���[���̕t����� (�t���[���P�ʂ̃G�b�W�ʒu)
        ''' </summary>
        ''' <remarks></remarks>
        Structure EDGE_PER_FRAME
            Dim edge_left As Integer									' �G�b�W�������ꍇ�� -1
            Dim edge_right As Integer
        End Structure


        ''' <summary>
        ''' �t���[���̕t����� (���胉�C���P�ʂ̃G�b�W�ʒu)
        ''' </summary>
        ''' <remarks></remarks>
        Structure EDGE_PER_LINE
            Dim edge_left() As Integer									' �G�b�W�������ꍇ�� -1
            Dim edge_right() As Integer
        End Structure


        ''' <summary>
        ''' �e�J�������
        ''' </summary>
        Structure CAM_INFO
            ''' <summary>��ʒu�B���C���Z���^�[����̊e�J�������[�̈ʒu(�������́A�}�C�i�X�l) [mm] (PARAM_CAMSET��Xoffset�l���̂���)</summary>
            Dim dBaseXoffset As Single
            ''' <summary>���f�B������̃t���[�����[���玩�J�����摜���[�܂ł̋���</summary>
            Dim nBaseLeft As Integer
            ''' <summary>���J�����Ƃ̉E���̃I�[�o�[���b�v��</summary>
            Dim nOverLap As Integer
        End Structure


        ''' <summary>
        ''' RPI�`���̃w�b�_�[���
        ''' </summary>
        ''' <remarks></remarks>
        Structure RPI_HEADER
            ''' <summary>�񈳏k�`����=true  ���k�`����=false</summary>
            Dim IsBmp As Boolean
            ''' <summary>����\�L��`���� true</summary>
            Dim IsRes As Boolean
            ''' <summary>�t���[���t�����L��`���� true</summary>
            Dim IsFrm As Boolean


            '''' �ǉ�
            ''' <summary>TEX�w�b�_�[���L��</summary>
            Dim IsTHeader As Boolean
            ''' <summary>���k�`��</summary>
            Dim emCompType As EM_COMP_TYPE


            '' �t�@�C������
            ''' <summary>�^�C�v 4byte</summary>
            Dim type As String
            ''' <summary>�w�b�_�[�T�C�Y</summary>
            Dim header_size As Integer

            ''' <summary>�摜��</summary>
            Dim width As Integer
            ''' <summary>�摜����</summary>
            Dim height As Integer
            ''' <summary>�摜�̃`�����l����(�O���[�摜=1�ARGB�J���[=3�A�Ȃ�)</summary>
            Dim nchannel As Integer
            ''' <summary>1��f�̃r�b�g��(8�Ȃ�)</summary>
            Dim pixel_nbit As Integer
            ''' <summary>�摜���ɑ�������H�H</summary>
            Dim pitch_nbyte As Integer
            ''' <summary>�t���[����</summary>
            Dim nframe As Integer
            ''' <summary>1�摜�̃o�C�g�� (�M���b�v����) (BMP�`���̂�)(�����`�����l���̏ꍇ�́A�S�摜���̃T�C�Y) </summary>
            Dim frame_nbyte As Integer
            ''' <summary>�t�@�C���擪���� �ŏ��̉摜�ʒu</summary>
            Dim frame_start_pos As Integer
            ''' <summary>1�摜�̃o�C�g�� (�M���b�v�L��) (BMP�`���̂�)</summary>
            Dim frame_pitch_nbyte As Integer

            ''' <summary>�t���[�����̎��(0:�����B1:�t���[���P�ʁB2:����P��)</summary>
            Dim frame_info_type As EM_FRM_TYPE

            ''' <summary>������\ [mm/pixel] (�����ꍇ��1�Œ�)</summary>
            Dim res_x As Single
            ''' <summary>�c����\ [mm/pixel] (�����ꍇ��1�Œ�)</summary>
            Dim res_y As Single

        End Structure



        ''' <summary>
        ''' TEX��RPI�`���̃w�b�_�[���
        ''' </summary>
        ''' <remarks></remarks>
        Structure RPI_T_HEADER
            ''' <summary>���̍\���̂̃T�C�Y</summary>
            Dim nTHeaderSize As Integer
            ''' <summary>TEX�w�b�_�[�o�[�W���� (���̂Ƃ���1�Œ�)</summary>
            Dim nTHeaderVer As Integer
            ''' <summary>�V�X�e������</summary>
            Dim cSysName As String
            ''' <summary>�r���Ǘ�No</summary>
            Dim cKizukenNo As String
            ''' <summary>�t���[����񕔂̊i�[�`��  (0:���� 1:T_RPI_FRAME_INFO_T1)</summary>
            Dim frame_info_type As Short
            ''' <summary>�t���[����񕔂̊i�[�T�C�Y</summary>
            Dim frame_info_size As Short
            ''' <summary>�R�C���擪�̃t���[��No</summary>
            Dim nFrameNumInit As Integer
            ''' <summary>���̃t�@�C���擪�̃t���[��No</summary>
            Dim nFrameNumStart As Integer
            ''' <summary>�R�C���P�ʂł̃t�@�C������(�P�R�C���̒��ŁA�t�@�C�������������ɉ��Z�B)(0:�������� 1�`:�����L��Ŋ����ځB1�R�ڂ̂Ƃ���1)</summary>
            Dim nFileNum As Integer
            ''' <summary>���C���Z���^�[��f(�A���摜���[����̃I�t�Z�b�g�l) [pixel]</summary>
            Dim nLineCenter As Integer
            ''' <summary>���[�J�����Z�b�g�ԍ� (1�I���W��)</summary>
            Dim nStartCamset As Integer
            ''' <summary>�������t���[�������� (1�I���W��)</summary>
            Dim nCamsetNum As Integer
            ''' <summary>�J�����p�x (1�I���W��)</summary>
            Dim nCamAngle As Integer
            ''' <summary>�\���敪 (0:�\ 1:��)</summary>
            Dim nMen As Integer
            ''' <summary>�J�������</summary>
            Dim caminfo() As CAM_INFO
            ''' <summary>�擪�t���[������ʒu [mm]</summary>
            Dim nStartLen As Integer
            ''' <summary>�摜�^�C�v (0:�O���[�摜  1:�J���[�摜  2:�F���摜)</summary>
            Dim nColorType As Integer
            ''' <summary>�r���^�C�v (0:�A�����C��  1:�o�b�`���C��)</summary>
            Dim nSysType As Integer
            ''' <summary>���[�ʒu�t���[��No (���̃t�@�C���擪����̃t���[��No (0�I���W��))</summary>
            Dim nFTposCfNo() As Integer
            ''' <summary>���[�ʒu�t���[����pixel (���̃t���[����[����̃s�N�Z���ʒu (0�I���W��))</summary>
            Dim nFTposPixel() As Integer
            ''' <summary>�t�b�^�[���I�t�Z�b�g�ʒu(���̃t�@�C���擪����t�b�^�[�܂ł̈ʒu)</summary>
            Dim nFooterPos As Long
            ''' <summary>�t�b�^�[���i1���R�[�h���j�̃T�C�Y�B����nframe(�W����)</summary>
            Dim nFooterSize As Integer

            Public Sub initialize()
                ReDim caminfo(16 - 1)               ' 20181102 �J�������g�� 8��16
                ReDim nFTposCfNo(2 - 1)
                ReDim nFTposPixel(2 - 1)
            End Sub
        End Structure

        ''' <summary>
        ''' TEX���t���[���t����� �^�C�v�@  128byte�Œ�
        ''' </summary>
        ''' <remarks></remarks>
        Structure T_RPI_FRAME_INFO_T1
            ''' <summary>�t���[��No</summary>
            Dim nFNo As Integer
            ''' <summary>���C�����x [mm/sec]</summary>
            Dim nSpeed As Integer
            ''' <summary>�c����\ [mm/pixel]</summary>
            Dim vRes As Single
            ''' <summary>�R�C����[����̋��� [mm]</summary>
            Dim nFromLen As Integer
            ''' <summary>�G�b�W�ʒu [mm]</summary>
            Dim dEdgePos() As Single
            ''' <summary>���[����̍��E�G�b�WPixel�ʒu [pixel]</summary>
            Dim nEdgeIdx() As Integer
            ''' <summary>�\��</summary>
            Dim nYobi() As Integer

            Public Sub initialize()
                ReDim dEdgePos(2 - 1)
                ReDim nEdgeIdx(2 - 1)
                ReDim nYobi(24 - 1)
            End Sub
        End Structure

        ''' <summary>
        ''' �t�b�^�[��� 
        ''' </summary>
        Structure T_RPI_FOOTER_INFO
            ''' <summary>�t�@�C���擪�A�h���X����̊Y���t���[�����擪�ʒu�܂ł̑��Έʒu (0�I���W��)</summary>
            Dim pos As Long
            ''' <summary>�t���[����񕔂̃T�C�Y</summary>
            Dim framesize As Integer
            ''' <summary>�摜�̃T�C�Y</summary>
            Dim imgsize As Integer
        End Structure


        '*****************************************************************************************************
        ' �����o�E���\�b�h


        Protected m_nActiveCh As Integer										' ���݂̑I���`�����l�� (0�I���W��)
        Protected m_RpiHeader As New RPI_HEADER				' EPC���w�b�_�[
        Protected m_RpiTHeader As New RPI_T_HEADER          ' TEX���w�b�_�[

#Region " IDisposable Support "
        ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
        Public Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' �f�X�g���N�^
        ''' </summary>
        ''' <param name="disposing"></param>
        ''' <remarks>�X���b�h�̒��f�Ƃ��A�t�@�C���̉���Ƃ��͂�����I�[�o�[���C�h���Ď��s����</remarks>
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        End Sub


        ''' <summary>
        ''' RPI�w�b�_��������
        ''' </summary>
        ''' <remarks>��{�I�ɵ��ްײ�ނ���K�v�͂Ȃ��͂��B</remarks>
        Public Overridable ReadOnly Property GetRpiHeader() As RPI_HEADER
            Get
                Return m_RpiHeader
            End Get
        End Property
        '�e�X�g
        Public Property PropRpiHeader() As RPI_HEADER
            Get
                Return m_RpiHeader
            End Get
            Set(value As RPI_HEADER)
                m_RpiHeader = value
            End Set
        End Property


        ''' <summary>
        ''' T�`��RPI�w�b�_��������
        ''' </summary>
        ''' <remarks>��{�I�ɵ��ްײ�ނ���K�v�͂Ȃ��͂��B</remarks>
        Public Overridable ReadOnly Property GetRpiTHeader() As RPI_T_HEADER
            Get
                Return m_RpiTHeader
            End Get
        End Property


        ''' <summary>
        ''' �t���[�����G�b�W����������
        ''' </summary>
        Public Overridable Function GetEdgePerFrame(ByVal fno As Integer) As EDGE_PER_FRAME
            Return Nothing
        End Function

        ''' <summary>
        ''' RPI�t���[������������
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>RPI�t���[�����</returns>
        Public Overridable ReadOnly Property GetRpiFrameInf(ByVal fno As Integer) As RPI_FRAME_INF
            Get
                Dim r As New RPI_FRAME_INF
                r.img_pos = 0
                r.img_size = 0
                r.inf_pos = 0
                r.no = fno
                Return r
            End Get
        End Property


        ''' <summary>
        ''' ���ܓǂ݂��߂����t���[���B
        ''' ���̃t���[���ȍ~�́A�܂��������ł��Ă��Ȃ��B
        ''' </summary>
        ''' <remarks>
        ''' �p����ɂ���ẮA�ݒ肷�邱�Ƃ��\�iReadRpiFrame�Ȃǁj
        ''' 
        ''' </remarks>
        Public MustOverride Property EffectiveRpiFrameCount() As Integer


        ''' <summary>
        ''' ���ܓǂ݂��߂����t���[�����������B
        ''' </summary>
        Public Overridable ReadOnly Property GetRpiFrameCount() As Integer
            Get
                Return EffectiveRpiFrameCount
            End Get
        End Property

        ''' <summary>
        ''' �\���p�̉摜�i���k�j�̃o�C�i�����ǂݍ���
        ''' </summary>
        ''' <param name="fno">�t���[���ԍ��i�P�I���W���j</param>
        ''' <param name="bWk">�ǂݍ��ސ�̃o�C�g�z��B�Ăяo�����͏\���傫�ȃT�C�Y���������Ă�</param>
        Public MustOverride Function ReadFile_ImgByte_ActiveCh(ByVal fno As Integer, ByVal bWk() As Byte) As Integer


        ''' <summary>
        ''' �t���[�����@���iT�`���j��Ԃ�
        ''' </summary>
        ''' <param name="fno">�t���[���ԍ��i�P�I���W���j</param>
        Public Overridable ReadOnly Property GetRpiFrameAdd1(ByVal fno As Integer) As T_RPI_FRAME_INFO_T1
            Get
                Return Nothing
            End Get
        End Property

        ''' <summary>
        ''' �\���p�̉摜�i���k�j�̃o�C�i�����ǂݍ��݁A�������B
        ''' </summary>
        ''' <param name="fno">�t���[���ԍ��i�P�I���W���j</param>
        Public Overridable Function ReadFile_ImgByte(ByVal fno As Integer) As Byte()
            Return Nothing
        End Function


        ''' <summary>
        ''' �����`���l������\�������Ƃ��A�`���l�����w��B
        ''' </summary>
        ''' <remarks>��{�I�ɵ��ްײ�ނ���K�v�͂Ȃ��͂��B</remarks>
        Public Overridable Property PropRpiActiveCh() As Integer
            Get
                Return m_nActiveCh
            End Get
            Set(ByVal value As Integer)
                m_nActiveCh = value
            End Set
        End Property

    End Class
End Namespace
