// Fill out your copyright notice in the Description page of Project Settings.


#include "Node.h"

Node::Node()
{
}

Node::Node(int x, int y,float g, float h, bool pass,bool IsTraited,bool IsWaiting) : x(x), y(y),g(g), h(h),pass(pass), IsTraited(IsTraited), IsWaiting(IsWaiting)
{

}

float Node::f() {
	return g + h;
}