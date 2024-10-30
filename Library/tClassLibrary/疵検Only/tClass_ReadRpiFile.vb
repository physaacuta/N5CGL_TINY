'*********************************************************************************
'RPI�t�@�C����ǂݍ��ރN���X�B
'	[Ver]
'		Ver.01    2006/03/04  vs2005 �Ή�
'		Ver.02    2014/09/02  ���ۃN���X����p��
'
'	[����]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.IO                       ' �t�@�C���A�N�Z�X
#Const READRPI_FILESTREAM = True

Namespace tClass


    ''' <summary>
    ''' �{����RPI�t�@�C������A�t�@�C����ǂރN���X
    ''' </summary>
    Public Class tClass_ReadRPIFile
        Inherits tClass_ReadRPI							' �f�X�g���N�^��`


        '*****************************************************************************************************
        ' ���J�萔��`
        '''' �w�b�_�[�^�C�v 
        ' EPC����long�f�[�^�Ƃ��ăZ�b�g���Ă���ׁA���]���Ă���
        Public Const RPI_HEAD_TYPE_RWFL As String = "LFWR"				' �t���[����񖳂��`��
        Public Const RPI_HEAD_TYPE_RWFI As String = "IFWR"				' �t���[�����L��`��


        '*****************************************************************************************************
        ' �����o�[�萔��`
        '''' �w�b�_�[�T�C�Y�ɂ��w�b�_�[���
        ' RWFL �`��
        Private Const RPI_HSIZE_BMP As Integer = 44						' BMP���̃w�b�_�[�T�C�Y
        Private Const RPI_HSIZE_BMP_RES As Integer = 52					' BMP(����\�L��)���̃w�b�_�[�T�C�Y
        Private Const RPI_HSIZE_JPG As Integer = 40						' JPG���̃w�b�_�[�T�C�Y
        Private Const RPI_HSIZE_JPG_RES As Integer = 48					' JPG(����\�L��)���̃w�b�_�[�T�C�Y (TEX�w�b�_�[�����邩���m��Ȃ�)

        Private Const RPI_HSIZE_BMP_MULTI_EXP As Integer = 60			' ���d�I�� (�܂��Ƃ肠�����\���ł���悤�ɂ�������)

        ' RWFI
        Private Const RPI_HSIZE_FRM_BMP As Integer = 44					' BMP(�t���[�����L��)���̃w�b�_�[�T�C�Y
        Private Const RPI_HSIZE_FRM_BMP_RES As Integer = 52				' BMP(�t���[�����L��)(����\�L��)���̃w�b�_�[�T�C�Y
        Private Const RPI_HSIZE_FRM_JPG As Integer = 40					' JPG(�t���[�����L��)���̃w�b�_�[�T�C�Y
        Private Const RPI_HSIZE_FRM_JPG_RES As Integer = 48				' JPG(�t���[�����L��)(����\�L��)���̃w�b�_�[�T�C�Y


        ''''''''''''''''
        '' ���ʃw�b�_�[��
        ''''''''''''''''
        Private Const RPI_POS_TYPE_POS As Integer = 0					' �^�C�v �I�t�Z�b�g
        Private Const RPI_POS_TYPE_SIZE As Integer = 4					' �^�C�v 4byte
        Private Const RPI_POS_HEADERSIZE As Integer = 4					' �w�b�_�[�T�C�Y

        '*****************************************************************************************************
        '�O���[�o���ϐ���`


        '*****************************************************************************************************
        '���[�J���ϐ���`
        Private m_IsAccess As Boolean                                       ' RPI�t�@�C���ɃA�N�Z�X�\��
        Private m_RpiFileSize As Long

        ' RPI�t�@�C���̓ǂݍ��ݗp�B
#If READRPI_FILESTREAM Then
        Private m_RpiFileStream As IO.FileStream 
#Else
        Private m_RpiFileStream As Win32AsyncFile
#End If



        Public m_RpiFrameInf As New Dictionary(Of Integer, RPI_FRAME_INF)   ' ���X�g (�t���[��No, �t���[�����)
        Private mcls_WThread As tClass_WorkThread                           ' ���X�g�����p���[�J�[�X���b�h�{��

        ' TEX�p�t���[���t�����
        Private ReadOnly m_RpiFrameAdd1 As New Dictionary(Of Integer, T_RPI_FRAME_INFO_T1) ' �t���[���t�����


        '*****************************************************************************************************
        '�v���p�e�B
        ''' <summary>
        ''' RPI�t���[���A�N�Z�X
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>RPI�t���[�����</returns>
        Public Overrides ReadOnly Property GetRpiFrameInf(ByVal fno As Integer) As RPI_FRAME_INF
            Get
                SyncLock m_RpiFrameInf
                    Return m_RpiFrameInf.Item(fno)
                End SyncLock
            End Get
        End Property
        ''' <summary>
        ''' RPI�t���[���A�N�Z�X (�I���`�����l���l����)
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>RPI�t���[�����</returns>
        Public ReadOnly Property GetRpiFrameInf_ActiveCh(ByVal fno As Integer) As RPI_FRAME_INF
            Get
                SyncLock m_RpiFrameInf
                    Dim wk As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)
                    wk.img_pos = wk.img_pos + wk.img_size * m_nActiveCh
                    Return wk
                End SyncLock
            End Get
        End Property
        ''' <summary>
        ''' RPI �����ς݃t���[�����X�g�J�E���g (�񓯊��Ǎ��݂Ŏg���̂݁H)
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Property EffectiveRpiFrameCount() As Integer
            Get
                SyncLock m_RpiFrameInf
                    Return m_RpiFrameInf.Count
                End SyncLock
            End Get
            Set(ByVal value As Integer)
                ''not supported,.
                Throw New NotSupportedException("tClass_ReadRPIFile�́A�t���[�����ݒ�ł��܂���B")
            End Set
        End Property

        ''' <summary>
        ''' RPI�t�@�C���ɃA�N�Z�X�\��
        ''' </summary>
        ''' <returns>RPI�A�N�Z�X</returns>
        Public ReadOnly Property IsAccess() As Boolean
            Get
                Return m_IsAccess
            End Get
        End Property
        ''' <summary>
        ''' Bitmap�摜(true)�HJpeg�摜(false)�H
        ''' </summary>
        Public ReadOnly Property IsBmp() As Boolean
            Get
                Return m_RpiHeader.IsBmp
            End Get
        End Property
        ''' <summary>
        ''' TEX�w�b�_�[�L��
        ''' </summary>
        Public ReadOnly Property IsTHeader() As Boolean
            Get
                Return m_RpiHeader.IsTHeader
            End Get
        End Property
        ''' <summary>
        ''' ���k�`��
        ''' </summary>
        Public ReadOnly Property IsCompType() As EM_COMP_TYPE
            Get
                Return m_RpiHeader.emCompType
            End Get
        End Property


        ' �t���[���t����� ===========================================
        ''' <summary>
        ''' RPI�t���[���t�����A�N�Z�X.�L���b�V���ɂȂ���΃t�@�C������ǂ�
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>RPI�t���[���t�����</returns>
        ''' <remarks> �S���FrameAdd���b�N���Ƃ�</remarks>
        Public Overrides ReadOnly Property GetRpiFrameAdd1(ByVal fno As Integer) As T_RPI_FRAME_INFO_T1
            Get
                Dim wk As New T_RPI_FRAME_INFO_T1
                Try
                    SyncLock m_RpiFrameAdd1
                        If Not m_RpiFrameAdd1.TryGetValue(fno, wk) Then
                            If ReadFile_FrameInfo_T1_Async(fno, AddressOf OnFinishRead_FrameInfo_T1) Then
                                '�Ƃ肠�����A�_�~�[�f�[�^�����߂Ă����AReadFile���������s����Ȃ��悤�ɂ���
                                wk.nFNo = -fno
                                m_RpiFrameAdd1.Add(fno, wk)
                            End If
                        End If
                    End SyncLock
                Catch ex As Exception
                End Try
                Return wk
            End Get
        End Property


        ''' <summary>
        ''' �t�@�C������ǂݍ��߂�(�����ʒm�j
        ''' </summary>
        ''' <param name="ms">������</param>
        ''' <param name="read">�ǂݍ��񂾃o�C�g���B</param>
        ''' <param name="o">fno. �ǂݍ��񂾃t���[��NO�iRPI���΁j</param>
        Private Sub OnFinishRead_FrameInfo_T1(ms As MemoryStream, read As Integer, o As Object)
            Dim wk As T_RPI_FRAME_INFO_T1 = Parse_FrameInfo_T1(ms)
            Dim fno As Integer = CInt(o)

            '��ŗp�ӂ����_�~�[���㏑���B
            SyncLock m_RpiFrameAdd1
                '����if��RpiClose�����OnFinish�������Ƃ��p
                If m_RpiFrameInf.Count <> 0 Then
                    m_RpiFrameAdd1(fno) = wk
                End If
            End SyncLock
        End Sub

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()

        End Sub

        ' IDisposable
        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            Call RpiClose()
        End Sub


        ''' <summary>
        ''' RPI�t�@�C�� �A�N�Z�X�\���`�F�b�N
        ''' </summary>
        ''' <param name="strFPath">RPI�t�@�C���̃t���p�X</param>
        Public Shared Function RpiOpenCheck(ByVal strFPath As String) As Boolean
            '''' RPI�t�@�C���̎��O�m�F
            If String.IsNullOrEmpty(strFPath) Then
                Return False
            End If
            ' RPI�t�@�C���̑��݊m�F
            Try
                If Not IO.File.Exists(strFPath) Then Return False
            Catch ex As Exception
                ' �����̃t�@�C������NULL�������ꍇ�́A�G���[�ƂȂ��
                Return False
            End Try

            '''''' �t�@�C���I�[�v�� �i�Ǎ���p�j
            Try

                Using fs As IO.FileStream = New FileStream(strFPath, FileMode.Open, FileAccess.Read, FileShare.Delete Or FileShare.Read)

                    ' �^�C�v���� �ǂݍ���
                    Dim wk(4 - 1) As Byte
                    fs.Position = 0
                    fs.Read(wk, 0, 4)
                    Dim type As String = tMod.tMod_Common.ByteToString(wk)
                    Return RPI_HEAD_TYPE_RWFL = type Or RPI_HEAD_TYPE_RWFI = type
                End Using

            Catch ex As Exception
                Return False
            End Try

        End Function


        Public Const READ_FROM_RPI_FILE As String = "[READ_RPI_FILE]"

        ''' <summary>
        ''' RPI�t�@�C�� �A�N�Z�X�J�n	
        ''' </summary>
        ''' <param name="strFPath">RPI�t�@�C���̃t���p�X</param>
        ''' <param name="bThreadRead">RPI�t�@�C���̔񓯊��Ǎ���</param>
        ''' <param name="sKanriNo">�㏑���p�����ԍ��i�K���RPI�t�@�C������j</param>
        ''' <returns>0�`�摜���� -1�`:�I�[�v�����s</returns>
        ''' <remarks></remarks>
        Public Function RpiOpen(ByVal strFPath As String, Optional ByVal bThreadRead As Boolean = False, Optional ByVal sKanriNo As String = READ_FROM_RPI_FILE) As Integer
            '''''' ����������Ă��邩�`�F�b�N
            If 0 <> m_RpiFrameInf.Count Then
                Return -9
            End If

            '''''' �t�@�C���I�[�v�� �i�Ǎ���p�j
            Try
#If READRPI_FILESTREAM Then
                'RpiViewer�̓ǂݕ��ł́AFileStream�̃o�b�t�@�����ɗ����Ƃ͏��Ȃ��̂ŁA���������Ă����B
                m_RpiFileStream = New FileStream(strFPath, FileMode.Open, FileAccess.Read, FileShare.Delete Or FileShare.Read, 32, True)
                m_RpiFileSize = m_RpiFileStream.Length
#Else
                m_RpiFileStream = Win32AsyncFile.OpenForRead(strFPath)
                m_RpiFileSize = m_RpiFileStream.GetFileSize
#End If
            Catch ex As Exception

                Return -1
            End Try


            '''''' RPI�t�@�C������w�b�_�[���擾
            If Not Read_Header() Then
                '' �N���[�Y�������鎖
                RpiClose()
                Return -2
            End If

            ''''''����NO�̏㏑��
            If sKanriNo <> READ_FROM_RPI_FILE Then
                m_RpiTHeader.cKizukenNo = sKanriNo
            End If


            '''''' �X�̉摜���̃C���f�b�N�X���쐬
            If Not bThreadRead Then
                Dim nRetc As Integer = ListCreate()

                If 0 <> nRetc Then Return nRetc
            Else
                '' �X���b�h�Ŕ񓯊�����
                ThreadStart_RpiCreateList(Threading.ThreadPriority.Lowest)
            End If


            ''''''' �����܂ŗ����� �t�@�C���A�N�Z�X�\
            m_IsAccess = True
            Return m_RpiHeader.nframe
        End Function


        ''' <summary>
        ''' RPI�t�@�C�� �N���[�Y
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub RpiClose()
            '' �`�F�b�N
            If m_RpiFileStream Is Nothing Then
                Return
            End If

            '' ���X�g�񓯊������X���b�h��~
            ThreadStop_RpiCreateList()

            '' ���X�g �J��
            m_RpiFrameInf.Clear()
            SyncLock m_RpiFrameAdd1
                m_RpiFrameAdd1.Clear()
            End SyncLock

            '' �t�@�C�� �N���[�Y
            m_RpiFileStream.Close()
            m_RpiFileStream = Nothing

            m_IsAccess = False

        End Sub


        '*****************************************************************************************************
        ' �w�b�_�[����


        ''' <summary>
        ''' RPI�t�@�C������w�b�_�[�����擾����
        ''' </summary>
        ''' <remarks></remarks>
        Private Function Read_Header() As Boolean
            ' �܂��̓w�b�_�[����������
            m_RpiHeader = Nothing

            Dim br As New BinaryReaderSP(ReadFile_Raw(0, 2048))

            '' ���ʃw�b�_�[�̕������擾
            Dim type As String = br.ReadString(RPI_POS_TYPE_SIZE)
            Dim header_size As Integer = br.ReadInt32()
            m_RpiHeader.type = type
            m_RpiHeader.header_size = header_size

            ' ================================================
            '' �^�C�v�̔���
            If RPI_HEAD_TYPE_RWFL = type Then                   ' �t���[����񖳂��`��
                m_RpiHeader.IsFrm = False

                Select Case header_size
                    Case RPI_HSIZE_BMP
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = False
                    Case RPI_HSIZE_BMP_RES
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = True
                    Case RPI_HSIZE_JPG
                        m_RpiHeader.IsBmp = False
                        m_RpiHeader.IsRes = False
                    Case RPI_HSIZE_JPG_RES
                        m_RpiHeader.IsBmp = False
                        m_RpiHeader.IsRes = True
                    Case RPI_HSIZE_BMP_MULTI_EXP
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = True

                    Case Else
                        Return False
                End Select

            ElseIf RPI_HEAD_TYPE_RWFI = type Then               ' �t���[�����L��`��
                m_RpiHeader.IsFrm = True

                Select Case header_size
                    Case RPI_HSIZE_FRM_BMP
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = False
                    Case RPI_HSIZE_FRM_BMP_RES
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = True
                    Case RPI_HSIZE_FRM_JPG
                        m_RpiHeader.IsBmp = False
                        m_RpiHeader.IsRes = False
                    Case RPI_HSIZE_FRM_JPG_RES
                        m_RpiHeader.IsBmp = False
                        m_RpiHeader.IsRes = True
                    Case Else
                        Return False
                End Select
            Else
                Return False
            End If

            ' ================================================
            '' TEX�g���w�b�_�[�̗L�����`�F�b�N
            br.Position = m_RpiHeader.header_size
            Dim nTHeadPos As Integer = br.ReadInt32()      ' TEX���w�b�_�[�I�t�Z�b�g�ʒu (EPC�w�b�_�[�̎��Œ�)

            ' �b��΍�
            If 2048 < nTHeadPos Then nTHeadPos = 0 ' EPC�w�b�_�[����Ƀf�[�^�L��

            If RPI_HEAD_TYPE_RWFL = type And m_RpiHeader.IsRes Then
                If 0 <> nTHeadPos Then
                    '' TEX���w�b�_�[�A��
                    m_RpiHeader.IsTHeader = True

                    '' TEX���w�b�_�[�ǂݍ���
                    m_RpiTHeader.initialize()
                    br.Position = nTHeadPos

                    m_RpiTHeader.nTHeaderSize = br.ReadInt32
                    m_RpiTHeader.nTHeaderVer = br.ReadInt32
                    m_RpiTHeader.cSysName = br.ReadString(20)
                    m_RpiTHeader.cKizukenNo = br.ReadString(20)
                    m_RpiTHeader.frame_info_type = br.ReadInt16
                    m_RpiTHeader.frame_info_size = br.ReadInt16
                    m_RpiTHeader.nFrameNumInit = br.ReadInt32
                    m_RpiTHeader.nFrameNumStart = br.ReadInt32
                    m_RpiTHeader.nFileNum = br.ReadInt32
                    m_RpiTHeader.nLineCenter = br.ReadInt32
                    m_RpiTHeader.nStartCamset = br.ReadInt32
                    m_RpiTHeader.nCamsetNum = br.ReadInt32
                    m_RpiTHeader.nCamAngle = br.ReadInt32
                    m_RpiTHeader.nMen = br.ReadInt32
                    For ii As Integer = 0 To m_RpiTHeader.nCamsetNum - 1
                        m_RpiTHeader.caminfo(ii).dBaseXoffset = br.ReadSingle()
                        m_RpiTHeader.caminfo(ii).nBaseLeft = br.ReadInt32()
                        m_RpiTHeader.caminfo(ii).nOverLap = br.ReadInt32()
                    Next
                    'br.Position = nTHeadPos + 180
                    If m_RpiTHeader.nTHeaderVer = 1 Then
                        br.Position = nTHeadPos + 180
                    Else
                        br.Position = nTHeadPos + 276
                    End If
                    m_RpiTHeader.nStartLen = br.ReadInt32
                    m_RpiTHeader.nColorType = br.ReadInt32
                    m_RpiTHeader.nSysType = br.ReadInt32
                    ' ���[�ʒu
                    m_RpiTHeader.nFTposCfNo(0) = br.ReadInt32()
                    m_RpiTHeader.nFTposCfNo(1) = br.ReadInt32()
                    m_RpiTHeader.nFTposPixel(0) = br.ReadInt32()
                    m_RpiTHeader.nFTposPixel(1) = br.ReadInt32()
                    ' �t�b�^�[�ʒu
                    'br.Position = nTHeadPos + 208
                    If m_RpiTHeader.nTHeaderVer = 1 Then
                        br.Position = nTHeadPos + 208
                    Else
                        br.Position = nTHeadPos + 304
                    End If
                    m_RpiTHeader.nFooterPos = br.ReadInt64()
                    m_RpiTHeader.nFooterSize = br.ReadInt32()

                End If
            End If

            ' ================================================
            '''' ���k�`������
            ' �W���w�b�_�[�ł��R�[�f�b�N������

            br.Position = 8
            If False = m_RpiHeader.IsTHeader And m_RpiHeader.IsBmp Then         ' ��{�^�C�v
                m_RpiHeader.emCompType = EM_COMP_TYPE.COMP_TYPE_RAW
            Else                                                ' �w�b�_�[�Ɉ��k�`���̋L�ڗL��

                ' ���k�`���̏ꍇ�́A���k�^�C�v���`�F�b�N
                Dim nCompType As Integer = br.ReadInt32
                m_RpiHeader.emCompType = CType(nCompType, EM_COMP_TYPE)  ' ���k�`������

            End If


            m_RpiHeader.width = br.ReadInt32                    ' �摜��
            m_RpiHeader.height = br.ReadInt32                ' �摜����
            m_RpiHeader.nchannel = br.ReadInt32         ' �摜�̃`�����l����(�O���[�摜=1�ARGB�J���[=3�A�Ȃ�)
            m_RpiHeader.pixel_nbit = br.ReadInt32           ' 1��f�̃r�b�g��(8�Ȃ�)
            m_RpiHeader.pitch_nbyte = br.ReadInt32          ' �摜���ɑ�������H�H��
            m_RpiHeader.nframe = br.ReadInt32               ' �t���[����

            ' ================================================
            '' �w�b�_�[���擾
            ' ���܂��낵���Ȃ����A����̊g���ׁ̈A�ׂ��ł��E���ł� �ɂ��Ă�����
            If True = m_RpiHeader.IsBmp And False = m_RpiHeader.IsFrm Then          ' �񈳏kRPI (�t���[����񖳂�)
                m_RpiHeader.frame_nbyte = br.ReadInt32          ' 1�摜�̃o�C�g�� (�M���b�v����) (BMP�`���̂�)
                m_RpiHeader.frame_start_pos = br.ReadInt32      ' �t�@�C���擪���� �ŏ��̉摜�ʒu
                m_RpiHeader.frame_pitch_nbyte = br.ReadInt32    ' 1�摜�̃o�C�g�� (�M���b�v�L��) (BMP�`���̂�)



                ' �s�v���ڏ�����
                m_RpiHeader.frame_info_type = EM_FRM_TYPE.EDGE_PER_NON


            ElseIf False = m_RpiHeader.IsBmp And False = m_RpiHeader.IsFrm Then     ' ���kRPI (�t���[����񖳂�) �� TEX��RPI�͂���̂�
                m_RpiHeader.frame_start_pos = br.ReadInt32         ' �t�@�C���擪���� �ŏ��̉摜�ʒu

                ' �s�v���ڏ�����
                m_RpiHeader.frame_nbyte = -1
                m_RpiHeader.frame_pitch_nbyte = -1
                m_RpiHeader.frame_info_type = EM_FRM_TYPE.EDGE_PER_NON


                ' TEX�w�b�_�[�ł킩�����
                If m_RpiHeader.IsTHeader Then
                    If EM_COMP_TYPE.COMP_TYPE_RAW = m_RpiHeader.emCompType Or EM_COMP_TYPE.COMP_TYPE_BMP = m_RpiHeader.emCompType Then
                        m_RpiHeader.frame_nbyte = m_RpiHeader.width * m_RpiHeader.height * m_RpiHeader.nchannel
                        If EM_COMP_TYPE.COMP_TYPE_BMP = m_RpiHeader.emCompType Then m_RpiHeader.frame_nbyte += 1078
                        m_RpiHeader.frame_pitch_nbyte = tMod.GET_SIZE_JUST(12 + m_RpiHeader.frame_nbyte + m_RpiTHeader.frame_info_size, 512)
                    End If
                End If

            ElseIf True = m_RpiHeader.IsBmp And True = m_RpiHeader.IsFrm Then       ' �񈳏kRPI (�t���[�����L��)

                m_RpiHeader.frame_nbyte = br.ReadInt32()          ' 1�摜�̃o�C�g�� (�M���b�v����) (BMP�`���̂�)
                m_RpiHeader.frame_start_pos = br.ReadInt32()      ' �t�@�C���擪���� �ŏ��̉摜�ʒu
                m_RpiHeader.frame_info_type = CType(br.ReadInt32(), EM_FRM_TYPE) ' �t���[�����̎��

                ' �s�v���ڏ�����
                m_RpiHeader.frame_pitch_nbyte = -1


            ElseIf False = m_RpiHeader.IsBmp And True = m_RpiHeader.IsFrm Then      ' ���kRPI (�t���[�����L��)
                m_RpiHeader.frame_start_pos = br.ReadInt32()      ' �t�@�C���擪���� �ŏ��̉摜�ʒu
                m_RpiHeader.frame_info_type = CType(br.ReadInt32(), EM_FRM_TYPE) ' �t���[�����̎��


                ' �s�v���ڏ�����
                m_RpiHeader.pitch_nbyte = m_RpiHeader.width
                m_RpiHeader.frame_nbyte = -1
                m_RpiHeader.frame_pitch_nbyte = -1
                m_RpiHeader.frame_info_type = EM_FRM_TYPE.EDGE_PER_NON

            End If

            If m_RpiHeader.IsRes Then
                m_RpiHeader.res_x = br.ReadSingle       ' ������\ [mm/pixel]
                m_RpiHeader.res_y = br.ReadSingle           ' �c����\ [mm/pixel]
            Else
                m_RpiHeader.res_x = 1
                m_RpiHeader.res_y = 1
            End If
            'ReadFile_Int(m_RpiFile.Length - 4)
            'MsgBox("")
            Return True
        End Function


        '*****************************************************************************************************
        ' ���X�g����

        Function CanCreateListImmidiately() As Boolean
            If m_RpiHeader.IsTHeader Then
                ' �t�b�^�[����C���f�b�N�X�𐶐��\
                If 0 <> m_RpiTHeader.nFooterPos AndAlso 0 <> m_RpiTHeader.nFooterSize Then
                    Return True
                End If
            End If
            If Not m_RpiHeader.IsFrm And m_RpiHeader.IsBmp Then
                '' �t���[���t����񖳂�& �񈳏k (bmp)
                Return True
            End If
            Return False
        End Function

        ''' <summary>
        ''' ���X�g����
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function ListCreate() As Integer
            ' ================================================
            ' TEX���w�b�_�[�̂�
            If m_RpiHeader.IsTHeader Then
                If m_RpiHeader.IsFrm Then Return -99 ' �T�|�[�g�O
                'If m_RpiHeader.IsBmp Then Return -98	' BMP ���T�|�[�g�����I

                ' �t�b�^�[����C���f�b�N�X�𐶐��\
                If 0 <> m_RpiTHeader.nFooterPos AndAlso 0 <> m_RpiTHeader.nFooterSize Then
                    If ListCreate_Footer(CBool(m_RpiTHeader.frame_info_type)) Then
                        Return 0
                    Else
                        Return -19
                    End If
                End If



                If EM_COMP_TYPE.COMP_TYPE_RAW = m_RpiHeader.emCompType Or EM_COMP_TYPE.COMP_TYPE_BMP = m_RpiHeader.emCompType Then
                    '------------------------
                    ' �񈳏k�`����
                    If 0 = m_RpiTHeader.frame_info_type Then
                        If Not ListCreate_RAW() Then                    ' TEX���t���[����񖳂�
                            Return -10
                        End If
                    ElseIf 1 = m_RpiTHeader.frame_info_type Then
                        If Not ListCreate_RAW_AddFrmInfoRead_T1() Then  ' TEX���t���[���t�����P �����Ǎ���
                            Return -12
                        End If
                    Else
                        Return -90
                    End If
                Else
                    '------------------------
                    ' ���k�`����
                    If 0 = m_RpiTHeader.frame_info_type Then
                        If Not ListCreate_JPG() Then                    ' �t���[����񖳂�
                            Return -11
                        End If
                    ElseIf 1 = m_RpiTHeader.frame_info_type Then
                        If Not ListCreate_JPG_AddFrmInfoRead_T1() Then  ' TEX���t���[���t�����P �����Ǎ���
                            Return -13
                        End If
                    Else
                        Return -90
                    End If
                End If

                Return 0
            End If


            ' ================================================
            ' ��{
            If Not m_RpiHeader.IsFrm Then					'' �t���[���t����񖳂�
                If m_RpiHeader.IsBmp Then							'' �񈳏k (bmp)
                    If Not ListCreate_BMP() Then
                        Return -10
                    End If

                Else
                    If Not ListCreate_JPG() Then                    '' ���k (jpeg, lac)
                        Return -11
                    End If
                End If

            Else											'' �t���[���t�����L��
                If m_RpiHeader.IsBmp Then							'' �񈳏k (bmp)
                    If Not ListCreate_BMP_AddFrmInfo() Then
                        Return -12
                    End If

                Else
                    If Not ListCreate_JPG_AddFrmInfo() Then         '' ���k (jpeg, lac)
                        Return -13
                    End If
                End If
            End If
            Return 0
        End Function

        ''' <summary>
        ''' BMP�`���� ���X�g���� (�t���[����񖳂�)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_BMP() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos       ' �ŏ��̉摜�ʒu

            SyncLock m_RpiFrameInf
                For ii As Integer = 1 To m_RpiHeader.nframe
                    ' �f�[�^����
                    Dim inf As new RPI_FRAME_INF
                    inf.no = ii
                    inf.img_pos = pos                               ' �t�@�C���擪����摜�܂ł̃I�t�Z�b�g
                    inf.img_size = CInt(m_RpiHeader.frame_nbyte / m_RpiHeader.nchannel)    ' �摜�T�C�Y
                    inf.inf_pos = 0                                 ' �t����񖳂��ׁ̈A0�Œ�

                    ' �f�[�^�Z�b�g
                    m_RpiFrameInf.Add(ii, inf)

                    ' ����
                    pos += m_RpiHeader.frame_pitch_nbyte

                    ' �Œ���̃T�C�Y�`�F�b�N
                    If pos > m_RpiFileSize Then
                        Return False
                    End If
                Next ii
            End SyncLock

            Return True
        End Function

        ''' <summary>
        ''' BMP�`���� ���X�g���� (�t���[�����L��)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_BMP_AddFrmInfo() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' �ŏ��̉摜�ʒu

            For ii As Integer = 1 To m_RpiHeader.nframe
                Dim br As New BinaryReaderSP(ReadFile_Raw(pos, 8))
                ' �f�[�^����
                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 8   ' (8=next=bmp_image)    ' �t�@�C���擪����摜�܂ł̃I�t�Z�b�g
                inf.img_size = CInt(m_RpiHeader.frame_nbyte / m_RpiHeader.nchannel)       '  �摜�T�C�Y
                inf.inf_pos = inf.img_pos + m_RpiHeader.frame_nbyte     ' �t�����̃I�t�Z�b�g�B(�摜�̌��ɏ�񂪂��Ă���)

                ' �f�[�^�Z�b�g
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                ' ����
                pos = br.ReadInt64

                ' �Œ���̃T�C�Y�`�F�b�N
                If pos > m_RpiFileSize Then
                    Return False
                End If
                ' �X���b�h����~�H
                If TreadStopCheck_RpiCreatList() Then Return True
            Next ii

            Return True
        End Function

        ''' <summary>
        ''' ���k�`���� ���X�g���� (�t���[����񖳂�)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_JPG() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' �ŏ��̉摜�ʒu (�e�摜�f�[�^��size�ʒu�������悤�ɂ���)
            Dim nFileLength As Long = m_RpiFileSize

            For ii As Integer = 1 To m_RpiHeader.nframe
                Dim br As New BinaryReaderSP(ReadFile_Raw(pos, 16))
                ' �f�[�^����
                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 12   ' (12=size+next=jpeg_image)	' �t�@�C���擪����摜�܂ł̃I�t�Z�b�g
                inf.img_size = br.ReadInt32 ' �摜�T�C�Y
                inf.inf_pos = 0                                 ' �t����񖳂��ׁ̈A0�Œ�

                ' �f�[�^�Z�b�g
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                ' ����
                pos = br.ReadInt64

                ' �Œ���̃T�C�Y�`�F�b�N (�Ō�̃t���[�����̓`�F�b�N���Ȃ�)
                If ii <> m_RpiHeader.nframe And pos >= nFileLength Then
                    ''��
                    'm_RpiHeader.nframe = ii
                    'Return True

                    Return False
                End If

                ' �X���b�h����~�H
                If TreadStopCheck_RpiCreatList() Then Return True
            Next ii

            Return True
        End Function

        ''' <summary>
        ''' JPG�`���� ���X�g���� (�t���[�����L��)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_JPG_AddFrmInfo() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' �ŏ��̉摜�ʒu (�e�摜�f�[�^��size�ʒu�������悤�ɂ���)

            For ii As Integer = 1 To m_RpiHeader.nframe
                ' �f�[�^����
                Dim br As New BinaryReaderSP(ReadFile_Raw(pos, 16))

                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 12   ' (12=size+next=jpeg_image)	' �t�@�C���擪����摜�܂ł̃I�t�Z�b�g
                inf.img_size = br.ReadInt32 ' �摜�T�C�Y
                inf.inf_pos = inf.img_pos + inf.img_size                ' �t�����̃I�t�Z�b�g�B(�摜�̌��ɏ�񂪂��Ă���)


                ' �f�[�^�Z�b�g
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                ' ����
                pos = br.ReadInt64

                ' �Œ���̃T�C�Y�`�F�b�N (�Ō�̃t���[�����̓`�F�b�N���Ȃ�)
                If ii <> m_RpiHeader.nframe And pos >= m_RpiFileSize Then
                    Return False
                End If

                ' �X���b�h����~�H
                If TreadStopCheck_RpiCreatList() Then Return True
            Next ii

            Return True
        End Function


        ''' <summary>
        ''' ���k�`���� ���X�g���� (�t���[�����L��:�����Ǎ�TEX�`��T1) TEX�w�b�_�[�̏ꍇ�̂�
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_JPG_AddFrmInfoRead_T1() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' �ŏ��̉摜�ʒu (�e�摜�f�[�^��size�ʒu�������悤�ɂ���)

            For ii As Integer = 1 To m_RpiHeader.nframe
                ' �f�[�^����
                Dim br As New BinaryReaderSP(ReadFile_Raw(pos, 16))

                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 12   ' (12=size+next=jpeg_image)	' �t�@�C���擪����摜�܂ł̃I�t�Z�b�g
                inf.img_size = br.ReadInt32 ' �摜�T�C�Y
                inf.inf_pos = inf.img_pos + inf.img_size                ' �t�����̃I�t�Z�b�g�B(�摜�̌��ɏ�񂪂��Ă���)


                ' �f�[�^�Z�b�g
                ' �t���[�����Ǎ��� & �Z�b�g
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                SyncLock m_RpiFrameAdd1
                    If Not m_RpiFrameAdd1.ContainsKey(ii) Then
                        m_RpiFrameAdd1.Add(ii, ReadFile_FrameInfo_T1(ii))
                    End If
                End SyncLock


                ' ����
                pos = br.ReadInt64

                ' �Œ���̃T�C�Y�`�F�b�N (�Ō�̃t���[�����̓`�F�b�N���Ȃ�)
                If ii <> m_RpiHeader.nframe And pos >= m_RpiFileSize Then
                    Return False
                End If

                ' �X���b�h����~�H
                If TreadStopCheck_RpiCreatList() Then Return True
            Next ii

            Return True
        End Function


        ''' <summary>
        ''' �񈳏k�`���� ���X�g���� (�t���[����񖳂�)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_RAW() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' �ŏ��̉摜�ʒu
            SyncLock m_RpiFrameInf

                For ii As Integer = 1 To m_RpiHeader.nframe
                    ' �f�[�^����
                    Dim inf As RPI_FRAME_INF
                    inf.no = ii
                    inf.img_pos = pos + 12       ' (12=size+next=jpeg_image)			' �t�@�C���擪����摜�܂ł̃I�t�Z�b�g
                    inf.img_size = CInt(m_RpiHeader.frame_nbyte / m_RpiHeader.nchannel)    ' �摜�T�C�Y
                    inf.inf_pos = 0                                 ' �t����񖳂��ׁ̈A0�Œ�

                    ' �f�[�^�Z�b�g
                    m_RpiFrameInf.Add(ii, inf)

                    ' ����
                    pos += m_RpiHeader.frame_pitch_nbyte

                    ' �Œ���̃T�C�Y�`�F�b�N
                    If pos > m_RpiFileSize Then
                        Return False
                    End If
                Next ii
            End SyncLock


            Return True
        End Function

        ''' <summary>
        ''' �񈳏k�`���� ���X�g���� (�t���[�����L��:�����Ǎ�TEX�`��T1) TEX�w�b�_�[�̏ꍇ�̂�
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_RAW_AddFrmInfoRead_T1() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' �ŏ��̉摜�ʒu

            For ii As Integer = 1 To m_RpiHeader.nframe
                ' �f�[�^����
                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 12   ' (12=size+next=jpeg_image)	' �t�@�C���擪����摜�܂ł̃I�t�Z�b�g
                inf.img_size = CInt(m_RpiHeader.frame_nbyte / m_RpiHeader.nchannel)       '  �摜�T�C�Y
                inf.inf_pos = inf.img_pos + m_RpiHeader.frame_nbyte     ' �t�����̃I�t�Z�b�g�B(�摜�̌��ɏ�񂪂��Ă���)


                ' �f�[�^�Z�b�g
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                ' �t���[�����Ǎ��� & �Z�b�g
                Try
                    SyncLock m_RpiFrameAdd1
                        If Not m_RpiFrameAdd1.ContainsKey(ii) Then
                            m_RpiFrameAdd1.Add(ii, ReadFile_FrameInfo_T1(ii))
                        End If
                    End SyncLock
                Catch ex As Exception
                End Try

                ' ���� (512�P��)
                pos += m_RpiHeader.frame_pitch_nbyte

                ' �Œ���̃T�C�Y�`�F�b�N
                If pos > m_RpiFileSize Then
                    Return False
                End If
            Next ii

            Return True
        End Function

        '*****************************************************************************************************
        ' �t�b�^�[����C���f�b�N�X����

        ''' <summary>
        ''' �t�b�^�[����C���f�b�N�X����
        ''' </summary>
        ''' <param name="bAddInf"></param>
        Private Function ListCreate_Footer(ByVal bAddInf As Boolean) As Boolean
            Dim br As New BinaryReaderSP(ReadFile_Raw(m_RpiTHeader.nFooterPos, m_RpiHeader.nframe * m_RpiTHeader.nFooterSize))

            '�����ɂ����͂��Ȃ̂ŁA�L�͈͂�LOCK����
            SyncLock m_RpiFrameInf
                For ii As Integer = 1 To m_RpiHeader.nframe
                    Dim wk As New T_RPI_FOOTER_INFO
                    br.Position = m_RpiTHeader.nFooterSize * (ii - 1)

                    wk.pos = br.ReadInt64
                    wk.framesize = br.ReadInt32
                    wk.imgsize = br.ReadInt32


                    ' �f�[�^����
                    Dim inf As RPI_FRAME_INF
                    inf.no = ii
                    inf.img_pos = 12 + wk.pos   ' �t�@�C���擪����摜�܂ł̃I�t�Z�b�g
                    inf.img_size = wk.imgsize      '  �摜�T�C�Y
                    If bAddInf Then
                        inf.inf_pos = inf.img_pos + inf.img_size        ' �t�����̃I�t�Z�b�g�B(�摜�̌��ɏ�񂪂��Ă���)
                    End If

                    ' �f�[�^�Z�b�g
                    m_RpiFrameInf.Add(ii, inf)
                Next
            End SyncLock
            Return True
        End Function





        '*****************************************************************************************************
        ' �t�@�C���Ǎ��֌W(��{�E�{�̌n)

        ''' <summary>
        ''' �\���p�̉摜�i���k�j�̃o�C�i�����ǂݍ���
        ''' </summary>
        ''' <param name="fno">�t���[���ԍ��i�P�I���W���j</param>
        ''' <param name="bWk">�ǂݍ��ސ�̃o�C�g�z��B�Ăяo�����͏\���傫�ȃT�C�Y���������Ă�</param>
        Public Overrides Function ReadFile_ImgByte_ActiveCh(ByVal fno As Integer, ByVal bWk() As Byte) As Integer
            'Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)
            Dim inf As RPI_FRAME_INF = GetRpiFrameInf_ActiveCh(fno)
            ' �ǂݍ��݈ʒu�`�F�b�N, �t���[����
            If inf.img_pos >= m_RpiFileSize Or fno > m_RpiHeader.nframe Then
                Return 0
            End If

            ' �摜�擾

            Return doReadFile_Byte(inf.img_pos, bWk, inf.img_size)
        End Function


        ''' <summary>
        ''' RPI�t�@�C�� ���� �t���[���摜 ��ǂݍ���
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>�o�C�g�f�[�^</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_Img(ByVal fno As Integer) As Drawing.Bitmap
            Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)

            ' �ǂݍ��݈ʒu�`�F�b�N, �t���[����
            If inf.img_pos >= m_RpiFileSize Or fno > m_RpiHeader.nframe Then
                Return Nothing
            End If

            Dim wk(inf.img_size - 1) As Byte

            ' �摜�擾
            doReadFile_Byte(inf.img_pos, wk, inf.img_size)

            ' �摜�`������ ���� �C���[�W�𐶐�
            If m_RpiHeader.IsBmp Then
                Return tClass.tClass_BmpJpeg.DataToBmp(m_RpiHeader.pitch_nbyte, m_RpiHeader.height, wk)
            Else
                Return tClass.tClass_BmpJpeg.DataToImage(wk)
            End If
        End Function

        ''' <summary>
        ''' RPI�t�@�C�� ���� �t���[���摜�̃o�C�i���f�[�^ ��ǂݍ���
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>�o�C�g�f�[�^</returns>
        ''' <remarks></remarks>
        Public Overrides Function ReadFile_ImgByte(ByVal fno As Integer) As Byte()
            Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)

            ' �ǂݍ��݈ʒu�`�F�b�N, �t���[����
            If inf.img_pos >= m_RpiFileSize Or fno > m_RpiHeader.nframe Then
                Return Nothing
            End If

            ' �摜�擾
            Dim wk(inf.img_size - 1) As Byte

            ' �摜�擾
            doReadFile_Byte(inf.img_pos, wk, inf.img_size)

            Return wk
        End Function

        '*****************************************************************************************************
        ' �t�@�C���Ǎ��֌W(��{�E�w�b�_�n)
        ''' <summary>
        ''' �t���[���t�����̓ǂݍ��� (�t���[���P�ʂ̃G�b�W�ʒu)
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>�t���[���P�ʂ̃G�b�W�ʒu</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_EdgePerFrame(ByVal fno As Integer) As EDGE_PER_FRAME
            Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)
            Dim br As New BinaryReaderSP(ReadFile_Raw(inf.inf_pos, 8))

            Dim wk As EDGE_PER_FRAME
            wk.edge_left = br.ReadInt32
            wk.edge_right = br.ReadInt32
            Return wk
        End Function

        ''' <summary>
        ''' �t���[���t�����̓ǂݍ��� (���胉�C���P�ʂ̃G�b�W�ʒu)
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>���胉�C���P�ʂ̃G�b�W�ʒu</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_EdgePerLine(ByVal fno As Integer) As EDGE_PER_LINE
            Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)

            ' �̈�m��
            Dim wk As New EDGE_PER_LINE
            ReDim wk.edge_left(m_RpiHeader.height - 1)
            ReDim wk.edge_right(m_RpiHeader.height - 1)

            ' �o�C�i���f�[�^�`���Ŏ擾���āA��C��int�z��ɕϊ�
            Dim size As Integer = 4 * m_RpiHeader.height  ' �Б��݂̂̃T�C�Y
            Dim buf As MemoryStream = ReadFile_Raw(inf.inf_pos, size * 2)
            Buffer.BlockCopy(buf.GetBuffer(), 0, wk.edge_left, 0, m_RpiHeader.height * 4)
            Buffer.BlockCopy(buf.GetBuffer(), m_RpiHeader.height * 4, wk.edge_right, 0, m_RpiHeader.height * 4)

            Return wk
        End Function

        ''' <summary>
        ''' �t���[���t�����̓ǂݍ��� (TEX�`���P)
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>�t���[���P�ʂ̃G�b�W�ʒu</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_T1(ByVal fno As Integer) As T_RPI_FRAME_INFO_T1

            '��O����Ԃ̂͂���
            Dim inf As New RPI_FRAME_INF
            If Not m_RpiFrameInf.TryGetValue(fno, inf) Then
                Return Nothing
            End If

            Dim br As New BinaryReaderSP(ReadFile_Raw(inf.inf_pos, 64))

            Dim wk As New T_RPI_FRAME_INFO_T1
            wk.initialize()
            wk.nFNo = br.ReadInt32()
            wk.nSpeed = br.ReadInt32()
            wk.vRes = br.ReadSingle()
            wk.nFromLen = br.ReadInt32()
            wk.dEdgePos(0) = br.ReadSingle()
            wk.dEdgePos(1) = br.ReadSingle()
            wk.nEdgeIdx(0) = br.ReadInt32()
            wk.nEdgeIdx(1) = br.ReadInt32()
            Return wk
        End Function


        ''' <summary>
        ''' �t���[���t�����̓ǂݍ��� (TEX�`���P)
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>�t���[���P�ʂ̃G�b�W�ʒu</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_T1_Async(ByVal fno As Integer, cb As CallbackReadFile) As Boolean

            '��O����Ԃ̂͂���
            Dim inf As New RPI_FRAME_INF
            If Not m_RpiFrameInf.TryGetValue(fno, inf) Then
                Return False
            End If

            If inf.inf_pos = 0 Then
                Return False
            End If

            Try
                ReadFile_RawAsync(inf.inf_pos, 64, cb, fno)
                Return True
            Catch e As System.IO.IOException
                Return False
            End Try

        End Function

        Public Function Parse_FrameInfo_T1(ms As MemoryStream) As T_RPI_FRAME_INFO_T1
            Dim br As New BinaryReaderSP(ms)

            Dim wk As New T_RPI_FRAME_INFO_T1
            wk.initialize()
            wk.nFNo = br.ReadInt32()
            wk.nSpeed = br.ReadInt32()
            wk.vRes = br.ReadSingle()
            wk.nFromLen = br.ReadInt32()
            wk.dEdgePos(0) = br.ReadSingle()
            wk.dEdgePos(1) = br.ReadSingle()
            wk.nEdgeIdx(0) = br.ReadInt32()
            wk.nEdgeIdx(1) = br.ReadInt32()
            Return wk
        End Function

        '*****************************************************************************************************
        ' �t�@�C���Ǎ��֌W(��{�n)
        ''' <summary>
        ''' RPI�t�@�C������length�o�C�g�ǂݍ��݁AMemoryStream�Ƃ��ĕԂ��B
        ''' </summary>
        ''' <param name="pos">�t�@�C���̈ʒu</param>
        ''' <param name="length">�ǂݍ��ރT�C�Y</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function ReadFile_Raw(ByVal pos As Long, ByVal length As Integer) As MemoryStream
            Dim ms As New MemoryStream()
            ms.SetLength(length)

            doReadFile_Byte(pos, ms.GetBuffer(), length)
            Return ms
        End Function

        Public Delegate Sub CallbackReadFile(ms As MemoryStream, read As Integer, o As Object)

        Private Class Tuple3
            Public cb As CallbackReadFile
            Public ms As MemoryStream
            Public o As Object
            Sub New(cb As CallbackReadFile, ms As MemoryStream, o As Object)
                Me.cb = cb
                Me.ms = ms
                Me.o = o
            End Sub
        End Class

        Private Function ReadFile_RawAsync(ByVal pos As Long, ByVal length As Integer, cb As CallbackReadFile, o As Object) As Boolean
            Dim ms As New MemoryStream()
            ms.SetLength(length)

            Dim buf() As Byte = ms.GetBuffer()
            Dim ar As IAsyncResult
            SyncLock m_RpiFileStream
                m_RpiFileStream.Position = pos
                'ar = m_RpiFileStream.BeginRead()
                ar = m_RpiFileStream.BeginRead(buf, 0, length, AddressOf BeginRead_finish, New Tuple3(cb, ms, o))
            End SyncLock

            Return True
        End Function

        Private Sub BeginRead_finish(ar As IAsyncResult)
            Dim kv As Tuple3 = TryCast(ar.AsyncState, Tuple3)
            Dim read As Integer = m_RpiFileStream.EndRead(ar)
            kv.cb.Invoke(kv.ms, read, kv.o)
        End Sub

        ''' <summary>
        ''' RPI�t�@�C������length�o�C�g�ǂށB�X���b�h�Z�[�t
        ''' </summary>
        ''' <param name="pos"></param>
        ''' <param name="buf"></param>
        ''' <param name="readlen"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function doReadFile_Byte(ByVal pos As Long, ByVal buf() As Byte, readlen As Integer) As Integer
#If READRPI_FILESTREAM Then
            'SyncLock m_RpiFileStream
            '    m_RpiFileStream.Position = pos
            '    Return m_RpiFileStream.Read(buf, 0, readlen)
            'End SyncLock


            Dim ar As IAsyncResult = doReadFile_Begin(pos, buf, readlen)
            ar.AsyncWaitHandle.WaitOne()
            Return m_RpiFileStream.EndRead(ar)
#Else
            Using ov As ReadOverlapped = m_RpiFileStream.IssureRead(pos, buf, readlen)
                Dim ret As Integer = ov.WaitResult()
                Return ret
            End Using
#End If
        End Function

        Private Function doReadFile_Begin(ByVal pos As Long, ByVal buf() As Byte, readlen As Integer) As IAsyncResult
            SyncLock m_RpiFileStream
                m_RpiFileStream.Position = pos
                'ar = m_RpiFileStream.BeginRead()
                Return m_RpiFileStream.BeginRead(buf, 0, readlen, Nothing, Nothing)
            End SyncLock
        End Function






        '*****************************************************************************************************
        '/ ���C�������E�o�b�N�O���E���h�ŁA�t���[�������N�����擾����Ƃ��B

        ''' <summary>
        ''' �X���b�h�X�^�[�g
        ''' </summary>
        Private Function ThreadStart_RpiCreateList(Optional ByVal priority As Threading.ThreadPriority = Threading.ThreadPriority.Normal) As Boolean
            If Not mcls_WThread Is Nothing Then
                Call ThreadStop_RpiCreateList()
            End If

            '''' ���C���X���b�h���s
            mcls_WThread = New tClass_WorkThread
            mcls_WThread.ThreadStart(AddressOf mainThread_RpiCreateList, priority, "ReadRpiCreatList", True)
            Return True
        End Function

        ''' <summary>
        ''' �X���b�h�X�g�b�v
        ''' </summary>
        Private Sub ThreadStop_RpiCreateList()
            '' �C���X�^���X�m�F
            If Not mcls_WThread Is Nothing Then
                '' ���C���X���b�h�I��
                mcls_WThread.ThreadStop(200)
                mcls_WThread.Dispose()
                mcls_WThread = Nothing
            End If
        End Sub

        ''' <summary>
        ''' �X���b�h�I���`�F�b�N
        ''' </summary>
        ''' <returns>true:�X���b�h��~�v��  false:run</returns>
        ''' <remarks></remarks>
        Private Function TreadStopCheck_RpiCreatList() As Boolean
            If mcls_WThread Is Nothing Then Return False

            '''' �X���b�h�L�莞
            If mcls_WThread.g_evStop.WaitOne(0, True) Then Return True
            Return False
        End Function


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
    End Class

End Namespace
