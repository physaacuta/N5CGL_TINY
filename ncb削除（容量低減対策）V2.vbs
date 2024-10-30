Option Explicit

'==================================
' 対象指定
'==================================
'''' 対象サブフォルダ (例:"SRC")
Dim sSelectFolder(10)
sSelectFolder(0) = "SRC"
sSelectFolder(1) = "LIBRARY"
sSelectFolder(2) = "TOOL"
sSelectFolder(3) = "SRC_LIB"
sSelectFolder(4) = "EPC"



'''' 削除対象拡張子 (例:"*.txt1")
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

'''' 削除フォルダ (例:"x64")
Dim sDeleteFolder(10)
sDeleteFolder(0) = "x64"
sDeleteFolder(1) = "win32"
sDeleteFolder(2) = "ipch"

'==================================
' メイン処理
'==================================
Call FolderSearch


'==================================
' エントリーポイント
'==================================
Sub FolderSearch()
    Dim fs               ' シェルアプリケーションオブジェクト
    Dim objFolder        ' ベースフォルダ
    Dim objFolderItems
    Dim fi
    Dim sWk
    On Error Resume Next
    
    ''' 自フォルダ取得用
    Dim ws
    Set ws = CreateObject("WScript.Shell")


    ''' 指定ファイル削除用
    ' シェルアプリケーションオブジェクト生成
    Set fs = CreateObject("Scripting.FileSystemObject")
    ' 実行場所を基点としたフォルダオブジェクトを生成
    Set objFolder = fs.GetFolder(ws.CurrentDirectory)
    ' 対象のフォルダ構成を取得
    Set objFolderItems = objFolder.SubFolders
    
    ' サブフォルダを検索
    For Each fi In objFolderItems
        For Each sWk In sSelectFolder
	        ' 小文字に変換して比較
            If LCase(sWk) = LCase(fi.Name) Then

                Call FileDelete(fs, fi)

                Call FolderAllDelete(fs, fi)
            End If
        Next
    Next

    msgbox "完了"
End Sub

'==================================
' 再回帰
'==================================
Sub FileDelete(fs, ofi)
    Dim fi
    Dim ii
    Dim sWk
    On Error Resume Next

    '''' サブフォルダがある場合は、再回帰で最終フォルダまで移動
    'If 0 <> ofi.SubFolders.Count Then
        For Each fi In ofi.SubFolders
            ' フォルダの場合は、サブフォルダに移動
            If 16 = fi.Attributes Then
                Call FileDelete(fs, fi)
            End If
        Next
        
    'Else
        '''' ここまできたら、最終階層 なので、ファイル削除
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

        ' フォルダの場合
        If 16 = fi.Attributes Then
            'msgbox (fi.Path)

            For Each sWk In sDeleteFolder
                if LCase(sWk) = LCase(fi.Name) Then
                  'msgbox (fi.Path)
          
                  Call fs.DeleteFolder(fi.Path, True)
                  exit sub
                end if
            Next

            ' ここまで来たらまだ
            Call FolderAllDelete(fs, fi)
        end if
    next 



'    '''' サブフォルダがある場合は、再回帰で最終フォルダまで移動
'    If 0 <> ofi.SubFolders.Count Then
'        For Each fi In ofi.SubFolders
'            ' フォルダの場合は、サブフォルダに移動
'            If 16 = fi.Attributes Then
'                Call FolderAllDelete(fs, fi)
'            End If
'        Next
'        
'    Else
'        '''' ここまできたら、最終階層 なので、ファイル削除
'        msgbox(ofi.Name)
'        if LCase(sWk) = LCase(ofi.Name) Then
'          msgbox (ofi.Path)
'          
'          Call fs.DeleteFolder(ofi.Path, True)
'        End If
'    End If
End Sub