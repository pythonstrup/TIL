# What is Domain Name

## Domain name 이란?

> A domain name is a string of text that maps to an alphanumeric IP address, used to access a website from client software. In plain English, a domain name is the text that a user types into a browser window to reach a particular website. For instance, the domain name for Google is ‘google.com’.

> The actual address of a website is a complex numerical IP address (e.g. 192.0.2.2), but thanks to DNS, users are able to enter human-friendly domain names and be routed to the websites they are looking for. This process is known as a DNS lookup.

- IP 주소는 컴퓨터가 처리하기에는 간단하지만 사람이 보기에도, 기억하기에도 너무 어렵다.
- 이 문제를 해결하기 위해 Domain Name이 생겼다. 도메인 이름이란 사람이 읽기 좋은 알파벳으로 적힌 주소 이름이다.
- 숫자로 되어있는 IP 주소를 DNS(Domain Name System)를 통해 도메인 이름과 매핑한다.

<hr/>
<br/>

## Domain Name Structure

<img src="img/domain-name-structure.png">

- 도메인 이름은 점으로 구분되며 오른쪽에서 왼쪽으로 읽는다.
- 점으로 구분된 도메인 이름은 특정 정보를 제공해준다.

### TLD (Top-Level-Domain)

- TLD는 도메인 이름 중 가장 오른쪽에 있는 것으로 서비스의 목적을 보여주는 정보이다.
- 가장 일반적인 TLD의 예로는 `.org`, `.com`, `.net` 등이 있는데, 이런 TLD들은 특정한 기준을 요구하지 않지만 일부 TLD들은 엄격한 정책이 존재한다.
- 아래는 엄격한 정책을 가지고 있는 TLD이다.
  - Local TLDs (`.us`, `.fr`, `.se`, `kr`): 지역이나 언어 정보를 가지고 있는 TLD이다.
  - 정부 표시(`.gov`): 정부 부서에 속한 도메인 이름을 표시할 때 사용한다.
  - 교육 표시(`.edu`): 교육 및 학술 기관에서만 사용할 수 있다.
- TMI of TLD
  - TLD에는 특수 문자와 라틴 문자가 포함될 수 있다고 한다.
  - 최대 길이는 63자이다. 하지만 대부분 TLD의 길이는 2~3장이다.

### Label (or Component)

- Label은 TLD의 왼쪽에 위치하는 정보로 TLD와 다르게 문자가 엄격하게 정해져있다.
  - 1~63자의 길이를 가져야한다.
  - 문자는 `A~Z`, 숫자는 `0~9`, 특수문자는 `-`(하이픈)만 가능하다!
- TLD 바로 옆에 있는 Label을 SLD(Secondary Level Domain)라고 부른다.
- 도메인 이름은 Label을 많이 가지고 있을 수 있다. 단 하나 이상의 Label을 가지고 있어야한다. (SLD는 필수라는 얘기)

<hr/>
<br/>

## 도메인 이름의 관리자

### 도메인은 누가 관리할까?

