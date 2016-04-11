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
	LT_Background = 0,
    LT_Game,
    LT_Operate
};

enum PhysicsCategory
{
	PC_Hero = 1 << 0,
	PC_Bullet = 1 << 1,
	PC_Ground = 1 << 2,
	PC_Box = 1 << 3,
	PC_Slope = 1 << 4,
	PC_Damage = 1 << 5,
	PC_Bomb = 1 << 6
};

#define EPSILON 0.00001f


#endif /* Macro_h */
