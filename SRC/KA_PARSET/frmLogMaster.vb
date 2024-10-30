'*********************************************************************************
' ���O�}�X�^�[�ݒ���
'	[Ver]
'		Ver.01    2012/06/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

''' <summary>
''' ���O�}�X�^�[�ݒ���
''' </summary>
''' <remarks></remarks>
Public Class frmLogMaster
	'---------------------------------------------------------------------------
	' �񋓑̒�`
	'---------------------------------------------------------------------------
	''' <summary>
    ''' ���O�}�X�^�[�ݒ��
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_COL_DGVMST
        emTxt_Alarm_No = 0                      ' �e�L�X�g�{�b�N�X�@ �A���[��No
        emCmb_Syubetsu                          ' �R���{�{�b�N�X�@�@ ���
        emCmb_Kubun                             ' �R���{�{�b�N�X     �敪
		emChk_GaidUmu							' �`�F�b�N�{�b�N�X   �I�y�K�C�L��
		emTxt_Gaidtime							' �e�L�X�g�{�b�N�X   �I�y�K�C����
		emTxt_Naiyou							' �e�L�X�g�{�b�N�X   ���e
        emTxt_Syousai                           ' �e�L�X�g�{�b�N�X   �ڍ�
    End Enum

    '---------------------------------------------------------------------------
    ' �\���̒�`
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' ���O�}�X�^�[�\���f�[�^
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure LOG_DISP_VALUE
        Dim strName As String
        Dim nDBValue As Integer
        Dim nForeClr As Color
        Dim nBackClr As Color

        ''' <summary>
        ''' �f�[�^�Z�b�g
        ''' </summary>
        ''' <param name="name">�\���f�[�^��</param>
        ''' <param name="value">DB��̒l</param>
        ''' <param name="fore">�\�������F</param>
        ''' <param name="back">�\���w�i�F</param>
        ''' <remarks></remarks>
        Public Sub SetValue(ByVal name As String, ByVal value As Integer, ByVal fore As Color, ByVal back As Color)
            strName = name
            nDBValue = value
            nForeClr = fore
            nBackClr = back
        End Sub
    End Structure

    '---------------------------------------------------------------------------
    ' �萔��`
    '---------------------------------------------------------------------------
    Private m_stSyubetsuErr() As LOG_DISP_VALUE             ' ���
    Private m_stKubunErr() As LOG_DISP_VALUE                ' �敪

    '---------------------------------------------------------------------------
    ' �����o�ϐ�
    '---------------------------------------------------------------------------
    Private m_Adapter As New SqlClient.SqlDataAdapter       ' �A�_�v�^�[Type
    Private m_DataSet As New DataSet                        ' ��ڑ��^�e�[�u��TYPE_NAME

    '---------------------------------------------------------------------------
    ' ���\�b�h
    '---------------------------------------------------------------------------

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���j���[�{�^��
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmLogMaster_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "���O�}�X�^�[�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    ''' <param name="cancel">�I��������߂�ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmLogMaster_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DataSet.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If

        ' ��ʐؑ�
        ShowForm(frmID)
    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmLogMaster_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmLogMaster             ' ���O�}�X�^�[�ݒ���

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        Data_Init()                                                             ' �\���f�[�^������
        dgvInitialize(dgvData)                                                  ' �r��dgv������

        If Not DB_Select() Then                                                 ' �c�a�̒l�擾���s�Ȃ�I��
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                    ' �I��
        End If

        Data_Disp()                                                             ' ��ʂɒl��\��

        dgvData.DataGridViewSelectionRowColorChange(g_ColorSelection)           ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�

        Call UpdateEnabled(g_bPassFlg)                                          ' �X�V�{�^���̗L��/����

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
    ''' �\���f�[�^�̏�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Init()

        '-----------------------------------------------------------------------
        ' ��ʂ̕\�����쐬
        '-----------------------------------------------------------------------
        ReDim m_stSyubetsuErr(3 - 1)
		m_stSyubetsuErr(0).SetValue("�S��", 1, Color.Yellow, g_ColorKikiOk)
        m_stSyubetsuErr(1).SetValue("�\�t�g�E�F�A", 2, Color.Black, g_ColorKikiKei)
        m_stSyubetsuErr(2).SetValue("�n�[�h�E�F�A", 3, Color.Black, g_ColorKikiJyu)

        '-----------------------------------------------------------------------
        ' �敪�̕\�����쐬
        '-----------------------------------------------------------------------
		ReDim m_stKubunErr(4 - 1)
		m_stKubunErr(0).SetValue("�C�x���g", 1, Color.Yellow, g_ColorKikiOk)
		m_stKubunErr(1).SetValue("�d�̏�", 2, Color.Black, g_ColorKikiJyu)
		m_stKubunErr(2).SetValue("�y�̏�", 3, Color.Black, g_ColorKikiKei)
		m_stKubunErr(3).SetValue("�R�����g", 4, Color.Black, Color.White)

    End Sub

    ''' <summary>
    ''' ��ʎ����̔j���C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmLogMaster_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then
            modMain.TaskEnd()                         '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        End If
    End Sub

    ''' <summary>
    ''' �f�[�^�̎擾
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""

        '-----------------------------------------------------------------------
        ' ���O�}�X�^�[�擾���쐬
        '-----------------------------------------------------------------------
        strSQL = "SELECT �A���[��No"
        strSQL &= ", ���, �敪"
		strSQL &= ", �I�y�K�C�L��, �I�y�K�C����"
		strSQL &= ", ���e, �ڍ�"
		strSQL &= " FROM " & DB_LOG_MASTER
		strSQL &= " ORDER BY �A���[��No ASC"

        '-----------------------------------------------------------------------
        ' ���O�}�X�^�[�e�[�u���擾
        '-----------------------------------------------------------------------
        Dim strErrMsg As String = ""
        If 0 > MyBase.Get_SelectData(strSQL, m_DataSet, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LOG_MASTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True
    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        Dim rows() As DataRow = m_DataSet.Tables(0).Select()
        dgvData.RowCount = rows.Length
        For ii As Integer = 0 To rows.Length - 1
            Dim row As DataRow = rows(ii)
            '-------------------------------------------------------------------
            ' �A���[��No
            '-------------------------------------------------------------------
            dgvData(EM_COL_DGVMST.emTxt_Alarm_No, ii).Value = row.Item("�A���[��No")

            '-------------------------------------------------------------------
            ' ���
            '-------------------------------------------------------------------
            For Each dsp As LOG_DISP_VALUE In m_stSyubetsuErr
                If dsp.nDBValue.Equals(row.Item("���")) = True Then
                    dgvData(EM_COL_DGVMST.emCmb_Syubetsu, ii).Value = dsp.strName
                End If
            Next dsp

            '-------------------------------------------------------------------
            ' �敪
            '-------------------------------------------------------------------
            For Each dsp As LOG_DISP_VALUE In m_stKubunErr
                If dsp.nDBValue.Equals(row.Item("�敪")) = True Then
                    dgvData(EM_COL_DGVMST.emCmb_Kubun, ii).Value = dsp.strName
                End If
            Next dsp

			'-------------------------------------------------------------------
			' �I�y�K�C�L��
            '-------------------------------------------------------------------
            If row.Item("�I�y�K�C�L��") Is DBNull.Value = True Then
                dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value = False
            Else
                Dim nWK As Integer = CInt(row.Item("�I�y�K�C�L��"))
                If 1 = nWK Then
                    dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value = True
                Else
                    dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value = False
                End If
                'dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value = row.Item("�I�y�K�C�L��")
            End If

            '-------------------------------------------------------------------
            ' �I�y�K�C����
            '-------------------------------------------------------------------
            If row.Item("�I�y�K�C����") Is DBNull.Value = True Then
                dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value = ""
            Else
                dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value = row.Item("�I�y�K�C����")
            End If
            '-------------------------------------------------------------------
            ' ���e
            '-------------------------------------------------------------------
            dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value = row.Item("���e")

            '-------------------------------------------------------------------
            ' �ڍ�
            '-------------------------------------------------------------------
            dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value = row.Item("�ڍ�")

        Next ii
    End Sub

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

            ' ��ʂ̃f�[�^���擾
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

            ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
            If Not DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
                frmMsg.Dispose()
                modMain.TaskEnd()                       '�I��
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�m�菈���G���[ [" & ex.Message & "] " & Me.Text)
        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        '' ��ʂɒl��\��
        Data_Disp()

    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String
        Dim bNullChk As Boolean
        Dim nAlarmNo As Integer = 0

        ' �A���[��No�����[�v
        For ii As Integer = 0 To dgvData.RowCount - 1
            nAlarmNo = CInt(dgvData(EM_COL_DGVMST.emTxt_Alarm_No, ii).Value)

            If Not Nothing Is dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value Then        ' ���e�̕������`�F�b�N
                strWk = dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value.ToString
                If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 128, "�A���[��No=" & nAlarmNo & " ���e") <> 0 Then Return False
                dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value = strWk
            End If

            If Not Nothing Is dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value Then        ' �ڍׂ̕������`�F�b�N
                strWk = dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value.ToString
                If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "�A���[��No=" & nAlarmNo & " �ڍ�") <> 0 Then Return False
                dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value = strWk
            End If

            ' �I�y�K�C����
            ' �I�y�K�C�L���ɂ�� NULL���@�𔻒肷��
            If CBool(dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value) = True Then
                '�I�y�K�C�L�̏ꍇ�ANULL�����Ȃ�
                bNullChk = False
            Else
                '�I�y�K�C�����̏ꍇ�ANULL����
                bNullChk = True
            End If

            strWk = CStr(dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value)
            'HACK 2015/02/18�@�A���[��NO��\��
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 10, 172800, bNullChk, "�A���[��No=" & nAlarmNo & " �I�y�K�C����") <> 0 Then Return False

        Next ii

        Return True
    End Function

    ''' <summary>
    ''' ���͏����f�[�^�Z�b�g�ɐݒ肷��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim rows() As DataRow = m_DataSet.Tables(0).Select()
        For ii As Integer = 0 To rows.Length - 1
            Dim row As DataRow = rows(ii)
            Dim strVal As String = ""

            '-----------------------------------------------------------------------------
            ' ���
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Syubetsu, ii).Value)
            For Each dsp As LOG_DISP_VALUE In m_stSyubetsuErr
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("���") = dsp.nDBValue
                    Exit For
                End If
            Next dsp

            '-----------------------------------------------------------------------------
            ' �敪
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Kubun, ii).Value)
            For Each dsp As LOG_DISP_VALUE In m_stKubunErr
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("�敪") = dsp.nDBValue
                    Exit For
                End If
            Next dsp

			'-------------------------------------------------------------------
			' �I�y�K�C�L��
            '-------------------------------------------------------------------
            If IsDBNull(dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value) Or _
                Nothing Is dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value Then
                row.Item("�I�y�K�C�L��") = DBNull.Value
            Else
                If CBool(dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value) Then
                    row.Item("�I�y�K�C�L��") = 1                                              ' �L��
                Else
                    row.Item("�I�y�K�C�L��") = 0                                              ' ����
                End If
            End If
            '-------------------------------------------------------------------
            ' �I�y�K�C����
            '-------------------------------------------------------------------
            If IsDBNull(dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value) Or _
                Nothing Is dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value Then
                row.Item("�I�y�K�C����") = DBNull.Value
            Else
                strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value)
                If "" = strVal Then
                    row.Item("�I�y�K�C����") = DBNull.Value
                Else
                    row.Item("�I�y�K�C����") = dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value
                End If
            End If
            'row.Item("�I�y�K�C����") = dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value
            '-----------------------------------------------------------------------------
            ' ���e
            '-----------------------------------------------------------------------------
            If IsDBNull(dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value) Or _
                Nothing Is dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value Then
                row.Item("���e") = DBNull.Value
            Else
                strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value)
                If strVal.Length > 0 Then
                    row.Item("���e") = strVal
                Else
                    row.Item("���e") = DBNull.Value
                End If
            End If

            '-----------------------------------------------------------------------------
            ' �ڍ�
            '-----------------------------------------------------------------------------
            If IsDBNull(dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value) Or _
                Nothing Is dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value Then
                row.Item("�ڍ�") = DBNull.Value
            Else
                strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value)
                If strVal.Length > 0 Then
                    row.Item("�ڍ�") = strVal
                Else
                    row.Item("�ڍ�") = DBNull.Value
                End If
            End If

        Next ii
    End Sub

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   '�g�����U�N�V��������

        Try
            ' DB�ڑ�
            m_Adapter.SelectCommand.Connection.Open()

            '�g�����U�N�V�����J�n
            transact = m_Adapter.SelectCommand.Connection.BeginTransaction()
            m_Adapter.SelectCommand.Transaction = transact

            Using builder As New SqlClient.SqlCommandBuilder()
                builder.DataAdapter = m_Adapter
                m_Adapter.Update(m_DataSet)                             ' �ύX�����A�b�v�f�[�g����
            End Using

            '�R�~�b�g
            transact.Commit()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_SETTIG_LOG)                      ' �V�X���O
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

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)							' �I��F����

		With dgv
			'-------------------------------------------------------------------
			' ��{�����`
			'-------------------------------------------------------------------
			.ColumnHeadersHeight = 42										' �w�b�_�[�̍���
			.ScrollBars = ScrollBars.Both									' �X�N���[���o�[�̕\��

			' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor		'�I��F
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue				' 1�s�����ɐF�ւ�
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue		'��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

			.AllowUserToResizeColumns = True											' ���[�U�[ ��T�C�Y�ύX �\�Ƃ���

        End With

        '-------------------------------------------------------------------
        ' ���`
        '-------------------------------------------------------------------
        ' �A���[��No
		dgvColSetText(dgv, "�װ�" & vbNewLine & "No", 60, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' ���
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "���"
			clm.Width = 140
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As LOG_DISP_VALUE In m_stSyubetsuErr
                lstItem.Items.Add(dsp.strName)
            Next dsp
            dgv.Columns.Add(clm)
        End Using

        ' �敪
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�敪"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As LOG_DISP_VALUE In m_stKubunErr
                lstItem.Items.Add(dsp.strName)
            Next dsp
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

		' �I�y�K�C�L��
        dgvColSetCheck(dgv, "��߶޲" & vbNewLine & "�L����1", 80, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)


        ' �I�y�K�C����
		dgvColSetText(dgv, "��߶޲" & vbNewLine & "����[�b]", 80, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' ���e
		dgvColSetText(dgv, "���e", 285, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        ' �ڍ�
		dgvColSetText(dgv, "�ڍ�", 285, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

    End Sub

    ''' <summary>
    ''' dgv�̃Z�����I�����ꂽ�Ƃ��ɃR���{�{�b�N�X�Ȃ�h���b�v�_�E������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}�ł̃h���b�v�_�E�����삪�A�J��PC���Əo���Ȃ��A�����s��</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)                     ' �Ώۂ�dgv
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then   ' ��̃^�C�v���R���{�{�b�N�X���ǂ���
            SendKeys.Send("%{DOWN}")                                              ' �h���b�v�_�E���\��
        End If
    End Sub

    ''' <summary>
    ''' �Z���̒l���ύX���ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellValueChanged

        ' ��w�b�_�[�A�s�w�b�_�[�ύX���͏�������
        If e.RowIndex < 0 Then Exit Sub
        If e.ColumnIndex < 0 Then Exit Sub

        Dim nRow As Integer = e.RowIndex
        Dim nCol As Integer = e.ColumnIndex

        ' NULL�l�͏�������
        If IsDBNull(dgvData(nCol, nRow).Value) Then
            Exit Sub
        End If

        Dim strVal As String = CStr(dgvData(nCol, nRow).Value)

        If nCol = EM_COL_DGVMST.emCmb_Kubun Then                                ' �敪�̏ꍇ
            For ii As Integer = 0 To m_stKubunErr.Length - 1
                Dim dsp As LOG_DISP_VALUE = m_stKubunErr(ii)
                ' �敪�ύX���̐F�ւ�����
                If dsp.strName.CompareTo(strVal) = 0 Then
                    dgvData(EM_COL_DGVMST.emCmb_Kubun, nRow).Style.ForeColor = m_stKubunErr(ii).nForeClr
                    dgvData(EM_COL_DGVMST.emCmb_Kubun, nRow).Style.BackColor = m_stKubunErr(ii).nBackClr
                End If
            Next ii
        End If
    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                                            ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub

End Class
