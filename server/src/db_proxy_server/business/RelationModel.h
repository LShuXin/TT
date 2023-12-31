/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：RelationModel.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef RELATION_SHIP_H_
#define RELATION_SHIP_H_

#include <list>

#include "util.h"
#include "ImPduBase.h"
#include "IM.BaseDefine.pb.h"

using namespace std;


class CRelationModel {
public:
	virtual ~CRelationModel();

	static CRelationModel* getInstance();

    /**
     *  获取会话关系ID
     *  对于群组，必须把nUserBId设置为群ID
     *
     *  @param nUserAId  <#nUserAId description#>
     *  @param nUserBId  <#nUserBId description#>
     *  @param bAdd      <#bAdd description#>
     *  @param nStatus 0 获取未被删除会话，1获取所有。
     */
    uint32_t getRelationId(uint32_t nUserAId, uint32_t nUserBId, bool bAdd);
    bool updateRelation(uint32_t nRelationId, uint32_t nUpdateTime);
    bool removeRelation(uint32_t nRelationId);
    
private:
	CRelationModel();
    uint32_t addRelation(uint32_t nSmallId, uint32_t nBigId);
private:
	static CRelationModel*	m_pInstance;
};
#endif
