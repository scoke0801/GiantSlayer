#pragma once
#include "State.h"

template <class object_Type>
class CStateMachine
{
private:
	object_Type* m_pOwner;
	CState<object_Type>* m_pCurrentState;

public:
	CStateMachine(object_Type* owner) : m_pOwner(owner), m_pCurrentState(NULL) {};
	~CStateMachine() {}

	void SetCurrentState(CState<object_Type>* s) { m_pCurrentState = s; }

	void  Update()const
	{
		if (m_pCurrentState) {
			m_pCurrentState->Execute(m_pOwner);
		}
	}

	void  ChangeState(CState<object_Type>* pNewState)
	{
		m_pCurrentState->Exit(m_pOwner);
		m_pCurrentState = pNewState;
		m_pCurrentState->Enter(m_pOwner);
	}

	CState<object_Type>* CurrentState()  const { return m_pCurrentState; }
};
