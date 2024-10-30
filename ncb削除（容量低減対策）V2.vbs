Option Explicit

'==================================
' �Ώێw��
'==================================
'''' �ΏۃT�u�t�H���_ (��:"SRC")
Dim sSelectFolder(10)
sSelectFolder(0) = "SRC"
sSelectFolder(1) = "LIBRARY"
sSelectFolder(2) = "TOOL"
sSelectFolder(3) = "SRC_LIB"
sSelectFolder(4) = "EPC"



'''' �폜�Ώۊg���q (��:"*.txt1")
Dim sDelType(20)
sDelType(0) = "*.ncb"
sDelType(1) = "*.obj"
sDelType(2) = "*.lib"
sDelType(3) = "*.pch"
sDelType(4) = "*.ilk"
sDelType(5) = "*.sdf"
sDelType(6) = "*.pdb"
sDelType(7) = "*.ipch"
sDelType(8) = "*.db"
sDelType(9) = "*.ipdb"
sDelType(10) = "*.iobj"

'''' �폜�t�H���_ (��:"x64")
Dim sDeleteFolder(10)
sDeleteFolder(0) = "x64"
sDeleteFolder(1) = "win32"
sDeleteFolder(2) = "ipch"

'==================================
' ���C������
'==================================
Call FolderSearch


'==================================
' �G���g���[�|�C���g
'==================================
Sub FolderSearch()
    Dim fs               ' �V�F���A�v���P�[�V�����I�u�W�F�N�g
    Dim objFolder        ' �x�[�X�t�H���_
    Dim objFolderItems
    Dim fi
    Dim sWk
    On Error Resume Next
    
    ''' ���t�H���_�擾�p
    Dim ws
    Set ws = CreateObject("WScript.Shell")


    ''' �w��t�@�C���폜�p
    ' �V�F���A�v���P�[�V�����I�u�W�F�N�g����
    Set fs = CreateObject("Scripting.FileSystemObject")
    ' ���s�ꏊ����_�Ƃ����t�H���_�I�u�W�F�N�g�𐶐�
    Set objFolder = fs.GetFolder(ws.CurrentDirectory)
    ' �Ώۂ̃t�H���_�\�����擾
    Set objFolderItems = objFolder.SubFolders
    
    ' �T�u�t�H���_������
    For Each fi In objFolderItems
        For Each sWk In sSelectFolder
	        ' �������ɕϊ����Ĕ�r
            If LCase(sWk) = LCase(fi.Name) Then

                Call FileDelete(fs, fi)

                Call FolderAllDelete(fs, fi)
            End If
        Next
    Next

    msgbox "����"
End Sub

'==================================
' �ĉ�A
'==================================
Sub FileDelete(fs, ofi)
    Dim fi
    Dim ii
    Dim sWk
    On Error Resume Next

    '''' �T�u�t�H���_������ꍇ�́A�ĉ�A�ōŏI�t�H���_�܂ňړ�
    'If 0 <> ofi.SubFolders.Count Then
        For Each fi In ofi.SubFolders
            ' �t�H���_�̏ꍇ�́A�T�u�t�H���_�Ɉړ�
            If 16 = fi.Attributes Then
                Call FileDelete(fs, fi)
            End If
        Next
        
    'Else
        '''' �����܂ł�����A�ŏI�K�w �Ȃ̂ŁA�t�@�C���폜
        For Each sWk In sDelType
            Call fs.DeleteFile(ofi.Path & "\" & sWk, True)
        Next
    'End If
End Sub


Sub FolderAllDelete(fs, ofi)
    Dim fi
    Dim ii
    Dim sWk
    On Error Resume Next

    For Each fi In ofi.SubFolders
        'msgbox(fi.Name)

        ' �t�H���_�̏ꍇ
        If 16 = fi.Attributes Then
            'msgbox (fi.Path)

            For Each sWk In sDeleteFolder
                if LCase(sWk) = LCase(fi.Name) Then
                  'msgbox (fi.Path)
          
                  Call fs.DeleteFolder(fi.Path, True)
                  exit sub
                end if
            Next

            ' �����܂ŗ�����܂�
            Call FolderAllDelete(fs, fi)
        end if
    next 



'    '''' �T�u�t�H���_������ꍇ�́A�ĉ�A�ōŏI�t�H���_�܂ňړ�
'    If 0 <> ofi.SubFolders.Count Then
'        For Each fi In ofi.SubFolders
'            ' �t�H���_�̏ꍇ�́A�T�u�t�H���_�Ɉړ�
'            If 16 = fi.Attributes Then
'                Call FolderAllDelete(fs, fi)
'            End If
'        Next
'        
'    Else
'        '''' �����܂ł�����A�ŏI�K�w �Ȃ̂ŁA�t�@�C���폜
'        msgbox(ofi.Name)
'        if LCase(sWk) = LCase(ofi.Name) Then
'          msgbox (ofi.Path)
'          
'          Call fs.DeleteFolder(ofi.Path, True)
'        End If
'    End If
End Sub