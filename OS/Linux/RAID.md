# RAID

- Redundant Array of Independent(Inexpensive) Disks
- 여러 개의 물리 디스크를 하나의 논리적 디스크로 묶어 **성능 향상**, **데이터 안정성**, 또는 **둘 다**를 달성하는 기술이다.
- 1988년 UC 버클리의 Patterson, Gibson, Katz가 논문 "A Case for Redundant Arrays of Inexpensive Disks (RAID)"에서 처음 제안했다.

## 왜 RAID가 필요한가?

단일 디스크의 한계:

| 문제 | 설명 |
|------|------|
| 용량 한계 | 하나의 디스크로는 대용량 데이터를 수용하기 어렵다 |
| 성능 한계 | 단일 디스크의 I/O 처리량(throughput)에는 물리적 한계가 있다 |
| 장애 취약성 | 디스크 하나가 고장나면 데이터 전체를 잃는다 |

RAID는 이 세 가지 문제를 여러 디스크를 조합하여 해결한다.

## 핵심 개념

### Striping (스트라이핑)

- 데이터를 일정 크기(stripe unit/chunk)로 나누어 여러 디스크에 **번갈아** 저장한다.
- 여러 디스크가 동시에 읽기/쓰기를 수행하므로 **성능이 향상**된다.

```
데이터: [A1][A2][A3][A4][A5][A6]

Disk 0: [A1][A3][A5]
Disk 1: [A2][A4][A6]
```

### Mirroring (미러링)

- 동일한 데이터를 **두 개 이상의 디스크에 복제**하여 저장한다.
- 하나의 디스크가 고장 나도 다른 디스크에서 데이터를 읽을 수 있어 **안정성이 향상**된다.

```
데이터: [A1][A2][A3]

Disk 0: [A1][A2][A3]  (원본)
Disk 1: [A1][A2][A3]  (복제)
```

### Parity (패리티)

- 데이터의 오류를 감지하고 **복구할 수 있는 추가 정보**를 계산하여 저장한다.
- 미러링보다 디스크 공간을 효율적으로 사용하면서도 장애 복구가 가능하다.
- XOR 연산으로 패리티를 계산한다.

```
Disk 0: 1010 (데이터)
Disk 1: 1100 (데이터)
Parity:  0110 (Disk 0 XOR Disk 1)

→ Disk 0이 고장나면: Parity XOR Disk 1 = 0110 XOR 1100 = 1010 (복구 완료)
```

### Stripe Unit / Chunk (스트라이프 단위)

RAID에서 데이터를 디스크에 분배하는 **최소 단위**를 chunk(또는 stripe unit)라고 한다. 보통 **64KB ~ 512KB** 크기이며, `mdadm`의 기본값은 512KB이다.

중요한 점은 **비트 단위로 쪼개서 흩뿌리는 게 아니라, chunk라는 의미 있는 덩어리 단위로 나눈다**는 것이다.

```
잘못된 이해 (비트 단위 분배):
  원본:  10110010 11001010 ...
  Disk 0: 1 1 0 1 ...     ← 비트를 한 개씩 분배?  ❌

실제 동작 (chunk 단위 분배):
  원본:  [128KB 덩어리][128KB 덩어리][128KB 덩어리]...
  Disk 0: [128KB 덩어리 통째로]                     ✅
  Disk 1: [128KB 덩어리 통째로]
```

예를 들어 256KB 파일을 chunk size 128KB인 RAID 5(디스크 3개)에 저장하면:

```
파일: photo.jpg (256KB), chunk size: 128KB

Disk 0: [chunk 1: 128KB]  ← photo.jpg의 앞 128KB (온전한 데이터)
Disk 1: [chunk 2: 128KB]  ← photo.jpg의 뒤 128KB (온전한 데이터)
Disk 2: [parity: 128KB]   ← chunk 1 XOR chunk 2 (복구용)
```

각 chunk는 해당 디스크에서 **독립적으로 읽을 수 있는 완전한 데이터**다. 그래서:

| 상황 | 읽기 시 접근하는 디스크 수 | 설명 |
|------|--------------------------|------|
| 정상 | **1개** | 데이터가 있는 디스크만 접근하면 된다 |
| 디스크 1개 장애 | **나머지 전부** | 패리티 + 나머지 데이터로 XOR 복구 |

