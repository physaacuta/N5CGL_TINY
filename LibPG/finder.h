#pragma once
// FindFirstFile/NextFile APIの ラッパー。  Version 1.1
// 目的：FindFirstFileをfor文の中で使いたい！
//
// 使い方:
//    for(Win32Finder f("c:\\hoge", "*.exe"); f; f++ ){
//      //発見したファイルに対する処置。
//      //たとえば： printf("%s\t%lld\n", f.cFileName, f.filesize() );
//    }
// ++とか嫌いな場合は、こんな感じ
//    for(Win32Finder f("c:\\hoge", "*.exe"); f.valid(); f.next() ){}
//    for(Win32Finder f("c:\\hoge", "*.exe"); f.IsValid(); f.MoveNext() ){}

// 単独ファイル・フォルダがなかったときの処理。
//    if(  !Win32Finder::stat("c:\\hoge\\bar") ){
//       if(!Win32Finder::IsLastBadError() ){ /* なかったときの処理*/ } }
//       else { /*Findがエラーになった*/ }
//    }
// 単独ファイル・フォルダがあったときの処理(プラスアルファ)。
//    Win32FindData wd;
//    if(  Win32Finder::stat("c:\\hoge\\bar", &wd) ){ /* あったとき＋ファイル情報を使った処理*/ }

// IsDirectory()は、「.」や「..」の時はfalse。

// エラー処理についての注意
// 厳密にいうなら、!f、!stat()が成立したとき、実は（ファイルがないのでなく）
// ネットワークエラーが発生していた、という場合がある。そのような場合には、
//  if( Win32Finder::IsLastBadError() ) { エラー処理 }
//  if( f.error() ) { エラー処理 }
//  if( f.IsError() ) { エラー処理 }
// みたいな記述を追加する必要がある。
// なので、上記の、「なかったときの処理」では、errorもチェックしている


//WIN32_FIND_DATAAのラッパーアクセッサークラス。ラッピングメソッドが足されているだけ。
//なので、WIN32_FIND_DATAをダウンキャストしてもOK.
class Win32FindData : public ::WIN32_FIND_DATAA
{
public:
    //ファイルのサイズ。64bit整数で返す。
    long long filesize() const {
        return (((long long)nFileSizeHigh) << 32) + nFileSizeLow;
    }
    long long GetFileSize() const { return filesize(); }

