#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(location) if(GetWorld()) DrawDebugSphere(GetWorld(), location, 25.0f, 12, FColor::Red, true);
#define DRAW_SPHERE_COLOR(location, Color) DrawDebugSphere(GetWorld(), location, 8.0f, 12, Color, false, 5.0f);
#define DRAW_SPHERE_SINGLE_FRAME(location) if(GetWorld()) DrawDebugSphere(GetWorld(), location, 25.0f, 12, FColor::Red, false, -1.0f);
#define DRAW_LINE(startLoc, endLoc) if(GetWorld()) DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, true, -1.0f, 0, 1.0f);
#define DRAWL_LINE_SINGLE_FRAME(startLoc, endLoc) if(GetWorld()) DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, false, -1.0f, 0, 1.0f);
#define DRAW_POINT(location) if(GetWorld()) DrawDebugPoint(GetWorld(), location, 15.f, FColor::Red, true);
#define DRAW_POINT_SINGLE_FRAME(location) if(GetWorld()) DrawDebugPoint(GetWorld(), location, 15.f, FColor::Red, false, -1.0f);
#define DRAW_VECTOR(startLoc, endLoc) if(GetWorld()) { \
		DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, true, -1.0f, 0, 1.0f); \
		DrawDebugPoint(GetWorld(), endLoc, 15.f, FColor::Red, true); \
	}
#define DRAW_VECTOR_SINGLE_FRAME(startLoc, endLoc) if(GetWorld()) { \
		DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, false, -1.0f, 0, 1.0f); \
		DrawDebugPoint(GetWorld(), endLoc, 15.f, FColor::Red, false, -1.0f); \
	}