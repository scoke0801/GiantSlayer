#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "Terrain.h"
#include "State.h"
#include "SceneYJ.h"
CEnemy::CEnemy() : CGameObjectVer2()
{
	m_Type = OBJ_TYPE::Enemy;
	m_EnemyType = EnemyType::Skeleton;

	//m_HeightFromTerrain = 150.0f; 
	m_State = new PatrolState(this); 
	m_Speed = 165.0f * 2.5f;
}
  
CEnemy::~CEnemy()
{

}

void CEnemy::Update(float elapsedTime)
{ 
	m_State->Execute(this, elapsedTime);

	m_SightBox.Transform(m_SightAABB, XMLoadFloat4x4(&m_xmf4x4ToParent));

	CGameObjectVer2::Animate(elapsedTime);
	UpdateTransform(NULL);
}

void CEnemy::UpdateOnServer(float fTimeElapsed)
{
	CGameObjectVer2::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}


bool CEnemy::IsEnemyInSight() // Chase State
{ 
	if (m_AttackDelayTime > 0.0f) {
		return false;
	}
	for (auto player : m_ConnectedPlayers) { 
		if (false == player->IsDrawable()) {
			continue;
		}
		// 플레이어가 객체의 활동영역 안에 있는지 확인
		auto plAABBs = player->GetAABB();
		bool ret = false;
		for (auto plAABB : plAABBs) {
			if (ret = m_ActivityRegionBox.Intersects(plAABB->GetBox())) {
				break;
			}
		}
		
		// 추가할 코드
		// 만약 플레이어와 몬스터의 방향이 반대라면 쳐다볼수 없어야 한다...

		if (false == ret) { 
			return false;
		}

		// 플레이어가 객체의 시야에 있는지 확인
		for (auto plAABB : plAABBs) {
			if (ret = m_SightAABB.Intersects(plAABB->GetBox())) {
				// 해당 플레이어를 공격하도록 설정하자
				//cout << "공격할 대상을 찾았습니다.\n";
				 
				//m_AttackDelayTime = MELLE_ENEMY_ATTACK_TIME + 1.5f; 
				m_TargetPlayer = player;
				return true;
			}
		}
	}
	m_TargetPlayer = nullptr;
	return false;
}

void CEnemy::SetActivityScope(const XMFLOAT3& xmf3ActivityScope, const XMFLOAT3& xmf3Center)
{
	m_xmf3ActivityScope = xmf3ActivityScope;

	m_xmf3ActivityScopeCenter = xmf3Center;

	m_ActivityRegionBox = BoundingBox(xmf3Center, {
		xmf3ActivityScope.x * 0.5f,
		xmf3ActivityScope.y * 0.5f,
		xmf3ActivityScope.z * 0.5f
	});
}
 
void CEnemy::SetSightBoundingBox(const XMFLOAT3& sight)
{
	m_SightBox = BoundingBox({ 0,0,0 }, { sight.x * 0.5f, sight.y * 0.5f, sight.z * 0.5f });

	m_SightAABB = BoundingBox({ 0,0,0 }, { sight.x * 0.5f, sight.y * 0.5f, sight.z * 0.5f });
}

void CEnemy::ConnectPlayer(CPlayer** pPlayers, int playerCount)
{
	for (int i = 0; i < playerCount; ++i) {
		m_ConnectedPlayers.push_back( pPlayers[i] );
	}
}

void CEnemy::ChangeState(CState<CEnemy>* nextState)
{
	m_State->Exit(this);
	delete m_State;
	m_State = nextState;
}
 
