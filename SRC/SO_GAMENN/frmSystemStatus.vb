'*********************************************************************************
' �����J�n��~�ݒ���
'	[Ver]
'		Ver.01    2012/07/13  ����
'
'	[����]
'
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuLibCLI

Public Class frmSystemStatus

    ''' <summary>
    ''' �R���{�{�b�N�X�ɕ\������f�[�^
    ''' </summary>
    ''' <remarks>�����\�ʏ�ԁE�����p�^�[�� �p</remarks>
    Public Class cls_CmbItem
        ''' <summary>��L�[</summary>
        Public id As String                                 ' ��L�[
        ''' <summary>�R�t������</summary>
        Public name As String                               ' �R�t������
        ''' <summary>���[���X���b�g���M�f�[�^</summary>
        Public sendid As Integer                           ' ���[���X���b�g���M�f�[�^

        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="strID">��L�[</param>
        ''' <param name="strName">�R�t������</param>
        ''' <param name="nSendID">���[���X���b�g���M�f�[�^</param>		
        ''' <remarks></remarks>
        Public Sub New(ByVal strID As String, ByVal strName As String, ByVal nSendID As Integer)
            id = strID
            name = strName
            sendid = nSendID
        End Sub

        ''' <summary>
        ''' �\���̂̕\������
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return id
        End Function
    End Class

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const CHECK_TIME As Integer = 1000              '�Ď��^�C�}�[�Ď��Ԋu(msec)
    Private Const INIT_TIMEOUT_TIME As Integer = 10000      '��ʋN�����^�C���A�E�g����(msec)
    Private Const TIMEOUT_TIME As Integer = 150000          '�����҂��^�C���A�E�g
    Private Const STATUS_WAIT As Integer = -1               '�����҂��X�e�[�^�X
    Private Const STATUS_TIMEOUT As Integer = -2            '�����^�C���A�E�g�X�e�[�^�X
    Private Const COILINF_WAIT As Integer = -3              '�R�C����񉞓��҂��X�[�^�X
    Private Const COILINF_TIMEOUT As Integer = -4           '�R�C����񉞓��҂��^�C���A�E�g
    Private COnst STATUS_FIRST_TIMEOUT As Integer = -5      '���񉞓��҂��^�C���A�E�g
    Private Const PLG_NON As Integer = 0                    'PLG����
    Private Const MAX_CAMSET_NO As Integer = 20             '�J�����Z�b�gNo�ő�l

    '�Жʌ�������
    Private Enum EM_KENSA_UMU
        nKataMenNo = 0  '�Жʌ����s����
        nKataMenYes     '�Жʌ�������
    End Enum

    '���[�������t���O
    Public Enum EM_ANSER_FLG
        nWaite = 0      '�����҂�
        nAnser          '��������
        nUpdata         '�X�V�v��
        nCoilWaite      '�R�C����񉞓��҂�
        nFirstWaite     '���񉞓��҂�
    End Enum

    '�R���{�{�b�N�X���
    Private Enum EM_COMBO_MODE
        nScodeTop = 0   '�����\�ʏ�� �\
        nPcodeTop = 1   '�����p�^�[�� �\
        nScodeBot = 2   '�����\�ʏ�� ��
        nPcodeBot = 3   '�����p�^�[�� ��
    End Enum

    '�R�C����񔽉f��
    Private Enum EM_TARGET_COIL
        nNow = 0            '���R�C��
        nNext               '���R�C��
    End Enum

    ''' <summary>
    ''' ���W�b�N�p�������������
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_LP_KENSA
        nKensaStop = 0              '��~
        nKensaWaitStop = 1          '��~��
        nKensaWaitStart = 2         '�J�n��
        nKensaStart = 3             '������
        nNoAnser = 98               '��������
        nGetData = 99               '��Ԏ擾��
    End Enum

    ''' <summary>
    ''' �������
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_KENSA_KIND
        nPC1 = 0                'PC01
        nPC2 = 1                'PC02
        nNormal                 '�ʏ팟��
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_nUntenStatus As EM_DIV_UNTEN      '�^�]���[�h(1:�ʏ�,2:�e�X�g,3:�Z��,4:�V�~�����[�V����)
    Private m_nKadouStatus As EM_DIV_KADOU      '�ғ����(1:��������,2:��~��,3:�ғ������̓e�X�g��,4:��ײ��)
    Private m_nPlgStatus As EM_DIV_PLG          'PLG�敪(1:���C��,2:�\�Z��,3:���Z��,4:�e�X�g)
    Private m_nKatamenStatus As Integer = -1    '�Жʌ�������(0:�����s����,1:��������)
    Private m_nAnser As EM_ANSER_FLG            '���[�������t���O(0:�����҂�,1:��������,2:�X�V�v��)
    Private m_bLock As Boolean                  '��ʍX�V���t���O(True:��ʍX�V��,False:��ʍX�V���łȂ�)
    Private m_bCoilReqFlg As Boolean = False    '�R�C���v���t���O
    Private m_emLPStatusPc(NUM_HANTEI_PARA - 1) As EM_LP_KENSA  ' ���W�b�N�p���������

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���v���p�e�B
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �^�]���[�h�擾
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("�������J�n��~�ݒ��ʃv���p�e�B") _
     , System.ComponentModel.Description("�^�]���[�h�擾")>
    Public Property tProp_UntenStatus() As Integer
        Get
            Return m_nUntenStatus
        End Get
        Set(ByVal Value As Integer)
            m_nUntenStatus = CType(Value, EM_DIV_UNTEN)
        End Set
    End Property


    ''' <summary>
    ''' �ғ���Ԏ擾
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("�������J�n��~�ݒ��ʃv���p�e�B") _
     , System.ComponentModel.Description("�ғ���ԃ��[�h�擾")>
    Public Property tProp_KadouStatus() As Integer
        Get
            Return m_nKadouStatus
        End Get
        Set(ByVal Value As Integer)
            m_nKadouStatus = CType(Value, EM_DIV_KADOU)
        End Set
    End Property

    ''' <summary>
    ''' PLG�敪�擾
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("�������J�n��~�ݒ��ʃv���p�e�B") _
     , System.ComponentModel.Description("PLG�敪�擾")>
    Public Property tProp_PlgStatus() As Integer
        Get
            Return m_nPlgStatus
        End Get
        Set(ByVal Value As Integer)
            m_nPlgStatus = CType(Value, EM_DIV_PLG)
        End Set
    End Property

    ''' <summary>
    ''' �Жʌ�������
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("�������J�n��~�ݒ��ʃv���p�e�B") _
     , System.ComponentModel.Description("�Жʌ�������")>
    Public Property tProp_KatamenStatus() As Integer
        Get
            Return m_nKatamenStatus
        End Get
        Set(ByVal Value As Integer)
            m_nKatamenStatus = Value
        End Set
    End Property

    ''' <summary>
    ''' �����t���O�擾
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("�������J�n��~�ݒ��ʃv���p�e�B") _
     , System.ComponentModel.Description("�����t���O�擾")>
    Public Property tProp_Anser() As EM_ANSER_FLG
        Get
            Return m_nAnser
        End Get
        Set(ByVal Value As EM_ANSER_FLG)
            m_nAnser = Value
        End Set
    End Property

    ''' <summary>
    ''' ���W�b�N�p������PC�������
    ''' </summary>
    ''' <param name="nPcNo">�擾�Ώۂ�PC�ԍ�</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("�������J�n��~�ݒ��ʃv���p�e�B") _
     , System.ComponentModel.Description("���W�b�N�p������PC01�������")>
    Public Property tProp_LPStatusPc(ByVal nPcNo As Integer) As Integer
        Get
            If NUM_HANTEI_PARA < nPcNo Or 0 > nPcNo Then
                Return -1
            End If

            Return m_emLPStatusPc(nPcNo)
        End Get
        Set(ByVal Value As Integer)
            If NUM_HANTEI_PARA < nPcNo Or 0 > nPcNo Then
                Return
            End If

            If False = [Enum].IsDefined(GetType(EM_LP_KENSA), Value) Then   '��`���Ă��郍�W�b�N�p�������̏�ԈȊO
                Return
            End If

            m_emLPStatusPc(nPcNo) = CType(Value, EM_LP_KENSA)
        End Set
    End Property

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSystemStatus_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus] �t�H�[�����[�h�J�n"))

        Dim sw As New Stopwatch

        sw.Start()

        ' ����{�^���C�x���g
        MyBase.AddFunctionButton(cmdEnd, 11)

        ' �^�C�g���ݒ�
        Call SetTitleName(Me)

        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]�^�C�g���ݒ� [�o�ߎ���][{0}]", sw.Elapsed))

        '�����ݒ�
        Call InitDsp()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]�����ݒ� [�o�ߎ���][{0}]", sw.Elapsed))

        '�����\�ʏ��No�̃R���{�{�b�N�X�쐬
        Call SetScode()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]�����\�ʏ��No�̃R���{�{�b�N�X�쐬 [�o�ߎ���][{0}]", sw.Elapsed))

        '�����p�^�[���R�[�h�̃R���{�{�b�N�X�쐬
        Call SetPcode()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]�����p�^�[���R�[�h�̃R���{�{�b�N�X�쐬 [�o�ߎ���][{0}]", sw.Elapsed))

        '�X�e�[�^�X������
        Call IntiStatus()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]�X�e�[�^�X������ [�o�ߎ���][{0}]", sw.Elapsed))

        '�����҂��^�C�}�[�Z�b�g
        Call SetTimer(INIT_TIMEOUT_TIME, EM_ANSER_FLG.nFirstWaite)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]�����҂��^�C�}�[�Z�b�g [�o�ߎ���][{0}]", sw.Elapsed))

        '�J�����Z�b�g�̃R���{�{�b�N�X�쐬
        Call SetCamSet()
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]�J�����Z�b�g�̃R���{�{�b�N�X�쐬 [�o�ߎ���][{0}]", sw.Elapsed))

        '�����։ғ���Ԏ擾�v��
        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing

        '���M�f�[�^�쐬
        que.nEventNo = FACT_TO_DEFECT_04
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
        'que.cdata1 = System.Environment.MachineName
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]���M�f�[�^�쐬 [�o�ߎ���][{0}]", sw.Elapsed))


        '�ғ���Ԏ擾�v�����M
        Call SendMail(que, TO_DEFECT, "�ғ���Ԏ擾�v��")
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]�ғ���Ԏ擾�v�����M [�o�ߎ���][{0}]", sw.Elapsed))

        '��Ԏ擾��
        Call SetKadouStatus(STATUS_WAIT)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus]��Ԏ擾 [�o�ߎ���][{0}]", sw.Elapsed))

        '���W�b�N�p������PC������ԍăZ�b�g
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            Call SetLpStatus(ii, m_emLPStatusPc)
        Next

        '�G���[���b�Z�[�W�N���A
        lblErrMsg.Text = ""

        sw.Stop()

        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("frmSystemStatus] �t�H�[�����[�h�I��"))

    End Sub

    ''' <summary>
    ''' �X�e�[�^�X������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub IntiStatus()

        m_nUntenStatus = 0  '�^�]���[�h
        m_nKadouStatus = 0  '�ғ����
        m_nPlgStatus = 0    'PLG
        m_nKatamenStatus = -1   '�Жʋ���/�s����

    End Sub

    ''' <summary>
    ''' �I��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdEnd.Click
        Me.Enabled = False
        Me.Hide()
    End Sub

    ''' <summary>
    ''' �����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '�^�]���[�h���W�I�{�^�������ݒ�
        rdoNormal.Tag = EM_DIV_UNTEN.DIV_UNTEN_NOMAL
        rdoSamp.Tag = EM_DIV_UNTEN.DIV_UNTEN_SAMP
        rdoCamera.Tag = EM_DIV_UNTEN.DIV_UNTEN_CAMERA
        rdoSmyu.Tag = EM_DIV_UNTEN.DIV_UNTEN_SMYU


        '�C�x���g�ǉ�
        AddHandler rdoNormal.CheckedChanged, AddressOf rdoUnten_CheckedChanged
        AddHandler rdoSamp.CheckedChanged, AddressOf rdoUnten_CheckedChanged
        AddHandler rdoCamera.CheckedChanged, AddressOf rdoUnten_CheckedChanged
        AddHandler rdoSmyu.CheckedChanged, AddressOf rdoUnten_CheckedChanged


        'PLG�̃��W�I�{�^�������ݒ�
        rdoPlgLine.Tag = EM_DIV_PLG.DIV_PLG_LINE
        rdoPlgTop.Tag = EM_DIV_PLG.DIV_PLG_TOP
        rdoPlgBot.Tag = EM_DIV_PLG.DIV_PLG_BOT
        rdoPlgTest.Tag = EM_DIV_PLG.DIV_PLG_TEST

        '�C�x���g�ǉ�
        AddHandler rdoPlgLine.CheckedChanged, AddressOf rdoPlg_CheckedChanged
        AddHandler rdoPlgTop.CheckedChanged, AddressOf rdoPlg_CheckedChanged
        AddHandler rdoPlgBot.CheckedChanged, AddressOf rdoPlg_CheckedChanged
        AddHandler rdoPlgTest.CheckedChanged, AddressOf rdoPlg_CheckedChanged

        '�Жʌ������W�I�{�^�������ݒ�
        rdoKatamenNo.Tag = EM_KENSA_UMU.nKataMenNo
        rdoKatamenYes.Tag = EM_KENSA_UMU.nKataMenYes

        '�C�x���g�ǉ�
        AddHandler rdoKatamenNo.CheckedChanged, AddressOf rdoKatamen__CheckedChanged
        AddHandler rdoKatamenYes.CheckedChanged, AddressOf rdoKatamen__CheckedChanged

        '�R���{�{�b�N�X�����ݒ�
        cmbSCodeTop.Tag = EM_COMBO_MODE.nScodeTop   '�����\�ʏ�� �\
        cmbPCodeTop.Tag = EM_COMBO_MODE.nPcodeTop   '�����p�^�[�� �\
        cmbSCodeBot.Tag = EM_COMBO_MODE.nScodeBot   '�����\�ʏ�� ��
        cmbPCodeBot.Tag = EM_COMBO_MODE.nPcodeBot   '�����p�^�[�� ��

        '�C�x���g�ǉ�
        AddHandler cmbSCodeTop.SelectedIndexChanged, AddressOf cmbData_SelectedIndexChanged
        AddHandler cmbPCodeTop.SelectedIndexChanged, AddressOf cmbData_SelectedIndexChanged
        AddHandler cmbSCodeBot.SelectedIndexChanged, AddressOf cmbData_SelectedIndexChanged
        AddHandler cmbPCodeBot.SelectedIndexChanged, AddressOf cmbData_SelectedIndexChanged

        '�Ď��^�C�}�[�Z�b�g
        timCheck.Interval = CHECK_TIME
        timCheck.Enabled = True

        AddHandler modMain.tEV_MailSlotRecv_FACT_SO_GAMENN_01, AddressOf ReceiveFACT_SO_GAMENN_01

    End Sub


    ''' <summary>
    ''' �����J�n�{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub btnKensaStart_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKensaStart.Click

        SendKensaStart(EM_KENSA_KIND.nNormal, FACT_TO_DEFECT_02)

    End Sub


    ''' <summary>
    ''' �m�F���b�Z�[�W�\��
    ''' </summary>
    ''' <param name="strMsg"></param>
    ''' <remarks></remarks>
    Public Overridable Sub DspMsg(ByVal strMsg As String)

        Dim frmMsg As tClassForm.frmPop_MsgBox

        frmMsg = New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, False, System.Windows.Forms.DialogResult.OK)

        '���b�Z�[�W�\��
        frmMsg.ShowDialog()
        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub

    ''' <summary>
    ''' �m�F���b�Z�[�W�\��
    ''' </summary>
    ''' <param name="strMsg">�\�����b�Z�[�W</param>
    ''' <param name="bRet">�߂�l</param>
    ''' <remarks></remarks>
    Public Overridable Sub DspMsg(ByVal strMsg As String, ByRef bRet As DialogResult)

        Dim frmMsg As tClassForm.frmPop_MsgBox
        frmMsg = New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, True, System.Windows.Forms.DialogResult.OK, System.Windows.Forms.DialogResult.Cancel)

        '���b�Z�[�W�\��
        bRet = frmMsg.ShowDialog()
        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub


    ''' <summary>
    ''' �����I���{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub btnKensaEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKensaEnd.Click

        SendKensaStop(EM_KENSA_KIND.nNormal, FACT_TO_DEFECT_03)

    End Sub

    ''' <summary>
    ''' �V�X�e���I���{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSystemEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSystemEnd.Click

        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing
        Dim bRet As DialogResult


        '�V�X�e���I���m�F
        Call DspMsg("�V�X�e�����I�����܂����H", bRet)
        If System.Windows.Forms.DialogResult.OK <> bRet Then
            Exit Sub
        End If

        '�p�X���[�h���̓`�F�b�N
        '�p�X���[�h�m�F
        Dim frmPass As New tClassForm.frmPop_Pass(PASSWORD)
        bRet = frmPass.ShowDialog
        If System.Windows.Forms.DialogResult.OK <> bRet Then
            Exit Sub
        End If

        '���M�f�[�^�ݒ�
        que.nEventNo = FACT_KS_MASTER_06
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        '�V�X�e���I�����[�����M
        Call SendMail(que, KS_MASTER, "�V�X�e���I���v��")

    End Sub

    ''' <summary>
    ''' ��M�҂��^�C�}�[�Z�b�g
    ''' </summary>
    ''' <param name="nVal">�C���^�[�o��</param>
    ''' <param name="nWaite">�����҂��萔</param>
    ''' <remarks></remarks>
    Private Sub SetTimer(ByVal nVal As Integer, ByVal nWaite As EM_ANSER_FLG)

        '�^�C�}�[�Z�b�g
        timTimeOut.Interval = nVal
        timTimeOut.Enabled = True

        '�v���O���X�o�[�ݒ�
        prbTimeOut.Maximum = CInt(nVal / CHECK_TIME)
        prbTimeOut.Value = 0

        '�����҂�
        m_nAnser = nWaite

        '��U�^�C�}�[��STOP
        timCheck.Stop()
        timCheck.Start()

    End Sub


    ''' <summary>
    ''' �^�]���[�h�����`�F�b�N
    ''' </summary>
    ''' <remarks></remarks>
    Private Function CheckUnten() As Boolean

        '�ʏ�^�]
        If rdoNormal.Checked Then
            Return True
        End If

        '�e�X�g�p
        If rdoSamp.Checked Then
            Return True
        End If

        '�Z��
        If rdoCamera.Checked Then
            Return True
        End If

        '�V�~�����[�V����
        If rdoSmyu.Checked Then
            Return True
        End If

        Return False
    End Function
    ''' <summary>
    ''' PLG�敪�����`�F�b�N
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckPlg() As Boolean

        '���C��
        If rdoPlgLine.Checked Then
            Return True
        End If

        'PLGTOP
        If rdoPlgTop.Checked Then
            Return True
        End If

        'PLGBOT
        If rdoPlgBot.Checked Then
            Return True
        End If

        '�e�X�g
        If rdoPlgTest.Checked Then
            Return True
        End If

        Return False
    End Function
    ''' <summary>
    ''' �Жʌ������������`�F�b�N
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckKatamen() As Boolean

        '�Жʌ����s����
        If rdoKatamenNo.Checked Then
            Return True
        End If

        '�Жʌ�������
        If rdoKatamenYes.Checked Then
            Return True
        End If
        Return False
    End Function

    ''' <summary>
    ''' ���[�����M����
    ''' </summary>
    ''' <param name="que">���M�f�[�^</param>
    ''' <param name="strTaskName">�^�X�N��</param>
    ''' <param name="strEventName">�N���v������</param>
    ''' <remarks></remarks>
    Private Sub SendMail(ByVal que As KizuLibCLI.COMMON_QUE_STRMIX, ByVal strTaskName As String, ByVal strEventName As String)

        Dim nRet As Integer

        '�����J�n�v�����M
        If Not LOCAL Then
            '�{��
            nRet = KizuLibCLI.KizuMod.Send_Mail(strTaskName, KizuLibCLI.KizuMod.Tokatu, que)
        Else
            '�e�X�g
            nRet = KizuLibCLI.KizuMod.Send_Mail(strTaskName, ".", que)
        End If

        If 0 <> nRet Then
            If NOA_OFFLINE <> g_NOA Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_MAIL_SNED_ERR, String.Format("[{0}]", strEventName & " err_code = " & nRet))
            End If
        End If

    End Sub


    ''' <summary>
    ''' �Ď�����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timCheck_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timCheck.Tick

        '�������Ȃ��ꍇ�́A�I��
        If EM_ANSER_FLG.nFirstWaite = m_nAnser Or EM_ANSER_FLG.nWaite = m_nAnser Or EM_ANSER_FLG.nCoilWaite = m_nAnser Then
            prbTimeOut.Value += 1
            Exit Sub
        End If

        '�X�V�t���OON(������ŕ\������ꍇ�́A�ғ���~���ł��`�F�b�N�C�x���g��L���ɂ��邽��)
        m_bLock = True

        '�ғ��󋵃Z�b�g
        Call SetKadouStatus(CInt(m_nKadouStatus))

        '�^�]���[�h�Z�b�g
        Call SetUntenStatus(m_nUntenStatus)

        'PLG�敪�Z�b�g
        Call SetPlgStatus(m_nPlgStatus)

        '�Жʌ�����ԃZ�b�g
        Call SetKatamenStatus(m_nKatamenStatus)

        '���W�b�N�p��������ԃZ�b�g
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            Call SetLpStatus(ii, m_emLPStatusPc)
        Next

        timTimeOut.Enabled = False
        prbTimeOut.Value = 0

        '�X�V�t���OOFF
        m_bLock = False

    End Sub

    ''' <summary>
    ''' �ғ���Ԏ擾�^�C���A�E�g����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timTimeOut_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timTimeOut.Tick

        '�����^�C���A�E�g
        If EM_ANSER_FLG.nFirstWaite = m_nAnser Then
            Call SetKadouStatus(STATUS_FIRST_TIMEOUT)
        ElseIf EM_ANSER_FLG.nWaite = m_nAnser Then
            Call SetKadouStatus(STATUS_TIMEOUT)
        ElseIf EM_ANSER_FLG.nCoilWaite = m_nAnser Then
            Call SetKadouStatus(COILINF_TIMEOUT)
        End If

        prbTimeOut.Value = prbTimeOut.Maximum

        timTimeOut.Enabled = False

        m_nAnser = EM_ANSER_FLG.nAnser
    End Sub

    ''' <summary>
    ''' �ғ��󋵃Z�b�g
    ''' </summary>
    ''' <param name="nVal">�X�e�[�^�X</param>
    ''' <remarks></remarks>
    Private Sub SetKadouStatus(ByVal nVal As Integer)

        Select Case nVal

            Case STATUS_WAIT                '��Ԏ擾��
                lblSystemStatus.Text = "��Ԏ擾��"
                lblSystemStatus.BackColor = Color.Lime
                lblSystemStatus.ForeColor = Color.Black
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = False
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False
                tProp_LPStatusPc(0) = EM_LP_KENSA.nGetData
                tProp_LPStatusPc(1) = EM_LP_KENSA.nGetData

            Case STATUS_FIRST_TIMEOUT       '����^�C���A�E�g
                lblSystemStatus.Text = "��������"
                lblSystemStatus.BackColor = Color.Magenta
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = True
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False
                tProp_LPStatusPc(0) = EM_LP_KENSA.nNoAnser
                tProp_LPStatusPc(1) = EM_LP_KENSA.nNoAnser

            Case STATUS_TIMEOUT                 '�^�C���A�E�g
                lblSystemStatus.Text = "��������"
                lblSystemStatus.BackColor = Color.Magenta
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = True
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False

            Case EM_DIV_KADOU.DIV_KADOU_INIT    '������
                lblSystemStatus.Text = "��������"
                lblSystemStatus.BackColor = g_ColorKadouInit
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = False
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False

            Case EM_DIV_KADOU.DIV_KADOU_STOP    '��~
                lblSystemStatus.Text = "��~"
                lblSystemStatus.BackColor = g_ColorKadouStop
                lblSystemStatus.ForeColor = Color.Yellow

                btnKensaEnd.Enabled = False
                btnSystemEnd.Enabled = True

                btnKensaStart.Enabled = True
                cmbSCodeTop.Enabled = True
                lblSCodeTop.Enabled = True
                cmbPCodeTop.Enabled = True
                lblPCodeTop.Enabled = True
                cmbSCodeBot.Enabled = True
                lblSCodeBot.Enabled = True
                cmbPCodeBot.Enabled = True
                lblPCodeBot.Enabled = True

            Case EM_DIV_KADOU.DIV_KADOU_START   '�ғ������̓e�X�g��

                If m_nUntenStatus = EM_DIV_UNTEN.DIV_UNTEN_NOMAL Then
                    lblSystemStatus.Text = "�ғ���"
                Else
                    lblSystemStatus.Text = "�e�X�g��"
                End If

                lblSystemStatus.BackColor = g_ColorKadouStart
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = True
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False

            Case EM_DIV_KADOU.DIV_KADOU_RETRY   '��ײ��
                lblSystemStatus.Text = "���g���C��"
                lblSystemStatus.BackColor = g_ColorKadouRetry
                lblSystemStatus.ForeColor = Color.Yellow
                btnKensaStart.Enabled = False
                btnKensaEnd.Enabled = True
                btnSystemEnd.Enabled = False
                cmbSCodeTop.Enabled = False
                lblSCodeTop.Enabled = False
                cmbPCodeTop.Enabled = False
                lblPCodeTop.Enabled = False
                cmbSCodeBot.Enabled = False
                lblSCodeBot.Enabled = False
                cmbPCodeBot.Enabled = False
                lblPCodeBot.Enabled = False

        End Select

    End Sub

    ''' <summary>
    ''' �^�]���[�h�Z�b�g
    ''' </summary>
    ''' <param name="nVal">�X�e�[�^�X</param>
    ''' <remarks></remarks>
    Private Sub SetUntenStatus(ByVal nVal As Integer)

        Select Case nVal
            Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL   '�ʏ�
                rdoNormal.Checked = True
            Case EM_DIV_UNTEN.DIV_UNTEN_SAMP    '�e�X�g
                rdoSamp.Checked = True
            Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA  '�Z��
                rdoCamera.Checked = True
            Case EM_DIV_UNTEN.DIV_UNTEN_SMYU    '�V�~�����[�V����
                rdoSmyu.Checked = True
            Case Else

                '�S�Ă̍��ڂ̃`�F�b�N���O��
                rdoNormal.Checked = False
                rdoSamp.Checked = False
                rdoCamera.Checked = False
                rdoSmyu.Checked = False
        End Select

    End Sub
    ''' <summary>
    ''' PLG�敪�Z�b�g
    ''' </summary>
    ''' <param name="nVal">�X�e�[�^�X</param>
    ''' <remarks></remarks>
    Private Sub SetPlgStatus(ByVal nVal As Integer)

        Select Case nVal
            Case EM_DIV_PLG.DIV_PLG_LINE    '���C��
                rdoPlgLine.Checked = True
            Case EM_DIV_PLG.DIV_PLG_TOP     '�\�Z��
                rdoPlgTop.Checked = True
            Case EM_DIV_PLG.DIV_PLG_BOT     '���Z��
                rdoPlgBot.Checked = True
            Case EM_DIV_PLG.DIV_PLG_TEST    '�e�X�g
                rdoPlgTest.Checked = True
            Case Else

                '�S�Ă̍��ڂ̃`�F�b�N���O��
                rdoPlgLine.Checked = False
                rdoPlgTop.Checked = False
                rdoPlgBot.Checked = False
                rdoPlgTest.Checked = False
        End Select

    End Sub

    ''' <summary>
    ''' �Жʌ�����Ԃ��Z�b�g
    ''' </summary>
    ''' <param name="nVal"></param>
    ''' <remarks></remarks>
    Private Sub SetKatamenStatus(ByVal nVal As Integer)

        Select Case nVal
            Case EM_KENSA_UMU.nKataMenNo    '�Жʌ����s����
                rdoKatamenNo.Checked = True
            Case EM_KENSA_UMU.nKataMenYes   '�Жʌ�������
                rdoKatamenYes.Checked = True
            Case Else

                '�S�Ă̍��ڂ̃`�F�b�N���O��
                rdoKatamenNo.Checked = False
                rdoKatamenYes.Checked = False
        End Select

    End Sub

    ''' <summary>
    ''' ���W�b�N�p��������ԃZ�b�g
    ''' </summary>
    ''' <param name="nKind">�Ώ�PC(0:PC1,0�ȊO:PC2)</param>
    ''' <param name="nVal">���W�b�N�p���������</param>
    ''' <remarks></remarks>
    Private Sub SetLpStatus(ByVal nKind As Integer, ByVal nVal() As EM_LP_KENSA)

        Dim lblLPStart As Label
        Dim btnLpStart As Button
        Dim btnLpEnd As Button
        Dim cmbCamera1 As ComboBox
        Dim cmbCamera2 As ComboBox
        Dim nMyPcIdx As Integer
        Dim nOtherPcIdx As Integer

        '�@�Ώ�PC����
        If 0 = nKind Then
            lblLPStart = CType(lblLPStatusPC1, Label)
            btnLpStart = CType(btnLPStartPC1, Button)
            btnLpEnd = CType(btnLPEndPC1, Button)
            cmbCamera1 = CType(cmbCamera1PC1, ComboBox)
            cmbCamera2 = CType(cmbCamera2PC1, ComboBox)

            nMyPcIdx = 0
            nOtherPcIdx = 1
        Else
            lblLPStart = CType(lblLPStatusPC2, Label)
            btnLpStart = CType(btnLPStartPC2, Button)
            btnLpEnd = CType(btnLPEndPC2, Button)
            cmbCamera1 = CType(cmbCamera1PC2, ComboBox)
            cmbCamera2 = CType(cmbCamera2PC2, ComboBox)

            nMyPcIdx = 1
            nOtherPcIdx = 0
        End If

        ' �R���{�{�b�N�X�ݒ�
        If EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus _
           AndAlso (EM_LP_KENSA.nKensaWaitStart = nVal(nOtherPcIdx) _
                    OrElse EM_LP_KENSA.nKensaWaitStop = nVal(nOtherPcIdx)) Then         ' �ғ���ԁF�ғ��� and ���W�b�N�p�������J�n���A��~���i��PC�j
            cmbCamera1.Enabled = False
            cmbCamera2.Enabled = False
        ElseIf (EM_DIV_KADOU.DIV_KADOU_STOP = m_nKadouStatus _
            OrElse EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus) _
            AndAlso EM_LP_KENSA.nKensaStop = nVal(nMyPcIdx) Then                        ' �ғ���ԁF�ғ���or��~�� and ���W�b�N�p��������~ 
            cmbCamera1.Enabled = True
            cmbCamera2.Enabled = True
        Else
            cmbCamera1.Enabled = False
            cmbCamera2.Enabled = False
        End If

        ' �J�n�A��~�{�^���ݒ�
        If EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus _
           AndAlso (EM_LP_KENSA.nKensaWaitStart = nVal(nOtherPcIdx) _
                    OrElse EM_LP_KENSA.nKensaWaitStop = nVal(nOtherPcIdx)) Then         ' �ғ���ԁF�ғ��� and ���W�b�N�p�������J�n���A��~���i��PC�j
            btnLpStart.Enabled = False
            btnLpEnd.Enabled = False
        ElseIf EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus _
           AndAlso EM_LP_KENSA.nKensaStop = nVal(nMyPcIdx) Then                         ' �ғ���ԁF�ғ��� and ���W�b�N�p��������~
            btnLpStart.Enabled = True
            btnLpEnd.Enabled = False
        ElseIf EM_DIV_KADOU.DIV_KADOU_START = m_nKadouStatus _
           AndAlso EM_LP_KENSA.nKensaStart = nVal(nMyPcIdx) Then                        ' �ғ���ԁF�ғ��� and ���W�b�N�p������������
            btnLpStart.Enabled = False
            btnLpEnd.Enabled = True
        Else                            ' ��L�ȊO
            btnLpStart.Enabled = False
            btnLpEnd.Enabled = False
        End If

        ' ��ԃ��x���ݒ�
        Select Case nVal(nMyPcIdx)
            Case EM_LP_KENSA.nKensaStop         '��~
                lblLPStart.Text = "��~"
                lblLPStart.BackColor = g_ColorKadouStop
                lblLPStart.ForeColor = Color.Yellow

            Case EM_LP_KENSA.nKensaWaitStop     '��~��
                lblLPStart.Text = "��~��"
                lblLPStart.BackColor = g_ColorKadouStop
                lblLPStart.ForeColor = Color.Yellow

            Case EM_LP_KENSA.nKensaWaitStart    '�J�n��
                lblLPStart.Text = "�J�n��"
                lblLPStart.BackColor = Color.Lime
                lblLPStart.ForeColor = Color.Black

            Case EM_LP_KENSA.nKensaStart        '������
                lblLPStart.Text = "������"
                lblLPStart.BackColor = g_ColorKadouStart
                lblLPStart.ForeColor = Color.Yellow

            Case EM_LP_KENSA.nNoAnser           '��������
                lblLPStart.Text = "��������"
                lblLPStart.BackColor = Color.Magenta
                lblLPStart.ForeColor = Color.Yellow

            Case EM_LP_KENSA.nGetData           '��Ԏ擾��
                lblLPStart.Text = "��Ԏ擾��"
                lblLPStart.BackColor = Color.Lime
                lblLPStart.ForeColor = Color.Black

        End Select

    End Sub


    ''' <summary>
    ''' �^�]���[�h���W�I�{�^���`�F�b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub rdoUnten_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        ''��ʍX�V���łȂ��ꍇ���ғ���~���ȊO�܂��̓R�C�����҂��̏ꍇ�A���얳��
        If (m_bLock = False And EM_DIV_KADOU.DIV_KADOU_STOP <> m_nKadouStatus) _
            Or EM_ANSER_FLG.nCoilWaite = m_nAnser Then
            '���ɖ߂�
            Call SetUntenStatus(m_nUntenStatus)
            Exit Sub
        End If

        '�`�F�b�N���O�ꂽ�ꍇ�́A������
        If Not rdoData.Checked Then
            rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
            rdoData.ForeColor = Color.Black
            Exit Sub
        End If

        Select Case CType(rdoData.Tag, EM_DIV_UNTEN)

            Case EM_DIV_UNTEN.DIV_UNTEN_STOP    '��~
                rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
                rdoData.ForeColor = Color.Black
            Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL   '�ʏ�
                rdoData.BackColor = g_ColorUntenNomal
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_UNTEN.DIV_UNTEN_SAMP    '�e�X�g
                rdoData.BackColor = g_ColorUntenSamp
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA  '�Z��
                rdoData.BackColor = g_ColorUntenCamera
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_UNTEN.DIV_UNTEN_SMYU    '�V�~�����[�V����
                rdoData.BackColor = g_ColorUntenSmyu
                rdoData.ForeColor = Color.Yellow
            Case Else
                rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
                rdoData.ForeColor = Color.Black
        End Select

        '�X�e�[�^�X�Z�b�g
        m_nUntenStatus = CType(rdoData.Tag, EM_DIV_UNTEN)


    End Sub

    ''' <summary>
    ''' PLG���W�I�{�^���`�F�b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub rdoPlg_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        ''��ʍX�V���łȂ��ꍇ���ғ���~���ȊO�܂��̓R�C�����҂��̏ꍇ�A���얳��
        If (m_bLock = False And EM_DIV_KADOU.DIV_KADOU_STOP <> m_nKadouStatus) _
            Or EM_ANSER_FLG.nCoilWaite = m_nAnser Then

            '���ɖ߂�
            Call SetPlgStatus(m_nPlgStatus)
            Exit Sub
        End If

        '�`�F�b�N���O�ꂽ�ꍇ�́A������
        If Not rdoData.Checked Then
            rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
            rdoData.ForeColor = Color.Black
            Exit Sub
        End If

        Select Case CType(rdoData.Tag, EM_DIV_PLG)
            Case EM_DIV_PLG.DIV_PLG_LINE    '���C��
                rdoData.BackColor = g_ColorPlgLine
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_PLG.DIV_PLG_TOP     '�\�Z��
                rdoData.BackColor = g_ColorPlgTop
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_PLG.DIV_PLG_BOT     '���Z��
                rdoData.BackColor = g_ColorPlgBot
                rdoData.ForeColor = Color.Yellow
            Case EM_DIV_PLG.DIV_PLG_TEST    '�e�X�g
                rdoData.BackColor = g_ColorPlgTest
                rdoData.ForeColor = Color.Yellow
            Case Else
                rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
                rdoData.ForeColor = Color.Black
        End Select

        '�X�e�[�^�X�Z�b�g
        m_nPlgStatus = CType(rdoData.Tag, EM_DIV_PLG)

    End Sub

    ''' <summary>
    ''' �Жʌ����L���`�F�b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub rdoKatamen__CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        ''��ʍX�V���łȂ��ꍇ���ғ���~���ȊO�܂��̓R�C�����҂��̏ꍇ�A���얳��
        If (m_bLock = False And EM_DIV_KADOU.DIV_KADOU_STOP <> m_nKadouStatus) _
            Or EM_ANSER_FLG.nCoilWaite = m_nAnser Then
            ''���ɖ߂�
            Call SetKatamenStatus(m_nKatamenStatus)
            Exit Sub
        End If

        '�`�F�b�N���O�ꂽ�ꍇ�́A������
        If Not rdoData.Checked Then
            rdoData.BackColor = Color.FromKnownColor(KnownColor.Control)
            rdoData.ForeColor = Color.Black
            Exit Sub
        End If

        Select Case CInt(rdoData.Tag)
            Case EM_KENSA_UMU.nKataMenNo
                rdoData.BackColor = Color.Magenta
                rdoData.ForeColor = Color.Yellow
            Case EM_KENSA_UMU.nKataMenYes
                rdoData.BackColor = Color.Blue
                rdoData.ForeColor = Color.Yellow
        End Select

        '�X�e�[�^�X�Z�b�g
        m_nKatamenStatus = CInt(rdoData.Tag)

    End Sub

    ''' <summary>
    ''' �����\�ʏ�ԃR���{�{�b�N�X�쐬
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetScode()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        'SQL���쐬
        strSql = ""
        strSql &= " SELECT SCODE"
        strSql &= " ,�����\�ʏ�Ԗ���"
        strSql &= " FROM " & DB_SCODE_MASTER
        strSql &= " WHERE �L������ = 1"
        strSql &= " ORDER BY SCODE ASC "

        Try

            ' DB�ڑ�
            If Not tcls_DB.Open() Then Exit Sub

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            '�R���{�{�b�N�X����
            cmbSCodeTop.Items.Clear()
            cmbSCodeBot.Items.Clear()
            While sqlRead.Read()
                cmbSCodeTop.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(0))))
                cmbSCodeBot.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(0))))
            End While

            '�f�t�H���g�l��\��
            If cmbSCodeTop.Items.Count > 0 Then
                cmbSCodeTop.SelectedIndex = cmbSCodeTop.Items.Count - 1
            End If
            If cmbSCodeBot.Items.Count > 0 Then
                cmbSCodeBot.SelectedIndex = cmbSCodeBot.Items.Count - 1
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "SCODE_MASTER�擾���s")
        Finally
            If Not sqlRead Is Nothing Then sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub

    ''' <summary>
    ''' �����p�^�[���R�[�h�̃R���{�{�b�N�X�쐬
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetPcode()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        'SQL���쐬
        strSql = ""
        strSql &= " SELECT PCODE"
        strSql &= " ,�����p�^�[������"
        strSql &= " FROm " & DB_PCODE_MASTER
        strSql &= " WHERE �L������ = 1"
        strSql &= " ORDER BY PCODE ASC "

        Try

            ' DB�ڑ�
            If Not tcls_DB.Open() Then Exit Sub

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            '�R���{�{�b�N�X����
            cmbPCodeTop.Items.Clear()
            cmbPCodeBot.Items.Clear()
            While sqlRead.Read()
                cmbPCodeTop.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(0))))
                cmbPCodeBot.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(0))))
            End While

            '�f�t�H���g�l��\��
            If cmbPCodeTop.Items.Count > 0 Then
                cmbPCodeTop.SelectedIndex = cmbPCodeTop.Items.Count - 1
            End If
            If cmbPCodeBot.Items.Count > 0 Then
                cmbPCodeBot.SelectedIndex = cmbPCodeBot.Items.Count - 1
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "PCODE_MASTER�擾���s")
        Finally
            If Not sqlRead Is Nothing Then sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub


    ''' <summary>
    ''' �R���{�{�b�N�X�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbData_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim cmbData As ComboBox = CType(sender, ComboBox)

        Select Case CType(cmbData.Tag, EM_COMBO_MODE)

            Case EM_COMBO_MODE.nScodeTop    '�����\�ʏ�� �\

                '�����\�ʏ�Ԗ��̂�\��
                lblSCodeTop.Text = CType(cmbData.Items(cmbData.SelectedIndex), cls_CmbItem).name

            Case EM_COMBO_MODE.nPcodeTop    '�����p�^�[�� �\

                '�����p�^�[�����̂�\��
                lblPCodeTop.Text = CType(cmbData.Items(cmbData.SelectedIndex), cls_CmbItem).name

            Case EM_COMBO_MODE.nScodeBot    '�����\�ʏ�� ��

                '�����\�ʏ�Ԗ��̂�\��
                lblSCodeBot.Text = CType(cmbData.Items(cmbData.SelectedIndex), cls_CmbItem).name

            Case EM_COMBO_MODE.nPcodeBot    '�����p�^�[�� ��

                '�����p�^�[�����̂�\��
                lblPCodeBot.Text = CType(cmbData.Items(cmbData.SelectedIndex), cls_CmbItem).name


        End Select

    End Sub


    ''' <summary>
    ''' �ғ���Ԓʒm��M����
    ''' </summary>
    ''' <param name="que_l"></param>
    ''' <remarks></remarks>
    Private Sub ReceiveFACT_SO_GAMENN_01(ByVal que_l As COMMON_QUE_L)
        '������~�J�n�ݒ��ʂ��\�����Ȃ�ݒ�

        If tProp_Anser = frmSystemStatus.EM_ANSER_FLG.nFirstWaite OrElse tProp_Anser = frmSystemStatus.EM_ANSER_FLG.nWaite Then
            tProp_Anser = frmSystemStatus.EM_ANSER_FLG.nAnser   '��������
        Else

            '���݂̏�Ԃ���~���̏ꍇ�́A����
            If tProp_KadouStatus = EM_DIV_KADOU.DIV_KADOU_STOP Then

                ' ���W�b�N�p�������̏�Ԃ̂ݍX�V
                tProp_LPStatusPc(0) = que_l.data(4) '���W�b�N�p������PC01�������
                tProp_LPStatusPc(1) = que_l.data(5) '���W�b�N�p������PC02�������

                '���W�b�N�p��������ԃZ�b�g
                For ii As Integer = 0 To NUM_HANTEI_PARA - 1
                    Call SetLpStatus(ii, m_emLPStatusPc)
                Next

                Exit Sub
            End If
            tProp_Anser = frmSystemStatus.EM_ANSER_FLG.nUpdata  '�X�V�v��
        End If

        tProp_UntenStatus = que_l.data(0)   '�^�]���[�h
        tProp_PlgStatus = que_l.data(1)     'PLG�敪
        tProp_KatamenStatus = que_l.data(2) '�Жʌ������A�s����
        tProp_KadouStatus = que_l.data(3)   '�ғ����
        tProp_LPStatusPc(0) = que_l.data(4) '���W�b�N�p������PC01�������
        tProp_LPStatusPc(1) = que_l.data(5) '���W�b�N�p������PC02�������
    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^��
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    Private Sub frmSystemStatus_tEv_FunctionButton(FncNo As Integer, onKey As Keys) Handles MyBase.tEv_FunctionButton
        ' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 11                             ' F12 ����
                Me.Dispose()
        End Select
    End Sub

    ''' <summary>
    ''' �����J�n�iPC01�j�{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLPStartPC1_Click(sender As Object, e As EventArgs) Handles btnLPStartPC1.Click

        SendKensaStart(EM_KENSA_KIND.nPC1, FACT_TO_DEFECT_21)

    End Sub

    ''' <summary>
    ''' ������~�iPC01�j�{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLPEndPC1_Click(sender As Object, e As EventArgs) Handles btnLPEndPC1.Click

        SendKensaStop(EM_KENSA_KIND.nPC1, FACT_TO_DEFECT_22)

    End Sub

    ''' <summary>
    ''' �����J�n�iPC02�j�{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLPStartPC2_Click(sender As Object, e As EventArgs) Handles btnLPStartPC2.Click

        SendKensaStart(EM_KENSA_KIND.nPC2, FACT_TO_DEFECT_21)

    End Sub

    ''' <summary>
    ''' ������~�iPC02�j�{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLPEndPC2_Click(sender As Object, e As EventArgs) Handles btnLPEndPC2.Click

        SendKensaStop(EM_KENSA_KIND.nPC2, FACT_TO_DEFECT_22)

    End Sub

    ''' <summary>
    ''' �����J�n���M
    ''' </summary>
    ''' <param name="nKind">�������</param>
    ''' <param name="nEventNo">�C�x���g�ԍ�</param>
    ''' <remarks></remarks>
    Private Sub SendKensaStart(ByVal nKind As EM_KENSA_KIND, ByVal nEventNo As Integer)
        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing
        Dim strMsg As String
        Dim strSend As String

        '�I���`�F�b�N
        If Not CheckUnten() Then
            Call DspMsg("�^�]���[�h��I�����Ă��������B")
            Exit Sub
        End If

        If Not CheckPlg() Then
            Call DspMsg("PLG��I�����Ă��������B")
            Exit Sub
        End If

        If Not CheckKatamen() Then
            Call DspMsg("�Жʌ���������I�����Ă��������B")
            Exit Sub
        End If

        If Not UpdCamSetNo(nKind) Then
            lblErrMsg.Text = "DB�X�V���s�@�J�����ێ��Ɏ��s���܂����B"
        Else
            lblErrMsg.Text = ""
        End If

        '�����҂��^�C�}�[�Z�b�g
        Call SetTimer(TIMEOUT_TIME, EM_ANSER_FLG.nWaite)

        '���M�f�[�^�ݒ�
        que = SetStartQue(nKind, nEventNo)

        strMsg = " �^�]Ӱ�� = " & que.idata(0)
        strMsg &= " PLGӰ�� = " & que.idata(1)
        strMsg &= " �Жʌ������s���� = " & que.idata(2)
        strMsg &= String.Format(" �����\�ʏ�ԕ\,�� = {0},{1}", que.idata(3), que.idata(4))
        strMsg &= String.Format(" ��������ݕ\,�� = {0},{1}", que.idata(5), que.idata(6))

        If EM_KENSA_KIND.nNormal = nKind Then
            strMsg &= String.Format(" �����PC01_���01,02 = {0},{1}", que.idata(7), que.idata(8))
            strMsg &= String.Format(" �����PC02_���01,02 = {0},{1}", que.idata(9), que.idata(10))
            strSend = "�����J�n�v��"
        Else
            strMsg &= " �����J�n�敪 = " & que.idata(7)
            strMsg &= String.Format(" �����PC0{0}_���01,02 = {1},{2}", que.idata(7) + 1, que.idata(8), que.idata(9))
            strSend = "�r���J�n�v��"
        End If

        strMsg = String.Format("{0}", strSend) & strMsg

        '�����J�n�V�X���O
        If NOA_OFFLINE <> g_NOA Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_SYSTEM_START, String.Format("[{0}]", strMsg))
        End If

        '�����J�n�v�����M
        Call SendMail(que, TO_DEFECT, strSend)

        '�X�e�[�^�X������
        Call IntiStatus()

        '��Ԏ擾��
        Call SetKadouStatus(STATUS_WAIT)

        '���W�b�N�p������PC������ԍăZ�b�g
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            Call SetLpStatus(ii, m_emLPStatusPc)
        Next

    End Sub

    ''' <summary>
    ''' ������~���M
    ''' </summary>
    ''' <param name="nKind">�������</param>
    ''' <param name="nEventNo">�C�x���g�ԍ�</param>
    ''' <remarks></remarks>
    Private Sub SendKensaStop(ByVal nKind As EM_KENSA_KIND, ByVal nEventNo As Integer)
        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing
        Dim bRet As DialogResult
        Dim strMsg As String
        Dim strSend As String

        '�����I���m�F
        Call DspMsg("�������I�����܂����H", bRet)
        If System.Windows.Forms.DialogResult.OK <> bRet Then
            Exit Sub
        End If

        '�X�e�[�^�X������
        Call IntiStatus()

        '�����҂��^�C�}�[�Z�b�g
        Call SetTimer(TIMEOUT_TIME, EM_ANSER_FLG.nWaite)

        '��Ԏ擾��
        Call SetKadouStatus(STATUS_WAIT)

        '���W�b�N�p������PC������ԍăZ�b�g
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            Call SetLpStatus(ii, m_emLPStatusPc)
        Next

        '���M�f�[�^�ݒ�
        strMsg = ""
        que.inisialize()
        que.nEventNo = nEventNo
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
        If EM_KENSA_KIND.nNormal = nKind Then
            strSend = "������~�v��"
        Else
            que.idata(0) = nKind
            strMsg = " ������~�敪 = " & que.idata(0)
            strSend = "�r����~�v��"
        End If

        strMsg = String.Format("{0}", strSend) & strMsg

        '������~�v�����M
        Call SendMail(que, TO_DEFECT, strSend)

        '������~�V�X���O
        If NOA_OFFLINE <> g_NOA Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_SYSTEM_STOP, String.Format("[{0}]", strMsg))
        End If

        '�G���[���b�Z�[�W������
        lblErrMsg.Text = ""

    End Sub

    ''' <summary>
    '''QUE���쐬
    ''' </summary>
    ''' <param name="nKind">�������</param>
    ''' <param name="nEventNo">�C�x���g�ԍ�</param>
    ''' <remarks></remarks>
    Public Function SetStartQue(ByVal nKind As EM_KENSA_KIND, ByVal nEventNo As Integer) As KizuLibCLI.COMMON_QUE_STRMIX
        Dim que As KizuLibCLI.COMMON_QUE_STRMIX = Nothing

        '���M�f�[�^�ݒ�
        que.inisialize()
        que.nEventNo = nEventNo
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
        que.idata(0) = m_nUntenStatus    '�^�]���[�h
        que.idata(1) = m_nPlgStatus      'PLG���[�h
        que.idata(2) = m_nKatamenStatus  '�Жʌ������s����

        '�����\�ʏ�ԃR�[�h �\
        If -1 <> cmbSCodeTop.SelectedIndex Then
            que.idata(3) = CType(cmbSCodeTop.Items(cmbSCodeTop.SelectedIndex), cls_CmbItem).sendid
        Else
            '���ۂ́A���肦�Ȃ����O�̂���
            que.idata(3) = 1
        End If

        '�����\�ʏ�ԃR�[�h ��
        If -1 <> cmbSCodeBot.SelectedIndex Then
            que.idata(4) = CType(cmbSCodeBot.Items(cmbSCodeBot.SelectedIndex), cls_CmbItem).sendid
        Else
            '���ۂ́A���肦�Ȃ����O�̂���
            que.idata(4) = 1
        End If

        '���������p�^�[���R�[�h�@�\
        If -1 <> cmbPCodeTop.SelectedIndex Then
            que.idata(5) = CType(cmbPCodeTop.Items(cmbPCodeTop.SelectedIndex), cls_CmbItem).sendid
        Else
            '���ۂ́A���肦�Ȃ����O�̂���
            que.idata(5) = 1
        End If

        '���������p�^�[���R�[�h ��
        If -1 <> cmbPCodeBot.SelectedIndex Then
            que.idata(6) = CType(cmbPCodeBot.Items(cmbPCodeBot.SelectedIndex), cls_CmbItem).sendid
        Else
            '���ۂ́A���肦�Ȃ����O�̂���
            que.idata(6) = 1
        End If

        If EM_KENSA_KIND.nNormal = nKind Then
            que.idata(7) = CType(cmbCamera1PC1.SelectedIndex, Integer)  '���W�b�N�p�������@�\01_���̓J����01
            que.idata(8) = CType(cmbCamera2PC1.SelectedIndex, Integer)  '���W�b�N�p�������@�\01_���̓J����02
            que.idata(9) = CType(cmbCamera1PC2.SelectedIndex, Integer)  '���W�b�N�p�������@�\02_���̓J����01
            que.idata(10) = CType(cmbCamera2PC2.SelectedIndex, Integer) '���W�b�N�p�������@�\02_���̓J����02
        ElseIf EM_KENSA_KIND.nPC1 = nKind Then
            que.idata(7) = nKind                                        '�����J�n�敪
            que.idata(8) = CType(cmbCamera1PC1.SelectedIndex, Integer)  '���W�b�N�p�������@�\01_���̓J����01
            que.idata(9) = CType(cmbCamera2PC1.SelectedIndex, Integer)  '���W�b�N�p�������@�\01_���̓J����02
        Else
            que.idata(7) = nKind                                        '�����J�n�敪
            que.idata(8) = CType(cmbCamera1PC2.SelectedIndex, Integer)  '���W�b�N�p�������@�\02_���̓J����01
            que.idata(9) = CType(cmbCamera2PC2.SelectedIndex, Integer)  '���W�b�N�p�������@�\02_���̓J����02
        End If

        Return que
    End Function

    ''' <summary>
    ''' ��ʃJ�����Z�b�g�ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetCamSetIndex()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        'SQL���쐬
        strSql = " SELECT ���W�b�N�p�������@�\01_���̓J����01"
        strSql &= " ,���W�b�N�p�������@�\01_���̓J����02"
        strSql &= " ,���W�b�N�p�������@�\02_���̓J����01"
        strSql &= " ,���W�b�N�p�������@�\02_���̓J����02"
        strSql &= " FROM " & DB_COMMON_MASTER

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Exit Sub

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            With sqlRead
                .Read()
                cmbCamera1PC1.SelectedIndex = GetCamSetIndex(CType(.GetValue(0), Integer))
                cmbCamera2PC1.SelectedIndex = GetCamSetIndex(CType(.GetValue(1), Integer))
                cmbCamera1PC2.SelectedIndex = GetCamSetIndex(CType(.GetValue(2), Integer))
                cmbCamera2PC2.SelectedIndex = GetCamSetIndex(CType(.GetValue(3), Integer))
            End With

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "���W�b�N�p������ ���̓J�����ݒ�擾���s")
        Finally
            If Not sqlRead Is Nothing Then sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' �����̒l����J�����Z�b�g�R���{�{�b�N�X�ɐݒ肷��Index���擾
    ''' </summary>
    ''' <param name="nValue">Index�ϊ��l</param>
    ''' <remarks></remarks>
    Private Function GetCamSetIndex(ByVal nValue As Integer) As Integer

        '�J�����Z�b�gNo�͈̔͊O�ł���Ζ��I���ɐݒ�
        If MAX_CAMSET_NO < nValue Or 0 > nValue Then
            Return 0
        End If

        Return nValue

    End Function

    ''' <summary>
    ''' �J�����Z�b�g�ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetCamSet()
        '�R���{�{�b�N�X������
        cmbCamera1PC1.Items.Clear()
        cmbCamera2PC1.Items.Clear()
        cmbCamera1PC2.Items.Clear()
        cmbCamera2PC2.Items.Clear()

        '���I�����̒l
        cmbCamera1PC1.Items.Add(" ")
        cmbCamera2PC1.Items.Add(" ")
        cmbCamera1PC2.Items.Add(" ")
        cmbCamera2PC2.Items.Add(" ")

        '�J�����Z�b�gMAX���I�����ڂ�ݒ�
        For ii As Integer = 1 To MAX_CAMSET_NO
            cmbCamera1PC1.Items.Add(ii)
            cmbCamera2PC1.Items.Add(ii)
            cmbCamera1PC2.Items.Add(ii)
            cmbCamera2PC2.Items.Add(ii)
        Next

        '�f�t�H���g�l��ݒ�
        cmbCamera1PC1.SelectedIndex = 0
        cmbCamera2PC1.SelectedIndex = 0
        cmbCamera1PC2.SelectedIndex = 0
        cmbCamera2PC2.SelectedIndex = 0

        '���݂̃J�����Z�b�g��Ԃ���ʂɐݒ�
        Call SetCamSetIndex()

    End Sub

    ''' <summary>
    ''' ���̓J�����ݒ�X�V
    ''' </summary>
    ''' <remarks></remarks>
    Private Function UpdCamSetNo(ByVal nKind As EM_KENSA_KIND) As Boolean
        Dim bResult As Boolean = True
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String

        'SQL���쐬
        strSql = "UPDATE " & DB_COMMON_MASTER
        strSql &= " SET "
        If EM_KENSA_KIND.nPC1 = nKind Or EM_KENSA_KIND.nNormal = nKind Then
            strSql &= tClass.tClass_SQLServer.sqlAddUpdate("���W�b�N�p�������@�\01_���̓J����01", cmbCamera1PC1.SelectedIndex)
            strSql &= tClass.tClass_SQLServer.sqlAddUpdate("���W�b�N�p�������@�\01_���̓J����02", cmbCamera2PC1.SelectedIndex)
        End If

        If EM_KENSA_KIND.nPC2 = nKind Or EM_KENSA_KIND.nNormal = nKind Then
            strSql &= tClass.tClass_SQLServer.sqlAddUpdate("���W�b�N�p�������@�\02_���̓J����01", cmbCamera1PC2.SelectedIndex)
            strSql &= tClass.tClass_SQLServer.sqlAddUpdate("���W�b�N�p�������@�\02_���̓J����02", cmbCamera2PC2.SelectedIndex)
        End If

        tClass.tClass_SQLServer.sqlColumnTrim(strSql)

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Return False

            'SQL���s
            If 0 > tcls_DB.DMLExecute(strSql) Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "���W�b�N�p������ ���̓J�����ݒ�X�V���s")
                bResult = False
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "���W�b�N�p������ ���̓J�����ݒ�X�V���s")
            bResult = False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return bResult

    End Function

End Class
