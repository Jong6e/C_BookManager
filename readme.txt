선문대학교 통합도서정보관리 시스템 
프로젝트 파일 요약


book.h:
도서 정보를 저장하는 BOOK 구조체 정의 (ID, 제목, 저자, 평점)
MAX_BOOKS 상수 정의 (최대 700권)
도서 관련 함수들의 선언
파일 로드/저장
도서 검색/정렬/출력
메모리 관리
도서 시스템의 핵심 데이터 구조와 인터페이스를 제공

book.c:
book.h에 선언된 함수들의 실제 구현
도서 정보 동적 할당 및 해제 (createNewBook, freeAllBooks)
파일에서 도서 데이터 로드 (loadBooksFromFile)
검색 및 정렬 로직 구현 (searchBooks, sortBooks)
문자열 소문자 변환 (toLowerCase) 등 유틸리티 함수

book_command.h:
도서 관련 명령어를 처리하는 handleBookCommand 함수 선언
클라이언트-서버 통신에서 도서 관련 요청 처리 인터페이스 제공

book_command.c:
handleBookCommand 함수 구현
다음 명령어 처리: SEARCH, ADD, DELETE_BOOK, UPDATE, FIND_BOOK, SORT, RANK
클라이언트 요청 분석 및 해당 도서 기능 실행
결과를 응답 버퍼에 저장하여 클라이언트에게 반환
파일 저장 및 오류 처리 포함

book_menu.h:
도서관리 메뉴 UI 진입점 함수 선언 (bookMenu)
사용자 인터페이스 구성요소 정의

book_menu.c:
도서관리 메뉴 구현 (검색, 추가, 삭제, 수정, 정렬, 평점 랭킹)
사용자 입력 처리 및 서버 요청 전송
결과 출력 및 페이지네이션 처리
도서 관련 기능의 사용자 인터페이스 제공

user.h:
사용자 정보를 저장하는 USER 구조체 정의 (ID, 비밀번호)
MAX_USERS 상수 정의 (최대 100명)
사용자 관련 함수들의 선언
로그인/회원가입/삭제/수정
파일 로드/저장
사용자 관리 시스템의 핵심 인터페이스 제공

user.c:
user.h에 선언된 함수들의 실제 구현
사용자 정보 파일 로드/저장 (loadUsersFromFile, saveUsersToFile)
사용자 인증 및 관리 기능 구현 (checkLogin, registerUser, deleteUser, modifyUser)
ID 검색 및 인덱스 조회 (findUserIndex)

user_command.h:
사용자 관련 명령어를 처리하는 handleUserCommand 함수 선언
클라이언트-서버 통신에서 사용자 관련 요청 처리 인터페이스 제공

user_command.c:
handleUserCommand 함수 구현
다음 명령어 처리: LOGIN, REGISTER, DELETE, FIND, MODIFY
클라이언트 요청 분석 및 해당 사용자 관리 기능 실행
결과를 응답 버퍼에 저장하여 클라이언트에게 반환

user_menu.h:
사용자 메뉴 UI 진입점 함수 선언 (userMenu)
프로그램 종료 함수 선언 (exitProgram)
사용자 인터페이스 구성요소 정의

user_menu.c:
사용자 메뉴 구현 (로그인, 회원가입, 사용자 삭제, 조회, 비밀번호 수정)
사용자 입력 처리 및 서버 요청 전송
프로그램 종료 기능 구현 (exitProgram)
로그인 성공 시 도서 메뉴로 이동하는 흐름 제어

common_input.h:
공통 입력 처리 함수 선언 (clearScreen, getEscapableInput)
여러 메뉴에서 공통으로 사용하는 입력 인터페이스 제공

common_input.c:
화면 초기화 함수 구현 (clearScreen)
ESC 키 취소 기능이 있는 입력 함수 구현 (getEscapableInput)
비밀번호 입력 시 마스킹() 기능 제공

bmserver.c:
서버 메인 파일
소켓 통신 설정 및 클라이언트 연결 관리
클라이언트 요청 분석 및 적절한 명령 처리기 호출
사용자 및 도서 데이터 초기 로드
서버 응답 전송 및 로깅

bmclient.c:
클라이언트 메인 파일
서버 연결 설정
사용자 메뉴 진입
프로그램 종료 처리
클라이언트 측 네트워크 자원 관리

booklist2.txt:
도서 정보를 저장하는 데이터 파일
탭으로 구분된 도서 정보 (ID, 제목, 저자, 평점)
서버 시작 시 로드되고 변경 사항 발생 시 업데이트됨

users.txt:
사용자 정보를 저장하는 데이터 파일
콜론으로 구분된 사용자 정보 (ID, 비밀번호)
서버 시작 시 로드되고 변경 사항 발생 시 업데이트됨