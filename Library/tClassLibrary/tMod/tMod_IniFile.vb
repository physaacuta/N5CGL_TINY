'*********************************************************************************
'ini�t�@�C���̓ǂݏ������s����{���W���[���Q
'	[Ver]
'		Ver.01    2005/12/24  vs2005 �Ή�
'
'	[����]
'
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Runtime.InteropServices          ' DllImport
Imports System

Namespace tMod
    ''' <summary>
    ''' ini�t�@�C���̓ǂݏ������s����{���W���[���Q
    ''' </summary>
    ''' <remarks></remarks>
    Public Module tMod_IniFile
        '********************************************************************
        'API��`
        '********************************************************************
        Friend Class SafeNativeMethods
            ''' <summary>
            ''' [API] ini�t�@�C�����當����ǂݍ���
            ''' </summary>
            <DllImport("KERNEL32.DLL", EntryPoint:="GetPrivateProfileStringA", SetLastError:=True, ExactSpelling:=True, CharSet:=CharSet.Ansi)> _
            Friend Shared Function GetPrivateProfileString( _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpApplicationName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpKeyName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpDefault As String, _
                <Out(), MarshalAs(UnmanagedType.LPStr)> ByVal lpReturnedString As Text.StringBuilder, _
                ByVal nSize As Integer, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
            As Integer
            End Function

            ''' <summary>
            ''' INI�t�@�C������A�Z�N�V�����P�ʂœǂ݂���API
            ''' </summary>
            ''' <returns>�������񂾕�����</returns>
            ''' <remarks> null-separation(double-null-terminate)�Ȃ̂ŁAreturnstring��LPSTR�Ń}�[�V�������O�ł��Ȃ�</remarks>
            <DllImport("KERNEL32.DLL", SetLastError:=True, ExactSpelling:=True, CharSet:=CharSet.Ansi)> _
            Friend Shared Function GetPrivateProfileSectionA( _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpApplicationName As String, _
                ByVal lpReturnedString As IntPtr, _
                ByVal nSize As Integer, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
                As Integer
            End Function

            ''' <summary>
            ''' [API] ini�t�@�C�����琔�l�ǂݍ���
            ''' </summary>
            <DllImport("KERNEL32.DLL", EntryPoint:="GetPrivateProfileInt", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
            Friend Shared Function GetPrivateProfileInt( _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpAppName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpKeyName As String, _
                ByVal nDefault As Integer, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
                As Integer
            End Function

            ''' <summary>
            ''' [API] ini�t�@�C���ɕ����񏑂�����
            ''' </summary>
            <DllImport("KERNEL32.DLL", EntryPoint:="WritePrivateProfileStringA", SetLastError:=True, ExactSpelling:=True, CharSet:=CharSet.Ansi)> _
            Friend Shared Function WritePrivateProfileString( _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpAppName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpKeyName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpString As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
                As Boolean
            End Function
        End Class


        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' ini�t�@�C������ ������ ��ǂݍ���
        ''' </summary>
        ''' <param name="strSection">�Z�N�V����</param>
        ''' <param name="strKey">�L�[</param>
        ''' <param name="strFileName">ini�t�@�C���̃t���p�X</param>
        ''' <param name="strDef">�����l(���s�����ꍇ�̖߂�l)</param>
        ''' <param name="intSize">�ǂݍ��ލő啶����</param>
        ''' <returns>�擾�����l</returns>
        ''' <remarks></remarks>
        Public Function ReadIniStr(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String, Optional ByVal strDef As String = "", Optional ByVal intSize As Integer = 1024) As String
            Dim sbBuff As New Text.StringBuilder(intSize)
            '''' �l�擾
            Call SafeNativeMethods.GetPrivateProfileString(strSection, strKey, strDef, sbBuff, sbBuff.Capacity, strFileName)
            Return sbBuff.ToString
        End Function

        ''' <summary>
        ''' ini�t�@�C������ ���l ��ǂݍ���
        ''' </summary>
        ''' <param name="strSection">�Z�N�V����</param>
        ''' <param name="strKey">�L�[</param>
        ''' <param name="strFileName">ini�t�@�C���̃t���p�X</param>
        ''' <param name="intDef">�����l(���s�����ꍇ�̖߂�l)</param>
        ''' <returns>�擾�����l</returns>
        ''' <remarks></remarks>
        Public Function ReadIniInt(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String, Optional ByVal intDef As Integer = 0) As Integer
            '''' �l�擾
            Return SafeNativeMethods.GetPrivateProfileInt(strSection, strKey, intDef, strFileName)
        End Function


        Private Function ReadIniSecionApi(ByVal strsection As String, ByVal strfilename As String) As String
            '�ǂݍ��݃o�b�t�@�T�C�Y��i�X�����������Ă����B
            For Each intsize As Integer In New Integer() {256, 64 * 1024, 256 * 1024, 1024 * 1024}
                Dim len As Integer
                Dim memory(intsize - 1) As Byte
                Using pp As New PinnedPtr(memory)
                    len = SafeNativeMethods.GetPrivateProfileSectionA(strsection, pp.Ptr, intsize, strfilename)
                End Using

                If len = 0 Then
                    Return String.Empty
                End If

                'If the buffer is not large enough..., the return value is equal to nSize minus two.
                If len < intsize - 2 Then
                    Dim encoding As System.Text.Encoding = System.Text.Encoding.Default
                    Return encoding.GetString(memory, 0, len - 1)
                End If
            Next

            '��������INI��1MB�𒴂���̂͂Ȃ����낤����
            '��O���������ق������������B
            Return String.Empty
        End Function


        ''' <summary>
        ''' INI�t�@�C���̃Z�N�V�������܂邲�ƕ�����z��ł������B
        ''' </summary>
        ''' <returns> "key=value"�`�̕�����z��</returns>
        ''' <remarks> �O���TRIM��API������Ă����B</remarks>
        Public Function ReadIniSecion(ByVal strsection As String, ByVal strfilename As String) As String()
            Dim rstr As String = ReadIniSecionApi(strsection, strfilename)
            If rstr.Length < 2 Then
                Return New String() {}
            End If

            Return rstr.Split(vbNullChar(0))
        End Function

        ''' <summary>
        ''' INI�t�@�C���̃Z�N�V�������܂邲��Dictionary�ł������B
        ''' </summary>
        ''' <returns>Key�̑召��������ʂ��Ȃ��ADictionary</returns>
        Public Function ReadIniSecionAsDictionary(ByVal strsection As String, ByVal strfilename As String) As Dictionary(Of String, String)
            Dim list() As String = ReadIniSecion(strsection, strfilename)
            Dim rv As New Dictionary(Of String, String)(StringComparer.InvariantCultureIgnoreCase)
            For Each Str As String In list
                Dim eqpos As Integer = Str.IndexOf("="c)
                If eqpos >= 0 Then
                    'API�ŁA�O��̃X�y�[�X�͏������Ă���Ă�B
                    Dim key As String = Str.Substring(0, eqpos)
                    Dim value As String = Str.Substring(eqpos + 1)
                    rv.Add(key, value)
                End If
            Next
            Return rv
        End Function



        ''' <summary>
        ''' ini�t�@�C���� �l �ɕ����� ��������
        ''' </summary>
        ''' <param name="strSection">�Z�N�V����</param>
        ''' <param name="strKey">�L�[(�Y���L�[��������΃L�[���ǉ�)</param>
        ''' <param name="strFileName">ini�t�@�C���̃t���p�X</param>
        ''' <param name="strVal">�������ޒl</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function WriteIni(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String, ByVal strVal As String) As Boolean
            Try
                '''' �l��������
                Return SafeNativeMethods.WritePrivateProfileString(strSection, strKey, strVal, strFileName)
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' ini�t�@�C������w�肵���L�[���폜����
        ''' </summary>
        ''' <param name="strSection">�Z�N�V����</param>
        ''' <param name="strKey">�폜����L�[</param>
        ''' <param name="strFileName">ini�t�@�C���̃t���p�X</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function DeleteKeyIni(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String) As Boolean
            return WriteIni(strSection, strKey, strFileName, Nothing)
        End Function

        ''' <summary>
        ''' ini�t�@�C������w�肵���Z�N�V�������폜����
        ''' </summary>
        ''' <param name="strSection">�폜����Z�N�V����</param>
        ''' <param name="strFileName">ini�t�@�C���̃t���p�X</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function DeleteSectionIni(ByVal strSection As String, ByVal strFileName As String) As Boolean
            return WriteIni(strSection, Nothing, strFileName, Nothing)
        End Function
    End Module
End Namespace
