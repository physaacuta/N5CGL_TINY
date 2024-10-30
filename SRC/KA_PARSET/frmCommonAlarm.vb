'*********************************************************************************
' �x��o�͐ݒ��ʃN���X
'	[Ver]
'		Ver.01    2007/08/16  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmCommonAlarm
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[
    Private m_DS As New DataSet                                     ' ��ڑ��^�e�[�u��
    
    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmAlarm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�x��o�͐ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARCOLOR�֐ؑ֏���
            Case 10
                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' �ݒ�
            Case 11
                modMain.TaskEnd()               ' F12 �I��
        End Select

    End Sub

    ''' <summary>
    ''' ��ʕύX�v�����ꂽ�Ƃ��̏��� (�p�����N���X���R�[�������)
    ''' </summary>
    ''' <param name="frmID">�I�����ID</param>
    ''' <param name="cancel">�I��������߂����ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmAlarm_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If

        ' ��ʐؑ�
        ShowForm(frmID)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmAlarm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmCommonAlarm           ' �x��o�͐ݒ��ʌďo

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' �e�L�X�g�{�b�N�X�̐ݒ�
        txtInitialize()

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        ' ��ʂɒl��\��        
        If Not Data_Disp() Then
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        ' �X�V�{�^���̗L��/����
        Call UpdateEnabled(g_bPassFlg)

        '�v�����^���Ȃ���΁A��ʈ������
        If Not tClass_HeadCopy.IsCheckPrinter Then
            btnF07.Enabled = False
        Else
            btnF07.Enabled = True
        End If

        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub

    ''' <summary>
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCommonAlarm_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub


    ''' <summary>
    ''' �e�L�X�g�{�b�N�X�Ȃǂ̏�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub txtInitialize()
        ' �e�L�X�g�{�b�N�X�̏����ݒ�
        ' �n�ړ_�s���ѐ�[
        txtYousetuSen.ImeMode = Windows.Forms.ImeMode.Disable   ' IME���[�h�g�p�s��
        txtYousetuSen.Text = ""                                 ' ������

        ' �n�ړ_�s���є��[
        txtYousetuBi.ImeMode = Windows.Forms.ImeMode.Disable    ' IME���[�h�g�p�s��
        txtYousetuBi.Text = ""                                  ' ������

        ' �r�ڋߏo�͋�ԕ\�O
        txtKizuTopMae.ImeMode = Windows.Forms.ImeMode.Disable   ' IME���[�h�g�p�s��
        txtKizuTopMae.Text = ""                                 ' ������

        ' �r�ڋߏo�͋�ԕ\��
        txtKizuTopGo.ImeMode = Windows.Forms.ImeMode.Disable    ' IME���[�h�g�p�s��
        txtKizuTopGo.Text = ""                                  ' ������

        ' �{�C�X�o�͈ʒu�\
        txtVoiceTop.ImeMode = Windows.Forms.ImeMode.Disable     ' IME���[�h�g�p�s��
        txtVoiceTop.Text = ""                                   ' ������
    End Sub

    ''' <summary>
    ''' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL������
        strSQL = "SELECT LINEID "
        strSQL &= ",�r�ڋ߃����v_�s����_��["
        strSQL &= ",�r�ڋ߃����v_�s����_���["
        strSQL &= ",�r�ڋ߃����v_�o�͋��_�O"
        strSQL &= ",�r�ڋ߃����v_�o�͋��_��"
        strSQL &= ",�r�ڋ߃A���[��_�s����_��["
        strSQL &= ",�r�ڋ߃A���[��_�s����_���["
        strSQL &= ",�r�ڋ߃A���[��_�o�͋��_�O"
        strSQL &= ",�r�ڋ߃A���[��_�o�͋��_��"
        strSQL &= ",�{�C�X_�o�͈ʒu_�O"

        strSQL &= " FROM " & DB_COMMON_MASTER

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_COMMON_MASTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   '�g�����U�N�V��������

        Try
            ' DB�ڑ�
            m_Adapter.SelectCommand.Connection.Open()

            '�g�����U�N�V�����J�n
            transact = m_Adapter.SelectCommand.Connection.BeginTransaction()
            m_Adapter.SelectCommand.Transaction = transact

            Using cbWk As New SqlClient.SqlCommandBuilder()

                '�R�}���h�A�_�v�^���Z�b�g���āA�X�V
                cbWk.DataAdapter = m_Adapter
                m_Adapter.Update(m_DS)

            End Using

            '�R�~�b�g
            transact.Commit()

            '����ɓo�^���ꂽ�ꍇ�̓��[�����M
            modMain.Send_Main(CH_PARAM_SETTIG_ALARM)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)

            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            If Not transact Is Nothing Then
                transact.Rollback()
            End If

            Return False
        Finally
            ' ��n��
            If m_Adapter.SelectCommand.Connection.State = ConnectionState.Open Then
                m_Adapter.SelectCommand.Connection.Close()
            End If

            If Not transact Is Nothing Then
                transact.Dispose()
            End If
        End Try
    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean
        Dim drow() As DataRow     ' �f�[�^�s

        ' ��L�[���w�肵�Ď��o���Ă�̂ŁA1�������Ȃ��͂�
        drow = m_DS.Tables(0).Select()

        ' �擾�s��1�s�łȂ��Ȃ�G���[�I��
        If drow.Length <> 1 Then Return False

        ' ��ʂɕ\��
        txtYousetuSen.Text = CStr(drow(0).Item("�r�ڋ߃����v_�s����_��["))
        txtYousetuBi.Text = CStr(drow(0).Item("�r�ڋ߃����v_�s����_���["))
        txtKizuTopMae.Text = CStr(drow(0).Item("�r�ڋ߃����v_�o�͋��_�O"))
        txtKizuTopGo.Text = CStr(drow(0).Item("�r�ڋ߃����v_�o�͋��_��"))
        txtAlarmSen.Text = CStr(drow(0).Item("�r�ڋ߃A���[��_�s����_��["))
        txtAlarmBi.Text = CStr(drow(0).Item("�r�ڋ߃A���[��_�s����_���["))
        txtKizuBotMae.Text = CStr(drow(0).Item("�r�ڋ߃A���[��_�o�͋��_�O"))
        txtKizuBotGo.Text = CStr(drow(0).Item("�r�ڋ߃A���[��_�o�͋��_��"))
        txtVoiceTop.Text = CStr(drow(0).Item("�{�C�X_�o�͈ʒu_�O"))

        Return True
    End Function

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String     ' ��Ɨp
        ' �r�ڋ߃����v �n�ړ_�s���ѐ�[
        strWk = txtYousetuSen.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "�r�ڋ߃����v �n�ړ_�O��̕s���� ��[") <> 0 Then Return False
        txtYousetuSen.Text = strWk

        ' �r�ڋ߃����v �n�ړ_�s���є��[
        strWk = txtYousetuBi.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "�r�ڋ߃����v �n�ړ_�O��̕s���� ���[") <> 0 Then Return False
        txtYousetuBi.Text = strWk

        ' �r�ڋ߃A���[�� �n�ړ_�s���ѐ�[
        strWk = txtAlarmSen.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "�r�ڋ߃A���[�� �n�ړ_�O��̕s���� ��[") <> 0 Then Return False
        txtAlarmSen.Text = strWk

        ' �r�ڋ߃A���[�� �n�ړ_�s���є��[
        strWk = txtAlarmBi.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "�r�ڋ߃A���[�� �n�ړ_�O��̕s���� ���[") <> 0 Then Return False
        txtAlarmBi.Text = strWk

        ' �r�ڋߏo�͋�ԕ\�O
        strWk = txtKizuTopMae.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "�r�ڋߏo�͋�� �\������ ��O") <> 0 Then Return False
        txtKizuTopMae.Text = strWk

        ' �r�ڋߏo�͋�ԕ\��
        strWk = txtKizuTopGo.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "�r�ڋߏo�͋�� �\������ �ʉߌ�") <> 0 Then Return False
        txtKizuTopGo.Text = strWk

        ' �r�ڋ߃A���[���o�͋�ԗ��O
        strWk = txtKizuBotMae.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "�r�ڋߏo�͋�� �������� ��O") <> 0 Then Return False
        txtKizuBotMae.Text = strWk

        ' �r�ڋ߃A���[���o�͋�ԗ���
        strWk = txtKizuBotGo.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "�r�ڋߏo�͋�� �������� �ʉߌ�") <> 0 Then Return False
        txtKizuBotGo.Text = strWk

        ' �{�C�X�o�͈ʒu�\
        strWk = txtVoiceTop.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "�{�C�X�o�͈ʒu �\������") <> 0 Then Return False
        txtVoiceTop.Text = strWk

        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim drow As DataRow     ' �f�[�^�s

        ' ��L�[���w�肵�Ď��o����Ă�̂ňꌏ�����Ȃ�
        drow = m_DS.Tables(0).Select()(0)

        ' �Z�b�g
        drow.Item("�r�ڋ߃����v_�s����_��[") = CInt(txtYousetuSen.Text)
        drow.Item("�r�ڋ߃����v_�s����_���[") = CInt(txtYousetuBi.Text)
        drow.Item("�r�ڋ߃����v_�o�͋��_�O") = CInt(txtKizuTopMae.Text)
        drow.Item("�r�ڋ߃����v_�o�͋��_��") = CInt(txtKizuTopGo.Text)
        drow.Item("�r�ڋ߃A���[��_�s����_��[") = CInt(txtAlarmSen.Text)
        drow.Item("�r�ڋ߃A���[��_�s����_���[") = CInt(txtAlarmBi.Text)
        drow.Item("�r�ڋ߃A���[��_�o�͋��_�O") = CInt(txtKizuBotMae.Text)
        drow.Item("�r�ڋ߃A���[��_�o�͋��_��") = CInt(txtKizuBotGo.Text)
        drow.Item("�{�C�X_�o�͈ʒu_�O") = CInt(txtVoiceTop.Text)
    End Sub
    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()
        ' ���͒l�`�F�b�N
        If Not Data_Check() Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '��ʂ̃f�[�^���擾
            Data_Get()

            ' �z���g�ɓo�^        
            If DB_UpDate() Then
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            ' ���݂̓��e�ōĕ\��
            Data_Disp()

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try
    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub
End Class