패리티 계산도 이 chunk 단위로 비트별 XOR을 수행한다:

```
chunk 1: 10110010 01001101 ... (128KB = 1,048,576 bits)
chunk 2: 11001010 10110011 ... (128KB)
parity:  01111000 11111110 ... (각 비트 위치마다 XOR)
```

#### 쓰기 시의 동작 (Write Penalty)

데이터를 **쓸 때**는 패리티도 갱신해야 하므로 읽기보다 비용이 크다:

```
chunk 1을 chunk 1'로 수정하는 경우:

1. 기존 chunk 1 읽기       (Disk 0)
2. 기존 parity 읽기        (Disk 2)
3. 새 패리티 계산           parity' = chunk 1 XOR chunk 1' XOR parity
4. chunk 1' 쓰기           (Disk 0)
5. parity' 쓰기            (Disk 2)

→ 쓰기 1회에 읽기 2회 + 쓰기 2회 = 총 4회의 I/O 발생 (write penalty)
```

이것이 RAID 5의 쓰기 성능이 상대적으로 낮은 이유이고, DB처럼 쓰기가 빈번한 워크로드에 RAID 10을 권장하는 이유다.

### 스토리지 스택: 누가 chunk 분배를 하는가?

chunk 분배는 애플리케이션이나 파일시스템이 하는 것이 아니다. **RAID 계층**이 중간에서 투명하게 처리한다.

```
[애플리케이션]  ← photo.jpg를 저장해!
      ↓
[파일시스템]    ← ext4, xfs 등. "이 파일을 블록 단위로 디스크에 쓸게"
      ↓
[RAID 계층]    ← "받은 블록을 chunk 단위로 디스크들에 분배할게"  ★ 여기서 분배
      ↓
[물리 디스크]   ← 실제로 데이터가 기록됨
```

파일시스템은 RAID 어레이(`/dev/md0`)를 **그냥 하나의 큰 디스크로 본다.** 뒤에서 여러 디스크로 구성되어 있다는 사실을 모른다.

분배 주체는 RAID 종류에 따라 다르다:

| 종류 | 분배 주체 | 설명 |
|------|----------|------|
| Hardware RAID | **RAID 컨트롤러** (전용 프로세서) | CPU는 관여하지 않음. 컨트롤러 자체 CPU로 처리 |
| Software RAID (mdadm) | **커널의 md 드라이버** (CPU에서 실행) | CPU가 처리하지만, 단순 메모리 복사 수준이라 부하가 거의 없음 |

### 파일 형태는 변하는가?

**변하지 않는다.** 파일이 쪼개지거나 형태가 바뀌는 게 아니다.

```
1) 애플리케이션이 photo.jpg(256KB)를 /mnt/raid/에 저장한다

2) 파일시스템(ext4)은 /dev/md0이라는 "하나의 디스크"에 평소처럼 쓴다
   → inode 생성, 데이터 블록 할당 등 일반 디스크와 완전히 동일

3) md0에 쓰인 데이터가 물리 디스크로 내려갈 때 RAID 계층이 개입한다:

   ext4가 md0에 쓴 데이터:
   [         256KB 연속 데이터          ]

   RAID 계층이 chunk(128KB) 단위로 분배:
   Disk 0: [앞 128KB]
   Disk 1: [뒤 128KB]
   Disk 2: [parity ]
```

마운트된 디렉토리에서 보면 **완전히 정상적인 파일**이다:

```bash
$ ls -la /mnt/raid/photo.jpg
-rw-r--r-- 1 user user 262144 Mar 15 photo.jpg

$ file /mnt/raid/photo.jpg
photo.jpg: JPEG image data    # 그냥 정상적인 JPEG 파일
```

chunk로 나뉘어 있다는 것은 물리 디스크 레벨의 이야기이고, 파일시스템 위에서는 전혀 알 수 없다.

## RAID 레벨

### RAID 0 - Striping

```
Disk 0: [A1][A3][A5]
Disk 1: [A2][A4][A6]
```

| 항목 | 내용 |
|------|------|
| 최소 디스크 수 | 2개 |
| 사용 가능 용량 | 디스크 수 × 개별 디스크 용량 (100%) |
| 읽기 성능 | 매우 빠름 (디스크 수에 비례) |
| 쓰기 성능 | 매우 빠름 (디스크 수에 비례) |
| 내결함성 | **없음** - 디스크 하나만 고장나도 전체 데이터 손실 |
| 용도 | 임시 데이터, 캐시, 영상 편집 등 성능이 중요하고 손실을 감수할 수 있는 경우 |

