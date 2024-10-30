'*********************************************************************************
'�@�r�}�b�v�\�����Ǘ��N���X
'	[Ver]
'		Ver.01    2008/10/16  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class clsDspDefMap
    Implements IDisposable                          ' �f�X�g���N�^��`

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�N���X
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Param As clsParamManager                           ' �p�����[�^�Ǘ��N���X
    Private mcls_Log As tClass.tClass_LogManager                    ' ���O�Ǘ��N���X

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R�C�����\���R���g���[��
    ''' </summary>
    Public Structure TYP_MAP_CTRL
        Dim lblWs As Label                '�r�}�b�v���x��
        Dim lblDs As Label                '�r�}�b�v���x��
        Dim picMap As PictureBox
    End Structure

    '---------------------------------------------------------------------------
    ' �^��`
    '---------------------------------------------------------------------------
    '// �r�}�b�v�\����ʁi�U�z�}�j�p���r�V���{����`
    '// �e�[�u���\����
    Public Structure DEFMAP_SYMBOL
        Dim mSymbol As String                                                   ' �\���V���{��
        Dim mFont As String                                                     ' �t�H���g
        Dim mSize As Integer                                                    ' �T�C�Y
    End Structure
    Public Const PLOT_SYMBOL_FONT As String = "�l�r �S�V�b�N"                   ' �r�V���{���t�H���g��`
    Public Const PLOT_SYMBOL_SIZE As Integer = 8                                ' �r�V���{���t�H���g�T�C�Y
    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ ��`
    ''/ /////////////////////////////////////////////////////////////////////////////////
    Private Const nLabelHosei As Integer = 12                                   '���x���`��ʒu������

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private disposedValue As Boolean = False        ' �d������Ăяo�������o����ɂ�

    Private m_typCtrl As TYP_MAP_CTRL               '

    Private m_bTurnLR As Boolean                    ' ����c�����\�����A���E���] (True:���]�L�� False:���]�Ȃ�) 
    Private m_bTurnTB As Boolean                    ' ��������ɑ΂��āA�㉺���] (True:���]�L�� False:���]�Ȃ�)
    Private m_bMapDir As Boolean                    ' �}�b�v�̏㉺���� (True:�}�b�v�㕔���R�C���擪 False:�}�b�v�������R�C���擪)

    Private m_nMen As Integer                       ' �\����(0:�\�ʁA1:����)
    Private m_nCoilLen As Integer = 0               ' �R�C�����ђ� [mm]
    Private m_nCoilWid As Integer = 0               ' �� [mm]

    Private mcls_Map As tClass_MapBase              ' �`��^�C�v���r�}�b�v�\��

    Private m_lstDefData As New List(Of DETECT_BASE_DATA)   ' �r��񃊃X�g
    Private m_lstCutData As New List(Of CUT_INF_DATA)       ' �J�b�g���`���񃊃X�g

    Private m_nMarkerLabel As EM_MARKER_MODE         ' �\���}�[�J�[���x��ID
    Private m_nMarkerColor As EM_MARKER_COLOR_MODE   ' �\���}�[�J�[�J���[ID

    Private m_PlotSymbol() As DEFMAP_SYMBOL                                      ' �r�V���{�� (�r�킲�Ƃɒ�`)

    '---------------------------------------------------------------------------
    ' �v���p�e�B
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �}�b�v�̏㉺���� (True:�}�b�v�㕔���R�C���擪 False:�}�b�v�������R�C���擪)
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_MapDir As Boolean
        Get
            Return m_bMapDir
        End Get
        Set(ByVal Value As Boolean)
            m_bMapDir = Value
        End Set
    End Property

    ''' <summary>
    ''' ��������ɑ΂��āA�㉺���] (True:���]�L�� False:���]�Ȃ�)
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_TurnTB As Boolean
        Get
            Return m_bTurnTB
        End Get
        Set(ByVal Value As Boolean)
            m_bTurnTB = Value
        End Set
    End Property

    ''' <summary>
    ''' ����c�����\�����A���E���] (True:���]�L�� False:���]�Ȃ�) 
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_TurnLR As Boolean
        Get
            Return m_bTurnLR
        End Get
        Set(ByVal Value As Boolean)
            m_bTurnLR = Value
            If m_bTurnLR = True Then
                m_typCtrl.lblDs.Text = "WS"
                m_typCtrl.lblWs.Text = "DS"
            Else
                m_typCtrl.lblDs.Text = "DS"
                m_typCtrl.lblWs.Text = "WS"
            End If
        End Set
    End Property

    ''' <summary>
    '''  �\���}�[�J�[���x��ID
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_MarkerLabel As EM_MARKER_MODE
        Get
            Return m_nMarkerLabel
        End Get
        Set(ByVal Value As EM_MARKER_MODE)
            m_nMarkerLabel = Value
        End Set
    End Property

    ''' <summary>
    ''' �\���}�[�J�[�J���[ID
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_MarkerColor As EM_MARKER_COLOR_MODE
        Get
            Return m_nMarkerColor
        End Get
        Set(ByVal Value As EM_MARKER_COLOR_MODE)
            m_nMarkerColor = Value
        End Set
    End Property

    ''' <summary>
    ''' �R�C�������Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("���r�}�b�v�\�����Ǘ��N���X��p�v���p�e�B") _
     , System.ComponentModel.Description("�R�C�������Z�b�g")>
    Public Property tProp_nCoilWid() As Integer
        Get
            Return m_nCoilWid
        End Get
        Set(ByVal Value As Integer)
            m_nCoilWid = Value
        End Set
    End Property

    ''' <summary>
    ''' �R�C�������Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("���r�}�b�v�\�����Ǘ��N���X��p�v���p�e�B") _
     , System.ComponentModel.Description("�R�C�������Z�b�g")>
    Public Property tProp_nCoilLen() As Integer
        Get
            Return m_nCoilLen
        End Get
        Set(ByVal Value As Integer)
            m_nCoilLen = Value
        End Set
    End Property

    '///////////////////////////////////////////////////////////////////////////
    '�����o�֐�
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New(ByRef typCtrl As TYP_MAP_CTRL, ByVal nMen As Integer, ByRef param As clsParamManager, ByRef log As tClass_LogManager)

        '�r�}�b�v��ʃR���g���[��
        m_typCtrl = typCtrl

        m_nMen = nMen

        mcls_Log = log
        mcls_Param = param

        '�I���r�擾
        m_lstDefData = Nothing
        m_lstCutData = Nothing

        ' �\���V���{���A�t�H���g�A�T�C�Y�̏�����
        ReDim m_PlotSymbol(PLOT_SYMBOL_TYPE.Length - 1)
        For ii As Integer = 0 To PLOT_SYMBOL_TYPE.Length - 1
            m_PlotSymbol(ii).mSymbol = PLOT_SYMBOL_TYPE(ii)
            m_PlotSymbol(ii).mFont = PLOT_SYMBOL_FONT
            m_PlotSymbol(ii).mSize = PLOT_SYMBOL_SIZE
        Next

        mcls_Map = New tClass_MapBase()
        AddHandler mcls_Map.tEv_DrawPaint_End, AddressOf mcls_Map_tEv_DrawPaint_End
        AddHandler m_typCtrl.picMap.Paint, AddressOf PictureBox_Paint

    End Sub

    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
            End If

        End If
        Me.disposedValue = True
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
    ''' ���݂̕\���ݒ�ŁA�r�}�b�v���́E�ڐ���̕`����s���܂��B
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DrawKizuMapFrame()

        ' �r�}�b�v�̏�����
        InitializeMap()                                         ' �̈揉�����A�ڐ���ݒ�
        ' �R�C���ݒ�
        SetCoilSize(m_nCoilLen, m_nCoilWid)                     ' �R�C�����ђ����A���̕\��
    End Sub

    ''' <summary>
    ''' �`��
    ''' </summary>
    Public Sub SetDrawPaint()
        mcls_Map.DrawPaint()
    End Sub

    ''' <summary>
    ''' �r���ݒ�
    ''' </summary>
    ''' <param name="lstData">�r���</param>
    ''' <remarks></remarks>
    Public Sub SetDefectData(ByRef lstData As List(Of DETECT_BASE_DATA))
        Try
            m_lstDefData = lstData
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v��� �r���ݒ�ُ� [{0}] Men=[{1}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �J�b�g�ʒu���ݒ�
    ''' </summary>
    ''' <param name="lstData"></param>
    ''' <remarks></remarks>
    Public Sub SetCutData(ByRef lstData As List(Of CUT_INF_DATA))
        Try
            m_lstCutData = lstData
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v��� �J�b�g�ʒu���ݒ�ُ� [{0}] ", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r���ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ClearDefectData()
        Try
            m_lstDefData = Nothing
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v��� �r���N���A�ُ� [{0}] Men=[{1}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �J�b�g�ʒu���ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ClearCutData()
        Try
            m_lstCutData = Nothing
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v��� �J�b�g�ʒu���N���A�ُ� [{0}] ", ex.Message))
        End Try
    End Sub

    Public Sub DspInit()
        ClearCutData()
        ClearDefectData()
    End Sub

    ''' <summary>
    ''' �r�}�b�v�̏�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitializeMap()
        Try

            ' ���� �c�����\��
            With mcls_Map
                '-----------------------------------------------------------
                ' �\���̈���
                '-----------------------------------------------------------
                .tProp_AriaInf_BackGroundLineColor = Pens.Black
                .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
                .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
                .tProp_AriaInf_AriaSize = m_typCtrl.picMap.Size
                .tProp_AriaInf_Margin = New Padding(30, 30, 30, 30)

                '-----------------------------------------------------------
                ' �}�b�v�̈�
                '-----------------------------------------------------------
                .tProp_MapInf_IsCenterX = True
                .tProp_MapInf_IsCenterY = False
                .tProp_MapInf_IsBaseX_L = True
                .tProp_MapInf_IsBaseY_T = m_bMapDir
                .tProp_MapInf_IsStrechX = True
                .tProp_MapInf_IsStrechY = True
                .tProp_MapInf_LimitXmin = -1 * CInt(COIL_WID_MAX / 2)
                .tProp_MapInf_LimitXmax = CInt(COIL_WID_MAX / 2)
                .tProp_MapInf_LimitYmin = 0
                .tProp_MapInf_LimitYmax = COIL_LEN_MAX * 1000
                .tProp_MapInf_Offset = New tClass.tClass_MapBase.xyValue(0, 0)

                '-----------------------------------------------------------
                ' �}�b�v�ΏۊO�̈�ݒ�
                '-----------------------------------------------------------
                .tProp_LimitInf_LimitDraw = True

                '-----------------------------------------------------------
                ' �ڐ���ݒ�
                '-----------------------------------------------------------
                Dim p As Pen = New Pen(Color.LightBlue, 1)
                p.DashStyle = Drawing2D.DashStyle.DashDot
                .tProp_MemoriInf_LinePen = p
                .tProp_MemoriInf_StringFont = New Font("�l�r �S�V�b�N", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
                .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Black)
                .tProp_MemoriInf_PosStringX = 1
                .tProp_MemoriInf_PosStringY = 1
                .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(200, 50 * 1000)
                .tProp_MemoriInf_ScaleStringFormatX = "0"
                .tProp_MemoriInf_ScaleStringFormatY = "###0"
                .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1, 1000)
                .tProp_MemoriInf_StringDisp_T = True
                .tProp_MemoriInf_StringDisp_B = True
                .tProp_MemoriInf_StringDisp_L = True
                .tProp_MemoriInf_StringDisp_R = True
                .tProp_MemoriInf_LineDsp_X = True
                .tProp_MemoriInf_LineDsp_Y = True
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v��� �}�b�v�������ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �R�C���T�C�Y�ݒ肵�܂�
    ''' </summary>
    ''' <param name="nCoilLen">�R�C�����ђ� [mm]</param>
    ''' <param name="nCoilWidth">�� [mm]</param>
    ''' <remarks></remarks>
    Private Sub SetCoilSize(ByVal nCoilLen As Integer, ByVal nCoilWidth As Integer)
        Try
            ' �R�C����
            Dim nWid As Integer = nCoilWidth
            If 0 >= nWid Then                                                   ' ��0�Ȃ�ő�ɂ���
                nWid = COIL_WID_MAX
            End If

            ' ���� �c�����\��
            With mcls_Map
                '-----------------------------------------------------------
                ' �}�b�v�ΏۊO�̈�ݒ�
                '-----------------------------------------------------------
                .tProp_LimitInf_LimitDraw = True
                .tProp_LimitInf_LimitOutLineColor = Pens.Black
                .tProp_LimitInf_LimitOutColor = New SolidBrush(Color.Gray)

                .tProp_LimitInf_LimitXmin = CInt(nWid / 2) * -1
                .tProp_LimitInf_LimitXmax = CInt(nWid / 2)
                .tProp_MapInf_LimitYmax = nCoilLen
                .tProp_LimitInf_LimitYmin = 0
                .tProp_LimitInf_LimitYmax = .tProp_MapInf_LimitYmax

                Dim intval As tClass_MapBase.xyValue = .tProp_MemoriInf_ScaleInterval
                Dim dLen As Double = nCoilLen / 1000
                If dLen <= 200 Then                                         ' �R�C���� 200m�ȉ��Ȃ�20m�Ԋu
                    intval.Y = 20
                ElseIf dLen <= 1000 Then                                    ' �R�C���� 1000m�ȉ��Ȃ�100m�Ԋu
                    intval.Y = 100
                ElseIf dLen <= 3000 Then                                    ' �R�C���� 3000m�ȉ��Ȃ�200m�Ԋu
                    intval.Y = 200
                ElseIf dLen <= 6000 Then                                    ' �R�C���� 6000m�ȉ��Ȃ�500m�Ԋu
                    intval.Y = 500
                ElseIf dLen <= 10000 Then                                   ' �R�C���� 10000m�ȉ��Ȃ�1000m�Ԋu
                    intval.Y = 1000
                Else                                                        ' �R�C���� 10000m�z���Ȃ�2000m�Ԋu
                    intval.Y = 2000
                End If
                intval.Y *= 1000                                            ' ���[�g�� -> �~�����[�g�ϊ�
                .tProp_MemoriInf_ScaleInterval = intval

            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v��� �R�C���ύX�ُ� [{0}] Len=[{1}] Width=[{2}]", ex.Message, nCoilLen, nCoilWidth))
        End Try
    End Sub

    '***************************************************************************
    ' �s�N�`���[�{�b�N�X�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �r�}�b�v�̃s�N�`���[�{�b�N�X�`��C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub PictureBox_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        ' �r�b�g�}�b�v�擾
        If mcls_Map Is Nothing = True Then
            Exit Sub
        End If

        Dim bmp As Bitmap = mcls_Map.tProp_Bmp
        If bmp Is Nothing Then
            Exit Sub
        End If

        '''' �S�̃}�b�v�w�i�摜���Đ���
        e.Graphics.DrawImage(bmp, 0, 0)

    End Sub


    '***************************************************************************
    ' �r�}�b�v�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �r�}�b�v�`��I���C�x���g
    ''' </summary>
    ''' <param name="map">�C�x���g��</param>
    ''' <param name="g">�`��ւ̃O���t�B�b�N</param>
    ''' <remarks></remarks>
    Private Sub mcls_Map_tEv_DrawPaint_End(ByVal map As tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics)

        Try

            '==============================================================
            '  �J�b�g�ʒu�`��
            '==============================================================
            Dim nPicCutX1 As Integer = 0
            Dim nPicCutY1 As Integer = 0
            Dim nPicCutX2 As Integer = 0
            Dim nPicCutY2 As Integer = 0
            Dim nPosPre As Integer = 0
            Dim pen_Cut As New System.Drawing.Pen(Color.Black, 3)
            Dim fntwk As New System.Drawing.Font("�l�r �S�V�b�N", 16, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            Dim bshwk As New SolidBrush(Color.Black)

            If tProp_TurnTB Then
                nPosPre = map.tProp_MapInf.nLimitYmax
            End If
            If m_lstCutData IsNot Nothing Then
                For ii As Integer = 0 To m_lstCutData.Count - 1
                    Dim nPos As Integer = 0

                    nPos = CInt(m_lstCutData(ii).dPosS * 1000)
                    Select Case m_lstCutData(ii).nKubun
                        Case 1  ' �V���[�J�b�g�i�t�@�[�X�g�J�b�g)
                            pen_Cut.Color = g_ColorSharStart
                        Case 2  ' �V���[�J�b�g�i���X�g�J�b�g�j
                            pen_Cut.Color = g_ColorSharEnd
                        Case 3  ' ���� �i�������́A�P�_�̂݁B�J�b�g�͔͈́j
                            pen_Cut.Color = g_ColorShar
                        Case 4  ' �َ�_
                            pen_Cut.Color = g_ColorIsyuten
                        Case Else
                    End Select

                    nPicCutX1 = map.GetVtoP_X(map.tProp_MapInf_LimitXmin)
                    nPicCutX2 = map.GetVtoP_X(map.tProp_MapInf_LimitXmax)
                    '�����Ă���ꍇ�̓��~�b�g�l��
                    If (nPos > map.tProp_MapInf.nLimitYmax) Then
                        nPos = map.tProp_MapInf.nLimitYmax
                    End If
                    nPicCutY1 = map.GetVtoP_Y(nPos)
                    nPicCutY2 = nPicCutY1
                    g.DrawLine(pen_Cut, nPicCutX1, nPicCutY1, nPicCutX2, nPicCutY2)


                    '�}�b�v�[�ɃJ�b�g����ID��\��
                    Dim nPosY As Integer
                    If tProp_TurnTB Then
                        nPosY = nPosPre - CInt((nPosPre - nPos) / 2)
                    Else
                        nPosY = CInt((nPos - nPosPre) / 2) + nPosPre
                    End If

                    Dim nPicPos As Integer = map.GetVtoP_Y(nPosY)
                    g.DrawString(CUT_RESULT_ID_NO(ii), fntwk, bshwk, map.GetVtoP_X(map.tProp_MapInf_LimitXmax) + 1, nPicPos - 8)
                    '�O��l�ۑ�
                    nPosPre = nPos
                Next
            End If

            pen_Cut.Dispose()
            fntwk.Dispose()
            bshwk.Dispose()

            ''==============================================================
            ''  �D�揇�ʂŕ��בւ�
            ''==============================================================
            ''�D�揇�ʂ̒Ⴂ���Ƀ\�[�g(�D�揇�ʂ̍����r����ɗ���悤��)
            'm_lstDefData.Sort(AddressOf SortUnPriorty)

            '==============================================================
            '  �r�V���{���`��
            '==============================================================
            Dim nPicX As Integer = 0
            Dim nPicY As Integer = 0
            Dim fntMemori As New System.Drawing.Font("�l�r �S�V�b�N", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            Dim fntSymbol As New System.Drawing.Font("�l�r �S�V�b�N", 6, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)

            If m_lstDefData IsNot Nothing Then
                For ii As Integer = 0 To m_lstDefData.Count - 1
                    Dim typ As DETECT_BASE_DATA = m_lstDefData(ii)      ' �r���
                    '---------------------------------------------------------------
                    ' X���W�̍��W�Z�o (���]�l��)
                    '---------------------------------------------------------------
                    Dim nX As Integer = 0

                    If Not tProp_TurnLR Then
                        nX = CInt(typ.wsc)
                    Else
                        nX = CInt(typ.dsc)
                    End If

                    '---------------------------------------------------------------
                    ' X���W�̏㉺��
                    '---------------------------------------------------------------
                    If nX < map.tProp_LimitInf_LimitXmin Then                       ' �ŏ�X���W�𒴂����ꍇ�͍ŏ����W�܂�
                        nX = map.tProp_LimitInf_LimitXmin
                    End If
                    If nX > map.tProp_LimitInf_LimitXmax Then                       ' �ő�X���W�𒴂����ꍇ�͍ő���W�܂�
                        nX = map.tProp_LimitInf_LimitXmax
                    End If
                    nPicX = map.GetVtoP_X(nX)

                    '---------------------------------------------------------------
                    ' Y���W�̍��W�Z�o
                    '---------------------------------------------------------------
                    Dim nY As Integer = CInt(Int(typ.y))                        ' �o������̋���

                    '---------------------------------------------------------------
                    ' Y���W�̏㉺��
                    '---------------------------------------------------------------
                    If nY < map.tProp_LimitInf_LimitYmin Then                       ' �ŏ�X���W�𒴂����ꍇ�͍ŏ����W�܂�
                        nY = map.tProp_LimitInf_LimitYmin
                    End If
                    If nY > map.tProp_LimitInf_LimitYmax Then                       ' �ő�X���W�𒴂����ꍇ�͍ő���W�܂�
                        nY = map.tProp_LimitInf_LimitYmax
                    End If
                    nPicY = map.GetVtoP_Y(nY)

                    '---------------------------------------------------------------
                    ' �`��
                    '---------------------------------------------------------------
                    '---------------------------------------------------------------
                    Dim MapSymbolNo As Integer = mcls_Param.GetMapSymbolNo(typ.nTid)                                ' �\���V���{���ԍ��擾
                    If MapSymbolNo < 0 Then Continue For
                    Dim drawString As String = m_PlotSymbol(MapSymbolNo).mSymbol                                    ' �r�\���V���{��

                    Dim pen_in As SolidBrush
                    If EM_MARKER_COLOR_MODE.nMarkerColorModeType = m_nMarkerColor Then
                        pen_in = New SolidBrush(mcls_Param.GetTypeSymbolColor(typ.nTid))     ' �r��\���F
                    Else
                        pen_in = New SolidBrush(mcls_Param.GetGradeSymbolColor(typ.nGid))   ' �O���[�h�\���F
                    End If
                    Dim drawPoint As New PointF(nPicX - CInt(fntSymbol.Size / 2), nPicY - CInt(fntSymbol.Size / 2)) ' �r�\�����W
                    g.DrawString(drawString, fntSymbol, pen_in, drawPoint)                                          ' �r�\��

                    Dim DrawInfo As String = ""
                    If EM_MARKER_MODE.nMarkerModeType = m_nMarkerLabel Then
                        DrawInfo = mcls_Param.GetMapName(typ.nTid)
                    ElseIf EM_MARKER_MODE.nMarkerModeGrade = m_nMarkerLabel Then
                        DrawInfo = mcls_Param.GetGradeName(typ.nGid)
                    End If
                    g.DrawString(DrawInfo, fntMemori, pen_in, drawPoint.X, drawPoint.Y - nLabelHosei)
                    pen_in.Dispose()
                Next ii
            End If
            fntMemori.Dispose()
            fntSymbol.Dispose()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v��� �r�}�b�v�`��I���C�x���g�ُ� [{0}]", ex.Message))
        End Try

    End Sub

End Class
