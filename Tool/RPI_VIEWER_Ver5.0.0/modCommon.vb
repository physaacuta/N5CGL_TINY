
Module modCommon

    ' ===>> �⊪ �@���ێ� �c�[�����p����
    Public Const MC_CHECK_ACCSESS As Boolean = True	' �F�؏����L���iTRUE:�F�؏�������,FALSE:�F�؏����Ȃ��j
    ' <<===

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���d�v
    Public g_nLockMode As Integer = 0			' �g���@�\���� (0:�\���֎~ 1:�T���v�����J 9:�S���J)
    Public g_bOutTrk As Boolean					' �O���A���L�� (false:�]��=�P�Ɠ���  true:�A������ )
    Public g_typStartArgs As START_ARGS			' �N������

    Public gcls_Log As tClassLibrary.tClass.tClass_LogManager = Nothing							' ���O�N���X


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔 �\���̒�`
    Public Const TASK_NAME As String = "RPI_VIEWER"
    Public Const TASKINI_NAME As String = ".\RPI_VIEWER.ini"
    Public Const LOCALINI_NAME As String = ".\RPI_VIEWER_LOCAL.ini"
    Public Const MC_MAX_IMG As Integer = 4
    Public Const MC_RANP_CNT As Integer = 9			' �r�ڋ߃����v��
    ' ===>> �⊪ �@���ێ� �c�[�����p����
    Public Const MC_LINE_CNT As Integer = 100       ' �ő僉�C�����i���[�v�Ŏg�p�j
    ' <<===


    ''' <summary>
    ''' �ʔ���
    ''' </summary>
    Public Enum EM_DIV_SPEED
        'emDivSpeed_Stop = 0			' ��~
        emDivSpeed_Up = 1			' ���]
        emDivSpeed_Down = -1		' �t�]
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔 �\���̒�`
    ''' <summary>
    ''' �����Ώ�Key���
    ''' </summary>
    Public Structure SELECT_KEY
        Dim sKizukenNo As String				' �r���Ǘ�No
        Dim nMen As Integer						' ���݂̕\�����
    End Structure

    ''' <summary>
    ''' �R�C�����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DATA_COIL
        Dim sKizukenNo As String				' �r���Ǘ�No
        Dim sCoilNo As String		' �R�C��No
        Dim nLen As Integer			' �r��������[mm]
        Dim nWid As Integer			' �R�C���� [mm]
    End Structure
    ''' <summary>
    ''' �X�̃J�b�g �\����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DATA_CUT_INF
        ''''' ���]�l
        ''' <summary>����ʒu[mm]</summary>
        Dim nY As Integer

        ''''' ���]�O
        Dim nBaseY As Integer
    End Structure

    ''' <summary>
    ''' �X���r��� �\����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DATA_DETECT_INF
        ''' <summary>�\���L��</summary>
        Dim bDsp As Boolean


        ''' <summary>�rNo</summary>
        Dim nNo As Integer
        ''' <summary>�r��EdasysID</summary>
        Dim tId As Integer
        ''' <summary>�O���[�hEdasysID</summary>
        Dim gId As Integer
        ''' <summary>�r��D��x(1�ŋ�)</summary>
        Dim tYu As Integer
        ''' <summary>�O���[�h�D��x(1�ŋ�)</summary>
        Dim gYu As Integer

        ''' <summary>�J�����ԍ�(���[�J������0�Ƃ��ĉ��Ԗ�)</summary>
        Dim nCamPos As Integer
        ''' <summary>�t���[��No(�R�C���擪��1) (1�I���W��)</summary>
        Dim nFrameNo As Integer
        ''' <summary>�t���[��No(�R�C���擪��1) (1�I���W��) �̕␳�O�̒l</summary>
        Dim nFrameNoBase As Integer


        ''' <summary>�t���[��No(�I���W�i��)</summary>
        Dim nFrameNum As Integer

        ''' <summary>�t���[���摜���㌴�_�̊O�ڒ����`</summary>
        Dim nFrameXmin As Integer
        Dim nFrameXmax As Integer
        Dim nFrameYmin As Integer
        Dim nFrameYmax As Integer
        Dim nFrameXc As Integer		' C�(DS:-, WS:+)(�U�z�}�`��p)[mm]
        Dim nFrameBaseXc As Integer	' ���]�O

        ''' <summary>�؂�o���摜���㌴�_�̊O�ڒ����`</summary>
        Dim nKiriXmin As Integer
        Dim nKiriXmax As Integer
        Dim nKiriYmin As Integer
        Dim nKiriYmax As Integer


        ''''' ���]�l
        ''' <summary>����ʒu[mm]</summary>
        Dim nY As Integer
        ''' <summary>DS���ʒu[mm]</summary>
        Dim nX As Integer
        ''' <summary>WS���ʒu[mm]</summary>
        Dim nXws As Integer
        ''' <summary>C����ʒu[mm](DS:-, WS:+)</summary>
        Dim nXc As Integer

        ''''' ���]�O
        Dim nBaseY As Integer
        Dim nBaseX As Integer
        Dim nBaseXws As Integer
        Dim nBaseXc As Integer

    End Structure

    ''' <summary>
    ''' �؂�o���摜��� �\����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DETECT_KIRI_DATA
        ''' <summary>��{���</summary>
        Dim data As DATA_DETECT_INF

        ''' <summary>�C���[�W</summary>
        Dim imgAngle() As Bitmap
        Dim nImgSize() As Integer				' �摜�T�C�Y
        Dim nImgCnt As Integer					' �摜����


        Dim dResX As Double						' ����\
        Dim dResY As Double						' 
        Dim nImgCenterX As Integer				' �r���S�ʒu
        Dim nImgCenterY As Integer
        Dim nImgSizeX As Integer				' �摜�T�C�Y
        Dim nImgSizeY As Integer


        ' ������
        Public Sub initialize()
            ReDim imgAngle(MC_MAX_IMG - 1)
            ReDim nImgSize(MC_MAX_IMG - 1)
        End Sub
    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���������p
    Public Structure TYP_ITEM
        Dim sString As String
        Dim nKey As Integer

        Public Overrides Function ToString() As String
            Return String.Format("{0}	:{1}", nKey, sString)
        End Function
    End Structure

    ''' <summary>
    ''' �T�u��ʂ̈ʒu�����C���t�H�[���̒�����
    ''' </summary>
    ''' <param name="myfrm"></param>
    ''' <param name="own"></param>
    ''' <remarks></remarks>
    Public Sub SubFormLocation(ByVal myfrm As Form, ByVal own As Control)
        If own Is Nothing Then Return
        'myfrm.Location = New Point(own.Location.X + (own.Width - myfrm.Width) \ 2, _
        'own.Location.Y + (own.Height - myfrm.Height) \ 2)
        myfrm.Location = New Point(own.Location.X + 10, _
         own.Location.Y + 20)


        '''' �{�T�C�Y�ύX 4.5.3����
        Dim sWk As String = tClassLibrary.tMod.ReadIniStr("FORM_SIZE", myfrm.Name, TASKINI_NAME)
        If 0 <> sWk.Length Then
            Dim sSize() As String = sWk.Split(CChar(","))
            Dim nSizeW As Integer = CInt(sSize(0))
            Dim nSizeH As Integer = CInt(sSize(1))

            ' ���T�C�Y���擾
            Dim size As Size = myfrm.Size
            ' ini�t�@�C�����𔽉f
            size.Width = CInt(IIf(0 < nSizeW, nSizeW, size.Width))
            size.Height = CInt(IIf(0 < nSizeH, nSizeH, size.Height))
            ' �ăZ�b�g
            myfrm.Size = size
        End If
    End Sub
    ''' <summary>
    ''' �r���Ǘ�No����t�H���_�����擾����
    ''' </summary>
    ''' <param name="sKizukenNo">�r���Ǘ�No������</param>
    ''' <returns>�t�H���_��</returns>
    ''' <remarks></remarks>
    Public Function GetFolderName(ByRef sKizukenNo As String) As String
        Dim sFileName As String = sKizukenNo.Split(" "c)(0)
        ' �擪�����ڂ̃X�y�[�X�܂ł̕�������"�r��ID+���t������iYYYYMMDD�j"��10�����ł�����r��ID���̕�����2���������O����
        If sFileName.Length = 10 Then sFileName = sFileName.Remove(0, 2)
        Return sFileName
    End Function

    Public ReadOnly m_reParen As New System.Text.RegularExpressions.Regex("\{[^0-9].*?\}")
 
End Module