void CEnemy::FindNextPosition()
{ 
	m_ToMovePosition.x = (((float)rand() / (RAND_MAX)) * (m_xmf3ActivityScope.x* 2 )) + m_xmf3ActivityScopeCenter.x - m_xmf3ActivityScope.x;
	m_ToMovePosition.y = m_xmf3Position.y;
	m_ToMovePosition.z = (((float)rand() / (RAND_MAX)) * (m_xmf3ActivityScope.z * 2)) + m_xmf3ActivityScopeCenter.z - m_xmf3ActivityScope.z;

	//cout << "목표위치 설정 ";
	//DisplayVector3(m_ToMovePosition);
	m_xmf3Velocity = Vector3::Subtract(m_ToMovePosition, m_xmf3Position);
	m_xmf3Velocity = Vector3::Normalize(m_xmf3Velocity);

	XMFLOAT3 lookAt = Vector3::Normalize(GetLook()); 

	XMFLOAT3 cross = Vector3::CrossProduct(lookAt, m_xmf3Velocity);
	float dot = Vector3::DotProduct(lookAt, m_xmf3Velocity);

	float angle = atan2(Vector3::Length(cross), dot);

	float test = Vector3::DotProduct({ 0,1,0 }, cross);
	if (test < 0.0) angle = -angle; 

	//cout << "회전 각: " << angle << "\n";
	Rotate(XMFLOAT3(0, 1, 0), (XMConvertToDegrees( angle) ));
}

void CEnemy::FindClosePositionToTarget()
{
	XMFLOAT3 playerPos = m_TargetPlayer->GetPosition();
	playerPos.y = m_xmf3Position.y;

	XMFLOAT3 targetVec = Vector3::Subtract(m_xmf3Position, playerPos);
	targetVec = Vector3::Normalize(targetVec);
	 
	m_ToMovePosition = Vector3::Subtract(m_TargetPlayer->GetPosition(),
		Vector3::Multifly(targetVec, m_AttackRange * 1.5f));

	m_ToMovePosition.y = m_xmf3Position.y;
	 
	//cout << "목표위치 설정 ";
	//DisplayVector3(m_ToMovePosition);
	m_xmf3Velocity = Vector3::Subtract(m_ToMovePosition, m_xmf3Position);
	m_xmf3Velocity = Vector3::Normalize(m_xmf3Velocity);

	XMFLOAT3 cross = Vector3::CrossProduct(targetVec, m_xmf3Velocity);
	float dot = Vector3::DotProduct(targetVec, m_xmf3Velocity);

	float angle = atan2(Vector3::Length(cross), dot);

	float test = Vector3::DotProduct({ 0,1,0 }, cross);
	if (test < 0.0) angle = -angle;

	LookAt(m_xmf3Position, m_ToMovePosition, { 0,1,0 });
	//cout << "회전 각: " << angle << "\n";
	//Rotate(XMFLOAT3(0, 1, 0), (XMConvertToDegrees(angle)));
}

void CEnemy::MoveToNextPosition(float elapsedTime)
{ 
	SetPosition(Vector3::Add(m_xmf3Position, Vector3::Multifly(m_xmf3Velocity, m_Speed * elapsedTime)));
	m_ToMovePosition.y = m_xmf3Position.y;
	XMFLOAT3 gap = Vector3::Subtract(m_ToMovePosition, m_xmf3Position);
	if (Vector3::Length(gap) < 10.0f) {
		SetPosition(m_ToMovePosition);
		SetIsOnMoving(false);
	}
}

void CEnemy::LookTarget(bool rotatedModel)
{
	XMFLOAT3 playerPos = m_TargetPlayer->GetPosition();
	playerPos.y = m_xmf3Position.y;

	XMFLOAT3 targetVec = Vector3::Subtract(m_xmf3Position, playerPos);
	targetVec = Vector3::Normalize(targetVec);

	m_ToMovePosition = Vector3::Subtract(m_TargetPlayer->GetPosition(),
		Vector3::Multifly(targetVec, m_AttackRange * 1.5f));

	m_ToMovePosition.y = m_xmf3Position.y;

	LookAt(m_xmf3Position, m_ToMovePosition, { 0,1,0 });
	if (rotatedModel) {
		//Rotate({ 0,1,0 }, 180.0f);
	}
} 

void CEnemy::FixCollision(CGameObject* pCollideObject)
{
	SetPosition(m_xmf3PrevPosition);
}

