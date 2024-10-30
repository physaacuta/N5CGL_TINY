'*********************************************************************************
'���ʏ����̃��W���[���Q
'	[Ver]
'		Ver.01    2005/12/25  vs2005 �Ή�
'
'	[����]
'
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Runtime.InteropServices          ' DllImport

Public Module GlobalCommon
    ''' <summary>
    ''' Generic�Ή�IIf�B�L���X�g���s�v
    ''' </summary>
    ''' <typeparam name="T">���̌^</typeparam>
    ''' <param name="b">True�False</param>
    ''' <param name="tTrue">True���̒l</param>
    ''' <param name="tFalse">False���̒l</param>
    Public Function IIf(Of T)(ByVal b As Boolean, ByVal tTrue As T, ByVal tFalse As T) As T
        If b Then
            Return tTrue
        Else
            Return tFalse
        End If
    End Function

    ''' <summary>
    ''' ���lv��͈͓�(min~max)�ɂ����߂�B(Saturate)
    ''' </summary>
    Public Sub LimitValue(ByRef v As Integer, ByVal min As Integer, ByVal max As Integer)
        If v > max Then v = max
        If v < min Then v = min
        Return
    End Sub

    ''' <summary>
    ''' ���lv��͈͓�(min~max)�ɂ����߂�B(Saturate)
    ''' </summary>
    Public Sub LimitValue(ByRef v As Double, ByVal min As Double, ByVal max As Double)
        If v > max Then v = max
        If v < min Then v = min
        Return
    End Sub

    ''' <summary>
    ''' ���lv��͈͓�(min~max)�ɂ����߂��l��������
    ''' </summary>
    Public Function Saturate(ByVal v As Integer, ByVal min As Integer, ByVal max As Integer) As Integer
        If v > max Then v = max
        If v < min Then v = min
        Return v
    End Function

    ''' <summary>
    ''' ���lv��͈͓�(min~max)�ɂ����߂��l��������
    ''' </summary>
    Public Function Saturate(ByVal v As Double, ByVal min As Double, ByVal max As Double) As Double
        If v > max Then v = max
        If v < min Then v = min
        Return v
    End Function
End Module

Namespace tMod
    ''' <summary>
    ''' ���ʏ����̃��W���[���Q
    ''' </summary>
    ''' <remarks></remarks>
    Public Module tMod_Common



        '********************************************************************
        '���j���[�o�[ �� �~�{�^������
        '********************************************************************
        ' ���j���[�o�[ �� �~�{�^�����������Ŏg�p
        Private Declare Function GetSystemMenu Lib "user32.dll" Alias "GetSystemMenu" (ByVal hwnd As IntPtr, ByVal bRevert As Boolean) As IntPtr
        Private Declare Function RemoveMenu Lib "user32.dll" Alias "RemoveMenu" (ByVal hMenu As IntPtr, ByVal nPosition As Integer, ByVal wFlags As Integer) As Integer
        Private Const SC_CLOSE As Long = &HF060
        Private Const MF_BYCOMMAND As Long = &H0


        ''' <summary>
        ''' ���j���[�o�[ �� �~�{�^������
        ''' </summary>
        ''' <param name="hForm">�t�H�[���n���h�� (Me.Handle)</param>
        ''' <remarks></remarks>
        Public Sub subFormEndButton_Desebled(ByVal hForm As IntPtr)
            '�E�B���h�E�n���h�����Ƃ��Ƀ��j���[�𖳌���
            Dim ptr As IntPtr
            ptr = GetSystemMenu(hForm, False)
            RemoveMenu(ptr, SC_CLOSE, MF_BYCOMMAND)
        End Sub

        '********************************************************************
        '�R���s���[�^�[�̐ݒ��ύX����
        '********************************************************************
        Private Declare Function SetComputerName Lib "kernel32" Alias "SetComputerNameA" (ByVal strName As String) As Integer
        'Private Declare Function GetComputerName Lib "kernel32" Alias "GetComputerNameA" (ByVal strName As String) As Integer

        ''' <summary>
        ''' �R���s���[�^����ύX����
        ''' </summary>
        ''' <param name="strNewNetBiosComputerName"></param>
        ''' <returns></returns>
        Public Function SetNetBiosComputerName(ByVal strNewNetBiosComputerName As String) As Boolean
            '//// �R���s���[�^�[����ύX����
            '// �� �������ANetBIOS�̖��O�ˁB (�V�X�e���̃v���p�e�B / �R���s���[�^�� / �ύX / �ڍ� �ɏo�ė��閼�O)
            Dim retc As Integer = SetComputerName(strNewNetBiosComputerName)
            Return CBool(IIf(1 = retc, True, False))
        End Function

        ''' <summary>
        ''' �R���s���[�^�[�����擾����
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function GetNetBiosComputerName() As String
            Return System.Environment.MachineName
        End Function


        ''' <summary>
        ''' IP�A�h���X�̕ύX���s��
        ''' </summary>
        ''' <param name="Ip">�ύX���s��IP�A�h���X (127.0.0.1�`��)</param>
        ''' <param name="AdapterName">�ύX�Ώۂ̃A�_�v�^���� (�l�b�g���[�N�ڑ��v���p�e�B�ŕ\������Ă��閼�̂��w��)</param>
        ''' <param name="SubNetMask">�ύX���s���T�u�l�b�g�}�X�N</param>
        Public Sub SetIpAddress(ByVal Ip As String, Optional ByVal AdapterName As String = "���[�J�� �G���A�ڑ�", Optional ByVal SubNetMask As String = "255.255.255.0")
            Dim strCmd As String = ""        ' ��������DOS�R�}���h

            '''' �R�}���h����
            strCmd = "netsh interface ip set "
            strCmd &= "address name = """ & AdapterName & """ "         '�Ώۂ̓z
            strCmd &= "source=static addr=" & Ip & " "
            strCmd &= "mask=" & SubNetMask & ""

            '''' �R�}���h���s
            Shell(strCmd, AppWinStyle.Hide)
        End Sub




        '********************************************************************
        '������֘A
        '********************************************************************
        ''' <summary>
        ''' ������̃o�C�g����Ԃ�
        ''' </summary>
        ''' <param name="strVal">������</param>
        ''' <returns>�o�C�g��</returns>
        ''' <remarks></remarks>
        Public Function LenB(ByVal strVal As String) As Integer
            Return System.Text.Encoding.GetEncoding("shift_jis").GetByteCount(strVal)
        End Function

        ''' <summary>
        ''' �o�C�g�z�񂩂當����ɕϊ�
        ''' </summary>
        ''' <param name="val">�o�C�g�z��</param>
        ''' <returns>������</returns>
        ''' <remarks></remarks>
        Public Function ByteToString(ByVal val() As Byte) As String
            Dim nEnd As Integer = 0
            Dim bJis As Boolean = False

            'For ii As Integer = 0 To val.Length - 1
            '	If 0 = val(ii) Then
            '		Exit For
            '	End If
            '	If val(ii) > &H7F Then bJis = True
            '	nEnd += 1
            'Next

            Dim wk() As Byte
            ReDim wk(val.Length - 1)
            For ii As Integer = 0 To val.Length - 1
                If 0 = val(ii) Then
                    Exit For
                End If
                If val(ii) > &H7F Then bJis = True
                wk(ii) = val(ii)
                nEnd += 1
            Next

            ' ���Ԃ񂱂�Ȃ߂�ǂ��������Ƃ����Ȃ��Ă��A�S��shift-jis�ɂ��Ă����Ȃ��Ǝv�����A�����e���𒲍����Ȃ����߁A�b�菈�u
            If bJis Then
                Return System.Text.Encoding.GetEncoding("shift_jis").GetString(wk)
            Else
                Return System.Text.Encoding.ASCII.GetString(wk)
            End If
        End Function
        ''' <summary>
        ''' �����񂩂�o�C�g�z��ɕϊ�
        ''' </summary>
        ''' <param name="val">������</param>
        ''' <returns>�o�C�g�z��</returns>
        ''' <remarks></remarks>
        Public Function StringToByte(ByVal val As String) As Byte()
            Return System.Text.Encoding.ASCII.GetBytes(val)
        End Function
        ''' <summary>
        ''' ���[���X���b�g���Ŏg�p����Ƃ��ɁA�o�C�g�z��̈�͕ς������Ȃ��ꍇ�Ɏg�p����B
        ''' </summary>
        ''' <param name="ans">�o�C�g�z��</param>
        ''' <param name="val">������</param>
        ''' <param name="size">�T�C�Y</param>
        ''' <remarks></remarks>
        Public Sub StringToByte(ByVal ans As Byte(), ByVal val As String, ByVal size As Integer)
            '�ϊ��T�C�Y����
            Dim iwk As Integer = val.Length
            If iwk > size Then iwk = size
            '�ϊ�
            System.Text.Encoding.ASCII.GetBytes(val, 0, iwk, ans, 0)
        End Sub
        Public Sub StringToByte(ByVal ans As Byte(), ByVal val As String)
            StringToByte(ans, val, ans.Length)
        End Sub

        ''' <summary>
        ''' �n���ꂽ������̔��p�S�p���`�F�b�N
        ''' </summary>
        ''' <param name="val"></param>
        ''' <returns>-1:���낢�덬�� 0:���p�̂� 1:�S�p�̂�</returns>
        Public Function CheckStringType(ByVal val As String) As Integer
            Dim nCntStr As Integer = val.Length
            Dim nCntByte As Integer = System.Text.Encoding.GetEncoding("shift_jis").GetByteCount(val)

            ' �e�T�C�Y�`�F�b�N
            If nCntStr * 2 = nCntByte Then
                Return 1
            ElseIf nCntStr = nCntByte Then
                Return 0
            Else
                Return -1
            End If
        End Function

        '********************************************************************
        '�t�@�C��/�t�H���_�֘A
        '********************************************************************

        ''' <summary>
        ''' �t�H���_���̃R�s�[ (Framework�ł́A��������Ă��Ȃ����玩�삷��)
        ''' </summary>
        ''' <param name="strPathFrom">�R�s�[���p�X (�Ō��\�͖���)</param>
        ''' <param name="strPathTo">�R�s�[��p�X (�Ō��\�͖���)</param>
        ''' <param name="bOverWrite">����t�@�C��������ꍇ�㏑������:true</param>
        ''' <param name="bSubFolder">�T�u�t�H���_������ꍇ�R�s�[����:true</param>
        Public Sub CopyDirectory(ByVal strPathFrom As String, ByVal strPathTo As String, ByVal bOverWrite As Boolean, ByVal bSubFolder As Boolean)
            '''' �R�s�[��̃f�B���N�g�����쐬
            If System.IO.Directory.Exists(strPathTo) = False Then
                IO.Directory.CreateDirectory(strPathTo)
                IO.File.SetAttributes(strPathTo, IO.File.GetAttributes(strPathFrom))
                'bOverWrite = True
            End If

            '''' �R�s�[���̃f�B���N�g���ɂ��邷�ׂẴt�@�C�����R�s�[
            For Each strCopyFrom As String In System.IO.Directory.GetFiles(strPathFrom)
                Dim strCopyTo As String = System.IO.Path.Combine(strPathTo, System.IO.Path.GetFileName(strCopyFrom))

                If bOverWrite Then
                    System.IO.File.Copy(strCopyFrom, strCopyTo, bOverWrite) '�㏑������
                Else
                    If Not IO.File.Exists(strCopyTo) Then
                        System.IO.File.Copy(strCopyFrom, strCopyTo, bOverWrite) '�㏑���s����
                    End If
                End If


            Next strCopyFrom

            '''' �R�s�[���̃f�B���N�g�����ɂ���T�u�t�H���_
            If bSubFolder Then
                For Each strCopyFrom As String In System.IO.Directory.GetDirectories(strPathFrom)
                    Dim stCopyTo As String = System.IO.Path.Combine(strPathTo, System.IO.Path.GetFileName(strCopyFrom))

                    CopyDirectory(strCopyFrom, stCopyTo, bOverWrite, bSubFolder)
                Next strCopyFrom
            End If
        End Sub

        ''' <summary>
        ''' �w��t�H���_�������ꍇ�͐�������
        ''' </summary>
        ''' <param name="strFolder">�`�F�b�N�Ώۂ̃t�H���_�p�X</param>
        ''' <remarks></remarks>
        Public Sub CheckFolder(ByVal strFolder As String)
            '''' �w��t�H���_���L�邩�`�F�b�N
            If IO.Directory.Exists(strFolder) Then Return
            '''' �w��t�H���_����
            Try
                IO.Directory.CreateDirectory(strFolder)
            Catch ex As Exception
                ' �l�b�g���[�N�t�H���_�̏ꍇ�Ȃǂł��������Ȃ����Ă��Ȃ��ꍇ
            End Try
        End Sub


        '********************************************************************
        '�X�̃��W���[��
        '********************************************************************

        ''' <summary>
        ''' ����(RGB)�̐F�R�[�h���� Color�N���X�ɕϊ�
        ''' </summary>
        ''' <param name="intRBG">RGB�`��</param>
        ''' <returns>Color�N���X</returns>
        ''' <remarks></remarks>
        Public Function GetIntToColor(ByVal intRBG As Integer) As System.Drawing.Color
            Return System.Drawing.Color.FromArgb(System.Drawing.Color.FromArgb(intRBG).R, _
                      System.Drawing.Color.FromArgb(intRBG).G, _
                      System.Drawing.Color.FromArgb(intRBG).B)
        End Function

        ''' <summary>
        ''' Color�N���X ���� ����(RGB)�̐F�R�[�h�ɕϊ�
        ''' </summary>
        ''' <param name="clr">Color�N���X</param>
        ''' <returns>RGB�`��</returns>
        ''' <remarks></remarks>
        Public Function GetColorToInt(ByVal clr As System.Drawing.Color) As Integer
            Return System.Drawing.Color.FromArgb(clr.R, clr.G, clr.B).ToArgb
        End Function

        ''' <summary>
        ''' VB6(BGR)�̐F�R�[�h���� Color�N���X�ɕϊ�
        ''' </summary>
        ''' <param name="intGBR">RGB�`��(VB6�̐F�R�[�h)</param>
        ''' <returns>Color�N���X</returns>
        ''' <remarks></remarks>
        Public Function GetIntToColor_VB6(ByVal intGBR As Integer) As System.Drawing.Color
            Return System.Drawing.Color.FromArgb(System.Drawing.Color.FromArgb(intGBR).B, _
             System.Drawing.Color.FromArgb(intGBR).G, _
             System.Drawing.Color.FromArgb(intGBR).R)
        End Function

        ''' <summary>
        ''' Color�N���X ���� VB6(BGR)�̐F�R�[�h�ɕϊ�
        ''' </summary>
        ''' <param name="clr">Color�N���X</param>
        ''' <returns>BGR�`��</returns>
        ''' <remarks></remarks>
        Public Function GetColorToInt_VB6(ByVal clr As System.Drawing.Color) As Integer
            Return System.Drawing.Color.FromArgb(clr.B, clr.G, clr.R).ToArgb
        End Function


        ''' <summary>
        ''' �_���v�o�͊֐�
        ''' </summary>
        ''' <param name="strFileName">�_���v�t�@�C����</param>
        ''' <param name="strFolder">�o�̓t�H���_(�Ō��\Dump�͂���Ȃ�)</param>
        ''' <param name="len">�o�̓f�[�^�T�C�Y</param>
        ''' <param name="data">�f�[�^</param>
        ''' <param name="addTime">�_���v�t�@�C�����ɓ��t��t�^���邵�Ȃ�</param>
        ''' <remarks>�f�[�^�s�łȂ������� �擪�� # �����鎖�B(�r���_���v�t�@�C����p�t�H�[�}�b�g)</remarks>
        Public Sub DumpData(ByVal strFileName As String, ByVal strFolder As String, ByVal len As Integer, ByVal data As Byte(), ByVal addTime As Boolean)
            Dim strPath As String

            '' �t�H���_�L��
            If "" = strFolder Then
                strFolder = ".\Dump"
            Else
                strFolder = strFolder & "\Dump"
            End If
            '' ���O�t�H���_����
            CheckFolder(strFolder)

            '' �t�@�C���I�[�v��
            strFolder &= "\"
            If addTime Then             ' ���t�t�^
                strPath = strFolder & Now.ToString("yyyyMMdd_hhmmss_fff_") & strFileName & ".log"
            Else
                strPath = strFolder & strFileName & ".log"
            End If
            Dim os As New IO.StreamWriter(strPath, False, System.Text.Encoding.GetEncoding("shift-jis"))

            '' �_���v�o��
            Dim wk As String
            Dim ii As Integer
            Dim rec As Integer = 0

            os.WriteLine("# {0}[byte]����M", len)
            os.WriteLine("# (Hex)ADDR(Dec)  :  0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F | [ASCII]")

            While True
                If rec = len Then Exit While ' �Ō�܂œ��B

                os.Write("{0:X7} ({1:D7}) :", rec, rec)

                wk = " | "
                For ii = 0 To 15
                    If ii = 8 Then os.Write(" -")
                    os.Write(" {0:X2}", data(rec))

                    If data(rec) < &H20 Or data(rec) > &H7F Then
                        wk &= "�"
                    Else
                        wk &= Chr(data(rec))
                    End If

                    ' ��
                    rec += 1
                    If rec = len Then   ' �ŏI�f�[�^
                        '�]�����ݒ�
                        For jj As Integer = ii + 1 To 15
                            If jj = 8 Then os.Write(" -")
                            os.Write("   ")
                        Next
                        os.WriteLine(wk)
                        Exit For
                    End If
                    If ii = 15 Then     ' ��Ō�
                        os.WriteLine(wk)
                        Exit For
                    End If
                Next ii
            End While
            os.WriteLine()
            os.Flush()
            os.Close()
            os.Dispose()
        End Sub


        ''' <summary>
        ''' �w�胂�j�^�[No����̐�Έʒu
        ''' </summary>
        ''' <param name="nMonitorNo">���j�^�ԍ� (1�I���W��)</param>
        ''' <returns>��Έʒu [pixel]</returns>
        ''' <remarks></remarks>
        Public Function ScreenToLocation(ByVal nMonitorNo As Integer) As System.Drawing.Point

            '' �\�����������j�^�̏��ɂ��킹�ăt�H�[���T�C�Y��ύX
            Dim monitor() As System.Windows.Forms.Screen
            monitor = System.Windows.Forms.Screen.AllScreens()

            nMonitorNo -= 1 '�Ȃ����̂���1�I���W���ɂȂ��Ă����ׁA�݊����ׂ̈ɂ��̂܂܂Ƃ���

            '' �ԍ��`�F�b�N
            If 0 > nMonitorNo Or nMonitorNo >= monitor.Length Then
                nMonitorNo = 0
            End If

            Return monitor(nMonitorNo).Bounds.Location
        End Function

        '********************************************************************
        ' C++�̎g���₷�����W���[���̓W�J
        '********************************************************************
        ''' <summary>
        ''' ���חD��x��r Ex (�e�ϐ��z��ɏ����Z�b�g)
        ''' </summary>
        ''' <param name="pNow">��r����V�������ԂɃZ�b�g(�z��̐擪���ŗD��) </param>
        ''' <param name="pPre">��r���鋌�������ԂɃZ�b�g(�z��̐擪���ŗD��) </param>
        ''' <param name="bMode">��r���� (true:�V����r�����Ƃ��ɑ傫�����������B false:�V����r�����Ƃ��ɏ�������������)</param>
        ''' <returns>true:�V���ׂ̕�������  false:�����ׂ̕�������</returns>
        Public Function SelectPriorityDetect(ByVal pNow() As Double, ByVal pPre() As Double, ByVal bMode() As Boolean) As Boolean
            For ii As Integer = 0 To pNow.Length - 1
                If pNow(ii) = pPre(ii) Then Continue For

                If bMode(ii) Then
                    '// �傫����������
                    If pNow(ii) > pPre(ii) Then
                        Return True
                    Else
                        Return False
                    End If
                Else
                    '// ��������������
                    If pNow(ii) < pPre(ii) Then
                        Return True
                    Else
                        Return False
                    End If
                End If
            Next
            Return False    '// �S���ꏏ�Ȃ� �����ׂ�D��
        End Function


        ''' <summary>
        ''' nVal�T�C�Y��nPitch�̔{���ɑ�����(�K���AnVal�̂܂܂��A������Ƒ傫���Ȃ����)
        ''' </summary>
        ''' <param name="nVal">���T�C�Y</param>
        ''' <param name="nPitch">�{��</param>
        ''' <returns>����</returns>
        Public Function GET_SIZE_JUST(ByVal nVal As Integer, ByVal nPitch As Integer) As Integer
            Return ((nVal - 1) \ nPitch + 1) * nPitch
        End Function
        ''' <summary>
        ''' ����؂菈��
        ''' </summary>
        ''' <param name="nVal">�l</param>
        ''' <param name="nPitch">�s�b�`</param>
        ''' <returns>5,5 �̏ꍇ�́A0�ƂȂ�</returns>
        Public Function JUST_SIZE(ByVal nVal As Integer, ByVal nPitch As Integer) As Integer
            Return (nVal - 1) \ nPitch
        End Function

    End Module
End Namespace