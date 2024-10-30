'*********************************************************************************
' ��{����ꗗ���
'	[Ver]
'		Ver.01    2012/10/02  ��{����ꗗ��ʒǉ��Ή�(�V�K�쐬)
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' ��{����ꗗ���
''' </summary>
''' <remarks></remarks>
Public Class frmBaseList

#Region "�錾��"
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �^�錾
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' INI�t�@�C�� ���W���[���ϐ��p �\����
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure BASE_DATA
        Dim sCode As String     '�R�[�h
        Dim sName As String     '�\������
        Dim nMinute As Integer  '�o�ߔ���l�i���j
        Dim sCheckAdd As String '���e�ǉ�����
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �O���b�h �񍀖�
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum em_LogListColm
        iName = 0       '�\������
        idayDspTime     '��������
        iSysNo          '�R�[�h
        iMsg            '���e
        iend            '�ŏI�iMAX��j
    End Enum

    ''' <summary>
    ''' INI�t�@�C�� ��{����ꗗ����
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum em_IniBaseData
        iCode = 0       '�R�[�h
        iMinute         '�o�ߔ���l�i���j
        iName           '�\������
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_dsLogInf As New DataSet           '���O���e�[�u�� �f�[�^�Z�b�g
    Private m_BaseData() As BASE_DATA           '��{����ꗗ �i�[�p �\���̔z��

#End Region

#Region "��������"

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    Public Sub New()

        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmBaseList_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location
        Me.Height = Me.Owner.Height
        Me.Width = Me.Owner.Width

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        ' ���u��
        If NOA_ONLINE <> g_NOA Then
            ' �t�H�[�����
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
        End If

        '�I�����C������ �����l�Z�b�g
        Call SetTimePicker()

        '�O���b�h�ݒ菈��
        Call dgvInitialize(dgvBaseList)

        '��{����ꗗ�擾����
        If GetBaseListData() = False Then
            Dim frm As New frmRas
            frm.SetParentForm(Me.my_frmParent)
            Me.SetParentForm(Nothing)
            frm.Show()
            ExecFromClose()
            Exit Sub
        End If


        '�ꗗ��ʕ\�������i�����\���j
        Call DspGrid()

    End Sub

    ''' <summary>
    ''' �I�����C������ �����l�Z�b�g����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetTimePicker()

        Dim sWork As String         'INI�t�@�C�� �ꎞ���[�N�ϐ�
        Dim dtNow As Date = Now     '���ݎ���

        'INI�t�@�C������ �O��ݒ肵���I�����C���������擾����
        sWork = tMod.tMod_IniFile.ReadIniStr(SO_GAMENN, "ONLINE_DATE", LOCALINI_NAME, "")

        '�ŏ�10�o�C�g����t�ɃZ�b�g
        If IsDate(Mid(sWork, 1, 10)) Then
            '���t�^�������Ƃ��Z�b�g
            dtpStartDay.Text = Mid(sWork, 1, 10)
        Else
            '�s���Ȓl�̂Ƃ��͌��ݓ��t���Z�b�g����
            dtpStartDay.Text = Format(dtNow, "yyyy/MM/dd")
        End If

        '13�����ڂ��� 8�o�C�g�������ɃZ�b�g
        If IsDate(Mid(sWork, 12, 8)) Then
            '���t�^�������Ƃ��Z�b�g
            dtpStartTime.Text = Mid(sWork, 12, 8)
        Else
            '�s���Ȓl�̂Ƃ��͌��ݎ������Z�b�g����
            dtpStartTime.Text = Format(dtNow, "HH:mm:ss")
        End If

        '�I�����C������ �L���^���� �擾
        sWork = tMod.tMod_IniFile.ReadIniStr(SO_GAMENN, "ONLINE_CHECK", LOCALINI_NAME, "")
        If sWork = "0" Then
            '0�̂Ƃ��͖���
            chkStartDay.Checked = False
        End If
    End Sub

    ''' <summary>
    ''' ��{����ꗗ�擾����
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetBaseListData() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String
        Dim Adapter As New SqlClient.SqlDataAdapter      ' �A�_�v�^�[
        Dim bRet As Boolean = False
        Dim nCnt As Integer
        Dim dsData As New DataSet

        Try

            strSQL = ""
            strSQL &= " SELECT "
            strSQL &= "     �\������, "
            strSQL &= "     �A���[��No, "
            strSQL &= "     �����s�ُ펞��, "
            strSQL &= "     �\������, "
            strSQL &= "     ���e�ǉ����� "
            strSQL &= " FROM "
            strSQL &= DB_BASE_ACTION_MASTER
            strSQL &= " ORDER BY "
            strSQL &= "     �\������ ASC "

            tcls_DB.Open()

            'SQL���s
            Adapter = tcls_DB.SelectAdapter(strSQL)

            '�擾�O�Ƀf�[�^�Z�b�g�N���A
            dsData.Clear()

            '�f�[�^���f�[�^�Z�b�g�ɓo�^
            Adapter.Fill(dsData)

            For nCnt = 0 To dsData.Tables(0).Rows.Count - 1

                '�\���̔z��ǉ�
                ReDim Preserve m_BaseData(nCnt)

                '�e���ڐݒ�
                m_BaseData(nCnt).sCode = dsData.Tables(0).Rows(nCnt).Item("�A���[��No").ToString
                m_BaseData(nCnt).nMinute = CInt(dsData.Tables(0).Rows(nCnt).Item("�����s�ُ펞��").ToString)
                m_BaseData(nCnt).sName = dsData.Tables(0).Rows(nCnt).Item("�\������").ToString
                m_BaseData(nCnt).sCheckAdd = dsData.Tables(0).Rows(nCnt).Item("���e�ǉ�����").ToString

            Next

            '����I��
            bRet = True

        Catch ex As Exception
            '��O����
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "��{����ꗗ�擾�������s�B[" & ex.Message & "]")
            DspMsg("��{����ꗗ�擾�������s�B[" & ex.Message & "]")
        Finally
            '�N���X���
            If Not tcls_DB Is Nothing Then
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

            '�f�[�^�Z�b�g
            If Not dsData Is Nothing Then
                dsData.Dispose()
                dsData = Nothing
            End If
        End Try

        Return bRet
    End Function



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �O���b�h�ݒ�
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)

        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��

            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            ' �I���Z���F��`�i�I��F�����Ȃ̂ŃR�����g�j
            '.DefaultCellStyle().SelectionBackColor = SelectionBackColor         ' �I��
            '.DefaultCellStyle().SelectionForeColor = SelectionForeColor            '
            '.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue 'Color.LightSteelBlue         ' 1�s�����ɐF�ւ�

            .ColumnCount = 4        '�J�������F4

            '��w�b�_������
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .RowCount = 0
            .ColumnHeadersHeight = 25                                       ' ��w�b�_�T�C�Y

            '��1 (�\������)
            .Columns(em_LogListColm.iName).HeaderText = "�\������"
            .Columns(em_LogListColm.iName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iName).Width = 300
            .Columns(em_LogListColm.iName).SortMode = DataGridViewColumnSortMode.NotSortable
            '��2 (��������)
            .Columns(em_LogListColm.idayDspTime).HeaderText = "��������"
            .Columns(em_LogListColm.idayDspTime).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.idayDspTime).DefaultCellStyle.Format = "yyyy/MM/dd HH:mm:ss"
            .Columns(em_LogListColm.idayDspTime).Width = 160
            .Columns(em_LogListColm.idayDspTime).SortMode = DataGridViewColumnSortMode.NotSortable 'DataGridViewColumnSortMode.Automatic
            '��3 (�R�[�h)
            .Columns(em_LogListColm.iSysNo).HeaderText = "����"
            .Columns(em_LogListColm.iSysNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iSysNo).Width = 50
            .Columns(em_LogListColm.iSysNo).SortMode = DataGridViewColumnSortMode.NotSortable
            '��4 (���e)
            .Columns(em_LogListColm.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iMsg).HeaderText = "���e"
            .Columns(em_LogListColm.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iMsg).Width = 800
            .Columns(em_LogListColm.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable

        End With
    End Sub
#End Region

#Region "�t�@���N�V����"
    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmBaseList_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' 0�I���W��
			Case 4		'�\��
				Call DspGrid()

			Case 11									  ' RAS��ʒ��ڕ\��
				'��ʃN���[�Y �e��ʕ\��
				MyBase.ExecFromClose()
		End Select
    End Sub
#End Region

#Region "�f�[�^�\���n"
    ''' <summary>
    ''' �O���b�h�\������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspGrid()

        '��{���삪�ЂƂ��o�^����Ă��Ȃ��Ƃ��\�������I��
        If m_BaseData Is Nothing Then
            Exit Sub
        End If

        '�O���b�h����U������
        dgvBaseList.RowCount = 0

		'�O���b�h�l�ݒ菈��
        If SetGridRow() = False Then
            '�\�����s
            dgvBaseList.RowCount = 0
            Exit Sub
        End If

        '�I�����C�����Ԑݒ菈��
        Call SetIniData()

    End Sub

    ''' <summary>
    ''' �O���b�h�l�ݒ菈��
    ''' </summary>
    ''' <returns>True:����I�� False:�ُ�I��</returns>
    ''' <remarks></remarks>
    Private Function SetGridRow() As Boolean

        '�ꗗ��������̃A���[��No�Ō������Z�b�g
        Dim bRet As Boolean = False
        Dim dtNow As Date = Now     '���ݎ�����ۑ�
        Dim dtCheck As Date

        Try

            For nCnt As Integer = 0 To UBound(m_BaseData)

                '�O���b�h�s�J�E���g�A�b�v
                dgvBaseList.RowCount += 1

                '�\�����̃Z�b�g
                dgvBaseList(em_LogListColm.iName, nCnt).Value = m_BaseData(nCnt).sName

                '�A���[��No�Z�b�g
                dgvBaseList(em_LogListColm.iSysNo, nCnt).Value = m_BaseData(nCnt).sCode

				'���O���擾����
				If GetDbData(m_BaseData(nCnt).sCode, m_BaseData(nCnt).sCheckAdd) = False Then
                    Exit Try
                End If

				If m_dsLogInf.Tables(0).Rows.Count = 0 Then
					'�����ɊY������f�[�^�����݂��Ȃ��������A���������A���e�A�ڍׂ�
					'�󔒂ɐݒ肵�A��̔w�i�F���(�s���N)�ɐݒ肷��
					dgvBaseList(em_LogListColm.idayDspTime, nCnt).Value = ""          '��������
                    dgvBaseList(em_LogListColm.iMsg, nCnt).Value = ""                 '���e
                    dgvBaseList.Rows(nCnt).DefaultCellStyle.BackColor = Color.Pink	  '�w�i�F
				Else

					If m_BaseData(nCnt).nMinute > 0 Then
						'�o�ߔ���l��0�𒴂���l�̂Ƃ��̂ݎ��{
						'���ݎ�������o�ߔ���l(��)�̒l�����Z
						dtCheck = dtNow.AddMinutes(-1 * m_BaseData(nCnt).nMinute)
						If m_dsLogInf.Tables(0).Rows(0).Item("�o�^���t").ToString < Format(dtCheck, "yyyy/MM/dd HH:MM:ss") Then
							'�Y���f�[�^���o�ߔ���l���o�߂��Ă�����w�i�F�����F�ɐݒ�
							dgvBaseList.Rows(nCnt).DefaultCellStyle.BackColor = Color.Yellow
						End If
					End If

					'���������A���e�A�ڍׂɒl���Z�b�g
					dgvBaseList(em_LogListColm.idayDspTime, nCnt).Value = m_dsLogInf.Tables(0).Rows(0).Item("�o�^���t")

                    If m_dsLogInf.Tables(0).Rows(0).Item("���e1").ToString <> "" Then
                        dgvBaseList(em_LogListColm.iMsg, nCnt).Value = m_dsLogInf.Tables(0).Rows(0).Item("���e1").ToString & " " & m_dsLogInf.Tables(0).Rows(0).Item("���e2").ToString
                    Else
                        dgvBaseList(em_LogListColm.iMsg, nCnt).Value = m_dsLogInf.Tables(0).Rows(0).Item("���e2").ToString
                    End If

                End If

            Next

            '����I��
            bRet = True

        Catch ex As Exception
            '��O����
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�O���b�h�l�ݒ菈�����s�B[" & ex.Message & "]")
            DspMsg("�O���b�h�l�ݒ菈�����s�B[" & ex.Message & "]")
		Finally

		End Try

        Return bRet
    End Function

	''' <summary>
	''' ���O���擾����
	''' </summary>
	''' <param name="sCode">�A���[��No</param>
	''' <param name="sCheckAdd">���e�ǉ�����</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetDbData(ByVal sCode As String, ByVal sCheckAdd As String) As Boolean

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
		Dim strSQL As String
		Dim Adapter As New SqlClient.SqlDataAdapter		 ' �A�_�v�^�[
		Dim bRet As Boolean = False

		Try

			'�擾�O�Ƀf�[�^�Z�b�g�N���A
			m_dsLogInf.Clear()

			strSQL = ""
			strSQL &= "        SELECT "
			strSQL &= "              I.�o�^���t AS �o�^���t, "
            strSQL &= "              L.���e AS ���e1, "
            strSQL &= "              I.���e AS ���e2 "

            strSQL &= "        FROM ("
			' �A���[��No �� ���e�ǉ������P�ʂŏW��
			strSQL &= "            SELECT "
			strSQL &= "                MAX(I.�o�^���t) AS �o�^���t "
			strSQL &= "            FROM " & DB_LOG_INF & " AS I "
			strSQL &= "            WHERE I.�A���[��No = '" & sCode & "' "
			If sCheckAdd <> "" Then
				strSQL &= "            AND   I.���e LIKE '" & sCheckAdd & "'  "
			End If
			If chkStartDay.Checked Then
				strSQL &= "            AND �������t���� >= '" & dtpStartDay.Value.ToString("yyyy/MM/dd ") & dtpStartTime.Value.ToString("HH:mm:ss") & "' "
			End If
			strSQL &= "        ) AS MAX_DATA "

			strSQL &= " INNER JOIN " & DB_LOG_INF & " AS I ON I.�o�^���t = MAX_DATA.�o�^���t "

			strSQL &= " INNER JOIN " & DB_LOG_MASTER & " AS L ON I.�A���[��No = L.�A���[��No "

			strSQL &= "        WHERE I.�A���[��No = '" & sCode & "' "

			If sCheckAdd <> "" Then
				strSQL &= "        AND   I.���e LIKE '" & sCheckAdd & "'  "
			End If

			tcls_DB.Open()

			'SQL���s
			Adapter = tcls_DB.SelectAdapter(strSQL)

			'�f�[�^���f�[�^�Z�b�g�ɓo�^
			Adapter.Fill(m_dsLogInf)

			'����I��
			bRet = True

		Catch ex As Exception
			'��O����
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�f�[�^�擾�������s�B[" & ex.Message & "]")
			DspMsg("�f�[�^�擾�������s�B[" & ex.Message & "]")
		Finally
			'�N���X���
			If Not tcls_DB Is Nothing Then
				tcls_DB.Close()
				tcls_DB.Dispose()
				tcls_DB = Nothing
			End If
		End Try

		Return bRet
	End Function

    ''' <summary>
    ''' INI�t�@�C�� �I�����C�����Ԑݒ菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetIniData()
        Try
            '�`�F�b�N�������Ă���Ƃ��̂ݓo�^�������s��
            If chkStartDay.Checked = True Then
                tMod.tMod_IniFile.WriteIni(SO_GAMENN, "ONLINE_CHECK", LOCALINI_NAME, "1")
            Else
                tMod.tMod_IniFile.WriteIni(SO_GAMENN, "ONLINE_CHECK", LOCALINI_NAME, "0")
            End If

            If tMod.tMod_IniFile.WriteIni(SO_GAMENN, "ONLINE_DATE", LOCALINI_NAME, dtpStartDay.Value.ToString("yyyy/MM/dd") & " " & dtpStartTime.Value.ToString("HH:mm:ss")) = False Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "INI�t�@�C�� �I�����C�����Ԑݒ菈�����s�B")
                DspMsg("INI�t�@�C�� �I�����C�����Ԑݒ菈�����s�B")
            End If
        Catch ex As Exception
            '��O����
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "INI�t�@�C�� �I�����C�����Ԑݒ菈�����s�B[" & ex.Message & "]")
            DspMsg("INI�t�@�C�� �I�����C�����Ԑݒ菈�����s�B[" & ex.Message & "]")
        End Try
    End Sub
