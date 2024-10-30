'*******************************************************************************
'   �R�C�����\��
'
'   [Ver]
'       Ver.01  2017/08/24  ����
'
'   [����]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI.KizuMod

Imports KizuForm
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

''' <summary>
''' �R�C�����\��
''' </summary>
''' <remarks></remarks>
Public Class clsDispCoil
    Implements IDisposable

    '---------------------------------------------------------------------------
    ' �񋓑�
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �R�C�����1
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_COL_COLINF_1
        ORDER_NO = 0        ' ����No
        COIL_NO             ' ���Y�ԍ�
        TYPE_DIV            ' �i��
        MATERIAL            ' �ގ�
        NEXT_STEP           ' ���H��
        KENSA_REF           ' �����
        SIZE                ' �ޗ�����
        ORDER               ' ����
        MAX_COL             ' �ő��
    End Enum

    ''' <summary>
    ''' �R�C�����2
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_COL_COLINF_2
        MEN = 0                               '�\����
        P_CODE
        S_CODE
        COIL_LEN            ' �R�C����
        MAX_COL             ' �ő��
    End Enum

    ''' <summary>
    ''' �R�C�����\���R���g���[��
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_COIL_DSP
        Dim dgvInf1 As tCnt.tCnt_DataGridView
        Dim dgvInf2 As tCnt.tCnt_DataGridView
        Dim lblKiki As Label
        Dim lblKensa As Label
        Dim tblLayoutPnl As TableLayoutPanel
    End Structure

    '///////////////////////////////////////////////////////////////////////////
    '�����o�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                '���\�[�X�J���t���O[True:�J����,False:���J��]
    Private mcls_Log As tClass_LogManager                   '���O�Ǘ�
    Private m_typCoilDsp As TYP_COIL_DSP                    '�R�C�����\���R���g���[��
    Private mcls_Param As clsParamManager                   '�p�����[�^�N���X

    '---------------------------------------------------------------------------
    ' �v���p�e�B
    '---------------------------------------------------------------------------

    '///////////////////////////////////////////////////////////////////////////
    '�����o�֐�
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="typCoilDsp"></param>
    ''' <param name="clsLog"></param>
    ''' <remarks></remarks>
    Public Sub New(ByRef typCoilDsp As TYP_COIL_DSP, ByRef clsLog As tClass_LogManager, ByRef clsParam As clsParamManager)
        Try
            m_typCoilDsp = typCoilDsp
            mcls_Log = clsLog
            mcls_Param = clsParam

            dgvInit_Inf1()
            dgvInit_Inf2()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C�����\�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �f�X�g���N�^
    ''' </summary>
    ''' <param name="disposing">True:�����I�ȌĂяo�� False:�Öق̌Ăяo��</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            If Not m_bIsDesposed Then
                '�I�u�W�F�N�g�j��
            End If
            m_bIsDesposed = True                                                'Dispose�ς݂Ƃ���
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�u�W�F�N�g�j���ُ� [{0}]", ex.Message))
        End Try
    End Sub
#Region " IDisposable Support "
    '���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        '���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' ��ʏ����\��
    ''' </summary>
    Public Sub Load()

        Dim dgv As tCnt.tCnt_DataGridView = m_typCoilDsp.dgvInf1
        dgvWidthResize(dgv)
        AddHandler m_typCoilDsp.tblLayoutPnl.Resize, AddressOf Ctrl_ReSize

    End Sub

    ''' <summary>
    ''' �\��������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()
        Try
            '�R�C�����\���R���g���[��
            With m_typCoilDsp

                For ii As Integer = 0 To .dgvInf1.ColumnCount - 1
                    .dgvInf1.Item(ii, 0).Value = ""
                    .dgvInf1.Item(ii, 0).Style.ForeColor = .dgvInf1.DefaultCellStyle().ForeColor
                    .dgvInf1.Item(ii, 0).Style.BackColor = .dgvInf1.DefaultCellStyle().BackColor
                Next

                For ii As Integer = 0 To .dgvInf2.ColumnCount - 1
                    .dgvInf2.Item(ii, 0).Value = ""
                    .dgvInf2.Item(ii, 0).Style.ForeColor = .dgvInf2.DefaultCellStyle().ForeColor
                    .dgvInf2.Item(ii, 0).Style.BackColor = .dgvInf2.DefaultCellStyle().BackColor
                Next

                .lblKiki.Text = ""
                .lblKiki.BackColor = g_ColorControl
                .lblKensa.Text = ""
                .lblKensa.BackColor = g_ColorControl

            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�\���������ُ� [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �R�C�����\��
    ''' </summary>
    ''' <param name="nMen">��[0:�\,1:��]</param>
    ''' <remarks></remarks>
    Public Sub DspCoil(ByVal typCoilInf As clsDataMap.TYP_COIL_INF, ByVal nMen As Integer)

        Try
            With m_typCoilDsp

                For ii As Integer = 0 To .dgvInf1.ColumnCount - 1
                    Select Case ii
                        Case EM_COL_COLINF_1.ORDER_NO           ' ����No
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strOrderNo
                            If KizuForm.EM_DIV_KENSA.DIV_KENSA_OK = typCoilInf.nKensa(nMen) Then
                                .dgvInf1.Item(ii, 0).Style.BackColor = .dgvInf1.DefaultCellStyle().BackColor
                                .dgvInf1.Item(ii, 0).Style.SelectionBackColor = .dgvInf1.Item(ii, 0).Style.BackColor
                            Else
                                '������Ԃ�����ȊO�́A�w�i�F��Ԃɂ���
                                .dgvInf1.Item(ii, 0).Style.BackColor = Color.Red
                                .dgvInf1.Item(ii, 0).Style.SelectionBackColor = .dgvInf1.Item(ii, 0).Style.BackColor
                            End If

                        Case EM_COL_COLINF_1.COIL_NO            ' ���Y�ԍ�
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strCoilNo
                        Case EM_COL_COLINF_1.TYPE_DIV           ' �i��
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strHinCode
                        Case EM_COL_COLINF_1.MATERIAL           ' �ގ�
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strSyuZai
                        Case EM_COL_COLINF_1.NEXT_STEP          ' ���H��
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strNextCode
                        Case EM_COL_COLINF_1.KENSA_REF          ' �����
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strKenKijun(0) '�����_�\
                        Case EM_COL_COLINF_1.SIZE
                            ''�T�C�Y(��[��m��mm]�~��[mm])
                            Dim sThick As String = Format(typCoilInf.nCoilAtu / 1000, "0.000")
                            Dim sWidth As String = typCoilInf.nCoilWidth.ToString
                            .dgvInf1.Item(ii, 0).Value = String.Format("{0}�~{1,4}", sThick, sWidth)
                        Case EM_COL_COLINF_1.ORDER
                            ''�T�C�Y(��[��m��mm]�~��[mm])
                            Dim sThickSei As String = Format(typCoilInf.nCoilAtuSei / 1000, "0.000")
                            Dim sWidthSei As String = typCoilInf.nCoilWidSei.ToString
                            .dgvInf1.Item(ii, 0).Value = String.Format("{0}�~{1,4}", sThickSei, sWidthSei)
                        Case Else
                            .dgvInf1.Item(ii, 0).Value = ""
                            .dgvInf1.Item(ii, 0).Style.BackColor = .dgvInf1.DefaultCellStyle().BackColor
                    End Select
                Next

                For ii As Integer = 0 To .dgvInf2.ColumnCount - 1
                    Select Case ii
                        Case EM_COL_COLINF_2.MEN             ' �p�^�[��
                            '.dgvInf2.Item(ii, 0).Value = IIf(1 = nMen, "����", "�\��")
                            If 1 = nMen Then
                                .dgvInf2.Item(ii, 0).Value = "����"
                                .dgvInf2.Item(ii, 0).Style.ForeColor = Color.Black
                                .dgvInf2.Item(ii, 0).Style.BackColor = Color.Orange
                                .dgvInf2.Item(ii, 0).Style.SelectionForeColor = .dgvInf2.Item(ii, 0).Style.ForeColor
                                .dgvInf2.Item(ii, 0).Style.SelectionBackColor = .dgvInf2.Item(ii, 0).Style.BackColor
                            Else
                                .dgvInf2.Item(ii, 0).Value = "�\��"
                                .dgvInf2.Item(ii, 0).Style.ForeColor = Color.Yellow
                                .dgvInf2.Item(ii, 0).Style.BackColor = Color.Blue
                                .dgvInf2.Item(ii, 0).Style.SelectionForeColor = .dgvInf2.Item(ii, 0).Style.ForeColor
                                .dgvInf2.Item(ii, 0).Style.SelectionBackColor = .dgvInf2.Item(ii, 0).Style.BackColor
                            End If
                        Case EM_COL_COLINF_2.P_CODE             ' �p�^�[��
                            .dgvInf2.Item(ii, 0).Value = typCoilInf.nPcode(nMen).ToString
                        Case EM_COL_COLINF_2.S_CODE             ' �\�ʏ��
                            .dgvInf2.Item(ii, 0).Value = typCoilInf.nScode(nMen).ToString
                        Case EM_COL_COLINF_2.COIL_LEN           ' �R�C����
                            .dgvInf2.Item(ii, 0).Value = Format(typCoilInf.nCoilLen / 1000, "0")
                        Case Else
                            .dgvInf2.Item(ii, 0).Value = ""
                            .dgvInf2.Item(ii, 0).Style.BackColor = .dgvInf1.DefaultCellStyle().BackColor
                    End Select
                Next

                ' �@����
                KizuForm.ucStatusAria.DspKiki(.lblKiki, CType(typCoilInf.nKiki(nMen), KizuForm.EM_DIV_KIKI))
                If .lblKiki.Text = "�y�̏�" Or .lblKiki.Text = "�d�̏�" Then .lblKiki.ForeColor = Color.Yellow

                ' �������
                KizuForm.ucStatusAria.DspKensa(.lblKensa, CType(typCoilInf.nKensa(nMen), KizuForm.EM_DIV_KENSA))
                If .lblKensa.Text <> "��~" And .lblKensa.Text <> "�s��" Then .lblKensa.ForeColor = Color.Yellow

            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C�����\���ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''///////////////////////////////////////////////////////////////////////////
    ''�f�[�^�O���b�h�r���[
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �O���b�h1������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub dgvInit_Inf1()

        Dim dgv As tCnt.tCnt_DataGridView = m_typCoilDsp.dgvInf1

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        Try
            With dgv
                .ScrollBars = ScrollBars.Horizontal                             ' �X�N���[���o�[(�����̂�)
                .DoubleBuffered = True

                .ReadOnly = True                                                ' �ǂݎ���p
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������
                .ColumnHeadersVisible = True                                   ' ��w�b�_�\������
                .AllowUserToResizeColumns = True                               ' ���[�U�[ ��T�C�Y�ύX

                '' �I���Z���F��`
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .ColumnHeadersHeight = 30                                       ' ��w�b�_��������

                For col As EM_COL_COLINF_1 = EM_COL_COLINF_1.ORDER_NO To EM_COL_COLINF_1.MAX_COL
                    Select Case col
                        Case EM_COL_COLINF_1.ORDER_NO
                            ' ����No
                            dgvColSetText(dgv, "����No", 78, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.COIL_NO
                            ' ���Y�ԍ�
                            dgvColSetText(dgv, "���Y�ԍ�", 198, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.TYPE_DIV
                            ' �i��
                            dgvColSetText(dgv, "�i��", 46, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.MATERIAL
                            ' �ގ�
                            dgvColSetText(dgv, "�ގ�", 46, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.NEXT_STEP
                            ' ���H��
                            dgvColSetText(dgv, "��" & vbCrLf & "�H��", 46, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.KENSA_REF
                            ' �����
                            dgvColSetText(dgv, "�����", 78, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.SIZE
                            ' ����
                            dgvColSetText(dgv, "����", 182, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.ORDER
                            ' ����
                            dgvColSetText(dgv, "����", 182, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    End Select
                Next

                .RowCount = 1
                .Rows(0).Height = .ClientSize.Height - .ColumnHeadersHeight


            End With
        Catch ex As Exception
        End Try

    End Sub

    ''' <summary>
    ''' �O���b�h2������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub dgvInit_Inf2()

        Dim dgv As tCnt.tCnt_DataGridView = m_typCoilDsp.dgvInf2

        Call dgv.dgvInitialize(dgv)

        Try
            With dgv
                .ScrollBars = ScrollBars.None                                   ' �X�N���[���o�[�Ȃ�
                .DoubleBuffered = True

                .ReadOnly = True                                                ' �ǂݎ���p
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������
                .ColumnHeadersVisible = True                                   ' ��w�b�_�\������

                '' �I���Z���F��`
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .ColumnHeadersHeight = 28                                       ' ��w�b�_��������

                For col As EM_COL_COLINF_2 = EM_COL_COLINF_2.MEN To EM_COL_COLINF_2.MAX_COL
                    Select Case col
                        Case EM_COL_COLINF_2.MEN
                            dgvColSetText(dgv, "��" & vbCrLf & "", 74, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_COLINF_2.P_CODE
                            dgvColSetText(dgv, "�p�^" & vbCrLf & "�[��", 46, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_COLINF_2.S_CODE
                            dgvColSetText(dgv, "�\��" & vbCrLf & "���", 46, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_COLINF_2.COIL_LEN
                            ' �R�C����
                            dgvColSetText(dgv, "�e�`", 116, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.MiddleCenter)
                    End Select
                Next

                .RowCount = 1
                .Rows(0).Height = .ClientSize.Height - .ColumnHeadersHeight


            End With
        Catch ex As Exception
        End Try

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
    Private Sub dgvColSetText(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer,
     ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode,
     ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment,
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
            ' ���̍s�ɐ܂�Ԃ��Ȃ�
            clm.DefaultCellStyle.WrapMode = DataGridViewTriState.False

            ' ��쐬
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' ���T�C�Y�ɂ��\���ύX
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvWidthResize(ByVal dgv As tCnt.tCnt_DataGridView)
        Dim hsc As HScrollBar

        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            dgv.ColumnHeadersHeight = 16
            '�����X�N���[���o�[�\��
            dgv.Rows(0).Height = dgv.ClientSize.Height - dgv.ColumnHeadersHeight - hsc.Height
        Else
            dgv.ColumnHeadersHeight = 28
            dgv.Rows(0).Height = dgv.ClientSize.Height - dgv.ColumnHeadersHeight
        End If

        For ii As Integer = 0 To dgv.ColumnCount - 1
            If hsc.Visible Then
                '�����X�N���[���o�[�\��
                dgv.Item(ii, 0).Style.Font = New System.Drawing.Font("�l�r ����", 16.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Else
                dgv.Item(ii, 0).Style.Font = New System.Drawing.Font("�l�r ����", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            End If
        Next

    End Sub


    ''///////////////////////////////////////////////////////////////////////////
    ''�C�x���g�n���h���֐�
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���T�C�Y
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub Ctrl_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)
        dgvWidthResize(m_typCoilDsp.dgvInf1)
    End Sub

End Class