### RAID 1 - Mirroring

```
Disk 0: [A1][A2][A3]  (원본)
Disk 1: [A1][A2][A3]  (미러)
```

| 항목 | 내용 |
|------|------|
| 최소 디스크 수 | 2개 |
| 사용 가능 용량 | 개별 디스크 용량 (50%) |
| 읽기 성능 | 빠름 (두 디스크에서 동시 읽기 가능) |
| 쓰기 성능 | 단일 디스크와 유사 (양쪽 모두 기록해야 함) |
| 내결함성 | 1개 디스크 장애 허용 |
| 용도 | OS 부팅 디스크, 중요 시스템 로그 등 안정성이 최우선인 경우 |

### RAID 4 - Striping with Dedicated Parity

```
Disk 0: [A1][B1][C1][D1]  ← 데이터
Disk 1: [A2][B2][C2][D2]  ← 데이터
Disk 2: [A3][B3][C3][D3]  ← 데이터
Disk 3: [Ap][Bp][Cp][Dp]  ← 패리티 전용 디스크
```

| 항목 | 내용 |
|------|------|
| 최소 디스크 수 | 3개 |
| 사용 가능 용량 | (디스크 수 - 1) × 개별 디스크 용량 |
| 읽기 성능 | 빠름 (스트라이핑 효과) |
| 쓰기 성능 | **낮음** (패리티 디스크가 병목) |
| 내결함성 | 1개 디스크 장애 허용 |
| 용도 | 거의 사용하지 않음 (RAID 5로 대체됨) |

- RAID 4의 치명적 문제는 **패리티 디스크 병목**이다. 어떤 데이터를 수정하든 패리티 디스크에 반드시 쓰기가 발생하므로, 동시 쓰기가 많아지면 패리티 디스크 하나가 전체 성능을 제한한다.
- RAID 5는 이 문제를 해결하기 위해 패리티를 모든 디스크에 분산시킨 것이다.

### RAID 5 - Striping with Distributed Parity

```
Disk 0: [A1][B2][C3][Dp]
Disk 1: [A2][B3][Cp][D1]
Disk 2: [A3][Bp][C1][D2]
Disk 3: [Ap][B1][C2][D3]

(p = 패리티 블록, 각 스트라이프마다 패리티가 다른 디스크에 분산됨)
```

| 항목 | 내용 |
|------|------|
| 최소 디스크 수 | 3개 |
| 사용 가능 용량 | (디스크 수 - 1) × 개별 디스크 용량 |
| 읽기 성능 | 빠름 (스트라이핑 효과) |
| 쓰기 성능 | 보통 (패리티 계산 오버헤드) |
| 내결함성 | 1개 디스크 장애 허용 |
| 용도 | 웹 서버, 파일 서버 등 범용적인 용도 |

- 패리티를 분산 저장하는 이유: 특정 디스크에 패리티를 집중시키면(RAID 4) 쓰기 시 해당 디스크가 병목이 된다.

### RAID 6 - Striping with Double Distributed Parity

```
Disk 0: [A1][B2][Cp][Dp][E1]
Disk 1: [A2][Bp][Cq][D1][Eq]
Disk 2: [Ap][Bq][C1][D2][E2]
Disk 3: [Aq][B1][C2][Dq][Ep]

(p, q = 서로 다른 알고리즘의 패리티 블록)
```

| 항목 | 내용 |
|------|------|
| 최소 디스크 수 | 4개 |
| 사용 가능 용량 | (디스크 수 - 2) × 개별 디스크 용량 |
| 읽기 성능 | 빠름 |
| 쓰기 성능 | RAID 5보다 느림 (이중 패리티 계산) |
| 내결함성 | **2개** 디스크 동시 장애 허용 |
| 용도 | 대용량 스토리지, NAS, 디스크 수가 많은 어레이 |

- RAID 5에서 디스크 1개가 고장나면 리빌드 중에 추가 고장이 발생할 위험이 있다. RAID 6은 이를 방지한다.

### RAID 10 (1+0) - Mirroring + Striping

