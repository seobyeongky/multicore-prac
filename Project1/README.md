# Project1

## 방식
기본적으로 각 집마다 BFS(Breath First Search)를 수행합니다. 이를 수행하면 각 집마다의 최소 거리를 모두 알 수 있습니다. 이 최소 거리들 중 최소값과 최대값을 출력하는 방식입니다.

  - core의 수 <= 집의 수 :  core수 만큼 HouseWorker를 돌린다. 각  HouseWorker는 하나의 쓰레드를 배정시킨다.
    - core의 수 > 집의 수 : 집의 수 만큼 HouseWorker를 돌린다. 각 HouseWorker마다 적정한 갯수의 쓰레드를 배정시킨다.

### HouseWorker
경쟁적으로 집 리스트에서 집을 가져옵니다. 그리고 배정받은 쓰레드의 수에 따라 다른 BFS 알고리즘을 선택합니다.
  - 1개 쓰레드 : 싱글쓰레드 BFS
    - 2개 이상 쓰레드 : 멀티쓰레드 BFS

## 싱글쓰레드 BFS
BFS알고리즘대로 순차적으로 교차로를 방문합니다. 다음의 경우에 종료합니다.
 - 모든 집을 방문하였을 때
  - 더 이상 방문할 수 있는 교차로가 없을 때

## 멀티쓰레드 BFS
가용쓰레드 수 만큼 Worker Thread를 만듭니다. 하나를 제외하고 대기상태로 둡니다. 그리고 하나의 Worker를 실행시킵니다. 모든 Worker가 작업을 종료할 때까지 기다립니다.
### Worker
Worker는 기본적으로 BFS알고리즘을 수행합니다. 그러다 특정 조건을 만족할 시 대기중인 Worker를 할당받아 자신의 일을 나누어줍니다. 그래서 부하를 줄입니다.


