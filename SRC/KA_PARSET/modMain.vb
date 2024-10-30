'*********************************************************************************
' �G���g���[
'	[Ver]
'		Ver.01    2012/05/08  �V�K�쐬
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports System.Reflection

Public Module modMain

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Enum EM_FRM_ID
        emfrmTypeGr = 0             ' �r��E�O���[�h�ݒ�
        emfrmCommonMaster           ' �萔�ݒ�
        emfrmCommonAlarm            ' �x��o�͐ݒ�
        emfrmSCodeMaster            ' �����\�ʏ��Ͻ���o�^
        emfrmSCodeJudge             ' �����\�ʏ�ԏ����ݒ�
        emfrmSCodeOutput            ' �����\�ʏ�ԕʏo�͐ݒ�
        emfrmPCodeMaster            ' ���������Ͻ���o�^
        emfrmPCodeJudge             ' ��������ݏ����ݒ�
        emfrmPCodeOutput            ' ��������ݕʏo�͐ݒ�
        emfrmPCommonOutPut          ' ��������݌ŗL�o�͐ݒ�
        emfrmAllCodeOutput          ' �d�匇�ו\���ݒ�
        emfrmMaeTypeGr              ' �O�H���ݒ�
        emfrmStatusMaster           ' �@��}�X�^�[�ݒ�
        emfrmLogMaster              ' ���O�}�X�^�[�ݒ�   
        emfrmCameraLink             ' �J���������N�ݒ�
    End Enum

    Public Enum EM_PASSWORD_CHECK
        emPASS_CHECK_NON_NG = -1    '�p�X���[�h�m�F�Ȃ��A�ݒ�ύX�s�i�f�t�H���g)
        emPASS_CHECK_NON_OK = 0     '�p�X���[�h�m�F�Ȃ��A�ݒ�ύX�\
        emPASS_CHECK_ARI = 1        '�p�X���[�h�m�F����
    End Enum

    Public Enum EM_HARDCOPY_CHECK
        emHardCopy_Nasi = 0         '�n�[�h�R�s�[�Ȃ�
        emHardCopy_Ari              '�n�[�h�R�s�[����
    End Enum

    Public Enum EM_LOGICPARA_CHECK
        emLogicPara_Nasi = 0        '���W�b�N�p�������ݒ�Ȃ�
        emLogicPara_Ari             '���W�b�N�p�������ݒ肠��
    End Enum

    Public Enum EM_LABEL_KIND
        emONLINE = 0                '���x�����F�I�����C��
        emPARA                      '���x�����F���W�b�N�p������
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���[�o���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public g_nScodeIndex As Integer = 0         '�\�ʏ�Ԃ̃��X�g�C���f�b�N�X��ۑ�
    Public g_nPaternIndex As Integer = 0        '�����p�^�[���̃��X�g�C���f�b�N�X��ۑ�
    Public g_nMaeIndex As Integer = 0           '�O�H���̃��X�g�C���f�b�N�X��ۑ�
    Public gcls_Log As tClassLibrary.tClass.tClass_LogManager           ' ���O�Ǘ��N���X
    Public g_nDspfrmID As Integer = 0           '���ݕ\�����̉��ID

    Public g_NOA As Integer                     'NOA�ڑ��t���O (0:�I�����C��  1:���u)
    Public g_sPass As String = ""               '�p�X���[�h������ INI�t�@�C������擾
    Public g_bPassFlg As Boolean                '�p�X���[�h���͌��� (True:OK False:NG)
    Public g_iPassUmu As Integer                '�p�X���[�h�m�F�L��(0:����(�ݒ�s��) 1:����(�ݒ��) 2:�p�X���[�h�m�F�L)
    Public g_iHardCopyFlg As Integer            '�ݒ莞�̃n�[�h�R�s�[�L��(0:���� 1:���{)
    Public g_nCountIndex As Integer = 0         '�J�E���g�@�\ccode�̃��X�g�C���f�b�N�X��ۑ�
    Public g_nE1VoteIndex As Integer = 0        '�������ʘA���[�@�\�p�^�[�����X�g�C���f�b�N�X��ۑ�
    Public g_nParaFlg As Integer                '���W�b�N�p�������ݒ� (0:���W�b�N�p�������ݒ�s�� 1:���W�b�N�p�������ݒ��)

    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod               ' �����N���X �i�ʃX���b�h�Ȃ̂Œ��Ӂj

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_ap As ApplicationContext = Nothing                        ' �R���e�L�X�g

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �G���g���[�|�C���g
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        ' ��KizuLibCLI 32bit / 64bit �؂�ւ��p
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        ' �F�؏���
        If Not tNinsho.IsUserCheck(KA_PARSET) Then
            Exit Sub
        End If

        Try
            Task_First()
        Catch ex As Exception
            MessageBox.Show("KizuLibCLI.dll �� �m�F���ĉ������B" + vbCrLf + vbCrLf +
                            "---------------------------------------------------------------" + vbCrLf + vbCrLf +
                            ex.ToString, String.Format("�^�X�N���� ���s"), MessageBoxButtons.OK, MessageBoxIcon.Error)

            Exit Sub
        End Try

        '�p�X���[�h�m�F
        If EM_PASSWORD_CHECK.emPASS_CHECK_ARI = g_iPassUmu Then
            '�p�X���[�h����()
            If Windows.Forms.DialogResult.OK <> ConfPass() Then
                g_bPassFlg = False
            Else
                g_bPassFlg = True
            End If
        Else
            If EM_PASSWORD_CHECK.emPASS_CHECK_NON_OK = g_iPassUmu Then
                g_bPassFlg = True
            Else
                g_bPassFlg = False
            End If
        End If

        '�r��E�O���[�h�ݒ��ʂ�\��
        Call ShowForm(EM_FRM_ID.emfrmTypeGr)

        '' ���C���X���b�h����
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThread���R�[�������܂� �����Ńu���b�N

        Task_Last()

    End Sub

    ''' <summary>
    ''' �^�X�N������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_First()
        '���C��ID�Z�b�g
        KizuLibCLI.KizuMod.SetLineID()

        mcls_Mod = New KizuLibCLI.KizuMod

        '' �^�X�N�C�j�V����
        If 0 <> mcls_Mod.Task_Init(KA_PARSET) Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", KA_PARSET))

            End
        End If

        '���O�N���X����
        gcls_Log = New tClassLibrary.tClass.tClass_LogManager(KA_PARSET, Nothing, Nothing)
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "�p�����[�^��ʋN��")

        '�ݒ�t�@�C���Ǎ�
        Call GetIniFile()

    End Sub

    ''' <summary>
    ''' �^�X�N�I������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_Last()
        gcls_Log.Dispose()
        gcls_Log = Nothing

        ' �^�X�N�I��
        mcls_Mod.Task_Exit()

    End Sub

    ''' <summary>
    ''' �^�X�N�I������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TaskEnd()

        ' �X���b�h����������Ă��Ȃ�������I��
        If m_ap Is Nothing Then
            gcls_Log.Dispose()
            gcls_Log = Nothing
            End
        End If

        m_ap.ExitThread()                                       ' ���C���X���b�h�I��

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �t�H�[���^�C�g���ݒ�
    ''' </summary>
    ''' <param name="frmData">�t�H�[��</param>
    ''' <remarks></remarks>
    Public Sub SetTitleName(ByRef frmData As Form)

        frmData.Text = String.Format("[{0}] {1}", SYSTEM_TITLE, frmData.Text)

    End Sub

    ''' <summary>
    ''' ���j���[���X�g�ǉ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetListMenu(ByVal frm As KizuForm.frmMain_ParSet)
        frm.SetListMenu("�r��E�O���[�h�ݒ�", EM_FRM_ID.emfrmTypeGr)
        frm.SetListMenu("�萔�ݒ�", EM_FRM_ID.emfrmCommonMaster)
        frm.SetListMenu("�x��o�͐ݒ�", EM_FRM_ID.emfrmCommonAlarm)
        frm.SetListMenu("�����\�ʏ��Ͻ���o�^", EM_FRM_ID.emfrmSCodeMaster)
        frm.SetListMenu("�����\�ʏ�ԏ����ݒ�", EM_FRM_ID.emfrmSCodeJudge)
        ''frm.SetListMenu("�����\�ʏ�ԕʏo�͐ݒ�", EM_FRM_ID.emfrmSCodeOutput) '�v���r�W����
        frm.SetListMenu("���������Ͻ���o�^", EM_FRM_ID.emfrmPCodeMaster)
        frm.SetListMenu("��������ݏ����ݒ�", EM_FRM_ID.emfrmPCodeJudge)
        frm.SetListMenu("��������ݕʏo�͐ݒ�", EM_FRM_ID.emfrmPCodeOutput)
        'frm.SetListMenu("��������݌ŗL�o�͐ݒ�", EM_FRM_ID.emfrmPCommonOutPut)
        frm.SetListMenu("�d�匇�ו\���ݒ�", EM_FRM_ID.emfrmAllCodeOutput)
        frm.SetListMenu("�O�H���ݒ�", EM_FRM_ID.emfrmMaeTypeGr)
        frm.SetListMenu("�@��}�X�^�[�ݒ�", EM_FRM_ID.emfrmStatusMaster)
        frm.SetListMenu("���O�}�X�^�[�ݒ�", EM_FRM_ID.emfrmLogMaster)

        ' ���u�ȊO�̏ꍇ�̂݃J���������N�ݒ��ǉ�
        If NOA_OFFLINE <> g_NOA Then
            frm.SetListMenu("�J���������N�ݒ�", EM_FRM_ID.emfrmCameraLink)
        End If

    End Sub

    ''' <summary>
    ''' �V�X�e�������x���ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetLabelSysName(ByVal frm As frmMain_ParSet_SysName)

        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            ' �p�������\��
            frm.SetLabelSysName(EM_LABEL_KIND.emPARA)
        Else
            ' �I�����C���\��
            frm.SetLabelSysName(EM_LABEL_KIND.emONLINE)
        End If
    End Sub

    ''' <summary>
    ''' �V�X�e�������x���ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetLabelSysName(ByVal frm As frmMain_ParSet_UplList_SysName)

        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            ' �p�������\��
            frm.SetLabelSysName(EM_LABEL_KIND.emPARA)
        Else
            ' �I�����C���\��
            frm.SetLabelSysName(EM_LABEL_KIND.emONLINE)
        End If
    End Sub

    ''' <summary>
    ''' ��ʕ\��
    ''' </summary>
    ''' <param name="frmID"></param>
    ''' <remarks></remarks>
    Public Sub ShowForm(ByVal frmID As Integer)

        Select Case frmID
            Case EM_FRM_ID.emfrmTypeGr              ' �r��E�O���[�h�ݒ�
                frmTypeGr.Show()
            Case EM_FRM_ID.emfrmCommonMaster        ' �萔�ݒ�
                frmCommonMaster.Show()
            Case EM_FRM_ID.emfrmCommonAlarm         ' �x��o�͐ݒ�
                frmCommonAlarm.Show()
            Case EM_FRM_ID.emfrmSCodeMaster         ' �����\�ʏ��Ͻ���o�^
                frmSCodeMaster.Show()
            Case EM_FRM_ID.emfrmSCodeJudge          ' �����\�ʏ�ԏ����ݒ�
                frmSCodeJudge.Show()
            Case EM_FRM_ID.emfrmSCodeOutput         ' �����\�ʏ�ԕʏo�͐ݒ�
                frmSCodeOutPut.Show()
            Case EM_FRM_ID.emfrmPCodeMaster         ' ���������Ͻ���o�^
                frmPCodeMaster.Show()
            Case EM_FRM_ID.emfrmPCodeJudge          ' ��������ݏ����ݒ�
                frmPCodeJudge.Show()
            Case EM_FRM_ID.emfrmPCodeOutput         ' ��������ݕʏo�͐ݒ�
                frmPCodeOutput.Show()
            Case EM_FRM_ID.emfrmPCommonOutPut       ' ��������݌ŗL�o�͐ݒ�
                frmPCommonOutPut.Show()
            Case EM_FRM_ID.emfrmAllCodeOutput       ' �d�匇�ו\���ݒ�
                frmAllCodeOutput.Show()
            Case EM_FRM_ID.emfrmMaeTypeGr           ' �O�H���ݒ�
                frmMaeTypeGr.Show()
            Case EM_FRM_ID.emfrmStatusMaster        ' �@��}�X�^�[�ݒ�
                frmStatusMaster.Show()
            Case EM_FRM_ID.emfrmLogMaster           ' ���O�}�X�^�[�ݒ�
                frmLogMaster.Show()
            Case EM_FRM_ID.emfrmCameraLink           ' �J���������N�ݒ�
                frmCameraLink.Show()
            Case Else
		End Select

        g_nDspfrmID = frmID
    End Sub

    ''' <summary>
    ''' �p�����[�^�ύX�ʒm
    ''' </summary>
    ''' <param name="paramID">�p�����[�^�e�[�u��ID</param>
    ''' <param name="scode">�ύX�����\�ʏ��No (�S����=CH_SCODE_ALL)</param>
    ''' <remarks></remarks>
    Public Sub Send_Main(ByVal paramID As Integer, Optional ByVal scode As Integer = 0)
        Dim que As New KizuLibCLI.COMMON_QUE_L
        que.inisialize()
        que.nEventNo = FACT_TO_DEFECT_01
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        ' �f�[�^��
        que.data(0) = 0                         ' �x�����s�L�[  0�Œ�
        que.data(1) = paramID                   ' �p�����[�^�e�[�u��ID
        que.data(2) = scode                     ' �ύX�����\�ʏ��No (�����\�ʏ��No:1�`  �S��:CH_SCODE_ALL  ����:0)

        KizuLibCLI.KizuMod.Send_Mail(TO_DEFECT, "*", que)

    End Sub

    ''' <summary>
    ''' �p�X���[�h�m�F
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function ConfPass() As DialogResult

        '�p�X���[�h�m�F
        Dim frmPass As New frmPassword(g_sPass, , , "�p�����[�^�ݒ��� �p�X���[�h����")
        Dim bRet As DialogResult

        bRet = frmPass.ShowDialog

        Return bRet

    End Function

    ''' <summary>
    ''' INI�t�@�C���Ǎ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetIniFile()

        '' ini�t�@�C����� �萔�I�ȃO���[�o�[���ϐ��ǂݍ���
        g_NOA = tMod.tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)

        '�p�X���[�h�m�F���s�L���擾
        g_iPassUmu = tMod.ReadIniInt(KA_PARSET, "PASSCHK_UMU", TASKINI_NAME, EM_PASSWORD_CHECK.emPASS_CHECK_NON_OK)

        '�p�X���[�h�擾
        g_sPass = tMod.ReadIniStr(KA_PARSET, "PASSWORD", TASKINI_NAME, "", 256)

        '�n�[�h�R�s�[�L���擾
        g_iHardCopyFlg = tMod.ReadIniInt(KA_PARSET, "HARDCOPY_UMU", TASKINI_NAME, EM_HARDCOPY_CHECK.emHardCopy_Nasi)

        '���W�b�N�p�������ݒ�L���擾
        g_nParaFlg = tMod.ReadIniInt(KA_PARSET, "LOGICPARA_UMU", TASKINI_NAME, EM_LOGICPARA_CHECK.emLogicPara_Nasi)

    End Sub


    ''' <summary>
    ''' DB�ۑ����̃n�[�h�R�s�[���c���Ă���
    ''' </summary>
    ''' <param name="myForm"></param>
    ''' <remarks></remarks>
    Public Sub HardCopy(ByVal myForm As Form)
        Dim strFPath As String
        Dim strFolder As String
        Dim strName As String

        '�n�[�h�R�s�[�̗L���`�F�b�N
        If EM_HARDCOPY_CHECK.emHardCopy_Ari = g_iHardCopyFlg Then
            strFolder = IMG_PATH
            strName = myForm.Text.Replace("[" & SYSTEM_TITLE & "]", "").Trim & "_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg"

            '''' �t���p�X
            strFPath = IO.Path.Combine(strFolder, strName)

            '''' �t�H���_�̗L���`�F�b�N(�����������낤)
            tMod.CheckFolder(strFolder)

            '''' �n�[�h�R�s�[
            Dim cls_Hcopy As New tClass.tClass_HeadCopy
            Call cls_Hcopy.HCopy(myForm, strFPath, Drawing.Imaging.ImageFormat.Jpeg)

        End If
    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�e�L�X�g�{�b�N�X��ݒ�
    ''' </summary>
    ''' <param name="dgv">�R���g���[��</param>
    ''' <param name="sHeader">��w�b�_�[����</param>
    ''' <param name="nWidth">��</param>
    ''' <param name="bFrozen">���X�N���[�����̃��b�N</param>
    ''' <param name="bRead">�Ǎ���p</param>
    ''' <param name="nSort">���בւ����[�h</param>
    ''' <param name="nDefaultCellStyle">��X�^�C��</param>
    ''' <param name="nHeaderCellStyle">��w�b�_�[�X�^�C��</param>
    ''' <param name="sNullVal">�f�[�^��NULL���̕\�����e</param>
    ''' <param name="sDataNullVal">NULL���͎��̃f�[�^���e</param>
    ''' <param name="bDispVis">�\���E��\��</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetText(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                             ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                             ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                             Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewTextBoxColumn
            ' ��w�b�_�[����
            clm.HeaderText = sHeader
            ' ��
            clm.Width = nWidth
            ' ���X�N���[�����̃��b�N
            clm.Frozen = bFrozen
            ' �Ǎ���p�ݒ�
            clm.ReadOnly = bRead
            ' ���בւ����[�h
            clm.SortMode = nSort
            ' ��X�^�C��
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' ��w�b�_�[�X�^�C��
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' �\���E��\��
            clm.Visible = bDispVis
            ' �f�[�^��NULL���̕\�����e 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL���͎��̃f�[�^���e 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal
            ' ��쐬
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�`�F�b�N�{�b�N�X��ݒ�
    ''' </summary>
    ''' <param name="dgv">�R���g���[��</param>
    ''' <param name="sHeader">��w�b�_�[����</param>
    ''' <param name="nWidth">��</param>
    ''' <param name="bFrozen">���X�N���[�����̃��b�N</param>
    ''' <param name="bRead">�Ǎ���p</param>
    ''' <param name="nSort">���בւ����[�h</param>
    ''' <param name="nDefaultCellStyle">��X�^�C��</param>
    ''' <param name="nHeaderCellStyle">��w�b�_�[�X�^�C��</param>
    ''' <param name="sNullVal">�f�[�^��NULL���̕\�����e</param>
    ''' <param name="sDataNullVal">NULL���͎��̃f�[�^���e</param>
    ''' <param name="bDispVis">�\���E��\��</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetCheck(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                              ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                              ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                              Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewCheckBoxColumn
            ' ��w�b�_�[����
            clm.HeaderText = sHeader
            ' ��
            clm.Width = nWidth
            ' ���X�N���[�����̃��b�N
            clm.Frozen = bFrozen
            ' �Ǎ���p�ݒ�
            clm.ReadOnly = bRead
            ' ���בւ����[�h
            clm.SortMode = nSort
            ' ��X�^�C��
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' ��w�b�_�[�X�^�C��
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' �\���E��\��
            clm.Visible = bDispVis
            ' �f�[�^��NULL���̕\�����e 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL���͎��̃f�[�^���e 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' ��쐬
            dgv.Columns.Add(clm)
        End Using

    End Sub
    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�{�^����ݒ�
    ''' </summary>
    ''' <param name="dgv">�R���g���[��</param>
    ''' <param name="sHeader">��w�b�_�[����</param>
    ''' <param name="nWidth">��</param>
    ''' <param name="bFrozen">���X�N���[�����̃��b�N</param>
    ''' <param name="bRead">�Ǎ���p</param>
    ''' <param name="nSort">���בւ����[�h</param>
    ''' <param name="nDefaultCellStyle">��X�^�C��</param>
    ''' <param name="nHeaderCellStyle">��w�b�_�[�X�^�C��</param>
    ''' <param name="fontDefaultFont">��t�H���g</param>
    ''' <param name="bBtnTextValue">�{�^���������\���E��\��</param>
    ''' <param name="sBtnTextName">�{�^���������ݒ�</param>
    ''' <param name="sNullVal">�f�[�^��NULL���̕\�����e</param>
    ''' <param name="sDataNullVal">NULL���͎��̃f�[�^���e</param>
    ''' <param name="bDispVis">�\���E��\��</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetButton(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                               ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                               ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                               ByVal fontDefaultFont As System.Drawing.Font, ByVal bBtnTextValue As Boolean, ByVal sBtnTextName As String, _
                               Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewButtonColumn
            ' ��w�b�_�[����
            clm.HeaderText = sHeader
            ' ��
            clm.Width = nWidth
            ' ���X�N���[�����̃��b�N
            clm.Frozen = bFrozen
            ' �Ǎ���p�ݒ�
            clm.ReadOnly = bRead
            ' ���בւ����[�h
            clm.SortMode = nSort
            ' ��X�^�C��
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' ��w�b�_�[�X�^�C��
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' ��t�H���g
            clm.DefaultCellStyle.Font = fontDefaultFont
            ' �\���E��\��
            clm.Visible = bDispVis
            ' �f�[�^��NULL���̕\�����e 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL���͎��̃f�[�^���e 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' �{�^���ݒ�쐬
            Dim BtnText As DataGridViewButtonColumn = CType(clm, DataGridViewButtonColumn)
            ' �{�^���������\���E��\��
            BtnText.UseColumnTextForButtonValue = bBtnTextValue
            ' �{�^���������ݒ�
            BtnText.Text = sBtnTextName

            ' ��쐬
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' �v���W�F�N�g�ؑ֏���
    ''' </summary>
	''' <remarks></remarks>
    Public Sub ChgProject()
        'HACK 2015/02/04�@KA_PARACOLOR��
        tClassLibrary.tClass.tClass_TaskCheck.StartExe(KA_PARCOLOR)
        TaskEnd()
    End Sub

    ''' <summary>
    ''' KizuLibCLI.DLL�̓Ǎ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="args"></param>
    ''' <returns></returns>
    ''' <remarks>32bit/64bit�؂�ւ��Ή�</remarks>
    Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

            If IntPtr.Size = 8 Then
                ' 64bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
            Else
                ' 32bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
            End If

        End If

        Return Nothing

    End Function
End Module
