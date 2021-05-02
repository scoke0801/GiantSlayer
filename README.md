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

2021.04.10 - Veresion 1.08 수정사항 6ddc82e  
: 충돌처리 작업 추가[클라이언트]  
: FBX 파일의 애니메이션 정보 추출하여 필요한 정보만 사용할 수 있도록 구조 변경  
: 충돌처리 작업 추가[서버]   
: 클라이언트의 마우스 입력을 서버로 전송하여 카메라 이동을 처리하도록 구현  

2021.04.18 - Version 1.09 수정사항 913c14c    
: 스케일링 된 지형의 x,z 좌표값을 통해 높이 맵에서 정상적으로 값을 얻어오지 못하는 문제가 있었는데 해당 부분을   
이중 선형 보간 작업을 통하여 해결  
: 일부 충돌 영역이 정의되지 않았던 객체들의 충돌 영역을 추가 [다리, 나무, 돌 등 FBX모델들]  
: 그림자 영역에 비해 맵의 크기가 큰 문제로 인해 그림자가 적용되는 부분의 크기를 수정하는 작업 수행  
: FBX 메쉬 애니메이션 동작 추가  
: 클라이언트에 정의 및 수정한 충돌 영역, 높이 계산 관련 내용을 서버에도 반영   

2021.04.25 - Version 1.10 수정사항 a0f69ba  
: 기존 32bit형식에 맞게 작성된 클라이언트 프로그램을 64bit형식에 맞도록 구조를 변경  
: 분할된 메쉬를 사용하던 지형 객체를 통합된 메쉬를 사용하도록 구조를 변경하여 Draw Call을 줄이도록 수정  
: 애니메이션 작업을 위한 스켈레톤 데이터 추가  
: 그림자가 그려지는 영역을 크기를 조절  
 
2021.05.02 - Version 1.11 수정사항 4d4f515  
: 객체 충돌 영역을 제외하고 지형 자체에서 높낮이 차이, 이전 지형으로의 이동 등 사유에 따라서 이동이 불가능한 영역을 정의하고,  
해당 지형으로 이동할 수 없도록 수정  
: 하나의 메쉬로 통합 후, 지형에 모두 같은 텍스쳐가 입혀지던 문제를 수정하여  
정상적으로 여러 텍스쳐들이 지형에 입혀지도록 함   
: 애니메이션 프레임 행렬 데이터 추가  
: 깊이 버퍼에 그려지는 그림자 맵 디버깅용 객체 추가 정의    


