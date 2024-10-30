'*********************************************************************************
'DataGridView�̓Ǝ��N���X
'	[Ver]
'		Ver.01    2006/03/04  vs2005 �Ή�
'
'	[����]
'		�r���ł́A�T����ʂ��r�}�b�v�����Ɏg�p����B
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Windows.Forms						' �R���g���[��
Imports System.Drawing

Namespace tCnt

    '' ���̃f�U�C�i�A�g���r���[�g��ǉ������̂́AMS�̃o�O�̑΍�B
    ' �� ���̂ق��ɂ��ATableLayoutPanel�AMenuStrip�AToolStrip ���X���L��
    ' �EDataGridView���A�p�������t�H�[���ł���DataGridView�I�u�W�F�N�g�̃v���p�e�B��ύX�ł��Ȃ�
    ' �E�f�U�C�i����̃T�C�Y�ύX�����o���Ȃ�
    <System.ComponentModel.Designer("System.Windows.Forms.Design.ControlDesigner, System.Design")> _
    Public Class tCnt_DataGridView
        Inherits DataGridView

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �O���v���p�e�B
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �_�u���o�b�t�@�����O���O�Ɍ��J
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.DefaultValue(True) _
         , System.ComponentModel.Category("����p�v���p�e�B") _
         , System.ComponentModel.Description("�_�u���o�b�t�@�����O���O�Ɍ��J")> _
        Public Shadows Property DoubleBuffered() As Boolean
            Get
                Return MyBase.DoubleBuffered
            End Get
            Set(ByVal value As Boolean)
                MyBase.DoubleBuffered = value
            End Set
        End Property


        ''' <summary>
        ''' �I�[�i�[�R���g���[���ύX
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub tCnt_DataGridView_ParentChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.ParentChanged
            ' ��ʂɃR���g���[���\��t�������ɁA�N�������͂�
            Me.DoubleBuffered = True
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ DataGridView �֘A
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �\���ݒ�
        ''' </summary>
        ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
        ''' <remarks></remarks>
        Public Overloads Sub dgvInitialize(ByVal dgv As DataGridView)

            With dgv
                .ColumnCount = 0
                .AutoGenerateColumns = False									' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
                .AllowUserToAddRows = False										' ���[�U�[ �s�ǉ�
                .AllowUserToDeleteRows = False									' ���[�U�[ �s�폜
                .AllowUserToOrderColumns = False								' ���[�U�[ ��ړ�
                .AllowUserToResizeColumns = False								' ���[�U�[ ��T�C�Y�ύX
                .AllowUserToResizeRows = False									' ���[�U�[ �s�T�C�Y�ύX
                .ShowCellErrors = False											' �Z���G���[�̕\��
                .ShowCellToolTips = False										' �c�[���q���g�̕\��
                .ShowEditingIcon = False										' �ҏW���̃Z���̖���
                .ShowRowErrors = False											' �G���[�s�̖���
                .ScrollBars = ScrollBars.Horizontal								' �X�N���[���o�[�̕\��
                '.ColumnHeadersHeight = 42										' �w�b�_�[�̍���

                .SelectionMode = DataGridViewSelectionMode.CellSelect			' �Z���I��
                .MultiSelect = False											' �����s�I��
                .ReadOnly = False												' �ǂݎ���p�̓_��
                .EditMode = DataGridViewEditMode.EditOnEnter					' �Z����������

                .ColumnHeadersVisible = True									' ��w�b�_�\��
                .RowHeadersVisible = False										' �s�w�b�_�\��
                .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None		' ����T�C�Y��������
                .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None			' ���s�T�C�Y��������
                .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing	'
                .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing			'

                ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

                '' �I���Z���F��`
                ''.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
                '.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
                ''.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
                '.DefaultCellStyle().SelectionForeColor = Color.Black
                '.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1�s�����ɐF�ւ�
                '.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
                '.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                '.DefaultCellStyle().ForeColor = Color.Black
            End With
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �f�[�^�O���b�h�r���[ �I���s �w�i�F�������Ή�
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �I���L�����Z��
        ''' </summary>
        ''' <param name="dgv">�f�[�^�O���b�h�r���[�R���g���[��</param>
        ''' <remarks></remarks>
        Public Overridable Sub DataGridViewSelectionCancel(ByVal dgv As DataGridView)
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


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �f�[�^�O���b�h�r���[ �I���s �ύX�֌W
        '/ /////////////////////////////////////////////////////////////////////////////////
        Private m_SelectionColor As Color				' �I��F

        ''' <summary>
        ''' �I����s�F�ς�
        ''' </summary>
        ''' <param name="dgv">�f�[�^�O���b�h�r���[�R���g���[��</param>
        ''' <remarks></remarks>
        Public Overridable Sub DataGridViewSelectionRowColorChange(ByVal dgv As DataGridView, ByVal selectionColor As Color)
            m_SelectionColor = selectionColor
            ' ���̊֐��ɃC�x���g�n���h�����t
            AddHandler dgv.RowEnter, AddressOf dgvData_RowEnter
            AddHandler dgv.RowLeave, AddressOf dgvData_RowLeave
        End Sub
        Public Overridable Sub DataGridViewSelectionRowColorChange(ByVal selectionColor As Color)
            m_SelectionColor = selectionColor
            ' ���̊֐��ɃC�x���g�n���h�����t
            AddHandler Me.RowEnter, AddressOf dgvData_RowEnter
            AddHandler Me.RowLeave, AddressOf dgvData_RowLeave
        End Sub
        ''' <summary>
        ''' �f�[�^�O���b�h�r���[�s�I��
        ''' </summary>
        Private Sub dgvData_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
            Dim dgv As DataGridView = CType(sender, DataGridView)	' �Ώۂ�dgv

            dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = m_SelectionColor		'�s�̔w�i��ύX
        End Sub
        ''' <summary>
        ''' �f�[�^�O���b�h�r���[�s��I��
        ''' </summary>
        Private Sub dgvData_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
            Dim dgv As DataGridView = CType(sender, DataGridView)	' �Ώۂ�dgv

            dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty			'�s�̔w�i���f�t�H���g�F�ɖ߂�
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �ݒ荀�ڑ����m�� �����`
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �O���b�h���� �n���h�����蓖��
        ''' </summary>
        ''' <param name="dgv"></param>
        ''' <remarks></remarks>
        Public Overridable Sub DataGridViewDataExec(ByVal dgv As DataGridView)
            'AddHandler dgv.CellValueChanged, AddressOf dgv_CellValueChanged
            AddHandler dgv.CurrentCellDirtyStateChanged, AddressOf dgvData_CurrentCellDirtyStateChanged
        End Sub

        ''' <summary>
        ''' �Z���̏�Ԃ��ω��������̏���
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub dgvData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs)
            Dim dgv As DataGridView = CType(sender, DataGridView)

            ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ�R�~�b�g����
            If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
        End Sub


        '''' <summary>
        '''' �Z���`�F���W����
        '''' </summary>
        '''' <param name="sender"></param>
        '''' <param name="e"></param>
        '''' <remarks></remarks>
        'Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        '	Dim dgv As DataGridView = CType(sender, DataGridView)
        '	'�w�b�_�͏������Ȃ�
        '	If e.RowIndex = -1 Then Exit Sub

        '	If e.ColumnIndex = EM_FILTER_COL.emCheck Then
        '		If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
        '			'' �`�F�b�N��t������
        '			dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen
        '			dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.LightGreen
        '		Else
        '			'' �`�F�b�N���O������
        '			dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
        '			dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
        '		End If
        '	End If

        'End Sub


    End Class
End Namespace