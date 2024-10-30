Imports System.Runtime.InteropServices
Imports System.Runtime.InteropServices.ComTypes

Namespace SHAPI

    Public Enum SIGDN
        NormalDisplay = 0
        ParentRelativeParsing = &H80018001
        DesktopAbosoluteParsing = &H80028000
        ParentRelativeEditing = &H80031001
        DesktopAbosoluteEditing = &H8004C000
        FileSysPath = &H80058000
        URL = &H80068000
        ParentRelativeForAddressbar = &H8007C001
        ParentRelative = &H80080001
    End Enum

    <ComImport()> _
    <Guid("43826d1e-e718-42ee-bc55-a1e261c37bfe")> _
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface IShellItem
        Function BindToHandler(ByVal pbc As IBindCtx, <MarshalAs(UnmanagedType.LPStruct)> ByVal bhid As Guid, <MarshalAs(UnmanagedType.LPStruct)> ByVal riid As Guid) As <MarshalAs(UnmanagedType.[Interface])> Object

        Function GetParent() As IShellItem

        Function GetDisplayName(ByVal sigdnName As SIGDN) As <MarshalAs(UnmanagedType.LPWStr)> String

        Function GetAttributes(ByVal sfgaoMask As Integer) As Integer

        Function Compare(ByVal psi As IShellItem, ByVal hint As Integer) As Integer
    End Interface


    <Flags()> _
    Public Enum SFGAO
        CANCOPY = 1 '// Objects can be copied    (0x1)
        CANMOVE = 2 '// Objects can be moved     (0x2)
        CANLINK = 4 '// Objects can be linked    (0x4)
        STORAGE = &H8 '     // supports BindToObject(IID_IStorage)
        CANRENAME = &H10 '     // Objects can be renamed
        CANDELETE = &H20 '     // Objects can be deleted
        HASPROPSHEET = &H40 '     // Objects have Property sheets
        DROPTARGET = &H100 '     // Objects are drop target
        CAPABILITYMASK = &H177 '
        ENCRYPTED = &H2000 '     // Object Is encrypted (use alt color)
        ISSLOW = &H4000 '     // 'slow' object
        GHOSTED = &H8000 '     // ghosted icon
        LINK = &H10000 '     // Shortcut (link)
        SHARE = &H20000 '     // Shared
        READ_ONLY = &H40000 '     // read-only
        HIDDEN = &H80000 '     // hidden Object
        DISPLAYATTRMASK = &HFC000 '
        FILESYSANCESTOR = &H10000000 '     // may contain children With SFGAO_FILESYSTEM
        FOLDER = &H20000000 '     // support BindToObject(IID_IShellFolder)
        FILESYSTEM = &H40000000 '     // Is a win32 file system Object (file/folder/root)
        HASSUBFOLDER = &H80000000 '     // may contain children With SFGAO_FOLDER
        CONTENTSMASK = &H80000000 '
        VALIDATE = &H1000000 '     // invalidate cached information
        REMOVABLE = &H2000000 '     // Is this removeable media?
        COMPRESSED = &H4000000 '     // Object Is compressed (use alt color)
        BROWSABLE = &H8000000 '     // supports IShellFolder, but only Implements CreateViewObject() (non-folder view)
        NONENUMERATED = &H100000 '     // Is a non-enumerated Object
        NEWCONTENT = &H200000 '     // should show bold In explorer tree
        CANMONIKER = &H400000 '     // defunct
        HASSTORAGE = &H400000 '     // defunct
        STREAM = &H400000 '     // supports BindToObject(IID_IStream)
        STORAGEANCESTOR = &H800000 '     // may contain children With SFGAO_STORAGE Or SFGAO_STREAM
        STORAGECAPMASK = &H70C50008 '     // For determining storage capabilities, ie For open/save semantics
    End Enum


    <Flags()> _
    Public Enum FileOperationFlags

        MULTIDESTFILES = &H1
        CONFIRMMOUSE = &H2
        SILENT = &H4  ' don't display progress UI (confirm prompts may be displayed still)
        RENAMEONCOLLISION = &H8  ' automatically rename the source files To avoid the collisions
        NOCONFIRMATION = &H10  ' don't display confirmation UI, assume "yes" for cases that can be bypassed, "no" for those that can not
        WANTMAPPINGHANDLE = &H20  ' Fill In SHFILEOPSTRUCT.hNameMappings
        ALLOWUNDO = &H40  ' enable undo including Recycle behavior For IFileOperation: Delete()
        FILESONLY = &H80  ' only operate On the files (non folders), both files And folders are assumed without this
        SIMPLEPROGRESS = &H100  ' means don't show names of files
        NOCONFIRMMKDIR = &H200  ' don't dispplay confirmatino UI before making any needed directories, assume "Yes" in these cases
        NOERRORUI = &H400  ' don't put up error UI, other UI may be displayed, progress, confirmations
        NOCOPYSECURITYATTRIBS = &H800  ' dont copy file security attributes (ACLs)
        NORECURSION = &H1000  ' don't recurse into directories for operations that would recurse
        NO_CONNECTED_ELEMENTS = &H2000  ' don't operate on connected elements ("xxx_files" folders that go with .htm files)
        WANTNUKEWARNING = &H4000  ' during delete operation, warn If nuking instead Of recycling (partially Overrides FOF_NOCONFIRMATION)
        NORECURSEREPARSE = &H8000  ' deprecated; the operations engine always does the right thing On FolderLink objects (symlinks, reparse points, folder shortcuts)
        NO_UI = (SILENT + NOCONFIRMATION + NOERRORUI + NOCONFIRMMKDIR) ' don't display any UI at all

        X_NOSKIPJUNCTIONS = &H10000  ' Don't avoid binding to junctions (like Task folder, Recycle-Bin)
        X_PREFERHARDLINK = &H20000  ' Create hard link If possible
        X_SHOWELEVATIONPROMPT = &H40000  ' Show elevation prompts When Error UI Is disabled (use With FOF_NOERRORUI)
        X_EARLYFAILURE = &H100000  ' Fail operation As soon As a Single Error occurs rather than trying To process other items (applies only When Using FOF_NOERRORUI)
        X_PRESERVEFILEEXTENSIONS = &H200000  ' Rename collisions preserve file extns (use With FOF_RENAMEONCOLLISION)
        X_KEEPNEWERFILE = &H400000  ' Keep newer file On naming conflicts
        X_NOCOPYHOOKS = &H800000  ' Don't use copy hooks
        X_NOMINIMIZEBOX = &H1000000  ' Don't allow minimizing the progress dialog
        X_MOVEACLSACROSSVOLUMES = &H2000000  ' Copy security information When performing a cross-volume move operation
        X_DONTDISPLAYSOURCEPATH = &H4000000  ' Don't display the path of source file in progress dialog
        X_DONTDISPLAYDESTPATH = &H8000000  ' Don't display the path of destination file in progress dialog
        X_REQUIREELEVATION = &H10000000  ' User expects the elevation; don't show a dialog to confirm
        X_COPYASDOWNLOAD = &H40000000  ' Show Downloading instead Of Copying
        X_DONTDISPLAYLOCATIONS = &H80000000  ' Hides the locations line In the progress dialog

    End Enum

    <Guid("04b0f1a7-9490-44bc-96e1-4296a31252e2")> _
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface IFileOperationProgressSink
        Sub StartOperations()
        Sub FinishOperations(ByVal hrResult As Integer)

        Sub PreRenameItem(ByVal dwFlags As Integer, ByVal psiItem As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String)
        Sub PostRenameItem(ByVal dwFlags As Integer, ByVal psiItem As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String, ByVal hrRename As Integer, ByVal psiNewlyCreated As IShellItem)

        Sub PreMoveItem(ByVal dwFlags As Integer, ByVal psiItem As IShellItem, ByVal psiDestinationFolder As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String)
        Sub PostMoveItem(ByVal dwFlags As Integer, ByVal psiItem As IShellItem, ByVal psiDestinationFolder As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String, ByVal hrMove As Integer, ByVal psiNewlyCreated As IShellItem)

        Sub PreCopyItem(ByVal dwFlags As Integer, ByVal psiItem As IShellItem, ByVal psiDestinationFolder As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String)
        Sub PostCopyItem(ByVal dwFlags As Integer, ByVal psiItem As IShellItem, ByVal psiDestinationFolder As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String, ByVal hrCopy As Integer, ByVal psiNewlyCreated As IShellItem)

        Sub PreDeleteItem(ByVal dwFlags As Integer, ByVal psiItem As IShellItem)
        Sub PostDeleteItem(ByVal dwFlags As Integer, ByVal psiItem As IShellItem, ByVal hrDelete As Integer, ByVal psiNewlyCreated As IShellItem)

        Sub PreNewItem(ByVal dwFlags As Integer, ByVal psiDestinationFolder As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String)
        Sub PostNewItem(ByVal dwFlags As Integer, ByVal psiDestinationFolder As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String, _
                        <MarshalAs(UnmanagedType.LPWStr)> ByVal pszTemplateName As String, ByVal dwFileAttributes As Integer, ByVal hrNew As Integer, ByVal psiNewItem As IShellItem)

        Sub UpdateProgress(ByVal iWorkTotal As Integer, ByVal iWorkSoFar As Integer)

        Sub ResetTimer()
        Sub PauseTimer()
        Sub ResumeTimer()
    End Interface





    <ComImport()> _
    <Guid("947aab5f-0a5c-4c13-b4d6-4bf7836fc9f8")> _
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface IFileOperation
        Function Advise(ByVal pfops As IFileOperationProgressSink) As Integer
        Sub Unadvise(ByVal dwCookie As Integer)

        Sub SetOperationFlags(ByVal dwOperationFlags As FileOperationFlags)
        Sub SetProgressMessage(<MarshalAs(UnmanagedType.LPWStr)> ByVal pszMessage As String)
        Sub SetProgressDialog(<MarshalAs(UnmanagedType.[Interface])> ByVal popd As Object)
        Sub SetProperties(<MarshalAs(UnmanagedType.[Interface])> ByVal pproparray As Object)
        Sub SetOwnerWindow(ByVal hwndParent As IntPtr)

        Sub ApplyPropertiesToItem(ByVal psiItem As IShellItem)
        Sub ApplyPropertiesToItems(<MarshalAs(UnmanagedType.[Interface])> ByVal punkItems As Object)

        Sub RenameItem(ByVal psiItem As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String, ByVal pfopsItem As IFileOperationProgressSink)
        Sub RenameItems(<MarshalAs(UnmanagedType.[Interface])> ByVal pUnkItems As Object, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String)

        Sub MoveItem(ByVal psiItem As IShellItem, ByVal psiDestinationFolder As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszNewName As String, ByVal pfopsItem As IFileOperationProgressSink)
        Sub MoveItems(<MarshalAs(UnmanagedType.[Interface])> ByVal punkItems As Object, ByVal psiDestinationFolder As IShellItem)

        Sub CopyItem(ByVal psiItem As IShellItem, ByVal psiDestinationFolder As IShellItem, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszCopyName As String, ByVal pfopsItem As IFileOperationProgressSink)
        Sub CopyItems(<MarshalAs(UnmanagedType.[Interface])> ByVal punkItems As Object, ByVal psiDestinationFolder As IShellItem)

        Sub DeleteItem(ByVal psiItem As IShellItem, ByVal pfopsItem As IFileOperationProgressSink)
        Sub DeleteItems(<MarshalAs(UnmanagedType.[Interface])> ByVal punkItems As Object)

        Sub NewItem(ByVal psiDestinationFolder As IShellItem, ByVal dwFileAttributes As IO.FileAttributes, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszName As String, <MarshalAs(UnmanagedType.LPWStr)> ByVal pszTemplateName As String, ByVal pfopsItem As IFileOperationProgressSink)

        Sub PerformOperations()

        Function GetAnyOperationsAborted() As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Interface



    <ComImport()> _
    <Guid("3ad05575-8857-4850-9277-11b85bdb8e09")> _
    Public Class CFileOperation
    End Class

    Public Module CFileOperationHelper
        ''' <summary>
        ''' Shell IFileOperation Objectを作成する
        ''' </summary>
        ''' <returns>XP以前は作れないので、Nothingがかえる</returns>
        Public Function CreateIFileOperation() As IFileOperation
            Try
                Dim t As Type = Type.GetTypeFromCLSID(GetType(CFileOperation).GUID)
                Return DirectCast(Activator.CreateInstance(t), IFileOperation)
            Catch e As COMException
                Return Nothing
            End Try
        End Function

        <DllImport("shell32", CharSet:=CharSet.Unicode, PreserveSig:=True)> _
        Private Function SHCreateItemFromParsingName( _
        <[In]()> <MarshalAs(UnmanagedType.LPWStr)> ByVal pszPath As String, _
        <[In]()> ByVal pbc As IntPtr, _
        <[In]()> <MarshalAs(UnmanagedType.LPStruct)> ByVal iIdIShellItem As Guid, _
        <[Out]()> <MarshalAs(UnmanagedType.Interface, IidParameterIndex:=2)> ByRef iShellItem As IShellItem) As Integer
        End Function


        ''' <summary>
        ''' ファイル・フォルダパスから、IShellItemを得る
        ''' </summary>
        ''' <param name="path">ファイル・フォルダパス</param>
        ''' <returns>パスを指し示すIShellItem</returns>
        Public Function FromName(ByVal path As String) As IShellItem
            Dim result As IShellItem = Nothing
            SHCreateItemFromParsingName(path, IntPtr.Zero, GetType(IShellItem).GUID, result)
            Return result
        End Function


        ''' <summary>
        ''' ファイル群をフォルダにコピーする。進捗ダイアログが出る
        ''' </summary>
        ''' <param name="hWnd">親ウインドウ。Form.Handleとか。</param>
        ''' <param name="dst_path">コピー先フォルダ</param>
        ''' <param name="src_files">コピー元のファイル連中</param>
        ''' <returns>０：正常、それ以外：異常コード</returns>
        ''' <remarks>Shell File Operationのラッパー</remarks>
        Public Function CopyFilesToFolder(ByVal hWnd As IntPtr, ByVal dst_path As String, ByVal src_files() As String) As Integer

            Dim op As IFileOperation = CreateIFileOperation()

            If op IsNot Nothing Then
                Try
                    If hWnd <> IntPtr.Zero Then
                        op.SetOwnerWindow(hWnd)
                    End If
                    Dim dst As IShellItem = FromName(dst_path)
                    For Each s As String In src_files
                        op.CopyItem(FromName(s), dst, Nothing, Nothing)
                    Next
                    op.PerformOperations()
                    Return 0
                Catch e As COMException
                    Return e.ErrorCode
                End Try
            Else
                ' before Vista. i.e. XP
                Dim fop As New SHFileOpStruct
                fop.hWnd = hWnd
                fop.fOP = FO.Copy
                fop.SetFromFile(src_files)
                fop.SetToFile(dst_path)
                fop.flags = 0
                fop.bAnyOperationAbort = True
                fop.hNameMappings = IntPtr.Zero
                fop.ProgressTitle = "Copying..."
                Return SHFileOperation(fop)
            End If
        End Function

    End Module

End Namespace