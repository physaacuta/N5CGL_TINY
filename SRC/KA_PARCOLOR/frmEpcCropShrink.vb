'*********************************************************************************
' �Ԉ����ݒ�
'	[Ver]
'		Ver.01    2019/01/28  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcCropShrink
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�
    Private Enum em_DataColumn
        emTxt_Order = 0                 ' �e�L�X�g�{�b�N�X  Order
        emTxt_Width                     ' �e�L�X�g�{�b�N�X  ���
        emTxt_Height                    ' �e�L�X�g�{�b�N�X  ���
        emTxt_ShrinkRatio               ' �e�L�X�g�{�b�N�X  �k����
        emTxt_InputRange                ' �e�L�X�g�{�b�N�X  ���͔͈�
        iInsButton                      ' �}���{�^��
        iDelButton                      ' �폜�{�^��
    End Enum

    '�폜�Ώۂ�ID
    Private Structure TypeStruct
        Dim Order As Integer        ' �폜�Ώۂ�Order

        Public Sub initialize()
            Order = 0
        End Sub

    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterCropShrink As New SqlClient.SqlDataAdapter      ' �A�_�v�^�[
    Private m_DsCropShrink As New DataSet                            ' ��ڑ��^�e�[�u��PARAM_CROP_SHRINK
    Private mlst_DelID As New List(Of TypeStruct)                    ' �폜�Ώۂ�ID

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
    Private Sub frmEpcCropShrink_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�Ԉ����ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARSET�֐ؑ֏���
            Case 10
                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' F11 �ݒ�
            Case 11
                modMain.TaskEnd()               ' F12 �I��
        End Select
    End Sub

    ''' <summary>
    ''' ��ʕύX�v�����ꂽ�Ƃ��̏��� (�p�����N���X���R�[�������)
    ''' </summary>
    ''' <param name="frmID">�I�����ID</param>
    ''' <param name="cancel">�I��������߂����ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' �J������
        m_DsCropShrink.Dispose()
        If Not m_AdapterCropShrink Is Nothing Then
            m_AdapterCropShrink.Dispose()
            m_AdapterCropShrink = Nothing
        End If

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
    Private Sub frmEpcCropShrink_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcCropShrink         ' �Ԉ����ݒ�Ăяo��

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' �X�V�{�^���̗L��/����
        Call UpdateEnabled(g_bPassFlg)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        'dgv������
        dgvInitialize(dgvCropShrink)
        dgvColunmDef(dgvCropShrink)

        ' ��ʂɒl��\��
        Data_Disp()

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        MyBase.DataGridViewSelectionRowColorChange(dgvCropShrink)

        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub
    ''' <summary>
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' �f�[�^�̎擾
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL������
        strSQL = "SELECT [Order], "
        strSQL &= "Width, Height, ShrinkRatio "
        strSQL &= " FROM " & DB_PARAM_CROP_SHRINK
        strSQL &= " ORDER BY [Order] ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsCropShrink, m_AdapterCropShrink, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_REDUCTION & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Try
            '�l��o�^
            If Not DB_DeleteUp() Then
                Return False
            Else
                modMain.Send_Main(CH_PARAM_CROP_SHRINK)
            End If

            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        End Try
    End Function

    ''' <summary>
    ''' dgv�Ƀf�[�^�s��}��
    ''' </summary>
    ''' <param name="nRowIndex">�s�̑}���ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_TypeData(ByVal nRowIndex As Integer)
        ' dgvReduction�Ƀ��R�[�h��}��
        dgvCropShrink.Rows.Insert(nRowIndex)

        ' �f�[�^������
        dgvCropShrink(em_DataColumn.emTxt_Order, nRowIndex).Value = ""       ' Order
        dgvCropShrink(em_DataColumn.emTxt_Width, nRowIndex).Value = ""       ' ���
        dgvCropShrink(em_DataColumn.emTxt_Height, nRowIndex).Value = ""      ' ���
        dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, nRowIndex).Value = "" ' �k����
    End Sub

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()

        ' �Z���l�ύX�C�x���g������
        ' ���֐����ŃZ���l��ύX����ׁA�C�x���g������
        RemoveHandler dgvCropShrink.CellValueChanged, AddressOf dgvCropShrink_CellValueChanged

        '�f�[�^��\��
        Data_DispdgvReduction(dgvCropShrink)

        '�C���f�b�N�X�ƃ{�^���̕\���ݒ�
        InitIndexdgvReduction(dgvCropShrink)

        ' ���͔͈͐ݒ�
        Data_InputRangeSet(dgvCropShrink)

        ' �s�̐F��ς��� �i�����\���ŐF���ς��΍�j
        dgvCropShrink.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

        ' �Z���l�ύX�C�x���g��ݒ�
        AddHandler dgvCropShrink.CellValueChanged, AddressOf dgvCropShrink_CellValueChanged

    End Sub

    ''' <summary>
    ''' dgv�Ƀf�[�^��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispdgvReduction(ByVal dgv As DataGridView)
        Dim drowCropShrink() As DataRow = m_DsCropShrink.Tables(0).Select()         ' �S���R�[�h��\�����邽��WHERE��Ȃ�

        '�s��������
        dgv.Rows.Clear()
        ' ��ʂɕ\��
        dgv.RowCount = MAX_CROP_SHRINK                                  ' dgv�̃��R�[�h���͌Œ�

        ' ��ʂɕ\��
        Try

            For ii As Integer = 0 To drowCropShrink.Length - 1
                Dim nLine As Integer = CInt(drowCropShrink(ii).Item("Order")) - 1     ' �i�[�s

                If ii < dgv.RowCount And nLine < dgv.RowCount Then
                    dgv(em_DataColumn.emTxt_Width, nLine).Value = drowCropShrink(ii).Item("Width")              ' ���
                    dgv(em_DataColumn.emTxt_Height, nLine).Value = drowCropShrink(ii).Item("Height")            ' ���
                    dgv(em_DataColumn.emTxt_ShrinkRatio, nLine).Value = drowCropShrink(ii).Item("ShrinkRatio")  ' �k����
                End If
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_CROP_SHRINK & " �\���G���[ [" & ex.Message & "] " & Me.Text)
        End Try

        ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
        dgv(1, 0).Selected = True

        Return
    End Sub

    ''' <summary>
    ''' ���͔͈͐ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_InputRangeSet(ByVal dgv As DataGridView)
        Dim sBeforeWidth As String = "0"            ' ����i�O��l�j
        Dim sBeforeHeight As String = "0"           ' ����i�O��l�j
        Dim lstRow As New List(Of DataGridViewRow)

        'dgv�s���[�v
        For ii As Integer = 0 To dgvCropShrink.Rows.Count - 1
            Dim sWidth As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value))             ' ���
            Dim sHeight As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value))           ' ���
            Dim sShrinkRatio As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value)) ' �k����

            '�� or �����������͂ł���Γ��͔͈͖͂�����
            If sWidth = "" OrElse sHeight = "" Then
                dgvCropShrink(em_DataColumn.emTxt_InputRange, ii).Value = ""
                Continue For
            End If

            ' ���͔͈͐ݒ蔻��s���擾
            lstRow.Add(dgvCropShrink.Rows(ii))
        Next ii

        ' ���ŏ����Ƀ\�[�g
        lstRow.Sort(AddressOf SortWidth)

        ' ���͔͈͂��\�[�g���ɐݒ�
        For ii As Integer = 0 To lstRow.Count - 1
            ' �`�F�b�N�Ώۃf�[�^�擾
            Dim sNowWidth As String = Trim(CStr(lstRow(ii).Cells(em_DataColumn.emTxt_Width).Value))            ' ���
            Dim sNowHeight As String = Trim(CStr(lstRow(ii).Cells(em_DataColumn.emTxt_Height).Value))          ' ���
            Dim sShrinkRatio As String = Trim(CStr(lstRow(ii).Cells(em_DataColumn.emTxt_ShrinkRatio).Value))   ' �k����
            Dim nLen As Integer = CInt(lstRow(ii).Cells(em_DataColumn.emTxt_InputRange).Tag)                   ' �Ώۂ̍s�ʒu

            ' ���͔͈͕\��
            dgv(em_DataColumn.emTxt_InputRange, nLen).Value = String.Format("{0,4}�������{1,4},{2,4}�������{3,4} �� �k����={4}�{",
                                                                            sBeforeWidth, sNowWidth, sBeforeHeight, sNowHeight, sShrinkRatio)

            sBeforeWidth = sNowWidth            ' ����i�O��l�j�X�V
            sBeforeHeight = sNowHeight          ' ����i�O��l�j�X�V
        Next

    End Sub

    ''' <summary>
    ''' ���͒l�`�F�b�N
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String     'dgv�̃f�[�^�i�[�p
        Dim lstRow As New List(Of DataGridViewRow)
        Dim lstTarget As New List(Of Integer)

        'dgv�s���[�v
        For ii As Integer = 0 To dgvCropShrink.Rows.Count - 1
            Dim sWidth As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value))             ' ���
            Dim sHeight As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value))           ' ���
            Dim sShrinkRatio As String = Trim(CStr(dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value)) ' �k����

            '�S�Ė����͂ł���΃`�F�b�N�s�v
            If sWidth = "" AndAlso sHeight = "" AndAlso sShrinkRatio = "" Then
                Continue For
            End If

            '���
            strWk = CStr(dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 4096, False, "�Ԉ����ݒ� No=" & ii + 1 & " ���") <> 0 Then Return False
            dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value = strWk

            '���
            strWk = CStr(dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 4096, False, "�Ԉ����ݒ� No=" & ii + 1 & " ���") <> 0 Then Return False
            dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value = strWk

            '�k����
            strWk = CStr(dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 4, False, "�Ԉ����ݒ� No=" & ii + 1 & " �k����") <> 0 Then Return False
            dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value = strWk

            ' �`�F�b�N�Ώۍs���擾
            lstRow.Add(dgvCropShrink.Rows(ii))
        Next ii

        ' ���ŏ����Ƀ\�[�g
        lstRow.Sort(AddressOf SortWidth)

        ' �Ԉ����͈͂̕��A���������ɑ傫���Ȃ��Ă��邩����
        Dim wkMsg As String = ""
        For ii As Integer = 0 To lstRow.Count - 1
            ' �`�F�b�N�Ώۃf�[�^�擾
            Dim nTargetWidth As Integer = CInt(lstRow(ii).Cells(em_DataColumn.emTxt_Width).Value)
            Dim nTargetHeight As Integer = CInt(lstRow(ii).Cells(em_DataColumn.emTxt_Height).Value)
            Dim nTargetOrder As Integer = CInt(lstRow(ii).Cells(em_DataColumn.emTxt_Order).Value)

            ' ����������f�[�^��NG
            If lstTarget.Contains(nTargetWidth) Then
                Dim nErrOrder As Integer = CInt(lstRow(lstTarget.IndexOf(nTargetWidth)).Cells(em_DataColumn.emTxt_Order).Value)
                wkMsg = "�y �Ԉ����ݒ� No=" & nErrOrder & "," & nTargetOrder & " ���,���" & " �z" & vbCrLf
                MsgBox(wkMsg & "�K�؂Ȓl�ł͂���܂���B�l��ύX���Ă��������B", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                Return False
            End If

            ' �`�F�b�N�Ώۃf�[�^��蕝���傫����΍������傫�����Ƃ��m�F
            For jj As Integer = ii + 1 To lstRow.Count - 1
                Dim nComp As Integer = CInt(lstRow(jj).Cells(em_DataColumn.emTxt_Height).Value)
                If nTargetHeight >= nComp Then
                    Dim nErrOrder As Integer = CInt(lstRow(jj).Cells(em_DataColumn.emTxt_Order).Value)
                    wkMsg = "�y �Ԉ����ݒ� No=" & nTargetOrder & "," & nErrOrder & " ���,���" & " �z" & vbCrLf
                    MsgBox(wkMsg & "�K�؂Ȓl�ł͂���܂���B�l��ύX���Ă��������B", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                    Return False
                End If
            Next

            ' �f�[�^�i�[
            lstTarget.Add(nTargetWidth)
        Next

        Return True

    End Function

    ''' <summary>
    ''' �\�[�g�֐�
    ''' ���Ń\�[�g
    ''' </summary>
    ''' <param name="a"></param>
    ''' <param name="b"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SortWidth(ByVal a As DataGridViewRow, ByVal b As DataGridViewRow) As Integer

        ' ����l���擾
        Dim nCmpWidthA As Integer
        Dim nCmpWidthB As Integer
        tClass_ImputCheck.CheckData(a.Cells(em_DataColumn.emTxt_Width).Value.ToString, nCmpWidthA)
        tClass_ImputCheck.CheckData(b.Cells(em_DataColumn.emTxt_Width).Value.ToString, nCmpWidthB)

        ' ���𔻒�
        If nCmpWidthA <= nCmpWidthB Then
            Return -1
        End If

        Return 1

    End Function

    ''' <summary>
    ''' ��ʂɕ\������Order��DB��Order���ׂāA�폜���ꂽID�����邩�`�F�b�N����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_CheckDelete()
        Dim drowReduction() As DataRow = m_DsCropShrink.Tables(0).Select()
        Dim bCheck As Boolean   '�`�F�b�N�t���O True�Ȃ�ID������
        Dim typ_Type As TypeStruct = Nothing

        ' ��ڑ��e�[�u���őS�s���[�v
        For ii As Integer = 0 To drowReduction.Length - 1
            ' �`�F�b�N�t���O�̏�����
            bCheck = False
            ' dgv�őS�s���[�v
            For kk As Integer = 0 To dgvCropShrink.Rows.Count - 1

                typ_Type.Order = CInt(drowReduction(ii).Item("Order"))      ' Order

                ' Order��r
                If CInt(dgvCropShrink(em_DataColumn.emTxt_Order, kk).Value) = CInt(typ_Type.Order) Then
                    ' ID����v�����Ƃ��A�`�F�b�NOK
                    bCheck = True
                    ' �폜�����L�[���܂��������ꍇ
                    If mlst_DelID.Contains(typ_Type) Then
                        '�폜���X�g���폜
                        mlst_DelID.Remove(typ_Type)
                    End If
                    Exit For
                End If
            Next kk

            ' �`�F�b�N�t���O��False�̂Ƃ������ꂽOrder�Ȃ̂ŁA�o�^����
            If bCheck = False Then
                typ_Type.initialize()
                ' �폜�Ώۂ�Order���i�[
                typ_Type.Order = CInt(drowReduction(ii).Item("Order"))

                ' ���ɃL�[���������ꍇ
                If mlst_DelID.Contains(typ_Type) Then
                    '�폜���X�g���폜
                    mlst_DelID.Remove(typ_Type)
                End If
                mlst_DelID.Add(typ_Type)
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' ��ʕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim drowReduction() As DataRow = m_DsCropShrink.Tables(0).Select()
        Dim nRowCount As Integer    '�폜�Ώۂ̃f�[�^��

        '���݉�ʂɕ\�����Ă���O�H���R�[�h�̔�ڑ��e�[�u���̃f�[�^���폜(��납��폜)
        nRowCount = drowReduction.Length
        For ii As Integer = 1 To nRowCount
            m_DsCropShrink.Tables(0).Rows.Remove(drowReduction(nRowCount - ii))
        Next

        ' dgv�̒l���ڑ��e�[�u���֊i�[
        For ii As Integer = 0 To dgvCropShrink.RowCount - 1
            Dim sOrder As String = CStr(dgvCropShrink(em_DataColumn.emTxt_Order, ii).Value)             ' Order
            Dim sWidth As String = CStr(dgvCropShrink(em_DataColumn.emTxt_Width, ii).Value)             ' ���
            Dim sHeight As String = CStr(dgvCropShrink(em_DataColumn.emTxt_Height, ii).Value)           ' ���
            Dim sShrinkRatio As String = CStr(dgvCropShrink(em_DataColumn.emTxt_ShrinkRatio, ii).Value) ' �k����

            '�S�ē��͂���Ă���s�����擾
            If sWidth = "" AndAlso sHeight = "" AndAlso sShrinkRatio = "" Then Continue For
            Dim drowNew As DataRow = m_DsCropShrink.Tables(0).NewRow()    '�V�K�s

            '�f�[�^���Z�b�g
            drowNew.Item("Order") = CInt(sOrder)                ' Order
            drowNew.Item("Width") = CInt(sWidth)                ' ���
            drowNew.Item("Height") = CInt(sHeight)              ' ���
            drowNew.Item("ShrinkRatio") = CInt(sShrinkRatio)    ' �k����
            '�e�[�u���ɒǉ�
            m_DsCropShrink.Tables(0).Rows.InsertAt(drowNew, m_DsCropShrink.Tables(0).Rows.Count)
        Next
    End Sub

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        '���͒l�`�F�b�N
        If Not Data_Check() Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '��ʂ���폜���ꂽID���擾
            Data_CheckDelete()

            '��ʂ̃f�[�^���擾
            Data_Get()

            '��ʂ���폜���ꂽID���ēx�o�^���ꂽ�����擾
            Data_CheckDelete()

            '�z���g�ɓo�^
            If DB_UpDate() Then
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            '���̃t�H�[���ŕK�v�ȃf�[�^���擾����
            If Not DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       ' �I��
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' �ݒ�̏�����
        Data_Init()

        '��ʂɒl��\��
        Data_Disp()

    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[������
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        ''''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With


    End Sub

    ''' <summary>
    ''' dgv�̗��`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvColunmDef(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        With dgv
            ''=================================
            '' ���`
            ' Order
            dgvColSetText(dgv, "No", 40, True, True,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' ���
            dgvColSetText(dgv, "���" & vbNewLine & "( 0 - 4096 )", 147, False, False,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' ���
            dgvColSetText(dgv, "���" & vbNewLine & "( 0 - 4096 )", 147, False, False,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' �k����
            dgvColSetText(dgv, "�k����" & vbNewLine & "( 1 - 4 )", 147, False, False,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' ���͔͈�
            dgvColSetText(dgv, "���͔͈�", 430, False, True,
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

            ' ��֑}��
            dgvColSetButton(dgv, "���" & vbNewLine & "�}��", 50, False, False,
                            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter,
                            New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�}��")

            ' �폜
            dgvColSetButton(dgv, "�폜", 50, False, False,
                            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter,
                            New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�폜")

        End With
    End Sub


    ''' <summary>
    ''' dgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvReduction_A_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvCropShrink.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case em_DataColumn.iInsButton   '��֑}���{�^��
                InsBtnClick_Type(dgv, e.RowIndex)
            Case em_DataColumn.iDelButton   '�폜�{�^��
                DelBtnClick_Type(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' dgv�̑}���{�^�������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv">�Ώۂ̃f�[�^�O���b�h�r���[</param>
    ''' <param name="nRowIndex">���R�[�h��}������ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        ' �}���{�^�����g�p�\���`�F�b�N
        If (dgv.Rows.Count - 1 = nRowIndex) Then
            ' �ŏI�s�͎g�p�s�Ȃ̂Ŕ�����
            Exit Sub
        End If

        '�ŏI�s�̃e�L�X�g�{�b�N�X�ɉ�������̃f�[�^�������Ă���ꍇ�͑}���s��
        If Trim(CStr(dgv(em_DataColumn.emTxt_Width, dgv.Rows.Count - 1).Value)) <> "" Or
        Trim(CStr(dgv(em_DataColumn.emTxt_Height, dgv.Rows.Count - 1).Value)) <> "" Or
        Trim(CStr(dgv(em_DataColumn.emTxt_ShrinkRatio, dgv.Rows.Count - 1).Value)) <> "" Then
            ' <<----------------
            MsgBox("�ŏI�s�Ƀf�[�^�����݂��܂��B")
            Exit Sub
        End If

        '�}�����邩���b�Z�[�W���o��
        If MsgBox("�}�����܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "�����m�F") = MsgBoxResult.No Then Exit Sub

        ' dgv�Ƀ��R�[�h��}��
        InsertAt_TypeData(nRowIndex)

        ' dgv�̍ŏI�s���폜
        dgv.Rows.RemoveAt(dgvCropShrink.Rows.Count - 1)

        'No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexdgvReduction(dgv)
    End Sub

    ''' <summary>
    ''' dgv�̍폜�{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        '�폜���邩���b�Z�[�W���o��
        If MsgBox("�폜���܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "�����m�F") = MsgBoxResult.No Then Exit Sub

        ' dgv�̍ŏI�s�Ƀ��R�[�h��}��
        InsertAt_TypeData(dgv.Rows.Count)

        ' dgv�̃��R�[�h���폜
        dgv.Rows.RemoveAt(nRowIndex)

        'No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexdgvReduction(dgv)
    End Sub

    ''' <summary>
    ''' dgvReduction�̃C���f�b�N�X��U��A�{�^���̕\���ݒ������
    ''' </summary>
    ''' <param name="dgv">dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexdgvReduction(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(em_DataColumn.emTxt_Order, ii).Value = ii + 1   ' ��̃C���f�b�N�X�ԍ�

            ' ���͔͈͐ݒ莞�ɍs�ԍ��𔻕ʂ���ׂ�Tag�ɍs�ԍ���ݒ�
            dgv(em_DataColumn.emTxt_InputRange, ii).Tag = ii

            ' �}���{�^���̎g�p�\��Ԃ�ݒ�    (�{�^�����g�p�s�ɂ͂ł��Ȃ��̂ŁA�F��ύX�j
            If ii = dgv.RowCount - 1 Then
                '�ŏI�s�͑}���{�^�����g�p�s��
                dgv(em_DataColumn.iInsButton, ii).Style.ForeColor = Color.Gray
            Else
                '����ȊO�͎g�p��
                dgv(em_DataColumn.iInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next
    End Sub

    ''' <summary>
    ''' dgv�̑I�����ύX���ꂽ��A�Z���̑I����Ԃ��L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv
        dgv.CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' �Z���̑I��l���ύX���ꂽ�ꍇ�A���͔͈͂��X�V
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvCropShrink_CellValueChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        ' �Z���l�ύX�C�x���g������
        ' ���֐����ŃZ���l��ύX����ׁA�C�x���g������
        RemoveHandler dgvCropShrink.CellValueChanged, AddressOf dgvCropShrink_CellValueChanged

        Data_InputRangeSet(dgvCropShrink)

        ' �Z���l�ύX�C�x���g��ݒ�
        AddHandler dgvCropShrink.CellValueChanged, AddressOf dgvCropShrink_CellValueChanged
    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub


    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_DeleteUp() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB�ڑ�
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        Dim sqlReader As SqlClient.SqlDataReader = Nothing   'DataReader�I�u�W�F�N�g
        Dim strSql As String    'SQL��

        Try
            '�f�[�^�x�[�X�I�[�v��
            If Not tcls_DB.Open() Then Return False

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            strSql = "DELETE FROM " & DB_PARAM_CROP_SHRINK
            'SQL���̔��s
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�f�[�^��o�^ 
            For ii As Integer = 0 To m_DsCropShrink.Tables(0).Rows.Count - 1
                With m_DsCropShrink.Tables(0).Rows(ii)

                    '�o�^SQL���쐬
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_CROP_SHRINK & " VALUES ("

                    strSql &= .Item("Order").ToString & ","                 'Order
                    strSql &= .Item("Width").ToString & ","                 '���
                    strSql &= .Item("Height").ToString & ","                '���
                    strSql &= .Item("ShrinkRatio").ToString                 '�k����
                    strSql &= " )"

                    'SQL���s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        '�G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            '�R�~�b�g
            tcls_DB.TransactCommint()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �o�^�G���[ [" & ex.Message & "] " & Me.Text)
            '�G���[�Ȃ烍�[���o�b�N���ďI��
            tcls_DB.TransactRollback()
            Return False

        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()

        End Try
    End Function

    ''' <summary>
    ''' �����o�ϐ��̏����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Init()
        ' �폜�Ώۂ�ID���X�g�̏�����
        mlst_DelID.Clear()
    End Sub

End Class
