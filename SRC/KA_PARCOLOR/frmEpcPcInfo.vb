'*********************************************************************************
' ��ʏ����e�ݒ�
'	[Ver]
'		Ver.01    2012/10/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcPcInfo
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////

    ' ��ʏ����e�ݒ�
    Private Enum EM_PCINFO_GRID_ID
        emTxt_Order = 0                 ' �e�L�X�g�{�b�N�X  No
        emTxt_Name                      ' �e�L�X�g�{�b�N�X  ����
        emTxt_Size                      ' �e�L�X�g�{�b�N�X  �o�C�g�T�C�Y
        emTxt_EdasysID                  ' �e�L�X�g�{�b�N�X  EdasysID
        emCmb_IsAscii                   ' �R���{�{�b�N�X    Ascii or Binary
        emCmb_IsNum                     ' �R���{�{�b�N�X    Ascii���l�ϊ�
        emTxt_ConvCoef                  ' �e�L�X�g�{�b�N�X  ���Z�W��
        emTxt_CoilInfColName            ' �e�L�X�g�{�b�N�X  �R�C�����e�[�u������
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterPcInfo As SqlClient.SqlDataAdapter = Nothing   '�A�_�v�^�[ (PcInfo_Desk)
    Private m_DsPcInfo As New DataSet                               '��ڑ��^�e�[�u�� (PcInfo_Desk)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�@���N�V�����{�^������������
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmEpcPcInfo_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "��ʏ����e�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARSET�֐ؑ֏���
            Case 10
                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' F11 �ݒ�
            Case 11
                modMain.TaskEnd()               ' F12 �I��
        End Select
    End Sub

    ''' <summary>
    ''' ��ʕύX�v�����ꂽ�Ƃ��̏��� (�p�����N���X���R�[�������)
    ''' </summary>
    ''' <param name="frmID">�I�����ID</param>
    ''' <param name="cancel">�I��������߂�ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmEpcPcInfo_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        '' ���낢��J������
        'PcInfo�֌W
        m_DsPcInfo.Dispose()
        If Not m_AdapterPcInfo Is Nothing Then
            m_AdapterPcInfo.Dispose()
            m_AdapterPcInfo = Nothing
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
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcPcInfo_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                 ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                             ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcPcInfo         ' ��ʏ����e�ݒ�Ăяo��

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        '�O���b�h�̏����ݒ� ------------->>>>>
        ' �W�v�ΏۃO���[�v�ݒ�
        dgvInitializePcInfo(dgvPcInfo)

        '' �f�[�^�̎擾
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        '' ��ʂɒl��\��
        If Not Data_Disp() Then
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        ''�I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        dgvPcInfo.DataGridViewSelectionRowColorChange(g_ColorSelection)

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
    Private Sub frmGapSetting_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h��`
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �f�[�^�̎擾
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ''PcInfo�f�[�^�擾
        strSQL = "SELECT [Order]"
        strSQL &= ", Name, Size, EdasysID, IsAscii"
        strSQL &= ", IsNum, ConvCoef, CoilInfColName"
        strSQL &= " FROM " & DB_PARAM_PCINFO_DESC
        strSQL &= " ORDER BY [Order] ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPcInfo, m_AdapterPcInfo, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_PCINFO_DESC & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean

        'PcInfo�f�[�^�̕\��
        If Not Data_DispPcInfo() Then Return False

        Return True
    End Function

    ''' <summary>
    ''' ��ʏ����e(PcInfo)�f�[�^�̕\��
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_DispPcInfo() As Boolean
        Dim drowPcInfo() As DataRow = m_DsPcInfo.Tables(0).Select()         ' �S���R�[�h��\�����邽��WHERE��Ȃ�

        ' ��ʂɕ\��
        Try
            ' �J�����Z�b�g��
            dgvPcInfo.RowCount = drowPcInfo.Length + 1                      ' ��{�I�Ƀ}�X�^�[�ɂ͐���ȃ��R�[�h��o�^�ς݂̂͂������A�ėp�I�ɂ����

            For ii As Integer = 0 To drowPcInfo.Length - 1

                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Order, ii).Value = drowPcInfo(ii).Item("Order")                       ' No
                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Name, ii).Value = drowPcInfo(ii).Item("Name")                         ' ����
                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Size, ii).Value = drowPcInfo(ii).Item("Size")                         ' �o�C�g�T�C�Y
                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value = drowPcInfo(ii).Item("EdasysID")                 ' EdasysID

                ' Ascii or Binary
                If CBool(drowPcInfo(ii).Item("IsAscii")) Then
                    dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsAscii, ii).Value = "Ascii"
                Else
                    dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsAscii, ii).Value = "Binary"
                End If

                ' Ascii���l�ϊ�
                If CBool(drowPcInfo(ii).Item("IsNum")) Then
                    dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsNum, ii).Value = "True"
                Else
                    dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsNum, ii).Value = "False"
                End If

                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_ConvCoef, ii).Value = drowPcInfo(ii).Item("ConvCoef")                 ' �o�C�g�T�C�Y
                dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value = drowPcInfo(ii).Item("CoilInfColName")     ' EdasysID

            Next ii

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_PCINFO_DESC & " �\���G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        End Try

    End Function

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' ���͒l�`�F�b�N
        If Not Data_Check() Then Exit Sub

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' �f�[�^�擾
            Data_Get()

            ' �z���g�ɓo�^        
            If DB_UpDate() Then
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

                Data_Disp()
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean

        'PcInfo�f�[�^���̓`�F�b�N
        If Not Data_CheckPcInfo() Then Return False

        Return True
    End Function

    ''' <summary>
    ''' ��ʏ����e(PcInfo)�f�[�^���̓`�F�b�N
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_CheckPcInfo() As Boolean

        Dim strWk As String = ""
        Dim strMsg As String    ' �d���p�̃G���[���b�Z�[�W

        For ii As Integer = 0 To dgvPcInfo.Rows.Count - 2

            ' ����
            strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Name, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "����", True) <> 0 Then Return False
            dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Name, ii).Value = strWk

            ' �o�C�g�T�C�Y
            strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Size, ii).Value)
            If 0 <> tClassLibrary.tClass.tClass_ImputCheck.CheckImputData_Int(strWk, 0, Int32.MaxValue, False) Then
                strMsg = "�y �o�C�g�T�C�Y �z" & vbNewLine
                strMsg &= "�K�؂Ȓl�ł͂���܂���B" & vbNewLine
                strMsg &= "���͔͈́F�u0�ȏ�v"
                MsgBox(strMsg, MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                Return False

            End If

            ' EdasysID
            strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99999, False, "EdasysID", True) <> 0 Then Return False
			dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value = strWk
			
			' EdasysID��0�̏ꍇ�͏d���`�F�b�N���s��Ȃ�
			If 0 <> CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value) Then
				' EdasysID�̏d���`�F�b�N
				For jj As Integer = ii + 1 To dgvPcInfo.Rows.Count - 1
					' ��r���EdasysID�����l�Ȃ��r
					If IsNumeric(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, jj).Value) Then
						' EdasysID�������Ȃ烁�b�Z�[�W���o���ďI��
						If CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value) = CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, jj).Value) Then
							strMsg = "�y ��ʏ����e�ݒ� �z" & vbNewLine
							strMsg &= "����EdasysID�͓o�^�ł��܂���B" & vbNewLine
							strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
							MsgBox(strMsg)
							Return False
						End If
					End If
				Next jj
			End If

			' Ascii or Binary 
			If "" = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsAscii, ii).Value) Then
				strMsg = "�y ��ʏ����e�ݒ� �z" & vbNewLine
				strMsg &= "Ascii or Binary �̑I�����ĉ������B" & vbNewLine
				strMsg &= "- - - No." & CStr(ii + 1)
				MsgBox(strMsg)
				Return False
			End If

			' Ascii���l�ϊ�
			If "" = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsNum, ii).Value) Then
				strMsg = "�y ��ʏ����e�ݒ� �z" & vbNewLine
				strMsg &= "Ascii���l�ϊ��̑I�����ĉ������B" & vbNewLine
				strMsg &= "- - - No." & CStr(ii + 1)
				MsgBox(strMsg)
				Return False
			End If

            ' ���Z�W��
            strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_ConvCoef, ii).Value)
            If 0 <> tClassLibrary.tClass.tClass_ImputCheck.CheckImputData_Dbl(strWk, 0, , , 8) Then
                strMsg = "�y ���Z�W�� �z" & vbNewLine
                strMsg &= "�K�؂Ȓl�ł͂���܂���B" & vbNewLine
                strMsg &= "���͔͈́F�u0�ȏ�v�A�L�������_�����u8���v"
                MsgBox(strMsg, MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                Return False

            End If

            ' �R�C�����e�[�u������
			If System.DBNull.Value Is dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value Then
			Else
				strWk = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value)
				If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 64, "�R�C�����e�[�u������", True) <> 0 Then Return False
				dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value = strWk
			End If

		Next ii

        Return True

    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()

        'PcInfo�Z�b�g
        Call Data_GetPcInfo()
    End Sub

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^(PcInfo)���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_GetPcInfo()

        m_DsPcInfo.Tables(0).Rows.Clear()

        For ii As Integer = 0 To dgvPcInfo.Rows.Count - 2
            Dim drowNew As DataRow = m_DsPcInfo.Tables(0).NewRow()                                          ' �V�K�s
            drowNew.Item("Order") = dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Order, ii).Value                      ' No
            drowNew.Item("Name") = dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Name, ii).Value                        ' ����
            drowNew.Item("Size") = CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_Size, ii).Value)                  ' �o�C�g�T�C�Y
            drowNew.Item("EdasysID") = CInt(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_EdasysID, ii).Value)          ' EdasysID
            If "Ascii" = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsAscii, ii).Value) Then                    ' Ascii or Binary
                drowNew.Item("IsAscii") = "True"
            Else
                drowNew.Item("IsAscii") = "False"
            End If
            If "True" = CStr(dgvPcInfo(EM_PCINFO_GRID_ID.emCmb_IsNum, ii).Value) Then                           ' Ascii���l�ϊ�
                drowNew.Item("IsNum") = "True"
            Else
                drowNew.Item("IsNum") = "False"
            End If
            drowNew.Item("ConvCoef") = CDbl(dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_ConvCoef, ii).Value)              ' ���Z�W��
            drowNew.Item("CoilInfColName") = dgvPcInfo(EM_PCINFO_GRID_ID.emTxt_CoilInfColName, ii).Value  ' �R�C�����e�[�u������
            ' �e�[�u���ɒǉ�
            m_DsPcInfo.Tables(0).Rows.InsertAt(drowNew, m_DsPcInfo.Tables(0).Rows.Count)
        Next ii

    End Sub

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB�ڑ�
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String                                'Sql��

        Try
            '''' �f�[�^�x�[�X�I�[�v��
            If Not tcls_DB.Open() Then Return False

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            ' �f���[�g�C���T�[�g����̂Ńe�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_PARAM_PCINFO_DESC
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            For ii As Integer = 0 To m_DsPcInfo.Tables(0).Rows.Count - 1
                With m_DsPcInfo.Tables(0).Rows(ii)
                    '�o�^SQL���쐬
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_PCINFO_DESC & " VALUES ("
                    strSql &= .Item("Order").ToString & ",'"                 ' No
                    strSql &= .Item("Name").ToString & "',"                  ' ����
                    strSql &= .Item("Size").ToString & ","                   ' �o�C�g�T�C�Y
                    strSql &= .Item("EdasysID").ToString & ",'"              ' EdasysID
                    strSql &= .Item("IsAscii").ToString & "','"               ' Ascii or Binary
                    strSql &= .Item("IsNum").ToString & "',"                 ' Ascii���l�ϊ�
                    strSql &= .Item("ConvCoef").ToString & ",'"              ' ���Z�W��
                    strSql &= .Item("CoilInfColName").ToString & "')"        ' �R�C�����e�[�u������

                    'SQL���s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' �G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            '�R�~�b�g
            tcls_DB.TransactCommint()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_PCINFODESC)    ' ��ʏ��
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' ��n��
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ��ʏ����e�ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializePcInfo(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        '''' �ʐݒ�
        With dgv
            .RowCount = 0

            .AllowUserToAddRows = True                                      ' ���[�U�[ �s�ǉ�
			.AllowUserToResizeColumns = True								' ���[�U�[ ��T�C�Y�ύX

            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���

        End With

        '' ���`
        ' No
        dgvColSetText(dgv, "No", 50, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' ����
        dgvColSetText(dgv, "����", 260, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �o�C�g�T�C�Y
        dgvColSetText(dgv, "�o�C�g" & vbNewLine & "�T�C�Y", 88, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' EdasysID
        dgvColSetText(dgv, "EdasysID" & vbNewLine & "(0�ȏ�)", 88, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' Ascii or Binary
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "Ascii or" & vbNewLine & "Binary"
            clm.Width = 90
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.Items.AddRange("Ascii", "Binary")
            clm.MaxDropDownItems = clm.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' Ascii���l�ϊ�
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "Ascii" & vbNewLine & "���l�ϊ�"
            clm.Width = 90
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.Items.AddRange("True", "False")
            clm.MaxDropDownItems = clm.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' ���Z�W��
        dgvColSetText(dgv, "���Z�W��", 104, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' �R�C�����e�[�u������
        dgvColSetText(dgv, "�R�C�����" & vbNewLine & "�e�[�u������", 260, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

    End Sub

    ''' <summary>
    ''' dgv�̍ŏI�s�Z���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvPcInfo_UserAddedRow(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewRowEventArgs) Handles dgvPcInfo.UserAddedRow
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' No�A�ԕ\��
        If (dgv.RowCount - 3) < 0 Then
            dgv(EM_PCINFO_GRID_ID.emTxt_Order, dgv.RowCount - 2).Value = "1"            ' No
        Else
            dgv(EM_PCINFO_GRID_ID.emTxt_Order, dgv.RowCount - 2).Value = CInt(dgv(EM_PCINFO_GRID_ID.emTxt_Order, dgv.RowCount - 3).Value) + 1
        End If

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