RAID 1(미러링)과 RAID 0(스트라이핑)을 **조합**한 구성이다. 이름의 "1+0"이 의미하는 것은 **먼저 RAID 1로 미러 쌍을 만들고, 그 쌍들을 RAID 0으로 스트라이핑**한다는 뜻이다.

#### 구성 과정을 단계별로 보면

디스크 4개(Disk 0~3)에 데이터 `[A1][A2][A3][A4]`를 저장한다고 하자.

**Step 1: 미러 쌍을 구성한다 (RAID 1)**

```
미러 쌍 0: Disk 0 + Disk 1  → 서로 동일한 데이터를 저장
미러 쌍 1: Disk 2 + Disk 3  → 서로 동일한 데이터를 저장
```

**Step 2: 미러 쌍들을 스트라이핑한다 (RAID 0)**

```
데이터 [A1][A2][A3][A4]를 chunk 단위로 미러 쌍에 번갈아 분배:

미러 쌍 0 → [A1][A3]
미러 쌍 1 → [A2][A4]
```

**최종 결과: 4개 디스크의 실제 저장 상태**

```
              미러 쌍 0              미러 쌍 1
          Disk 0    Disk 1      Disk 2    Disk 3
          [A1]      [A1]        [A2]      [A2]
          [A3]      [A3]        [A4]      [A4]
           │          │           │          │
           └──동일────┘           └──동일────┘
```

- Disk 0과 Disk 1은 **완전히 같은 데이터**를 갖고 있다.
- Disk 2와 Disk 3도 **완전히 같은 데이터**를 갖고 있다.
- 미러 쌍 0과 미러 쌍 1은 **서로 다른 데이터**를 갖고 있다 (스트라이핑).

#### 읽기 동작

```
"A1을 읽고 싶다"
  → 미러 쌍 0에 있다
  → Disk 0 또는 Disk 1 중 아무 데서나 읽으면 된다
  → 두 디스크에 동시에 다른 읽기 요청을 분산할 수도 있다 (읽기 성능 향상)

"A1과 A2를 동시에 읽고 싶다"
  → A1은 미러 쌍 0에서, A2는 미러 쌍 1에서 동시에 읽는다 (스트라이핑 효과)
```

#### 쓰기 동작

```
"A1을 A1'로 수정"
  → Disk 0에 A1' 쓰기
  → Disk 1에 A1' 쓰기 (미러이므로 양쪽 모두)
  → 패리티 계산 없음! 단순히 두 디스크에 같은 데이터를 쓰면 끝

RAID 5와 비교:
  RAID 5  → 읽기 2회 + 쓰기 2회 = 4 I/O (패리티 계산 필요)
  RAID 10 → 쓰기 2회 = 2 I/O (미러에 복제만 하면 됨)
```

이것이 RAID 10의 쓰기 성능이 RAID 5보다 좋은 이유다.

#### 장애 허용: 어떤 디스크가 고장나도 되는가?

```
              미러 쌍 0              미러 쌍 1
          Disk 0    Disk 1      Disk 2    Disk 3

Case 1: Disk 0 고장 → Disk 1이 살아있으므로 OK          ✅
Case 2: Disk 2 고장 → Disk 3이 살아있으므로 OK          ✅
Case 3: Disk 0 + Disk 2 동시 고장 → 각 쌍에 1개씩 남음  ✅
Case 4: Disk 0 + Disk 1 동시 고장 → 미러 쌍 0 전멸      ❌ 데이터 손실
```

핵심: **같은 미러 쌍 내에서 2개가 동시에 죽지만 않으면** 데이터는 안전하다.

#### RAID 10 vs RAID 01 (0+1) — 순서가 왜 중요한가?

RAID 10은 "먼저 미러, 그 위에 스트라이프"이고, RAID 01은 그 반대다.

```
RAID 10 (1+0): 미러 먼저 → 스트라이프
         RAID 0
        /      \
   RAID 1    RAID 1
   D0  D1    D2  D3

RAID 01 (0+1): 스트라이프 먼저 → 미러
         RAID 1
        /      \
   RAID 0    RAID 0
   D0  D1    D2  D3
```

RAID 01에서 Disk 0이 고장나면 **스트라이프 그룹 전체**(RAID 0 그룹)가 무력화된다. 이후 Disk 2 또는 Disk 3 중 하나만 더 고장나면 데이터를 잃는다. RAID 10은 Disk 0이 고장나도 미러 쌍 0의 Disk 1이 살아있으므로 나머지 쌍에는 영향이 없다. **RAID 10이 RAID 01보다 장애에 강하므로 실무에서는 항상 RAID 10을 사용한다.**

