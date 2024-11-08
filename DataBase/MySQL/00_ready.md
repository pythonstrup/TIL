# Docker Mysql 설치 및 실행

```shell
$ docker pull mysql:8.0
$  docker run -p 3306:3307 -e MYSQL_ROOT_PASSWORD=Qwer1234 -d mysql:8.0
```

# 데이터 구성

```mermaid
erDiagram
    dept_manager }o--|| departments: r
    departments ||--o{ dept_emp: r
    employees ||--o{ dept_manager: r
    employees ||--o{ dept_emp: r
    employees ||--o{ titles: r
    employees ||--o{ salaries: r
    dept_manager {
        CHAR(4) dept_no PK, FK 
        INTEGER emp_no PK, FK
        Date from_date
        Date to_date
    }
    departments {
        CHAR(4) dept_no PK
        VARCHAR(40) dept_name "AK1.1"
    }
    dept_emp {
        CHAR(4) dept_no PK, FK
        INTEGER emp_no PK, FK
        Date from_date "IE1.1, IE2.2"
        Date to_date
    }
    employees {
        INTEGER emp_no PK
        Date birth_date
        VARCHAR(14) first_name "IE1.1"
        VARCHAR(16) last_name
        ENUM gender "enum: 'M', 'F'"
        Date hire_date "IE2.1"
    }
    titles {
        INTEGER emp_no PK, FK
        Date from_date PK
        VARCHAR(50) title PK
        Date to_date "IE1.1"
    }
    salaries {
        INTEGER emp_no PK, FK
        Date from_date PK
        INT salary "IE1.1"
        Date to_date
    }
```

# 데이터 넣기

- 먼저 예제 파일로 들어간다.
- 그리고 mysql을 실행해주자.

```shell
$ mysql -h 127.0.0.1 -P 3307 -u root -p                                                                                                                                          1 ↵  1664  22:06:04
Enter password:
Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 8
Server version: 8.0.40 MySQL Community Server - GPL

Copyright (c) 2000, 2024, Oracle and/or its affiliates.

Oracle is a registered trademark of Oracle Corporation and/or its
affiliates. Other names may be trademarks of their respective
owners.

Type 'help;' or '\h' for help. Type '\c' to clear the current input statement.

mysql> CREATE DATABASE employees DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci;
Query OK, 1 row affected (0.01 sec)

mysql> use employees
Database changed
mysql> source employees.sql
...

Query OK, 0 rows affected (0.00 sec)

Query OK, 0 rows affected (0.00 sec)

Query OK, 0 rows affected (0.00 sec)

Query OK, 0 rows affected (0.00 sec)

Query OK, 0 rows affected (0.00 sec)

Query OK, 0 rows affected (0.00 sec)
```
