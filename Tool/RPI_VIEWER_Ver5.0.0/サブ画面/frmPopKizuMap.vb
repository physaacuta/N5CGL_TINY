Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Public Class frmPopKizuMap

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�C�x���g
    Public Event tEv_MapClick(ByVal pos As Point)						' �N���b�N (mm���W�n) �}�b�v���オ0mm�Ƃ��Ă̋���
    Public Event tEv_KizuClick(ByVal pos As Point)						' �N���b�N (mm���W�n) �}�b�v���オ0mm�Ƃ��Ă̋���

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�

    ' �N���X
    Private mcls_pParam As clsParamManager = Nothing					' �p�����[�^�Ǘ��N���X (�|�C���^)
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPI�t�@�C���\���N���X (�|�C���^)

    ' �f�[�^
    Private m_typCoil As DATA_COIL										' �R�C�����
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing           ' �r��񃊃X�g
    Private m_lstCut As List(Of DATA_CUT_INF) = Nothing                 ' �J�b�g��񃊃X�g

    Private m_nHabaCol As Integer                                       ' ��������
    Private m_nInterval As Integer                                      ' ����T�C�Y
    Private m_nHabaInterval As Integer                                  ' ���T�C�Y
    Private m_nCoilLenRow As Integer                                    ' DataGridView�s��(�R�C�������Z�o)

    Private m_nSizeX As Integer                                         ' ��ʃT�C�YX�iRESIZE���̔�r�p�j
    Private m_nSizeY As Integer                                         ' ��ʃT�C�YY�iRESIZE���̔�r�p�j

    Private m_nNowRow As Integer                                        ' �S���摜�\���ʒu�F����l�i�}�b�v�s�ʒu���Z�j
    Private m_nOldRow As Integer                                        ' �S���摜�\���ʒu�F�O��l�i�}�b�v�s�ʒu���Z�j

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���p�e�B
    ''' <summary>
    ''' �p�����[�^�N���X�Z�b�g
    ''' </summary>
    ''' <param name="cls">�v�Z�N���X</param>
    Public Sub SetCls_Param(ByRef cls As clsParamManager)
        mcls_pParam = cls
    End Sub

    ''' <summary>
    ''' RPI�N���X
    ''' </summary>
    ''' <param name="cls">RPI�N���X</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub

    ''' <summary>
    ''' �R�C�����
    ''' </summary>
    ''' <param name="typ">�r��񃊃X�g</param>
    ''' <remarks></remarks>
    Friend Sub SetData_Coil(ByVal typ As DATA_COIL)
        m_typCoil = typ
    End Sub

    ''' <summary>
    ''' �r��񃊃X�g
    ''' </summary>
    ''' <param name="lst">�r��񃊃X�g</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub

    ''' <summary>
    ''' �J�b�g��񃊃X�g
    ''' </summary>
    ''' <param name="lst">�J�b�g��񃊃X�g</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstCut(ByRef lst As List(Of DATA_CUT_INF))
        m_lstCut = lst
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{
    ''' <summary>
    ''' ���[�h
    ''' </summary>
    Private Sub frmPopKizuMap_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�����l�Z�b�g(�O��l�Ǎ��j
        Load_last_Control_Ini()
    End Sub

    ''' <summary>
    ''' �t�H�[���T�C�Y�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPopKizuMap_ResizeEnd(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.ResizeEnd
        ' �c�݂̂̃T�C�Y�ύX�̏ꍇ�͉�ʍX�V����
        If Me.Size.Width = m_nSizeX Then
            ' �ύX��̃T�C�Y��ۑ�
            m_nSizeX = Me.Size.Width
            m_nSizeX = Me.Size.Height
            Exit Sub
        End If
        ' �ύX��̃T�C�Y��ۑ�
        m_nSizeX = Me.Size.Width
        m_nSizeX = Me.Size.Height

        '�P�񕝃T�C�Y�Z�o
        Dim nSize As Integer = CInt((dgvMap.Width - 50 - 18) / m_nHabaCol)

        '�񕝂�ύX
        For ii As Integer = 1 To m_nHabaCol
            dgvMap.Columns(ii).Width = nSize
        Next ii
    End Sub

    ''' <summary>
    ''' �}�b�v�I��(�rJUMP)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMap_CellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs) Handles dgvMap.CellMouseDown
        Dim dgv As tCnt.tCnt_DataGridView = CType(sender, tCnt.tCnt_DataGridView)   ' �Ώۂ�dgv
        Dim dgvCell As DataGridViewCell

        Dim nX As Integer = 0
        Dim nY As Integer = 0
        Try
            If e.RowIndex < 0 Then Exit Sub

            ' �I���Z�����Z�b�g
            dgvCell = dgv(e.ColumnIndex, e.RowIndex)
            If 0 = e.ColumnIndex Or "" = dgvCell.Value.ToString Then
                '�r����
                nX = e.ColumnIndex * m_nHabaInterval * m_nInterval
                nY = e.RowIndex * 1000 * m_nInterval
                RaiseEvent tEv_MapClick(New Point(nX, nY))
            Else
                '�r����
                nX = CType(dgvCell.Tag, DATA_DETECT_INF).nFrameXc
                nY = CType(dgvCell.Tag, DATA_DETECT_INF).nY
                RaiseEvent tEv_KizuClick(New Point(nX, nY))
            End If

            m_nNowRow = CInt(Math.Floor(nY / (m_nInterval * 1000)))	'��

        Catch ex As Exception

        End Try
    End Sub

    ''' <summary>
    ''' ���f�{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnReflect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnReflect.Click

        MapDisp()

    End Sub

    ''' <summary>
    ''' �`��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMap_Paint(ByVal sender As System.Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles dgvMap.Paint
        Dim penNow As New System.Drawing.Pen(Color.Orange, 3)                '�S���摜�\���ʒu�F
        Dim penCut As New System.Drawing.Pen(Color.Green, 2)                '�S���摜�\���ʒu�F
        penCut.DashStyle = Drawing2D.DashStyle.Dash
        Try

            Dim nStartPos As Integer = dgvMap.FirstDisplayedScrollingRowIndex

            For nDspRow As Integer = 0 To dgvMap.DisplayedRowCount(False) - 1
                If m_nNowRow = (nDspRow + nStartPos) Then
                    e.Graphics.DrawRectangle(penNow, 0, dgvMap(0, 0).Size.Height * nDspRow + dgvMap.ColumnHeadersHeight, _
                                                        dgvMap.Width - 18, dgvMap(0, 0).Size.Height)
                End If
            Next

            For nDspRow As Integer = 0 To dgvMap.DisplayedRowCount(False) - 1
                If CType(dgvMap(0, nDspRow + nStartPos).Tag, Integer) = 1 Then
                    e.Graphics.DrawLine(penCut, 0, dgvMap(0, 0).Size.Height * nDspRow + dgvMap.ColumnHeadersHeight, _
                                                        dgvMap.Width - 18, dgvMap(0, 0).Size.Height * nDspRow + dgvMap.ColumnHeadersHeight)

                End If
            Next
        Catch ex As Exception
        Finally
            penNow.Dispose()
            penCut.Dispose()
        End Try
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �}�b�v�`��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub MapDisp()

        '��{���Z�b�g
        BaseInfSet()
        '��ʏ������i�����Z�b�g�j
        DspInit()

        '�R�C����񂪖����ꍇ�͏�������
        If m_typCoil.sKizukenNo Is Nothing Then Exit Sub


        Dim typdt As DATA_DETECT_INF
        Dim nRow As Integer
        Dim nCol As Integer

        For ii As Integer = 0 To m_lstDetect.Count - 1
            typdt = m_lstDetect(ii)
            If Not typdt.bDsp Then Continue For

            '�Ώۍs�ʒu�擾
            'nRow = CInt(typdt.nY / (m_nInterval * 1000))
            nRow = CInt(Math.Floor(typdt.nY / (m_nInterval * 1000)))	'��
            If dgvMap.RowCount <= nRow Then
                nRow = dgvMap.RowCount - 1
            End If
            If nRow < 0 Then
                nRow = 0
            End If
            '�Ώۗ�ʒu�擾
            nCol = SelectColPosition(1, typdt.nXc, m_typCoil.nWid, m_nHabaCol, m_nHabaInterval) + 1
            Dim bWrite As Boolean = True
            If CType(dgvMap(nCol, nRow).Tag, DATA_DETECT_INF).nNo <> 0 Then
                ' �D�揇��
                ' �@ �O���[�h�D��x
                ' �A �r��D��x
                ' �B ���菇
                Dim dNow(2) As Double
                Dim dPer(2) As Double
                Dim bMode(2) As Boolean

                dNow(0) = CDbl(typdt.gYu)
                dPer(0) = CDbl(CType(dgvMap(nCol, nRow).Tag, DATA_DETECT_INF).gYu)
                bMode(0) = False

                dNow(1) = CDbl(typdt.tYu)
                dPer(1) = CDbl(CType(dgvMap(nCol, nRow).Tag, DATA_DETECT_INF).tYu)
                bMode(1) = False

                dNow(2) = CDbl(typdt.nY)
                dPer(2) = CDbl(CType(dgvMap(nCol, nRow).Tag, DATA_DETECT_INF).nY)
                bMode(2) = False

                ' �D��x��r
                If Not tMod.SelectPriorityDetect(dNow, dPer, bMode) Then
                    ' �V������ւ��Ȃ�
                    bWrite = False
                End If
            End If

            '��\�r�Z�b�g
            If bWrite Then
                dgvMap(nCol, nRow).Value = mcls_pParam.GetParamType(typdt.tId).sSName
                dgvMap(nCol, nRow).Style.BackColor = mcls_pParam.GetParamGrade(typdt.gId).clrDspColor

                dgvMap(nCol, nRow).Tag = typdt
            End If
        Next

        Dim typCutdt As DATA_CUT_INF
        For ii As Integer = 0 To m_lstCut.Count - 1
            typCutdt = m_lstCut(ii)
            nRow = CInt(Math.Ceiling(typCutdt.nY / (m_nInterval * 1000)))   '��
            If dgvMap.RowCount <= nRow Then
                nRow = dgvMap.RowCount - 1
            End If
            If nRow < 0 Then
                nRow = 0
            End If
            dgvMap(0, nRow).Tag = 1
        Next

    End Sub

    ''' <summary>
    ''' �r�}�b�v����������
    ''' </summary>
    ''' <param name="dgv">�}�b�v�f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)

        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv

            .RowCount = m_nCoilLenRow
            .ColumnHeadersHeight = 24

            '' �����`
            .ScrollBars = ScrollBars.Vertical                             ' �X�N���[���o�[�̕\��
            '.ScrollBars = ScrollBars.None
            '.DoubleBuffered = True

            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = True                                   ' ��w�b�_�\��

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F

            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().WrapMode = DataGridViewTriState.True

            '' ���`
            .ColumnCount = CInt(nudHabaCol.Value) + 1           '�}�b�v��+�����\����

            '��0 (�A�h���X)
            .Columns(0).HeaderText = "����"
            .Columns(0).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(0).Width = 50
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(0).Frozen = True                                       ' �����ړ��֎~
            .Columns(0).HeaderCell.Style.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Dim nSize As Integer = CInt((dgv.Width - 50 - 18) / m_nHabaCol)

            '��1�`7 (�f�[�^)
            For ii As Integer = 1 To m_nHabaCol
                .Columns(ii).HeaderText = HeaderName(ii, m_typCoil.nWid, m_nHabaCol)
                .Columns(ii).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).Width = nSize
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).Frozen = True                                       ' �����ړ��֎~
                .Columns(ii).HeaderCell.Style.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Next ii

        End With

    End Sub

    ''' <summary>
    ''' �O���b�h�ʒu����֐� (�Z���^�[�)
    ''' </summary>
    ''' <param name="DorW">0:DS����r 1:WS����r ���P�Œ�</param>
    ''' <param name="nXc">C����ʒu[mm](DS:-, WS:+)</param>
    ''' <param name="Width">�R�C���� [mm]</param>
    ''' <param name="col_max">�ő�O���b�h(�����I�ȍő�O���b�h��)</param>
    ''' <param name="col_interval">�O���b�h�̕� [mm]</param>
    ''' <returns>�Z�o�O���b�h�ʒu (0:DS�`)</returns>
    ''' <remarks></remarks>
    Private Function SelectColPosition(ByVal DorW As Integer, ByVal nXc As Integer, ByVal Width As Integer, ByVal col_max As Integer, ByVal col_interval As Integer) As Integer
        Dim env As Integer = col_max Mod 2                      '// �T����ʂ̗� (0:������  1:���)
        Dim centerWid As Integer = Width \ 2                    '// ���̒��S
        Dim centerCol As Integer = col_max \ 2                  '// DS��̃O���b�h�ʒu�Z�o�p (�߂�l)
        Dim nSetCol As Integer = 0                              '// �Z�o�O���b�h�ʒu�p���[�N

        If 0 > nXc Then
            If 0 = DorW Then
                DorW = 1
            Else
                DorW = 0
            End If
            nXc = System.Math.Abs(nXc)
        End If

        '//-----------------------------------------
        '// �R�C���Z���^�[�
        Dim col As Integer = 0
        If 0 = env Then '// ����	
            col = (nXc - 1) \ (col_interval)
            If 0 = DorW Then
                nSetCol = centerCol - col - 1   '// ���
            Else
                nSetCol = centerCol + col       '// ���Α�
            End If
        Else            '// �
            nXc -= col_interval \ 2
            col = (nXc - 1) \col_interval
            If 0 = DorW Then
                nSetCol = centerCol - col - 1   '// ���
            Else
                nSetCol = centerCol + col + 1   '// ���Α�
            End If
        End If

        '�Z�o���ʂ̏㉺���`�F�b�N
        If nSetCol < 0 Then
            nSetCol = 0
        End If
        If nSetCol > col_max - 1 Then
            nSetCol = col_max - 1
        End If

        Return nSetCol

    End Function

    ''' <summary>
    ''' �w�b�_�[���̎擾
    ''' </summary>
    ''' <param name="nCol">�擾�w�b�_�[��(1�`)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function HeaderName(ByVal nCol As Integer, ByVal Width As Integer, ByVal col_max As Integer) As String
        Dim env As Integer = col_max Mod 2                      '// �T����ʂ̗� (0:������  1:���)
        Dim centerCol As Integer = CInt(Int(col_max / 2)) + 1      '// DS��̃O���b�h�ʒu�Z�o�p (�߂�l)
        Dim nWid As Integer = CInt(Math.Ceiling(Width / col_max))
        Dim nHalfWid As Integer = CInt(Math.Ceiling(nWid / 2))
        Dim strName As String = ""

        If 0 = env Then
            '// ����
            If centerCol - 1 = nCol Then
                strName = nWid & "�`" '"�`C"
            ElseIf centerCol = nCol Then
                strName = "�`" & nWid '"C�`" & nWid
            Else
                If centerCol > nCol Then
                    strName = nWid * (centerCol - nCol) & "�`"
                Else
                    strName = "�`" & nWid * (nCol - centerCol + 1)
                End If
            End If
        Else
            '// �
            If centerCol = nCol Then
                strName = nHalfWid & "�`C�`" & nHalfWid
            Else
                If centerCol > nCol Then
                    strName = nWid * (centerCol - nCol) + nHalfWid & "�`"
                Else
                    strName = "�`" & nWid * (nCol - centerCol) + nHalfWid
                End If
            End If
        End If

        Return strName
    End Function

    ''' <summary>
    ''' ��{���Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub BaseInfSet()
        m_nNowRow = 0
        m_nOldRow = -1
        m_nHabaCol = CInt(nudHabaCol.Value)                                 ' ��������
        m_nInterval = CInt(nudInterval.Value)                               ' ����T�C�Y
        m_nCoilLenRow = CInt((m_typCoil.nLen / 1000) / m_nInterval) + 1     ' �\���s��
        m_nHabaInterval = CInt(Math.Ceiling(m_typCoil.nWid / m_nHabaCol))	' �؂�グ
        If (1 > m_nHabaInterval) Then
            m_nHabaInterval = 1

        End If
    End Sub

    ''' <summary>
    ''' �}�b�v�������i�����̓Z�b�g�j
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInit()
        ' �}�b�v������
        dgvInitialize(dgvMap)

        For ii As Integer = 0 To m_nCoilLenRow - 1
            dgvMap(0, ii).Value = m_nInterval * ii
            ' �Z���̏�����
            For jj As Integer = 1 To m_nHabaCol
                dgvMap(jj, ii).Value = ""
                dgvMap(jj, ii).Style.BackColor = Color.Empty
            Next jj
        Next ii
    End Sub

    ''' <summary>
    ''' �w��ʒu�փW�����v�i���S�j
    ''' </summary>
    ''' <param name="nLen">�w��ʒu[mm]</param>
    ''' <remarks></remarks>
    Public Sub DspJumpRow(ByVal nLen As Integer)
        Dim nRow As Integer

        '�P�s���������͏����I��
        If dgvMap.RowCount <= 0 Then
            Exit Sub
        End If

        Try
            '���[�U�[�ɕ\�������s�̐����擾
            Dim nDspRowCnt As Integer = dgvMap.DisplayedRowCount(False)

            '�w��ʒu[mm]���s�ɕϊ�(�؂�̂�)
            m_nNowRow = CInt(Math.Floor(nLen / (m_nInterval * 1000)))
            If m_nNowRow < 0 Then
                m_nNowRow = 0
            End If
            If m_nNowRow > dgvMap.RowCount - 1 Then
                m_nNowRow = dgvMap.RowCount - 1
            End If

            '�O��l�Ɠ����s�ʒu�������牽�������ɏI��
            If m_nNowRow = m_nOldRow Then
                Exit Sub
            End If

            '�w��ʒu�����S�ɂȂ�悤�ɐ擪�ʒu(����)���Z�o����
            nRow = m_nNowRow - CInt(Math.Floor(nDspRowCnt / 2))
            '�ŏI�s�𒴂���ꍇ
            If nRow > (dgvMap.RowCount - nDspRowCnt) Then
                nRow = dgvMap.RowCount - nDspRowCnt
            End If
            '�擪�s�𒴂���ꍇ
            If nRow < 0 Then
                nRow = 0
            End If

            '�O��l�֍���l��ۑ�
            m_nOldRow = m_nNowRow
            '
            '�\�������ŏ��̍s�̃C���f�b�N�X��ݒ�
            dgvMap.FirstDisplayedScrollingRowIndex = nRow

            '��ʃ��t���b�V��
            dgvMap.Refresh()

        Catch ex As Exception

        End Try

    End Sub

    ''' <summary>
    ''' �O��l�ۑ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Save_last_Control_Ini()
        '�V�F�[�f�B���O�␳
        tMod.WriteIni("RPI_VIEWER", "KIZU_MAP_HABA_COL", LOCALINI_NAME, CStr(nudHabaCol.Value))                 '��������
        tMod.WriteIni("RPI_VIEWER", "KIZU_MAP_INTERVAL", LOCALINI_NAME, CStr(nudInterval.Value))                '����T�C�Y
    End Sub

    ''' <summary>
    ''' �O��l�Ǎ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Load_last_Control_Ini()
        nudHabaCol.Value = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "KIZU_MAP_HABA_COL", LOCALINI_NAME, 10)   '���w��̏ꍇ�́A10
        nudInterval.Value = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "KIZU_MAP_INTERVAL", LOCALINI_NAME, 2)   '���w��̏ꍇ�́A 2
    End Sub

End Class