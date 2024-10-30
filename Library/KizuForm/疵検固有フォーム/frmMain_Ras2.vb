'*******************************************************************************
' RAS��ʂ̐e�N���X
'	[Ver]
'		Ver.01    2010/01/05  �V�K�쐬
'
'	[����]
'		�@���Ԃ͓o�^�����R���g���[���̃^�O�ɐݒ肳���
'*******************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' RAS��ʂ̐e�N���X
''' </summary>
''' <remarks></remarks>
Public Class frmMain_Ras2
	'/ /////////////////////////////////////////////////////////////////////////
	'/ �񋓑�
	'/ /////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �@��敪
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_RAS_TYPE
		PC = 0																	' PC�̏�Ԃ�����
		KIKI																	' �@��̏�Ԃ�����
	End Enum

	''' <summary>
	''' �@���Ԃ̕\���敪
	''' </summary>
	''' <remarks>RAS��ʂ̊e���x����{�^���ɕ\��������e</remarks>
	Public Enum EM_RAS_DSPTYPE
		DSP_STRING = 0															' ������\��
		DSP_VALUE																' �l�\��
		DSP_AUTO																' ��������
	End Enum

	''' <summary>
	''' �@���Ԑݒ��
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_RAS_SETTYPE
		DSP_CONTROL = 0															' �o�^����Ă���R���g���[���ɐݒ�
		DSP_LINE																' (PC��ԕ\���p) ���C���ɐݒ�
	End Enum

	''' <summary>
	''' PC�@�\���\���敪
	''' </summary>
	''' <remarks>PC�{�^���ɕ\������PC�@�\���̕\���敪</remarks>
	Public Enum EM_DSP_PCKIND_NAME
		PROGRAM = 0																' �\�t�g�Œ�̖��O��\��
		PCKIND																	' PC�@�\�̖��̂�\��
	End Enum

	''' <summary>
	''' PC���\���敪
	''' </summary>
	''' <remarks>PC�{�^���ɕ\������PC���̕\���敪</remarks>
	Public Enum EM_DSP_PC_NAME
		SYSTEM = 0																' PC����\��
		USER																	' �p�\�R������\��
	End Enum

	'/ /////////////////////////////////////////////////////////////////////////
	'/ �\����
	'/ /////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �@����
	''' </summary>
	''' <remarks></remarks>
	Public Structure STATUS_INF
		Dim nKikiId As Integer													' �@��ID
		Dim nStatus As Integer													' �@����
		Dim nKikiErr As Integer													' �@��ُ�x
		Dim sDspData As String													' ��ʕ\���f�[�^
		Dim nPcKind As Integer													' PCKIND
		Dim nOffLine As Integer													' PC�敪
		' 2014.05.21 �|�R VRCL�p�S���ۑ��V�X�e���ǉ�     >>>>>>>>
		Dim nKendErr As Integer													' �����ُ�x
		' 2014.05.21 �|�R VRCL�p�S���ۑ��V�X�e���ǉ�     <<<<<<<<
	End Structure

	''' <summary>
	''' PC���
	''' </summary>
	''' <remarks></remarks>
	Public Structure PC_INF
		Dim strNameSystem As String												' PC���� (�l�b�g���[�N��̃R���s���[�^��)
		Dim strNameUser As String												' �p�\�R������ (�\����̃R���s���[�^��)
	End Structure

	''' <summary>
	''' PC�\���R���g���[�����
	''' </summary>
	''' <remarks></remarks>
	Public Structure PC_CTRL
		Dim strName As String													' �{�^����
		Dim btnPcName As Button													' PC�{�^��
		Dim lblShadow As Label													' PC�{�^���e
		Dim lblLine As List(Of Label)											' ������x��

		''' <summary>
		''' �R���g���[���������������܂�
		''' </summary>
		''' <remarks></remarks>
		Public Sub initialize()
			lblLine = New List(Of Label)
		End Sub
	End Structure

	''' <summary>
	''' �@���ԕ\���R���g���[�����
	''' </summary>
	''' <remarks></remarks>
	Private Structure KIKI_CTRL
		Dim nKikiType As EM_RAS_TYPE											' �@��敪
		Dim nSetType As EM_RAS_SETTYPE											' ��Ԑݒ��
		Dim nDspType As EM_RAS_DSPTYPE											' �@��\���敪
		Dim lstCtrl As List(Of Control)											' 1�̋@���Ԃ�\������R���g���[���ꗗ

		''' <summary>
		''' �R���g���[���������������܂�
		''' </summary>
		''' <remarks></remarks>
		Public Sub initialize()
			lstCtrl = New List(Of Control)
		End Sub
	End Structure

	'/ /////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////
	Private m_dctCtrlPC As New Dictionary(Of Integer, PC_CTRL)					' PC��ԕ\���R���g���[���ꗗ (�L�[:PCKIND)
	Private m_dctCtrlKiki As New Dictionary(Of Integer, KIKI_CTRL)				' �@���ԕ\���R���g���[���ꗗ (�L�[:�@��ID)
	Private m_dctPcInf As New Dictionary(Of Integer, PC_INF)					' PC���ꗗ

	Private m_sDB_PC_INF As String = DB_PC_INF									' DB�e�[�u�� PC���
	Private m_sDB_PC_M As String = DB_PC_MASTER									' DB�e�[�u�� PC�}�X�^�[
	Private m_sDB_STATUS_I As String = DB_STATUS_INF							' DB�e�[�u�� �@����
	Private m_sDB_STATUS_M As String = DB_STATUS_MASTER							' DB�e�[�u�� �@���ԃ}�X�^�[
	Private m_sDB_COMMON As String = DB_COMMON_MASTER							' DB�e�[�u�� �r���萔�}�X�^�[

	Private m_nToukatu As Integer												' ����PC��PCKIND
	Private m_nAllResetID As Integer											' �SPC�̃��Z�b�g����PCKIND���Z�b�g(�~��,�~�ςƓ������ꏏ�̏ꍇ�͓���)
	Private m_nLogDspCnt As Integer												' ���O�\����
	Private m_dgvLog As DataGridView											' ���O�\���f�[�^�O���b�h
    Private m_strResetPass As String = ""                                       ' PC���Z�b�g�p�X���[�h
	Private m_nEventExit As Integer = FACT_KS_MASTER_06							' �V�X�e���I��
	Private m_nEventReset As Integer = FACT_KS_MASTER_07						' PC�ċN���̃C�x���g�ԍ�
	Private m_nEventLogOff As Integer = FACT_KS_MASTER_08						' PC���O�I�t�̃C�x���g�ԍ�
	Private m_nLineID As Integer = LINE_ID										' ���C��ID
	Private m_nNoa As Integer													' NOA�ڑ��t���O (0:�I�����C�� 1:���u 9:�f�o�b�O)
	Private m_sDbSession As String = INI_DB										' DB�Z�N�V������
	Private m_sMasterTask As String = KS_MASTER									' PC�}�X�^�[�Ǘ��v���Z�X��
	Private m_sMasterPC As String = MASTER_PC_NAME								' PC�}�X�^�[�Ǘ����N������PC��
	Private m_nDspPcKindName As EM_DSP_PCKIND_NAME = EM_DSP_PCKIND_NAME.PCKIND	' PC�{�^���ɕ\������PC�@�\���\���敪
	Private m_nDspPcName As EM_DSP_PC_NAME = EM_DSP_PC_NAME.USER				' PC�{�^���ɕ\��PC���敪

	Protected Friend m_lstItem As New List(Of Control)							' �F�\������R���g���[���̃��X�g

	''' <summary>
	''' PC��ʃe�[�u�����̎擾�Ɛݒ�
	''' </summary>
	''' <value>PC��ʃe�[�u����</value>
	''' <returns>PC��ʃe�[�u����</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_PC_INF) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("PC_INF �̃e�[�u������")> _
	Public Property tProp_TableName_PcInf() As String
		Get
			Return m_sDB_PC_INF
		End Get
		Set(ByVal Value As String)
			m_sDB_PC_INF = Value
		End Set
	End Property
	''' <summary>
	''' PC�}�X�^�[�e�[�u�����̎擾�Ɛݒ�
	''' </summary>
	''' <value>PC�}�X�^�[�e�[�u����</value>
	''' <returns>PC�}�X�^�[�e�[�u����</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_PC_MASTER) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("PC_MASTER �̃e�[�u������")> _
	Public Property tProp_TableName_PcMaster() As String
		Get
			Return m_sDB_PC_M
		End Get
		Set(ByVal Value As String)
			m_sDB_PC_M = Value
		End Set
	End Property

	''' <summary>
	''' �@���ԃe�[�u�����̎擾�Ɛݒ�
	''' </summary>
	''' <value>�@���ԃe�[�u����</value>
	''' <returns>�@���ԃe�[�u����</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_STATUS_INF) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("STATUS_INF �̃e�[�u������")> _
	Public Property tProp_TableName_StatusInf() As String
		Get
			Return m_sDB_STATUS_I
		End Get
		Set(ByVal Value As String)
			m_sDB_STATUS_I = Value
		End Set
	End Property

	''' <summary>
	''' �@��}�X�^�[�e�[�u�����̎擾�Ɛݒ�
	''' </summary>
	''' <value>�@��}�X�^�[�e�[�u����</value>
	''' <returns>�@��}�X�^�[�e�[�u����</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_STATUS_MASTER) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("STATUS_MASTER �̃e�[�u������")> _
	Public Property tProp_TableName_StatusMaster() As String
		Get
			Return m_sDB_STATUS_M
		End Get
		Set(ByVal Value As String)
			m_sDB_STATUS_M = Value
		End Set
	End Property


	''' <summary>
	''' �r���萔�}�X�^�[�e�[�u�����̎擾�Ɛݒ�
	''' </summary>
	''' <value>�r���萔�}�X�^�[�e�[�u����</value>
	''' <returns>�r���萔�}�X�^�[�e�[�u����</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_COMMON_MASTER) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("COMMON_MASTER �̃e�[�u������")> _
	Public Property tProp_TableName_Com_Master() As String
		Get
			Return m_sDB_STATUS_M
		End Get
		Set(ByVal Value As String)
			m_sDB_STATUS_M = Value
		End Set
	End Property

	''' <summary>
	''' ����PC��PCKIND�l�̎擾�Ɛݒ�
	''' </summary>
	''' <value>����PC��PCKIND�l</value>
	''' <returns>����PC��PCKIND�l</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("����PC��PCKIND�l")> _
	Public Property tProp_TokatuID() As Integer
		Get
			Return m_nToukatu
		End Get
		Set(ByVal Value As Integer)
			m_nToukatu = Value
		End Set
	End Property

	''' <summary>
	''' �SPC���Z�b�g�pPCKIND�l�̎擾�Ɛݒ�
	''' </summary>
	''' <value>�SPC���Z�b�g�pPCKIND�l</value>
	''' <returns>�SPC���Z�b�g�pPCKIND�l</returns>
	''' <remarks>�ʏ�́A�~�ρB�~�ς����Ȃ��ꍇ�͓���</remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("�SPC���Z�b�g�p��PCKIND")> _
	Public Property tProp_AllPCRestID() As Integer
		Get
			Return m_nAllResetID
		End Get
		Set(ByVal Value As Integer)
			m_nAllResetID = Value
		End Set
	End Property

	''' <summary>
	''' ���O�\���p�f�[�^�O���b�h�r���[�̎擾�Ɛݒ�
	''' </summary>
	''' <value>���O�\���p�f�[�^�O���b�h�r���[</value>
	''' <returns>���O�\���p�f�[�^�O���b�h�r���[</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("���O�\���p�f�[�^�O���b�h�r���[")> _
	Public Property tProp_DspGrid() As DataGridView
		Get
			Return m_dgvLog
		End Get
		Set(ByVal Value As DataGridView)
			m_dgvLog = Value
		End Set
	End Property

	''' <summary>
	''' PC���Z�b�g�p�X���[�h�̎擾�Ɛݒ�
	''' </summary>
	''' <value>PC���Z�b�g�p�X���[�h</value>
	''' <returns>PC���Z�b�g�p�X���[�h</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("PC���Z�b�g�p�X���[�h")> _
	Public Property tProp_Pass() As String
		Get
			Return m_strResetPass
		End Get
		Set(ByVal Value As String)
			m_strResetPass = Value
		End Set
	End Property

	''' <summary>
	''' PC���Z�b�g�C�x���gNo�̎擾�Ɛݒ�
	''' </summary>
	''' <value>PC���Z�b�g�C�x���gNo</value>
	''' <returns>PC���Z�b�g�C�x���gNo</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("PC���Z�b�g�C�x���gNo")> _
	Public Property tProp_ResetEventNo() As Integer
		Get
			Return m_nEventReset
		End Get
		Set(ByVal Value As Integer)
			m_nEventReset = Value
		End Set
	End Property

	''' <summary>
	''' PC���O�I�t�C�x���gNo�̎擾�Ɛݒ�
	''' </summary>
	''' <value>PC���O�I�t�C�x���gNo</value>
	''' <returns>PC���O�I�t�C�x���gNo</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("PC���O�I�t�C�x���gNo")> _
	Public Property tProp_LogOffEventNo() As Integer
		Get
			Return m_nEventLogOff
		End Get
		Set(ByVal Value As Integer)
			m_nEventLogOff = Value
		End Set
	End Property

	''' <summary>
	''' �V�X�e���I���C�x���gNo�̎擾�Ɛݒ�
	''' </summary>
	''' <value>�V�X�e���I���C�x���gNo</value>
	''' <returns>�V�X�e���I���C�x���gNo</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("�V�X�e���I���C�x���gNo")> _
	Public Property tProp_SystemExitEventNo() As Integer
		Get
			Return m_nEventExit
		End Get
		Set(ByVal Value As Integer)
			m_nEventExit = Value
		End Set
	End Property

	''' <summary>
	''' ���C��ID�̎擾�Ɛݒ�
	''' </summary>
	''' <value>���C��ID</value>
	''' <returns>���C��ID</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("���C��ID")> _
	Public Property tProp_LineID() As Integer
		Get
			Return m_nLineID
		End Get
		Set(ByVal Value As Integer)
			m_nLineID = Value
		End Set
	End Property

	''' <summary>
	''' NOA�ڑ��t���O�̐ݒ�Ǝ擾
	''' </summary>
	''' <value>0:�I�����C�� 1:���u 9:�f�o�b�O</value>
	''' <returns>0:�I�����C�� 1:���u 9:�f�o�b�O</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("NOA�ڑ��t���O 0:�I�����C�� 1:���u 9:�f�o�b�O")> _
	Public Property tProp_Noa() As Integer
		Get
			Return m_nNoa
		End Get
		Set(ByVal Value As Integer)
			m_nNoa = Value
		End Set
	End Property

	''' <summary>
	''' RAS��ʕ\�����O���̐ݒ�Ǝ擾
	''' </summary>
	''' <value>�\�����O��</value>
	''' <returns>�\�����O��</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("RAS��ʕ\�����O��")> _
	Public Property tProp_LogDspCnt() As Integer
		Get
			Return m_nLogDspCnt
		End Get
		Set(ByVal Value As Integer)
			m_nLogDspCnt = Value
		End Set
	End Property

	''' <summary>
	''' �r���^�X�N�ݒ�t�@�C�����ڑ���DB�A�h���X�������ꂽ�Z�N�V�������̎擾�Ɛݒ�
	''' </summary>
	''' <value>DB�Z�N�V������</value>
	''' <returns>DB�Z�N�V������</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(INI_DB) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("�r���^�X�N�ݒ�t�@�C�����ڑ���DB�A�h���X�������ꂽ�Z�N�V������")> _
	Public Property tProp_DbSession() As String
		Get
			Return m_sDbSession
		End Get
		Set(ByVal Value As String)
			m_sDbSession = Value
		End Set
	End Property

	''' <summary>
	''' PC�}�X�^�[�Ǘ��v���Z�X���̎擾�Ɛݒ�
	''' </summary>
	''' <value>PC�}�X�^�[�Ǘ��v���Z�X��</value>
	''' <returns>PC�}�X�^�[�Ǘ��v���Z�X��</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(KS_MASTER) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("PC�}�X�^�[�Ǘ��v���Z�X��")> _
	Public Property tProp_MasterTask() As String
		Get
			Return m_sMasterTask
		End Get
		Set(ByVal Value As String)
			m_sMasterTask = Value
		End Set
	End Property

	''' <summary>
	''' PC�}�X�^�[�Ǘ����N������PC���̎擾�Ɛݒ�
	''' </summary>
	''' <value>PC�}�X�^�[�Ǘ����N������PC��</value>
	''' <returns>PC�}�X�^�[�Ǘ����N������PC��</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	, System.ComponentModel.DefaultValue(MASTER_PC_NAME) _
	, System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	, System.ComponentModel.Description("PC�}�X�^�[�Ǘ����N������PC��")> _
	 Public Property tProp_MasterPC() As String
		Get
			Return m_sMasterPC
		End Get
		Set(ByVal Value As String)
			m_sMasterPC = Value
		End Set
	End Property

	''' <summary>
	'''  PC�{�^���ɕ\������PC�@�\�̖��̋敪�̎擾�Ɛݒ�
	''' </summary>
	''' <value>0:�v���O�����ŗL 1:PC��ʖ�</value>
	''' <returns>0:�v���O�����ŗL 1:PC��ʖ�</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	, System.ComponentModel.DefaultValue(1) _
	, System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	, System.ComponentModel.Description("PC�{�^���ɕ\������PC�@�\�̖��̋敪 0:�v���O�����ŗL 1:PC��ʖ�")> _
	 Public Property tProp_PcKindName() As EM_DSP_PCKIND_NAME
		Get
			Return m_nDspPcKindName
		End Get
		Set(ByVal Value As EM_DSP_PCKIND_NAME)
			m_nDspPcKindName = Value
		End Set
	End Property

	''' <summary>
	'''  �p�\�R�����̂̎g�p�ݒ�(True:PC_INF��PC���,False:�v���O�����̐ݒ�)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks>���݂Ƃ̌݊��̂��ߎc��</remarks>
	Public Property tProp_PcKindNameFlg() As Boolean
		Get
			If tProp_PcKindName = EM_DSP_PCKIND_NAME.PCKIND Then
				Return True
			Else
				Return False
			End If
		End Get
		Set(ByVal Value As Boolean)
			If Value = True Then
				tProp_PcKindName = EM_DSP_PCKIND_NAME.PCKIND
			Else
				tProp_PcKindName = EM_DSP_PCKIND_NAME.PROGRAM
			End If
		End Set
	End Property

	''' <summary>
	''' PC�{�^���ɕ\������PC���̕\���敪�̎擾�Ɛݒ�
	''' </summary>
	''' <value>0:PC�� 1:�p�\�R����</value>
	''' <returns>0:PC�� 1:�p�\�R����</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	, System.ComponentModel.DefaultValue(1) _
	, System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	, System.ComponentModel.Description("PC�{�^���ɕ\������PC���̕\���敪�̎擾�Ɛݒ� 0:PC�� 1:�p�\�R����")> _
	 Public Property tProp_PcName() As EM_DSP_PC_NAME
		Get
			Return m_nDspPcName
		End Get
		Set(ByVal Value As EM_DSP_PC_NAME)
			m_nDspPcName = Value
		End Set
	End Property

	''' <summary>
	'''  �p�\�R�����̂̎g�p�ݒ�(True:PC_MASTER�̃p�\�R������,False:PC_MASTER��PC��)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Property tProp_PcNameFlg() As Boolean
		Get
			If m_nDspPcName = EM_DSP_PC_NAME.USER Then
				Return True
			Else
				Return False
			End If
		End Get
		Set(ByVal Value As Boolean)
			If Value = True Then
				m_nDspPcName = EM_DSP_PC_NAME.USER
			Else
				m_nDspPcName = EM_DSP_PC_NAME.SYSTEM
			End If
		End Set
	End Property


	''' <summary>
	''' PC���{�^���A�������o�^
	''' </summary>
	''' <param name="nPcKind">PCKIND</param>
	''' <param name="btnPc">PC�{�^���R���g���[��</param>
	''' <param name="lblPcShadow">PC�{�^���̉e���x��</param>
	''' <param name="strName">�{�^����</param>
	''' <param name="lblLine1">������x��</param>
	''' <param name="lblLine2">������x��</param>
	''' <param name="lblLine3">������x��</param>
	''' <param name="lblLine4">������x��</param>
	''' <param name="lblLine5">������x��</param>
	''' <param name="lblLine6">������x��</param>
	''' <param name="lblLine7">������x��</param>
	''' <param name="lblLine8">������x��</param>
	''' <param name="lblLine9">������x��</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddButtonPCName(ByVal nPcKind As Integer, ByVal btnPc As Button, _
	   ByVal lblPcShadow As Label, ByVal strName As String, _
	   Optional ByVal lblLine1 As Label = Nothing, _
	   Optional ByVal lblLine2 As Label = Nothing, Optional ByVal lblLine3 As Label = Nothing, _
	   Optional ByVal lblLine4 As Label = Nothing, Optional ByVal lblLine5 As Label = Nothing, _
	   Optional ByVal lblLine6 As Label = Nothing, Optional ByVal lblLine7 As Label = Nothing, _
	   Optional ByVal lblLine8 As Label = Nothing, Optional ByVal lblLine9 As Label = Nothing)

		Dim ctrl As New PC_CTRL
		With ctrl
			.initialize()														' PC�R���g���[����񏉊���

			.strName = strName													' PC���̓o�^
			.btnPcName = btnPc													' PC�{�^���o�^
			.lblShadow = lblPcShadow											' PC�{�^���̉e�o�^

			If Not lblLine1 Is Nothing Then .lblLine.Add(lblLine1)
			If Not lblLine2 Is Nothing Then .lblLine.Add(lblLine2)
			If Not lblLine3 Is Nothing Then .lblLine.Add(lblLine3)
			If Not lblLine4 Is Nothing Then .lblLine.Add(lblLine4)
			If Not lblLine5 Is Nothing Then .lblLine.Add(lblLine5)
			If Not lblLine6 Is Nothing Then .lblLine.Add(lblLine6)
			If Not lblLine7 Is Nothing Then .lblLine.Add(lblLine7)
			If Not lblLine8 Is Nothing Then .lblLine.Add(lblLine8)
			If Not lblLine9 Is Nothing Then .lblLine.Add(lblLine9)
		End With

		m_dctCtrlPC.Add(nPcKind, ctrl)											' PCKIND���L�[�ɂ��ă{�^���R���g���[����o�^
		AddHandler btnPc.Click, AddressOf btn_Click								' �N���b�N�C�x���g�n���h���ɒǉ�
	End Sub

	''' <summary>
	''' �@��ɑ΂���R���g���[���̓o�^ (�R���g���[���������̏ꍇ)
	''' </summary>
	''' <param name="nKikiID">�@��ID</param>
	''' <param name="lstData">�R���g���[��</param>
	''' <param name="nKikiType">�@��敪</param>
	''' <param name="nSetType">��Ԑݒ�敪</param>
	''' <param name="nDspType">�\���敪</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddControlKiki( _
	 ByVal nKikiID As Integer, _
	 ByVal lstData As List(Of Control), _
	 Optional ByVal nKikiType As EM_RAS_TYPE = EM_RAS_TYPE.KIKI, _
	 Optional ByVal nSetType As EM_RAS_SETTYPE = EM_RAS_SETTYPE.DSP_CONTROL, _
	 Optional ByVal nDspType As EM_RAS_DSPTYPE = EM_RAS_DSPTYPE.DSP_STRING)

		Dim ctrl As New KIKI_CTRL												' �@��\���R���g���[���쐬
		With ctrl
			.lstCtrl = lstData													' �@��\���R���g���[���ꗗ�o�^
			.nKikiType = nKikiType												' �@��敪
			.nSetType = nSetType												' ��Ԑݒ�敪
			.nDspType = nDspType												' �\���敪
		End With

		m_dctCtrlKiki.Add(nKikiID, ctrl)										' �@��\���R���g���[����o�^
	End Sub

	''' <summary>
	''' �@���ԂɕR�Â��R���g���[����o�^(�R���g���[���������̏ꍇ)
	''' </summary>
	''' <param name="nKikiID">PCKIND</param>
	''' <param name="lstData">�R���g���[��</param>
	''' <remarks>���ݗp�̃��b�p�֐�</remarks>
	Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal lstData As List(Of Control))
		AddControlKiki(nKikiID, lstData)
	End Sub

	''' <summary>
	''' �@��ɑ΂���R���g���[���̓o�^ (�R���g���[�����P�̏ꍇ)
	''' </summary>
	''' <param name="nKikiID">�@��ID</param>
	''' <param name="ctrl">�R���g���[��</param>
	''' <param name="nKikiType">�@��敪</param>
	''' <param name="nSetType">��Ԑݒ�敪</param>
	''' <param name="nDspType">�\���敪</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddControlKiki( _
	 ByVal nKikiID As Integer, _
	 ByVal ctrl As Control, _
	 Optional ByVal nKikiType As EM_RAS_TYPE = EM_RAS_TYPE.KIKI, _
	 Optional ByVal nSetType As EM_RAS_SETTYPE = EM_RAS_SETTYPE.DSP_CONTROL, _
	 Optional ByVal nDspType As EM_RAS_DSPTYPE = EM_RAS_DSPTYPE.DSP_STRING)

		Dim list As New List(Of Control)										' ���X�g�Ǘ��Ȃ̂Ń��X�g���쐬����
		list.Add(ctrl)

		AddControlKiki(nKikiID, list, nKikiType, nSetType, nDspType)			' �@��o�^
	End Sub

	''' <summary>
	''' �@���ԂɕR�Â��R���g���[����o�^(�R���g���[������ꍇ)
	''' </summary>
	''' <param name="nKikiID">PCKIND</param>
	''' <param name="ctlData">�R���g���[��</param>
	''' <remarks>���ׂ݂̈̃��b�p�֐�</remarks>
	Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal ctlData As Control)
		AddControlKiki(nKikiID, ctlData)
	End Sub

	''' <summary>
	''' �@��ɑ΂���R���g���[���̓o�^ (PC�p)
	''' </summary>
	''' <param name="nKikiID">�@��ID</param>
	''' <param name="bData">PC�t���O(True:PC�{�^���Ɉُ�t���O�𗧂Ă�,False:������x���Ɉُ�t���O�𗧂Ă�</param>
	''' <remarks>���ׂ݂̈Ɏc��</remarks>
	Protected Friend Overridable Sub AddControlKiki(ByVal nKikiID As Integer, ByVal bData As Boolean)
		Dim nSetType As EM_RAS_SETTYPE = EM_RAS_SETTYPE.DSP_CONTROL
		If bData = False Then
			nSetType = EM_RAS_SETTYPE.DSP_LINE
		End If

		'�@��o�^
		AddControlKiki(nKikiID, CType(Nothing, List(Of Control)), EM_RAS_TYPE.PC, nSetType, EM_RAS_DSPTYPE.DSP_STRING)
	End Sub

	''' <summary>
	''' �@���ԂɕR�Â��R���g���[����o�^
	''' </summary>
	''' <param name="nKikiID">PCKIND</param>
	''' <param name="bData">PC�t���O(True:PC�{�^���Ɉُ�t���O�𗧂Ă�,False:������x���Ɉُ�t���O�𗧂Ă�</param>
	''' <remarks>���ׂ݂̈̃��b�p�֐�</remarks>
	Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal bData As Boolean)
		AddControlKiki(nKikiID, bData)
	End Sub

	''' <summary>
	''' PC�{�^����PC����o�^
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetPCName()
		Dim tcls_DB As tClass.tClass_SQLServer = Nothing
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Try
			tcls_DB = New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
			If Not tcls_DB.Open() Then											' DB�I�[�v���B���s�����炠����߂�
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' PC���擾SQL�쐬
			'-------------------------------------------------------------------
			Dim strSQL As String = ""
			strSQL = ""
			strSQL &= " SELECT A.PCKIND,"
			strSQL &= "        B.PC��,"
			strSQL &= "        A.PC���,"
			strSQL &= "        B.�p�\�R������"
			strSQL &= " FROM " & m_sDB_PC_INF & " A " & "INNER JOIN " & m_sDB_PC_M & " B "
			strSQL &= " ON A.PCID = B.PCID "

			sqlRead = tcls_DB.SelectExecute(strSQL)								' PC�����c�a����擾����
			If sqlRead Is Nothing Then											' �擾���s���́A������߂�
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' PC���ݒ��PC�{�^�����̐ݒ�
			'-------------------------------------------------------------------
			m_dctPcInf.Clear()													' PC��񏉊���
			While sqlRead.Read()												' True�̊Ԃ̓f�[�^������s
				Dim nPcKind As Integer = sqlRead.GetInt32(0)					' PCKIND���擾

				If m_dctCtrlPC.ContainsKey(nPcKind) = True Then					' �o�^����Ă���PC�Ȃ�ݒ�

					Dim pc_inf As PC_INF = Nothing
					pc_inf.strNameSystem = sqlRead.GetString(1)
					pc_inf.strNameUser = sqlRead.GetString(3)

					Dim strBtnText As String = ""
					If m_nDspPcKindName = EM_DSP_PCKIND_NAME.PCKIND Then		' PC��ʂ�\������̂ł����PC��ʂ��擾
						strBtnText = sqlRead.GetString(2)
					Else														' �v���O�����ŗL�̋@�\����\��
						strBtnText = m_dctCtrlPC(nPcKind).strName
					End If

					strBtnText &= vbCrLf										' �@�\����PC���ŉ��s����

					If m_nDspPcName = EM_DSP_PC_NAME.USER Then					' �p�\�R������\������
						strBtnText &= pc_inf.strNameUser
					Else														' PC����\������
						strBtnText &= pc_inf.strNameSystem
					End If

					m_dctCtrlPC(nPcKind).btnPcName.Text = strBtnText			' �{�^�����\������ݒ�
					m_dctPcInf.Add(nPcKind, pc_inf)								' PC����ۑ�
				End If
			End While

		Catch ex As Exception
		Finally
			If Not sqlRead Is Nothing Then sqlRead.Close()
			If Not tcls_DB Is Nothing Then tcls_DB.Dispose()
		End Try
	End Sub

	''' <summary>
	''' �@����̐ݒ�
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetErr()
		Dim tcls_DB As tClass.tClass_SQLServer = Nothing
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Try
			tcls_DB = New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
			If Not tcls_DB.Open() Then											' DB�I�[�v���Ɏ��s�����炠����߂�
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' �@����擾SQL�̍쐬
			'-------------------------------------------------------------------
			Dim strSQL As String = ""
			strSQL &= " SELECT A.�@��ID,"
			strSQL &= "        ISNULL(C.PCKIND,0),"
			strSQL &= "        B.�@����,"
			strSQL &= "        A.�@��ُ�x,"
			strSQL &= "        A.�����ُ�x,"
			strSQL &= "        ISNULL(C.PC�敪,0)"
			strSQL &= " FROM " & m_sDB_STATUS_M & " A "
			strSQL &= "INNER JOIN " & m_sDB_STATUS_I & " B "
			strSQL &= " ON A.�@��ID = B.�@��ID "
			strSQL &= "LEFT OUTER JOIN " & m_sDB_PC_INF & " C "
			strSQL &= " ON A.PCID = C.PCID "

			sqlRead = tcls_DB.SelectExecute(strSQL)
			If sqlRead Is Nothing Then											' �@������擾
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' ���ɐݒ肳��Ă���@����̏�����
			'-------------------------------------------------------------------
			Dim typErrInf As STATUS_INF = Nothing
			typErrInf.sDspData = ""

			For ii As Integer = 0 To m_lstItem.Count - 1
				m_lstItem(ii).Tag = typErrInf
			Next ii

			'-------------------------------------------------------------------
			' �@����ݒ�
			'-------------------------------------------------------------------
			While sqlRead.Read()
				Dim nKikiID As Integer = CInt(sqlRead.GetValue(0))				' �@��ID
				Dim nKindID As Integer = CInt(sqlRead.GetValue(1))				' PCKIND
				Dim nStatus As Integer = CInt(sqlRead.GetValue(2))				' �@����
				Dim nKikiErr As Integer = CInt(sqlRead.GetValue(3))				' �@��ُ�x
				Dim nKensaErr As Integer = CInt(sqlRead.GetValue(4))			' �����ُ�x
				Dim nPcDiv As Integer = CInt(sqlRead.GetValue(5))				' PC�敪

				If m_dctCtrlKiki.ContainsKey(nKikiID) = False Then				' ���o�^�ɋ@��ID������Γǂݔ�΂�
					Continue While
				End If

				If 0 <> nKindID AndAlso m_dctCtrlPC.ContainsKey(nKindID) = False Then	' ���o�^��PCID������Γǂݔ�΂�
					Continue While
				End If

				Dim kiki_ctrl As KIKI_CTRL = m_dctCtrlKiki(nKikiID)
				If kiki_ctrl.nKikiType = EM_RAS_TYPE.PC Then					' PC�̏ꍇ
					If 0 <> nKindID Then										' PCKIND���L���̏ꍇ
						If kiki_ctrl.nSetType = EM_RAS_SETTYPE.DSP_CONTROL Then	' �R���g���[���ɒ��ڐݒ�
							SetStatusInf( _
							 m_dctCtrlPC(nKindID).btnPcName.Tag, _
							 nKikiID, _
							 nStatus, _
							 nKikiErr, _
							 nKensaErr, _
							 nPcDiv, _
							 nKindID)
						Else													' ���ɐݒ�
							For Each ctrl As Control In m_dctCtrlPC(nKindID).lblLine
								SetStatusInf( _
								 ctrl.Tag, _
								 nKikiID, _
								 nStatus, _
								 nKikiErr, _
								 nKensaErr, _
								 nPcDiv, _
								 nKindID)
							Next
						End If
					End If
				Else															' �@��̏ꍇ
					For Each ctrl As Control In m_dctCtrlKiki(nKikiID).lstCtrl
						SetStatusInf( _
						 ctrl.Tag, _
						 nKikiID, _
						 nStatus, _
						 nKikiErr, _
						 nKensaErr, _
						 nPcDiv)
					Next
				End If
			End While

		Catch ex As Exception
		Finally
			If Not sqlRead Is Nothing Then sqlRead.Close()
			If Not tcls_DB Is Nothing Then tcls_DB.Dispose()
		End Try
	End Sub
	''' <summary>
	''' �X�e�[�^�X�����Z�b�g
	''' </summary>
	''' <param name="objTag">�Ώۂ̃R���g���[����Tag</param>
	''' <param name="nKikiId">�@��ID</param>
	''' <param name="nStatus">�@����</param>
	''' <param name="nKikiErr">�@��ُ�x</param>
	''' <param name="nKendErr">�����ُ�x</param>
	''' <param name="nOffLine">PC�敪(0:�I�����C��,1:�I�t���C��)</param>
	''' <param name="nPcKind">PCKIND</param>
	''' <remarks></remarks>
	Private Sub SetStatusInf( _
	 ByRef objTag As Object, _
	 ByVal nKikiId As Integer, _
	 ByVal nStatus As Integer, _
	 ByVal nKikiErr As Integer, _
	 ByVal nKendErr As Integer, _
	 ByVal nOffLine As Integer, _
	 Optional ByVal nPcKind As Integer = 0)

		Dim inf As STATUS_INF = CType(objTag, STATUS_INF)

		Dim nDspType As EM_RAS_DSPTYPE = m_dctCtrlKiki(nKikiId).nDspType		' �\���敪�̎擾
		If nDspType = EM_RAS_DSPTYPE.DSP_VALUE _
		 OrElse (nDspType = EM_RAS_DSPTYPE.DSP_AUTO AndAlso nKikiErr = 0 AndAlso nKendErr = 0) Then	' �l��\������ꍇ
			inf.sDspData = CStr(nStatus)										' �\���l�ɒl��ݒ肷��
			inf.nKikiId = nKikiId
		Else																	' �������\������ꍇ
			If 0 <> nStatus And 0 = inf.nStatus Then							' ����->�ُ�֕ω�������X�e�[�^�X��ݒ�
				inf.nStatus = nStatus
				inf.nKikiId = nKikiId
			End If

			If 0 <> nStatus And inf.nKikiErr < nKikiErr Then					' ���[���ȋ@��ُ�ɂȂ����ꍇ�ɐݒ�
				inf.nKikiErr = nKikiErr
				inf.nKikiId = nKikiId
			End If
		End If

		inf.nOffLine = nOffLine													' PC�敪

		If 0 <> nPcKind Then													' PCKIND���L���ł���ΐݒ�
			inf.nPcKind = nPcKind
		End If
		' 2014.05.21 �|�R VRCL�p�S���ۑ��V�X�e���ǉ�     >>>>>>>>
		' �@���Ԃ�����łȂ��A�����ُ�x������ȊO�œo�^����Ă���Ȃ�A�����ُ�x���L������
		If 0 <> nStatus AndAlso 0 <> nKendErr AndAlso 0 = inf.nKendErr Then
			inf.nKendErr = nKendErr												' �����ُ�x
		End If
		' 2014.05.21 �|�R VRCL�p�S���ۑ��V�X�e���ǉ�     <<<<<<<<
		objTag = inf															' �֘A�t����ꂽ�^�O�֐ݒ�
	End Sub

	''' <summary>
	''' �X�e�[�^�X�擾
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub timGetStatus_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timGetStatus.Tick
		Call SetPCName()														' PC�{�^���Z�b�g
		Call SetErr()															' �X�e�[�^�X��Ԏ擾
		Call DspLog()															' ���O�\��
	End Sub

	''' <summary>
	''' ���O�\��
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub DspLog()
		Dim tcls_DB As tClass.tClass_SQLServer = Nothing
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing

		Try
			tcls_DB = New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
			If Not tcls_DB.Open() Then											' DB�I�[�v�Ɏ��s�����炠����߂�
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' ���O�擾SQL�쐬
			'-------------------------------------------------------------------
			Dim strSQL As String = ""
			strSQL &= " SELECT A.�ُ�MSG "
			strSQL &= " FROM " & m_sDB_STATUS_M & " A "
			strSQL &= " INNER JOIN " & m_sDB_STATUS_I & " B "
			strSQL &= " ON A.�@��ID = B.�@��ID "
			strSQL &= " LEFT OUTER JOIN " & m_sDB_PC_INF & " C "
			strSQL &= " ON A.PCID = C.PCID "
			strSQL &= " WHERE (A.�@��ُ�x <> 0 or A.�����ُ�x <> 0)"
			strSQL &= "  AND ISNULL(C.PC�敪,0) = 0 "
			strSQL &= "  AND B.�@���� <> 0"
			strSQL &= " ORDER BY ISNULL(A.�D��x, 1000) asc, A.�@��ُ�x desc "

			sqlRead = tcls_DB.SelectExecute(strSQL)
			If sqlRead Is Nothing Then											' SQL���s�Ɏ��s�����炠����߂�
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' ���O�\��
			'-------------------------------------------------------------------
			For ii As Integer = 0 To m_nLogDspCnt - 1							' ���O�\��������
				m_dgvLog(0, ii).Value = ""
			Next ii

			Dim nCnt As Integer = 0												' ���O�\������
			While sqlRead.Read()
				If m_nLogDspCnt - 1 < nCnt Then									' �\��MAX�𒴂�����I��
					Exit While
				End If

				m_dgvLog(0, nCnt).Value = sqlRead.GetString(0)					' �ُ�MSG��\��
				nCnt += 1
			End While
		Catch ex As Exception
		Finally
			If Not sqlRead Is Nothing Then sqlRead.Close()
			If Not tcls_DB Is Nothing Then tcls_DB.Dispose()
		End Try
	End Sub

	''' <summary>
	''' PC�{�^���N���b�N�C�x���g
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub btn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
		Dim strDspPcName As String	'���b�Z�[�W�ɕ\������PC��

		'2015.10.15 �a�� NOA_KAISEKI(-1) �ǉ�
		'If Not (NOA_ONLINE = m_nNoa Or NOA_DEBUG = m_nNoa) Then					' ���u���A�f�o�b�O���͉������Ȃ�
		If Not (NOA_ONLINE = m_nNoa Or NOA_DEBUG = m_nNoa Or -1 = m_nNoa) Then		' ���u���͉������Ȃ�
			Exit Sub
		End If

		'-----------------------------------------------------------------------
		' ���Z�b�gPC���擾
		'-----------------------------------------------------------------------
		Dim nPcKind As Integer = -1
		For Each key As Integer In m_dctCtrlPC.Keys								' �����ꂽ�{�^��������PC�R���g���[��������
			If m_dctCtrlPC(key).btnPcName.Equals(sender) = True Then
				nPcKind = key
				Exit For
			End If
		Next

		Dim strPcName As String = m_dctPcInf(nPcKind).strNameSystem				' PC���擾
		If m_nDspPcName = EM_DSP_PC_NAME.USER Then								' �\��PC���̎擾
			strDspPcName = m_dctPcInf(nPcKind).strNameUser
		Else
			strDspPcName = m_dctPcInf(nPcKind).strNameSystem
		End If

		'-----------------------------------------------------------------------
		' ���Z�b�g�m�F
		'-----------------------------------------------------------------------
		Dim nResetMode As Windows.Forms.DialogResult							' ���Z�b�g���[�h (Yes:���O�I�t No:�ċN�� Cancel:���~)
		nResetMode = ConfRest(nPcKind, strDspPcName)							' ���Z�b�g�m�F�_�C�A���O�\��
		If nResetMode = Windows.Forms.DialogResult.Cancel Then
			Exit Sub
		End If

		If Windows.Forms.DialogResult.OK <> ConfPass() Then						' �p�X���[�h�m�F�B����Ă���L�����Z��
			Exit Sub
		End If

		'-----------------------------------------------------------------------
		' ���Z�b�g�v�����M
		'-----------------------------------------------------------------------
		If nPcKind = m_nAllResetID Then											' �~�ς̏ꍇ�́A�SPC�̂���"*"�ɂ���
			strPcName = "*"														' 
		ElseIf nPcKind = m_nToukatu Then										' �����̏ꍇ�́A�}�X�^�[PC��
			strPcName = m_sMasterPC
		End If

		If nResetMode = Windows.Forms.DialogResult.Yes Then						' YES�Ȃ烍�O�I�t
			Call SendReset(m_nEventLogOff, strPcName)
		Else																	' No�Ȃ�ċN��
			Call SendReset(m_nEventReset, strPcName)
		End If
	End Sub

	''' <summary>
	''' ���Z�b�g�m�F
	''' </summary>
	''' <param name="nPcKind">PCKIND</param>
	''' <param name="strPcName">PC��</param>
	''' <param name="bStopFlg">��~�t���O(True:��~,False:��~�łȂ�)</param>
	''' <returns>���A���</returns>
	''' <remarks></remarks>
	Protected Friend Overridable Function ConfRest(Optional ByVal nPcKind As Integer = 0, Optional ByVal strPcName As String = "", Optional ByVal bStopFlg As Boolean = False) As DialogResult
		Dim strMsg As String													' �\�����b�Z�[�W 

		If nPcKind = m_nAllResetID Then											' �SPC���Z�b�g�̏ꍇ
			strMsg = strPcName & "�̃��Z�b�g���s����" & _
			   vbNewLine & "�S�Ă�PC�����Z�b�g����܂��B" & _
			   vbNewLine & "���s���܂����H"

		ElseIf nPcKind = m_nToukatu Then										' �����̏ꍇ
			strMsg = strPcName & "�̃��Z�b�g���s����" & _
			   vbNewLine & "�~��PC�������S�Ă�PC�����Z�b�g����܂��B" & _
			   vbNewLine & "���s���܂����H"
		Else																	' �����ȊO�̏ꍇ
			strMsg = strPcName & "�̃��Z�b�g�����s���܂����H"
		End If

		' ���ʃ��b�Z�[�W
		strMsg &= vbCrLf & vbCrLf
		strMsg &= "�͂�   �F �p�\�R���̃��O�I�t�����s" & vbCrLf
		strMsg &= "������ �F �p�\�R���̍ċN�������s" & vbCrLf
		strMsg &= "��ݾ�  �F �I�����L�����Z��" & vbCrLf
		strMsg &= "   (�ʏ�A�u�͂��B���O�I�t�v��I��)"

		Dim frmMsg As New tClassForm.frmPop_MsgBox3(strMsg, "�m�F", , True, True, True, DialogResult.Yes, DialogResult.No, DialogResult.Cancel)
		Dim bRet As DialogResult = frmMsg.ShowDialog()
		Return bRet
	End Function
	''' <summary>
	''' �V�X�e���I���m�F
	''' </summary>
	''' <param name="bStopFlg">��~�t���O(True:��~,False:��~�łȂ�)</param>
	''' <returns>���A���</returns>
	''' <remarks></remarks>
	Protected Friend Overridable Function ConfShatdown(Optional ByVal bStopFlg As Boolean = False) As DialogResult
		Dim strMsg As String	'���b�Z�[�W 

		If bStopFlg Then														' PC�����Ȃ��ꍇ�́A�V�X�e���I��
			strMsg = "�SPC�̃V���b�g�_�E�������s���܂����H"
		Else
			strMsg = "�V�X�e�����ғ����ł��B" & _
			vbNewLine & "����ł��A�SPC�̃V���b�g�_�E�������s���܂����H"
		End If

		Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		Dim bRet As DialogResult = frmMsg.ShowDialog()
		Return bRet
	End Function

	''' <summary>
	''' �p�X���[�h�m�F
	''' </summary>
	''' <returns>���A���</returns>
	''' <remarks></remarks>
    Protected Friend Overridable Function ConfPass() As DialogResult
        ' �p�X���[�h���ݒ�̏ꍇ
        If "" = m_strResetPass Then
            Return DialogResult.OK
        End If

        Dim frmPass As New tClassForm.frmPop_Pass(m_strResetPass)
        Dim bRet As DialogResult = frmPass.ShowDialog()                         ' �p�X���[�h�m�F
        Return bRet
    End Function

	''' <summary>
	''' PC���Z�b�gor�V�X�e���I�����[�����M
	''' </summary>
	''' <param name="nEventNo">�C�x���gNo</param>
	''' <param name="strPcName">PC��</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SendReset(ByVal nEventNo As Integer, Optional ByVal strPcName As String = "")
		Dim que As KizuLibCLI.COMMON_QUE_STR = Nothing

		que.nEventNo = nEventNo													' PC���Z�b�g
		que.nLineNo = m_nLineID
		que.data = strPcName

		Call KizuLibCLI.KizuMod.Send_Mail(m_sMasterTask, m_sMasterPC, que)		' ���[�����M 
	End Sub

	''' <summary>
	''' �V�X�e���I������
    ''' <param name="bStopFlg">��~���t���O(True:��~��,False:����ȊO)</param>
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExitSystem(ByVal bStopFlg As Boolean)
		If Windows.Forms.DialogResult.OK <> ConfShatdown(bStopFlg) Then			' ���Z�b�g�m�F
			Exit Sub
		End If

		If Windows.Forms.DialogResult.OK <> ConfPass() Then						' �p�X���[�h�m�F
			Exit Sub
		End If
		Call SendReset(m_nEventExit)											' �V�X�e���I�����[�����M
	End Sub
End Class