| 항목 | 내용 |
|------|------|
| 최소 디스크 수 | 4개 (항상 짝수) |
| 사용 가능 용량 | 전체 용량의 50% |
| 읽기 성능 | 매우 빠름 (스트라이핑 + 미러 분산 읽기) |
| 쓰기 성능 | 빠름 (패리티 계산 없이 미러 복제만) |
| 내결함성 | 각 미러 쌍에서 1개씩 장애 허용 |
| 용도 | **데이터베이스 서버**, 고성능이면서 안정성이 필요한 경우 |

RAID 10은 성능과 안정성 모두를 갖추어 **프로덕션 DB 서버에서 가장 널리 사용**되는 구성이다. 용량 효율(50%)을 희생하는 대신, RAID 5의 write penalty 없이 빠른 쓰기 성능과 확실한 이중화를 보장한다.

### RAID 레벨 비교 요약

| RAID | 최소 디스크 | 사용 용량 | 읽기 | 쓰기 | 내결함성 | 주 용도 |
|------|------------|-----------|------|------|----------|--------|
| 0 | 2 | 100% | 최고 | 최고 | 없음 | 캐시, 임시 |
| 1 | 2 | 50% | 좋음 | 보통 | 1개 | 부팅, 로그 |
| 5 | 3 | (N-1)/N | 좋음 | 보통 | 1개 | 파일 서버 |
| 6 | 4 | (N-2)/N | 좋음 | 낮음 | 2개 | 대용량 NAS |
| 10 | 4 | 50% | 최고 | 좋음 | 미러당 1개 | DB 서버 |

## Hardware RAID vs Software RAID

### Hardware RAID

- 별도의 **RAID 컨트롤러 카드**(HBA with RAID)가 RAID 로직을 처리한다.
- CPU 부하 없이 독립적으로 동작하며, BBU(Battery Backup Unit)를 통해 정전 시에도 캐시 데이터를 보호한다.
- BIOS/UEFI 레벨에서 설정하므로 OS는 RAID 구성을 인식하지 못하고 단일 디스크로 본다.

```
[OS] → [RAID Controller] → [Disk 0]
                          → [Disk 1]
                          → [Disk 2]
```

장점:
- CPU 자원을 소모하지 않음
- BBU를 통한 write-back 캐시로 쓰기 성능 극대화
- OS 독립적 (OS를 재설치해도 어레이 유지)

단점:
- 고가 (컨트롤러 + BBU)
- 벤더 종속 (컨트롤러 고장 시 동일 모델이 필요할 수 있음)

### Software RAID (mdadm)

- 리눅스 커널의 `md`(multiple device) 드라이버가 RAID 로직을 처리한다.
- `mdadm` 유틸리티로 생성, 관리, 모니터링한다.
- 별도의 하드웨어 없이 소프트웨어로 구현하므로 비용이 들지 않는다.

```
[OS / md driver] → [Disk 0]
                 → [Disk 1]
                 → [Disk 2]
```

장점:
- 무료 (추가 하드웨어 불필요)
- 하드웨어 벤더에 종속되지 않음
- 디스크를 다른 시스템으로 이동해도 어레이 재구성 가능

단점:
- CPU 자원 일부 소모 (현대 CPU에서는 거의 무시할 수준)
- BBU 없이 write-back 캐시를 사용하면 정전 시 데이터 손실 위험

## 실습: mdadm으로 Software RAID 구성하기

### 환경 준비

실습 환경은 두 가지 방법으로 구성할 수 있다.

**방법 1: 가상 머신 (VirtualBox / VMware)**

```bash
# VirtualBox에서 VM에 가상 디스크 추가
# VM 설정 → Storage → SATA 컨트롤러에 VDI 디스크 4개 추가 (각 1GB)
# → /dev/sdb, /dev/sdc, /dev/sdd, /dev/sde 로 인식됨
```

**방법 2: 루프백 디바이스 (실제 디스크 없이 파일로 실습)**

```bash
# 가상 디스크 파일 4개 생성 (각 100MB)
for i in 0 1 2 3; do
    dd if=/dev/zero of=/tmp/disk${i}.img bs=1M count=100
done

# 루프백 디바이스에 연결
for i in 0 1 2 3; do
    sudo losetup /dev/loop${i} /tmp/disk${i}.img
done

# 연결 확인
losetup -a
```