void CEnemy::Attack(float elapsedTime)
{
}


void CEnemy::ChangeState(ObjectState stateInfo, void* pData)
{
	switch (stateInfo)
	{
	case ObjectState::Wait:
		break;
	case ObjectState::Idle:
		break;
	case ObjectState::Patrol:
		break;
	case ObjectState::Trace:
		break;
	case ObjectState::Attack:
		ChangeState(new AttackState(this));
		break;
	case ObjectState::Attacked:
		ChangeState(new AttackedState(this));
		break;
	case ObjectState::Mummy_1_Die_Anger:
		ChangeState(new Mummy_1_Die_Anger_State(this));
		break;
	case ObjectState::Mummy_2_Die_Anger:
		//ChangeState(new AttackedState(this));
		break;
	case ObjectState::Die:
		break;
	case ObjectState::RunAway:
		break;
	default:
		break;
	}
}
void CEnemy::CollideToObstacle()
{
	auto stateName = m_State->GetStatename();
	switch (stateName)
	{ 
	case ObjectState::Patrol: 
	case ObjectState::Trace: 
	case ObjectState::RunAway:
		FindNextPosition();
		SetIsOnMoving(true);
		break;
	default:
		break;
	}
}

void CEnemy::ChangeAnimation(ObjectState stateInfo)
{
	switch (stateInfo)
	{
	case ObjectState::Wait:
		SetAnimationSet(1);
		break;
	case ObjectState::Idle:
		SetAnimationSet(1);
		break;
	case ObjectState::Patrol: 
		SetAnimationSet(1);
		break;
	case ObjectState::Trace:
		SetAnimationSet(1);
		break;
	case ObjectState::Attack:
		SetAnimationSet(2);
		break;
	case ObjectState::Attacked:
		SetAnimationSet(3);
		break;
	case ObjectState::Die:
		SetAnimationSet(4);
		break;
	case ObjectState::RunAway:
		break;
	case ObjectState::BossSkill_1:
		break;
	case ObjectState::BossSkill_2:
		break;
	case ObjectState::BossSkill_3:
		break;
	case ObjectState::BossSkill_4:
		break;
	case ObjectState::BossSkill_5:
		break;
	case ObjectState::BossBorn:
		break;
	default:
		break;
	}
}
 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CRangedEnemy::CRangedEnemy()
{
	m_Type = OBJ_TYPE::Enemy;
	m_AttackType = EnemyAttackType::Ranged;

	m_EnemyType = EnemyType::Skeleton;

	m_AttackRange = 1200.0f;
	//m_HeightFromTerrain = 150.0f; 
	m_State = new PatrolState(this);
	m_Speed = 165.0f * 2.5f;
}
 
CRangedEnemy::~CRangedEnemy()
{

}

void CRangedEnemy::Attack(float elapsedTime)
{
	if (m_AttackDelayTime <= 0.0f) {
		// 실제 공격!
		cout << "원거리 몬스터 화살 발사\n"; 
		m_AttackDelayTime = RANGED_ENEMY_ATTACK_TIME + 1.0f;
		MAIN_GAME_SCENE_Y->ShotMonsterArrow(this, GetLook());
	}
}
  
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CMeleeEnemy::CMeleeEnemy()
{
	m_Type = OBJ_TYPE::Enemy;
	m_AttackType = EnemyAttackType::Melee;
	m_AttackRange = 320.0f;
	

	m_EnemyType = EnemyType::Skeleton;

	//m_HeightFromTerrain = 150.0f;
	m_State = new PatrolState(this);
	m_Speed = 165.0f * 2.5f;
} 

CMeleeEnemy::~CMeleeEnemy()
{

}
void CMeleeEnemy::Attack(float elapsedTime)
{
	if (m_AttackDelayTime <= 0.0f) {
		// 실제 공격!  
		m_AttackDelayTime = MELLE_ENEMY_ATTACK_TIME + 1.0f; 
	}
}

