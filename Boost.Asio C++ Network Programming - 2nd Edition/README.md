# Boost.Asio C++ Network Programming - 2nd Edition

## 소스 코드 차이 설명

```original```은 원래 소스 코드이고 ```modified```는 수정한 소스 코드이다. 차이는 다음과 같다.

* ASIO 라이브러리 버전은 1.11.0
* ASIO 라이브러리는 Boost에 의존하지 않고 독립형(standalone)으로 사용
* 최신 C++ 형식에 맞춤
* 6장에서 사용하는 래퍼 클래스(```wrapper.h```, ```wrapper.cpp```) 오류 수정

## 수정 소스 코드 빌드 방법

미리 만들어 둔 ```CMakeLists.txt```를 이용해 빌드할 때는 ASIO 라이브러리의 ```include``` 디렉터리를 ```modified\third_party\asio```에 두면 된다.
