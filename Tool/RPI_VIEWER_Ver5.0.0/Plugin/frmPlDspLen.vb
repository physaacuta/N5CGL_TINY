Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin

Public Class frmPlDspLen
    Implements IPluginActivatable, IMouseToolStrip, IDrawer

    Private mcls_RpiView As tClass_ViewRpiThread
    Private mcls_pCalc As tClass_ImageDsp_Calc  ' �摜�v�Z�N���X (�|�C���^)

    Public ReadOnly Property PluginLevel As Single Implements IPluginActivatable.PluginLevel
        Get
            Return 100
        End Get
    End Property

    Public ReadOnly Property PluginName As String Implements IPluginActivatable.PluginName
        Get
            Return "frmPlDspLen"
        End Get
    End Property

    Dim maindoc As DisplayShrinker
    Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
        mcls_RpiView = v
        mcls_pCalc = mcls_RpiView.GetCls_ImgDsp()

        maindoc = PeropertiesBase.GetPropertyMember(Of DisplayShrinker)(mainfrm)

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔 �\���̒�`

    ''' <summary>
    ''' �v�Z���W�̍\����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure INFO_LEN
        Dim bIsData As Boolean          ' �L���ȃf�[�^�H

        Dim nFrameNo As Integer         ' �w�肵���_�̃t���[��No���Z�b�g
        Dim nPixelX As Integer          ' �t���[�����ォ��̋��� [RP] [pixel]
        Dim nPixelY As Integer          ' �t���[�����ォ��̋��� [RP] [pixel]
    End Structure

    ''' <summary>
    ''' ���ʍ\����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure ANS_LEN
        Dim dY As Double                ' ���� [mm]
        Dim dX As Double                ' �� [mm]
        Dim dL As Double                ' �Ε� [mm]
    End Structure



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    Private mfrm_pPopCont As frmPopControl      ' �R���g���[����� (�|�C���^)


    Private m_InfoLen(1) As INFO_LEN            ' �v�Z���W
    Private m_AnsLen As ANS_LEN                 ' �v�Z����


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���p�e�B

    Public Sub InitFrm_Control(ByVal frm As frmPopControl)
        mfrm_pPopCont = frm
    End Sub



    ''' <summary>
    ''' �v�Z���W �Z�b�g
    ''' </summary>
    ''' <param name="Index">0:�n�_ 1:�I�_</param>
    ''' <param name="nPixelX">���摜�擪����̒���ʒu [pixel] [RP]</param>
    ''' <param name="nPixelY">���摜�擪����̕��ʒu [pixel] [RP]</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Public Function SetInfoLen(ByVal Index As Integer, ByVal nPixelX As Integer, ByVal nPixelY As Integer) As Boolean
        If 0 > Index Or 1 < Index Then Return False

        m_InfoLen(Index).nFrameNo = nPixelY \ mcls_pCalc.tProp_ImgInf.nOneImgH
        m_InfoLen(Index).nPixelY = nPixelY Mod mcls_pCalc.tProp_ImgInf.nOneImgH
        m_InfoLen(Index).nPixelX = nPixelX
        m_InfoLen(Index).bIsData = True

        '' �f�[�^�Z�b�g�C�x���g�𔭐�������
        'RaiseEvent EV_DataSet()
        Return True
    End Function

    ''' <summary>
    ''' �v�Z���W �A�N�Z�X
    ''' </summary>
    ''' <param name="Index">0:�n�_ 1:�I�_</param>
    Public ReadOnly Property GetInfoLen(ByVal Index As Integer) As INFO_LEN
        Get
            If 0 > Index Or 1 < Index Then Return Nothing

            Return m_InfoLen(Index)
        End Get
    End Property

    ''' <summary>
    ''' �v�Z���� �A�N�Z�X
    ''' </summary>
    Public ReadOnly Property GetAnsLen() As ANS_LEN
        Get
            Return m_AnsLen
        End Get
    End Property


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{

    Dim m_parentForm As Form
    Public Sub New(parent As Form)
        Me.New()
        m_parentForm = parent
    End Sub
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
        ' �f�[�^�̏�����
        Call InitData(True)
    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    Private Sub frmPop_DspLength_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        ' ��ʂ̍X�V�����Ă���
        Call Dsp()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����

    ''' <summary>
    ''' �����v�Z
    ''' </summary>
    ''' <param name="LenS">�n�_�̍��W</param>
    ''' <param name="LenE">�I�_�̍��W</param>
    ''' <returns>�v�Z����</returns>
    ''' <remarks></remarks>
    Public Function ExecLength(ByVal LenS As INFO_LEN, ByVal LenE As INFO_LEN, ByRef ans As ANS_LEN) As Boolean
        ' �f�[�^���L��
        If Not LenS.bIsData Or Not LenE.bIsData Then Return False

        Dim CalcAns As tClass_ImageDsp_Calc.CALC_ANS = mcls_pCalc.tProp_CalcAns

        '''' �v�Z pixel
        ' ������ (�P�t���[������pixel��)
        Dim x As Integer = Math.Abs(LenE.nPixelX - LenS.nPixelX)
        ' ������� (0�t���[���ڂ���̃g�[�^��pixel���ɒ����B���̌�A�v�Z)
        Dim y As Integer = Math.Abs((LenE.nFrameNo * mcls_pCalc.tProp_ImgInf.nOneImgH + LenE.nPixelY) - (LenS.nFrameNo * mcls_pCalc.tProp_ImgInf.nOneImgH + LenS.nPixelY))

        '''' ����
        ans.dX = mcls_pCalc.GetPos_RpToLen_X(x)
        ans.dY = mcls_pCalc.GetPos_RpToLen_Y(y)
        ans.dL = Math.Sqrt(ans.dX ^ 2 + ans.dY ^ 2)
        Return True
    End Function

    ''' <summary>
    ''' �����v�Z��A��ʕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspLength()
        '' �v�Z
        If Not ExecLength(m_InfoLen(0), m_InfoLen(1), m_AnsLen) Then
            Call InitData(False)
        End If

        ' ��ʕ\��
        Call Dsp()
    End Sub

    ''' <summary>
    ''' �����o�[�ϐ�������
    ''' </summary>
    ''' <param name="bMode">true:�v�Z����+�v�Z���W�N���A false:�v�Z���ʂ̂݃N���A</param>
    ''' <remarks></remarks>
    Public Sub InitData(Optional ByVal bMode As Boolean = False)
        ' �v�Z���W�N���A
        If bMode Then
            For ii As Integer = 0 To m_InfoLen.Length - 1
                m_InfoLen(ii) = New INFO_LEN
            Next
        End If

        ' �v�Z���ʃN���A
        m_AnsLen = New ANS_LEN

        ' ��ʂ̍X�V�����Ă���
        Call Dsp()
    End Sub

    ''' <summary>
    ''' �v�Z���ʉ�ʕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Dsp()
        If ckbRes.Checked Then
            ' ������\�Œ����v��
            Dim inf As tClass_ImageDsp_Calc.IMG_INF = mcls_pCalc.tProp_ImgInf
            Dim dX As Double = (maindoc.nBaseResX / inf.dResW) * m_AnsLen.dX
            Dim dY As Double = (maindoc.nBaseResY / inf.dResH) * m_AnsLen.dY

            lblY.Text = String.Format("{0:####0.0}", dY)
            lblX.Text = String.Format("{0:####0.0}", dX)
            lblL.Text = String.Format("{0:####0.0}", Math.Sqrt(dX ^ 2 + dY ^ 2))
        Else
            lblY.Text = String.Format("{0:####0.0}", m_AnsLen.dY)
            lblX.Text = String.Format("{0:####0.0}", m_AnsLen.dX)
            lblL.Text = String.Format("{0:####0.0}", m_AnsLen.dL)
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �`��

    ''' <summary>
    ''' �w��n�_�`��
    ''' </summary>
    ''' <param name="g">�O���t�B�b�N�X</param>
    ''' <remarks></remarks>
    Public Sub PaintPos(ByVal g As Graphics, ByVal nNowStartFNo As Integer, ByVal nDspFrameRowCnt As Integer)
        Dim nHosei As Integer = 10
        Dim pos As Point        ' �\�����W�ʒu
        Dim infoLen As INFO_LEN

        Dim pen(1) As Pen
        pen(0) = New System.Drawing.Pen(Color.Red, 1)
        pen(1) = New System.Drawing.Pen(Color.Yellow, 1)

        '' �n�_�ʒu
        For ii As Integer = 0 To 1
            infoLen = GetInfoLen(ii)
            If Not infoLen.bIsData Then Continue For ' �f�[�^�̗L��

            ' �\�����Ă���t���[���H
            If infoLen.nFrameNo >= nNowStartFNo And infoLen.nFrameNo <= nNowStartFNo + nDspFrameRowCnt - 1 Then
                ' �\�����W�ɕϊ�
                infoLen.nPixelY += (infoLen.nFrameNo - nNowStartFNo) * mcls_pCalc.tProp_ImgInf.nOneImgH
                pos = mcls_pCalc.GetPosition(infoLen.nPixelX, infoLen.nPixelY)

                ' �\��
                g.DrawLine(pen(ii), pos.X - nHosei, pos.Y, pos.X + nHosei, pos.Y)
                g.DrawLine(pen(ii), pos.X, pos.Y - nHosei, pos.X, pos.Y + nHosei)
            End If
        Next ii

        '' ��n��
        pen(0).Dispose()
        pen(1).Dispose()
    End Sub

    ''' <summary>
    ''' ���ʔ��f
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub ckbRes_CheckedChanged(sender As System.Object, e As System.EventArgs) Handles ckbRes.CheckedChanged
        Call Dsp()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����


    ''' <summary>
    ''' �W�����v��ʂɌ��ʔ��f
    ''' </summary>
    Private Sub btnSetCalc_Click(sender As System.Object, e As System.EventArgs) Handles btnSetCalc.Click
        'mfrm_pPopConttsbDspLen
        mfrm_pPopCont.SetVal_Calc(m_AnsLen.dY)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���O�C���C���^�[�t�F�[�X

    Public Function InitToolbar() As ToolStripButton Implements IMouseToolStrip.InitToolbar
        'Dim resources As New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Dim tsbDspLen As New System.Windows.Forms.ToolStripButton()
        tsbDspLen.CheckOnClick = True
        tsbDspLen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        tsbDspLen.Image = LabelIcon.Image 'CType(resources.GetObject("tsbDspLen.Image"), System.Drawing.Image)
        tsbDspLen.ImageTransparentColor = System.Drawing.Color.Magenta
        tsbDspLen.Name = "tsbDspLen"
        tsbDspLen.Size = New System.Drawing.Size(23, 23)
        tsbDspLen.Text = "�����v��"
        Return tsbDspLen
        
    End Function

    Public Shadows Function OnMouseClick(e As EventArgs) As Boolean Implements IMouseToolStrip.OnMouseClick
        '''' �n�_/�I�_�ʒu

        Dim Index As Integer
        Dim evtMouse As MouseEventArgs = CType(e, MouseEventArgs)
        If evtMouse.Button = Windows.Forms.MouseButtons.Left Then       ' �n�_�ʒu
            Index = 0
        ElseIf evtMouse.Button = Windows.Forms.MouseButtons.Right Then  ' �I�_�ʒu
            Index = 1
        Else
            Return False
        End If


        '''' ���摜�̍��W���Z�o
        ' �w��ʒu�̕\�����W �Z�o
        Dim pos As Point = mcls_RpiView.GetCls_ImgDsp.GetDspPixelToImgPosition(evtMouse.X, evtMouse.y)
        pos.Y += mcls_RpiView.GetDspAriaInf.nDspFno * mcls_RpiView.GetImg_ImgInf.nOneImgH   ' �����Ă��Ȃ��������Z


        '''' �f�[�^�Z�b�g
        Call Me.SetInfoLen(Index, pos.X, pos.Y)
        ' �v�Z&���ʕ\��
        Call Me.DspLength()

        mcls_RpiView.RePaint()

        Return True
    End Function

    Public Shadows Function Activate(bActive As Boolean) As Boolean Implements IMouseToolStrip.Activate
        If Me.ShowToggle(m_parentForm) Then
            Me.InitData(True)
        End If
        Return True
    End Function

    Public Sub OnDraw(g As Graphics) Implements IDrawer.OnDraw
        If Me.Visible Then
            Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf()
            PaintPos(g, DspInf.nDspFno, DspInf.nDspFrameCnt)
        End If
    End Sub

    Public Sub OnChangeFrameNo(nf_start As Integer, nf_end As Integer) Implements IDrawer.OnChangeFrameNo
        Return
    End Sub


End Class