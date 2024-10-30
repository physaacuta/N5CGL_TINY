#pragma once
// FindFirstFile/NextFile API�� ���b�p�[�B  Version 1.1
// �ړI�FFindFirstFile��for���̒��Ŏg�������I
//
// �g����:
//    for(Win32Finder f("c:\\hoge", "*.exe"); f; f++ ){
//      //���������t�@�C���ɑ΂��鏈�u�B
//      //���Ƃ��΁F printf("%s\t%lld\n", f.cFileName, f.filesize() );
//    }
// ++�Ƃ������ȏꍇ�́A����Ȋ���
//    for(Win32Finder f("c:\\hoge", "*.exe"); f.valid(); f.next() ){}
//    for(Win32Finder f("c:\\hoge", "*.exe"); f.IsValid(); f.MoveNext() ){}

// �P�ƃt�@�C���E�t�H���_���Ȃ������Ƃ��̏����B
//    if(  !Win32Finder::stat("c:\\hoge\\bar") ){
//       if(!Win32Finder::IsLastBadError() ){ /* �Ȃ������Ƃ��̏���*/ } }
//       else { /*Find���G���[�ɂȂ���*/ }
//    }
// �P�ƃt�@�C���E�t�H���_���������Ƃ��̏���(�v���X�A���t�@)�B
//    Win32FindData wd;
//    if(  Win32Finder::stat("c:\\hoge\\bar", &wd) ){ /* �������Ƃ��{�t�@�C�������g��������*/ }

// IsDirectory()�́A�u.�v��u..�v�̎���false�B

// �G���[�����ɂ��Ă̒���
// �����ɂ����Ȃ�A!f�A!stat()�����������Ƃ��A���́i�t�@�C�����Ȃ��̂łȂ��j
// �l�b�g���[�N�G���[���������Ă����A�Ƃ����ꍇ������B���̂悤�ȏꍇ�ɂ́A
//  if( Win32Finder::IsLastBadError() ) { �G���[���� }
//  if( f.error() ) { �G���[���� }
//  if( f.IsError() ) { �G���[���� }
// �݂����ȋL�q��ǉ�����K�v������B
// �Ȃ̂ŁA��L�́A�u�Ȃ������Ƃ��̏����v�ł́Aerror���`�F�b�N���Ă���


//WIN32_FIND_DATAA�̃��b�p�[�A�N�Z�b�T�[�N���X�B���b�s���O���\�b�h��������Ă��邾���B
//�Ȃ̂ŁAWIN32_FIND_DATA���_�E���L���X�g���Ă�OK.
class Win32FindData : public ::WIN32_FIND_DATAA
{
public:
    //�t�@�C���̃T�C�Y�B64bit�����ŕԂ��B
    long long filesize() const {
        return (((long long)nFileSizeHigh) << 32) + nFileSizeLow;
    }
    long long GetFileSize() const { return filesize(); }

