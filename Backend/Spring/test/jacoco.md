# Jacoco

## Gradle 기본 설정

### build.gradle (groovy)

```groovy
plugins {
    id 'java'
    id 'org.springframework.boot' version '3.1.2'
    id 'io.spring.dependency-management' version '1.1.2'

    // 1. Jacoco 플러그인 추가
    id 'jacoco'
}

group = 'com.onebyte'
version = '0.0.1-SNAPSHOT'

java {
    sourceCompatibility = '17'
}

configurations {
    compileOnly {
        extendsFrom annotationProcessor
    }
}

repositories {
    mavenCentral()
}

dependencies {
    implementation 'org.springframework.boot:spring-boot-starter-data-jpa'
    implementation 'org.springframework.boot:spring-boot-starter-web'
    compileOnly 'org.projectlombok:lombok'
    runtimeOnly 'com.mysql:mysql-connector-j'
    annotationProcessor 'org.projectlombok:lombok'

    /**
     * Test Implementation
     */
    testImplementation 'org.springframework.boot:spring-boot-starter-test'

    ...
}

/**
 * Jacoco
 */
tasks.named('test') {
    useJUnitPlatform()
    // 2. test 종료 후 jacocoTestReport 실행
    finalizedBy jacocoTestReport
}

// 3. 커버리지 결과를 html 파일로 가공
jacocoTestReport {
    dependsOn test
}

// 4. 커버리지 기준을 만족하는지 확인해 주는 task
jacocoTestCoverageVerification {
    violationRules {
        rule {
            enabled = true
        
            element = 'CLASS'

            limit {
                counter = 'BRANCH'
                value = 'COVERDRATIO'
                minimum = 0.80
            }
            
            excludes = listOf(
                    "*.Config.*",
            )
        }
    }
}
```

### build.gradle.kts

```kotlin
plugins {
    java
    id("org.springframework.boot") version "3.1.2"
    id("io.spring.dependency-management") version "1.1.2"
    
    // 1. Jacoco 플러그인 추가
    id("jacoco")
}

group = "com.onebyte"
version = "0.0.1-SNAPSHOT"

java {
    sourceCompatibility = JavaVersion.VERSION_17
}

configurations {
    compileOnly {
        extendsFrom(configurations.annotationProcessor.get())
    }
}

repositories {
    mavenCentral()
}

dependencies {
    implementation("org.springframework.boot:spring-boot-starter-data-jpa")
    implementation("org.springframework.boot:spring-boot-starter-web")
    compileOnly("org.projectlombok:lombok")
    annotationProcessor("org.projectlombok:lombok")
    runtimeOnly("com.mysql:mysql-connector-j")

    /**
     * Test
     */
    testImplementation("org.springframework.boot:spring-boot-starter-test")
}


/**
 * Jacoco
 */
tasks {

    test {
        useJUnitPlatform()
        // 2. test 종료 후 jacocoTestReport 실행
        finalizedBy(jacocoTestReport)
    }

    // 3. 커버리지 결과를 html 파일로 가공
    jacocoTestReport {
        dependsOn(test)
    }

    // 4. 커버리지 기준을 만족하는지 확인해주는 task
    jacocoTestCoverageVerification {
        violationRules {
            rule {
                enabled = true

                element = "CLASS"

                limit {
                    counter = "BRANCH"
                    value = "COVEREDRATIO"
                    minimum = "0.80".toBigDecimal()
                }

                // 커버리지 체크 제외 클래스 지정
                excludes = listOf(
                    "*.Config.*",
                )
            }
        }
    }
}
```

# 파일 제외 설정하기

- 일반 설정

```kotlin
tasks.test {
  finalizedBy(tasks.jacocoTestReport)
}

tasks.jacocoTestReport {
  dependsOn(tasks.test)

  classDirectories.setFrom(files(classDirectories.files.map {
      fileTree(it) {
          setExcludes(listOf(
              "**/config/**",
              "**/Q*.*",
              "**/*SureMsg*.*",
              "**/auth/handler/**",
              "**/auth/filter/**",
          ))
      }
  }))
}
```

- 멀티 모듈에서 설정하기

```kotlin
subprojects {

	...

  tasks.test {
      finalizedBy(tasks.jacocoTestReport)
  }

  tasks.jacocoTestReport {
      dependsOn(tasks.test)

      classDirectories.setFrom(files(classDirectories.files.map {
          fileTree(it) {
              setExcludes(listOf(
                  "**/config/**",
                  "**/Q*.*",
                  "**/*SureMsg*.*",
                  "**/auth/handler/**",
                  "**/auth/filter/**",
              ))
          }
      }))
  }
	
	...
}
```