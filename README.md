2021.02.25 - Version1.0 수정사항 - bb79eaa

-UI위치 및 크기 수정  
: 미니맵 위치 수정 (우측 하단 -> 좌측 상단)  
: 플레이어 HP, SP 게이지 별도 출력  
: 플레이어 선택 무기 UI 이미지 출력  

-맵 전반적인 형태 잡기  
: 테셀레이션 적용  
: 계단 형식으로 가장 높은 지형에서 시작하여 가장 낮은 지형으로 연결되는 형태로 구성  
  
    
2021.02.28 - Version1.01 수정사항 - f902b34  

-작업 내용 정리 및 개별 작업 씬 합치는 작업 진행  
-UI 수정  
: 미니맵 이동 방향이 플레이어 이동방향과 일치하도록 수정  
: 미니맵 플레이어 직접 출력 대신 화살표 아이콘이 출력되도록 수정  
  
2021.03.02 - Version 1.02 수정사항 - 21946b9  
: 개별 지형 각각에 별도의 텍스쳐를 적용하도록 변경  
  
2021.03.09 - Version 1.03 수정사항 - 8447ed3  

-거울 효과 구현  
-지역 별 도움말 추가  

2021.03.13 - Version 1.04 수정사항 1f57f86    

-맵 구조 수정   
-맵 생성 로직 변경    
: 파일에서 읽어온 값을 이용하여 생성하도록 변경  
-객체 바운딩 박스 출력 기능 추가  
  
2021.03.18 - Version 1.05 수정사항 88409cd  

-맵 지형지물 배치 작업   
: 1번 지역 [나무, 덤불]  
: 2번 지역 [그루터기, 죽은 나무, 암석, 버드나무]  
: 2-3번 지역 [다리 연결]  
: 3번 지역 [퍼즐 배치] 
: 표지판 배치 [시작지역 앞, 퍼즐 앞]  
: 벽 배치 [퍼즐1 앞, 퍼즐 2앞, 보스 앞]    
  
2021.03.27 - Version 1.06 수정사항 01917ac  

-맵 보완 작업  
: 지형의 경계에 해당하는 부분에 벽을 생성  
: 지형의 고저차에 의해서 텍스쳐가 벌어지는 문제점 수정  
-보스 지역에 안개 적용[지형 및 보스]  
-오브젝트 추가 배치
: 보스 지역 앞에 벽 및 문 오브젝트 추가 배치    

2021.04.04 - Version 1.07 수정사항 ebb47f1  

-조명 수정  
: 노멀 벡터 추가[FBX 모델, 지형, 기타 오브젝트 등]   
-그림자 적용을 위한 그림자 맵 생성 기반 작업   
-FBX 파일에서 애니메이션 정보를 읽어와 애니메이션 기능 수행하도록 구현  
-서버, 클라이언트 데이터 전송 구조 변경  
: 기존 string 형식의 문자열 정보를 사용하여 데이터를 전송하던 방식에서  
: 클라이언트와 서버가 공유하는 구조체 정보를 사용하여 데이터를 전송하는 방식으로 변경  
: 현재 통신 및 처리 데이터[로그인, 플레이어 로그아웃, 플레이어 추가, 플레이어 삭제, 위치 좌표 갱신, 키보드 입력 처리]   