    //FILETIME�́A64bit�^�����AWIn32_FIND_DATA�ł́A�A���C�������g������Ă���̂Ŗʓ|�B
    static long long FileTimeToInt64(const FILETIME &ft) {
        return (((long long)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    }

    //�쐬��
    long long cftime()          const { return FileTimeToInt64(ftCreationTime); }
    long long GetCreationTime() const { return FileTimeToInt64(ftCreationTime); }
    //������
    long long mftime()          const { return FileTimeToInt64(ftLastWriteTime); }
    long long GetLastWriteTime()const { return FileTimeToInt64(ftLastWriteTime); }
    // �A�N�Z�X��LastAccessTime�͈Ӗ����Ȃ����Ƃ������̂ŁA�A�N�Z�b�T�͍��Ȃ��B


    //�f�B���N�g�����H ����t�H���_�u.�v�u..�v��false�Ƃ���B
    bool directory() const {
        return IsDirectoryRaw()
            && !IsDiretoryDot();
    }
    bool IsDirectory() const { return directory(); }

    //�t�@�C�����H
    bool file() const {
        return !IsDirectoryRaw();
    }
    bool IsFile() const {
        return file();
    }

    //�S��I�ɂ́APrivate�BWIN32_FIND_DATA��public�p�����Ă���̂�..Public
    // �f�B���N�g���������H
    bool IsDirectoryRaw() const {
        return 0 != (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    // ., ..�Ƃ��̓���ȃf�B���N�g�����H
    bool IsDiretoryDot() const {
        return (cFileName[0] == '.' && cFileName[1] == '\0') ||
            (cFileName[0] == '.' && cFileName[1] == '.' && cFileName[2] == '\0');

    }
};

//�����N���X�B
class Win32Finder : public Win32FindData
{
protected:
    HANDLE m_hFind;         //�����n���h���B��{�I�ɂ́A�T��������Close
    char m_path[MAX_PATH];  //�����x�[�X�t�H���_�B�Ō��\�����B
    DWORD m_last_error;

    void close(){
        if( valid() ) {
            // �Ō��FindNextFile��Error��ۑ�����B
            DWORD lerr = ::GetLastError();
            ::FindClose(m_hFind);
            ::SetLastError( lerr );
        }
        m_hFind = INVALID_HANDLE_VALUE;
    }

    // SJIS2�o�C�g������1�o�C�g�ڂɊY�����邩�H
    static  inline  bool isSJISlead(char c) {
        unsigned int uc = (unsigned char)c;
        return ( ( uc ^ 0x20) - 0x0A1) < 0x3C;
    }
    // src ��dst�ɃR�s�[����B���̍ہAsrc�̍Ō��\�����Ă���悤�ɂ���B
    //�@SJIS�̋��|�F src="c:\�`�\" �̂Ƃ��A src[ strlen(src) -1 ] �� '\\' �ɂȂ�.
    int  CopyAndAddYen(char *dst, const char *src) {
        bool bLead = false;
        char chLast = 0;    //�Ō�̕����B������2byte(�����Ƃ�)�̏ꍇ�͍ŏ���1byte���B
        int ii;
        for (ii = 0; ; ii++) {
            const char ch = src[ii];
            dst[ii] = ch;
            if (ch == 0) { // NUL CHAR �͍ŗD��
                break;
            }

            if (bLead) {    //2byte������2byte�ڂȂ̂�chLast���X�V���Ȃ�.
                bLead = false;
                continue;
            }

            chLast = ch;
            if(isSJISlead(ch)) {
                bLead = true;
            }
        }
        //�Ōオ\�łȂ���΁A\�𑫂��B
        if (chLast != '\\') {
            dst[ii++] = '\\';
            dst[ii] = 0;
        }
        return ii;
    }


    // ���̃N���X�̓R�s�[�֎~�B
    Win32Finder &operator = (const Win32Finder &rhs) { return *this; }
    Win32Finder(const Win32Finder &) {};
public:

    //FindFirst/NextFile���g���Ƃ��A�t�@�C�����Ȃ��̂��A
    //�G���[�����������̂�����ʂ������΂���������B
    static bool IsLastBadError(DWORD nLastError)
    {
        return nLastError != 0
            && nLastError != ERROR_FILE_NOT_FOUND
            && nLastError != ERROR_PATH_NOT_FOUND
            && nLastError != ERROR_NO_MORE_FILES
            ;
    }
    static bool IsLastBadError() { return IsLastBadError( ::GetLastError() ); }
    
    // �P�ƃt�@�C���̂���Ȃ��B
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
    
    // �P�ƃt�@�C���̂���Ȃ��B
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

    // �t�@�C���E�t�H���_�̗L�薳���B
    // stat���A�l�b�g���[�N���L�g�b�v�̑��ݔ���Ɏ��s����̂ŁB( \\10.27.198.34\f$ �Ȃǁj
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

    // �R���X�g���N�^
    // path : �t�@�C���E�t�H���_��T���x�[�X�t�H���_
    // pattern : �T���p�^�[��������(�ȗ���)
    Win32Finder( const char * path, const char * pattern){
        CopyAndAddYen(m_path, path);

        char find_name[MAX_PATH * 2];
        sprintf_s(find_name, "%s%s", m_path, pattern);
//        TODO: Win7�ȍ~�ł́AFindFirstFileEx���g���ƁA���т��Ƒ����Ȃ�炵��
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

    //WIN32_FIND_DATA���L���� 
    // (next�Ăяo���Ńt�@�C�����Ȃ��Ȃ�����A�n���h���͕���̂ŁB)
    bool valid() const { return m_hFind != INVALID_HANDLE_VALUE;}
    bool IsValid() const { return valid(); }
    bool IsInvalid() const { return !valid(); }

    // explicit operator bool()�̑���B
    operator const void *() const {
        return valid() ? (const void *)this : (const void*)NULL;
    }
    bool  operator !() const { 
        return !valid();
    }


//���̃t�@�C���Ɉړ�����B�Ȃ���΁AFindHandle�����B
    bool next()
    {
        while(0 == ::FindNextFileA(m_hFind, this ) ){
            //���{���130�����ȏ�̏ꍇ�AANSI�ɕϊ��ł��Ȃ��̂�
            //�G���[�ɂȂ邪�A����(�ʂ̃t�@�C��)�͂��邩������Ȃ��B
            if( ERROR_MORE_DATA == ::GetLastError() ){
                continue;
            }
            else {
                m_last_error = ::GetLastError();
                //�{���ɃG���[���A�I���Ȃ̂ŁAclose()����B
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
    //�i�� ++�̑O�u�ƌ�u�͈Ӗ����Ȃ��̂ŋ�ʂ��Ȃ��j

    //�Ō��FindFile�̃G���[�R�[�h��Ԃ��B
    DWORD errorcode() const { return m_last_error; }
    DWORD GetLastError() const { return m_last_error; }
    
    //�G���[�������̂�(true)�A�t�@�C�����Ȃ������̂�(false)��Ԃ��B
    bool IsError() const { return IsLastBadError(m_last_error); }

//���������t�H���_�����������B
    //�Ō�ɂ͐��"\"����
    const char *basepath() const { return m_path; }
    const char *GetBasePath() const { return m_path;}
    
#ifndef NO_MFC_CSTRING
    //�t���p�X�Ńt�@�C������Ԃ��B
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
