Public Module Hy_So_Common

	'====================================================================
    '	���щ�ʁA�r�}�b�v�\����� �J�b�g�ʒu���p�\����
	'====================================================================
    Public Structure CUT_INF_DATA
        Dim nKubun As Integer                                       '// �敪
        Dim dPosS As Single                                         '// From [m] (�o�������R�C����[���狗��)
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>		
		Dim dPosS_MM As Single                                      '// From [mm] (�o�������R�C����[���狗��)
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
    End Structure

    '�}�b�v�Z�����
    Public Structure TYP_MAP_LIST_VALUE
        Dim nRec As Integer                                 ' ���R�[�hNo(�T����ʂ̂�)
        Dim nPnt As Integer                                 ' �r���e�[�u���ւ̃|�C���^(�T����ʂ̂�)
        Dim nKizuNo As Integer                              ' �rNo
        Dim nTid As Integer                                 ' �r��EdasysID
        Dim nTYuu As Integer                                ' �r��D��x
        Dim nGid As Integer                                 ' �O���[�hEdasysID
        Dim nGYuu As Integer                                ' �O���[�h�D��x
        Dim nAria As Integer                                ' �ʐ� 
        Dim nY As Integer                                   ' ����ʒu
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
		Dim nY_Out As Integer                               ' ����ʒu(�o��)
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
        Dim nCycDiv As Integer                              ' �����r�敪
        Dim strFFType As String                             ' FF�r��(TCM�p)
        Dim strFFGrade As String                            ' FF�O���[�h(TCM�p)
        Dim strProc As String                               ' �O�H��(TCM�p)
        ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
        Dim nKouteiID As Integer                            ' �H����� 
        ' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

        Shared Operator =(ByVal a As TYP_MAP_LIST_VALUE, ByVal b As TYP_MAP_LIST_VALUE) As Boolean
        	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
            'If a.nRec <> b.nRec Or a.nPnt <> b.nPnt Or a.nKizuNo <> b.nKizuNo Then
			If a.nRec <> b.nRec Or a.nPnt <> b.nPnt Or a.nKizuNo <> b.nKizuNo Or a.nY_Out <> b.nY_Out Then
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
                Return False
            End If
            Return True
        End Operator

        Shared Operator <>(ByVal a As TYP_MAP_LIST_VALUE, ByVal b As TYP_MAP_LIST_VALUE) As Boolean
        	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
            'If a.nRec <> b.nRec Or a.nPnt <> b.nPnt Or a.nKizuNo <> b.nKizuNo Then
            If a.nRec <> b.nRec Or a.nPnt <> b.nPnt Or a.nKizuNo <> b.nKizuNo Or a.nY_Out <> b.nY_Out Then
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
                Return True
            End If
            Return False
        End Operator

        ''' <summary>
        ''' ������\��
        ''' </summary>
        ''' <returns>������\��</returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return String.Format("KizuNo={0} TypeID={1} GradeID={2}", nKizuNo, nTid, nGid)
        End Function

        ''' <summary>
        ''' �C�j�V����
        ''' </summary>
        Public Sub Initialize()
            nRec = -1
            nPnt = -1
            nKizuNo = -1
            strFFType = ""
            strFFGrade = ""
            strProc = ""
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
			nY_Out = 0
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
        End Sub

    End Structure

    '-------------------------------------------------------
    '����
    '-------------------------------------------------------
    ''' <summary>
    ''' ����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_COL
        Dim strKanriNo As String                            '�Ǘ�No.
        Dim nKizuNo As Integer                              '�rNo
        Dim nTid As Integer                                 '�r��EasysID
        Dim nGid As Integer                                 '�O���[�hEdasysID
    End Structure

    ''' <summary>
    ''' ����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_TCM_COL
        Dim strKanriNo As String                            '�Ǘ�No.
        Dim nKizuNo As Integer                              '�rNo
        Dim strProc As String                               ' �O�H��(TCM�p)
        Dim strFFType As String                             ' FF�r��(TCM�p)
        Dim strFFGrade As String                            ' FF�O���[�h(TCM�p)
    End Structure

    '-------------------------------------------------------
    '�s���
    '-------------------------------------------------------
    ''' <summary>
    ''' �s���
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_ROW
        Dim nRec As Integer                                 '���R�[�h�ԍ�(�T����ʂ̂�)
        Dim nMemRowPos As Integer                           '�������s�ԍ�(�T����ʂ̂�)
        Dim nEnable As Integer                              '����M���(�T����ʂ̂�)
        Dim emKensa As EM_DIV_KENSA                         '������� (DIV_KENSA_NON���)
        Dim emKiki As EM_DIV_KIKI                           '�@���� (DIV_KIKI_OK���)
        Dim nKizuCnt As Integer                             '�s�P�ʕ\���Ώ��r��
        Dim nLen_i As Integer                               '��������(�v�͗n�ړ_����̋���) [m]
        Dim nLen_o As Integer                               '�o������(�v�̓J�b�g����̋���) [m]
        Dim nCut As Integer                                 '�V���[�J�b�g�敪(0:�V���[�J�b�g���� 1:�V���[�J�b�g���ꂽ)
        Dim bYousetu As Boolean                             '�n�ړ_�t���O
        Dim col() As G_MAP_COL                              '����
        Dim tcm() As G_MAP_TCM_COL                          '����
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim col(MAP_COL_NUM - 1)
            ReDim tcm(1 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '�ʏ��
    '-------------------------------------------------------
    ''' <summary>
    ''' �ʏ��
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_MEN
        Dim row() As G_MAP_ROW
        '�C�j�V�����C�Y
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            If nRow = 0 Then
                ReDim row(MAP_ROW_NUM - 1)
            Else
                ' �s���w��̏��������s��ꂽ�ꍇ�̑Ή�
                ReDim row(nRow - 1)
            End If
            For ii As Integer = 0 To row.Length - 1
                row(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '�}�b�v�ؑ�
    '-------------------------------------------------------
    ''' <summary>
    ''' �}�b�v�ؑ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_STAGE
        Dim men() As G_MAP_MEN
        '�C�j�V�����C�Y
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            ReDim men(NUM_MEN - 1)
            For ii As Integer = 0 To men.Length - 1
                men(ii).initialize(nRow)
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '���R�[�h
    '-------------------------------------------------------
    Public Structure DSP_G_MAP
        Dim stage() As G_MAP_STAGE
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            ReDim stage(MAP_STAGE_NUM - 1)
            For ii As Integer = 0 To stage.Length - 1
                stage(ii).initialize(nRow)
            Next
        End Sub
    End Structure



    '-------------------------------------------------------
    'TCM�r���(���щ�ʗp)
    '-------------------------------------------------------
    Public Structure TYP_MAE_DEFECT_DATA
        Dim ff As DSP_TYP_MAE_FFINF                     ' FF���
        Dim data As DSP_TYP_MAE_DEFECTINF               ' �O�H���r���
        Dim strKanriNo As String                        ' �Ǘ�No.
        Dim nKizuNo As Integer                          ' ���H��DB �rNo
        Public Sub initialize()
            strKanriNo = ""
            ff.initialize()
        End Sub

    End Structure

    ''' <summary>
    ''' ��ʕ\���R���{�{�b�N�X�ɕ\������f�[�^
    ''' </summary>
    ''' <remarks>�\������</remarks>
    Public Class cls_CmbMakerItem

        ''' <summary>nMode</summary>
        Public mode As Integer          'nMode
        ''' <summary>�\����</summary>
        Public name As String           '�\����

        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="nMode">���[�h</param>
        ''' <param name="strName">�\����</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal nMode As Integer, ByVal strName As String)
            mode = nMode
            name = strName
        End Sub

        ''' <summary>
        ''' �\���̂̕\������
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return name
        End Function
    End Class

    '�r�}�[�J�[���x��
    Public Enum EM_MARKER_MODE
        nMarkerModeNone = 0     '�r�Ȃ�
        nMarkerModeType         '�r��
        nMarkerModeGrade        '��ڰ��
    End Enum

    '�r�}�[�J�[�F�ւ�
    Public Enum EM_MARKER_COLOR_MODE
        nMarkerColorModeType = 0    '�r��
        nMarkerColorModeGrade       '��ڰ��
    End Enum

    ''' <summary>
    ''' �����σt�H���g�T�C�Y�̎擾(DataGridView)
    ''' </summary>
    ''' <param name="dgv">�Ώ�DataGridView</param>
    ''' <param name="strString">�\��������</param>
    ''' <param name="nDefaultFontSize">����t�H���g�T�C�Y</param>
    ''' <param name="nMaxWidth">�ő�\����</param>
    ''' <param name="nMaxHeight">�ő�\������</param>
    ''' <returns>�ő�\�����ɓK�����t�H���g�T�C�Y(1pt�P��)</returns>
    ''' <remarks>
    ''' �t�H���g�T�C�Y��8�����̏ꍇ��8��ԋp����B
    ''' �܂��A����t�H���g�T�C�Y�ōő�\�����𒴂��Ȃ��ꍇ�͊���t�H���g�T�C�Y��Ԃ�
    ''' </remarks>
    Public Function GetGridFontScaleSize(ByVal dgv As DataGridView, ByVal strString As String, ByVal nDefaultFontSize As Single, ByVal nMaxWidth As Integer, ByVal nMaxHeight As Integer) As Single
        Dim nFontWidth As Single                    ' �����\����
        Dim nFontHeight As Single                   ' �����\������
        Dim nScaleFontSize As Single                ' �����t�H���g�T�C�Y
        Dim fntScale As System.Drawing.Font         ' �����t�H���g
        Dim nMaxDspWidth As Integer                 ' �����\���ő啝
        Dim g As Graphics = dgv.CreateGraphics()
        Dim fntDefault As Font = New System.Drawing.Font("�l�r ����", nDefaultFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

        Try

            ' ������
            nFontWidth = g.MeasureString(strString, fntDefault).Width
            nScaleFontSize = nDefaultFontSize
            fntScale = New System.Drawing.Font("�l�r ����", nScaleFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            ' �����\���ő啝�͕\���ő啝�������t�H���g�T�C�Y���p�P�������]�T����������
            nMaxDspWidth = nMaxWidth - CShort(g.MeasureString("@", fntDefault).Width)
            If nMaxDspWidth < 0 Then
                nMaxDspWidth = nMaxWidth
            End If

            ' �\���͈͓��Ɏ��܂�ꍇ�͊���t�H���g�T�C�Y��Ԃ�
            If nFontWidth <= nMaxDspWidth Then
                nFontHeight = g.MeasureString(strString, fntScale).Height
                If nFontHeight + 2 < nMaxHeight Then
                    fntScale.Dispose()
                    Return nDefaultFontSize
                End If
            End If

            ' 1pt���t�H���g�T�C�Y�����������āA�����\���ő啝�������t�H���g�T�C�Y���Z�o
            While nScaleFontSize > 0
                nScaleFontSize = nScaleFontSize - 1.0F
                If nScaleFontSize < 6 Then
                    Exit While
                End If

                fntScale = New System.Drawing.Font("�l�r ����", nScaleFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                nFontWidth = g.MeasureString(strString, fntScale).Width

                If nFontWidth <= nMaxDspWidth Then
                    nFontHeight = g.MeasureString(strString, fntScale).Height
                    If nFontHeight + 2 < nMaxHeight Then
                        Exit While
                    End If
                End If

            End While

            ' �]�菬�����ƌ����Ȃ��̂Ńt�H���g�T�C�Y��7������7�ɂ���B
            If nScaleFontSize < 6 Then
                nScaleFontSize = 6
            End If

            fntScale.Dispose()

        Catch ex As Exception

        Finally
            If g IsNot Nothing Then
                g.Dispose()
                g = Nothing
            End If

        End Try

        Return nScaleFontSize
    End Function

End Module
