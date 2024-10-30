'*********************************************************************************
' �p�����[�^�ݒ��ʂ̐e�t�H�[��
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

''' <summary>
''' �p�����[�^�ݒ��ʗp�̊��N���X
''' </summary>
''' <remarks></remarks>
Public Class frmMain_ParSet

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �萔
	'/ /////////////////////////////////////////////////////////////////////////////////
	' ���ʂ̐F�R�[�h
	Public g_ColorSetting As Color = Color.PeachPuff					'�I���Z���̐F�ݒ�i�f�[�^�O���b�h�r���[�̃`�F�b�N�{�b�N�X����j
	Public g_ColorSelection As Color = Color.LightGreen					'�I���Z����s���̐F�ς�


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ���j���[���X�g���I�����ꂽ (�p����ŃI�[�o�[���C�h���ĂˁB)
	''' </summary>
	''' <param name="frmID">�I�����ID</param>
	''' <param name="cancel">�I��������߂����ꍇ�́ATrue���Z�b�g</param>
	''' <remarks></remarks>
	Public Event tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o �ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
	Private my_frmID As Integer								' �����̉��ID


	''' <summary>
	''' �R���{�{�b�N�X�ɕ\������f�[�^
	''' </summary>
	''' <remarks>�����\�ʏ�ԁE�����p�^�[�� �p</remarks>
	Public Class cls_CmbItem
		''' <summary>��L�[</summary>
		Public id As String									' ��L�[
		''' <summary>�R�t������</summary>
		Public name As String								' �R�t������
		''' <summary>�L�������t���O 0:���� 1:�L��</summary>
		Public num As Integer								' �L�������t���O 0:���� 1:�L��

		''' <summary>
		''' �R���X�g���N�^
		''' </summary>
		''' <param name="strID">��L�[</param>
		''' <param name="strName">�R�t������</param>
		''' <param name="inum">�L�������t���O</param>		
		''' <remarks></remarks>
		Public Sub New(ByVal strID As String, ByVal strName As String, ByVal inum As Integer)
			id = strID
			name = strName
			num = inum
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
    '/ �O���v���p�e�B
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �����̉��ID(��ʕ\�����ɑI�΂�Ă��鍀��)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property tProp_MyFrmID() As Integer
        Get
            Return my_frmID
        End Get
		Set(ByVal Value As Integer)

			Dim nCnt As Integer = 0

			If Value < 0 Then Exit Property

			' ������ID�Ƃ��ĕێ�
			my_frmID = Value
			' �����̃��j���[���X�g��ύX
			For Each lstData As ListViewItem In lstVMenu.Items
				If CInt(lstData.Tag) = Value Then

					'������ID�̃��X�g��F�ς�
					lstVMenu.Items(nCnt).BackColor = Color.Pink
					lstVMenu.Items(nCnt).Font = New Font(lstVMenu.Items(nCnt).Font, FontStyle.Bold)
					Exit For
				End If
				nCnt += 1
			Next

		End Set
    End Property


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���j���[�A�C�e���ǉ�
    ''' </summary>
    ''' <param name="strName">����</param>
    ''' <param name="frmID">���ID</param>
    ''' <remarks></remarks>
    Public Sub SetListMenu(ByVal strName As String, ByVal frmID As Integer)
        Dim item As New ListViewItem(strName)
        item.Tag = frmID
        lstVMenu.Items.Add(item)
    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_ParSet_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '' ���j���[�̒�`
        With lstVMenu
			.Columns.Add("���j���[", .ClientSize.Width, HorizontalAlignment.Center)
		End With
    End Sub

    ''' <summary>
    ''' ���j���[���X�g�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub lstVMenu_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstVMenu.Click
        If 0 = lstVMenu.SelectedItems.Count Then Exit Sub ' ��������
        Dim frmID As Integer = CInt(lstVMenu.SelectedItems(0).Tag)     ' �I���ʒu�擾
        If my_frmID = frmID Then Exit Sub ' �������g�Ȃ烄��

        '' �p����ŏ���������
        Dim cancel As Boolean = False
        RaiseEvent tEv_SelectMenuList(frmID, cancel)

        '' �L�����Z�����ꂽ
        If cancel Then Exit Sub

        '' �����̏I��
        Me.Close()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f�[�^�O���b�h�r���[�֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �I���L�����Z��
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h�r���[�R���g���[��</param>
	''' <remarks></remarks>
	Public Sub DataGridViewSelectionCancel(ByVal dgv As DataGridView)
		' ���̊֐��ɃC�x���g�n���h�����t
		AddHandler dgv.SelectionChanged, AddressOf dgvData_SelectionChanged
	End Sub

	''' <summary>
	''' �f�[�^�O���b�h�r���[�I���𖳌���
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        CType(sender, DataGridView).CurrentCell.Selected = False
    End Sub


	''' <summary>
	''' �I����s�F�ς�
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h�r���[�R���g���[��</param>
	''' <remarks></remarks>
	Public Sub DataGridViewSelectionRowColorChange(ByVal dgv As DataGridView)
		' ���̊֐��ɃC�x���g�n���h�����t
		AddHandler dgv.RowEnter, AddressOf dgvData_RowEnter
		AddHandler dgv.RowLeave, AddressOf dgvData_RowLeave
	End Sub
	''' <summary>
	''' �f�[�^�O���b�h�r���[�s�I��
	''' </summary>
	Private Sub dgvData_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
		Dim dgv As DataGridView = CType(sender, DataGridView)	' �Ώۂ�dgv

		dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSelection		'�s�̔w�i��ύX
	End Sub
	''' <summary>
	''' �f�[�^�O���b�h�r���[�s��I��
	''' </summary>
	Private Sub dgvData_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
		Dim dgv As DataGridView = CType(sender, DataGridView)	' �Ώۂ�dgv

		dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty			'�s�̔w�i���f�t�H���g�F�ɖ߂�
	End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����\�ʏ�ԁE�����p�^�[�� ��荞�݊֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �����\�ʏ�� �� DB���擾���A�R���{�{�b�N�X�ɃZ�b�g
    ''' </summary>
    ''' <param name="cmb">�R���{�{�b�N�X �R���g���[��</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Public Function SetComboBox_Scode(ByVal cmb As ComboBox) As Boolean
        Dim strSQL As String = ""       ' SQL������
        strSQL = "SELECT scode, �����\�ʏ�Ԗ���, �L������"
        strSQL &= " FROM " & DB_SCODE_MASTER
        strSQL &= " ORDER BY scode ASC"

        ' SQL���s & �R���{�{�b�N�X�ɃZ�b�g
        Return SetComboBox(cmb, strSQL)
    End Function

    ''' <summary>
    ''' �����p�^�[�� �� DB���擾���A�R���{�{�b�N�X�ɃZ�b�g
    ''' </summary>
    ''' <param name="cmb">�R���{�{�b�N�X �R���g���[��</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Public Function SetComboBox_Pcode(ByVal cmb As ComboBox) As Boolean
        Dim strSQL As String = ""       ' SQL������
        strSQL = "SELECT pcode, �����p�^�[������, �L������"
        strSQL &= " FROM " & DB_PCODE_MASTER
        strSQL &= " ORDER BY pcode ASC"

        ' SQL���s & �R���{�{�b�N�X�ɃZ�b�g
        Return SetComboBox(cmb, strSQL)
    End Function

	''' <summary>
	''' �����p�^�[�� �� DB���擾���A�R���{�{�b�N�X�ɃZ�b�g(PCODE���l�Ή���)
	''' </summary>
	''' <param name="cmb">�R���{�{�b�N�X �R���g���[��</param>
	''' <returns>���A���</returns>
	''' <remarks></remarks>
	Public Function SetComboBox_Pcode_Ascii(ByVal cmb As ComboBox) As Boolean
		Dim strSQL As String = ""		' SQL������
		strSQL = "SELECT ascii(pcode) as pcode, �����p�^�[������, �L������"
		strSQL &= " FROM " & DB_PCODE_MASTER
		strSQL &= " ORDER BY pcode ASC"

		' SQL���s & �R���{�{�b�N�X�ɃZ�b�g
		Return SetComboBox(cmb, strSQL)
	End Function

    ''' <summary>
    ''' DB���擾���A�R���{�{�b�N�X�ɃZ�b�g
    ''' </summary>
    ''' <param name="cmb">�R���{�{�b�N�X �R���g���[��</param>
    ''' <param name="strSQL">SQL��</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function SetComboBox(ByVal cmb As ComboBox, ByVal strSQL As String) As Boolean
        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing

        '' �R���{�{�b�N�X ������
        cmb.Items.Clear()

        Try
            ' SQL���s
            If Not tcls_DB.Open() Then Return False
            sqlRead = tcls_DB.SelectExecute(strSQL)
            ' �f�[�^�擾
            If Not sqlRead Is Nothing Then
                While sqlRead.Read
                    '' �R���{�{�b�N�X �ɃA�C�e���ǉ� (string int ��������̂�GetValue�ɂ��Ă���)
                    cmb.Items.Add(New cls_CmbItem(CStr(sqlRead.GetValue(0)), CStr(sqlRead.GetValue(1)), CInt(sqlRead.GetValue(2))))
                End While
            End If
            Return True
        Catch ex As Exception
            Return False
        Finally
            If Not sqlRead Is Nothing Then sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Function

    ''' <summary>
    ''' �R���{�{�b�N�X�I������ �R�t�����̂��Z�b�g
    ''' </summary>
    ''' <param name="index">�R���{�{�b�N�X�̑I���ʒu</param>
    ''' <param name="cmb">�R���{�{�b�N�X</param>
    ''' <param name="lbl">���̕\�����x��</param>
    ''' <remarks></remarks>
    Public Sub SelectComboBoxItem(ByVal index As Integer, ByVal cmb As ComboBox, ByVal lbl As Label)
        Dim item As cls_CmbItem    '�R���{�{�b�N�X�̃A�C�e��
        item = CType(cmb.Items(index), cls_CmbItem)

        ' ���̂��Z�b�g
        lbl.Text = item.name
        '�L�����������Ŕw�i�F�ύX
		lbl.BackColor = CType(IIf(item.num = 0, Color.LightGray, Color.PeachPuff), Color)
    End Sub

    ''' <summary>
    ''' �R���{�{�b�N�X�̑I�����ꂢ�Ă����L�[���擾����
    ''' </summary>
    ''' <param name="index">�z��ʒu</param>
    ''' <param name="cmb">�R���{�{�b�N�X</param>
    ''' <returns>��L�[</returns>
    ''' <remarks></remarks>
    Public Function SelectComboBoxKey(ByVal index As Integer, ByVal cmb As ComboBox) As String
        Dim item As cls_CmbItem    '�R���{�{�b�N�X�̃A�C�e��
        item = CType(cmb.Items(index), cls_CmbItem)
        Return item.id
    End Function

    ''' <summary>
    ''' ��L�[����R���{�{�b�N�X�̔z��ʒu���擾����
    ''' </summary>
    ''' <param name="key">��L�[</param>
    ''' <param name="cmb">�R���{�{�b�N�X</param>
    ''' <returns>�z��ʒu</returns>
    ''' <remarks></remarks>
    Public Function SelectComboBoxIndex(ByVal key As String, ByVal cmb As ComboBox) As Integer
        Dim item As cls_CmbItem    '�R���{�{�b�N�X�̃A�C�e��
        For ii As Integer = 0 To cmb.Items.Count - 1
            item = CType(cmb.Items(ii), cls_CmbItem)
            If item.id = key Then Return ii
        Next ii
        Return -1
    End Function

    ''' <summary>
    ''' �R���{�{�b�N�X�̔C�Ӄf�[�^���폜
    ''' </summary>
    ''' <param name="cmb">�R���{�{�b�N�X</param>
    ''' <param name="strDelItem">�폜�������C�ӂ̃f�[�^</param>
    ''' <remarks></remarks>
    Public Sub Delete_CodeEtc(ByVal cmb As ComboBox, ByVal strDelItem As String)
        ' �A�C�e�������[�v
        For ii As Integer = 0 To cmb.Items.Count - 1
            ' �A�C�e����Z�Ȃ�
            If strDelItem = cmb.Items.Item(ii).ToString Then
                ' �폜
                cmb.Items.RemoveAt(ii)
                Exit For
            End If
        Next
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL�֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="strSQL">���s����SQL��</param>
    ''' <param name="DsDataSet">�f�[�^�Z�b�g�I�u�W�F�N�g</param>
    ''' <param name="AdapterData">�f�[�^�A�_�v�^�I�u�W�F�N�g</param>
    ''' <param name="strErrMsg">�G���[���b�Z�[�W</param>
    ''' <returns>�e�����󂯂��s����Ԃ��@-1:���s�@-9:DB�ڑ����s</returns>
    ''' <remarks></remarks>
    Public Function Get_SelectData(ByVal strSQL As String, ByRef DsDataSet As System.Data.DataSet, _
                                    ByRef AdapterData As SqlClient.SqlDataAdapter, ByRef strErrMsg As String) As Integer

        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        '���b�Z�[�W������
        strErrMsg = ""

        Try

            ' SQL���s
            If Not tcls_DB.Open() Then
                strErrMsg = "DB�ڑ����s"
                Return -9
            End If

            ' �f�[�^�擾
            DsDataSet.Clear()
            AdapterData = tcls_DB.SelectAdapter(strSQL)

            Return AdapterData.Fill(DsDataSet)


        Catch ex As Exception
            strErrMsg = ex.Message
            Return -1

        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function

End Class