### mdadm 설치

```bash
# Debian/Ubuntu
sudo apt install mdadm

# RHEL/CentOS/Rocky
sudo yum install mdadm
# 또는
sudo dnf install mdadm
```

### RAID 1 구성 실습

```bash
# 1) RAID 1 어레이 생성 (미러링, 디스크 2개)
sudo mdadm --create /dev/md0 \
    --level=1 \
    --raid-devices=2 \
    /dev/loop0 /dev/loop1

# 2) 어레이 상태 확인
cat /proc/mdstat
sudo mdadm --detail /dev/md0

# 3) 파일시스템 생성 및 마운트
sudo mkfs.ext4 /dev/md0
sudo mkdir -p /mnt/raid1
sudo mount /dev/md0 /mnt/raid1

# 4) 테스트 데이터 쓰기
echo "RAID 1 test data" | sudo tee /mnt/raid1/test.txt

# 5) 디스크 장애 시뮬레이션
sudo mdadm --manage /dev/md0 --fail /dev/loop1
cat /proc/mdstat  # [U_] - 디스크 하나가 faulty 상태

# 6) 데이터가 여전히 살아있는지 확인
cat /mnt/raid1/test.txt  # 정상 출력됨

# 7) 장애 디스크 제거 후 새 디스크로 교체
sudo mdadm --manage /dev/md0 --remove /dev/loop1
sudo mdadm --manage /dev/md0 --add /dev/loop1    # 실제로는 새 디스크
cat /proc/mdstat  # 리빌드 진행 상태 확인
```

### RAID 5 구성 실습

```bash
# 1) RAID 5 어레이 생성 (디스크 3개 + 스페어 1개)
sudo mdadm --create /dev/md1 \
    --level=5 \
    --raid-devices=3 \
    --spare-devices=1 \
    /dev/loop0 /dev/loop1 /dev/loop2 /dev/loop3

# 2) 동기화 진행률 확인
watch cat /proc/mdstat

# 3) 상세 정보 확인
sudo mdadm --detail /dev/md1

# 4) 파일시스템 생성 및 마운트
sudo mkfs.ext4 /dev/md1
sudo mkdir -p /mnt/raid5
sudo mount /dev/md1 /mnt/raid5

# 5) 장애 시뮬레이션 - 스페어가 자동으로 리빌드 시작하는지 확인
sudo mdadm --manage /dev/md1 --fail /dev/loop1
cat /proc/mdstat  # spare가 자동으로 리빌드에 투입되는 것을 확인
```

### RAID 10 구성 실습

```bash
# 1) RAID 10 어레이 생성 (디스크 4개)
sudo mdadm --create /dev/md2 \
    --level=10 \
    --raid-devices=4 \
    /dev/loop0 /dev/loop1 /dev/loop2 /dev/loop3

# 2) 상태 확인
sudo mdadm --detail /dev/md2

# 3) 파일시스템 생성 및 마운트
sudo mkfs.ext4 /dev/md2
sudo mkdir -p /mnt/raid10
sudo mount /dev/md2 /mnt/raid10
```

### 어레이 영구 설정 (재부팅 후에도 유지)

```bash
# mdadm 설정 파일에 어레이 정보 저장
sudo mdadm --detail --scan | sudo tee -a /etc/mdadm/mdadm.conf

# initramfs 업데이트 (부팅 시 어레이를 자동 조립하도록)
sudo update-initramfs -u

# /etc/fstab에 마운트 정보 추가
echo '/dev/md0 /mnt/raid1 ext4 defaults 0 0' | sudo tee -a /etc/fstab
```

### 실습 정리

```bash
# 마운트 해제
sudo umount /mnt/raid1

# 어레이 중지
sudo mdadm --stop /dev/md0

# 슈퍼블록 제거 (디스크에서 RAID 메타데이터 삭제)
sudo mdadm --zero-superblock /dev/loop0
sudo mdadm --zero-superblock /dev/loop1

# 루프백 디바이스 해제
for i in 0 1 2 3; do
    sudo losetup -d /dev/loop${i}
done

# 가상 디스크 파일 삭제
rm /tmp/disk*.img
```

## 온프레미스 운영 가이드

