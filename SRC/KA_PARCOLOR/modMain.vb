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
        emfrmEpcCommon = 0           '�Œ�p�����[�^�ݒ�
        emfrmEpcPara                 '���W�b�N�p�������ݒ�
        emfrmEpcCropShrink           '�Ԉ����ݒ�
        emfrmEpcCamera               '�J�����ݒ�
        emfrmEpcMixColor             '���F�����ݒ�
        emfrmEpcEdge                 '�G�b�W���o�ݒ�
        emfrmEpcShading              '����ިݸޕ␳�ݒ�
        emfrmEpcThresHold            '臒l�ݒ�
        emfrmEpcDetect               '�r���o�ݒ�
        emfrmEpcCycle                '�����r����
        emfrmEpcMerge                '���������ݒ�
        emfrmEpcLedLight             'LED�Ɩ��ݒ�
        emfrmEpcTajuuThresHold       '���d����\臒l�ݒ�
        emfrmEpcReduction            '�k�����ݒ�
        emfrmEpcFilter               '�t�B���^�ݒ�
        emfrmEpcPcInfo               '��ʏ����e�ݒ�
		emfrmEpcAttrName             '�����ʖ��̃}�X�^�[�ݒ�
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		emfrmLongPd                  '���������׌��o�ݒ��� 
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
		emfrmGridSetting             '�ް���د�ޕ\���ݒ�
        emfrmLovItem                 '�I�����ڱ���Ͻ��
    End Enum

    Public Enum EM_PASSWORD_CHECK
        emPASS_CHECK_NON_NG = -1     '�p�X���[�h�m�F�Ȃ��A�ݒ�ύX�s�i�f�t�H���g)
        emPASS_CHECK_NON_OK = 0      '�p�X���[�h�m�F�Ȃ��A�ݒ�ύX�\
        emPASS_CHECK_ARI = 1         '�p�X���[�h�m�F����
    End Enum

    Public Enum EM_HARDCOPY_CHECK
        emHardCopy_Nasi = 0          '�n�[�h�R�s�[�Ȃ�
        emHardCopy_Ari               '�n�[�h�R�s�[����
    End Enum

    Public Enum EM_LOGICPARA_CHECK
        emLogicPara_Nasi = 0        '���W�b�N�p�������ݒ�Ȃ�
        emLogicPara_Ari             '���W�b�N�p�������ݒ肠��
    End Enum

    Public Enum EM_LABEL_KIND
        emONLINE = 0                '���x�����F�I�����C��
        emPARA                      '���x�����F���W�b�N�p������
    End Enum

    Public Const COL_WIDTH_CAMERA_ANGLE As Integer = 50          '�J�����p�x�̃f�[�^�O���b�h��
    Public Const COL_WIDTH_CAMERA_SET As Integer = 50            '�J�����Z�b�g�̃f�[�^�O���b�h��
    Public Const PARAM_COMMON_KEY As Integer = 0                 ' ���ʃp�����[�^�e�[�u�� KEY

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���[�o���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public g_nScodeIndex As Integer = 0         '�\�ʏ�Ԃ̃��X�g�C���f�b�N�X��ۑ�
    Public g_nPaternIndex As Integer = 0        '�����p�^�[���̃��X�g�C���f�b�N�X��ۑ�
    Public gcls_Log As tClassLibrary.tClass.tClass_LogManager           ' ���O�Ǘ��N���X
    Public g_nDspfrmID As Integer = 0           '���ݕ\�����̉��ID

    Public g_NOA As Integer                     'NOA�ڑ��t���O (0:�I�����C��  1:���u)
    Public g_sPass As String = ""               '�p�X���[�h������ INI�t�@�C������擾
    Public g_bPassFlg As Boolean                '�p�X���[�h���͌��� (True:OK False:NG)
    Public g_iPassUmu As Integer                '�p�X���[�h�m�F�L��(0:����(�ݒ�s��) 1:����(�ݒ��) 2:�p�X���[�h�m�F�L)
    Public g_iHardCopyFlg As Integer            '�ݒ莞�̃n�[�h�R�s�[�L��(0:���� 1:���{)
    Public g_iParaRun As Integer                '�ݒ�ύX�����[���X���b�g���M����(0:���M�L�� 1:���M����)
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
        If Not tNinsho.IsUserCheck(KA_PARCOLOR) Then
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

        '�Œ�p�����[�^�ݒ��ʂ�\��
        Call ShowForm(EM_FRM_ID.emfrmEpcCommon)

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
        If 0 <> mcls_Mod.Task_Init(KA_PARCOLOR) Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", KA_PARCOLOR))

            End
        End If

        '���O�N���X����
        gcls_Log = New tClassLibrary.tClass.tClass_LogManager(KA_PARCOLOR, Nothing, Nothing)
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
    Public Sub SetListMenu(ByVal frm As frmMain_AutoParSet)

        frm.SetListMenu("�Œ�p�����[�^�ݒ�", EM_FRM_ID.emfrmEpcCommon)
        ' �p�������p�ݒ肩�I�����C���ȊO�̏ꍇ�A���W�b�N�p�������ݒ�\
        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            frm.SetListMenu("���W�b�N�p�������ݒ�", EM_FRM_ID.emfrmEpcPara)
        End If
        frm.SetListMenu("�Ԉ����ݒ�", EM_FRM_ID.emfrmEpcCropShrink)
        frm.SetListMenu("�J�����ݒ�", EM_FRM_ID.emfrmEpcCamera)
        frm.SetListMenu("���F�����ݒ�", EM_FRM_ID.emfrmEpcMixColor)
        frm.SetListMenu("�G�b�W���o�ݒ�", EM_FRM_ID.emfrmEpcEdge)
        frm.SetListMenu("����ިݸޕ␳�ݒ�", EM_FRM_ID.emfrmEpcShading)
        frm.SetListMenu("臒l�ݒ�", EM_FRM_ID.emfrmEpcThresHold)
        frm.SetListMenu("�r���o�ݒ�", EM_FRM_ID.emfrmEpcDetect)
        frm.SetListMenu("�����r����", EM_FRM_ID.emfrmEpcCycle)
        frm.SetListMenu("���������ݒ�", EM_FRM_ID.emfrmEpcMerge)
        frm.SetListMenu("LED�Ɩ��ݒ�", EM_FRM_ID.emfrmEpcLedLight)
        frm.SetListMenu("���d����\臒l�ݒ�", EM_FRM_ID.emfrmEpcTajuuThresHold)
        frm.SetListMenu("�k�����ݒ�", EM_FRM_ID.emfrmEpcReduction)
        frm.SetListMenu("�t�B���^�ݒ�", EM_FRM_ID.emfrmEpcFilter)
        frm.SetListMenu("��ʏ����e�ݒ�", EM_FRM_ID.emfrmEpcPcInfo)
		frm.SetListMenu("�����ʖ���Ͻ���ݒ�", EM_FRM_ID.emfrmEpcAttrName)
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		frm.SetListMenu("���������׌��o�ݒ�", EM_FRM_ID.emfrmLongPd)
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
		frm.SetListMenu("�ް���د�ޕ\���ݒ�", EM_FRM_ID.emfrmGridSetting)
        frm.SetListMenu("�I�����ڱ���Ͻ��", EM_FRM_ID.emfrmLovItem)
    End Sub

    ''' <summary>
    ''' ���j���[���X�g�ǉ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetListMenu(ByVal frm As KizuForm.frmMain_ParSet)

        frm.SetListMenu("�Œ�p�����[�^�ݒ�", EM_FRM_ID.emfrmEpcCommon)
        ' �p�������p�ݒ肩�I�����C���ȊO�̏ꍇ�A���W�b�N�p�������ݒ�\
        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            frm.SetListMenu("���W�b�N�p�������ݒ�", EM_FRM_ID.emfrmEpcPara)
        End If
        frm.SetListMenu("�Ԉ����ݒ�", EM_FRM_ID.emfrmEpcCropShrink)
        frm.SetListMenu("�J�����ݒ�", EM_FRM_ID.emfrmEpcCamera)
        frm.SetListMenu("���F�����ݒ�", EM_FRM_ID.emfrmEpcMixColor)
        frm.SetListMenu("�G�b�W���o�ݒ�", EM_FRM_ID.emfrmEpcEdge)
        frm.SetListMenu("����ިݸޕ␳�ݒ�", EM_FRM_ID.emfrmEpcShading)
        frm.SetListMenu("臒l�ݒ�", EM_FRM_ID.emfrmEpcThresHold)
        frm.SetListMenu("�r���o�ݒ�", EM_FRM_ID.emfrmEpcDetect)
        frm.SetListMenu("�����r����", EM_FRM_ID.emfrmEpcCycle)
        frm.SetListMenu("���������ݒ�", EM_FRM_ID.emfrmEpcMerge)
        frm.SetListMenu("LED�Ɩ��ݒ�", EM_FRM_ID.emfrmEpcLedLight)
        frm.SetListMenu("���d����\臒l�ݒ�", EM_FRM_ID.emfrmEpcTajuuThresHold)
        frm.SetListMenu("�k�����ݒ�", EM_FRM_ID.emfrmEpcReduction)
        frm.SetListMenu("�t�B���^�ݒ�", EM_FRM_ID.emfrmEpcFilter)
        frm.SetListMenu("��ʏ����e�ݒ�", EM_FRM_ID.emfrmEpcPcInfo)
		frm.SetListMenu("�����ʖ���Ͻ���ݒ�", EM_FRM_ID.emfrmEpcAttrName)
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		frm.SetListMenu("���������׌��o�ݒ�", EM_FRM_ID.emfrmLongPd)
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<---------------
		frm.SetListMenu("�ް���د�ޕ\���ݒ�", EM_FRM_ID.emfrmGridSetting)
        frm.SetListMenu("�I�����ڱ���Ͻ��", EM_FRM_ID.emfrmLovItem)
    End Sub

    ''' <summary>
    ''' �V�X�e�������x���ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetLabelSysName(ByVal frm As frmMain_AutoParSet_SysName)

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
    Public Sub SetLabelSysName(ByVal frm As frmMain_AutoParSet_UpList_SysName)

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
            Case EM_FRM_ID.emfrmEpcCommon         ' �Œ�p�����[�^�ݒ�
                frmEpcCommon.Show()
            Case EM_FRM_ID.emfrmEpcPara           ' ���W�b�N�p�������ݒ�
                frmEpcPara.Show()
            Case EM_FRM_ID.emfrmEpcCropShrink     ' �Ԉ����ݒ�
                frmEpcCropShrink.Show()
            Case EM_FRM_ID.emfrmEpcCamera         ' �J�����ݒ�
                frmEpcCamera.Show()
            Case EM_FRM_ID.emfrmEpcMixColor       ' ���F�����ݒ�
                frmEpcMixColor.Show()
            Case EM_FRM_ID.emfrmEpcEdge           ' �G�b�W���o�ݒ�
                frmEpcEdge.Show()
            Case EM_FRM_ID.emfrmEpcShading        ' ����ިݸޕ␳�ݒ�
                frmEpcShading.Show()
            Case EM_FRM_ID.emfrmEpcThresHold      ' 臒l�ݒ�
                frmEpcThresHold.Show()
            Case EM_FRM_ID.emfrmEpcDetect         ' �r���o�ݒ�
                frmEpcDetect.Show()
            Case EM_FRM_ID.emfrmEpcCycle          ' �����r����
                frmEpcCycle.Show()
            Case EM_FRM_ID.emfrmEpcMerge          ' ���������ݒ�
                frmEpcMerge.Show()
            Case EM_FRM_ID.emfrmEpcLedLight       ' LED�Ɩ��ݒ�
                frmEpcLedLight.Show()
            Case EM_FRM_ID.emfrmEpcTajuuThresHold ' ���d����\臒l�ݒ�
                frmEpcTajuuThresHold2.Show()
            Case EM_FRM_ID.emfrmEpcReduction      ' �k�����ݒ�
                frmEpcReduction.Show()
            Case EM_FRM_ID.emfrmEpcFilter         ' �t�B���^�ݒ�
                frmEpcFilter.Show()
            Case EM_FRM_ID.emfrmEpcPcInfo         ' ��ʏ����e�ݒ�
                frmEpcPcInfo.Show()
            Case EM_FRM_ID.emfrmEpcAttrName       ' �����ʖ��̃}�X�^�[�ݒ�
				frmEpcAttrName.Show()

				'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
			Case EM_FRM_ID.emfrmLongPd
				frmLongPd.Show()                  ' ���������׌��o�ݒ�
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
			Case EM_FRM_ID.emfrmGridSetting       ' ����������ʃf�[�^�O���b�h�\���ݒ�
                frmGridSetting.Show()
            Case EM_FRM_ID.emfrmLovItem           ' ����������ʑI�����ڃA�C�e���}�X�^�[
                frmLovItem.Show()
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

        ' ����p�������@�\���́A���[���X���b�g���M����
        If 1 = g_iParaRun Then Exit Sub

        Dim que As New KizuLibCLI.COMMON_QUE_L
        que.inisialize()
        que.nEventNo = FACT_TO_DEFECT_01
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        ' �f�[�^��
        que.data(0) = 0                         ' �x�����s�L�[  0�Œ�
        que.data(1) = paramID                   ' �p�����[�^�e�[�u��ID
        que.data(2) = scode                     ' �ύX�����\�ʏ��No (�����\�ʏ��No:1�`  �S��:CH_SCODE_ALL  ����:0)

        If Not LOCAL Then
            '�{��
            KizuLibCLI.KizuMod.Send_Mail(TO_DEFECT, KizuLibCLI.KizuMod.Tokatu(), que)
        Else
            '�e�X�g
            KizuLibCLI.KizuMod.Send_Mail(TO_DEFECT, ".", que)
        End If

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

        '���[���X���b�g���M�L���擾
        g_iParaRun = tMod.ReadIniInt(KA_PARSET, "PARA_RUN", TASKINI_NAME, 0)

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
    '''�v���W�F�N�g�ؑ֏���
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ChgProject()

        'Dim startInfo As New ProcessStartInfo("KA_PARSET.exe")
        'startInfo.WorkingDirectory = "../../../KA_PARSET/bin/Debug"
        'System.Diagnostics.Process.Start(startInfo)
		Try
			tClassLibrary.tClass.tClass_TaskCheck.StartExe(KA_PARSET)
			TaskEnd()
		Catch ex As Exception
		End Try
	End Sub

    ''' <summary>
    ''' �J�������̃Z�b�g
    ''' </summary>
    ''' <param name="cntCamName">�R���g���[��</param>
    ''' <param name="nPtn">�p�^�[��</param>
    ''' <remarks></remarks>
    Public Sub SetCamAria(ByVal cntCamName As CamName, Optional ByVal nPtn As Integer = 0)
        Dim sCamAngle As New List(Of String)
        Dim sCamSet As New List(Of String)
        Dim sCamSetKo As New List(Of String)

        '''' �p�x
        sCamAngle.Add("1:R�摜")
        sCamAngle.Add("2:G�摜")
        sCamAngle.Add("3:B�摜")
        sCamAngle.Add("4:G-B�摜")
        cntCamName.SetName(0, sCamAngle)

        '''' �J�����Z�b�g
        sCamSet.Add(" 1:�\DS  6:�\W4")
        sCamSet.Add(" 2:�\D1  7:�\W3")
        sCamSet.Add(" 3:�\D2  8:�\W2")
        sCamSet.Add(" 4:�\D3  9:�\W1")
        sCamSet.Add(" 5:�\D4 10:�\WS")
        sCamSet.Add("")
        sCamSet.Add("11:��DS 16:��W4")
        sCamSet.Add("12:��D1 17:��W3")
        sCamSet.Add("13:��D2 18:��W2")
        sCamSet.Add("14:��D3 19:��W1")
        sCamSet.Add("15:��D4 20:��WS")
        cntCamName.SetName(1, sCamSet)

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