#End Region

#Region "�C�x���g����"
    ''' <summary>
    ''' ���t�R���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpStartTime.MouseWheel
        ' ���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    ''' <summary>
    ''' �����͈͐ݒ�`�F�b�N�{�b�N�X �`�F�b�N�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkStartEndDay_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkStartDay.CheckedChanged
        '�I�����C�������̗L���^�����؂�ւ�
        dtpStartDay.Enabled = chkStartDay.Checked
        dtpStartTime.Enabled = chkStartDay.Checked
    End Sub

    ''' <summary>
    ''' ��ʃN���[�Y������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmBaseList_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        '�f�[�^�Z�b�g�����
        If Not m_dsLogInf Is Nothing Then
            m_dsLogInf.Dispose()
            m_dsLogInf = Nothing
        End If
    End Sub
#End Region

#Region "�m�F���b�Z�[�W"
    ''' <summary>
    ''' �m�F���b�Z�[�W�\��
    ''' </summary>
    ''' <param name="strMsg"></param>
    ''' <remarks></remarks>
    Public Overridable Sub DspMsg(ByVal strMsg As String)

        Dim frmMsg As tClassForm.frmPop_MsgBox  '���b�Z�[�W�N���X

        '�N���X����
        frmMsg = New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, False, System.Windows.Forms.DialogResult.OK)

        '���b�Z�[�W�\��
        frmMsg.ShowDialog()

        '���b�Z�[�W�N���X��n��
        If Not frmMsg Is Nothing Then
            frmMsg.Dispose()
            frmMsg = Nothing
        End If

    End Sub
#End Region

End Class