void CMeleeEnemy::FindNextPosition()
{ 
	m_ToMovePosition.x = (((float)rand() / (RAND_MAX)) * (m_xmf3ActivityScope.x * 2)) + m_xmf3ActivityScopeCenter.x - m_xmf3ActivityScope.x;
	m_ToMovePosition.y = m_xmf3Position.y;
	m_ToMovePosition.z = (((float)rand() / (RAND_MAX)) * (m_xmf3ActivityScope.z * 2)) + m_xmf3ActivityScopeCenter.z - m_xmf3ActivityScope.z;

	//cout << "목표위치 설정 ";
	//DisplayVector3(m_ToMovePosition);
	m_xmf3Velocity = Vector3::Subtract(m_ToMovePosition, m_xmf3Position);
	m_xmf3Velocity = Vector3::Normalize(m_xmf3Velocity);

	XMFLOAT3 lookAt = Vector3::Normalize(GetLook());
	//lookAt = Vector3::Multifly(lookAt, -1);

	XMFLOAT3 cross = Vector3::CrossProduct(lookAt, m_xmf3Velocity);
	float dot = Vector3::DotProduct(lookAt, m_xmf3Velocity);

	float angle = atan2(Vector3::Length(cross), dot);

	float test = Vector3::DotProduct({ 0,1,0 }, cross);
	if (test < 0.0) angle = -angle;

	//cout << "회전 각: " << angle << "\n";
	Rotate(XMFLOAT3(0, 1, 0), (XMConvertToDegrees(angle)));
}

void CMeleeEnemy::FindClosePositionToTarget()
{
	XMFLOAT3 playerPos = m_TargetPlayer->GetPosition();
	playerPos.y = m_xmf3Position.y;

	XMFLOAT3 targetVec = Vector3::Subtract(playerPos, m_xmf3Position);
	targetVec = Vector3::Multifly(Vector3::Normalize(targetVec), -1);

	XMFLOAT3 lookAt = Vector3::Normalize(GetLook());

	m_ToMovePosition = Vector3::Subtract(m_TargetPlayer->GetPosition(),
		Vector3::Multifly(targetVec, m_AttackRange * 1.5f));

	m_ToMovePosition.y = m_xmf3Position.y;

	//cout << "목표위치 설정 ";
	//DisplayVector3(m_ToMovePosition);
	m_xmf3Velocity = Vector3::Subtract(m_ToMovePosition, m_xmf3Position);
	m_xmf3Velocity = Vector3::Normalize(m_xmf3Velocity);

	XMFLOAT3 cross = Vector3::CrossProduct(targetVec, m_xmf3Velocity);
	float dot = Vector3::DotProduct(targetVec, m_xmf3Velocity);

	float angle = atan2(Vector3::Length(cross), dot);

	float test = Vector3::DotProduct({ 0,1,0 }, cross);
	//if (test < 0.0) angle = -angle;

	LookAt(m_xmf3Position, m_ToMovePosition, { 0,1,0 });
	//Rotate({ 0,1,0 }, 180.0f);
	//cout << "회전 각: " << angle << "\n";
	//Rotate(XMFLOAT3(0, 1, 0), (XMConvertToDegrees(angle)));
}


CMummy::CMummy()
{
	m_Type = OBJ_TYPE::Mummy;
	m_AttackType = EnemyAttackType::Mummy1;

	m_HP = 15.0f;
	m_AttackRange = 1200.0f;
	m_Mummy_Die = 0.0f;
	//m_HeightFromTerrain = 150.0f; 

	m_State = new PatrolState(this);
	m_ExistingSector = SECTOR_POSITION::SECTOR_4;
}

CMummy::~CMummy()
{

}

void CMummy::AddFriends(CMummy* mummy)
{
	m_Friends.push_back(mummy);
}
void CMummy::AddFriends_Laser(CMummyLaser* Laser)
{
	m_Friends_Laser.push_back(Laser);
}

