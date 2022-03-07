// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class STEERPROJECT_API Node
{
public:
	Node();
	Node(int x,	int y,float g = 0 ,float h = 0 ,bool pass = 1,bool IsTraited =false,bool IsWaiting=false );


	int x;
	int y;
	float g; //cost
	float h; //heuristic
	float f();
	
	
	int Parentx,Parenty;
	
	
	bool pass;
	bool IsTraited;
	bool IsWaiting;


};
