//
//  Macro.h
//  SpaceX
//
//  Created by jeffcai on 16/2/18.
//
//

#ifndef _SPACEX_CLASSES_MACRO_H_
#define _SPACEX_CLASSES_MACRO_H_

enum LayerTag
{
    LT_Game = 0,
    LT_Operate
};

enum PhysicsCategory
{
    PC_Hero = 1 << 0,
    PC_Bullet = 1 << 1,
    PC_Ground = 1 << 2
};


#endif /* Macro_h */
