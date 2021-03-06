# Project2

### 컴파일 및 실행하기
```sh
make
bin/project2 [number_of_threads] [duration]
bin/project2 [number_of_threads] [duration] verbose
```

### 전체적인 디자인
인자로 받은 숫자만큼 update 쓰레드를 생성한다. update 쓰레드는 설명서대로 동작을 하게 된다. 그리고 GC Thread를 생성한다. 그리고 main thread는 인자로 받은 기간동안 sleep한다. 그리고 main thread가 깨어나면 모든 쓰레드를 종료시키고 출력을 하고 프로그램이 종료한다.

update thread는 loop를 돌면서 설명서대로 알고리즘을 수행한다. active_thread_list에 자신의 쓰레드 번호와 방금전 발급받은 나의 가장 최근 version값을 넣는다. 그리고 임의로 thread i를 선택한다. 만약 active thread list에 thread i가 존재할 경우, 해당 active thread list의 version의 값을 사용한다. 만약 active thread list에 없다면, 발급받은 execution_order보다 작은 version을 thread i의 data list를 순회하며 찾는다. 올바른 값을 찾았으면 정상적으로 값을 계산하고 자신의 data list에 추가한다. 그리고 atomic하게 자신의 노드를 global active thread list에서 제거한다.

### 직접 구현한 Lock 기법(MyLock), 그리고 pthread_mutexd와의 성능 비교
__sync_val_compare_and_swap이라는 함수가 값을 비교하면서 같으면 수치를 atomic하게 바꾸는 기능을 한다.
이 함수를 적극 활용해서 변수 throne_이 있을 때 각 쓰레드마다 경쟁적으로 throne_을 자신의 thread id로 변화시키는 방법을 모색하였다.
비어있는 상태(-1)일 때를 비교하면서 비어있으면 자신의 thread id로 바꾼다.
throne_의 값을 봐서 자신의 thread id일 경우 성공이므로 자신이 lock을 획득한다.
그렇지 않을 경우 loop를 돌면서 계속 시도한다.

##### MyLock
```
bin/project2 3 10
Total throughput : 3.77256e+06 (updates/sec)
Fairness : 0.999911

bin/project2 10 10
Total throughput : 2.01384e+06 (updates/sec)
Fairness : 0.99927
```
##### pthread_mutex
```
bin/project2 3 10
Total throughput : 1.16132e+06 (updates/sec)
Fairness : 0.999973

bin/project 10 
Total throughput : 1.09625e+06 (updates/sec)
Fairness : 0.99993
```

### Garbage Collection 동작 방식
GC는 single thread로 동작한다. GC의 목표는 필요없는 데이터 노드를 삭제하는 것이다. 미래에 만에 하나라도 필요한 데이터 노드는 삭제하지 않게 하였다. 그것은 우선 ref_min_version값을 구한다. 이 값은 Active thread list의 version중 최소값과 현재의 thread들 data list의 head version중 최소값 중 작은 값이다. 그리고 각 쓰레드 마다 데이터 리스트를 삭제하는 작업을 하게 된다. 이 때 ref_min_version 보다 바로 작은 version 보다 작은 version부터 삭제를 시작하면 안전하다. ref_min_version 보다 바로 작은 버전은 참조될 수 있지만 그 아래부터는 참조되지 않기 때문이다.

### Correctness
쓰레드의 업데이트 알고리즘은 DB table의 시뮬레이션이다. update가 시작될 때 read-view로 snapshot을 찍어서 올바른 데이터 버전의 참조하는 방식이다. 사실 Correctness가 크리티컬한 부분마다 lock을 걸어줬기 때문에 자명하게 안전할 수밖에 없다. 다만, data linked list의 경우 lock없이 wait-free하게 add, delete, iterate을 수행한다. 우선 list의 메소드별로 invariant를 나열해보면 single writer add, single deleter delete, multipe reader iterate이다. 그래서 add는 head만 correctness가 깨지지 않게 스와핑을 하면 문제가 없다. 그러면 당연하게 head와 가까울 수록 version이 높아지는 내림차순으로 order가 생긴다. 그러므로 필요하지 않은 버전부터 GC가 차례대로 delete를 하더라도 문제가 없다. 그 버전을 다른 쓰레드가 참조할 일이 없기 때문이다. order가 생겨서 원하는 버전을 head와 가까운 곳에서 찾아버리기 때문이다.




