'*********************************************************************************
'�@��{���Ǘ��N���X
'	[Ver]
'		Ver.01    2008/10/16  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports KizuLibCLI
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tMod

Public Class clsDspDefCoil
    Implements IDisposable                          ' �f�X�g���N�^��`

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R�C�����\���R���g���[��
    ''' </summary>
    Public Structure TYP_COIL_DSP
        Dim dgvCoilInf As tCnt.tCnt_DataGridView
    End Structure

    ' �R�C�����O���b�h���`
    Private Enum EM_GRID_COIL_COL
        emOrderNo = 0           '����No
        emProductNo             '���Y�ԍ�
        emCoilLen               '�R�C����
        emSizeZai               '�ޗ�
        emHin                   '�i��
        emYouto                 '�p�r
        emNextLine              '���H��
        emBase                  '�����

        emColEnd                ' �ŏI�� ���񐔌v�Z�p
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�N���X
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager    ' ���O�Ǘ��N���X

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False        ' �d������Ăяo�������o����ɂ�
    Private m_typCtrl As TYP_COIL_DSP               ' ��{���Ǘ��N���X�R���g���[��
    ' Private m_typMapInf As DSP_MAP_INF
    Private m_typCoil As clsDataMap.TYP_COIL_INF

    '---------------------------------------------------------------------------
    ' �v���p�e�B
    '---------------------------------------------------------------------------
    '''' <summary>
    '''' �R�C�����
    '''' </summary>
    '''' <value></value>
    '''' <returns></returns>
    '''' <remarks></remarks>
    '<System.ComponentModel.Browsable(True) _
    ' , System.ComponentModel.Category("����{���Ǘ��N���X��p�v���p�e�B") _
    ' , System.ComponentModel.Description("�R�C�������Z�b�g")>
    'Public Property tProp_typMapInf() As DSP_MAP_INF
    '    Get
    '        Return m_typMapInf
    '    End Get
    '    Set(value As DSP_MAP_INF)
    '        m_typMapInf = value
    '    End Set
    'End Property

    ''' <summary>
    ''' �R�C�����
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("����{���Ǘ��N���X��p�v���p�e�B") _
     , System.ComponentModel.Description("�R�C�������Z�b�g")>
    Public Property tProp_typCoil() As clsDataMap.TYP_COIL_INF
        Get
            Return m_typCoil
        End Get
        Set(value As clsDataMap.TYP_COIL_INF)
            m_typCoil = value
        End Set
    End Property

    '///////////////////////////////////////////////////////////////////////////
    '�����o�֐�
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="typCoilDsp">��{���</param>
    ''' <param name="clsLog">���O�N���X</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef typCoilDsp As TYP_COIL_DSP, ByRef clsLog As tClass.tClass_LogManager)

        m_typCtrl = typCoilDsp

        '���O�N���X
        mcls_Log = clsLog

        InitdgvDefCoilInf(m_typCtrl.dgvCoilInf)
    End Sub

    ' IDisposable
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
    ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' ��{��񏉊�������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()
        With m_typCtrl.dgvCoilInf
            For ii As Integer = 0 To .ColumnCount - 1
                Select Case ii
                    Case EM_GRID_COIL_COL.emOrderNo             ' ����No
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emProductNo           ' ���Y�ԍ�
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emCoilLen             ' �R�C����
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emNextLine            ' ���H��
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emSizeZai             ' �ޗ�
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emBase                ' �����
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emHin                 ' �i��
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emYouto               ' �p�r
                        .Item(ii, 0).Value = ""
                End Select
            Next
        End With
    End Sub

    ''' <summary>
    ''' �R�C�����\������
    ''' </summary>
    ''' <param name="typCoilInf">��{���\����</param>
    ''' <remarks></remarks>
    Public Sub DspData(typCoilInf As COINFO_BASE)

        Try
            '����{���\��
            With m_typCtrl.dgvCoilInf
                For ii As Integer = 0 To .ColumnCount - 1
                    Select Case ii
                        Case EM_GRID_COIL_COL.emOrderNo             ' ����No
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.base.cMeiNo)
                        Case EM_GRID_COIL_COL.emProductNo           ' ���Y�ԍ�
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.base.cCoilNoIn)
                        Case EM_GRID_COIL_COL.emCoilLen            ' �R�C����
                            '.Item(ii, 0).Value = Format(Math.Floor((m_typMapInf.nCut_end - m_typMapInf.nCut_start) / 100) / 10, "0.0")
                        Case EM_GRID_COIL_COL.emNextLine            ' ���H��
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cNextLine)
                        Case EM_GRID_COIL_COL.emSizeZai             ' �ޗ�
                            ''�T�C�Y(��[��m��mm]�~��[mm])
                            Dim sThick As String = Format(typCoilInf.data.CoilInf.nAtuZai / 1000, "0.000")
                            Dim sWidth As String = typCoilInf.data.CoilInf.nWidZai.ToString
                            'If 0 <> typCoilInf.data.CoilInf.nWidTrim Then
                            '    '�g�����Ђ�0�łȂ���΃g�����Ђ�\��
                            '    sWidth = typCoilInf.data.CoilInf.nWidTrim.ToString
                            'End If
                            .Item(ii, 0).Value = String.Format("{0}�~{1,4}", sThick, sWidth)
                        Case EM_GRID_COIL_COL.emBase                ' �����
                            Dim strKensaO As String = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cKenKijun_O)
                            Dim strKensaU As String = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cKenKijun_U)
                            .Item(ii, 0).Value = strKensaO & "/" & strKensaU
                        Case EM_GRID_COIL_COL.emHin                 ' �i��
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cHinCode)
                        Case EM_GRID_COIL_COL.emYouto               ' �p�r
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cYouto)
                    End Select
                Next
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C�����\���ُ� [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �R�C�����\������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspData()

        Try
            If m_typCoil.strKanriNo = "" Then
                DspInit()
            End If
            '����{���\��
            With m_typCtrl.dgvCoilInf
                For ii As Integer = 0 To .ColumnCount - 1
                    Select Case ii
                        Case EM_GRID_COIL_COL.emOrderNo             ' ����No
                            .Item(ii, 0).Value = m_typCoil.strOrderNo
                        Case EM_GRID_COIL_COL.emProductNo           ' ���Y�ԍ�
                            .Item(ii, 0).Value = m_typCoil.strCoilNo
                        Case EM_GRID_COIL_COL.emCoilLen            ' �R�C����
                            .Item(ii, 0).Value = Format(Math.Floor(m_typCoil.nCoilLen / 100) / 10, "0.0")
                        Case EM_GRID_COIL_COL.emNextLine            ' ���H��
                            .Item(ii, 0).Value = m_typCoil.strNextCode
                        Case EM_GRID_COIL_COL.emSizeZai             ' �ޗ�
                            ''�T�C�Y(��[��m��mm]�~��[mm])
                            Dim sThick As String = Format(m_typCoil.nCoilAtu / 1000, "0.000")
                            Dim sWidth As String = m_typCoil.nCoilWidth.ToString
                            'If 0 <> m_typCoil.nTrimWidth Then
                            '    '�g�����Ђ�0�łȂ���΃g�����Ђ�\��
                            '    sWidth = m_typCoil.nTrimWidth.ToString
                            'End If
                            .Item(ii, 0).Value = String.Format("{0}�~{1,4}", sThick, sWidth)
                        Case EM_GRID_COIL_COL.emBase                ' �����
                            .Item(ii, 0).Value = m_typCoil.strKenKijun(0)    '�����(�\)
                        Case EM_GRID_COIL_COL.emHin                 ' �i��
                            .Item(ii, 0).Value = m_typCoil.strHinCode
                        Case EM_GRID_COIL_COL.emYouto               ' �p�r
                            .Item(ii, 0).Value = m_typCoil.strUse
                    End Select
                Next
            End With
            ' ���A�t�H���g�T�C�Y����
            Call ResizeGridCoil()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C�����\���ُ� [{0}]", ex.Message))
        End Try

    End Sub

    ''///////////////////////////////////////////////////////////////////////////
    ''�f�[�^�O���b�h�r���[
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R�C�����DataGrid�ݒ�(��Z�b�g)
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub InitdgvDefCoilInf(ByVal dgv As tCnt.tCnt_DataGridView)

        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .RowCount = 0
            .ScrollBars = ScrollBars.None
            .DoubleBuffered = True
            .EditMode = DataGridViewEditMode.EditProgrammatically
            .ColumnHeadersHeight = 28
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 16.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .RowTemplate.Height = .Height - .ColumnHeadersHeight - SystemInformation.Border3DSize.Height
            .RowHeadersVisible = False
            .ColumnHeadersVisible = True
            .DefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 26.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

            '' ���`
            .ColumnCount = 8
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter

            ' ����No
            With .Columns(EM_GRID_COIL_COL.emOrderNo)
                .HeaderText = "����No"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' ���Y�ԍ�
            With .Columns(EM_GRID_COIL_COL.emProductNo)
                .HeaderText = "���Y�ԍ�"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' �R�C����
            With .Columns(EM_GRID_COIL_COL.emCoilLen)
                .HeaderText = "�R�C����"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' �ޗ�
            With .Columns(EM_GRID_COIL_COL.emSizeZai)
                .HeaderText = "�ޗ�"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' �i��
            With .Columns(EM_GRID_COIL_COL.emHin)
                .HeaderText = "�i��"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' �p�r
            With .Columns(EM_GRID_COIL_COL.emYouto)
                .HeaderText = "�p�r"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' ���H��
            With .Columns(EM_GRID_COIL_COL.emNextLine)
                .HeaderText = "���H��"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' �����
            With .Columns(EM_GRID_COIL_COL.emBase)
                .HeaderText = "�����"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            .RowCount = 1

            ' ���A�t�H���g�T�C�Y����
            Call ResizeGridCoil()
        End With
    End Sub

    ''' <summary>
    ''' �R�C�����DataGrid���T�C�Y����
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ResizeGridCoil()
        ' �R���g���[���T�C�Y�ɉ������Z���̑傫���̒���
        Dim nAreaSize As Integer = m_typCtrl.dgvCoilInf.Width - SystemInformation.Border3DSize.Width * 2
        Dim nSum As Integer = 0
        Dim nFontSize As Integer
        Dim strVal As String
        Dim dSizeRate() As Double = {0.084, 0.207, 0.108, 0.209, 0.098, 0.098, 0.098, 0.098}
        'Dim dSizeRate() As Double = {0.084, 0.207, 0.108, 0.209, 0.093, 0.088, 0.078, 0.133}
        Dim nCellSize(EM_GRID_COIL_COL.emColEnd - 1) As Integer

        ' �Z�����̌v�Z
        For ii As Integer = 0 To EM_GRID_COIL_COL.emColEnd - 2
            nCellSize(ii) = CInt(Math.Floor(nAreaSize * dSizeRate(ii)))
            nSum += nCellSize(ii)
        Next
        nCellSize(EM_GRID_COIL_COL.emColEnd - 1) = nAreaSize - nSum

        ' ���ɍ��킹���t�H���g�T�C�Y�̒���
        With m_typCtrl.dgvCoilInf
            For nColIdx As Integer = 0 To EM_GRID_COIL_COL.emColEnd - 1
                ' ���A�w�b�_�[�t�H���g
                With .Columns(nColIdx)
                    .Width = nCellSize(nColIdx)

                    strVal = .HeaderText
                    nFontSize = CInt(GetGridFontScaleSize(m_typCtrl.dgvCoilInf, strVal, m_typCtrl.dgvCoilInf.ColumnHeadersDefaultCellStyle.Font.Size, .HeaderCell.Size.Width, .HeaderCell.Size.Height))
                    .HeaderCell.Style.Font = New System.Drawing.Font("�l�r ����", nFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End With

                ' �l�t�H���g
                With .Item(nColIdx, 0)
                    ' �l
                    If .Value IsNot Nothing Then
                        strVal = .Value.ToString
                    Else
                        strVal = ""
                    End If
                    nFontSize = CInt(GetGridFontScaleSize(m_typCtrl.dgvCoilInf, strVal, m_typCtrl.dgvCoilInf.DefaultCellStyle.Font.Size, .Size.Width, .Size.Height))
                    .Style.Font = New System.Drawing.Font("�l�r ����", nFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End With
            Next
        End With
    End Sub

End Class
