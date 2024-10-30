Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary.tCnt

''' <summary>
''' �x����
''' </summary>
''' <remarks></remarks>
Public Class frmHyAlarm
    '///////////////////////////////////////////////////////
    '�����o�[�ϐ�
    '///////////////////////////////////////////////////////
    Private m_typHyAlarm As TBL_HY_ALARM

    '///////////////////////////////////////////////////////
    '�����o�[�֐�
    '///////////////////////////////////////////////////////
    ''' <summary>
    ''' �f�[�^�O���b�h������
    ''' </summary>
    ''' <param name="dgvCtrl">�f�[�^�O���b�h�R���g���[��</param>
    ''' <remarks></remarks>
    Private Sub Initdgv(ByRef dgvCtrl As tCnt_DataGridView)
        '��{�ݒ�
        dgvCtrl.dgvInitialize(dgvCtrl)

        '�ʐݒ�
        With dgvCtrl
            '�Z���ݒ�
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '�s�ݒ�
            .RowCount = 2 * NUM_MEN              '�s��
            .RowTemplate.Height = 20

            '��ݒ�
			.ColumnCount = ALARM_COL_NUM + 2				  '��
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20

            '�o�͈ʒu
            .Columns(0).HeaderText = "�o�͈ʒu"
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(0).Width = 120
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(0).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .Columns(0).ReadOnly = True

            '��
            .Columns(1).HeaderText = "��"
            .Columns(1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(1).Width = 45
            .Columns(1).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(1).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .Columns(1).ReadOnly = True

            '�����v������
            For ii As Integer = 0 To ALARM_COL_NUM - 1
				.Columns(2 + ii).HeaderText = CStr(ii + 1)
				.Columns(2 + ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				.Columns(2 + ii).Width = 45
				.Columns(2 + ii).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(2 + ii).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			Next ii
        End With
    End Sub

    ''' <summary>
    ''' �f�[�^�ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadData()
        Dim nRow As Integer = 0
        Dim strTmp As String

        Try
            '���L�������ǂݍ���
            Dim nRetc As ValueType = 0
            m_typHyAlarm = CType(ksMemRead(TBL_HY_ALARM_NAME, 0, GetType(TBL_HY_ALARM), nRetc), TBL_HY_ALARM)

            '�f�[�^�\��
            For ii As Integer = 0 To 2 - 1           '�o�͈ʒu
                    For jj As Integer = 0 To NUM_MEN - 1            '��

                        If ii = 0 Then
                            dgvFlag(0, nRow).Value = "�\������ʒu"
                        Else
                            dgvFlag(0, nRow).Value = "��������ʒu"
                        End If

                        If jj = 0 Then
                            dgvFlag(1, nRow).Value = "�\"
                        Else
                            dgvFlag(1, nRow).Value = "��"
                        End If

                    For kk As Integer = 0 To ALARM_COL_NUM - 1

                        If CBool(nRetc) Then
                            dgvFlag(2 + kk, nRow).Value = m_typHyAlarm.a_mode.kendai(ii).torb(jj).row(kk)
                        Else
                            dgvFlag(2 + kk, nRow).Value = ""
                        End If
                    Next
                    nRow += 1

                    Next
                Next

        Catch ex As Exception
            strTmp = String.Format("�f�[�^�ǂݍ��ُ݈�[{0}]", ex.Message)
            MsgBox(strTmp)
        End Try
    End Sub

    ''' <summary>
    ''' �f�[�^��������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub WriteData()
        Dim strTmp As String
        Dim nPos As Integer
        Dim nMen As Integer

        Try
            For ii As Integer = 0 To dgvFlag.RowCount - 1      '�s��
                If CStr(dgvFlag(0, ii).Value) = "�\������ʒu" Then
                    nPos = 0
                Else
                    nPos = 1
                End If

                If CStr(dgvFlag(1, ii).Value) = "�\" Then
                    nMen = 0
                Else
                    nMen = 1
                End If

                For jj As Integer = 0 To ALARM_COL_NUM - 1
                    m_typHyAlarm.a_mode.kendai(nPos).torb(nMen).row(jj) = CInt(dgvFlag(2 + jj, ii).Value)
                Next
            Next

            '���L��������������
            If Not ksMemWrite(TBL_HY_ALARM_NAME, m_typHyAlarm) Then
                strTmp = String.Format("���L�������������ُ݈�[{0}]", TBL_HY_ALARM_NAME)
                MsgBox(strTmp)
            End If
        Catch ex As Exception
            strTmp = String.Format("�f�[�^�������ُ݈�[{0}]", ex.Message)
            MsgBox(strTmp)
        End Try
    End Sub

    '///////////////////////////////////////////////////////
    '�C�x���g�֐�
    '///////////////////////////////////////////////////////
    ''' <summary>
    ''' ��ʃ��[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmHyAlarm_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '�f�[�^�O���b�h������
        Initdgv(dgvFlag)

        '�f�[�^�ǂݍ���
        ReadData()
    End Sub

    ''' <summary>
    ''' �ǂݍ��݃{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        '�f�[�^�ǂݍ���
        ReadData()
    End Sub

    ''' <summary>
    ''' �������݃{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click
        '�f�[�^��������
        WriteData()
	End Sub

	Private WithEvents m_tmAuto As New Timer

    ''' <summary>
    ''' �����X�V����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_tmAuto_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles m_tmAuto.Tick
		'�f�[�^�ǂݍ���
		ReadData()
	End Sub

    Private Sub CheckBox1_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CheckBox1.CheckedChanged
        Dim chkBox As CheckBox = CType(sender, CheckBox)
        If chkBox.Checked Then
            btnRead.Enabled = False
            btnWrite.Enabled = False
            chkBox.BackColor = Color.Cyan
            m_tmAuto.Interval = 500 ' 500msec ���Ɏ����ǂݍ���
            m_tmAuto.Start()
        Else
            chkBox.BackColor = SystemColors.Control
            btnRead.Enabled = True
            btnWrite.Enabled = True
            m_tmAuto.Stop()
        End If
    End Sub

    Private Sub frmHyAlarm_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_tmAuto.Stop()
        m_tmAuto.Dispose()
    End Sub
End Class
