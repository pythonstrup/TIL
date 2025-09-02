# 접근 권한

## 1. 사용자 생성

```sql
CREATE USER 'app_user'@'localhost' IDENTIFIED BY 'secure_password'; -- 사용자 생성
CREATE USER 'app_user'@'%' IDENTIFIED BY 'secure_password'; -- 원격 접속 (`와일드카드 %`로 설정하면 모든 IP에서 접속 가능)

-- 사용자 목록 확인
SELECT User, Host FROM mysql.user;
```

| User             | Host |
|:-----------------| :--- |
| app_user         | % |
| app_user         | localhost |
| mysql.infoschema | localhost |
| mysql.session    | localhost |
| mysql.sys        | localhost |
| root             | localhost |


## 2. 사용자 권한 부여

```sql
-- 권한 부여
GRANT SELECT, INSERT, UPDATE, DELETE ON database_name.* TO 'app_user'@'%';
-- 권한 확인
SHOW GRANTS FOR 'app_user'@'%';
-- 권한 적용
FLUSH PRIVILEGES;
```

| Grants for app_user@%                                                          |
|:----------------------------------------------------------------------------|
| GRANT USAGE ON *.* TO `app_user`@`%`                                        |
| GRANT SELECT, INSERT, UPDATE, DELETE ON `database_name`.* TO `app_user`@`%` |


