Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin


Public Class frmPopImgControl
    Implements RpiPlugin.IPluginActivatable, RpiPlugin.IEvToolStrip, RpiPlugin.IFilterBitmap

    Public Sub New()

        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
    End Sub

    Private m_doc As PropImageControl

    Dim m_parentForm As Form
    Public Sub New(parent As Form)
        m_parentForm = parent
        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B

        m_doc = GlobalProperties.GetPropertyMember(Of PropImageControl)(parent, "imagecontrol")
        AddHandler m_doc.OnValueChanged, AddressOf OnValueChanged

        InitializeComponent()
        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

    End Sub


    Private m_bStopEmit As Boolean = False
    Private m_prev As PropImageControl
    Private Sub OnValueChanged(name As String, doc As PeropertiesBase)
        m_bStopEmit = True
        Dim bNeedRecreate As Boolean = False
        If m_prev IsNot Nothing Then

            If m_doc.bShdMode And Not m_prev.bShdMode Then
                chkShd.Checked = True
                chkShd.BackColor = Color.Orange
                chkShd.Text = "�L��"
                bNeedRecreate = True
            ElseIf Not m_doc.bShdMode And m_prev.bShdMode Then
                chkShd.Checked = False
                chkShd.BackColor = SystemColors.Control
                chkShd.Text = "����"
                bNeedRecreate = True
            End If

            If m_doc.bBinariMode And Not m_prev.bBinariMode Then
                chkBinari.Checked = True
                bNeedRecreate = True
            ElseIf Not m_doc.bBinariMode And m_prev.bBinariMode Then
                chkBinari.Checked = False
                bNeedRecreate = True
            End If

            If m_doc.bContMode <> m_prev.bContMode Then
                bNeedRecreate = True
            End If
            If m_doc.bGammaMode <> m_prev.bGammaMode Then
                bNeedRecreate = True
            End If
            If m_doc.bSigmoidMode <> m_prev.bSigmoidMode Then
                bNeedRecreate = True
            End If

            If m_doc.nShdX <> m_prev.nShdX Or _
               m_doc.nShdY <> m_prev.nShdY Or _
               m_doc.nShdTarget <> m_prev.nShdTarget Then
                bNeedRecreate = m_doc.bShdMode
            End If



            If m_doc.nContrastX <> m_prev.nContrastX Or _
                m_doc.nBright <> m_prev.nBright Then
                lblContrast.Text = CStr(m_doc.nContrastX)
                lblBright.Text = CStr(m_doc.nBright)
                SetDsp(1)
                bNeedRecreate = m_doc.bContMode
            End If

            If m_doc.nGammaX <> m_prev.nGammaX Then
                '�ݒ�l�i�[
                lblGamma.Text = CStr(m_doc.nGammaX)
                lblGammaRitu.Text = CStr(m_doc.dGammaRitu)
                '��ʃf�[�^�`��
                SetDsp(2)
                bNeedRecreate = m_doc.bGammaMode
            End If

            If m_doc.nSigmoidX <> m_prev.nSigmoidX Or _
               m_doc.nSigmoidKei <> m_prev.nSigmoidKei Then
                lblSigmoidX.Text = CStr(m_doc.nSigmoidX)
                lblSigmoidKei.Text = CStr(m_doc.nSigmoidKei)
                SetDsp(3)
                bNeedRecreate = m_doc.bSigmoidMode
            End If

            If m_doc.nBinari01 <> m_prev.nBinari01 Or _
               m_doc.nBinari02 <> m_prev.nBinari02 Then

                lblBinari01.Text = CStr(m_doc.nBinari01)
                lblBinari02.Text = CStr(m_doc.nBinari02)
                '��ʃf�[�^�`��
                SetDsp(4)

                bNeedRecreate = m_doc.bBinariMode
            End If




            If bNeedRecreate Then
                ReCreateImage()
            Else
                If mcls_RpiView IsNot Nothing Then
                    '                    mcls_RpiView.RePaint()
                End If
            End If
        End If

        radContrast.Checked = m_doc.m_Filter = PropImageControl.FilterSelection.CONTRAST
        radGamma.Checked = m_doc.m_Filter = PropImageControl.FilterSelection.GAMMA
        radSigmoid.Checked = m_doc.m_Filter = PropImageControl.FilterSelection.SIGMOID
        radNon.Checked = m_doc.m_Filter = PropImageControl.FilterSelection.NONE
        If m_doc.m_Filter = PropImageControl.FilterSelection.NONE Then
            radNon.BackColor = Color.Orange
        Else
            radNon.BackColor = SystemColors.Control
        End If

        m_prev = New PropImageControl(m_doc)
        m_bStopEmit = False
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    Private Const mc_NUM As Integer = 4                                 ' �O���t�O���[�v��

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �\����
    ''' <summary>
    ''' �O���t�\���e�[�u��
    ''' </summary>
    Private Structure TYP_TMP_GRAPH
        Dim nNo As Integer                                              ' No
        Dim clsGraph As tClass.tClass_MapBase                           ' �O���t�G���A
        Dim picGraph As PictureBox                                      ' �O���t�\���G���A
    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    Private m_typGraph(mc_NUM - 1) As TYP_TMP_GRAPH
    Private m_bSetEnd As Boolean = False                            ' �������Z�b�g�����t���O(True:�Z�b�g�����AFalse:������)



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{����
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    Private Sub frmPop_Control_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        '�C�x���g�ǉ�
        AddHandler radNon.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radContrast.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radGamma.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radSigmoid.CheckedChanged, AddressOf radbtn_CheckedChanged

        '''' ��ʏ����ݒ�
        radNon.Checked = True
        Call SetInit()

        '''' �}�b�v������`
        For ii As Integer = 0 To mc_NUM - 1
            '''' �}�b�v������`
            Call MapInitialize(m_typGraph(ii).clsGraph)
        Next ii
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �V�F�[�f�B���O�␳
    ''' <summary>
    ''' �V�F�[�f�B���O�␳���[�h�ύX
    ''' </summary>
    Private Sub ckbShd_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkShd.CheckedChanged
        Call ChangeShdMode(CInt(chkShd.Checked))
    End Sub

    ''' <summary>
    ''' �ڕW�P�x�l�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudShdTag_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudShdTag.ValueChanged
        m_doc.nShdTarget = CInt(nudShdTag.Value)
        m_doc.Emit()
    End Sub

    Private Sub ReCreateImage()
        If mcls_RpiView IsNot Nothing Then
            Call mcls_RpiView.ReCreateImg()
            Call mcls_RpiView.RePaint()
        End If
    End Sub

    ''' <summary>
    ''' ���T�C�Y�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudShdX_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudShdX.ValueChanged
        m_doc.nShdX = CInt(nudShdX.Value)
        '8�̔{���̂ݗL��
        If 0 <> (m_doc.nShdX Mod 8) Then
            m_doc.nShdX = tMod.GET_SIZE_JUST(m_doc.nShdX, 8)
            nudShdX.Value = m_doc.nShdX
        End If
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' �c�T�C�Y�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudShdY_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudShdY.ValueChanged
        m_doc.nShdY = CInt(nudShdY.Value)
        '8�̔{���̂ݗL��
        If 0 <> (m_doc.nShdY Mod 8) Then
            m_doc.nShdY = tMod.GET_SIZE_JUST(m_doc.nShdY, 8)
            nudShdY.Value = m_doc.nShdY
        End If
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' �V�F�[�f�B���O�␳�\���ؑ�
    ''' </summary>
    ''' <param name="nMode"></param>
    ''' <remarks></remarks>
    Private Sub ChangeShdMode(ByVal nMode As Integer)
        m_doc.bShdMode = nMode <> 0

        '�摜�Đ���
        'ReCreateImage()
        m_doc.Emit()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �Q�l���␳
    ''' <summary>
    ''' �Q�l���␳���[�h�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkBinari_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBinari.CheckedChanged
        Call ChangeBinariMode(CInt(chkBinari.Checked))
    End Sub

    ''' <summary>
    ''' �Q�l���␳�\���ؑ�
    ''' </summary>
    ''' <param name="nMode"></param>
    ''' <remarks></remarks>
    Private Sub ChangeBinariMode(ByVal nMode As Integer)
        m_doc.bBinariMode = nMode <> 0

        '�摜�Đ���
        m_doc.Emit()
    End Sub


    ''' <summary>
    ''' �������Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetInit()
        '�O���t�ԍ��i�g��Ȃ����E�E�E�j
        m_typGraph(0).nNo = 1
        m_typGraph(1).nNo = 2
        m_typGraph(2).nNo = 3
        m_typGraph(3).nNo = 4

        '�}�b�v�̃����o�[�ϐ��ݒ�
        For ii As Integer = 0 To mc_NUM - 1
            m_typGraph(ii).clsGraph = New tClass.tClass_MapBase
        Next ii
        '�s�N�`���[�̃����o�[�ϐ��ݒ�
        m_typGraph(0).picGraph = picGraph1
        m_typGraph(1).picGraph = picGraph2
        m_typGraph(2).picGraph = picGraph3
        m_typGraph(3).picGraph = picGraph4
        ' �C�x���g�n���h���֘A�t��
        For ii As Integer = 0 To mc_NUM - 1
            ' �}�b�v
            AddHandler m_typGraph(ii).clsGraph.tEv_DrawPaint_End, AddressOf DrawMap

            ' �s�N�`���[�{�b�N�X
            AddHandler m_typGraph(ii).picGraph.Paint, AddressOf PictureBox_Paint

        Next ii


        '�����l���f
        trbContrast.Value = m_doc.nContrastX
        trbBright.Value = m_doc.nBright
        trbGamma.Value = m_doc.nGammaX

        trbSigmoidX.Value = m_doc.nSigmoidX
        trbSigmoidKei.Value = m_doc.nSigmoidKei
        trbBinari01.Value = m_doc.nBinari01
        trbBinari02.Value = m_doc.nBinari02

        nudShdTag.Value = m_doc.nShdTarget
        nudShdX.Value = m_doc.nShdX
        nudShdY.Value = m_doc.nShdY


        '�X���C�h�o�[�l�\��
        lblContrast.Text = CStr(trbContrast.Value)
        lblBright.Text = CStr(trbBright.Value)
        lblGamma.Text = CStr(trbGamma.Value)
        lblGammaRitu.Text = CStr(m_doc.dGammaRitu)
        lblSigmoidX.Text = CStr(trbSigmoidX.Value)
        lblSigmoidKei.Text = CStr(trbSigmoidKei.Value)
        lblBinari01.Text = CStr(trbBinari01.Value)
        lblBinari02.Text = CStr(trbBinari02.Value)

        '�������Z�b�g�����t���O�n�m
        m_bSetEnd = True
    End Sub

    ''' <summary>
    ''' �}�b�v��{����`
    ''' </summary>
    ''' <param name="MapBase">�O���t�}�b�v</param>
    ''' <remarks></remarks>
    Private Sub MapInitialize(ByVal MapBase As tClass.tClass_MapBase)
        '''' �O���t�`��ɕK�v�ȏ���ݒ�
        With MapBase
            '' �\���̈���
            .tProp_AriaInf_BackGroundLineColor = Pens.Black
            .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
            .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
            .tProp_AriaInf_AriaSize = picGraph1.Size
            .tProp_AriaInf_Margin = New Padding(0, 0, 0, 0)

            '' �}�b�v�̈�
            .tProp_MapInf_IsCenterX = False             'X���Z���^�[��H(true���Z���^�[��Bfalse������)
            .tProp_MapInf_IsCenterY = False             'Y���Z���^�[��H(true���Z���^�[��Bfalse������)
            .tProp_MapInf_IsBaseX_L = True              'X���(true������Bfalse���E�)(bIsCenterX=false���̂ݗL��)
            .tProp_MapInf_IsBaseY_T = False             'Y���(true�����Bfalse�����)(bIsCenterY=false���̂ݗL��)
            .tProp_MapInf_IsStrechX = True              'X���̍ŏ��ő���}�b�v�̒[�ɂ��킹��
            .tProp_MapInf_IsStrechY = True              'Y���̍ŏ��ő���}�b�v�̒[�ɂ��킹��
            .tProp_MapInf_LimitXmin = 0                 'X���ŏ�[mm]
            .tProp_MapInf_LimitXmax = 255               'X���ő�[mm]
            .tProp_MapInf_LimitYmin = 0                 'Y���ŏ�[mm]
            .tProp_MapInf_LimitYmax = 255               'Y���ő�[mm]
            '�I�t�Z�b�g�ʒu (����pixel�ʒu�̒l���Z�b�g) (bIsCenter�œ��삪�ς��Bfalse�̂ݗL���Btrue���͎����Z�o)
            .tProp_MapInf_Offset = New tClass.tClass_MapBase.xyValue(0, 0)

            '' �}�b�v�ΏۊO�̈�ݒ�
            .tProp_LimitInf_LimitDraw = False           '�}�b�v�ƑΏ۔͈͂̋��E��

            '' �������ݒ�
            Dim p As Pen = New Pen(Color.LightBlue, 1)
            p.DashStyle = Drawing2D.DashStyle.DashDot
            .tProp_MemoriInf_LinePen = p
            .tProp_MemoriInf_StringFont = New Font("�l�r �S�V�b�N", 11, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Red)
            .tProp_MemoriInf_PosStringX = 128           'X���̕����\���ʒu�I�t�Z�b�g(X�����牽�s�N�Z������)
            .tProp_MemoriInf_PosStringY = 128           'Y���̕����\���ʒu�I�t�Z�b�g(Y�����牽�s�N�Z������)

            .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(128, 128)    '�������Ԋu [mm]
            '.tProp_MemoriInf_ScaleStringFormatX = "###0"
            '.tProp_MemoriInf_ScaleStringFormatY = "0"
            .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1, 1)
            .tProp_MemoriInf_StringDisp_T = False       '�ڐ��蕶���̕\����\�� (True:�\�� False:��\��)
            .tProp_MemoriInf_StringDisp_B = False       '�ڐ��蕶���̕\����\�� (True:�\�� False:��\��)
            .tProp_MemoriInf_StringDisp_L = False       '�ڐ��蕶���̕\����\�� (True:�\�� False:��\��)
            .tProp_MemoriInf_StringDisp_R = False       '�ڐ��蕶���̕\����\�� (True:�\�� False:��\��)
            .tProp_MemoriInf_LineDsp_X = True           '���������\���L�� (True:�\�� False:��\��)
            .tProp_MemoriInf_LineDsp_Y = True           '���������\���L�� (True:�\�� False:��\��)
        End With
    End Sub

    ''' <summary>
    ''' �t�H�[���\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPopImgControl_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        Try

            SetDsp()
        Catch ex As Exception
        End Try

    End Sub

    ''' <summary>
    ''' ��ʃf�[�^�`�� (�ݒ�ύX���ɃC�x���g���s) (��ʕ\�����ɂ�)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetDsp()

        '�������Z�b�g�������̏ꍇ�͏I��
        If Not m_bSetEnd Then
            Exit Sub
        End If

        For ii As Integer = 0 To mc_NUM - 1

            '''' �}�b�v�Đ����˗�
            Call m_typGraph(ii).clsGraph.DrawPaint()

            '''' �}�b�v�ĕ`��
            Call m_typGraph(ii).picGraph.Refresh()

        Next ii

    End Sub

    ''' <summary>
    ''' ��ʃf�[�^�`�� (�ݒ�ύX���ɃC�x���g���s) (��ʕ\�����ɂ�)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetDsp(ByRef nNo As Integer)

        '�����`�F�b�N
        If nNo > mc_NUM Or nNo < 1 Then
            Exit Sub
        End If

        '�������Z�b�g�������̏ꍇ�͏I��
        If Not m_bSetEnd Then
            Exit Sub
        End If

        '''' �}�b�v�Đ����˗�
        Call m_typGraph(nNo - 1).clsGraph.DrawPaint()

        '''' �}�b�v�ĕ`��
        Call m_typGraph(nNo - 1).picGraph.Refresh()

    End Sub

    ''' <summary>
    ''' �`��
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' </summary>
    Private Sub PictureBox_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
        For ii As Integer = 0 To mc_NUM - 1
            ' �O���t�O���[�v�̌���
            If sender Is m_typGraph(ii).picGraph Then
                '''' �r�b�g�}�b�v�擾
                Dim bmp As Bitmap = m_typGraph(ii).clsGraph.tProp_Bmp
                If bmp Is Nothing Then Return

                '''' �w�i���Z�b�g
                e.Graphics.DrawImage(bmp, 0, 0)
            End If
        Next ii

    End Sub

    ''' <summary>
    ''' ��ʃf�[�^�`��i�O���t�\���j
    ''' </summary>
    ''' <param name="cls"></param>
    ''' <param name="g"></param>
    ''' <remarks></remarks>
    Private Sub DrawMap(ByVal cls As tClassLibrary.tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics)
        Dim nGrpNo As Integer               ' �O���t�\���O���[�v�m��
        Dim ii As Integer                   ' INDEX
        Try

            ' �O���t�O���[�v�̌���
            For ii = 0 To mc_NUM - 1
                If cls Is m_typGraph(ii).clsGraph Then
                    nGrpNo = ii
                    Exit For
                End If
            Next ii

            '''' �`��
            Dim dVal As Single
            Dim x1, y1, x2, y2 As Integer

            For ii = 0 To 255 - 1

                ' �n�_�Z�o (�擪�̍��W�Z�o) X��
                x1 = cls.GetVtoP_X(ii + 0)

                ' �n�_�Z�o (�擪�̍��W�Z�o) Y��
                If 0 = nGrpNo Then
                    '�R���g���X�g�A���邳�␳
                    dVal = tClass.tClass_BmpJpeg.GetIntensityContrast(ii, m_doc.nContrastX, m_doc.nBright)
                ElseIf 1 = nGrpNo Then
                    ' �K���}�␳
                    dVal = tClass.tClass_BmpJpeg.GetIntensityGamma(ii, m_doc.dGammaRitu)
                ElseIf 2 = nGrpNo Then
                    '�V�O���C�h�␳
                    dVal = tClass.tClass_BmpJpeg.GetIntensitySigmoid(ii, m_doc.nSigmoidX, m_doc.nSigmoidKei)
                ElseIf 3 = nGrpNo Then
                    '�Q�l���␳
                    dVal = tClass.tClass_BmpJpeg.GetIntensityBinari(ii, m_doc.nBinari01, m_doc.nBinari02)
                End If
                '�㉺���l�`�F�b�N
                If dVal > 255 Then dVal = 255
                If dVal < 0 Then dVal = 0
                y1 = cls.GetVtoP_Y(dVal)

                ' �I�_�Z�o X��
                x2 = cls.GetVtoP_X(ii + 1)
                ' �I�_�Z�o Y��
                If 0 = nGrpNo Then
                    '�R���g���X�g�A���邳�␳
                    dVal = tClass.tClass_BmpJpeg.GetIntensityContrast(ii + 1, m_doc.nContrastX, m_doc.nBright)
                ElseIf 1 = nGrpNo Then
                    ' �K���}�␳
                    dVal = tClass.tClass_BmpJpeg.GetIntensityGamma(ii + 1, m_doc.dGammaRitu)
                ElseIf 2 = nGrpNo Then
                    '�V�O���C�h�␳
                    dVal = tClass.tClass_BmpJpeg.GetIntensitySigmoid(ii + 1, m_doc.nSigmoidX, m_doc.nSigmoidKei)
                ElseIf 3 = nGrpNo Then
                    '�Q�l���␳
                    dVal = tClass.tClass_BmpJpeg.GetIntensityBinari(ii + 1, m_doc.nBinari01, m_doc.nBinari02)
                End If
                '�㉺���l�`�F�b�N
                If dVal > 255 Then dVal = 255
                If dVal < 0 Then dVal = 0
                y2 = cls.GetVtoP_Y(dVal)

                Using p As New Pen(Color.Red, 2)
                    ' �`��
                    g.DrawLine(p, x1, y1, x2, y2)
                End Using
            Next ii

        Catch ex As Exception
        End Try
    End Sub

    ''' <summary>
    ''' �R���g���X�g�����X���C�h�o�[�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbContrast_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbContrast.ValueChanged
        '�ݒ�l�i�[
        m_doc.nContrastX = trbContrast.Value

        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' ���邳�����X���C�h�o�[�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbBright_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbBright.ValueChanged
        '�ݒ�l�i�[
        m_doc.nBright = trbBright.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' �K���}�␳�X���C�h�o�[�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbGamma_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbGamma.ValueChanged
        m_doc.nGammaX = trbGamma.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' �V�O���C�h�␳�P�x�X���C�h�o�[�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbSigmoidX_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbSigmoidX.ValueChanged
        '�ݒ�l�i�[
        m_doc.nSigmoidX = trbSigmoidX.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' �V�O���C�h�␳�W���X���C�h�o�[�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbSigmoidKei_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbSigmoidKei.ValueChanged
        '�ݒ�l�i�[
        m_doc.nSigmoidKei = trbSigmoidKei.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' �Q�l�� 臒l�P�X���C�h�o�[�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbBinari01_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbBinari01.ValueChanged
        '�ݒ�l�i�[
        m_doc.nBinari01 = trbBinari01.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' �Q�l�� 臒l�Q�X���C�h�o�[�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbBinari02_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbBinari02.ValueChanged
        '�ݒ�l�i�[
        m_doc.nBinari02 = trbBinari02.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' ���W�I�{�^���`�F�b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radbtn_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        If m_bStopEmit Then Exit Sub
        If radContrast.Checked Then
            m_doc.m_Filter = PropImageControl.FilterSelection.CONTRAST
        ElseIf radGamma.Checked Then
            m_doc.m_Filter = PropImageControl.FilterSelection.GAMMA
        ElseIf radSigmoid.Checked Then
            m_doc.m_Filter = PropImageControl.FilterSelection.SIGMOID
        Else
            m_doc.m_Filter = PropImageControl.FilterSelection.NONE
        End If
        m_doc.Emit()
    End Sub



    Public ReadOnly Property PluginLevel As Single Implements IPluginActivatable.PluginLevel
        Get
            Return 100
        End Get
    End Property

    Private mcls_RpiView As tClass_ViewRpiThread
    Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
        mcls_RpiView = v
    End Sub

    Public ReadOnly Property PluginName As String Implements IPluginActivatable.PluginName
        Get
            Return "ImageControl"
        End Get
    End Property

    Public ReadOnly Property ToolStripName As String Implements IEvToolStrip.ToolStripName
        Get
            Return "tsbBrite"
        End Get
    End Property

    Public Sub InitToolbar(mainstrip As ToolStripButton, o As Object) Implements IEvToolStrip.InitToolbar
        AddHandler mainstrip.Click, AddressOf tsbBrite_Click
    End Sub

    ''' <summary>
    ''' �P�x������ʕ\��
    ''' </summary>
    Private Sub tsbBrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        If Not Me.Visible Then
            Call Me.Show(m_parentForm)
        Else
            Me.Visible = False
        End If
        If m_parentForm IsNot Nothing Then
            m_parentForm.Select()
        End If
    End Sub


    Public Sub OnFilterImage(ByRef img As Bitmap, ByVal p As tClass_ViewRpiThread.CLS_IMG_INF) Implements IFilterBitmap.OnFilterImage
        '''' �V�F�[�����O�␳ �L��
        If m_doc.bShdMode Then
            '''' �O����
            Dim head As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header() ' �w�b�_�[�擾
            Dim clsShdCmp As ImgLibCLI.ShadingComp2 = Nothing                           ' �V�F�[�����O�␳�N���X


            ' �r�b�g�}�b�v�f�[�^���̔z����擾
            Dim bBmpArray() As Byte = tClass.tClass_BmpJpeg.ImagToData(img)                                 ' �r�b�g�}�b�v�f�[�^���̔z��
            Try
                '' �V�F�[�f�B���O�␳����
                clsShdCmp = New ImgLibCLI.ShadingComp2
                clsShdCmp.Alloc(head.pitch_nbyte, head.height)
                clsShdCmp.CreateLPF(m_doc.nShdX, m_doc.nShdY, CByte(m_doc.nShdTarget))

                '' �G�b�W���݂̂̃V�F�[�f�B���O�␳���\�H
                If head.frame_info_type = tClass.tClass_ReadRPI.EM_FRM_TYPE.EDGE_PER_FRAME Then
                    ' �G�b�W���݂̂ɃV�F�[�f�B���O�␳
                    Dim typ As tClass.tClass_ReadRPI.EDGE_PER_FRAME = mcls_RpiView.ReadFile_FrameInfo_EdgePerFrame(p.nIndex)
                    Call clsShdCmp.Exec(bBmpArray, bBmpArray) ', typ.edge_left, typ.edge_right)

                Else
                    ' �S�ʂɑ΂��ăV�F�[�f�B���O�␳()
                    Call clsShdCmp.Exec(bBmpArray, bBmpArray)
                End If

            Catch ex As Exception
            Finally
                If Not clsShdCmp Is Nothing Then
                    clsShdCmp.Free()    ' �V�F�[�����O�␳�N���X�J��
                    clsShdCmp = Nothing
                End If
            End Try


            '' ���̃C���[�W�ɖ߂�
            img = tClassLibrary.tClass.tClass_BmpJpeg.DataToBmp(img.Width, img.Height, bBmpArray)
        End If

        '=====================================
        '''' �R���g���X�g���� �L��
        If m_doc.bContMode Then
            tClassLibrary.tClass.tClass_BmpJpeg.ContrastCmpBmp(img, m_doc.nContrastX, m_doc.nBright)
        End If

        '=====================================
        '''' �K���}�␳ �L��
        If m_doc.bGammaMode Then
            tClassLibrary.tClass.tClass_BmpJpeg.GammaCmpBmp(img, m_doc.dGammaRitu)
        End If

        '=====================================
        '''' �V�O���C�h�␳ �L��
        If m_doc.bSigmoidMode Then
            tClassLibrary.tClass.tClass_BmpJpeg.SigmoidCmpBmp(img, m_doc.nSigmoidX, m_doc.nSigmoidKei)
        End If

        '------------------------------------------------------------------------------
        ' ���ȉ��̏���(�Q�l���␳)�͕K���Ō�Ɏ��{���邱�ƁI
        '   ���̕␳�ƕ��p�̏ꍇ�A���P�x�ɑ΂��ăJ���[�p���b�g���쐬���邽�߁B
        '------------------------------------------------------------------------------
        '''' �Q�l���␳ �L��
        If m_doc.bBinariMode Then
            tClassLibrary.tClass.tClass_BmpJpeg.BinariCmpBmp(img, m_doc.nBinari01, m_doc.nBinari02)
        End If
    End Sub
End Class



''' <summary>
''' �P�x�R���g���[���v���p�e�B��
''' </summary>
Public Class PropImageControl
    Inherits PeropertiesBase

    Public Enum FilterSelection
        NONE
        CONTRAST
        GAMMA
        SIGMOID
    End Enum
    '/ /////////////////////////////////////////////////////////////////////////////////
    '''' ���J�ϐ�

    ''' <summary>
    ''' �V�F�[�f�B���O�␳�L��
    ''' </summary>
    Public bShdMode As Boolean = False                            ' 


    ''' <summary>
    ''' �t�B���^�͉����L���H
    ''' </summary>
    Public m_Filter As FilterSelection

    ''' <summary>
    ''' �R���g���X�g�E�P�x�␳�L���H
    ''' </summary>
    Public ReadOnly Property bContMode As Boolean
        Get
            Return m_Filter = FilterSelection.CONTRAST
        End Get
    End Property


    ''' <summary>
    ''' �K���}�␳�L��?
    ''' </summary>
    Public ReadOnly Property bGammaMode As Boolean
        Get
            Return m_Filter = FilterSelection.GAMMA
        End Get
    End Property

    ''' <summary>
    ''' �V�O���C�h�␳�L��
    ''' </summary>
    Public ReadOnly Property bSigmoidMode As Boolean
        Get
            Return m_Filter = FilterSelection.SIGMOID
        End Get
    End Property

    ''' <summary>�Q�l���L�� </summary>
    Public bBinariMode As Boolean = False

    ''' <summary>�V�F�[�f�B���O�ڕW�P�x�l </summary>
    Public nShdTarget As Integer = 0
    ''' <summary>�V�F�[�f�B���O���T�C�Y </summary>
    Public nShdX As Integer = 0
    ''' <summary>�V�F�[�f�B���O�c�T�C�Y </summary>
    Public nShdY As Integer = 0

    ''' <summary>�R���g���X�g�␳ </summary>
    Public nContrastX As Integer = 0
    ''' <summary>���邳�␳ </summary>
    Public nBright As Integer = 0
    ''' <summary>�K���}�␳ </summary>
    Public nGammaX As Integer = 0
    ''' <summary>�K���}�␳�� </summary>
    Public ReadOnly Property dGammaRitu As Double
        Get
            Return GetGammaRitu(nGammaX)
        End Get
    End Property

    ''' <summary>�V�O���C�h�␳ X�� </summary>
    Public nSigmoidX As Integer = 0
    ''' <summary>�V�O���C�h�␳ �W�� </summary>
    Public nSigmoidKei As Integer = 0
    ''' <summary>�Q�l��臒l lo </summary>
    Public nBinari01 As Integer = 0
    ''' <summary>�Q�l��臒l ho</summary>
    Public nBinari02 As Integer = 0


    Public Sub New(r As PropImageControl)
        bShdMode = r.bShdMode
        m_Filter = r.m_Filter
        bBinariMode = r.bBinariMode

        nShdTarget = r.nShdTarget
        nShdX = r.nShdX
        nShdY = r.nShdY

        nContrastX = r.nContrastX
        nBright = r.nBright
        nGammaX = r.nGammaX
        nSigmoidX = r.nSigmoidX
        nSigmoidKei = r.nSigmoidKei
        nBinari01 = r.nBinari01
        nBinari02 = r.nBinari02

    End Sub

    Public Sub New()
        Load()
    End Sub


    Const Section As String = "RPI_VIEWER"
    ''' <summary>
    ''' �O��l�ۑ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Save()
        '�V�F�[�f�B���O�␳
        tMod.WriteIni(Section, "SHD_SHDTAG", LOCALINI_NAME, CStr(Me.nShdTarget))           '�ڕW�P�x�l
        tMod.WriteIni(Section, "SHD_SHDX", LOCALINI_NAME, CStr(Me.nShdX))               '���T�C�Y
        tMod.WriteIni(Section, "SHD_SHDY", LOCALINI_NAME, CStr(Me.nShdY))               '�c�T�C�Y

        '�R���g���X�g�A���邳�␳
        tMod.WriteIni(Section, "CNT_CONTRASTX", LOCALINI_NAME, CStr(Me.nContrastX))     '�R���g���X�g�␳�X���C�hBar�l
        tMod.WriteIni(Section, "CNT_BRIGHT", LOCALINI_NAME, CStr(Me.nBright))           '���邳�␳�X���C�hBar�l
        '�K���}�␳
        tMod.WriteIni(Section, "GMA_GAMMAX", LOCALINI_NAME, CStr(Me.nGammaX))           '�K���}�␳�X���C�hBar�l
        '�V�O���C�h�␳
        tMod.WriteIni(Section, "SGM_SIGMOIDX", LOCALINI_NAME, CStr(Me.nSigmoidX))       '�V�O���C�h�␳�X���C�hBar�l(X��)
        tMod.WriteIni(Section, "SGM_SIGMOIDKEI", LOCALINI_NAME, CStr(Me.nSigmoidKei))   '�V�O���C�h�␳�X���C�hBar�l(�W��)
        '�Q�l���␳
        tMod.WriteIni(Section, "BIN_BINARI01", LOCALINI_NAME, CStr(Me.nBinari01))       '�Q�l���X���C�hBar�l(�P)
        tMod.WriteIni(Section, "BIN_BINARI02", LOCALINI_NAME, CStr(Me.nBinari02))       '�Q�l���X���C�hBar�l(�Q)
    End Sub

    ''' <summary>
    ''' �O��l�Ǎ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Load()
        Me.nShdTarget = tMod.ReadIniInt(Section, "SHD_SHDTAG", LOCALINI_NAME, 128)        '���w��̏ꍇ�́A128
        Me.nShdX = tMod.ReadIniInt(Section, "SHD_SHDX", LOCALINI_NAME, 32)             '���w��̏ꍇ�́A32
        Me.nShdY = tMod.ReadIniInt(Section, "SHD_SHDY", LOCALINI_NAME, 32)             '���w��̏ꍇ�́A32
        Me.nContrastX = tMod.ReadIniInt(Section, "CNT_CONTRASTX", LOCALINI_NAME, 128)  '���w��̏ꍇ�́A128
        Me.nBright = tMod.ReadIniInt(Section, "CNT_BRIGHT", LOCALINI_NAME, 128)        '���w��̏ꍇ�́A128
        Me.nGammaX = tMod.ReadIniInt(Section, "GMA_GAMMAX", LOCALINI_NAME, 0)          '���w��̏ꍇ�́A0
        Me.nSigmoidX = tMod.ReadIniInt(Section, "SGM_SIGMOIDX", LOCALINI_NAME, 128)    '���w��̏ꍇ�́A128
        Me.nSigmoidKei = tMod.ReadIniInt(Section, "SGM_SIGMOIDKEI", LOCALINI_NAME, 5)  '���w��̏ꍇ�́A5
        Me.nBinari01 = tMod.ReadIniInt(Section, "BIN_BINARI01", LOCALINI_NAME, 255)    '���w��̏ꍇ�́A255
        Me.nBinari02 = tMod.ReadIniInt(Section, "BIN_BINARI02", LOCALINI_NAME, 0)      '���w��̏ꍇ�́A0

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�֐�
    ''' <summary>
    ''' �K���}�l
    ''' </summary>
    ''' <param name="nVal">�␳�l</param>
    Public Shared Function GetGammaRitu(ByVal nVal As Integer) As Double
        ' �w��␳�l����K���}�l���Z�o
        If nVal > 0 Then
            Return CDbl(1.0 + 0.2 * nVal)
        ElseIf nVal <= 0 And nVal >= -5 Then
            Return CDbl(1.0 + 0.1 * nVal)
        Else
            Return CDbl(0.5 + 0.05 * (nVal + 5))
        End If
    End Function
End Class
