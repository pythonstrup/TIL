# 가상 스레드 심화

## MariaBD Benchmark

- 가상스레드를 사용하는 것이 좋긴 하다.

<img src="img/mariadb01.png">

## CPU를 사용하는 API?

- 가상 스레드는 과연 I/O을 하는 API에서만 효과가 있을까?

### 실험: 스프링 부트 + 피보나치 계산 + 1000 사용자 + 30분

- 출처: https://blog.fastthread.io/virtual-threads-performance-in-spring-boot/
- JMeter로 성능 테스트를 해봤을 때, 플랫폼 쓰레드와 가상 스레드 간 처리량, 평균 실행 시간 큰 차이 없다.
- 하지만 쓰레드 개수에서 차이가 났다.
- 플랫폼 쓰레드 235개
- 가상 쓰레드 43개 (더 적은 메모리!!)

# 참고자료

- [자바21 가상 쓰레드 쓰면 정말 성능 좋아?](https://www.youtube.com/watch?v=_lp3ohne-i8)
- [MariaDB 공식문서 - Benchmark JDBC connectors and Java 21 virtual threads](https://mariadb.com/resources/blog/benchmark-jdbc-connectors-and-java-21-virtual-threads/)
- [Virtual Threads Performance in Spring Boot](https://blog.fastthread.io/virtual-threads-performance-in-spring-boot/)