- 도메인 이름을 구매하려면 관리자가 누군지 알아야한다.
- 도메인 이름은 [domain name registry](https://ko.wikipedia.org/wiki/%EB%8F%84%EB%A9%94%EC%9D%B8_%EB%84%A4%EC%9E%84_%EB%A0%88%EC%A7%80%EC%8A%A4%ED%8A%B8%EB%A6%AC)에서 가지고 있는데 [registrars](https://www.cloudflare.com/ko-kr/learning/dns/glossary/what-is-a-domain-name-registrar/)(도메인 이름 등록 대행자)라고 부르는 곳에 관리가 위임되어있다.
- `domain name registry`는 DNS의 일부분으로 최상위 도메인(TLD)에 등록된 모든 도메인 이름의 데이터베이스이다.
  - `.kr`의 경우 한국인터넷진흥원이 가지고 있다고 한다.
  - `.com`과 `.net`은 VeriSign(베리사인)이라는 곳에서 가지고 있다.
- `domain registrars`는 일반 최상위 도메인 레지스트리나 국가 코드 최상위 도메인 레지스트리에 의해 공인된 기구 또는 사업체이다.
  - 대표적인 회사는 가비아이다.

### 도메인을 구매하는 법

- 사실 도메인은 구매하는 것이 아니라 대여의 개념으로 운영된다. 누군가가 도메인을 모두 구매해버린다면 더이상 도메인을 구매할 수 없게되어 웹이 막혀버리는 사태가 발생할 수 있기 때문이다.
- 1년 이상 계약해 대여하며, 대신 연장할 때 원래 사용자에게 우선권을 준다.
- 한국에서는 가비아가 가장 유명하다. 가비아 사이트를 사용하면 도메인을 쉽게 대여할 수 있다.

### 이미 사용하고 있는 도메인인지 확인하는법

- 만약 누군가가 이미 도메인을 점유하고 있다면 대여할 수 없기 때문에 미리 확인하는 절차가 필요할 수 있다. 이 때 필요한 것은 `whois` 명령어이다.
- 아래는 네이버의 도메인을 확인해본 자료이다. 여러 정보를 확인해볼 수 있는데 이미 점유되었음을 확인할 수 있다.

```
$ whois naver.com
% IANA WHOIS server
% for more information on IANA, visit http://www.iana.org
% This query returned 1 object

refer:        whois.verisign-grs.com

domain:       COM

organisation: VeriSign Global Registry Services
address:      12061 Bluemont Way
address:      Reston VA 20190
address:      United States of America (the)

contact:      administrative
name:         Registry Customer Service
organisation: VeriSign Global Registry Services
address:      12061 Bluemont Way
address:      Reston VA 20190
address:      United States of America (the)
phone:        +1 703 925-6999

...

# whois.verisign-grs.com

   Domain Name: NAVER.COM
   Registry Domain ID: 793803_DOMAIN_COM-VRSN
   Registrar WHOIS Server: whois.gabia.com
   Registrar URL: http://www.gabia.com
   Updated Date: 2022-09-20T06:51:25Z
   Creation Date: 1997-09-12T04:00:00Z
   Registry Expiry Date: 2023-09-11T04:00:00Z
   Registrar: Gabia, Inc.
   Registrar IANA ID: 244
   Registrar Abuse Contact Email: abuse@gabia.com
   Registrar Abuse Contact Phone: +82.2.829.3543
   Domain Status: clientDeleteProhibited https://icann.org/epp#clientDeleteProhibited
   Name Server: NS1.NAVER.COM
   Name Server: NS2.NAVER.COM
   DNSSEC: unsigned
   URL of the ICANN Whois Inaccuracy Complaint Form: https://www.icann.org/wicf/
>>> Last update of whois database: 2023-01-01T09:12:28Z <<<
```

- 반면 점유되지 않은 도메인을 검색하면 `No match for domain`라고 표시해준다.

```
$ whois fskwenn321948neslkn-gwenl-gwenk.com

...

No match for domain "FSKWENN321948NESLKN-GWENL-GWENK.COM".
>>> Last update of whois database: 2023-01-01T09:11:13Z <<<
```

<hr/>
<br/>

## 도메인 이름과 URL의 차이?

- URL(Uniform Resource Locator)은 도메인과 기타 정보(프로토콜과 경로)를 포함한 정보이다.
- `https://comic.naver.com/webtoon/weekday`은 URL이고, `naver.com`은 도메인이다.
  - 예시에서 볼 수 있다시피 URL은 `https`라는 프로토콜 정보와 `/webtoon/weekday`라는 경로를 추가로 가지고 있다.

<hr/>
<br/>

## DNS 요청의 작동 원리

1. 먼저 도메인 이름이나, URL을 주소검색창에 입력한다.
2. 브라우저는 컴퓨터에게 도메인 이름에 대한 캐싱을 통해 가지고 올 수 있는 IP 주소가 있는지 질의하고 만약 존재한다면 바로 해당 IP 주소로 접속한다.
3. 만약 없다면, DNS 서버에 도메인 네임으로 요청을 보내 IP 주소를 매핑해달라고 해야한다.
4. DNS 서버가 정상적으로 IP 주소를 전달해주면 해당 IP 주소로 접속하게 된다.

<img src="img/dns-request2.png">

<hr/>
<br/>

## 참고자료

- [CloudFlare: What is a domain name? | Domain name vs. URL](cloudflare.com/ko-kr/learning/dns/glossary/what-is-a-domain-name/)

- [Mozilla: What is a Domain Name](https://developer.mozilla.org/en-US/docs/Learn/Common_questions/What_is_a_domain_name)
