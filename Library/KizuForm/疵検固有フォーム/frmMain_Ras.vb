'*********************************************************************************
' RAS��ʂ̐e�N���X
'	[Ver]
'		Ver.01    2007/08/23  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' RAS��ʂ̐e�N���X
''' </summary>
''' <remarks></remarks>
Public Class frmMain_Ras

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �\����
    '/ /////////////////////////////////////////////////////////////////////////////////

    'PCKIND�ɕR�Â����
    Public Structure PC_CTRL
        Dim strName As String           '�{�^����
        Dim btnPcName As Button         'PC�{�^��
        Dim lblShadow As Label          'PC�{�^���e
        Dim lblLine As List(Of Label)   '������x��

        '�C�j�V�����C�Y
        Public Sub initialize()
            lblLine = New List(Of Label)
        End Sub
    End Structure

    '�@��ID�ɕR�Â����
	Public Structure STATUS_INF
		Dim nKikiId As Integer		'�@��ID
		Dim nStatus As Integer		'�@����
		Dim nKikiErr As Integer		'�@��ُ�x
		Dim sDspData As String		'��ʕ\���f�[�^
		Dim nPcKind As Integer		'PCKIND
		Dim nOffLine As Integer		'PC�敪
		' 2014.05.21 �|�R VRCL�p�S���ۑ��V�X�e���ǉ�     >>>>>>>>
		Dim nKendErr As Integer		'�����ُ�x
		' 2014.05.21 �|�R VRCL�p�S���ۑ��V�X�e���ǉ�     <<<<<<<<
	End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o �ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_hasBtnPcName As New Hashtable         'PC�{�^���R�t���p
    Private m_hasKikiName As New Hashtable          '�@���ԕR�t���p
    Private m_dctPcName As New Dictionary(Of Integer, String)   'PCKIND�ɕR�Â�PC����o�^
    Private m_dctPcNameJis As New Dictionary(Of Integer, String) 'PCKIND�ɕR�Â�PC���̂�o�^

    Private m_sDB_PC_I As String = DB_PC_INF                    ' DB�e�[�u�� PC���
    Private m_sDB_PC_M As String = DB_PC_MASTER                 ' DB�e�[�u�� PC�}�X�^�[
    Private m_sDB_STATUS_I As String = DB_STATUS_INF            ' DB�e�[�u�� �@����
    Private m_sDB_STATUS_M As String = DB_STATUS_MASTER         ' DB�e�[�u�� �@���ԃ}�X�^�[
    Private m_sDB_COMMON As String = DB_COMMON_MASTER           ' DB�e�[�u�� �r���萔�}�X�^�[
    Private m_nToukatu As Integer                               ' ������PCKINDID���Z�b�g
    Private m_nAllResetID As Integer                            ' �SPC�̃��Z�b�g����PCKIND���Z�b�g(�~��,�~�ςƓ������ꏏ�̏ꍇ�͓���)
    Private m_nLogDspCnt As Integer                             ' ���O�\����
    Private m_dgvData As DataGridView                           ' ���O�\���O���b�h
    Private m_sPass As String = ""                              ' �p�X���[�h
    Private m_nSYSTEM_EXIT As Integer = FACT_KS_MASTER_06       ' �V�X�e���I��
	Private m_nRESET_EVENT As Integer = FACT_KS_MASTER_07		' PC�ċN���̃C�x���g�ԍ�
	Private m_nLOGOFF_EVENT As Integer = FACT_KS_MASTER_08		' PC���O�I�t�̃C�x���g�ԍ�
    Private m_nLineID As Integer = LINE_ID                      ' ���C��ID
	Private m_nNoa As Integer									' NOA�ڑ��t���O (0:�I�����C��  1:���u)
	Private m_sDbSession As String = INI_DB						' DB�Z�b�V����
	Private m_sMasterTask As String = KS_MASTER					' �}�X�^�[�^�X�N
    Private m_sMasterPC As String = MASTER_PC_NAME              ' �}�X�^�[PC(KS_MASTER���N������PC)
    Private m_bPcKindNameFlg As Boolean = True                  ' PC_MASTER��PC��ʂ��g�����A�v���O�����ŕ\�������邩�̃t���O(True:PC���,False:�v���O����)
    Private m_bPcNameFlg As Boolean = True                      ' �p�\�R�����̂�PC_MASTER��PC�����g�����A�p�\�R�����̂��g�����̃t���O(True:�p�\�R������,False:PC��)
    Protected Friend m_lstItem As New List(Of Object)           ' �F�\������R���g���[���̃��X�g

    ''' <summary>
    ''' PC_INF �̃e�[�u������
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
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
    ''' PC_MASTER �̃e�[�u������
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' STATUS_INF �̃e�[�u������
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' STATUS_MASTER �̃e�[�u������
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' COMMON_MASTER �̃e�[�u������
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' �SPC���Z�b�g�p��PCKIND���Z�b�g(�ʏ�́A�~�ρB�~�ς����Ȃ��ꍇ�͓���)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("�SPC���Z�b�g�p��PCKIND")> _
    Public Property tProp_TokatuID() As Integer
        Get
            Return m_nToukatu
        End Get
        Set(ByVal Value As Integer)
            m_nToukatu = Value
        End Set
    End Property

    ''' <summary>
    ''' �SPC���Z�b�g�p��PCKIND���Z�b�g(�ʏ�́A�~�ρB�~�ς����Ȃ��ꍇ�͓���)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
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
    ''' ���O�̃f�[�^�O���b�h���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("�f�[�^�O���b�h")> _
    Public Property tProp_DspGrid() As DataGridView
        Get
            Return m_dgvData
        End Get
        Set(ByVal Value As DataGridView)
            m_dgvData = Value
        End Set
    End Property

    ''' <summary>
    ''' PC���Z�b�g�Ɏg�p����p�X���[�h���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("�p�X���[�h")> _
    Public Property tProp_Pass() As String
        Get
            Return m_sPass
        End Get
        Set(ByVal Value As String)
            m_sPass = Value
        End Set
    End Property


    ''' <summary>
    ''' PC���Z�b�g�̃C�x���gNo���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("PC���Z�b�g�̃C�x���gNo")> _
    Public Property tProp_ResetEventNo() As Integer
        Get
            Return m_nRESET_EVENT
        End Get
        Set(ByVal Value As Integer)
            m_nRESET_EVENT = Value
        End Set
    End Property

	''' <summary>
	''' PC���O�I�t�̃C�x���gNo���Z�b�g
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("PC���O�I�t�̃C�x���gNo")> _
	Public Property tProp_LogOffEventNo() As Integer
		Get
			Return m_nLOGOFF_EVENT
		End Get
		Set(ByVal Value As Integer)
			m_nLOGOFF_EVENT = Value
		End Set
	End Property

    ''' <summary>
    ''' �V�X�e���I���̃C�x���gNo���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("�V�X�e���I���̃C�x���gNo")> _
    Public Property tProp_SystemExitEventNo() As Integer
        Get
            Return m_nSYSTEM_EXIT
        End Get
        Set(ByVal Value As Integer)
            m_nSYSTEM_EXIT = Value
        End Set
    End Property

    ''' <summary>
    ''' ���C��ID���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' NOA�ڑ��t���O���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("NOA�ڑ��t���O")> _
    Public Property tProp_Noa() As Integer
        Get
            Return m_nNoa
        End Get
        Set(ByVal Value As Integer)
            m_nNoa = Value
        End Set
    End Property

    ''' <summary>
    ''' RAS��ʕ\�����O�����Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("RAS��ʕ\�����O�����Z�b�g")> _
    Public Property tProp_LogDspCnt() As Integer
        Get
            Return m_nLogDspCnt
        End Get
        Set(ByVal Value As Integer)
            m_nLogDspCnt = Value
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
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
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
	''' �}�X�^�[�^�X�N
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(KS_MASTER) _
	 , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("�}�X�^�[�^�X�N")> _
	Public Property tProp_MasterTask() As String
		Get
			Return m_sMasterTask
		End Get
		Set(ByVal Value As String)
			m_sMasterTask = Value
		End Set
	End Property

	''' <summary>
	''' �}�X�^�[PC(KS_MASTER)���N������PC
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	, System.ComponentModel.DefaultValue(MASTER_PC_NAME) _
	, System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
	, System.ComponentModel.Description("�}�X�^�[PC")> _
	 Public Property tProp_MasterPC() As String
		Get
			Return m_sMasterPC
		End Get
		Set(ByVal Value As String)
			m_sMasterPC = Value
		End Set
	End Property

    ''' <summary>
    '''  �p�\�R�����̂̎g�p�ݒ�(True:PC_INF��PC���,False:�v���O�����̐ݒ�)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
    , System.ComponentModel.DefaultValue(True) _
    , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
    , System.ComponentModel.Description("�}�X�^�[PC")> _
     Public Property tProp_PcKindNameFlg() As Boolean
        Get
            Return m_bPcKindNameFlg
        End Get
        Set(ByVal Value As Boolean)
            m_bPcKindNameFlg = Value
        End Set
    End Property

    ''' <summary>
    '''  �p�\�R�����̂̎g�p�ݒ�(True:PC_MASTER�̃p�\�R������,False:PC_MASTER��PC��)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
    , System.ComponentModel.DefaultValue(True) _
    , System.ComponentModel.Category("��RAS��ʐ�p�v���p�e�B") _
    , System.ComponentModel.Description("�}�X�^�[PC")> _
     Public Property tProp_PcNameFlg() As Boolean
        Get
            Return m_bPcNameFlg
        End Get
        Set(ByVal Value As Boolean)
            m_bPcNameFlg = Value
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

        Dim typData As New PC_CTRL

        '�C�j�V�����C�Y
        typData.initialize()

        'PC�{�^����PC�̉e���x����o�^
        typData.strName = strName
        typData.btnPcName = btnPc
        typData.lblShadow = lblPcShadow

        '������x����o�^
        If Not lblLine1 Is Nothing Then typData.lblLine.Add(lblLine1)
        If Not lblLine2 Is Nothing Then typData.lblLine.Add(lblLine2)
        If Not lblLine3 Is Nothing Then typData.lblLine.Add(lblLine3)
        If Not lblLine4 Is Nothing Then typData.lblLine.Add(lblLine4)
        If Not lblLine5 Is Nothing Then typData.lblLine.Add(lblLine5)
        If Not lblLine6 Is Nothing Then typData.lblLine.Add(lblLine6)
        If Not lblLine7 Is Nothing Then typData.lblLine.Add(lblLine7)
        If Not lblLine8 Is Nothing Then typData.lblLine.Add(lblLine8)
        If Not lblLine9 Is Nothing Then typData.lblLine.Add(lblLine9)

        'PCKIND���L�[�ɂ��ă{�^���R���g���[����o�^
        m_hasBtnPcName.Add(nPcKind, typData)

        '' ���łɃN���b�N�C�x���g�n���h���ɒǉ�
        AddHandler btnPc.Click, AddressOf btn_Click


    End Sub
    ''' <summary>
    ''' �@���ԂɕR�Â��R���g���[����o�^(�R���g���[���������̏ꍇ)
    ''' </summary>
    ''' <param name="nKikiID">PCKIND</param>
    ''' <param name="lstData">�R���g���[��</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal lstData As List(Of Control))

        'PCKIND���L�[�ɂ��ă{�^���R���g���[����o�^
        m_hasKikiName.Add(nKikiID, lstData)

    End Sub

    ''' <summary>
    ''' �@���ԂɕR�Â��R���g���[����o�^(�R���g���[������ꍇ)
    ''' </summary>
    ''' <param name="nKikiID">PCKIND</param>
    ''' <param name="ctlData">�R���g���[��</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal ctlData As Control)

        Dim lstData As New List(Of Control)

        lstData.Add(ctlData)

        'PCKIND���L�[�ɂ��ă{�^���R���g���[����o�^
        m_hasKikiName.Add(nKikiID, lstData)

    End Sub

    ''' <summary>
    ''' �@���ԂɕR�Â��R���g���[����o�^
    ''' </summary>
    ''' <param name="nKikiID">PCKIND</param>
    ''' <param name="bData">PC�t���O(True:PC�{�^���Ɉُ�t���O�𗧂Ă�,False:������x���Ɉُ�t���O�𗧂Ă�</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal bData As Boolean)


        'PCKIND���L�[�ɂ��ă{�^���R���g���[����o�^
        m_hasKikiName.Add(nKikiID, bData)

    End Sub

    ''' <summary>
    ''' PC�{�^����PC����o�^
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetPCName()

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim nPcKind As Integer
        Dim strName As String


        'DB�I�[�v��
        If Not tcls_DB.Open() Then Exit Sub

        'SQL���쐬
        strSQL = ""
        strSQL &= " SELECT A.PCKIND,"
        strSQL &= "        B.PC��,"
        strSQL &= "        A.PC���,"
        strSQL &= "        B.�p�\�R������"
        strSQL &= " FROM " & m_sDB_PC_I & " A " & "INNER JOIN " & m_sDB_PC_M & " B "
        strSQL &= " ON A.PCID = B.PCID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            m_dctPcName.Clear()
            m_dctPcNameJis.Clear()

            ' �f�[�^�\��
            While sqlRead.Read()

                nPcKind = sqlRead.GetInt32(0)   'PCKIND

                'PCKIND���L�[��PC����ݒ�
                If m_hasBtnPcName.Contains(nPcKind) Then

                    If m_bPcKindNameFlg Then
                        'PC��ʂ�\��
                        strName = sqlRead.GetString(2)
                    Else
                        '�v���O�����̐ݒ��\��
                        strName = CType(m_hasBtnPcName(nPcKind), PC_CTRL).strName
                    End If

                    strName &= vbCrLf

                    If m_bPcNameFlg Then

                        '�p�\�R�����̂�\��
                        strName &= sqlRead.GetString(3)
                    Else

                        'PC����\��
                        strName &= sqlRead.GetString(1)
                    End If


                    CType(m_hasBtnPcName(nPcKind), PC_CTRL).btnPcName.Text = strName

                    'PCKIND�ɕR�Â�PC����o�^
                    m_dctPcName.Add(nPcKind, sqlRead.GetString(1))

                    'PCKIND�ɕR�Â�PC���̂�o�^
                    m_dctPcNameJis.Add(nPcKind, sqlRead.GetString(3))
                End If

            End While

        Catch ex As Exception

        Finally

            ' DB�J��
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

        End Try

    End Sub

    ''' <summary>
    ''' �G���[���Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetErr()

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim typErrInf As STATUS_INF = Nothing

        Dim objData As Control      '���[�N�R���g���[��
        Dim nKikiID As Integer      '�@��ID
        Dim nKindID As Integer      'PCKIND
        Dim nStatus As Integer      '�@����
        Dim nKikiErr As Integer     '�@��ُ�x
        Dim nKensaErr As Integer    '�����ُ�x
        Dim nPcDiv As Integer       'PC�敪
        Dim ii As Integer           '�J�E���^

        'DB�I�[�v��
        If Not tcls_DB.Open() Then Exit Sub

        'SQL���쐬
        strSQL = ""
        strSQL &= " SELECT A.�@��ID,"
        strSQL &= "        ISNULL(C.PCKIND,0),"
        strSQL &= "        B.�@����,"
        strSQL &= "        A.�@��ُ�x,"
        strSQL &= "        A.�����ُ�x,"
        strSQL &= "        ISNULL(C.PC�敪,0)"
        strSQL &= " FROM " & m_sDB_STATUS_M & " A "
        strSQL &= "INNER JOIN " & m_sDB_STATUS_I & " B "
        strSQL &= " ON A.�@��ID = B.�@��ID "
        strSQL &= "LEFT OUTER JOIN " & m_sDB_PC_I & " C "
        strSQL &= " ON A.PCID = C.PCID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

			'�R���g���[����Tag����������
			typErrInf.nKikiId = 0
            typErrInf.nStatus = 0
            typErrInf.nOffLine = 0
            typErrInf.sDspData = ""
            typErrInf.nKikiErr = 0

            For ii = 0 To m_lstItem.Count - 1
                CType(m_lstItem(ii), Control).Tag = typErrInf
            Next ii

            ' �f�[�^�擾�\��
            While sqlRead.Read()

                nKikiID = CInt(sqlRead.GetValue(0)) '�@��ID
                nKindID = CInt(sqlRead.GetValue(1)) 'PCKIND
                nStatus = CInt(sqlRead.GetValue(2))  '�@����
                nKikiErr = CInt(sqlRead.GetValue(3)) '�@��ُ�x
                nKensaErr = CInt(sqlRead.GetValue(4))   '�����ُ�x
                nPcDiv = CInt(sqlRead.GetValue(5))  'PC�敪

				'�L�[�����݂��Ȃ��ꍇ�́A��΂�
                If Not m_hasKikiName.ContainsKey(nKikiID) Then Continue While

                '�L�[�ɕR�Â��f�[�^���������`�F�b�N
                If IsNumeric(m_hasKikiName(nKikiID)) Then

                    If 0 <> nKindID Then

                        If CType(m_hasKikiName(nKikiID), Boolean) Then
                            '�����̏ꍇ�́APCKIND����R�Â��{�^����ݒ�

                            '�ُ���Z�b�g
							Call SetStatusInf(CType(m_hasBtnPcName(nKindID), PC_CTRL).btnPcName.Tag, nKikiID, nStatus, _
								 nKikiErr, nKensaErr, nPcDiv, nKindID)


                        Else

                            '�����̏ꍇ�́APCKIND����R�Â����x����ݒ�
                            '����֌W�̃��x����\������
                            For ii = 0 To CType(m_hasBtnPcName(nKindID), PC_CTRL).lblLine.Count - 1

                                '�ُ���Z�b�g
								Call SetStatusInf(CType(m_hasBtnPcName(nKindID), PC_CTRL).lblLine(ii).Tag, nKikiID, nStatus, _
									nKikiErr, nKensaErr, nPcDiv, nKindID)


                            Next ii
                        End If
                    End If

                Else

                    '�Y������R���g���[�����Z�b�g
                    For Each objData In CType(m_hasKikiName(nKikiID), List(Of Control))

                        '�ُ���Z�b�g
						Call SetStatusInf(CType(objData, Control).Tag, nKikiID, nStatus, _
											nKikiErr, nKensaErr, nPcDiv)
                    Next

                End If

            End While

        Catch ex As Exception

        Finally
            ' DB�J��
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()


        End Try


    End Sub
    ''' <summary>
    ''' �X�e�[�^�X�����Z�b�g
    ''' </summary>
	''' <param name="objData">�Ώۂ̃R���g���[����Tag</param>
	''' <param name="nKikiId">�@��ID</param>
    ''' <param name="nStatus">�@����</param>
    ''' <param name="nKikiErr">�@��ُ�x</param>
    ''' <param name="nKendErr">�����ُ�x</param>
    ''' <param name="nOffLine">PC�敪(0:�I�����C��,1:�I�t���C��)</param>
    ''' <param name="nPcKind">PCKIND</param>
    ''' <remarks></remarks>
	Private Sub SetStatusInf(ByRef objData As Object, ByVal nKikiId As Integer, ByVal nStatus As Integer, _
		   ByVal nKikiErr As Integer, ByVal nKendErr As Integer, _
		   ByVal nOffLine As Integer, Optional ByVal nPcKind As Integer = 0)

		Dim typWkInf As STATUS_INF = Nothing

		typWkInf = CType(objData, STATUS_INF)

		If 0 = nKikiErr And 0 = nKendErr Then
			'�@��ُ�x��0�̏ꍇ�́A�@���Ԃ�\���l�ɃZ�b�g
			typWkInf.sDspData = CStr(nStatus)
			typWkInf.nKikiId = nKikiId
		Else

			'���݂̏�Ԃ��ُ�ňُ�̃f�[�^���Z�b�g����Ă��Ȃ��ꍇ�̂݃Z�b�g
			If 0 <> nStatus And 0 = typWkInf.nStatus Then
				typWkInf.nStatus = nStatus
				typWkInf.nKikiId = nKikiId
			End If

			'���݂̋@��ُ�x���������ُ�x�Ȃ�Z�b�g
			If 0 <> nStatus And typWkInf.nKikiErr < nKikiErr Then
				typWkInf.nKikiErr = nKikiErr
				typWkInf.nKikiId = nKikiId
			End If

		End If

		typWkInf.nOffLine = nOffLine	'PC�敪

		If 0 <> nPcKind Then
			typWkInf.nPcKind = nPcKind		'PCKIND
		End If
		' 2014.05.21 �|�R VRCL�p�S���ۑ��V�X�e���ǉ�     >>>>>>>>
		' �@���Ԃ�����łȂ��A�����ُ�x������ȊO�œo�^����Ă���Ȃ�A�����ُ�x���L������
		If 0 <> nStatus AndAlso 0 <> nKendErr AndAlso 0 = typWkInf.nKendErr Then
			typWkInf.nKendErr = nKendErr										' �����ُ�x
		End If
		' 2014.05.21 �|�R VRCL�p�S���ۑ��V�X�e���ǉ�     <<<<<<<<

		'�f�[�^���Z�b�g
		objData = typWkInf

	End Sub


    ''' <summary>
    ''' �X�e�[�^�X�擾
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
	Protected Friend Overridable Sub timGetStatus_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timGetStatus.Tick

		'PC�{�^���Z�b�g
		Call SetPCName()

		'�X�e�[�^�X��Ԏ擾
		Call SetErr()

		'���O�\��
		Call DspLog()

	End Sub

    ''' <summary>
    ''' ���O�\��
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub DspLog()


		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim nCnt As Integer  '���O�\����
        Dim ii As Integer

        'DB�I�[�v��
        If Not tcls_DB.Open() Then Exit Sub

        'SQL���쐬
        strSQL = ""
        strSQL &= " SELECT A.�ُ�MSG "
        strSQL &= " FROM " & m_sDB_STATUS_M & " A "
        strSQL &= " INNER JOIN " & m_sDB_STATUS_I & " B "
        strSQL &= " ON A.�@��ID = B.�@��ID "
        strSQL &= " LEFT OUTER JOIN " & m_sDB_PC_I & " C "
        strSQL &= " ON A.PCID = C.PCID "
        strSQL &= " WHERE (A.�@��ُ�x <> 0 or A.�����ُ�x <> 0)"
        strSQL &= "  AND ISNULL(C.PC�敪,0) = 0 "
        strSQL &= "  AND B.�@���� <> 0"
        strSQL &= " ORDER BY ISNULL(A.�D��x, 1000) asc, A.�@��ُ�x desc "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            '�O���b�h������
            For ii = 0 To m_nLogDspCnt - 1
                m_dgvData(0, ii).Value = ""
            Next ii
            nCnt = 0
            ' �f�[�^�擾�\��
            While sqlRead.Read()

                '�\��MAX�𒴂�����I��
                If m_nLogDspCnt - 1 < nCnt Then Exit While

                '�ُ�MSG��\��
                m_dgvData(0, nCnt).Value = sqlRead.GetString(0)

                nCnt += 1

            End While


        Catch ex As Exception

        Finally
            ' DB�J��
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

        End Try

    End Sub

    ''' <summary>
    ''' PC�{�^���N���b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
	Protected Friend Overridable Sub btn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

		Dim typStatus As STATUS_INF
		Dim nPcKind As Integer	'PCKIND
		Dim strPcName As String	  'PC��
		Dim strDspPcName As String	'���b�Z�[�W�ɕ\������PC��

		'���u���́A�����Ȃ�
		'2015.10.15 �a�� NOA_KAISEKI(-1) �ǉ�
		'If Not (NOA_ONLINE = m_nNoa Or NOA_DEBUG = m_nNoa) Then Exit Sub
		If Not (NOA_ONLINE = m_nNoa Or NOA_DEBUG = m_nNoa Or -1 = m_nNoa) Then Exit Sub ' ���u���A�f�o�b�O���͉������Ȃ�

		'PC�����擾
		typStatus = CType(CType(sender, Button).Tag, STATUS_INF)

		'PCKIND�擾
		nPcKind = typStatus.nPcKind

		'�Y��PC�����Ȃ���΁A�I��
		If Not m_dctPcName.ContainsKey(nPcKind) Then
			Exit Sub
		End If

		'PC���擾
		strPcName = m_dctPcName(nPcKind)

		'�\��PC���̎擾
		If m_bPcNameFlg Then
			strDspPcName = m_dctPcNameJis(nPcKind)
		Else
			strDspPcName = m_dctPcName(nPcKind)
		End If

		'���Z�b�g�m�F
		Dim nResetMode As Windows.Forms.DialogResult		' ���Z�b�g���[�h (Yes:���O�I�t No:�ċN�� Cancel:���~)
		nResetMode = ConfRest(nPcKind, strDspPcName)
		If nResetMode = Windows.Forms.DialogResult.Cancel Then
			Exit Sub
		End If

		'�p�X���[�h�m�F
		If Windows.Forms.DialogResult.OK <> ConfPass() Then
			Exit Sub
		End If

		'���Z�b�g�v�����M
		If nPcKind = m_nAllResetID Then

			'�~�ς̏ꍇ�́A�SPC�̂���"*"�ɂ���
			strPcName = "*"

		ElseIf nPcKind = m_nToukatu Then

			'�����̏ꍇ�́A�}�X�^�[PC��
			strPcName = m_sMasterPC

		End If

		If nResetMode = Windows.Forms.DialogResult.Yes Then
			Call SendReset(m_nLOGOFF_EVENT, strPcName)
		Else
			Call SendReset(m_nRESET_EVENT, strPcName)
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


		Dim strMsg As String	'���b�Z�[�W 
		Dim bRet As DialogResult


		If nPcKind = m_nAllResetID Then

			'�SPC���Z�b�g�̏ꍇ
			strMsg = strPcName & "�̃��Z�b�g���s����" & _
			   vbNewLine & "�S�Ă�PC�����Z�b�g����܂��B" & _
			   vbNewLine & "���s���܂����H"

		ElseIf nPcKind = m_nToukatu Then

			strMsg = strPcName & "�̃��Z�b�g���s����" & _
			   vbNewLine & "�~��PC�������S�Ă�PC�����Z�b�g����܂��B" & _
			   vbNewLine & "���s���܂����H"
		Else

			'�����łȂ�
			strMsg = strPcName & "�̃��Z�b�g�����s���܂����H"

		End If

		' ���ʃ��b�Z�[�W
		strMsg &= vbCrLf & vbCrLf
		strMsg &= "�͂�   �F �p�\�R���̃��O�I�t�����s" & vbCrLf
		strMsg &= "������ �F �p�\�R���̍ċN�������s" & vbCrLf
		strMsg &= "��ݾ�  �F �I�����L�����Z��" & vbCrLf
		strMsg &= "   (�ʏ�A�u�͂��B���O�I�t�v��I��)"

		Dim frmMsg As New tClassForm.frmPop_MsgBox3(strMsg, "�m�F", , True, True, True, Windows.Forms.DialogResult.Yes, Windows.Forms.DialogResult.No, Windows.Forms.DialogResult.Cancel)

		bRet = frmMsg.ShowDialog()

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
		Dim bRet As DialogResult


		'PC�����Ȃ��ꍇ�́A�V�X�e���I��
		If bStopFlg Then
			strMsg = "�SPC�̃V���b�g�_�E�������s���܂����H"
		Else
			strMsg = "�V�X�e�����ғ����ł��B" & _
			vbNewLine & "����ł��A�SPC�̃V���b�g�_�E�������s���܂����H"

		End If

		Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

		bRet = frmMsg.ShowDialog()

		Return bRet

	End Function

    ''' <summary>
    ''' �p�X���[�h�m�F
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Protected Friend Overridable Function ConfPass() As DialogResult

        '�p�X���[�h���ݒ�̏ꍇ
        If "" = m_sPass Then
            Return DialogResult.OK
        End If

        '�p�X���[�h�m�F
        Dim frmPass As New tClassForm.frmPop_Pass(m_sPass)
        Dim bRet As DialogResult

        bRet = frmPass.ShowDialog

        Return bRet

    End Function
    ''' <summary>
    ''' PC���Z�b�gor�V�X�e���I�����[�����M
    ''' </summary>
    ''' <remarks></remarks>
	Protected Friend Overridable Sub SendReset(ByVal nEventNo As Integer, Optional ByVal strPcName As String = "")

		Dim que As KizuLibCLI.COMMON_QUE_STR = Nothing

		'PC���Z�b�g
		que.nEventNo = nEventNo
		que.nLineNo = m_nLineID
		que.data = strPcName

		'���[�����M 
		Call KizuLibCLI.KizuMod.Send_Mail(m_sMasterTask, m_sMasterPC, que)

	End Sub

    ''' <summary>
    ''' �V�X�e���I������
    ''' <param name="bStopFlg">��~���t���O(True:��~��,False:����ȊO)</param>
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExitSystem(ByVal bStopFlg As Boolean)

        '���Z�b�g�m�F
		If Windows.Forms.DialogResult.OK <> ConfShatdown(bStopFlg) Then
			Exit Sub
		End If

        '�p�X���[�h�m�F
		If Windows.Forms.DialogResult.OK <> ConfPass() Then
			Exit Sub
		End If

        '�V�X�e���I�����[�����M
        Call SendReset(m_nSYSTEM_EXIT)

    End Sub


End Class
