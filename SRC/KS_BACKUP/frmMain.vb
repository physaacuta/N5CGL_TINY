Imports tClassLibrary

Public Class frmMain

#Region "���[�J���萔"

    ''' <summary>
    ''' ���̃^�X�N��p��ini�t�@�C���p�X
    ''' </summary>
    ''' <remarks></remarks>
    Private Const mcINIFILE As String = ".\KizuBackUp.ini"

#End Region

#Region "���[�J���ϐ�"

    ''' <summary>
    ''' �������~��PC��KizuPc.ini�̕ۑ��ꏊ
    ''' </summary>
    ''' <remarks></remarks>
    Private m_iniPcIni As String = ""
    ''' <summary>
    ''' �������~��PC��KizuPc.ini�̌��ݎ��s���̏ꏊ
    ''' </summary>
    ''' <remarks></remarks>
    Private m_iniPcIniToFPath As String = ""

    ''' <summary>
    ''' �������~�ϋ@�\_A�n�����삵�Ă���PC����
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_sName_A_To As String
    ''' <summary>
    ''' �������~�ϋ@�\_B�n�����삵�Ă���PC����
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_sName_B_To As String
    ''' <summary>
    ''' �������~�ϋ@�\_A�n�����삵�Ă���PC����(���{��)
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_sNameJ_A_To As String
    ''' <summary>
    ''' �������~�ϋ@�\_B�n�����삵�Ă���PC����(���{��)
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_sNameJ_B_To As String

    ''' <summary>
    ''' �\��PC �I�t���C���n�̑䐔
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_nHyOffLineNum As Integer
    ''' <summary>
    ''' �\��PC �I�����C���n�̑䐔
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_nHyOnLineNum As Integer
    ''' <summary>
    ''' �\���I�t���C��PC�̃R���s���[�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sName_Off_Hy() As String
    ''' <summary>
    ''' �\���I�t���C��PC�̃R���s���[�^��(���{��)
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sNameJ_Off_Hy() As String
    ''' <summary>
    ''' �\���I�t���C���@�\�̖���
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sKind_Off_Hy() As String

    ''' <summary>
    ''' �\���@�\�����삵�Ă���PC����
    ''' </summary>
    ''' <remarks>�I�����C���n�̑䐔��KizuBackUp.ini����擾��ARedim����</remarks>
    Private m_sName_Hy() As String
    ''' <summary>
    ''' �\���@�\�����삵�Ă���PC����(���{��)
    ''' </summary>
    ''' <remarks>�I�����C���n�̑䐔��KizuBackUp.ini����擾��ARedim����</remarks>
    Private m_sNameJ_Hy() As String
    ''' <summary>
    ''' �\��PC�œ��삵�Ă���@�\
    ''' </summary>
    ''' <remarks>�I�����C���n�̑䐔��KizuBackUp.ini����擾��ARedim����</remarks>
    Private m_sKind_Hy() As String

    ''' <summary>
    ''' ����@�\_A�n�����삵�Ă���PC����
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_sName_A_So As String
    ''' <summary>
    ''' ����@�\_B�n�����삵�Ă���PC����
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_sName_B_So As String
    ''' <summary>
    ''' ����@�\_A�n�����삵�Ă���PC����(���{��)
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_sNameJ_A_So As String
    ''' <summary>
    ''' ����@�\_B�n�����삵�Ă���PC����(���{��)
    ''' </summary>
    ''' <remarks>�t�H�[�����[�h����KizuBackUp.ini����Ǎ�</remarks>
    Private m_sNameJ_B_So As String

    ''' <summary>
    ''' �S���摜����PC �I�����C���䐔
    ''' </summary>
    ''' <remarks></remarks>
    Private m_nRpiOnLineNum As Integer
    ''' <summary>
    ''' �S���摜����PC �I�t���C���䐔
    ''' </summary>
    ''' <remarks></remarks>
    Private m_nRpiOffLineNum As Integer
    ''' <summary>
    ''' �S���摜�����I�����C��PC����
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sName_RpiOn() As String
    ''' <summary>
    ''' �S���摜�����I�����C��PC����(���{��)
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sNameJ_RpiOn() As String
    ''' <summary>
    ''' �S���摜�����I�����C��PC�œ��삵�Ă���@�\
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sKind_RpiOn() As String
    ''' <summary>
    ''' �S���摜�����I�t���C��PC����
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sName_RpiOff() As String
    ''' <summary>
    ''' �S���摜�����I�t���C��PC����(���{��)
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sNameJ_RpiOff() As String
    ''' <summary>
    ''' �S���摜�����I�t���C��PC�œ��삵�Ă���@�\
    ''' </summary>
    ''' <remarks></remarks>
    Private m_sKind_RpiOff() As String

    Public Sub New()

        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

    End Sub

#End Region

#Region "�t�H�[�����[�h"

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        ''=============================================
        '' ini�t�@�C�����̑����珉���ݒ��Ǎ�

        '''' ���C��ID
        KizuLibCLI.KizuMod.SetLineID()


        '''' ���ʍ��ڂ�ini�t�@�C���l�ǂݍ���
        m_iniPcIni = tMod.ReadIniStr("COMMON", "PCINI", mcINIFILE)
        m_iniPcIniToFPath = tMod.ReadIniStr("COMMON", "PCINITO", mcINIFILE)



        '''' ��ʌŒ�t�H�[�}�b�g������
        ' �������~�ϋ@�\
        m_sName_A_To = tMod.ReadIniStr("TO", "PCNAME_A", mcINIFILE)
        m_sName_B_To = tMod.ReadIniStr("TO", "PCNAME_B", mcINIFILE)
        m_sNameJ_A_To = tMod.ReadIniStr("TO", "PCNAME_J_A", mcINIFILE)
        m_sNameJ_B_To = tMod.ReadIniStr("TO", "PCNAME_J_B", mcINIFILE)

        ' �\���@�\
        m_nHyOffLineNum = tMod.ReadIniInt("HY", "HY_OFF_NUM", mcINIFILE)
        ReDim m_sName_Off_Hy(m_nHyOffLineNum - 1)
        ReDim m_sNameJ_Off_Hy(m_nHyOffLineNum - 1)
        ReDim m_sKind_Off_Hy(m_nHyOffLineNum - 1)
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            m_sName_Off_Hy(ii) = tMod.ReadIniStr("HY", "PCNAME_OFF_" & ii + 1, mcINIFILE)
            m_sNameJ_Off_Hy(ii) = tMod.ReadIniStr("HY", "PCNAME_J_OFF_" & ii + 1, mcINIFILE)
            m_sKind_Off_Hy(ii) = tMod.ReadIniStr("HY", "KINDNAME_OFF_" & ii + 1, mcINIFILE)
        Next
        m_nHyOnLineNum = tMod.ReadIniInt("HY", "HY_ON_NUM", mcINIFILE)
        ReDim m_sName_Hy(m_nHyOnLineNum - 1)
        ReDim m_sNameJ_Hy(m_nHyOnLineNum - 1)
        ReDim m_sKind_Hy(m_nHyOnLineNum - 1)
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            m_sName_Hy(ii) = tMod.ReadIniStr("HY", "PCNAME_" & ii + 1, mcINIFILE)
            m_sNameJ_Hy(ii) = tMod.ReadIniStr("HY", "PCNAME_J_" & ii + 1, mcINIFILE)
            m_sKind_Hy(ii) = tMod.ReadIniStr("HY", "KINDNAME_" & ii + 1, mcINIFILE)
        Next

        ' ����@�\
        m_sName_A_So = tMod.ReadIniStr("SO", "PCNAME_A", mcINIFILE)
        m_sName_B_So = tMod.ReadIniStr("SO", "PCNAME_B", mcINIFILE)
        m_sNameJ_A_So = tMod.ReadIniStr("SO", "PCNAME_J_A", mcINIFILE)
        m_sNameJ_B_So = tMod.ReadIniStr("SO", "PCNAME_J_B", mcINIFILE)

        ' �S���摜�����@�\
        m_nRpiOnLineNum = tMod.ReadIniInt("RPI", "RPI_ON_NUM", mcINIFILE)
        ReDim m_sName_RpiOn(m_nRpiOnLineNum - 1)
        ReDim m_sNameJ_RpiOn(m_nRpiOnLineNum - 1)
        ReDim m_sKind_RpiOn(m_nRpiOnLineNum - 1)
        m_nRpiOffLineNum = tMod.ReadIniInt("RPI", "RPI_OFF_NUM", mcINIFILE)
        ReDim m_sName_RpiOff(m_nRpiOffLineNum - 1)
        ReDim m_sNameJ_RpiOff(m_nRpiOffLineNum - 1)
        ReDim m_sKind_RpiOff(m_nRpiOffLineNum - 1)

        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            m_sName_RpiOn(ii) = tMod.ReadIniStr("RPI", "PCNAME_ON_" & ii + 1, mcINIFILE)
            m_sNameJ_RpiOn(ii) = tMod.ReadIniStr("RPI", "PCNAME_J_ON_" & ii + 1, mcINIFILE)
            m_sKind_RpiOn(ii) = tMod.ReadIniStr("RPI", "KINDNAME_ON_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            m_sName_RpiOff(ii) = tMod.ReadIniStr("RPI", "PCNAME_OFF_" & ii + 1, mcINIFILE)
            m_sNameJ_RpiOff(ii) = tMod.ReadIniStr("RPI", "PCNAME_J_OFF_" & ii + 1, mcINIFILE)
            m_sKind_RpiOff(ii) = tMod.ReadIniStr("RPI", "KINDNAME_OFF_" & ii + 1, mcINIFILE)
        Next



        ''=============================================
        '' �������~�ϋ@�\
        lblTo_A.Text = m_sNameJ_A_To
        lblTo_B.Text = m_sNameJ_B_To
        btnTO_ChangeTo.Text = String.Format(btnTO_ChangeTo.Text, m_sNameJ_A_To)
        btnTO_ChangeBk.Text = String.Format(btnTO_ChangeBk.Text, m_sNameJ_B_To)



        ' ======================
        ' �\���@�\
        'btnHy_Change.Text = String.Format(btnHy_Change.Text, m_sNameJ_Off_Hy)
        'btnHy_ChangeOff.Text = String.Format(btnHy_ChangeOff.Text, m_sNameJ_Off_Hy, m_sKind_Off_Hy)
        cmbHy_Select.Items.Clear()
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            Dim cls As New tClass_CmbItem(ii, m_sKind_Hy(ii))
            cmbHy_Select.Items.Add(cls)
        Next
        cmbHy_Select.SelectedIndex = 0



        ' ======================
        ' ����@�\
        btnSO_ChangeSo.Text = String.Format(btnSO_ChangeSo.Text, m_sNameJ_A_So)
        btnSO_ChangeKa.Text = String.Format(btnSO_ChangeKa.Text, m_sNameJ_B_So)



        ' ======================
        ' �S���摜�����@�\
        ' DB�����Ԏ擾�o������A�{�^���\�����X�V����
        cmbRpi_Select.Items.Clear()
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            Dim cls As New tClass_CmbItem(ii, m_sKind_RpiOn(ii))
            cmbRpi_Select.Items.Add(cls)
        Next
        cmbRpi_Select.SelectedIndex = 0



    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h�I������1�񂾂��񓯊����s
    ''' </summary>
    ''' <remarks></remarks>
    Protected Overrides Sub LoadEnding()
        ' ���X�̃��\�b�h�����s
        MyBase.LoadEnding()
        Application.DoEvents()


        ' ��������g����
        Call CheckTo_Status()       ' �������~�ϋ@�\ ��ԃ`�F�b�N

        Call CheckHy_Status()       ' �\���@�\�����`�F�b�N

        Call CheckSo_Status()       ' ����@�\�����`�F�b�N

        Call CheckRpi_Status()      ' �S���摜�@�\�����`�F�b�N

        Application.DoEvents()      ' ��ʂ������Ƃł�悤�ɂ���

        SetMsgBox(True, "�o�b�N�A�b�v�ؑ֎x��")

    End Sub

#End Region


#Region "�������~�ϋ@�\"

    ''' <summary>
    ''' �������~�ϋ@�\ ��ԃ`�F�b�N
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CheckTo_Status()
        Dim strPCNameWk As String = tMod.ReadIniStr("TO", "PCNAME_BACK", mcINIFILE)
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC����
        If strPcName <> strPCNameWk Then
            lblTo_NowBk.Text = "�s��(����ΏۊOPC)"
            lblTo_NowBk.BackColor = Color.Tomato
        Else

            '''' ���݂̑���PC�Ŏ��s�̋@�\��c������
            Dim strPath As String = tMod.ReadIniStr("TO", "INIPC_FPATH", mcINIFILE)
            Dim nWk As Integer = tMod.ReadIniInt("NOWID", "ID", strPath)


            '''' �`�F�b�N
            If nWk = 0 Then
                lblTo_NowBk.Text = "�o�b�N�A�b�v�@�\"
                lblTo_NowBk.BackColor = Color.DodgerBlue
            ElseIf nWk = 1 Then
                lblTo_NowBk.Text = "�������~�ϋ@�\"
                lblTo_NowBk.BackColor = Color.Tomato
            Else
                lblTo_NowBk.Text = "�s��"
                lblTo_NowBk.BackColor = Color.Red
            End If

        End If



        '''' DB��Ԏ擾
        ' A�n
        Dim bWk As Boolean = tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_A)
        SetTi_Status(bWk, lblTO_DbStatusA)

        ' B�n
        bWk = tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_B)
        SetTi_Status(bWk, lblTO_DbStatusB)

    End Sub

    ''' <summary>
    ''' DB��Ԃ��Z�b�g
    ''' </summary>
    ''' <param name="bMode">true:���� false:�ُ�</param>
    ''' <remarks></remarks>
    Private Sub SetTi_Status(ByVal bMode As Boolean, ByVal lbl As Label)
        If bMode Then
            lbl.Text = "����"
            lbl.BackColor = Color.DodgerBlue
        Else
            lbl.Text = "�ُ�"
            lbl.BackColor = Color.Tomato
        End If
    End Sub

    ''' <summary>
    ''' �������~�ϋ@�\ �ؑ֎��s
    ''' </summary>
    ''' <param name="sender">�N���b�N���ꂽ�{�^��</param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnTO_ChangeBk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTO_ChangeBk.Click, btnTO_ChangeTo.Click
        Dim strMsg As String = ""           ' ���b�Z�[�W�p
        Dim strIniKey As String = INI_DB_A  ' ini�t�@�C����DB�L�[

        '''' ���s�ł���̂́A�o�b�N�A�b�vPC�̂�
        Dim strPCNameWk As String = tMod.ReadIniStr("TO", "PCNAME_BACK", mcINIFILE)
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC����
        If strPcName <> strPCNameWk Then
            MsgBox(String.Format("�{����́A{0}�Ŏ��s���Ă��������B", m_sNameJ_B_To), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If

        '''' A�n��B�n or B�n��A�n
        If sender Is btnTO_ChangeTo Then        ' B��A
            strIniKey = INI_DB_A
            strMsg = String.Format("�y{0}(���K�@)�� �������~�ϋ@�\ �����s�z", m_sNameJ_A_To)

        ElseIf sender Is btnTO_ChangeBk Then    ' A��B
            strIniKey = INI_DB_B
            strMsg = String.Format("�y{0}�� �������~�ϋ@�\ �����s�z", m_sNameJ_B_To)

        End If

        '''' �m�F
        Dim retc As MsgBoxResult = MsgBox("�������~�ϋ@�\�̐ؑւ����{���܂��B" & vbCrLf & strMsg, _
            MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "�������~�ϋ@�\�ؑ�")
        If retc <> MsgBoxResult.Ok Then Return


        ' =======================================================
        ' DB����
        ' =======================================================
        '''' ���̎擾
        Dim strSQL As String
        Dim nPcKind_A As Integer = tMod.ReadIniInt("TO", "PCKIND_A", mcINIFILE)
        Dim nPcKind_B As Integer = tMod.ReadIniInt("TO", "PCKIND_B", mcINIFILE)
        Dim nPcid_A As Integer = tMod.ReadIniInt("TO", "PCID_A", mcINIFILE)
        Dim nPcid_B As Integer = tMod.ReadIniInt("TO", "PCID_B", mcINIFILE)
        Dim nKikiid_HB_TO As Integer = tMod.ReadIniInt("TO", "KIKIDI_HB_TO", mcINIFILE)
        Dim nKikiid_HB_BK As Integer = tMod.ReadIniInt("TO", "KIKIDI_HB_BK", mcINIFILE)
        If 0 = nPcKind_A Or 0 = nPcKind_B Or 0 = nPcid_A Or 0 = nPcid_B Then
            SetMsgBox(False, "�������~�ϋ@�\�ؑւɕK�v��PC_INF��񂪑���܂���B")
            Return
        End If

        '''' PCID������
        Dim cls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, strIniKey)
        Using cls_DB
            If Not cls_DB.Open() Then
                SetMsgBox(False, "�f�[�^�x�[�X(" & strIniKey & ") �ɐڑ��ł��܂���")
                Return
            End If

            ' B�n��A�n�̂�
            If strIniKey = INI_DB_A Then
                '''' �o�b�N�A�b�v�@��PCINF�e�[�u��������
                strSQL = "UPDATE " & DB_PC_INF & " SET PCID=" & nPcid_A & " WHERE PCKIND=" & nPcKind_A
                cls_DB.DMLExecute(strSQL)
                strSQL = "UPDATE " & DB_PC_INF & " SET PCID=" & nPcid_B & " WHERE PCKIND=" & nPcKind_B
                cls_DB.DMLExecute(strSQL)
            End If

            ' A�n��B�n�̂�
            If strIniKey = INI_DB_B Then
                '''' �o�b�N�A�b�v�@��PCINF�e�[�u��������
                strSQL = "UPDATE " & DB_PC_INF & " SET PCID=" & nPcid_B & " WHERE PCKIND=" & nPcKind_A
                cls_DB.DMLExecute(strSQL)
                strSQL = "UPDATE " & DB_PC_INF & " SET PCID=" & nPcid_A & " WHERE PCKIND=" & nPcKind_B
                cls_DB.DMLExecute(strSQL)
            End If

            '''' �o�b�N�A�b�v�pPC�̃n�[�g�r�[�g�Ď�����������
            ' B�n��A�n�̂�
            If sender Is btnTO_ChangeTo And nKikiid_HB_TO <> 0 Then
                strSQL = "UPDATE " & DB_STATUS_MASTER & " SET ����m�F�L��=0, �����l=1 WHERE �@��ID=" & nKikiid_HB_TO
                cls_DB.DMLExecute(strSQL)
            End If
            If sender Is btnTO_ChangeTo And nKikiid_HB_BK <> 0 Then
                strSQL = "UPDATE " & DB_STATUS_MASTER & " SET ����m�F�L��=1, �����l=1 WHERE �@��ID=" & nKikiid_HB_BK
                cls_DB.DMLExecute(strSQL)
            End If


            ' A�n��B�n�̂�
            If sender Is btnTO_ChangeBk And nKikiid_HB_TO <> 0 Then
                strSQL = "UPDATE " & DB_STATUS_MASTER & " SET ����m�F�L��=1, �����l=0 WHERE �@��ID=" & nKikiid_HB_TO
                cls_DB.DMLExecute(strSQL)
            End If
            If sender Is btnTO_ChangeBk And nKikiid_HB_BK <> 0 Then
                strSQL = "UPDATE " & DB_STATUS_MASTER & " SET ����m�F�L��=0, �����l=0 WHERE �@��ID=" & nKikiid_HB_BK
                cls_DB.DMLExecute(strSQL)
            End If

            cls_DB.Close()
        End Using


        '�����̐� IP�A�h���X�ύX����񂾂���ADB�Z�b�V�����l�̏��������͕s�v�ł́E�E�E
        ' =======================================================
        ' �������~��PC�̑SKizuPc.ini��DB�Z�b�V�����l������
        ' =======================================================
        ''For Each strFPath As String In System.IO.Directory.GetFiles(m_iniPcIni, "*KizuPc.ini")

        ''    tMod.WriteIni("DB", "SESSION", strFPath, strIniKey)
        ''Next


        ' =======================================================
        ' �o�b�N�A�b�vPC�Ŏ��s����KizuTask.ini�̐ؑ�
        ' =======================================================
        Dim strPath As String = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("TO", "INIPC", mcINIFILE))
        '' �����̒���KizuTask.ini��؂�ւ���
        strPath = tMod.ReadIniStr("TO", "INIPC_FPATH", mcINIFILE)
        If sender Is btnTO_ChangeTo Then        'A�n
            tMod.WriteIni("NOWID", "ID", strPath, "0")
        Else
            tMod.WriteIni("NOWID", "ID", strPath, "1")
        End If



        ' ======================
        ' �G���h����
        ' ======================
        SetMsgBox(True, "�������~�ϋ@�\�ؑւ��������܂����B" & vbCrLf & "�SPC���V���b�g�_�E�����Ă��������B")
        retc = MsgBox("�������~�ϋ@�\�̐ؑւ����������܂����B" & vbCrLf & "�SPC���V���b�g�_�E�����܂��B", _
           MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "�������~�ϋ@�\�ؑ�")

        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0

        '''' �����ɃV���b�g�_�E���ʒm
        ' KS_MASTER/KS_SLAVE �̂ǂ��炪�����Ă��邩�킩��Ȃ��̂ŁA�����ɒʒm
        que.nEventNo = FACT_KS_MASTER_06
        que.nLineNo = 0
        Call KizuLibCLI.KizuMod.Send_Mail(KS_MASTER, "*", que)
        ' �S���ɃV���b�g�_�E��
        que.nEventNo = FACT_KS_SLAVE_04
        Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, "*", que)


        ''''' ���̑��SPC�ɍċN���v��
        'que.nEventNo = FACT_KS_SLAVE_05
        'que.data(0) = 0
        'Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, "*", que)


        ' KS_MASTER�����[�����M��̑҂�����[�b]�B(TO_DEFECT�̒�~������҂����킹�鎞��)
        ' ����~�������I������O��IP�A�h���X���ύX������DB�A�N�Z�X�s�ƂȂ�A�V�X�e���I��������ɍs���Ȃ��B
        '   IP�ύX�O�ɃV���b�g�_�E�����������삵�Ă��܂����߁A�҂�����-10msec�Ƃ���
        Dim nWk As Integer = tMod.ReadIniInt("KS_MASTER", "STOP_AFFTER_WAIT_TIME", strPath) * 1000 - 10
        Threading.Thread.Sleep(nWk)

        ' ======================
        ' �o�b�N�A�b�v�@��IP�A�h���X�ύX
        ' ======================
        ' �ǂ���IP�A�h���X���ς���UDP�ł����[������΂Ȃ��̂ŁA���̈ʒu�Ɉړ�������
        If sender Is btnTO_ChangeTo Then       'A�n
            tMod.SetIpAddress(tMod.ReadIniStr("TO", "IP_B", mcINIFILE), tMod.ReadIniStr("TO", "IP_ADAPTER", mcINIFILE))
        Else
            tMod.SetIpAddress(tMod.ReadIniStr("TO", "IP_A", mcINIFILE), tMod.ReadIniStr("TO", "IP_ADAPTER", mcINIFILE))
        End If

    End Sub

    ''' <summary>
    ''' B�n��A�n �}�X�^�[����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnTO_Synclo_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTO_Synclo.Click
        '''' �m�F
        Dim retc As MsgBoxResult = MsgBox("�}�X�^�[�̓��������{���܂��B", _
          MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "�������~�ϋ@�\�ؑ�")
        If retc <> MsgBoxResult.Ok Then Return

        '' �f�[�^�x�[�X���`�F�b�N
        If Not tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_A) Or Not tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_B) Then
            MsgBox(String.Format("{0} �y�� {1} �����쒆�̏ꍇ�̂݁A���s�\�ł��B", m_sNameJ_A_To, m_sNameJ_B_To))
            Return
        End If

        '' ���[����ʒm
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0

        '''' DB�����Ǘ��� �����˗�
        que.nEventNo = FACT_KS_EQALDB_01
        que.nLineNo = 0
        que.data(0) = 1     'B��A (�ʏ��`�Ƌt)
        que.data(1) = 1     '�}�X�^�[����
        Call KizuLibCLI.KizuMod.Send_Mail(KS_EQALDB, "*", que)
    End Sub

    ''' <summary>
    ''' B�n��A�n ���шړ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnTO_Move_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTO_Move.Click
        '''' �m�F
        Dim retc As MsgBoxResult = MsgBox("���т̈ړ������{���܂��B", _
         MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "�������~�ϋ@�\�ؑ�")
        If retc <> MsgBoxResult.Ok Then Return

        '' �f�[�^�x�[�X���`�F�b�N
        If Not tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_A) Or Not tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, INI_DB_B) Then
            MsgBox(String.Format("{0} �y�� {1} �����쒆�̏ꍇ�̂݁A���s�\�ł��B", m_sNameJ_A_To, m_sNameJ_B_To))
            Return
        End If

        '' ���[����ʒm
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0

        '''' �������s
        que.nEventNo = FACT_KS_EQALDB_01
        que.nLineNo = 0
        que.data(0) = 1     'B��A (�ʏ��`�Ƌt)
        que.data(1) = 2     '���шړ�
        Call KizuLibCLI.KizuMod.Send_Mail(KS_EQALDB, "*", que)
    End Sub

#End Region

#Region "�\���@�\"

    ''' <summary>
    ''' �\���@�\ ��ԃ`�F�b�N
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CheckHy_Status()
        Dim nHyOn As Integer = -1
        Dim nHyOff As Integer = -1
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC����
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            If strPcName = m_sName_Hy(ii) Then nHyOn = ii
        Next
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            If strPcName = m_sName_Off_Hy(ii) Then nHyOff = ii
        Next
        If -1 = nHyOn And -1 = nHyOff Then
            ' �I�����C���^�I�t���C���̂ǂ��炩��PC����v����͂�
            Me.lblHy_Now01.Text = "�s��(����ΏۊOPC)"
            Me.lblHy_Now01.BackColor = Color.Tomato
            'Me.lblHy_Now02.Text = "�s��(����ΏۊOPC)"
            'Me.lblHy_Now02.BackColor = Color.Tomato

            btnHy_Change.Text = String.Format(btnHy_Change.Text, m_sNameJ_Off_Hy(0))
            btnHy_ChangeOff.Text = String.Format(btnHy_ChangeOff.Text, m_sNameJ_Off_Hy(0), m_sKind_Off_Hy(0))

            Return
        End If


        '''' ���݂̕\���I�t���C��PC�Ŏ��s�̋@�\��c������
        'Dim strWk As String
        'strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF", mcINIFILE))
        'Dim nPcId_OffNow As Integer = tMod.ReadIniInt("COMMON", "PCID", strWk)

        Dim strWk As String
        Dim nPcId_OffNow(m_nHyOffLineNum - 1) As Integer       ' �I�t���C��PC�Ō��ݓ��삵�Ă���PCID
        Dim nPcId_Off(m_nHyOffLineNum - 1) As Integer          ' �I�t���C��PC��PCID
        Dim nPcId_On(m_nHyOnLineNum - 1) As Integer            ' �I�����C��PC��PCID
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF_" & ii + 1, mcINIFILE))
            nPcId_OffNow(ii) = tMod.ReadIniInt("COMMON", "PCID", strWk)
            nPcId_Off(ii) = tMod.ReadIniInt("HY", "PCID_HY_OFF_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            nPcId_On(ii) = tMod.ReadIniInt("HY", "PCID_HY_ON_" & ii + 1, mcINIFILE)
        Next


        '''' �`�F�b�N
        '  �o�b�N�A�b�v�߂����́A�I�����C��PC�����(KizuPc.ini��PCID�������)
        If -1 = nHyOff Then
            For ii As Integer = 0 To m_nHyOffLineNum - 1
                If nPcId_OffNow(ii) = nPcId_On(nHyOn) Then nHyOff = ii '�I�t���C��PC�����
        Next
        End If


        '''' �`�F�b�N
        Dim bHy As Boolean = False
        Dim lblHy_NowOff(m_nHyOffLineNum - 1) As Label
        lblHy_NowOff(0) = lblHy_Now01
        'lblHy_NowOff(1) = lblHy_Now02

        For ii As Integer = 0 To m_nHyOffLineNum - 1
            bHy = False
            For jj As Integer = 0 To m_nRpiOnLineNum - 1
                If nPcId_OffNow(ii) = nPcId_On(jj) Then
                    lblHy_NowOff(ii).Text = m_sKind_Hy(jj)
                    lblHy_NowOff(ii).BackColor = Color.Tomato

                    bHy = True
            End If
        Next
            If Not bHy Then
                lblHy_NowOff(ii).Text = m_sKind_Off_Hy(ii)
                lblHy_NowOff(ii).BackColor = Color.DodgerBlue
            End If
        Next


        ''''' �`�F�b�N
        'nHyOn = False
        'For ii As Integer = 0 To m_nHyOnLineNum - 1
        '    If nPcId_OffNow = nPcId(ii) Then
        '        lblHy_NowOff01.Text = m_sKind_Hy(ii)
        '        lblHy_NowOff01.BackColor = Color.Tomato

        '        nHyOn = True
        '    End If
        'Next
        'For ii As Integer = 0 To m_nHyOffLineNum - 1
        '    If nPcId_OffNow = nPcId(ii) Then
        '        lblHy_NowOff01.Text = m_sKind_Off_Hy(ii)
        '        lblHy_NowOff01.BackColor = Color.DodgerBlue
        '    End If
        'Next

        If nHyOff <> -1 Then
            ' �{�^���̕\���������X�V
            btnHy_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_Off_Hy(nHyOff))
            btnHy_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_Off_Hy(nHyOff), m_sKind_Off_Hy(nHyOff))
        Else
            btnHy_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_Off_Hy(0))
            btnHy_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_Off_Hy(0), m_sKind_Off_Hy(0))
        End If

    End Sub

    ''' <summary>
    ''' �\���@�\ �ؑ֎��s(B�n��A�n)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnHy_ChangeOff_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHy_ChangeOff.Click
        Call ExecHyChange(-1)
    End Sub
    ''' <summary>
    ''' �\���@�\ �ؑ֎��s(A�n��B�n)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnHy_Change_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHy_Change.Click
        Dim nId As Integer = cmbHy_Select.SelectedIndex
        Call ExecHyChange(nId)
    End Sub

    ''' <summary>
    ''' �\���ؑ�
    ''' </summary>
    ''' <param name="nId">-1:�\��  0�`:�I�����C���n</param>
    ''' <remarks></remarks>
    Private Sub ExecHyChange(ByVal nId As Integer)
        Dim strMsg As String = ""                       ' ���b�Z�[�W�p
        Dim strSQL As String
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing


        '''' ���s�ł���̂́A�\���I�t���C��PC�̂�
        Dim nHyOn As Integer = -1
        Dim nHyOff As Integer = -1
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC����
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            If strPcName = m_sName_Hy(ii) Then nHyOn = ii
        Next
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            If strPcName = m_sName_Off_Hy(ii) Then nHyOff = ii
        Next
        If (-1 = nHyOff And -1 <> nId) Or (-1 = nHyOn And -1 = nId) Then
            'MsgBox(String.Format("�{����́A{0}�A�܂���{1}�Ŏ��s���Ă��������B", m_sNameJ_Off_Hy(0), m_sNameJ_Off_Hy(1)), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            MsgBox(String.Format("�{����́A{0}�Ŏ��s���Ă��������B", m_sNameJ_Off_Hy(0)), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If

         

        '''' ���݂̕\���I�t���C��PC�Ŏ��s�̋@�\��c������
        Dim strWk As String
        Dim bRpi As Boolean = False                             ' �ؑ֎��s�̉۔��f(True:�ؑ֍� False:�֖ؑ�)
        Dim nPcId_OffNow(m_nHyOffLineNum - 1) As Integer       ' �I�t���C��PC�Ō��ݓ��삵�Ă���PCID
        Dim nPcId_Off(m_nHyOffLineNum - 1) As Integer          ' �I�t���C��PC��PCID
        Dim nPcId_On(m_nHyOnLineNum - 1) As Integer            ' �I�����C��PC��PCID
        For ii As Integer = 0 To m_nHyOffLineNum - 1
            strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF_" & ii + 1, mcINIFILE))
            nPcId_OffNow(ii) = tMod.ReadIniInt("COMMON", "PCID", strWk)
            nPcId_Off(ii) = tMod.ReadIniInt("HY", "PCID_HY_OFF_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nHyOnLineNum - 1
            nPcId_On(ii) = tMod.ReadIniInt("HY", "PCID_HY_ON_" & ii + 1, mcINIFILE)
        Next
        Dim nGroupNoOff As Integer = tMod.ReadIniInt("HY", "GROUPNO_OFF_" & nHyOff + 1, mcINIFILE)
        Dim nGroupNoOn As Integer = tMod.ReadIniInt("HY", "GROUPNO_ON_" & nId + 1, mcINIFILE)


        '''' �`�F�b�N(PCID�ɂĎ��{)
        '  �o�b�N�A�b�v�ؑ֎��́A�I�����C��PC�����(�ؑ֐�Ɏw�肵��PC�������)
        '  �o�b�N�A�b�v�߂����́A�I�����C��PC�����(KizuPc.ini��PCID�������)
        If -1 = nId Then        '�o�b�N�A�b�v�߂�
            If -1 <> nHyOn Then
                nHyOff = -1        'PCID�ōă`�F�b�N
                For ii As Integer = 0 To m_nHyOffLineNum - 1
                    If nPcId_OffNow(ii) = nPcId_On(nHyOn) Then nHyOff = ii '�I�t���C��PC�����
                Next
            End If
        Else                    '�o�b�N�A�b�v�ؑ�
            If -1 <> nHyOff Then
                If nPcId_OffNow(nHyOff) = nPcId_Off(nHyOff) Then
                    ' �o�b�N�A�b�v�֖ؑ����{���̂ݐ؂�ւ�����
                    nHyOn = nId    '�I�����C��PC
                    bRpi = False
                    For ii As Integer = 0 To m_nHyOffLineNum - 1
                        If nPcId_OffNow(ii) = nPcId_On(nHyOn) Then bRpi = True
                    Next

                    Dim nGroupNoDb As Integer           ' DB�擾��GroupNo
                    ' �o�b�N�A�b�v�֑ؑΏ�PC������PC�Ƃ��ē��쒆�͐ؑ֕s��
                    Using cls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
                        If Not cls_DB.Open() Then
                            SetMsgBox(False, "�f�[�^�x�[�X�ɐڑ��ł��܂���")
                            Return
                        End If

                        '''' PCINF�e�[�u��������
                        strSQL = "SELECT GroupNo FROM " & DB_PC_INF & " WHERE PCID=" & nPcId_OffNow(nHyOff)
                        sqlRead = cls_DB.SelectExecute(strSQL)
                        If sqlRead Is Nothing Then bRpi = True ' �ؑ֕s��
                        Try
                            ' �f�[�^�\��
                            sqlRead.Read()

                            nGroupNoDb = sqlRead.GetValue(0)
                            If nGroupNoOff <> nGroupNoDb Then bRpi = True ' �ؑ֕s��
                        Catch ex As Exception

                        Finally

                            ' DB�J��
                            If Not sqlRead Is Nothing Then sqlRead.Close()
                            Call cls_DB.Dispose()

                        End Try

                    End Using

                    If bRpi Then nHyOff = -1 ' �֑ؑΏۂ̃I�����C��PC�͊��ɗL��


                Else
                    nHyOn = -1     '�ؑ֍ς�
                End If
            End If
        End If

        If -1 = nHyOn Or -1 = nHyOff Then
            If -1 = nHyOn Then strMsg = String.Format("{0}�́A�ؑ֍ς݂ł��B�o�b�N�A�b�v�߂����{��Ɏ��s���Ă��������B", m_sNameJ_Off_Hy(nHyOff))
            If -1 = nHyOff Then strMsg = String.Format("{0}�́A�ؑ֍ς݂ł��B�֑ؑΏۂ��m�F��A���s���Ă��������B", m_sNameJ_Hy(nHyOn))
            MsgBox(strMsg, MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If


        '''' �ǂ�
        If -1 = nId Then        '�\��
            strMsg = String.Format("�y{0} �� {1} �����s�z", m_sNameJ_Off_Hy(nHyOff), m_sKind_Off_Hy(nHyOff))
        Else
            strMsg = String.Format("�y{0} �� {1} �����s�z", m_sNameJ_Off_Hy(nHyOff), m_sKind_Hy(nId))
        End If


        '''' �m�F
        Dim retc As MsgBoxResult = MsgBox("�\���@�\�̐ؑւ����{���܂��B" & vbCrLf & strMsg, _
           MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "�\���@�\�ؑ�")
        If retc <> MsgBoxResult.Ok Then Return




        ' ======================
        ' �\���I�t���C��PC�pKizuPc.ini��ύX
        ' ======================
        'Dim strTo As String
        'Dim strFrom As String
        'strTo = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF", mcINIFILE))
        'If -1 = nId Then        '�\��
        '    strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_FROM_OFF", mcINIFILE))
        'Else
        '    strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_FROM_" & nId + 1, mcINIFILE))
        'End If
        Dim strTo As String
        Dim strFrom As String
        strTo = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_HY_OFF_" & nHyOff + 1, mcINIFILE))
        If -1 = nId Then        '�\��
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_FROM_OFF_" & nHyOff + 1, mcINIFILE))
        Else
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("HY", "INI_FROM_" & nId + 1, mcINIFILE))
        End If

        ' KizuPc.ini��ؑ�PC�p�ŏ㏑��
        IO.File.Copy(strFrom, strTo, True)

        ' ���̕\���I�t���C��PC�ɂ������R�s�[���Ă���(������KizuPC.ini���ς���������ł́A�������オ��Ŗ�肪�������ׂ̏��u)
        IO.File.Copy(strTo, PCINI_NAME, True)


        ' ======================
        ' PC����ύX
        ' ======================
        If -1 = nId Then        '�\��
            tMod.SetNetBiosComputerName(m_sName_Off_Hy(nHyOff))
        Else
            tMod.SetNetBiosComputerName(m_sName_Hy(nId))
        End If


        ' ======================
        ' �G���h����
        ' ======================
        SetMsgBox(True, "�\���@�\�ؑւ��������܂����B" & vbCrLf & m_sNameJ_Off_Hy(nHyOff) & "���V���b�g�_�E�����Ă��������B")
        retc = MsgBox("�\���@�\�̐ؑւ����������܂����B" & vbCrLf & m_sNameJ_Off_Hy(nHyOff) & "���V���b�g�_�E�����܂��B",
           MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "�\���@�\�ؑ�")

        '''' �����ɃV���b�g�_�E���ʒm
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0
        que.nEventNo = FACT_KS_SLAVE_04
        Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, ".", que)

    End Sub

#End Region

#Region "����@�\"

    ''' <summary>
    ''' ����@�\ ��ԃ`�F�b�N
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CheckSo_Status()
        Dim strPCNameA As String
        strPCNameA = tMod.ReadIniStr("SO", "PCNAME_A", mcINIFILE)
        Dim strPCNameB As String
        strPCNameB = tMod.ReadIniStr("SO", "PCNAME_B", mcINIFILE)
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC����
        If strPcName <> strPCNameA And strPcName <> strPCNameB Then
            lblSo_NowKa.Text = "�s��(����ΏۊOPC)"
            lblSo_NowKa.BackColor = Color.Tomato
            Return
        End If

        '''' ���PC�Ŏ��s���̋@�\�͂ǂ�H
        Dim strPath As String = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("SO", "INI_TO", mcINIFILE))

        Dim nPcId_Now As Integer = tMod.ReadIniInt("COMMON", "PCID", strPath)
        Dim nPcId_A As Integer = tMod.ReadIniInt("SO", "PCID_A", mcINIFILE)

        '''' �`�F�b�N
        If nPcId_Now = nPcId_A Then
            lblSo_NowKa.Text = "����@�\"
            lblSo_NowKa.BackColor = Color.Tomato
        Else
            lblSo_NowKa.Text = "��͋@�\"
            lblSo_NowKa.BackColor = Color.DodgerBlue
        End If

    End Sub

    ''' <summary>
    ''' ����@�\ �ؑ֎��s
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSo_Change_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSO_ChangeSo.Click, btnSO_ChangeKa.Click
        Dim strMsg As String = ""                       ' ���b�Z�[�W�p

        '''' ���s�ł���̂́A���PC�̂�
        Dim strPCNameA As String
        strPCNameA = tMod.ReadIniStr("SO", "PCNAME_A", mcINIFILE)
        Dim strPCNameB As String
        strPCNameB = tMod.ReadIniStr("SO", "PCNAME_B", mcINIFILE)
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC����
        If strPcName <> strPCNameA And strPcName <> strPCNameB Then
            MsgBox(String.Format("�{����́A{0}�Ŏ��s���Ă��������B", m_sNameJ_B_So), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If

        '''' �ǂ�
        If sender Is btnSO_ChangeSo Then        'A�n
            strMsg = String.Format("�y{0} �� ����@�\ �����s�z", m_sNameJ_A_So)
        Else                                    'B�n
            strMsg = String.Format("�y{0} �� ����@�\ �����s�z", m_sNameJ_B_So)
        End If

        '''' �m�F
        Dim retc As MsgBoxResult = MsgBox("����@�\�̐ؑւ����{���܂��B" & vbCrLf & strMsg, _
           MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "����@�\�ؑ�")
        If retc <> MsgBoxResult.Ok Then Return

        ' ======================
        ' ���PC�pKizuPc.ini��ύX
        ' ======================
        Dim strTo As String
        Dim strFrom As String
        strTo = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("SO", "INI_TO", mcINIFILE))
        If sender Is btnSO_ChangeSo Then        'A�n
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("SO", "INI_FROM_B", mcINIFILE))
        Else                                    'B�n
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("SO", "INI_FROM_A", mcINIFILE))
        End If

        ' KizuPc.ini��ؑ�PC�p�ŏ㏑��
        IO.File.Copy(strFrom, strTo, True)

        ' ���̉��PC�ɂ������R�s�[���Ă���(������KizuPC.ini���ς���������ł́A�������オ��Ŗ�肪�������ׂ̏��u)
        IO.File.Copy(strTo, PCINI_NAME, True)

        ' ======================
        ' PC����ύX
        ' ======================
        If sender Is btnSO_ChangeSo Then        'A�n
            tMod.SetNetBiosComputerName(m_sName_B_So)
        Else                                    'B�n
            tMod.SetNetBiosComputerName(m_sName_A_So)
        End If

        ' ======================
        ' �G���h����
        ' ======================
        SetMsgBox(True, "����@�\�ؑւ��������܂����B" & vbCrLf & m_sNameJ_B_So & "���V���b�g�_�E�����Ă��������B")
        retc = MsgBox("����@�\�̐ؑւ����������܂����B" & vbCrLf & m_sNameJ_B_So & "���V���b�g�_�E�����܂��B", _
           MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "����@�\�ؑ�")

        '''' �����ɃV���b�g�_�E���ʒm
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
        que.nEventNo = FACT_KS_SLAVE_04
        Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, ".", que)

    End Sub

#End Region

#Region "�S���摜�����@�\"

    ''' <summary>
    ''' �S���摜�����@�\ ��ԃ`�F�b�N
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CheckRpi_Status()
        '''' ���s�ł���̂́A����o�b�N�A�b�vPC�̂�
        ' PC���̂�����{PC�����
        '  �o�b�N�A�b�v�ؑ֎��́A�I�t���C��PC�����
        '  �o�b�N�A�b�v�߂����́A�I�����C��PC�����
        Dim nRpiOn As Integer = -1
        Dim nRpiOff As Integer = -1
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC����
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            If strPcName = m_sName_RpiOn(ii) Then nRpiOn = ii
        Next
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            If strPcName = m_sName_RpiOff(ii) Then nRpiOff = ii
        Next
        If -1 = nRpiOn And -1 = nRpiOff Then
            lblRpi_Now.Text = "�s��(����ΏۊOPC)"
            lblRpi_Now.BackColor = Color.Tomato
            btnRpi_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_RpiOff(0))
            btnRpi_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_RpiOff(0), m_sKind_RpiOff(0))

            Return
        End If


        '''' ���݂̑S���摜�I�t���C��PC�Ŏ��s�̋@�\��c������
        Dim strWk As String
        Dim nPcId_OffNow(m_nRpiOffLineNum - 1) As Integer       ' �I�t���C��PC�Ō��ݓ��삵�Ă���PCID
        Dim nPcId_Off(m_nRpiOffLineNum - 1) As Integer          ' �I�t���C��PC��PCID
        Dim nPcId_On(m_nRpiOnLineNum - 1) As Integer            ' �I�����C��PC��PCID
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_RPI_OFF_" & ii + 1, mcINIFILE))
            nPcId_OffNow(ii) = tMod.ReadIniInt("COMMON", "PCID", strWk)
            nPcId_Off(ii) = tMod.ReadIniInt("RPI", "PCID_RPI_OFF_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            nPcId_On(ii) = tMod.ReadIniInt("RPI", "PCID_RPI_ON_" & ii + 1, mcINIFILE)
        Next


        '''' �`�F�b�N
        '  �o�b�N�A�b�v�߂����́A�I�����C��PC�����(KizuPc.ini��PCID�������)
        If -1 = nRpiOff Then
            For ii As Integer = 0 To m_nRpiOffLineNum - 1
                If nPcId_OffNow(ii) = nPcId_On(nRpiOn) Then nRpiOff = ii '�I�t���C��PC�����
            Next
        End If


        '''' �`�F�b�N
        Dim bRpi As Boolean = False
        Dim lblRpi_NowOff(m_nRpiOffLineNum - 1) As Label
        lblRpi_NowOff(0) = lblRpi_Now

        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            bRpi = False
            For jj As Integer = 0 To m_nRpiOnLineNum - 1
                If nPcId_OffNow(ii) = nPcId_On(jj) Then
                    lblRpi_NowOff(ii).Text = m_sKind_RpiOn(jj)
                    lblRpi_NowOff(ii).BackColor = Color.Tomato

                    bRpi = True
                End If
            Next
            If Not bRpi Then
                lblRpi_NowOff(ii).Text = m_sKind_RpiOff(ii)
                lblRpi_NowOff(ii).BackColor = Color.DodgerBlue
            End If
        Next

        If nRpiOff <> -1 Then
            ' �{�^���̕\���������X�V
            btnRpi_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_RpiOff(nRpiOff))
            btnRpi_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_RpiOff(nRpiOff), m_sKind_RpiOff(nRpiOff))
        Else
            btnRpi_Change.Text = String.Format(btnRpi_Change.Text, m_sNameJ_RpiOff(0))
            btnRpi_ChangeOff.Text = String.Format(btnRpi_ChangeOff.Text, m_sNameJ_RpiOff(0), m_sKind_RpiOff(0))
        End If

    End Sub

    ''' <summary>
    ''' �S���摜�����@�\ �ؑ֎��s(B�n��A�n)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRpi_ChangeOff_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRpi_ChangeOff.Click
        Call ExecRpiChange(sender, e, -1)
    End Sub
    ''' <summary>
    ''' �S���摜�����@�\ �ؑ֎��s(A�n��B�n)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRpi_Change_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRpi_Change.Click
        Dim nId As Integer = cmbRpi_Select.SelectedIndex
        Call ExecRpiChange(sender, e, nId)
    End Sub

    ''' <summary>
    ''' �S���摜�ؑ�
    ''' </summary>
    ''' <param name="nId">-1:�\��  0�`:�I�����C���n</param>
    ''' <remarks></remarks>
    Private Sub ExecRpiChange(ByVal sender As System.Object, ByVal e As System.EventArgs, ByVal nId As Integer)
        Dim strMsg As String = ""                       ' ���b�Z�[�W�p
        Dim strSQL As String
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing


        '''' ���s�ł���̂́A����o�b�N�A�b�vPC�̂�
        ' PC���̂�����{PC�����
        '  �o�b�N�A�b�v�ؑ֎��́A�I�t���C��PC�����
        '  �o�b�N�A�b�v�߂����́A�I�����C��PC�����
        Dim nRpiOn As Integer = -1
        Dim nRpiOff As Integer = -1
        Dim strPcName As String = tMod.tMod_Common.GetNetBiosComputerName()     ' PC����
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            If strPcName = m_sName_RpiOn(ii) Then nRpiOn = ii
        Next
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            If strPcName = m_sName_RpiOff(ii) Then nRpiOff = ii
        Next
        If (-1 = nRpiOff And -1 <> nId) Or (-1 = nRpiOn And -1 = nId) Then
            MsgBox(String.Format("�{����́A{0}�Ŏ��s���Ă��������B", m_sNameJ_RpiOff(0)), MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If


        '''' ���݂̑S���摜�I�t���C��PC�Ŏ��s�̋@�\��c������
        Dim strWk As String
        Dim bRpi As Boolean = False                             ' �ؑ֎��s�̉۔��f(True:�ؑ֍� False:�֖ؑ�)
        Dim nPcId_OffNow(m_nRpiOffLineNum - 1) As Integer       ' �I�t���C��PC�Ō��ݓ��삵�Ă���PCID
        Dim nPcId_Off(m_nRpiOffLineNum - 1) As Integer          ' �I�t���C��PC��PCID
        Dim nPcId_On(m_nRpiOnLineNum - 1) As Integer            ' �I�����C��PC��PCID
        For ii As Integer = 0 To m_nRpiOffLineNum - 1
            strWk = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_RPI_OFF_" & ii + 1, mcINIFILE))
            nPcId_OffNow(ii) = tMod.ReadIniInt("COMMON", "PCID", strWk)
            nPcId_Off(ii) = tMod.ReadIniInt("RPI", "PCID_RPI_OFF_" & ii + 1, mcINIFILE)
        Next
        For ii As Integer = 0 To m_nRpiOnLineNum - 1
            nPcId_On(ii) = tMod.ReadIniInt("RPI", "PCID_RPI_ON_" & ii + 1, mcINIFILE)
        Next
        Dim nGroupNoOff As Integer = tMod.ReadIniInt("RPI", "GROUPNO_OFF_" & nRpiOff + 1, mcINIFILE)
        Dim nGroupNoOn As Integer = tMod.ReadIniInt("RPI", "GROUPNO_ON_" & nId + 1, mcINIFILE)


        '''' �`�F�b�N(PCID�ɂĎ��{)
        '  �o�b�N�A�b�v�ؑ֎��́A�I�����C��PC�����(�ؑ֐�Ɏw�肵��PC�������)
        '  �o�b�N�A�b�v�߂����́A�I�����C��PC�����(KizuPc.ini��PCID�������)
        If -1 = nId Then        '�o�b�N�A�b�v�߂�
            If -1 <> nRpiOn Then
                nRpiOff = -1        'PCID�ōă`�F�b�N
                For ii As Integer = 0 To m_nRpiOffLineNum - 1
                    If nPcId_OffNow(ii) = nPcId_On(nRpiOn) Then nRpiOff = ii '�I�t���C��PC�����
                Next
            End If
        Else                    '�o�b�N�A�b�v�ؑ�
            If -1 <> nRpiOff Then
                If nPcId_OffNow(nRpiOff) = nPcId_Off(nRpiOff) Then
                    ' �o�b�N�A�b�v�֖ؑ����{���̂ݐ؂�ւ�����
                    nRpiOn = nId    '�I�����C��PC
                    bRpi = False
                    For ii As Integer = 0 To m_nRpiOffLineNum - 1
                        If nPcId_OffNow(ii) = nPcId_On(nRpiOn) Then bRpi = True
                    Next

                    Dim nGroupNoDb As Integer           ' DB�擾��GroupNo
                    ' �o�b�N�A�b�v�֑ؑΏ�PC������PC�Ƃ��ē��쒆�͐ؑ֕s��
                    Using cls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
                        If Not cls_DB.Open() Then
                            SetMsgBox(False, "�f�[�^�x�[�X�ɐڑ��ł��܂���")
                            Return
                        End If

                        '''' PCINF�e�[�u��������
                        strSQL = "SELECT GroupNo FROM " & DB_PC_INF & " WHERE PCID=" & nPcId_OffNow(nRpiOff)
                        sqlRead = cls_DB.SelectExecute(strSQL)
                        If sqlRead Is Nothing Then bRpi = True ' �ؑ֕s��
                        Try
                            ' �f�[�^�\��
                            sqlRead.Read()

                            nGroupNoDb = sqlRead.GetValue(0)
                            If nGroupNoOff <> nGroupNoDb Then bRpi = True ' �ؑ֕s��
                        Catch ex As Exception

                        Finally

                            ' DB�J��
                            If Not sqlRead Is Nothing Then sqlRead.Close()
                            Call cls_DB.Dispose()

                        End Try

                    End Using

                    If bRpi Then nRpiOff = -1 ' �֑ؑΏۂ̃I�����C��PC�͊��ɗL��


                Else
                    nRpiOn = -1     '�ؑ֍ς�
                End If
            End If
        End If

        If -1 = nRpiOn Or -1 = nRpiOff Then
            If -1 = nRpiOn Then strMsg = String.Format("{0}�́A�ؑ֍ς݂ł��B�o�b�N�A�b�v�߂����{��Ɏ��s���Ă��������B", m_sNameJ_RpiOff(nRpiOff))
            If -1 = nRpiOff Then strMsg = String.Format("{0}�́A�ؑ֍ς݂ł��B�֑ؑΏۂ��m�F��A���s���Ă��������B", m_sNameJ_RpiOn(nRpiOn))
            MsgBox(strMsg, MsgBoxStyle.OkOnly Or MsgBoxStyle.Critical)
            Return
        End If



        '''' �ǂ�
        If -1 = nId Then        '�\��
            strMsg = String.Format("�y{0} �� {1} �����s�z", m_sNameJ_RpiOff(nRpiOff), m_sKind_RpiOff(nRpiOff))
        Else
            strMsg = String.Format("�y{0} �� {1} �����s�z", m_sNameJ_RpiOff(nRpiOff), m_sKind_RpiOn(nId))
        End If


        '''' �m�F
        Dim retc As MsgBoxResult = MsgBox("�S���摜�@�\�̐ؑւ����{���܂��B" & vbCrLf & strMsg,
           MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "�S���摜�@�\�ؑ�")
        If retc <> MsgBoxResult.Ok Then Return

        Dim nPcId_A As Integer = nPcId_Off(nRpiOff)     '�֑ؑΏۂ̃I�t���C��PCID

        Using cls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
            If Not cls_DB.Open() Then
                SetMsgBox(False, "�f�[�^�x�[�X�ɐڑ��ł��܂���")
                Return
            End If

            '''' PCID������
            If -1 = nId Then        '�o�b�N�A�b�v�߂�
                '''' PCINF�e�[�u��������
                strSQL = "UPDATE " & DB_PC_INF & " SET GroupNo=" & nGroupNoOff & " WHERE PCID=" & nPcId_A
                cls_DB.DMLExecute(strSQL)
            Else                    '�o�b�N�A�b�v�ؑ�
                '''' PCINF�e�[�u��������
                strSQL = "UPDATE " & DB_PC_INF & " SET GroupNo=" & nGroupNoOn & " WHERE PCID=" & nPcId_A
                cls_DB.DMLExecute(strSQL)
            End If

            cls_DB.Close()
        End Using



        ' ======================
        ' �S���摜�I�t���C��PC�pKizuPc.ini��ύX
        ' ======================
        Dim strTo As String
        Dim strFrom As String
        strTo = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_RPI_OFF_" & nRpiOff + 1, mcINIFILE))
        If -1 = nId Then        '�\��
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_FROM_OFF_" & nRpiOff + 1, mcINIFILE))
        Else
            strFrom = IO.Path.Combine(m_iniPcIni, tMod.ReadIniStr("RPI", "INI_FROM_ON_" & nId + 1, mcINIFILE))
        End If

        ' KizuPc.ini��ؑ�PC�p�ŏ㏑��
        IO.File.Copy(strFrom, strTo, True)

        ' ���̑S���摜�I�t���C��PC�ɂ������R�s�[���Ă���(������KizuPC.ini���ς���������ł́A�������オ��Ŗ�肪�������ׂ̏��u)
        IO.File.Copy(strTo, PCINI_NAME, True)


        ' ======================
        ' PC����ύX
        ' ======================
        If -1 = nId Then        '�\��
            tMod.SetNetBiosComputerName(m_sName_RpiOff(nRpiOff))
        Else
            tMod.SetNetBiosComputerName(m_sName_RpiOn(nId))
        End If


        ' ======================
        ' �G���h����
        ' ======================
        SetMsgBox(True, "�S���摜�@�\�ؑւ��������܂����B" & vbCrLf & m_sNameJ_RpiOff(nRpiOff) & "���V���b�g�_�E�����Ă��������B")
        retc = MsgBox("�S���摜�@�\�̐ؑւ����������܂����B" & vbCrLf & m_sNameJ_RpiOff(nRpiOff) & "���V���b�g�_�E�����܂��B",
           MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "�S���摜�@�\�ؑ�")

        '''' �����ɃV���b�g�_�E���ʒm
        Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
        que.inisialize()
        que.nLineNo = 0
        que.nEventNo = FACT_KS_SLAVE_04
        Call KizuLibCLI.KizuMod.Send_Mail(KS_SLAVE, ".", que)


        ' =======================================================
        ' �o�b�N�A�b�vPC�Ŏ��s����KizuTask.ini�̐ؑ�
        ' =======================================================
        Dim strPath As String
        '' �����̒���KizuTask.ini��؂�ւ���
        strPath = tMod.ReadIniStr("TO", "INIPC_FPATH", mcINIFILE)

        ' ======================
        ' �o�b�N�A�b�v�@��IP�A�h���X�ύX
        ' ======================
        ' �ǂ���IP�A�h���X���ς���UDP�ł����[������΂Ȃ��̂ŁA���̈ʒu�Ɉړ�������
        Dim jj As Integer = 1
        While True
            Dim sAdapterNameas As String = ""
            Dim sIpAddress As String = ""
            sAdapterNameas = tMod.ReadIniStr("RPI", "IP_ADAPTER_" & jj, mcINIFILE)
            If sAdapterNameas = "" Then Exit While

            '''' IP�Z�b�g
            ' IP�擾
            If sender Is btnRpi_ChangeOff Then
                sIpAddress = tMod.ReadIniStr("RPI", "IP_OFF_" & nRpiOff + 1 & "_" & jj & "_1", mcINIFILE)
            Else
                sIpAddress = tMod.ReadIniStr("RPI", "IP_ON_" & nRpiOn + 1 & "_" & jj & "_1", mcINIFILE)
            End If
            If sIpAddress = "" Then Exit While

            tMod.SetIpAddress(sIpAddress, sAdapterNameas)

            Dim kk As Integer = 2
            While True
                sIpAddress = ""

                '''' IP�ǉ�
                ' IP�擾
                If sender Is btnRpi_ChangeOff Then
                    sIpAddress = tMod.ReadIniStr("RPI", "IP_OFF_" & nRpiOff + 1 & "_" & jj & "_" & kk, mcINIFILE)
                Else
                    sIpAddress = tMod.ReadIniStr("RPI", "IP_ON_" & nRpiOn + 1 & "_" & jj & "_" & kk, mcINIFILE)
                End If
                If sIpAddress = "" Then Exit While

                AddIpAddress(sIpAddress, sAdapterNameas)

                kk += 1
            End While
            jj += 1
        End While


    End Sub

#End Region


    ''' <summary>
    ''' ���b�Z�[�W�{�b�N�X�Ƀ��b�Z�[�W��������
    ''' </summary>
    ''' <param name="bln">���b�Z�[�W�F[True:��/False:��]</param>
    ''' <param name="strMsg">���b�Z�[�W���e</param>
    ''' <remarks></remarks>
    Private Sub SetMsgBox(ByVal bln As Boolean, ByVal strMsg As String)
        lblMsgBox.Text = strMsg
        If bln Then
            lblMsgBox.ForeColor = Color.Black
        Else
            lblMsgBox.ForeColor = Color.Red
        End If
        System.Windows.Forms.Application.DoEvents()
    End Sub

    ''' <summary>
    ''' IP�A�h���X��ǉ�����
    ''' </summary>
    ''' <param name="Ip">�ύX���s��IP�A�h���X (127.0.0.1�`��)</param>
    ''' <param name="AdapterName">�ύX�Ώۂ̃A�_�v�^���� (�l�b�g���[�N�ڑ��v���p�e�B�ŕ\������Ă��閼�̂��w��)</param>
    ''' <param name="SubNetMask">�ύX���s���T�u�l�b�g�}�X�N</param>
    Public Sub AddIpAddress(ByVal Ip As String, Optional ByVal AdapterName As String = "���[�J�� �G���A�ڑ�", Optional ByVal SubNetMask As String = "255.255.255.0")
        Dim strCmd As String = ""        ' ��������DOS�R�}���h

        '''' �R�}���h����
        strCmd = "netsh interface ip add "
        strCmd &= "address name = """ & AdapterName & """ "         '�Ώۂ̓z
        strCmd &= "addr=" & Ip & " "
        strCmd &= "mask=" & SubNetMask & ""

        '''' �R�}���h���s
        Shell(strCmd, AppWinStyle.Hide)
    End Sub

    ''' <summary>
    ''' IP�A�h���X���폜����
    ''' </summary>
    ''' <param name="Ip">�ύX���s��IP�A�h���X (127.0.0.1�`��)</param>
    ''' <param name="AdapterName">�ύX�Ώۂ̃A�_�v�^���� (�l�b�g���[�N�ڑ��v���p�e�B�ŕ\������Ă��閼�̂��w��)</param>
    Public Sub DeleteIpAddress(ByVal Ip As String, Optional ByVal AdapterName As String = "���[�J�� �G���A�ڑ�")
        Dim strCmd As String = ""        ' ��������DOS�R�}���h

        '''' �R�}���h����
        strCmd = "netsh interface ip delete "
        strCmd &= "address name = """ & AdapterName & """ "         '�Ώۂ̓z
        strCmd &= "addr=" & Ip & " "

        '''' �R�}���h���s
        Shell(strCmd, AppWinStyle.Hide)
    End Sub

    ''' <summary>
    ''' ��� ���悤�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMenu_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' ���C���X���b�h�I���ʒm
        modMain.TaskEnd()
    End Sub

End Class