실제 온프레미스 서버를 운영한다고 했을 때, 용도별 RAID 구성 전략과 운영 방법을 정리한다.

### 서버 용도별 RAID 구성 전략

#### 1) 데이터베이스 서버 (MySQL, PostgreSQL)

```
[OS 영역]       RAID 1   ×2 SSD (부팅 + OS)
[데이터 영역]   RAID 10  ×4 SSD (DB 데이터 파일)
[로그 영역]     RAID 1   ×2 SSD (WAL / binlog)
[백업 영역]     RAID 5   ×3 HDD (DB 덤프, 아카이브)
```

- DB 데이터 영역은 RAID 10이 사실상 표준이다. 랜덤 I/O 성능과 내결함성을 동시에 확보한다.
- WAL/binlog는 순차 쓰기가 주이므로 RAID 1로 충분하다. 데이터 영역과 물리적으로 분리하면 I/O 경합을 줄일 수 있다.

#### 2) 웹/애플리케이션 서버

```
[OS + App]      RAID 1   ×2 SSD
[로그]          RAID 1   ×2 HDD (또는 원격 로그 서버로 전송)
```

- 웹 서버는 stateless하게 운영하는 것이 일반적이므로 복잡한 RAID 구성이 필요 없다.
- 애플리케이션 코드와 OS만 보호하면 된다.

#### 3) 파일/NAS 서버

```
[OS 영역]       RAID 1   ×2 SSD
[데이터 영역]   RAID 6   ×6 HDD 이상 (대용량 파일 저장)
[핫 스페어]     ×1 HDD  (자동 리빌드용)
```

- 디스크 수가 많은 환경에서는 RAID 5보다 RAID 6을 권장한다.
- 대용량 HDD(4TB+)의 리빌드 시간이 길어서 리빌드 중 추가 장애 위험이 높기 때문이다.
- 핫 스페어를 반드시 구성하여 장애 발생 시 자동으로 리빌드가 시작되도록 한다.

#### 4) 로그/모니터링 서버 (ELK, Prometheus)

```
[OS 영역]       RAID 1   ×2 SSD
[데이터 영역]   RAID 5   ×4 SSD (시계열 데이터, 인덱스)
```

- 로그/메트릭 데이터는 재수집이 가능한 경우가 많으므로 RAID 5로 충분하다.
- 쓰기가 많으므로 SSD를 사용하는 것이 좋다.

### 운영 시 모니터링

#### mdadm 모니터링 설정

```bash
# 이메일 알림 설정 (/etc/mdadm/mdadm.conf)
MAILADDR admin@example.com

# mdadm 모니터 데몬이 주기적으로 어레이 상태를 확인한다
# 장애 발생 시 이메일 또는 스크립트로 알림을 보낸다
sudo systemctl enable mdmonitor
sudo systemctl start mdmonitor
```

#### 상태 확인 스크립트

```bash
#!/bin/bash
# /usr/local/bin/check-raid.sh

RAID_STATUS=$(cat /proc/mdstat)

# degraded 상태 감지
if echo "$RAID_STATUS" | grep -q '\[.*_.*\]'; then
    echo "RAID DEGRADED detected!" >&2
    echo "$RAID_STATUS" >&2
    # 슬랙 웹훅 등으로 알림 전송
    exit 1
fi

echo "All RAID arrays healthy"
exit 0
```

```bash
# cron으로 5분마다 체크
*/5 * * * * /usr/local/bin/check-raid.sh || /usr/local/bin/send-alert.sh
```

#### 주기적으로 확인해야 할 항목

| 항목 | 명령어 | 주기 |
|------|--------|------|
| 어레이 상태 | `cat /proc/mdstat` | 실시간 모니터링 |
| 상세 정보 | `mdadm --detail /dev/mdX` | 일 1회 |
| 디스크 S.M.A.R.T | `smartctl -a /dev/sdX` | 주 1회 |
| 리빌드 진행률 | `cat /proc/mdstat` | 리빌드 중 수시 |
| 디스크 온도 | `smartctl -a /dev/sdX \| grep Temp` | 일 1회 |

### 장애 대응 절차

#### 디스크 장애 발생 시