void CMummy::SendDieInfotoFriends()
{
	cout << "뭐지" << m_Mummy_Die << endl;
	m_Mummy_Die+=1.0f;

	for (auto& mummy : m_Friends)
	{
		mummy->RemoveFriends(mummy);
	}
	cout << "뭐지끝" << m_Mummy_Die << endl;

}
void CMummy::RemoveFriends(CMummy* mummy)
{
	auto res2 = std::find(m_Friends.begin(), m_Friends.end(), mummy);
	m_Friends.erase(res2);
}


void CMummy::Update(float elapsedTime)
{
	m_State->Execute(this, elapsedTime);
	
	m_SightBox.Transform(m_SightAABB, XMLoadFloat4x4(&m_xmf4x4ToParent));


	// 4스테이지 플레이어 진입 체크
	for (auto player : m_ConnectedPlayers)
	{
		if (player->GetPosition().x > 20600.0f && player->GetPosition().z > 2400.0f)
		{
			PlayerCheck = true;
		}
		else
		{
			PlayerCheck = false;
		}
	}

	// 머미가 플레이어가 스테이지에 들어온 순간 부터 레이저 공격 시간 감소
	if (PlayerCheck == true)
	{
		for (int i = 0; i < 3; i++)
		{
			if (m_LaserAttackDelayTime[i] > 0.0f) {
				m_LaserAttackDelayTime[i] = max(m_LaserAttackDelayTime[i] - elapsedTime, 0.0f);
			}
		}
	}
	
	if (this->GetMummyDie() == true)
	{
		// 만약 레이저발사시간이 됬으면 발사 하고 시간을 다시 초기화
		if (m_LaserAttack[0] == false && m_LaserAttackDelayTime[0] < 1.0f)
		{
			if (shotLaser[0] == false)
			{
				shotLaser[0] = true;
				if (this->GetEnemyAttackType() == EnemyAttackType::Mummy1)
				{
					//this->SetAnimationSet(2);
					//this->SetTargetVector(Vector3::Multifly(XMFLOAT3(15, 0, -150), 1));
					MAIN_GAME_SCENE->ShotMummyLaser(this, GetLook());
				}
			}
			m_LaserAttackDelayTime[0] = 5.0f;
			for (auto& Laser : m_Friends_Laser)
			{
				if (Laser->GetLaserType() == Laser_TYPE::Laser1)
				{
					this->SetAnimationSet(2);
					this->SetTargetVector(Vector3::Multifly(XMFLOAT3(0, 0, -150), 1));

					XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ this->GetPosition() }, { 0,200,0 });
					Laser->SetPosition(pos);
				}
					
			}
			m_LaserAttack[0] = true;
			
		}
		
		
		// 발사틈을 줘서 발사하고 나서 방향벡터 다시 원상복귀 
		if (m_LaserAttack[0] == true)
		{
			if (m_LaserAttackDelayTime[0] < 4.7f && m_LaserAttackDelayTime[0]>4.5f)
			{
				if (shotLaser[0] == true)
				{
					if (this->GetEnemyAttackType() == EnemyAttackType::Mummy1)
					{
						dir[0] = -dir[0];
						this->SetIsOnMoving(false);
						this->SetTargetVector(Vector3::Multifly(XMFLOAT3(dir[0], 0, 0), 1));
					}
				}
				shotLaser[0] = false;
				m_LaserAttack[0] = false;
			}
			else if (m_LaserAttackDelayTime[0] < 4.5f)
			{
				this->FindNextPosition();
				this->SetIsOnMoving(true);
			}

		}
	}
	//////////////// 0번
	if (this->GetMummyDie2() == true)
	{
		// 만약 레이저발사시간이 됬으면 발사 하고 시간을 다시 초기화
		if (m_LaserAttack[1] == false && m_LaserAttackDelayTime[1] < 1.0f)
		{
			if (shotLaser[1] == false)
			{
				shotLaser[1] = true;

				if (this->GetEnemyAttackType() == EnemyAttackType::Mummy2)
				{
					//this->SetAnimationSet(2);
					//this->SetTargetVector(Vector3::Multifly(XMFLOAT3(15, 0, -150), 1));
					MAIN_GAME_SCENE->ShotMummyLaser(this, GetLook());
				}

			}
			m_LaserAttackDelayTime[1] = 7.0f;
			for (auto& Laser : m_Friends_Laser)
			{
				if (Laser->GetLaserType() == Laser_TYPE::Laser2)
				{
					cout << "레이저2" << endl;
					this->SetAnimationSet(2);
					this->SetTargetVector(Vector3::Multifly(XMFLOAT3(0, 0, -150), 1));

					XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ this->GetPosition() }, { 0,200,0 });
					Laser->SetPosition(pos);
				}

			}
			m_LaserAttack[1] = true;
		}

		// 발사틈을 줘서 발사하고 나서 방향벡터 다시 원상복귀 
		if (m_LaserAttack[1] == true)
		{
			if (m_LaserAttackDelayTime[1] < 6.7f && m_LaserAttackDelayTime[1]>6.5f)
			{
				if (shotLaser[1] == true)
				{
					if (this->GetEnemyAttackType() == EnemyAttackType::Mummy2)
					{
						dir[1] = -dir[1];
						this->SetIsOnMoving(false);
						this->SetTargetVector(Vector3::Multifly(XMFLOAT3(dir[1], 0, 0), 1));
					}
				}
				shotLaser[1] = false;
				m_LaserAttack[1] = false;
			}
			else if (m_LaserAttackDelayTime[1] < 6.5f)
			{
				this->FindNextPosition();
				this->SetIsOnMoving(true);
			}
		}
	}
	if (this->GetMummyDie3() == true)
	{
		// 만약 레이저발사시간이 됬으면 발사 하고 시간을 다시 초기화
		if (m_LaserAttack[2] == false && m_LaserAttackDelayTime[2] < 1.0f)
		{
			if (shotLaser[2] == false)
			{
				shotLaser[2] = true;
				if (this->GetEnemyAttackType() == EnemyAttackType::Mummy3)
				{
					//this->SetAnimationSet(2);
					//this->SetTargetVector(Vector3::Multifly(XMFLOAT3(15, 0, -150), 1));
					MAIN_GAME_SCENE->ShotMummyLaser(this, GetLook());
				}
			}
			m_LaserAttackDelayTime[2] = 11.0f;
			for (auto& Laser : m_Friends_Laser)
			{
				if (Laser->GetLaserType() == Laser_TYPE::Laser3)
				{
					cout << "레이저3" << endl;
					//cout << m_Friends_Laser.size();
					this->SetAnimationSet(2);
					this->SetTargetVector(Vector3::Multifly(XMFLOAT3(0, 0, -150), 1));

					XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ this->GetPosition() }, { 0,200,0 });
					Laser->SetPosition(pos);
				}

			}
			m_LaserAttack[2] = true;

		}

		// 발사틈을 줘서 발사하고 나서 방향벡터 다시 원상복귀 
		if (m_LaserAttack[2] == true)
		{
			if (m_LaserAttackDelayTime[2] < 10.7f && m_LaserAttackDelayTime[2]>10.5f)
			{
				if (shotLaser[2] == true)
				{
					if (this->GetEnemyAttackType() == EnemyAttackType::Mummy3)
					{
						dir[2] = -dir[2];
						this->SetIsOnMoving(false);
						this->SetTargetVector(Vector3::Multifly(XMFLOAT3(dir[2], 0, 0), 1));
					}
				}

				shotLaser[2] = false;
				m_LaserAttack[2] = false;
			}
			else if (m_LaserAttackDelayTime[2] < 10.5f)
			{
				this->FindNextPosition();
				this->SetIsOnMoving(true);
			}
		}
	}

	
	CGameObjectVer2::Animate(elapsedTime);
	UpdateTransform(NULL);
}

void CMummy::Attack(float elapsedTime)
{
}
