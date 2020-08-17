#ifndef _MY_ODBC_H_
#define _MY_ODBC_H_


#include <windows.h> // WindowsAPI (MFC) 사용하기 위해 추가
#include <atlstr.h> // MFC CString 사용하기 위해 추가

// ODBC API를 사용하기 위한 헤더 파일 추가
#include <odbcinst.h>
#include <sqlext.h>

// ODBC API를 사용하기 위한 라이브러리 추가
#pragma comment(lib, "odbc32.lib")


// 디버그 출력
#define TR(s){CString str;str.Format(L"%s", _T(s));OutputDebugString(str);}


// ODBC_API 라이브러리
//#ifdef _DEBUG
//	#pragma comment(lib, "D_My_ODBC.lib")
//#else
//	#pragma comment(lib, "R_My_ODBC.lib")
//#endif


// typedef 함수 포인터 : 반환형(*함수이름)(매개변수)
// SET_RECORD_INFO    : Query의 결과에 해당하는 데이터를 담을 메모리(사용자 정의 객체)와 연결하기 위한 코드 구성
// SQL_RESULT_RECORD  : SQL 명령문에 의해서 가져온 데이터들을 자신이 원하는 형태로 변환하거나 저장,처리하는 작업
// 실제로 호출되는 함수는 실제로 작업을 하는(라이브러리를 사용하는) cpp 파일에 선언해서 사용
typedef void(*SET_RECORD_INFO)(void* ap_owner, HSTMT ah_statement, void* ap_data);
typedef int(*SQL_RESULT_RECORD)(void* ap_owner, int a_step_index, void* ap_data, ULONG a_count, unsigned short* ap_state, int option);



/*
// ex) 데이터 객체 클래스
class User
{
private:
	wchar_t id[20];
	wchar_t pw[20];
	wchar_t name[20];

public:
	wchar_t* GetID() { return id; }
	wchar_t* GetPW() { return pw; }
	wchar_t* GetName() { return name; }
	void SetID(wchar_t* ap_id) { wcscpy(id, ap_id); }
	void SetPW(wchar_t* ap_pw) { wcscpy(pw, ap_pw); }
	void SetName(wchar_t* ap_name) { wcscpy(name, ap_name); }
};

// Query의 결과에 해당하는 데이터를 User 객체와 연결하기 위한 코드 구성
void SetRecordInfo(void* ap_owner, HSTMT ah_statement, void* ap_data)
{
	User* p = (User*)ap_data;

	// 테이블에서 가져온 데이터를 속성별로 raw_data 변수에 저장하기 위해 속성별로 저장할 메모리 위치를 설정 (데이터 버퍼를 결과 집합의 열에 바인딩)
	SQLBindCol(ah_statement, 1, SQL_WCHAR, p->GetID(), sizeof(wchar_t) * 20, NULL);
	SQLBindCol(ah_statement, 2, SQL_WCHAR, p->GetPW(), sizeof(wchar_t) * 20, NULL);
	SQLBindCol(ah_statement, 3, SQL_WCHAR, p->GetName(), sizeof(wchar_t) * 20, NULL);
}

// SQL 명령문에 의해서 가져온 데이터들을 자신이 원하는 형태로 변환하거나 저장하는 작업
int ResultRecord(void* ap_owner, int a_step_index, void* ap_data, ULONG a_count, unsigned short* ap_state, int option)
{
	int result = 0; // 성공: 1반환
	CString str;
	User* p = (User*)ap_data;

	// 전체 사용자 검색
	if (option == 0)
	{
		for (ULONG i = 0; i < a_count; i++)
		{
			if (ap_state[i] != SQL_ROW_DELETED && ap_state[i] != SQL_ROW_ERROR)
			{
				str.Format(L"ID: %s\nPW:%s\nName:%s", p->GetID(), p->GetPW(), p->GetName());
				MessageBox(NULL, str, L"사용자", MB_OK);
				result = 1;
			}
			else
			{
				MessageBox(NULL, L"ap_state[0] : SQL_ROW_DELETED or SQL_ROW_ERROR", NULL, MB_OK);
				result = 0;
			}
			p++;
			ap_state++;
		}
	}
}

// SET_RECORD_INFO 함수가 호출될 때 실제로 SetRecordInfo함수가 실행된다

*/




class My_Odbc
{
private:
	SQLHANDLE mh_environment;  // ODBC 기술을 사용하기 위한 환경 정보 (환경 핸들)
	SQLHDBC mh_odbc;           // ODBC 함수를 사용하기 위한 정보 (연결 핸들)
	int m_connect_flag;        // 서버와 연결 여부를 저장할 변수 (1:연결)
	void* mp_owner;            // 윈도우 핸들을 저장할 변수

public:
	My_Odbc();
	virtual ~My_Odbc();


	inline int IsConnected() { return m_connect_flag; }
	inline SQLHANDLE GetEnvHandle() { return mh_environment; }
	inline SQLHDBC GetOdbcHandle() { return mh_odbc; }


	// 멤버변수 초기화
	void InitObject();

	// 서버 연결 (성공:1반환)
	int Connect(const wchar_t* ap_dsn, const wchar_t* ap_id, const wchar_t* ap_pw, void* ap_owner = 0);

	// 서버 연결 해제
	void Disconnect();


	// 명령문 실행 (insert, update, delete) (성공: 1반환)
	int ExecQuery(const wchar_t* ap_query);

	// 명령문 실행 (select) (성공: 1반환)
	int ExecQuery(const wchar_t* ap_query, int a_record_size, SET_RECORD_INFO set_record_info, SQL_RESULT_RECORD sql_result_record,
		int option, int a_record_count_per_step = 100);


};


#endif