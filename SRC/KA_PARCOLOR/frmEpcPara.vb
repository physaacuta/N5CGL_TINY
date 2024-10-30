'*********************************************************************************
' ���W�b�N�p�������ݒ�
'	[Ver]
'		Ver.01    2019/01/30  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcPara
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const GAMEN_ID_NAME As String = GAMEN_ID_EPC_PARA
    Private Const MY_EM_FRM_ID As EM_FRM_ID = EM_FRM_ID.emfrmEpcPara
    Private Const MY_CH_PARAM_ID As Integer = CH_PARAM_PARA
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���j���[�{�^��
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmEpcPara_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 0
                btnF01.Select()                 ' F01�R���{�{�b�N�X�ύX���Ƀt�@���N�V�����������ꂽ���̑΍�
                ChangeCode(False)               ' �O��
            Case 1
                btnF02.Select()                 ' F02�R���{�{�b�N�X�ύX���Ƀt�@���N�V�����������ꂽ���̑΍�
                ChangeCode(True)                ' ����
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "���W�b�N�p�������ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARSET�֐ؑ֏���
            Case 10
                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' �ݒ�
            Case 11
                modMain.TaskEnd()               ' F12 �I��
        End Select

    End Sub

    ''' <summary>
    ''' ��ʕύX�v�����ꂽ�Ƃ��̏��� (�p�����N���X���R�[�������)
    ''' </summary>
    ''' <param name="frmID">�I�����ID</param>
    ''' <param name="cancel">�I��������߂�ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmEpcPara_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ��ʐؑ�
        ShowForm(frmID)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h��`
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcPara_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '���j���[���X�g����
        modMain.SetListMenu(Me)

        '�V�X�e�������x������
        modMain.SetLabelSysName(Me)

        '����ʌďo
        MyBase.tProp_MyFrmID = MY_EM_FRM_ID
        '�p�����[�^�ύX�敪�Z�b�g
        MyBase.tProp_MyParamID = MY_CH_PARAM_ID

        ' �J�������̃Z�b�g
        Call modMain.SetCamAria(camName, 0)

        ' �A�b�v�R�[�h�̃R���{�{�b�N�X
        MyBase.SetComboBox_Scode(cmbUpCodeItem)
        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' �X�V�{�^���̗L��/����
        MyBase.UpdateEnabled(g_bPassFlg)

        '=======================================================
        '�Ώۃf�[�^�O���b�h�r���[�ݒ�
        Dim dgv() As DgvControlInfo
        ReDim dgv(3 - 1)
        dgv(0).dgv = dgvSaveSet
        dgv(0).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType
        dgv(0).wSize.nNo = 0
        dgv(0).wSize.nName = 200
        dgv(0).wSize.nValue = 95
        dgv(0).wSize.nRange = 150

        dgv(1).dgv = dgvPath
        dgv(1).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
        dgv(1).wSize.initialize()

        dgv(2).dgv = dgvExport
        dgv(2).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType
        dgv(2).wSize.nNo = 0
        dgv(2).wSize.nName = 200
        dgv(2).wSize.nValue = 95
        dgv(2).wSize.nRange = 150
        '=======================================================
        '�g�p�e�[�u�����ݒ�
        Dim wk As TABLE_INFO
        MyBase.Clear_BaseDBList()

        wk.sTable = DB_PARAM_COMMON
        wk.SelectSQLFormat = "SELECT [Key],{0} FROM {1} WHERE [Key]=" & PARAM_COMMON_KEY
        wk.UpdateSQLFormat = ""
        wk.bCopyDB = False
        MyBase.Add_BaseDBList(DB_PARAM_COMMON, wk)

        wk.sTable = DB_PARAM_CAMSET
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = ""
        wk.bCopyDB = False
        MyBase.Add_BaseDBList(DB_PARAM_CAMSET, wk)

        wk.sTable = DB_PARAM_SCODE
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_SCODE, wk)
        '=======================================================

        '�f�[�^�O���b�h�ݒ����DB����擾
        MyBase.InitGridSetting(GAMEN_ID_NAME, dgv)

        '�����\�ʏ��No�R���{�{�b�N�X��񏉊��Z�b�g
        MyBase.Set_cmbSCode_Item(cmbSCode, lblSCode)

        '���[�h����
        MyBase.InitLoading()
    End Sub

    ''' <summary>
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcPara_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

 End Class
