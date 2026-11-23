# PocketUnreal

> 언리얼 UObject 시스템의 핵심인 **리플렉션 · RPC · GC**를 C++20으로 구현했습니다.

GitHub : https://github.com/siyoung-devops/SimpleUnreal

<br>

## 시작한 계기

언리얼 엔진에서는 멤버에 `UPROPERTY` · `UFUNCTION` 매크로만 붙이면 에디터 노출, 네트워크 함수 호출, 메모리 해제를 엔진이 처리해 줍니다.
매크로 뒤에서 이 기능들이 어떤 구조로 동작하는지 이해하고 싶어, 핵심 구조를 C++로 직접 구현해 보았습니다.

| 언리얼 엔진 | PocketUnreal | 하는 일 |
| --- | --- | --- |
| Unreal Header Tool | `HeaderParser` → `Generated.cpp` | 헤더를 읽어 클래스 등록 코드 생성 |
| `FProperty` | `FProperty` + 타입별 6종 | 멤버 이름 · 위치 · 값 변환 |
| `UFunction` | `FFunction` / `TFunction` | 함수를 이름으로 찾아 호출 |
| `GUObjectArray` | `FObjectArray` | 모든 오브젝트를 담는 전역 배열 |
| Mark & Sweep GC | `FGarbageCollector` | 안 쓰는 오브젝트 찾아서 삭제 |

## 목표와 결과

**오브젝트 10만 개에서 GC 때문에 한 번에 멈추는 시간을 10ms 이내로 만드는 것**을 목표로 잡았습니다.

- 8가지 상황을 만들어 30회씩 측정했고, **모든 상황에서 10ms 이내**로 나왔습니다. (가장 오래 걸린 Cycle 평균 5.6ms, 30회 중 최악 9.3ms)

## 구현 기능

| | 기능 | 내용 |
| :---: | --- | --- |
| 01 | **리플렉션** | 헤더에 `UPROPERTY()` · `UFUNCTION()` 표시만 하면, 빌드 전에 `HeaderParser`가 등록 코드를 만들도록 구현했습니다. |
| 02 | **RPC** | `"SetInfo Lee 30"` 같은 문자열 한 줄로 함수를 찾아 인자를 변환해서 호출하도록 구현했습니다. |
| 03 | **GC** | 루트에서 닿지 않는 오브젝트를 지우는 Mark & Sweep을 구현했습니다. |
| 04 | **GC 최적화** | 10만 개 · 8가지 상황을 직접 측정하면서 3단계에 걸쳐 개선했습니다. |

<br>

## 빌드 & 실행

- Visual Studio 2026 (v145) · C++20 · Windows x64
- `PocketUnreal.slnx`를 열고 **`Release | x64`** 로 실행합니다. (Ctrl + F5)
- 빌드하면 `HeaderParser.exe`가 먼저 실행되어 `Generated.cpp`를 새로 만들도록 설정했습니다.
- 실행하면 GC 시나리오 8개를 돌린 뒤 리플렉션 · RPC 데모를 출력합니다.

## 전체 구조

```
PocketUnreal/
├─ HeaderParser/HeaderParser.cpp    # UHT 역할: 헤더를 읽어 Generated.cpp 생성
└─ PocketUnreal/
   ├─ Headers/ · Source/            # 헤더와 cpp를 나누고, 안은 같은 폴더 구조
   │  ├─ Reflection/
   │  │  ├─ UClass                  # 클래스 정보: 프로퍼티 · 함수 · GC 참조 스키마
   │  │  ├─ ReflectionMacro.h       # GENERATED_BODY / UPROPERTY / UFUNCTION
   │  │  ├─ F(Int|Float|Bool|Str|Object|Array)Property
   │  │  └─ CreateProperty.h        # C++ 타입에 맞는 프로퍼티 자동 선택
   │  ├─ RPC/
   │  │  └─ FFunction / TFunction   # 문자열 인자를 타입으로 바꿔 멤버 함수 호출
   │  ├─ GC/
   │  │  ├─ FObjectArray            # 전역 오브젝트 배열 (등록 · 빈 칸 재사용)
   │  │  └─ FGarbageCollector       # Mark / Sweep
   │  ├─ UObject                    # 모든 오브젝트의 부모
   │  └─ UCompany / UCoworker       # 예제 클래스
   ├─ Source/Generated.cpp          # (자동 생성) 클래스 등록 코드
   └─ Source/main.cpp               # GC 시나리오 8개 + 리플렉션 / RPC 데모
```

- **`UObject`** 는 자기 클래스 정보(`UClass`)와, 전역 배열에서 자기가 몇 번 칸에 있는지(`InternalIndex`)를 들고 있습니다.
- **`UClass`** 는 멤버와 함수 목록, 그리고 GC가 쓸 **참조 스키마**를 들고 있습니다.
- **`FGarbageCollector`** 는 전역 배열과 참조 스키마만 보고 동작하도록 해서, 구체적인 클래스를 전혀 모릅니다.

<br>

## 1. 리플렉션

