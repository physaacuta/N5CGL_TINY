'*********************************************************************************
'RPI�摜�\���ɓ����������N���X (���񏈗��ŁA�摜�Ǎ��݁A�\���G���A�Ƀ��X�P���s���A�����`����s��)
'	[Ver]
'		Ver.01    2009/02/20  vs2005 �Ή�
'		Ver.2	  2014/09/27  �J���[�摜�Ή�
'
'	[����]
'		�E��1�t���[�� �c?�t���[�� �`����Pixel�P�ʂł��炵�ĕ\���������p�N���X
'		�E��{�I�ɂ��̃N���X�̒��ŕ`��͂��Ȃ��B(= �ʒu�ύX�Ƃ�����Ă��g���ق��ŕ`����R�[�����鎖�B���ꂪ�J��Ԃ��`��̌����ւƂȂ���)
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Option Explicit On

Imports System.Windows.Forms
Imports System.Drawing
Imports tClassLibrary.RpiPlugin


Namespace tClass

    Partial Public Class tClass_ViewRpiThread
        Implements IDisposable                                                  ' �f�X�g���N�^��`

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �O���C�x���g
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' �������Ă��Ȃ����k�`���̂��߂̓W�J�v��
        ''' </summary>
        ''' <param name="img">�W�J��r�b�g�}�b�v</param>
        ''' <param name="byData">���k�f�[�^</param>
        ''' <param name="nDataSize">���k�f�[�^�̃o�C�g��</param>
        ''' <param name="rpiHeader">���݂�RPI�w�b�_���</param>
        Public Event tEv_DecodeImage(ByRef img As Bitmap, ByVal byData() As Byte, ByVal nDataSize As Integer, ByVal rpiHeader As tClass_ReadRPI.RPI_HEADER)

        ''' <summary>
        ''' ���摜 �ɑ΂���摜�����v�� (���N���X���Ő���) (�}���`�X���b�h���ŃR�[�����邽�߁A��ʂƂ͕ʃX���b�h)
        ''' </summary>
        ''' <param name="p">(in/out)�C���Ώۂ̉摜�N���X���</param>
        ''' <param name="img">(in/out)�C���Ώۂ�Raw�摜</param>
        ''' <remarks></remarks>
        Public Event tEv_CreatingImg(ByVal p As CLS_IMG_INF, ByRef img As Bitmap)

        ''' <summary>
        ''' �\���擪�t���[��No���ύX�A���́A�\���͈͂��ύX (���N���X���Ő���)
        ''' </summary>
        ''' <param name="nFrameNoStart">����̕\���擪�t���[��No(1�I���W��)</param>
        ''' <param name="nFrameNoEnd">����̕\����[�t���[��No(1�I���W��)</param>
        ''' <remarks></remarks>
        Public Event tEv_ChangeDspFrameNo(ByVal nFrameNoStart As Integer, ByVal nFrameNoEnd As Integer)

        ''' <summary>
        ''' �T�C�Y�ύX (���N���X���Ő���)
        ''' </summary>
        ''' <param name="nOldClientSize">�O��̃N���C�A���g�T�C�Y</param>
        ''' <remarks></remarks>
        Public Event tEv_ReSize(ByVal nOldClientSize As Size)


        ''' <summary>
        ''' ���[�U�[�����`�� (tClass.tClass_ImageDspBase����̎󂯓n��)
        ''' </summary>
        ''' <param name="e">�O���t�B�b�N�I�u�W�F�N�g</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_Start(ByVal e As System.Windows.Forms.PaintEventArgs)
        ''' <summary>
        ''' ���[�U�[�ŏI�`�� (tClass.tClass_ImageDspBase����̎󂯓n��)
        ''' </summary>
        ''' <param name="e">�O���t�B�b�N�I�u�W�F�N�g</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)

        ''' <summary>
        ''' �I�[�o�[���C���N���b�N (tClass.tClass_ImageDsp����̎󂯓n��)
        ''' </summary>
        ''' <param name="sender">�I�[�o�[���C���</param>
        ''' <param name="e">�\������Ă���I�[�o�[���C�ʒu���</param>
        ''' <remarks></remarks>
        Public Event tEv_OverLay_Click(ByVal sender As tClass_ImageDsp.OVERLAY_INF, ByVal e As Rectangle)



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ RPI�t�@�C�����񑀍� �����o�[
        '/ /////////////////////////////////////////////////////////////////////////////////
        Private m_sThreadName As String = "tCls_ViewRpiTh"

        ' �������N���X
        Private mcls_Img As New tClass_ImageDsp                                 ' �摜�\���N���X  (�{��)
        Private mcls_Rpi As tClass_ReadRPI = New tClass_ReadRPIFile()           ' RPI�t�@�C���Ǎ� (�{��)
        Private mcls_Cache As New lCls_ImageCache                               ' �f�[�^�Ǘ��N���X (�{��)
        Private mcls_MultiExec As New List(Of MultiThreadDecoder)               ' ����Ǎ��N���X (�{��)
        Private mque_MultiExec As New tClass_Queue(Of CLS_IMG_INF)(1024)             ' ���񑀍�p�L���[ (�{��) (�� �����̒l�͏\���ɑ傫�����邱��)

        Public ReadOnly Property MTDecoder As MultiThreadDecoder
            Get
                Return mcls_MultiExec(0)
            End Get
        End Property

        ' �O���Z�b�g�N���X
        Private mcls_pLog As tClass_LogManager = Nothing                        ' ���O�Ǘ��N���X (�|�C���^)

        ' ������ԊǗ�
        Private m_bIsRpi As Boolean = False                                     ' �t�@�C���A�N�Z�X����������
        Private m_DspAriaInf As DSP_ARIA_INF                                    ' �摜�\�����
        Private mcntl_Item As CNTL_ITEM                                         ' �O���Z�b�g�I�u�W�F�N�g

        Const m_nScrollPitch As Integer = 1                                   ' �����RPI�t�@�C���̏ꍇ�̍ŏ��ړ���f�� [�\�����W�n�ł�pixel]   ' �X�N���[���o�[�� SmallChange, LargeChange�������Əo�������߁A�قƂ�ǎg���@����B�����Ă��ǂ����ꉞ�c������


        ' �\���͈͌��� 
        Private m_dLenOffset(1) As Double                                       ' RPI�t�@�C���̕\���͈� [mm] (�擪��0mm�Ƃ����Ƃ��ɁAx�`y���w��B�A�v�����ł�x=0m�Ƃ��Č�����)
        Private m_nImgOffsetY As Integer = 0                                    ' �摜���Y���X [��pixel] (�Y���V���ʒu��0mm�Ƃ���) 200���ƍ��G���A��200�}�������



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �\���͈͑Ή�
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �\���͈� [mm] �̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="nIndex">(0:�J�n�ʒu 1:�I���ʒu)</param>
        Public Property tProp_LenOff(ByVal nIndex As Integer) As Double
            Get
                Return m_dLenOffset(nIndex)
            End Get
            Set(ByVal value As Double)
                m_dLenOffset(nIndex) = value
            End Set
        End Property

        ''' <summary>
        ''' ���ꂼ��͈͈̔ʒu�����̂܂܎擾
        ''' </summary>
        Public Function GetLenOff_Len(ByVal nIndex As Integer) As Double
            Return m_dLenOffset(nIndex)
        End Function
        Public Function GetLenOff_Dp(ByVal nIndex As Integer) As Integer
            Return GetPos_LenToDp_Y(m_dLenOffset(nIndex))
        End Function
        Public Function GetLenOff_Rp(ByVal nIndex As Integer) As Integer
            Return GetPos_LenToRp_Y(m_dLenOffset(nIndex))
        End Function


        ''' <summary>
        ''' �摜���Y���X [��pixel]  �̎擾�E�ݒ� (�Y���V���ʒu��0mm�Ƃ���) 200���ƍ��G���A��200�}�������
        ''' </summary>
        Public Property tProp_ImgOffY() As Integer
            Get
                Return m_nImgOffsetY
            End Get
            Set(ByVal value As Integer)
                m_nImgOffsetY = value
            End Set
        End Property




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �󂯓n���p
        '/ /////////////////////////////////////////////////////////////////////////////////

        '===================================================
        ' ���C�� �\����

        ''' <summary>
        ''' �g�p�R���g���[�� �󂯓n���\����
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure CNTL_ITEM
            ''' <summary>�摜�\���s�N�`���[</summary>
            Dim picDspImg As PictureBox                                         ' �摜�\���s�N�`���[
            ''' <summary>�c�X�N���[���o�[ (NULL����)</summary>
            Dim vsBar As tCnt.tCnt_VScrollBar                                   ' �c�X�N���[���o�[ (NULL����)
            ''' <summary>���X�N���[���o�[ (NULL����)</summary>
            Dim hsBar As tCnt.tCnt_HScrollBar                                   ' ���X�N���[���o�[ (NULL����)
        End Structure




        '===================================================
        ''' <summary>
        ''' �摜�\�����
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure DSP_ARIA_INF
            Dim DspImgSize As tClass_ImageDspBase.SizeD                         ' �\�����W�n�ł̉摜�T�C�Y (1�t���[��)
            Dim DspPixelTotalOffset As Point                                    ' �\�����W�n�ł̕\������܂ł̃g�[�^���I�t�Z�b�gpixel�l
            Dim nDspFno As Integer                                              ' �\���擪�̃t���[��No (0�I���W��)
            Dim nDspFnoEnd As Integer                                           ' �\�����Z�̃t���[��No (0�I���W��)
            Dim bDspAllOk As Boolean                                            ' ��ʂɕ\������t���[���S��������Ɏ擾�ςݎ�true
            Dim nDspFrameCnt As Integer                                         ' ��ʂɉ��t���[���\���\���H(�؂�グ)
            Dim ImgNonAriaSize As Size                                          ' �\���G���A�ɕ\������Ă��Ȃ������̉摜�T�C�Y (�\���{���ɕϊ��������Pixel��) = �X�N���[���o�[�̗ʂƂȂ�
        End Structure




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����N���X �� �����J�p �֌W
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �摜�\���N���X
        ''' </summary>
        Public ReadOnly Property GetCls_ImgDsp() As tClass_ImageDsp
            Get
                Return mcls_Img
            End Get
        End Property
        '�e�X�g
        Public Property PropCls_ImgDsp() As tClass_ImageDsp
            Get
                Return mcls_Img
            End Get
            Set(value As tClass_ImageDsp)
                mcls_Img = value
            End Set
        End Property

        ''' <summary>
        ''' RPI�w�b�_�擾
        ''' </summary>
        Public ReadOnly Property GetRpi_Header() As tClass_ReadRPI.RPI_HEADER
            Get
                Return mcls_Rpi.GetRpiHeader
            End Get
        End Property

        ''' <summary>
        ''' TEX��RPI�w�b�_�擾
        ''' </summary>
        Public ReadOnly Property GetRpi_THeader() As tClass_ReadRPI.RPI_T_HEADER
            Get
                Return mcls_Rpi.GetRpiTHeader
            End Get
        End Property

        ''' <summary>
        ''' RPI�t���[���A�N�Z�X
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>RPI�t���[�����</returns>
        Public ReadOnly Property GetRpi_FrameInf(ByVal fno As Integer) As tClass_ReadRPI.RPI_FRAME_INF
            Get
                Return mcls_Rpi.GetRpiFrameInf(fno)
            End Get
        End Property

        ''' <summary>
        ''' RPI�N���X�{��
        ''' </summary>
        Public ReadOnly Property GetRpi_Cls() As tClass_ReadRPI
            Get
                Return mcls_Rpi
            End Get
        End Property

        ''' <summary>
        ''' �摜�����擾
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property GetImg_ImgInf() As tClass_ImageDsp.IMG_INF
            Get
                Return mcls_Img.tProp_ImgInf
            End Get
        End Property
        ''' <summary>
        ''' �\���`�����擾
        ''' </summary>
        Public ReadOnly Property GetImg_ImgKind() As tClass_ImageDsp.IMG_KIND
            Get
                Return mcls_Img.tProp_ImgKind
            End Get
        End Property
        ''' <summary>
        ''' �\���̈���擾
        ''' </summary>
        Public ReadOnly Property GetImg_AriaInf() As tClass_ImageDsp.ARIA_INF
            Get
                Return mcls_Img.tProp_AriaInf
            End Get
        End Property
        ''' <summary>
        ''' ���W�ŏI���ʎ擾
        ''' </summary>
        Public ReadOnly Property GetImg_CalcAns() As tClass_ImageDsp.CALC_ANS
            Get
                Return mcls_Img.tProp_CalcAns
            End Get
        End Property

        ''' <summary>
        ''' ��ǂݏ��
        ''' </summary>
        ''' <param name="nIndex">�t�@�C���擪����̃t���[��No(1�I���W��)</param>
        Public ReadOnly Property GetCls_Img_inf(ByVal nIndex As Integer) As CLS_IMG_INF
            Get
                Dim result As CLS_IMG_INF = Nothing
                mcls_Cache.g_lstImg.TryGetValue(nIndex, result)
                Return result
            End Get
        End Property


        ''' <summary>
        ''' 90����]���邩�ǂ����B���邢�́A��������\�����邩�ǂ���
        ''' </summary>
        Public Property PropRotate90() As Boolean
            Get
                Return mcls_Img.tProp_ImgKind_Rotate90
            End Get
            Set(ByVal value As Boolean)
                mcls_Img.tProp_ImgKind_Rotate90 = value
            End Set
        End Property


        ''' <summary>
        ''' ���葤�X�N���[���o�[
        ''' </summary>
        Private ReadOnly Property scrNagate() As ScrollBar
            Get
                If mcls_Img.tProp_ImgKind_Rotate90 Then
                    Return mcntl_Item.hsBar
                Else
                    Return mcntl_Item.vsBar
                End If
            End Get
        End Property

        ''' <summary>
        ''' ���葤�X�N���[���o�[�̃}�E�X������
        ''' </summary>
        Private ReadOnly Property scrNagateMaximumVirtual() As Integer
            Get
                If mcls_Img.tProp_ImgKind_Rotate90 Then
                    Return mcntl_Item.hsBar.MaximumVirtual
                Else
                    Return mcntl_Item.vsBar.MaximumVirtual
                End If
            End Get
        End Property

        ''' <summary>
        ''' ���葤�X�N���[���o�[�l�̕ύX
        ''' </summary>
        Private WriteOnly Property scrNagateValueProgramable() As Integer
            Set(ByVal value As Integer)
                If mcls_Img.tProp_ImgKind_Rotate90 Then
                    mcntl_Item.hsBar.ValueProgramable = value
                Else
                    mcntl_Item.vsBar.ValueProgramable = value
                End If
            End Set
        End Property


        ''' <summary>
        ''' ���X�N���[���o�[
        ''' </summary>
        Private ReadOnly Property scrHaba() As ScrollBar
            Get
                If Not mcls_Img.tProp_ImgKind_Rotate90 Then
                    Return mcntl_Item.hsBar
                Else
                    Return mcntl_Item.vsBar
                End If
            End Get
        End Property

        ''' <summary>
        ''' ���X�N���[���o�[�̃}�E�X������
        ''' </summary>
        Private ReadOnly Property scrHabaMaximumVirtual() As Integer
            Get
                If Not mcls_Img.tProp_ImgKind_Rotate90 Then
                    Return mcntl_Item.hsBar.MaximumVirtual
                Else
                    Return mcntl_Item.vsBar.MaximumVirtual
                End If
            End Get
        End Property

        ''' <summary>
        ''' �����X�N���[���o�[�l�̕ύX
        ''' </summary>
        Private WriteOnly Property scrHabaValueProgramable() As Integer
            Set(ByVal value As Integer)
                If Not mcls_Img.tProp_ImgKind_Rotate90 Then
                    mcntl_Item.hsBar.ValueProgramable = value
                Else
                    mcntl_Item.vsBar.ValueProgramable = value
                End If
            End Set
        End Property

        ''' <summary>
        ''' �i���ݒ� (��ŕύX�p)
        ''' </summary>
        ''' <param name="nQuarity">�摜�k���i�� (0:��掿  1:���掿)</param>
        ''' <param name="bIsUseImgLibCLI">�摜�k�����@ (false:API  true:ImgLibCLI)</param> 
        Public Sub SetParam_Quarity(ByVal nQuarity As Integer, ByVal bIsUseImgLibCLI As Boolean)
            For Each cls As MultiThreadDecoder In mcls_MultiExec
                cls.SetParam_Quarity(nQuarity, bIsUseImgLibCLI)
            Next
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����o�[�N���X�ւ̏��Z�b�g
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �I�[�o�[���C���o�^
        ''' </summary>
        ''' <param name="ovl">�I�[�o�[���C���</param>
        ''' <remarks></remarks>
        Public Sub SetOverLayInf(ByVal ovl As tClass_ImageDsp.OVERLAY_INF)
            Call mcls_Img.SetOverLayInf(ovl)
        End Sub
        ''' <summary>
        ''' �I�[�o�[���C���J��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ResetOverLayInf()
            Call mcls_Img.ResetOverLayInf()
        End Sub

        ''' <summary>
        ''' ��ʕ\�� ���E���]
        ''' </summary>
        ''' <param name="nMode">���̂܂�:0 ���]:1</param>
        ''' <remarks></remarks>
        Public Sub SetProp_DSWS(ByVal nMode As Integer)
            ' �f�t�H���g�ł́A��ʍ����́ADS
            If 0 = nMode Then mcls_Img.tProp_ImgKind_IsTurnLR = False
            If 1 = nMode Then mcls_Img.tProp_ImgKind_IsTurnLR = True
        End Sub
        Public Function GetProp_DSWS() As Boolean
            Return mcls_Img.tProp_ImgKind_IsTurnLR
        End Function
        ''' <summary>
        ''' ��ʕ\�� �㉺���]
        ''' </summary>
        ''' <param name="nMode">���̂܂�:0 ���]:1</param>
        ''' <remarks></remarks>
        Public Sub SetProp_TB(ByVal nMode As Integer)
            If 0 = nMode Then mcls_Img.tProp_ImgKind_IsTurnTB = False
            If 1 = nMode Then mcls_Img.tProp_ImgKind_IsTurnTB = True
        End Sub
        Public Function GetProp_TB() As Boolean
            Return mcls_Img.tProp_ImgKind_IsTurnTB
        End Function

        ''' <summary>
        ''' �摜�g��
        ''' </summary>
        ''' <param name="dRate">�g��{��</param>
        ''' <remarks></remarks>
        Public Sub SetProp_Zoom(ByVal dRate As Double)
            mcls_Img.tProp_AriaInf_Zoom = New tClass_ImageDsp_Calc.RATE(dRate, dRate)
        End Sub
        ''' <summary>
        ''' �摜�g��
        ''' </summary>
        ''' <param name="dRateX">�g��{��X</param>
        ''' <param name="dRateY">�g��{��Y</param>
        ''' <remarks></remarks>
        Public Sub SetProp_Zoom(ByVal dRateX As Double, ByVal dRateY As Double)
            mcls_Img.tProp_AriaInf_Zoom = New tClass_ImageDsp_Calc.RATE(dRateX, dRateY)
        End Sub
        ''' <summary>
        ''' �摜�\�����
        ''' </summary>
        Public Sub SetProp_ImgKind_StrechType(Optional ByVal em As tClass_ImageDsp_Calc.EM_IMG_STRECH = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth)
            mcls_Img.tProp_ImgKind_StrechType = em
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �v���p�e�B�֌W
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �摜�\��OK
        ''' </summary>
        Public ReadOnly Property IsRpi() As Boolean
            Get
                Return m_bIsRpi
            End Get
        End Property
        ''' <summary>
        ''' �摜�\�����
        ''' </summary>
        Public ReadOnly Property GetDspAriaInf() As DSP_ARIA_INF
            Get
                Return m_DspAriaInf
            End Get
        End Property

        ''' <summary>
        ''' �摜�\����� (�S�t���[���摜�\���\)
        ''' </summary>
        Public ReadOnly Property GetDspAriaInf_DspAllOk() As Boolean
            Get
                Return m_DspAriaInf.bDspAllOk
            End Get
        End Property

        ''' <summary>
        ''' �t���[���t�����擾 (�t���[���P�ʂ̃G�b�W�ʒu)
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>�t���[���P�ʂ̃G�b�W�ʒu</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_EdgePerFrame(ByVal fno As Integer) As tClass_ReadRPI.EDGE_PER_FRAME
            Return mcls_Rpi.GetEdgePerFrame(fno)
        End Function

        ''' <summary>
        ''' ��ǂ݃t���[����
        ''' </summary>
        Public Property tProp_ResvNum(ByVal nIndex As Integer) As Integer
            Get
                Return mcls_Cache.tProp_ResvNum(nIndex)
            End Get
            Set(ByVal value As Integer)
                mcls_Cache.tProp_ResvNum(nIndex) = value
            End Set
        End Property



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ���ʌ��J�����֌W
        '/ /////////////////////////////////////////////////////////////////////////////////


        ''' <summary>
        ''' �L���ȃt���[������ύX����B
        ''' </summary>
        ''' <param name="nCount">�t���[����</param>
        ''' <returns>���������ꍇTrue</returns>
        ''' <remarks>RpiFile�̏ꍇ�͎��s���܂��B</remarks>
        Public Function SetEffectiveFrameCount(ByVal nCount As Integer) As Boolean
            Try
                mcls_Rpi.EffectiveRpiFrameCount = nCount
                ResizePaintAria(Nothing, Nothing)
            Catch e As Exception
                Return False
            End Try
            Return True
        End Function

        ''' <summary>
        ''' �\�����W�n�̑S�̐擪�I�t�Z�b�g�ʂ���Y������t���[��No�A�t���[���擪����̃I�t�Z�b�g���Z�o
        ''' </summary>
        ''' <param name="nTotalOffsetY">0�t���[���擪����̃I�t�Z�b�g��(�\�����W�n)</param>
        ''' <param name="nFrameNo">�Z�o�����t���[��No(0�I���W��)</param>
        ''' <param name="nOneOffsetY">�Z�o�����I�t�Z�b�g�l</param>
        ''' <remarks></remarks>
        Public Sub GetOffset_TotalToOne(ByVal nTotalOffsetY As Integer, ByRef nFrameNo As Integer, ByRef nOneOffsetY As Integer)
            nFrameNo = GetOffset_TotalToOne_FrameNo(nTotalOffsetY)
            nOneOffsetY = GetOffset_TotalToOne_OffsetY(nTotalOffsetY) 'nTotalOffsetY - CInt(Math.Floor(nFrameNo * OneImgPixel))
            If nOneOffsetY < 0 Then
                nFrameNo = 0
            End If
        End Sub
        Public Function GetOffset_TotalToOne_FrameNo(ByVal nTotalOffsetY As Integer) As Integer
            Dim nWk As Integer = nTotalOffsetY - GetPos_RpToDp_Y(m_nImgOffsetY)
            'If m_nImgOffsetY <> 0 Then
            '	nWk += CInt(Math.Floor(m_DspAriaInf.DspImgSize.Height)) - CInt(Math.Floor(CDbl(GetPos_RpToDp_Y(m_nImgOffsetY) Mod m_DspAriaInf.DspImgSize.Height)))
            'End If
            Return CInt(Math.Floor(nWk / m_DspAriaInf.DspImgSize.Height))
        End Function
        Public Function GetOffset_TotalToOne_OffsetY(ByVal nTotalOffsetY As Integer) As Integer
            Dim nWk As Integer = nTotalOffsetY
            If m_nImgOffsetY <> 0 Then
                Dim nImgOff As Integer = GetPos_RpToDp_Y(m_nImgOffsetY)
                Dim nOff As Integer = nImgOff - nTotalOffsetY
                If 0 < nOff Then
                    Return CInt(Math.Floor(m_DspAriaInf.DspImgSize.Height)) - CInt(Math.Floor(nOff Mod m_DspAriaInf.DspImgSize.Height))
                End If
                nWk -= nImgOff
            End If

            Return CInt(Math.Floor(nWk Mod m_DspAriaInf.DspImgSize.Height))
        End Function

        ''' <summary>
        ''' �Y���t���[��No�A�t���[���擪����̃I�t�Z�b�g����\�����W�擪����S�̂̃I�t�Z�b�g�ʂ��Z�o
        ''' </summary>
        ''' <param name="nFrameNo">�w�肵���t���[��No(0�I���W��)</param>
        ''' <param name="nOneOffsetY">���̃t���[�����ł̃I�t�Z�b�g��</param>
        ''' <returns>�\�����W�n �擪����̃g�[�^���I�t�Z�b�g��</returns>
        ''' <remarks></remarks>
        Public Function GetOffsetY_OneToTotal(ByVal nFrameNo As Integer, ByVal nOneOffsetY As Integer) As Integer
            Return CInt(nFrameNo * m_DspAriaInf.DspImgSize.Height) + nOneOffsetY + GetPos_RpToDp_Y(m_nImgOffsetY)
        End Function
        Public Function GetOffsetX_OneToTotal(ByVal nOneOffsetX As Integer) As Integer
            Return CInt(m_DspAriaInf.DspImgSize.Width) + nOneOffsetX
        End Function





        ''' <summary>
        ''' [�\��pixel]��[mm]
        ''' </summary>
        Public Overloads Function GetPos_DpToLen_Y(ByVal v As Integer) As Single
            Return CSng(v * mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_DpToLen_X(ByVal v As Integer) As Single
            Return CSng(v * mcls_Img.tProp_CalcAns.FinalRes.dW)
        End Function


        ''' <summary>
        ''' [mm]��[�\��pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToDp_Y(ByVal v As Double) As Integer
            Return CInt(v / mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_Y(ByVal v As Integer) As Integer
            Return CInt(v / mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_X(ByVal v As Double) As Integer
            Return CInt(v / mcls_Img.tProp_CalcAns.FinalRes.dW)
        End Function
        Public Overloads Function GetPos_LenToDp_X(ByVal v As Integer) As Integer
            Return CInt(v / mcls_Img.tProp_CalcAns.FinalRes.dW)
        End Function

        ''' <summary>
        ''' [��pixel]��[mm]
        ''' </summary>
        Public Overloads Function GetPos_RpToLen_Y(ByVal v As Integer) As Single
            Return CSng((v * mcls_Img.tProp_CalcAns.RateDsp.dH) * mcls_Img.tProp_CalcAns.FinalRes.dH)
            'Return CSng(v * (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.RateDsp.dH))
        End Function
        Public Overloads Function GetPos_RpToLen_X(ByVal v As Integer) As Single
            Return CSng(v * (mcls_Img.tProp_CalcAns.RateDsp.dW * mcls_Img.tProp_CalcAns.FinalRes.dW))
        End Function

        ''' <summary>
        ''' [mm]��[��pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToRp_Y(ByVal v As Double) As Integer
            Return CInt(v / (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_Y(ByVal v As Integer) As Integer
            Return CInt(v / (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_X(ByVal v As Double) As Integer
            Return CInt(v / (mcls_Img.tProp_CalcAns.RateDsp.dW * mcls_Img.tProp_CalcAns.FinalRes.dW))
        End Function
        Public Overloads Function GetPos_LenToRp_X(ByVal v As Integer) As Integer
            Return CInt(v / (mcls_Img.tProp_CalcAns.RateDsp.dW * mcls_Img.tProp_CalcAns.FinalRes.dW))
        End Function


        ''' <summary>
        ''' [�\��pixel]��[��pixel] �\���p�̍��W���猳�̉摜���W�ɕϊ�
        ''' </summary>
        Public Overloads Function GetPos_DpToRp(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetSimplePoint_DspToImg(x, y)
        End Function
        Public Overloads Function GetPos_DpToRp_X(ByVal x As Integer) As Integer
            Return CInt(x / mcls_Img.tProp_CalcAns.RateDsp.dW)
        End Function
        Public Overloads Function GetPos_DpToRp_Y(ByVal y As Integer) As Integer
            Return CInt(y / mcls_Img.tProp_CalcAns.RateDsp.dH)
        End Function

        ''' <summary>
        ''' [��pixel]��[�\��pixel] ���̉摜���W����\���p�̍��W�ɕϊ�
        ''' </summary>
        Public Overloads Function GetPos_RpToDp(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetSimplePoint_ImgToDsp(x, y)
        End Function
        Public Overloads Function GetPos_RpToDp_X(ByVal x As Integer) As Integer
            Return CInt(x * mcls_Img.tProp_CalcAns.RateDsp.dW)
        End Function
        Public Overloads Function GetPos_RpToDp_Y(ByVal y As Integer) As Integer
            Return CInt(y * mcls_Img.tProp_CalcAns.RateDsp.dH)
        End Function



        ''' <summary>
        ''' [��pixel]��[�\��pixel] ���̉摜���W����\���p�̍��W�ɕϊ� (���]�E�I�t�Z�b�g�����l��) 
        ''' </summary>
        ''' <param name="x">���摜X���W</param>
        ''' <param name="y">���摜Y���W</param>
        ''' <returns>�\�����W</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetPosition(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetPosition(x, y)
        End Function
        Public Overloads Function GetPosition(ByVal pos As Point) As Point
            Return mcls_Img.GetPosition(pos.X, pos.Y)
        End Function




        '''''''''�ʒu�Ɋւ���ϊ���B_Y�͋�����_mm�͈ʒu�B_mm�ɂ̓I�t�Z�b�g�����B
        '''''''''1000px���ĉ�mm�Ȃ́�_Y�ŕϊ��B�擪����10000px�͉�mm�̈ʒu�Ȃ́�_mm�ŕϊ��B
        '�擪�ʒu(dp=0, rp=0)�̃��x�������~���ƕ\�����邩�B
        Private m_mmLabelOffsetY As Double
        Public Property tProp_LabelOffsetYmm As Double
            Get
                Return m_mmLabelOffsetY
            End Get
            Set(value As Double)
                m_mmLabelOffsetY = value
            End Set
        End Property

        ''' <summary>
        ''' [�\��pixel]��[mm�ʒu]
        ''' </summary>
        Public Overloads Function GetPos_DpToLen_mm(ByVal v As Integer) As Single
            Return CSng(v * mcls_Img.tProp_CalcAns.FinalRes.dH + m_mmLabelOffsetY)
        End Function


        ''' <summary>
        ''' [mm�ʒu]��[�\��pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToDp_mm(ByVal v As Double) As Integer
            Return CInt((v - m_mmLabelOffsetY) / mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_mm(ByVal v As Integer) As Integer
            Return CInt((v - m_mmLabelOffsetY) / mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function

        ''' <summary>
        ''' [��pixel]��[mm�ʒu]
        ''' </summary>
        Public Overloads Function GetPos_RpToLen_mm(ByVal v As Integer) As Single
            Return CSng((v * mcls_Img.tProp_CalcAns.RateDsp.dH) * mcls_Img.tProp_CalcAns.FinalRes.dH + m_mmLabelOffsetY)
        End Function

        ''' <summary>
        ''' [mm�ʒu]��[��pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToRp_mm(ByVal v As Double) As Integer
            Return CInt((v - m_mmLabelOffsetY) / (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_mm(ByVal v As Integer) As Integer
            Return CInt((v - m_mmLabelOffsetY) / (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.FinalRes.dH))
        End Function



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����Z�b�g
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' ���O�N���X�Z�b�g
        ''' </summary>
        Public Sub SetCls_Log(ByRef cls As tClass_LogManager)
            mcls_pLog = cls
        End Sub

        ''' <summary>
        ''' �r�����摜���Z�b�g
        ''' </summary>
        ''' <param name="DefImg">�r�����摜</param>
        Public Sub SetItem_DefImg(ByVal DefImg As Bitmap)
            Call mcls_Img.SetDefImage(CType(DefImg, Image))
        End Sub




        ''' <summary>
        ''' �����Z�b�g (Set�`���I�������ŁA�������Ɉ��̂�)
        ''' </summary>
        ''' <param name="cntl">�{�N���X�Ŏg�p����R���g���[���Q</param>
        ''' <param name="nThreadNum">����Ǎ��݂���X���b�h��</param>
        ''' <param name="nQuarity">�摜�k���i�� (0:��掿  1:���掿)</param>
        ''' <param name="bIsUseImgLibCLI">�摜�k�����@ (false:API  true:ImgLibCLI)</param>
        ''' <remarks></remarks>
        Public Sub Alloc(ByRef cntl As CNTL_ITEM, ByVal nThreadNum As Integer, Optional ByVal nQuarity As Integer = 0, Optional ByVal bIsUseImgLibCLI As Boolean = False)
            '''' �R���g���[����o�^
            mcntl_Item = cntl

            ''''' �摜�\���N���X �� ���� 2
            ' �`��R���g���[���Z�b�g
            Call mcls_Img.AddHandler_PaintEvent(mcntl_Item.picDspImg)                           ' �y�C���g�C�x���g�o�^ & �s�N�`���[�o�^
            Call mcls_Img.AddHandler_OverLayClick()                                             ' �s�N�`���[�N���b�N�ŃI�[�o�[���C���N���b�N���������f �o�^


            '''' �����C�x���g�ƕR�t��
            ' �R���g���[���̃C�x���g�ƕR�t��
            AddHandler mcntl_Item.picDspImg.Resize, AddressOf ResizePaintAria                  ' �y�C���g�T�C�Y�ύX�C�x���g�o�^
            'AddHandler mcntl_Item.picDspImg.SizeChanged, AddressOf ResizePaintAria
            ' �����N���X�Ƃ̃C�x���g�R�t��
            AddHandler mcls_Img.tEv_Paint_Start, AddressOf Ev_Paint_Start                       ' �t���[���摜�ɓƎ��`��
            AddHandler mcls_Img.tEv_Paint_End, AddressOf Ev_Paint_End                           ' �t���[���摜�ɓƎ��`��
            AddHandler mcls_Img.tEv_OverLay_Click, AddressOf Ev_OverLay_Click                   ' �I�[�o�[���C�N���b�N


            '===================================================
            ' �f�[�^�Ǘ��N���X
            mcls_Cache.SetCls_Log(mcls_pLog)
            mcls_Cache.SetCls_ReadRpi(mcls_Rpi)
            mcls_Cache.SetCls_Img(mcls_Img)
            mcls_Cache.SetQue_MultiExec(mque_MultiExec)
            mcls_Cache.tProp_ResvNum(0) = 10
            mcls_Cache.tProp_ResvNum(1) = 10

            ' ����Ǎ��N���X
            For ii As Integer = 0 To nThreadNum - 1

                mcls_MultiExec.Add(New MultiThreadDecoder(String.Format("Loader{0}", ii), nQuarity, bIsUseImgLibCLI))
                mcls_MultiExec(ii).SetCls_Log(mcls_pLog)
                mcls_MultiExec(ii).SetCls_ReadRpi(mcls_Rpi)
                mcls_MultiExec(ii).SetQue_MultiExec(mque_MultiExec)
                InitDecoders(mcls_MultiExec(ii).m_decoders)
                'mcls_MultiExec(ii).ThreadStart(Threading.ThreadPriority.Lowest)
                AddHandler mcls_MultiExec(ii).tEv_DecodeImage, AddressOf Ev_DecodeImage         ' �������Ă��Ȃ����k�̓W�J
                AddHandler mcls_MultiExec(ii).tEv_CreatingImg, AddressOf Ev_CreatingImg         ' ���摜 �ɑ΂���摜�����v�� (���N���X���Ő���) (�}���`�X���b�h���ŃR�[�����邽�߁A��ʂƂ͕ʃX���b�h)

                mcls_MultiExec(ii).ThreadStart(Threading.ThreadPriority.Highest)
            Next


            '''' ���N���X������
            Call Init(True)

            '''' �T�C�Y�ύX�����Ńf�[�^��o�^
            Call ResizePaintAria(Nothing, Nothing)

            '''' �ĕ`��
            Call RePaint()
        End Sub


        Public m_decoder_types As New List(Of Type)
        Public Sub AddDecoders(decoders As IEnumerable(Of Type))
            m_decoder_types.AddRange(decoders)
        End Sub



        Private Sub InitDecoders(decoders As List(Of IRPIDecoder))
            decoders.Add(New tClass_RpiDecoder_LAC)
            decoders.Add(New tClass_RpiDecoder_BMP)
            decoders.Add(New tClass_RpiDecoder_Raw)
            decoders.Add(New tClass_RpiDecoder_Jpeg)
            For Each t As Type In m_decoder_types
                Dim obj As Object = Activator.CreateInstance(t)
                Dim decoder As IRPIDecoder = TryCast(obj, IRPIDecoder)
                decoders.Add(decoder)
            Next
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �������֘A
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        Public Sub New()
            '''' �摜�\���N���X �� ���� 1
            mcls_Img.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth    ' �\���G���A�̕��ɉ摜�������킹��
            mcls_Img.tProp_AriaInf_IsCenter = False                                             ' �r���S�ʒu�ړ�����
            mcls_Img.SetImgInf(1024, 1024, 1, 1, 0, 0)                                          ' �Ƃ肠�����������Ƃ��ăZ�b�g
            mcls_Img.tProp_Paint_IsDefImgDspn = True                                            ' �f�t�H���g�摜�\����ON
            mcls_Img.tProp_Paint_IsPaint = True                                                 ' �摜�\����ON
        End Sub

        ''' <summary>
        ''' �f�X�g���N�^
        ''' </summary>
#Region " IDisposable Support "
        Private disposedValue As Boolean = False        ' �d������Ăяo�������o����ɂ�

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��Ƀ}�l�[�W ���\�[�X��������܂�
                End If

                '''' �N���X���
                If Not mcls_Rpi Is Nothing Then
                    mcls_Rpi.Dispose()
                    mcls_Rpi = Nothing
                End If


                Call mcls_Img.RemoveHandler_OverLayClick()      ' ���I�n���h���J��
                If Not mcls_Img Is Nothing Then
                    mcls_Img.Dispose()
                    mcls_Img = Nothing
                End If

                mcls_Cache.Delete(-1, -1, 0)
                If Not mcls_Cache Is Nothing Then
                    mcls_Cache.g_lstImg.Clear()
                    mcls_Cache = Nothing
                End If
                If Not mcls_MultiExec Is Nothing Then
                    For ii As Integer = 0 To mcls_MultiExec.Count - 1
                        mcls_MultiExec(ii).ThreadStop()
                        mcls_MultiExec(ii).Dispose()
                        mcls_MultiExec(ii) = Nothing
                    Next
                    mcls_MultiExec.Clear()
                    mcls_MultiExec = Nothing
                End If
                If Not mque_MultiExec Is Nothing Then
                    mque_MultiExec.Dispose()
                    mque_MultiExec = Nothing
                End If

            End If

            Me.disposedValue = True
        End Sub


        ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
        Public Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region


        ''' <summary>
        ''' ���N���X������
        ''' </summary>
        ''' <param name="bInit">���S��������true   �\���ؑւȂǌ��ʒu����ێ��������ꍇfalse</param>
        Public Sub Init(ByVal bInit As Boolean)
            '''' RPI�t�@�C���N���[�Y
            Call RpiClose()
            ' �摜�\���G���A
            mcls_Img.tProp_Paint_IsDefImgDspn = True                                            ' �f�t�H���g�摜�\����ON

            '''' �����ϐ��J��
            m_bIsRpi = False

            '''' �摜�\����񏉊���
            If bInit Then
                m_DspAriaInf.DspPixelTotalOffset.X = 0
                m_DspAriaInf.DspPixelTotalOffset.Y = 0
                m_DspAriaInf.nDspFno = -1
                m_DspAriaInf.nDspFnoEnd = -1
                m_DspAriaInf.bDspAllOk = False
                m_DspAriaInf.nDspFrameCnt = 0
                m_DspAriaInf.ImgNonAriaSize.Height = 0
                m_DspAriaInf.ImgNonAriaSize.Width = 0
            End If


            '''' �R���g���[�������Z�b�g
            If Not scrNagate Is Nothing Then
                scrNagate.Enabled = False
            End If
            If Not scrHaba Is Nothing Then
                scrHaba.Enabled = False
            End If
        End Sub



        ''' <summary>
        ''' RPI�ɂ݂��������A�t���[���t�@�C���Q���J��
        ''' </summary>
        ''' <param name="strFormat">�t�@�C���p�X�̃t�H�[�}�b�g�B"c:\test\frame_{1:d4}.jpg"�Ƃ��B</param>
        ''' <param name="rpiheader">RPI�������炱��ȃw�b�_�������낤�ȁA�Ƃ������g</param>
        ''' <param name="trpiheader">RPI�������炱���T�w�b�_�����͂��A�Ƃ������g</param>
        ''' <param name="bExecAutoFindThread"> �t�@�C���������ŒT�����邩</param>
        ''' <param name="bInit">���S��������true   �\���ؑւȂǌ��ʒu����ێ��������ꍇfalse</param>
        Public Function OpenRpiFrameFiles(ByVal strFormat As String, ByVal rpiheader As tClass.tClass_ReadRPI.RPI_HEADER, ByVal trpiheader As tClass.tClass_ReadRPI.RPI_T_HEADER, ByVal bExecAutoFindThread As Boolean, Optional ByVal bInit As Boolean = True) As Boolean

            ' ��x ������
            Call Init(bInit)

            If bExecAutoFindThread Then
                mcls_Rpi = New tClass_ReadRpiFrameAuto(strFormat, rpiheader, trpiheader)
            Else
                mcls_Rpi = New tClass_ReadRpiFrame(strFormat, rpiheader, trpiheader)
            End If

            mcls_Cache.SetCls_ReadRpi(mcls_Rpi)
            For ii As Integer = 0 To mcls_MultiExec.Count - 1
                mcls_MultiExec(ii).SetCls_ReadRpi(mcls_Rpi)
            Next

            AfterInit(0, 0)
            Call SetCommonScrollBar(bInit)
            Return True
        End Function


        ''' <summary>
        ''' RPI�t�@�C�� �A�N�Z�X�J�n
        ''' </summary>
        ''' <param name="strFPath">RPI�t�@�C���̃t���p�X</param>
        ''' <param name="bInit">���S��������true   �\���ؑւȂǌ��ʒu����ێ��������ꍇfalse</param>
        ''' <param name="dResX">������\ (0�̏ꍇ�́ARPI�t�@�C�����g�p)</param>
        ''' <param name="dResY">�c����\ (0�̏ꍇ�́ARPI�t�@�C�����g�p)</param>
        ''' <param name="sKanriNo">�㏑���p�����ԍ��i�K���RPI�t�@�C������j</param>
        Public Function RpiOpen(ByVal strFPath As String, Optional ByVal bInit As Boolean = True, Optional ByVal dResX As Single = 0, Optional ByVal dResY As Single = 0, Optional ByVal sKanriNo As String = tClass_ReadRPIFile.READ_FROM_RPI_FILE) As Boolean
            '===================================================
            ' RPI�N���X�Ńt�@�C���ǂݍ��݉\���`�F�b�N
            If Not tClass_ReadRPIFile.RpiOpenCheck(strFPath) Then
                If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.ERR, String.Format("{0} RPI�t�@�C���ǂݍ��ݎ��s1 {1}", m_sThreadName, strFPath))
                Return False
            End If


            '===================================================
            '''' ������RPI�t�@�C���Ǎ������J�n
            ' ��x ������
            Call Init(bInit)
            ''''''''''''''''''''''''''''

            mcls_Rpi = New tClass_ReadRPIFile()
            mcls_Cache.SetCls_ReadRpi(mcls_Rpi)
            For ii As Integer = 0 To mcls_MultiExec.Count - 1
                mcls_MultiExec(ii).SetCls_ReadRpi(mcls_Rpi)
            Next

            ' �t�@�C���I�[�v�� �� true:�񓯊�RPI���X�g�Ǎ���
            If 0 >= CType(mcls_Rpi, tClass_ReadRPIFile).RpiOpen(strFPath, True, sKanriNo) Then
                Call RpiClose()
                Return False
            End If

            AfterInit(dResX, dResY)
            Call SetCommonScrollBar(bInit)                              ' �X�N���[���o�[�� ��`�ύX
            Return True
        End Function


        ''' <summary>
        ''' �S���摜���J�������Ƃ̏����E����
        ''' </summary>
        ''' <param name="dResX">������\ (0�̏ꍇ�́ARPI�t�@�C�����g�p)</param>
        ''' <param name="dResY">�c����\ (0�̏ꍇ�́ARPI�t�@�C�����g�p)</param>
        Private Function AfterInit(ByVal dResX As Single, ByVal dResY As Single) As Boolean
            ' ����Ǎ��ݗp���I�[�v��
            For ii As Integer = 0 To mcls_MultiExec.Count - 1
                Dim wk As New TYP_MULTI_EXEC_OPEN
                'wk.sFPath = strFPath
                wk.sKizukenNo = mcls_Rpi.GetRpiTHeader.cKizukenNo
                mcls_MultiExec(ii).RpiOpen(wk)
            Next

            ' �t���O�̕ύX
            m_bIsRpi = True
            mcls_Img.tProp_Paint_IsDefImgDspn = False                                           ' �f�t�H���g�摜�\����OFF


            '===================================================
            '''' �����v�Z
            ' �摜�\���N���X
            Dim rpiHead As tClass_ReadRPI.RPI_HEADER = mcls_Rpi.GetRpiHeader()

            '''' �Ȃ������ł�������Ă���H�H ����́A�����B��ʂňȊO��I�����Ă���ꍇ�A�ςɂȂ�
            'mcls_Img.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth


            ' ����\�O���w��
            dResX = IIf(dResX > 0, dResX, rpiHead.res_x)
            dResY = IIf(dResY > 0, dResY, rpiHead.res_y)
            Call mcls_Img.SetImgInf(rpiHead.width, rpiHead.height, dResX, dResY, 0, 0, 1, 1) ' �摜�\�������͉��ŃZ�b�g
            ' ���T�C�Y�̏��� (����\��摜�����ȂǂŃX�N���[���\�ʂȂǂ��v�Z)
            Call ResizePaintAria()

            '�I�t�Z�b�g�n�����N���A�B
            m_mmLabelOffsetY = 0 '���x���I�t�Z�b�g
            m_nImgOffsetY = 0    '�摜�I�t�Z�b�g

            '' �����\�����������肵�����߁A���̕������͐�ǂ݈˗����Ă���
            'For ii As Integer = 1 To m_DspAriaInf.nDspFrameCnt
            '	Call mcls_List.Add(ii)
            'Next

            ' �����ړ�
            'Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X)
            Return True
        End Function


        ''' <summary>
        ''' RPI�t�@�C�� �N���[�Y
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub RpiClose()
            '' �t���O�ύX
            m_bIsRpi = False

            '' �N���[�Y����
            If mcls_Rpi IsNot Nothing Then
                Call mcls_Rpi.Dispose()
            End If

            '' �o�b�t�@�폜
            Call mcls_Cache.Init()
            '' ����Ǎ��ݗp������
            For ii As Integer = 0 To mcls_MultiExec.Count - 1
                Call mcls_MultiExec(ii).RpiClose()
            Next
        End Sub

        ''' <summary>
        ''' �ĕ`��v��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub RePaint()
            mcls_Img.Refresh()
        End Sub


        ''' <summary>
        ''' �c�X�N���[��
        ''' </summary>
        ''' <param name="nScrollY">�X�N���[���o�[��[DP]</param>
        ''' <param name="bIsProgramable">�v���O�����ŏ���ɕς����ꍇtrue</param>
        ''' <remarks></remarks>
        Public Sub ScrollH(ByVal nScrollY As Integer, Optional ByVal bIsProgramable As Boolean = False)
            Dim nTotalOffsetY As Integer = nScrollY * m_nScrollPitch
            If nTotalOffsetY > scrNagateMaximumVirtual Then
                nTotalOffsetY = scrNagateMaximumVirtual
            End If
            Call SetNewImgPosition(nTotalOffsetY, m_DspAriaInf.DspPixelTotalOffset.X)

            If bIsProgramable AndAlso Not scrNagate Is Nothing Then scrNagateValueProgramable = nScrollY
        End Sub

        ''' <summary>
        ''' ���X�N���[��
        ''' </summary>
        ''' <param name="nTotalOffsetX">���[����̃I�t�Z�b�g��[DP]</param>
        ''' <param name="bIsProgramable">�v���O�����ŏ���ɕς����ꍇtrue</param>
        ''' <remarks></remarks>
        Public Sub ScrollW(ByVal nTotalOffsetX As Integer, Optional ByVal bIsProgramable As Boolean = False)
            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, nTotalOffsetX)

            If bIsProgramable AndAlso Not scrHaba Is Nothing Then scrHabaValueProgramable = nTotalOffsetX
        End Sub







        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �d�v����
        '/ /////////////////////////////////////////////////////////////////////////////////




        ''' <summary>
        ''' �t���[���摜�\���p�̃f�[�^��`��N���X�ɃZ�b�g
        ''' </summary>
        ''' <param name="nTotalOffsetY">0�t���[���擪����̃I�t�Z�b�g��(�\�����W�n)</param>
        ''' <param name="nTotalOffsetX">���[����̃I�t�Z�b�g��(�\�����W�n)</param>
        Private Sub SetNewImgPosition(ByVal nTotalOffsetY As Integer, Optional ByVal nTotalOffsetX As Integer = 0)

            '' �V�\���t���[��No ������
            Dim nNewDspFno As Integer           ' �w��ʒu�ɑ΂��� �\���擪�t���[��No (0�I���W��)
            Dim nNewOneOffsetY As Integer       ' �w��ʒu�ɑ΂��� �\���擪�t���[������̃I�t�Z�b�g�ʒu
            Call GetOffset_TotalToOne(nTotalOffsetY, nNewDspFno, nNewOneOffsetY)

            ' �}�C�i�X�̏ꍇ�́A�}�C�i�X�t���[���������Z
            If nNewDspFno <= 0 Then
                'If nNewDspFno < 0 AndAlso nNewDspFno <= m_DspAriaInf.nDspFno * -1 Then

                'nNewOneOffsetY += CInt(nNewDspFno * m_DspAriaInf.DspImgSize.Height) * -1
            End If
            ' ���[���тʂ��ĕ\������Ƃ��̑΍�
            If nNewDspFno > mcls_Rpi.GetRpiHeader.nframe Then
                nNewDspFno = mcls_Rpi.GetRpiHeader.nframe
            End If

            '' �㉺���]��
            If mcls_Img.tProp_ImgKind.bIsTurnTB Then
                nNewDspFno = mcls_Rpi.GetRpiHeader.nframe - nNewDspFno - m_DspAriaInf.nDspFrameCnt        ' �t���[���ʒu���t���琔����
                If nNewDspFno < 0 Then                                                              ' �擪�t���[�������̏ꍇ
                    nNewOneOffsetY += CInt(nNewDspFno * m_DspAriaInf.DspImgSize.Height) * -1    ' �������̕\���I�t�Z�b�g�𒲐�
                    nNewDspFno = 0                                                                  ' 0�t���[���ڂɌŒ� ��
                End If
            End If


            Try
                '''' �����o�b�t�@�ϐ����X�V
                mcls_Img.tProp_AriaInf_Offset = New Point(nTotalOffsetX * -1, nNewOneOffsetY * -1)  ' �\���擪�t���[������̃I�t�Z�b�g�l���Z�b�g
                m_DspAriaInf.DspPixelTotalOffset.Y = nTotalOffsetY  ' �擪����̃I�t�Z�b�g�l
                m_DspAriaInf.DspPixelTotalOffset.X = nTotalOffsetX


                '''' �\������Ώۂ̉摜���ύX���ꂽ�ꍇ�̂�
                If m_DspAriaInf.nDspFno <> nNewDspFno Or Not m_DspAriaInf.bDspAllOk Then
                    '' �\���Ώۉ摜�擾
                    Dim nDspFnoE As Integer = nNewDspFno + m_DspAriaInf.nDspFrameCnt    ' �ŏI�t���[��No (1�I���W��)
                    If nDspFnoE > mcls_Rpi.GetRpiHeader.nframe Then nDspFnoE = mcls_Rpi.GetRpiHeader.nframe
                    Dim nSa As Integer = nNewDspFno - m_DspAriaInf.nDspFno
                    Dim nMode As Integer
                    If nSa = 0 Then
                        nMode = 0
                    ElseIf nSa > 0 And Math.Abs(nSa) < m_DspAriaInf.nDspFrameCnt Then
                        nMode = 1
                    ElseIf nSa < 0 And Math.Abs(nSa) < m_DspAriaInf.nDspFrameCnt Then
                        nMode = -1
                    Else
                        nMode = 2
                    End If

                    '' �f�[�^�Z�b�g
                    mcls_Cache.Disp(nNewDspFno + 1, nDspFnoE, nSa)
                    m_DspAriaInf.bDspAllOk = False
                    Call ReSetImg(nNewDspFno)


                    '' �\���擪�t���[�����ύX
                    m_DspAriaInf.nDspFno = nNewDspFno
                    m_DspAriaInf.nDspFnoEnd = nDspFnoE - 1

                    ' �͈͂��ύX���ꂽ
                    RaiseEvent tEv_ChangeDspFrameNo(nNewDspFno + 1, nDspFnoE)
                End If

            Catch ex As Exception
                If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.ERR, String.Format("fno={0} offset={1} �\���f�[�^�Z�b�g�ُ�", nNewDspFno, nTotalOffsetY))
            End Try
        End Sub

        ''' <summary>
        ''' �`��N���X�ɉ摜�f�[�^���Z�b�g����
        ''' </summary>
        ''' <param name="nDspFnoS">�J�n�t���[��No(0�I���W��)</param>
        ''' <remarks></remarks>
        Public Sub ReSetImg(ByVal nDspFnoS As Integer)
            Try
                If Not m_DspAriaInf.bDspAllOk Then
                    Dim nDspFnoE As Integer = nDspFnoS + m_DspAriaInf.nDspFrameCnt  ' �ŏI�t���[��No (1�I���W��)
                    If nDspFnoE > mcls_Rpi.GetRpiHeader.nframe Then nDspFnoE = mcls_Rpi.GetRpiHeader.nframe

                    Dim bAllOk As Boolean = mcls_Cache.CheckDispImg(nDspFnoS + 1, nDspFnoE)

                    '' �摜�\���N���X�Ƀt���[���f�[�^��n��
                    Dim pImgWk(m_DspAriaInf.nDspFrameCnt - 1) As Image
                    For ii As Integer = nDspFnoS + 1 To nDspFnoE
                        If 0 = mcls_Cache.g_lstImg.Count Then Continue For
                        If Not mcls_Cache.g_lstImg(ii).bSyncExce Then
                            'If Not mcls_List.g_lstImg(ii).bImgDspSizeLimit And _
                            '  100 < Math.Abs(m_DspAriaInf.DspImgSize.Width - mcls_List.g_lstImg(ii).nDspW) Or _
                            '  100 < Math.Abs(m_DspAriaInf.DspImgSize.Height - mcls_List.g_lstImg(ii).nDspH) Then


                            'Debug.Print("�摜�T�C�Y={0},{1}, �\��={2}", mcls_List.g_lstImg(ii).nDspW, mcls_List.g_lstImg(ii).nDspH, m_DspAriaInf.DspImgSize.Width)

                            If mcls_Cache.g_lstImg(ii).bImgDspSizeLimit AndAlso Not mcls_Cache.g_lstImg(ii).pImgRaw Is Nothing Then
                                pImgWk(ii - nDspFnoS - 1) = mcls_Cache.g_lstImg(ii).pImgRaw
                            Else
                                pImgWk(ii - nDspFnoS - 1) = mcls_Cache.g_lstImg(ii).pImgDsp
                            End If
                        Else
                            pImgWk(ii - nDspFnoS - 1) = Nothing
                        End If
                    Next


                    'If bAllOk Then
                    Call mcls_Img.SetImage(pImgWk, False) ' �摜�́A���̒��ł͊J�����Ȃ����� false
                    'End If

                    m_DspAriaInf.bDspAllOk = bAllOk
                End If
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' �C���[�W��S����蒼��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ReCreateImg()
            m_DspAriaInf.bDspAllOk = False
            Dim pImgWk(m_DspAriaInf.nDspFrameCnt - 1) As Image
            Call mcls_Img.SetImage(pImgWk, False)
            Call mcls_Cache.Delete(-1, -1, 0)

            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X)
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ���J ��{�摜���쏈��
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �t���[���摜�\���s�N�`���[�{�b�N�X�T�C�Y�ύX���̏���
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ResizePaintAria()
            Try
                mcls_Img.SetAriaInf()
                mcls_Img.Calculation()                                          ' �ύX�����T�C�Y�ŉ��v�Z

                ''�����\���̂��߂ɁA�\���G���A�̏k�ڂɉ摜�����킹�Ă���
                m_DspAriaInf.DspImgSize.Width = mcls_Img.tProp_ImgInf.nOneImgW * mcls_Img.tProp_CalcAns.RateDsp.dW
                m_DspAriaInf.DspImgSize.Height = mcls_Img.tProp_ImgInf.nOneImgH * mcls_Img.tProp_CalcAns.RateDsp.dH

                '' �ύX�T�C�Y��ŏc�ɉ����t���[���摜���\���ł���H (�؂�グ)
                ' +1 ����̂��~�\�B��f�P�ʂňړ�������ƁA�㉺�Ńt���[���r���̉摜��\������K�v������ꍇ���o�Ă��邽��

                Dim pxAreaNagate As Integer = mcls_Img.tProp_AriaInf.nAriaHeight

                m_DspAriaInf.nDspFrameCnt = CInt(Math.Ceiling(pxAreaNagate / m_DspAriaInf.DspImgSize.Height)) + 1


                ' �ύX��̃t���[������ݒ�
                Call mcls_Img.ChangeImgCnt(1, m_DspAriaInf.nDspFrameCnt)                                            ' �摜�\���N���X
                Call mcls_Cache.ChangeDspImgSize(CInt(m_DspAriaInf.DspImgSize.Width), CInt(m_DspAriaInf.DspImgSize.Height))  ' RPI�t�@�C���Ǎ���

                ' �k���摜�ƕ\���̈悪1pix�Ƃ��Y���āA�`�悪bitbilt�ɂȂ�Ȃ��΍�B�R�����g���ŁA���o�[�W�����œ���\
                Call mcls_Cache.SetupImageSizeMethod(AddressOf mcls_Img.GetAnsOneImgSize)



                '' ��\���G���A�T�C�Y���Z�o
                If m_bIsRpi Then
                    Dim rpiHead As tClass_ReadRPI.RPI_HEADER = mcls_Rpi.GetRpiHeader()
                    ' �c
                    Dim nTotalPixelY As Integer = GetOffsetY_OneToTotal(rpiHead.nframe, 0)
                    'Dim nTotalPixelY As Integer = GetOffsetY_OneToTotal(rpiHead.nframe, 0) + m_nImgOffsetY
                    m_DspAriaInf.ImgNonAriaSize.Height = CInt(Math.Ceiling(nTotalPixelY / m_nScrollPitch)) - pxAreaNagate
                    If 0 > m_DspAriaInf.ImgNonAriaSize.Height Then m_DspAriaInf.ImgNonAriaSize.Height = 0
                    ' ��
                    Dim nTotalPixelX As Integer = GetOffsetX_OneToTotal(0)
                    m_DspAriaInf.ImgNonAriaSize.Width = nTotalPixelX - mcls_Img.tProp_AriaInf_Aria.Width
                    If 0 > m_DspAriaInf.ImgNonAriaSize.Width Then m_DspAriaInf.ImgNonAriaSize.Width = 0
                End If
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' �X�N���[���o�[ �ő�T�C�Y�ύX
        ''' </summary>
        ''' <param name="bInit">���S��������true</param>
        ''' <remarks></remarks>
        Private Sub SetCommonScrollBar(ByVal bInit As Boolean)
            ' ����X�N���[���o�[ �L����
            If Not scrNagate Is Nothing Then
                If 0 >= m_DspAriaInf.ImgNonAriaSize.Height Then
                    scrNagate.Enabled = False
                    scrNagate.Value = 0
                    scrNagate.Maximum = scrNagate.LargeChange
                Else

                    ' �G���[�ɂȂ�Ȃ��悤�ɑ΍�
                    'If scrNagate.Value > m_DspAriaInf.ImgNonAriaSize.Height + 1 Then
                    '	scrNagate.Value = m_DspAriaInf.ImgNonAriaSize.Height + 1
                    'End If
                    ' �G���[�ɂȂ�Ȃ��悤�ɑ΍� (���������Ȃ�����Ȃ��Ƃ����Ă��邩�v�����B�K�v�Ȃ��̂ł́H)
                    If scrNagate.Value > m_DspAriaInf.ImgNonAriaSize.Height + 1 Then
                        If (m_DspAriaInf.ImgNonAriaSize.Height + 1) > scrNagate.Maximum Then
                            scrNagate.Value = scrNagate.Maximum
                        ElseIf (m_DspAriaInf.ImgNonAriaSize.Height + 1) < scrNagate.Minimum Then
                            scrNagate.Value = scrNagate.Minimum
                        Else
                            scrNagate.Value = m_DspAriaInf.ImgNonAriaSize.Height + 1
                        End If
                    End If

                    If m_dLenOffset(1) = 0.0 Then
                        ' �T�C�Y�ύX�ɂ��X�N���[���o�[�l�ύX								��������ƃ~�\�B(maxval-LargeChange+1)���ő��value�ƂȂ邽�߂̏��u�B
                        scrNagate.Maximum = m_DspAriaInf.ImgNonAriaSize.Height + scrNagate.LargeChange
                    Else
                        scrNagate.Maximum = GetLenOff_Dp(1) - mcls_Img.tProp_AriaInf.nAriaHeight + scrNagate.LargeChange
                    End If
                    If m_dLenOffset(0) = 0.0 Then
                        scrNagate.Minimum = 0
                    Else
                        scrNagate.Minimum = GetLenOff_Dp(0)      ' ���min��ύX����ƁALargeChange �̒l���傫���Ƃ��ɁA�����LargeChange���ύX����邽�߂̑΍�
                    End If

                    scrNagate.Enabled = True
                End If
            End If


            ' ���X�N���[���o�[ �L����
            If Not scrHaba Is Nothing Then

                If 0 >= m_DspAriaInf.ImgNonAriaSize.Width Then
                    scrHaba.Enabled = False
                    scrHaba.Maximum = 0
                Else

                    If scrHaba.Value > m_DspAriaInf.ImgNonAriaSize.Width + 1 Then
                        scrHaba.Value = m_DspAriaInf.ImgNonAriaSize.Width + 1
                    End If
                    scrHaba.Maximum = m_DspAriaInf.ImgNonAriaSize.Width + scrHaba.LargeChange '+ 1
                    scrHaba.Enabled = True
                End If
            End If

            ' �����L�� ������ ��ł��������悢�̂ł́H
            If bInit Then
                If Not scrNagate Is Nothing Then scrNagate.Value = scrNagate.Minimum '0
                If Not scrHaba Is Nothing Then scrHaba.Value = scrHaba.Minimum '0
            End If
        End Sub




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ���� �C�x���g�̃G���g���[
        '/ /////////////////////////////////////////////////////////////////////////////////



        ''' <summary>
        ''' �s�N�`���[�{�b�N�X�T�C�Y�ύX�C�x���g
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Public Sub ResizePaintAria(ByVal sender As Object, ByVal e As System.EventArgs)
            '' �T�C�Y�`�F�b�N
            ' �����[�g�f�X�N�g�b�v�œ����āA�ؒf���ɁA�Ȃ���resize�ʒm�����āA�T�C�Y��0�ɂȂ��Ă���΍�
            If mcntl_Item.picDspImg.ClientSize.Width < 100 Or mcntl_Item.picDspImg.ClientSize.Height < 100 Then
                Return
            End If

            Dim oldSize As Size = mcls_Img.tProp_AriaInf_Aria       ' �O��l

            '' ���̍���̃I�t�Z�b�g�l�����摜�ł�Pixel�ɕϊ�
            ' ��ŁA�T�C�Y�ύX��̃I�t�Z�b�g�l�����߂��
            Dim dNowLenY As Double
            Dim dNowLenX As Double

            If m_bIsRpi Then
                dNowLenY = GetPos_DpToLen_Y(m_DspAriaInf.DspPixelTotalOffset.Y)
                dNowLenX = GetPos_DpToLen_X(m_DspAriaInf.DspPixelTotalOffset.X)
            End If

            ' ���T�C�Y�̏���
            Call ResizePaintAria()


            '' �`�悵�Ă���ꍇ�̂�
            If m_bIsRpi Then
                Dim nTotalOffset As Point
                nTotalOffset.X = GetPos_LenToDp_X(dNowLenX)
                nTotalOffset.Y = GetPos_LenToDp_Y(dNowLenY)
                m_DspAriaInf.DspPixelTotalOffset.Y = nTotalOffset.Y
                m_DspAriaInf.DspPixelTotalOffset.X = nTotalOffset.X
                m_DspAriaInf.bDspAllOk = False

                Call SetCommonScrollBar(False)                              ' �X�N���[���o�[�� ��`�ύX
                Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X)
                If Not scrNagate Is Nothing Then scrNagateValueProgramable = m_DspAriaInf.DspPixelTotalOffset.Y
                If Not scrHaba Is Nothing Then scrHabaValueProgramable = m_DspAriaInf.DspPixelTotalOffset.X
            End If
            RaiseEvent tEv_ReSize(oldSize)

        End Sub
        ''' <summary>
        ''' ���[�U�[�����`��
        ''' </summary>
        ''' <param name="e">�O���t�B�b�N�I�u�W�F�N�g</param>
        ''' <remarks></remarks>
        Private Sub Ev_Paint_Start(ByVal e As System.Windows.Forms.PaintEventArgs)
            RaiseEvent tEv_Paint_Start(e)
        End Sub

        ''' <summary>
        ''' ���[�U�[�ŏI�`��
        ''' </summary>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub Ev_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
            RaiseEvent tEv_Paint_End(e)
        End Sub

        ''' <summary>
        ''' �I�[�o�[���C�N���b�N
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub Ev_OverLay_Click(ByVal sender As tClass_ImageDsp.OVERLAY_INF, ByVal e As Rectangle)
            RaiseEvent tEv_OverLay_Click(sender, e)
        End Sub

        ''' <summary>
        ''' �������Ă��Ȃ��摜�̓W�J�v�� (���N���X���Ő���) (�}���`�X���b�h���ŃR�[�����邽�߁A��ʂƂ͕ʃX���b�h)
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub Ev_DecodeImage(ByRef img As Bitmap, ByVal byData() As Byte, ByVal nDataSize As Integer, ByVal rpiHeader As tClass_ReadRPI.RPI_HEADER)
            RaiseEvent tEv_DecodeImage(img, byData, nDataSize, rpiHeader)
        End Sub

        ''' <summary>
        ''' ���摜 �ɑ΂���摜�����v�� (���N���X���Ő���) (�}���`�X���b�h���ŃR�[�����邽�߁A��ʂƂ͕ʃX���b�h)
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub Ev_CreatingImg(ByVal p As CLS_IMG_INF, ByRef img As Bitmap)
            RaiseEvent tEv_CreatingImg(p, img)
        End Sub

    End Class
End Namespace
