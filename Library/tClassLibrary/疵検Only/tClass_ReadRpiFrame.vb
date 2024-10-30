'*********************************************************************************
' �S���t�@�C���Q��ǂݍ��ރN���X�B�@�P�t���[�����P�t�@�C���̍\�����ARPI�t�@�C���ɋU������
'	[Ver]
'		Ver.01    2014/09/02  ����
'
'	[����]
'		���t���[���܂Ńt�@�C�������邩�́A�g�������ݒ肷��(EffectiveRpiFrameCount)
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.IO						' �t�@�C���A�N�Z�X

Namespace tClass
    ''' <summary>
    '''  �S���t�@�C���Q��ǂݍ��ރN���X�B�@�P�t���[�����P�t�@�C���̍\�����ARPI�t�@�C���ɋU������
    ''' </summary>
    Public Class tClass_ReadRpiFrame
        Inherits tClass_ReadRPI


        Private m_PathFmt As String	' �t���[���ԍ���^����Format����ƁA�t�@�C�����ɂȂ镶����
        Protected m_nCanReadFrameNum As Integer = 0	' �ǂ��܂Ńt�@�C�������邩


        ''' <summary>
        ''' �S���t�@�C���i�P�t���[���P�t�@�C���j�N���X���쐬����
        ''' </summary>
        ''' <param name="pathfmt">�t���[���ԍ���^����Format����ƁA�t�@�C�����ɂȂ镶����</param>
        ''' <param name="rpiheader">RPI�w�b�_</param>
        ''' <param name="trpiheader">T_RPI�w�b�_</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal pathfmt As String, ByVal rpiheader As tClass_ReadRPI.RPI_HEADER, ByVal trpiheader As tClass_ReadRPI.RPI_T_HEADER)
            m_PathFmt = pathfmt
            m_RpiHeader = rpiheader
            m_RpiTHeader = trpiheader
        End Sub


        '************* �p���A��
        ''' <summary>
        ''' RPI�w�b�_��������
        ''' </summary>
        Public Overrides ReadOnly Property GetRpiHeader() As tClass_ReadRPI.RPI_HEADER
            Get
                '�ǂݍ��݉\�Ȑ����������B�w�b�_���̃t���[���������₵�Ă����B
                If m_RpiHeader.nframe < m_nCanReadFrameNum Then
                    m_RpiHeader.nframe = m_nCanReadFrameNum
                End If
                Return m_RpiHeader
            End Get
        End Property

        ''' <summary>
        ''' ���ܓǂ݂��߂����t���[���B
        ''' ���̃t���[���ȍ~�́A�܂��������ł��Ă��Ȃ��B
        ''' </summary>
        ''' <remarks>
        ''' ���̃N���X�́A���̃v���p�e�B�ɂ���āA�t���[���t�@�C���̏����m��
        ''' </remarks>
        Public Overrides Property EffectiveRpiFrameCount() As Integer
            Get
                Return m_nCanReadFrameNum
            End Get
            Set(ByVal value As Integer)
                m_nCanReadFrameNum = value
            End Set
        End Property

        ''' <summary>
        ''' �\���p�̉摜�i���k�j�̃o�C�i�����ǂݍ���
        ''' </summary>
        ''' <param name="fno">�t���[���ԍ��i�P�I���W���j</param>
        ''' <param name="bWk">�ǂݍ��ސ�̃o�C�g�z��B�Ăяo�����͏\���傫�ȃT�C�Y���������Ă�</param>
        Public Overrides Function ReadFile_ImgByte_ActiveCh(ByVal fno As Integer, ByVal bWk() As Byte) As Integer
            Dim frame_file As String = String.Format(m_PathFmt, fno)
            If frame_file = "NUL" Then Return 0

            Using fs As New FileStream(frame_file, FileMode.Open, FileAccess.Read, FileShare.Read)
                Dim nLength As Integer = CInt(fs.Length)
                fs.Read(bWk, 0, nLength)
                Return nLength
            End Using

        End Function

        ''' <summary>
        ''' �\���p�̉摜�i���k�j�̃o�C�i�����ǂݍ��݁A�������B
        ''' </summary>
        ''' <param name="fno">�t���[���ԍ��i�P�I���W���j</param>
        Public Overrides Function ReadFile_ImgByte(ByVal fno As Integer) As Byte()
            Dim frame_file As String = String.Format(m_PathFmt, fno)

            Using fs As New FileStream(frame_file, FileMode.Open, FileAccess.Read, FileShare.Read)
                Dim nLength As Integer = CInt(fs.Length)
                Dim bWk(nLength - 1) As Byte
                fs.Read(bWk, 0, nLength)
                Return bWk
            End Using
        End Function
    End Class




    ''' <summary>
    '''  �S���t�@�C���Q��ǂݍ��ރN���X�B�t�@�C���͎����T���B�P�t���[�����P�t�@�C���̍\�����ARPI�t�@�C���ɋU������.
    ''' </summary>
    ''' <remarks>����I�Ƀt�H���_�����ẮA�t���[���t�@�C���������ĂȂ����`�F�b�N���܂��B(�����t�@�C���͖����O��)</remarks>
    Public Class tClass_ReadRpiFrameAuto
        Inherits tClass_ReadRpiFrame


        Private mcls_WThread As New tClass_WorkThread '�t�@�C�����݃`�F�b�N�X���b�h

        Private m_PathFmt As String	' �t���[���ԍ���^����Format����ƁA�t�@�C�����ɂȂ镶����

        Private m_bInfiniteLoop As Boolean '�t���[�����ǂ��܂ŐL�т邩�킩��Ȃ��B



        ''' <summary>
        ''' �S���t�@�C���i�P�t���[���P�t�@�C���j�N���X���쐬����
        ''' </summary>
        ''' <param name="pathfmt">�t���[���ԍ���^����Format����ƁA�t�@�C�����ɂȂ镶����</param>
        ''' <param name="rpiheader">RPI�w�b�_</param>
        ''' <param name="trpiheader">T_RPI�w�b�_</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal pathfmt As String, ByVal rpiheader As tClass_ReadRPI.RPI_HEADER, ByVal trpiheader As tClass_ReadRPI.RPI_T_HEADER)
            MyBase.New(pathfmt, rpiheader, trpiheader)
            mcls_WThread.ThreadStart(AddressOf mainThread_RpiCreateList, Threading.ThreadPriority.Lowest, "ReadRpiCreatList", True)
        End Sub

        '************* �o�b�N�O���E���h�֘A

        ''' <summary>
        ''' �o�b�N�O���E���h�ŁA�t�@�C�����ǂ��܂łłł��������m����
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ListCreate()
            Do While True ' m_RpiHeader.nframe <> nCanReadFrameNum

                '�����炵���ł����t�@�C���͂��邩�ȁH
                For ii As Integer = Math.Max(1, m_nCanReadFrameNum) To Integer.MaxValue	'm_RpiHeader.nframe
                    Try
                        If File.Exists(String.Format(m_PathFmt, ii)) Then
                            m_nCanReadFrameNum = ii
                        Else
                            Exit For
                        End If
                    Catch iex As IOException

                    End Try
                Next

                '�����S���̃t�@�C����ǂݐ؂������ȁH
                If Not m_bInfiniteLoop Then
                    If m_nCanReadFrameNum >= m_RpiHeader.nframe Then
                        Exit Do
                    End If
                End If

                '���������͎��ʂׂ����ȁH
                If Me.mcls_WThread.g_evStop.WaitOne(1000) Then
                    Exit Do
                End If
            Loop
        End Sub


        ''' <summary>
        ''' �񓯊� RPI���X�g����
        ''' </summary>
        Private Sub mainThread_RpiCreateList()
            Try
                Call ListCreate()
            Catch ex As Threading.ThreadAbortException
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' �o�b�N�O���E���h�X���b�h���E��
        ''' </summary>
        ''' <param name="disposing"></param>
        ''' <remarks></remarks>
        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            MyBase.Dispose(disposing)
            '�o�b�N�O���E���h�œ����Ă���t�@�C�������X���b�h���~�߂�
            mcls_WThread.ThreadStop()
        End Sub


        ''' <summary>
        ''' ���ܓǂ݂��߂����t���[���B
        ''' ���̃t���[���ȍ~�́A�܂��������ł��Ă��Ȃ��B
        ''' </summary>
        ''' <remarks>
        ''' ���̃N���X�́A���̃v���p�e�B�ɂ���āA�t���[���t�@�C���̏����m��
        ''' </remarks>
        Public Overrides Property EffectiveRpiFrameCount() As Integer
            Get
                Return m_nCanReadFrameNum
            End Get
            Set(ByVal value As Integer)
                m_bInfiniteLoop = False	'�O�̂��߁B�T���X���b�h�������Ă�����E��
                m_nCanReadFrameNum = value
            End Set
        End Property
    End Class

End Namespace
