
Imports tClassLibrary

Public Class cntItem

    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ API
    Private Declare Function MoveWindow Lib "user32" Alias "MoveWindow" _
     (ByVal hwnd As IntPtr, ByVal x As Integer, ByVal y As Integer, _
      ByVal nWidth As Integer, ByVal nHeight As Integer, _
      ByVal nRepaint As Integer) As Integer



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ �萔
    Private m_nNo As Integer = -1										' No (0�I���W��)
    Private m_sMailName As String = ""									' �Y��RpiViewer�̃��[���X���b�g��

    Private m_pRpi As System.Diagnostics.Process						' RpiViewer �v���Z�X

    Dim mcls_Mail As New tClassLibrary.tClass.tClass_MailSlot_Base		' ���[���X���b�g(���M��p)

    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ �����ݒ�

    ''' <summary>
    ''' No
    ''' </summary>
    Public Property Prop_No() As Integer
        Get
            Return m_nNo
        End Get
        Set(ByVal value As Integer)
            m_nNo = value

            ' �֘A���ڐݒ�
            lblNo.Text = CStr(m_nNo + 1)
            m_sMailName = String.Format("{0}{1}", RPIV_NAME, m_nNo)
        End Set
    End Property

    ''' <summary>
    ''' �L�ї�
    ''' </summary>
    Public Property Prop_Rate() As Double
        Get
            Return nudRateY.Value
        End Get
        Set(ByVal value As Double)
            If nudRateY.Minimum > value Then value = nudRateY.Minimum
            If nudRateY.Maximum < value Then value = nudRateY.Maximum

            nudRateY.Value = CDec(value)
        End Set
    End Property

    ''' <summary>
    ''' �I�t�Z�b�g
    ''' </summary>
    ''' <param name="nIndex">0:X  1:Y</param>
    Public Property Prop_Offset(ByVal nIndex As Integer) As Integer
        Get
            Dim nudVal As NumericUpDown = CType(IIf(0 = nIndex, nudOffsetX, nudOffsetY), NumericUpDown)

            Return CInt(nudVal.Value)

        End Get
        Set(ByVal value As Integer)
            Dim nudVal As NumericUpDown = CType(IIf(0 = nIndex, nudOffsetX, nudOffsetY), NumericUpDown)
            nudVal.Value = CDec(value)
        End Set
    End Property



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ RPI �����\���֌W

    ''' <summary>
    ''' �I����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub cntItem_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Call Mail_RpiEnd()
    End Sub

    ''' <summary>
    ''' RPI�t�@�C�����h���b�O�Ŏw�肷�� (�h���b�O)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_DragEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles Me.DragEnter
        ' �h���b�O�ŗL���ɂ��Ă����Ȃ��ƃh���b�v�C�x���g������Ȃ�
        ' �t�H�[���v���p�e�B�� AllowDrop ��L���ɂ��Ă����Ȃ��ƃ_��
        If e.Data.GetDataPresent(DataFormats.FileDrop) Then
            e.Effect = DragDropEffects.Copy
        Else
            e.Effect = DragDropEffects.None
        End If
    End Sub

    ''' <summary>
    ''' RPI�t�@�C�����h���b�O�Ŏw�肷�� (�h���b�v)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_DragDrop(ByVal sender As Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles Me.DragDrop
        If Not e.Data.GetDataPresent(DataFormats.FileDrop) Then Return

        '' �h���b�O���ꂽ�f�[�^�`���𒲂ׁA�t�@�C���̂Ƃ��̓R�s�[�Ƃ���
        Dim fileName As String() = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
        Dim strFPath As String = fileName(0) ' �����w�莞���l�����āA�ŏ��̂P�R���g��


        '===============================================
        ' ���O����
        txtRpiPath.Text = strFPath

        ' RPI�t�@�C�����s
        'Call SelectRpiFile(strFPath, True)
    End Sub

    ''' <summary>
    ''' ���s
    ''' </summary>
    Private Sub btnExec_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExec.Click
        '===============================================
        ' RPI Viewer���N��
        Call ExecRpiV()

        '===============================================
        ' RPI Viewer �ɏ��ʒm
        If 0 < txtRpiPath.Text.Length Then
            Call Mail_RpiDsping(txtRpiPath.Text)
        End If
    End Sub

    ''' <summary>
    ''' RPI Viewer�N��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ExecRpiV()
        '===============================================
        ' RPI Viewer���N�����Ă��邩�`�F�b�N
        Dim bIsRun As Boolean = CheckRpiRun()								  ' RPIViewer�N���� true


        '===============================================
        ' RPI Viewer���N��
        If Not bIsRun Then
            Dim sCmd As String		' �N���R�}���h
            sCmd = String.Format("{0}{1} {2}{3}", New Object() {START_CMD_TASKNAME, m_sMailName, START_CMD_ID, m_nNo})
            Try
                m_pRpi = Process.Start("RPI_VIEWER.exe", sCmd)
                'm_pRpi= Process.Start("C:\��13_�J���Č�\RPI�A��\RPI_VIEWER Ver440\bin\Release\RPI_VIEWER.exe", sCmd)
                m_pRpi.WaitForInputIdle(1500)	' �N������܂ő҂�
                Threading.Thread.Sleep(1500)	' ���[����M�\�ɂȂ�܂ő҂�

            Catch ex As Exception
            End Try
        End If
    End Sub




    ''' <summary>
    ''' ��ʃT�C�Y�A�ʒu�ύX
    ''' </summary>
    ''' <param name="s">�T�C�Y</param>
    ''' <param name="p">�ʒu</param>
    ''' <remarks></remarks>
    Public Sub DspMove(ByVal s As Size, ByVal p As Point)

        MoveWindow(m_pRpi.MainWindowHandle, p.X, p.Y, s.Width, s.Height, 1)


    End Sub


    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ ���[���X���b�g�֌W

    ''' <summary>
    ''' �O�����痈���ʒm�����̂܂ܓ]��
    ''' </summary>
    ''' <param name="que">���̂���</param>
    ''' <remarks></remarks>
    Public Sub Mail_Send_Chain(ByRef que As COMMON_QUE_STRMIX)
        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub

    ''' <summary>
    ''' RPI Viewer���N�����Ă��邩�`�F�b�N
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function CheckRpiRun() As Boolean
        Using clsMail As New tClassLibrary.tClass.tClass_MailSlot_Base	' �d���N���m�F�p���[���X���b�g
            If clsMail.Mail_Open(m_sMailName) Then
                clsMail.Mail_Close()
                Return False
            Else
                Return True
            End If
        End Using
    End Function

    ''' <summary>
    ''' RPI�t�@�C���w��
    ''' </summary>
    ''' <param name="sFPath"></param>
    ''' <remarks></remarks>
    Private Sub Mail_RpiDsping(ByVal sFPath As String)
        Dim que As COMMON_QUE_STRMIX = Nothing
        que.inisialize()
        que.nEventNo = FACT_RPIV_DSPING

        que.cdata1 = IO.Path.GetDirectoryName(sFPath) & "\"	' �p�X
        Dim sName As String = IO.Path.GetFileName(sFPath)		' �t�@�C����
        Dim nCnt As Integer = tClassLibrary.tMod.LenB(sName)
        If 32 > nCnt Then
            que.cdata2 = sName.Substring(0, nCnt)
        Else
            que.cdata2 = sName.Substring(0, 31)
            que.cdata3 = sName.Substring(31, nCnt - 31)
        End If

        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub

    ''' <summary>
    ''' �I���ʒm
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Mail_RpiEnd()
        Dim que As COMMON_QUE_STRMIX = Nothing
        que.inisialize()
        que.nEventNo = FACT_RPIV_END

        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub

    ''' <summary>
    ''' ���E���]
    ''' </summary>
    Private Sub ckbTurn_Left_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ckbTurn_Left.CheckedChanged
        Dim que As COMMON_QUE_STRMIX = Nothing
        que.inisialize()
        que.nEventNo = FACT_RPIV_DSP_TURN

        que.idata(0) = CInt(IIf(CType(sender, CheckBox).Checked, 1, -1))

        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub

    ''' <summary>
    ''' �㉺���]
    ''' </summary>
    Private Sub ckbTurn_Top_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ckbTurn_Top.CheckedChanged
        Dim que As COMMON_QUE_STRMIX = Nothing
        que.inisialize()
        que.nEventNo = FACT_RPIV_DSP_TURN

        que.idata(1) = CInt(IIf(CType(sender, CheckBox).Checked, 1, -1))

        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub


    ''' <summary>
    ''' �O������ݒ蔽�f
    ''' </summary>
    Public Sub OutParamExec(ByRef que As COMMON_QUE_STRMIX)
        Dim nMode As Integer = que.idata(0)

        ' �ŉ���1�o�C�g
        If 0 <> (nMode And &H1) Then		' �L�ї�
            Prop_Rate = que.idata(1) / 1000000.0
        End If
        If 0 <> (nMode And &H2) Then		' �I�t�Z�b�gY
            Prop_Offset(1) = que.idata(2)
        End If
        If 0 <> (nMode And &H4) Then		' �I�t�Z�b�gX
            Prop_Offset(0) = que.idata(3)
        End If

        If 0 <> (nMode And &H8) Then
        End If

        If 0 <> (nMode And &H10) Then
        End If
        If 0 <> (nMode And &H20) Then
        End If
        If 0 <> (nMode And &H40) Then
        End If
        If 0 <> (nMode And &H80) Then
        End If

    End Sub

    ''' <summary>
    ''' �O������q�o�h�r���[�A�N���[�Y�v��
    ''' </summary>
    Public Sub OutMail_RpiEnd()
        Mail_RpiEnd()
    End Sub

    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ TRK����

    ''' <summary>
    ''' ���ݏ��
    ''' </summary>
    ''' <param name="typItem"></param>
    ''' <remarks></remarks>
    Public Sub SetNowData(ByVal typItem As TYPE_TRK_ITEM)

        txtValY.Text = String.Format("{0:#,##0}", typItem.nY)
        txtValX.Text = String.Format("{0:#,##0}", typItem.nX)

    End Sub

    ''' <summary>
    ''' ��I�����
    ''' </summary>
    ''' <param name="bMode">true:�I��</param>
    ''' <remarks></remarks>
    Public Sub SetActive(ByVal bMode As Boolean)
        If bMode Then
            lblNo.BackColor = Color.Orange
        Else
            lblNo.BackColor = SystemColors.Control
        End If
    End Sub




End Class
