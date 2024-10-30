Option Strict On
'*********************************************************************************
' �P�x�␳��ʃN���X
'	[Ver]
'		Ver.01    2014/07/25  ����
'
'
Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmKidoHosei

    ''' <summary>
    ''' �����o�ϐ�
    ''' </summary>
    ''' <remarks></remarks>
    ''' 
    Private mcls_ImgDsp As tClass_ImageDsp         ' �摜�\���N���X
    Private m_PicBox As PictureBox                 ' �摜�\���̈�
    Private mcls_Log As tClass_LogManager          ' ���O�Ǘ�
    Private m_pgb As ProgressBar                   ' �v���O���X�o�[

    Dim m_CorSave As String                        ' �␳�l�ۑ���

    Dim m_PicSouce() As Byte                       ' ���̉摜
    Dim m_PicCorrected() As Byte                   ' �␳�ς݉摜

    Dim m_KidoHAve() As Integer                    ' �c���ϋP�x�z��
    Dim m_KidoMvAve() As Integer                   ' �c���ϋP�x�z��(�P�x�ړ�����)
    Dim m_KidoHosei() As Integer                   ' �P�x�␳�l�z��
    Dim m_KidoBasePoint() As Integer               ' ��_�P�x�l�z��

    Dim m_bRaw As Boolean = False                  ' �P�x�v�Z����Flag�iFalse�F�������ATrue�F�����j
    Dim m_bAve As Boolean = False                  ' �P�x�i�ړ����ρj�v�Z����Flag�iFalse�F�������ATrue�F�����j
    Dim m_bCorPic As Boolean = False               ' �P�x�␳�ς݉摜Flag
    Dim m_bCorCalc As Boolean = False              ' �P�x�␳�v�Z����Flag
    Dim m_bBasePoint As Boolean = False            ' ��_�P�x�lFlag

    Dim m_nMvAveSize As Integer                    ' �ړ����σT�C�Y
    Dim m_nCameraSet As Integer                    ' �J�����Z�b�g
    Dim m_nBasePoint As Integer                    ' ��_�P�x�l
    Dim m_nKidoMax As Integer = 0                  ' �ő�P�x�l
    Dim m_nKidoMax_A As Integer = 0                ' �ړ����ύő�P�x�l
    Dim m_nPicWidth As Integer                     ' �摜�̕��T�C�Y
    Dim m_nPicHeight As Integer                    ' �摜�̏c�T�C�Y
    Dim m_nSelectX As Integer                      ' �������̑I���ʒu

    Dim mclr_LineRaw As Color                      ' ���̐F�i�c�v���W�F�N�V���������j
    Dim mclr_LineAve As Color                      ' ���̐F�i�c�v���W�F�N�V�������ړ����ρj
    Dim mclr_LineCor As Color                      ' ���̐F�i�␳�P�x���j
    Dim mclr_LineSel As Color                      ' ���̐F�i�������I����j

    ''' <summary>
    ''' �萔��`
    ''' </summary>
    ''' <remarks></remarks>
    Private Const ROW_KIDO_RAW As Integer = 1             ' �c�v���W�F�N�V�����i���j�̍s�ԍ�
    Private Const ROW_KIDO_AVE As Integer = 2             ' �c�v���W�F�N�V�������i�ړ����ρj�̍s�ԍ�
    Private Const ROW_KIDO_COR As Integer = 3             ' �␳�l�̍s�ԍ�
    Private Const ROW_KIDO_BP As Integer = 4              ' ��_�P�x�l�̍s�ԍ�

    Private Const RAD_INIT As Integer = 0                 ' ���W�I�{�^���i������ԁj
    Private Const RAD_BEFORE As Integer = 1               ' ���W�I�{�^���i�␳�O�j
    Private Const RAD_AFTER As Integer = 2                ' ���W�I�{�^���i�␳��j
    Private Const RAD_COR As Integer = 3                  ' ���W�I�{�^���i�␳�l�j

    Private Const MY_SECTION As String = "KIDO_HOSEI"     ' ini�t�@�C���Ɏg�p���鎩�Z�N�V������
    Private Const KEY_MV_AVE As String = "MV_AVE_SIZE"    ' ini�t�@�C���Ɏg�p����ړ����σT�C�Y��KEY
    Private Const HOSEI_SAVE As String = "HOSEI_SAVE"     ' �␳�l�o�͐�KEY
    Private Const PATH_COR_DEF As String = ".\KidoHosei"  ' �␳�l�̏o�͐�(Default�l)
    Private Const PATH_PIC_DEF As String = "..\TEMP\BMP"  ' �\���摜�̏o�͐�(Default�l)

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="log"></param>
    ''' <remarks></remarks>
    Public Sub New(ByVal log As tClass_LogManager)

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
        mcls_Log = log

    End Sub

    ''' <summary>
    ''' Load�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKidoHosei_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�P�x�␳�t�H�[���C�x���g�J�n "))

        mcls_ImgDsp = New tClass_ImageDsp()
        m_pgb = pgbProcess
        m_PicBox = pbxBitMap
        mcls_ImgDsp.AddHandler_PaintEvent(m_PicBox)

        ' PictureBox�̃v���p�e�B�[�ł͕ύX�ł��Ȃ����߂����ŕύX����
        pbxBitMap.AllowDrop = True
        ' ini̧�ٓǍ�
        ReadIniFile()
        ' dgv�̐ݒ�
        dgvInitialize(dgvKido)
        ' �c�v���W�F�N�V�����i�ړ����ρj��CheckBox��Default����
        chkBoxHProMvAve.Checked = True
        ' CheckBox�̃e�L�X�g�̐F�ݒ�
        chkBoxHProRow.ForeColor = mclr_LineRaw
        chkBoxHProMvAve.ForeColor = mclr_LineAve
        chkBoxCorrected.ForeColor = mclr_LineCor
        chkBoxSelect.ForeColor = mclr_LineSel
        ' PictureBox�̐ݒ�
        mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth
        pbxBitMap.Dock = DockStyle.Fill
        HScrollBM.Visible = False
        VScrollBM.Visible = False
        ' ���W�I�{�^���̐ݒ�
        ChgRadBtn(RAD_INIT)
        ' �ő�l�ړ��ֈړ��{�^��
        btnRawMax.Enabled = False
        btnAveMax.Enabled = False

        ' �C�x���g��`
        AddHandler mcls_ImgDsp.tEv_Paint_End, AddressOf PaintPicBoxFrame
    End Sub

    ''' <summary>
    ''' FormClose�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKidoHosei_FormClosed(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosedEventArgs) Handles MyBase.FormClosed
        SaveData()
        modMain.TaskEnd()
    End Sub

    ''' <summary>
    ''' �h���b�O�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub EV_DragEnter(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles pbxBitMap.DragEnter, Me.DragEnter
        Dim file() As String
        Dim ex As String

        If e.Data.GetDataPresent(DataFormats.FileDrop) Then
            file = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
            ex = IO.Path.GetExtension(file(0))
            ' BitMap��Jpeg�ȊO�͎󂯕t���Ȃ�
            If ex = ".bmp" Or ex = ".BMP" Or ex = ".jpg" Or ex = ".JPG" Or ex = ".jpeg" Or ex = ".JPEG" Then
                e.Effect = DragDropEffects.Copy
            End If
        End If
    End Sub

    ''' <summary>
    ''' �h���b�v�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub EV_DragDrop(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles pbxBitMap.DragDrop, Me.DragDrop
        Dim file As String()
        Dim byImg As Byte()

        Try
            file = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
            Dim BmPath As String = file(0)
            Dim ex As String = IO.Path.GetExtension(BmPath)
            Dim img As Image = tClass.tClass_BmpJpeg.ImgFileLoad(BmPath)

            ' ��荞�݉摜�̃����o��ݒ�
            Dim width As Integer = img.Width
            Dim height As Integer = img.Height

            ' �摜�f�[�^����o�C�g�z��ւ̕ϊ��f�[�^���擾����
            If ex = ".bmp" Or ex = ".BMP" Then
                byImg = tClass.tClass_BmpJpeg.ImagToData(CType(img, Bitmap))
            Else
                byImg = tClass.tClass_BmpJpeg.JpegToData(BmPath)
            End If

            If Not byImg Is Nothing Then
                ' �摜�����\���͑S�̂ɂ���
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emRaw Then
                    RateChg()
                End If

                '' �����o�̏�����
                m_PicCorrected = Nothing
                m_bRaw = False
                m_bAve = False
                m_bCorPic = False
                m_bCorCalc = False
                m_bBasePoint = False
                m_KidoHAve = Nothing
                m_KidoMvAve = Nothing
                m_KidoHosei = Nothing
                m_KidoBasePoint = Nothing

                m_nKidoMax = 0
                m_nKidoMax_A = 0

                m_nSelectX = 0

                ' MAX�\���{�^��
                btnRawMax.Enabled = False
                btnAveMax.Enabled = False

                ' �O���b�h��H�X�N���[���o�[
                HScrollDgv.Value = 0

                ' �摜�\��
                DisplayImage(width, height, byImg)

                ' �O���b�h�X�V
                dgvKido.Rows.Clear()
                dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)

                ' ��荞�݉摜�̃����o��ݒ肷��
                m_PicSouce = byImg
                m_nPicWidth = width
                m_nPicHeight = height

                ' ���W�I�{�^���̏�����
                ChgRadBtn(RAD_INIT)
                btnHoseitiRead.Enabled = False
            Else
                Dim frm As New tClassForm.frmPop_MsgBox("�摜�̎�荞�݂Ɏ��s���܂���", "�h���b�O�h���b�v")
                frm.Show()
            End If
        Catch ex As Exception
            ' 8bit�摜�ȊO�͗�O�������܂�
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�h���b�v�C�x���g�ŗ�O���������܂��� [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox("�摜�̎�荞�݂Ɏ��s���܂���", "�h���b�O�h���b�v")
            frm.Show()
        End Try
    End Sub

    ''' <summary>
    ''' PictureBox�ւ̕`��C�x���g
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub PaintPicBoxFrame(ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim g As Graphics = e.Graphics
        Dim pen_Law As New Pen(mclr_LineRaw, 2)
        Dim pen_Ave As New Pen(mclr_LineAve, 2)
        Dim pen_Cor As New Pen(mclr_LineCor, 2)
        Dim pen_Sel As New Pen(mclr_LineSel, 2)
        Dim pbx_width As Integer = pbxBitMap.ClientSize.Width
        Dim startP As Integer
        Dim endP As Integer

        ' PicBox�����ɍ��킹��0~255�͈̔͂�Line���Ђ�
        Dim yy As Double = pbxBitMap.Height / 255     ' PicBox������0~255�͈̔͂ɂ����ꍇ��Y��1�P�ʂ̒l
        Try
            ' �c�v���W�F�N�V�����i���j�`��
            If m_bRaw = True AndAlso chkBoxHProRow.Checked = True Then
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                    ' �摜�S�̕\����
                    startP = 0               ' �摜�̍��[
                    endP = m_nPicWidth - 1   ' �摜�̉E�[
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Law, CInt((i - 1) * (pbx_width / m_nPicWidth)), CInt(pbxBitMap.Height - (m_KidoHAve(i - 1) * yy)), CInt((i) * (pbx_width / m_nPicWidth)), CInt(pbxBitMap.Height - (m_KidoHAve(i) * yy)))
                        End If
                    Next
                Else
                    ' ���摜�\����
                    startP = HScrollBM.Value                        ' �摜�̕\���ʒu�̍��[
                    endP = pbxBitMap.Size.Width + startP - 1        ' �摜�̕\���ʒu�̉E�[
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Law, i - startP - 1, pbxBitMap.Height - CInt((m_KidoHAve(i - 1) * yy)), i - startP, pbxBitMap.Height - CInt((m_KidoHAve(i)) * yy))
                        End If
                    Next

                End If
            End If

            ' �c�v���W�F�N�V�����i�ړ����ρj�`��
            If m_bAve = True AndAlso chkBoxHProMvAve.Checked = True Then
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                    ' �摜�S�̕\����
                    startP = 0               ' �摜�̍��[
                    endP = m_nPicWidth - 1   ' �摜�̉E�[
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Ave, CInt((i - 1) * (pbx_width / m_nPicWidth)), pbxBitMap.Height - CInt((m_KidoMvAve(i - 1)) * yy), CInt((i) * (pbx_width / m_nPicWidth)), pbxBitMap.Height - CInt(m_KidoMvAve(i) * yy))
                        End If
                    Next
                Else
                    ' ���摜�\����
                    startP = HScrollBM.Value                         ' �摜�̕\���ʒu�̍��[
                    endP = pbxBitMap.Size.Width + startP - 1         ' �摜�̕\���ʒu�̉E�[
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Ave, i - startP - 1, pbxBitMap.Height - CInt((m_KidoMvAve(i - 1) * yy)), i - startP, pbxBitMap.Height - CInt((m_KidoMvAve(i) * yy)))
                        End If
                    Next

                End If
            End If

            ' �␳�P�x���`��
            If Not m_KidoBasePoint Is Nothing AndAlso chkBoxCorrected.Checked = True Then
                'g.DrawLine(pen_Cor, 0, pbxBitMap.Height - CInt(m_KidoBasePoint(0) * yy), pbx_width, pbxBitMap.Height - CInt(m_KidoBasePoint(0) * yy))
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                    ' �摜�S�̕\����
                    startP = 0               ' �摜�̍��[
                    endP = m_nPicWidth - 1   ' �摜�̉E�[
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Cor, CInt((i - 1) * (pbx_width / m_nPicWidth)), pbxBitMap.Height - CInt((m_KidoBasePoint(i - 1)) * yy), CInt((i) * (pbx_width / m_nPicWidth)), pbxBitMap.Height - CInt(m_KidoBasePoint(i) * yy))
                        End If
                    Next
                Else
                    ' ���摜�\����
                    startP = HScrollBM.Value                         ' �摜�̕\���ʒu�̍��[
                    endP = pbxBitMap.Size.Width + startP - 1         ' �摜�̕\���ʒu�̉E�[
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Cor, i - startP - 1, pbxBitMap.Height - CInt((m_KidoBasePoint(i - 1) * yy)), i - startP, pbxBitMap.Height - CInt((m_KidoBasePoint(i) * yy)))
                        End If
                    Next

                End If
            End If

            ' �␳�P�x���`��
            If m_nSelectX > 0 AndAlso chkBoxSelect.Checked = True Then
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                    ' �摜�S�̕\����
                    Dim nSel As Integer = CInt(pbx_width / m_nPicWidth * m_nSelectX)
                    g.DrawLine(pen_Sel, nSel, 0, nSel, pbxBitMap.Height)
                    'g.DrawLine(pen_Law, CInt((i - 1) * (pbx_width / m_nPicWidth)), CInt(pbxBitMap.Height - (m_KidoHAve(i - 1) * yy)), CInt((i) * (pbx_width / m_nPicWidth)), CInt(pbxBitMap.Height - (m_KidoHAve(i) * yy)))
                Else
                    ' ���摜�\����
                    If m_nSelectX >= HScrollBM.Value And m_nSelectX < (HScrollBM.Value + pbxBitMap.Size.Width) Then
                        Dim nSel As Integer = m_nSelectX - HScrollBM.Value
                        g.DrawLine(pen_Sel, nSel, 0, nSel, pbxBitMap.Height)
                    End If
                End If
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�y�C���g�C�x���g���ɗ�O���������܂��� [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �摜�̕\�����@�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRateChg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRateChg.Click
        InformLbl("", False, lblInform)
        RateChg()
    End Sub

    ''' <summary>
    ''' �T�C�Y�ؑւ��s��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub RateChg()
        HScrollBM.Minimum = 0
        HScrollBM.Maximum = m_nPicWidth - 1

        VScrollBM.Minimum = 0
        VScrollBM.Maximum = m_nPicHeight - 1

        If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
            ' �S�̃T�C�Y����~1�T�C�Y�֐ؑ�
            btnRateChg.Text = "�S��"
            HScrollBM.Visible = True
            VScrollBM.Visible = True
            mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emRaw
            HScrollBM.LargeChange = pbxBitMap.Size.Width
            VScrollBM.LargeChange = pbxBitMap.Size.Height
            pbxBitMap.Height = pbxBitMap.Parent.ClientSize.Height - VScrollBM.Width
            pbxBitMap.Width = pbxBitMap.Parent.ClientSize.Width - HScrollBM.Height
            pbxBitMap.Dock = DockStyle.None
        Else
            ' �~�P�T�C�Y����S�̃T�C�Y�֐ؑ�
            btnRateChg.Text = "�~1"
            mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth
            pbxBitMap.Dock = DockStyle.Fill
            HScrollBM.Visible = False
            VScrollBM.Visible = False
            HScrollBM.LargeChange = m_nPicWidth
            VScrollBM.LargeChange = m_nPicHeight
        End If

        mcls_ImgDsp.tProp_AriaInf_Aria = m_PicBox.ClientSize
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                         ' �I��F����
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .RowTemplate.Height = 27                                      ' ���s����Ă镶���������Ă��܂��̂ŁA�f�t�H���g�̍s������ύX
            .ScrollBars = ScrollBars.None                                 ' �X�N���[���o�[�̕\��

            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

            .ColumnHeadersVisible = False                                 ' ��w�b�_�\��
            .RowHeadersVisible = False                                    ' �s�w�b�_�\��

        End With
    End Sub

    ''' <summary>
    ''' ���W�I�{�^���̕ύX
    ''' �w��̃{�^����L���ɂ���iRAD_INIT�͏�����ԁj
    ''' </summary>
    ''' <param name="i_btn">���W�I�{�^��</param>
    ''' <remarks></remarks>
    Private Sub ChgRadBtn(ByVal i_btn As Integer)
        Select Case i_btn
            Case RAD_INIT
                radBefore.Checked = True
                radBefore.Enabled = False
                radAfter.Checked = False
                radAfter.Enabled = False
                radCor.Checked = False
                radCor.Enabled = False
                Exit Select
            Case RAD_BEFORE
                radBefore.Checked = False
                radBefore.Enabled = True
                radAfter.Checked = True
                radAfter.Enabled = False
                Exit Select
            Case RAD_AFTER
                radBefore.Checked = True
                radBefore.Enabled = False
                radAfter.Checked = False
                radAfter.Enabled = True
                Exit Select
            Case RAD_COR
                radCor.Checked = False
                radCor.Enabled = True
                Exit Select
        End Select

    End Sub

    ''' <summary>
    ''' �P�x�v�Z�{�^���N���b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btn_brightnessCheck_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKidoCheck.Click
        InformLbl("", False, lblInform)
        Dim s1 As String = ""
        Dim s2 As String = ""
        Dim s3 As String = ""
        Dim s4 As String = ""
        If Not m_PicSouce Is Nothing Then
            If m_bRaw = False Then
                ' �c�v���W�F�N�V�������i���j�̋P�x���擾
                m_KidoHAve = GetHeightBrightness(m_PicSouce, m_nPicWidth, m_nPicHeight)

                If Not m_KidoHAve Is Nothing Then
                    m_nKidoMax = GetArrayMax(m_KidoHAve)
                    ' �e�L�X�g�{�b�N�X�ɋP�x�ő�l��\��
                    If m_nKidoMax <> 0 Then
                        txtBasePointKido.Text = CStr(m_nKidoMax)
                    End If
                    btnRawMax.Enabled = True
                    m_bRaw = True
                    s1 = "�c�v���W�F�N�V������ "
                End If
            End If

            '�c�v���W�F�N�V�����i�ړ����ρj�̋P�x���擾
            If txtMoveAveraveSize.Text IsNot Nothing AndAlso txtMoveAveraveSize.Text <> "" Then
                If IsNumeric(txtMoveAveraveSize.Text) Then
                    If GetOddNumber(CInt(txtMoveAveraveSize.Text), 3) <= (m_nPicWidth - 1) \ 2 Then
                        ' �ړ����σT�C�Y�͊�̂�
                        m_nMvAveSize = GetOddNumber(CInt(txtMoveAveraveSize.Text), 3)
                        ' �ړ����σT�C�Y�e�L�X�g���X�V
                        txtMoveAveraveSize.Text = CStr(m_nMvAveSize)
                        ' �ړ����ϋP�x�z����擾����
                        m_KidoMvAve = GetMoveAvePro(m_KidoHAve, m_nMvAveSize)
                        If m_bAve = False Then
                            ' �ړ����όv�Z��Ɋ�_�P�x�l������͂����ꍇ�͎���͗D��
                            If Not m_KidoMvAve Is Nothing Then
                                m_nKidoMax_A = GetArrayMax(m_KidoMvAve)
                                ' ��_�P�x���X�V����
                                txtBasePointKido.Text = CStr(m_nKidoMax_A)
                            End If
                            btnAveMax.Enabled = True
                            m_bAve = True
                            s2 = "�c�v���W�F�N�V�����ړ����� "
                        End If
                    Else
                        Dim frm As New tClassForm.frmPop_MsgBox(String.Format("�ړ����σT�C�Y��[{0}]�ȉ��̒l����͂��Ă�������", CStr((m_nPicWidth - 1) \ 2)), "�P�x�v�Z�{�^��")
                        frm.Show()
                    End If
                End If
            Else
                Dim frm As New tClassForm.frmPop_MsgBox("�ړ����σT�C�Y����͂��Ă�������", "�P�x�v�Z�{�^��")
                frm.Show()
            End If

            ' �␳�l�̌v�Z
            If m_bAve = True Then
                If Not txtBasePointKido.Text Is Nothing AndAlso Not txtBasePointKido.Text = "" Then
                    If IsNumeric(txtMoveAveraveSize.Text) Then
                        If Not CInt(txtBasePointKido.Text) > 255 Then       ' ��_�P�x�ő�l��255�܂�
                            m_KidoHosei = GetKidoHosei(m_KidoMvAve, CInt(txtBasePointKido.Text))
                        Else
                            Dim frm As New tClassForm.frmPop_MsgBox("��_�P�x�l��0�`255�͈̔͂œ��͂��Ă�������", "�P�x�v�Z�{�^��")
                            frm.Show()
                        End If
                    End If
                    m_bCorCalc = True
                    ' �C���摜��\�����Ă�����A�␳�O�摜�ɖ߂�
                    If m_bCorPic = True Then
                        DisplayImage(m_nPicWidth, m_nPicHeight, m_PicSouce)
                        m_bCorPic = False
                    End If
                    ChgRadBtn(RAD_AFTER)
                    ChgRadBtn(RAD_COR)
                    btnHoseitiRead.Enabled = True

                    s3 = "�␳�l "
                Else
                    Dim frm As New tClassForm.frmPop_MsgBox("��_�P�x�l����͂��Ă�������", "�P�x�v�Z�{�^��")
                    frm.Show()
                End If
            End If

            ' ��_�P�x�l�̌v�Z
            If Not m_KidoHosei Is Nothing AndAlso Not m_KidoMvAve Is Nothing Then
                m_KidoBasePoint = GetKidoBasePoint(m_nPicWidth, m_KidoHosei, m_KidoMvAve)
                m_bBasePoint = True
                s4 = "��_�P�x�l "
            End If
            ' �O���b�h�X�V
            dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
            ' ��ʍX�V
            mcls_ImgDsp.Refresh()


            InformLbl(String.Format("{0}{1}{2}{3}�̌v�Z���������܂���", s1, s2, s3, s4), True, lblInform)
        Else
            Dim frm As New tClassForm.frmPop_MsgBox("�摜������܂���", "�P�x�v�Z�{�^��")
            frm.Show()
        End If
    End Sub

    ''' <summary>
    ''' ��_�P�x�l���擾����
    ''' </summary>
    ''' <param name="nWidth">�摜����</param>
    ''' <param name="nKidoHosei">�␳�l�z��</param>
    ''' <param name="nKidoMvAve">�ړ����ϋP�x�z��</param>
    ''' <returns>��_�P�x�l�z��</returns>
    ''' <remarks></remarks>
    Private Function GetKidoBasePoint(ByVal nWidth As Integer, ByVal nKidoHosei() As Integer, ByVal nKidoMvAve() As Integer) As Integer()
        Dim nKidoBasePoint(nWidth - 1) As Integer
        Dim iKidoBasePoint As Integer
        Try
            For ii As Integer = 0 To nWidth - 1
                iKidoBasePoint = (nKidoHosei(ii) + nKidoMvAve(ii))
                nKidoBasePoint(ii) = iKidoBasePoint
            Next
        Catch ex As Exception
            nKidoBasePoint = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("��_�P�x�l�v�Z���ɗ�O���������܂��� [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox("��_�P�x�l�v�Z���ɗ�O���������܂���", "��_�P�x�l�v�Z��")
            frm.Show()
        End Try

        Return nKidoBasePoint

    End Function

    ''' <summary>
    ''' �P�x�␳�l���擾����
    ''' </summary>
    ''' <param name="i_mvave">�ړ����ϋP�x�z��</param>
    ''' <param name="i_bp">��_�P�x�l</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetKidoHosei(ByVal i_mvave() As Integer, ByVal i_bp As Integer) As Integer()
        Dim i_cor(i_mvave.Length - 1) As Integer    ' �␳�l�z��
        Dim value As Integer                        ' �␳�l

        If Not i_mvave Is Nothing Then
            For i As Integer = 0 To i_mvave.Length - 1
                value = (i_bp - i_mvave(i))
                i_cor(i) = value
            Next
        Else
            i_cor = Nothing
        End If
        Return i_cor
    End Function

    ''' <summary>
    ''' �J�����Z�b�g�L�[����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtCameraSet_KeyPress(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles txtCameraSet.KeyPress
        '������Backspace�̂ݓ��͉�
        If (e.KeyChar < "0"c OrElse "9"c < e.KeyChar) AndAlso e.KeyChar <> ControlChars.Back Then
            e.Handled = True
        End If
    End Sub

    ''' <summary>
    ''' ��_�P�x�l����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtBasePointKido_KeyPress(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles txtBasePointKido.KeyPress
        '������Backspace�̂ݓ��͉�
        If (e.KeyChar < "0"c OrElse "9"c < e.KeyChar) AndAlso e.KeyChar <> ControlChars.Back Then
            e.Handled = True
        End If
        m_bCorCalc = False
    End Sub

    ''' <summary>
    ''' �ړ����σT�C�Y����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtMoveAveraveSize_KeyPress(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles txtMoveAveraveSize.KeyPress
        '������Backspace�̂ݓ��͉�
        If (e.KeyChar < "0"c OrElse "9"c < e.KeyChar) AndAlso e.KeyChar <> ControlChars.Back Then
            e.Handled = True
        End If
        m_bAve = False
    End Sub

    ''' <summary>
    ''' �o�̓{�^�����N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btn_export_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExport.Click
        Dim path As String
        Dim filename As String

        InformLbl("", False, lblInform)
        path = txtCorPath.Text

        If Not txtCameraSet.Text = "" Then  ' �J�����Z�b�g�����͍ς݂ł��邱��
            If m_bCorCalc = True Then       ' �P�x�␳�v�Z�ς݂ł��邱��

                filename = String.Format("KidoHosei_{0}.csv", txtCameraSet.Text)

                ' TextBox��Path��D��
                If CreateCsvFile(path, filename, m_KidoHosei) = True Then
                    InformLbl(String.Format("�t�@�C���o�͊������܂����B" & vbCrLf & "[{0}]", System.IO.Path.GetFullPath(IO.Path.Combine(path, filename))), True, lblInform)
                    m_CorSave = path
                    tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, HOSEI_SAVE, LOCALINI_NAME, m_CorSave)
                End If
            Else
                Dim frm As New tClassForm.frmPop_MsgBox("�P�x�v�Z��ɕ␳�l�o�͂��s���Ă�������", "�o�̓{�^��")
                frm.Show()
            End If
        Else
            Dim frm As New tClassForm.frmPop_MsgBox("�J�����Z�b�g����͂��Ă�������", "�o�̓{�^��")
            frm.Show()
        End If

    End Sub

    ''' <summary>
    ''' �n�[�h�R�s�[�{�^�����N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btn_HardCopy_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHardCopy.Click
        InformLbl("", False, lblInform)     ' ���x���ʒm���N���A

        Dim path As String = modMain.HardCopy(Me)
        Dim frm As New tClassForm.frmPop_MsgBox(String.Format("�o�͐�F[{0}]", System.IO.Path.GetFullPath(path)), "�n�[�h�R�s�[����")
        frm.Show()
    End Sub

    ''' <summary>
    ''' �v���W�F�N�V�������i���jCheckedChanged
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub CheckedChanged_HeightPro_Raw(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBoxHProRow.CheckedChanged
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' �v���W�F�N�V�������i�ړ����ρjCheckedChanged
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub CheckedChanged_HeightPro_MoveAverage_(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBoxHProMvAve.CheckedChanged
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' �␳�P�x�� CheckedChanged
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub CheckedChanged_BoxCorrectedKido(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBoxCorrected.CheckedChanged
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' �������I��� CheckedChanged
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub CheckedChanged_BoxSelectX(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBoxSelect.CheckedChanged
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' CSV�t�@�C���쐬
    ''' </summary>
    ''' <param name="strPath">�ۑ�����t�H���_��</param>
    ''' <param name="strTitle">File��</param>
    ''' <returns>True:���� False:���s</returns>
    ''' <remarks></remarks>
    Public Function CreateCsvFile(ByVal strPath As String, ByVal strTitle As String, ByVal i_x() As Integer) As Boolean
        Dim bRet As Boolean = True
        Dim enc As System.Text.Encoding = System.Text.Encoding.GetEncoding("Shift_JIS")
        Dim fullpath As String
        Try
            ' �t�H���_��������΍쐬����
            tMod.CheckFolder(strPath)

            fullpath = IO.Path.Combine(strPath, strTitle)


            Using sr As New System.IO.StreamWriter(fullpath, False, enc)
                Dim s As String
                ' �Z�N�V����
                s = "[KIDO]"
                sr.WriteLine(s)
                For i As Integer = 0 To i_x.Length - 1
                    s = CStr(i_x(i))
                    sr.WriteLine(s)
                Next

            End Using

        Catch ex As Exception
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("CSV�t�@�C���̏o�͂Ɏ��s���܂���[{0}] [{1}]", strPath, ex.Message), "�h���b�O�h���b�v")
            frm.Show()
            bRet = False
        End Try

        Return bRet

    End Function


    ''' <summary>
    ''' �␳�摜�쐬
    ''' </summary>
    ''' <param name="img">�摜�␳�O�̃f�[�^</param>
    ''' <param name="i_cor">�␳�l�z��</param>
    ''' <param name="i_Height">�摜�̍���</param>
    ''' <returns>�␳�摜�f�[�^</returns>
    ''' <remarks></remarks>
    Private Function CreateCorrectedPic(ByVal img() As Byte, ByVal i_cor() As Integer, ByVal i_Height As Integer) As Byte()
        Dim tmpImg(img.Length - 1) As Byte
        Dim i_value As Integer

        Try
            Dim i As Integer = 0
            Do Until i = img.Length
                For x As Integer = 0 To i_cor.Length - 1
                    ' �����l��0�Ƃ���
                    i_value = Math.Max(0, img(i) + i_cor(x))
                    ' �ő�P�x�l�𒴂����ꍇ��255�Ƃ���
                    tmpImg(i) = CByte(Math.Min(255, i_value))
                    i += 1
                Next
            Loop

        Catch ex As Exception
            tmpImg = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("CreateCorrectedPic�ŗ�O���������܂��� [{0}]", ex.Message))
        End Try

        Return tmpImg

    End Function

    ''' <summary>
    ''' �␳�l�摜���쐬����i0����_�ɕ␳�l��`�悵���摜�j
    ''' </summary>
    ''' <param name="img">�摜�␳�O�̃f�[�^</param>
    ''' <param name="i_cor">�␳�l�z��</param>
    ''' <returns>�␳�l�摜�f�[�^</returns>
    ''' <remarks></remarks>
    Private Function CreateCorrectionPic(ByVal img() As Byte, ByVal i_cor() As Integer) As Byte()
        Dim tmpImg(img.Length - 1) As Byte
        Dim i_value As Integer
        Try
            Dim i As Integer = 0
            Do Until i = img.Length
                For x As Integer = 0 To i_cor.Length - 1
                    ' �����l��0�Ƃ���
                    i_value = Math.Max(0, i_cor(x))
                    ' �ő�P�x�l�𒴂����ꍇ��255�Ƃ���
                    tmpImg(i) = CByte(Math.Min(255, i_value))
                    i += 1
                Next
            Loop
        Catch ex As Exception
            tmpImg = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("CreateCorrectionPic�ŗ�O���������܂��� [{0}]", ex.Message))
        End Try

        Return tmpImg

    End Function


    ''' <summary>
    ''' �����̏ꍇ��+�P������̒l��Ԃ�
    ''' </summary>
    ''' <param name="i">�l</param>
    ''' <param name="min">�����l</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetOddNumber(ByVal i As Integer, ByVal min As Integer) As Integer
        Dim oddNumber As Integer = i

        If Not CBool(i Mod 2) Then
            oddNumber += 1
        End If

        If min > oddNumber Then
            oddNumber = min
        End If

        Return oddNumber
    End Function

    ''' <summary>
    ''' �摜�i�c�j�̋P�x���ς�Ԃ�
    ''' </summary>
    ''' <param name="m_brightness">�P�x�z��</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Average(ByVal m_brightness As Integer()) As Integer
        Dim ave As Integer = 0
        Dim sum As Integer = 0

        '���v
        For a As Integer = 0 To m_brightness.Length - 1
            sum += m_brightness(a)
        Next
        '����
        ave = CInt(sum / m_brightness.Length)

        Return ave

    End Function

    ''' <summary>
    ''' BitMap�̏c�������̋P�x���ς��v�Z����
    ''' </summary>
    ''' <param name="byPic">���͉摜</param>
    ''' <param name="iWidth">�摜��</param>
    ''' <param name="iHeight">�摜����</param>
    ''' <returns>�P�x���ϔz��</returns>
    ''' <remarks></remarks>
    Private Function GetHeightBrightness(ByVal byPic() As Byte, ByVal iWidth As Integer, ByVal iHeight As Integer) As Integer()
        Dim i_x(iWidth - 1) As Integer         ' X�������̔z��
        Dim i_y(iHeight - 1) As Integer        ' Y�������̔z��
        Dim point As Integer
        ' �v���O���X�o�[�J�n
        InformLbl(String.Format(vbCrLf & "�P�x���όv�Z��"), True, lblInform)
        StartProgressBar(0, 0, m_pgb)
        Try
            For x As Integer = 0 To iWidth - 1
                If 0 = x Mod 100 Then
                    ' �v���O���X�o�[�X�V
                    UpdateProgressBar(x, iWidth - 1, m_pgb)
                End If

                ' �c�̋P�x���擾
                For y As Integer = 0 To iHeight - 1
                    point = x + y * iWidth
                    i_y(y) = byPic(point)
                Next
                ' �c�̋P�x���ς����߂�
                i_x(x) = Average(i_y)
            Next

        Catch ex As Exception
            i_x = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("GetHeightBrightness�ŗ�O�������܂��� [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("�P�x���όv�Z���ɗ�O���������܂���"), "�P�x���όv�Z")
            frm.Show()
        End Try

        InformLbl("", False, lblInform)
        FinishProgressBar(m_pgb)
        Return i_x

    End Function

    ''' <summary>
    ''' �c�v���W�F�N�V�����i�ړ����ρj���擾����
    ''' </summary>
    ''' <param name="kidoH">�c���ϋP�x�f�[�^</param>
    ''' <param name="mvave">�ړ����σT�C�Y</param>
    ''' <returns>�ړ����ϔz��</returns>
    ''' <remarks></remarks>
    Private Function GetMoveAvePro(ByVal kidoH() As Integer, ByVal mvave As Integer) As Integer()
        Dim mv As Integer = mvave \ 2               ' ���ݒn����O��Ɉړ�����l
        Dim i_ave(kidoH.Length - 1) As Integer      ' X�������ړ����ς̔z��
        Dim total As Integer

        Try
            ' �ړ����ς̌v�Z����
            For i As Integer = 0 To kidoH.Length - 1     ' i:���݈ʒu
                total = 0
                For ii As Integer = i - mv To i + mv     ' ii:���ݒn����O��ړ��ʒu
                    If 0 <= ii Then
                        If kidoH.Length - 1 >= ii Then
                            total += kidoH(ii)
                        Else
                            ' �E���͈֔͊O�̏ꍇ�A���O�̈ړ����ς����Z
                            If i = kidoH.Length - 1 Then
                                total += kidoH(i)
                            Else
                                total += kidoH(i + 1)
                            End If
                        End If
                    Else
                        ' �����͈֔͊O�̏ꍇ
                        If i = 0 Then   ' ���[�́A��ʒu�̋P�x�����Z
                            total += kidoH(i)
                        Else
                            total += kidoH(i - 1)  '���O�̋P�x�����Z 
                        End If
                    End If
                Next
                ' �c�ړ����ς����߂�
                i_ave(i) = CInt(total / m_nMvAveSize)
            Next
        Catch ex As Exception
            i_ave = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("GetMoveAvePro�ŗ�O�������܂��� [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("�ړ����όv�Z���ɗ�O���������܂���"), "�ړ����όv�Z")
            frm.Show()
        End Try

        Return i_ave

    End Function

    ''' <summary>
    ''' �z��̍ő�l�����߂�
    ''' </summary>
    ''' <param name="i_x">�z��</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetArrayMax(ByVal i_x() As Integer) As Integer
        Dim max As Integer
        Dim ar As New ArrayList(i_x)
        ar.Sort()
        max = CInt(ar(ar.Count() - 1))
        ar.Clear()

        Return max
    End Function

    ''' <summary>
    ''' INI�t�@�C���Ǎ�
    ''' KizuTask.ini�ɐݒ肪�Ȃ���΁ADefault�ݒ�l�ŏo�͂��܂�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadIniFile()
        Dim strCollor As String
        Dim strMvAveSize As String
        Const COLLOR_RAW As String = "COLLOR_RAW"     '// �c�v���W�F�N�V���������L�[
        Const COLLOR_AVE As String = "COLLOR_AVE"     '// �c�v���W�F�N�V�����ړ����ϐ��L�[
        Const COLLOR_COR As String = "COLLOR_COR"     '// �␳�P�x���L�[
        Const COLLOR_SEL As String = "COLLOR_SEL"     '// ���ʒu�I����L�[
        Const RED As String = "255,0,0"
        Const BLUE As String = "0,0,255"
        Const GREEN As String = "0,128,0"
        Const PINK As String = "255,174,201"

        ' �␳�l�ۊǐ�擾
        m_CorSave = tMod.ReadIniStr(MY_SECTION, HOSEI_SAVE, LOCALINI_NAME, "")
        If m_CorSave = "" Then
            m_CorSave = PATH_COR_DEF
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, HOSEI_SAVE, LOCALINI_NAME, m_CorSave)
        End If
        txtCorPath.Text = System.IO.Path.GetFullPath(m_CorSave)

        ' �c�v���W�F�N�V��������Line color�擾
        strCollor = tMod.ReadIniStr(MY_SECTION, COLLOR_RAW, LOCALINI_NAME, "")
        If StringToColor(strCollor, mclr_LineRaw) Then
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�c�v���W�F�N�V���������C���J���[����Ǎ�[{0}]", mclr_LineRaw))
        Else
            strCollor = RED
            StringToColor(strCollor, mclr_LineRaw)
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, COLLOR_RAW, LOCALINI_NAME, strCollor)
        End If

        ' �c�v���W�F�N�V�����ړ����ς�Line color�擾
        strCollor = tMod.ReadIniStr(MY_SECTION, COLLOR_AVE, LOCALINI_NAME, "")
        If StringToColor(strCollor, mclr_LineAve) Then
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�c�v���W�F�N�V�����ړ����σ��C���J���[����Ǎ�[{0}]", mclr_LineAve))
        Else
            strCollor = BLUE
            StringToColor(strCollor, mclr_LineAve)
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, COLLOR_AVE, LOCALINI_NAME, strCollor)
        End If

        ' �␳�P�x����Line color�擾
        strCollor = tMod.ReadIniStr(MY_SECTION, COLLOR_COR, LOCALINI_NAME, "")
        If StringToColor(strCollor, mclr_LineCor) Then
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�␳�P�x�����C���J���[����Ǎ� [{0}]", mclr_LineCor))
        Else
            strCollor = GREEN
            StringToColor(strCollor, mclr_LineCor)
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, COLLOR_COR, LOCALINI_NAME, strCollor)
        End If

        ' �������ʒu��Line color�擾
        strCollor = tMod.ReadIniStr(MY_SECTION, COLLOR_SEL, LOCALINI_NAME, "")
        If StringToColor(strCollor, mclr_LineSel) Then
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�������I������C���J���[����Ǎ� [{0}]", mclr_LineSel))
        Else
            strCollor = PINK
            StringToColor(strCollor, mclr_LineSel)
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, COLLOR_SEL, LOCALINI_NAME, strCollor)
        End If

        ' �O��I�����̈��p���f�[�^���擾(KizuLocal.ini)
        strMvAveSize = tMod.ReadIniStr(MY_SECTION, KEY_MV_AVE, LOCALINI_NAME, "")
        If Not strMvAveSize = "" Then
            If IsNumeric(strMvAveSize) Then
                If Not CInt(strMvAveSize) = 0 Then
                    m_nMvAveSize = CInt(strMvAveSize)
                    txtMoveAveraveSize.Text = CStr(m_nMvAveSize)
                End If
            End If
        End If

    End Sub

    ''' <summary>
    ''' R,G,B�����񂩂�F�ϊ��iR,G,B��1�`255�j
    ''' </summary>
    ''' <param name="str">������(��:255,235,0)</param>
    ''' <returns>True:�ϊ������AFalse:�ϊ����s</returns>
    ''' <remarks></remarks>
    Private Function StringToColor(ByVal str As String, ByRef clr As Color) As Boolean
        Dim bolRtn As Boolean = False                                                       ' ���s����
        Dim nClr(2) As Integer

        Try
            Dim sInt As String() = str.Split(New String() {","}, StringSplitOptions.None)   ' �����񕪉�

            If 3 = sInt.Length Then

                If Not (IsNumeric(sInt(0)) And IsNumeric(sInt(1)) And IsNumeric(sInt(2))) Then
                    Exit Try                                                                ' ���l�łȂ��ꍇ�͔�����
                End If

                For nCnt As Integer = 0 To 2
                    nClr(nCnt) = Integer.Parse(sInt(nCnt))
                    If Not (0 <= nClr(nCnt) And 255 >= nClr(nCnt)) Then                     ' �͈̓`�F�b�N
                        Exit Try
                    End If
                Next

                clr = Color.FromArgb(255, nClr(0), nClr(1), nClr(2))                        ' �F�ϊ� 

                bolRtn = True
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�F�ϊ��ُ� value=({0}) [{1}]", str, ex.Message))
        End Try
        Return bolRtn
    End Function

    ''' <summary>
    ''' ��ʂŐݒ肵��Data��ۑ�����
    ''' �ۑ�����Data:�ړ����σT�C�Y
    ''' </summary>
    ''' <returns>True:�����AFalse:���s</returns>
    ''' <remarks></remarks>
    Private Function SaveData() As Boolean
        Dim bRet As Boolean = True
        Try
            If Not txtMoveAveraveSize.Text = "" Then
                tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, KEY_MV_AVE, LOCALINI_NAME, txtMoveAveraveSize.Text)
            End If
        Catch ex As Exception
            bRet = False
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SaveData�ŗ�O���������܂��� [{0}]", ex.Message))
        End Try
        Return bRet
    End Function

    ''' <summary>
    ''' �v���O���X�o�[���J�n����
    ''' </summary>
    ''' <param name="min">Minimum</param>
    ''' <param name="v">Value</param>
    ''' <param name="pgb">ProgressBar</param>
    ''' <remarks></remarks>
    Private Sub StartProgressBar(ByVal min As Integer, ByVal v As Integer, ByVal pgb As ProgressBar)
        pgb.Minimum = min
        pgb.Value = v
        pgb.Visible = True
        Application.DoEvents()
    End Sub

    ''' <summary>
    ''' �v���O���X�o�[���X�V����
    ''' </summary>
    ''' <param name="v">Value</param>
    ''' <param name="max">Maximum</param>
    ''' <param name="pgb">ProgressBar</param>
    ''' <remarks></remarks>
    Private Sub UpdateProgressBar(ByVal v As Integer, ByVal max As Integer, ByVal pgb As ProgressBar)
        pgb.Value = v
        pgb.Maximum = max
        pgb.Refresh()
        Application.DoEvents()
    End Sub

    ''' <summary>
    ''' �v���O���X�o�[���I������
    ''' ���x���ƃv���O���X�o�[���\���ɂ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub FinishProgressBar(ByVal pgb As ProgressBar)
        pgb.Visible = False
    End Sub

    ''' <summary>
    ''' BitMap�C���[�W��\������
    ''' </summary>
    ''' <param name="iWidth">����</param>
    ''' <param name="iHeight">����</param>
    ''' <param name="img">�摜�f�[�^</param>
    ''' <remarks></remarks>
    Private Sub DisplayImage(ByVal iWidth As Integer, ByVal iHeight As Integer, ByVal img() As Byte)
        Dim tmpImg() As Image = Nothing
        Try
            ' �摜�̕\��
            With mcls_ImgDsp
                tmpImg = .GetImage()
                If tmpImg IsNot Nothing AndAlso tmpImg(0) IsNot Nothing Then
                    tmpImg(0).Dispose()
                    tmpImg(0) = Nothing
                End If
                .tProp_AriaInf_Aria = m_PicBox.ClientSize
                .tProp_Paint_IsPaint = True
                .tProp_Paint_IsDefImgDspn = False
                .tProp_AriaInf_IsCenter = False

                .SetImgInf(iWidth, iHeight, 1, 1)
                .SetImage(iWidth, iHeight, img)
                .Calculation()

                .Refresh()
            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DisplayImage�ŗ�O���������܂��� [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("��ʕ\�����ɗ�O���������܂���"), "��ʕ\��")
            frm.Show()
        End Try
    End Sub

    ''' <summary>
    ''' �X�N���[���o�[��Value Change
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub scrollValueChange(ByVal sender As System.Object, ByVal e As System.Boolean) Handles HScrollBM.ValueChanged, VScrollBM.ValueChanged
        Dim offset_x As Integer = HScrollBM.Value
        Dim offset_y As Integer = VScrollBM.Value

        mcls_ImgDsp.tProp_AriaInf_Offset = New Point(-1 * offset_x, -1 * offset_y)
        mcls_ImgDsp.Refresh()

    End Sub

    ''' <summary>
    ''' PictureBox�̃��T�C�Y�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub PicBoxKidoHosei_Resize(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles pbxBitMap.Resize, pnlBitMap.Resize
        If Not mcls_ImgDsp Is Nothing AndAlso Not m_PicBox Is Nothing Then
            If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                ' �~1���[�h
                HScrollBM.LargeChange = m_nPicWidth
                VScrollBM.LargeChange = m_nPicHeight
                mcls_ImgDsp.tProp_AriaInf_Offset = New Point(0, 0)
            Else
                ' �S�̃��[�h
                pbxBitMap.Height = pbxBitMap.Parent.ClientSize.Height - VScrollBM.Width
                pbxBitMap.Width = pbxBitMap.Parent.ClientSize.Width - HScrollBM.Height
                HScrollBM.LargeChange = pbxBitMap.Size.Width
                VScrollBM.LargeChange = pbxBitMap.Size.Height
            End If

            ' x�������̉E�[�ƃX�N���[���̏I�_�����킹��
            If HScrollBM.Value > HScrollBM.Maximum - HScrollBM.LargeChange Then
                HScrollBM.Value = HScrollBM.Maximum - HScrollBM.LargeChange + 1
            End If

            ' y�������̍ŉ��ӂƃX�N���[���̏I�_�����킹��
            If VScrollBM.Value > VScrollBM.Maximum - VScrollBM.LargeChange Then
                VScrollBM.Value = VScrollBM.Maximum - VScrollBM.LargeChange + 1
            End If

            mcls_ImgDsp.tProp_AriaInf_Aria = m_PicBox.ClientSize
            mcls_ImgDsp.Refresh()
        End If

    End Sub

    ''' <summary>
    ''' �ʒm���x���֕\���A��\�����s��
    ''' </summary>
    ''' <param name="s">�ʒm���e</param>
    ''' <param name="bVisible">true�\��/false��\��</param>
    ''' <param name="lbl">���x��</param>
    ''' <remarks></remarks>
    Private Sub InformLbl(ByVal s As String, ByVal bVisible As Boolean, ByVal lbl As Label)
        lbl.Text = s
        lbl.Visible = bVisible
        lbl.Refresh()
    End Sub

    ''' <summary>
    ''' �\���摜�ۑ��{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnPicSave_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPicSave.Click
        InformLbl("", False, lblInform)

        Dim header(tClass.tClass_BmpJpeg.GetBitmapHeaderSize_Gray) As Byte
        Dim path As String
        Dim filename As String

        If Not m_PicSouce Is Nothing Then
            If radAfter.Checked = True Then
                filename = "�P�x�␳��_" & txtBasePointKido.Text & ".bmp"
                path = IO.Path.Combine(PATH_PIC_DEF, filename)
                tMod.CheckFolder(PATH_PIC_DEF)
                tClass.tClass_BmpJpeg.CreateBitmapHeader_Gray(m_nPicWidth, m_nPicHeight, header, False)
                tClass.tClass_BmpJpeg.BmpFileSave(path, header, m_PicCorrected)
            Else
                filename = "�P�x�␳�O.bmp"
                path = IO.Path.Combine(PATH_PIC_DEF, filename)
                tMod.CheckFolder(PATH_PIC_DEF)
                tClass.tClass_BmpJpeg.CreateBitmapHeader_Gray(m_nPicWidth, m_nPicHeight, header, False)
                tClass.tClass_BmpJpeg.BmpFileSave(path, header, m_PicSouce)
            End If

            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("�o�͐�F[{0}]", System.IO.Path.GetFullPath(path)), "�\���摜�ۑ�����")
            frm.Show()

        End If
    End Sub

    ''' <summary>
    ''' �␳�O�̉摜�\���{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radBefore_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radBefore.CheckedChanged
        InformLbl("", False, lblInform)
        ' �␳�O�̉摜��\������
        If radBefore.Checked = True Then
            radBefore.Enabled = False
            If Not m_PicSouce Is Nothing Then
                DisplayImage(m_nPicWidth, m_nPicHeight, m_PicSouce)
                m_bCorPic = False
            End If
        Else
            radBefore.Enabled = True
        End If
    End Sub

    ''' <summary>
    ''' �␳��̉摜�\���{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radAfter_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radAfter.CheckedChanged
        InformLbl("", False, lblInform)
        ' �␳�����摜��\������
        If radAfter.Checked = True Then
            radAfter.Enabled = False
            If Not m_PicSouce Is Nothing Then
                m_PicCorrected = CreateCorrectedPic(m_PicSouce, m_KidoHosei, m_nPicHeight)
                If Not m_PicCorrected Is Nothing Then
                    DisplayImage(m_nPicWidth, m_nPicHeight, m_PicCorrected)
                    m_bCorPic = True
                End If
            End If
        Else
            radAfter.Enabled = True
        End If
    End Sub

    ''' <summary>
    ''' �␳�l�̉摜�\���{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radCor_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radCor.CheckedChanged
        If radCor.Checked = True Then
            radCor.Enabled = False
            If Not m_PicSouce Is Nothing Then
                m_PicCorrected = CreateCorrectionPic(m_PicSouce, m_KidoHosei)
                If Not m_PicCorrected Is Nothing Then
                    DisplayImage(m_nPicWidth, m_nPicHeight, m_PicCorrected)
                    'm_bCorPic = True
                End If
            End If
        Else
            radCor.Enabled = True
        End If
    End Sub

    ''' <summary>
    ''' DGV���X�V
    ''' </summary>
    ''' <param name="i_R">�c���ϋP�x�z��</param>
    ''' <param name="i_Ave">�c�ړ����ϔz��</param>
    ''' <param name="i_Cor">�␳�l�z��</param>
    ''' <param name="i_BasePt">��_�P�x�l�z��</param>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvUpdate(ByVal i_R As Integer(), ByVal i_Ave As Integer(), ByVal i_Cor As Integer(), ByVal i_BasePt As Integer(), ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        Dim clm As Integer              ' �\�������
        Dim titleW As Integer = 135     ' Title��̕��iBase�j
        Dim dataW As Integer = 27       ' Data��̕��iBase�j
        Try
            clm = ((dgv.Width - titleW) \ dataW) + 1

            ' �X�N���[���o�[�̐ݒ�
            If Not i_R Is Nothing Then
                HScrollDgv.Minimum = 0
                HScrollDgv.Maximum = i_R.Length
                HScrollDgv.LargeChange = clm
            End If

            ' �s�̒ǉ�
            If dgv.RowCount = 0 Then
                With dgv
                    ' �^�C�g���s��ǉ�

                    dgvColSetText(dgv, "", titleW, True, True, _
                    DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    .Columns(0).Frozen = True       ' �^�C�g����̓X�N���[���Œ�
                    .Rows.Add("No.")
                    .Rows.Add("�cProj�i���j")
                    .Rows.Add("�cProj�i�ړ����ρj")
                    .Rows.Add("�␳�l")
                    .Rows.Add("��_�P�x�l")
                    dgvKido.Rows(0).DefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 7.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                    .Item(0, 0).Style.Font = New Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                End With
            End If

            dgvFixColumn(dgv, titleW, dataW, clm)

            ' �f�[�^�I�_���O���b�h�̉E�[�ɂȂ�悤�ɂ���
            If HScrollDgv.Value > HScrollDgv.Maximum - HScrollDgv.LargeChange Then
                HScrollDgv.Value = HScrollDgv.Maximum - HScrollDgv.LargeChange + 1
            End If

            ' Scroll�̈ʒu
            Dim start As Integer = HScrollDgv.Value

            If start >= 0 Then

                ' ���摜�̋P�x��\��
                If m_bRaw = True Then
                    ' No.��\��
                    For ii As Integer = start To start + clm - 2  ' Title�����}�C�i�X

                        dgvKido.Item(ii + 1 - start, 0).Value = ii

                    Next

                    DisplayBrightness(m_nPicWidth, i_R, ROW_KIDO_RAW, start, clm)
                End If
                ' �c�v���W�F�N�V�����i�ړ�����)��\��
                If m_bAve = True Then
                    DisplayBrightness(m_nPicWidth, i_Ave, ROW_KIDO_AVE, start, clm)
                End If
                ' �␳�l
                If m_bCorCalc = True Then
                    DisplayBrightness(m_nPicWidth, i_Cor, ROW_KIDO_COR, start, clm)
                End If
                ' ��_�P�x�l
                If m_bBasePoint = True Then
                    DisplayBrightness(m_nPicWidth, i_BasePt, ROW_KIDO_BP, start, clm)
                End If

                ' ���摜�̋P�x�ƈړ����ύő�l��Cell��ԐF�ɂ���
                ChgCellCollorMAX(m_nKidoMax, ROW_KIDO_RAW, i_R, start, clm)
                ChgCellCollorMAX(m_nKidoMax_A, ROW_KIDO_AVE, i_Ave, start, clm)

            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("dgvUpdate�ŗ�O���������܂���[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �O���b�h�̕\���ɍ��킹�ė�̒ǉ��폜���s��
    ''' </summary>
    ''' <param name="dgv">DataGridView</param>
    ''' <param name="i_TW">Title����Base</param>
    ''' <param name="i_DW">Data����Base</param>
    ''' <param name="i_Col">�\����</param>
    ''' <remarks></remarks>
    Private Sub dgvFixColumn(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal i_TW As Integer, ByVal i_DW As Integer, ByVal i_Col As Integer)
        ' �O���b�h�̕��ɍ��킹�ė�̒ǉ�/�폜
        If dgv.ColumnCount < i_Col Then
            ' DataGridView�̃T�C�Y���摜�T�C�Y���傫���ꍇ�͗��ǉ�����
            For ii As Integer = 1 To i_Col - dgv.ColumnCount
                dgvColSetText(dgv, "", i_DW, True, True, _
                DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
            Next

        ElseIf dgv.ColumnCount > i_Col Then
            ' DataGridView�̃T�C�Y���摜�T�C�Y���������ꍇ�͗���폜����
            For ii As Integer = 1 To dgv.ColumnCount - i_Col
                dgv.Columns.RemoveAt(1)
            Next
        End If

        ' �񕝏�����
        For i As Integer = 1 To i_Col - 1
            dgv.Columns(i).Width = i_DW
        Next

        ' DGV�̕���Fix����܂�Data�����̕������Ԃ�+�P���Ă���
        If dgv.Width - 3 > i_TW + i_DW * (i_Col - 1) Then
            Dim i As Integer = 1    ' ��ʒu
            Dim j As Integer = 0    ' ���������J�E���g
            Do Until dgv.Width - 3 = i_TW + i_DW * (i_Col - 1) + j  ' �O���b�h�̕�����-3�����l�ɗ񕝂�Fix�����̂ł����ɍ��킹��
                dgv.Columns(i).Width = dgv.Columns(i).Width + 1
                If Not i = i_Col - 1 Then
                    i += 1
                Else
                    i = 1
                End If
                j += 1
            Loop
        End If
    End Sub

    ''' <summary>
    ''' DataGridView�ւ̕\��
    ''' </summary>
    ''' <param name="iWidth">����</param>
    ''' <param name="i_x">�f�[�^�z��</param>
    ''' <param name="i_row">�s�ԍ�</param>
    ''' <returns>True:���� False:���s</returns>
    ''' <remarks></remarks>
    Private Function DisplayBrightness(ByVal iWidth As Integer, ByVal i_x() As Integer, ByVal i_row As Integer, ByVal iStart As Integer, ByVal iclm As Integer) As Boolean
        Dim bRet As Boolean = True      '// ���s����
        Try
            For ii As Integer = iStart To iStart + iclm - 2  ' Title�����}�C�i�X
                dgvKido.Item(ii + 1 - iStart, i_row).Value = i_x(ii)
            Next
        Catch ex As Exception
            bRet = False
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DisplayBrightness�ŗ�O���������܂��� [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("�f�[�^�\�����ɗ�O���������܂���"), "�f�[�^�\��")
            frm.Show()
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' �ő�l��Cell��ԐF�ɂ���
    ''' </summary>
    ''' <param name="i_Max">�ő�l</param>
    ''' <param name="i_Row">�s�ԍ�</param>
    ''' <param name="i_x">�z��</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ChgCellCollorMAX(ByVal i_Max As Integer, ByVal i_Row As Integer, ByVal i_x() As Integer, ByVal iStart As Integer, ByVal iclm As Integer) As Boolean
        Try
            If i_Max <> 0 Then
                For ii As Integer = iStart To iStart + iclm - 2  ' Title�����}�C�i�X
                    With dgvKido
                        If i_x(ii) = i_Max Then
                            .Item(ii + 1 - iStart, i_Row).Style.BackColor = Color.Red
                        Else
                            .Item(ii + 1 - iStart, i_Row).Style.BackColor() = .Item(0, i_Row).Style.BackColor()
                        End If
                    End With
                Next
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ChgCellCollorMAX�ŗ�O���������܂���[{0}]", ex.Message))
        End Try

    End Function


    ''' <summary>
    ''' �X�N���[���o�[ValueChanged�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub scrDgvValueChaned(ByVal sender As System.Object, ByVal e As System.Boolean) Handles HScrollDgv.ValueChanged
        dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
    End Sub

    ''' <summary>
    ''' DGV���T�C�Y�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKido_Resize(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles dgvKido.Resize
        If Not dgvKido.Width = 0 Then
            dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
        End If
    End Sub

    ''' <summary>
    ''' DGV�ł̃L�[�_�E���C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKido_KeyDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles dgvKido.KeyDown
        Dim x As Integer = dgvKido.CurrentCell.ColumnIndex
        Dim y As Integer = dgvKido.CurrentCell.ColumnIndex

        Select Case e.KeyCode
            Case Keys.Left
                ' �f�[�^���[�̗�ō��J�[�\�����삵���ꍇ��-1�X�N���[������
                If dgvKido.CurrentCell.ColumnIndex = 1 Then
                    If Not dgvKido.Item(1, 0).Value Is Nothing Then
                        If Not CInt(dgvKido.Item(1, 0).Value) = 0 Then
                            e.Handled = True
                            HScrollDgv.Value = HScrollDgv.Value - 1
                            dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
                        End If
                    End If

                End If
            Case Keys.Right
                ' �f�[�^���[�̗�ŉE�J�[�\�����삵���ꍇ�́{1�X�N���[������
                If dgvKido.CurrentCell.ColumnIndex = dgvKido.Columns.Count - 1 Then
                    If Not dgvKido.Item(dgvKido.Columns.Count - 1, 0).Value Is Nothing Then
                        If Not dgvKido.Item(dgvKido.Columns.Count - 1, 0).Value.ToString = CStr(m_nPicWidth - 1) Then
                            e.Handled = True
                            HScrollDgv.Value = HScrollDgv.Value + 1
                            dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
                        End If
                    End If

                End If
        End Select

    End Sub

    ''' <summary>
    ''' DGV�ł̃}�E�X�N���b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKido_CellClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKido.CellClick
        Dim x As Integer = dgvKido.CurrentCell.ColumnIndex

        If x > 0 Then
            m_nSelectX = CInt(dgvKido.Item(x, 0).Value)
            If mcls_ImgDsp.tProp_ImgKind_StrechType <> tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                ' �S�̃��[�h
                Dim nSel As Integer = m_nSelectX - CInt(pbxBitMap.Size.Width / 2)
                If nSel < 0 Then
                    nSel = 0
                End If
                HScrollBM.Value = nSel
            End If

            mcls_ImgDsp.Refresh()
        End If

    End Sub

    ''' <summary>
    ''' �cProjection���̍ő�l�ֈړ�����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRawMax_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRawMax.Click
        Dim start As Integer    ' DGV�̊J�n�ʒu
        Dim max As Integer      ' �P�x�ő�l
        max = GetArrayMax(m_KidoHAve)
        start = GetMaxValueClm(max, m_KidoHAve)
        HScrollDgv.Value = start

        dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
    End Sub

    ''' <summary>
    ''' �cProjection�ړ����ς̍ő�l�ֈړ�����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAveMax_Click(ByVal sender As System.Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles btnAveMax.MouseClick
        Dim start As Integer    ' DGV�̊J�n�ʒu
        Dim max As Integer      ' �P�x�ő�l
        max = GetArrayMax(m_KidoMvAve)
        start = GetMaxValueClm(max, m_KidoMvAve)
        HScrollDgv.Value = start
        dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)

    End Sub

    ''' <summary>
    ''' �ő�l�̔z��Index��Ԃ�
    ''' �ő�l����������ꍇ�͐擪�̍ő�l�̔z��Index�ƂȂ�܂�
    ''' </summary>
    ''' <param name="i_max">�P�x�ő�l</param>
    ''' <param name="i_x">�P�x�z��</param>
    ''' <returns>�P�x�z��Index</returns>
    ''' <remarks></remarks>
    Private Function GetMaxValueClm(ByVal i_max As Integer, ByVal i_x() As Integer) As Integer
        Dim i_clm As Integer = 0    ' �P�x�ő�l��Index

        For i As Integer = 0 To i_x.Length - 1
            If i_x(i) = i_max Then
                i_clm = i
                Exit For
            End If
        Next

        Return i_clm

    End Function

    ''' <summary>
    ''' �␳�l�Ǎ��{�^���N���b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnHoseitiRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHoseitiRead.Click
        InformLbl("", False, lblInform)
        Dim nHoseiti() As Integer   '�␳�l
        Dim ofd As New OpenFileDialog           '�I�[�v���_�C�A���O

        ofd.InitialDirectory = m_CorSave        '�␳�l�ۑ����Initial directory�ɂ���
        ofd.Filter = "csv files (*.csv)|*.csv|All files (*.*)|*.*"
        ofd.FilterIndex = 1
        ofd.RestoreDirectory = True

        If m_nPicWidth = 0 Then Exit Sub ' �����T�C�Y��0�͏����ł��Ȃ��̂ŉ������Ȃ�
        If ofd.ShowDialog() = DialogResult.OK Then
            Dim strPath As String = ofd.FileName
            nHoseiti = ReadHoseiti(m_nPicWidth, strPath)  'CSV�t�@�C������ǂݍ��񂾕␳�l���擾����
            If nHoseiti Is Nothing Then Exit Sub '�␳�l�擾�Ɏ��s���Ă���̂ŏI��
            m_KidoHosei = nHoseiti      '�����o�X�V

            ' ��_�P�x�l�̌v�Z
            If Not m_KidoHosei Is Nothing AndAlso Not m_KidoMvAve Is Nothing Then
                m_KidoBasePoint = GetKidoBasePoint(m_nPicWidth, m_KidoHosei, m_KidoMvAve)
                m_bBasePoint = True
            End If

            ' �␳�����摜��\������
            If Not m_PicSouce Is Nothing Then
                m_PicCorrected = CreateCorrectedPic(m_PicSouce, m_KidoHosei, m_nPicHeight)      '�␳�摜�擾
                If Not m_PicCorrected Is Nothing Then
                    DisplayImage(m_nPicWidth, m_nPicHeight, m_PicCorrected) '�摜�\��
                    dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
                    m_bCorPic = True '�␳�摜�\��Flag�X�V
                    ChgRadBtn(RAD_BEFORE)       '�{�^���͕␳�O�摜�ɖ߂���悤�ɂ��Ƃ�
                    InformLbl("�␳�l�Ǎ��������������܂���", True, lblInform)       '�摜�\�������̒ʒm
                End If
            End If
        End If

        ofd.Dispose()

    End Sub

    ''' <summary>
    ''' �␳�l�t�@�C���iCSV�j�̓Ǎ�
    ''' </summary>
    ''' <param name="nWidth">�摜����</param>
    ''' <returns>�␳�l�z��</returns>
    ''' <remarks></remarks>
    Private Function ReadHoseiti(ByVal nWidth As Integer, ByVal strPath As String) As Integer()
        Dim nHoseiti(nWidth - 1) As Integer     '�␳�l�z��

        Using cls_Stram As New tClass.tClass_FileStream(strPath)
            Try
                '�I�[�v��
                cls_Stram.Open()
                Dim aryHoseiti As New ArrayList
                aryHoseiti = cls_Stram.ReadAll      '�t�@�C���̑S�Ǎ��f�[�^���擾

                '�Ǎ��f�[�^�̃T�C�Y�`�F�b�N�B���摜�̉����ƍ����Ă��Ȃ��ƕs���Ƃ݂Ȃ��B�Z�N�V������1�s���l���B
                If aryHoseiti.Count <> m_nPicWidth + 1 Then
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�Ǎ��f�[�^�Ɖ摜�f�[�^���T�C�Y�ƈ�v���Ȃ��B�摜�f�[�^���F[{0}] �Ǎ��f�[�^�F[{1}]", m_nPicWidth, aryHoseiti.Count - 1))
                    Dim frm As New tClassForm.frmPop_MsgBox(String.Format("�Ǎ��f�[�^�Ɖ摜�f�[�^���T�C�Y����v���܂���ł����B" & vbCrLf & " �摜�f�[�^���F[{0}]" & vbCrLf & " �Ǎ��f�[�^�F[{1}]", m_nPicWidth, aryHoseiti.Count - 1), "�␳�l�Ǎ�")
                    frm.Show()
                    cls_Stram.Close()
                    cls_Stram.Dispose()
                    Return Nothing
                End If

                '�Ǎ��f�[�^�̕␳�l�f�[�^�����o��
                For ii As Integer = 0 To aryHoseiti.Count - 2  '�擪�s�̓Z�N�V�������Ȃ̂ŏ��O
                    nHoseiti(ii) = CInt(aryHoseiti(ii + 1))
                Next

                cls_Stram.Close()
                cls_Stram.Dispose()

            Catch ex As Exception
                nHoseiti = Nothing
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�␳�l�ǂݎ�蒆�ɗ�O���������܂��� [{0}]", ex.Message))
                Dim frm As New tClassForm.frmPop_MsgBox(String.Format("�␳�l�Ǎ����ɗ�O���������܂����B�Ǎ��t�@�C�����J���Ă���ꍇ�̓t�@�C������Ă���ēx���s���Ă��������B"), "�␳�l�Ǎ�")
                frm.Show()
            End Try
        End Using

        Return nHoseiti

    End Function

End Class