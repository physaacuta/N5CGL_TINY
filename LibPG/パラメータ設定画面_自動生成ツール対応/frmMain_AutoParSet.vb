'*********************************************************************************
' �p�����[�^�ݒ��ʂ̐e�t�H�[��
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary.tClass

''' <summary>
''' �p�����[�^�ݒ��ʗp�̊��N���X
''' </summary>
''' <remarks></remarks>
Public Class frmMain_AutoParSet
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �f�[�^�O���b�h�r���[���
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DgvControlInfo
        Public dgv As tClassLibrary.tCnt.tCnt_DataGridView      '�f�[�^�O���b�h�r���[
        Public dgvType As EM_DATAGRIDVIEW_DISPTYPE              '�\���^�C�v
        Public wSize As ROW_VERTION_WIDTH_SIZE                  '���T�C�Y(�cVer�̂ݗL��)
    End Structure

    ''' <summary>
    ''' ��ʍ\�����
    ''' </summary>
    ''' <remarks></remarks>
    Class DataSetHolder
        Public drGridSetting() As DataRow                       '��ʍ\�����
        Public dsMain As DataSet                                '��ڑ��^�e�[�u��
        Public adapter As SqlClient.SqlDataAdapter              '�A�_�v�^�[
        Public dgvCnt As DgvControlInfo                         '�f�[�^�O���b�h�r���[���

        Public Function Rows() As DataRow()
            Return drGridSetting
        End Function

        ''' <summary>
        ''' 
        ''' </summary>
        ''' <param name="gridSettings"></param>
        ''' <param name="dsMain"></param>
        ''' <param name="adapter"></param>
        ''' <param name="dgv"></param>
        ''' <remarks></remarks>
        Sub New(ByVal gridSettings As DataRow(), ByVal dsMain As DataSet, ByVal adapter As SqlClient.SqlDataAdapter, ByVal dgv As DgvControlInfo)
            Me.drGridSetting = gridSettings
            Me.dsMain = dsMain
            Me.adapter = adapter
            Me.dgvCnt = dgv
        End Sub
    End Class

    Protected m_DataSets As New List(Of DataSetHolder)

    Public m_DBlst As New Dictionary(Of String, TABLE_INFO)      ' �g�pDB��� (DB����, �f�[�^)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �񋓑̒�`
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Enum EM_DATAGRIDVIEW_CELLTYPE
        '�f�[�^�O���b�h�Z���^�C�v
        emTextBoxCell = 0
        emComboBoxCell = 1
        emCheckBoxCell = 2
        emButtonCell = 3
    End Enum

    Public Enum EM_DATAGRIDVIEW_DISPTYPE
        '�\���^�C�v
        emColType = 0             ' ��w�b�_�[�^�C�v(��)
        emRowType = 1             ' �s�w�b�_�[�^�C�v(�c)
    End Enum

    Public Enum EM_DB_TYPE
        ' �f�[�^�^�C�v
        emIntType = 0
        emFloatType = 1
        emStrType = 2
        emBitType = 3
    End Enum

    ' �f�[�^�O���b�h�r���[�̗� (�\�ʏ�ԕʋ���)
    Public Enum EM_ROW_VERTION_ID
        emNo = 0                ' No
        emName                  ' ����
        emValue                 ' �l
        emRange                 ' �͈�
    End Enum

    Public Enum EM_DLG_TYPE
        ' �f�[�^�O���b�h�_�u���N���b�N���ɕ\������_�C�A���O�^�C�v
        emNoSet = 0
        emFileDialog = 1
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �\����
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗� (�\�ʏ�ԕʋ���)
    Public Structure ROW_VERTION_WIDTH_SIZE
        Dim nNo As Integer      ' No
        Dim nName As Integer    ' ����
        Dim nValue As Integer   ' �l
        Dim nRange As Integer   ' �͈�
        Public Sub initialize()
            nNo = 0
            nName = 0
            nValue = 0
            nRange = 0
        End Sub
    End Structure

    '��L�[��� �\����
    Private Structure MAIN_KEY_INFO
        Dim nKeyPos As Integer                                          ' �\���ʒu
        Dim sKeyName As String                                          ' ��L�[����
        Public Sub initialize()
            nKeyPos = 0
            sKeyName = ""
        End Sub
    End Structure

    '�g�pDB��� �\����
    Public Structure TABLE_INFO
        Dim sTable As String                                            'DB����
        Dim SelectSQLFormat As String                                   'DB�Q�Ɨp�t�H�[�}�b�g
        Dim UpdateSQLFormat As String                                   '�ꊇ�R�s�[��pUpDate�t�H�[�}�b�g
        Dim bCopyDB As Boolean                                          '�ꊇ�R�s�[���{�L��

        Public Sub initialize()
            sTable = ""
            SelectSQLFormat = ""
            UpdateSQLFormat = ""
            bCopyDB = False
        End Sub
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' ���ʂ̐F�R�[�h
    Public g_ColorSetting As Color = Color.PeachPuff                    '�I���Z���̐F�ݒ�i�f�[�^�O���b�h�r���[�̃`�F�b�N�{�b�N�X����j
    Public g_ColorSelection As Color = Color.LightGreen                 '�I���Z����s���̐F�ς�

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
    Private my_frmID As Integer                             ' �����̉��ID
    Private my_paramID As Integer                           ' �����̃p�����[�^�e�[�u��ID

    ''' <summary>
    ''' �R���{�{�b�N�X�ɕ\������f�[�^
    ''' </summary>
    ''' <remarks>�����\�ʏ�ԁE�����p�^�[�� �p</remarks>
    Public Class cls_CmbItem
        ''' <summary>��L�[</summary>
        Public id As String                                 ' ��L�[
        ''' <summary>�R�t������</summary>
        Public name As String                               ' �R�t������
        ''' <summary>�L�������t���O 0:���� 1:�L��</summary>
        Public num As Integer                               ' �L�������t���O 0:���� 1:�L��

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

    Public Property tProp_MyParamID() As Integer
        Get
            Return my_paramID
        End Get
        Set(ByVal value As Integer)
            my_paramID = value
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
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSelection      '�s�̔w�i��ύX
    End Sub
    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�s��I��
    ''' </summary>
    Private Sub dgvData_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty           '�s�̔w�i���f�t�H���g�F�ɖ߂�
    End Sub

    ''' <summary>
    ''' �O���b�h�f�[�^�G���[����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Sub dgvData_DataError(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewDataErrorEventArgs)
        ' �f�[�^�O���b�h�̃f�[�^�G���[���ɌĂ΂��
        ' �������Ȃ��Ƃ������Ȃ��ŁI�I
        MsgBox("���͕�����̌`��������������܂���")
        'e.Cancel = False
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
        Dim strSQL As String = ""       ' SQL������
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



    ''' <summary>
    ''' �f�[�^�̎擾
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Protected Overridable Function DB_Select() As Boolean
        Try
            For Each Data As DataSetHolder In m_DataSets
                Dim strErrMsg As String = ""
                If 0 > Get_SelectData(Create_Sql_Select(Data), Data.dsMain, Data.adapter, strErrMsg) Then
                    Return False
                End If
            Next
            Return True
        Catch ex As Exception
            Return False
        End Try
    End Function

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Protected Overridable Function Data_Check() As Boolean
        Try
            For Each Data As DataSetHolder In m_DataSets
                If Data.dgvCnt.dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType Then
                    If Not Data_Check_RowHeader(Data) Then Return False
                Else
                    If Not Data_Check_ColHeader(Data) Then Return False
                End If
            Next
            Return True
        Catch ex As Exception
            Return False
        End Try
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Protected Overridable Function Data_Get() As Boolean
        Dim strErrMsg As String = ""
        For Each Data As DataSetHolder In m_DataSets
            If Data.dgvCnt.dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType Then
                If Not Data_Get_RowHeader(Data, strErrMsg) Then

                End If
            Else
                If Not Data_Get_ColHeader(Data, strErrMsg) Then

                End If
            End If
        Next
        Return True
    End Function

    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Overridable Sub dgvInitialize()
        For Each Data As DataSetHolder In m_DataSets
            If Data.dgvCnt.dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType Then
                dgvInitialize_RowHeader(Data.dgvCnt.dgv)
            Else
                Dim cmbColumns As New Generic.SortedList(Of String, DataGridViewComboBoxColumn)
                dgvInitialize_ColHeader(Data.dgvCnt.dgv, True)
                MakeComb_ColHeader(Data, cmbColumns)
                setGridCellInfo4ColHeader(Data, cmbColumns, DataGridViewColumnSortMode.NotSortable)
            End If
            '�O���b�h�f�[�^�G���[����
            AddHandler Data.dgvCnt.dgv.DataError, AddressOf dgvData_DataError
        Next
    End Sub

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <param name="Key">�����\�ʏ��(scode)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overridable Function Data_Disp(Optional ByVal Key As String = "") As Boolean
        For Each Data As DataSetHolder In m_DataSets
            If Data.dgvCnt.dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType Then
                '�O���b�h�����ݒ�E�f�[�^�\�������i�s�w�b�_�^�C�v�p�j
                GridSet(Key, Data)
                '�f�[�^�O���b�h�r���[���ݒ�
                dgvWidthSet_RowHeader(Data.dgvCnt.dgv, Data.dgvCnt.wSize.nNo, Data.dgvCnt.wSize.nName, Data.dgvCnt.wSize.nValue, Data.dgvCnt.wSize.nRange)
            Else
                '�f�[�^�\�������i��w�b�_�^�C�v�p�j
                Set_DataSource(Key, Data)
            End If
            '�s�̔w�i���f�t�H���g�F�ɖ߂�
            SetBackColorEmpty(Data.dgvCnt.dgv)
        Next

        Return True
    End Function

    ''' <summary>
    ''' �f�[�^�\�[�X�Z�b�g
    ''' </summary>
    ''' <param name="key">�����\�ʏ��(scode)</param>
    ''' <param name="Data"></param>
    ''' <remarks></remarks>
    Private Sub Set_DataSource(ByVal key As String, ByVal Data As DataSetHolder)
        If Not CopyTargetTable(Data) Then
            Data.dgvCnt.dgv.DataSource = New DataView(Data.dsMain.Tables(0))
        Else
            Data.dgvCnt.dgv.DataSource = New DataView(Data.dsMain.Tables(0), "scode=" & key, "scode", DataViewRowState.CurrentRows)
        End If
    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[����ʏ����擾
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function Get_GridInfo(ByVal dgv As DataGridView) As DataSetHolder
        Dim Data As DataSetHolder = Nothing
        For Each dtWk As DataSetHolder In m_DataSets
            If dtWk.dgvCnt.dgv.Equals(dgv) Then
                Data = dtWk
                Exit For
            End If
        Next
        Return Data
    End Function

    ''' <summary>
    ''' ���񃍁[�f�B���O����(�����\�ʏ��(scode)����)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub InitLoading()
        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        dgvInitialize()

        ' �f�[�^�̎擾
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
            Exit Sub
        End If

        ' ��ʂɒl��\��
        If Not Data_Disp() Then
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        For Each Data As DataSetHolder In m_DataSets
            Data.dgvCnt.dgv.DataGridViewSelectionRowColorChange(g_ColorSelection)
            DataGridViewColumnWidthChanged(Data.dgvCnt.dgv)
        Next

        '=======================================================
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
    ''' ��ʍ\������DB����擾(�����̂�)
    ''' </summary>
    ''' <param name="cID">���ID</param>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[���</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function InitGridSetting(ByVal cID As String, ByVal ParamArray dgv() As DgvControlInfo) As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing
        Dim dsTmp As New DataSet


        m_DataSets.Clear()
        Try
            strSQL = "SELECT * FROM " & DB_PAR_GRID_SETTING & " WHERE ���ID = '" & cID & "'"

            'SQL�����s
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                Return False
            End If
            sqlAdp.Fill(dsTmp)
            For ii As Integer = 1 To 100
                Dim rows() As DataRow = dsTmp.Tables(0).Select(String.Format("�O���b�hID = '{0}'", ii))

                strSQL = "SELECT * FROM " & DB_PAR_GRID_SETTING & " WHERE ���ID = '" & cID & "' and  �O���b�hID = " & ii
                Dim dtstWK As System.Data.DataSet = New System.Data.DataSet
                Dim sqlAdpWK As System.Data.SqlClient.SqlDataAdapter = Nothing
                If 0 > Get_SelectData(strSQL, dtstWK, sqlAdpWK, strErrMsg) Then
                    Return False
                End If

                'If rows Is Nothing Then Exit For
                If rows.Length = 0 Then Exit For
                m_DataSets.Add(New DataSetHolder(rows, dtstWK, sqlAdpWK, dgv(ii - 1)))
            Next ii

        Catch ex As Exception
            Return False
        End Try
        Return True
    End Function


    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Public Function subData_UpDate() As Boolean

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' ���͒l�`�F�b�N
        If Not Data_Check() Then
            Return False
        End If

        Data_Get()

        Try
            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then
                Return False
            End If

            ' �z���g�ɓo�^        
            If DB_UpDate() Then
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then
                    Return False
                End If
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
                Return False
            End If
            Return True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�m�菈�� �ُ�[{0}]", ex.Message))
            Return False
        Finally
            If frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Function


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






    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ��ʍ\������DB����擾(�}�j���A���p)
    ''' </summary>
    ''' <param name="cID">���ID</param>
    ''' <param name="nNo">�O���b�h�ԍ�</param>
    ''' <param name="gdtGridSetting">��ʍ\�����(OUT)</param>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Public Function gblnGetGridSetting(ByVal cID As String, ByVal nNo As Integer, ByRef gdtGridSetting As DataTable) As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing

        Try
            strSQL = "SELECT * FROM " & DB_PAR_GRID_SETTING & " WHERE ���ID = '" & cID & "' and  �O���b�hID = " & nNo

            'SQL�����s
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                Return False
            End If
            sqlAdp.Fill(gdtGridSetting)
        Catch ex As Exception
            Return False
        End Try
        Return True
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' dgv�������i�s�w�b�_�^�C�v�j
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize_RowHeader(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        '''' ��{��ݒ�
        'Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                     ' �I��F����

        '''' �Ǝ��̍��ڂ�ݒ�
        With dgv
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                               ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                                         ' �Z���G���[�̕\��
            .ShowCellToolTips = False                                       ' �c�[���q���g�̕\��
            .ShowEditingIcon = False                                        ' �ҏW���̃Z���̖���
            .ShowRowErrors = False                                          ' �G���[�s�̖���
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = False                                               ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' �I���Z���F��`
            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .DefaultCellStyle().SelectionBackColor = dgv.DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter

        End With


    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[���ݒ�i�s�w�b�_�^�C�v�j
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <param name="n1">No  �F��[pixel] ����{0</param>
    ''' <param name="n2">���́F��[pixel]</param>
    ''' <param name="n3">�l  �F��[pixel]</param>
    ''' <param name="n4">���l�F��[pixel]</param>
    ''' <remarks></remarks>
    Private Sub dgvWidthSet_RowHeader(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal n1 As Integer, ByVal n2 As Integer, ByVal n3 As Integer, ByVal n4 As Integer)
        With dgv
            .Columns(EM_ROW_VERTION_ID.emNo).Width = n1
            .Columns(EM_ROW_VERTION_ID.emName).Width = n2
            .Columns(EM_ROW_VERTION_ID.emValue).Width = n3
            .Columns(EM_ROW_VERTION_ID.emRange).Width = n4
        End With
    End Sub

    ''' <summary>
    ''' dgv�������i��w�b�_�^�C�v�j
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <param name="bResizeColumns">��T�C�Y�ύX�L��(True:��T�C�Y�ύX��)</param>
    ''' <remarks></remarks>
    Public Sub dgvInitialize_ColHeader(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, Optional ByVal bResizeColumns As Boolean = False)

        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .RowCount = 0

            .AllowUserToResizeColumns = bResizeColumns                      ' ���[�U�[ ��T�C�Y�ύX
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���

        End With

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �Z������ݒ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �Z������ݒ肵�܂��i�s�w�b�_�^�C�v�j�B
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="comboboxCells">�R���{�{�b�N�X�Z�����</param>
    ''' <remarks></remarks>
    Private Sub setGridCellInfo4RowHeader(ByVal gdtGridSetting As DataSetHolder, _
                ByRef comboboxCells As Generic.SortedList(Of String, DataGridViewComboBoxCell))

        With gdtGridSetting.dgvCnt.dgv

            .Columns(EM_ROW_VERTION_ID.emNo).Visible = False

            .Columns(EM_ROW_VERTION_ID.emNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_ROW_VERTION_ID.emName).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_ROW_VERTION_ID.emValue).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_ROW_VERTION_ID.emRange).SortMode = DataGridViewColumnSortMode.NotSortable

            For Each gridSetRow As DataRow In gdtGridSetting.Rows

                Dim rowId As Integer = CInt(gridSetRow.Item("����No")) - 1
                'Alignment�ݒ�
                Dim tmpAlignment As DataGridViewContentAlignment = CType(gridSetRow.Item("�\��_���ڔz�u"), DataGridViewContentAlignment)
                If tmpAlignment = DataGridViewContentAlignment.NotSet Then
                    .Rows(rowId).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                Else
                    .Rows(rowId).DefaultCellStyle.Alignment = tmpAlignment
                End If
                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emName).Style.Alignment = DataGridViewContentAlignment.MiddleLeft
                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Style.Alignment = DataGridViewContentAlignment.MiddleLeft

                '�\����\���ݒ�()
                '.Rows(rowId).Visible = CBool(gridSetRow.Item("�\��_���ڕ\���L��"))
                If CBool(gridSetRow.Item("�\��_���ڕ\���L��")) = False Then
                    If CBool(gridSetRow.Item("DB_��L�[")) Then
                        .Rows(rowId).Height = 0
                    Else
                        .Rows(rowId).Visible = False
                    End If
                End If

                .Rows(rowId).ReadOnly = True

                '��P�ʂɐݒ肪�K�v�ȕ�����ݒ�
                '��U�Z�����N���A
                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue).Dispose()
                '�O���b�h�Z���ݒ�
                Select Case CType(gridSetRow.Item("���ڃZ���^�C�v"), EM_DATAGRIDVIEW_CELLTYPE)
                    Case EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell
                        .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = New DataGridViewTextBoxCell
                    Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                        Dim tmpStr As String = gridSetRow.Item("DB_�t�B�[���h��").ToString().ToUpper()

                        .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = New DataGridViewComboBoxCell
                        For Each key As String In comboboxCells.Keys
                            If key.ToUpper = tmpStr Then
                                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = DirectCast(comboboxCells.Item(key).Clone(), DataGridViewCell)
                                Exit For
                            End If
                        Next key

                    Case EM_DATAGRIDVIEW_CELLTYPE.emCheckBoxCell
                        .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = New DataGridViewCheckBoxCell
                    Case EM_DATAGRIDVIEW_CELLTYPE.emButtonCell
                        .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue) = New DataGridViewButtonCell

                End Select
                .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue).ReadOnly = CBool(gridSetRow.Item("�\��_�ǂݎ���p"))
                If CBool(gridSetRow.Item("�\��_�ǂݎ���p")) Then
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue).Style.ForeColor = Color.DimGray
                Else
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emValue).Style.ForeColor = Color.Black
                End If


                ' �f�[�^�O���b�h�_�u���N���b�N���ɕ\������_�C�A���O�^�C�v
                If EM_DLG_TYPE.emFileDialog = CInt(gridSetRow.Item("����_�\���_�C�A���O")) Then
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Dispose()
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange) = New DataGridViewButtonCell
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Style.BackColor = Color.FromName("Control")
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Style.Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                    '.Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).UseColumnTextForButtonValue = True
                    .Rows(rowId).Cells(EM_ROW_VERTION_ID.emRange).Value = "�Q ��"
                End If

                'PK���ڂ̗�Œ�
                .Rows(rowId).Frozen = CBool(gridSetRow.Item("�\��_��Œ�"))

                '�\���t�H�[�}�b�g
                If "" <> CStr(gridSetRow.Item("�\��_�t�H�[�}�b�g")) Then
                    .Rows(rowId).DefaultCellStyle.Format = CStr(gridSetRow.Item("�\��_�t�H�[�}�b�g"))
                End If

            Next gridSetRow

        End With
    End Sub

    ''' <summary>
    ''' �Z������ݒ肵�܂��i��w�b�_�^�C�v�j�B
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="comboboxCells">�R���{�{�b�N�X�Z�����</param>
    ''' <param name="sortMode">�\�[�g���[�h�̐ݒ�</param>
    ''' <remarks></remarks>
    Protected Sub setGridCellInfo4ColHeader(ByVal gdtGridSetting As DataSetHolder, _
                ByRef comboboxCells As Generic.SortedList(Of String, DataGridViewComboBoxColumn), _
                Optional ByVal sortMode As DataGridViewColumnSortMode = DataGridViewColumnSortMode.Automatic)

        With gdtGridSetting.dgvCnt.dgv
            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                Dim rowId As Integer = CInt(gridSetRow.Item("����No")) - 1

                ' �s�����Ă���񐔂�ǉ�
                While (True)
                    If .Columns.Count - 1 < rowId Then
                        'gridView.Columns.Add("", "")
                        Select Case CType(gridSetRow.Item("���ڃZ���^�C�v"), EM_DATAGRIDVIEW_CELLTYPE)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell
                                .Columns.Add(New DataGridViewTextBoxColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                                'gridView.Columns.Add(New DataGridViewComboBoxColumn)
                                Dim comboColumn As DataGridViewComboBoxColumn = CType(comboboxCells(CStr(gridSetRow.Item("DB_�t�B�[���h��"))).Clone(), DataGridViewComboBoxColumn)
                                .Columns.Add(comboColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emCheckBoxCell
                                .Columns.Add(New DataGridViewCheckBoxColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emButtonCell
                                .Columns.Add(New DataGridViewButtonColumn)
                        End Select

                    Else
                        Exit While
                    End If
                End While

                '�\����\���ݒ�
                .Columns(rowId).Visible = CBool(gridSetRow.Item("�\��_���ڕ\���L��"))

                '�\�����ݒ�
                .Columns(rowId).Width = CInt(gridSetRow.Item("�\��_���ڕ�"))

                '�f�[�^�v���p�e�B���ݒ�
                .Columns(rowId).DataPropertyName = CStr(gridSetRow.Item("DB_�t�B�[���h��"))

                '�w�b�_�^�C�g���̐ݒ�
                Dim headTitle As New System.Text.StringBuilder
                headTitle.Append(gridSetRow.Item("���ڃw�b�_�[1"))
                If Not isNullOrEmpty(gridSetRow.Item("���ڃw�b�_�[2")) Then
                    headTitle.Append(vbCr)
                    headTitle.Append(gridSetRow.Item("���ڃw�b�_�[2"))
                End If
                .Columns(rowId).HeaderText = headTitle.ToString

                'Alignment�ݒ�
                Dim tmpAlignment As DataGridViewContentAlignment = CType(gridSetRow.Item("�\��_���ڔz�u"), DataGridViewContentAlignment)
                If tmpAlignment = DataGridViewContentAlignment.NotSet Then
                    .Columns(rowId).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                Else
                    .Columns(rowId).DefaultCellStyle.Alignment = tmpAlignment
                End If

                '�\�[�g���̐ݒ�
                If sortMode <> DataGridViewColumnSortMode.Automatic Then
                    .Columns(rowId).SortMode = sortMode
                End If

                .Columns(rowId).ReadOnly = CBool(gridSetRow.Item("�\��_�ǂݎ���p"))
                If CBool(gridSetRow.Item("�\��_�ǂݎ���p")) = True Then
                    .Columns(rowId).DefaultCellStyle.ForeColor = Color.DimGray
                Else
                    .Columns(rowId).DefaultCellStyle.ForeColor = Color.Black
                End If

                'PK���ڂ̗�Œ�
                .Columns(rowId).Frozen = CBool(gridSetRow.Item("�\��_��Œ�"))
                '�\���t�H�[�}�b�g
                If "" <> CStr(gridSetRow.Item("�\��_�t�H�[�}�b�g")) Then
                    .Columns(rowId).DefaultCellStyle.Format = CStr(gridSetRow.Item("�\��_�t�H�[�}�b�g"))
                End If

            Next gridSetRow

            'If .ColumnCount > gdtGridSetting.Rows.Length Then
            '    For ii As Integer = gdtGridSetting.Rows.Length To .ColumnCount - 1
            '        .Columns(ii).Visible = False
            '    Next
            'End If

        End With
    End Sub

    ''' <summary>
    ''' �Z������ݒ肵�܂��i��w�b�_�^�C�v�j[�}�j���A���p]
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="comboboxCells">�R���{�{�b�N�X�Z�����</param>
    ''' <param name="sortMode">�\�[�g���[�h�̐ݒ�</param>
    ''' <remarks></remarks>
    Protected Sub setGridCellInfo4ColHeader(ByVal gdtGridSetting As DataTable, ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, _
                ByRef comboboxCells As Generic.SortedList(Of String, DataGridViewComboBoxColumn), _
                Optional ByVal sortMode As DataGridViewColumnSortMode = DataGridViewColumnSortMode.Automatic)

        With dgv
            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                Dim rowId As Integer = CInt(gridSetRow.Item("����No")) - 1

                ' �s�����Ă���񐔂�ǉ�
                While (True)
                    If .Columns.Count - 1 < rowId Then
                        'gridView.Columns.Add("", "")
                        Select Case CType(gridSetRow.Item("���ڃZ���^�C�v"), EM_DATAGRIDVIEW_CELLTYPE)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell
                                .Columns.Add(New DataGridViewTextBoxColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                                'gridView.Columns.Add(New DataGridViewComboBoxColumn)
                                Dim comboColumn As DataGridViewComboBoxColumn = CType(comboboxCells(CStr(gridSetRow.Item("DB_�t�B�[���h��"))).Clone(), DataGridViewComboBoxColumn)
                                .Columns.Add(comboColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emCheckBoxCell
                                .Columns.Add(New DataGridViewCheckBoxColumn)
                            Case EM_DATAGRIDVIEW_CELLTYPE.emButtonCell
                                .Columns.Add(New DataGridViewButtonColumn)
                        End Select

                    Else
                        Exit While
                    End If
                End While

                '�\����\���ݒ�
                .Columns(rowId).Visible = CBool(gridSetRow.Item("�\��_���ڕ\���L��"))

                '�\�����ݒ�
                .Columns(rowId).Width = CInt(gridSetRow.Item("�\��_���ڕ�"))

                '�f�[�^�v���p�e�B���ݒ�
                .Columns(rowId).DataPropertyName = CStr(gridSetRow.Item("DB_�t�B�[���h��"))

                '�w�b�_�^�C�g���̐ݒ�
                Dim headTitle As New System.Text.StringBuilder
                headTitle.Append(gridSetRow.Item("���ڃw�b�_�[1"))
                If Not isNullOrEmpty(gridSetRow.Item("���ڃw�b�_�[2")) Then
                    headTitle.Append(vbCr)
                    headTitle.Append(gridSetRow.Item("���ڃw�b�_�[2"))
                End If
                .Columns(rowId).HeaderText = headTitle.ToString

                'Alignment�ݒ�
                Dim tmpAlignment As DataGridViewContentAlignment = CType(gridSetRow.Item("�\��_���ڔz�u"), DataGridViewContentAlignment)
                If tmpAlignment = DataGridViewContentAlignment.NotSet Then
                    .Columns(rowId).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                Else
                    .Columns(rowId).DefaultCellStyle.Alignment = tmpAlignment
                End If

                '�\�[�g���̐ݒ�
                If sortMode <> DataGridViewColumnSortMode.Automatic Then
                    .Columns(rowId).SortMode = sortMode
                End If

                .Columns(rowId).ReadOnly = CBool(gridSetRow.Item("�\��_�ǂݎ���p"))
                If CBool(gridSetRow.Item("�\��_�ǂݎ���p")) = True Then
                    .Columns(rowId).DefaultCellStyle.ForeColor = Color.DimGray
                Else
                    .Columns(rowId).DefaultCellStyle.ForeColor = Color.Black
                End If

                'PK���ڂ̗�Œ�
                .Columns(rowId).Frozen = CBool(gridSetRow.Item("�\��_��Œ�"))
                '�\���t�H�[�}�b�g
                If "" <> CStr(gridSetRow.Item("�\��_�t�H�[�}�b�g")) Then
                    .Columns(rowId).DefaultCellStyle.Format = CStr(gridSetRow.Item("�\��_�t�H�[�}�b�g"))
                End If

            Next gridSetRow

            'If .ColumnCount > gdtGridSetting.Rows.Length Then
            '    For ii As Integer = gdtGridSetting.Rows.Length To .ColumnCount - 1
            '        .Columns(ii).Visible = False
            '    Next
            'End If

        End With
    End Sub

    ''' <summary>
    ''' �O���b�h�����ݒ�E�f�[�^�\�������i�s�w�b�_�^�C�v�p�j
    ''' </summary>
    ''' <param name="key">��L�[�̒l(scode)</param>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <remarks></remarks>
    Private Sub GridSet(ByVal key As String, ByVal gdtGridSetting As DataSetHolder)
        Dim dtRow As DataRow
        Dim dttbl As DataTable

        ' DGV�pDataTable�쐬
        Dim dc0 As New DataColumn
        Dim dc1 As New DataColumn
        Dim dc2 As New DataColumn
        Dim dc3 As New DataColumn
        dc0.ColumnName = "No"
        dc0.DataType = GetType(Integer)
        dc1.ColumnName = "����"
        dc1.DataType = GetType(Object)
        dc2.ColumnName = "�l"
        dc2.DataType = GetType(Object)
        dc3.ColumnName = "���l"
        dc3.DataType = GetType(Object)
        dttbl = New DataTable
        dttbl.Columns.AddRange(New System.Data.DataColumn() {dc0, dc1, dc2, dc3})
        gdtGridSetting.dgvCnt.dgv.DataSource = dttbl

        '�R���g���[���̃��C�A�E�g ���W�b�N���ꎞ�I�ɒ��f
        gdtGridSetting.dgvCnt.dgv.SuspendLayout()

        ' �ꊇ�̃e�[�u���Ȃ̂ŁA�����p�^�[�����̒l�̂ݎ��o��
        Dim strWhere As String = ""                                  ' WHERE��ɑ����̏�������
        If CopyTargetTable(gdtGridSetting) Then
            strWhere = String.Format("scode='{0}'", key)                                  ' WHERE��ɑ����̏�������
        End If
        Dim drowScode() As DataRow = gdtGridSetting.dsMain.Tables(0).Select(strWhere)

        ' �e�[�u���̍s�Ɨ�����ւ�
        dttbl.Rows.Clear()
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            dtRow = dttbl.NewRow()
            dtRow.Item(EM_ROW_VERTION_ID.emNo) = gridSetRow.Item("����No")
            Dim strName As String = ""
            If Not gridSetRow.Item("���ڃw�b�_�[1") Is DBNull.Value Then
                strName &= CStr(gridSetRow.Item("���ڃw�b�_�[1"))
            End If
            If Not gridSetRow.Item("���ڃw�b�_�[2") Is DBNull.Value Then
                strName &= CStr(gridSetRow.Item("���ڃw�b�_�[2"))
            End If
            dtRow.Item(EM_ROW_VERTION_ID.emName) = strName.ToString

            If (drowScode(0).Table.Columns.Count > 0) Then
                dtRow.Item(EM_ROW_VERTION_ID.emValue) = drowScode(0).Item(gridSetRow.Item("DB_�t�B�[���h��").ToString)
            End If

            dtRow.Item(EM_ROW_VERTION_ID.emRange) = gridSetRow.Item("���l").ToString
            dttbl.Rows.Add(dtRow)
        Next

        Dim cmbCells As New Generic.SortedList(Of String, DataGridViewComboBoxCell)
        MakeComb_RowHeader(gdtGridSetting, cmbCells)

        '�Z������ݒ肵�܂�
        gdtGridSetting.dgvCnt.dgv.CurrentCell = Nothing
        setGridCellInfo4RowHeader(gdtGridSetting, cmbCells)

        '�ʏ�̃��C�A�E�g ���W�b�N���ĊJ
        gdtGridSetting.dgvCnt.dgv.ResumeLayout()

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �f�[�^���̓`�F�b�N
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �f�[�^���̓`�F�b�N�i�s�w�b�_�^�C�v�j
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Public Function Data_Check_RowHeader(ByVal gdtGridSetting As DataSetHolder) As Boolean
        Dim strWk As String


        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            '�ǂݎ���p���H
            If CBool(gridSetRow.Item("�\��_�ǂݎ���p")) Then Continue For
            '���̓^�C�v�ȊO���H
            If EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell <> CInt(gridSetRow.Item("���ڃZ���^�C�v")) Then Continue For

            Dim nRowId As Integer = CInt(gridSetRow.Item("����No")) - 1

            strWk = ""
            If Not gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value Is DBNull.Value Then
                strWk = CStr(gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value)
            End If

            Dim strMsg As String = ""
            If Not gridSetRow.Item("���ڃw�b�_�[1") Is DBNull.Value Then
                strMsg &= CStr(gridSetRow.Item("���ڃw�b�_�[1"))
            End If
            If Not gridSetRow.Item("���ڃw�b�_�[2") Is DBNull.Value Then
                strMsg &= CStr(gridSetRow.Item("���ڃw�b�_�[2"))
            End If

            Select Case CInt(gridSetRow.Item("DB_�t�B�[���h�^�C�v"))
                Case EM_DB_TYPE.emIntType      'Int
                    Dim nMax As Long = Integer.MaxValue
                    Dim nMin As Long = Integer.MinValue
                    If Not gridSetRow.Item("����_���͉\�ő�l") Is DBNull.Value Then
                        nMax = CInt(gridSetRow.Item("����_���͉\�ő�l"))
                    End If
                    If Not gridSetRow.Item("����_���͉\�ŏ��l") Is DBNull.Value Then
                        nMin = CInt(gridSetRow.Item("����_���͉\�ŏ��l"))
                    End If
                    Dim bNullOK As Boolean = CBool(gridSetRow.Item("����_NULL���͉�"))
                    If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, nMin, nMax, bNullOK, strMsg, True) <> 0 Then Return False
                    gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value = CInt(strWk)
                Case EM_DB_TYPE.emFloatType      'float
                    Dim dMax As Double = Double.MaxValue
                    Dim dMin As Double = Double.MinValue
                    If Not gridSetRow.Item("����_���͉\�ő�l") Is DBNull.Value Then
                        dMax = CDbl(gridSetRow.Item("����_���͉\�ő�l"))
                    End If
                    If Not gridSetRow.Item("����_���͉\�ŏ��l") Is DBNull.Value Then
                        dMin = CDbl(gridSetRow.Item("����_���͉\�ŏ��l"))
                    End If
                    Dim bNullOK As Boolean = CBool(gridSetRow.Item("����_NULL���͉�"))
                    Dim nDecimalLen As Integer = 0
                    If Not gridSetRow.Item("����_�����_�ȉ�����") Is DBNull.Value Then
                        nDecimalLen = CInt(gridSetRow.Item("����_�����_�ȉ�����"))
                    End If
                    If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, dMin, dMax, bNullOK, nDecimalLen, strMsg, True) <> 0 Then Return False
                    gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value = CDbl(strWk)
                Case EM_DB_TYPE.emStrType      'char
                    Dim nMaxLen As Integer = Integer.MaxValue
                    Dim nMinLen As Integer = 0
                    If Not gridSetRow.Item("����_���͉\����") Is DBNull.Value Then
                        nMaxLen = CInt(gridSetRow.Item("����_���͉\����"))
                    End If
                    If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, nMinLen, nMaxLen, strMsg, True) <> 0 Then Return False
                    gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nRowId).Value = strWk
            End Select
        Next

        Return True

    End Function

    ''' <summary>
    ''' �f�[�^���̓`�F�b�N�i��w�b�_�^�C�v�j
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Public Function Data_Check_ColHeader(ByVal gdtGridSetting As DataSetHolder) As Boolean

        Dim strWk As String = ""
        For ii As Integer = 0 To gdtGridSetting.dgvCnt.dgv.Rows.Count - 1

            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                '�ǂݎ���p���H
                If CBool(gridSetRow.Item("�\��_�ǂݎ���p")) Then Continue For
                '���̓^�C�v�ȊO���H
                If EM_DATAGRIDVIEW_CELLTYPE.emTextBoxCell <> CInt(gridSetRow.Item("���ڃZ���^�C�v")) Then Continue For

                Dim nColId As Integer = CInt(gridSetRow.Item("����No")) - 1

                strWk = ""
                If Not gdtGridSetting.dgvCnt.dgv(nColId, ii).Value Is DBNull.Value Then
                    strWk = CStr(gdtGridSetting.dgvCnt.dgv(nColId, ii).Value)
                End If

                Dim strMsg As String = ""
                If Not gridSetRow.Item("���ڃw�b�_�[1") Is DBNull.Value Then
                    strMsg &= CStr(gridSetRow.Item("���ڃw�b�_�[1"))
                End If
                If Not gridSetRow.Item("���ڃw�b�_�[2") Is DBNull.Value Then
                    strMsg &= CStr(gridSetRow.Item("���ڃw�b�_�[2"))
                End If

                Select Case CInt(gridSetRow.Item("DB_�t�B�[���h�^�C�v"))
                    Case EM_DB_TYPE.emIntType      'Int
                        Dim nMax As Long = Integer.MaxValue
                        Dim nMin As Long = Integer.MinValue
                        If Not gridSetRow.Item("����_���͉\�ő�l") Is DBNull.Value Then
                            nMax = CInt(gridSetRow.Item("����_���͉\�ő�l"))
                        End If
                        If Not gridSetRow.Item("����_���͉\�ŏ��l") Is DBNull.Value Then
                            nMin = CInt(gridSetRow.Item("����_���͉\�ŏ��l"))
                        End If
                        Dim bNullOK As Boolean = CBool(gridSetRow.Item("����_NULL���͉�"))
                        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, nMin, nMax, bNullOK, strMsg, True) <> 0 Then Return False
                        gdtGridSetting.dgvCnt.dgv(nColId, ii).Value = CInt(strWk)
                    Case EM_DB_TYPE.emFloatType      'float
                        Dim dMax As Double = Double.MaxValue
                        Dim dMin As Double = Double.MinValue
                        If Not gridSetRow.Item("����_���͉\�ő�l") Is DBNull.Value Then
                            dMax = CDbl(gridSetRow.Item("����_���͉\�ő�l"))
                        End If
                        If Not gridSetRow.Item("����_���͉\�ŏ��l") Is DBNull.Value Then
                            dMin = CDbl(gridSetRow.Item("����_���͉\�ŏ��l"))
                        End If
                        Dim bNullOK As Boolean = CBool(gridSetRow.Item("����_NULL���͉�"))
                        Dim nDecimalLen As Integer = 0
                        If Not gridSetRow.Item("����_�����_�ȉ�����") Is DBNull.Value Then
                            nDecimalLen = CInt(gridSetRow.Item("����_�����_�ȉ�����"))
                        End If
                        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, dMin, dMax, bNullOK, nDecimalLen, strMsg, True) <> 0 Then Return False
                        gdtGridSetting.dgvCnt.dgv(nColId, ii).Value = CDbl(strWk)
                    Case EM_DB_TYPE.emStrType      'char
                        Dim nMaxLen As Integer = Integer.MaxValue
                        Dim nMinLen As Integer = 0
                        If Not gridSetRow.Item("����_���͉\����") Is DBNull.Value Then
                            nMaxLen = CInt(gridSetRow.Item("����_���͉\����"))
                        End If
                        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, nMinLen, nMaxLen, strMsg, True) <> 0 Then Return False
                        gdtGridSetting.dgvCnt.dgv(nColId, ii).Value = strWk
                End Select
            Next
        Next ii

        Return True

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �R���{�{�b�N�X�ҏW
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���{�{�b�N�X�ҏW�i�s�w�b�_�^�C�v�j
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="comboboxCells">�ΏۃR���{�{�b�N�X(OUT)</param>
    ''' <remarks></remarks>
    Public Sub MakeComb_RowHeader(ByVal gdtGridSetting As DataSetHolder, ByRef comboboxCells As Generic.SortedList(Of String, DataGridViewComboBoxCell))
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim dttbl As System.Data.DataTable = New System.Data.DataTable
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing

        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            Select Case CType(gridSetRow.Item("���ڃZ���^�C�v"), EM_DATAGRIDVIEW_CELLTYPE)
                Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                Case Else
                    Continue For
            End Select

            Dim nNo As Integer = CInt(gridSetRow.Item("Lov_id"))
            If nNo < 1 Then Continue For

            ' SQL���쐬
            strSQL = "SELECT disp_member, val_member_i, val_member_c, val_member_b  FROM " & DB_PAR_LOV_ITEM & " WHERE (lov_id = " & nNo & ")  ORDER BY seq"
            ' �f�[�^�e�[�u���Ɏ擾
            'SQL�����s
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                'gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LOV_ITEM & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            End If
            dttbl = New DataTable

            sqlAdp.Fill(dttbl)

            Dim cmbDt As New DataGridViewComboBoxCell()
            cmbDt.DataSource = dttbl
            cmbDt.DisplayMember = "disp_member"
            Select Case CInt(gridSetRow.Item("DB_�t�B�[���h�^�C�v"))
                Case EM_DB_TYPE.emStrType
                    cmbDt.ValueMember = "val_member_c"
                Case EM_DB_TYPE.emBitType
                    cmbDt.ValueMember = "val_member_b"
                Case Else
                    cmbDt.ValueMember = "val_member_i"
            End Select

            comboboxCells.Add(gridSetRow.Item("DB_�t�B�[���h��").ToString(), cmbDt)
        Next
    End Sub

    ''' <summary>
    ''' �R���{�{�b�N�X�ҏW�i��w�b�_�^�C�v�j
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="comboboxColumns">�ΏۃR���{�{�b�N�X(OUT)</param>
    ''' <remarks></remarks>
    Public Sub MakeComb_ColHeader(ByVal gdtGridSetting As DataSetHolder, ByRef comboboxColumns As Generic.SortedList(Of String, DataGridViewComboBoxColumn))
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim dttbl As System.Data.DataTable = New System.Data.DataTable
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing

        comboboxColumns.Clear()

        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            Select Case CType(gridSetRow.Item("���ڃZ���^�C�v"), EM_DATAGRIDVIEW_CELLTYPE)
                Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                Case Else
                    Continue For
            End Select

            Dim nNo As Integer = CInt(gridSetRow.Item("Lov_id"))
            If nNo < 1 Then Continue For

            ' SQL���쐬
            strSQL = "SELECT disp_member, val_member_i, val_member_c, val_member_b  FROM " & DB_PAR_LOV_ITEM & " WHERE (lov_id = " & nNo & ")  ORDER BY seq"
            ' �f�[�^�e�[�u���Ɏ擾
            'SQL�����s
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                'gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LOV_ITEM & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            End If
            dttbl = New DataTable

            sqlAdp.Fill(dttbl)

            Dim cmbDt As New DataGridViewComboBoxColumn()
            cmbDt.DataSource = dttbl
            cmbDt.DisplayMember = "disp_member"
            Select Case CInt(gridSetRow.Item("DB_�t�B�[���h�^�C�v"))
                Case EM_DB_TYPE.emStrType
                    cmbDt.ValueMember = "val_member_c"
                Case EM_DB_TYPE.emBitType
                    cmbDt.ValueMember = "val_member_b"
                Case Else
                    cmbDt.ValueMember = "val_member_i"
            End Select

            comboboxColumns.Add(gridSetRow.Item("DB_�t�B�[���h��").ToString(), cmbDt)
        Next
    End Sub

    ''' <summary>
    ''' �R���{�{�b�N�X�ҏW�i��w�b�_�^�C�v�j[�}�j���A���p]
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="comboboxColumns">�ΏۃR���{�{�b�N�X(OUT)</param>
    ''' <remarks></remarks>
    Public Sub MakeComb_ColHeader(ByVal gdtGridSetting As DataTable, ByRef comboboxColumns As Generic.SortedList(Of String, DataGridViewComboBoxColumn))
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""
        Dim dtst As System.Data.DataSet = New System.Data.DataSet
        Dim dttbl As System.Data.DataTable = New System.Data.DataTable
        Dim sqlAdp As System.Data.SqlClient.SqlDataAdapter = Nothing

        comboboxColumns.Clear()

        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            Select Case CType(gridSetRow.Item("���ڃZ���^�C�v"), EM_DATAGRIDVIEW_CELLTYPE)
                Case EM_DATAGRIDVIEW_CELLTYPE.emComboBoxCell
                Case Else
                    Continue For
            End Select

            Dim nNo As Integer = CInt(gridSetRow.Item("Lov_id"))
            If nNo < 1 Then Continue For

            ' SQL���쐬
            strSQL = "SELECT disp_member, val_member_i, val_member_c, val_member_b  FROM " & DB_PAR_LOV_ITEM & " WHERE (lov_id = " & nNo & ")  ORDER BY seq"
            ' �f�[�^�e�[�u���Ɏ擾
            'SQL�����s
            If 0 > Get_SelectData(strSQL, dtst, sqlAdp, strErrMsg) Then
                'gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LOV_ITEM & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            End If
            dttbl = New DataTable

            sqlAdp.Fill(dttbl)

            Dim cmbDt As New DataGridViewComboBoxColumn()
            cmbDt.DataSource = dttbl
            cmbDt.DisplayMember = "disp_member"
            Select Case CInt(gridSetRow.Item("DB_�t�B�[���h�^�C�v"))
                Case EM_DB_TYPE.emStrType
                    cmbDt.ValueMember = "val_member_c"
                Case EM_DB_TYPE.emBitType
                    cmbDt.ValueMember = "val_member_b"
                Case Else
                    cmbDt.ValueMember = "val_member_i"
            End Select

            comboboxColumns.Add(gridSetRow.Item("DB_�t�B�[���h��").ToString(), cmbDt)
        Next
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g�i��w�b�_�^�C�v�j
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="strErrMsg">�G���[���b�Z�[�W</param>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Public Function Data_Get_ColHeader(ByVal gdtGridSetting As DataSetHolder, ByRef strErrMsg As String) As Boolean

        Dim drowData As DataRow                 ' �f�[�^�s
        Dim strWhere As String                  ' WHERE��ɑ����̏�������
        Dim lstDt As New List(Of MAIN_KEY_INFO)
        Dim typdt As MAIN_KEY_INFO = Nothing
        Try
            '��L�[�擾
            lstDt.Clear()
            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                If Not CBool(gridSetRow.Item("DB_��L�[")) Then Continue For
                typdt.initialize()
                typdt.nKeyPos = CInt(gridSetRow.Item("����No"))
                typdt.sKeyName = gridSetRow.Item("DB_�t�B�[���h��").ToString
                lstDt.Add(typdt)
            Next

            ' ����No�����[�v
            For ii As Integer = 0 To gdtGridSetting.dgvCnt.dgv.Rows.Count - 1
                strWhere = ""
                For jj As Integer = 0 To lstDt.Count - 1
                    If jj <> 0 Then strWhere &= " and "
                    strWhere &= lstDt(jj).sKeyName & "=" & CInt(gdtGridSetting.dgvCnt.dgv(lstDt(jj).nKeyPos - 1, ii).Value)
                Next
                drowData = gdtGridSetting.dsMain.Tables(0).Select(strWhere)(0)                                             ' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�

                SetDrowData(gdtGridSetting, ii, drowData)
            Next ii

            Return True

        Catch ex As Exception
            strErrMsg = ex.Message
            Return False
        End Try

    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g�i��w�b�_�^�C�v�j
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="strErrMsg">�G���[���b�Z�[�W</param>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Public Function Data_Get_RowHeader(ByVal gdtGridSetting As DataSetHolder, ByRef strErrMsg As String) As Boolean

        Dim drowData As DataRow                 ' �f�[�^�s
        Dim strWhere As String                  ' WHERE��ɑ����̏�������
        Dim lstDt As New List(Of MAIN_KEY_INFO)
        Dim typdt As MAIN_KEY_INFO = Nothing
        Try
            '��L�[�擾
            lstDt.Clear()
            For Each gridSetRow As DataRow In gdtGridSetting.Rows
                If Not CBool(gridSetRow.Item("DB_��L�[")) Then Continue For
                typdt.initialize()
                typdt.nKeyPos = CInt(gridSetRow.Item("����No"))
                typdt.sKeyName = gridSetRow.Item("DB_�t�B�[���h��").ToString
                lstDt.Add(typdt)
            Next

            strWhere = ""
            For jj As Integer = 0 To lstDt.Count - 1
                If jj <> 0 Then strWhere &= " and "
                strWhere &= typdt.sKeyName & "=" & CInt(gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, typdt.nKeyPos - 1).Value)
            Next
            drowData = gdtGridSetting.dsMain.Tables(0).Select(strWhere)(0)        ' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�

            SetDrowData(gdtGridSetting, 0, drowData)

            Return True

        Catch ex As Exception
            strErrMsg = ex.Message
            Return False
        End Try

    End Function

    ''' <summary>
    ''' ��ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="nPos">�f�[�^�O���b�h�r���[���̍s�ʒu(��w�b�_�^�C�v�̂ݎg�p)</param>
    ''' <param name="drowData">��ڑ��^�e�[�u��</param>
    ''' <remarks></remarks>
    Private Sub SetDrowData(ByVal gdtGridSetting As DataSetHolder, ByVal nPos As Integer, ByRef drowData As DataRow)
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            '�ǂݎ���p���H
            If CBool(gridSetRow.Item("�\��_�ǂݎ���p")) Then Continue For
            '��ʒu�擾
            Dim nAddr As Integer = CInt(gridSetRow.Item("����No")) - 1

            '�f�[�^�Z�b�g
            If EM_DATAGRIDVIEW_DISPTYPE.emColType = gdtGridSetting.dgvCnt.dgvType Then
                drowData.Item(CStr(gridSetRow.Item("DB_�t�B�[���h��"))) = gdtGridSetting.dgvCnt.dgv(nAddr, nPos).Value     '
            Else
                drowData.Item(CStr(gridSetRow.Item("DB_�t�B�[���h��"))) = gdtGridSetting.dgvCnt.dgv(EM_ROW_VERTION_ID.emValue, nAddr).Value    '
            End If
        Next
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' SELECT�pSQL���쐬
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <returns>SELECT�pSQL��</returns>
    ''' <remarks></remarks>
    Public Function Create_Sql_Select(ByVal gdtGridSetting As DataSetHolder) As String
        Dim lstDt As New List(Of MAIN_KEY_INFO)
        Dim typdt As MAIN_KEY_INFO = Nothing

        Dim sql0 As String = ""
        sql0 = Create_Sql_Select_Item(gdtGridSetting)
        Dim DBName As String = GetItemDBName(gdtGridSetting)

        Dim strWk As String = ""
        '��L�[�擾
        lstDt.Clear()
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            If Not CBool(gridSetRow.Item("DB_��L�[")) Then Continue For
            typdt.initialize()
            typdt.nKeyPos = CInt(gridSetRow.Item("����No"))
            typdt.sKeyName = gridSetRow.Item("DB_�t�B�[���h��").ToString
            lstDt.Add(typdt)
        Next

        '��L�[�ɂ����ёւ��쐬
        Dim strOrder As String = ""
        For ii As Integer = 0 To lstDt.Count - 1
            If ii <> 0 Then strOrder &= ","
            strOrder &= lstDt(ii).sKeyName
        Next

        If m_DBlst.ContainsKey(DBName) Then
            If "" = strOrder Then
                strWk = String.Format(m_DBlst.Item(DBName).SelectSQLFormat, sql0, DBName)
            Else
                strWk = String.Format(m_DBlst.Item(DBName).SelectSQLFormat, sql0, DBName, strOrder)
            End If
        End If

        Return strWk
    End Function

    ''' <summary>
    ''' SELECT�p�f�[�^���ژA��
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Create_Sql_Select_Item(ByVal gdtGridSetting As DataSetHolder) As String
        Dim strWk As String = ""
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            If strWk.Length <> 0 Then
                strWk &= ", "
            End If
            strWk &= gridSetRow.Item("DB_�t�B�[���h��").ToString
        Next
        Return strWk
    End Function

    ''' <summary>
    ''' UPDATE�pSQL���쐬
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <param name="strFromKey">�R�s�[���L�[ </param>
    ''' <param name="strToKey">�R�s�[��L�[ </param>
    ''' <returns>UPDATE�pSQL��</returns>
    ''' <remarks></remarks>
    Public Function Create_Sql_Update(ByVal gdtGridSetting As DataSetHolder, ByVal strFromKey As String, ByVal strToKey As String) As String
        Dim sql0 As String = ""
        sql0 = Create_Sql_Update_Item(gdtGridSetting)
        Dim DBName As String = GetItemDBName(gdtGridSetting)
        Dim strWk As String = ""
        If m_DBlst.ContainsKey(DBName) Then
            strWk = String.Format(m_DBlst.Item(DBName).UpdateSQLFormat, DBName, sql0, DBName, DBName, strFromKey, strToKey)
        End If

        Return strWk
    End Function

    ''' <summary>
    ''' UPDATE�p�f�[�^���ژA��
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Create_Sql_Update_Item(ByVal gdtGridSetting As DataSetHolder) As String
        Dim strWk As String = ""
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            '�ǂݎ���p���H
            If CBool(gridSetRow.Item("�\��_�ǂݎ���p")) Then Continue For
            '�R�s�[���̗񖼂�As��"b"(�Œ�t�H�[�}�b�g�̗��݂ŁE�E�E)
            strWk &= tClassLibrary.tClass.tClass_SQLServer.sqlColumnUpdate(CStr(gridSetRow.Item("DB_�t�B�[���h��")), "b")
        Next
        tClassLibrary.tClass.tClass_SQLServer.sqlColumnTrim(strWk)                  ' ���܂��Ȃ��A�K���L�q
        Return strWk
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���̑�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �f�t�H���g�F�ɖ߂�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[�R���g���[��</param>
    ''' <remarks></remarks>
    Public Sub SetBackColorEmpty(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '�s�̔w�i���f�t�H���g�F�ɖ߂�
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv.Rows(ii).DefaultCellStyle.BackColor = Color.Empty
        Next
    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Public Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub

    ''' <summary>
    ''' �񕝕ύX
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[�R���g���[��</param>
    ''' <remarks></remarks>
    Public Sub DataGridViewColumnWidthChanged(ByVal dgv As DataGridView)
        ' ���̊֐��ɃC�x���g�n���h�����t
        AddHandler dgv.ColumnWidthChanged, AddressOf dgvColumnWidthChanged
    End Sub

    ''' <summary>
    ''' �񕝒������Adgv���ĕ`��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>�_�~�[�ŕ\�����Ă���X�N���[���o�[�Ɏc�����c���</remarks>
    Private Sub dgvColumnWidthChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewColumnEventArgs)

        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �X�N���[���o�[�̍ĕ`��
        dgv.Refresh()

    End Sub

    ''' <summary>
    ''' �Ώۃe�[�u�����̎擾
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <returns>�e�[�u������</returns>
    ''' <remarks></remarks>
    Public Function GetItemDBName(ByVal gdtGridSetting As DataSetHolder) As String
        Dim strDBName As String = ""
        For Each gridSetRow As DataRow In gdtGridSetting.Rows
            If 1 = CInt(gridSetRow.Item("����No")) Then
                strDBName = gridSetRow.Item("DB_�e�[�u����").ToString()
                Exit For
            End If
        Next
        Return strDBName
    End Function


    ''' <summary>
    ''' ���X�g�R�s�[�ΏۗL��(scode�ΏۗL��)
    ''' </summary>
    ''' <param name="gdtGridSetting">��ʍ\�����</param>
    ''' <returns>���X�g�R�s�[�ΏۗL�� True:�ΏہAFalse:�ΏۊO</returns>
    ''' <remarks></remarks>
    Public Function CopyTargetTable(ByVal gdtGridSetting As DataSetHolder) As Boolean
        Dim bRet As Boolean = False
        Dim DBName As String = GetItemDBName(gdtGridSetting)
        If m_DBlst.ContainsKey(DBName) Then
            bRet = m_DBlst.Item(DBName).bCopyDB
        End If

        Return bRet
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="targetObj"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Shared Function isNullOrEmpty(ByVal targetObj As Object) As Boolean
        'DB�]����NULL�̏ꍇ��True��Ԃ�
        If IsDBNull(targetObj) Then
            Return True
        End If
        '�^�[�Q�b�g��null�̏ꍇTrue��Ԃ�
        If IsNothing(targetObj) Then
            Return True
        End If
        '�^�[�Q�b�g��null�łȂ��ꍇ�́A������ɕϊ������肷��
        Return String.IsNullOrEmpty(CStr(targetObj))

    End Function



    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Protected Overridable Function DB_UpDate() As Boolean

        Try
            Using cbWk As New SqlClient.SqlCommandBuilder()
                For Each Data As DataSetHolder In m_DataSets
                    cbWk.DataAdapter = Data.adapter
                    Data.adapter.Update(Data.dsMain)
                Next
            End Using

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(my_paramID)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        End Try

    End Function



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���܂�
    '/ /////////////////////////////////////////////////////////////////////////////////
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

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DB���
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �g�pDB��񏉊���
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Clear_BaseDBList()
        m_DBlst.Clear()
    End Sub

    ''' <summary>
    ''' �g�pDB���o�^
    ''' </summary>
    ''' <param name="DBName"></param>
    ''' <param name="typ"></param>
    ''' <remarks></remarks>
    Public Sub Add_BaseDBList(ByRef DBName As String, ByVal typ As TABLE_INFO)
        m_DBlst.Add(DBName, typ)
    End Sub
End Class