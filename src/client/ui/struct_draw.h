#pragma once

#include "MessageHub.h"
#include "protocol.h"

#include <map>

class enum EEncompasRes
{
	Yes,
	No,
	Errror
};
// is a encompases b
//       [     a     ]
//					[  b  ]
inline EEncompasRes IsEncompas(const STimeInterval* a, const STimeInterval* b)
{
	if (a->start <= b->start && a->end >= b->end)
		return EEncompasRes::Yes;
	else if (a->start > b->end || a->end < b->start)
		return EEncompasRes::No;
	else
		return EEncompasRes::Error;
}

struct INode {
  ITimedEvent* pTimedEvent;
  std::vector<std::unique_ptr<INode>> children;

	bool AddChild(std::unique_ptr<INode> pNewChild){
		bool bSuccess = true;
		STimeInterval* pTIntervalA = dynamic_cast<STimeInterval*>(pNewChild->pTimedEvent);
		if (pTInterval)	{
			if (children.empty())
				children.emplace_back(pNewChild);
			else{
				int start = 0;
				int end = 0;
				while (end < children.size()){
					
					STimeInterval* pTIntervalB = dynamic_cast<STimeInterval*>(children[start]->pTimedEvent);

					if (pTIntervalB)
					{
						//three cases are possible:
						//1. B encompases A => recursive add A to B
						if (IsEncompas(pTIntervalB, pTIntervalA) == EEncomasRes::Yes){
							bSuccess = children[start]->AddChild(std::move(pNewChild));
							break;
						}

						//2. A encompases some B_start, ..., B_end
						EEncompasRes res = IsEncompas(pTIntervalA, pTIntervalB);
						if (res == EEncomasRes::Yes){ //start or continue encompassing
							end++;
							continue;
							} else if (start != end){
								
								if ( res == EEncompasRes::No && )  // encompas end
									break;
								else if (res == EEncompasRes::Error)
									bSuccess = false;
							}


						//3. A is between some B's
						// [    B   ]  [    A    ] [    B    ]
						if (start == 0 && pTIntervalA.end < pTIntervalB.start)
						{

						}
						else if (start == children.size()-1 && pTIntervalB.end < pTIntervalA.start)
						{

						}
						else if ()

					}


					}
						
					

				}
			}

			

		}

		return bSuccess;
	}
};

struct ThreadView{
  int name;
  int topY;
  int bottomY;

	std::unique_ptr<INode> m_pRoot{nullptr};

  void Insert(ITimedEvent* e){

		std::unique_ptr<INode> pNode = std::make_unique<INode>();
		node->pTimedEvent = e;

		if (!m_pRoot)
			m_pRoot = node;
		else
			m_pRoot->AddChild(std::move(pNode));


	};
	void Render(RenderContext& ctx){};
};

struct ThreadsLayout {
  std::vector<ThreadView> treadViews;
};

struct ThreadsRender {

  void Render(RenderContext&) {}

  ThreadsLayout m_layout;

	std::map<int, ThreadView> m_threads;
  void InsertTimedEvent(ITimedEvent* e){};
  void InitFromMessageHub(MessageHub& messageHub) {

		std::vector<ITimedEvent*> tmpNodes;
    messageHub.GetAllNodes(tmpNodes);

		for (auto& n : tmpNodes){
			int tid = n->tid;	
			auto it = m_threads.find(tid);
			if (it == m_threads.end())
				it = m_threads.insert(std::make_pair(tid, ThreadView())).first;

				it->second.Insert(n);
		}
  }

};
