'*********************************************************************************
'RPI�摜�\���ɓ����������N���X
'	[Ver]
'		Ver.01    2009/02/20  vs2005 �Ή�
'
'	[����]
'		�E��1�t���[�� �c?�t���[�� �`����Pixel�P�ʂł��炵�ĕ\���������p�N���X
'		�E��{�I�ɂ��̃N���X�̒��ŕ`��͂��Ȃ��B(= �ʒu�ύX�Ƃ�����Ă��g���ق��ŕ`����R�[�����鎖�B���ꂪ�J��Ԃ��`��̌����ւƂȂ���)
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.Windows.Forms
Imports System.Drawing

Imports tClassLibrary


Namespace tClass
    Public Class tClass_ViewRPI
        Implements IDisposable															' �f�X�g���N�^��`


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �O���C�x���g
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' ���摜 �ɑ΂���摜�����v�� (���N���X���Ő���)
        ''' </summary>
        ''' <param name="fno">����Ώۂ̃t���[��No(1�I���W��)</param>
        ''' <param name="img">(in/out)�C���Ώۂ̉摜�N���X</param>
        ''' <remarks></remarks>
        Public Event tEv_CreatingImg(ByVal fno As Integer, ByRef img As Bitmap)

        ''' <summary>
        ''' �\���擪�t���[��No���ύX�A���́A�\���͈͂��ύX (���N���X���Ő���)
        ''' </summary>
        ''' <param name="nFrameNoStart">����̕\���擪�t���[��No(1�I���W��)</param>
        ''' <param name="nFrameNoEnd">����̕\����[�t���[��No(1�I���W��)</param>
        ''' <remarks></remarks>
        Public Event tEv_ChangeDspFrameNo(ByVal nFrameNoStart As Integer, ByVal nFrameNoEnd As Integer)

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
        '/ �v���C�x�[�g�萔
        '/ /////////////////////////////////////////////////////////////////////////////////
        Private Const mcIMGBUF_SP As Integer = 2										' �p�t�H�[�}���X�΍��p�̓����摜�ێ��������l

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ���J�\����
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �g�p�R���g���[�� �󂯓n���\����
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure CNTL_ITEM
            ''' <summary>�摜�\���s�N�`���[</summary>
            Dim picDspImg As PictureBox													' �摜�\���s�N�`���[
            ''' <summary>�c�X�N���[���o�[ (NULL����)</summary>
            Dim vsBar As tCnt.tCnt_VScrollBar											' �c�X�N���[���o�[ (NULL����)
            ''' <summary>���X�N���[���o�[ (NULL����)</summary>
            Dim hsBar As tCnt.tCnt_HScrollBar											' ���X�N���[���o�[ (NULL����)
        End Structure

        ''' <summary>
        ''' �摜�\�����
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure DSP_ARIA_INF
            Dim DspPixelTotalOffset As Point											' �\�����W�n�ł̕\������܂ł̃g�[�^���I�t�Z�b�gpixel�l
            Dim nDspFno As Integer														' �\���擪�̃t���[��No (0�I���W��)
            Dim nDspFrameCnt As Integer													' ��ʂɉ��t���[���\���\���H(�؂�グ)

            Dim ImgNonAriaSize As Size													' �\���G���A�ɕ\������Ă��Ȃ������̉摜�T�C�Y (�\���{���ɕϊ��������Pixel��) = �X�N���[���o�[�̗ʂƂȂ�

            Dim nImgBufFno As Integer													' �o�b�t�@���̐擪�t���[��No (0�I���W��)
            Dim ImgBuf() As Bitmap														' �p�t�H�[�}���X�΍��p�̓����摜 (�V�F�[�f�B���O�␳���̕ϊ���̉摜)
        End Structure



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����o�[�ϐ�
        '/ /////////////////////////////////////////////////////////////////////////////////
        ' �O���Z�b�g�N���X
        Private mcls_pLog As tClass.tClass_LogManager									' ���O�Ǘ��N���X (�|�C���^)

        ' �O���Z�b�g�I�u�W�F�N�g
        Private mcntl_Item As CNTL_ITEM													' �O���Z�b�g�I�u�W�F�N�g

        ' �������N���X
        Private mcls_Rpi As New tClass.tClass_ReadRPIFile									' RPI�t�@�C���Ǎ��N���X
        Private mcls_Img As New tClass.tClass_ImageDsp									' �摜�\���N���X 

        ' ������ԊǗ�
        'Private m_bIsPaint As Boolean = False											' Paint�`��\��
        Private m_bIsRpi As Boolean = False												' �t�@�C���A�N�Z�X����������
        Private m_DspAriaInf As DSP_ARIA_INF											' �摜�\�����
        Private m_nImgBufSp(1) As Integer												' �p�t�H�[�}���X�΍��p�̓����摜�ێ���(0:�O 1:��) (���̐�+�\���\�ȃt���[���� �� ���v�ƂȂ�)


        ' ���̑��������


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����N���X �� �����J�p �֌W
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' RPI�w�b�_�擾
        ''' </summary>
        Public ReadOnly Property GetRpi_Header() As tClass.tClass_ReadRPI.RPI_HEADER
            Get
                Return mcls_Rpi.GetRpiHeader
            End Get
        End Property
        ''' <summary>
        ''' RPI�t���[���A�N�Z�X
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>RPI�t���[�����</returns>
        Public ReadOnly Property GetRpi_FrameInf(ByVal fno As Integer) As tClass.tClass_ReadRPI.RPI_FRAME_INF
            Get
                Return mcls_Rpi.GetRpiFrameInf(fno)
            End Get
        End Property

        ''' <summary>
        ''' �摜�����擾
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property GetImg_ImgInf() As tClass.tClass_ImageDsp.IMG_INF
            Get
                Return mcls_Img.tProp_ImgInf
            End Get
        End Property
        ''' <summary>
        ''' �\���`�����擾
        ''' </summary>
        Public ReadOnly Property GetImg_ImgKind() As tClass.tClass_ImageDsp.IMG_KIND
            Get
                Return mcls_Img.tProp_ImgKind
            End Get
        End Property
        ''' <summary>
        ''' �\���̈���擾
        ''' </summary>
        Public ReadOnly Property GetImg_AriaInf() As tClass.tClass_ImageDsp.ARIA_INF
            Get
                Return mcls_Img.tProp_AriaInf
            End Get
        End Property
        ''' <summary>
        ''' ���W�ŏI���ʎ擾
        ''' </summary>
        Public ReadOnly Property GetImg_CalcAns() As tClass.tClass_ImageDsp.CALC_ANS
            Get
                Return mcls_Img.tProp_CalcAns
            End Get
        End Property




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
        ''' ��ʍ�����DS/WS�敪
        ''' </summary>
        ''' <param name="nMode">DS:0 WS:1</param>
        ''' <remarks></remarks>
        Public Sub SetProp_DSWS(ByVal nMode As Integer)
            'm_nDsWs = nMode

            ' �f�t�H���g�ł́A��ʍ����́ADS
            If 0 = nMode Then mcls_Img.tProp_ImgKind_IsTurnLR = False
            If 1 = nMode Then mcls_Img.tProp_ImgKind_IsTurnLR = True
        End Sub


        ''' <summary>
        ''' �摜�g��
        ''' </summary>
        ''' <param name="dRate">�g��{��</param>
        ''' <remarks></remarks>
        Public Sub SetProp_Zoom(ByVal dRate As Double)
            mcls_Img.tProp_AriaInf_Zoom = New tClass.tClass_ImageDsp_Calc.RATE(dRate, dRate)
        End Sub

        ''' <summary>
        ''' �摜�g��
        ''' </summary>
        ''' <param name="dRateX">�g��{��(x)</param>
        ''' <param name="dRateY">�g��{��(x)</param>
        ''' <remarks></remarks>
        Public Sub SetProp_Zoom(ByVal dRateX As Double, ByVal dRateY As Double)
            mcls_Img.tProp_AriaInf_Zoom = New tClass.tClass_ImageDsp_Calc.RATE(dRateX, dRateY)
        End Sub

        ''' <summary>
        ''' �摜�\�����
        ''' </summary>
        Public Sub SetProp_ImgKind_StrechType(Optional ByVal em As tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH = tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth)
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
        ''' �p�t�H�[�}���X�΍��p�̓����摜�ێ���(0:�O 1:��)
        ''' </summary>
        Public Property PropImgBufSp(ByVal Index As Integer) As Integer
            Get
                Return m_nImgBufSp(Index)
            End Get
            Set(ByVal value As Integer)
                m_nImgBufSp(Index) = value
                ' ��
                ' �����Ȃ�ς��Ă����H
            End Set
        End Property

        ''' <summary>
        ''' �t���[���t�����擾 (�t���[���P�ʂ̃G�b�W�ʒu)
        ''' </summary>
        ''' <param name="fno">�t���[��No (1�I���W��)</param>
        ''' <returns>�t���[���P�ʂ̃G�b�W�ʒu</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_EdgePerFrame(ByVal fno As Integer) As tClass_ReadRPI.EDGE_PER_FRAME
            Return mcls_Rpi.ReadFile_FrameInfo_EdgePerFrame(fno)
        End Function

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ���ʌ��J�����֌W
        '/ /////////////////////////////////////////////////////////////////////////////////
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
        End Sub
        Public Function GetOffset_TotalToOne_FrameNo(ByVal nTotalOffsetY As Integer) As Integer
            Dim OneImgPixel As Double = mcls_Img.tProp_ImgInf.nOneImgH * mcls_Img.tProp_CalcAns.RateDsp.dH
            Return CInt(Math.Floor(nTotalOffsetY / OneImgPixel))
        End Function
        Public Function GetOffset_TotalToOne_OffsetY(ByVal nTotalOffsetY As Integer) As Integer
            Dim OneImgPixel As Double = mcls_Img.tProp_ImgInf.nOneImgH * mcls_Img.tProp_CalcAns.RateDsp.dH
            Return CInt(Math.Floor(nTotalOffsetY Mod OneImgPixel))
        End Function

        ''' <summary>
        ''' �Y���t���[��No�A�t���[���擪����̃I�t�Z�b�g����\�����W�擪����S�̂̃I�t�Z�b�g�ʂ��Z�o
        ''' </summary>
        ''' <param name="nFrameNo">�w�肵���t���[��No(0�I���W��)</param>
        ''' <param name="nOneOffsetY">���̃t���[�����ł̃I�t�Z�b�g��</param>
        ''' <returns>�\�����W�n �擪����̃g�[�^���I�t�Z�b�g��</returns>
        ''' <remarks></remarks>
        Public Function GetOffsetY_OneToTotal(ByVal nFrameNo As Integer, ByVal nOneOffsetY As Integer) As Integer
            Return CInt(Math.Floor(nFrameNo * mcls_Rpi.GetRpiHeader().height * mcls_Img.tProp_CalcAns.RateDsp.dH)) + nOneOffsetY
        End Function
        Public Function GetOffsetX_OneToTotal(ByVal nOneOffsetX As Integer) As Integer
            Return CInt(Math.Floor(mcls_Rpi.GetRpiHeader().width * mcls_Img.tProp_CalcAns.RateDsp.dW)) + nOneOffsetX
        End Function

        ''' <summary>
        ''' ���̉摜���W����\���p�̍��W�ɕϊ�
        ''' </summary>
        Public Overloads Function GetSimplePoint_ImgToDsp(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetSimplePoint_ImgToDsp(x, y)
        End Function

        ''' <summary>
        ''' �\���p�̍��W���猳�̉摜���W�ɕϊ�
        ''' </summary>
        Public Overloads Function GetSimplePoint_DspToImg(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetSimplePoint_DspToImg(x, y)
        End Function


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����Z�b�g
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' ���O�N���X�Z�b�g
        ''' </summary>
        Public Sub SetCls_Log(ByRef cls As tClass.tClass_LogManager)
            mcls_pLog = cls

            ' �z���̃N���X�ɂ��Z�b�g
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
        ''' <remarks></remarks>
        Public Sub Alloc(ByRef cntl As CNTL_ITEM)
            '''' �R���g���[����o�^
            mcntl_Item = cntl


            ''''' �摜�\���N���X �� ���� 2
            ' �`��R���g���[���Z�b�g
            Call mcls_Img.AddHandler_PaintEvent(mcntl_Item.picDspImg)							' �y�C���g�C�x���g�o�^ & �s�N�`���[�o�^
            Call mcls_Img.AddHandler_OverLayClick()												' �s�N�`���[�N���b�N�ŃI�[�o�[���C���N���b�N���������f �o�^


            '''' �����C�x���g�ƕR�t��
            ' �R���g���[���̃C�x���g�ƕR�t��
            AddHandler mcntl_Item.picDspImg.Resize, AddressOf ResizePaintAria					' �y�C���g�T�C�Y�ύX�C�x���g�o�^
            ' �����N���X�Ƃ̃C�x���g�R�t��
            AddHandler mcls_Img.tEv_Paint_Start, AddressOf Ev_Paint_Start						' �t���[���摜�ɓƎ��`��
            AddHandler mcls_Img.tEv_Paint_End, AddressOf Ev_Paint_End							' �t���[���摜�ɓƎ��`��
            AddHandler mcls_Img.tEv_OverLay_Click, AddressOf Ev_OverLay_Click					' �I�[�o�[���C�N���b�N


            '''' ���N���X������
            Call Init(True)

            '''' �T�C�Y�ύX�����Ńf�[�^��o�^
            Call ResizePaintAria(Nothing, Nothing)

            '''' �ĕ`��
            Call RePaint()
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �������֘A
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        Public Sub New()
            '''' ������돉����
            m_nImgBufSp(0) = mcIMGBUF_SP
            m_nImgBufSp(1) = mcIMGBUF_SP

            '''' �摜�\���N���X �� ���� 1
            mcls_Img.tProp_ImgKind_StrechType = tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth	' �\���G���A�̕��ɉ摜�������킹��
            mcls_Img.tProp_AriaInf_IsCenter = False												' �r���S�ʒu�ړ�����
            mcls_Img.SetImgInf(1024, 1024, 1, 1, 0, 0)											' �Ƃ肠�����������Ƃ��ăZ�b�g
            mcls_Img.tProp_Paint_IsDefImgDspn = True											' �f�t�H���g�摜�\����ON
            mcls_Img.tProp_Paint_IsPaint = True													' �摜�\����ON

            ' �Ƃ肠�����摜�f�[�^�̊������Ă��� (�������摜�\�����Ƀ��T�C�Y)
            m_DspAriaInf.nImgBufFno = -1
            ReDim m_DspAriaInf.ImgBuf(0)
        End Sub

        ''' <summary>
        ''' �f�X�g���N�^
        ''' </summary>
#Region " IDisposable Support "
        Private disposedValue As Boolean = False		' �d������Ăяo�������o����ɂ�

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��Ƀ}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�

                '''' �N���X���
                mcls_Rpi.RpiClose()
                mcls_Rpi.Dispose()
                mcls_Rpi = Nothing

                Call mcls_Img.AddHandler_OverLayClick()		' ���I�n���h���J��
                mcls_Img.Dispose()
                mcls_Img = Nothing

                ' �����摜�o�b�t�@�N���A
                Dispose_ImgBuf()
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
        ''' <param name="bInit">���S��������true   �p�x�ؑւȂǌ��ʒu����ێ��������ꍇfalse</param>
        Public Sub Init(ByVal bInit As Boolean)
            '''' RPI�t�@�C���N���[�Y
            Call RpiClose()
            ' �摜�\���G���A
            mcls_Img.tProp_Paint_IsDefImgDspn = True											' �f�t�H���g�摜�\����ON

            '''' �����ϐ��J��
            m_bIsRpi = False

            '''' �摜�\����񏉊���
            If bInit Then
                m_DspAriaInf.DspPixelTotalOffset.X = 0
                m_DspAriaInf.DspPixelTotalOffset.Y = 0
                m_DspAriaInf.nDspFno = -1
                m_DspAriaInf.nDspFrameCnt = 0
                m_DspAriaInf.ImgNonAriaSize.Height = 0
                m_DspAriaInf.ImgNonAriaSize.Width = 0
            End If
            ' �摜�o�b�t�@�N���A
            m_DspAriaInf.nImgBufFno = -1
            Call Dispose_ImgBuf()


            '''' �R���g���[�������Z�b�g
            If Not mcntl_Item.vsBar Is Nothing Then
                mcntl_Item.vsBar.Enabled = False
            End If
            If Not mcntl_Item.hsBar Is Nothing Then
                mcntl_Item.hsBar.Enabled = False
            End If
        End Sub

        ''' <summary>
        ''' �����摜�o�b�t�@�N���A
        ''' </summary>
        Private Sub Dispose_ImgBuf()
            For ii As Integer = 0 To m_DspAriaInf.ImgBuf.Length - 1
                Call Dispose_ImgBuf(ii)
            Next ii
        End Sub
        Private Sub Dispose_ImgBuf(ByVal Index As Integer)
            If m_DspAriaInf.ImgBuf.Length - 1 < Index Then Return
            If m_DspAriaInf.ImgBuf(Index) Is Nothing Then Return

            ' �J��
            m_DspAriaInf.ImgBuf(Index).Dispose()
            m_DspAriaInf.ImgBuf(Index) = Nothing
        End Sub



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ���J ��{�摜���쏈��
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' RPI�t�@�C�� �A�N�Z�X�J�n
        ''' </summary>
        ''' <param name="strFPath">RPI�t�@�C���̃t���p�X</param>
        ''' <param name="bInit">���S��������true   �p�x�ؑւȂǌ��ʒu����ێ��������ꍇfalse</param>
        ''' <param name="dResX">������\ (0�̏ꍇ�́ARPI�t�@�C�����g�p)</param>
        ''' <param name="dResY">�c����\ (0�̏ꍇ�́ARPI�t�@�C�����g�p)</param>
        ''' <returns></returns>
        Public Function RpiOpen(ByVal strFPath As String, Optional ByVal bInit As Boolean = True, Optional ByVal dResX As Single = 0, Optional ByVal dResY As Single = 0) As Boolean

            '''' RPI�t�@�C���̎��O�m�F
            ' RPI�t�@�C���̑��݊m�F
            Try
                Dim iof As New IO.FileInfo(strFPath)
                If Not iof.Exists() Then Return False
            Catch ex As Exception
                ' �����̃t�@�C������NULL�������ꍇ�́A�G���[�ƂȂ��
                Return False
            End Try

            '' RPI���N���X�Ńt�@�C���ǂݍ��݉\���`�F�b�N
            ' ���s�����ꍇ�́A���J���Ă���RPI�����̂܂ܕێ���������
            Using wk As New tClass.tClass_ReadRPIFile
                If 0 >= wk.RpiOpen(strFPath) Then
                    Return False
                End If
            End Using


            '''' ������RPI�t�@�C���Ǎ������J�n
            ' ��x ������
            Call Init(bInit)

            ' �t�@�C���I�[�v��
            If 0 >= mcls_Rpi.RpiOpen(strFPath) Then
                Call RpiClose()
                Return False
            End If

            ' �t���O�̕ύX
            m_bIsRpi = True
            mcls_Img.tProp_Paint_IsDefImgDspn = False											' �f�t�H���g�摜�\����OFF


            '''' �����v�Z
            ' �摜�\���N���X
            Dim rpiHead As tClass.tClass_ReadRPI.RPI_HEADER = mcls_Rpi.GetRpiHeader()
            ' ����\�O���w��
            dResX = CSng(IIf(dResX > 0, dResX, rpiHead.res_x))
            dResY = CSng(IIf(dResY > 0, dResY, rpiHead.res_y))
            Call mcls_Img.SetImgInf(rpiHead.width, rpiHead.height, dResX, dResY, 0, 0, 1, 1)   ' �摜�\�������͉��ŃZ�b�g
            ' ���T�C�Y�̏��� (����\��摜�����ȂǂŃX�N���[���\�ʂȂǂ��v�Z)
            Call ResizePaintAria()
            ' �ύX��̃t���[������ݒ�
            Call mcls_Img.ChangeImgCnt(1, m_DspAriaInf.nDspFrameCnt)
            ' �����ړ�
            Call SetCommonScrollBar(bInit)								' �X�N���[���o�[�� ��`�ύX
            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X, True, True)

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
            Call mcls_Rpi.RpiClose()
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
        ''' <param name="nTotalOffsetY">�S�̐擪����̃I�t�Z�b�g��</param>
        ''' <param name="bIsProgramable">�v���O�����ŏ���ɕς����ꍇtrue</param>
        ''' <remarks></remarks>
        Public Sub ScrollH(ByVal nTotalOffsetY As Integer, Optional ByVal bIsProgramable As Boolean = False)
            Call SetNewImgPosition(nTotalOffsetY, m_DspAriaInf.DspPixelTotalOffset.X, bIsProgramable)
        End Sub

        ''' <summary>
        ''' ���X�N���[��
        ''' </summary>
        ''' <param name="nTotalOffsetX">���[����̃I�t�Z�b�g��</param>
        ''' <param name="bIsProgramable">�v���O�����ŏ���ɕς����ꍇtrue</param>
        ''' <remarks></remarks>
        Public Sub ScrollW(ByVal nTotalOffsetX As Integer, Optional ByVal bIsProgramable As Boolean = False)
            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, nTotalOffsetX, bIsProgramable)
        End Sub

        ''' <summary>
        ''' �C���[�W�č�蒼��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ReCreateImg()
            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X, False, True)
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ��{�R���g���[������n
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' �t���[���摜������o�b�t�@�Ɏ�荞��
        ''' </summary>
        ''' <param name="nTotalOffsetY">0�t���[���擪����̃I�t�Z�b�g��(�\�����W�n)</param>
        ''' <param name="bIsClearBuffer">���̃o�b�t�@���g���܂킷�ꍇfalse</param>
        ''' <remarks></remarks>
        Private Sub CreateImgBuffer(ByVal nTotalOffsetY As Integer, Optional ByVal bIsClearBuffer As Boolean = False)
            Dim ii As Integer
            Dim nRecWk As Integer
            Dim imgwk As Bitmap

            Dim nOldImbBufFno(1) As Integer				' �������o�b�t�@��Index
            Dim nNewImbDspFno(1) As Integer				' ����̕\���G���A��Index
            nOldImbBufFno(0) = m_DspAriaInf.nImgBufFno
            nOldImbBufFno(1) = nOldImbBufFno(0) + m_DspAriaInf.ImgBuf.Length - 1
            nNewImbDspFno(0) = GetOffset_TotalToOne_FrameNo(nTotalOffsetY)
            nNewImbDspFno(1) = nNewImbDspFno(0) + m_DspAriaInf.nDspFrameCnt - 1


            '''' ����̕\���摜�́A�����o�b�t�@�̉摜�݂̂ŕ\���\�H
            If Not bIsClearBuffer Then
                If nOldImbBufFno(0) <= nNewImbDspFno(0) And nOldImbBufFno(0) <= nNewImbDspFno(1) And _
                   nOldImbBufFno(1) >= nNewImbDspFno(0) And nOldImbBufFno(1) >= nNewImbDspFno(1) Then
                    ' �S�摜�f�[�^����H
                    '   �ʏ킠�肦�Ȃ��͂��B�Ǝv�������ǁA���͈̔͂����߂鎞�ɁA�������R�[�h�̏ꍇ�Ƃ����ƁA�Ō�̔z�񂪉摜�����Ƃ����肦��B
                    For ii = nNewImbDspFno(0) To nNewImbDspFno(1)
                        nRecWk = ii - nOldImbBufFno(0)
                        If m_DspAriaInf.ImgBuf(nRecWk) Is Nothing Then
                            Exit For
                        End If
                    Next ii

                    ' �����o�b�t�@�̉摜�݂̂ŕ\���\�ȈׁA�������Ȃ�
                    If ii = nNewImbDspFno(1) + 1 Then Return
                End If
            Else
                ' ���̃o�b�t�@�͋����N���A
                Call Dispose_ImgBuf()
            End If


            '''' �����o�b�t�@����
            Dim OldImgBuf() As Bitmap = m_DspAriaInf.ImgBuf				' ���o�b�t�@���i�[
            Dim bOldImgBufRepeat(OldImgBuf.Length - 1) As Boolean		' ���o�b�t�@�̉摜���g���܂킷�ꍇ true
            Dim NewImgBuf(m_nImgBufSp(0) + m_nImgBufSp(1) + m_DspAriaInf.nDspFrameCnt - 1) As Bitmap	' �V�o�b�t�@�G���A���m��
            Dim nNewImbBufFno(1) As Integer								' ����̓����o�b�t�@��Index
            nNewImbBufFno(0) = nNewImbDspFno(0) - m_nImgBufSp(0)
            nNewImbBufFno(1) = nNewImbDspFno(1) + m_nImgBufSp(1)
            If nNewImbBufFno(0) < 0 Then nNewImbBufFno(0) = 0
            If nNewImbBufFno(1) > mcls_Rpi.GetRpiHeader.nframe - 1 Then nNewImbBufFno(1) = mcls_Rpi.GetRpiHeader.nframe - 1


            '''' �摜�Ǎ�
            Dim nNewBufIndex As Integer = 0
            For ii = nNewImbBufFno(0) To nNewImbBufFno(1)

                ' ���o�b�t�@�̔z��l �Z�o (-1=���o�b�t�@�����ɂ͑Ώۉ摜����)
                nRecWk = ii - nOldImbBufFno(0)
                If nRecWk < 0 Then nRecWk = -1
                If nRecWk > OldImgBuf.Length - 1 Then nRecWk = -1

                ' ���o�b�t�@�ɑΏۃt���[���̉摜�L��H
                If nRecWk <> -1 AndAlso Not OldImgBuf(nRecWk) Is Nothing Then
                    ' �g���܂킵
                    NewImgBuf(nNewBufIndex) = OldImgBuf(nRecWk)
                    bOldImgBufRepeat(nRecWk) = True

                Else

                    ' �t���[���f�[�^�����̕���
                    If ii > mcls_Rpi.GetRpiHeader.nframe Or ii < 0 Then
                        ' �_�~�[�t���[������
                        NewImgBuf(nNewBufIndex) = New Bitmap(mcls_Rpi.GetRpiHeader.width, mcls_Rpi.GetRpiHeader.height)	' ���[�̂�����Ƃ̉摜�́A�_�~�[���Z�b�g���Ă���
                    Else
                        ' �V�K�Ǎ�
                        imgwk = mcls_Rpi.ReadFile_Img(ii + 1)			' RPI�t�@�C������摜��荞��
                        RaiseEvent tEv_CreatingImg(ii + 1, imgwk)		' �O���ɉ摜�������˗�
                        NewImgBuf(nNewBufIndex) = imgwk					' ���[�N�ɃZ�b�g
                    End If
                End If


                nNewBufIndex += 1
            Next ii


            '''' ���o�b�t�@�Ŏg���Ȃ������摜���J��
            For ii = 0 To m_DspAriaInf.ImgBuf.Length - 1
                If Not bOldImgBufRepeat(ii) Then Call Dispose_ImgBuf(ii)
                m_DspAriaInf.ImgBuf(ii) = Nothing
            Next ii


            '''' �V�̈���m�ۂ��āA�摜���Z�b�g
            m_DspAriaInf.nImgBufFno = nNewImbBufFno(0)
            m_DspAriaInf.ImgBuf = NewImgBuf
        End Sub



        ''' <summary>
        ''' �t���[���摜�\��
        ''' </summary>
        ''' <param name="nTotalOffsetY">0�t���[���擪����̃I�t�Z�b�g��(�\�����W�n)</param>
        ''' <param name="nTotalOffsetX">���[����̃I�t�Z�b�g��(�\�����W�n)</param>
        ''' <param name="bIsProgramable">�v���O�����ŏ���ɕς����ꍇtrue</param>
        ''' <param name="bIsClearBuffer">���̃o�b�t�@���g���܂킷�ꍇfalse</param>
        ''' <remarks></remarks>
        Private Sub SetNewImgPosition(ByVal nTotalOffsetY As Integer, Optional ByVal nTotalOffsetX As Integer = 0, Optional ByVal bIsProgramable As Boolean = False, Optional ByVal bIsClearBuffer As Boolean = False)
            Dim nNewDspFno As Integer			' �w��ʒu�ɑ΂��� �\���擪�t���[��No
            Dim nNewOneOffsetY As Integer		' �w��ʒu�ɑ΂��� �\���擪�t���[������̃I�t�Z�b�g�ʒu
            Call GetOffset_TotalToOne(nTotalOffsetY, nNewDspFno, nNewOneOffsetY)

            Try
                '''' �����o�b�t�@�摜����
                Call CreateImgBuffer(nTotalOffsetY, bIsClearBuffer)

                '''' �����o�b�t�@�ϐ����X�V
                mcls_Img.tProp_AriaInf_Offset = New Point(nTotalOffsetX * -1, nNewOneOffsetY * -1)	' �\���擪�t���[������̃I�t�Z�b�g�l���Z�b�g
                m_DspAriaInf.DspPixelTotalOffset.Y = nTotalOffsetY		' �擪����̃I�t�Z�b�g�l
                m_DspAriaInf.DspPixelTotalOffset.X = nTotalOffsetX


                '''' �\������Ώۂ̉摜���ύX���ꂽ�ꍇ�̂�
                If m_DspAriaInf.nDspFno <> nNewDspFno Or bIsClearBuffer Then

                    '' �\������摜�̂ݎ��o��
                    Dim DspImg(m_DspAriaInf.nDspFrameCnt - 1) As Bitmap
                    Dim nImgBufIndex As Integer = nNewDspFno - m_DspAriaInf.nImgBufFno 'GetOffset_TotalToOne_FrameNo(nTotalOffsetY) - m_DspAriaInf.nImgBufFno
                    For ii As Integer = 0 To DspImg.Length - 1
                        DspImg(ii) = m_DspAriaInf.ImgBuf(nImgBufIndex + ii)
                    Next ii


                    '' �摜�\���N���X�Ƀt���[���f�[�^��n��
                    Call mcls_Img.SetImage(CType(DspImg, Image()))

                    '' �\���擪�t���[�����ύX
                    m_DspAriaInf.nDspFno = nNewDspFno

                    '' �͈͂��ύX���ꂽ
                    RaiseEvent tEv_ChangeDspFrameNo(nNewDspFno + 1, nNewDspFno + m_DspAriaInf.nDspFrameCnt)
                End If

                '' �X�N���[���o�[���f
                If bIsProgramable Then
                    If Not mcntl_Item.vsBar Is Nothing Then mcntl_Item.vsBar.ValueProgramable = nTotalOffsetY
                    If Not mcntl_Item.hsBar Is Nothing Then mcntl_Item.hsBar.ValueProgramable = nTotalOffsetX
                End If

            Catch ex As Exception
                'Call Paint_Stop()
                mcls_pLog.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("fno={0} offset={1} �\���ُ�", nNewDspFno, nTotalOffsetY))
            End Try

            Return
        End Sub

        ''' <summary>
        ''' �t���[���摜�\���s�N�`���[�{�b�N�X�T�C�Y�ύX���̏���
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub ResizePaintAria()
            Try
                '' �t���[���摜�\�� �T�C�Y�o�^
                'mcls_Img.tProp_AriaInf_Aria = mcntl_Item.picDspImg.ClientSize
                mcls_Img.SetAriaInf()
                mcls_Img.Calculation()			' �ύX�����T�C�Y�ōČv�Z


                '' ��\���G���A�T�C�Y���Z�o
                ' �c
                Dim nTotalPixelY As Integer = GetOffsetY_OneToTotal(mcls_Rpi.GetRpiHeader.nframe, 0)  'CInt(Math.Floor(mcls_Rpi.GetRpiHeader.nframe * mcls_Rpi.GetRpiHeader().height * mcls_Img.tProp_CalcAns.RateDsp.dH))
                m_DspAriaInf.ImgNonAriaSize.Height = nTotalPixelY - mcls_Img.tProp_AriaInf_Aria.Height
                If 0 > m_DspAriaInf.ImgNonAriaSize.Height Then m_DspAriaInf.ImgNonAriaSize.Height = 0
                ' ��
                Dim nTotalPixelX As Integer = GetOffsetX_OneToTotal(0)	'CInt(Math.Floor(mcls_Rpi.GetRpiHeader().width * mcls_Img.tProp_CalcAns.RateDsp.dW))
                m_DspAriaInf.ImgNonAriaSize.Width = nTotalPixelX - mcls_Img.tProp_AriaInf_Aria.Width
                If 0 > m_DspAriaInf.ImgNonAriaSize.Width Then m_DspAriaInf.ImgNonAriaSize.Width = 0


                '' �ύX�T�C�Y��ŏc�ɉ����t���[���摜���\���ł���H (�؂�グ)
                ' +1 ����̂��~�\�B��f�P�ʂňړ�������ƁA�㉺�Ńt���[���r���̉摜��\������K�v������ꍇ���o�Ă��邽��
                m_DspAriaInf.nDspFrameCnt = CInt(Math.Ceiling(mcls_Img.tProp_AriaInf.nAriaHeight / (mcls_Img.tProp_ImgInf.nOneImgH * mcls_Img.tProp_CalcAns.RateDsp.dH))) + 1
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' �X�N���[���o�[ �ő�T�C�Y�ύX
        ''' </summary>
        ''' <param name="bInit">���S��������true</param>
        ''' <remarks></remarks>
        Private Sub SetCommonScrollBar(ByVal bInit As Boolean)
            ' �c�X�N���[���o�[ �L����
            If Not mcntl_Item.vsBar Is Nothing Then
                If 0 >= m_DspAriaInf.ImgNonAriaSize.Height Then
                    mcntl_Item.vsBar.Enabled = False
                Else
                    ' �G���[�ɂȂ�Ȃ��悤�ɑ΍�
                    If mcntl_Item.vsBar.Value > m_DspAriaInf.ImgNonAriaSize.Height Then
                        mcntl_Item.vsBar.Value = m_DspAriaInf.ImgNonAriaSize.Height
                    End If
                    ' �T�C�Y�ύX�ɂ��X�N���[���o�[�l�ύX								��������ƃ~�\�B�X�N���[���o�[�̈ړ��ʂ�傫������ƁA����؂�Ȃ��ꍇ�́A�\������Ȃ��悤�ɂȂ�ׂ̏��u�B�Ō�̐���f�ɃS�~�������ǂ�
                    mcntl_Item.vsBar.Maximum = m_DspAriaInf.ImgNonAriaSize.Height + mcntl_Item.vsBar.LargeChange

                    mcntl_Item.vsBar.Enabled = True
                End If
            End If


            ' ���X�N���[���o�[ �L����
            If Not mcntl_Item.hsBar Is Nothing Then

                If 0 >= m_DspAriaInf.ImgNonAriaSize.Width Then
                    mcntl_Item.hsBar.Enabled = False
                Else

                    If mcntl_Item.hsBar.Value > m_DspAriaInf.ImgNonAriaSize.Width Then
                        mcntl_Item.hsBar.Value = m_DspAriaInf.ImgNonAriaSize.Width
                    End If
                    mcntl_Item.hsBar.Maximum = m_DspAriaInf.ImgNonAriaSize.Width

                    mcntl_Item.hsBar.Enabled = True
                End If
            End If

            ' �����L��
            If bInit Then
                If Not mcntl_Item.vsBar Is Nothing Then mcntl_Item.vsBar.Value = 0
                If Not mcntl_Item.hsBar Is Nothing Then mcntl_Item.hsBar.Value = 0
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

            '' ���̍���̃I�t�Z�b�g�l�����摜�ł�Pixel�ɕϊ�
            ' ��ŁA�T�C�Y�ύX��̃I�t�Z�b�g�l�����߂��
            Dim nTotalOffset As Point
            If m_bIsRpi Then
                nTotalOffset = mcls_Img.GetSimplePoint_DspToImg(m_DspAriaInf.DspPixelTotalOffset)	' �\�����W�n���猳�摜�n�ɕϊ�
            End If


            ' ���T�C�Y�̏���
            Call ResizePaintAria()

            ' �ύX��̃t���[������ݒ�
            Call mcls_Img.ChangeImgCnt(1, m_DspAriaInf.nDspFrameCnt)

            ' �`�悵�Ă���ꍇ�̂�
            If m_bIsRpi Then
                '' �T�C�Y�ύX��̃I�t�Z�b�g��ݒ肷��
                nTotalOffset = mcls_Img.GetSimplePoint_ImgToDsp(nTotalOffset)	' ���摜�n����\�����W�n�ɕϊ�
                If nTotalOffset.Y > m_DspAriaInf.ImgNonAriaSize.Height Then nTotalOffset.Y = m_DspAriaInf.ImgNonAriaSize.Height ' ������ƃ~�\�B�T�C�Y���k�߂��ꍇ�A��ԍŌ��pixel��\���G���A�̍Ō�Ɏ����Ă���悤�ɂ���
                If nTotalOffset.X > m_DspAriaInf.ImgNonAriaSize.Width Then nTotalOffset.X = m_DspAriaInf.ImgNonAriaSize.Width ' ������ƃ~�\�B�T�C�Y���k�߂��ꍇ�A��ԍŌ��pixel��\���G���A�̍Ō�Ɏ����Ă���悤�ɂ���
                m_DspAriaInf.DspPixelTotalOffset.Y = nTotalOffset.Y
                m_DspAriaInf.DspPixelTotalOffset.X = nTotalOffset.X

                Call SetCommonScrollBar(False)								' �X�N���[���o�[�� ��`�ύX
                Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X, True, True)
            End If
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
        ''' <remarks></remarks>s
        Private Sub Ev_OverLay_Click(ByVal sender As tClass_ImageDsp.OVERLAY_INF, ByVal e As Rectangle)
            RaiseEvent tEv_OverLay_Click(sender, e)
        End Sub
    End Class
End Namespace