C++은 실행 중에 "이 클래스에 어떤 멤버가 있는지" 알 수 없습니다.
그래서 언리얼처럼 **헤더에는 표시만 해 두고, 빌드 전에 도구가 그 표시를 읽어 등록 코드를 만들도록** 구현했습니다.

```cpp
// UCoworker.h — 평범한 C++ 선언에 표시만
class UCoworker : public UObject
{
public:
	GENERATED_BODY();

	UFUNCTION()
	void SetInfo(const std::string& InName, const int InAge);

private:
	UPROPERTY()
	int Age = 40;
};
```

```cpp
// Generated.cpp — HeaderParser가 자동으로 만든 코드
ClassInstance.AddProperty(CreateProperty<decltype(UCoworker::Age)>("Age", offsetof(UCoworker, Age)));
ClassInstance.AddFunction(CreateFunction("SetInfo", &UCoworker::SetInfo));
```

- 프로퍼티는 **이름 + 오프셋**으로 관리했습니다. 오브젝트 주소에 오프셋을 더하면 멤버 주소가 됩니다.
- `CreateProperty<T>`가 `if constexpr`로 타입에 맞는 프로퍼티(`int`, `float`, `bool`, `string`, `UObject*`, `vector`)를 고르고, 지원하지 않는 타입은 `static_assert`로 **빌드할 때** 막도록 했습니다.
- 프로퍼티를 등록할 때 오브젝트 포인터인 멤버는 `{오프셋, 종류}`를 따로 저장해서 **GC용 참조 스키마**도 같이 만들도록 했습니다.

**이렇게 만든 이유**
- 처음에는 `UPROPERTY(int, Age) = 40;`처럼 매크로가 등록 코드를 만들었는데, 선언이 일반 C++과 달라지고 멤버마다 정적 변수가 생겼습니다. 그래서 **선언은 평범한 C++로 두고 도구가 따로 등록 코드를 만드는** 방식으로 바꿨습니다.
- 파서는 이름만 읽고, 타입은 `decltype`으로 **컴파일러가 판단**하게 해서 파서를 단순하게 유지했습니다.

<br>

## 2. RPC

문자열 한 줄로 함수를 찾아서 호출하도록 구현했습니다. 네트워크로 받은 메시지를 실제 함수 호출로 바꾸는 과정을 생각하면서 만들었습니다.

```cpp
Coworker->CallFunction("SetInfo Lee 30");
Coworker->CallFunction("AddAge 5");        // 출력: AddAge Result : 35
```

```cpp
// TFunction.h (요약)
std::vector<std::string> ArgTexts = SplitArgs(InArgs, sizeof...(ArgTypes));   // 공백으로 나누고 개수 검사

size_t Index = 0;
std::tuple<ArgTypes...> CallArgs{ FromString<ArgTypes>(ArgTexts[Index++])... };   // 문자열 → 타입

ReturnType ReturnValue = std::apply(CallMember, CallArgs);                         // 튜플을 풀어서 호출
return ToString(ReturnValue);                                                      // 반환값 → 문자열
```

- 인자 변환은 따로 만들지 않고 **리플렉션의 `ImportText` / `ExportText`를 재사용**해서, 리플렉션과 RPC가 같은 규칙으로 변환되도록 했습니다.
- 함수 호출 괄호 `( )` 안은 인자 계산 순서가 정해져 있지 않아서, **왼쪽부터 순서가 보장되는 중괄호 `{ }` 초기화**로 인자를 차례대로 꺼냈습니다.
- 없는 함수, 인자 개수 틀림, 숫자가 아닌 값, `true/false`가 아닌 값이 들어와도 죽지 않고 이유를 출력하도록 했습니다.

<br>

## 3. 가비지 컬렉션

**루트에서 출발해서 닿을 수 있는 오브젝트만 살리고, 나머지는 지우도록** 구현했습니다.

```cpp
UCompany* Company = UObject::NewObject<UCompany>();
Company->AddToRoot();                                   // 루트: GC가 출발하는 지점

FGCStats Stats = FGarbageCollector::Collect();          // Mark + Sweep
```

| 단계 | 하는 일 |
| --- | --- |
| **Mark** | 루트에서 참조 스키마를 따라가며 살아 있는 오브젝트 표시 |
| **Sweep** | 표시 안 된 오브젝트를 `delete`하고, 빈 칸은 재사용 목록으로 |

```
FObjectArray::ItemList  (칸 번호 = 오브젝트의 InternalIndex)
┌──────────────┬──────────────┬──────────────┐
│ Object ─→ A  │ nullptr      │ Object ─→ C  │
│ bRooted  ✔   │  (빈 칸)     │ bRooted      │
│ bMarked      │              │ bMarked      │
└──────────────┴──────────────┴──────────────┘
FreeIndexList = [1]   ← 다음 NewObject가 재사용
```

- Mark는 재귀 대신 **작업 목록(`WorkList`)** 을 써서, 깊이 10만인 구조에서도 스택 오버플로가 나지 않도록 했습니다.
- 작업 목록에 **넣는 순간 표시**해서 순환 참조가 있어도 무한 루프 없이 끝납니다.
- 참조 스키마는 클래스마다 자기 멤버만 들고 있고, Mark가 **부모 클래스까지 올라가며** 읽습니다. 그래서 `UCompany`를 상속한 클래스도 부모의 `Coworkers`를 놓치지 않습니다.

