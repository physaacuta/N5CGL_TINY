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
Public Class clsDefMap
    Implements IDisposable                          ' �f�X�g���N�^��`

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�N���X
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Param As clsParamManager                                       ' �p�����[�^�Ǘ��N���X
    Private mcls_Log As tClass.tClass_LogManager                                ' ���O�Ǘ��N���X

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const MEMWID_MAX As Integer = COIL_WID_MAX                          ' �}�b�v�`�敝�ő�l�i2000mm�j

    Private Const HOSEI_Y As Integer = 5                                        ' Y���`��␳�i�����ʒu�����p�j

    Private Const PICMAP_START As Integer = 20                                  ' �`��̈�㕔�]����
    Private Const PICMAP_END As Integer = 10                                    ' �`��̈扺���]����
    Private Const PICMAP_HEIGHT_OUT As Integer = PICMAP_START + PICMAP_END      ' �`��̈�㕔�����]�����v
    Private Const PICMAP_EDGE As Integer = 40                                   ' �`��̈敝�����]����
    Private Const PICMAP_WIDTH_OUT As Integer = PICMAP_EDGE * 2                 ' �`��̈敝�����]�����v
    Private Const PICMAP_MMtoM As Integer = 1000                                ' mm->m
    Private Const PICMAP_DSP_HOSEI As Integer = 1                               ' �ʒu������
    Private Const PICMAP_HOSEI_X As Integer = 6                                 ' �}�[�J�[�`��ʒu������
    Private Const PICMAP_LBL_HOSEI As Integer = 12                              ' ���x���`��ʒu������

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private disposedValue As Boolean = False                                    ' �d������Ăяo�������o����ɂ�
    Private m_typDsp As DSP_CONTROL_DATA                                        ' �r�}�b�v��ʃN���X
    Private m_blnUpTurn As Boolean                                              ' �㉺���]�t���O(True:���]����)
    Private m_blnSideTurn As Boolean                                            ' ���E���]�t���O(True:���]����)
    Private m_nMode As Integer                                                  ' ���\�敪(0:�\�A1:��)
    Private m_DefMapData As New List(Of MAP_DATA)                               ' �r�}�b�v�\���p�f�[�^
    Private m_nDrawRangeWidth As Integer = MEMWID_MAX                           ' �r�}�b�v�`�敝
    Private m_nCoilWid As Integer                                               ' �R�C����
    Private m_nCoilLen As Integer                                               ' �R�C����
    Private m_typMapPri As MAP_PRI                                              ' �}�b�v�\���D��x�iPCODE_OUTPUT�ݒ���e�j
    Private m_ArrayPri As New ArrayList                                         ' �}�b�v�\���D��x�ۑ��p

    '��{���擾�p
    Public Structure BASE_DATA
        Dim strSeisanNo As String                                       ' ���YNo
        Dim nCoilLen As Integer                                         ' �R�C����
        Dim nWidZai As Integer                                          ' �ޗ��T�C�Y��
        Dim nPCode() As Integer                                         ' �����p�^�[���R�[�h

        Public Sub initialize()
            strSeisanNo = ""
            nCoilLen = 0
            nWidZai = 0
            ReDim nPCode(NUM_MEN - 1)
        End Sub
    End Structure

    '�\���p�f�[�^�p
    Public Structure DSP_DATA
        Dim strKizukenNo As String                                      ' �r���Ǘ�NO

        Dim typBaseData As BASE_DATA                                    ' ��{���
        Dim nMaxDefPos As Integer                                       ' �Œ��r�ʒu
        Dim DefectList() As List(Of clsDefMap.MAP_DATA)                 ' �r�}�b�v���
        Dim CutLineList As List(Of clsDefMap.CUT_DATA)                  ' �J�b�g�����
        Dim SaisunList As List(Of clsDefMap.SAISUN_DATA)                ' �̐����

        Dim bGetDefectFlg As Boolean                                    ' �r���擾��

        ' ������
        Public Sub Initialize()
            strKizukenNo = ""
            typBaseData = New BASE_DATA
            typBaseData.initialize()
            nMaxDefPos = 0
            ReDim DefectList(NUM_MEN - 1)
            For ii As Integer = 0 To NUM_MEN - 1
                DefectList(ii) = New List(Of clsDefMap.MAP_DATA)
            Next
            CutLineList = New List(Of clsDefMap.CUT_DATA)
            SaisunList = New List(Of clsDefMap.SAISUN_DATA)

            bGetDefectFlg = False
        End Sub
    End Structure

    '�r�}�b�v���
    Public Structure DSP_CONTROL_DATA
        Dim lblMsg As Label               '�K�C�_���X���b�Z�[�W
        Dim prbMsg As ProgressBar         '�K�C�_���X�i��
        Dim lblWs As Label                '�r�}�b�v���x��
        Dim lblDs As Label                '�r�}�b�v���x��
        Dim lblNgMsg As Label             '�ُ�K�C�_���X�\���p
    End Structure

    '�o�͑Ώ��r�A�D��x�擾�p
    Public Structure MAP_PRI
        Dim t_id As Integer                                      '// �r��EdasysID
        Dim priority As Integer                                  '// �D��x
        Public Sub initialize()
            t_id = 0
            priority = 0
        End Sub
    End Structure

    '�r�}�b�v���擾�p
    Public Structure MAP_DATA
        Dim t_id As UShort                                      '// �r��EdasysID
        Dim g_id As UShort                                      '// �O���[�hEdasysID
        Dim y As Single                                         '// �n�ڂ���̈ʒu[m]
        Dim absX As Single                                      '// ��������Έʒu[mm]

        Public Sub initialize()
            t_id = 0
            g_id = 0
            y = 0
            absX = 0
        End Sub
    End Structure

    '�J�b�g���擾�p
    Public Structure CUT_DATA
        Dim cut_flg As Integer             '// �J�b�g�ʒu�敪(�R�C�����������r�\���Ɏg�p�B0:�R�C�����������r�Ȃ�,1:�R�C�����������r����)
        Dim Cut_Point As Integer           '// �J�b�g�ʒu�i�R�C�����������r���݂̏ꍇ�̓R�C���I�[�ʒu�j
        Public Sub initialize()
            Cut_Point = 0
        End Sub
    End Structure

    '�̐����擾�p
    Public Structure SAISUN_DATA
        Dim Start_Point As Integer         '// �̐��J�n�ʒu�i�R�C�����������r���݂̏ꍇ�͍Œ��r����ʒu�j
        Dim End_Point As Integer           '// �̐��I���ʒu�i�R�C�����������r���݂̏ꍇ�͍Œ��r����ʒu�j
        Public Sub initialize()
            Start_Point = 0
            End_Point = 0
        End Sub
    End Structure

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
    ''' �R�C�������Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("���r�}�b�v�\�����Ǘ��N���X��p�v���p�e�B") _
     , System.ComponentModel.Description("�R�C�������Z�b�g")> _
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
     , System.ComponentModel.Description("�R�C�������Z�b�g")> _
    Public Property tProp_nCoilLen() As Integer
        Get
            Return m_nCoilLen
        End Get
        Set(ByVal Value As Integer)
            m_nCoilLen = Value
        End Set
    End Property

    ''' <summary>
    ''' �r�����Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("���r�}�b�v�\�����Ǘ��N���X��p�v���p�e�B") _
     , System.ComponentModel.Description("�r�����Z�b�g")> _
    Public Property tProp_DefMapData() As List(Of MAP_DATA)
        Get
            Return m_DefMapData
        End Get
        Set(ByVal Value As List(Of MAP_DATA))
            m_DefMapData = Value
        End Set
    End Property

    ''' <summary>
    ''' �\���敪���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("���r�}�b�v�\�����Ǘ��N���X��p�v���p�e�B") _
     , System.ComponentModel.Description("�\���敪���Z�b�g")> _
    Public Property tProp_nMode() As Integer
        Get
            Return m_nMode
        End Get
        Set(ByVal Value As Integer)
            m_nMode = Value
        End Set
    End Property

    ''' <summary>
    ''' �㉺���]���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("���r�}�b�v�\�����Ǘ��N���X��p�v���p�e�B") _
     , System.ComponentModel.Description("�㉺���]���Z�b�g")> _
    Public Property tProp_bUpTurn() As Boolean
        Get
            Return m_blnUpTurn
        End Get
        Set(ByVal Value As Boolean)
            m_blnUpTurn = Value
        End Set
    End Property

    ''' <summary>
    ''' ���E���]���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("���r�}�b�v�\�����Ǘ��N���X��p�v���p�e�B") _
     , System.ComponentModel.Description("���E���]���Z�b�g")> _
    Public Property tProp_bSideTurn() As Boolean
        Get
            Return m_blnSideTurn
        End Get
        Set(ByVal Value As Boolean)
            m_blnSideTurn = Value
        End Set
    End Property

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="typDspdata">�r�}�b�v���</param>
    ''' <param name="blnUpTurn">�㉺���]�敪</param>
    ''' <param name="blnSideTurn">���E���]�敪</param>
    ''' <param name="nMode">���\�敪</param>
    ''' <param name="clsLog">���O�N���X</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef typDspdata As DSP_CONTROL_DATA, _
                    ByVal blnUpTurn As Boolean, ByVal blnSideTurn As Boolean, ByVal nMode As Integer, _
                    ByRef clsLog As tClass.tClass_LogManager, ByVal clsParam As clsParamManager)

        '�r�}�b�v��ʃR���g���[��
        m_typDsp = typDspdata

        '�㉺���]�敪
        m_blnUpTurn = blnUpTurn

        '�㉺���]�敪
        m_blnSideTurn = blnSideTurn

        '���\�敪
        m_nMode = nMode

        '���O�N���X
        mcls_Log = clsLog

        '���O�N���X
        mcls_Param = clsParam

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
    ''' �r�}�b�v��񏉊�������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()

        ''�ۑ��̈�N���A
        m_DefMapData.Clear()

    End Sub


    ''' <summary>
    ''' �r�}�b�v���y�C���g�C�x���g�ŕ\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Sub picMapBase_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        Dim fntMemori As New System.Drawing.Font("�l�r �S�V�b�N", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        Dim penMemori As New System.Drawing.Pen(Color.Black, 1)
        Dim penMemori_2 As New System.Drawing.Pen(Color.Cyan, 1)
        penMemori_2.DashStyle = Drawing2D.DashStyle.Dash
        Dim penEdge As New System.Drawing.Pen(Color.Red, 2)
        penEdge.DashStyle = Drawing2D.DashStyle.Dot

        Dim bshMemori As New SolidBrush(Color.Black)
        Dim ii As Integer
        Dim picMap As PictureBox = CType(sender, PictureBox)
        Dim nInterval As Double
        Dim dPicMapMemWid As Double = (picMap.Width - PICMAP_WIDTH_OUT) / m_nDrawRangeWidth ' �}�b�v�`�敝����1����������
        Dim nPicMapCenterW As Integer = CInt(picMap.Width / 2)                              ' �}�b�v�`��̈敝���S�ʒu
        Dim nCoilWid_Max As Integer = CInt(m_nDrawRangeWidth / 2)
        Dim nCoilWid As Integer = CInt(m_nCoilWid / 2 * dPicMapMemWid)


        '����������������擾
        If m_nCoilLen = 0 Then
            nInterval = 1
        Else
            nInterval = CDbl((picMap.Height - PICMAP_HEIGHT_OUT) / m_nCoilLen)
        End If

        '�R�C�����C���f�b�N�X
        e.Graphics.DrawString("C", fntMemori, bshMemori, nPicMapCenterW - HOSEI_Y, 0)
        If m_blnSideTurn = False Then
            m_typDsp.lblWs.Text = "WS"
            m_typDsp.lblDs.Text = "DS"
        Else
            m_typDsp.lblWs.Text = "DS"
            m_typDsp.lblDs.Text = "WS"
        End If

        '�R�C�����_���\��
        '�R�C�������S���`��
        e.Graphics.DrawLine(penMemori_2, nPicMapCenterW, PICMAP_START, nPicMapCenterW, picMap.Height - PICMAP_END)
        For ii = 1 To nCoilWid_Max
            '��200mm���ɓ_���ƃ�������`��
            If ii Mod 200 = 0 Then
                'CENTER��
                e.Graphics.DrawLine(penMemori_2, CInt(nPicMapCenterW - (dPicMapMemWid * ii)), PICMAP_START, CInt(nPicMapCenterW - (dPicMapMemWid * ii)), picMap.Height - PICMAP_END)
                e.Graphics.DrawString(CStr(ii), fntMemori, bshMemori, CInt(nPicMapCenterW - (dPicMapMemWid * ii) - PICMAP_END), HOSEI_Y)

                'CENTER�E
                e.Graphics.DrawLine(penMemori_2, CInt(nPicMapCenterW + (dPicMapMemWid * ii)), PICMAP_START, CInt(nPicMapCenterW + (dPicMapMemWid * ii)), picMap.Height - PICMAP_END)
                e.Graphics.DrawString(CStr(ii), fntMemori, bshMemori, CInt(nPicMapCenterW + (dPicMapMemWid * ii) - PICMAP_END), HOSEI_Y)

            End If
        Next ii

        '�R�C�����_���A�������`��
        Dim nY As Integer = PICMAP_START
        If m_blnUpTurn Then                 '�㉺���]����
            nY = picMap.Height - PICMAP_END - HOSEI_Y * 2
        End If

        e.Graphics.DrawString(CStr(0).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY)
        For ii = 1 To m_nCoilLen
            nY = CInt((nInterval * ii) + PICMAP_START)
            If m_blnUpTurn Then                    '�㉺���]����
                nY = picMap.Height - CInt((nInterval * ii) + PICMAP_END)
            End If

            '�R�C�����ɂ�胁�����`�敝��ύX����
            If m_nCoilLen <= 200000 Then           '�R�C����200m�ȉ��i�������Ԋu20m�j
                '20m���ɉ����A�������`��
                If ii Mod 20000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            ElseIf m_nCoilLen <= 1000000 Then       '�R�C����1000m�ȉ��i�������Ԋu100m�j
                '100m���ɉ����A�������`��
                If ii Mod 100000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            ElseIf m_nCoilLen <= 3000000 Then       '�R�C����3000m�ȉ��i�������Ԋu200m�j
                '200m���ɉ����A�������`��
                If ii Mod 200000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            ElseIf m_nCoilLen <= 6000000 Then       '�R�C����6000m�ȉ��i�������Ԋu500m�j
                '500m���ɉ����A�������`��
                If ii Mod 500000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            ElseIf m_nCoilLen <= 10000000 Then       '�R�C����10000m�ȉ��i�������Ԋu1000m�j
                '1000m���ɉ����A�������`��
                If ii Mod 1000000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            Else                                     '�R�C����10000m�����i�������Ԋu2000m�j
                '2000m���ɉ����A�������`��
                If ii Mod 2000000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            End If

        Next ii

        '�G�b�W���i�ΏۊO�����j�����`��
        '�ΏۊO�Ƃ̋��E���`��i�c���j
        If m_nCoilWid <> 0 And m_nCoilWid <= m_nDrawRangeWidth Then
            e.Graphics.DrawLine(penEdge, CInt(nPicMapCenterW - nCoilWid), PICMAP_START, CInt(nPicMapCenterW - nCoilWid), picMap.Height - PICMAP_END)   '�c���i���ΏۊO���E���j
            e.Graphics.DrawLine(penEdge, CInt(nPicMapCenterW + nCoilWid), PICMAP_START, CInt(nPicMapCenterW + nCoilWid), picMap.Height - PICMAP_END)   '�c���i�E�ΏۊO���E���j
        End If

    End Sub

    ''' <summary>
    ''' �r�}�b�v�ΏۊO�ʒu�\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <param name="typArray"></param>
    ''' <remarks></remarks>
    Public Sub pinMapCutDrw(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs,
                            ByVal typArray As List(Of SAISUN_DATA))

        Dim rect As System.Drawing.Rectangle
        Dim ii As Integer
        Dim picMap As PictureBox = CType(sender, PictureBox)
        Dim nInterval As Double

        Dim dPicMapMemWid As Double = (picMap.Width - PICMAP_WIDTH_OUT) / m_nDrawRangeWidth        ' �}�b�v�`�敝����1����������
        Dim nCoilWid_Max As Integer = CInt(m_nDrawRangeWidth / 2)
        Dim typCut As SAISUN_DATA
        Dim nY As Integer
        Dim nY2 As Integer


        '����������������擾
        If m_nCoilLen = 0 Then
            nInterval = 1
        Else
            nInterval = CDbl((picMap.Height - PICMAP_HEIGHT_OUT) / m_nCoilLen)
        End If

        '�S�̈�h��ׂ��ݒ�i�S�̈�ΏۊO�j
        rect = New System.Drawing.Rectangle(PICMAP_EDGE, PICMAP_START, CInt(picMap.Width - PICMAP_WIDTH_OUT), picMap.Height - PICMAP_HEIGHT_OUT) '�J�n�ʒuX:Y�A���A�����j

        e.Graphics.FillRectangle(Brushes.DarkGray, rect)

        '�ΏۊO�ȊO�̗̈�𔒂œh��ׂ�
        For ii = 0 To typArray.Count - 1

            '�}�[�J�[�p�\���̃f�[�^�擾
            typCut = CType(typArray.Item(ii), SAISUN_DATA)

            '�R�C�����������r�\���p�̃f�[�^�̏ꍇ�A�ΏۊO�̂܂܂Ƃ���B�i�J�n�I���̔��h��ׂ��͍s��Ȃ��j
            'If typCut.cut_flg = 2 Then Continue For

            '��������t�]�̏ꍇ�A�R�C�����������
            If m_blnUpTurn Then
                typCut.Start_Point = m_nCoilLen - typCut.Start_Point
                typCut.End_Point = m_nCoilLen - typCut.End_Point
            End If

            '�ΏۊO�͈͂��`��͈͂𒴂���ꍇ�͕`��͈͓��ɒ���
            If typCut.Start_Point <= 0 Then typCut.Start_Point = 0
            If typCut.Start_Point > m_nCoilLen Then typCut.Start_Point = m_nCoilLen

            '�ΏۊO�͈͂��`��͈͂𒴂���ꍇ�͕`��͈͓��ɒ���
            If typCut.End_Point <= 0 Then typCut.End_Point = 0
            If typCut.End_Point > m_nCoilLen Then typCut.End_Point = m_nCoilLen

            '��������t�]�̏ꍇ�A�h��ׂ��͈͎w����t�]����B
            If m_blnUpTurn Then
                nY = CInt(typCut.End_Point * nInterval)
                nY2 = CInt((typCut.Start_Point - typCut.End_Point) * nInterval)
            Else
                nY = CInt(typCut.Start_Point * nInterval)
                nY2 = CInt((typCut.End_Point - typCut.Start_Point) * nInterval)
            End If

            rect = New System.Drawing.Rectangle(PICMAP_EDGE, PICMAP_START + nY, CInt(picMap.Width - PICMAP_WIDTH_OUT), nY2) '�J�n�ʒuX:Y�A���A�����j
            e.Graphics.FillRectangle(Brushes.White, rect)

        Next ii

    End Sub

    ''' <summary>
    ''' �J�b�g�ʒu�\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <param name="typArray"></param>
    ''' <remarks></remarks>
    Public Sub picMapCut_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs, _
                                    ByVal typArray As List(Of CUT_DATA))

        Dim cutMemori As New System.Drawing.Pen(Color.Red, 3)
        Dim typCut As CUT_DATA
        Dim ii As Integer
        Dim picMap As PictureBox = CType(sender, PictureBox)
        Dim nInterval As Double
        Dim nY As Integer
        Dim dPicMapMemWid As Double = (picMap.Width - PICMAP_WIDTH_OUT) / m_nDrawRangeWidth        ' �}�b�v�`�敝����1����������

        '����������������擾
        If m_nCoilLen = 0 Then
            nInterval = 1
        Else
            nInterval = CDbl((picMap.Height - PICMAP_HEIGHT_OUT) / m_nCoilLen)
        End If

        '�`��R�C�����A�G�b�W�ʒu�擾�inX���W�͈̔͂�`��g�͈͓��ɌŒ�j
        For ii = 0 To typArray.Count - 1

            '�}�[�J�[�p�\���̃f�[�^�擾
            typCut = CType(typArray.Item(ii), CUT_DATA)

            '��������t�]�̏ꍇ�A�R�C�����������
            If m_blnUpTurn Then
                typCut.Cut_Point = m_nCoilLen - typCut.Cut_Point
            End If

            '�J�b�g�ʒu����̏ꍇ�̓J�b�g���`��icut_flg = 1:�R�C����[(����ʒu0), 2:�R�C���I�[�ʒu�ȍ~�̏ꍇ�̓J�b�g���`��͍s��Ȃ��j
            If typCut.cut_flg = 0 Then
                '�J�b�g�����R�C�����͈͊O�̏ꍇ�A�͈͓��ɕ␳
                If 0 > typCut.Cut_Point Then
                    typCut.Cut_Point = 0
                ElseIf typCut.Cut_Point > m_nCoilLen Then
                    typCut.Cut_Point = m_nCoilLen
                End If
                nY = CInt(typCut.Cut_Point * nInterval)             '�J�b�g�ʒu����ʒu�擾
                '�J�b�g���`��
                e.Graphics.DrawLine(cutMemori, PICMAP_EDGE, nY + PICMAP_START, picMap.Width - PICMAP_EDGE, nY + PICMAP_START)
            End If

        Next ii

        '�}�b�v�`��̈�O�g���ĕ`��i�h��ׂ������ŏ㏑�����ꂽ�ꍇ�̑Ή��j
        Dim penMemori As New System.Drawing.Pen(Color.Black, 1)
        e.Graphics.DrawLine(penMemori, PICMAP_EDGE, PICMAP_START, picMap.Width - PICMAP_EDGE, PICMAP_START)    '�㉡�g
        e.Graphics.DrawLine(penMemori, PICMAP_EDGE, picMap.Height - PICMAP_END, picMap.Width - PICMAP_EDGE, picMap.Height - PICMAP_END)   '�����g
        e.Graphics.DrawLine(penMemori, PICMAP_EDGE, PICMAP_START, PICMAP_EDGE, picMap.Height - PICMAP_END)   '�c�g���i���j
        e.Graphics.DrawLine(penMemori, picMap.Width - PICMAP_EDGE, PICMAP_START, picMap.Width - PICMAP_EDGE, picMap.Height - PICMAP_END)   '�c�g���i�E�j

    End Sub

    ''' <summary>
    ''' �r�}�b�v�f�[�^���y�C���g�C�x���g�Ń}�[�N�\��(�T����ʁA���щ�ʋ���)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Sub picMapMarker_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        Dim fntMemori As New System.Drawing.Font("�l�r �S�V�b�N", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        Dim fntSymbol As New System.Drawing.Font("�l�r �S�V�b�N", 10, System.Drawing.FontStyle.Regular, GraphicsUnit.Pixel)

        Dim bshMemori As New SolidBrush(Color.Black)
        Dim strSymbol As String = ""
        Dim ii As Integer
        Dim drwColor As Color
        Dim typMapdata As MAP_DATA
        Dim nCoilWid As Integer
        Dim nCoilStart As Integer
        Dim nX As Integer
        Dim nY As Integer
        Dim picMap As PictureBox = CType(sender, PictureBox)
        Dim nInterval As Double
        Dim dPicMapMemWid As Double = (picMap.Width - PICMAP_WIDTH_OUT) / m_nDrawRangeWidth ' �}�b�v�`�敝����1����������
        Dim nPicMapCenterW As Integer = CInt(picMap.Width / 2)                              ' �}�b�v�`��̈敝���S�ʒu
        Dim dPocMapCenterX As Double

        '����������������擾
        If m_nCoilLen = 0 Then
            nInterval = 1
        Else
            nInterval = CDbl((picMap.Height - PICMAP_HEIGHT_OUT) / m_nCoilLen)
        End If

        '�R�C�����A�G�b�W�ʒu�擾
        nCoilWid = m_nCoilWid
        If nCoilWid <= 0 Or nCoilWid > COIL_WID_MAX Then
            '�R�C������0�A�܂��͍ő�l�𒴂���ꍇ�ő�l���Z�b�g
            nCoilWid = COIL_WID_MAX
        End If
        nCoilStart = CInt(nCoilWid / 2 * dPicMapMemWid)
        dPocMapCenterX = nCoilWid / 2.0R

        '�r���擾��
        If m_nMode = 0 Then
            Call DspWaitInf("�r�}�b�v�\�����i�\�ʁj", m_DefMapData.Count)
        Else
            Call DspWaitInf("�r�}�b�v�\�����i���ʁj", m_DefMapData.Count)
        End If

        For ii = 0 To m_DefMapData.Count - 1
            '�`��F������
            bshMemori.Dispose()

            '�}�[�J�[�p�\���̃f�[�^�擾
            typMapdata = CType(m_DefMapData.Item(ii), MAP_DATA)
            drwColor = tMod.GetIntToColor(mcls_Param.GetMapSymbolColor(typMapdata.g_id))      '�O���[�h�V���{���\���F

            '�V���{���擾
            strSymbol = mcls_Param.GetMapSymbol(typMapdata.t_id)

            '�}�[�J�[�\���F�ݒ�
            bshMemori = New SolidBrush(drwColor)

            '���W�v�Z
            '������
            If m_blnSideTurn Then
                '���E���]���莞

                '�}�b�v�\���͈͊O�̏ꍇ�A�g���Ɏ��߂�
                nX = CInt((nPicMapCenterW + nCoilStart) - (dPocMapCenterX + typMapdata.absX) * dPicMapMemWid)
                If nX < PICMAP_EDGE Then
                    nX = PICMAP_EDGE
                ElseIf nX > picMap.Width - PICMAP_EDGE Then
                    nX = picMap.Width - PICMAP_EDGE
                End If
            Else
                '���E���]������

                '�}�b�v�\���͈͊O�̏ꍇ�A�g���Ɏ��߂�
                nX = CInt((nPicMapCenterW - nCoilStart) + ((dPocMapCenterX + typMapdata.absX) * dPicMapMemWid))
                If nX < PICMAP_EDGE Then
                    nX = PICMAP_EDGE
                ElseIf nX > picMap.Width - PICMAP_EDGE Then
                    nX = picMap.Width - PICMAP_EDGE
                End If
            End If

            '�������
            '��������t�]�̏ꍇ�A�R�C�����������
            If m_blnUpTurn Then typMapdata.y = m_nCoilLen - typMapdata.y
            nY = PICMAP_START + CInt(typMapdata.y * nInterval)
            '�}�b�v�\���͈͊O�̏ꍇ�A�g���Ɏ��߂�
            If nY < PICMAP_START Then
                nY = PICMAP_START
            ElseIf nY > (picMap.Height - PICMAP_END) Then
                nY = picMap.Height - PICMAP_END
            End If

            '�}�[�J�[�`��
            e.Graphics.DrawString(strSymbol, fntSymbol, bshMemori, nX - PICMAP_HOSEI_X, nY - HOSEI_Y)

            '�󋵕\��
            Call DspWaitProg(ii + 1)

        Next ii

    End Sub

    ''' <summary>
    ''' �f�[�^�擾�󋵕\����
    ''' </summary>
    ''' <param name="strVal"></param>
    ''' <param name="iMax"></param>
    ''' <remarks></remarks>
    Public Sub DspWaitInf(ByVal strVal As String, ByVal iMax As Long)
        m_typDsp.lblMsg.Text = strVal
        m_typDsp.prbMsg.Minimum = 0
        m_typDsp.prbMsg.Maximum = CInt(IIf(iMax > m_typDsp.prbMsg.Minimum, iMax, m_typDsp.prbMsg.Minimum + 1))
        m_typDsp.prbMsg.Value = 0

        'Application.DoEvents()�͎g�p�ł��Ȃ��׃��t���b�V���ŕ\��
        m_typDsp.lblMsg.Refresh()
        m_typDsp.prbMsg.Refresh()

    End Sub

    ''' <summary>
    ''' �f�[�^�擾���K�C�_���X �X�V
    ''' </summary>
    ''' <param name="nVal">�i���l</param>
    ''' <remarks></remarks>
    Public Sub DspWaitProg(ByVal nVal As Integer)

        '�i���l�Z�b�g
        m_typDsp.prbMsg.Value = CInt(IIf(nVal < m_typDsp.prbMsg.Maximum, nVal, m_typDsp.prbMsg.Maximum))

    End Sub

End Class