```
1. 알림 수신 (mdmonitor / 모니터링 시스템)
    ↓
2. 상태 확인
   $ cat /proc/mdstat
   $ mdadm --detail /dev/md0
    ↓
3. 장애 디스크 식별
   $ smartctl -a /dev/sdX    # S.M.A.R.T 정보 확인
    ↓
4-a. 핫 스페어가 있는 경우
     → 자동 리빌드 시작, 진행률 모니터링
     → 리빌드 완료 후 장애 디스크 교체하여 새 스페어로 추가
    ↓
4-b. 핫 스페어가 없는 경우
     → 장애 디스크 제거: mdadm --manage /dev/md0 --remove /dev/sdX
     → 물리적으로 디스크 교체 (핫스왑 지원 시 서버 중단 불필요)
     → 새 디스크 추가: mdadm --manage /dev/md0 --add /dev/sdY
     → 리빌드 진행률 모니터링
    ↓
5. 리빌드 완료 확인
   $ cat /proc/mdstat    # [UUU] 상태 확인
    ↓
6. 사후 조치
   - 교체된 디스크의 S/N 기록
   - 같은 배치의 디스크가 다른 슬롯에 있는지 확인 (동시 고장 위험)
   - 인벤토리 업데이트
```

### 운영 베스트 프랙티스

1. **핫 스페어는 반드시 구성한다**
   - RAID 5/6 환경에서는 항상 1개 이상의 핫 스페어를 두어 장애 즉시 자동 리빌드가 시작되도록 한다.
   - 리빌드에 걸리는 시간이 곧 데이터가 위험에 노출되는 시간이다.

2. **같은 제조사, 같은 배치 디스크를 피한다**
   - 동일 배치 디스크는 비슷한 시기에 함께 고장나는 경향이 있다.
   - 가능하면 제조사나 구매 시기를 분산시킨다.

3. **RAID는 백업이 아니다**
   - RAID는 디스크 **장애**로부터 보호하지만, 실수로 인한 삭제, 랜섬웨어, 논리적 오류는 막지 못한다.
   - **반드시 별도의 백업 전략**(원격 백업, 스냅샷 등)을 함께 운용해야 한다.

4. **S.M.A.R.T 모니터링을 활성화한다**
   - 디스크가 완전히 고장나기 전에 징후를 포착하여 선제적으로 교체할 수 있다.
   - `smartd` 데몬으로 자동 모니터링하고, `Reallocated_Sector_Ct`, `Current_Pending_Sector` 값을 주시한다.

5. **리빌드 중에는 I/O 부하를 줄인다**
   - 리빌드는 디스크에 큰 부하를 주므로, 리빌드 중 추가 장애 위험이 높아진다.
   - 가능하면 리빌드 중에는 배치 작업이나 대규모 쿼리를 자제한다.
   - 리빌드 속도 조절: `echo 50000 > /proc/sys/dev/raid/speed_limit_min`

6. **정기적으로 어레이 일관성을 검사한다**
   - 주기적으로 scrub/check를 수행하여 비트 로트(silent data corruption)를 감지한다.
   - `echo check > /sys/block/md0/md/sync_action`
   - 월 1회 정도가 적당하다.

## RAID + LVM 조합

실무에서는 RAID 위에 LVM을 올려 사용하는 것이 일반적이다. RAID의 안정성 위에 LVM의 유연한 볼륨 관리를 더할 수 있다.

```
물리 디스크:  /dev/sda  /dev/sdb  /dev/sdc  /dev/sdd
                 ↓         ↓         ↓         ↓
RAID 10:            /dev/md0 (mdadm)
                         ↓
LVM PV:             /dev/md0 (pvcreate)
                         ↓
LVM VG:             vg_data (vgcreate)
                    /         \
LVM LV:        lv_db        lv_log
                 ↓              ↓
Mount:       /var/lib/mysql   /var/log
```

```bash
# RAID 어레이 위에 LVM 구성
sudo pvcreate /dev/md0
sudo vgcreate vg_data /dev/md0
sudo lvcreate -L 50G -n lv_db vg_data
sudo lvcreate -L 10G -n lv_log vg_data

sudo mkfs.ext4 /dev/vg_data/lv_db
sudo mkfs.ext4 /dev/vg_data/lv_log

sudo mount /dev/vg_data/lv_db /var/lib/mysql
sudo mount /dev/vg_data/lv_log /var/log
```

이렇게 하면 RAID가 디스크 장애를 방어하고, LVM이 필요에 따라 볼륨 크기를 유연하게 조정할 수 있다.
