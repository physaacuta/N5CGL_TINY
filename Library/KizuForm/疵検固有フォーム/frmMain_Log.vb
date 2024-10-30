'*********************************************************************************
' ���O��ʂ̐e�N���X
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' ���O��ʂ̐e�N���X
''' </summary>
''' <remarks></remarks>
Public Class frmMain_Log
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ��`
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ���O���
	''' </summary>
	''' <remarks></remarks>
	Public Enum gem_LogMode
		''' <summary>�S��</summary>
		iAns = 0
		''' <summary>�\�t�g�E�F�A</summary>
		iSoft = 1
		''' <summary>�n�[�h�E�F�A</summary>
		iHard = 2
	End Enum

	''' <summary>
	''' ���O�敪
	''' </summary>
	''' <remarks></remarks>
	Public Enum gem_LogKind
		''' <summary>�C�x���g</summary>
		iEvt = 0
		''' <summary>�d�̏�</summary>
		iJyu = 1
		''' <summary>�y�̏�</summary>
		iKei = 2
		''' <summary>�R�����g</summary>
		iCmt = 3
	End Enum

	Private Enum em_LogListColm
		idayAddTime = 0
		idayDspTime
		iKind
		iMode
		iTaskName
		iPcName
		iPcKindName
		iSysNo
		iMsg
		iSubMsg
		iPcKind																' �R�����g�@�\�L�����̂݃f�[�^�Z�b�g
		iAutoNo																' �R�����g�@�\�L�����̂݃f�[�^�Z�b�g


		iend
	End Enum

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_clLogKindColor(3) As Color									' ���O�\���F (0:�C�x���g 1:�d�̏� 2:�y�̏� 3:�R�����g)
	'Private m_nLogMaxRow As Integer											' ���O�\���ő�s��
	Private m_lstChkPCName As New List(Of CheckBox)							' �o�b���`�F�b�N�{�b�N�X�R���g���[�� ���X�g
	Private m_lstChkPCKind As New List(Of CheckBox)							' PCKIND�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
	Private m_lstChkKubun_A As New List(Of CheckBox)						' �S�ʋ敪�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
	Private m_lstChkKubun_S As New List(Of CheckBox)						' �\�t�g�敪�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
	Private m_lstChkKubun_H As New List(Of CheckBox)						' �n�[�h�敪�`�F�b�N�{�b�N�X�R���g���[�� ���X�g

	' DB�֘A
	Private m_nLogModeSyubetu() As Integer = {1, 2, 3}						' ���O��� (�S��, �\�t�g, �n�[�h)
	Private m_nLogKindKubun() As Integer = {1, 2, 3, 4}						' ���O�敪 (�C�x���g, �d�̏�, �y�̏�, �R�����g)
	Private m_sDB_LOG_M As String = DB_LOG_MASTER							' DB�e�[�u�� ���O�}�X�^
	Private m_sDB_LOG_I As String = DB_LOG_INF								' DB�e�[�u�� ���O�C���t
	Private m_sDB_PC_I As String = DB_PC_INF								' DB�e�[�u�� PC���
	Private m_sDbSession As String = INI_DB									' DB�Z�b�V����

	' ���O�֘A
	Private m_bLog_ExecCancel As Boolean = False							' �L�����Z���� True

	' �I�v�V�������[�h
	Private m_bIsVisibleAddTime As Boolean = False							' �o�^���t�\����\���@�\
	Private m_bIsCommentOn As Boolean = False								' �R�����g�@�\ON/OFF (AutoNo��K�{)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���v���p�e�B
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' LOG_MASTER �� ���O�敪 (�C�x���g)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(1) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("LOG_MASTER �� ���O�敪 (�C�x���g)")> _
	Public Property tProp_LogKind_Evt() As Integer
		Get
			Return m_nLogKindKubun(gem_LogKind.iEvt)
		End Get
		Set(ByVal Value As Integer)
			m_nLogKindKubun(gem_LogKind.iEvt) = Value
		End Set
	End Property
	''' <summary>
	''' LOG_MASTER �� ���O�敪 (�d�̏�)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(2) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("LOG_MASTER �� ���O�敪 (�d�̏�)")> _
	Public Property tProp_LogKind_Jyu() As Integer
		Get
			Return m_nLogKindKubun(gem_LogKind.iJyu)
		End Get
		Set(ByVal Value As Integer)
			m_nLogKindKubun(gem_LogKind.iJyu) = Value
		End Set
	End Property
	''' <summary>
	''' LOG_MASTER �� ���O�敪 (�y�̏�)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(3) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("LOG_MASTER �� ���O�敪 (�y�̏�)")> _
	Public Property tProp_LogKind_Kei() As Integer
		Get
			Return m_nLogKindKubun(gem_LogKind.iKei)
		End Get
		Set(ByVal Value As Integer)
			m_nLogKindKubun(gem_LogKind.iKei) = Value
		End Set
	End Property
	''' <summary>
	''' LOG_MASTER �� ���O�敪 (�R�����g)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(4) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("LOG_MASTER �� ���O�敪 (�R�����g)")> _
	Public Property tProp_LogKind_Cmt() As Integer
		Get
			Return m_nLogKindKubun(gem_LogKind.iCmt)
		End Get
		Set(ByVal Value As Integer)
			m_nLogKindKubun(gem_LogKind.iCmt) = Value
		End Set
	End Property
	''' <summary>
	''' LOG_Master �̃e�[�u������
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_LOG_MASTER) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("LOG_MASTER �̃e�[�u������")> _
	Public Property tProp_TableName_LogMaster() As String
		Get
			Return m_sDB_LOG_M
		End Get
		Set(ByVal Value As String)
			m_sDB_LOG_M = Value
		End Set
	End Property

	''' <summary>
	''' LOG_INF �̃e�[�u������
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_LOG_INF) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("LOG_INF �̃e�[�u������")> _
	Public Property tProp_TableName_LogInf() As String
		Get
			Return m_sDB_LOG_I
		End Get
		Set(ByVal Value As String)
			m_sDB_LOG_I = Value
		End Set
	End Property

	''' <summary>
	''' PC_INF �̃e�[�u������
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_PC_INF) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("PC_INF �̃e�[�u������")> _
	Public Property tProp_TableName_PcInf() As String
		Get
			Return m_sDB_PC_I
		End Get
		Set(ByVal Value As String)
			m_sDB_PC_I = Value
		End Set
	End Property

	''' <summary>
	''' DB�Z�N�V����
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(INI_DB) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("DB�Z�b�V����")> _
	Public Property tProp_DbSession() As String
		Get
			Return m_sDbSession
		End Get
		Set(ByVal Value As String)
			m_sDbSession = Value
		End Set
	End Property
	''' <summary>
	''' �o�^���t��ON/OFF
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(False) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("�o�^���t ON/OFF")> _
	Public Property tProp_IsVisibleAddTime() As Boolean
		Get
			Return m_bIsVisibleAddTime
		End Get
		Set(ByVal Value As Boolean)
			m_bIsVisibleAddTime = Value
		End Set
	End Property
	''' <summary>
	''' �R�����g�@�\��ON/OFF
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(False) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("�R�����g�@�\ ON/OFF (Load�O�ɐݒ�ς݂̎��B���s���́A�ύX�s��)")> _
	Public Property tProp_IsComment() As Boolean
		Get
			Return m_bIsCommentOn
		End Get
		Set(ByVal Value As Boolean)
			m_bIsCommentOn = Value
		End Set
	End Property

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���A�N�Z�X ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ���O�\���F�ݒ� (-1�̎��́A���w��Ƃ���j
	''' </summary>
	''' <param name="nEvt">�C�x���g�F</param>
	''' <param name="nJyu">�d�̏�F</param>
	''' <param name="nKei">�y�̏�F</param>
	''' <param name="nCmt">�R�����g�F</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetLogKindColor(Optional ByVal nEvt As Integer = -1, Optional ByVal nJyu As Integer = -1, Optional ByVal nKei As Integer = -1, Optional ByVal nCmt As Integer = -1)
		If -1 <> nEvt Then m_clLogKindColor(gem_LogKind.iEvt) = Color.FromArgb(nEvt) 'tMod.tMod_Common.GetItoColor(nEvt)
		If -1 <> nJyu Then m_clLogKindColor(gem_LogKind.iJyu) = Color.FromArgb(nJyu) 'tMod.tMod_Common.GetItoColor(nJyu)
		If -1 <> nKei Then m_clLogKindColor(gem_LogKind.iKei) = Color.FromArgb(nKei) 'tMod.tMod_Common.GetItoColor(nKei)
		If -1 <> nCmt Then m_clLogKindColor(gem_LogKind.iCmt) = Color.FromArgb(nCmt)
	End Sub

	''' <summary>
	''' ���O�\���ő�s��
	''' </summary>
	''' <param name="nMaxRow">���O�\���ő�s��</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetLogMaxRow(ByVal nMaxRow As Integer)
		'm_nLogMaxRow = nMaxRow
		nudRowMaxCount.Value = nMaxRow
	End Sub

	''' <summary>
	''' �o�b���`�F�b�N�{�b�N�X�R���g���[�� ��o�^
	''' </summary>
	''' <param name="chk">�o�b���`�F�b�N�{�b�N�X�R���g���[��</param>
	''' <param name="pcName">DB��������PC����</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddCheckBoxPCName(ByVal chk As CheckBox, ByVal pcName As String)
		chk.Tag = pcName		' Tag�ɕ������ǉ�
		m_lstChkPCName.Add(chk)		' ���X�g�ɓo�^
		'' ���łɃN���b�N�C�x���g�n���h���ɒǉ�
		AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
	End Sub

	''' <summary>
	''' PCKind�`�F�b�N�{�b�N�X�R���g���[�� ��o�^
	''' </summary>
	''' <param name="chk">PCKind�`�F�b�N�{�b�N�X�R���g���[��</param>
	''' <param name="pcName">DB��������PCKind</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddCheckBoxPCKind(ByVal chk As CheckBox, ByVal pcName As String)
		chk.Tag = pcName		' Tag�ɕ������ǉ�
		m_lstChkPCKind.Add(chk)		' ���X�g�ɓo�^
		'' ���łɃN���b�N�C�x���g�n���h���ɒǉ�
		AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
	End Sub

	''' <summary>
	''' �`�F�b�N�{�b�N�X �w�i�F�ύX
	''' </summary>
	''' <param name="chk">�`�F�b�N�{�b�N�X</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetChkBackColor_Change(ByVal chk As CheckBox)
		If chk.Checked Then					' ON
			chk.BackColor = Color.Cyan
		Else								' OFF
			chk.BackColor = Drawing.SystemColors.Control
		End If
	End Sub
	''' <summary>
	''' �`�F�b�N�{�b�N�X �w�i�F�ύX
	''' </summary>
	''' <param name="chk">�`�F�b�N�{�b�N�X</param>
	''' <param name="checked"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetChkBackColor_Change(ByVal chk As CheckBox, ByVal checked As Boolean)
		If checked Then					' ON
			chk.Checked = checked
			chk.BackColor = Color.Cyan
		Else								' OFF
			chk.Checked = checked
			chk.BackColor = Drawing.SystemColors.Control
		End If
	End Sub

	''' <summary>
	''' ���X�g�z��ɓo�^����Ă���R���g���[���̃`�F�b�N�ς݌����Z�o
	''' </summary>
	''' <param name="lstChk"></param>
	''' <returns></returns>
	''' <remarks></remarks>
	Protected Friend Function GetLstCheckCount(ByVal lstChk As List(Of CheckBox)) As Integer
		Dim iCnt As Integer = 0
		For ii As Integer = 0 To lstChk.Count - 1
			If lstChk.Item(ii).Checked Then iCnt += 1
		Next ii
		Return iCnt
	End Function

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' ����
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Log_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		'' �f�t�H���g�F
		m_clLogKindColor(gem_LogKind.iEvt) = Color.Black
		m_clLogKindColor(gem_LogKind.iJyu) = Color.Red
		m_clLogKindColor(gem_LogKind.iKei) = Color.Blue
		m_clLogKindColor(gem_LogKind.iCmt) = Color.DarkGreen  'Color.FromArgb(0, 64, 0)

		'' �O���b�h��`
		dgvInitialize(dgvLog)
		'' �敪�`�F�b�N�{�b�N�X�����X�g�ɒǉ� (���̏��� �厖)
		m_lstChkKubun_A.Add(chkKubun_A_Evt)
		m_lstChkKubun_A.Add(chkKubun_A_Jyu)
		m_lstChkKubun_A.Add(chkKubun_A_Kei)
		m_lstChkKubun_A.Add(chkKubun_A_Cmt)
		m_lstChkKubun_S.Add(chkKubun_S_Evt)
		m_lstChkKubun_S.Add(chkKubun_S_Jyu)
		m_lstChkKubun_S.Add(chkKubun_S_Kei)
		m_lstChkKubun_S.Add(chkKubun_S_Cmt)
		m_lstChkKubun_H.Add(chkKubun_H_Evt)
		m_lstChkKubun_H.Add(chkKubun_H_Jyu)
		m_lstChkKubun_H.Add(chkKubun_H_Kei)
		m_lstChkKubun_H.Add(chkKubun_H_Cmt)

		'' �����Z�b�g
		dtpStartDay.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
		dtpStartTime.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
		dtpEndDay.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))
		dtpEndTime.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))

		'' �R�����g�@�\
		chkKubun_A_Cmt.Enabled = m_bIsCommentOn
		chkKubun_S_Cmt.Enabled = m_bIsCommentOn
		chkKubun_H_Cmt.Enabled = m_bIsCommentOn
		my_btnFunction(8 - 1).Enabled = m_bIsCommentOn
		'If Not m_bIsCommentOn Then my_btnFunction(8 - 1).Text = ""
	End Sub
	''' <summary>
	''' �J��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Log_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
		m_lstChkPCName.Clear()
	End Sub
	''' <summary>
	''' �`�F�b�N�{�b�N�X �`�F�b�N��ԕύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub chk_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkKubun_A_Jyu.CheckedChanged, chkKubun_A_Kei.CheckedChanged, chkKubun_A_Evt.CheckedChanged, chkKubun_H_Evt.CheckedChanged, chkKubun_H_Kei.CheckedChanged, chkKubun_H_Jyu.CheckedChanged, chkKubun_S_Evt.CheckedChanged, chkKubun_S_Kei.CheckedChanged, chkKubun_S_Jyu.CheckedChanged, chkKubun_A_Cmt.CheckedChanged, chkKubun_S_Cmt.CheckedChanged, chkKubun_H_Cmt.CheckedChanged
		' �`�F�b�N�{�b�N�X �w�i�ύX
		SetChkBackColor_Change(CType(sender, CheckBox))
	End Sub
	''' <summary>
	''' �����\��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub timClock_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timClock.Tick
		lblClock.Text = Now.ToString("yyyy/MM/dd HH:mm:ss")
	End Sub
	''' <summary>
	''' �����͈͐ݒ�`�F�b�N�{�b�N�X
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub chkStartEndDay_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkStartDay.CheckedChanged, chkEndDay.CheckedChanged
		If chkStartDay Is sender Then
			dtpStartDay.Enabled = chkStartDay.Checked
			dtpStartTime.Enabled = chkStartDay.Checked
		Else
			dtpEndDay.Enabled = chkEndDay.Checked
			dtpEndTime.Enabled = chkEndDay.Checked
		End If
	End Sub
	''' <summary>
	''' ���t�R���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpStartTime.MouseWheel, dtpEndDay.MouseWheel, dtpEndTime.MouseWheel
		' ���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
		SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ DataGridView �֘A
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �\���ݒ�
	''' </summary>
	''' <param name="dgv"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
		With dgv
			'' �����`
			.AutoGenerateColumns = False									' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
			.AllowUserToAddRows = False										' ���[�U�[ �s�ǉ�
			.AllowUserToDeleteRows = False									' ���[�U�[ �s�폜
			.AllowUserToOrderColumns = False								' ���[�U�[ ��ړ�
			.AllowUserToResizeColumns = True								' ���[�U�[ ��T�C�Y�ύX
			.AllowUserToResizeRows = False									' ���[�U�[ �s�T�C�Y�ύX
			.ShowCellErrors = False
			.ShowCellToolTips = False
			.ShowEditingIcon = False
			.ShowRowErrors = False

			.SelectionMode = DataGridViewSelectionMode.FullRowSelect		' �Z���I��
			.MultiSelect = False											' �����s�I��
			.ReadOnly = True												' �ǂݎ���p
			.EditMode = DataGridViewEditMode.EditProgrammatically			' �Z����������

			.ColumnHeadersVisible = True									' ��w�b�_�\��
			.RowHeadersVisible = False										' �s�w�b�_�\��
			.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None		' ����T�C�Y��������
			.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None			' ���s�T�C�Y��������
			.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing	'
			.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing			'

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = Color.DarkBlue			' �I��
			'.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
			'.DefaultCellStyle().SelectionForeColor = Color.Transparent
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue 'Color.LightSteelBlue         ' 1�s�����ɐF�ւ�


			'' ���`
			'.ColumnCount = 4			'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
			'.Columns.AddRange( _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn())

            .ColumnCount = CInt(IIf(m_bIsCommentOn, CType(em_LogListColm.iend, Integer), CType(em_LogListColm.iend - 2, Integer)))


			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.RowCount = 0
			.ColumnHeadersHeight = 25										' ��w�b�_�T�C�Y

			'��9 (����)
			.Columns(em_LogListColm.idayAddTime).HeaderText = "���/�����p���ꎞ��"
			.Columns(em_LogListColm.idayAddTime).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.idayAddTime).DefaultCellStyle.Format = "yyyy/MM/dd HH:mm:ss"
			.Columns(em_LogListColm.idayAddTime).Width = 160
			.Columns(em_LogListColm.idayAddTime).SortMode = DataGridViewColumnSortMode.NotSortable 'DataGridViewColumnSortMode.Automatic
			.Columns(em_LogListColm.idayAddTime).Visible = m_bIsVisibleAddTime
			'��0 (����)
			.Columns(em_LogListColm.idayDspTime).HeaderText = "�e�p�\�R����������"
			.Columns(em_LogListColm.idayDspTime).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.idayDspTime).DefaultCellStyle.Format = "yyyy/MM/dd HH:mm:ss"
			.Columns(em_LogListColm.idayDspTime).Width = 160
			.Columns(em_LogListColm.idayDspTime).SortMode = DataGridViewColumnSortMode.NotSortable 'DataGridViewColumnSortMode.Automatic
			'��1 (���)
			.Columns(em_LogListColm.iKind).HeaderText = "���"
			.Columns(em_LogListColm.iKind).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iKind).Width = 50
			.Columns(em_LogListColm.iKind).SortMode = DataGridViewColumnSortMode.NotSortable
			'��2 (�敪)
			.Columns(em_LogListColm.iMode).HeaderText = "�敪"
			.Columns(em_LogListColm.iMode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iMode).Width = 60
			.Columns(em_LogListColm.iMode).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(em_LogListColm.iMode).Frozen = True
			'��3 (������)
			.Columns(em_LogListColm.iTaskName).HeaderText = "������"
			.Columns(em_LogListColm.iTaskName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iTaskName).Width = 85
			.Columns(em_LogListColm.iTaskName).SortMode = DataGridViewColumnSortMode.NotSortable
			'��4 (�o�b��)
			.Columns(em_LogListColm.iPcName).HeaderText = "PC��"
			.Columns(em_LogListColm.iPcName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iPcName).Width = 80
			.Columns(em_LogListColm.iPcName).SortMode = DataGridViewColumnSortMode.NotSortable
			'��5 (�o�b���)
			.Columns(em_LogListColm.iPcKindName).HeaderText = "PC�@�\"
			.Columns(em_LogListColm.iPcKindName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iPcKindName).Width = 110
			.Columns(em_LogListColm.iPcKindName).SortMode = DataGridViewColumnSortMode.NotSortable
			'��6 (�R�[�h)
			.Columns(em_LogListColm.iSysNo).HeaderText = "����"
			.Columns(em_LogListColm.iSysNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iSysNo).Width = 50
			.Columns(em_LogListColm.iSysNo).SortMode = DataGridViewColumnSortMode.NotSortable
			'��7 (���e)
			.Columns(em_LogListColm.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.iMsg).HeaderText = "���e"
			.Columns(em_LogListColm.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.iMsg).Width = 800
			.Columns(em_LogListColm.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable
			'��8 (�ڍ�)
			.Columns(em_LogListColm.iSubMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.iSubMsg).HeaderText = "�ڍ�"
			.Columns(em_LogListColm.iSubMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.iSubMsg).Width = 500
			.Columns(em_LogListColm.iSubMsg).SortMode = DataGridViewColumnSortMode.NotSortable

			If m_bIsCommentOn Then
				'��10 (PCKIND) ��\��
				.Columns(em_LogListColm.iPcKind).HeaderText = "PCKIND"
				.Columns(em_LogListColm.iPcKind).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				.Columns(em_LogListColm.iPcKind).Width = 80
				.Columns(em_LogListColm.iPcKind).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(em_LogListColm.iPcKind).Visible = False
				'��11 (AutoNo) ��\��
				.Columns(em_LogListColm.iAutoNo).HeaderText = "AutoNo"
				.Columns(em_LogListColm.iAutoNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				.Columns(em_LogListColm.iAutoNo).Width = 80
				.Columns(em_LogListColm.iAutoNo).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(em_LogListColm.iAutoNo).Visible = False
			End If
		End With
	End Sub


	''' <summary>
	''' �I���s�������΍� (�I���s�F�ւ��𖳌�������ꍇ�́A�K�{�B�����ɂ���ƃf�[�^�Z�b�g����1�s�ڂ��ςȎ��ɂȂ�)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLog_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvLog.SelectionChanged
		' �w�i�F�������̏ꍇ�̂݃L�����Z��
		If dgvLog.DefaultCellStyle().SelectionBackColor = Color.Transparent Then CType(sender, DataGridView).ClearSelection()
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ SQL�������p
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' SQL������ (SELECT��,FROM��,WHERE��,END�� �̏��ԂɃR�[��)
	''' </summary>
	''' <param name="sql">����SQL��</param>
	''' <param name="bMode">true:�ʏ� false:Count(*)</param>
	''' <param name="nSelectCnt">0�ȊO�̏ꍇ top 10 �Ƃ��ɂ���</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub subSQL_Create(ByRef sql As String, ByVal bMode As Boolean, Optional ByVal nSelectCnt As Integer = 0)
		'' SELECT��
		subSQL_Create_Select(sql, bMode, nSelectCnt)
		'' FROM��
		subSQL_Create_From(sql)
		'' WHERE��
		subSQL_Create_Where(sql)
		'' END��
		subSQL_Create_End(sql, bMode)

	End Sub
	''' <summary>
	''' SQL������ (SELECT��)
	''' </summary>
	''' <param name="sql">����SQL��</param>
	''' <param name="bMode">true:�ʏ� false:Count(*)</param>
	''' <param name="nSelectCnt">0�ȊO�̏ꍇ top 10 �Ƃ��ɂ���</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub subSQL_Create_Select(ByRef sql As String, ByVal bMode As Boolean, Optional ByVal nSelectCnt As Integer = 0)
		If bMode Then
			sql = "SELECT "
			If 0 <> nSelectCnt Then sql &= "TOP " & nSelectCnt & " "
			sql &= "ISNULL(I.�������t����,''),"
			sql &= "ISNULL(M.���,'0'),"
			sql &= "ISNULL(M.�敪,'0'),"
			sql &= "ISNULL(I.������,''),"
			sql &= "ISNULL(I.PC��,''),"
			sql &= "ISNULL(P.PC���,''),"
			sql &= "ISNULL(M.�A���[��No,''),"
			sql &= "ISNULL(M.���e,''),"
			sql &= "ISNULL(I.���e,''),"
			sql &= "ISNULL(M.�ڍ�,''), "
			sql &= "ISNULL(I.�o�^���t,'')"
			If m_bIsCommentOn Then
				sql &= ",ISNULL(P.PCKIND,'0'),"
				sql &= "ISNULL(I.AutoNO,'0')"
			End If
		Else
			sql = "SELECT "
			sql &= "COUNT(*) "
		End If
	End Sub
	''' <summary>
	''' SQL������ (FROM��)
	''' </summary>
	''' <param name="sql">����SQL��</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub subSQL_Create_From(ByRef sql As String)
		sql &= "FROM " & m_sDB_LOG_I & " I "
		sql &= "LEFT OUTER JOIN " & m_sDB_LOG_M & " M ON I.�A���[��No = M.�A���[��No "
        sql &= "LEFT OUTER JOIN " & m_sDB_PC_I & " P ON I.PCKIND = P.PCKIND "

	End Sub
	''' <summary>
	''' SQL������ (WHERE��)
	''' </summary>
	''' <param name="sql">����SQL��</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub subSQL_Create_Where(ByRef sql As String)
		Dim strWhere As String = ""				' Where�像�[�N
		Dim ii As Integer
		Dim iWk As Integer
		Dim bWk As Boolean

		'''' �����t�B���^
		'' �J�n
		If chkStartDay.Checked Then
            strWhere &= "AND I.�������t���� >= '" & dtpStartDay.Value.ToString("yyyy/MM/dd ") & dtpStartTime.Value.ToString("HH:mm:ss") & "' "
		End If
		'' �I��
		If chkEndDay.Checked Then
            strWhere &= "AND I.�������t���� <= '" & dtpEndDay.Value.ToString("yyyy/MM/dd ") & dtpEndTime.Value.ToString("HH:mm:ss") & "' "
		End If


		'''' �o�b���t�B���^
		iWk = GetLstCheckCount(m_lstChkPCName)					' SQL�ȗ����΍�
		If iWk = 0 OrElse iWk = m_lstChkPCName.Count Then		' 0���I�� ���� �S���I��
			' ��������
		Else
			bWk = False											' PC�t�B���^��1���ł��L�莞 true
			For ii = 0 To m_lstChkPCName.Count - 1
				If m_lstChkPCName.Item(ii).Checked Then					' PC�t�B���^ �`�F�b�N
					If Not bWk Then
						strWhere &= "AND ("
						bWk = True
					Else
						strWhere &= " OR "
					End If

					' PC�Z�b�g
					strWhere &= " I.PC�� = '" & CStr(m_lstChkPCName.Item(ii).Tag) & "'"
				End If
			Next ii
			If bWk Then
				strWhere &= ") "
			End If
		End If

		'''' PCKind�t�B���^
		iWk = GetLstCheckCount(m_lstChkPCKind)					' SQL�ȗ����΍�
		If iWk = 0 OrElse iWk = m_lstChkPCKind.Count Then		' 0���I�� ���� �S���I��
			' ��������
		Else
			bWk = False											' PC�t�B���^��1���ł��L�莞 true
			For ii = 0 To m_lstChkPCKind.Count - 1
				If m_lstChkPCKind.Item(ii).Checked Then					' PC�t�B���^ �`�F�b�N
					If Not bWk Then
						strWhere &= "AND ("
						bWk = True
					Else
						strWhere &= " OR "
					End If

					' PC�Z�b�g
					strWhere &= " I.PCKIND = " & CStr(m_lstChkPCKind.Item(ii).Tag)
				End If
			Next ii
			If bWk Then
				strWhere &= ") "
			End If
		End If

		'''' �敪�t�B���^
		iWk = GetLstCheckCount(m_lstChkKubun_A)				' SQL�ȗ����΍�
		iWk += GetLstCheckCount(m_lstChkKubun_S)
		iWk += GetLstCheckCount(m_lstChkKubun_H)
		If iWk = 0 Then										' 0���I��
			' ��������
		Else
			Dim bKubun As Boolean = False
			strWhere &= "AND ("
			'''''''''''''''''''''''
			bWk = False											' �敪�t�B���^��1���ł��L�莞 true
			For ii = 0 To m_lstChkKubun_A.Count - 1
				If m_lstChkKubun_A.Item(ii).Checked Then				' �敪�t�B���^ �`�F�b�N
					If Not bWk Then
						If bKubun Then
							strWhere &= "OR "
						End If
						strWhere &= " (���= " & CStr(m_nLogModeSyubetu(0)) & " AND ("
						bWk = True
						bKubun = True
					Else
						strWhere &= " OR "
					End If

					' PC�Z�b�g
					strWhere &= " M.�敪 = " & CStr(m_nLogKindKubun(ii))
				End If
			Next ii
			If bWk Then
				strWhere &= ")) "
			End If


			'''''''''''''''''''''''
			bWk = False											' �敪�t�B���^��1���ł��L�莞 true
			For ii = 0 To m_lstChkKubun_S.Count - 1
				If m_lstChkKubun_S.Item(ii).Checked Then				' �敪�t�B���^ �`�F�b�N
					If Not bWk Then
						If bKubun Then
							strWhere &= "OR "
						End If
						strWhere &= " (���= " & CStr(m_nLogModeSyubetu(1)) & " AND ("
						bWk = True
						bKubun = True
					Else
						strWhere &= " OR "
					End If

					' PC�Z�b�g
					strWhere &= " M.�敪 = " & CStr(m_nLogKindKubun(ii))
				End If
			Next ii
			If bWk Then
				strWhere &= ")) "
			End If

			'''''''''''''''''''''''
			bWk = False											' �敪�t�B���^��1���ł��L�莞 true
			For ii = 0 To m_lstChkKubun_H.Count - 1
				If m_lstChkKubun_H.Item(ii).Checked Then				' �敪�t�B���^ �`�F�b�N
					If Not bWk Then
						If bKubun Then
							strWhere &= "OR "
						End If
						strWhere &= " (���= " & CStr(m_nLogModeSyubetu(2)) & " AND ("
						bWk = True
						bKubun = True
					Else
						strWhere &= " OR "
					End If

					' PC�Z�b�g
					strWhere &= " M.�敪 = " & CStr(m_nLogKindKubun(ii))
				End If
			Next ii
			If bWk Then
				strWhere &= ")) "
			End If

			strWhere &= ")"
		End If


		'''' �R�[�h�t�B���^
		'' �㉺���͈� & ����R�[�h���o
		If IsNumeric(txtCode_From.Text) Or IsNumeric(txtCode_To.Text) Or 0 <> txtCode_Select.TextLength Then
			' ���O����
			strWhere &= "AND ("
			bWk = False

			' �͈͎w��
			If IsNumeric(txtCode_From.Text) And IsNumeric(txtCode_To.Text) Then
				strWhere &= "(M.�A���[��No BETWEEN " & CStr(CInt(txtCode_From.Text)) & " AND " & CStr(CInt(txtCode_To.Text)) & ") "
				bWk = True
			ElseIf IsNumeric(txtCode_From.Text) Then
				strWhere &= "(M.�A���[��No BETWEEN " & CStr(CInt(txtCode_From.Text)) & " AND 9999) "
				bWk = True
			ElseIf IsNumeric(txtCode_To.Text) Then
				strWhere &= "(M.�A���[��No BETWEEN 0 AND " & CStr(CInt(txtCode_To.Text)) & ") "
				bWk = True
			End If

			' ����R�[�h���o
			If (0 <> txtCode_Select.TextLength) Then
				Dim strAry() As String = txtCode_Select.Text.Split(Chr(Asc(",")))
				For ii = 0 To strAry.Length - 1
					If IsNumeric(strAry(ii)) Then					' ����R�[�h���o�L��
						If bWk Then
							strWhere &= " OR "
						End If

						strWhere &= "M.�A���[��No=" & CStr(CInt(strAry(ii)))
						bWk = True
					End If
				Next ii
			End If

			' �㏈��
			strWhere &= ") "
		End If
		'' �ΏۊO�R�[�h�w��
		bWk = False
		If (0 <> txtCode_Not.TextLength) Then
			Dim strAry() As String = txtCode_Not.Text.Split(Chr(Asc(",")))
			For ii = 0 To strAry.Length - 1
				If IsNumeric(strAry(ii)) Then					' �ΏۊO�R�[�h�w��L��
					If Not bWk Then
						strWhere &= "AND ( "
						bWk = True
					Else
						strWhere &= " AND "
					End If

					strWhere &= "M.�A���[��No<>" & CStr(CInt(strAry(ii)))
				End If
			Next ii
			If bWk Then strWhere &= ") "
		End If


		'''' WHERE�吶��
		If 0 <> strWhere.Length Then
			sql &= "WHERE 1=1 " & strWhere
		End If
	End Sub
	''' <summary>
	''' SQL������ (END��)
	''' </summary>
	''' <param name="sql">����SQL��</param>
	''' <param name="bMode">true:�ʏ� false:Count(*)</param>	
	''' <remarks></remarks>
	Protected Friend Overridable Sub subSQL_Create_End(ByRef sql As String, ByVal bMode As Boolean)
		If bMode Then
			sql &= "ORDER BY I.�o�^���t DESC"
			If m_bIsCommentOn Then sql &= ", I.AutoNo DESC"
		End If
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �������s
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �������s���f
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecCancel_Log()
		m_bLog_ExecCancel = True
	End Sub
	''' <summary>
	''' ���O�\���J�n��
	''' </summary>
	''' <remarks></remarks>
    Protected Friend Overridable Sub ExecStart()
        '20180510 �敪�S�I���A�S�����ǉ� --->>>
        my_btnFunction(1).Enabled = False
        my_btnFunction(2).Enabled = False
        '20180510 �敪�S�I���A�S�����ǉ� <<<---
        my_btnFunction(3).Enabled = True
        my_btnFunction(4).Enabled = False
        my_btnFunction(11).Enabled = False
        m_bLog_ExecCancel = False
    End Sub
	''' <summary>
	''' ���O�\���I����
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecEnd()
        '20180510 �敪�S�I���A�S�����ǉ� --->>>
        my_btnFunction(1).Enabled = True
        my_btnFunction(2).Enabled = True
        '20180510 �敪�S�I���A�S�����ǉ� <<<---
        my_btnFunction(3).Enabled = False
		my_btnFunction(4).Enabled = True
		my_btnFunction(11).Enabled = True
		m_bLog_ExecCancel = False
	End Sub

	''' <summary>
	''' �R�����g�ҏW
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecComment()
		If Not m_bIsCommentOn Then Return

		'''' �I���s�m�F
		Dim nIndex As Integer			' �I���s (���I�����A-1) 
		If dgvLog.CurrentRow Is Nothing OrElse dgvLog.CurrentRow.Index = -1 Then nIndex = -1
		If -1 <> nIndex Then nIndex = dgvLog.CurrentRow.Index

		'''' �I���f�[�^�擾
		Dim typ As frmPop_LogComment.typSelectRowData = Nothing
		If -1 = nIndex Then				' �s���I��
			typ.nAutoNo = 0
			typ.nSysNo = 0
			typ.dayAddTime = Now
			typ.dayDspTime = Now
		Else							' �s�I��
			typ.dayAddTime = CDate(dgvLog(em_LogListColm.idayAddTime, nIndex).Value)
			typ.dayDspTime = CDate(dgvLog(em_LogListColm.idayDspTime, nIndex).Value)

			If m_nLogKindKubun(gem_LogKind.iCmt) = CInt(dgvLog(em_LogListColm.iMode, nIndex).Value) Then		' �R�����g��I��
				typ.nAutoNo = CInt(dgvLog(em_LogListColm.iAutoNo, nIndex).Value)
				typ.nSysNo = CInt(dgvLog(em_LogListColm.iSysNo, nIndex).Value)
				typ.sTaskName = CStr(dgvLog(em_LogListColm.iTaskName, nIndex).Value)
				typ.sPcName = CStr(dgvLog(em_LogListColm.iPcName, nIndex).Value)
				typ.nPcKind = CInt(dgvLog(em_LogListColm.iPcKind, nIndex).Value)
				typ.sMsg = CStr(dgvLog(em_LogListColm.iMsg, nIndex).Value)
			Else																			' �R�����g�ȊO
				typ.nAutoNo = 0
				typ.nSysNo = 0
				typ.sTaskName = ""
				typ.sPcName = ""
				typ.nPcKind = 0
				typ.sMsg = ""
			End If
		End If

		'''' �R�����g�ҏW��ʕ\��
		Using frm As New frmPop_LogComment
			' �K�v�ȏ����Z�b�g
			frm.tProp_SelectRowData = typ		' �I�����Z�b�g
			frm.tProp_DbSession = Me.m_sDbSession

			' ��ʕ\��
			Call frm.ShowDialog()

			' ���O���ҏW�� ��ʍĕ`��
			Dim bAns As Boolean
			bAns = frm.Prop_IsDataUpDate()		' ��������̕ύX���s�����ꍇ true
			If Not bAns Then Return
			MyBase.SetEvFunctionButton(5 - 1, True, Nothing)
		End Using
	End Sub

	''' <summary>
	''' �������s
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecSelect_Log()
		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSQL As String = ""
		Dim ii As Integer = 0
		Dim rowCnt As Integer = 0
		Dim rowMaxCnt As Integer = 0

		Try
			'''' ������
			Call ExecStart()
			dgvLog.RowCount = 0
			If Not tcls_DB.Open() Then Exit Sub
			Application.DoEvents()

			'''' ���X�g�����擾
            Call subSQL_Create(strSQL, False)
            If Not tcls_DB.GetSelectKey(strSQL, rowMaxCnt) Then Exit Sub
			' �p�t�H�[�}���X�ׂ̈ɁA�f�[�^�O���b�h�r���[�ɍs���Z�b�g
			If nudRowMaxCount.Value < rowMaxCnt Then		' �������
				rowMaxCnt = CInt(nudRowMaxCount.Value)
				lblRowCount.BackColor = Color.Pink
			Else
				rowMaxCnt = rowMaxCnt
				lblRowCount.BackColor = SystemColors.Control
			End If
			dgvLog.RowCount = rowMaxCnt
			lblRowCount.Text = rowMaxCnt.ToString("#,##0")
			prbReadRow.Minimum = 0
			prbReadRow.Value = 0
			prbReadRow.Maximum = rowMaxCnt
			prbReadRow.Visible = True
            Application.DoEvents()

			'''' �f�[�^���擾
			Call subSQL_Create(strSQL, True, rowMaxCnt)
			sqlRead = tcls_DB.SelectExecute(strSQL)	' Select�����s
			If sqlRead Is Nothing Then Exit Sub
			' �f�[�^���o��
			While sqlRead.Read()

				' �f�[�^���o��
				dgvLog(em_LogListColm.idayDspTime, rowCnt).Value = sqlRead.GetDateTime(0).ToString("yyyy/MM/dd HH:mm:ss")
				dgvLog(em_LogListColm.iKind, rowCnt).Value = sqlRead.GetInt32(1)
				dgvLog(em_LogListColm.iMode, rowCnt).Value = sqlRead.GetInt32(2)
				dgvLog(em_LogListColm.iTaskName, rowCnt).Value = sqlRead.GetString(3)
				dgvLog(em_LogListColm.iPcName, rowCnt).Value = sqlRead.GetString(4)
				dgvLog(em_LogListColm.iPcKindName, rowCnt).Value = sqlRead.GetString(5)
				dgvLog(em_LogListColm.iSysNo, rowCnt).Value = sqlRead.GetInt32(6)
				' �Œ胁�b�Z�[�W�������̏ꍇ�A���e�̐擪�ɃX�y�[�X������ɂ��Ă��܂��̂�����
				'dgvLog(em_LogListColm.iMsg, rowCnt).Value = sqlRead.GetString(7) & " " & sqlRead.GetString(8)
				If "" <> sqlRead.GetString(7) Then
					dgvLog(em_LogListColm.iMsg, rowCnt).Value = sqlRead.GetString(7) & " " & sqlRead.GetString(8)
				Else
					dgvLog(em_LogListColm.iMsg, rowCnt).Value = sqlRead.GetString(8)
				End If
				dgvLog(em_LogListColm.iSubMsg, rowCnt).Value = sqlRead.GetString(9)
				dgvLog(em_LogListColm.idayAddTime, rowCnt).Value = sqlRead.GetDateTime(10).ToString("yyyy/MM/dd HH:mm:ss")

				' �R�����g�@�\ON���̂�
				If m_bIsCommentOn Then
					dgvLog(em_LogListColm.iPcKind, rowCnt).Value = sqlRead.GetInt32(11)
					dgvLog(em_LogListColm.iAutoNo, rowCnt).Value = sqlRead.GetInt32(12)
				End If


                ' �v���O���X�o�[
                '20180510 �v���O���X�o�[�����ύX --->>>
                'prbReadRow.PerformStep()
                'If 0 = (rowCnt Mod 100) Then
                '    Application.DoEvents()
                'End If
                If 0 = (rowCnt Mod prbReadRow.Step) Then
                    prbReadRow.PerformStep()
                    Application.DoEvents()
                End If
                '20180510 �v���O���X�o�[�����ύX <<<---

				'''' ���s����
				rowCnt += 1
				If rowCnt >= rowMaxCnt Then Exit Sub
				If m_bLog_ExecCancel Then					' �L�����Z��
					' �L�����Z���������� �\�����Ă���s�������ɂ���
					dgvLog.RowCount = rowCnt
					lblRowCount.Text = rowCnt.ToString
					Exit Sub
				End If
			End While

			'''' DB�ؒf
			tcls_DB.Close()
		Catch ex As Exception
		Finally
			' DB�J��
			If Not sqlRead Is Nothing Then sqlRead.Close()
			Call tcls_DB.Dispose()

			' �R���g���[��������
			prbReadRow.Visible = False
			Call ExecEnd()
		End Try
	End Sub

	''' <summary>
	''' �\���G���A�݂̂̕`��w��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLog_CellFormatting(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs) Handles dgvLog.CellFormatting
		' ��ʂ��當���ɕύX
		If e.ColumnIndex = em_LogListColm.iKind Then
			Dim strVal As String = ""
			Select Case CInt(e.Value)
				Case m_nLogModeSyubetu(gem_LogMode.iAns)
					strVal = "�S��"
				Case m_nLogModeSyubetu(gem_LogMode.iSoft)
					strVal = "���"
				Case m_nLogModeSyubetu(gem_LogMode.iHard)
					strVal = "ʰ��"
				Case Else
					strVal = ""
			End Select
			e.Value = strVal
		End If

		' �敪���當���ɕύX
		If e.ColumnIndex = em_LogListColm.iMode Then
			Dim strVal As String = ""
			Select Case CInt(e.Value)
				Case m_nLogKindKubun(gem_LogKind.iEvt)
					strVal = "�����"
				Case m_nLogKindKubun(gem_LogKind.iJyu)
					strVal = "�d�̏�"
				Case m_nLogKindKubun(gem_LogKind.iKei)
					strVal = "�y�̏�"
				Case m_nLogKindKubun(gem_LogKind.iCmt)
					strVal = "����"
				Case Else
					strVal = ""
			End Select
			e.Value = strVal
		End If

		' �Z�������F���w�� (���낢���������ʂ��ꂪ��ԑ���)
		If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then		'�f�t�H���g�F�̏ꍇ�̂ݐݒ�
			Dim color As Color = Drawing.Color.Black
			Select Case CInt(dgvLog(em_LogListColm.iMode, e.RowIndex).Value)
				Case m_nLogKindKubun(gem_LogKind.iEvt)
					color = m_clLogKindColor(gem_LogKind.iEvt)
				Case m_nLogKindKubun(gem_LogKind.iJyu)
					color = m_clLogKindColor(gem_LogKind.iJyu)
				Case m_nLogKindKubun(gem_LogKind.iKei)
					color = m_clLogKindColor(gem_LogKind.iKei)
				Case m_nLogKindKubun(gem_LogKind.iCmt)
					color = m_clLogKindColor(gem_LogKind.iCmt)
				Case Else
			End Select
			If e.CellStyle.ForeColor = color Then Exit Sub
			e.CellStyle.ForeColor = color

		End If
	End Sub


    ''' <summary>
    ''' CSV�o��
    ''' </summary>
	''' <param name="strFolder">�ۑ��t�@�C���̃t�H���_�p�X</param>
	''' <param name="strName">�ۑ��t�@�C���̃t�@�C����</param>
    ''' <remarks></remarks>
	Protected Friend Overridable Sub CsvOut(ByVal strFolder As String, ByVal strName As String)
		Dim strFPath As String
		Dim strMsg As String = ""
		Dim strData As String = ""						'1�s�̃f�[�^

		'' �t���p�X
		strFPath = IO.Path.Combine(strFolder, strName)

		'' �t�H���_�̗L���`�F�b�N(�����������낤)
		tMod.CheckFolder(strFolder)

		'' CSV�o��
		Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
			Try
				' �I�[�v��
				cls_Stram.Open()

                strData = ""
                '�w�b�_�[�o��
                For ii As Integer = 0 To dgvLog.Columns.Count - 1
                    strData &= dgvLog.Columns(ii).HeaderText & ","
                Next ii

                '�Ō��","��r��
                strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)


				'�f�[�^�o��
				For ii As Integer = 0 To dgvLog.Rows.Count - 1
					strData = ""
					For jj As Integer = 0 To dgvLog.ColumnCount - 1
						'�敪�̏ꍇ�́A�ʏ���
                        If jj = 2 Then
                            Select Case CInt(dgvLog(jj, ii).Value)
                                Case m_nLogModeSyubetu(gem_LogMode.iAns)
                                    strData &= "�S��,"
                                Case m_nLogModeSyubetu(gem_LogMode.iSoft)
                                    strData &= "���,"
                                Case m_nLogModeSyubetu(gem_LogMode.iHard)
                                    strData &= "ʰ��,"
                                Case Else
                                    strData &= ","
                            End Select

                        ElseIf jj = 3 Then
                            Select Case CInt(dgvLog(jj, ii).Value)
                                Case m_nLogKindKubun(gem_LogKind.iEvt)
                                    strData &= "�����,"
                                Case m_nLogKindKubun(gem_LogKind.iJyu)
                                    strData &= "�d�̏�,"
                                Case m_nLogKindKubun(gem_LogKind.iKei)
                                    strData &= "�y�̏�,"
                                Case m_nLogKindKubun(gem_LogKind.iCmt)
                                    strData &= "����,"
                                Case Else
                                    strData &= ","
                            End Select
                        Else
                            strData &= dgvLog(jj, ii).Value.ToString & ","
                        End If
					Next jj

                    '�Ō��","��r��
					strData = strData.Remove(strData.Length - 1, 1)

					cls_Stram.WriteLine(strData)
				Next ii

				' �N���[�Y
				cls_Stram.Close()
				strMsg = System.IO.Path.GetFullPath(strFPath) & "��CSV�t�@�C����ۑ����܂����B"

			Catch ex As Exception
				strMsg = "CSV�o�͂Ɏ��s���܂����B"
			End Try
		End Using

		'''' �K�C�_���X�\��
		Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
			frmMsg.ShowDialog()
		End Using
	End Sub

    '20180510 �敪�S�I���A�S�����ǉ� --->>>
    ''' <summary>
    ''' �敪�`�F�b�N�S�I��
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub btnAllOn_Click()

        For ii As Integer = 0 To m_lstChkKubun_A.Count - 1
            m_lstChkKubun_A(ii).CheckState = CheckState.Checked
        Next

        For ii As Integer = 0 To m_lstChkKubun_S.Count - 1
            m_lstChkKubun_S(ii).CheckState = CheckState.Checked
        Next

        For ii As Integer = 0 To m_lstChkKubun_H.Count - 1
            m_lstChkKubun_H(ii).CheckState = CheckState.Checked
        Next

    End Sub

    ''' <summary>
    ''' �敪�`�F�b�N�S����
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub btnAllOff_Click()

        For ii As Integer = 0 To m_lstChkKubun_A.Count - 1
            m_lstChkKubun_A(ii).CheckState = CheckState.Unchecked
        Next

        For ii As Integer = 0 To m_lstChkKubun_S.Count - 1
            m_lstChkKubun_S(ii).CheckState = CheckState.Unchecked
        Next

        For ii As Integer = 0 To m_lstChkKubun_H.Count - 1
            m_lstChkKubun_H(ii).CheckState = CheckState.Unchecked
        Next

    End Sub
    '20180510 �敪�S�I���A�S�����ǉ� <<<---
End Class