<br>

## 4. GC 최적화 과정

게임에서 문제가 되는 건 GC 총 시간이 아니라 **한 프레임이 끊기는 시간**이라고 생각했습니다.
그래서 **한 번 GC할 때 멈추는 시간(Mark + Sweep)** 을 기준으로 잡고 측정하면서 개선했습니다.

| 단계 | 바꾼 것 | 결과 |
| :---: | --- | --- |
| v1 | Mark(작업 목록) + Sweep에서 바로 `delete` | 정확하게 동작하는 GC |
| v2 | **참조 스키마** — 가상 함수 · 임시 vector 복사 대신 `주소 + 오프셋`으로 바로 읽기 | Mark: AllAlive **−35%** |
| v3 | **GC 플래그를 전역 배열로 분리** + 빈 칸 재사용 | Sweep: Chain **−89%**, 루트 찾기 **−80%** |

<details>
<summary>단계별로 고민한 내용</summary>

- **v2 참조 스키마:** 클래스 구조는 실행 중에 바뀌지 않으니, "어디에 오브젝트 포인터가 있는지"는 등록할 때 한 번만 계산하면 된다고 생각했습니다. 다만 Chain · Ring은 효과가 적었는데, 다음 오브젝트를 알려면 이전 오브젝트를 먼저 읽어야 해서 **캐시 미스를 기다리는 시간**이 병목이었기 때문입니다.
- **v3 플래그 분리:** 플래그가 오브젝트 안에 있으면 루트를 찾을 때 흩어진 10만 개를 다 읽어야 하고, Sweep이 매번 목록을 새로 만들었습니다. 플래그를 연속된 배열로 모으고 칸 번호로 관리해서 둘 다 없앴습니다. 대신 Mark가 칸을 한 번 더 거쳐야 해서 Chain은 조금 느려졌지만(5.91 → 6.63ms), 목표 안이고 배열 전체를 훑는 비용이 크게 줄어서 이 구조를 선택했습니다.
- **측정:** 노트북 전원 모드에 따라 같은 코드도 4.5 ~ 7ms까지 차이가 나서, 버전끼리는 번갈아 실행하며 비교했습니다.

</details>

<br>

## 5. 측정 결과

> AMD Ryzen 5 7430U · Release x64 · 오브젝트 10만 개 · **30회 평균**(ms) · 최악 = 30회 중 가장 큰 값

| 시나리오 | 상황 | Mark | Sweep | **멈춤**<br>평균 / 최악 |
| --- | --- | ---: | ---: | ---: |
| AllAlive | 모두 생존 | 2.26 | 0.08 | **2.34** / 3.83 |
| AllDead | 모두 삭제 | 0.07 | 2.77 | **2.83** / 3.41 |
| Random | 무작위 절반 삭제 | 1.42 | 1.98 | **3.40** / 4.43 |
| Half | 뒤 절반 삭제 | 1.02 | 2.06 | **3.08** / 4.61 |
| Chain | 10만 개 한 줄 연결 | 4.20 | 0.08 | **4.28** / 5.10 |
| Hierarchy | 트리에서 가지 하나 끊기 | 1.38 | 0.44 | **1.82** / 4.61 |
| Cycle | 서로 참조하는 쌍, 루트 없음 | 0.08 | 5.49 | **5.57** / 9.29 |
| Ring | Chain + 끝이 처음을 참조 | 4.20 | 0.08 | **4.28** / 7.38 |

- **모든 시나리오에서 멈춤이 10ms 이내**로 나왔습니다.
- 가장 오래 걸린 건 Cycle로, 10만 개를 Sweep에서 한 번에 `delete`하기 때문입니다.
- Chain · Ring은 Mark가 무겁습니다. 포인터를 줄줄이 따라가며 생기는 **캐시 미스** 때문입니다.

<br>

## 6. 아쉬운 점과 다음 단계

- **Mark + Sweep을 한 번에 하기 때문에**, 오브젝트가 더 늘면 삭제가 많은 Cycle이나 Chain 같은 구조부터 10ms에 가까워질 것입니다. 이미 죽은 오브젝트는 언제 지워도 결과가 같으니, 삭제를 여러 프레임에 나누는 방법이 첫 후보입니다. 언리얼이 쓰는 **증분 Mark, 병렬 Mark, GC 클러스터**를 다음으로 공부해 보고 싶습니다.
- Chain · Ring의 캐시 미스를 줄이려면 오브젝트를 연속된 메모리에 모으는 **메모리 풀**이 필요하다고 판단했고, 다음 단계로 남겨 두었습니다.
- RPC 인자는 공백으로 나누기 때문에 공백이 들어간 문자열이나 배열 인자는 아직 지원하지 않습니다.
- `HeaderParser`는 간단한 토큰 파서라서 `/* */` 주석이나 한 헤더에 여러 클래스가 있는 경우는 처리하지 못합니다.