    //FILETIMEは、64bit型だが、WIn32_FIND_DATAでは、アラインメントがずれているので面倒。
    static long long FileTimeToInt64(const FILETIME &ft) {
        return (((long long)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    }

    //作成日
    long long cftime()          const { return FileTimeToInt64(ftCreationTime); }
    long long GetCreationTime() const { return FileTimeToInt64(ftCreationTime); }
    //書込日
    long long mftime()          const { return FileTimeToInt64(ftLastWriteTime); }
    long long GetLastWriteTime()const { return FileTimeToInt64(ftLastWriteTime); }
    // アクセス日LastAccessTimeは意味がないことが多いので、アクセッサは作らない。


    //ディレクトリか？ 特殊フォルダ「.」「..」はfalseとする。
    bool directory() const {
        return IsDirectoryRaw()
            && !IsDiretoryDot();
    }
    bool IsDirectory() const { return directory(); }

    //ファイルか？
    bool file() const {
        return !IsDirectoryRaw();
    }
    bool IsFile() const {
        return file();
    }

    //心情的には、Private。WIN32_FIND_DATAをpublic継承しているので..Public
    // ディレクトリ属性か？
    bool IsDirectoryRaw() const {
        return 0 != (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    // ., ..とかの特殊なディレクトリか？
    bool IsDiretoryDot() const {
        return (cFileName[0] == '.' && cFileName[1] == '\0') ||
            (cFileName[0] == '.' && cFileName[1] == '.' && cFileName[2] == '\0');

    }
};

//検索クラス。
class Win32Finder : public Win32FindData
{
protected:
    HANDLE m_hFind;         //検索ハンドル。基本的には、探索完了でClose
    char m_path[MAX_PATH];  //検索ベースフォルダ。最後に\がつく。
    DWORD m_last_error;

    void close(){
        if( valid() ) {
            // 最後のFindNextFileのErrorを保存する。
            DWORD lerr = ::GetLastError();
            ::FindClose(m_hFind);
            ::SetLastError( lerr );
        }
        m_hFind = INVALID_HANDLE_VALUE;
    }

    // SJIS2バイト文字の1バイト目に該当するか？
    static  inline  bool isSJISlead(char c) {
        unsigned int uc = (unsigned char)c;
        return ( ( uc ^ 0x20) - 0x0A1) < 0x3C;
    }
    // src をdstにコピーする。その際、srcの最後に\がついているようにする。
    //　SJISの恐怖： src="c:\～表" のとき、 src[ strlen(src) -1 ] は '\\' になる.
    int  CopyAndAddYen(char *dst, const char *src) {
        bool bLead = false;
        char chLast = 0;    //最後の文字。文字が2byte(漢字とか)の場合は最初の1byte側。
        int ii;
        for (ii = 0; ; ii++) {
            const char ch = src[ii];
            dst[ii] = ch;
            if (ch == 0) { // NUL CHAR は最優先
                break;
            }

            if (bLead) {    //2byte文字の2byte目なのでchLastを更新しない.
                bLead = false;
                continue;
            }

            chLast = ch;
            if(isSJISlead(ch)) {
                bLead = true;
            }
        }
        //最後が\でなければ、\を足す。
        if (chLast != '\\') {
            dst[ii++] = '\\';
            dst[ii] = 0;
        }
        return ii;
    }


    // このクラスはコピー禁止。
    Win32Finder &operator = (const Win32Finder &rhs) { return *this; }
    Win32Finder(const Win32Finder &) {};
public:

    //FindFirst/NextFileを使うとき、ファイルがないのか、
    //エラーが発生したのかを区別したいばあいがある。
    static bool IsLastBadError(DWORD nLastError)
    {
        return nLastError != 0
            && nLastError != ERROR_FILE_NOT_FOUND
            && nLastError != ERROR_PATH_NOT_FOUND
            && nLastError != ERROR_NO_MORE_FILES
            ;
    }
    static bool IsLastBadError() { return IsLastBadError( ::GetLastError() ); }
    
    // 単独ファイルのありなし。
    static bool stat( const char *path, WIN32_FIND_DATAA *pWd = 0){
        ::WIN32_FIND_DATAA wk;
        if( !pWd ) pWd = &wk;
        
        const HANDLE hFind = ::FindFirstFileA( path, pWd );
        if( hFind != INVALID_HANDLE_VALUE){
            ::FindClose(hFind);
            return true;
        }
        return false;
    }
    
    // 単独ファイルのありなし。
    // 
    static bool stat( const char *dir, const char *file, WIN32_FIND_DATAA *pWd = 0){
        Win32Finder wf( dir, file );
        if( wf.valid() ){
            if( pWd ){
                *pWd = wf;
            }
            SetLastError( 0 );
            return true;
        }
        return false;
    }

    // ファイル・フォルダの有り無し。
    // statが、ネットワーク共有トップの存在判定に失敗するので。( \\10.27.198.34\f$ など）
    static bool IsFolderExists( const char *dir ) {
        DWORD attr =  ::GetFileAttributes(dir);

        // Generic Error.
        if( attr == -1 ){
            return false;
        }
        
        if( 0 != (attr & FILE_ATTRIBUTE_DIRECTORY) ){
            return true;
        }

        // dir is file. not folder.
        SetLastError( ERROR_FILE_EXISTS );
        return false;
    }

    // コンストラクタ
    // path : ファイル・フォルダを探すベースフォルダ
    // pattern : 探すパターン文字列(省略可)
    Win32Finder( const char * path, const char * pattern){
        CopyAndAddYen(m_path, path);

        char find_name[MAX_PATH * 2];
        sprintf_s(find_name, "%s%s", m_path, pattern);
//        TODO: Win7以降では、FindFirstFileExを使うと、ちびっと早くなるらしい
//      if (1)
//          m_hFind = FindFirstFileEx(find_name, FindExInfoBasic, this, FindExSearchNameMatch, NULL, 0);
//      else
            m_hFind = FindFirstFileA(find_name, this );
            
        if (m_hFind == INVALID_HANDLE_VALUE) {
            m_last_error = ::GetLastError();
        }
        else{
            m_last_error = 0;
        }
 
    }
    
    ~Win32Finder(){
        close();
    }

    //WIN32_FIND_DATAが有効か 
    // (next呼び出しでファイルがなくなったら、ハンドルは閉じるので。)
    bool valid() const { return m_hFind != INVALID_HANDLE_VALUE;}
    bool IsValid() const { return valid(); }
    bool IsInvalid() const { return !valid(); }

    // explicit operator bool()の代わり。
    operator const void *() const {
        return valid() ? (const void *)this : (const void*)NULL;
    }
    bool  operator !() const { 
        return !valid();
    }


//次のファイルに移動する。なければ、FindHandleを閉じる。
    bool next()
    {
        while(0 == ::FindNextFileA(m_hFind, this ) ){
            //日本語で130文字以上の場合、ANSIに変換できないので
            //エラーになるが、続き(別のファイル)はあるかもしれない。
            if( ERROR_MORE_DATA == ::GetLastError() ){
                continue;
            }
            else {
                m_last_error = ::GetLastError();
                //本当にエラーか、終わりなので、close()する。
                close();
                return false;
            }
        }
        return true;
    }
    bool MoveNext() { return next(); }
    Win32Finder &operator ++(int){
        next();
        return *this;
    }
    Win32Finder &operator ++(){
        next();
        return *this;
    }
    //（↑ ++の前置と後置は意味がないので区別しない）

    //最後のFindFileのエラーコードを返す。
    DWORD errorcode() const { return m_last_error; }
    DWORD GetLastError() const { return m_last_error; }
    
    //エラーだったのか(true)、ファイルがなかったのか(false)を返す。
    bool IsError() const { return IsLastBadError(m_last_error); }

//検索したフォルダ名をかえす。
    //最後には絶対"\"がつく
    const char *basepath() const { return m_path; }
    const char *GetBasePath() const { return m_path;}
    
#ifndef NO_MFC_CSTRING
    //フルパスでファイル名を返す。
    CString fullpath() const {
        CString r;
        r.Format("%s%s", m_path, cFileName);
        return r;
    }
    CString GetFullPath() const {
        return fullpath();
    }
#endif
};
