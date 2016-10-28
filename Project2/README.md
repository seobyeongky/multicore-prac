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
GC는 single thread로 동작한다. 

### Corectness




