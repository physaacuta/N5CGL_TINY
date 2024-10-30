'*********************************************************************************
' �J���������N�ݒ��ʃN���X
'	[Ver]
'		Ver.01    2019/04/10  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tMod

Public Class frmCameraLink
    '/ ////////////////////////////////////////////////////////////////////////////////
    '  �񋓑�
    '/ ////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���W�b�N�p������PC
    ''' </summary>
    Private Enum EM_PARAPC
        PARA_PC1 = 0          ' ���W�b�N�p������PC�P
        PARA_PC2 = 1          ' ���W�b�N�p������PC�Q
    End Enum

    '/ ////////////////////////////////////////////////////////////////////////////////
    '  �\����
    '/ ////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' M_PC_MASTER�e�[�u����PC�����i�[
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure PC_MASTER_VALUE
        Dim nPcId As Integer            ' PCID
        Dim nGroupNo As Integer         ' GroupNo
    End Structure

    ''' <summary>
    ''' �p�������ݒ�l
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure PARA_VALUE
        Dim nPcId As Integer            ' PCID
        Dim nLinkSw As Integer          ' �����N�X�C�b�`���
        Dim nFPGABoard As Integer       ' FPGA�{�[�hNo
        Dim nConnectCh As Integer       ' �ڑ�CH�i��ʗp�@FPGA0,FPGA1�̒l���i�[�����j
        Dim nConnectChOutPut As Integer ' �ڑ�CH�i�ݒ�t�@�C���o�͗p 10�`15�̒l���i�[�����j
        Dim bGetFlg As Boolean          ' �擾�t���O

        Public Sub Initialize()
            nPcId = 0
            nLinkSw = 0
            nFPGABoard = 0
            nConnectCh = 0
            nConnectChOutPut = 0
            bGetFlg = False
        End Sub
    End Structure

    ''' <summary> ����ini�t�@�C����` </summary>
    Private Structure TOKATU_PC_INI
        Dim strIniPath_Set As String        ' ����PC ini�t�@�C�� ���f�p
        Dim strIniPath_Exe As String        ' ����PC ini�t�@�C�� EXE��
        Dim strToIpAddr As String           ' ����PC IP�A�h���X
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[
    Private m_DS As New DataSet                                     ' ��ڑ��^�e�[�u��
    Private m_lstPcMaster As List(Of PC_MASTER_VALUE) = New List(Of PC_MASTER_VALUE)        ' M_PC_MASTER�e�[�u����PC��񃊃X�g
    Private m_nParaPcId(NUM_HANTEI_PARA - 1) As Integer                                     ' ���W�b�N�p������PC��PCID�i�[��
    Private m_typNowPara(NUM_HANTEI_PARA - 1, NUM_CAMLINK_BOARD - 1) As PARA_VALUE          ' ���W�b�N�p������PC�ڑ��ݒ�i���ݒl�j
    Private m_lstConnectCh(NUM_CAMLINKSW - 1) As List(Of Integer)                           ' �g�p�ϐڑ�CH���X�g�i�����N�X�C�b�`���j
    Private m_nLinkSwSelectIdx(NUM_HANTEI_PARA - 1) As Integer                              ' ���ݑI�𒆂̃����N�X�C�b�`�iIDX�j
    Private m_typTokatuIni As TOKATU_PC_INI                                                 ' ����ini�t�@�C����`
    Private m_bIniReadFlg As Boolean                                                        ' ini�t�@�C���ǂݍ��ݐ���^�ُ�t���O(�ُ펞�͎Q�Ƃ̂�)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const PARA_PC_GROUP_NO As Integer = 5                   ' ���W�b�N�p������PC�@�O���[�vNo
    Private Const PARA_PC1_CAMERA_KIND As Integer = 2               ' ���W�b�N�p������PC�P�@�J��������CH�敪
    Private Const PARA_PC2_CAMERA_KIND As Integer = 3               ' ���W�b�N�p������PC�P�@�J��������CH�敪
    Private Const CAMERA_LINK_NAME As String = "LINKSW"             ' �J���������N�X�C�b�`�̃R���{�{�b�N�X�Ŏg�p���閼��
    Private Const FPGA0_CONNECTOR_MIN As Integer = 10               ' FPGA0�ł̐ڑ�CH�I���ŏ��l
    Private Const FPGA0_CONNECTOR_MAX As Integer = 15               ' FPGA0�ł̐ڑ�CH�I���ő�l
    Private Const FPGA1_CONNECTOR_MIN As Integer = 25               ' FPGA1�ł̐ڑ�CH�I���ŏ��l
    Private Const FPGA1_CONNECTOR_MAX As Integer = 30               ' FPGA1�ł̐ڑ�CH�I���ő�l


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h��`
    '/ /////////////////////////////////////////////////////////////////////////////////
#Region "�t�H�[���\��/�I���C�x���g�֘A"
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmCameraLink_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmCameraLink            ' �J���������N�ݒ��ʌďo

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        '''' �����o�ϐ��C���X�^���X��
        For ii As Integer = 0 To NUM_CAMLINKSW - 1
            m_lstConnectCh(ii) = New List(Of Integer)
        Next


        '''' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If


        '''' INI�t�@�C�����瓝��PC�����擾
        m_bIniReadFlg = True
        If Not Read_Ini() Then
            gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.WAR, "ini�t�@�C���Ǎ����s�̂��߁A�Q�Ƃ̂�")
            m_bIniReadFlg = False
        Else
            ' �ݒ�Ώ�PC�����肷��B����PC�ɐڑ��\���Ŕ���B
            If Not CheckSettingPossiblePC() Then
                gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.WAR, "����PC�ɐڑ��s�̂��߁A�Q�Ƃ̂�")
                m_bIniReadFlg = False
            End If
        End If

        If m_bIniReadFlg = False Then
            ' INI�t�@�C���擾���s���͍X�V�{�^������
            UpdateEnabled(False)
        Else
            ' �X�V�{�^���̗L��/����
            Call UpdateEnabled(g_bPassFlg)
        End If


        '''' DB����擾�����l�������o�ϐ��ɐݒ�
        If Not SetDBData() Then
            ' DB�擾�l�ُ�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�擾�l���ُ�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If


        '''' ��ʂɒl��\��        
        If Not Data_Disp() Then
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("��ʕ\�����Ɉُ킪���������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If


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
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCameraLink_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    '''  ini�t�@�C���ǂݍ���
    ''' </summary>
    ''' <returns>true:�ǂݍ��ݐ��� false:�ǂݍ��ݎ��s</returns>
    ''' <remarks>�e�O�H����ini�t�@�C����`��ǂݍ���</remarks>
    Private Function Read_Ini() As Boolean
        Dim bRet As Boolean = True
        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        Dim key As String = ""

        Try

            '''' ����PC IP�A�h���X�擾
            key = "PC_IPADDR"
            m_typTokatuIni.strToIpAddr = tClassLibrary.tMod.tMod_IniFile.ReadIniStr(KA_PARSET, key, TASKINI_NAME)
            If m_typTokatuIni.strToIpAddr = "" Then Throw New Exception(String.Format("ini�t�@�C�� �Z�N�V����[KA_PARSET] �L�[ {1}����`����Ă��Ȃ��ׁA" & vbCrLf & "�u�J���������N�ݒ�v" & vbCrLf & "�͎Q�Ƃ݂̂ƂȂ�܂��B", key))

            '''' ����PC EXE��ini�t�@�C���p�X�擾
            key = "INI_SAVE_PATH_EXE"
            m_typTokatuIni.strIniPath_Exe = tClassLibrary.tMod.tMod_IniFile.ReadIniStr(KA_PARSET, key, TASKINI_NAME)
            If m_typTokatuIni.strIniPath_Exe = "" Then Throw New Exception(String.Format("ini�t�@�C�� �Z�N�V����[KA_PARSET] �L�[ {1}����`����Ă��Ȃ��ׁA" & vbCrLf & "�u�J���������N�ݒ�v" & vbCrLf & "�͎Q�Ƃ݂̂ƂȂ�܂��B", key))

            '''' ����PC ���f�pini�t�@�C���p�X�擾
            key = "INI_SAVE_PATH_INISET"
            m_typTokatuIni.strIniPath_Set = tClassLibrary.tMod.tMod_IniFile.ReadIniStr(KA_PARSET, key, TASKINI_NAME)
            If m_typTokatuIni.strIniPath_Set = "" Then Throw New Exception(String.Format("ini�t�@�C�� �Z�N�V����[KA_PARSET] �L�[ {1}����`����Ă��Ȃ��ׁA" & vbCrLf & "�u�J���������N�ݒ�v" & vbCrLf & "�͎Q�Ƃ݂̂ƂȂ�܂��B", key))

        Catch ex As Exception
            bRet = False
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' �ݒ�\PC������
    ''' </summary>
    ''' <returns>(True:�ݒ�\ False:�ݒ�s��)</returns>
    ''' <remarks></remarks>
    Private Function CheckSettingPossiblePC() As Boolean

        '''' DB�ɓo�^���ꂽ�O�H���R�[�h���o�^����Ă��邩
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "INI�t�@�C���̐ݒ�ۃ`�F�b�N")

        ' ����PC��Ping���ʂ邩
        If Not PingCheck(m_typTokatuIni.strToIpAddr) Then Return False
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "����PC Ping�`�F�b�N���� IP:" & m_typTokatuIni.strToIpAddr)
        ' ����PC EXE��ini�t�@�C�������݂��Ă��邩
        If Not System.IO.File.Exists(m_typTokatuIni.strIniPath_Exe) Then Return False
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "����PC EXE��ini�t�@�C���`�F�b�N���� FILE:" & m_typTokatuIni.strIniPath_Exe)
        ' ����PC ���f�pini�t�@�C�������݂��Ă��邩
        If Not System.IO.File.Exists(m_typTokatuIni.strIniPath_Set) Then Return False
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "����PC ���f�pini�t�@�C���`�F�b�N���� FILE:" & m_typTokatuIni.strIniPath_Set)

        Return True

    End Function

    ''' <summary>
    ''' ping�m�F
    ''' </summary>
    ''' <param name="IPaddress">ping�m�F�̃T�[�o�[IP�A�h���X</param>
    ''' <returns>true:ping���� false:ping���s</returns>
    ''' <remarks></remarks>
    Private Function PingCheck(ByVal IPaddress As String) As Boolean
        Dim bRet As Boolean = True
        Try
            If Not My.Computer.Network.Ping(IPaddress) Then
                Throw New Exception("Ping ���s")
            End If
        Catch ex As Exception
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' DB����擾�����l�������o�ϐ��ɐݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Function SetDBData() As Boolean
        Dim nParaPc As Integer = 0
        Dim rows() As DataRow = m_DS.Tables(0).Select()

        ' DB����擾����PC��񕪃��[�v
        For ii As Integer = 0 To rows.Length - 1
            Dim pcMasterVal As PC_MASTER_VALUE

            '''' PC��񃊃X�g�ɐݒ�
            pcMasterVal.nPcId = CInt(rows(ii).Item("PCID"))         ' PCID
            pcMasterVal.nGroupNo = CInt(rows(ii).Item("GroupNo"))   ' GroupNo
            m_lstPcMaster.Add(pcMasterVal)

            '''' ���W�b�N�p������PCID���X�g�ɐݒ�
            If PARA_PC_GROUP_NO = pcMasterVal.nGroupNo Then

                ' ���W�b�N�p������PC���ő�l�ȏ�ݒ肳��Ă����ꍇ
                If NUM_HANTEI_PARA = nParaPc Then Return False

                ' �O���[�vNo����v�����ꍇ���W�b�N�p������PCID���X�g�Ɋi�[
                ' PCID��������PC���烍�W�b�N�p������PC1,2,����Ƃ���
                m_nParaPcId(nParaPc) = pcMasterVal.nPcId
                nParaPc += 1
            End If
        Next

        ' ���W�b�N�p������PC�̐����ő�l�ȊO
        If NUM_HANTEI_PARA <> nParaPc Then Return False

        Return True
    End Function

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub

    ''' <summary>
    ''' ��ʕύX�v�����ꂽ�Ƃ��̏��� (�p�����N���X���R�[�������)
    ''' </summary>
    ''' <param name="frmID">�I�����ID</param>
    ''' <param name="cancel">�I��������߂����ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmCameraLink_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If

        ' ��ʐؑ�
        ShowForm(frmID)
    End Sub
#End Region

#Region "DB�֘A����"
    ''' <summary>
    ''' PC�}�X�^�[����PC�����擾����
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL������
        strSQL = "SELECT PCID "
        strSQL &= ",GroupNo "
        strSQL &= " FROM " & DB_PC_MASTER
        strSQL &= " ORDER BY PCID"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_COMMON_MASTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function
#End Region

#Region "��ʕ\���֘A"
    ''' <summary>
    ''' �ݒ�t�@�C��������ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean
        Dim bResult As Boolean = False

        ' LINKSW�R���{�{�b�N�X�ɒl��ݒ肷��׃C�x���g������
        RemoveHandler cmbLinkSw1.SelectedIndexChanged, AddressOf cmbLinkSw_SelectedIndexChanged
        RemoveHandler cmbLinkSw2.SelectedIndexChanged, AddressOf cmbLinkSw_SelectedIndexChanged

        Try
            '''' ��ʍ��ڂ̏�����
            ItemInitialize()

            '''' �ڑ�CH�ݒ�
            SetConnector()

            '''' ��ʃ����N�X�C�b�`�R���{�{�b�N�X�ɒl��ݒ�
            SetLinkSwComboBox()

            '''' ��ʐڑ�CH�R���{�{�b�N�X�ɒl��ݒ�
            SetConnectComboBox(EM_PARAPC.PARA_PC1, cmbLinkSw1, cmbPara1ConnectCh1, cmbPara1ConnectCh2)
            SetConnectComboBox(EM_PARAPC.PARA_PC2, cmbLinkSw2, cmbPara2ConnectCh1, cmbPara2ConnectCh2)

            bResult = True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�t�@�C�����\���G���[ [" & ex.Message & "] " & Me.Text)
        Finally
            ' LINKSW�R���{�{�b�N�X�ɒl��ݒ肷��׃C�x���g���Đݒ�
            AddHandler cmbLinkSw1.SelectedIndexChanged, AddressOf cmbLinkSw_SelectedIndexChanged
            AddHandler cmbLinkSw2.SelectedIndexChanged, AddressOf cmbLinkSw_SelectedIndexChanged
        End Try

        Return bResult
    End Function

    ''' <summary>
    ''' ���ڏ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ItemInitialize()
        ' �����N�X�C�b�`�R���{�{�b�N�X������
        cmbLinkSw1.Items.Clear()
        cmbLinkSw2.Items.Clear()

        ' �����N�X�C�b�`�R���{�{�b�N�X���ڒl�ݒ�
        For ii As Integer = 1 To NUM_CAMLINKSW
            cmbLinkSw1.Items.Add("LINKSW" & ii)
            cmbLinkSw2.Items.Add("LINKSW" & ii)
        Next

        ' �ڑ�CH�R���{�{�b�N�X ������
        cmbPara1ConnectCh1.Items.Clear()
        cmbPara1ConnectCh2.Items.Clear()
        cmbPara2ConnectCh1.Items.Clear()
        cmbPara2ConnectCh2.Items.Clear()

        ' �g�p�ϐڑ�CH ������
        For ii As Integer = 0 To NUM_CAMLINKSW - 1
            m_lstConnectCh(ii).Clear()
        Next

        ' ���W�b�N�p������PC�ڑ��ݒ� ������
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1
                m_typNowPara(ii, jj).Initialize()
            Next
        Next

    End Sub

    ''' <summary>
    ''' �ڑ�CH�ݒ�
    ''' �����N�X�C�b�`���Ƀ��W�b�N�p������PC�ȊO�Ŏg�p����Ă���ڑ�CH���擾
    ''' ���W�b�N�p������PC�̐ڑ�CH�ݒ���擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetConnector()

        ' ����PC��Ping���ʂ邩
        If Not PingCheck(m_typTokatuIni.strToIpAddr) Then Return

        ' DB����擾����PCID�����[�v
        For Each pcMasterVal As PC_MASTER_VALUE In m_lstPcMaster
            For ii As Integer = 1 To NUM_CAMLINK_BOARD
                '''' INI�t�@�C������J���������N�ݒ���擾
                Dim strItemName As String = String.Format("CAMLINKSW_PCID{0}_{1}", pcMasterVal.nPcId, ii)
                Dim nCameraLink As Integer = tMod_IniFile.ReadIniInt(TO_DEFECT, strItemName, m_typTokatuIni.strIniPath_Exe, 0)

                ' �J���������N���ݒ�
                If 0 >= nCameraLink Then Continue For


                '''' �J���������N�ݒ肩��FPGA�{�[�hNo�A�ڑ��R�l�N�^�擾
                Dim nFPGABoard As Integer = nCameraLink \ 100                   ' FPGA�{�[�hNo
                Dim nConnectCh As Integer = nCameraLink - (nFPGABoard * 100)    ' �ڑ�CH�i��ʗp�j
                Dim nConnectChOutPut As Integer = nConnectCh                    ' �ڑ�CH�i�t�@�C���o�͗p�j

                ' FPGA1�������ꍇ�AINI�t�@�C���̐ڑ�CH��FPGA1�p�̐ڑ�CH�ɕϊ�
                If 0 <> nFPGABoard Mod NUM_CAMLINK_BOARD Then
                    nConnectCh = nConnectCh - FPGA0_CONNECTOR_MIN + FPGA1_CONNECTOR_MIN
                End If


                '''' FPGA�{�[�hNo���烊���N�X�C�b�`���擾
                Dim nLinkSw As Integer = nFPGABoard \ NUM_CAMLINK_BOARD + 1

                ' LinkSw���ΏۊO
                If nLinkSw > m_lstConnectCh.Length Then Continue For


                '''' ���W�b�N�p������PC�̐ڑ��ݒ���擾
                Dim bChAddFlg As Boolean = True      ' �g�p�ϐڑ�CH�ǉ��t���O
                For jj As Integer = 0 To m_nParaPcId.Length - 1

                    ' ���W�b�N�p������PC��PCID�ƈ�v
                    If pcMasterVal.nPcId = m_nParaPcId(jj) Then
                        ' ���W�b�N�p������PC�̌��ݐݒ�l���擾
                        m_typNowPara(jj, ii - 1).nPcId = pcMasterVal.nPcId
                        m_typNowPara(jj, ii - 1).nLinkSw = nLinkSw
                        m_typNowPara(jj, ii - 1).nFPGABoard = nFPGABoard
                        m_typNowPara(jj, ii - 1).nConnectCh = nConnectCh
                        m_typNowPara(jj, ii - 1).nConnectChOutPut = nConnectChOutPut
                        m_typNowPara(jj, ii - 1).bGetFlg = True

                        ' ���W�b�N�p������PC�ɐݒ肳��Ă���ڑ�CH�͎g�p�ϐڑ�CH�ɂ͒ǉ����Ȃ�
                        bChAddFlg = False
                        Exit For
                    End If
                Next


                '''' �����N�X�C�b�`���Ƀ��W�b�N�p������PC�ȊO�Ŏg�p���Ă���ڑ�CH���i�[
                If bChAddFlg Then m_lstConnectCh(nLinkSw - 1).Add(nConnectCh)
            Next
        Next

    End Sub

    ''' <summary>
    ''' �����N�X�C�b�`�R���{�{�b�N�X�ݒ�
    ''' ��ʂ�LinkSw�R���{�{�b�N�X�ɐݒ�t�@�C������擾����LinkSw�̒l��ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetLinkSwComboBox()
        Dim nSelectIdx As Integer

        '''' ���W�b�N�p������PC1�̐ݒ�
        ' �f�t�H���g�l�� LINKSW1
        nSelectIdx = 0
        ' �ڑ��搔�����[�v
        For ii As Integer = 0 To NUM_CAMLINK_BOARD - 1

            ' ���W�b�N�p������PC1��LinkSw���擾��
            If 0 < m_typNowPara(EM_PARAPC.PARA_PC1, ii).nLinkSw Then
                nSelectIdx = m_typNowPara(EM_PARAPC.PARA_PC1, ii).nLinkSw - 1
                Exit For
            End If

            ' LinkSw���擾�ł���܂Ń��[�v
        Next
        cmbLinkSw1.SelectedIndex = nSelectIdx
        m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC1) = nSelectIdx

        '''' ���W�b�N�p������PC2�̐ݒ�
        ' �f�t�H���g�l�� LINKSW1
        nSelectIdx = 0
        ' �ڑ��搔�����[�v
        For ii As Integer = 0 To NUM_CAMLINK_BOARD - 1

            ' ���W�b�N�p������PC2��LinkSw���擾��
            If 0 < m_typNowPara(EM_PARAPC.PARA_PC2, ii).nLinkSw Then
                nSelectIdx = m_typNowPara(EM_PARAPC.PARA_PC2, ii).nLinkSw - 1
                Exit For
            End If

            ' LinkSw���擾�ł���܂Ń��[�v
        Next
        cmbLinkSw2.SelectedIndex = nSelectIdx
        m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC2) = nSelectIdx

    End Sub

    ''' <summary>
    ''' �ڑ�CH�R���{�{�b�N�X�ݒ�
    ''' ��ʂ̐ڑ�CH�R���{�{�b�N�X�ɒl��ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetConnectComboBox(ByVal nPC As EM_PARAPC, ByRef cmbLinkSw As ComboBox, ByRef cmbParaConCh1 As ComboBox, ByRef cmbParaConCh2 As ComboBox)
        Dim nLinkSwPara As Integer = 1  ' �f�t�H���g��LINKSW1
        If 0 <= cmbLinkSw.SelectedIndex Then nLinkSwPara = cmbLinkSw.SelectedIndex + 1

        '''' �ڑ�CH�̒ǉ�
        ' FPGA0�̐ڑ�CH��ǉ�
        For ii As Integer = FPGA0_CONNECTOR_MIN To FPGA0_CONNECTOR_MAX

            '''' ���W�b�N�p������PC�̐ڑ�CH�ǉ�
            ' ���Ɏg�p�ς݂̐ڑ�CH�͑I��s��
            If False = m_lstConnectCh(nLinkSwPara - 1).Contains(ii) Then
                cmbParaConCh1.Items.Add("CH" & ii)
                cmbParaConCh2.Items.Add("CH" & ii)
            End If
        Next

        ' FPGA1�̐ڑ�CH��ǉ�
        For ii As Integer = FPGA1_CONNECTOR_MIN To FPGA1_CONNECTOR_MAX

            '''' ���W�b�N�p������PC�̐ڑ�CH�ǉ�
            ' ���Ɏg�p�ς݂̐ڑ�CH�͑I��s��
            If False = m_lstConnectCh(nLinkSwPara - 1).Contains(ii) Then
                cmbParaConCh1.Items.Add("CH" & ii)
                cmbParaConCh2.Items.Add("CH" & ii)
            End If
        Next


        '''' �����I��l�ݒ�i���݂��Ȃ��ڑ�CH��ݒ肵�Ă����ꍇ�͖��I���ɂȂ�j
        cmbParaConCh1.SelectedIndex = cmbParaConCh1.FindString("CH" & m_typNowPara(nPC, 0).nConnectCh)
        cmbParaConCh2.SelectedIndex = cmbParaConCh2.FindString("CH" & m_typNowPara(nPC, 1).nConnectCh)
    End Sub

#End Region

#Region "INI�t�@�C���X�V�֘A"
    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            '''' ��ʐݒ�l���烍�W�b�N�p������PC�̐ڑ�����擾
            ' ���W�b�N�p������PC�P
            Dim typParaVal(NUM_HANTEI_PARA - 1, NUM_CAMLINK_BOARD - 1) As PARA_VALUE
            ' �ڑ���1�̐ݒ�
            typParaVal(0, 0) = GetParaSetting(m_nParaPcId(EM_PARAPC.PARA_PC1), cmbLinkSw1, cmbPara1ConnectCh1)
            ' �ڑ���2�̐ݒ�
            typParaVal(0, 1) = GetParaSetting(m_nParaPcId(EM_PARAPC.PARA_PC1), cmbLinkSw1, cmbPara1ConnectCh2)

            ' ���W�b�N�p������PC�Q
            ' �ڑ���1�̐ݒ�
            typParaVal(1, 0) = GetParaSetting(m_nParaPcId(EM_PARAPC.PARA_PC2), cmbLinkSw2, cmbPara2ConnectCh1)
            ' �ڑ���2�̐ݒ�
            typParaVal(1, 1) = GetParaSetting(m_nParaPcId(EM_PARAPC.PARA_PC2), cmbLinkSw2, cmbPara2ConnectCh2)


            '''' ���͒l�`�F�b�N
            If Not Data_Check(typParaVal) Then Exit Sub


            '''' ���b�Z�[�W�\��
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub


            '''' INI�t�@�C���X�V
            If INI_UpDate(typParaVal) Then
                '''' �n�[�h�R�s�[�ۑ�
                modMain.HardCopy(Me)

                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If


            '''' ��ʂɒl��\�� 
            If Not Data_Disp() Then
                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("��ʕ\�����Ɉُ킪���������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '�I��
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�{�^���������ُ�" & Me.Text)
        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Sub

    ''' <summary>
    ''' ��ʍ��ڂ��烍�W�b�N�p������PC�ݒ�l���擾
    ''' </summary>
    ''' <param name="nPcId">���W�b�N�p������PCID</param>
    ''' <param name="cmbLinkSw">�擾�Ώۂ̃J���������N�X�C�b�`�R���{�{�b�N�X</param>
    ''' <param name="cmbParaConnectCh">�擾�Ώۂ̐ڑ�CH</param>
    ''' <returns>>���W�b�N�p������PC�ݒ�l</returns>
    ''' <remarks></remarks>
    Private Function GetParaSetting(ByVal nPcId As Integer, ByVal cmbLinkSw As ComboBox, ByVal cmbParaConnectCh As ComboBox) As PARA_VALUE

        '''' ���W�b�N�p������PC�ݒ���擾
        Dim typParaVal As PARA_VALUE
        typParaVal.nPcId = nPcId
        typParaVal.nLinkSw = CInt(cmbLinkSw.Text.ToString.Replace(CAMERA_LINK_NAME, ""))
        typParaVal.nConnectCh = CInt(IIf("" = cmbParaConnectCh.Text.ToString(), -1, cmbParaConnectCh.Text.ToString.Replace("CH", "")))
        typParaVal.nConnectChOutPut = typParaVal.nConnectCh
        typParaVal.nFPGABoard = typParaVal.nLinkSw * NUM_CAMLINK_BOARD - NUM_CAMLINK_BOARD  ' FPGA0��FPGA�{�[�gNo

        ' �ڑ�CH��FPGA1�̏ꍇ
        If 0 <= typParaVal.nConnectChOutPut - FPGA1_CONNECTOR_MIN Then
            typParaVal.nConnectChOutPut = typParaVal.nConnectChOutPut - FPGA1_CONNECTOR_MIN + FPGA0_CONNECTOR_MIN
            typParaVal.nFPGABoard += NUM_CAMLINK_BOARD - 1
        End If

        Return typParaVal

    End Function

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal typParaVal(,) As PARA_VALUE) As Boolean
        Dim lstCheck As List(Of Integer) = New List(Of Integer)

        ' LINKSW���ŏd�������ڑ�CH��ݒ肵�Ă��Ȃ�������
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1

                ' �ڑ�CH���I��
                If -1 = typParaVal(ii, jj).nConnectChOutPut Then
                    MsgBox(String.Format("���W�b�N�p������PC{0} �ڑ���{1}�̐ڑ�CH��I�����Ă��������B", ii + 1, jj + 1), MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                    Return False
                End If

                ' KizuTask.ini CAMLINKSW_PCID�̏o�͒l�ɕϊ����ďd���L���𔻒�
                Dim nLinkSwValue As Integer = CInt(String.Format("{0}{1}", typParaVal(ii, jj).nFPGABoard, typParaVal(ii, jj).nConnectChOutPut))

                ' �d���ڑ�CH�ݒ�L
                If lstCheck.Contains(nLinkSwValue) Then
                    MsgBox(String.Format("LINKSW{0} �ڑ�CH���d�����Ă��܂��B", typParaVal(ii, jj).nLinkSw), MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                    Return False
                End If

                lstCheck.Add(nLinkSwValue)
            Next
        Next

        Return True
    End Function

    ''' <summary>
    ''' INI�t�@�C���̍X�V
    ''' </summary>
    ''' <param name="typParaVal">��ʐݒ�l</param>
    ''' <remarks></remarks>
    Private Function INI_UpDate(ByVal typParaVal(,) As PARA_VALUE) As Boolean
        Dim bResult As Boolean = False
        Dim bIniExe As Boolean = False
        Dim bIniSet As Boolean = False

        Try
            '''' ����PC�ɑ΂���ping�m�F
            If Not PingCheck(m_typTokatuIni.strToIpAddr) Then
                ' ����PC��Ping���ʂ�Ȃ�
                gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.ERR, "����PC��Ping���ʂ�܂���ł���")
                Return False
            End If


            '''' ����������ini�t�@�C�������݂��Ă��邩
            bIniExe = System.IO.File.Exists(m_typTokatuIni.strIniPath_Exe)
            bIniSet = System.IO.File.Exists(m_typTokatuIni.strIniPath_Set)
            If Not bIniExe OrElse Not bIniSet Then
                ' �w�肳�ꂽini�t�@�C�������݂��Ȃ�
                Dim strMsg As String = ""
                strMsg = CStr(IIf(Not bIniSet, String.Format("[���f�p�F{0}]", m_typTokatuIni.strIniPath_Exe), ""))
                strMsg &= CStr(IIf(Not bIniExe, String.Format("[EXE���F{0}]", m_typTokatuIni.strIniPath_Exe), ""))
                gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.ERR, "����PC��ini�t�@�C�������݂��܂��� " & strMsg)
                Return False
            End If


            '''' ini�t�@�C���̃o�b�N�A�b�v���쐬
            bIniExe = CreateIniBackupFile(m_typTokatuIni.strIniPath_Exe)
            bIniSet = CreateIniBackupFile(m_typTokatuIni.strIniPath_Set)
            If Not bIniExe OrElse Not bIniSet Then
                ' �w�肳�ꂽini�t�@�C���̃o�b�N�A�b�v�쐬�Ɏ��s
                Dim strMsg As String = ""
                strMsg = CStr(IIf(Not bIniSet, String.Format("[���f�p�F{0}]", m_typTokatuIni.strIniPath_Exe), ""))
                strMsg &= CStr(IIf(Not bIniExe, String.Format("[EXE���F{0}]", m_typTokatuIni.strIniPath_Exe), ""))
                gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.ERR, "ini�t�@�C���̃o�b�N�A�b�v�쐬�Ɏ��s���܂��� " & strMsg)
                Return False
            End If


            '''' TO_DEFECT���� ��ʐݒ�l�𔽉f
            For ii As Integer = 0 To NUM_HANTEI_PARA - 1
                For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1
                    ' EXE����KizuTask.ini�Ɣz�M�p��KizuTask.ini���X�V
                    WriteTO_DEFECT(m_typTokatuIni.strIniPath_Exe, typParaVal(ii, jj), jj + 1)
                    WriteTO_DEFECT(m_typTokatuIni.strIniPath_Set, typParaVal(ii, jj), jj + 1)
                Next
            Next


            '''' TO_LINKSW���� ���ݐݒ�l�������l�ɐݒ�
            For ii As Integer = 0 To NUM_HANTEI_PARA - 1
                For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1
                    ' ���ݒl���擾�̏ꍇ�A�����l�ݒ菈���͎��{���Ȃ�
                    If False = m_typNowPara(ii, jj).bGetFlg Then Continue For

                    ' EXE����KizuTask.ini�Ɣz�M�p��KizuTask.ini���X�V
                    WriteTO_LINKSW(m_typTokatuIni.strIniPath_Exe, m_typNowPara(ii, jj), 0)
                    WriteTO_LINKSW(m_typTokatuIni.strIniPath_Set, m_typNowPara(ii, jj), 0)
                Next
            Next


            '''' TO_LINKSW���� ��ʐݒ�l�𔽉f
            Dim nKind As Integer = PARA_PC1_CAMERA_KIND
            For ii As Integer = 0 To NUM_HANTEI_PARA - 1
                ' �p������PC�Q��ݒ肷��ꍇ
                If 1 = ii Then
                    nKind = PARA_PC2_CAMERA_KIND
                End If

                For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1
                    ' EXE����KizuTask.ini�Ɣz�M�p��KizuTask.ini���X�V
                    WriteTO_LINKSW(m_typTokatuIni.strIniPath_Exe, typParaVal(ii, jj), nKind)
                    WriteTO_LINKSW(m_typTokatuIni.strIniPath_Set, typParaVal(ii, jj), nKind)
                Next
            Next

            bResult = True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "KizuTask.ini �X�V�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

        Return bResult
    End Function

    ''' <summary>
    ''' ini�t�@�C���̃o�b�N�A�b�v���쐬����
    ''' </summary>
    ''' <param name="ini_filename">�R�s�[����ini�t�@�C���̃t���p�X</param>
    ''' <returns>true:�o�b�N�A�b�v���� false;�o�b�N�A�b�v���s</returns>
    ''' <remarks>�w�肳�ꂽini�t�@�C���Ɠ���̃t�H���_�� {���̃t�@�C����}_bak.ini �t�@�C���ŃR�s�[����</remarks>
    Private Function CreateIniBackupFile(ByVal ini_filename As String) As Boolean
        Dim bRet As Boolean = True

        Try
            Dim path As String = System.IO.Path.GetDirectoryName(ini_filename)
            Dim filename As String = System.IO.Path.GetFileNameWithoutExtension(ini_filename)
            Dim ext As String = System.IO.Path.GetExtension(ini_filename)
            Dim bak_filename As String = String.Format("{0}\{1}_bak{2}", path, filename, ext)

            System.IO.File.Copy(ini_filename, bak_filename, True)

        Catch ex As Exception
            bRet = False
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' INI�t�@�C��[TO_DEFECT]�̐ݒ���X�V
    ''' </summary>
    ''' <param name="strWritePath">�����݃t�@�C���p�X</param>
    ''' <param name="typParaVal">���W�b�N�p������PC�ݒ�l</param>
    ''' <param name="nBoard">�ڑ���</param>
    ''' <remarks></remarks>
    Private Function WriteTO_DEFECT(ByVal strWritePath As String, ByVal typParaVal As PARA_VALUE, ByVal nBoard As Integer) As Boolean

        Dim bResult As Boolean = False
        Dim strItemName As String = String.Format("CAMLINKSW_PCID{0}_{1}", typParaVal.nPcId, nBoard)
        Dim nLinkSwValue As Integer = CInt(String.Format("{0}{1}", typParaVal.nFPGABoard, typParaVal.nConnectChOutPut))
        bResult = tClassLibrary.tMod.WriteIni(TO_DEFECT, strItemName, strWritePath, nLinkSwValue.ToString)

        Return bResult

    End Function

    ''' <summary>
    ''' INI�t�@�C��[TO_LINKSW]�̐ݒ���X�V
    ''' </summary>
    ''' <param name="strWritePath">�����݃t�@�C���p�X</param>
    ''' <param name="typParaVal">���W�b�N�p������PC�ݒ�l</param>
    ''' <param name="nKind">�敪  �������F0 / ���W�b�N�p������PC1�F2 / ���W�b�N�p������PC2�F3 </param>
    ''' <remarks></remarks>
    Private Function WriteTO_LINKSW(ByVal strWritePath As String, ByVal typParaVal As PARA_VALUE, ByVal nKind As Integer) As Boolean

        Dim bResult As Boolean = False
        Dim strItemName As String = String.Format("FPGA_{0}_BOARD_{1}", typParaVal.nFPGABoard, typParaVal.nConnectChOutPut)
        Dim strLinkSwValue As String = String.Format("0, {0}", nKind)
        bResult = tClassLibrary.tMod.WriteIni(TO_LINKSW, strItemName, strWritePath, strLinkSwValue)

        Return bResult

    End Function
#End Region


    '/ /////////////////////////////////////////////////////////////////////////////////
    '  �C�x���g�n���h���֐�
    '/ /////////////////////////////////////////////////////////////////////////////////
#Region "LINKSW�R���{�{�b�N�X�֘A�C�x���g"
    ''' <summary>
    ''' LINKSW�ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbLinkSw_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs)
        Dim cmbLinkSw As ComboBox = CType(sender, ComboBox)
        If cmbLinkSw1.Equals(cmbLinkSw) Then
            ' ����LINKSW���đI�������ꍇ�͏����s�v
            If m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC1) = cmbLinkSw1.SelectedIndex Then Exit Sub

            ' �ڑ�CH���N���A
            cmbPara1ConnectCh1.Items.Clear()
            cmbPara1ConnectCh2.Items.Clear()

            ' �ڑ�CH���Đݒ�
            SetConnectComboBox(EM_PARAPC.PARA_PC1, cmbLinkSw1, cmbPara1ConnectCh1, cmbPara1ConnectCh2)

            ' �Đݒ莞�̓R���{�{�b�N�X�ɂ͖��I����ݒ�
            cmbPara1ConnectCh1.SelectedIndex = -1
            cmbPara1ConnectCh2.SelectedIndex = -1

            ' �I��LINKSW���擾
            m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC1) = cmbLinkSw1.SelectedIndex
        Else
            ' ����LINKSW���đI�������ꍇ�͏����s�v
            If m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC2) = cmbLinkSw2.SelectedIndex Then Exit Sub

            ' �ڑ�CH���N���A
            cmbPara2ConnectCh1.Items.Clear()
            cmbPara2ConnectCh2.Items.Clear()

            ' �ڑ�CH���Đݒ�
            SetConnectComboBox(EM_PARAPC.PARA_PC2, cmbLinkSw2, cmbPara2ConnectCh1, cmbPara2ConnectCh2)

            ' �Đݒ莞�̓R���{�{�b�N�X�ɂ͖��I����ݒ�
            cmbPara2ConnectCh1.SelectedIndex = -1
            cmbPara2ConnectCh2.SelectedIndex = -1

            ' �I��LINKSW���擾
            m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC2) = cmbLinkSw2.SelectedIndex
        End If

    End Sub
#End Region

#Region "�L�[�C�x���g"
    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmCameraLink_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�J���������N�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARCOLOR�֐ؑ֏���
            Case 10
                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' �ݒ�
            Case 11
                modMain.TaskEnd()               ' F12 �I��
        End Select

    End Sub
#End Region

End Class
