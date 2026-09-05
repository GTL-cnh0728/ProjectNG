[주요 구현 소스코드 및 아키텍처 링크]</br>
아키텍처 설계: 책임 분리 및 다형성 구조화

[그리드 데이터 관리](https://github.com/GTL-cnh0728/ProjectNG/blob/main/Source/ProjectNG/Public/Player/NGPlayerState.h#L195)
[Grid Helper](https://github.com/GTL-cnh0728/ProjectNG/blob/main/Source/ProjectNG/Private/Combat/Grid/Grid.cpp#L113)

네트워크 복제 비용 절감을 위해 데이터는 경량 USTRUCT로 관리하고, 다형성이 필요한 로직은 식별자 기반 Helper 클래스로 위임하여 Hex/Quad 맵 확장성 확보 및 SRP 적용

전투 프레임워크 최적화: 이동 동기화 및 오브젝트 풀링

[이동 동기화 및 클라이언트 예측](https://github.com/GTL-cnh0728/ProjectNG/blob/main/Source/ProjectNG/Private/Pawn/NGPawnBase.cpp#L859)

다수 유닛 최적화를 위해 가벼운 Pawn을 사용하며, 클라이언트 예측(Client Prediction)을 적용하여 멀티플레이 환경의 끊김 현상 및 서버 판정 불일치 해결

[액터 풀 서브시스템](https://github.com/GTL-cnh0728/ProjectNG/blob/main/Source/ProjectNG/Private/Core/NGPoolSubSystem.cpp)

전투 중 유닛 및 투사체의 빈번한 스폰/파괴로 인한 GC 부하와 메모리 단편화를 방지하기 위해 액터 풀(Actor Pool) 시스템 구축
