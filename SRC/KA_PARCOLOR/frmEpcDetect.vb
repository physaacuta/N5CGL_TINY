''*********************************************************************************
'' �d�o�b�r���o�ݒ��ʃN���X
''	[Ver]
''		Ver.01    2012/10/08  ����
''
''	[����]
''		
''*********************************************************************************
Option Strict On

Public Class frmEpcDetect

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const GAMEN_ID_NAME As String = GAMEN_ID_EPC_DEFECT
    Private Const MY_EM_FRM_ID As EM_FRM_ID = EM_FRM_ID.emfrmEpcDetect
    Private Const MY_CH_PARAM_ID As Integer = CH_PARAM_DETECT
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
    Private Sub frmEpcDtect_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
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
                Call HardCopy(IMG_PATH, "�r���o�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmEpcDetect_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        '' ���낢��J������

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
    Private Sub frm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

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
		ReDim dgv(2 - 1)

		dgv(0).dgv = dgvPScode
        dgv(0).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType
		dgv(0).wSize.nNo = 0
        dgv(0).wSize.nName = 280
        dgv(0).wSize.nValue = 550
		dgv(0).wSize.nRange = 200

		dgv(1).dgv = dgvCamAngleScode
		dgv(1).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
		dgv(1).wSize.initialize()
        '=======================================================
        Dim wk As TABLE_INFO
        MyBase.Clear_BaseDBList()

		wk.sTable = DB_PARAM_SCODE
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_SCODE, wk)

        wk.sTable = DB_PARAM_CAMANGLE_SCODE
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camangle = b.camangle AND a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_CAMANGLE_SCODE, wk)
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
    Private Sub frmEpcDetect_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��ʓ��ꏈ��
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvPScode_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPScode.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        Dim Data As DataSetHolder = MyBase.Get_GridInfo(dgv)
        If Data Is Nothing Then
            Exit Sub
        End If
        Try
            If ((e.ColumnIndex <> EM_ROW_VERTION_ID.emRange) Or (e.RowIndex = -1)) Then
                Exit Try
            End If
            If (CType(Data.Rows(CInt(dgv.Item(EM_ROW_VERTION_ID.emNo, e.RowIndex).Value) - 1).Item("����_�\���_�C�A���O"), CallType) = EM_DLG_TYPE.emFileDialog) Then
                With ofdDcdFile
                    .CheckFileExists = True     '�t�@�C�������݂��邩�m�F
                    .RestoreDirectory = True    '�f�B���N�g���̕���
                    .ReadOnlyChecked = True
                    .ShowReadOnly = True
                    .Filter = _
                    "���ׂẴt�@�C��(*.*)|*.*|DCD�t�@�C��(*.dcd)|*.dcd"
                    .FilterIndex = 2            'Filter�v���p�e�B��2�ڂ�\��

                    '�_�C�A���O�{�b�N�X��\�����A�m�J��]���N���b�N�����ꍇ
                    If .ShowDialog = Windows.Forms.DialogResult.OK Then
                        '�e�L�X�g�t�@�C�����I������Ă���ꍇ�̓t�@�C�����J��
                        If System.IO.Path.GetExtension(.FileName) = ".dcd" Then

                            '�J�����t�@�C�����e�L�X�g�{�b�N�X�ɕ\������
                            dgv(EM_ROW_VERTION_ID.emValue, e.RowIndex).Value = .FileName
                        Else
                            MessageBox.Show("DCD�t�@�C�����w�肵�Ă�������", "�G���[")
                            Exit Sub
                        End If
                    End If
                End With

            End If

        Catch ex As Exception
            gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.ERR, String.Format("���l���{�^������ �ُ�[{0}]", ex.Message))
        End Try

    End Sub

End Class
