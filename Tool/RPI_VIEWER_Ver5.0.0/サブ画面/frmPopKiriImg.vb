Imports tClassLibrary

Public Class frmPopKiriImg
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �A�N�Z�X�Z�b�V����
    Private m_Session As String													' ini�t�@�C���Z�b�V����

    ' �N���X
    Private mcls_pParam As clsParamManager								' �p�����[�^�Ǘ��N���X (�|�C���^)
    Private mcls_Img(MC_MAX_IMG - 1) As tClass.tClass_ImageDsp			' �摜�\���N���X 

    ' �f�[�^
    Private m_DetectData As DETECT_KIRI_DATA = Nothing					' �؂�o���摜���
    Private m_ImgKind As New tClass.tClass_ImageDsp.IMG_KIND


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���p�e�B
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �p�����[�^�N���X�Z�b�g
    ''' </summary>
    ''' <param name="cls">�v�Z�N���X</param>
    Public Sub SetCls_Param(ByRef cls As clsParamManager)
        mcls_pParam = cls
    End Sub

    ''' <summary>
    ''' �؂�o���摜��� �Z�b�g
    ''' </summary>
    ''' <param name="data"></param>
    ''' <remarks></remarks>
    Public Sub SetItem_DetectKiriData(ByVal data As ValueType)
        m_DetectData = CType(data, DETECT_KIRI_DATA)
    End Sub
    ''' <summary>
    ''' �؂�o���摜�e��t���O�ރZ�b�g
    ''' </summary>
    ''' <param name="ImgKind"></param>
    ''' <remarks></remarks>
    Public Sub SetProp_ImgKinf(ByVal ImgKind As tClass.tClass_ImageDsp.IMG_KIND)
        m_ImgKind = ImgKind
    End Sub

    ''' <summary>
    ''' INI�t�@�C���Z�N�V����
    ''' </summary>
    ''' <param name="sSession">ini�t�@�C���Z�b�V����</param>
    ''' <remarks></remarks>
    Public Sub Set_DbSession(ByVal sSession As String)
        m_Session = sSession
    End Sub

    ''' <summary>
    ''' ���葤�X�N���[���o�[
    ''' </summary>
    Private ReadOnly Property scrNagate() As ScrollBar
        Get
            If m_ImgKind.bIsRotate90 Then
                Return Me.hsbImg
            Else
                Return Me.vsbImg
            End If
        End Get
    End Property

    ''' <summary>
    ''' ���葤�X�N���[���o�[�̃}�E�X������
    ''' </summary>
    Private ReadOnly Property scrNagateMaximumVirtual() As Integer
        Get
            If m_ImgKind.bIsRotate90 Then
                Return hsbImg.MaximumVirtual
            Else
                Return vsbImg.MaximumVirtual
            End If
        End Get
    End Property

    ''' <summary>
    ''' ���葤�X�N���[���o�[�l�̕ύX
    ''' </summary>
    Private WriteOnly Property scrNagateValueProgramable() As Integer
        Set(ByVal value As Integer)
            If m_ImgKind.bIsRotate90 Then
                hsbImg.ValueProgramable = value
            Else
                vsbImg.ValueProgramable = value
            End If
        End Set
    End Property


    ''' <summary>
    ''' �����N���[���o�[
    ''' </summary>
    Private ReadOnly Property scrHaba() As ScrollBar
        Get
            If Not m_ImgKind.bIsRotate90 Then
                Return hsbImg
            Else
                Return vsbImg
            End If
        End Get
    End Property

    ''' <summary>
    ''' �����X�N���[���o�[�̃}�E�X������
    ''' </summary>
    Private ReadOnly Property scrHabaMaximumVirtual() As Integer
        Get
            If Not m_ImgKind.bIsRotate90 Then
                Return hsbImg.MaximumVirtual
            Else
                Return vsbImg.MaximumVirtual
            End If
        End Get
    End Property

    ''' <summary>
    ''' �����X�N���[���o�[�l�̕ύX
    ''' </summary>
    Private WriteOnly Property scrHabaValueProgramable() As Integer
        Set(ByVal value As Integer)
            If Not m_ImgKind.bIsRotate90 Then
                hsbImg.ValueProgramable = value
            Else
                vsbImg.ValueProgramable = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' ���X�N���[���o�[�œ����ׂ�Offset
    ''' </summary>
    ''' <param name="cls_Img">�ݒ肷��摜</param>
    ''' <value></value>
    ''' <returns>�c��offset��</returns>
    ''' <remarks>��]�\���ɉ����āAX�EY���ς��</remarks>
    Private Property nhOffsetOfPic(ByVal cls_Img As tClass.tClass_ImageDsp) As Integer
        Get
            If Not m_ImgKind.bIsRotate90 Then
                Return cls_Img.tProp_AriaInf_OffsetX
            Else
                Return cls_Img.tProp_AriaInf_OffsetY
            End If
        End Get

        Set(ByVal value As Integer)
            If Not m_ImgKind.bIsRotate90 Then
                cls_Img.tProp_AriaInf_OffsetX = value
            Else
                cls_Img.tProp_AriaInf_OffsetY = value
            End If
        End Set
    End Property


    ''' <summary>
    ''' �c�X�N���[���o�[�œ����ׂ�Offset
    ''' </summary>
    ''' <param name="cls_Img">�ݒ肷��摜</param>
    ''' <value></value>
    ''' <returns>�c��offset��</returns>
    ''' <remarks>��]�\���ɉ����āAX�EY���ς��</remarks>
    Private Property nvOffsetOfPic(ByVal cls_Img As tClass.tClass_ImageDsp) As Integer
        Get
            If Not m_ImgKind.bIsRotate90 Then
                Return cls_Img.tProp_AriaInf_OffsetY
            Else
                Return cls_Img.tProp_AriaInf_OffsetX
            End If
        End Get

        Set(ByVal value As Integer)
            If Not m_ImgKind.bIsRotate90 Then
                cls_Img.tProp_AriaInf_OffsetY = value
            Else
                cls_Img.tProp_AriaInf_OffsetX = value
            End If
        End Set
    End Property

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �ŐV�̏����r����\������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspUpDate()
        Dim ii As Integer

        ' �w�b�_�[
        Dim lbl(MC_MAX_IMG - 1) As Label
        lbl(0) = lblName4	' ������
        lbl(1) = lblName3
        lbl(2) = lblName2
        lbl(3) = lblName1

        Dim sWk As String = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_IMG_NAME", TASKINI_NAME)
        Dim sLblName(-1) As String
        If Not String.IsNullOrEmpty(sWk) Then
            sLblName = sWk.Split(","c)
        End If



        '''' �摜������
        'For ii = 0 To MC_MAX_IMG - m_DetectData.nImgCnt - 1
        '	tlpMain.ColumnStyles(ii).Width = 1
        'Next
        For ii = 0 To MC_MAX_IMG - m_DetectData.nImgCnt - 1
            tlpMain.ColumnStyles(ii).Width = 0
            mcls_Img(ii).tProp_Paint_IsDefImgDspn = True			' Ver443
        Next
        For ii = MC_MAX_IMG - m_DetectData.nImgCnt To MC_MAX_IMG - 1

            tlpMain.ColumnStyles(ii).Width = CSng((100 / (MC_MAX_IMG / m_DetectData.nImgCnt)))
        Next


        ' �X�N���[���o�[������
        vsbImg.Value = 0
        hsbImg.Value = 0

        '''' �摜�\��
        '' �C���[�W�\���N���X�����Z�b�g
        For ii = 0 To m_DetectData.nImgCnt - 1
            ' �w�b�_�[
            If ii < sLblName.Length Then
                lbl(ii).Text = sLblName(ii)
            Else
                lbl(ii).Text = CStr(lbl(ii).Tag)
            End If


            '' �I�t�Z�b�g�l������
            mcls_Img(ii).tProp_AriaInf_OffsetX = 0
            mcls_Img(ii).tProp_AriaInf_OffsetY = 0
            '' ��{���Z�b�g
            mcls_Img(ii).SetImgInf(m_DetectData.nImgSizeX, m_DetectData.nImgSizeY, m_DetectData.dResX, m_DetectData.dResY, m_DetectData.nImgCenterX, m_DetectData.nImgCenterY, 1, 1)
            mcls_Img(ii).tProp_ImgKind_IsTurnTB = m_ImgKind.bIsTurnTB
            mcls_Img(ii).tProp_ImgKind_IsTurnLR = m_ImgKind.bIsTurnLR
            mcls_Img(ii).tProp_ImgKind_Rotate90 = m_ImgKind.bIsRotate90

            mcls_Img(ii).SetAriaInf()
            '			mcls_Img(ii).tProp_AriaInf_Aria = picImg1.ClientSize			' �S�������T�C�Y������ �����OK
            mcls_Img(ii).tProp_ImgKind_StrechType = tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH.emRawHeight

            ' �����\���Ƃ����r���S�ʒu�Ɉړ��ݒ�B�v�Z�I�������A�{���̐ݒ�ɕύX
            mcls_Img(ii).tProp_AriaInf_IsCenter = True									' �r���S�ʒu�ړ�
            mcls_Img(ii).Calculation()													' �\�������m��
            mcls_Img(ii).tProp_AriaInf_IsCenter = False									' �r���S�ʒu�ړ�����



            If 0 <> m_DetectData.nImgSize(ii) Then
                '' �I�[�o�[���C
                Dim oly As tClass.tClass_ImageDsp.OVERLAY_INF = Nothing
                oly.bIsDsp = True
                oly.Tag = Nothing
                oly.Color = Color.Blue
                oly.LineWidth = 1
                oly.HoseiX = 5
                oly.HoseiY = 5
                oly.ovlPos1 = New Point(m_DetectData.data.nKiriXmin, m_DetectData.data.nKiriYmin)
                oly.ovlPos2 = New Point(m_DetectData.data.nKiriXmax, m_DetectData.data.nKiriYmax)
                mcls_Img(ii).ResetOverLayInf()
                mcls_Img(ii).SetOverLayInf(oly)

                ' �؂�o���摜�\��
                mcls_Img(ii).SetImage(CType(m_DetectData.imgAngle(ii), Image))
                mcls_Img(ii).tProp_Paint_IsDefImgDspn = False
            Else
                ' �~�摜
                mcls_Img(ii).tProp_Paint_IsDefImgDspn = True
            End If


            '' �摜�\��
            mcls_Img(ii).tProp_Paint_IsPaint = True													' �摜�\����ON
            mcls_Img(ii).Refresh()
        Next


        '''' �X�N���[���o�[�͈�
        ' �X�N���[���o�[���f
        SetCommonScrollBar()


        scrNagateValueProgramable = -1 * mcls_Img(0).tProp_AriaInf_Offset.Y	' ���ݒl�ύX
        scrHabaValueProgramable = -1 * mcls_Img(0).tProp_AriaInf_Offset.X	 ' ���ݒl�ύX


        '''' ���̑���{���
        lblTid.Text = m_DetectData.data.tId.ToString
        lblGid.Text = m_DetectData.data.gId.ToString
        lblT.Text = mcls_pParam.GetParamType(m_DetectData.data.tId).sName
        lblG.Text = mcls_pParam.GetParamGrade(m_DetectData.data.gId).sName
        lblG.BackColor = mcls_pParam.GetParamGrade(m_DetectData.data.gId).clrDspColor
        lblNo.Text = m_DetectData.data.nNo.ToString
        lblFno.Text = m_DetectData.data.nFrameNo.ToString
        lblFnum.Text = m_DetectData.data.nFrameNum.ToString
        lblCamset.Text = CStr(m_DetectData.data.nCamPos + 1)
        lblY.Text = String.Format("{0:0.0}", m_DetectData.data.nY / 1000)
        'lblXds.Text = m_DetectData.data.nX.ToString
        'lblXws.Text = m_DetectData.data.nXws.ToString
        lblXds.Text = m_DetectData.data.nBaseX.ToString		' ���]�����Ă��l�͕ω������Ȃ�
        lblXws.Text = m_DetectData.data.nBaseXws.ToString
        picImg_Resize(Nothing, Nothing)
        Me.Activate()
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmKiriImg_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        SetFormLocationAtOnce = True


        Dim ii As Integer


        '''' �C���[�W�\���N���X�����Z�b�g
        '' �R���g���[���z��
        Dim pic(MC_MAX_IMG - 1) As PictureBox
        pic(0) = picImg4	' ������
        pic(1) = picImg3
        pic(2) = picImg2
        pic(3) = picImg1

        'For ii As Integer = 0 To m_DetectData.nImgCnt - 1
        For ii = 0 To MC_MAX_IMG - 1
            ' �N���X����
            mcls_Img(ii) = New tClass.tClass_ImageDsp()
            mcls_Img(ii).AddHandler_PaintEvent(pic(ii))
            ' �r�����摜���Z�b�g
            Try
                Dim asmData As System.Reflection.Assembly = System.Reflection.Assembly.GetExecutingAssembly()	' �A�Z���u���ɃA�N�Z�X
                Dim semImage As IO.Stream = asmData.GetManifestResourceStream(String.Format("{0}.�r����.bmp", Me.CompanyName))	 ' ���ߍ��܂ꂽ���\�[�X�ɃA�N�Z�X
                Call mcls_Img(ii).SetDefImage(New Bitmap(semImage))											  ' �f�t�H���g�摜�Z�b�g
            Catch ex As Exception
            End Try

            ' �������֌W
            Dim memo As tClass.tClass_ImageDspBase.MEMORI_INF
            memo.bIsDsp = True
            memo.LineWidth = 2
            memo.FontSize = 12
            memo.Color = Color.Cyan	  'Color.DarkGreen
            memo.HojoLineLen = 5

            memo.strBaseString = "0[mm]"
            memo.nPosStringBot = 15
            memo.nPosStringLeft = 5

            memo.nPosLineBot = 20
            memo.nPosLineLeft = 30

            memo.nLoopCntX = 2
            memo.nIntervalX = 30
            memo.nLoopCntY = 2
            memo.nIntervalY = 30
            mcls_Img(ii).tProp_MemoriInf = memo
        Next

        ' �R���{�{�b�N�X
        cmbPicRate.Items.Clear()
        cmbPicRate.Items.Add(New tClass_CmbItem(0, "�O�D�T�{", 5))
        cmbPicRate.Items.Add(New tClass_CmbItem(0, "�P�{", 10))
        cmbPicRate.Items.Add(New tClass_CmbItem(0, "�Q�{", 20))
        cmbPicRate.SelectedIndex = 1

        ' �`��Ώۂ̃s�N�`���[������
        For ii = 0 To MC_MAX_IMG - m_DetectData.nImgCnt - 1
            tlpMain.ColumnStyles(ii).Width = 0
        Next
        For ii = MC_MAX_IMG - m_DetectData.nImgCnt To MC_MAX_IMG - 1
            tlpMain.ColumnStyles(ii).Width = CSng((100 / (MC_MAX_IMG / m_DetectData.nImgCnt)))
        Next
        'For ii As Integer = 0 To MC_MAX_IMG - m_DetectData.nImgCnt - 1
        '	tlpMain.ColumnStyles(ii).Width = 0
        'Next
    End Sub
    ''' <summary>
    ''' �t�H�[�����B��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CloseFrom()
        For ii As Integer = 0 To m_DetectData.nImgCnt - 1
            mcls_Img(ii).tProp_Paint_IsPaint = False											' �摜�\����OFF
        Next ii

        ' �I��
        Me.Visible = False
    End Sub
    Private Sub frmKiriImg_MouseDoubleClick(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles Me.MouseDoubleClick
        CloseFrom()
    End Sub
    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClose.Click
        CloseFrom()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����
    '/ /////////////////////////////////////////////////////////////////////////////////



    ''' <summary>
    ''' ���T�C�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picImg_Resize(ByVal sender As Object, ByVal e As System.EventArgs) Handles picImg1.Resize, picImg2.Resize, picImg4.Resize, picImg3.Resize
        Dim pic(MC_MAX_IMG - 1) As PictureBox
        pic(0) = picImg4	' ������
        pic(1) = picImg3
        pic(2) = picImg2
        pic(3) = picImg1

        Dim bChange As Boolean = False
        For ii As Integer = 0 To m_DetectData.nImgCnt - 1
            If mcls_Img(ii) Is Nothing Then Return

            ' �T�C�Y�ύX
            mcls_Img(ii).SetAriaInf()
            mcls_Img(ii).Calculation()
            bChange = True
        Next
        If Not bChange Then Return


        SetCommonScrollBar()

        For ii As Integer = 0 To m_DetectData.nImgCnt - 1
            scrNagateValueProgramable = -1 * mcls_Img(ii).tProp_AriaInf_Offset.Y  ' ���ݒl�ύX
            scrHabaValueProgramable = -1 * mcls_Img(ii).tProp_AriaInf_Offset.X	' ���ݒl�ύX

            mcls_Img(ii).Refresh()
        Next
    End Sub

    ''' <summary>
    ''' �X�N���[���o�[ �ő�T�C�Y�ύX
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetCommonScrollBar()
        ' �c�X�N���[���o�[ �L����
        If 0 >= mcls_Img(0).tProp_CalcAns.ImgNonAriaSize.Height Then
            scrNagate.Enabled = False
            scrNagate.Maximum = 0
        Else
            ' �G���[�ɂȂ�Ȃ��悤�ɑ΍�
            If scrNagate.Value > mcls_Img(0).tProp_CalcAns.ImgNonAriaSize.Height + 1 Then
                scrNagate.Value = mcls_Img(0).tProp_CalcAns.ImgNonAriaSize.Height + 1
            End If
            ' �T�C�Y�ύX�ɂ��X�N���[���o�[�l�ύX
            scrNagate.Maximum = mcls_Img(0).tProp_CalcAns.ImgNonAriaSize.Height
            scrNagate.Minimum = 0

            scrNagate.Enabled = True
        End If


        ' ���X�N���[���o�[ �L����
        If 0 >= mcls_Img(0).tProp_CalcAns.ImgNonAriaSize.Width Then
            scrHaba.Enabled = False
            scrHaba.Maximum = 0
        Else
            ' �G���[�ɂȂ�Ȃ��悤�ɑ΍�
            If scrHaba.Value > mcls_Img(0).tProp_CalcAns.ImgNonAriaSize.Width + 1 Then
                scrHaba.Value = mcls_Img(0).tProp_CalcAns.ImgNonAriaSize.Width + 1
            End If
            ' �T�C�Y�ύX�ɂ��X�N���[���o�[�l�ύX
            scrHaba.Maximum = mcls_Img(0).tProp_CalcAns.ImgNonAriaSize.Width
            scrHaba.Minimum = 0

            scrHaba.Enabled = True
        End If
    End Sub


    ''' <summary>
    ''' �c�X�N���[��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub vsbImg_ValueChanged(ByVal sender As Object, ByVal e As Boolean) Handles vsbImg.ValueChanged
        If e = True Then Return ' �v���O�������ŕύX�����ꍇ�́A������

        Dim val As Integer = -1 * vsbImg.Value
        For ii As Integer = 0 To m_DetectData.nImgCnt - 1
            If mcls_Img(ii) Is Nothing Then Exit Sub
            If nvOffsetOfPic(mcls_Img(ii)) = val Then Exit Sub
            nvOffsetOfPic(mcls_Img(ii)) = val

            mcls_Img(ii).Calculation()
            mcls_Img(ii).Refresh()
        Next
    End Sub
    ''' <summary>
    ''' ���X�N���[��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub hsbImg_ValueChanged(ByVal sender As Object, ByVal e As Boolean) Handles hsbImg.ValueChanged
        If e = True Then Return ' �v���O�������ŕύX�����ꍇ�́A������

        Dim val As Integer = -1 * hsbImg.Value
        For ii As Integer = 0 To m_DetectData.nImgCnt - 1
            If mcls_Img(ii) Is Nothing Then Exit Sub
            If nhOffsetOfPic(mcls_Img(ii)) = val Then Exit Sub
            nhOffsetOfPic(mcls_Img(ii)) = val

            mcls_Img(ii).Calculation()
            mcls_Img(ii).Refresh()
        Next
    End Sub

    ''' <summary>
    ''' �\���{���R���{�{�b�N�X
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbPicRate_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbPicRate.SelectedIndexChanged
        Dim dRate As Double = CType(cmbPicRate.SelectedItem, tClass_CmbItem).g_nItem / 10.0	 '�{��

        ' ���W�f�[�^�v�Z
        For ii As Integer = 0 To m_DetectData.nImgCnt - 1
            mcls_Img(ii).tProp_AriaInf_Zoom = New tClass.tClass_ImageDsp_Calc.RATE(dRate, dRate)
            mcls_Img(ii).Calculation()
        Next

        ' �X�N���[���o�[���f
        SetCommonScrollBar()

        ' �ĕ`��
        For ii As Integer = 0 To m_DetectData.nImgCnt - 1
            Call mcls_Img(ii).Refresh()
        Next
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �}�E�X����֌W
    '/ /////////////////////////////////////////////////////////////////////////////////
    Dim m_bIsImgDrag As Boolean = False				' �摜�����N���b�N�ł���ł���Œ��H
    Dim m_PointS As Point							' �J�n�����}�E�X�n�_

    ''' <summary>
    ''' �}�E�X�_�E��
    ''' </summary>
    Private Sub picFrameImg_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picImg1.MouseDown, picImg2.MouseDown, picImg4.MouseDown, picImg3.MouseDown
        Dim pic As PictureBox = CType(sender, PictureBox)
        If e.Button = Windows.Forms.MouseButtons.Left Then
            m_bIsImgDrag = True
            m_PointS = pic.PointToClient(Windows.Forms.Cursor.Position)
            pic.Capture = True

        End If
    End Sub


    ''' <summary>
    ''' �}�X�E�A�b�v
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picFrameImg_MouseUp(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picImg1.MouseUp, picImg2.MouseUp, picImg4.MouseUp, picImg3.MouseUp
        Dim pic As PictureBox = CType(sender, PictureBox)
        If e.Button = Windows.Forms.MouseButtons.Left Then
            m_bIsImgDrag = False
            pic.Capture = False
        End If
    End Sub

    ''' <summary>
    ''' �}�E�X�ړ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picImg_MouseMove(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picImg1.MouseMove, picImg2.MouseMove, picImg4.MouseMove, picImg3.MouseMove
        Dim pic As PictureBox = CType(sender, PictureBox)

        '''' �摜�ړ�
        If m_bIsImgDrag Then
            Dim PointE As Point = pic.PointToClient(Windows.Forms.Cursor.Position)
            Dim p As Point

            If hsbImg.Enabled Then
                p.X = (m_PointS.X - PointE.X) + hsbImg.Value
                If p.X < 0 Then p.X = 0
                If p.X >= hsbImg.Maximum Then p.X = hsbImg.Maximum - 1

                For ii As Integer = 0 To m_DetectData.nImgCnt - 1
                    nhOffsetOfPic(mcls_Img(ii)) = -p.X
                Next
                hsbImg.ValueProgramable = p.X
            End If

            If vsbImg.Enabled Then
                p.Y = (m_PointS.Y - PointE.Y) + vsbImg.Value
                If p.Y < 0 Then p.Y = 0
                If p.Y >= vsbImg.Maximum Then p.Y = vsbImg.Maximum - 1

                For ii As Integer = 0 To m_DetectData.nImgCnt - 1
                    nvOffsetOfPic(mcls_Img(ii)) = -p.Y
                Next
                vsbImg.ValueProgramable = p.Y
            End If

            ' ���ݒl���ăZ�b�g
            m_PointS = pic.PointToClient(Windows.Forms.Cursor.Position)

            ' �S���I�������ĕ`��
            For ii As Integer = 0 To m_DetectData.nImgCnt - 1
                mcls_Img(ii).Calculation()
            Next
            For ii As Integer = 0 To m_DetectData.nImgCnt - 1
                Call mcls_Img(ii).Refresh()
            Next
        End If
    End Sub


End Class